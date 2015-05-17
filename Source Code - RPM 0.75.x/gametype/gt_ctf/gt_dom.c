// Copyright (C) 2001-2002 Raven Software.
//

#include "gt_local.h"

#define	ITEM_REDFLAG			100
#define ITEM_BLUEFLAG			101
								
#define TRIGGER_REDCAPTURE		200
#define TRIGGER_BLUECAPTURE		201

void	GT_UpdateCvars   ( void );
void	GT_RegisterCvars ( void );

extern vmCvar_t	gt_flagReturnTime;
extern vmCvar_t	gt_simpleScoring;


void GT_Init_DOM ( void )
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
	trap_Cmd_RegisterItem ( ITEM_REDFLAG,  "red_flag", &itemDef );
	trap_Cmd_RegisterItem ( ITEM_BLUEFLAG, "blue_flag", &itemDef );

	// Register the triggers
	memset ( &triggerDef, 0, sizeof(triggerDef) );
	trap_Cmd_RegisterTrigger ( TRIGGER_REDCAPTURE, "red_capture_point", &triggerDef );
	trap_Cmd_RegisterTrigger ( TRIGGER_BLUECAPTURE, "blue_capture_point", &triggerDef );
}

void GT_RunFrame_DOM ( int time )
{
	gametype.time = time;
	GT_UpdateCvars ( );
}

int GT_Event_DOM ( int cmd, int time, int arg0, int arg1, int arg2, int arg3, int arg4 )
{		
	switch ( cmd )
	{
		case GTEV_ITEM_DEFEND:
			if ( !gt_simpleScoring.integer )
			{
				trap_Cmd_AddClientScore ( arg1, 5 );
			}
			return 0;
		case GTEV_TRIGGER_TOUCHED:
				//trap_Cmd_StartGlobalSound ( gametype.flagTakenSound );
				//if(time > sound[arg2]) ///dont spam sounds
				//{
				switch(rand() % 3)
				{
				case 1:
					trap_Cmd_RadioMessage ( arg1, "hold_position" );
					break;
				case 2:
					trap_Cmd_RadioMessage ( arg1, "cover_me" );
					break;
				default:
				case 3:
					trap_Cmd_RadioMessage ( arg1, "need_backup" );
					break;
				//}
				//sound[arg2] = time + 4000;
			}
			return 0;

		case GTEV_ITEM_CANBEUSED:
			if ( arg2 != arg3 )
			{
				return 1;
			}
			return 0;

		case GTEV_TEAM_ELIMINATED:
			switch ( arg2 )
			{
				case TEAM_BLUE:
					//if(arg3){
					//	trap_Cmd_TextMessage ( -1, va("The ^4BLUE ^7Team is ^3DOMINATING ^7the ^$RED ^7Team!") );
					//	//trap_Cmd_RadioMessage ( arg1, "obj_compelete" );
					//}
					//trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
					//trap_Cmd_AddTeamScore ( arg2, 1 );
					//if ( !gt_simpleScoring.integer )
					//	trap_Cmd_AddClientScore ( arg1, 10 );
					return 1;	
				case TEAM_RED:
					//if(arg3){
					//	trap_Cmd_TextMessage ( -1, va("The ^$RED ^7Team is ^3DOMINATING ^7the ^4BLUE ^7Team!" ));
					//	//trap_Cmd_RadioMessage ( arg1, "obj_compelete" );
					//}
					//trap_Cmd_StartGlobalSound ( gametype.flagCaptureSound );
					//trap_Cmd_AddTeamScore ( arg2, 1 );
					//if ( !gt_simpleScoring.integer )
					//	trap_Cmd_AddClientScore ( arg1, 10 );
					return 1;
				default:
					break;
			}
			return 0;
	}

	return 0;
}
