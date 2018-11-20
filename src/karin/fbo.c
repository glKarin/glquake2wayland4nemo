#include "fbo.h"

#include <stdio.h>
#include <string.h>

#include <EGL/egl.h>

#include "gl_vkb.h"

static unsigned fboProcInited = 0;
PFNGLISRENDERBUFFEROESPROC qglIsRenderbuffer = NULL;
PFNGLBINDRENDERBUFFEROESPROC qglBindRenderbuffer = NULL;
PFNGLDELETERENDERBUFFERSOESPROC qglDeleteRenderbuffers = NULL;
PFNGLGENRENDERBUFFERSOESPROC qglGenRenderbuffers = NULL;
PFNGLRENDERBUFFERSTORAGEOESPROC qglRenderbufferStorage = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVOESPROC qglGetRenderbufferParameteriv = NULL;
PFNGLISFRAMEBUFFEROESPROC qglIsFramebuffer = NULL;
PFNGLBINDFRAMEBUFFEROESPROC qglBindFramebuffer = NULL;
PFNGLDELETEFRAMEBUFFERSOESPROC qglDeleteFramebuffers = NULL;
PFNGLGENFRAMEBUFFERSOESPROC qglGenFramebuffers = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSOESPROC qglCheckFramebufferStatus = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEROESPROC qglFramebufferRenderbuffer = NULL;
PFNGLFRAMEBUFFERTEXTURE2DOESPROC qglFramebufferTexture2D = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVOESPROC qglGetFramebufferAttachmentParameteriv = NULL;
PFNGLGENERATEMIPMAPOESPROC qglGenerateMipmap = NULL;

static const GLuint M_MinSize = 2;
static const GLenum M_TextureFormat = GL_RGB;

static void bind(karin_FrameBufferObject *fbo);
static void unbind(karin_FrameBufferObject *fbo);
static unsigned isBind(const karin_FrameBufferObject *fbo);
static void blit(karin_FrameBufferObject *fbo);
static void resize(karin_FrameBufferObject *fbo, GLuint w, GLuint h, GLuint vw, GLuint vh);
static unsigned init(karin_FrameBufferObject *fbo, GLuint width, GLuint height);
static void delete_karin_FrameBufferObject(karin_FrameBufferObject *fbo);
static void destory(karin_FrameBufferObject *fbo);
static void initBuffer(karin_FrameBufferObject *fbo, GLfloat w, GLfloat h);
static void attachFrameBuffer(karin_FrameBufferObject *fbo, GLuint w, GLuint h);

static void getFBOProc(void);
static unsigned getError(void);
static unsigned getFBOStatus(void);
static void printRenderBufferParams(void);
static GLuint genTexture2D(GLuint width, GLuint height);

static GLint active_texture;
static GLint client_active_texture;
static GLboolean texture2d;
static GLfloat alpha_ref;
static GLint alpha_func;
static GLint blend_src;
static GLint blend_dst;
static GLboolean blend;
static GLboolean alpha_test;
static GLboolean depth_test;
static GLboolean scissor_test;
static GLint matrix_mode;
static GLint viewport[4];

static void karinBeginRender2D(GLint width, GLint height)
{
	if(render_lock)
		return;
	// Get
	{
		texture2d = qglIsEnabled(GL_TEXTURE_2D);
		blend = qglIsEnabled(GL_BLEND);
		alpha_test = qglIsEnabled(GL_ALPHA_TEST);
		depth_test = qglIsEnabled(GL_DEPTH_TEST);
		scissor_test = qglIsEnabled(GL_SCISSOR_TEST);
		if(qglActiveTexture)
			qglGetIntegerv(GL_ACTIVE_TEXTURE, &active_texture);
		if(qglClientActiveTexture)
			qglGetIntegerv(GL_CLIENT_ACTIVE_TEXTURE, &client_active_texture);
		qglGetFloatv(GL_ALPHA_TEST_REF, &alpha_ref);
		qglGetIntegerv(GL_ALPHA_TEST_FUNC, &alpha_func);
		qglGetIntegerv(GL_BLEND_SRC, &blend_src);
		qglGetIntegerv(GL_BLEND_DST, &blend_dst);
		qglGetIntegerv(GL_MATRIX_MODE, &matrix_mode);
		qglGetIntegerv(GL_VIEWPORT, viewport);
	}
	// Set
	{
		qglAlphaFunc(GL_GREATER, 0.1);
		if(alpha_test)
			qglDisable(GL_ALPHA_TEST);
		if(blend)
			qglDisable(GL_BLEND);
		qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		if(!texture2d)
			qglEnable(GL_TEXTURE_2D);
		if(depth_test)
			qglDisable(GL_DEPTH_TEST);
		if(scissor_test)
			qglDisable(GL_SCISSOR_TEST);
		if(qglClientActiveTexture)
			qglClientActiveTexture(GL_TEXTURE0);
		if(qglActiveTexture)
			qglActiveTexture(GL_TEXTURE0);
	}
	// Matrix
	{
		qglViewport(0, 0, width, height);
		qglMatrixMode(GL_PROJECTION);
		qglPushMatrix();
		qglLoadIdentity();
		qglOrthof(0.0, width, 0.0, height, -1.0, 1.0);
		qglMatrixMode(GL_MODELVIEW);
		qglPushMatrix();
		qglLoadIdentity();
	}
	render_lock = btrue;
}

