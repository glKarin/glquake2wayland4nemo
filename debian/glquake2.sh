#!/bin/bash

USER_EXEC=/home/nemo/.quake2/GLquake2.sh

EXEC="/usr/bin/quake2 +set basedir /home/nemo/.quake2 +set vid_ref glx +set gl_driver libGLESv1_CM.so +set vid_fullscreen 1 +set gl_ext_multitexture 0 +set gl_ext_pointparameters 1 +set gl_shadows 1 +set gl_stencilshadow 1"
#+set cl_drawfps 1 

echo "Run GL Quake II......"
if [ -x ${USER_EXEC} ]; then
	echo "Using user shell script."
	${USER_EXEC} $*
else
	echo "Using global default shell script."
	echo "${EXEC} $*"
	${EXEC} $*
fi

