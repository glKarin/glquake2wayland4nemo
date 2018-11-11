/*
** EGL with OpenGL ES1.1
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
#endif
#ifdef _HARMATTAN_MULTITOUCH
#include "m_xi2.h"
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>

#ifdef Joystick
# if defined (__linux__)
#include <linux/joystick.h>
# elif defined (__FreeBSD__)
#include <sys/joystick.h>
# endif
#include <glob.h>
#endif
#include <GLES/egl.h>

glwstate_t glw_state;

Display *dpy = NULL;
Window win;
static int scrnum;
static Atom wmDeleteWindow;

#ifdef Joystick
static int joy_fd;
#endif

#define KEY_MASK (KeyPressMask | KeyReleaseMask)
 #define MOUSE_MASK (ButtonPressMask | ButtonReleaseMask | \
		    PointerMotionMask | ButtonMotionMask )
#define X_MASK (KEY_MASK | MOUSE_MASK | VisibilityChangeMask | StructureNotifyMask )


/*****************************************************************************/
/* MOUSE                                                                     */
/*****************************************************************************/

// this is inside the renderer shared lib, so these are called from vid_so

int mx, my, mouse_buttonstate;
int win_x, win_y;

static cvar_t	*in_dgamouse;

static cvar_t	*r_fakeFullscreen;

static qboolean vidmode_active = true;

static qboolean mouse_active = false;
static qboolean dgamouse = false;
static qboolean vidmode_ext = false;

/* stencilbuffer shadows */
qboolean have_stencil = false;

static Time myxtime;

/* from ioquake3 on maemo5 OpenGL ES1.1 */
#define _NET_WM_STATE_REMOVE        0	/* remove/unset property */
#define _NET_WM_STATE_ADD           1	/* add/set property */
#define _NET_WM_STATE_TOGGLE        2	/* toggle property  */

EGLContext eglContext = NULL;
EGLDisplay eglDisplay = NULL;
EGLSurface eglSurface = NULL;

#ifdef _HARMATTAN

#include "win_general.c"

Bool karinXI2MouseEvent(int button, Bool pressed, int x, int y)
{
	return karinGLVKBMouseEvent(button, pressed, x, vid.height - y, karinHandleVKBAction);
}

Bool karinXI2MotionEvent(int button, Bool pressed, int x, int y, int dx, int dy)
{
	return karinGLVKBMouseMotionEvent(button, pressed, x, vid.height - y, dx, -dy, karinHandleVKBAction);
}
#endif

static void hildon_set_non_compositing(void)
{
	Atom atom;
	int one = 1;

	atom = XInternAtom(dpy, "_HILDON_NON_COMPOSITED_WINDOW", False);
	XChangeProperty(dpy, win, atom, XA_INTEGER, 32, PropModeReplace, (unsigned char *)&one, 1);
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
			GLimp_StringErrors[err]);
	assert(0);
}

static void GLimp_DisableComposition(void)
{
	XClientMessageEvent xclient;
	Atom atom;
	int one = 1;

	atom = XInternAtom(dpy, "_HILDON_NON_COMPOSITED_WINDOW", False);
	XChangeProperty(dpy, win, atom, XA_INTEGER, 32, PropModeReplace,
			(unsigned char *)&one, 1);

	xclient.type = ClientMessage;
	xclient.window = win;	//GDK_WINDOW_XID (window);
	xclient.message_type = XInternAtom(dpy, "_NET_WM_STATE", False);
	xclient.format = 32;
	xclient.data.l[0] =
#ifdef _HARMATTAN
		True || 
#endif
		r_fakeFullscreen->value ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
	//gdk_x11_atom_to_xatom_for_display (display, state1);
	//gdk_x11_atom_to_xatom_for_display (display, state2);
	xclient.data.l[1] = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
	xclient.data.l[2] = 0;
	xclient.data.l[3] = 0;
	xclient.data.l[4] = 0;
	XSendEvent(dpy, DefaultRootWindow(dpy), False,
			SubstructureRedirectMask | SubstructureNotifyMask,
			(XEvent *) & xclient);
}
/* ************************************ */


static Cursor CreateNullCursor(Display *display, Window root)
{
	Pixmap cursormask; 
	XGCValues xgc;
	GC gc;
	XColor dummycolour;
	Cursor cursor;

	cursormask = XCreatePixmap(display, root, 1, 1, 1/*depth*/);
	xgc.function = GXclear;
	gc =  XCreateGC(display, cursormask, GCFunction, &xgc);
	XFillRectangle(display, cursormask, gc, 0, 0, 1, 1);
	dummycolour.pixel = 0;
	dummycolour.red = 0;
	dummycolour.flags = 04;
	cursor = XCreatePixmapCursor(display, cursormask, cursormask,
			&dummycolour,&dummycolour, 0,0);
	XFreePixmap(display,cursormask);
	XFreeGC(display,gc);
	return cursor;
}

