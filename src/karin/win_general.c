/*
 * This file is private for gl_egl1.c and swr_sdl.c.
 * Only for including.
 * Don't use it single.
 * */

static unsigned karinHandleVKBAction(int action, unsigned pressed, int dx, int dy)
{
#define		MAXCMDLINE	256
#define		MAXCMDLENGTH 1024
	static int _keys[MAXCMDLINE];
	static char _cmd[MAXCMDLENGTH];
	unsigned int key_count = 0;

	int r = karinGetActionData(action, _keys, MAXCMDLINE, &key_count, _cmd, MAXCMDLENGTH);
	if(r == Cmd_Data)
	{
		if(karinAddCommand)
		{
			unsigned int time = Sys_Milliseconds();
			char cmd[MAXCMDLENGTH];
			if(pressed)
			{
				if (_cmd[0] == '+')
				{	// button commands add keynum and time as a parm
					Com_sprintf (cmd, sizeof(cmd), "%s %i %i\n", _cmd, _keys[0], time);
					karinAddCommand(cmd);
				}
				else
				{
					karinAddCommand(_cmd);
					karinAddCommand("\n");
				}
			}
			else
			{
				if (_cmd[0] == '+')
				{
					Com_sprintf (cmd, sizeof(cmd), "-%s %i %i\n", _cmd+1, _keys[0], time);
					karinAddCommand(cmd);
				}
			}
			return 1;
		}
	}
	else if(r == Key_Data)
	{
		in_state_t *in_state = getState();
		if (in_state && in_state->Key_Event_fp)
		{
			int i = 0;
			for(i = 0; i < key_count; i++)
			{
				in_state->Key_Event_fp (_keys[i], pressed);
				//printf("%c ", k[i]);
			}
			return 1;
		}
	}
	else if(r == Button_Data)
	{
		if(pressed && (dx != 0 || dy != 0))
		{
			mx += dx;
			my -= dy;
			mouse_buttonstate = 0;
		}
	}

	return 0;
#undef MAXCMDLINE
#undef MAXCMDLENGTH
}

static void karinPrintDev(void)
{
	char fmt_str[] = "[%s]: %s -> %s\n";
	ri.Con_Printf( PRINT_ALL, fmt_str, _HARMATTAN_APPNAME, "Ver", _HARMATTAN_VER);
	ri.Con_Printf( PRINT_ALL, fmt_str, _HARMATTAN_APPNAME, "Code", _HARMATTAN_DEVCODE);
	ri.Con_Printf( PRINT_ALL, fmt_str, _HARMATTAN_APPNAME, "Dev", _HARMATTAN_DEV);
	ri.Con_Printf( PRINT_ALL, fmt_str, _HARMATTAN_APPNAME, "Release", _HARMATTAN_RELEASE);
	ri.Con_Printf( PRINT_ALL, fmt_str, _HARMATTAN_APPNAME, "Desc", _HARMATTAN_DESC);
}
