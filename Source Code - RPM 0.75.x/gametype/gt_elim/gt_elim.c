// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

void	GT_UpdateCvars  ( void );
void	GT_RegisterCvars	( void );


void GT_Init_ELIM ( void )
{
	memset ( &gametype, 0, sizeof(gametype) );

	// Register all cvars for this gametype
	GT_RegisterCvars ( );
}

void GT_RunFrame_ELIM ( int time )
{
	gametype.time = time;

	GT_UpdateCvars ( );
}

int GT_Event_ELIM ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
{
	switch ( cmd )
	{
		case GTEV_TEAM_ELIMINATED:
			switch ( arg0 )
			{
				case TEAM_RED:
					trap_Cmd_TextMessage ( -1, "Red team eliminated!" );
					trap_Cmd_AddTeamScore ( TEAM_BLUE, 1 );
					trap_Cmd_Restart ( 5 );
					break;

				case TEAM_BLUE:
					trap_Cmd_TextMessage ( -1, "Blue team eliminated!" );
					trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
					trap_Cmd_Restart ( 5 );
					break;
			}
			break;

		case GTEV_TIME_EXPIRED:
			trap_Cmd_TextMessage ( -1, "Round Draw!" );
			trap_Cmd_Restart ( 5 );
			break;
	}

	return 0;
}
