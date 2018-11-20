/*
** EGL with OpenGL ES1.1 for wayland EGL
*/

#ifdef _HARMATTAN

#include <termios.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <dlfcn.h>
#ifdef Joystick
#include <fcntl.h>
#endif
#include "../ref_gl/gl_local.h"

#include "../client/keys.h"

#include "../linux/rw_linux.h"
#include "../linux/glw_linux.h"
#ifdef _HARMATTAN
#include "gl_vkb.h"
#include "fbo.h"
#endif
#ifdef _HARMATTAN_MULTITOUCH
//#include "m_xi2.h"
#endif

#include <wayland-egl.h>
#include <wayland-client.h>

#ifdef Joystick
# if defined (__linux__)
#include <linux/joystick.h>
# elif defined (__FreeBSD__)
#include <sys/joystick.h>
# endif
#include <glob.h>
#endif
#include <GLES/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglmesaext.h>
#include <sys/mman.h>

glwstate_t glw_state;

struct wl_display *dpy = NULL;
struct wl_registry *registry = NULL;
struct wl_compositor *compositor = NULL;
struct wl_shell *shell = NULL;
struct wl_shm *shm = NULL;
struct wl_seat *seat = NULL;
struct wl_pointer *pointer = NULL;
struct wl_keyboard *keyboard = NULL;
struct wl_touch *touch = NULL;
struct wl_surface *surface = NULL;
struct wl_shell_surface *shell_surface = NULL;
struct wl_egl_window *win = NULL;
struct wl_buffer *buffer = NULL;
void *data = NULL;

static karin_FrameBufferObject fbo_object;
karin_FrameBufferObject *fbo = &fbo_object;
#define VID_HEIGHT fbo->m_viewportWidth
#define VID_WIDTH fbo->m_viewportHeight
#define K_UNUSED(x) (void)(x)

#define EGLDLSYM(name) _##name = eglGetProcAddress(#name)

#ifdef Joystick
static int joy_fd;
#endif


/*****************************************************************************/
/* MOUSE                                                                     */
/*****************************************************************************/

// this is inside the renderer shared lib, so these are called from vid_so

int mx, my, mouse_buttonstate;
int win_x, win_y;

static cvar_t	*in_dgamouse;

static cvar_t	*r_fakeFullscreen;


static qboolean mouse_active = false;
static qboolean dgamouse = false;

/* stencilbuffer shadows */
qboolean have_stencil = false;

//static Time myxtime;

/* from ioquake3 on maemo5 OpenGL ES1.1 */
EGLContext eglContext = NULL;
EGLDisplay eglDisplay = NULL;
EGLSurface eglSurface = NULL;

#ifdef _HARMATTAN

#include "win_general.c"

typedef unsigned (* MultiMouseEvent_Function)(int button, unsigned pressed, int x, int y);
typedef unsigned (* MultiMotionEvent_Function)(int button, unsigned pressed, int x, int y, int dx, int dy);

#define KARIN_MAX_TOUCH_COUNT 10
#define KARIN_TOUCH_BUTTON 1
#define KARIN_TRUE 1
#define KARIN_FALSE 0

typedef enum _karin_TouchState_e
{
	Touch_None = 0,
	Touch_Down,
	Touch_Motion,
	Touch_Up,
} karin_TouchState_e;

typedef struct _karin_TouchEvent
{
	unsigned mask; // 0 - invalid, 1 - down, 2 - motion
	uint32_t id;
	int last_x;
	int last_y;
	int x;
	int y;
	uint32_t time;
} karin_TouchEvent;

static karin_TouchEvent touch_events[KARIN_MAX_TOUCH_COUNT];
static int touch_event_depth = 0;

static void karinCoord(int ox, int oy, int w, int h, int *rx, int *ry)
{
	if(!rx || !ry)
		return;

	K_UNUSED(w);
	K_UNUSED(h);

	*rx = oy;
	*ry = ox;
}

static karin_TouchEvent * karinFind(uint32_t id)
{
	int i;
	karin_TouchEvent *e;

	for(i = 0; i < KARIN_MAX_TOUCH_COUNT; i++)
	{
		e = touch_events + i;
		if(e->mask && e->id == id)
		{
			return e;
		}
	}
	return NULL;
}

static karin_TouchEvent * karinUpdate(unsigned mask, uint32_t id, wl_fixed_t x, wl_fixed_t y, uint32_t time)
{
	int i;
	karin_TouchEvent *e;

	if(mask == Touch_None)
		return NULL;

	// update
	e = karinFind(id);
	if(e)
	{
		e->mask = mask;
		e->last_x = e->x;
		e->last_y = e->y;
		e->x = wl_fixed_to_int(x);
		e->y = wl_fixed_to_int(y);
		e->time = time;
		return e;
	}

	// add
	if(mask != Touch_Down)
		return NULL;

	for(i = 0; i < KARIN_MAX_TOUCH_COUNT; i++)
	{
		e = touch_events + i;
		if(e->mask == Touch_None)
		{
			e->mask = mask;
			e->id = id;
			e->x = wl_fixed_to_int(x);
			e->y = wl_fixed_to_int(y);
			e->last_x = e->x;
			e->last_y = e->y;
			e->time = time;
			return e;
		}
	}
	return NULL;
}

