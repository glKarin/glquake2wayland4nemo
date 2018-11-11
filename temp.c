GLfloat *vs;
GLfloat *ts;
GLfloat *cs;

qglEnableClientState(GL_VERTEX_ARRAY);
qglEnableClientState(GL_TEXTURE_COORD_ARRAY);
qglEnableClientState(GL_COLOR_ARRAY);

qglVertexPointer(3, GL_FLOAT, 0, vs);
qglTexCoordPointer(2, GL_FLOAT, 0, ts);
qglColorPointer(4, GL_FLOAT, 0, cs);

qglDisableClientState(GL_VERTEX_ARRAY);
qglDisableClientState(GL_TEXTURE_COORD_ARRAY);
qglDisableClientState(GL_COLOR_ARRAY);

vs = calloc( * 3, sizeof(GLfloat));
ts = calloc( * 2, sizeof(GLfloat));
cs = calloc( * 4, sizeof(GLfloat));

vs[i * 3] = ;
vs[i * 3 + 1] = ;
vs[i * 3 + 2] = ;

ts[i * 2] = ;
ts[i * 2 + 1] = ;

free(vs);
free(ts);
free(cs);

GLfloat vs[] = {
};

GLfloat ts[] = {
};

GLfloat cs[] = {
};
