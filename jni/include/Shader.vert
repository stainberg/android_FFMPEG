static const char* VERTEX_SHADER =
"attribute vec4 vPosition;\n"  
"attribute vec2 a_texCoord;\n"
"varying vec2 tc;\n"  
"void main()\n"  
"{\n"  
"gl_Position = vPosition;\n"  
"tc = a_texCoord;\n"  
"}\n";  