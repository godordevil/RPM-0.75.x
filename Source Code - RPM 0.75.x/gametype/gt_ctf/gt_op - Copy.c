// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

#define	ITEM_REDFLAG			100
#define ITEM_BLUEFLAG			101
//#define ITEM_FLAG				102
								
#define TRIGGER_REDCAPTURE		200
#define TRIGGER_BLUECAPTURE		201

void	GT_UpdateCvars   ( void );
void	GT_RegisterCvars ( void );

extern vmCvar_t	gt_flagReturnTime;
extern vmCvar_t	gt_simpleScoring;
extern vmCvar_t	gt_customType;

static qboolean flagNeutral = qtrue;
static qboolean started = qfalse;
static team_t flagOwner = TEAM_FREE;

void GT_Init_OP ( void )
{
	gtItemDef_t		itemDef;
	gtTriggerDef_t	triggerDef;

	memset ( &gametype, 0, sizeof(gametype) );

	// Register the global sounds
	gametype.flagReturnSound  = trap_Cmd_RegisterGlobalSound ( "sound/ctf_return.mp3" );
	gametype.flagTakenSound   = trap_Cmd_RegisterGlobalSound ( "sound/ctf_flag.mp3" );
	gametype.flagCaptureSound = trap_Cmd_RegisterGlobalSound ( "sound/ctf_win.mp3" );

	// Register all cvars for this gametype
	//GT_RegisterCvars ( );

	// Register the items
	memset ( &itemDef, 0, sizeof(itemDef) );
	trap_Cmd_RegisterItem ( ITEM_REDFLAG,	"red_flag", &itemDef );
	trap_Cmd_RegisterItem ( ITEM_BLUEFLAG,  "blue_flag", &itemDef );
	//trap_Cmd_RegisterItem ( ITEM_FLAG,		"briefcase", &itemDef );

	// Register the triggers
	memset ( &triggerDef, 0, sizeof(triggerDef) );
	trap_Cmd_RegisterTrigger ( TRIGGER_REDCAPTURE, "red_capture_point", &triggerDef );
	trap_Cmd_RegisterTrigger ( TRIGGER_BLUECAPTURE, "blue_capture_point", &triggerDef );
}


void GT_RunFrame_OP ( int time )
{
	gametype.time = time;

	if(!gametype.paused && gametype.blueFlagDropTime && gametype.blueFlagDropTime < gametype.time)
	{
		gametype.blueFlagDropTime = gametype.time + 5000;
		trap_Cmd_AddTeamScore ( TEAM_BLUE, 2 );
	}
	else if(!gametype.paused && gametype.redFlagDropTime && gametype.redFlagDropTime < gametype.time)
	{
		gametype.redFlagDropTime = gametype.time + 5000;
		trap_Cmd_AddTeamScore ( TEAM_RED, 2 );
	}

	GT_UpdateCvars ( );
}

