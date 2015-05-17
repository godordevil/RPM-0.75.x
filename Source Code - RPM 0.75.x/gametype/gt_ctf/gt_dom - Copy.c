// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

//void	GT_Init_DOM		( void );
void	GT_RunFrame_DOM	( int time );
int		GT_Event_DOM	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );
void	GT_UpdateCvars( void );

extern vmCvar_t	gt_flagReturnTime;
extern vmCvar_t	gt_simpleScoring;
extern vmCvar_t	gt_customType;

//gametypeLocals_t	gametype;

/*
================
GT_RunFrame

Runs all thinking code for gametype
================
*/
void GT_RunFrame_DOM ( int time )
{
	gametype.time = time;
	GT_UpdateCvars ( );
}

/*
================
GT_Event

Handles all events sent to the gametype
================
*/
int GT_Event_DOM ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
{		

	switch ( cmd )
	{
		case GTEV_ITEM_DEFEND:
			if ( !gt_simpleScoring.integer )
			{
				trap_Cmd_AddClientScore ( arg1, 5 );
			}
			return 0;
		case GTEV_TRIGGER_TOUCHED:
			trap_Cmd_StartGlobalSound ( gametype.flagTakenSound );
				//if(time > sound[arg2]) ///dont spam sounds
				//{
				switch(rand() % 3)
				{
				case 1:
					trap_Cmd_RadioMessage ( arg1, "hold_position" );
					break;
				case 2:
					trap_Cmd_RadioMessage ( arg1, "cover_me" );
					break;
				default:
				case 3:
					trap_Cmd_RadioMessage ( arg1, "need_backup" );
					break;
				//}
				//sound[arg2] = time + 4000;
			}
			return 0;

		case GTEV_ITEM_CANBEUSED:
			//if ( arg0 == ITEM_PLANTED_BOMB /*&& arg2 == TEAM_RED*/ )
			if ( arg2 != arg3 )
			{
				return 1;
			}
			return 0;

		case GTEV_TEAM_ELIMINATED:
			switch ( arg2 )
			{
				case TEAM_BLUE:
					if(arg3){
						trap_Cmd_TextMessage ( -1, va("The ^4BLUE ^7Team is ^3DOMINATING ^7the ^$RED ^7Team!") );
						//trap_Cmd_RadioMessage ( arg1, "obj_compelete" );
					}
					trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
					trap_Cmd_AddTeamScore ( arg2, 1 );
					if ( !gt_simpleScoring.integer )
						trap_Cmd_AddClientScore ( arg1, 10 );
					return 1;	
				case TEAM_RED:
					if(arg3){
						trap_Cmd_TextMessage ( -1, va("The ^$RED ^7Team is ^3DOMINATING ^7the ^4BLUE ^7Team!" ));
						//trap_Cmd_RadioMessage ( arg1, "obj_compelete" );
					}
					trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
					trap_Cmd_AddTeamScore ( arg2, 1 );
					if ( !gt_simpleScoring.integer )
						trap_Cmd_AddClientScore ( arg1, 10 );
					return 1;
				default:
					break;
			}
			return 0;
	}

	return 0;
}

//
//#ifndef GAMETYPE_HARD_LINKED
//// this is only here so the functions in q_shared.c and bg_*.c can link (FIXME)
//
//void QDECL Com_Error( int level, const char *msg, ... ) 
//{
//	va_list		argptr;
//	char		text[1024];
//
//	va_start (argptr, msg);
//	vsprintf (text, msg, argptr);
//	va_end (argptr);
//
//	trap_Error( text );
//}
//
//void QDECL Com_Printf( const char *msg, ... ) 
//{
//	va_list		argptr;
//	char		text[1024];
//
//	va_start (argptr, msg);
//	vsprintf (text, msg, argptr);
//	va_end (argptr);
//
//	trap_Print( text );
//}
//
//#endif
