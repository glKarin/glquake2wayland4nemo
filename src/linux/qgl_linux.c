/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
/*
** QGL_WIN.C
**
** This file implements the operating system binding of GL to QGL function
** pointers.  When doing a port of Quake2 you must implement the following
** two functions:
**
** QGL_Init() - loads libraries, assigns function pointers, etc.
** QGL_Shutdown() - unloads libraries, NULLs function pointers
*/
#include "../ref_gl/gl_local.h"
#include "glw_linux.h"

#include <dlfcn.h>

void ( APIENTRY * qglAlphaFunc )(GLenum func, GLclampf ref);
void ( APIENTRY * qglClearColor )(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
void ( APIENTRY * qglClearDepthf )(GLclampf depth);
void ( APIENTRY * qglClipPlanef )(GLenum plane, const GLfloat *equation);
void ( APIENTRY * qglColor4f )(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void ( APIENTRY * qglDepthRangef )(GLclampf zNear, GLclampf zFar);
void ( APIENTRY * qglFogf )(GLenum pname, GLfloat param);
void ( APIENTRY * qglFogfv )(GLenum pname, const GLfloat *params);
void ( APIENTRY * qglFrustumf )(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
void ( APIENTRY * qglGetClipPlanef )(GLenum pname, GLfloat eqn[4]);
void ( APIENTRY * qglGetFloatv )(GLenum pname, GLfloat *params);
void ( APIENTRY * qglGetLightfv )(GLenum light, GLenum pname, GLfloat *params);
void ( APIENTRY * qglGetMaterialfv )(GLenum face, GLenum pname, GLfloat *params);
void ( APIENTRY * qglGetTexEnvfv )(GLenum env, GLenum pname, GLfloat *params);
void ( APIENTRY * qglGetTexParameterfv )(GLenum target, GLenum pname, GLfloat *params);
void ( APIENTRY * qglLightModelf )(GLenum pname, GLfloat param);
void ( APIENTRY * qglLightModelfv )(GLenum pname, const GLfloat *params);
void ( APIENTRY * qglLightf )(GLenum light, GLenum pname, GLfloat param);
void ( APIENTRY * qglLightfv )(GLenum light, GLenum pname, const GLfloat *params);
void ( APIENTRY * qglLineWidth )(GLfloat width);
void ( APIENTRY * qglLoadMatrixf )(const GLfloat *m);
void ( APIENTRY * qglMaterialf )(GLenum face, GLenum pname, GLfloat param);
void ( APIENTRY * qglMaterialfv )(GLenum face, GLenum pname, const GLfloat *params);
void ( APIENTRY * qglMultMatrixf )(const GLfloat *m);
void ( APIENTRY * qglMultiTexCoord4f )(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
void ( APIENTRY * qglNormal3f )(GLfloat nx, GLfloat ny, GLfloat nz);
void ( APIENTRY * qglOrthof )(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
void ( APIENTRY * qglPointParameterf )(GLenum pname, GLfloat param);
void ( APIENTRY * qglPointParameterfv )(GLenum pname, const GLfloat *params);
void ( APIENTRY * qglPointSize )(GLfloat size);
void ( APIENTRY * qglPolygonOffset )(GLfloat factor, GLfloat units);
void ( APIENTRY * qglRotatef )(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void ( APIENTRY * qglScalef )(GLfloat x, GLfloat y, GLfloat z);
void ( APIENTRY * qglTexEnvf )(GLenum target, GLenum pname, GLfloat param);
void ( APIENTRY * qglTexEnvfv )(GLenum target, GLenum pname, const GLfloat *params);
void ( APIENTRY * qglTexParameterf )(GLenum target, GLenum pname, GLfloat param);
void ( APIENTRY * qglTexParameterfv )(GLenum target, GLenum pname, const GLfloat *params);
void ( APIENTRY * qglTranslatef )(GLfloat x, GLfloat y, GLfloat z);
void ( APIENTRY * qglActiveTexture )(GLenum texture);
void ( APIENTRY * qglAlphaFuncx )(GLenum func, GLclampx ref);
void ( APIENTRY * qglBindBuffer )(GLenum target, GLuint buffer);
void ( APIENTRY * qglBindTexture )(GLenum target, GLuint texture);
void ( APIENTRY * qglBlendFunc )(GLenum sfactor, GLenum dfactor);
void ( APIENTRY * qglBufferData )(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
void ( APIENTRY * qglBufferSubData )(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
void ( APIENTRY * qglClear )(GLbitfield mask);
void ( APIENTRY * qglClearColorx )(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
void ( APIENTRY * qglClearDepthx )(GLclampx depth);
void ( APIENTRY * qglClearStencil )(GLint s);
void ( APIENTRY * qglClientActiveTexture )(GLenum texture);
void ( APIENTRY * qglClipPlanex )(GLenum plane, const GLfixed *equation);
void ( APIENTRY * qglColor4ub )(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
void ( APIENTRY * qglColor4x )(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
void ( APIENTRY * qglColorMask )(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
void ( APIENTRY * qglColorPointer )(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void ( APIENTRY * qglCompressedTexImage2D )(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
void ( APIENTRY * qglCompressedTexSubImage2D )(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
void ( APIENTRY * qglCopyTexImage2D )(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
void ( APIENTRY * qglCopyTexSubImage2D )(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void ( APIENTRY * qglCullFace )(GLenum mode);
void ( APIENTRY * qglDeleteBuffers )(GLsizei n, const GLuint *buffers);
void ( APIENTRY * qglDeleteTextures )(GLsizei n, const GLuint *textures);
void ( APIENTRY * qglDepthFunc )(GLenum func);
void ( APIENTRY * qglDepthMask )(GLboolean flag);
void ( APIENTRY * qglDepthRangex )(GLclampx zNear, GLclampx zFar);
void ( APIENTRY * qglDisable )(GLenum cap);
void ( APIENTRY * qglDisableClientState )(GLenum array);
void ( APIENTRY * qglDrawArrays )(GLenum mode, GLint first, GLsizei count);
void ( APIENTRY * qglDrawElements )(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
void ( APIENTRY * qglEnable )(GLenum cap);
void ( APIENTRY * qglEnableClientState )(GLenum array);
void ( APIENTRY * qglFinish )( void);
void ( APIENTRY * qglFlush )( void);
void ( APIENTRY * qglFogx )(GLenum pname, GLfixed param);
void ( APIENTRY * qglFogxv )(GLenum pname, const GLfixed *params);
void ( APIENTRY * qglFrontFace )(GLenum mode);
void ( APIENTRY * qglFrustumx )(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
void ( APIENTRY * qglGetBooleanv )(GLenum pname, GLboolean *params);
void ( APIENTRY * qglGetBufferParameteriv )(GLenum target, GLenum pname, GLint *params);
void ( APIENTRY * qglGetClipPlanex )(GLenum pname, GLfixed eqn[4]);
void ( APIENTRY * qglGenBuffers )(GLsizei n, GLuint *buffers);
void ( APIENTRY * qglGenTextures )(GLsizei n, GLuint *textures);
GLenum ( APIENTRY * qglGetError )( void);
void ( APIENTRY * qglGetFixedv )(GLenum pname, GLfixed *params);
void ( APIENTRY * qglGetIntegerv )(GLenum pname, GLint *params);
void ( APIENTRY * qglGetLightxv )(GLenum light, GLenum pname, GLfixed *params);
void ( APIENTRY * qglGetMaterialxv )(GLenum face, GLenum pname, GLfixed *params);
void ( APIENTRY * qglGetPointerv )(GLenum pname, void **params);
const GLubyte ( APIENTRY * qglGetString )(GLenum name);
void ( APIENTRY * qglGetTexEnviv )(GLenum env, GLenum pname, GLint *params);
void ( APIENTRY * qglGetTexEnvxv )(GLenum env, GLenum pname, GLfixed *params);
void ( APIENTRY * qglGetTexParameteriv )(GLenum target, GLenum pname, GLint *params);
void ( APIENTRY * qglGetTexParameterxv )(GLenum target, GLenum pname, GLfixed *params);
void ( APIENTRY * qglHint )(GLenum target, GLenum mode);
GLboolean ( APIENTRY * qglIsBuffer )(GLuint buffer);
GLboolean ( APIENTRY * qglIsEnabled )(GLenum cap);
GLboolean ( APIENTRY * qglIsTexture )(GLuint texture);
void ( APIENTRY * qglLightModelx )(GLenum pname, GLfixed param);
void ( APIENTRY * qglLightModelxv )(GLenum pname, const GLfixed *params);
void ( APIENTRY * qglLightx )(GLenum light, GLenum pname, GLfixed param);
void ( APIENTRY * qglLightxv )(GLenum light, GLenum pname, const GLfixed *params);
void ( APIENTRY * qglLineWidthx )(GLfixed width);
void ( APIENTRY * qglLoadIdentity )( void);
void ( APIENTRY * qglLoadMatrixx )(const GLfixed *m);
void ( APIENTRY * qglLogicOp )(GLenum opcode);
void ( APIENTRY * qglMaterialx )(GLenum face, GLenum pname, GLfixed param);
void ( APIENTRY * qglMaterialxv )(GLenum face, GLenum pname, const GLfixed *params);
void ( APIENTRY * qglMatrixMode )(GLenum mode);
void ( APIENTRY * qglMultMatrixx )(const GLfixed *m);
void ( APIENTRY * qglMultiTexCoord4x )(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q);
void ( APIENTRY * qglNormal3x )(GLfixed nx, GLfixed ny, GLfixed nz);
void ( APIENTRY * qglNormalPointer )(GLenum type, GLsizei stride, const GLvoid *pointer);
void ( APIENTRY * qglOrthox )(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
void ( APIENTRY * qglPixelStorei )(GLenum pname, GLint param);
void ( APIENTRY * qglPointParameterx )(GLenum pname, GLfixed param);
void ( APIENTRY * qglPointParameterxv )(GLenum pname, const GLfixed *params);
void ( APIENTRY * qglPointSizex )(GLfixed size);
void ( APIENTRY * qglPolygonOffsetx )(GLfixed factor, GLfixed units);
void ( APIENTRY * qglPopMatrix )( void);
void ( APIENTRY * qglPushMatrix )( void);
void ( APIENTRY * qglReadPixels )(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
void ( APIENTRY * qglRotatex )(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
void ( APIENTRY * qglSampleCoverage )(GLclampf value, GLboolean invert);
void ( APIENTRY * qglSampleCoveragex )(GLclampx value, GLboolean invert);
void ( APIENTRY * qglScalex )(GLfixed x, GLfixed y, GLfixed z);
void ( APIENTRY * qglScissor )(GLint x, GLint y, GLsizei width, GLsizei height);
void ( APIENTRY * qglShadeModel )(GLenum mode);
void ( APIENTRY * qglStencilFunc )(GLenum func, GLint ref, GLuint mask);
void ( APIENTRY * qglStencilMask )(GLuint mask);
void ( APIENTRY * qglStencilOp )(GLenum fail, GLenum zfail, GLenum zpass);
void ( APIENTRY * qglTexCoordPointer )(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void ( APIENTRY * qglTexEnvi )(GLenum target, GLenum pname, GLint param);
void ( APIENTRY * qglTexEnvx )(GLenum target, GLenum pname, GLfixed param);
void ( APIENTRY * qglTexEnviv )(GLenum target, GLenum pname, const GLint *params);
void ( APIENTRY * qglTexEnvxv )(GLenum target, GLenum pname, const GLfixed *params);
void ( APIENTRY * qglTexImage2D )(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
void ( APIENTRY * qglTexParameteri )(GLenum target, GLenum pname, GLint param);
void ( APIENTRY * qglTexParameterx )(GLenum target, GLenum pname, GLfixed param);
void ( APIENTRY * qglTexParameteriv )(GLenum target, GLenum pname, const GLint *params);
void ( APIENTRY * qglTexParameterxv )(GLenum target, GLenum pname, const GLfixed *params);
void ( APIENTRY * qglTexSubImage2D )(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
void ( APIENTRY * qglTranslatex )(GLfixed x, GLfixed y, GLfixed z);
void ( APIENTRY * qglVertexPointer )(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void ( APIENTRY * qglViewport )(GLint x, GLint y, GLsizei width, GLsizei height);
void ( APIENTRY * qglPointSizePointerOES )(GLenum type, GLsizei stride, const GLvoid *pointer);


void ( APIENTRY * qglLockArraysEXT)( int, int);
void ( APIENTRY * qglUnlockArraysEXT) ( void );

void ( APIENTRY * qglPointParameterfEXT)( GLenum param, GLfloat value );
void ( APIENTRY * qglPointParameterfvEXT)( GLenum param, const GLfloat *value );
void ( APIENTRY * qglColorTableEXT)( GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid * );

void ( APIENTRY * qgl3DfxSetPaletteEXT)( GLuint * );
void ( APIENTRY * qglSelectTextureSGIS)( GLenum );
void ( APIENTRY * qglMTexCoord2fSGIS)( GLenum, GLfloat, GLfloat );
void ( APIENTRY * qglActiveTextureARB) ( GLenum );
void ( APIENTRY * qglClientActiveTextureARB) ( GLenum );

static void ( APIENTRY * dllAlphaFunc )(GLenum func, GLclampf ref);
static void ( APIENTRY * dllClearColor )(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
static void ( APIENTRY * dllClearDepthf )(GLclampf depth);
static void ( APIENTRY * dllClipPlanef )(GLenum plane, const GLfloat *equation);
static void ( APIENTRY * dllColor4f )(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
static void ( APIENTRY * dllDepthRangef )(GLclampf zNear, GLclampf zFar);
static void ( APIENTRY * dllFogf )(GLenum pname, GLfloat param);
static void ( APIENTRY * dllFogfv )(GLenum pname, const GLfloat *params);
static void ( APIENTRY * dllFrustumf )(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
static void ( APIENTRY * dllGetClipPlanef )(GLenum pname, GLfloat eqn[4]);
static void ( APIENTRY * dllGetFloatv )(GLenum pname, GLfloat *params);
static void ( APIENTRY * dllGetLightfv )(GLenum light, GLenum pname, GLfloat *params);
static void ( APIENTRY * dllGetMaterialfv )(GLenum face, GLenum pname, GLfloat *params);
static void ( APIENTRY * dllGetTexEnvfv )(GLenum env, GLenum pname, GLfloat *params);
static void ( APIENTRY * dllGetTexParameterfv )(GLenum target, GLenum pname, GLfloat *params);
static void ( APIENTRY * dllLightModelf )(GLenum pname, GLfloat param);
static void ( APIENTRY * dllLightModelfv )(GLenum pname, const GLfloat *params);
static void ( APIENTRY * dllLightf )(GLenum light, GLenum pname, GLfloat param);
static void ( APIENTRY * dllLightfv )(GLenum light, GLenum pname, const GLfloat *params);
static void ( APIENTRY * dllLineWidth )(GLfloat width);
static void ( APIENTRY * dllLoadMatrixf )(const GLfloat *m);
static void ( APIENTRY * dllMaterialf )(GLenum face, GLenum pname, GLfloat param);
static void ( APIENTRY * dllMaterialfv )(GLenum face, GLenum pname, const GLfloat *params);
static void ( APIENTRY * dllMultMatrixf )(const GLfloat *m);
static void ( APIENTRY * dllMultiTexCoord4f )(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
static void ( APIENTRY * dllNormal3f )(GLfloat nx, GLfloat ny, GLfloat nz);
static void ( APIENTRY * dllOrthof )(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
static void ( APIENTRY * dllPointParameterf )(GLenum pname, GLfloat param);
static void ( APIENTRY * dllPointParameterfv )(GLenum pname, const GLfloat *params);
static void ( APIENTRY * dllPointSize )(GLfloat size);
static void ( APIENTRY * dllPolygonOffset )(GLfloat factor, GLfloat units);
static void ( APIENTRY * dllRotatef )(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
static void ( APIENTRY * dllScalef )(GLfloat x, GLfloat y, GLfloat z);
static void ( APIENTRY * dllTexEnvf )(GLenum target, GLenum pname, GLfloat param);
static void ( APIENTRY * dllTexEnvfv )(GLenum target, GLenum pname, const GLfloat *params);
static void ( APIENTRY * dllTexParameterf )(GLenum target, GLenum pname, GLfloat param);
static void ( APIENTRY * dllTexParameterfv )(GLenum target, GLenum pname, const GLfloat *params);
static void ( APIENTRY * dllTranslatef )(GLfloat x, GLfloat y, GLfloat z);
static void ( APIENTRY * dllActiveTexture )(GLenum texture);
static void ( APIENTRY * dllAlphaFuncx )(GLenum func, GLclampx ref);
static void ( APIENTRY * dllBindBuffer )(GLenum target, GLuint buffer);
static void ( APIENTRY * dllBindTexture )(GLenum target, GLuint texture);
static void ( APIENTRY * dllBlendFunc )(GLenum sfactor, GLenum dfactor);
static void ( APIENTRY * dllBufferData )(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
static void ( APIENTRY * dllBufferSubData )(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
static void ( APIENTRY * dllClear )(GLbitfield mask);
static void ( APIENTRY * dllClearColorx )(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha);
static void ( APIENTRY * dllClearDepthx )(GLclampx depth);
static void ( APIENTRY * dllClearStencil )(GLint s);
static void ( APIENTRY * dllClientActiveTexture )(GLenum texture);
static void ( APIENTRY * dllClipPlanex )(GLenum plane, const GLfixed *equation);
static void ( APIENTRY * dllColor4ub )(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
static void ( APIENTRY * dllColor4x )(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha);
static void ( APIENTRY * dllColorMask )(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
static void ( APIENTRY * dllColorPointer )(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
static void ( APIENTRY * dllCompressedTexImage2D )(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
static void ( APIENTRY * dllCompressedTexSubImage2D )(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
static void ( APIENTRY * dllCopyTexImage2D )(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
static void ( APIENTRY * dllCopyTexSubImage2D )(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
static void ( APIENTRY * dllCullFace )(GLenum mode);
static void ( APIENTRY * dllDeleteBuffers )(GLsizei n, const GLuint *buffers);
static void ( APIENTRY * dllDeleteTextures )(GLsizei n, const GLuint *textures);
static void ( APIENTRY * dllDepthFunc )(GLenum func);
static void ( APIENTRY * dllDepthMask )(GLboolean flag);
static void ( APIENTRY * dllDepthRangex )(GLclampx zNear, GLclampx zFar);
static void ( APIENTRY * dllDisable )(GLenum cap);
static void ( APIENTRY * dllDisableClientState )(GLenum array);
static void ( APIENTRY * dllDrawArrays )(GLenum mode, GLint first, GLsizei count);
static void ( APIENTRY * dllDrawElements )(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
static void ( APIENTRY * dllEnable )(GLenum cap);
static void ( APIENTRY * dllEnableClientState )(GLenum array);
static void ( APIENTRY * dllFinish )( void);
static void ( APIENTRY * dllFlush )( void);
static void ( APIENTRY * dllFogx )(GLenum pname, GLfixed param);
static void ( APIENTRY * dllFogxv )(GLenum pname, const GLfixed *params);
static void ( APIENTRY * dllFrontFace )(GLenum mode);
static void ( APIENTRY * dllFrustumx )(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
static void ( APIENTRY * dllGetBooleanv )(GLenum pname, GLboolean *params);
static void ( APIENTRY * dllGetBufferParameteriv )(GLenum target, GLenum pname, GLint *params);
static void ( APIENTRY * dllGetClipPlanex )(GLenum pname, GLfixed eqn[4]);
static void ( APIENTRY * dllGenBuffers )(GLsizei n, GLuint *buffers);
static void ( APIENTRY * dllGenTextures )(GLsizei n, GLuint *textures);
static GLenum ( APIENTRY * dllGetError )( void);
static void ( APIENTRY * dllGetFixedv )(GLenum pname, GLfixed *params);
static void ( APIENTRY * dllGetIntegerv )(GLenum pname, GLint *params);
static void ( APIENTRY * dllGetLightxv )(GLenum light, GLenum pname, GLfixed *params);
static void ( APIENTRY * dllGetMaterialxv )(GLenum face, GLenum pname, GLfixed *params);
static void ( APIENTRY * dllGetPointerv )(GLenum pname, void **params);
static const GLubyte ( APIENTRY * dllGetString )(GLenum name);
static void ( APIENTRY * dllGetTexEnviv )(GLenum env, GLenum pname, GLint *params);
static void ( APIENTRY * dllGetTexEnvxv )(GLenum env, GLenum pname, GLfixed *params);
static void ( APIENTRY * dllGetTexParameteriv )(GLenum target, GLenum pname, GLint *params);
static void ( APIENTRY * dllGetTexParameterxv )(GLenum target, GLenum pname, GLfixed *params);
static void ( APIENTRY * dllHint )(GLenum target, GLenum mode);
static GLboolean ( APIENTRY * dllIsBuffer )(GLuint buffer);
static GLboolean ( APIENTRY * dllIsEnabled )(GLenum cap);
static GLboolean ( APIENTRY * dllIsTexture )(GLuint texture);
static void ( APIENTRY * dllLightModelx )(GLenum pname, GLfixed param);
static void ( APIENTRY * dllLightModelxv )(GLenum pname, const GLfixed *params);
static void ( APIENTRY * dllLightx )(GLenum light, GLenum pname, GLfixed param);
static void ( APIENTRY * dllLightxv )(GLenum light, GLenum pname, const GLfixed *params);
static void ( APIENTRY * dllLineWidthx )(GLfixed width);
static void ( APIENTRY * dllLoadIdentity )( void);
static void ( APIENTRY * dllLoadMatrixx )(const GLfixed *m);
static void ( APIENTRY * dllLogicOp )(GLenum opcode);
static void ( APIENTRY * dllMaterialx )(GLenum face, GLenum pname, GLfixed param);
static void ( APIENTRY * dllMaterialxv )(GLenum face, GLenum pname, const GLfixed *params);
static void ( APIENTRY * dllMatrixMode )(GLenum mode);
static void ( APIENTRY * dllMultMatrixx )(const GLfixed *m);
static void ( APIENTRY * dllMultiTexCoord4x )(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q);
static void ( APIENTRY * dllNormal3x )(GLfixed nx, GLfixed ny, GLfixed nz);
static void ( APIENTRY * dllNormalPointer )(GLenum type, GLsizei stride, const GLvoid *pointer);
static void ( APIENTRY * dllOrthox )(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar);
static void ( APIENTRY * dllPixelStorei )(GLenum pname, GLint param);
static void ( APIENTRY * dllPointParameterx )(GLenum pname, GLfixed param);
static void ( APIENTRY * dllPointParameterxv )(GLenum pname, const GLfixed *params);
static void ( APIENTRY * dllPointSizex )(GLfixed size);
static void ( APIENTRY * dllPolygonOffsetx )(GLfixed factor, GLfixed units);
static void ( APIENTRY * dllPopMatrix )( void);
static void ( APIENTRY * dllPushMatrix )( void);
static void ( APIENTRY * dllReadPixels )(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
static void ( APIENTRY * dllRotatex )(GLfixed angle, GLfixed x, GLfixed y, GLfixed z);
static void ( APIENTRY * dllSampleCoverage )(GLclampf value, GLboolean invert);
static void ( APIENTRY * dllSampleCoveragex )(GLclampx value, GLboolean invert);
static void ( APIENTRY * dllScalex )(GLfixed x, GLfixed y, GLfixed z);
static void ( APIENTRY * dllScissor )(GLint x, GLint y, GLsizei width, GLsizei height);
static void ( APIENTRY * dllShadeModel )(GLenum mode);
static void ( APIENTRY * dllStencilFunc )(GLenum func, GLint ref, GLuint mask);
static void ( APIENTRY * dllStencilMask )(GLuint mask);
static void ( APIENTRY * dllStencilOp )(GLenum fail, GLenum zfail, GLenum zpass);
static void ( APIENTRY * dllTexCoordPointer )(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
static void ( APIENTRY * dllTexEnvi )(GLenum target, GLenum pname, GLint param);
static void ( APIENTRY * dllTexEnvx )(GLenum target, GLenum pname, GLfixed param);
static void ( APIENTRY * dllTexEnviv )(GLenum target, GLenum pname, const GLint *params);
static void ( APIENTRY * dllTexEnvxv )(GLenum target, GLenum pname, const GLfixed *params);
static void ( APIENTRY * dllTexImage2D )(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
static void ( APIENTRY * dllTexParameteri )(GLenum target, GLenum pname, GLint param);
static void ( APIENTRY * dllTexParameterx )(GLenum target, GLenum pname, GLfixed param);
static void ( APIENTRY * dllTexParameteriv )(GLenum target, GLenum pname, const GLint *params);
static void ( APIENTRY * dllTexParameterxv )(GLenum target, GLenum pname, const GLfixed *params);
static void ( APIENTRY * dllTexSubImage2D )(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
static void ( APIENTRY * dllTranslatex )(GLfixed x, GLfixed y, GLfixed z);
static void ( APIENTRY * dllVertexPointer )(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
static void ( APIENTRY * dllViewport )(GLint x, GLint y, GLsizei width, GLsizei height);
static void ( APIENTRY * dllPointSizePointerOES )(GLenum type, GLsizei stride, const GLvoid *pointer);


#define SIG( x ) fprintf( glw_state.log_fp, x "\n" )
#define SIG_OUTPUT_DEVICE glw_state.log_fp
#if 0
static void APIENTRY logAlphaFunc(GLenum func, GLclampf ref)
{
	fprintf( SIG_OUTPUT_DEVICE, "glAlphaFunc( 0x%X, %f )\n", func, ref );
	dllAlphaFunc(func, ref);
}

static void APIENTRY logClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	fprintf( SIG_OUTPUT_DEVICE, "glClearColor( %f, %f, %f, %f )\n", red, green, blue, alpha );
	dllClearColor(red, green, blue, alpha);
}

static void APIENTRY logClearDepthf(GLclampf depth)
{
	fprintf( SIG_OUTPUT_DEVICE, "glClearDepthf( %f )\n", depth );
	dllClearDepthf(depth);
}

static void APIENTRY logClipPlanef(GLenum plane, const GLfloat * equation)
{
	fprintf( SIG_OUTPUT_DEVICE, "glClipPlanef( 0x%X, %p )\n", plane, equation );
	dllClipPlanef(plane, equation);
}

static void APIENTRY logColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	fprintf( SIG_OUTPUT_DEVICE, "glColor4f( %f, %f, %f, %f )\n", red, green, blue, alpha );
	dllColor4f(red, green, blue, alpha);
}

static void APIENTRY logDepthRangef(GLclampf zNear, GLclampf zFar)
{
	fprintf( SIG_OUTPUT_DEVICE, "glDepthRangef( %f, %f )\n", zNear, zFar );
	dllDepthRangef(zNear, zFar);
}

static void APIENTRY logFogf(GLenum pname, GLfloat param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glFogf( 0x%X, %f )\n", pname, param );
	dllFogf(pname, param);
}

static void APIENTRY logFogfv(GLenum pname, const GLfloat * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glFogfv( 0x%X, %p )\n", pname, params );
	dllFogfv(pname, params);
}

static void APIENTRY logFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
	fprintf( SIG_OUTPUT_DEVICE, "glFrustumf( %f, %f, %f, %f, %f, %f )\n", left, right, bottom, top, zNear, zFar );
	dllFrustumf(left, right, bottom, top, zNear, zFar);
}

static void APIENTRY logGetClipPlanef(GLenum pname, GLfloat eqn[4])
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetClipPlanef( 0x%X, %p )\n", pname, eqn );
	dllGetClipPlanef(pname, eqn);
}

static void APIENTRY logGetFloatv(GLenum pname, GLfloat * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetFloatv( 0x%X, %p )\n", pname, params );
	dllGetFloatv(pname, params);
}

static void APIENTRY logGetLightfv(GLenum light, GLenum pname, GLfloat * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetLightfv( 0x%X, 0x%X, %p )\n", light, pname, params );
	dllGetLightfv(light, pname, params);
}

static void APIENTRY logGetMaterialfv(GLenum face, GLenum pname, GLfloat * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetMaterialfv( 0x%X, 0x%X, %p )\n", face, pname, params );
	dllGetMaterialfv(face, pname, params);
}

static void APIENTRY logGetTexEnvfv(GLenum env, GLenum pname, GLfloat * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetTexEnvfv( 0x%X, 0x%X, %p )\n", env, pname, params );
	dllGetTexEnvfv(env, pname, params);
}

static void APIENTRY logGetTexParameterfv(GLenum target, GLenum pname, GLfloat * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetTexParameterfv( 0x%X, 0x%X, %p )\n", target, pname, params );
	dllGetTexParameterfv(target, pname, params);
}

static void APIENTRY logLightModelf(GLenum pname, GLfloat param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glLightModelf( 0x%X, %f )\n", pname, param );
	dllLightModelf(pname, param);
}

static void APIENTRY logLightModelfv(GLenum pname, const GLfloat * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glLightModelfv( 0x%X, %p )\n", pname, params );
	dllLightModelfv(pname, params);
}

static void APIENTRY logLightf(GLenum light, GLenum pname, GLfloat param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glLightf( 0x%X, 0x%X, %f )\n", light, pname, param );
	dllLightf(light, pname, param);
}

static void APIENTRY logLightfv(GLenum light, GLenum pname, const GLfloat * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glLightfv( 0x%X, 0x%X, %p )\n", light, pname, params );
	dllLightfv(light, pname, params);
}

static void APIENTRY logLineWidth(GLfloat width)
{
	fprintf( SIG_OUTPUT_DEVICE, "glLineWidth( %f )\n", width );
	dllLineWidth(width);
}

static void APIENTRY logLoadMatrixf(const GLfloat * m)
{
	fprintf( SIG_OUTPUT_DEVICE, "glLoadMatrixf( %p )\n", m );
	dllLoadMatrixf(m);
}

static void APIENTRY logMaterialf(GLenum face, GLenum pname, GLfloat param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glMaterialf( 0x%X, 0x%X, %f )\n", face, pname, param );
	dllMaterialf(face, pname, param);
}

static void APIENTRY logMaterialfv(GLenum face, GLenum pname, const GLfloat * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glMaterialfv( 0x%X, 0x%X, %p )\n", face, pname, params );
	dllMaterialfv(face, pname, params);
}

static void APIENTRY logMultMatrixf(const GLfloat * m)
{
	fprintf( SIG_OUTPUT_DEVICE, "glMultMatrixf( %p )\n", m );
	dllMultMatrixf(m);
}

static void APIENTRY logMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	fprintf( SIG_OUTPUT_DEVICE, "glMultiTexCoord4f( 0x%X, %f, %f, %f, %f )\n", target, s, t, r, q );
	dllMultiTexCoord4f(target, s, t, r, q);
}

static void APIENTRY logNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
	fprintf( SIG_OUTPUT_DEVICE, "glNormal3f( %f, %f, %f )\n", nx, ny, nz );
	dllNormal3f(nx, ny, nz);
}

static void APIENTRY logOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
	fprintf( SIG_OUTPUT_DEVICE, "glOrthof( %f, %f, %f, %f, %f, %f )\n", left, right, bottom, top, zNear, zFar );
	dllOrthof(left, right, bottom, top, zNear, zFar);
}

static void APIENTRY logPointParameterf(GLenum pname, GLfloat param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glPointParameterf( 0x%X, %f )\n", pname, param );
	dllPointParameterf(pname, param);
}

static void APIENTRY logPointParameterfv(GLenum pname, const GLfloat * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glPointParameterfv( 0x%X, %p )\n", pname, params );
	dllPointParameterfv(pname, params);
}

static void APIENTRY logPointSize(GLfloat size)
{
	fprintf( SIG_OUTPUT_DEVICE, "glPointSize( %f )\n", size );
	dllPointSize(size);
}

static void APIENTRY logPolygonOffset(GLfloat factor, GLfloat units)
{
	fprintf( SIG_OUTPUT_DEVICE, "glPolygonOffset( %f, %f )\n", factor, units );
	dllPolygonOffset(factor, units);
}

static void APIENTRY logRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	fprintf( SIG_OUTPUT_DEVICE, "glRotatef( %f, %f, %f, %f )\n", angle, x, y, z );
	dllRotatef(angle, x, y, z);
}

static void APIENTRY logScalef(GLfloat x, GLfloat y, GLfloat z)
{
	fprintf( SIG_OUTPUT_DEVICE, "glScalef( %f, %f, %f )\n", x, y, z );
	dllScalef(x, y, z);
}

static void APIENTRY logTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTexEnvf( 0x%X, 0x%X, %f )\n", target, pname, param );
	dllTexEnvf(target, pname, param);
}

static void APIENTRY logTexEnvfv(GLenum target, GLenum pname, const GLfloat * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTexEnvfv( 0x%X, 0x%X, %p )\n", target, pname, params );
	dllTexEnvfv(target, pname, params);
}

static void APIENTRY logTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTexParameterf( 0x%X, 0x%X, %f )\n", target, pname, param );
	dllTexParameterf(target, pname, param);
}

static void APIENTRY logTexParameterfv(GLenum target, GLenum pname, const GLfloat * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTexParameterfv( 0x%X, 0x%X, %p )\n", target, pname, params );
	dllTexParameterfv(target, pname, params);
}

static void APIENTRY logTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTranslatef( %f, %f, %f )\n", x, y, z );
	dllTranslatef(x, y, z);
}

static void APIENTRY logActiveTexture(GLenum texture)
{
	fprintf( SIG_OUTPUT_DEVICE, "glActiveTexture( 0x%X )\n", texture );
	dllActiveTexture(texture);
}

static void APIENTRY logAlphaFuncx(GLenum func, GLclampx ref)
{
	fprintf( SIG_OUTPUT_DEVICE, "glAlphaFuncx( 0x%X, %+d )\n", func, ref );
	dllAlphaFuncx(func, ref);
}

static void APIENTRY logBindBuffer(GLenum target, GLuint buffer)
{
	fprintf( SIG_OUTPUT_DEVICE, "glBindBuffer( 0x%X, %d )\n", target, buffer );
	dllBindBuffer(target, buffer);
}

static void APIENTRY logBindTexture(GLenum target, GLuint texture)
{
	fprintf( SIG_OUTPUT_DEVICE, "glBindTexture( 0x%X, %d )\n", target, texture );
	dllBindTexture(target, texture);
}

static void APIENTRY logBlendFunc(GLenum sfactor, GLenum dfactor)
{
	fprintf( SIG_OUTPUT_DEVICE, "glBlendFunc( 0x%X, 0x%X )\n", sfactor, dfactor );
	dllBlendFunc(sfactor, dfactor);
}

static void APIENTRY logBufferData(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage)
{
	fprintf( SIG_OUTPUT_DEVICE, "glBufferData( 0x%X, %d, %p, 0x%X )\n", target, size, data, usage );
	dllBufferData(target, size, data, usage);
}

static void APIENTRY logBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data)
{
	fprintf( SIG_OUTPUT_DEVICE, "glBufferSubData( 0x%X, %d, %d, %p )\n", target, offset, size, data );
	dllBufferSubData(target, offset, size, data);
}

static void APIENTRY logClear(GLbitfield mask)
{
	fprintf( SIG_OUTPUT_DEVICE, "glClear( 0x%X )\n", mask );
	dllClear(mask);
}

static void APIENTRY logClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
	fprintf( SIG_OUTPUT_DEVICE, "glClearColorx( %+d, %+d, %+d, %+d )\n", red, green, blue, alpha );
	dllClearColorx(red, green, blue, alpha);
}

static void APIENTRY logClearDepthx(GLclampx depth)
{
	fprintf( SIG_OUTPUT_DEVICE, "glClearDepthx( %+d )\n", depth );
	dllClearDepthx(depth);
}

static void APIENTRY logClearStencil(GLint s)
{
	fprintf( SIG_OUTPUT_DEVICE, "glClearStencil( %+d )\n", s );
	dllClearStencil(s);
}

static void APIENTRY logClientActiveTexture(GLenum texture)
{
	fprintf( SIG_OUTPUT_DEVICE, "glClientActiveTexture( 0x%X )\n", texture );
	dllClientActiveTexture(texture);
}

static void APIENTRY logClipPlanex(GLenum plane, const GLfixed * equation)
{
	fprintf( SIG_OUTPUT_DEVICE, "glClipPlanex( 0x%X, %p )\n", plane, equation );
	dllClipPlanex(plane, equation);
}

static void APIENTRY logColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
	fprintf( SIG_OUTPUT_DEVICE, "glColor4ub( %d, %d, %d, %d )\n", red, green, blue, alpha );
	dllColor4ub(red, green, blue, alpha);
}

static void APIENTRY logColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
	fprintf( SIG_OUTPUT_DEVICE, "glColor4x( %+d, %+d, %+d, %+d )\n", red, green, blue, alpha );
	dllColor4x(red, green, blue, alpha);
}

static void APIENTRY logColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	fprintf( SIG_OUTPUT_DEVICE, "glColorMask( %+d, %+d, %+d, %+d )\n", red, green, blue, alpha );
	dllColorMask(red, green, blue, alpha);
}

static void APIENTRY logColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
	fprintf( SIG_OUTPUT_DEVICE, "glColorPointer( %+d, 0x%X, %+d, %p )\n", size, type, stride, pointer );
	dllColorPointer(size, type, stride, pointer);
}

static void APIENTRY logCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data)
{
	fprintf( SIG_OUTPUT_DEVICE, "glCompressedTexImage2D( 0x%X, %+d, 0x%X, %+d, %+d, %+d, %+d, %p )\n", target, level, internalformat, width, height, border, imageSize, data );
	dllCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
}

static void APIENTRY logCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid * data)
{
	fprintf( SIG_OUTPUT_DEVICE, "glCompressedTexSubImage2D( 0x%X, %+d, %+d, %+d, %+d, %+d, 0x%X, %+d, %p )\n", target, level, xoffset, yoffset, width, height, format, imageSize, data );
	dllCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
}

static void APIENTRY logCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
	fprintf( SIG_OUTPUT_DEVICE, "glCopyTexImage2D( 0x%X, %+d, 0x%X, %+d, %+d, %+d, %+d, %+d )\n", target, level, internalformat, x, y, width, height, border );
	dllCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
}

static void APIENTRY logCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	fprintf( SIG_OUTPUT_DEVICE, "glCopyTexSubImage2D( 0x%X, %+d, %+d, %+d, %+d, %+d, %+d, %+d )\n", target, level, xoffset, yoffset, x, y, width, height );
	dllCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
}

static void APIENTRY logCullFace(GLenum mode)
{
	fprintf( SIG_OUTPUT_DEVICE, "glCullFace( 0x%X )\n", mode );
	dllCullFace(mode);
}

static void APIENTRY logDeleteBuffers(GLsizei n, const GLuint * buffers)
{
	fprintf( SIG_OUTPUT_DEVICE, "glDeleteBuffers( %+d, %p )\n", n, buffers );
	dllDeleteBuffers(n, buffers);
}

static void APIENTRY logDeleteTextures(GLsizei n, const GLuint * textures)
{
	fprintf( SIG_OUTPUT_DEVICE, "glDeleteTextures( %+d, %p )\n", n, textures );
	dllDeleteTextures(n, textures);
}

static void APIENTRY logDepthFunc(GLenum func)
{
	fprintf( SIG_OUTPUT_DEVICE, "glDepthFunc( 0x%X )\n", func );
	dllDepthFunc(func);
}

static void APIENTRY logDepthMask(GLboolean flag)
{
	fprintf( SIG_OUTPUT_DEVICE, "glDepthMask( %+d )\n", flag );
	dllDepthMask(flag);
}

static void APIENTRY logDepthRangex(GLclampx zNear, GLclampx zFar)
{
	fprintf( SIG_OUTPUT_DEVICE, "glDepthRangex( %+d, %+d )\n", zNear, zFar );
	dllDepthRangex(zNear, zFar);
}

static void APIENTRY logDisable(GLenum cap)
{
	fprintf( SIG_OUTPUT_DEVICE, "glDisable( 0x%X )\n", cap );
	dllDisable(cap);
}

static void APIENTRY logDisableClientState(GLenum array)
{
	fprintf( SIG_OUTPUT_DEVICE, "glDisableClientState( 0x%X )\n", array );
	dllDisableClientState(array);
}

static void APIENTRY logDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	fprintf( SIG_OUTPUT_DEVICE, "glDrawArrays( 0x%X, %+d, %+d )\n", mode, first, count );
	dllDrawArrays(mode, first, count);
}

static void APIENTRY logDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices)
{
	fprintf( SIG_OUTPUT_DEVICE, "glDrawElements( 0x%X, %+d, 0x%X, %p )\n", mode, count, type, indices );
	dllDrawElements(mode, count, type, indices);
}

static void APIENTRY logEnable(GLenum cap)
{
	fprintf( SIG_OUTPUT_DEVICE, "glEnable( 0x%X )\n", cap );
	dllEnable(cap);
}

static void APIENTRY logEnableClientState(GLenum array)
{
	fprintf( SIG_OUTPUT_DEVICE, "glEnableClientState( 0x%X )\n", array );
	dllEnableClientState(array);
}

static void APIENTRY logFinish(void)
{
	fprintf( SIG_OUTPUT_DEVICE, "glFinish()\n" );
	dllFinish();
}

static void APIENTRY logFlush(void)
{
	fprintf( SIG_OUTPUT_DEVICE, "glFlush()\n" );
	dllFlush();
}

static void APIENTRY logFogx(GLenum pname, GLfixed param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glFogx( 0x%X, %+d )\n", pname, param );
	dllFogx(pname, param);
}

static void APIENTRY logFogxv(GLenum pname, const GLfixed * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glFogxv( 0x%X, %p )\n", pname, params );
	dllFogxv(pname, params);
}

static void APIENTRY logFrontFace(GLenum mode)
{
	fprintf( SIG_OUTPUT_DEVICE, "glFrontFace( 0x%X )\n", mode );
	dllFrontFace(mode);
}

static void APIENTRY logFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
	fprintf( SIG_OUTPUT_DEVICE, "glFrustumx( %+d, %+d, %+d, %+d, %+d, %+d )\n", left, right, bottom, top, zNear, zFar );
	dllFrustumx(left, right, bottom, top, zNear, zFar);
}

static void APIENTRY logGetBooleanv(GLenum pname, GLboolean * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetBooleanv( 0x%X, %p )\n", pname, params );
	dllGetBooleanv(pname, params);
}

static void APIENTRY logGetBufferParameteriv(GLenum target, GLenum pname, GLint * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetBufferParameteriv( 0x%X, 0x%X, %p )\n", target, pname, params );
	dllGetBufferParameteriv(target, pname, params);
}

static void APIENTRY logGetClipPlanex(GLenum pname, GLfixed eqn[4])
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetClipPlanex( 0x%X, %p )\n", pname, eqn );
	dllGetClipPlanex(pname, eqn);
}

static void APIENTRY logGenBuffers(GLsizei n, GLuint * buffers)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGenBuffers( %+d, %p )\n", n, buffers );
	dllGenBuffers(n, buffers);
}

static void APIENTRY logGenTextures(GLsizei n, GLuint * textures)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGenTextures( %+d, %p )\n", n, textures );
	dllGenTextures(n, textures);
}

static GLenum APIENTRY logGetError(void)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetError()\n" );
	return dllGetError();
}

static void APIENTRY logGetFixedv(GLenum pname, GLfixed * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetFixedv( 0x%X, %p )\n", pname, params );
	dllGetFixedv(pname, params);
}

static void APIENTRY logGetIntegerv(GLenum pname, GLint * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetIntegerv( 0x%X, %p )\n", pname, params );
	dllGetIntegerv(pname, params);
}

static void APIENTRY logGetLightxv(GLenum light, GLenum pname, GLfixed * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetLightxv( 0x%X, 0x%X, %p )\n", light, pname, params );
	dllGetLightxv(light, pname, params);
}

static void APIENTRY logGetMaterialxv(GLenum face, GLenum pname, GLfixed * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetMaterialxv( 0x%X, 0x%X, %p )\n", face, pname, params );
	dllGetMaterialxv(face, pname, params);
}

static void APIENTRY logGetPointerv(GLenum pname, void * *params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetPointerv( 0x%X, %p )\n", pname, *params );
	dllGetPointerv(pname, *params);
}

static const GLubyte APIENTRY logGetString(GLenum name)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetString( 0x%X )\n", name );
	return dllGetString(name);
}

static void APIENTRY logGetTexEnviv(GLenum env, GLenum pname, GLint * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetTexEnviv( 0x%X, 0x%X, %p )\n", env, pname, params );
	dllGetTexEnviv(env, pname, params);
}

static void APIENTRY logGetTexEnvxv(GLenum env, GLenum pname, GLfixed * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetTexEnvxv( 0x%X, 0x%X, %p )\n", env, pname, params );
	dllGetTexEnvxv(env, pname, params);
}

static void APIENTRY logGetTexParameteriv(GLenum target, GLenum pname, GLint * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetTexParameteriv( 0x%X, 0x%X, %p )\n", target, pname, params );
	dllGetTexParameteriv(target, pname, params);
}

static void APIENTRY logGetTexParameterxv(GLenum target, GLenum pname, GLfixed * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glGetTexParameterxv( 0x%X, 0x%X, %p )\n", target, pname, params );
	dllGetTexParameterxv(target, pname, params);
}

static void APIENTRY logHint(GLenum target, GLenum mode)
{
	fprintf( SIG_OUTPUT_DEVICE, "glHint( 0x%X, 0x%X )\n", target, mode );
	dllHint(target, mode);
}

static GLboolean APIENTRY logIsBuffer(GLuint buffer)
{
	fprintf( SIG_OUTPUT_DEVICE, "glIsBuffer( %d )\n", buffer );
	return dllIsBuffer(buffer);
}

static GLboolean APIENTRY logIsEnabled(GLenum cap)
{
	fprintf( SIG_OUTPUT_DEVICE, "glIsEnabled( 0x%X )\n", cap );
	return dllIsEnabled(cap);
}

static GLboolean APIENTRY logIsTexture(GLuint texture)
{
	fprintf( SIG_OUTPUT_DEVICE, "glIsTexture( %d )\n", texture );
	return dllIsTexture(texture);
}

static void APIENTRY logLightModelx(GLenum pname, GLfixed param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glLightModelx( 0x%X, %+d )\n", pname, param );
	dllLightModelx(pname, param);
}

static void APIENTRY logLightModelxv(GLenum pname, const GLfixed * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glLightModelxv( 0x%X, %p )\n", pname, params );
	dllLightModelxv(pname, params);
}

static void APIENTRY logLightx(GLenum light, GLenum pname, GLfixed param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glLightx( 0x%X, 0x%X, %+d )\n", light, pname, param );
	dllLightx(light, pname, param);
}

static void APIENTRY logLightxv(GLenum light, GLenum pname, const GLfixed * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glLightxv( 0x%X, 0x%X, %p )\n", light, pname, params );
	dllLightxv(light, pname, params);
}

static void APIENTRY logLineWidthx(GLfixed width)
{
	fprintf( SIG_OUTPUT_DEVICE, "glLineWidthx( %+d )\n", width );
	dllLineWidthx(width);
}

static void APIENTRY logLoadIdentity(void)
{
	fprintf( SIG_OUTPUT_DEVICE, "glLoadIdentity()\n" );
	dllLoadIdentity();
}

static void APIENTRY logLoadMatrixx(const GLfixed * m)
{
	fprintf( SIG_OUTPUT_DEVICE, "glLoadMatrixx( %p )\n", m );
	dllLoadMatrixx(m);
}

static void APIENTRY logLogicOp(GLenum opcode)
{
	fprintf( SIG_OUTPUT_DEVICE, "glLogicOp( 0x%X )\n", opcode );
	dllLogicOp(opcode);
}

static void APIENTRY logMaterialx(GLenum face, GLenum pname, GLfixed param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glMaterialx( 0x%X, 0x%X, %+d )\n", face, pname, param );
	dllMaterialx(face, pname, param);
}

static void APIENTRY logMaterialxv(GLenum face, GLenum pname, const GLfixed * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glMaterialxv( 0x%X, 0x%X, %p )\n", face, pname, params );
	dllMaterialxv(face, pname, params);
}

static void APIENTRY logMatrixMode(GLenum mode)
{
	fprintf( SIG_OUTPUT_DEVICE, "glMatrixMode( 0x%X )\n", mode );
	dllMatrixMode(mode);
}

static void APIENTRY logMultMatrixx(const GLfixed * m)
{
	fprintf( SIG_OUTPUT_DEVICE, "glMultMatrixx( %p )\n", m );
	dllMultMatrixx(m);
}

static void APIENTRY logMultiTexCoord4x(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
	fprintf( SIG_OUTPUT_DEVICE, "glMultiTexCoord4x( 0x%X, %+d, %+d, %+d, %+d )\n", target, s, t, r, q );
	dllMultiTexCoord4x(target, s, t, r, q);
}

static void APIENTRY logNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
	fprintf( SIG_OUTPUT_DEVICE, "glNormal3x( %+d, %+d, %+d )\n", nx, ny, nz );
	dllNormal3x(nx, ny, nz);
}

static void APIENTRY logNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer)
{
	fprintf( SIG_OUTPUT_DEVICE, "glNormalPointer( 0x%X, %+d, %p )\n", type, stride, pointer );
	dllNormalPointer(type, stride, pointer);
}

static void APIENTRY logOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
	fprintf( SIG_OUTPUT_DEVICE, "glOrthox( %+d, %+d, %+d, %+d, %+d, %+d )\n", left, right, bottom, top, zNear, zFar );
	dllOrthox(left, right, bottom, top, zNear, zFar);
}

static void APIENTRY logPixelStorei(GLenum pname, GLint param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glPixelStorei( 0x%X, %+d )\n", pname, param );
	dllPixelStorei(pname, param);
}

static void APIENTRY logPointParameterx(GLenum pname, GLfixed param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glPointParameterx( 0x%X, %+d )\n", pname, param );
	dllPointParameterx(pname, param);
}

static void APIENTRY logPointParameterxv(GLenum pname, const GLfixed * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glPointParameterxv( 0x%X, %p )\n", pname, params );
	dllPointParameterxv(pname, params);
}

static void APIENTRY logPointSizex(GLfixed size)
{
	fprintf( SIG_OUTPUT_DEVICE, "glPointSizex( %+d )\n", size );
	dllPointSizex(size);
}

static void APIENTRY logPolygonOffsetx(GLfixed factor, GLfixed units)
{
	fprintf( SIG_OUTPUT_DEVICE, "glPolygonOffsetx( %+d, %+d )\n", factor, units );
	dllPolygonOffsetx(factor, units);
}

static void APIENTRY logPopMatrix(void)
{
	fprintf( SIG_OUTPUT_DEVICE, "glPopMatrix()\n" );
	dllPopMatrix();
}

static void APIENTRY logPushMatrix(void)
{
	fprintf( SIG_OUTPUT_DEVICE, "glPushMatrix()\n" );
	dllPushMatrix();
}

static void APIENTRY logReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid * pixels)
{
	fprintf( SIG_OUTPUT_DEVICE, "glReadPixels( %+d, %+d, %+d, %+d, 0x%X, 0x%X, %p )\n", x, y, width, height, format, type, pixels );
	dllReadPixels(x, y, width, height, format, type, pixels);
}

static void APIENTRY logRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
	fprintf( SIG_OUTPUT_DEVICE, "glRotatex( %+d, %+d, %+d, %+d )\n", angle, x, y, z );
	dllRotatex(angle, x, y, z);
}

static void APIENTRY logSampleCoverage(GLclampf value, GLboolean invert)
{
	fprintf( SIG_OUTPUT_DEVICE, "glSampleCoverage( %f, %+d )\n", value, invert );
	dllSampleCoverage(value, invert);
}

static void APIENTRY logSampleCoveragex(GLclampx value, GLboolean invert)
{
	fprintf( SIG_OUTPUT_DEVICE, "glSampleCoveragex( %+d, %+d )\n", value, invert );
	dllSampleCoveragex(value, invert);
}

static void APIENTRY logScalex(GLfixed x, GLfixed y, GLfixed z)
{
	fprintf( SIG_OUTPUT_DEVICE, "glScalex( %+d, %+d, %+d )\n", x, y, z );
	dllScalex(x, y, z);
}

static void APIENTRY logScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	fprintf( SIG_OUTPUT_DEVICE, "glScissor( %+d, %+d, %+d, %+d )\n", x, y, width, height );
	dllScissor(x, y, width, height);
}

static void APIENTRY logShadeModel(GLenum mode)
{
	fprintf( SIG_OUTPUT_DEVICE, "glShadeModel( 0x%X )\n", mode );
	dllShadeModel(mode);
}

static void APIENTRY logStencilFunc(GLenum func, GLint ref, GLuint mask)
{
	fprintf( SIG_OUTPUT_DEVICE, "glStencilFunc( 0x%X, %+d, %d )\n", func, ref, mask );
	dllStencilFunc(func, ref, mask);
}

static void APIENTRY logStencilMask(GLuint mask)
{
	fprintf( SIG_OUTPUT_DEVICE, "glStencilMask( %d )\n", mask );
	dllStencilMask(mask);
}

static void APIENTRY logStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
	fprintf( SIG_OUTPUT_DEVICE, "glStencilOp( 0x%X, 0x%X, 0x%X )\n", fail, zfail, zpass );
	dllStencilOp(fail, zfail, zpass);
}

static void APIENTRY logTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTexCoordPointer( %+d, 0x%X, %+d, %p )\n", size, type, stride, pointer );
	dllTexCoordPointer(size, type, stride, pointer);
}

static void APIENTRY logTexEnvi(GLenum target, GLenum pname, GLint param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTexEnvi( 0x%X, 0x%X, %+d )\n", target, pname, param );
	dllTexEnvi(target, pname, param);
}

static void APIENTRY logTexEnvx(GLenum target, GLenum pname, GLfixed param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTexEnvx( 0x%X, 0x%X, %+d )\n", target, pname, param );
	dllTexEnvx(target, pname, param);
}

static void APIENTRY logTexEnviv(GLenum target, GLenum pname, const GLint * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTexEnviv( 0x%X, 0x%X, %p )\n", target, pname, params );
	dllTexEnviv(target, pname, params);
}

static void APIENTRY logTexEnvxv(GLenum target, GLenum pname, const GLfixed * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTexEnvxv( 0x%X, 0x%X, %p )\n", target, pname, params );
	dllTexEnvxv(target, pname, params);
}

static void APIENTRY logTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTexImage2D( 0x%X, %+d, %+d, %+d, %+d, %+d, 0x%X, 0x%X, %p )\n", target, level, internalformat, width, height, border, format, type, pixels );
	dllTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}

static void APIENTRY logTexParameteri(GLenum target, GLenum pname, GLint param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTexParameteri( 0x%X, 0x%X, %+d )\n", target, pname, param );
	dllTexParameteri(target, pname, param);
}

static void APIENTRY logTexParameterx(GLenum target, GLenum pname, GLfixed param)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTexParameterx( 0x%X, 0x%X, %+d )\n", target, pname, param );
	dllTexParameterx(target, pname, param);
}

static void APIENTRY logTexParameteriv(GLenum target, GLenum pname, const GLint * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTexParameteriv( 0x%X, 0x%X, %p )\n", target, pname, params );
	dllTexParameteriv(target, pname, params);
}

static void APIENTRY logTexParameterxv(GLenum target, GLenum pname, const GLfixed * params)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTexParameterxv( 0x%X, 0x%X, %p )\n", target, pname, params );
	dllTexParameterxv(target, pname, params);
}

static void APIENTRY logTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTexSubImage2D( 0x%X, %+d, %+d, %+d, %+d, %+d, 0x%X, 0x%X, %p )\n", target, level, xoffset, yoffset, width, height, format, type, pixels );
	dllTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
}

static void APIENTRY logTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
	fprintf( SIG_OUTPUT_DEVICE, "glTranslatex( %+d, %+d, %+d )\n", x, y, z );
	dllTranslatex(x, y, z);
}

static void APIENTRY logVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
	fprintf( SIG_OUTPUT_DEVICE, "glVertexPointer( %+d, 0x%X, %+d, %p )\n", size, type, stride, pointer );
	dllVertexPointer(size, type, stride, pointer);
}

static void APIENTRY logViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	fprintf( SIG_OUTPUT_DEVICE, "glViewport( %+d, %+d, %+d, %+d )\n", x, y, width, height );
	dllViewport(x, y, width, height);
}

static void APIENTRY logPointSizePointerOES(GLenum type, GLsizei stride, const GLvoid * pointer)
{
	fprintf( SIG_OUTPUT_DEVICE, "glPointSizePointerOES( 0x%X, %+d, %p )\n", type, stride, pointer );
	dllPointSizePointerOES(type, stride, pointer);
}

#else

static void APIENTRY logAlphaFunc(GLenum func, GLclampf ref)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glAlphaFunc( 0x%X, %f )\n", func, ref );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllAlphaFunc(func, ref);
}

static void APIENTRY logClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glClearColor( %f, %f, %f, %f )\n", red, green, blue, alpha );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllClearColor(red, green, blue, alpha);
}

static void APIENTRY logClearDepthf(GLclampf depth)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glClearDepthf( %f )\n", depth );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllClearDepthf(depth);
}

static void APIENTRY logClipPlanef(GLenum plane, const GLfloat * equation)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glClipPlanef( 0x%X, %p )\n", plane, equation );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllClipPlanef(plane, equation);
}

