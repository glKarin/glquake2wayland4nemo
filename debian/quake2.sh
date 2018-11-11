#!/bin/sh

EXEC="/usr/bin/sdlquake2 +set basedir /home/user/.quake2 +set vid_ref softsdl +set vid_fullscreen 1"
#+set cl_drawfps 1 

echo "Run Quake 2 (Renderer: SDL)......"
echo "${EXEC} $*"
${EXEC} $*