int GT_Event_OP ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
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

		case GTEV_ITEM_STUCK:
			switch ( arg0 )
			{
				case ITEM_BLUEFLAG:
					trap_Cmd_ResetItem ( ITEM_BLUEFLAG );
					trap_Cmd_TextMessage ( -1, "^7The ^3Flag ^7has returned!" );
					trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
					gametype.blueFlagDropTime = 0;
					gametype.redFlagDropTime = 0;
					flagOwner = TEAM_FREE;
					return 1;
				case ITEM_REDFLAG:
					trap_Cmd_ResetItem ( ITEM_REDFLAG );
					trap_Cmd_TextMessage ( -1, "^7The ^3Flag ^7has returned!" );
					trap_Cmd_StartGlobalSound ( gametype.flagReturnSound );
					gametype.blueFlagDropTime = 0;
					gametype.redFlagDropTime = 0;
					flagOwner = TEAM_FREE;
					return 1;
			}
			
			break;

		case GTEV_ITEM_DROPPED:
		{
			char clientname[MAX_QPATH];

			trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );

			if ( arg0 == ITEM_BLUEFLAG || arg0 == ITEM_REDFLAG)
			{
				if(arg1 == TEAM_BLUE)
					trap_Cmd_TextMessage ( -1, va("%s ^1has dropped the Flag!", clientname ) );
				else
					trap_Cmd_TextMessage ( -1, va("%s ^4has dropped the Flag!", clientname ) );
				gametype.blueFlagDropTime = 0;
				gametype.redFlagDropTime = 0;
			//	flagNeutral = qtrue;
				flagOwner = TEAM_FREE;
				return 1;
			}
			return 0;
		}
		
		case GTEV_ITEM_TOUCHED:

			switch ( arg0 )
			{
				case ITEM_BLUEFLAG:
					if ( arg2 == TEAM_RED && flagOwner != TEAM_RED )
					{
						//if((gametype.blueFlagDropTime) || (!gametype.blueFlagDropTime && !gametype.redFlagDropTime))
						{
							char clientname[MAX_QPATH];
							trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
							trap_Cmd_TextMessage ( -1, va("%s ^$has taken the Flag!", clientname ) );
							trap_Cmd_StartGlobalSound ( gametype.flagTakenSound );
							trap_Cmd_RadioMessage ( arg1, "got_it" );
							gametype.blueFlagDropTime = 0;
							gametype.redFlagDropTime = 0;
							flagOwner = TEAM_FREE;
							//flagNeutral = qfalse;
							return 1;
						}
					}
					break;

				case ITEM_REDFLAG:		
					if ( arg2 == TEAM_BLUE && flagOwner != TEAM_BLUE  )
					{
						//if((gametype.redFlagDropTime) || (!gametype.blueFlagDropTime && !gametype.redFlagDropTime))
						{
							char clientname[MAX_QPATH];
							trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
							trap_Cmd_TextMessage ( -1, va("%s ^4has taken the Flag!", clientname ) );
							trap_Cmd_StartGlobalSound ( gametype.flagTakenSound );
							trap_Cmd_RadioMessage ( arg1, "got_it" );
							gametype.blueFlagDropTime = 0;
							gametype.redFlagDropTime = 0;
							//flagNeutral = qfalse;
							return 1;
						}
					}
					break;
			}

			return 0;

		case GTEV_TRIGGER_TOUCHED:
			switch ( arg0 )
			{
				case TRIGGER_BLUECAPTURE:
					if ( trap_Cmd_DoesClientHaveItem ( arg1, ITEM_REDFLAG )/* || (!flagOwner && arg2 == TEAM_BLUE && trap_Cmd_DoesClientHaveItem ( arg1, ITEM_BLUEFLAG ))*/ )
					//if ( arg2 == TEAM_BLUE && trap_Cmd_DoesClientHaveItem ( arg1, ITEM_BLUEFLAG ) )
					{
						char clientname[MAX_QPATH];
						trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("%s ^4has set the Red Flag!", clientname ) );
						trap_Cmd_ResetItem ( ITEM_REDFLAG );
						trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
						trap_Cmd_AddTeamScore ( arg2, 1 );
						trap_Cmd_RadioMessage ( -777, va("%i", arg1) );
						//flagNeutral = qfalse;
						//started = qtrue;
						flagOwner = TEAM_BLUE;
						if ( !gt_simpleScoring.integer )
						{
							trap_Cmd_AddClientScore ( arg1, 10 );
						}

						gametype.blueFlagDropTime = 1;
						gametype.redFlagDropTime = 0;
						trap_Cmd_GetClientName ( ITEM_REDFLAG, clientname, ITEM_BLUEFLAG );
						//trap_Cmd_GetClientName ( arg1, clientname, flagOwner+MAX_QPATH );
						return 1;
					}
					break;

				case TRIGGER_REDCAPTURE:		
					if ( trap_Cmd_DoesClientHaveItem ( arg1, ITEM_BLUEFLAG ) /*|| (!flagOwner && arg2 == TEAM_BLUE && trap_Cmd_DoesClientHaveItem ( arg1, ITEM_REDFLAG ))*/ )
					//if ( arg2 == TEAM_RED && trap_Cmd_DoesClientHaveItem ( arg1, ITEM_BLUEFLAG ) )
					{
						char clientname[MAX_QPATH];
						trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
						trap_Cmd_TextMessage ( -1, va("%s ^$has set the Blue Flag!", clientname ) );
						trap_Cmd_ResetItem ( ITEM_BLUEFLAG );
						trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
						trap_Cmd_AddTeamScore ( arg2, 1 );
						trap_Cmd_RadioMessage ( -777, va("%i", arg1) );
						//flagNeutral = qfalse;
						//started = qtrue;
						flagOwner = TEAM_RED;

						if ( !gt_simpleScoring.integer )
						{
							trap_Cmd_AddClientScore ( arg1, 10 );
						}

						gametype.blueFlagDropTime = 0;
						gametype.redFlagDropTime = 1;
						trap_Cmd_GetClientName ( ITEM_BLUEFLAG, clientname, ITEM_REDFLAG );
						//trap_Cmd_GetClientName ( arg1, clientname, flagOwner+MAX_QPATH );
						return 1;
					}
					break;
			}


		/*
		case GTEV_ITEM_TOUCHED:

			if ( (arg0 == ITEM_BLUEFLAG || arg0 == ITEM_REDFLAG) && arg2 == TEAM_RED || arg2 == TEAM_BLUE)
			{
				char clientname[MAX_QPATH];
				trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
				if(arg1 == TEAM_BLUE)
					trap_Cmd_TextMessage ( -1, va("^3%s ^4has taken the Flag!", clientname ) );
				else
					trap_Cmd_TextMessage ( -1, va("^3%s ^1has taken the Flag!", clientname ) );

				trap_Cmd_StartGlobalSound ( gametype.flagTakenSound );
				trap_Cmd_RadioMessage ( arg1, "got_it" );
				gametype.blueFlagDropTime = 0;
				gametype.redFlagDropTime = 0;			
				return 1;
			}
			return 0;
			
		case GTEV_TRIGGER_TOUCHED:
				//if((arg0 == TRIGGER_REDCAPTURE && gametype.redFlagDropTime && arg2 == TEAM_BLUE) && !trap_Cmd_DoesClientHaveItem ( arg1, ITEM_FLAG ))
				//{
				//	trap_Cmd_ResetItem ( ITEM_FLAG );
				//	return 1;
				//}	
				//
				//if((arg0 == TRIGGER_BLUECAPTURE && gametype.blueFlagDropTime && arg2 == TEAM_RED) && !trap_Cmd_DoesClientHaveItem ( arg1, ITEM_FLAG ))
				//{
				//	trap_Cmd_ResetItem ( ITEM_FLAG );
				//	return 1;
				//}
	
				if ( ((arg0 == TRIGGER_REDCAPTURE && arg2 == TEAM_RED)) && (trap_Cmd_DoesClientHaveItem ( arg1, ITEM_REDFLAG ) || trap_Cmd_DoesClientHaveItem ( arg1, ITEM_BLUEFLAG )) )
				{
					char clientname[MAX_QPATH];
					trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
						
					trap_Cmd_TextMessage ( -1, va("^3%s ^7has set the ^1Flag ^7for the ^1RED TEAM^7!", clientname) );

					trap_Cmd_ResetItem ( ITEM_FLAG );
					trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
					//trap_Cmd_AddTeamScore ( arg2, 1 );
					trap_Cmd_RadioMessage ( -777, va("%i", arg1) );
					//trap_Cmd_RadioMessage ( -222, va("%i", arg2) );
					//trap_Cmd_GetClientName ( -222, va("%i", ITEM_REDFLAG), arg2 );
					gametype.blueFlagDropTime = 0;
					gametype.redFlagDropTime = 1;

					if ( !gt_simpleScoring.integer )
					{
						trap_Cmd_AddClientScore ( arg1, 10 );
					}
					return 1;
				}
				if ( ((arg0 == TRIGGER_BLUECAPTURE && arg2 == TEAM_BLUE)) && (trap_Cmd_DoesClientHaveItem ( arg1, ITEM_REDFLAG ) || trap_Cmd_DoesClientHaveItem ( arg1, ITEM_BLUEFLAG )) )
				{
					char clientname[MAX_QPATH];
					trap_Cmd_GetClientName ( arg1, clientname, MAX_QPATH );
					trap_Cmd_TextMessage ( -1, va("^3%s ^7has set the ^4Flag ^7for the ^4BLUE TEAM^7!", clientname) );
					
					trap_Cmd_ResetItem ( ITEM_FLAG );
					trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
					//trap_Cmd_AddTeamScore ( arg2, 1 );
					trap_Cmd_RadioMessage ( -777, va("%i", arg1) );
					//trap_Cmd_RadioMessage ( -222, va("%i", arg2) );
					//trap_Cmd_GetClientName ( -222, va("%i", ITEM_BLUEFLAG), arg2 );
					gametype.blueFlagDropTime = 1;
					gametype.redFlagDropTime = 0;

					if ( !gt_simpleScoring.integer )
					{
						trap_Cmd_AddClientScore ( arg1, 10 );
					}
					return 1;
				}

			return 0;
			*/
	}

	return 0;
}