static void APIENTRY logColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glColor4f( %f, %f, %f, %f )\n", red, green, blue, alpha );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllColor4f(red, green, blue, alpha);
}

static void APIENTRY logDepthRangef(GLclampf zNear, GLclampf zFar)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glDepthRangef( %f, %f )\n", zNear, zFar );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllDepthRangef(zNear, zFar);
}

static void APIENTRY logFogf(GLenum pname, GLfloat param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glFogf( 0x%X, %f )\n", pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllFogf(pname, param);
}

static void APIENTRY logFogfv(GLenum pname, const GLfloat * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glFogfv( 0x%X, %p )\n", pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllFogfv(pname, params);
}

static void APIENTRY logFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glFrustumf( %f, %f, %f, %f, %f, %f )\n", left, right, bottom, top, zNear, zFar );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllFrustumf(left, right, bottom, top, zNear, zFar);
}

static void APIENTRY logGetClipPlanef(GLenum pname, GLfloat eqn[4])
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetClipPlanef( 0x%X, %p )\n", pname, eqn );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetClipPlanef(pname, eqn);
}

static void APIENTRY logGetFloatv(GLenum pname, GLfloat * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetFloatv( 0x%X, %p )\n", pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetFloatv(pname, params);
}

static void APIENTRY logGetLightfv(GLenum light, GLenum pname, GLfloat * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetLightfv( 0x%X, 0x%X, %p )\n", light, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetLightfv(light, pname, params);
}