static int karinFlush(void)
{
	int i;
	int c;
	karin_TouchEvent *e;

	c = 0;
	for(i = 0; i < KARIN_MAX_TOUCH_COUNT; i++)
	{
		e = touch_events + i;
		if(e->mask == Touch_Up)
		{
			memset(e, 0, sizeof(karin_TouchEvent));
		}
		if(e->mask != Touch_None)
			c++;
	}
	touch_event_depth = c;
	return c;
}

static void GLimp_HandleError(void)
{
	static char *GLimp_StringErrors[] = {
		"EGL_SUCCESS",
		"EGL_NOT_INITIALIZED",
		"EGL_BAD_ACCESS",
		"EGL_BAD_ALLOC",
		"EGL_BAD_ATTRIBUTE",
		"EGL_BAD_CONFIG",
		"EGL_BAD_CONTEXT",
		"EGL_BAD_CURRENT_SURFACE",
		"EGL_BAD_DISPLAY",
		"EGL_BAD_MATCH",
		"EGL_BAD_NATIVE_PIXMAP",
		"EGL_BAD_NATIVE_WINDOW",
		"EGL_BAD_PARAMETER",
		"EGL_BAD_SURFACE",
		"EGL_CONTEXT_LOST",
	};

	GLint err = eglGetError();

	fprintf(stderr, "%s: 0x%04x: %s\n", __func__, err,
			GLimp_StringErrors[err - EGL_SUCCESS]);
	assert(0);
}

unsigned karinXI2MouseEvent(int button, unsigned pressed, int x, int y)
{
	int rx;
	int ry;

	karinCoord(x, y, VID_WIDTH, VID_HEIGHT, &rx, &ry);

	return karinGLVKBMouseEvent(button, pressed, rx, ry, karinHandleVKBAction);
}

unsigned karinXI2MotionEvent(int button, unsigned pressed, int x, int y, int dx, int dy)
{
	int rx;
	int ry;

	karinCoord(x, y, VID_WIDTH, VID_HEIGHT, &rx, &ry);

	return karinGLVKBMouseMotionEvent(button, pressed, rx,  ry, dy, dx, karinHandleVKBAction);
}

// touch listener
static void karinTouchDown(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time, struct wl_surface *surface, int32_t id, wl_fixed_t x, wl_fixed_t y);
static void karinTouchUp(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time, int32_t id);
static void karinTouchMotion(void *data, struct wl_touch *wl_touch, uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y);
static void karinTouchFrame(void *data, struct wl_touch *wl_touch);
static void karinTouchCancel(void *data, struct wl_touch *wl_touch);

// global listener
static void karinRegistryListenerGlobal(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version);
static void karinRegistryListenerGlobalRemove(void *data, struct wl_registry *registry, uint32_t name);

// shell surface listener
static void karinShellSurfaceListenerPing(void *data, struct wl_shell_surface *shell_surface, uint32_t serial);
static void karinShellSurfaceListenerConfigure(void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height);
static void karinShellSurfaceListenerPopupDone(void *data, struct wl_shell_surface *shell_surface);

static const struct wl_registry_listener listener = {
	karinRegistryListenerGlobal, karinRegistryListenerGlobalRemove
};

static const struct wl_shell_surface_listener shell_surface_listener = {
	karinShellSurfaceListenerPing, karinShellSurfaceListenerConfigure, karinShellSurfaceListenerPopupDone
};

static const struct wl_touch_listener touch_listener = {
	karinTouchDown, karinTouchUp, karinTouchMotion, karinTouchFrame, karinTouchCancel
};

static void karinResize(int32_t width, int32_t height)
{
	printf("Wayland window resize: (%d, %d)\n", width, height);
	wl_egl_window_resize(win, width, height, 0, 0);
#if 0
	ri.Vid_NewWindow (width, height);
	vid.width = width;
	vid.height = height;
#endif

	fbo->resize(fbo, 0, 0, width, height);
	karinResizeGLVKB(height, width);
}

