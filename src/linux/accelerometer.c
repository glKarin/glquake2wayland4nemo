#include <stdio.h>
#include "accelerometer.h"

static int ocnt=0;
static int oax=0;
static int oay=0;
static int oaz=0;
	
static const char *accel_filename = "/sys/class/i2c-adapter/i2c-3/3-001d/coord";

int accel_read(int *ax,int *ay,int *az)
{
	FILE *fd;
	int rs;
	fd = fopen(accel_filename, "r");
	if(fd==NULL){ return -1;}	
	rs=fscanf((FILE*) fd,"%i %i %i",ax,ay,az);	
	fclose(fd);	
	if(rs != 3){ return -2;}
	int bx=*ax;
	int by=*ay;
	int bz=*az;
	if(ocnt>0)
	{
		*ax=oax+(bx-oax)*1;
		*ay=oay+(by-oay)*1;
		*az=oaz+(bz-oaz)*1;
	}
	oax=*ax;
	oay=*ay;
	oaz=*az;
	ocnt++;
	return 0;
}