static void install_grabs(void)
{

	XSync(dpy, False);
	hildon_set_non_compositing();
	// inviso cursor
	XDefineCursor(dpy, win, CreateNullCursor(dpy, win));

	/*
		 XGrabPointer(dpy, win,
		 True,
		 0,
		 GrabModeAsync, GrabModeAsync,
		 win,
		 None,
		 CurrentTime);
		 */

	if (in_dgamouse->value) {
		ri.Cvar_Set( "in_dgamouse", "0" );
	} else {
		XWarpPointer(dpy, None, win,
				0, 0, 0, 0,
				vid.width / 2, vid.height / 2);
	}

	XGrabKeyboard(dpy, win,
			False,
			GrabModeAsync, GrabModeAsync,
			CurrentTime);

	mouse_active = true;

	//	XSync(dpy, True);
}

static void uninstall_grabs(void)
{
	if (!dpy || !win)
		return;

	if (dgamouse) {
		dgamouse = false;
#ifndef _HARMATTAN
		XF86DGADirectVideo(dpy, DefaultScreen(dpy), 0);
#endif
	}

#ifndef _HARMATTAN
	XUngrabPointer(dpy, CurrentTime);
#endif
	XUngrabKeyboard(dpy, CurrentTime);

	// inviso cursor
	XUndefineCursor(dpy, win);

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
	if (active || vidmode_active)
		IN_ActivateMouse();
	else
		IN_DeactivateMouse ();
}

/*****************************************************************************/
/* KEYBOARD                                                                  */
/*****************************************************************************/

static int XLateKey(XKeyEvent *ev)
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


/* Check to see if this is a repeated key.
	 (idea shamelessly lifted from SDL who...)
	 (idea shamelessly lifted from GII -- thanks guys! :)
	 This has bugs if two keys are being pressed simultaneously and the
	 events start getting interleaved.
	 */
int X11_KeyRepeat(Display *display, XEvent *event)
{
	XEvent peekevent;
	int repeated;

	repeated = 0;
	if ( XPending(display) ) {
		XPeekEvent(display, &peekevent);
		if ( (peekevent.type == KeyPress) &&
				(peekevent.xkey.keycode == event->xkey.keycode) &&
				((peekevent.xkey.time-event->xkey.time) < 2) ) {
			repeated = 1;
			XNextEvent(display, &peekevent);
		}
	}
	return(repeated);
}


static void HandleEvents(void)
{
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
	Window sowner;
	Atom type, property;
	unsigned long len, bytes_left, tmp;
	unsigned char *data;
	int format, result;
	char *ret = NULL;

	sowner = XGetSelectionOwner(dpy, XA_PRIMARY);

	if (sowner != None) {
		property = XInternAtom(dpy,
				"GETCLIPBOARDDATA_PROP",
				False);

		XConvertSelection(dpy,
				XA_PRIMARY, XA_STRING,
				property, win, myxtime); /* myxtime == time of last X event */
		XFlush(dpy);

		XGetWindowProperty(dpy,
				win, property,
				0, 0, False, AnyPropertyType,
				&type, &format, &len,
				&bytes_left, &data);
		if (bytes_left > 0) {
			result =
				XGetWindowProperty(dpy,
						win, property,
						0, bytes_left, True, AnyPropertyType,
						&type, &format, &len,
						&tmp, &data);
			if (result == Success) {
				ret = strdup((char *)data);
			}
			XFree(data);
		}
	}
	return ret;
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
	int attrib[] = {
		EGL_NATIVE_VISUAL_TYPE, 0,

		/* RGB565 */
		EGL_BUFFER_SIZE, 16,
		EGL_RED_SIZE, 5,
		EGL_GREEN_SIZE, 6,
		EGL_BLUE_SIZE, 5,

		EGL_DEPTH_SIZE, 8,

		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
		EGL_STENCIL_SIZE, 1,

		EGL_NONE
	};
	int attrib_nostencil[] = {
		EGL_NATIVE_VISUAL_TYPE, 0,

		/* RGB565 */
		EGL_BUFFER_SIZE, 16,
		EGL_RED_SIZE, 5,
		EGL_GREEN_SIZE, 6,
		EGL_BLUE_SIZE, 5,

		EGL_DEPTH_SIZE, 8,

		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,

		EGL_NONE
	};

	Visual *vis;

	Window root;
	Screen *screen;
	unsigned long mask;
	int i;

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

	ri.Con_Printf( PRINT_ALL, " %d %d\n", width, height );

	// destroy the existing window
	GLimp_Shutdown ();

#if 0 // this breaks getenv()? - sbf
	// Mesa VooDoo hacks
	if (fullscreen)
		putenv("MESA_GLX_FX=fullscreen");
	else
		putenv("MESA_GLX_FX=window");
#endif

	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "Error couldn't open the X display\n");
		return rserr_invalid_mode;
	}

	screen = DefaultScreenOfDisplay(dpy);
	scrnum = DefaultScreen(dpy);
	vis = DefaultVisual(dpy, DefaultScreen(dpy));
	root = RootWindow(dpy, scrnum);

	// Get video mode list
	vidmode_ext = false;
	EGLint major, minor;
	eglDisplay = eglGetDisplay((NativeDisplayType) dpy);
	if (!eglInitialize(eglDisplay, &major, &minor))
		GLimp_HandleError();

