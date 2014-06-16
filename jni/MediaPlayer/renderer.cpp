//
// Copyright 2013 Yen
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

//#define _DEBUG_

#include "Shader.vert"
#include "Shader.frag"
#include <stdint.h>
#include <unistd.h>
#include <android/native_window.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include  "myLog.h"
#include "renderer.h"
#include "performance.h"
#include "videoState.h"
#include "mutex.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "renderer.cpp"
#define OPENGL_ES_VERSION_2 2

static const EGLint attribs[] =
{
	EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
	EGL_RED_SIZE, 5,
	EGL_GREEN_SIZE, 6,
	EGL_BLUE_SIZE, 5,
	EGL_SAMPLE_BUFFERS, 1,
	EGL_SAMPLES, 4,
	EGL_BUFFER_SIZE, 16,
	EGL_DEPTH_SIZE, 16,
	EGL_NONE
};

static const EGLint ctx_attribs[] =
{
	EGL_CONTEXT_CLIENT_VERSION, 2,
	EGL_NONE
};

static GLfloat squareVertices[] =
{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
};

static GLfloat coordVertices[] =
{
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f,  0.0f,
		1.0f,  0.0f,
};

Renderer::Renderer()
{
    LOGI("Renderer instance created");
    _display = 0;
    _surface = 0;
    _context = 0;
    _height = _video_h = 0;
    _width = _video_w = 0;
    listener = NULL;
    simpleProgram = 0;
    _window = NULL;
//    surface_change_lock = 0;
    return;
}

Renderer::~Renderer()
{
	_display = 0;
	_surface = 0;
	_context = 0;
	_height = 0;
	_width = 0;
	listener = NULL;
	simpleProgram = 0;
	_window = NULL;
	LOGI("Renderer::~Renderer");
    return;
}

void Renderer::checkGlError(const char* op)
{
    GLint error;
    for (error = glGetError(); error; error = glGetError())
    {
        LOGI("error::after %s() glError (0x%x)\n", op, error);
    }
}

bool Renderer::initialize(int w, int h)
{
	EGLConfig config = NULL;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    EGLint majorVersion;
	EGLint minorVersion;
	EGLint numConfigs = 1, n = 0;

    LOGI("Initializing context");

    _video_h = h;
    _video_w = w;

    if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY)
    {
        LOGE("eglGetDisplay() returned error %d", eglGetError());
        return false;
    }

    if (!eglInitialize(display, &majorVersion, &minorVersion))
    {
        LOGE("eglInitialize() returned error %d", eglGetError());
        return false;
    }

    _width = ANativeWindow_getWidth(_window);
    _height = ANativeWindow_getHeight(_window);
    if (eglGetConfigs(display, NULL, 0, &numConfigs) == EGL_FALSE)
		return BAD_VALUE;
    EGLConfig* const configs = (EGLConfig*)av_malloc(sizeof(EGLConfig)*numConfigs);
    if (!eglChooseConfig(display, attribs, configs, numConfigs, &n))
    {
        LOGE("eglChooseConfig() returned error %d", eglGetError());
        destroy();
        return false;
    }
	for (int i=0 ; i<n ; i++) {
		EGLint nativeVisualId = 0;
		eglGetConfigAttrib(display, configs[i], EGL_NATIVE_VISUAL_ID, &nativeVisualId);
		if (nativeVisualId > 0) {
			config = configs[i];
			break;
		}
	}
	av_free(configs);

    if (!(surface = eglCreateWindowSurface(display, config, _window, 0)))
    {
        LOGE("eglCreateWindowSurface() returned error %d", eglGetError());
        destroy();
        return false;
    }

    if (!(context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctx_attribs)))
    {
        LOGE("eglCreateContext() returned error %d", eglGetError());
        destroy();
        return false;
    }

    if (!eglMakeCurrent(display, surface, surface, context))
    {
        LOGE("eglMakeCurrent() returned error %d", eglGetError());
        destroy();
        return false;
    }

    if (!eglQuerySurface(display, surface, EGL_WIDTH, &_width) || !eglQuerySurface(display, surface, EGL_HEIGHT, &_height))
	{
		LOGE("eglQuerySurface() returned error %d", eglGetError());
		destroy();
		return false;
	}

    _display = display;
    _surface = surface;
    _context = context;
    simpleProgram = buildProgram(VERTEX_SHADER, FRAG_SHADER);
	glUseProgram(simpleProgram);

	tex__ID_list[0] = glGetUniformLocation(simpleProgram, "SamplerY");
	checkGlError("glGetUniformLocation");
	tex__ID_list[1] = glGetUniformLocation(simpleProgram, "SamplerU");
	checkGlError("glGetUniformLocation");
	tex__ID_list[2] = glGetUniformLocation(simpleProgram, "SamplerV");
	checkGlError("glGetUniformLocation");
	glBindAttribLocation(simpleProgram, ATTRIB_VERTEX, "vPosition");
	checkGlError("glBindAttribLocation");
	glBindAttribLocation(simpleProgram, ATTRIB_TEXTURE, "a_texCoord");
	checkGlError("glBindAttribLocation");
	glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, squareVertices);
	checkGlError("glVertexAttribPointer");
	glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, coordVertices);
	checkGlError("glVertexAttribPointer");

	float diff = (float)((float)_width/(float)_height) - (float)((float)w/(float)h);
