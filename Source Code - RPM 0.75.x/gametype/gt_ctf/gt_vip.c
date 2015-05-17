// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

					
#define	ITEM_REDFLAG			100
#define ITEM_BLUEFLAG			101

void	GT_UpdateCvars  ( void );
void	GT_RegisterCvars	( void );

extern vmCvar_t	gt_simpleScoring;

void GT_Init_VIP ( void )
{
	gtItemDef_t		itemDef;

	memset ( &gametype, 0, sizeof(gametype) );


	// Register the global sounds
	//gametype.bombTakenSound    = trap_Cmd_RegisterGlobalSound ( "sound/ctf_flag.mp3" );
	//gametype.bombExplodedSound = trap_Cmd_RegisterGlobalSound ( "sound/ctf_return.mp3" );
	//gametype.bombPlantedSound  = trap_Cmd_RegisterGlobalSound ( "sound/ctf_base.mp3" );
	gametype.flagTakenSound   = trap_Cmd_RegisterGlobalSound ( "sound/ctf_flag.mp3" );
	gametype.flagReturnSound  = trap_Cmd_RegisterGlobalSound ( "sound/ctf_return.mp3" );
	gametype.flagCaptureSound = trap_Cmd_RegisterGlobalSound ( "sound/ctf_base.mp3" );
	
	trap_Cmd_RegisterGlobalSound ( "sound/enemy/english/male/report_targets.mp3" );
	trap_Cmd_RegisterGlobalSound ( "sound/enemy/english/male/investigate_area.mp3" );
	trap_Cmd_RegisterGlobalSound ( "sound/enemy/english/fmale/report_targets.mp3" );
	trap_Cmd_RegisterGlobalSound ( "sound/enemy/english/fmale/investigate_area.mp3" );


	// Register the items
	memset ( &itemDef, 0, sizeof(itemDef) );
	trap_Cmd_RegisterItem ( ITEM_REDFLAG,  "red_flag", &itemDef );
	trap_Cmd_RegisterItem ( ITEM_BLUEFLAG, "blue_flag", &itemDef );
}

void GT_RunFrame_VIP ( int time )
{
	gametype.time = time;
	GT_UpdateCvars ( );
}

int GT_Event_VIP ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
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
		
		case GTEV_ITEM_DROPPED:
		{
			char clientname[MAX_QPATH];
			trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
			if(arg2 == TEAM_BLUE)
			{
				//trap_Cmd_TextMessage ( -1, va("^$V^TI^oP^7 %s ^7was ^yk^li^ll^+l^7ed", clientname) );
				trap_Cmd_TextMessage ( -1, va("^$R^TE^oD ^+WINS!") );
				trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
				trap_Cmd_RadioMessage ( -888, va("%i",TEAM_RED) );
				trap_Cmd_RadioMessage ( -333, va("%i", TEAM_BLUE) );
			}
			else
			{
				//trap_Cmd_TextMessage ( -1, va("^iV^yI^fP^7 %s ^7was ^1k^Ti^ol^ql^+e^7d", clientname) );
				trap_Cmd_TextMessage ( -1, va("^iB^yL^fU^+E WINS!") );
				trap_Cmd_AddTeamScore ( TEAM_BLUE, 1 );
				trap_Cmd_RadioMessage ( -888, va("%i",TEAM_BLUE) );
				trap_Cmd_RadioMessage ( -333, va("%i", TEAM_RED) );
			}
			trap_Cmd_Restart ( 5 );

			trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
			break;
		}		
	
		case GTEV_CLIENT_DEATH:
			break;
	}

	return 0;
}


