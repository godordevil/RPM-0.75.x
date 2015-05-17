// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

void	GT_UpdateCvars( void ); 
void	GT_RegisterCvars( void ); 

#define TRIGGER_REDCAPTURE		200

/*
================
GT_Init

initializes the gametype by spawning the gametype items and 
preparing them
================
*/
void GT_Init_FOR ( void )
{
	gtTriggerDef_t	triggerDef;
	memset ( &gametype, 0, sizeof(gametype) );

	// Register all cvars for this gametype
	//GT_RegisterCvars ( );

	gametype.caseCaptureSound = trap_Cmd_RegisterGlobalSound ( "sound/ctf_win.mp3" );
	memset ( &triggerDef, 0, sizeof(triggerDef) );
	trap_Cmd_RegisterTrigger ( TRIGGER_REDCAPTURE, "blue_capture_point", &triggerDef );
}

/*
================
GT_RunFrame

Runs all thinking code for gametype
================
*/
void GT_RunFrame_FOR ( int time )
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
int GT_Event_FOR ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
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
		case GTEV_TRIGGER_TOUCHED:
			{
			char clientname[MAX_QPATH];
			trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
			trap_Cmd_TextMessage ( -1, va("%s ^7has ^3Captured ^7the ^1Fortress!", clientname ) );
			trap_Cmd_RadioMessage ( -888, va("%i", arg2) );
			trap_Cmd_StartGlobalSound ( gametype.caseCaptureSound );
			trap_Cmd_AddTeamScore ( arg2, 1 );
			}
			break;


		default:
			break;
	}

	return 0;
}
