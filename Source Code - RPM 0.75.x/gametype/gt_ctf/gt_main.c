// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

void	GT_RegisterCvars( void );

//#CTF
void	GT_Init_CTF 	( void );
void	GT_RunFrame_CTF ( int time );
int		GT_Event_CTF	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

//#CCTF
void	GT_Init_CCTF 	( void );
void	GT_RunFrame_CCTF ( int time );
int	GT_Event_CCTF	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

//#TCTF
void	GT_Init_TCTF 	( void );
void	GT_RunFrame_TCTF ( int time );
int		GT_Event_TCTF	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

//#ACTF
void	GT_Init_ACTF 	( void );
void	GT_RunFrame_ACTF ( int time );
int		GT_Event_ACTF	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

//#OCTF
void	GT_Init_OCTF 	( void );
void	GT_RunFrame_OCTF ( int time );
int		GT_Event_OCTF	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

//#CON
void	GT_Init_CON 	( void );
void	GT_RunFrame_CON	( int time );
int		GT_Event_CON	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

//#DOM
void	GT_Init_DOM	( void );
void	GT_RunFrame_DOM	( int time );
int		GT_Event_DOM	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

//#KOTH
void	GT_Init_KOTH	( void );
void	GT_RunFrame_KOTH( int time );
int		GT_Event_KOTH	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

//#RUSH
void	GT_Init_RUSH	( void );
void	GT_RunFrame_RUSH( int time );
int		GT_Event_RUSH	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );


//#DCTF
#ifdef _DCTF
void	GT_Init_DCTF 	( void );
void	GT_RunFrame_DCTF ( int time );
int		GT_Event_DCTF	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );
#endif
//END

//#OP
void	GT_Init_OP 	( void );
void	GT_RunFrame_OP ( int time );
int		GT_Event_OP	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );
//END

//#VIP
void	GT_Init_VIP		( void );
void	GT_RunFrame_VIP	( int time );
int		GT_Event_VIP		( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );
//endif

static qboolean		customChecked = qfalse;

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

vmCvar_t	gt_flagReturnTime;
vmCvar_t	gt_simpleScoring;
vmCvar_t	gt_customType;
vmCvar_t	gt_bombPlantTime;

static cvarTable_t gametypeCvarTable[] = 
{
	// don't override the cheat state set by the system
	{ &gt_flagReturnTime,	"gt_flagReturnTime",	"30", CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
	{ &gt_simpleScoring,	"gt_simpleScoring",		"0",  CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
	{ &gt_bombPlantTime,	"gt_bombPlantTime",		"10",  CVAR_ARCHIVE|CVAR_LATCH, 0.0f, 0.0f, 0, qtrue },

	// *!!!THIS MUST BE THE LAST CVAR!!!*  so when we load the custom GT_Init_* it will have the values inplace
	{ &gt_customType,		"gt_customType",		"0",  CVAR_TEMP, 0.0f, 0.0f, 0, qfalse },
	{ NULL, NULL, NULL, 0, 0.0f, 0.0f, 0, qfalse },
};


/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
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
			case NV_GT_CTF:
				GT_Init_CTF ( );
				return 0;
			case NV_GT_CCTF:
				GT_Init_CCTF ( );
				return 0;
			case NV_GT_OCTF:
				GT_Init_OCTF ( );
				return 0;
			case NV_GT_TCTF:
				GT_Init_TCTF ( );
				return 0;
			case NV_GT_ACTF:
				GT_Init_ACTF ( );
				return 0;
			case NV_GT_CON:
				GT_Init_CON( );
				return 0;
			case NV_GT_DOM:
				GT_Init_DOM( );
				return 0;
			case NV_GT_KOTH:
				GT_Init_KOTH( );
				return 0;
			case NV_GT_RUSH:
				GT_Init_RUSH( );
				return 0;	
#ifdef _DCTF
			case NV_GT_DCTF:
				GT_Init_DCTF( );
				return 0;	
#endif
#ifdef _OVERPOWER
		case NV_GT_OP:
				GT_Init_OP( );
				return 0;	
#endif	
			case NV_GT_VIP:
				GT_Init_VIP ( );
				return 0;
			}

		case GAMETYPE_START:
			return 0;

		case GAMETYPE_RUN_FRAME:
			switch(gt_customType.integer)
			{
			default:
			case NV_GT_CTF:
				GT_RunFrame_CTF ( arg0 );
				return 0;
			case NV_GT_CCTF:
				GT_RunFrame_CCTF ( arg0 );
				return 0;
			case NV_GT_OCTF:
				GT_RunFrame_OCTF ( arg0 );
				return 0;
			case NV_GT_TCTF:
				GT_RunFrame_TCTF ( arg0 );
				return 0;
			case NV_GT_ACTF:
				GT_RunFrame_ACTF ( arg0 );
				return 0;
			case NV_GT_CON:
				GT_RunFrame_CON( arg0 );
				return 0;		
			case NV_GT_DOM:
				GT_RunFrame_DOM( arg0 );
				return 0;	
			case NV_GT_KOTH:
				GT_RunFrame_KOTH( arg0 );
				return 0;	
			case NV_GT_RUSH:
				GT_RunFrame_RUSH( arg0 );
				return 0;	
#ifdef _DCTF
			case NV_GT_DCTF:
				GT_RunFrame_DCTF( arg0 );
				return 0;	
#endif
#ifdef _OVERPOWER
		case NV_GT_OP:
				GT_RunFrame_OP( arg0 );
				return 0;
#endif
			case NV_GT_VIP:
				GT_RunFrame_VIP( arg0 );
				return 0;

			}

		case GAMETYPE_EVENT:
			switch(gt_customType.integer)
			{
			default:
			case NV_GT_CTF:
				return GT_Event_CTF ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
			case NV_GT_OCTF:
				return GT_Event_OCTF ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
			case NV_GT_CCTF:
				return GT_Event_CCTF ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
			case NV_GT_TCTF:
				return GT_Event_TCTF ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
			case NV_GT_ACTF:
				return GT_Event_ACTF ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
			case NV_GT_DOM:
				return GT_Event_DOM ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
			case NV_GT_KOTH:
				return GT_Event_KOTH ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
			case NV_GT_CON:
				return GT_Event_CON ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
			case NV_GT_RUSH:
				return GT_Event_RUSH( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
#ifdef _DCTF
			case NV_GT_DCTF:
				return GT_Event_DCTF( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
#endif
#ifdef _OVERPOWER
			case NV_GT_OP:
				return GT_Event_OP( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
#endif
			case NV_GT_VIP:
				return GT_Event_VIP  ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
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
		if (cv->vmCvar && !Q_stricmp(cv->cvarName,"gt_customType") && !customChecked)
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
