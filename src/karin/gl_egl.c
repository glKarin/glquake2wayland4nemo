#include "gl_egl.h"
#include "../ref_gl/qgl.h"

#define UNSUPPORT(f) fprintf(stdout, #f" is not support for OpenGLES 1.1\n");

void APIENTRY karinLockArraysEXT( int a, int b)
{
	UNSUPPORT(glLockArrayEXT)
}

void APIENTRY karinUnlockArraysEXT( void )
{
	UNSUPPORT(glUnlockArrayEXT)
}

void APIENTRY karinPointParameterfEXT( GLenum param, GLfloat value )
{
	glPointParamterf(param, value);
}

void APIENTRY karinPointParameterfvEXT( GLenum param, const GLfloat *value )
{
	glPointParameterfv(param, value);
}

void APIENTRY karinColorTableEXT( GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid * )
{
	UNSUPPORT(glColorTableEXT)
}

void APIENTRY karin3DfxSetPaletteEXT( GLuint * p)
{
	UNSUPPORT(gl3DfxSetPaletteEXT)
}

void APIENTRY karinSelectTextureSGIS( GLenum target)
{
	UNSUPPORT(glSelectTextureSGIS)
}

void APIENTRY karinMTexCoord2fSGIS( GLenum target, GLfloat s, GLfloat t)
{
	qglMultiTexCoord4f(target, s, t, 0.0, 1.0);
}

void APIENTRY karinActiveTextureARB( GLenum target)
{
	qglActiveTexture(target);
}

void APIENTRY karinClientActiveTextureARB( GLenum target)
{
	qglClientActiveTexture(target);
}