static void karinWayland(int *width, int *height)
{
	int w, h;
	
	w = width ? *width : HARMATTAN_WIDTH;
	h = height ? *height : HARMATTAN_HEIGHT;

	dpy = wl_display_connect(NULL);
	registry = wl_display_get_registry(dpy);
	wl_registry_add_listener(registry, &listener, &dpy);
	wl_display_dispatch(dpy);

	surface = wl_compositor_create_surface(compositor);

	shell_surface = wl_shell_get_shell_surface(shell, surface);
	wl_display_roundtrip(dpy);
	wl_display_get_fd(dpy);
	wl_shell_surface_add_listener(shell_surface, &shell_surface_listener, NULL);
	wl_shell_surface_set_title(shell_surface, "GL Quake II on Nemo Wayland");
	wl_shell_surface_set_toplevel(shell_surface);
	wl_shell_surface_set_maximized(shell_surface, NULL);
	//wl_shell_surface_set_fullscreen(shell_surface, WL_SHELL_SURFACE_FULLSCREEN_METHOD_FILL, 60, NULL);
	//wl_surface_set_buffer_transform(surface, WL_OUTPUT_TRANSFORM_90);
	//wl_surface_damage(surface, 0, 0, 480, 854);
	wl_surface_commit(surface);
	
	wl_display_flush(dpy);

	win = wl_egl_window_create(surface, w, h);

	/* for test
	*width = 640;
	*height = 480;
	*/
}

static void karinEGL(void)
{
#define MAX_NUM_CONFIGS 4
	int i;
	EGLint major, minor;
	EGLint config_count;
	EGLConfig configs[MAX_NUM_CONFIGS];
	int red_bits, blue_bits, green_bits, depth_bits, alpha_bits;
	int stencil_bits;
	int attrib[] = {
		EGL_NATIVE_VISUAL_TYPE, 0,

		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,

		EGL_DEPTH_SIZE, 8, // 24
		EGL_STENCIL_SIZE, 8,
		EGL_BUFFER_SIZE, 16,

		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,

		EGL_NONE
	};

	eglSurface = EGL_NO_SURFACE;
	eglDisplay = eglGetDisplay((NativeDisplayType) dpy);
	if (!eglInitialize(eglDisplay, &major, &minor))
		GLimp_HandleError();

	if (!eglGetConfigs(eglDisplay, configs, MAX_NUM_CONFIGS, &config_count))
		GLimp_HandleError();

	if (!eglChooseConfig (eglDisplay, attrib, configs, MAX_NUM_CONFIGS, &config_count))
		GLimp_HandleError();

	for (i = 0; i < config_count; i++) {
		if ((eglSurface = eglCreateWindowSurface(eglDisplay, configs[i], (NativeWindowType) win, NULL)) != EGL_NO_SURFACE)
			break;
	}
	if (eglSurface == EGL_NO_SURFACE)
		GLimp_HandleError();

	if ((eglContext = eglCreateContext(eglDisplay, configs[i], EGL_NO_CONTEXT, NULL)) == EGL_NO_CONTEXT)
		GLimp_HandleError();

	if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext))
		GLimp_HandleError();

	/* do some pantsness */
	if(i < MAX_NUM_CONFIGS)
	{
		eglGetConfigAttrib(eglDisplay, configs[i], EGL_RED_SIZE, &red_bits);
		eglGetConfigAttrib(eglDisplay, configs[i], EGL_BLUE_SIZE, &blue_bits);
		eglGetConfigAttrib(eglDisplay, configs[i], EGL_GREEN_SIZE, &green_bits);
		eglGetConfigAttrib(eglDisplay, configs[i], EGL_DEPTH_SIZE, &depth_bits);
		eglGetConfigAttrib(eglDisplay, configs[i], EGL_ALPHA_SIZE, &alpha_bits);

		ri.Con_Printf(PRINT_ALL, "I: got %d bits of red\n", red_bits);
		ri.Con_Printf(PRINT_ALL, "I: got %d bits of blue\n", blue_bits);
		ri.Con_Printf(PRINT_ALL, "I: got %d bits of green\n", green_bits);
		ri.Con_Printf(PRINT_ALL, "I: got %d bits of depth\n", depth_bits);
		ri.Con_Printf(PRINT_ALL, "I: got %d bits of alpha\n", alpha_bits);

		if (eglGetConfigAttrib(eglDisplay, configs[i], EGL_STENCIL_SIZE, &stencil_bits)) {
			ri.Con_Printf(PRINT_ALL, "I: got %d bits of stencil\n", stencil_bits);
			if (stencil_bits >= 1) {
				have_stencil = true;
			}
		}
		else
			have_stencil = true;
	}

	// let the sound and input subsystems know about the new window
}
static void karinTouchDown(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time, struct wl_surface *surface, int32_t id, wl_fixed_t x, wl_fixed_t y)
{
	karin_TouchEvent *e;

	K_UNUSED(data);

	e = karinUpdate(Touch_Down, id, x, y, time);

	if(e)
	{
		karinXI2MouseEvent(KARIN_TOUCH_BUTTON, KARIN_TRUE, e->x, e->y);
		//printf("D %d %d\n", e->x, e->y);
	}
}