static void APIENTRY logGetMaterialfv(GLenum face, GLenum pname, GLfloat * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetMaterialfv( 0x%X, 0x%X, %p )\n", face, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetMaterialfv(face, pname, params);
}

static void APIENTRY logGetTexEnvfv(GLenum env, GLenum pname, GLfloat * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetTexEnvfv( 0x%X, 0x%X, %p )\n", env, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetTexEnvfv(env, pname, params);
}

static void APIENTRY logGetTexParameterfv(GLenum target, GLenum pname, GLfloat * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetTexParameterfv( 0x%X, 0x%X, %p )\n", target, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetTexParameterfv(target, pname, params);
}

static void APIENTRY logLightModelf(GLenum pname, GLfloat param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glLightModelf( 0x%X, %f )\n", pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllLightModelf(pname, param);
}

static void APIENTRY logLightModelfv(GLenum pname, const GLfloat * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glLightModelfv( 0x%X, %p )\n", pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllLightModelfv(pname, params);
}

static void APIENTRY logLightf(GLenum light, GLenum pname, GLfloat param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glLightf( 0x%X, 0x%X, %f )\n", light, pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllLightf(light, pname, param);
}

static void APIENTRY logLightfv(GLenum light, GLenum pname, const GLfloat * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glLightfv( 0x%X, 0x%X, %p )\n", light, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllLightfv(light, pname, params);
}

