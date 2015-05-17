// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

#define	ITEM_BRIEFCASE			100								
#define TRIGGER_EXTRACTION		200
#define TRIGGER_EXTRACTION2		201

void	GT_Init_SAB		( void );
void	GT_RunFrame_SAB	( int time );
int		GT_Event_SAB	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );
void	GT_UpdateCvars  ( void );
void	GT_RegisterCvars	( void );

extern	vmCvar_t	gt_customType;
extern  vmCvar_t		gt_simpleScoring;


void GT_Init_SAB ( void )
{
	gtItemDef_t		itemDef;
	gtTriggerDef_t	triggerDef;

	memset ( &gametype, 0, sizeof(gametype) );

	// Register all cvars for this gametype
	//RCW DONT DO THIS AGAIN.. thats how we got here :)
	GT_RegisterCvars ( );

	// Register the global sounds
	gametype.caseTakenSound   = trap_Cmd_RegisterGlobalSound ( "sound/ctf_flag.mp3" );
	gametype.caseCaptureSound = trap_Cmd_RegisterGlobalSound ( "sound/ctf_win.mp3" );
	gametype.caseReturnSound  = trap_Cmd_RegisterGlobalSound ( "sound/ctf_return.mp3" );

	// Register the items
	memset ( &itemDef, 0, sizeof(itemDef) );
	trap_Cmd_RegisterItem ( ITEM_BRIEFCASE,  "briefcase", &itemDef );

	// Register the triggers
	memset ( &triggerDef, 0, sizeof(triggerDef) );
	trap_Cmd_RegisterTrigger ( TRIGGER_EXTRACTION, "briefcase_destination_red", &triggerDef );
	trap_Cmd_RegisterTrigger ( TRIGGER_EXTRACTION2, "briefcase_destination_blue", &triggerDef );
}


void GT_RunFrame_SAB ( int time )
{		
	gametype.time = time;
	GT_UpdateCvars ( );
}


int GT_Event_SAB ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
{
	switch ( cmd )
	{
		case GTEV_ITEM_DEFEND:
			if ( !gt_simpleScoring.integer )
			{
				trap_Cmd_AddClientScore ( arg1, 5 );
			}
			return 0;

		case GTEV_ITEM_STUCK:
			trap_Cmd_ResetItem ( ITEM_BRIEFCASE );
			trap_Cmd_TextMessage ( -1, "The ^3Bomb ^7has returned!" );
			trap_Cmd_StartGlobalSound ( gametype.caseReturnSound );
			return 1;

		case GTEV_TEAM_ELIMINATED:
			//switch ( arg0 )
			//{
			//	case TEAM_RED:
			//		trap_Cmd_TextMessage ( -1, "Red team eliminated!" );
			//		trap_Cmd_AddTeamScore ( TEAM_BLUE, 1 );
			//		trap_Cmd_Restart ( 5 );
			//		break;

			//	case TEAM_BLUE:
			//		trap_Cmd_TextMessage ( -1, "Blue team eliminated!" );
			//		trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
			//		trap_Cmd_Restart ( 5 );
			//		break;
			//}
			break;

		case GTEV_TIME_EXPIRED:
			trap_Cmd_TextMessage ( -1, "^3*Round time has expired^3*" );
			//trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
			trap_Cmd_Restart ( 5 );
			break;

		case GTEV_ITEM_DROPPED:
		{
			char clientname[MAX_QPATH];
			trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
			trap_Cmd_TextMessage ( -1, va("%s ^7has dropped the ^3Bomb!", clientname ) );
			break;
		}

		case GTEV_ITEM_TOUCHED:
			switch ( arg0 )
			{
				case ITEM_BRIEFCASE:
					if ( arg2 == TEAM_BLUE || arg2 == TEAM_RED )
					{
						char clientname[MAX_QPATH];
						trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("%s ^7has taken the ^3Bomb!", clientname ) );
						trap_Cmd_StartGlobalSound ( gametype.caseTakenSound );
						trap_Cmd_RadioMessage ( arg1, "got_it" );
						return 1;
					}
					break;
			}
			return 0;

		case GTEV_TRIGGER_TOUCHED:
			switch ( arg0 )
			{
				case TRIGGER_EXTRACTION:
					trap_Cmd_TextMessage ( -1, "The ^_RED ^7Team ^3WINS ^7the round!" );
					trap_Cmd_StartGlobalSound ( gametype.caseCaptureSound );
					trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
					trap_Cmd_AddClientScore ( arg1, 5 );
					trap_Cmd_Restart ( 5 );
					break;
				case TRIGGER_EXTRACTION2:
					trap_Cmd_TextMessage ( -1, "The ^4BLUE ^7Team ^3WINS ^7the round!" );
					trap_Cmd_StartGlobalSound ( gametype.caseCaptureSound );
					trap_Cmd_AddTeamScore ( TEAM_BLUE, 1 );
					trap_Cmd_AddClientScore ( arg1, 5 );
					trap_Cmd_Restart ( 5 );
					break;
			}
			return 0;
	}

	return 0;
}