static void karinEndRender2D(void)
{
	if(!render_lock)
		return;
	{
		qglMatrixMode(GL_MODELVIEW);
		qglPopMatrix();
		qglMatrixMode(GL_PROJECTION);
		qglPopMatrix();
	}
	{
		if(!texture2d)
			qglDisable(GL_TEXTURE_2D);
		if(qglClientActiveTexture)
			qglClientActiveTexture(client_active_texture);
		if(qglActiveTexture)
			qglActiveTexture(active_texture);
		qglAlphaFunc(alpha_func, alpha_ref);
		if(alpha_test)
			qglEnable(GL_ALPHA_TEST);
		if(blend)
			qglEnable(GL_BLEND);
		qglBlendFunc(blend_src, blend_dst);
		if(depth_test)
			qglEnable(GL_DEPTH_TEST);
		if(scissor_test)
			qglEnable(GL_SCISSOR_TEST);
		qglMatrixMode(matrix_mode);
		qglViewport(viewport[0], viewport[1], viewport[2],viewport[3]);
	}
	render_lock = bfalse;
}

karin_FrameBufferObject new_karin_FrameBufferObject(GLuint width, GLuint height, GLuint vw, GLuint vh)
{
	getFBOProc();

	karin_FrameBufferObject fbo;
	fbo.m_inited = (0);
	fbo.m_handle = (0);
	fbo.m_texture = (0);
	fbo.m_viewportWidth = (vw <= 0 ? M_MinSize : vw);
	fbo.m_viewportHeight = (vh <= 0 ? M_MinSize : vh);
	fbo.m_width = (width <= 0 ? M_MinSize : width);
	fbo.m_height = (height <= 0 ? M_MinSize : height);
	memset(fbo.m_renderBuffer, 0, Total_Render_Buffer * sizeof(GLuint));
	memset(fbo.m_buffer, 0, Total_Buffer * sizeof(GLuint));
	fbo.m_inited = init(&fbo, fbo.m_width, fbo.m_height);
	initBuffer(&fbo, fbo.m_viewportWidth, fbo.m_viewportHeight);

	fbo.bind = bind;
	fbo.unbind = unbind;
	fbo.isBind = isBind;
	fbo.blit = blit;
	fbo.resize = resize;
	fbo.delete__ = delete_karin_FrameBufferObject;

	return fbo;
}

void delete_karin_FrameBufferObject(karin_FrameBufferObject *fbo)
{
	if(!fbo)
		return;
	destory(fbo);
}

unsigned init(karin_FrameBufferObject *fbo, GLuint w, GLuint h)
{
	GLenum status;

	if(!fbo)
		return 0;
	if(fbo->m_inited)
		return 1;

	qglGenFramebuffers(1, &fbo->m_handle);
	qglBindFramebuffer(GL_FRAMEBUFFER, fbo->m_handle);

	attachFrameBuffer(fbo, fbo->m_width, fbo->m_height);

	status = getFBOStatus();

	qglBindFramebuffer(GL_FRAMEBUFFER, 0);

	return ((status == GL_FRAMEBUFFER_COMPLETE) ? 1 : 0);
}

void destory(karin_FrameBufferObject *fbo)
{
	if(!fbo)
		return;
	if(!fbo->m_inited)
		return;

	qglBindFramebuffer(GL_FRAMEBUFFER, 0);
	qglDeleteTextures(1, &fbo->m_texture);
	qglDeleteRenderbuffers(Total_Render_Buffer, fbo->m_renderBuffer);
	qglDeleteFramebuffers(1, &fbo->m_handle);
	qglDeleteBuffers(Total_Buffer, fbo->m_buffer);
	fbo->m_inited = 0;
}

