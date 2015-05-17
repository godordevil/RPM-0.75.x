#include "../cg_local.h"
#include "../../game/bg_local.h"
#include "../../ui/ui_shared.h"
#include "../../ui/keycodes.h"

#ifdef _DEBUG
typedef struct {
	char	*name;
	int		keynum;
} keyname_t;


// names not in this list can either be lowercase ascii, or '0xnn' hex sequences
keyname_t keynames[] =
{
	{"TAB", K_TAB},
	{"ENTER", K_ENTER},
	{"ESCAPE", K_ESCAPE},
	{"SPACE", K_SPACE},
	{"BACKSPACE", K_BACKSPACE},
	{"UPARROW", K_UPARROW},
	{"DOWNARROW", K_DOWNARROW},
	{"LEFTARROW", K_LEFTARROW},
	{"RIGHTARROW", K_RIGHTARROW},

	{"ALT", K_ALT},
	{"CTRL", K_CTRL},
	{"SHIFT", K_SHIFT},

	{"COMMAND", K_COMMAND},

	{"CAPSLOCK", K_CAPSLOCK},

	
	{"F1", K_F1},
	{"F2", K_F2},
	{"F3", K_F3},
	{"F4", K_F4},
	{"F5", K_F5},
	{"F6", K_F6},
	{"F7", K_F7},
	{"F8", K_F8},
	{"F9", K_F9},
	{"F10", K_F10},
	{"F11", K_F11},
	{"F12", K_F12},

	{"INS", K_INS},
	{"DEL", K_DEL},
	{"PGDN", K_PGDN},
	{"PGUP", K_PGUP},
	{"HOME", K_HOME},
	{"END", K_END},

	{"MOUSE1", K_MOUSE1},
	{"MOUSE2", K_MOUSE2},
	{"MOUSE3", K_MOUSE3},
	{"MOUSE4", K_MOUSE4},
	{"MOUSE5", K_MOUSE5},

	{"MWHEELUP",	K_MWHEELUP },
	{"MWHEELDOWN",	K_MWHEELDOWN },

	{"JOY1", K_JOY1},
	{"JOY2", K_JOY2},
	{"JOY3", K_JOY3},
	{"JOY4", K_JOY4},
	{"JOY5", K_JOY5},
	{"JOY6", K_JOY6},
	{"JOY7", K_JOY7},
	{"JOY8", K_JOY8},
	{"JOY9", K_JOY9},
	{"JOY10", K_JOY10},
	{"JOY11", K_JOY11},
	{"JOY12", K_JOY12},
	{"JOY13", K_JOY13},
	{"JOY14", K_JOY14},
	{"JOY15", K_JOY15},
	{"JOY16", K_JOY16},
	{"JOY17", K_JOY17},
	{"JOY18", K_JOY18},
	{"JOY19", K_JOY19},
	{"JOY20", K_JOY20},
	{"JOY21", K_JOY21},
	{"JOY22", K_JOY22},
	{"JOY23", K_JOY23},
	{"JOY24", K_JOY24},
	{"JOY25", K_JOY25},
	{"JOY26", K_JOY26},
	{"JOY27", K_JOY27},
	{"JOY28", K_JOY28},
	{"JOY29", K_JOY29},
	{"JOY30", K_JOY30},
	{"JOY31", K_JOY31},
	{"JOY32", K_JOY32},

	{"AUX1", K_AUX1},
	{"AUX2", K_AUX2},
	{"AUX3", K_AUX3},
	{"AUX4", K_AUX4},
	{"AUX5", K_AUX5},
	{"AUX6", K_AUX6},
	{"AUX7", K_AUX7},
	{"AUX8", K_AUX8},
	{"AUX9", K_AUX9},
	{"AUX10", K_AUX10},
	{"AUX11", K_AUX11},
	{"AUX12", K_AUX12},
	{"AUX13", K_AUX13},
	{"AUX14", K_AUX14},
	{"AUX15", K_AUX15},
	{"AUX16", K_AUX16},

	{"KP_HOME",			K_KP_HOME },
	{"KP_UPARROW",		K_KP_UPARROW },
	{"KP_PGUP",			K_KP_PGUP },
	{"KP_LEFTARROW",	K_KP_LEFTARROW },
	{"KP_5",			K_KP_5 },
	{"KP_RIGHTARROW",	K_KP_RIGHTARROW },
	{"KP_END",			K_KP_END },
	{"KP_DOWNARROW",	K_KP_DOWNARROW },
	{"KP_PGDN",			K_KP_PGDN },
	{"KP_ENTER",		K_KP_ENTER },
	{"KP_INS",			K_KP_INS },
	{"KP_DEL",			K_KP_DEL },
	{"KP_SLASH",		K_KP_SLASH },
	{"KP_MINUS",		K_KP_MINUS },
	{"KP_PLUS",			K_KP_PLUS },
	{"KP_NUMLOCK",		K_KP_NUMLOCK },
	{"KP_STAR",			K_KP_STAR },
	{"KP_EQUALS",		K_KP_EQUALS },

	{"PAUSE", K_PAUSE},
	
	{"SEMICOLON", ';'},	// because a raw semicolon seperates commands

	{NULL,0}
};


int Key_StringToKeynum( char *str ) {
	keyname_t	*kn;
	
	if ( !str || !str[0] ) {
		return -1;
	}
	if ( !str[1] ) {
		return str[0];
	}

	// check for hex code
	if ( str[0] == '0' && str[1] == 'x' && strlen( str ) == 4) {
		int		n1, n2;
		
		n1 = str[2];
		if ( n1 >= '0' && n1 <= '9' ) {
			n1 -= '0';
		} else if ( n1 >= 'a' && n1 <= 'f' ) {
			n1 = n1 - 'a' + 10;
		} else {
			n1 = 0;
		}

		n2 = str[3];
		if ( n2 >= '0' && n2 <= '9' ) {
			n2 -= '0';
		} else if ( n2 >= 'a' && n2 <= 'f' ) {
			n2 = n2 - 'a' + 10;
		} else {
			n2 = 0;
		}

		return n1 * 16 + n2;
	}

	// scan for a text match
	for ( kn=keynames ; kn->name ; kn++ ) {
		if ( !Q_stricmp( str,kn->name ) )
			return kn->keynum;
	}

	return -1;
}

char *Key_KeynumToString( int keynum ) {
	keyname_t	*kn;	
	static	char	tinystr[5];
	int			i, j;

	if ( keynum == -1 ) {
		return "1";
	}

	if ( keynum < 0 || keynum > 255 ) {
		return "1";
	}

	// check for printable ascii (don't use quote)
	if ( keynum > 32 && keynum < 127 && keynum != '"' && keynum != ';' ) {
		tinystr[0] = keynum;
		tinystr[1] = 0;
		return tinystr;
	}

	// check for a key string
	for ( kn=keynames ; kn->name ; kn++ ) {
		if (keynum == kn->keynum) {
			return kn->name;
		}
	}

	// make a hex string
	i = keynum >> 4;
	j = keynum & 15;

	tinystr[0] = '0';
	tinystr[1] = 'x';
	tinystr[2] = i > 9 ? i - 10 + 'a' : i + '0';
	tinystr[3] = j > 9 ? j - 10 + 'a' : j + '0';
	tinystr[4] = 0;

	return tinystr;
}

#endif

