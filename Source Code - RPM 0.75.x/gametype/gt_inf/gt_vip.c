// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"


#define	ITEM_BRIEFCASE			100								

void	GT_UpdateCvars  ( void );
void	GT_RegisterCvars	( void );

extern vmCvar_t	gt_simpleScoring;

void GT_Init_VIP ( void )
{
	gtItemDef_t		itemDef;

	memset ( &gametype, 0, sizeof(gametype) );


	// Register the global sounds
	gametype.bombTakenSound    = trap_Cmd_RegisterGlobalSound ( "sound/ctf_flag.mp3" );
	gametype.bombExplodedSound = trap_Cmd_RegisterGlobalSound ( "sound/ctf_return.mp3" );
	gametype.bombPlantedSound  = trap_Cmd_RegisterGlobalSound ( "sound/ctf_base.mp3" );

	// Register the items
	memset ( &itemDef, 0, sizeof(itemDef) );
	trap_Cmd_RegisterItem ( ITEM_BRIEFCASE,  "briefcase", &itemDef );
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
		
		case GTEV_RESET_BOMB:
			//trap_Cmd_ResetItem ( ITEM_BOMB );
			gametype.roundOver = qfalse;
			break;

		case GTEV_ITEM_DEFEND:
			//if(gametype.roundOver)
			//{
			//	return 0;
			//}
			if ( !gt_simpleScoring.integer )
			{
				trap_Cmd_AddClientScore ( arg1, 5 );
			}
			return 0;
		
		case GTEV_ITEM_DROPPED:
		{
			char clientname[MAX_QPATH];
			//if(gametype.roundOver)
			//{
			//	return 0;
			//}
			trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
			if(arg2 == TEAM_BLUE)
			{
				trap_Cmd_TextMessage ( -1, va("^$V^TI^oP^7 %s ^7was ^yk^li^ll^+l^7ed", clientname) );
				trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
				trap_Cmd_RadioMessage ( -888, va("%i",TEAM_RED) );
				trap_Cmd_RadioMessage ( -333, va("%i", TEAM_BLUE) );
			}
			else	
			{
				trap_Cmd_TextMessage ( -1, va("^iV^yI^fP^7 %s ^7was ^1k^Ti^ol^ql^+e^7d", clientname) );
				trap_Cmd_AddTeamScore ( TEAM_BLUE, 1 );
				trap_Cmd_RadioMessage ( -888, va("%i",TEAM_BLUE) );
				trap_Cmd_RadioMessage ( -333, va("%i", TEAM_RED) );
			}
			trap_Cmd_Restart ( 5 );

			trap_Cmd_StartGlobalSound ( gametype.bombPlantedSound );
			gametype.roundOver = qtrue;
			break;
		}
		
		/*
		case GTEV_ITEM_DROPPED:
		{
			switch(arg2)
			{
				case TEAM_BLUE:
				{
					if(arg3 != arg4 && arg3 >= 0)
					{		
						char vip[MAX_QPATH];
						char killer[MAX_QPATH];
						trap_Cmd_GetClientName ( arg1, vip, MAX_QPATH );
						trap_Cmd_GetClientName ( arg2, killer, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("^$V^TI^oP^7 %s ^yk^li^ll^+l^7ed by ^3%s!", vip,  killer) );
					}
					else
					{		
						char vip[MAX_QPATH];		
						trap_Cmd_GetClientName ( arg1, vip, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("^$V^TI^oP^7 %s ^7is ^yD^lE^lA^+D^7!", vip) );
					}
					trap_Cmd_RadioMessage ( -333, va("%i", TEAM_BLUE) );

					//trap_Cmd_TextMessage ( -1, va("^$V^TI^oP^7 %s ^7was ^yk^li^ll^+l^7ed", clientname) );
					trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
					trap_Cmd_RadioMessage ( -888, va("%i",TEAM_RED) );
				}	
				break;

				case TEAM_RED:
				{
					if(arg3 != arg4 && arg3 >= 0)
					{		
						char vip[MAX_QPATH];
						char killer[MAX_QPATH];
						trap_Cmd_GetClientName ( arg1, vip, MAX_QPATH );
						trap_Cmd_GetClientName ( arg2, killer, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("^$V^TI^oP^7 %s ^yk^li^ll^+l^7ed by ^3%s!", vip,  killer) );
					}
					else
					{		
						char vip[MAX_QPATH];		
						trap_Cmd_GetClientName ( arg1, vip, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("^$V^TI^oP^7 %s ^7is ^yD^lE^lA^+D^7!", vip) );
					}
					trap_Cmd_AddTeamScore ( TEAM_BLUE, 1 );
					trap_Cmd_RadioMessage ( -888, va("%i",TEAM_BLUE) );
					trap_Cmd_RadioMessage ( -333, va("%i", TEAM_RED) );
				}	
				break;
			}
			trap_Cmd_Restart ( 5 );
			//trap_Cmd_StartGlobalSound ( gametype.bombPlantedSound );
			gametype.roundOver = qtrue;
			break;
		}
		*/
		case GTEV_CLIENT_DEATH:
			//if(gametype.roundOver)
			//{
			//	return 0;
			//}
			//switch ( arg1 )
			//{
			//	case TEAM_RED:
			//		if(arg0 != arg2 && arg2 >= 0)
			//		{		
			//			char vip[MAX_QPATH];
			//			char killer[MAX_QPATH];
			//			trap_Cmd_GetClientName ( arg0, vip, MAX_QPATH );
			//			trap_Cmd_GetClientName ( arg2, killer, MAX_QPATH );
			//			trap_Cmd_TextMessage ( -1, va("^$V^TI^oP^7 %s ^yk^li^ll^+l^7ed by ^3%s!", vip,  killer) );
			//		}
			//		else
			//		{		
			//			char vip[MAX_QPATH];		
			//			trap_Cmd_GetClientName ( arg0, vip, MAX_QPATH );
			//			trap_Cmd_TextMessage ( -1, va("^$V^TI^oP^7 %s ^7is ^yD^lE^lA^+D^7!", vip) );
			//		}
			//		//trap_Cmd_AddTeamScore ( TEAM_BLUE, 1 );
			//		//trap_Cmd_RadioMessage ( -888, va("%i",TEAM_BLUE) );
			//		//trap_Cmd_Restart ( 5 );
			//		//trap_Cmd_StartGlobalSound ( gametype.bombPlantedSound );
			//		gametype.roundOver = qtrue;
			//		break;

			//	case TEAM_BLUE:
			//		if(arg0 != arg2 && arg2 >= 0)
			//		{		
			//			char vip[MAX_QPATH];
			//			char killer[MAX_QPATH];
			//			trap_Cmd_GetClientName ( arg0, vip, MAX_QPATH );
			//			trap_Cmd_GetClientName ( arg2, killer, MAX_QPATH );
			//			trap_Cmd_TextMessage ( -1, va("^iV^yI^fP^7 %s ^1k^Ti^ol^ql^+e^7d by ^3%s!", vip,  killer) );
			//		}
			//		else
			//		{	
			//			char vip[MAX_QPATH];	
			//			trap_Cmd_GetClientName ( arg0, vip, MAX_QPATH );
			//			trap_Cmd_TextMessage ( -1, va("^iV^yI^fP^7 %s ^7is ^1D^TE^oA^qD^7!", vip) );
			//		}
			//		//trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
			//		//trap_Cmd_RadioMessage ( -888, va("%i",TEAM_RED) );
			//		//trap_Cmd_Restart ( 5 );
			//		//trap_Cmd_StartGlobalSound ( gametype.bombPlantedSound );
			//		gametype.roundOver = qtrue;
			//		break;
			//}
			break;

		case GTEV_TIME_EXPIRED:
			//if(gametype.roundOver)
			//{
			//	return 0;
			//}
			trap_Cmd_Restart ( 5 );
			trap_Cmd_TextMessage ( -1, va("VIP's are SAFE!") );
			gametype.roundOver = qtrue;
			break;
	}

	return 0;
}

