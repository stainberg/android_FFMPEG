#ifndef RENDERER_H
#define RENDERER_H

#include "mutex.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "smartpointer.h"
#include "refBase.h"
#include "packetQueue.h"

#include "listener.h"

#define MAX_PLANES  3
typedef unsigned char BYTE;

enum {
    ATTRIB_VERTEX,
    ATTRIB_TEXTURE,
};

typedef struct YV12Image
{
    BYTE     *planeData[MAX_PLANES];
    int      planeSize[MAX_PLANES];
    unsigned stride[MAX_PLANES];
    unsigned width;
    unsigned height;
    unsigned flags;

    unsigned cshift_x; /* this is the chroma shift used */
    unsigned cshift_y;
}YV12Image;

typedef struct _YUVPLANE
{
    GLuint ID;  //texture的ID
    unsigned texwidth;
    unsigned texheight;

}YUVPLANE;

typedef YUVPLANE YUVPLANES[MAX_PLANES];

typedef struct YUVBUFFER
{
    YV12Image image;  // YUV source data
    YUVPLANES planes;  // YUV to texture
}YUVBUFFER;

class Renderer: public My_RefBase<PacketQueue> {

public:
    Renderer();
    virtual ~Renderer();
    inline void setWindow(ANativeWindow* window) {_window = window;};
    bool initialize(int w, int h);
    void destroy();
    void drawFrame(YUVBUFFER* buffer);
    inline void setRendererFlag() {surface_change_lock = 1;};
private:
    void checkGlError(const char* op);
    GLuint buildProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
    GLuint buildShader(const char* source, GLenum shaderType);
    GLuint bindTexture(GLuint texture, const BYTE *buffer, GLuint w , GLuint h);
private:
    ANativeWindow* _window;
    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;
    int _width, _video_w;
    int _height, _video_h;
    YUVBUFFER yuvbuffer;
    GLuint texture_ID_list[3];
    GLint tex__ID_list[3];
    GLuint simpleProgram;
public:
    int surface_change_lock;
    Listener* listener;
};

#endif // RENDERER_H