static void APIENTRY logLineWidth(GLfloat width)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glLineWidth( %f )\n", width );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllLineWidth(width);
}

static void APIENTRY logLoadMatrixf(const GLfloat * m)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glLoadMatrixf( %p )\n", m );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllLoadMatrixf(m);
}

static void APIENTRY logMaterialf(GLenum face, GLenum pname, GLfloat param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glMaterialf( 0x%X, 0x%X, %f )\n", face, pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllMaterialf(face, pname, param);
}

static void APIENTRY logMaterialfv(GLenum face, GLenum pname, const GLfloat * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glMaterialfv( 0x%X, 0x%X, %p )\n", face, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllMaterialfv(face, pname, params);
}

static void APIENTRY logMultMatrixf(const GLfloat * m)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glMultMatrixf( %p )\n", m );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllMultMatrixf(m);
}

static void APIENTRY logMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glMultiTexCoord4f( 0x%X, %f, %f, %f, %f )\n", target, s, t, r, q );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllMultiTexCoord4f(target, s, t, r, q);
}

static void APIENTRY logNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glNormal3f( %f, %f, %f )\n", nx, ny, nz );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllNormal3f(nx, ny, nz);
}

static void APIENTRY logOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glOrthof( %f, %f, %f, %f, %f, %f )\n", left, right, bottom, top, zNear, zFar );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllOrthof(left, right, bottom, top, zNear, zFar);
}

