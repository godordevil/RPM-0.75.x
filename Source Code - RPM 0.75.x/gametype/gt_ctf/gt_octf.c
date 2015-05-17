// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

#define	ITEM_FLAG			100
								
#define TRIGGER_REDCAPTURE		200
#define TRIGGER_BLUECAPTURE		201

void	GT_UpdateCvars   ( void );
void	GT_RegisterCvars ( void );

extern vmCvar_t	gt_flagReturnTime;
extern vmCvar_t	gt_simpleScoring;
extern vmCvar_t	gt_customType;


void GT_Init_OCTF ( void )
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
	trap_Cmd_RegisterItem ( ITEM_FLAG,  "red_flag", &itemDef );

	// Register the triggers
	memset ( &triggerDef, 0, sizeof(triggerDef) );
	trap_Cmd_RegisterTrigger ( TRIGGER_REDCAPTURE, "red_capture_point", &triggerDef );
	trap_Cmd_RegisterTrigger ( TRIGGER_BLUECAPTURE, "blue_capture_point", &triggerDef );
}


void GT_RunFrame_OCTF ( int time )
{
	gametype.time = time;

	if(gametype.paused)
	{
		if(gametype.redFlagDropTime )
		{
			gametype.redFlagDropTime  += 50;
		}	
	}
	else
	{
	// See if we need to return the red flag yet
		if ( gametype.redFlagDropTime && time - gametype.redFlagDropTime > gt_flagReturnTime.integer * 1000 )
		{
			trap_Cmd_ResetItem ( ITEM_FLAG );
			trap_Cmd_TextMessage ( -1, "The Flag has returned!" );
			trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
			gametype.redFlagDropTime = 0;
		}
	}

	GT_UpdateCvars ( );
}

int GT_Event_OCTF ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
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
				case ITEM_FLAG:
					trap_Cmd_ResetItem ( ITEM_FLAG );
					trap_Cmd_TextMessage ( -1, "^7The ^3Flag ^7has ^3returned!" );
					trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
					gametype.redFlagDropTime = 0;
					return 1;
			}
			
			break;

		case GTEV_ITEM_DROPPED:
		{
			char clientname[MAX_QPATH];

			trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );

			if ( arg0 == ITEM_FLAG)
			{
				if(arg1 == TEAM_BLUE)
					trap_Cmd_TextMessage ( -1, va("%s ^1has dropped the Flag!", clientname ) );
				else
					trap_Cmd_TextMessage ( -1, va("%s ^4has dropped the Flag!", clientname ) );
				gametype.redFlagDropTime = time;
				return 1;
			}
			return 0;
		}

		case GTEV_ITEM_TOUCHED:

			if ( arg0 == ITEM_FLAG && arg2 == TEAM_RED || arg2 == TEAM_BLUE)
			{
				char clientname[MAX_QPATH];
				trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
				if(arg1 == TEAM_BLUE)
					trap_Cmd_TextMessage ( -1, va("^3%s ^4has taken the Flag!", clientname ) );
				else
					trap_Cmd_TextMessage ( -1, va("^3%s ^1has taken the Flag!", clientname ) );

				trap_Cmd_StartGlobalSound ( gametype.flagTakenSound );
				trap_Cmd_RadioMessage ( arg1, "got_it" );
				gametype.redFlagDropTime = 0;
				return 1;
			}
			return 0;

		case GTEV_TRIGGER_TOUCHED:
				if ( ((arg0 == TRIGGER_BLUECAPTURE && arg2 == TEAM_BLUE) || (arg0 == TRIGGER_REDCAPTURE && arg2 == TEAM_RED)) && trap_Cmd_DoesClientHaveItem ( arg1, ITEM_FLAG ) )
				{
					char clientname[MAX_QPATH];
					trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
					switch(arg2)
					{
						case TEAM_BLUE:	
							trap_Cmd_TextMessage ( -1, va("^3%s ^7has captured the ^4Flag!", clientname) );
							break;	
						case TEAM_RED:	
							trap_Cmd_TextMessage ( -1, va("^3%s ^7has captured the ^1Flag!", clientname) );
							break;
					}						
					trap_Cmd_ResetItem ( ITEM_FLAG );
					trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
					trap_Cmd_AddTeamScore ( arg2, 1 );
					trap_Cmd_RadioMessage ( -777, va("%i", arg1) );
					gametype.redFlagDropTime = 0;

					if ( !gt_simpleScoring.integer )
					{
						trap_Cmd_AddClientScore ( arg1, 10 );
					}
					return 1;
				}

			return 0;
	}

	return 0;
}