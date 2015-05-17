// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

void	GT_RegisterCvars( void );

//#INF
void	GT_Init_INF 	( void );
void	GT_RunFrame_INF ( int time );
int		GT_Event_INF	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

//#CTC
void	GT_Init_CTC 	( void );
void	GT_RunFrame_CTC ( int time );
int		GT_Event_CTC	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

//#SAB
void	GT_Init_SAB		( void );
void	GT_RunFrame_SAB	( int time );
int		GT_Event_SAB	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

//#RAZE
void	GT_Init_RAZE	( void );
void	GT_RunFrame_RAZE( int time );
int		GT_Event_RAZE	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

//#DEM
void	GT_Init_DEM		( void );
void	GT_RunFrame_DEM	( int time );
int		GT_Event_DEM	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

//#CR
void	GT_Init_CR		( void );
void	GT_RunFrame_CR	( int time );
int		GT_Event_CR		( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

//#ZOM
#ifdef _ZOMBIES
void	GT_Init_ZOM 	( void );
void	GT_RunFrame_ZOM ( int time );
int		GT_Event_ZOM	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );
#endif

void	GT_UpdateCvars  ( void );

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

vmCvar_t	gt_simpleScoring;

vmCvar_t	gt_bombTimer;
vmCvar_t	gt_bombDefuseTime;
vmCvar_t	gt_bombPlantTime;
vmCvar_t	gt_targetHealth;
vmCvar_t	gt_caserunRestart;
vmCvar_t	gt_customType;

static cvarTable_t gametypeCvarTable[] = 
{
	{ &gt_simpleScoring,	"gt_simpleScoring",		"0",  CVAR_ARCHIVE, 0.0f, 0.0f, 0, qfalse },
	
	{ &gt_bombTimer,		"gt_bombTimer",		"45", CVAR_ARCHIVE|CVAR_LATCH, 0.0f, 0.0f, 0, qtrue },
	{ &gt_bombDefuseTime,	"gt_bombDefuseTime",	"10",  CVAR_ARCHIVE|CVAR_LATCH, 0.0f, 0.0f, 0, qtrue },
	{ &gt_bombPlantTime,	"gt_bombPlantTime",		"10",  CVAR_ARCHIVE|CVAR_LATCH, 0.0f, 0.0f, 0, qtrue },
	{ &gt_targetHealth,		"gt_targetHealth",		"3",  CVAR_ARCHIVE|CVAR_LATCH, 0.0f, 0.0f, 0, qtrue },
	
	{ &gt_caserunRestart,	"gt_caserunRestart",	"0",  CVAR_ARCHIVE, 0.0f, 0.0f, 0, qtrue },
	
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
			case NV_GT_INF:
				GT_Init_INF ( );
				return 0;
			case NV_GT_SAB:
				GT_Init_SAB( );
				return 0;	
			case NV_GT_RAZE:
				GT_Init_RAZE( );
				return 0;
			case NV_GT_DEM:
				GT_Init_DEM( );
				return 0;	
			case NV_GT_CR:
				GT_Init_CR( );
				return 0;
			case NV_GT_CTC:
				GT_Init_CTC ( );
				return 0;

#ifdef _ZOMBIES
			case NV_GT_ZOM:
				GT_Init_ZOM ( );
				return 0;
#endif
				
			}

		case GAMETYPE_START:
			switch(gt_customType.integer)
			{
			case NV_GT_DEM:
				gametype.firstFrame    = qtrue;
				gametype.bombPlantTime = 0;
				gametype.bombBeepTime  = 0;
				gametype.roundOver     = qfalse;
				break;	
			case NV_GT_CR:
				gametype.bombPlantTime		= 0;
				gametype.roundOver			= qfalse;
				gametype.bombPlantedTeam	= -1;
				break;
			default:
				return 0;
			}

		case GAMETYPE_RUN_FRAME:
			switch(gt_customType.integer)
			{
			default:
				return 0;
			case NV_GT_INF:
				GT_RunFrame_INF ( arg0 );
				return 0;
			case NV_GT_CTC:
				GT_RunFrame_CTC ( arg0 );
				return 0;
			case NV_GT_SAB:
				GT_RunFrame_SAB( arg0 );
				return 0;		
			case NV_GT_RAZE:
				GT_RunFrame_RAZE( arg0 );
				return 0;	
			case NV_GT_DEM:
				GT_RunFrame_DEM( arg0 );
				return 0;	
			case NV_GT_CR:
				GT_RunFrame_CR( arg0 );
				return 0;

#ifdef _ZOMBIES
			case NV_GT_ZOM:
				GT_RunFrame_ZOM ( arg0  );
				return 0;
#endif
			}

		case GAMETYPE_EVENT:
			switch(gt_customType.integer)
			{
			default:
				return 0;
			case NV_GT_INF:
				return GT_Event_INF ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
			case NV_GT_CTC:
				return GT_Event_CTC ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
			case NV_GT_SAB:
				return GT_Event_SAB ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
			case NV_GT_RAZE:
				return GT_Event_RAZE( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
			case NV_GT_DEM:
				return GT_Event_DEM ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
			case NV_GT_CR:
				return GT_Event_CR  ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );

#ifdef _ZOMBIES
			case NV_GT_ZOM:
				return GT_Event_ZOM  ( arg0, arg1, arg2, arg3, arg4, arg5, arg6 );
#endif
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
