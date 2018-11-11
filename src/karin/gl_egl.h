#ifndef _KARIN_GL_EGL_H
#define _KARIN_GL_EGL_H

#ifndef _HARMATTAN
#define _HARMATTAN
#endif

#include <GLES/gl.h>
#include <GLES/glext.h>
#define APIENTRY GL_APIENTRY
typedef float GLclampd;
typedef float GLdouble;
/*
#define GL_RGBA8 GL_RGBA8_OES
#define GL_RGB8 GL_RGB8_OES
*/

void APIENTRY karinLockArraysEXT( int a, int b);
void APIENTRY karinUnlockArraysEXT( void );
void APIENTRY karinPointParameterfEXT( GLenum param, GLfloat value );
void APIENTRY karinPointParameterfvEXT( GLenum param, const GLfloat *value );
void APIENTRY karinColorTableEXT( GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid * );
void APIENTRY karin3DfxSetPaletteEXT( GLuint * p);
void APIENTRY karinSelectTextureSGIS( GLenum target);
void APIENTRY karinMTexCoord2fSGIS( GLenum target, GLfloat s, GLfloat t);
void APIENTRY karinActiveTextureARB( GLenum target);
void APIENTRY karinClientActiveTextureARB( GLenum target);

#endif
