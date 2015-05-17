// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

void	GT_UpdateCvars( void ); 
void	GT_RegisterCvars( void ); 

/*
================
GT_Init

initializes the gametype by spawning the gametype items and 
preparing them
================
*/
void GT_Init ( void )
{
	memset ( &gametype, 0, sizeof(gametype) );

	// Register all cvars for this gametype
	//GT_RegisterCvars ( );
}

/*
================
GT_RunFrame

Runs all thinking code for gametype
================
*/
void GT_RunFrame ( int time )
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
int GT_Event ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
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

		default:
			break;
	}

	return 0;
}