static void karinTouchUp(void *data, struct wl_touch *wl_touch, uint32_t serial, uint32_t time, int32_t id)
{
	karin_TouchEvent *e;

	K_UNUSED(data);

	e = karinUpdate(Touch_Up, id, 0, 0, time);

	if(e)
	{
		karinXI2MouseEvent(KARIN_TOUCH_BUTTON, KARIN_FALSE, e->last_x, e->last_y);
		//printf("U %d %d\n", e->x, e->y);
	}

	karinFlush();
}

static void karinTouchMotion(void *data, struct wl_touch *wl_touch, uint32_t time, int32_t id, wl_fixed_t x, wl_fixed_t y)
{
	karin_TouchEvent *e;

	K_UNUSED(data);

	e = karinUpdate(Touch_Motion, id, x, y, time);

	if(e)
	{
		karinXI2MotionEvent(KARIN_TOUCH_BUTTON, KARIN_TRUE, e->x, e->y, e->x - e->last_x, e->y - e->last_y);
		//printf("M %d %d\n", e->x, e->y);
	}
}

static void karinTouchFrame(void *data, struct wl_touch *wt)
{
	K_UNUSED(data);
	K_UNUSED(wt);
}

static void karinTouchCancel(void *data, struct wl_touch *wt)
{
	K_UNUSED(data);
	K_UNUSED(wt);
}


void karinShellSurfaceListenerPing(void *data, struct wl_shell_surface *shell_surface, uint32_t serial)
{
	K_UNUSED(data);
	wl_shell_surface_pong(shell_surface, serial);
}

void	karinShellSurfaceListenerConfigure(void *data, struct wl_shell_surface *shell_surface, uint32_t edges, int32_t width, int32_t height)
{
	K_UNUSED(data);
	// intex aquafish 720x1280
	karinResize(width, height);
}

void karinShellSurfaceListenerPopupDone(void *data, struct wl_shell_surface *shell_surface)
{
	K_UNUSED(data);
	K_UNUSED(shell_surface);
}


void karinRegistryListenerGlobalRemove(void *data, struct wl_registry *reg, uint32_t name)
{
	K_UNUSED(data);
	K_UNUSED(reg);
	K_UNUSED(name);
} 

void karinRegistryListenerGlobal(void *data, struct wl_registry *reg, uint32_t id, const char *interface, uint32_t version)
{
	if(strcmp(interface, "wl_compositor") == 0)
	{
		compositor = wl_registry_bind(reg, id, &wl_compositor_interface, 1);	
	}
	else if(strcmp(interface, "wl_shell") == 0)
	{
		shell = wl_registry_bind(reg, id, &wl_shell_interface, 1);
	}
	else if(strcmp(interface, "wl_shm") == 0)
	{
		//shm = wl_registry_bind(reg, id, &wl_shm_interface, 1);
//wl_shm_add_listener(shm, &shm_listenter, NULL);
	}
	else if(strcmp(interface, "wl_seat") == 0)
	{
		seat = wl_registry_bind(reg, id, &wl_seat_interface, 1);
		//pointer = wl_seat_get_pointer(seat);
		//keyboard = wl_seat_get_keyboard(seat);
		touch = wl_seat_get_touch(seat);
		wl_touch_add_listener(touch, &touch_listener, NULL);
	}
}

#endif

/* ************************************ */


static void install_grabs(void)
{

	if (in_dgamouse->value) {
		ri.Cvar_Set( "in_dgamouse", "0" );
	} else {
	}

	mouse_active = true;

}

static void uninstall_grabs(void)
{
	if (!dpy || !win)
		return;

	if (dgamouse) {
		dgamouse = false;
	}

	mouse_active = false;
}

static void IN_DeactivateMouse( void ) 
{
	//if (!mouse_avail || !dpy || !win)
	//return;

	if (mouse_active) {
		uninstall_grabs();
		mouse_active = false;
	}
}

static void IN_ActivateMouse( void ) 
{
	//if (!mouse_avail || !dpy || !win)
	//return;

	if (!mouse_active) {
		mx = my = 0; // don't spazz
		install_grabs();
		mouse_active = true;
	}
}

void getMouse(int *x, int *y, int *state) {
	*x = mx;
	*y = my;
	*state = mouse_buttonstate;
}

void doneMouse() {
	mx = my = 0;
}

void RW_IN_PlatformInit()
{

	in_dgamouse = ri.Cvar_Get ("in_dgamouse", "0", CVAR_ARCHIVE);
}

void RW_IN_Activate(qboolean active)
{
	if (active)
		IN_ActivateMouse();
	else
		IN_DeactivateMouse ();
}

/*****************************************************************************/
/* KEYBOARD                                                                  */
/*****************************************************************************/

