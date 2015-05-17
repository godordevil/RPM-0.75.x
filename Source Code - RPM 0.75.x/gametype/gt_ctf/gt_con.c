// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

#define	ITEM_REDFLAG			100
#define ITEM_BLUEFLAG			101
								
#define TRIGGER_REDCAPTURE		200
#define TRIGGER_BLUECAPTURE		201

void	GT_UpdateCvars   ( void );
void	GT_RegisterCvars ( void );

extern vmCvar_t	gt_flagReturnTime;
extern vmCvar_t	gt_simpleScoring;


void GT_Init_CON ( void )
{
	gtItemDef_t		itemDef;
	gtTriggerDef_t	triggerDef;

	memset ( &gametype, 0, sizeof(gametype) );

	// Register the global sounds
	gametype.flagReturnSound  = trap_Cmd_RegisterGlobalSound ( "sound/ctf_return.mp3" );
	gametype.flagTakenSound   = trap_Cmd_RegisterGlobalSound ( "sound/ctf_flag.mp3" );
	gametype.flagCaptureSound = trap_Cmd_RegisterGlobalSound ( "sound/ctf_win.mp3" );

	// Register all cvars for this gametype
	//GT_RegisterCvars ( );

	// Register the items
	memset ( &itemDef, 0, sizeof(itemDef) );
	trap_Cmd_RegisterItem ( ITEM_REDFLAG,  "red_flag", &itemDef );
	trap_Cmd_RegisterItem ( ITEM_BLUEFLAG, "blue_flag", &itemDef );

	// Register the triggers
	memset ( &triggerDef, 0, sizeof(triggerDef) );
	trap_Cmd_RegisterTrigger ( TRIGGER_REDCAPTURE, "red_capture_point", &triggerDef );
	trap_Cmd_RegisterTrigger ( TRIGGER_BLUECAPTURE, "blue_capture_point", &triggerDef );
}

void GT_RunFrame_CON ( int time )
{
	gametype.time = time;

	GT_UpdateCvars ( );
}

int GT_Event_CON ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
{
	switch ( cmd )
	{
		case GTEV_CLIENT_DEATH:
			// arg0 = clientID;
			// arg1 = clientTeam;
			// arg2 = killerID
			// arg3 = killerTeam
			
			// If another client killed this guy and wasnt on the same team, then
			// augment the team score for the killer
			if ( arg2 != -1 && arg3 != arg1 )
			{
				trap_Cmd_AddTeamScore ( arg3, 1 );
			}

			break;

		//case GTEV_TIME_EXPIRED:
		//	trap_Cmd_TextMessage ( -1, "Red team has defended the briefcase!" );
		//	trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
		//	trap_Cmd_Restart ( 5 );
		//	break;
		//case GTEV_ITEM_DEFEND:
		//		trap_Cmd_AddClientScore ( arg1, 5 );
		//	return 0;
		case GTEV_TRIGGER_TOUCHED:
			{
			char clientname[MAX_QPATH];
			trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
			trap_Cmd_TextMessage ( -1, va("%s ^7has ^3Conquered ^7the ^1Map!", clientname ) );
			trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
			trap_Cmd_AddTeamScore ( arg2, 1 );
			trap_Cmd_Restart ( 5 );
			}
			break;


		default:
			break;
	}

	return 0;
}
