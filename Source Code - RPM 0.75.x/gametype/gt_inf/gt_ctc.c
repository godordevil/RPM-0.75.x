// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

#define	ITEM_BRIEFCASE			100								
//#define TRIGGER_EXTRACTION_BLUE	200
//#define TRIGGER_EXTRACTION_RED	201
#define TRIGGER_EXTRACTION	200

void	GT_UpdateCvars  ( void );
void	GT_RegisterCvars	( void );

extern  vmCvar_t		gt_simpleScoring;


void GT_Init_CTC ( void )
{
	gtItemDef_t		itemDef;
	gtTriggerDef_t	triggerDef;

	memset ( &gametype, 0, sizeof(gametype) );

	//// Register all cvars for this gametype
	//GT_RegisterCvars ( );

	// Register the global sounds
	gametype.caseTakenSound   = trap_Cmd_RegisterGlobalSound ( "sound/ctf_flag.mp3" );
	gametype.caseCaptureSound = trap_Cmd_RegisterGlobalSound ( "sound/ctf_win.mp3" );
	//gametype.caseReturnSound  = trap_Cmd_RegisterGlobalSound ( "sound/ctf_return.mp3" );
	gametype.caseReturnSound	= trap_Cmd_RegisterGlobalSound ( "sound/ctf_base.mp3" );

	// Register the items
	memset ( &itemDef, 0, sizeof(itemDef) );
	trap_Cmd_RegisterItem ( ITEM_BRIEFCASE,  "briefcase", &itemDef );
	
	// Register the triggers
	memset ( &triggerDef, 0, sizeof(triggerDef) );
	trap_Cmd_RegisterTrigger ( TRIGGER_EXTRACTION, "briefcase_destination", &triggerDef );

	// Register the triggers
	//memset ( &triggerDef, 0, sizeof(triggerDef) );
	//trap_Cmd_RegisterTrigger ( TRIGGER_EXTRACTION_RED, "briefcase_destination_red", &triggerDef );
	//trap_Cmd_RegisterTrigger ( TRIGGER_EXTRACTION_BLUE, "briefcase_destination_blue", &triggerDef );
}

void GT_RunFrame_CTC ( int time )
{		
	gametype.time = time;
	GT_UpdateCvars ( );
}


int GT_Event_CTC ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
{
	switch ( cmd )
	{
		case GTEV_ITEM_DEFEND:
			if ( !gt_simpleScoring.integer )
			{
				trap_Cmd_AddClientScore ( arg1, 5 );
			}
			return 0;

		case GTEV_ITEM_STUCK:
			trap_Cmd_ResetItem ( ITEM_BRIEFCASE );
			trap_Cmd_TextMessage ( -1, "^7The ^3Briefcase ^7has returned!" );
			trap_Cmd_StartGlobalSound ( gametype.caseReturnSound );
			return 1;			
/*	
		case GTEV_ITEM_DROPPED:
		{
			char clientname[MAX_QPATH];
			trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
			trap_Cmd_TextMessage ( -1, va("%s has dropped the briefcase!", clientname ) );
			break;
		}
*/
		case GTEV_ITEM_DROPPED:
			switch ( arg2 )
			{
			case TEAM_RED:
				{				
					char clientname[MAX_QPATH];
					trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
					trap_Cmd_TextMessage ( -1, va("^1%s ^1has dropped the briefcase!", clientname ) );
					trap_Cmd_StartGlobalSound ( gametype.caseReturnSound );
					break;
				}	
			case TEAM_BLUE:
				{				
					char clientname[MAX_QPATH];
					trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
					trap_Cmd_TextMessage ( -1, va("^4%s ^4has dropped the briefcase!", clientname ) );
					trap_Cmd_StartGlobalSound ( gametype.caseReturnSound );
					break;
				}
			default:
				break;

			}
			break;

		case GTEV_TEAM_ELIMINATED:
			switch ( arg0 )
			{		

				case TEAM_RED:
					trap_Cmd_TextMessage ( -1, "Red team eliminated!" );
					trap_Cmd_AddTeamScore ( TEAM_BLUE, 1 );
					trap_Cmd_RadioMessage ( -888, va("%i", TEAM_BLUE ));
					trap_Cmd_Restart ( 5 );
					break;

				case TEAM_BLUE:
					trap_Cmd_TextMessage ( -1, "Blue team eliminated!" );
					trap_Cmd_AddTeamScore ( TEAM_RED, 1 );
					trap_Cmd_RadioMessage ( -888, va("%i",TEAM_RED) );
					trap_Cmd_Restart ( 5 );
					break;
				default:
					break;
			}
			break;

		case GTEV_TIME_EXPIRED:
			trap_Cmd_TextMessage ( -1, "Time Expired!" );
			trap_Cmd_Restart ( 5 );
			break;

		case GTEV_ITEM_TOUCHED:

			switch ( arg0 )
			{
				case ITEM_BRIEFCASE:
					switch(arg2)
					{
					case TEAM_RED:
					{
						char clientname[MAX_QPATH];
						trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("^1%s ^1has taken the briefcase!", clientname ) );
						trap_Cmd_StartGlobalSound ( gametype.caseTakenSound );
						trap_Cmd_RadioMessage ( arg1, "got_it" );
						return 1;
					}
					case TEAM_BLUE:
					{
						char clientname[MAX_QPATH];
						trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("^4%s ^4has taken the briefcase!", clientname ) );
						trap_Cmd_StartGlobalSound ( gametype.caseTakenSound );
						trap_Cmd_RadioMessage ( arg1, "got_it" );
						return 1;
					}
					default:
						break;
					}
			}

			return 0;

		case GTEV_TRIGGER_TOUCHED:
			switch ( arg0 )
			{
			case TRIGGER_EXTRACTION:
				if ( trap_Cmd_DoesClientHaveItem ( arg1, ITEM_BRIEFCASE ) )
				{
					char clientname[MAX_QPATH];
					trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
					trap_Cmd_TextMessage ( -1, va("^3%s ^7has ^3captured ^7the ^3briefcase!", clientname ) );
					trap_Cmd_StartGlobalSound ( gametype.caseCaptureSound );
					trap_Cmd_RadioMessage ( -777, va("%i", arg1) );
					trap_Cmd_AddTeamScore ( arg2, 1 );
					trap_Cmd_RadioMessage ( -888, va("%i",arg2) );

					if ( !gt_simpleScoring.integer )
					{
						trap_Cmd_AddClientScore ( arg1, 10 );
					}

					trap_Cmd_Restart ( 5 );
				}
				break;			

			}

			return 0;
	}

	return 0;
}


