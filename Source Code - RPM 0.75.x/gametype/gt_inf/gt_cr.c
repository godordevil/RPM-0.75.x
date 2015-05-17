// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

#define	ITEM_BRIEFCASE			100								
#define TRIGGER_EXTRACTION		200

void	GT_UpdateCvars  ( void );
void	GT_RegisterCvars	( void );

extern  vmCvar_t		gt_simpleScoring;
extern	vmCvar_t		gt_caserunRestart;

//extern customGametypeInfo_t gtInfo[];

void GT_Init_CR ( void )
{
	gtItemDef_t		itemDef;
	//gtTriggerDef_t	triggerDef;

	memset ( &gametype, 0, sizeof(gametype) );

	// Register all cvars for this gametype
	//GT_RegisterCvars ( );

	// Register the global sounds
	gametype.bombTakenSound    = trap_Cmd_RegisterGlobalSound ( "sound/ctf_flag.mp3" );
	gametype.bombExplodedSound = trap_Cmd_RegisterGlobalSound ( "sound/ctf_return.mp3" );
	gametype.bombPlantedSound  = trap_Cmd_RegisterGlobalSound ( "sound/ctf_base.mp3" );

	// Register the items
	memset ( &itemDef, 0, sizeof(itemDef) );
	trap_Cmd_RegisterItem ( ITEM_BRIEFCASE,  "briefcase", &itemDef );

	// Register the triggers
	//memset ( &triggerDef, 0, sizeof(triggerDef) );
	//trap_Cmd_RegisterTrigger ( TRIGGER_EXTRACTION, "briefcase_destination", &triggerDef );
}

void GT_RunFrame_CR ( int time )
{
	gametype.time = time;

	if(gametype.paused && gametype.bombPlantTime )
	{
		gametype.bombPlantTime  += 50;
	}
	else
	{
		if(gametype.bombPlantTime < time + 1000)
		{
			 switch(gametype.bombPlantedTeam)
			 {
				  case TEAM_RED:
					  trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
					   break;
				  case TEAM_BLUE:
					 trap_Cmd_AddTeamScore ( TEAM_BLUE, 1 );
					   break;
				  default:
					   break;
			 }
			 gametype.bombPlantTime = time + 5000;
		}
	}
	GT_UpdateCvars ( );
}

int GT_Event_CR ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
{
	//gtInfo[2].based;
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
			trap_Cmd_ResetItem ( ITEM_BRIEFCASE );
			trap_Cmd_TextMessage ( -1, "The Briefcase has returned!" );
			trap_Cmd_StartGlobalSound ( gametype.bombExplodedSound );
			if(gt_caserunRestart.integer == 2)
				trap_Cmd_Restart ( 2 );
			gametype.bombPlantedTeam = -1;
			return 1;

		case GTEV_RESET_BOMB:
			trap_Cmd_ResetItem ( ITEM_BRIEFCASE );
			gametype.bombPlantTime = 0;
			gametype.bombPlantedTeam = -1;
			return 0;

		case GTEV_TIME_EXPIRED:
			if(gt_caserunRestart.integer == 1)
			{
				trap_Cmd_ResetItem ( ITEM_BRIEFCASE );
				gametype.bombPlantTime = 0;
				gametype.bombPlantedTeam = -1;
				trap_Cmd_Restart ( 2 );
				trap_Cmd_TextMessage ( -1, va("Times up!") );
			}
			break;

		case GTEV_ITEM_DROPPED:
			gametype.bombPlantedTeam = -1;
			if(gt_caserunRestart.integer == 2)
			{
				trap_Cmd_Restart ( 3 );
				trap_Cmd_ResetItem ( ITEM_BRIEFCASE );
				trap_Cmd_TextMessage ( -1, "The Briefcase has returned!" );
				trap_Cmd_StartGlobalSound ( gametype.bombExplodedSound );
			}
			else 
			{
				char clientname[MAX_QPATH];
				trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
				trap_Cmd_TextMessage ( -1, va("%s has dropped the briefcase!", clientname ) );
				trap_Cmd_StartGlobalSound ( gametype.bombPlantedSound );
				break;
			}	
			return 1;

		case GTEV_ITEM_TOUCHED:

			switch ( arg0 )
			{
				case ITEM_BRIEFCASE:
					if ( arg2 == TEAM_BLUE || arg2 == TEAM_RED)
					{
						char clientname[MAX_QPATH];
						trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("%s has taken the briefcase!", clientname ) );
						trap_Cmd_StartGlobalSound ( gametype.bombTakenSound );
						trap_Cmd_RadioMessage ( arg1, "got_it" );
						gametype.bombPlantedTeam = arg2;
						return 1;
					}
					break;
			}

			return 0;
	}

	return 0;
}

