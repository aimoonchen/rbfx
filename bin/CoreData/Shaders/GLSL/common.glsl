#ifdef GL_ES
    precision highp float;
    #define _LAYOUT(index)
#else
    #define _LAYOUT(index) layout(binding=index)
#endif