static void APIENTRY logPointParameterf(GLenum pname, GLfloat param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glPointParameterf( 0x%X, %f )\n", pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllPointParameterf(pname, param);
}

static void APIENTRY logPointParameterfv(GLenum pname, const GLfloat * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glPointParameterfv( 0x%X, %p )\n", pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllPointParameterfv(pname, params);
}

static void APIENTRY logPointSize(GLfloat size)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glPointSize( %f )\n", size );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllPointSize(size);
}

static void APIENTRY logPolygonOffset(GLfloat factor, GLfloat units)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glPolygonOffset( %f, %f )\n", factor, units );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllPolygonOffset(factor, units);
}

static void APIENTRY logRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glRotatef( %f, %f, %f, %f )\n", angle, x, y, z );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllRotatef(angle, x, y, z);
}

static void APIENTRY logScalef(GLfloat x, GLfloat y, GLfloat z)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glScalef( %f, %f, %f )\n", x, y, z );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllScalef(x, y, z);
}

static void APIENTRY logTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTexEnvf( 0x%X, 0x%X, %f )\n", target, pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTexEnvf(target, pname, param);
}

static void APIENTRY logTexEnvfv(GLenum target, GLenum pname, const GLfloat * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTexEnvfv( 0x%X, 0x%X, %p )\n", target, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTexEnvfv(target, pname, params);
}

static void APIENTRY logTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTexParameterf( 0x%X, 0x%X, %f )\n", target, pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTexParameterf(target, pname, param);
}

static void APIENTRY logTexParameterfv(GLenum target, GLenum pname, const GLfloat * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTexParameterfv( 0x%X, 0x%X, %p )\n", target, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTexParameterfv(target, pname, params);
}

static void APIENTRY logTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTranslatef( %f, %f, %f )\n", x, y, z );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTranslatef(x, y, z);
}

static void APIENTRY logActiveTexture(GLenum texture)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glActiveTexture( 0x%X )\n", texture );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllActiveTexture(texture);
}

static void APIENTRY logAlphaFuncx(GLenum func, GLclampx ref)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glAlphaFuncx( 0x%X, %+d )\n", func, ref );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllAlphaFuncx(func, ref);
}

static void APIENTRY logBindBuffer(GLenum target, GLuint buffer)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glBindBuffer( 0x%X, %d )\n", target, buffer );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllBindBuffer(target, buffer);
}

static void APIENTRY logBindTexture(GLenum target, GLuint texture)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glBindTexture( 0x%X, %d )\n", target, texture );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllBindTexture(target, texture);
}

static void APIENTRY logBlendFunc(GLenum sfactor, GLenum dfactor)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glBlendFunc( 0x%X, 0x%X )\n", sfactor, dfactor );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllBlendFunc(sfactor, dfactor);
}

static void APIENTRY logBufferData(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glBufferData( 0x%X, %d, %p, 0x%X )\n", target, size, data, usage );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllBufferData(target, size, data, usage);
}

static void APIENTRY logBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glBufferSubData( 0x%X, %d, %d, %p )\n", target, offset, size, data );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllBufferSubData(target, offset, size, data);
}

static void APIENTRY logClear(GLbitfield mask)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glClear( 0x%X )\n", mask );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllClear(mask);
}

static void APIENTRY logClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glClearColorx( %+d, %+d, %+d, %+d )\n", red, green, blue, alpha );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllClearColorx(red, green, blue, alpha);
}

static void APIENTRY logClearDepthx(GLclampx depth)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glClearDepthx( %+d )\n", depth );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllClearDepthx(depth);
}

static void APIENTRY logClearStencil(GLint s)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glClearStencil( %+d )\n", s );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllClearStencil(s);
}

static void APIENTRY logClientActiveTexture(GLenum texture)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glClientActiveTexture( 0x%X )\n", texture );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllClientActiveTexture(texture);
}

static void APIENTRY logClipPlanex(GLenum plane, const GLfixed * equation)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glClipPlanex( 0x%X, %p )\n", plane, equation );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllClipPlanex(plane, equation);
}

static void APIENTRY logColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glColor4ub( %d, %d, %d, %d )\n", red, green, blue, alpha );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllColor4ub(red, green, blue, alpha);
}

static void APIENTRY logColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glColor4x( %+d, %+d, %+d, %+d )\n", red, green, blue, alpha );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllColor4x(red, green, blue, alpha);
}

static void APIENTRY logColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glColorMask( %+d, %+d, %+d, %+d )\n", red, green, blue, alpha );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllColorMask(red, green, blue, alpha);
}

static void APIENTRY logColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glColorPointer( %+d, 0x%X, %+d, %p )\n", size, type, stride, pointer );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllColorPointer(size, type, stride, pointer);
}

static void APIENTRY logCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glCompressedTexImage2D( 0x%X, %+d, 0x%X, %+d, %+d, %+d, %+d, %p )\n", target, level, internalformat, width, height, border, imageSize, data );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
}

static void APIENTRY logCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid * data)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glCompressedTexSubImage2D( 0x%X, %+d, %+d, %+d, %+d, %+d, 0x%X, %+d, %p )\n", target, level, xoffset, yoffset, width, height, format, imageSize, data );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
}

static void APIENTRY logCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glCopyTexImage2D( 0x%X, %+d, 0x%X, %+d, %+d, %+d, %+d, %+d )\n", target, level, internalformat, x, y, width, height, border );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
}

static void APIENTRY logCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glCopyTexSubImage2D( 0x%X, %+d, %+d, %+d, %+d, %+d, %+d, %+d )\n", target, level, xoffset, yoffset, x, y, width, height );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
}

static void APIENTRY logCullFace(GLenum mode)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glCullFace( 0x%X )\n", mode );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllCullFace(mode);
}

static void APIENTRY logDeleteBuffers(GLsizei n, const GLuint * buffers)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glDeleteBuffers( %+d, %p )\n", n, buffers );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllDeleteBuffers(n, buffers);
}

static void APIENTRY logDeleteTextures(GLsizei n, const GLuint * textures)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glDeleteTextures( %+d, %p )\n", n, textures );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllDeleteTextures(n, textures);
}

static void APIENTRY logDepthFunc(GLenum func)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glDepthFunc( 0x%X )\n", func );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllDepthFunc(func);
}

static void APIENTRY logDepthMask(GLboolean flag)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glDepthMask( %+d )\n", flag );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllDepthMask(flag);
}

static void APIENTRY logDepthRangex(GLclampx zNear, GLclampx zFar)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glDepthRangex( %+d, %+d )\n", zNear, zFar );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllDepthRangex(zNear, zFar);
}

static void APIENTRY logDisable(GLenum cap)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glDisable( 0x%X )\n", cap );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllDisable(cap);
}

static void APIENTRY logDisableClientState(GLenum array)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glDisableClientState( 0x%X )\n", array );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllDisableClientState(array);
}

static void APIENTRY logDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glDrawArrays( 0x%X, %+d, %+d )\n", mode, first, count );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllDrawArrays(mode, first, count);
}

static void APIENTRY logDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glDrawElements( 0x%X, %+d, 0x%X, %p )\n", mode, count, type, indices );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllDrawElements(mode, count, type, indices);
}

static void APIENTRY logEnable(GLenum cap)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glEnable( 0x%X )\n", cap );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllEnable(cap);
}

static void APIENTRY logEnableClientState(GLenum array)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glEnableClientState( 0x%X )\n", array );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllEnableClientState(array);
}

static void APIENTRY logFinish(void)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glFinish()\n" );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllFinish();
}

static void APIENTRY logFlush(void)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glFlush()\n" );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllFlush();
}

static void APIENTRY logFogx(GLenum pname, GLfixed param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glFogx( 0x%X, %+d )\n", pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllFogx(pname, param);
}

static void APIENTRY logFogxv(GLenum pname, const GLfixed * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glFogxv( 0x%X, %p )\n", pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllFogxv(pname, params);
}

static void APIENTRY logFrontFace(GLenum mode)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glFrontFace( 0x%X )\n", mode );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllFrontFace(mode);
}

static void APIENTRY logFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glFrustumx( %+d, %+d, %+d, %+d, %+d, %+d )\n", left, right, bottom, top, zNear, zFar );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllFrustumx(left, right, bottom, top, zNear, zFar);
}

static void APIENTRY logGetBooleanv(GLenum pname, GLboolean * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetBooleanv( 0x%X, %p )\n", pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetBooleanv(pname, params);
}

static void APIENTRY logGetBufferParameteriv(GLenum target, GLenum pname, GLint * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetBufferParameteriv( 0x%X, 0x%X, %p )\n", target, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetBufferParameteriv(target, pname, params);
}

static void APIENTRY logGetClipPlanex(GLenum pname, GLfixed eqn[4])
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetClipPlanex( 0x%X, %p )\n", pname, eqn );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetClipPlanex(pname, eqn);
}

static void APIENTRY logGenBuffers(GLsizei n, GLuint * buffers)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGenBuffers( %+d, %p )\n", n, buffers );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGenBuffers(n, buffers);
}

static void APIENTRY logGenTextures(GLsizei n, GLuint * textures)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGenTextures( %+d, %p )\n", n, textures );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGenTextures(n, textures);
}

static GLenum APIENTRY logGetError(void)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetError()\n" );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	return dllGetError();
}

static void APIENTRY logGetFixedv(GLenum pname, GLfixed * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetFixedv( 0x%X, %p )\n", pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetFixedv(pname, params);
}

static void APIENTRY logGetIntegerv(GLenum pname, GLint * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetIntegerv( 0x%X, %p )\n", pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetIntegerv(pname, params);
}

static void APIENTRY logGetLightxv(GLenum light, GLenum pname, GLfixed * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetLightxv( 0x%X, 0x%X, %p )\n", light, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetLightxv(light, pname, params);
}

static void APIENTRY logGetMaterialxv(GLenum face, GLenum pname, GLfixed * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetMaterialxv( 0x%X, 0x%X, %p )\n", face, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetMaterialxv(face, pname, params);
}

static void APIENTRY logGetPointerv(GLenum pname, void * *params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetPointerv( 0x%X, %p )\n", pname, *params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetPointerv(pname, *params);
}

static const GLubyte APIENTRY logGetString(GLenum name)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetString( 0x%X )\n", name );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	return dllGetString(name);
}

static void APIENTRY logGetTexEnviv(GLenum env, GLenum pname, GLint * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetTexEnviv( 0x%X, 0x%X, %p )\n", env, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetTexEnviv(env, pname, params);
}

static void APIENTRY logGetTexEnvxv(GLenum env, GLenum pname, GLfixed * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetTexEnvxv( 0x%X, 0x%X, %p )\n", env, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetTexEnvxv(env, pname, params);
}

static void APIENTRY logGetTexParameteriv(GLenum target, GLenum pname, GLint * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetTexParameteriv( 0x%X, 0x%X, %p )\n", target, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetTexParameteriv(target, pname, params);
}

static void APIENTRY logGetTexParameterxv(GLenum target, GLenum pname, GLfixed * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glGetTexParameterxv( 0x%X, 0x%X, %p )\n", target, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllGetTexParameterxv(target, pname, params);
}

static void APIENTRY logHint(GLenum target, GLenum mode)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glHint( 0x%X, 0x%X )\n", target, mode );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllHint(target, mode);
}

static GLboolean APIENTRY logIsBuffer(GLuint buffer)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glIsBuffer( %d )\n", buffer );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	return dllIsBuffer(buffer);
}

static GLboolean APIENTRY logIsEnabled(GLenum cap)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glIsEnabled( 0x%X )\n", cap );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	return dllIsEnabled(cap);
}

static GLboolean APIENTRY logIsTexture(GLuint texture)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glIsTexture( %d )\n", texture );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	return dllIsTexture(texture);
}

static void APIENTRY logLightModelx(GLenum pname, GLfixed param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glLightModelx( 0x%X, %+d )\n", pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllLightModelx(pname, param);
}

static void APIENTRY logLightModelxv(GLenum pname, const GLfixed * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glLightModelxv( 0x%X, %p )\n", pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllLightModelxv(pname, params);
}

static void APIENTRY logLightx(GLenum light, GLenum pname, GLfixed param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glLightx( 0x%X, 0x%X, %+d )\n", light, pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllLightx(light, pname, param);
}

static void APIENTRY logLightxv(GLenum light, GLenum pname, const GLfixed * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glLightxv( 0x%X, 0x%X, %p )\n", light, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllLightxv(light, pname, params);
}

static void APIENTRY logLineWidthx(GLfixed width)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glLineWidthx( %+d )\n", width );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllLineWidthx(width);
}

static void APIENTRY logLoadIdentity(void)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glLoadIdentity()\n" );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllLoadIdentity();
}

static void APIENTRY logLoadMatrixx(const GLfixed * m)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glLoadMatrixx( %p )\n", m );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllLoadMatrixx(m);
}

static void APIENTRY logLogicOp(GLenum opcode)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glLogicOp( 0x%X )\n", opcode );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllLogicOp(opcode);
}

static void APIENTRY logMaterialx(GLenum face, GLenum pname, GLfixed param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glMaterialx( 0x%X, 0x%X, %+d )\n", face, pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllMaterialx(face, pname, param);
}

static void APIENTRY logMaterialxv(GLenum face, GLenum pname, const GLfixed * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glMaterialxv( 0x%X, 0x%X, %p )\n", face, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllMaterialxv(face, pname, params);
}