//	LOGI("org  _width = %d, w = %d, _height = %d, h = %d", _width, w, _height, h);
	GLint height = 0;
	GLint width = 0;
	if(diff > 0) {
		height = 2*_height;
		width = double((double)w/(double)h)*height;
		glViewport(-width/2 + (2*_width-width)/4, -_height, width, height);
//		glViewport(0,0,_width,_height);
//		LOGI("up  x = %d, _width = %d, y = %d, _height = %d", -width/2 + (2*_width-width)/4, _width, -_height, _height);

	} else {
		width = 2*_width;
		height = double((double)width/(double)w)*h;
		glViewport(-_width, -height/2 + (2*_height-height)/4, width, height);
//		glViewport(0,0,_width,_height);
//		LOGI("down  x = %d, _width = %d, y = %d, _height = %d", -_width, _width, -height/2 + (2*_height-height)/4, _height);

	}
    return true;
}

GLuint Renderer::buildProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    GLuint vertexShader = buildShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = buildShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    GLuint programHandle = glCreateProgram();

    if(programHandle)
    {
        glAttachShader(programHandle, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(programHandle, fragmentShader);
        checkGlError("glAttachShader");
        glLinkProgram(programHandle);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
        if(linkStatus != GL_TRUE)
        {
            GLint bufLength = 0;
            glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &bufLength);
            if(bufLength)
            {
                char* buf = (char*) av_malloc(bufLength);
                if (buf)
                {
                    glGetProgramInfoLog(programHandle, bufLength, NULL, buf);
                    LOGI("error::Could not link program:\n%s\n", buf);
                    av_free(buf);
                }
            }
            glDeleteProgram(programHandle);
            programHandle = 0;
        }
    }
    glClearColor(0.0f, 0.0f, 0.0f, 0);
	checkGlError("glClearColor");
    return programHandle;
}


GLuint Renderer::buildShader(const char* source, GLenum shaderType)
{
    GLuint shaderHandle = glCreateShader(shaderType);
    if(shaderHandle)
    {
        glShaderSource(shaderHandle, 1, &source, 0);
        glCompileShader(shaderHandle);
        GLint compiled = 0;
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            GLint infoLen = 0;
            glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen)
            {
                char* buf = (char*) av_malloc(infoLen);
                if (buf)
                {
                    glGetShaderInfoLog(shaderHandle, infoLen, NULL, buf);
                    LOGI("error::Could not compile shader %d:\n%s\n", shaderType, buf);
                    av_free(buf);
                }
                glDeleteShader(shaderHandle);
                shaderHandle = 0;
            }
        }
    }
    return shaderHandle;
}

