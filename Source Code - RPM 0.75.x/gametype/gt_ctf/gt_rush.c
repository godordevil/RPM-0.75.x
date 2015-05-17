// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"
				
// Keeping these as 200 / 201 because I think they are that way in 'game' code for stats
#define TRIGGER_DEMOSITE_1		200
#define TRIGGER_DEMOSITE_2		201

// Here its just targets but setup for my target array.. just easier to track
#define TARGET_1		0
#define TARGET_2		1


void	GT_UpdateCvars  ( void );
void	GT_RegisterCvars	( void );

extern vmCvar_t	gt_flagReturnTime;
extern vmCvar_t	gt_simpleScoring;
extern vmCvar_t	gt_bombPlantTime;
extern vmCvar_t	gt_customType;

static int targetState[2]; //[target] = state  (0 - red team; 1 = blue team; 2 destroyed)
static int targetTimer[2]; //[target] = time

void GT_Init_RUSH ( void )
{
	gtTriggerDef_t	triggerDef;

	memset ( &gametype, 0, sizeof(gametype) );

	// Register all cvars for this gametype
	//GT_RegisterCvars ( );

	gametype.flagTakenSound     = trap_Cmd_RegisterGlobalSound ( "sound/ctf_flag.mp3" );
	gametype.flagReturnSound	= trap_Cmd_RegisterGlobalSound ( "sound/ctf_win.mp3" );
	gametype.flagCaptureSound	= trap_Cmd_RegisterGlobalSound ( "sound/ctf_base.mp3" );
	
	// Register the triggers
	memset ( &triggerDef, 0, sizeof(triggerDef) );
	triggerDef.use		= qtrue;
	triggerDef.useTime	= gt_bombPlantTime.integer * 600;
	trap_Cmd_RegisterTrigger ( TRIGGER_DEMOSITE_1, "briefcase_destination_blue", &triggerDef );
	trap_Cmd_RegisterTrigger ( TRIGGER_DEMOSITE_2, "briefcase_destination_red", &triggerDef );

	targetState[TARGET_1] = 0;
	targetState[TARGET_2] = 0;

	targetTimer[TARGET_1] = 0;
	targetTimer[TARGET_2] = 0;
}


void GT_RunFrame_RUSH ( int time )
{
	gametype.time = time;
	GT_UpdateCvars ( );
}


int GT_Event_RUSH ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
{		
	switch ( cmd )
	{	
		//case GTEV_TIME_EXPIRED:
		//	{
		//		trap_Cmd_TextMessage ( -1, "Red team has defended the targets!" );
		//		trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
		//		trap_Cmd_Restart ( 5 );
		//		break;
		//	}
		//	break;
		case GTEV_TEAM_ELIMINATED:
			switch ( arg0 )
			{
				case TEAM_BLUE: // This means red wins by killing all blues reinforcements
					{
						trap_Cmd_TextMessage ( -1, "Blue has run out of Reinforcements!" );
						//trap_Cmd_AddTeamScore ( TEAM_RED, 10000 );
						//trap_Cmd_Restart ( 5 );
					}
					break;
				case TEAM_RED:  // This means the next phase has started.  Just for sound really
					{		
						trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
						//trap_Cmd_Restart ( 5 ); // not going to restart, dont feel like making a 'timer rester' for active bomb timers.
					}
					break;
			}
			break;

		case GTEV_TRIGGER_USED:
			{
			switch (arg2)
			{
				case TEAM_RED:
					{
						char	name[128];			
						trap_Cmd_GetClientName ( arg1, name, 128 );
						trap_Cmd_TextMessage ( -1, va("%s ^$defused the target!", name ) );
						trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
						return 1;
					}
				case TEAM_BLUE:
					{
						char	name[128];			
						//char	target[64];			
						trap_Cmd_GetClientName ( arg1, name, 128 );
						trap_Cmd_TextMessage ( -1, va("%s ^$activated the target!", name ) );
						trap_Cmd_StartGlobalSound ( gametype.flagTakenSound );
						//trap_Cmd_GetClientName( arg0, target, sizeof(target) );
						//trap_Cmd_RadioMessage ( -999, target );
						return 1;
					}
				}
			}
			break;
		case GTEV_TRIGGER_CANBEUSED:
			{
				if(arg2 == arg0 - 199)
				{
					return 1;
				}
				break;
			}
			break;
	}

	return 0;
}