static void APIENTRY logMatrixMode(GLenum mode)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glMatrixMode( 0x%X )\n", mode );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllMatrixMode(mode);
}

static void APIENTRY logMultMatrixx(const GLfixed * m)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glMultMatrixx( %p )\n", m );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllMultMatrixx(m);
}

static void APIENTRY logMultiTexCoord4x(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glMultiTexCoord4x( 0x%X, %+d, %+d, %+d, %+d )\n", target, s, t, r, q );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllMultiTexCoord4x(target, s, t, r, q);
}

static void APIENTRY logNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glNormal3x( %+d, %+d, %+d )\n", nx, ny, nz );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllNormal3x(nx, ny, nz);
}

static void APIENTRY logNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glNormalPointer( 0x%X, %+d, %p )\n", type, stride, pointer );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllNormalPointer(type, stride, pointer);
}

static void APIENTRY logOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glOrthox( %+d, %+d, %+d, %+d, %+d, %+d )\n", left, right, bottom, top, zNear, zFar );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllOrthox(left, right, bottom, top, zNear, zFar);
}

static void APIENTRY logPixelStorei(GLenum pname, GLint param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glPixelStorei( 0x%X, %+d )\n", pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllPixelStorei(pname, param);
}

static void APIENTRY logPointParameterx(GLenum pname, GLfixed param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glPointParameterx( 0x%X, %+d )\n", pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllPointParameterx(pname, param);
}

static void APIENTRY logPointParameterxv(GLenum pname, const GLfixed * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glPointParameterxv( 0x%X, %p )\n", pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllPointParameterxv(pname, params);
}

static void APIENTRY logPointSizex(GLfixed size)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glPointSizex( %+d )\n", size );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllPointSizex(size);
}

static void APIENTRY logPolygonOffsetx(GLfixed factor, GLfixed units)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glPolygonOffsetx( %+d, %+d )\n", factor, units );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllPolygonOffsetx(factor, units);
}

static void APIENTRY logPopMatrix(void)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glPopMatrix()\n" );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllPopMatrix();
}

static void APIENTRY logPushMatrix(void)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glPushMatrix()\n" );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllPushMatrix();
}

static void APIENTRY logReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid * pixels)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glReadPixels( %+d, %+d, %+d, %+d, 0x%X, 0x%X, %p )\n", x, y, width, height, format, type, pixels );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllReadPixels(x, y, width, height, format, type, pixels);
}

static void APIENTRY logRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glRotatex( %+d, %+d, %+d, %+d )\n", angle, x, y, z );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllRotatex(angle, x, y, z);
}

static void APIENTRY logSampleCoverage(GLclampf value, GLboolean invert)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glSampleCoverage( %f, %+d )\n", value, invert );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllSampleCoverage(value, invert);
}

static void APIENTRY logSampleCoveragex(GLclampx value, GLboolean invert)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glSampleCoveragex( %+d, %+d )\n", value, invert );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllSampleCoveragex(value, invert);
}

static void APIENTRY logScalex(GLfixed x, GLfixed y, GLfixed z)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glScalex( %+d, %+d, %+d )\n", x, y, z );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllScalex(x, y, z);
}

static void APIENTRY logScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glScissor( %+d, %+d, %+d, %+d )\n", x, y, width, height );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllScissor(x, y, width, height);
}

static void APIENTRY logShadeModel(GLenum mode)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glShadeModel( 0x%X )\n", mode );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllShadeModel(mode);
}

static void APIENTRY logStencilFunc(GLenum func, GLint ref, GLuint mask)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glStencilFunc( 0x%X, %+d, %d )\n", func, ref, mask );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllStencilFunc(func, ref, mask);
}

static void APIENTRY logStencilMask(GLuint mask)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glStencilMask( %d )\n", mask );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllStencilMask(mask);
}

static void APIENTRY logStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glStencilOp( 0x%X, 0x%X, 0x%X )\n", fail, zfail, zpass );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllStencilOp(fail, zfail, zpass);
}

static void APIENTRY logTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTexCoordPointer( %+d, 0x%X, %+d, %p )\n", size, type, stride, pointer );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTexCoordPointer(size, type, stride, pointer);
}

static void APIENTRY logTexEnvi(GLenum target, GLenum pname, GLint param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTexEnvi( 0x%X, 0x%X, %+d )\n", target, pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTexEnvi(target, pname, param);
}

static void APIENTRY logTexEnvx(GLenum target, GLenum pname, GLfixed param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTexEnvx( 0x%X, 0x%X, %+d )\n", target, pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTexEnvx(target, pname, param);
}

static void APIENTRY logTexEnviv(GLenum target, GLenum pname, const GLint * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTexEnviv( 0x%X, 0x%X, %p )\n", target, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTexEnviv(target, pname, params);
}

static void APIENTRY logTexEnvxv(GLenum target, GLenum pname, const GLfixed * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTexEnvxv( 0x%X, 0x%X, %p )\n", target, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTexEnvxv(target, pname, params);
}

static void APIENTRY logTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTexImage2D( 0x%X, %+d, %+d, %+d, %+d, %+d, 0x%X, 0x%X, %p )\n", target, level, internalformat, width, height, border, format, type, pixels );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}

static void APIENTRY logTexParameteri(GLenum target, GLenum pname, GLint param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTexParameteri( 0x%X, 0x%X, %+d )\n", target, pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTexParameteri(target, pname, param);
}

static void APIENTRY logTexParameterx(GLenum target, GLenum pname, GLfixed param)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTexParameterx( 0x%X, 0x%X, %+d )\n", target, pname, param );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTexParameterx(target, pname, param);
}

static void APIENTRY logTexParameteriv(GLenum target, GLenum pname, const GLint * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTexParameteriv( 0x%X, 0x%X, %p )\n", target, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTexParameteriv(target, pname, params);
}

static void APIENTRY logTexParameterxv(GLenum target, GLenum pname, const GLfixed * params)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTexParameterxv( 0x%X, 0x%X, %p )\n", target, pname, params );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTexParameterxv(target, pname, params);
}

static void APIENTRY logTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTexSubImage2D( 0x%X, %+d, %+d, %+d, %+d, %+d, 0x%X, 0x%X, %p )\n", target, level, xoffset, yoffset, width, height, format, type, pixels );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
}

static void APIENTRY logTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glTranslatex( %+d, %+d, %+d )\n", x, y, z );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllTranslatex(x, y, z);
}

static void APIENTRY logVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glVertexPointer( %+d, 0x%X, %+d, %p )\n", size, type, stride, pointer );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllVertexPointer(size, type, stride, pointer);
}

static void APIENTRY logViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glViewport( %+d, %+d, %+d, %+d )\n", x, y, width, height );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllViewport(x, y, width, height);
}

static void APIENTRY logPointSizePointerOES(GLenum type, GLsizei stride, const GLvoid * pointer)
{
	GLenum err = dllGetError();
	if(err != GL_NO_ERROR)
	{
		fprintf( SIG_OUTPUT_DEVICE, "glPointSizePointerOES( 0x%X, %+d, %p )\n", type, stride, pointer );
		fprintf(SIG_OUTPUT_DEVICE, "GL_Error -> %X\n", err);
	}
	dllPointSizePointerOES(type, stride, pointer);
}
#endif

#undef SIG_OUTPUT_DEVICE


/*
** QGL_Shutdown
**
** Unloads the specified DLL then nulls out all the proc pointers.
*/
void QGL_Shutdown( void )
{
	if ( glw_state.OpenGLLib )
	{
		dlclose ( glw_state.OpenGLLib );
		glw_state.OpenGLLib = NULL;
	}

	glw_state.OpenGLLib = NULL;

	qglAlphaFunc = NULL;
	qglClearColor = NULL;
	qglClearDepthf = NULL;
	qglClipPlanef = NULL;
	qglColor4f = NULL;
	qglDepthRangef = NULL;
	qglFogf = NULL;
	qglFogfv = NULL;
	qglFrustumf = NULL;
	qglGetClipPlanef = NULL;
	qglGetFloatv = NULL;
	qglGetLightfv = NULL;
	qglGetMaterialfv = NULL;
	qglGetTexEnvfv = NULL;
	qglGetTexParameterfv = NULL;
	qglLightModelf = NULL;
	qglLightModelfv = NULL;
	qglLightf = NULL;
	qglLightfv = NULL;
	qglLineWidth = NULL;
	qglLoadMatrixf = NULL;
	qglMaterialf = NULL;
	qglMaterialfv = NULL;
	qglMultMatrixf = NULL;
	qglMultiTexCoord4f = NULL;
	qglNormal3f = NULL;
	qglOrthof = NULL;
	qglPointParameterf = NULL;
	qglPointParameterfv = NULL;
	qglPointSize = NULL;
	qglPolygonOffset = NULL;
	qglRotatef = NULL;
	qglScalef = NULL;
	qglTexEnvf = NULL;
	qglTexEnvfv = NULL;
	qglTexParameterf = NULL;
	qglTexParameterfv = NULL;
	qglTranslatef = NULL;
	qglActiveTexture = NULL;
	qglAlphaFuncx = NULL;
	qglBindBuffer = NULL;
	qglBindTexture = NULL;
	qglBlendFunc = NULL;
	qglBufferData = NULL;
	qglBufferSubData = NULL;
	qglClear = NULL;
	qglClearColorx = NULL;
	qglClearDepthx = NULL;
	qglClearStencil = NULL;
	qglClientActiveTexture = NULL;
	qglClipPlanex = NULL;
	qglColor4ub = NULL;
	qglColor4x = NULL;
	qglColorMask = NULL;
	qglColorPointer = NULL;
	qglCompressedTexImage2D = NULL;
	qglCompressedTexSubImage2D = NULL;
	qglCopyTexImage2D = NULL;
	qglCopyTexSubImage2D = NULL;
	qglCullFace = NULL;
	qglDeleteBuffers = NULL;
	qglDeleteTextures = NULL;
	qglDepthFunc = NULL;
	qglDepthMask = NULL;
	qglDepthRangex = NULL;
	qglDisable = NULL;
	qglDisableClientState = NULL;
	qglDrawArrays = NULL;
	qglDrawElements = NULL;
	qglEnable = NULL;
	qglEnableClientState = NULL;
	qglFinish = NULL;
	qglFlush = NULL;
	qglFogx = NULL;
	qglFogxv = NULL;
	qglFrontFace = NULL;
	qglFrustumx = NULL;
	qglGetBooleanv = NULL;
	qglGetBufferParameteriv = NULL;
	qglGetClipPlanex = NULL;
	qglGenBuffers = NULL;
	qglGenTextures = NULL;
	qglGetError = NULL;
	qglGetFixedv = NULL;
	qglGetIntegerv = NULL;
	qglGetLightxv = NULL;
	qglGetMaterialxv = NULL;
	qglGetPointerv = NULL;
	qglGetString = NULL;
	qglGetTexEnviv = NULL;
	qglGetTexEnvxv = NULL;
	qglGetTexParameteriv = NULL;
	qglGetTexParameterxv = NULL;
	qglHint = NULL;
	qglIsBuffer = NULL;
	qglIsEnabled = NULL;
	qglIsTexture = NULL;
	qglLightModelx = NULL;
	qglLightModelxv = NULL;
	qglLightx = NULL;
	qglLightxv = NULL;
	qglLineWidthx = NULL;
	qglLoadIdentity = NULL;
	qglLoadMatrixx = NULL;
	qglLogicOp = NULL;
	qglMaterialx = NULL;
	qglMaterialxv = NULL;
	qglMatrixMode = NULL;
	qglMultMatrixx = NULL;
	qglMultiTexCoord4x = NULL;
	qglNormal3x = NULL;
	qglNormalPointer = NULL;
	qglOrthox = NULL;
	qglPixelStorei = NULL;
	qglPointParameterx = NULL;
	qglPointParameterxv = NULL;
	qglPointSizex = NULL;
	qglPolygonOffsetx = NULL;
	qglPopMatrix = NULL;
	qglPushMatrix = NULL;
	qglReadPixels = NULL;
	qglRotatex = NULL;
	qglSampleCoverage = NULL;
	qglSampleCoveragex = NULL;
	qglScalex = NULL;
	qglScissor = NULL;
	qglShadeModel = NULL;
	qglStencilFunc = NULL;
	qglStencilMask = NULL;
	qglStencilOp = NULL;
	qglTexCoordPointer = NULL;
	qglTexEnvi = NULL;
	qglTexEnvx = NULL;
	qglTexEnviv = NULL;
	qglTexEnvxv = NULL;
	qglTexImage2D = NULL;
	qglTexParameteri = NULL;
	qglTexParameterx = NULL;
	qglTexParameteriv = NULL;
	qglTexParameterxv = NULL;
	qglTexSubImage2D = NULL;
	qglTranslatex = NULL;
	qglVertexPointer = NULL;
	qglViewport = NULL;
	qglPointSizePointerOES = NULL;

}

#define GPA( a ) dlsym( glw_state.OpenGLLib, a )

void *qwglGetProcAddress(char *symbol)
{
	if (glw_state.OpenGLLib)
		return GPA ( symbol );
	return NULL;
}

/*
** QGL_Init
**
** This is responsible for binding our qgl function pointers to 
** the appropriate GL stuff.  In Windows this means doing a 
** LoadLibrary and a bunch of calls to GetProcAddress.  On other
** operating systems we need to do the right thing, whatever that
** might be.
** 
*/

