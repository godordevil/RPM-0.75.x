// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

void	GT_UpdateCvars  ( void );
void	GT_RegisterCvars	( void );

void GT_Init_TFT ( void )
{

	memset ( &gametype, 0, sizeof(gametype) );

	// Register all cvars for this gametype
	//GT_RegisterCvars ( );

	// Register the global sounds
	gametype.winRoundSound = trap_Cmd_RegisterGlobalSound ( "sound/ctf_win.mp3" );
}


void GT_RunFrame_TFT ( int time )
{		
	gametype.time = time;
	GT_UpdateCvars ( );
}


int GT_Event_TFT ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
{
	switch ( cmd )
	{
		//case GTEV_CLIENT_DEATH:
		//		trap_Cmd_AddClientScore ( arg1, 2 );
		//	break;
		//case GTEV_ITEM_DEFEND:
		//		trap_Cmd_AddClientScore ( arg1, 1 );
		//	break;

		case GTEV_TEAM_ELIMINATED:
			switch ( arg0 )
			{
				case TEAM_RED:
					trap_Cmd_TextMessage ( -1, "^_Red ^7team ^iF^DR^dO^7Z^kE^iN!" );
					trap_Cmd_AddTeamScore ( TEAM_BLUE, 50 );
					trap_Cmd_RadioMessage ( -888, va("%i",TEAM_BLUE) );
					trap_Cmd_Restart ( 5 );
					break;

				case TEAM_BLUE:
					trap_Cmd_TextMessage ( -1, "^iBlue ^7team ^iF^DR^dO^7Z^kE^iN" );
					trap_Cmd_AddTeamScore ( TEAM_RED, 50 );
					trap_Cmd_RadioMessage ( -888, va("%i",TEAM_RED ));
					trap_Cmd_Restart ( 5 );
					break;
			}
			trap_Cmd_StartGlobalSound ( gametype.winRoundSound );
			break;

		case GTEV_TIME_EXPIRED:
			trap_Cmd_TextMessage ( -1, "*Round Draw*" );
			trap_Cmd_Restart ( 5 );
			break;
	}

	return 0;
}