unsigned getFBOStatus(void)
{
#define K_CASE(x) case x: printf("[karin_FrameBufferObject]: FBO Status->%s\n", #x); return x;
	GLenum status;

	status = qglCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch(status)
	{
		K_CASE(GL_FRAMEBUFFER_COMPLETE)
			K_CASE(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
			K_CASE(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
			K_CASE(GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS)
			K_CASE(GL_FRAMEBUFFER_UNSUPPORTED)
	}
#undef K_CASE
	return status;
}

unsigned getError(void)
{
	GLuint error;

	static const char *ErrorString[] = {
		"GL_INVALID_ENUM", 
		"GL_INVALID_VALUE", 
		"GL_INVALID_OPERATION", 
		"GL_OUT_OF_MEMORY", 
	};
	error = qglGetError();
	if(error == GL_NO_ERROR)
		return 0;
	fprintf(stderr, "[karin_FrameBufferObject]: GL Error->%s\n", ErrorString[error - 0x500]);
	return 1;
}

GLuint genTexture2D(GLuint w, GLuint h)
{
	GLuint tex;

	qglGenTextures(1, &tex);
	qglBindTexture(GL_TEXTURE_2D, tex);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	qglGenerateMipmap(GL_TEXTURE_2D);
	qglTexImage2D(GL_TEXTURE_2D, 0, M_TextureFormat, w, h, 0, M_TextureFormat, GL_UNSIGNED_BYTE, NULL);
	qglBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

void bind(karin_FrameBufferObject *fbo)
{
	if(!fbo)
		return;
	if(!fbo->m_inited)
		return;

	qglBindFramebuffer(GL_FRAMEBUFFER, fbo->m_handle);
}

void unbind(karin_FrameBufferObject *fbo)
{
	if(!fbo)
		return;
	if(!fbo->m_inited)
		return;

	qglBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void blit(karin_FrameBufferObject *fbo)
{
	if(!fbo)
		return;
	if(!fbo->m_inited)
		return;

	/*
		 qglBindTexture(GL_TEXTURE_2D, m_texture);
		 qglCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, m_width, m_height);
		 */
	unbind(fbo);
	karinBeginRender2D(fbo->m_viewportWidth, fbo->m_viewportHeight);
	{
		//qglClearColor(0.0, 0.0, 0.0, 1.0);
		//qglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		qglEnableClientState(GL_VERTEX_ARRAY);
		qglEnableClientState(GL_TEXTURE_COORD_ARRAY);
		qglBindTexture(GL_TEXTURE_2D, fbo->m_texture);
		qglBindBuffer(GL_ARRAY_BUFFER, fbo->m_buffer[TexCoord_Buffer]);
		qglTexCoordPointer(2, GL_FLOAT, 0, NULL);
		qglBindBuffer(GL_ARRAY_BUFFER, fbo->m_buffer[Vertex_Buffer]);
		qglVertexPointer(2, GL_FLOAT, 0, NULL);
		qglDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		qglBindTexture(GL_TEXTURE_2D, 0);
		qglBindBuffer(GL_ARRAY_BUFFER, 0);

		//if(h_cUsingVKB.GetBool())
			karinRenderGLVKB();

		qglDisableClientState(GL_VERTEX_ARRAY);
		qglDisableClientState(GL_TEXTURE_COORD_ARRAY);
		qglDisable(GL_ALPHA_TEST);
		qglDisable(GL_BLEND);
		qglFlush();
	}
	karinEndRender2D();
}

void initBuffer(karin_FrameBufferObject *fbo, GLfloat w, GLfloat h)
{
	if(!fbo)
		return;
	if(!fbo->m_inited)
		return;

	const GLfloat vs[] = {
		0.0, 0.0,
		w, 0.0,
		w, h,
		0.0, h,
	};
	const GLfloat ts[] = {
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
		0.0, 0.0,
	};

	qglGenBuffers(Total_Buffer, fbo->m_buffer);

	qglBindBuffer(GL_ARRAY_BUFFER, fbo->m_buffer[Vertex_Buffer]);
	qglBufferData(GL_ARRAY_BUFFER, sizeof(vs), vs, GL_STATIC_DRAW);
	qglBindBuffer(GL_ARRAY_BUFFER, 0);

	qglBindBuffer(GL_ARRAY_BUFFER, fbo->m_buffer[TexCoord_Buffer]);
	qglBufferData(GL_ARRAY_BUFFER, sizeof(ts), ts, GL_STATIC_DRAW);
	qglBindBuffer(GL_ARRAY_BUFFER, 0);
}

void resize(karin_FrameBufferObject *fbo, GLuint w, GLuint h, GLuint vw, GLuint vh)
{
	if(!fbo)
		return;
	if(!fbo->m_inited)
		return;

	if((w != fbo->m_width || h != fbo->m_height) && w > M_MinSize && h > M_MinSize)
	{
		fbo->m_width = w;
		fbo->m_height = h;

		qglBindFramebuffer(GL_FRAMEBUFFER, fbo->m_handle);
		qglFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		qglDeleteTextures(1, &fbo->m_texture);
		fbo->m_texture = 0;

		qglFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_OES, GL_RENDERBUFFER, 0);
		qglDeleteRenderbuffers(Total_Render_Buffer, fbo->m_renderBuffer);
		memset(fbo->m_renderBuffer, 0, Total_Render_Buffer * sizeof(GLuint));
		memset(fbo->m_buffer, 0, Total_Buffer * sizeof(GLuint));

		attachFrameBuffer(fbo, fbo->m_width, fbo->m_height);

		qglBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	if((vw != fbo->m_viewportWidth || vh != fbo->m_viewportHeight) && vw > M_MinSize && vh > M_MinSize)
	{
		fbo->m_viewportWidth = vw;
		fbo->m_viewportHeight = vh;

		const GLfloat vs[] = {
			0.0, 0.0,
			fbo->m_viewportWidth, 0.0,
			fbo->m_viewportWidth, fbo->m_viewportHeight,
			0.0, fbo->m_viewportHeight,
		};

		qglBindBuffer(GL_ARRAY_BUFFER, fbo->m_buffer[Vertex_Buffer]);
		qglBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vs), vs);
		qglBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void attachFrameBuffer(karin_FrameBufferObject *fbo, GLuint w, GLuint h)
{
	GLenum status;
	if(!fbo)
		return;

	fbo->m_texture = genTexture2D(w, h);
	qglFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->m_texture, 0);

	qglGenRenderbuffers(Total_Render_Buffer, fbo->m_renderBuffer);

	qglBindRenderbuffer(GL_RENDERBUFFER, fbo->m_renderBuffer[Depth_Render_Buffer]);
	qglRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, w, h);
	printRenderBufferParams();
	qglBindRenderbuffer(GL_RENDERBUFFER, 0);
	qglFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo->m_renderBuffer[Depth_Render_Buffer]);
	qglFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo->m_renderBuffer[Depth_Render_Buffer]);

	status = getFBOStatus();
}