qboolean QGL_Init( const char *dllname )
{
	// update 3Dfx gamma irrespective of underlying DLL
	{
		char envbuffer[1024];
		float g;

		g = 2.00 * ( 0.8 - ( vid_gamma->value - 0.5 ) ) + 1.0F;
		Com_sprintf( envbuffer, sizeof(envbuffer), "SSTV2_GAMMA=%f", g );
		putenv( envbuffer );
		Com_sprintf( envbuffer, sizeof(envbuffer), "SST_GAMMA=%f", g );
		putenv( envbuffer );
	}

	if ( glw_state.OpenGLLib )
		QGL_Shutdown();
	
	if ( ( glw_state.OpenGLLib = dlopen( dllname, RTLD_LAZY ) ) == 0 )
	{
		char	fn[MAX_OSPATH];
		char	*path;

//		ri.Con_Printf(PRINT_ALL, "QGL_Init: Can't load %s from /etc/ld.so.conf: %s\n", 
//				dllname, dlerror());

		// try basedir next
		path = ri.Cvar_Get ("basedir", ".", CVAR_NOSET)->string;
		
		snprintf (fn, MAX_OSPATH, "%s/%s", path, dllname );

		if ( ( glw_state.OpenGLLib = dlopen( fn, RTLD_LAZY ) ) == 0 ) {
			ri.Con_Printf( PRINT_ALL, "%s\n", dlerror() );
			return false;
		}
		Com_Printf ("Using %s for OpenGL...", fn); 
	} else {
		Com_Printf ("Using %s for OpenGL...", dllname);
	}

#ifdef USE_GLU
	if ( (dlopen( "libGLU.so", RTLD_LAZY ) ) == 0 )
	{
	  ri.Con_Printf( PRINT_ALL, "%s\n", dlerror() );
	  return false;
	} else {
	  Com_Printf ("Opened GLU sucessfully OpenGLU...", dllname);
	}
#endif
	qglAlphaFunc = dllAlphaFunc = GPA( "glAlphaFunc" );
	qglClearColor = dllClearColor = GPA( "glClearColor" );
	qglClearDepthf = dllClearDepthf = GPA( "glClearDepthf" );
	qglClipPlanef = dllClipPlanef = GPA( "glClipPlanef" );
	qglColor4f = dllColor4f = GPA( "glColor4f" );
	qglDepthRangef = dllDepthRangef = GPA( "glDepthRangef" );
	qglFogf = dllFogf = GPA( "glFogf" );
	qglFogfv = dllFogfv = GPA( "glFogfv" );
	qglFrustumf = dllFrustumf = GPA( "glFrustumf" );
	qglGetClipPlanef = dllGetClipPlanef = GPA( "glGetClipPlanef" );
	qglGetFloatv = dllGetFloatv = GPA( "glGetFloatv" );
	qglGetLightfv = dllGetLightfv = GPA( "glGetLightfv" );
	qglGetMaterialfv = dllGetMaterialfv = GPA( "glGetMaterialfv" );
	qglGetTexEnvfv = dllGetTexEnvfv = GPA( "glGetTexEnvfv" );
	qglGetTexParameterfv = dllGetTexParameterfv = GPA( "glGetTexParameterfv" );
	qglLightModelf = dllLightModelf = GPA( "glLightModelf" );
	qglLightModelfv = dllLightModelfv = GPA( "glLightModelfv" );
	qglLightf = dllLightf = GPA( "glLightf" );
	qglLightfv = dllLightfv = GPA( "glLightfv" );
	qglLineWidth = dllLineWidth = GPA( "glLineWidth" );
	qglLoadMatrixf = dllLoadMatrixf = GPA( "glLoadMatrixf" );
	qglMaterialf = dllMaterialf = GPA( "glMaterialf" );
	qglMaterialfv = dllMaterialfv = GPA( "glMaterialfv" );
	qglMultMatrixf = dllMultMatrixf = GPA( "glMultMatrixf" );
	/*qglMultiTexCoord4f = */dllMultiTexCoord4f = GPA( "glMultiTexCoord4f" );
	qglNormal3f = dllNormal3f = GPA( "glNormal3f" );
	qglOrthof = dllOrthof = GPA( "glOrthof" );
	/*qglPointParameterf = */dllPointParameterf = GPA( "glPointParameterf" );
	/*qglPointParameterfv = */dllPointParameterfv = GPA( "glPointParameterfv" );
	qglPointSize = dllPointSize = GPA( "glPointSize" );
	qglPolygonOffset = dllPolygonOffset = GPA( "glPolygonOffset" );
	qglRotatef = dllRotatef = GPA( "glRotatef" );
	qglScalef = dllScalef = GPA( "glScalef" );
	qglTexEnvf = dllTexEnvf = GPA( "glTexEnvf" );
	qglTexEnvfv = dllTexEnvfv = GPA( "glTexEnvfv" );
	qglTexParameterf = dllTexParameterf = GPA( "glTexParameterf" );
	qglTexParameterfv = dllTexParameterfv = GPA( "glTexParameterfv" );
	qglTranslatef = dllTranslatef = GPA( "glTranslatef" );
	/*qglActiveTexture = */dllActiveTexture = GPA( "glActiveTexture" );
	qglAlphaFuncx = dllAlphaFuncx = GPA( "glAlphaFuncx" );
	qglBindBuffer = dllBindBuffer = GPA( "glBindBuffer" );
	qglBindTexture = dllBindTexture = GPA( "glBindTexture" );
	qglBlendFunc = dllBlendFunc = GPA( "glBlendFunc" );
	qglBufferData = dllBufferData = GPA( "glBufferData" );
	qglBufferSubData = dllBufferSubData = GPA( "glBufferSubData" );
	qglClear = dllClear = GPA( "glClear" );
	qglClearColorx = dllClearColorx = GPA( "glClearColorx" );
	qglClearDepthx = dllClearDepthx = GPA( "glClearDepthx" );
	qglClearStencil = dllClearStencil = GPA( "glClearStencil" );
	/*qglClientActiveTexture = */dllClientActiveTexture = GPA( "glClientActiveTexture" );
	qglClipPlanex = dllClipPlanex = GPA( "glClipPlanex" );
	qglColor4ub = dllColor4ub = GPA( "glColor4ub" );
	qglColor4x = dllColor4x = GPA( "glColor4x" );
	qglColorMask = dllColorMask = GPA( "glColorMask" );
	qglColorPointer = dllColorPointer = GPA( "glColorPointer" );
	qglCompressedTexImage2D = dllCompressedTexImage2D = GPA( "glCompressedTexImage2D" );
	qglCompressedTexSubImage2D = dllCompressedTexSubImage2D = GPA( "glCompressedTexSubImage2D" );
	qglCopyTexImage2D = dllCopyTexImage2D = GPA( "glCopyTexImage2D" );
	qglCopyTexSubImage2D = dllCopyTexSubImage2D = GPA( "glCopyTexSubImage2D" );
	qglCullFace = dllCullFace = GPA( "glCullFace" );
	qglDeleteBuffers = dllDeleteBuffers = GPA( "glDeleteBuffers" );
	qglDeleteTextures = dllDeleteTextures = GPA( "glDeleteTextures" );
	qglDepthFunc = dllDepthFunc = GPA( "glDepthFunc" );
	qglDepthMask = dllDepthMask = GPA( "glDepthMask" );
	qglDepthRangex = dllDepthRangex = GPA( "glDepthRangex" );
	qglDisable = dllDisable = GPA( "glDisable" );
	qglDisableClientState = dllDisableClientState = GPA( "glDisableClientState" );
	qglDrawArrays = dllDrawArrays = GPA( "glDrawArrays" );
	qglDrawElements = dllDrawElements = GPA( "glDrawElements" );
	qglEnable = dllEnable = GPA( "glEnable" );
	qglEnableClientState = dllEnableClientState = GPA( "glEnableClientState" );
	qglFinish = dllFinish = GPA( "glFinish" );
	qglFlush = dllFlush = GPA( "glFlush" );
	qglFogx = dllFogx = GPA( "glFogx" );
	qglFogxv = dllFogxv = GPA( "glFogxv" );
	qglFrontFace = dllFrontFace = GPA( "glFrontFace" );
	qglFrustumx = dllFrustumx = GPA( "glFrustumx" );
	qglGetBooleanv = dllGetBooleanv = GPA( "glGetBooleanv" );
	qglGetBufferParameteriv = dllGetBufferParameteriv = GPA( "glGetBufferParameteriv" );
	qglGetClipPlanex = dllGetClipPlanex = GPA( "glGetClipPlanex" );
	qglGenBuffers = dllGenBuffers = GPA( "glGenBuffers" );
	qglGenTextures = dllGenTextures = GPA( "glGenTextures" );
	qglGetError = dllGetError = GPA( "glGetError" );
	qglGetFixedv = dllGetFixedv = GPA( "glGetFixedv" );
	qglGetIntegerv = dllGetIntegerv = GPA( "glGetIntegerv" );
	qglGetLightxv = dllGetLightxv = GPA( "glGetLightxv" );
	qglGetMaterialxv = dllGetMaterialxv = GPA( "glGetMaterialxv" );
	qglGetPointerv = dllGetPointerv = GPA( "glGetPointerv" );
	qglGetString = dllGetString = GPA( "glGetString" );
	qglGetTexEnviv = dllGetTexEnviv = GPA( "glGetTexEnviv" );
	qglGetTexEnvxv = dllGetTexEnvxv = GPA( "glGetTexEnvxv" );
	qglGetTexParameteriv = dllGetTexParameteriv = GPA( "glGetTexParameteriv" );
	qglGetTexParameterxv = dllGetTexParameterxv = GPA( "glGetTexParameterxv" );
	qglHint = dllHint = GPA( "glHint" );
	qglIsBuffer = dllIsBuffer = GPA( "glIsBuffer" );
	qglIsEnabled = dllIsEnabled = GPA( "glIsEnabled" );
	qglIsTexture = dllIsTexture = GPA( "glIsTexture" );
	qglLightModelx = dllLightModelx = GPA( "glLightModelx" );
	qglLightModelxv = dllLightModelxv = GPA( "glLightModelxv" );
	qglLightx = dllLightx = GPA( "glLightx" );
	qglLightxv = dllLightxv = GPA( "glLightxv" );
	qglLineWidthx = dllLineWidthx = GPA( "glLineWidthx" );
	qglLoadIdentity = dllLoadIdentity = GPA( "glLoadIdentity" );
	qglLoadMatrixx = dllLoadMatrixx = GPA( "glLoadMatrixx" );
	qglLogicOp = dllLogicOp = GPA( "glLogicOp" );
	qglMaterialx = dllMaterialx = GPA( "glMaterialx" );
	qglMaterialxv = dllMaterialxv = GPA( "glMaterialxv" );
	qglMatrixMode = dllMatrixMode = GPA( "glMatrixMode" );
	qglMultMatrixx = dllMultMatrixx = GPA( "glMultMatrixx" );
	/*qglMultiTexCoord4x = */dllMultiTexCoord4x = GPA( "glMultiTexCoord4x" );
	qglNormal3x = dllNormal3x = GPA( "glNormal3x" );
	qglNormalPointer = dllNormalPointer = GPA( "glNormalPointer" );
	qglOrthox = dllOrthox = GPA( "glOrthox" );
	qglPixelStorei = dllPixelStorei = GPA( "glPixelStorei" );
	/*qglPointParameterx = */dllPointParameterx = GPA( "glPointParameterx" );

	/*qglPointParameterxv = */dllPointParameterxv = GPA( "glPointParameterxv" );
	qglPointSizex = dllPointSizex = GPA( "glPointSizex" );
	qglPolygonOffsetx = dllPolygonOffsetx = GPA( "glPolygonOffsetx" );
	qglPopMatrix = dllPopMatrix = GPA( "glPopMatrix" );
	qglPushMatrix = dllPushMatrix = GPA( "glPushMatrix" );
	qglReadPixels = dllReadPixels = GPA( "glReadPixels" );
	qglRotatex = dllRotatex = GPA( "glRotatex" );
	qglSampleCoverage = dllSampleCoverage = GPA( "glSampleCoverage" );
	qglSampleCoveragex = dllSampleCoveragex = GPA( "glSampleCoveragex" );
	qglScalex = dllScalex = GPA( "glScalex" );
	qglScissor = dllScissor = GPA( "glScissor" );
	qglShadeModel = dllShadeModel = GPA( "glShadeModel" );
	qglStencilFunc = dllStencilFunc = GPA( "glStencilFunc" );
	qglStencilMask = dllStencilMask = GPA( "glStencilMask" );
	qglStencilOp = dllStencilOp = GPA( "glStencilOp" );
	qglTexCoordPointer = dllTexCoordPointer = GPA( "glTexCoordPointer" );
	qglTexEnvi = dllTexEnvi = GPA( "glTexEnvi" );
	qglTexEnvx = dllTexEnvx = GPA( "glTexEnvx" );
	qglTexEnviv = dllTexEnviv = GPA( "glTexEnviv" );
	qglTexEnvxv = dllTexEnvxv = GPA( "glTexEnvxv" );
	qglTexImage2D = dllTexImage2D = GPA( "glTexImage2D" );
	qglTexParameteri = dllTexParameteri = GPA( "glTexParameteri" );
	qglTexParameterx = dllTexParameterx = GPA( "glTexParameterx" );
	qglTexParameteriv = dllTexParameteriv = GPA( "glTexParameteriv" );
	qglTexParameterxv = dllTexParameterxv = GPA( "glTexParameterxv" );
	qglTexSubImage2D = dllTexSubImage2D = GPA( "glTexSubImage2D" );
	qglTranslatex = dllTranslatex = GPA( "glTranslatex" );
	qglVertexPointer = dllVertexPointer = GPA( "glVertexPointer" );
	qglViewport = dllViewport = GPA( "glViewport" );
	qglPointSizePointerOES = dllPointSizePointerOES = GPA( "glPointSizePointerOES" );

	
	qglLockArraysEXT = 0;
	qglUnlockArraysEXT = 0;
	qglPointParameterfEXT = 0;
	qglPointParameterfvEXT = 0;
	qglColorTableEXT = 0;
	qgl3DfxSetPaletteEXT = 0;
	qglSelectTextureSGIS = 0;
	qglMTexCoord2fSGIS = 0;
	qglActiveTextureARB = 0;
	qglClientActiveTextureARB = 0;

#ifdef _HARMATTAN
	karinEnablePointParameter(0, 1);
	karinEnableMultiTexture(0, 1);
#endif
	// karin(debug)
#ifdef _HARMATTAN_DEBUG
	GLimp_EnableLogging(1);
#endif
	return true;
}