#if 0
static int XLateKey()
{
	int key;
	char buf[64];
	KeySym keysym;

	key = 0;

	XLookupString(ev, buf, sizeof buf, &keysym, 0);

	switch(keysym)
	{
		case XK_KP_Page_Up:	 key = K_KP_PGUP; break;
		case XK_Page_Up:	 key = K_PGUP; break;

		case XK_KP_Page_Down: key = K_KP_PGDN; break;
		case XK_Page_Down:	 key = K_PGDN; break;

		case XK_KP_Home: key = K_KP_HOME; break;
		case XK_Home:	 key = K_HOME; break;

		case XK_KP_End:  key = K_KP_END; break;
		case XK_End:	 key = K_END; break;

		case XK_KP_Left: key = K_KP_LEFTARROW; break;
		case XK_Left:	 key = K_LEFTARROW; break;

		case XK_KP_Right: key = K_KP_RIGHTARROW; break;
		case XK_Right:	key = K_RIGHTARROW;		break;

		case XK_KP_Down: key = K_KP_DOWNARROW; break;
		case XK_Down:	 key = K_DOWNARROW; break;

		case XK_KP_Up:   key = K_KP_UPARROW; break;
		case XK_Up:		 key = K_UPARROW;	 break;

		case XK_Escape: key = K_ESCAPE;		break;

		case XK_KP_Enter: key = K_KP_ENTER;	break;
		case XK_Return: key = K_ENTER;		 break;

		case XK_Tab:		key = K_TAB;			 break;

		case XK_F1:		 key = K_F1;				break;

		case XK_F2:		 key = K_F2;				break;

		case XK_F3:		 key = K_F3;				break;

		case XK_F4:		 key = K_F4;				break;

		case XK_F5:		 key = K_F5;				break;

		case XK_F6:		 key = K_F6;				break;

		case XK_F7:		 key = K_F7;				break;

		case XK_F8:		 key = K_F8;				break;

		case XK_F9:		 key = K_F9;				break;

		case XK_F10:		key = K_F10;			 break;

		case XK_F11:		key = K_F11;			 break;

		case XK_F12:		key = K_F12;			 break;

		case XK_BackSpace: key = K_BACKSPACE; break;

		case XK_KP_Delete: key = K_KP_DEL; break;
		case XK_Delete: key = K_DEL; break;

		case XK_Pause:	key = K_PAUSE;		 break;

		case XK_Shift_L:
		case XK_Shift_R:	key = K_SHIFT;		break;

		case XK_Execute: 
		case XK_Control_L: 
		case XK_Control_R:	key = K_CTRL;		 break;

		case XK_Alt_L:	
		case XK_Meta_L: 
		case XK_Alt_R:	
		case XK_Meta_R: key = K_ALT;			break;

		case XK_KP_Begin: key = K_KP_5;	break;

		case XK_Insert:key = K_INS; break;
		case XK_KP_Insert: key = K_KP_INS; break;

		case XK_KP_Multiply: key = '*'; break;
		case XK_KP_Add:  key = K_KP_PLUS; break;
		case XK_KP_Subtract: key = K_KP_MINUS; break;
		case XK_KP_Divide: key = K_KP_SLASH; break;

#if 0
		case 0x021: key = '1';break;/* [!] */
		case 0x040: key = '2';break;/* [@] */
		case 0x023: key = '3';break;/* [#] */
		case 0x024: key = '4';break;/* [$] */
		case 0x025: key = '5';break;/* [%] */
		case 0x05e: key = '6';break;/* [^] */
		case 0x026: key = '7';break;/* [&] */
		case 0x02a: key = '8';break;/* [*] */
		case 0x028: key = '9';;break;/* [(] */
		case 0x029: key = '0';break;/* [)] */
		case 0x05f: key = '-';break;/* [_] */
		case 0x02b: key = '=';break;/* [+] */
		case 0x07c: key = '\'';break;/* [|] */
		case 0x07d: key = '[';break;/* [}] */
	case 0x07b: key = ']';break;/* [{] */
	case 0x022: key = '\'';break;/* ["] */
	case 0x03a: key = ';';break;/* [:] */
	case 0x03f: key = '/';break;/* [?] */
	case 0x03e: key = '.';break;/* [>] */
	case 0x03c: key = ',';break;/* [<] */
#endif

	default:
							key = *(unsigned char*)buf;
							if (key >= 'A' && key <= 'Z')
								key = key - 'A' + 'a';
							if (key >= 1 && key <= 26) /* ctrl+alpha */
								key = key + 'a' - 1;
							break;
} 

return key;
}
#endif


/* Check to see if this is a repeated key.
	 (idea shamelessly lifted from SDL who...)
	 (idea shamelessly lifted from GII -- thanks guys! :)
	 This has bugs if two keys are being pressed simultaneously and the
	 events start getting interleaved.
	 */