unsigned isBind(const karin_FrameBufferObject *fbo)
{
	GLint cur_fbo;

	if(!fbo)
		return 0;
	if(!fbo->m_inited)
		return 0;

	qglGetIntegerv(GL_FRAMEBUFFER_BINDING, &cur_fbo);

	return cur_fbo == fbo->m_handle ? 1 : 0;
}

void printRenderBufferParams()
{
	GLint _v;
#define _GETP(x) qglGetRenderbufferParameteriv(GL_RENDERBUFFER, x, &_v); \
	printf(#x" -> %d 0x%x\n", _v, _v);
	_GETP(GL_RENDERBUFFER_WIDTH)
		_GETP(GL_RENDERBUFFER_HEIGHT)
		_GETP(GL_RENDERBUFFER_INTERNAL_FORMAT)
		_GETP(GL_RENDERBUFFER_RED_SIZE)
		_GETP(GL_RENDERBUFFER_GREEN_SIZE)
		_GETP(GL_RENDERBUFFER_BLUE_SIZE)
		_GETP(GL_RENDERBUFFER_ALPHA_SIZE)
		_GETP(GL_RENDERBUFFER_DEPTH_SIZE)
		_GETP(GL_RENDERBUFFER_STENCIL_SIZE)
#undef _GETP
}

void getFBOProc(void)
{
	if(fboProcInited)
		return;

	qglIsRenderbuffer = (PFNGLISRENDERBUFFEROESPROC)eglGetProcAddress("glIsRenderbuffer");
	qglBindRenderbuffer = (PFNGLBINDRENDERBUFFEROESPROC)eglGetProcAddress("glBindRenderbuffer");
	qglDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSOESPROC)eglGetProcAddress("glDeleteRenderbuffers");
	qglGenRenderbuffers = (PFNGLGENRENDERBUFFERSOESPROC)eglGetProcAddress("glGenRenderbuffers");
	qglRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEOESPROC)eglGetProcAddress("glRenderbufferStorage");
	qglGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVOESPROC)eglGetProcAddress("glGetRenderbufferParameteriv");
	qglIsFramebuffer = (PFNGLISFRAMEBUFFEROESPROC)eglGetProcAddress("glIsFramebuffer");
	qglBindFramebuffer = (PFNGLBINDFRAMEBUFFEROESPROC)eglGetProcAddress("glBindFramebuffer");
	qglDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSOESPROC)eglGetProcAddress("glDeleteFramebuffers");
	qglGenFramebuffers = (PFNGLGENFRAMEBUFFERSOESPROC)eglGetProcAddress("glGenFramebuffers");
	qglCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSOESPROC)eglGetProcAddress("glCheckFramebufferStatus");
	qglFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFEROESPROC)eglGetProcAddress("glFramebufferRenderbuffer");
	qglFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DOESPROC)eglGetProcAddress("glFramebufferTexture2D");
	qglGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVOESPROC)eglGetProcAddress("glGetFramebufferAttachmentParameteriv");
	qglGenerateMipmap = (PFNGLGENERATEMIPMAPOESPROC)eglGetProcAddress("glGenerateMipmap");

	fboProcInited = 1;
}