void Renderer::destroy()
{
    LOGI("Destroying context");
    glClearColor(0.0f, 0.0f, 0.0f, 0);
	checkGlError("glClearColor");
    eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(_display, _context);
    eglDestroySurface(_display, _surface);
    eglTerminate(_display);
    _display = EGL_NO_DISPLAY;
    _surface = EGL_NO_SURFACE;
    _context = EGL_NO_CONTEXT;
    _width = 0;
    _height = 0;
    return;
}

GLuint Renderer::bindTexture(GLuint texture, const BYTE *buffer, GLuint w , GLuint h)
{
    glBindTexture ( GL_TEXTURE_2D, texture );
    checkGlError("glBindTexture");
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);
    checkGlError("glTexImage2D");
    glTexSubImage2D ( GL_TEXTURE_2D, 0, 0, 0, w, h, GL_LUMINANCE, GL_UNSIGNED_BYTE, buffer);
	checkGlError("glTexSubImage2D");
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    checkGlError("glTexParameteri");
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    checkGlError("glTexParameteri");
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    checkGlError("glTexParameteri");
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    checkGlError("glTexParameteri");
    return texture;
}

void Renderer::drawFrame(YUVBUFFER* buffer)
{
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	checkGlError("glEnableVertexAttribArray");
	glEnableVertexAttribArray(ATTRIB_TEXTURE);
	checkGlError("glEnableVertexAttribArray");

	glGenTextures(3, texture_ID_list);
	checkGlError("glGenTextures");
	bindTexture(texture_ID_list[0], buffer->image.planeData[0], buffer->image.width, buffer->image.height);
	bindTexture(texture_ID_list[1], buffer->image.planeData[1], buffer->image.width/2, buffer->image.height/2);
	bindTexture(texture_ID_list[2], buffer->image.planeData[2], buffer->image.width/2, buffer->image.height/2);
	glClear(GL_COLOR_BUFFER_BIT);
	checkGlError("glClear");
	glClearColor(0.0f, 0.0f, 0.0f, 0);
	glActiveTexture(GL_TEXTURE0);
	checkGlError("glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, texture_ID_list[0]);
	checkGlError("glBindTexture");
	glUniform1i(tex__ID_list[0], 0);
	checkGlError("glUniform1i");

	glActiveTexture(GL_TEXTURE1);
	checkGlError("glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, texture_ID_list[1]);
	checkGlError("glBindTexture");
	glUniform1i(tex__ID_list[1], 1);
	checkGlError("glUniform1i");

	glActiveTexture(GL_TEXTURE2);
	checkGlError("glActiveTexture");
	glBindTexture(GL_TEXTURE_2D, texture_ID_list[2]);
	checkGlError("glBindTexture");
	glUniform1i(tex__ID_list[2], 2);
	checkGlError("glUniform1i");

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	checkGlError("glDrawArrays");
	int w ,h = 0;
	eglQuerySurface(_display, _surface, EGL_WIDTH, &w);
	eglQuerySurface(_display, _surface, EGL_HEIGHT, &h);
	if(_width != w || _height != h)
	{
		_width = w;
		_height = h;
		float diff = (float)((float)_width/(float)_height) - (float)((float)_video_w/(float)_video_h);
		GLint height = 0;
		GLint width = 0;
		if(diff > 0) {
			height = 2*_height;
			width = double((double)_video_w/(double)_video_h)*height;
			glViewport(-width/2 + (2*_width-width)/4, -_height, width, height);
//			glViewport(0,0,_width,_height);
		} else {
			width = 2*_width;
			height = double((double)width/(double)_video_w)*_video_h;
			glViewport(-_width, -height/2 + (2*_height-height)/4, width, height);
//			glViewport(0,0,_width,_height);
		}
	}

	if (!eglSwapBuffers(_display, _surface))
	{
		LOGE("eglSwapBuffers() returned error %d", eglGetError());
	}
	glDisableVertexAttribArray(ATTRIB_VERTEX);
	checkGlError("glDisableVertexAttribArray");
	glDisableVertexAttribArray(ATTRIB_TEXTURE);
	checkGlError("glDisableVertexAttribArray");
	glDeleteTextures(3, texture_ID_list);
}
