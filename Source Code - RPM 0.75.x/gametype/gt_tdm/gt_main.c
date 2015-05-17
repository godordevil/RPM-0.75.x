// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

void	GT_RegisterCvars( void );

void	GT_Init		( void );
void	GT_RunFrame	( int time );
int		GT_Event	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

//#FOR
void	GT_Init_FOR		( void );
void	GT_RunFrame_FOR	( int time );
int		GT_Event_FOR	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

static qboolean		customChecked = qfalse;

//END

gametypeLocals_t	gametype;

typedef struct 
{
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
	float		mMinValue, mMaxValue;
	int			modificationCount;  // for tracking changes
	qboolean	trackChange;	    // track this variable, and announce if changed
	qboolean	teamShader;			// track and if changed, update shader state

} cvarTable_t;

vmCvar_t	gt_customType;

static cvarTable_t gametypeCvarTable[] = 
{
	// *!!!THIS MUST BE THE LAST CVAR!!!*  so when we load the custom GT_Init_* it will have the values inplace
	{ &gt_customType,		"gt_customType",		"0",  CVAR_TEMP, 0.0f, 0.0f, 0, qfalse },
	{ NULL, NULL, NULL, 0, 0.0f, 0.0f, 0, qfalse },
};

int vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11 ) 
{
	switch ( command ) 
	{
		case GAMETYPE_INIT:
			switch(gt_customType.integer)
			{
			default:
				if(!customChecked)
					GT_RegisterCvars();
				return 0;
			case NV_GT_TDM:
				GT_Init ( );
				return 0;
			case NV_GT_FOR:
				GT_Init_FOR ( );
				return 0;
			}

		case GAMETYPE_START:
			return 0;

		case GAMETYPE_RUN_FRAME:
			switch(gt_customType.integer)
			{
			default:
			case NV_GT_TDM:
				GT_RunFrame ( arg0 );
				return 0;
			case NV_GT_FOR:
				GT_RunFrame_FOR ( arg0 );
				return 0;
			}

		case GAMETYPE_EVENT:
			switch(gt_customType.integer)
			{
			default:
			case NV_GT_TDM:
				return GT_Event ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
			case NV_GT_FOR:
				return GT_Event_FOR ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
			}
	}

	return -1;
}

/*
=================
GT_RegisterCvars
=================
*/
void GT_RegisterCvars( void ) 
{
	cvarTable_t	*cv;

	for ( cv = gametypeCvarTable ; cv->cvarName != NULL; cv++ ) 
	{
		trap_Cvar_Register( cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags, cv->mMinValue, cv->mMaxValue );
		// GodOrDevil - check for a cvar change and call the GT_INIT again if needed (register GT specific shit)
		if (!Q_stricmp(cv->cvarName,"gt_customType") && !customChecked)
		{
			customChecked = qtrue;
			vmMain( GAMETYPE_INIT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  );
		}
		//END
		if ( cv->vmCvar )
		{
			cv->modificationCount = cv->vmCvar->modificationCount;
		}
	}
}

/*
=================
GT_UpdateCvars
=================
*/
void GT_UpdateCvars( void ) 
{
	cvarTable_t	*cv;

	for ( cv = gametypeCvarTable ; cv->cvarName != NULL; cv++ ) 
	{
		if ( cv->vmCvar ) 
		{
			trap_Cvar_Update( cv->vmCvar );

			if ( cv->modificationCount != cv->vmCvar->modificationCount ) 
			{
				cv->modificationCount = cv->vmCvar->modificationCount;
			}
		}
	}
}

#ifndef GAMETYPE_HARD_LINKED
// this is only here so the functions in q_shared.c and bg_*.c can link (FIXME)

void QDECL Com_Error( int level, const char *msg, ... ) 
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	vsprintf (text, msg, argptr);
	va_end (argptr);

	trap_Error( text );
}

void QDECL Com_Printf( const char *msg, ... ) 
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	vsprintf (text, msg, argptr);
	va_end (argptr);

	trap_Print( text );
}

#endif