static void HandleEvents(void)
{
	if(!dpy)
		return;
	wl_display_dispatch_pending(dpy);
	
#if 0
	XEvent event;
	qboolean dowarp = false;
#ifndef _HARMATTAN_MULTITOUCH
	int b;
	int mwx = vid.width/2;
	int mwy = vid.height/2;
#endif
#ifndef _HARMATTAN_MULTITOUCH
	static int delta_x = 0;
	static int delta_y = 0;
	static int last_x = 0;
	static int last_y = 0;
	static unsigned mouse_pressed = 0;
#endif
	in_state_t *in_state = getState();
	if (!dpy)
		return;

#ifdef _HARMATTAN_MULTITOUCH
	while (X11_Pending(dpy)) 
#else
		while (XPending(dpy)) 
#endif
		{
			//ri.Con_Printf(PRINT_ALL,"Bar");
			XNextEvent(dpy, &event);
#ifndef _HARMATTAN
			mx = my = 0;
#endif
			switch(event.type) {
				case KeyPress:
					myxtime = event.xkey.time;
					if (in_state && in_state->Key_Event_fp)
						in_state->Key_Event_fp (XLateKey(&event.xkey), true);
					break;
				case KeyRelease:
					if (! X11_KeyRepeat(dpy, &event)) {
						if (in_state && in_state->Key_Event_fp)
							in_state->Key_Event_fp (XLateKey(&event.xkey), false);
					}
					break;
#ifndef _HARMATTAN_MULTITOUCH
				case MotionNotify:
#ifndef _HARMATTAN
					if (mouse_active) {
						if (dgamouse) {
							mx += (event.xmotion.x + win_x) * 2;
							my += (event.xmotion.y + win_y) * 2;
						}
						else 
						{
							mx -= ((int)event.xmotion.x - mwx) * 2;
							my -= ((int)event.xmotion.y - mwy) * 2;
							mwx = event.xmotion.x;
							mwy = event.xmotion.y;

							if (mx || my)
								dowarp = true;
						}
					}
#else
					{
						if(mouse_pressed)
						{
							myxtime = event.xmotion.time;
							delta_x = event.xmotion.x - last_x;
							delta_y = (vid.height - event.xmotion.y) - last_y;
							last_x = event.xmotion.x;
							last_y = vid.height - event.xmotion.y;
							karinVKBMouseMotionEvent(event.xmotion.state, mouse_pressed, last_x, last_y, delta_x, delta_y, karinHandleVKBAction);
						}
					}
#endif
					break;


				case ButtonPress:
					myxtime = event.xbutton.time;

#ifndef _HARMATTAN
					b=-1;
					if (event.xbutton.button == 1)
						b = 0;
					else if (event.xbutton.button == 2)
						b = 2;
					else if (event.xbutton.button == 3)
						b = 1;
					else if (event.xbutton.button == 4)
						in_state->Key_Event_fp (K_MWHEELUP, 1);
					else if (event.xbutton.button == 5)
						in_state->Key_Event_fp (K_MWHEELDOWN, 1);
					if (b>=0 && in_state && in_state->Key_Event_fp)
						in_state->Key_Event_fp (K_MOUSE1 + b, true);
					if (b>=0)
						mouse_buttonstate |= 1<<b;
#else
					mouse_pressed = 1;
					last_x = event.xbutton.x;
					lasy_y = vid.height - event.xbutton.y;
					delta_x = 0;
					delta_y = 0;
					karinVKBMouseEvent(event.xbutton.button, mouse_pressed, event.xbutton.x, vid.height - event.xbutton.y, karinHandleVKBAction);
#endif
					break;

				case ButtonRelease:
#ifndef _HARMATTAN
					b=-1;
					if (event.xbutton.button == 1)
						b = 0;
					else if (event.xbutton.button == 2)
						b = 2;
					else if (event.xbutton.button == 3)
						b = 1;
					else if (event.xbutton.button == 4)
						in_state->Key_Event_fp (K_MWHEELUP, 0);
					else if (event.xbutton.button == 5)
						in_state->Key_Event_fp (K_MWHEELDOWN, 0);
					if (b>=0 && in_state && in_state->Key_Event_fp)
						in_state->Key_Event_fp (K_MOUSE1 + b, false);
					if (b>=0)
						mouse_buttonstate &= ~(1<<b);
#else
					mouse_pressed = 0;
					last_x = 0;
					lasy_y = 0;
					delta_x = 0;
					delta_y = 0;
					karinVKBMouseEvent(event.xbutton.button, mouse_pressed, event.xbutton.x, vid.height - event.xbutton.y, karinHandleVKBAction);
#endif
					break;
#endif

				case CreateNotify :
					win_x = event.xcreatewindow.x;
					win_y = event.xcreatewindow.y;
#ifndef _HARMATTAN
#ifndef _HARMATTAN_MULTITOUCH
					mouse_pressed = 0;
#endif
#endif
					break;

				case ConfigureNotify :
					win_x = event.xconfigure.x;
					win_y = event.xconfigure.y;
#ifndef _HARMATTAN
#ifndef _HARMATTAN_MULTITOUCH
					mouse_pressed = 0;
#endif
#endif
					break;

				case ClientMessage:
					if (event.xclient.data.l[0] == wmDeleteWindow)
						ri.Cmd_ExecuteText(EXEC_NOW, "quit");
					break;
#ifdef _HARMATTAN_MULTITOUCH
				case GenericEvent:
					if(karinXI2Event(&event))
						myxtime = myxitime;
					/*
					else
						printf("Unhandle XGenericEvent\n");
						*/
					break;
#endif
			}
		}

	if (dowarp) {
		/* move the mouse to the window center again */
		XWarpPointer(dpy,None,win,0,0,0,0, vid.width/2,vid.height/2);
	}
#endif
}