void GLimp_EnableLogging( qboolean enable )
{
	if ( enable )
	{
		if ( !glw_state.log_fp )
		{
			struct tm *newtime;
			time_t aclock;
			char buffer[1024];

			time( &aclock );
			newtime = localtime( &aclock );

			asctime( newtime );

			Com_sprintf( buffer, sizeof(buffer), "%s/gl.log", ri.FS_Gamedir() ); 
			glw_state.log_fp = fopen( buffer, "wt" );

			fprintf( glw_state.log_fp, "%s\n", asctime( newtime ) );
		}

		qglAlphaFunc = logAlphaFunc;
		qglClearColor = logClearColor;
		qglClearDepthf = logClearDepthf;
		qglClipPlanef = logClipPlanef;
		qglColor4f = logColor4f;
		qglDepthRangef = logDepthRangef;
		qglFogf = logFogf;
		qglFogfv = logFogfv;
		qglFrustumf = logFrustumf;
		qglGetClipPlanef = logGetClipPlanef;
		qglGetFloatv = logGetFloatv;
		qglGetLightfv = logGetLightfv;
		qglGetMaterialfv = logGetMaterialfv;
		qglGetTexEnvfv = logGetTexEnvfv;
		qglGetTexParameterfv = logGetTexParameterfv;
		qglLightModelf = logLightModelf;
		qglLightModelfv = logLightModelfv;
		qglLightf = logLightf;
		qglLightfv = logLightfv;
		qglLineWidth = logLineWidth;
		qglLoadMatrixf = logLoadMatrixf;
		qglMaterialf = logMaterialf;
		qglMaterialfv = logMaterialfv;
		qglMultMatrixf = logMultMatrixf;
		//qglMultiTexCoord4f = logMultiTexCoord4f;
		qglNormal3f = logNormal3f;
		qglOrthof = logOrthof;
		//qglPointParameterf = logPointParameterf;
		//qglPointParameterfv = logPointParameterfv;
		qglPointSize = logPointSize;
		qglPolygonOffset = logPolygonOffset;
		qglRotatef = logRotatef;
		qglScalef = logScalef;
		qglTexEnvf = logTexEnvf;
		qglTexEnvfv = logTexEnvfv;
		qglTexParameterf = logTexParameterf;
		qglTexParameterfv = logTexParameterfv;
		qglTranslatef = logTranslatef;
		//qglActiveTexture = logActiveTexture;
		qglAlphaFuncx = logAlphaFuncx;
		qglBindBuffer = logBindBuffer;
		qglBindTexture = logBindTexture;
		qglBlendFunc = logBlendFunc;
		qglBufferData = logBufferData;
		qglBufferSubData = logBufferSubData;
		qglClear = logClear;
		qglClearColorx = logClearColorx;
		qglClearDepthx = logClearDepthx;
		qglClearStencil = logClearStencil;
		//qglClientActiveTexture = logClientActiveTexture;
		qglClipPlanex = logClipPlanex;
		qglColor4ub = logColor4ub;
		qglColor4x = logColor4x;
		qglColorMask = logColorMask;
		qglColorPointer = logColorPointer;
		qglCompressedTexImage2D = logCompressedTexImage2D;
		qglCompressedTexSubImage2D = logCompressedTexSubImage2D;
		qglCopyTexImage2D = logCopyTexImage2D;
		qglCopyTexSubImage2D = logCopyTexSubImage2D;
		qglCullFace = logCullFace;
		qglDeleteBuffers = logDeleteBuffers;
		qglDeleteTextures = logDeleteTextures;
		qglDepthFunc = logDepthFunc;
		qglDepthMask = logDepthMask;
		qglDepthRangex = logDepthRangex;
		qglDisable = logDisable;
		qglDisableClientState = logDisableClientState;
		qglDrawArrays = logDrawArrays;
		qglDrawElements = logDrawElements;
		qglEnable = logEnable;
		qglEnableClientState = logEnableClientState;
		qglFinish = logFinish;
		qglFlush = logFlush;
		qglFogx = logFogx;
		qglFogxv = logFogxv;
		qglFrontFace = logFrontFace;
		qglFrustumx = logFrustumx;
		qglGetBooleanv = logGetBooleanv;
		qglGetBufferParameteriv = logGetBufferParameteriv;
		qglGetClipPlanex = logGetClipPlanex;
		qglGenBuffers = logGenBuffers;
		qglGenTextures = logGenTextures;
		qglGetError = logGetError;
		qglGetFixedv = logGetFixedv;
		qglGetIntegerv = logGetIntegerv;
		qglGetLightxv = logGetLightxv;
		qglGetMaterialxv = logGetMaterialxv;
		qglGetPointerv = logGetPointerv;
		qglGetString = logGetString;
		qglGetTexEnviv = logGetTexEnviv;
		qglGetTexEnvxv = logGetTexEnvxv;
		qglGetTexParameteriv = logGetTexParameteriv;
		qglGetTexParameterxv = logGetTexParameterxv;
		qglHint = logHint;
		qglIsBuffer = logIsBuffer;
		qglIsEnabled = logIsEnabled;
		qglIsTexture = logIsTexture;
		qglLightModelx = logLightModelx;
		qglLightModelxv = logLightModelxv;
		qglLightx = logLightx;
		qglLightxv = logLightxv;
		qglLineWidthx = logLineWidthx;
		qglLoadIdentity = logLoadIdentity;
		qglLoadMatrixx = logLoadMatrixx;
		qglLogicOp = logLogicOp;
		qglMaterialx = logMaterialx;
		qglMaterialxv = logMaterialxv;
		qglMatrixMode = logMatrixMode;
		qglMultMatrixx = logMultMatrixx;
		//qglMultiTexCoord4x = logMultiTexCoord4x;
		qglNormal3x = logNormal3x;
		qglNormalPointer = logNormalPointer;
		qglOrthox = logOrthox;
		qglPixelStorei = logPixelStorei;
		//qglPointParameterx = logPointParameterx;
		//qglPointParameterxv = logPointParameterxv;
		qglPointSizex = logPointSizex;
		qglPolygonOffsetx = logPolygonOffsetx;
		qglPopMatrix = logPopMatrix;
		qglPushMatrix = logPushMatrix;
		qglReadPixels = logReadPixels;
		qglRotatex = logRotatex;
		qglSampleCoverage = logSampleCoverage;
		qglSampleCoveragex = logSampleCoveragex;
		qglScalex = logScalex;
		qglScissor = logScissor;
		qglShadeModel = logShadeModel;
		qglStencilFunc = logStencilFunc;
		qglStencilMask = logStencilMask;
		qglStencilOp = logStencilOp;
		qglTexCoordPointer = logTexCoordPointer;
		qglTexEnvi = logTexEnvi;
		qglTexEnvx = logTexEnvx;
		qglTexEnviv = logTexEnviv;
		qglTexEnvxv = logTexEnvxv;
		qglTexImage2D = logTexImage2D;
		qglTexParameteri = logTexParameteri;
		qglTexParameterx = logTexParameterx;
		qglTexParameteriv = logTexParameteriv;
		qglTexParameterxv = logTexParameterxv;
		qglTexSubImage2D = logTexSubImage2D;
		qglTranslatex = logTranslatex;
		qglVertexPointer = logVertexPointer;
		qglViewport = logViewport;
		qglPointSizePointerOES = logPointSizePointerOES;

	}
	else
	{
		qglAlphaFunc = dllAlphaFunc;
		qglClearColor = dllClearColor;
		qglClearDepthf = dllClearDepthf;
		qglClipPlanef = dllClipPlanef;
		qglColor4f = dllColor4f;
		qglDepthRangef = dllDepthRangef;
		qglFogf = dllFogf;
		qglFogfv = dllFogfv;
		qglFrustumf = dllFrustumf;
		qglGetClipPlanef = dllGetClipPlanef;
		qglGetFloatv = dllGetFloatv;
		qglGetLightfv = dllGetLightfv;
		qglGetMaterialfv = dllGetMaterialfv;
		qglGetTexEnvfv = dllGetTexEnvfv;
		qglGetTexParameterfv = dllGetTexParameterfv;
		qglLightModelf = dllLightModelf;
		qglLightModelfv = dllLightModelfv;
		qglLightf = dllLightf;
		qglLightfv = dllLightfv;
		qglLineWidth = dllLineWidth;
		qglLoadMatrixf = dllLoadMatrixf;
		qglMaterialf = dllMaterialf;
		qglMaterialfv = dllMaterialfv;
		qglMultMatrixf = dllMultMatrixf;
		//qglMultiTexCoord4f = dllMultiTexCoord4f;
		qglNormal3f = dllNormal3f;
		qglOrthof = dllOrthof;
		//qglPointParameterf = dllPointParameterf;
		//qglPointParameterfv = dllPointParameterfv;
		qglPointSize = dllPointSize;
		qglPolygonOffset = dllPolygonOffset;
		qglRotatef = dllRotatef;
		qglScalef = dllScalef;
		qglTexEnvf = dllTexEnvf;
		qglTexEnvfv = dllTexEnvfv;
		qglTexParameterf = dllTexParameterf;
		qglTexParameterfv = dllTexParameterfv;
		qglTranslatef = dllTranslatef;
		//qglActiveTexture = dllActiveTexture;
		qglAlphaFuncx = dllAlphaFuncx;
		qglBindBuffer = dllBindBuffer;
		qglBindTexture = dllBindTexture;
		qglBlendFunc = dllBlendFunc;
		qglBufferData = dllBufferData;
		qglBufferSubData = dllBufferSubData;
		qglClear = dllClear;
		qglClearColorx = dllClearColorx;
		qglClearDepthx = dllClearDepthx;
		qglClearStencil = dllClearStencil;
		//qglClientActiveTexture = dllClientActiveTexture;
		qglClipPlanex = dllClipPlanex;
		qglColor4ub = dllColor4ub;
		qglColor4x = dllColor4x;
		qglColorMask = dllColorMask;
		qglColorPointer = dllColorPointer;
		qglCompressedTexImage2D = dllCompressedTexImage2D;
		qglCompressedTexSubImage2D = dllCompressedTexSubImage2D;
		qglCopyTexImage2D = dllCopyTexImage2D;
		qglCopyTexSubImage2D = dllCopyTexSubImage2D;
		qglCullFace = dllCullFace;
		qglDeleteBuffers = dllDeleteBuffers;
		qglDeleteTextures = dllDeleteTextures;
		qglDepthFunc = dllDepthFunc;
		qglDepthMask = dllDepthMask;
		qglDepthRangex = dllDepthRangex;
		qglDisable = dllDisable;
		qglDisableClientState = dllDisableClientState;
		qglDrawArrays = dllDrawArrays;
		qglDrawElements = dllDrawElements;
		qglEnable = dllEnable;
		qglEnableClientState = dllEnableClientState;
		qglFinish = dllFinish;
		qglFlush = dllFlush;
		qglFogx = dllFogx;
		qglFogxv = dllFogxv;
		qglFrontFace = dllFrontFace;
		qglFrustumx = dllFrustumx;
		qglGetBooleanv = dllGetBooleanv;
		qglGetBufferParameteriv = dllGetBufferParameteriv;
		qglGetClipPlanex = dllGetClipPlanex;
		qglGenBuffers = dllGenBuffers;
		qglGenTextures = dllGenTextures;
		qglGetError = dllGetError;
		qglGetFixedv = dllGetFixedv;
		qglGetIntegerv = dllGetIntegerv;
		qglGetLightxv = dllGetLightxv;
		qglGetMaterialxv = dllGetMaterialxv;
		qglGetPointerv = dllGetPointerv;
		qglGetString = dllGetString;
		qglGetTexEnviv = dllGetTexEnviv;
		qglGetTexEnvxv = dllGetTexEnvxv;
		qglGetTexParameteriv = dllGetTexParameteriv;
		qglGetTexParameterxv = dllGetTexParameterxv;
		qglHint = dllHint;
		qglIsBuffer = dllIsBuffer;
		qglIsEnabled = dllIsEnabled;
		qglIsTexture = dllIsTexture;
		qglLightModelx = dllLightModelx;
		qglLightModelxv = dllLightModelxv;
		qglLightx = dllLightx;
		qglLightxv = dllLightxv;
		qglLineWidthx = dllLineWidthx;
		qglLoadIdentity = dllLoadIdentity;
		qglLoadMatrixx = dllLoadMatrixx;
		qglLogicOp = dllLogicOp;
		qglMaterialx = dllMaterialx;
		qglMaterialxv = dllMaterialxv;
		qglMatrixMode = dllMatrixMode;
		qglMultMatrixx = dllMultMatrixx;
		//qglMultiTexCoord4x = dllMultiTexCoord4x;
		qglNormal3x = dllNormal3x;
		qglNormalPointer = dllNormalPointer;
		qglOrthox = dllOrthox;
		qglPixelStorei = dllPixelStorei;
		//qglPointParameterx = dllPointParameterx;
		//qglPointParameterxv = dllPointParameterxv;
		qglPointSizex = dllPointSizex;
		qglPolygonOffsetx = dllPolygonOffsetx;
		qglPopMatrix = dllPopMatrix;
		qglPushMatrix = dllPushMatrix;
		qglReadPixels = dllReadPixels;
		qglRotatex = dllRotatex;
		qglSampleCoverage = dllSampleCoverage;
		qglSampleCoveragex = dllSampleCoveragex;
		qglScalex = dllScalex;
		qglScissor = dllScissor;
		qglShadeModel = dllShadeModel;
		qglStencilFunc = dllStencilFunc;
		qglStencilMask = dllStencilMask;
		qglStencilOp = dllStencilOp;
		qglTexCoordPointer = dllTexCoordPointer;
		qglTexEnvi = dllTexEnvi;
		qglTexEnvx = dllTexEnvx;
		qglTexEnviv = dllTexEnviv;
		qglTexEnvxv = dllTexEnvxv;
		qglTexImage2D = dllTexImage2D;
		qglTexParameteri = dllTexParameteri;
		qglTexParameterx = dllTexParameterx;
		qglTexParameteriv = dllTexParameteriv;
		qglTexParameterxv = dllTexParameterxv;
		qglTexSubImage2D = dllTexSubImage2D;
		qglTranslatex = dllTranslatex;
		qglVertexPointer = dllVertexPointer;
		qglViewport = dllViewport;
		qglPointSizePointerOES = dllPointSizePointerOES;

	}

#ifdef _HARMATTAN
	karinEnablePointParameter(enable ? 1 : 2, 0);
	karinEnableMultiTexture(enable ? 1 : 2, 0);
#endif

}


void GLimp_LogNewFrame( void )
{
	fprintf( glw_state.log_fp, "*** R_BeginFrame ***\n" );
}

#ifdef _HARMATTAN
void karinEnablePointParameter(unsigned enable, unsigned force)
{
	if(force)
	{
		qglPointParameterf = (enable == 0 ? NULL : (enable == 1 ? logPointParameterf : dllPointParameterf));
		qglPointParameterfv = (enable == 0 ? NULL : (enable == 1 ? logPointParameterfv : dllPointParameterfv));
		qglPointParameterx = (enable == 0 ? NULL : (enable == 1 ? logPointParameterx : dllPointParameterx));
		qglPointParameterxv = (enable == 0 ? NULL : (enable == 1 ? logPointParameterxv : dllPointParameterxv));
		printf("force %d\n", enable);
	}
	else
	{
		if(qglPointParameterf != NULL && enable > 0)
			qglPointParameterf = (enable == 1 ? logPointParameterf : dllPointParameterf);
		if(qglPointParameterfv != NULL && enable > 0)
			qglPointParameterfv = (enable == 1 ? logPointParameterfv : dllPointParameterfv);
		if(qglPointParameterx != NULL && enable > 0)
			qglPointParameterx = (enable == 1 ? logPointParameterx : dllPointParameterx);
		if(qglPointParameterxv != NULL && enable > 0)
			qglPointParameterxv = (enable == 1 ? logPointParameterxv : dllPointParameterxv);
	}
}

void karinEnableMultiTexture(unsigned enable, unsigned force)
{
	if(force)
	{
		qglMultiTexCoord4f = (enable == 0 ? NULL : (enable == 1 ? logMultiTexCoord4f : dllMultiTexCoord4f));
		qglMultiTexCoord4x = (enable == 0 ? NULL : (enable == 1 ? logMultiTexCoord4x : dllMultiTexCoord4x));

		qglActiveTexture = (enable == 0 ? NULL : (enable == 1 ? logActiveTexture : dllActiveTexture));
		qglClientActiveTexture = (enable == 0 ? NULL : (enable == 1 ? logClientActiveTexture : dllClientActiveTexture));
	}
	else
	{
		if(qglMultiTexCoord4f != NULL && enable > 0)
			qglMultiTexCoord4f = (enable == 1 ? logMultiTexCoord4f : dllMultiTexCoord4f);
		if(qglMultiTexCoord4x != NULL && enable > 0)
			qglMultiTexCoord4x = (enable == 1 ? logMultiTexCoord4x : dllMultiTexCoord4x);
		if(qglActiveTexture != NULL && enable > 0)
			qglActiveTexture = (enable == 1 ? logActiveTexture : dllActiveTexture);
		if(qglClientActiveTexture != NULL && enable > 0)
			qglClientActiveTexture = (enable == 1 ? logClientActiveTexture : dllClientActiveTexture);
	}
}
#endif
