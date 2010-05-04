#ifndef GLEX_GLOBALS_H
#define GLEX_GLOBALS_H

// Missing from GL_EXT_texture3D:
#ifndef GL_TEXTURE_BINDING_3D_EXT
    #define GL_TEXTURE_BINDING_3D_EXT 0x806A
#endif

// Introduced by GL_ARB_shader_objects:
typedef char GLcharARB;
typedef unsigned int GLhandleARB;

// Introduced by GL_ARB_vertex_buffer_object:
typedef ptrdiff_t GLintptrARB;
typedef ptrdiff_t GLsizeiptrARB;

#endif // GLEX_GLOBALS_H
