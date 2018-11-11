#ifndef KARIN_M_XI2_H
#define KARIN_M_XI2_H

#include <X11/Xlib.h>

typedef Bool (* MultiMouseEvent_Function)(int button, Bool pressed, int x, int y);
typedef Bool (* MultiMotionEvent_Function)(int button, Bool pressed, int x, int y, int dx, int dy);

extern Time myxitime;

void karinInitXI2(void);
void karinXI2Atom(void);
Bool karinXI2Event(XEvent *event);
int X11_Pending(Display *display);
void karinSetMultiMouseEventFunction(MultiMouseEvent_Function f);
void karinSetMultiMotionEventFunction(MultiMotionEvent_Function f);

#endif