Key_Event_fp_t Key_Event_fp;

void KBD_Init(Key_Event_fp_t fp)
{
	Key_Event_fp = fp;
}

void KBD_Update(void)
{
	// get events from x server
	HandleEvents();
}

void KBD_Close(void)
{
}

/*****************************************************************************/

char *RW_Sys_GetClipboardData()
{
	return NULL; // in heap
}

/*****************************************************************************/

qboolean GLimp_InitGL (void);

static void signal_handler(int sig)
{
	printf("Received signal %d, exiting...\n", sig);
	GLimp_Shutdown();
	_exit(0);
}

static void InitSig(void)
{
	signal(SIGHUP, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGTRAP, signal_handler);
	signal(SIGIOT, signal_handler);
	signal(SIGBUS, signal_handler);
	signal(SIGFPE, signal_handler);
	signal(SIGSEGV, signal_handler);
	signal(SIGTERM, signal_handler);
}

/*
 ** GLimp_SetMode
 */
int GLimp_SetMode( int *pwidth, int *pheight, int mode, qboolean fullscreen )
{
	int width, height;

	r_fakeFullscreen = ri.Cvar_Get( "r_fakeFullscreen", "0", CVAR_ARCHIVE);

	ri.Con_Printf( PRINT_ALL, "Initializing OpenGL display\n");

	if (fullscreen)
		ri.Con_Printf (PRINT_ALL, "...setting fullscreen mode %d:", mode );
	else
		ri.Con_Printf (PRINT_ALL, "...setting mode %d:", mode );

	if ( !ri.Vid_GetModeInfo( &width, &height, mode ) )
	{
		ri.Con_Printf( PRINT_ALL, " invalid mode\n" );
		return rserr_invalid_mode;
	}

	// destroy the existing window
	GLimp_Shutdown ();

#if 0 // this breaks getenv()? - sbf
	// Mesa VooDoo hacks
	if (fullscreen)
		putenv("MESA_GLX_FX=fullscreen");
	else
		putenv("MESA_GLX_FX=window");
#endif

	// Get video mode list
	gl_state.hwgamma = false;

	/* window attributes */
	karinWayland(&width, &height);

	/* EGL */
	karinEGL();

	harm_swipeSens = ri.Cvar_Get( HARM_SWIPE_SENS, HARM_SWIPE_SENS_DEFAULT_P, CVAR_USERINFO | CVAR_ARCHIVE);
	fbo_object = new_karin_FrameBufferObject(width, height, width, height);
	fbo->bind(fbo);

	*pwidth = width;
	*pheight = height;

	// let the sound and input subsystems know about the new window
	ri.Vid_NewWindow (width, height);

	return rserr_ok;
}

/*
 ** GLimp_Shutdown
 **
 ** This routine does all OS specific shutdown procedures for the OpenGL
 ** subsystem.  Under OpenGL this means NULLing out the current DC and
 ** HGLRC, deleting the rendering context, and releasing the DC acquired
 ** for the window.  The state structure is also nulled out.
 **
 */
void GLimp_Shutdown( void )
{
	uninstall_grabs();
	mouse_active = false;
	dgamouse = false;

	if (dpy) {
		fbo->delete__(fbo);
		eglDestroyContext(eglDisplay, eglContext);
		eglDestroySurface(eglDisplay, eglSurface);
		eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglTerminate(eglDisplay);

		if (win)
			wl_egl_window_destroy(win);

		wl_shell_surface_destroy(shell_surface);
		wl_surface_destroy(surface);
		//wl_seat_release(seat);
		// wl_keyboard_release(keyboard);
		// wl_pointer_release(pointer);
		wl_touch_release(touch);

		wl_display_disconnect(dpy);
	}

	seat = NULL;
	surface = NULL;
	keyboard = NULL;
	pointer = NULL;
	touch = NULL;

	shm = NULL;
	registry = NULL;
	compositor = NULL;
	shell = NULL;
	shell_surface = NULL;

	eglContext = NULL;
	eglDisplay = NULL;
	eglSurface = NULL;
	win = NULL;
	dpy = NULL;
}

