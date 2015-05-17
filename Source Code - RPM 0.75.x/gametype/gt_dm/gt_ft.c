// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

#define	ITEM_REDFLAG			100
#define ITEM_BLUEFLAG			101
								
#define TRIGGER_REDCAPTURE		200
#define TRIGGER_BLUECAPTURE		201

void	GT_Init_FT		( void );
void	GT_RunFrame_FT	( int time );
int		GT_Event_FT	( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

void	GT_UpdateCvars  ( void );
void	GT_RegisterCvars	( void );

void GT_Init_FT ( void )
{
	gtItemDef_t		itemDef;
	gtTriggerDef_t	triggerDef;
//	memset ( &gametype, 0, sizeof(gametype) );

	// Register all cvars for this gametype
	//GT_RegisterCvars ( );

	// Register the global sounds
	//gametype.winRoundSound = trap_Cmd_RegisterGlobalSound ( "sound/ctf_win.mp3" );

	// Register the items
	memset ( &itemDef, 0, sizeof(itemDef) );
	trap_Cmd_RegisterItem ( ITEM_REDFLAG,  "red_flag", &itemDef );
	trap_Cmd_RegisterItem ( ITEM_BLUEFLAG, "blue_flag", &itemDef );

	// Register the triggers
	memset ( &triggerDef, 0, sizeof(triggerDef) );
	trap_Cmd_RegisterTrigger ( TRIGGER_REDCAPTURE, "red_capture_point", &triggerDef );
	trap_Cmd_RegisterTrigger ( TRIGGER_BLUECAPTURE, "blue_capture_point", &triggerDef );
}


void GT_RunFrame_FT ( int time )
{		
	gametype.time = time;
	GT_UpdateCvars ( );
}


int GT_Event_FT ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
{
	switch ( cmd )
	{
		//case GTEV_CLIENT_DEATH:
		//		trap_Cmd_AddClientScore ( arg1, 2 );
		//	break;
		//case GTEV_ITEM_DEFEND:
		//		trap_Cmd_AddClientScore ( arg1, 1 );
		//	break;

		//case GTEV_TEAM_ELIMINATED:
		//	switch ( arg0 )
		//	{
		//		case TEAM_RED:
		//			trap_Cmd_TextMessage ( -1, "^_Red ^7team ^iF^DR^dO^7Z^kE^iN!" );
		//			trap_Cmd_AddTeamScore ( TEAM_BLUE, 1 );
		//			trap_Cmd_Restart ( 5 );
		//			break;

		//		case TEAM_BLUE:
		//			trap_Cmd_TextMessage ( -1, "^iBlue ^7team ^iF^DR^dO^7Z^kE^iN" );
		//			trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
		//			trap_Cmd_Restart ( 5 );
		//			break;
		//	}
		//	trap_Cmd_StartGlobalSound ( gametype.winRoundSound );
		//	break;

		//case GTEV_TIME_EXPIRED:
		//	trap_Cmd_TextMessage ( -1, "*Round Draw*" );
		//	trap_Cmd_Restart ( 5 );
		//	break;
		default:
			break;
	}

	return 0;
}