#define MAX_NUM_CONFIGS 4
	eglSurface = EGL_NO_SURFACE;
	EGLConfig configs[MAX_NUM_CONFIGS];
	EGLint config_count;
	XWindowAttributes WinAttr;
	int XResult = BadImplementation;
	int blackColour = BlackPixel(dpy, root);

	gl_state.hwgamma = false;

	/* window attributes */
	if (vidmode_active) {
		mask = CWBackPixel | CWColormap | CWSaveUnder | CWBackingStore | 
			CWEventMask | CWOverrideRedirect;
	} else
		mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

	win = XCreateSimpleWindow(dpy, root, 0, 0, 1, 1, 0, blackColour, blackColour);
	XStoreName(dpy, win, "Quake II");
#ifdef _HARMATTAN_MULTITOUCH
	karinInitXI2();
#endif
	XSelectInput(dpy, win, X_MASK);
	if (!(XResult = XGetWindowAttributes(dpy, win, &WinAttr)))
		GLimp_HandleError();

	GLimp_DisableComposition();
	XMapWindow(dpy, win);
	GLimp_DisableComposition();

	XFlush(dpy);

#ifdef _HARMATTAN_MULTITOUCH
	karinXI2Atom();

	karinSetMultiMouseEventFunction(karinXI2MouseEvent);
	karinSetMultiMotionEventFunction(karinXI2MotionEvent);
#endif

	/* EGL */
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
		int red_bits, blue_bits, green_bits, depth_bits, alpha_bits;

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

		int stencil_bits;

		if (eglGetConfigAttrib(eglDisplay, configs[i], EGL_STENCIL_SIZE, &stencil_bits)) {
			ri.Con_Printf(PRINT_ALL, "I: got %d bits of stencil\n", stencil_bits);
			if (stencil_bits >= 1) {
				have_stencil = true;
			}
		}
		else
			have_stencil = true;
	}

	*pwidth = width;
	*pheight = height;

	XMoveResizeWindow(dpy, win, 0, 0, WidthOfScreen(screen), HeightOfScreen(screen));

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
		eglDestroyContext(eglDisplay, eglContext);
		eglDestroySurface(eglDisplay, eglSurface);
		eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglTerminate(eglDisplay);

		if (win)
			XDestroyWindow(dpy, win);
		XUngrabKeyboard(dpy, CurrentTime);
		XCloseDisplay(dpy);
	}
	eglContext = NULL;
	eglDisplay = NULL;
	eglSurface = NULL;
	dpy = NULL;
	win = 0;
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
#define GPA( a ) dlsym( glw_state.OpenGLLib, a )

		/*
			 qglXChooseVisual             =  GPA("glXChooseVisual");
			 qglXCreateContext            =  GPA("glXCreateContext");
			 qglXDestroyContext           =  GPA("glXDestroyContext");
			 qglXMakeCurrent              =  GPA("glXMakeCurrent");
			 qglXCopyContext              =  GPA("glXCopyContext");
			 qglXSwapBuffers              =  GPA("glXSwapBuffers");
			 qglXGetConfig                =  GPA("glXGetConfig");
			 */

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
	eglSwapBuffers(eglDisplay, eglSurface);

#ifdef _HARMATTAN
	XForceScreenSaver(dpy, ScreenSaverReset);
#endif
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
#ifndef _HARMATTAN
	XF86VidModeGamma gamma;
	float g;

	g = (1.3 - vid_gamma->value + 1);
	g = (g>1 ? g : 1);
	gamma.red = oldgamma.red * g;
	gamma.green = oldgamma.green * g;
	gamma.blue = oldgamma.blue * g;
	XF86VidModeSetGamma(dpy, scrnum, &gamma);
#endif
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