/*
 ** GLimp_Init
 **
 ** This routine is responsible for initializing the OS specific portions
 ** of OpenGL.  
 */
int GLimp_Init( void *hinstance, void *wndproc )
{
	InitSig();

	karinPrintDev();

	if ( glw_state.OpenGLLib) {
		return true;
	}

	return false;
}

/*
 ** GLimp_BeginFrame
 */
void GLimp_BeginFrame( float camera_seperation )
{
}

/*
 ** GLimp_EndFrame
 ** 
 ** Responsible for doing a swapbuffers and possibly for other stuff
 ** as yet to be determined.  Probably better not to make this a GLimp
 ** function and instead do a call to GLimp_SwapBuffers.
 */
void GLimp_EndFrame (void)
{
	qglFlush();
	fbo->blit(fbo);
	eglSwapBuffers(eglDisplay, eglSurface);
	fbo->bind(fbo);
}

/*
 ** UpdateHardwareGamma
 **
 ** We are using gamma relative to the desktop, so that we can share it
 ** with software renderer and don't require to change desktop gamma
 ** to match hardware gamma image brightness. It seems that Quake 3 is
 ** using the opposite approach, but it has no software renderer after
 ** all.
 */
void UpdateHardwareGamma()
{
}

/*
 ** GLimp_AppActivate
 */
void GLimp_AppActivate( qboolean active )
{
}

void Fake_glColorTableEXT( GLenum target, GLenum internalformat,
		GLsizei width, GLenum format, GLenum type,
		const GLvoid *table )
{
	byte temptable[256][4];
	byte *intbl;
	int i;

	for (intbl = (byte *)table, i = 0; i < 256; i++) {
		temptable[i][2] = *intbl++;
		temptable[i][1] = *intbl++;
		temptable[i][0] = *intbl++;
		temptable[i][3] = 255;
	}
	if(qgl3DfxSetPaletteEXT)
		qgl3DfxSetPaletteEXT((GLuint *)temptable);
}


#ifdef Joystick
qboolean OpenJoystick(cvar_t *joy_dev) {
	int i, err;
	glob_t pglob;
	struct js_event e;

	err = glob(joy_dev->string, 0, NULL, &pglob);

	if (err) {
		switch (err) {
			case GLOB_NOSPACE:
				ri.Con_Printf(PRINT_ALL, "Error, out of memory while looking for joysticks\n");
				break;
			case GLOB_NOMATCH:
				ri.Con_Printf(PRINT_ALL, "No joysticks found\n");
				break;
			default:
				ri.Con_Printf(PRINT_ALL, "Error #%d while looking for joysticks\n",err);
		}
		return false;
	}  

	for (i=0;i<pglob.gl_pathc;i++) {
		ri.Con_Printf(PRINT_ALL, "Trying joystick dev %s\n", pglob.gl_pathv[i]);
		joy_fd = open (pglob.gl_pathv[i], O_RDONLY | O_NONBLOCK);
		if (joy_fd == -1) {
			ri.Con_Printf(PRINT_ALL, "Error opening joystick dev %s\n", 
					pglob.gl_pathv[i]);
			return false;
		}
		else {
			while (read(joy_fd, &e, sizeof(struct js_event))!=-1 &&
					(e.type & JS_EVENT_INIT))
				ri.Con_Printf(PRINT_ALL, "Read init event\n");
			ri.Con_Printf(PRINT_ALL, "Using joystick dev %s\n", pglob.gl_pathv[i]);
			return true;
		}
	}
	globfree(&pglob);
	return false;
}

void PlatformJoyCommands(int *axis_vals, int *axis_map) {
	struct js_event e;
	int key_index;
	in_state_t *in_state = getState();

	while (read(joy_fd, &e, sizeof(struct js_event))!=-1) {
		if (JS_EVENT_BUTTON & e.type) {
			key_index = (e.number < 4) ? K_JOY1 : K_AUX1;
			if (e.value) {
				in_state->Key_Event_fp (key_index + e.number, true);
			}
			else {
				in_state->Key_Event_fp (key_index + e.number, false);
			}
		}
		else if (JS_EVENT_AXIS & e.type) {
			axis_vals[axis_map[e.number]] = e.value;
		}
	}
}

qboolean CloseJoystick(void) {
	if (close(joy_fd))
		ri.Con_Printf(PRINT_ALL, "Error, Problem closing joystick.");
	return true;
}
#endif

#endif

