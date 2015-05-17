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
extern vmCvar_t	gt_customType;


void GT_Init_CTF ( void )
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


void GT_RunFrame_CTF ( int time )
{
	gametype.time = time;

	if(gametype.paused)
	{
		if(gametype.redFlagDropTime )
		{
			gametype.redFlagDropTime  += 50;
		}	
		if(gametype.blueFlagDropTime )
		{
			gametype.blueFlagDropTime  += 50;
		}
	}
	else
	{
		// See if we need to return the red flag yet
		if ( gametype.redFlagDropTime && time - gametype.redFlagDropTime > gt_flagReturnTime.integer * 1000 )
		{
			trap_Cmd_ResetItem ( ITEM_REDFLAG );
			trap_Cmd_TextMessage ( -1, "The Red Flag has returned!" );
			trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
			gametype.redFlagDropTime = 0;
		}

		// See if we need to return the blue flag yet
		if ( gametype.blueFlagDropTime && time - gametype.blueFlagDropTime > gt_flagReturnTime.integer * 1000 )
		{
			trap_Cmd_ResetItem ( ITEM_BLUEFLAG );
			trap_Cmd_TextMessage ( -1, "The Blue Flag has returned!" );
			trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
			gametype.blueFlagDropTime = 0;
		}
	}

	GT_UpdateCvars ( );
}

int GT_Event_CTF ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
{
	switch ( cmd )
	{	
		case GTEV_START_PAUSE:
 			gametype.paused = qtrue;
			return 0;

		case GTEV_END_PAUSE:
			gametype.paused = qfalse;
			return 0;

		case GTEV_ITEM_DEFEND:
			if ( !gt_simpleScoring.integer )
			{
				trap_Cmd_AddClientScore ( arg1, 5 );
			}
			return 0;

		case GTEV_ITEM_STUCK:
			switch ( arg0 )
			{
				case ITEM_REDFLAG:
					trap_Cmd_ResetItem ( ITEM_REDFLAG );
					trap_Cmd_TextMessage ( -1, "The Red Flag has returned!" );
					trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
					gametype.redFlagDropTime = 0;
					return 1;

				case ITEM_BLUEFLAG:
					trap_Cmd_ResetItem ( ITEM_BLUEFLAG );
					trap_Cmd_TextMessage ( -1, "The Blue Flag has returned!" );
					trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
					gametype.blueFlagDropTime = 0;
					return 1;
			}
			
			break;

		case GTEV_ITEM_DROPPED:
		{
			char clientname[MAX_QPATH];

			trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );

			switch ( arg0 )
			{
				case ITEM_BLUEFLAG:
					trap_Cmd_TextMessage ( -1, va("%s ^4has dropped the Blue Flag!", clientname ) );
					gametype.blueFlagDropTime = time;
					break;
				
				case ITEM_REDFLAG:
					trap_Cmd_TextMessage ( -1, va("%s ^$has dropped the Red Flag!", clientname ) );
					gametype.redFlagDropTime = time;
					break;
			}
			break;
		}

		case GTEV_ITEM_TOUCHED:

			switch ( arg0 )
			{
				case ITEM_BLUEFLAG:
					if ( arg2 == TEAM_RED )
					{
						char clientname[MAX_QPATH];
						trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("%s ^$has taken the Blue Flag!", clientname ) );
						trap_Cmd_StartGlobalSound ( gametype.flagTakenSound );
						trap_Cmd_RadioMessage ( arg1, "got_it" );
						gametype.blueFlagDropTime = 0;

						return 1;
					}
					break;

				case ITEM_REDFLAG:
					if ( arg2 == TEAM_BLUE )
					{
						char clientname[MAX_QPATH];
						trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("%s ^4has taken the Red Flag!", clientname ) );
						trap_Cmd_StartGlobalSound ( gametype.flagTakenSound );
						trap_Cmd_RadioMessage ( arg1, "got_it" );
						gametype.redFlagDropTime = 0;

						return 1;
					}
					break;
			}

			return 0;

		case GTEV_TRIGGER_TOUCHED:
			switch ( arg0 )
			{
				case TRIGGER_BLUECAPTURE:
					if ( trap_Cmd_DoesClientHaveItem ( arg1, ITEM_REDFLAG ) )
					{
						char clientname[MAX_QPATH];
						trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("%s ^4has captured the Red Flag!", clientname ) );
						trap_Cmd_ResetItem ( ITEM_REDFLAG );
						trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
						trap_Cmd_AddTeamScore ( arg2, 1 );
						trap_Cmd_RadioMessage ( -777, va("%i", arg1) );

						if ( !gt_simpleScoring.integer )
						{
							trap_Cmd_AddClientScore ( arg1, 10 );
						}
						gametype.redFlagDropTime = 0;
						return 1;
					}
					break;

				case TRIGGER_REDCAPTURE:
					if ( trap_Cmd_DoesClientHaveItem ( arg1, ITEM_BLUEFLAG ) )
					{
						char clientname[MAX_QPATH];
						trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("%s ^$has captured the Blue Flag!", clientname ) );
						trap_Cmd_ResetItem ( ITEM_BLUEFLAG );
						trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
						trap_Cmd_AddTeamScore ( arg2, 1 );
						trap_Cmd_RadioMessage ( -777, va("%i", arg1) );

						if ( !gt_simpleScoring.integer )
						{
							trap_Cmd_AddClientScore ( arg1, 10 );
						}

						gametype.blueFlagDropTime = 0;
						return 1;
					}
					break;
			}

			return 0;
	}

	return 0;
}
