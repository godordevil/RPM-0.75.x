// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

void	GT_UpdateCvars  ( void );
void	GT_RegisterCvars	( void );

void GT_Init_GG ( void )
{
	memset ( &gametype, 0, sizeof(gametype) );

	// Register all cvars for this gametype
	GT_RegisterCvars ( );
}

void GT_RunFrame_GG ( int time )
{
	gametype.time = time;

	GT_UpdateCvars ( );
}


int GT_Event_GG ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
{
	switch ( cmd )
	{
		default:
			break;
	}

	return 0;
}
