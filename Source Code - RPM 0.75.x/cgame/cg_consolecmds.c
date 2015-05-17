// Copyright (C) 2001-2002 Raven Software.
// Copyright (C) 2004-2005 Ryan Powell

//  This file is part of RPM.

//  RPM is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation;either version 2 of the License, or
//  (at your option) any later version.

//  RPM is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with RPM; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// cg_consolecmds.c -- text commands typed in at the local console, or
// executed by a key binding

#include "cg_local.h"
#include "../ui/ui_shared.h"
#include "../ui/ui_public.h"

qboolean adm_wh;

//Ryan march 7 2004 1:59pm
/*
===============
RPM_Music
===============
*/
void RPM_Music(void)
{
	int				i, numMP3Files, filelen;
	const char		*song;
	char			MP3Files[4096];
	char			fileName[MAX_QPATH];
	char			*fileptr;
	//char			musicFolder[MAX_INFO_STRING];

	song = CG_Argv(1);

	if(!Q_stricmp( song, "SOF2 Game Music"))
	{
		//do both so the song doesnt start when the map reloads but do not have to wait till
		//the engine updates the client cvar
		cg_currentSong.string[0] = 0;
		trap_Cvar_Set("cg_currentSong", "");

		CG_StartMusic( qtrue );
		Com_Printf("^3Playing Default SOF2 Game Music\n");
		return;
	}

	//trap_Cvar_VariableStringBuffer ( "ui_musicFolder", musicFolder, MAX_INFO_STRING );

	numMP3Files = trap_FS_GetFileList(/*musicFolder*/"RPM_Music", ".mp3", MP3Files, 4096 );

	fileptr = MP3Files;

	for( i=0; i<numMP3Files; i++, fileptr += filelen+1 )
	{
		// Grab the length so we can skip this file later
		filelen = strlen(fileptr);
		//cut the ".mp3" off the end for a positve compare result
		fileptr[filelen - 4] = '\0';

		if(!Q_stricmp( fileptr, song))
		{
			Com_sprintf(fileName, sizeof(fileName),"RPM_Music/%s", song );
			trap_S_StartBackgroundTrack( fileName, fileName, qfalse );
			trap_Cvar_Set("cg_currentSong", fileName);
			Com_Printf("^3Playing %s\n", song);
			return;
		}
	}

	Com_Printf("^1File not found, %s\n", song);
	cg_currentSong.string[0] = 0;
	trap_Cvar_Set("cg_currentSong", "");
}

//Ryan

/*
=============
CG_Viewpos_f

Debugging command to print the current position
=============
*/
static void CG_Viewpos_f (void) 
{
	Com_Printf ("(%i %i %i) : %i\n", (int)cg.refdef.vieworg[0],
		(int)cg.refdef.vieworg[1], (int)cg.refdef.vieworg[2], 
		(int)cg.refdef.viewangles[YAW]);
}

/*
=============
CG_ScoresDown_f
=============
*/
static void CG_ScoresDown_f( void ) 
{
	// GodOrDevil
	//if ( cg.scoresRequestTime + 2000 < cg.time ) 
	if ( cg.scoresRequestTime + 2000 < cg.time && !cg.demoPlayback) 
	// END
	{
		// the scores are more than two seconds out of data,
		// so request new ones
		cg.scoresRequestTime = cg.time;
		trap_SendClientCommand( "score" );

		// leave the current scores up if they were already
		// displayed, but if this is the first hit, clear them out
		if ( !cg.showScores ) 
		{
			cg.showScores = qtrue;
			cg.numScores = 0;
		}
	} 
	else 
	{
		// show the cached contents even if they just pressed if it
		// is within two seconds
		cg.showScores = qtrue;
	}
}

/*
=============
CG_ScoresUp_f
=============
*/
static void CG_ScoresUp_f( void ) 
{
	if ( cg.showScores ) 
	{
		cg.showScores = qfalse;
		cg.scoreFadeTime = cg.time;
	}
}

/*
=============
CG_AutomapDown_f
=============
*/
static void CG_AutomapDown_f( void ) 
{
	cg.showAutomap = qtrue;
}

/*
=============
CG_AutomapUp_f
=============
*/
static void CG_AutomapUp_f( void ) 
{
	cg.showAutomap = qfalse;
}

/*
=============
CG_ReloadHud_f
=============
*/
static void CG_ReloadHud_f ( void )
{
	// Reset the string table used for menus  
	String_Init();

	// Clear all menus
	Menu_Reset();
	
	// Reload the menus

	//Ryan
	if(cg_rpmHud.integer)
	{
		trap_Cvar_Set("cg_hudFiles", "ui/rpm_hud.txt");
		CG_LoadMenus("ui/rpm_hud.txt");
	}
	else
	{
		trap_Cvar_Set("cg_hudFiles", "ui/hud.txt");
		CG_LoadMenus ( "ui/hud.txt" );
	}

	//CG_LoadMenus ( "ui/hud.txt" );
	//Ryan
}

/*
=============
CG_Radio_f

Bring up the radio menu if all the conditions are met
=============
*/
static void CG_Radio_f ( void )
{
	// Only in team games
	if ( !cgs.gametypeData->teams )
	{
		return;
	}

	// Not when ghosting or following
	if ( cg.snap->ps.pm_flags & (PMF_FOLLOW|PMF_GHOST) )
	{
		return;
	}

	// Not when a spectator
	if ( cg.snap->ps.pm_type == PM_SPECTATOR )
	{
		return;
	}

	trap_UI_SetActiveMenu ( UIMENU_RADIO );
}

/*
=============
CG_Objectives_f

Bring up the objectives menu if all the conditions are met
=============
*/
static void CG_Objectives_f ( void )
{
	// Dont bother popping up the objectives dialog if there is
	// no objective text
	if ( !cgs.gametypeData->description )
	{
		return;
	}

	trap_UI_SetActiveMenu ( UIMENU_OBJECTIVES );
}

// GodOrDevil #mapEditor
void CG_AddChatText ( int client, const char *str ); 
void NV_mapEditorUI ( void )
{
	//if(nvc_mapEditor.integer)
		trap_UI_SetActiveMenu ( UIMENU_MAPEDITOR );
	//else
	//	CG_AddChatText ( 0, "Server must have ^3nv_editMap ^2enabled to use this command" );
}
// END

/*
=============
CG_Outfitting_f

Bring up the outfitting menu if all the conditions are met
=============
*/
static void CG_Outfitting_f ( void )
{
	// Only allow outfitting when pickups are disabled
	if ( !cgs.pickupsDisabled )
	{
		return;
	}

	trap_UI_SetActiveMenu ( UIMENU_OUTFITTING );
}

/*
=============
CG_Team_f

bring up the team selection user interface
=============
*/
static void CG_Team_f ( void )
{
	// No team menu in non-team games
	if ( !cgs.gametypeData->teams )
	{
		return;
	}

	// Special case which only brings up the team menu if its the clients first
	// time to the objectives dialog
	if ( atoi ( CG_Argv(1) ) )
	{
		if ( ui_info_seenobjectives.integer )
		{
			return;
		}
	}

	CG_UpdateTeamCountCvars ( );

	trap_Cvar_Set ( "ui_info_seenobjectives", "1" );
	trap_UI_SetActiveMenu ( UIMENU_TEAM );
}

/*
=============
CG_Drop_f

Drops the selected weapon
=============
*/
void CG_Drop_f ( void ) 
{
	char	cmd[128];
	int		exclude;

	// Cant drop when following or a ghost
	if ( cg.snap->ps.pm_flags & (PMF_FOLLOW|PMF_GHOST) )
	{
		return;
	}

	// Either dead or spectating if not normal
	if ( cg.predictedPlayerState.pm_type != PM_NORMAL )
	{
		return;
	}

	// Can only drop a weapon when in ready state
	if ( cg.predictedPlayerState.weaponstate != WEAPON_READY )
	{
		return;
	}

	// Cant drop the knife
	if( cg.weaponSelect == WP_KNIFE )
	{
		return;
	}

	// Close the menu since a weapon is being dropped
	cg.weaponMenuUp = qfalse;

	// Build the server command		
	Com_sprintf( cmd, 128, "drop %i", cg.weaponSelect );

	// Go to next weapon before the current drops
	exclude = cg.weaponSelect;
	cg.weaponSelect = WP_M67_GRENADE;
	CG_PrevWeapon ( qfalse, exclude );

	// Send server comand
	trap_SendClientCommand( cmd );
}

/*
=============
CG_GetOutfittingGroupFromString

Converts the given string into an outfitting group id
=============
*/
static int CG_GetOutfittingGroupFromString ( const char* str )
{
	if ( Q_stricmp ( str, "primary" ) == 0 )
	{
		return OUTFITTING_GROUP_PRIMARY;
	}
	else if ( Q_stricmp ( str, "secondary" ) == 0 )
	{
		return OUTFITTING_GROUP_SECONDARY;
	}
	else if ( Q_stricmp ( str, "pistol" ) == 0 )
	{
		return OUTFITTING_GROUP_PISTOL;
	}
	else if ( Q_stricmp ( str, "grenade" ) == 0 )
	{
		return OUTFITTING_GROUP_GRENADE;
	}
	else if ( Q_stricmp ( str, "knife" ) == 0 )
	{
		return OUTFITTING_GROUP_KNIFE;
	}

	return -1;
}

/*
=============
CG_WeaponToggle_f

toggles between multiple weapons
=============
*/
static void CG_WeaponToggle_f ( void )
{
	int			group1;
	int			group2;
	int			weapon1;
	int			weapon2;
	gitem_t*	item;
	int			i;

	// Get the toggle groups
	group1 = CG_GetOutfittingGroupFromString ( CG_Argv(1) );
	group2 = CG_GetOutfittingGroupFromString ( CG_Argv(2) );

	// Invalid toggle if either is -1
	if ( group1 == -1 ) // || group2 == -1 )
	{
		return;
	}

	// Make sure they have something from both of the groups
	weapon1 = WP_NONE;
	weapon2 = WP_NONE;
	for ( i = WP_KNIFE; i < WP_NUM_WEAPONS; i ++ )
	{	
		// Make sure this weapon is selectable.
		if ( !CG_WeaponSelectable ( i, qtrue ) )
		{
			continue;
		}

		item = BG_FindWeaponItem ( i );
		if ( item->outfittingGroup == group1 )
		{
			weapon1 = i;
		}
		else if ( item->outfittingGroup == group2 )
		{
			weapon2 = i;
		}
	}
	
	// IF only one of the two weapons is available then go to it
	if ( weapon1 == WP_NONE && weapon2 == WP_NONE )
	{
		return;
	}
	else if ( weapon1 == WP_NONE )
	{
		cg.weaponSelect = weapon2;
		return;
	}
	else if ( weapon2 == WP_NONE )
	{
		cg.weaponSelect = weapon1;
		return;
	}
		
	// They have both weapons, so figure out which to go to
	item = BG_FindWeaponItem ( cg.weaponSelect );

	if ( item->outfittingGroup == group1 )
	{
		cg.weaponSelect = weapon2;
	}
	else
	{
		cg.weaponSelect = weapon1;
	}
}

/*
=============
CG_NextWeapon_f

selects next weapon in inventory and allows empty weapons to
be selected
=============
*/

static void CG_NextWeapon_f ( void )
{
#ifdef _MAPEDITOR
	if ( (cg_zoomWeaponChange.integer && (cg.predictedPlayerState.pm_flags & PMF_ZOOMED)) || cg.predictedPlayerState.pm_flags & PMF_MAPEDITOR ){
		trap_SendConsoleCommand ( "+zoomin; wait; -zoomin;" );
		return;
	}
#else
	if ( (cg_zoomWeaponChange.integer && (cg.predictedPlayerState.pm_flags & PMF_ZOOMED)) ){
		trap_SendConsoleCommand ( "+zoomin; wait; -zoomin;" );
		return;
	}
#endif
	CG_NextWeapon ( qtrue, -1 );
}



/*
=============
CG_PrevWeapon_f

selects previous weapon in inventory and allows empty weapons
to be selectd
=============
*/
static void CG_PrevWeapon_f ( void )
{
#ifdef _MAPEDITOR
	if ( (cg_zoomWeaponChange.integer && (cg.predictedPlayerState.pm_flags & PMF_ZOOMED)) || cg.predictedPlayerState.pm_flags & PMF_MAPEDITOR ){
		trap_SendConsoleCommand ( "+zoomout; wait; -zoomout;" );
		return;
	}
#else
	if ( (cg_zoomWeaponChange.integer && (cg.predictedPlayerState.pm_flags & PMF_ZOOMED)) ){
		trap_SendConsoleCommand ( "+zoomout; wait; -zoomout;" );
		return;
	}
#endif

	CG_PrevWeapon ( qtrue, -1 );
}


/*
=============
CG_LastWeapon_f

Selects the last weapon that was selected
=============
*/
static void CG_LastWeapon_f ( void )
{
	if ( CG_WeaponSelectable ( cg.weaponLastSelect, qtrue ) )
	{
		int swap = cg.weaponSelect;
		cg.weaponSelect = cg.weaponLastSelect;
		cg.weaponLastSelect = swap;
	}
}


static void CG_TellTarget_f( void ) {
	int		clientNum;
	char	command[128];
	char	message[128];

	clientNum = CG_CrosshairPlayer();
	if ( clientNum == -1 ) {
		return;
	}

	trap_Args( message, 128 );
	Com_sprintf( command, 128, "tell %i %s", clientNum, message );
	trap_SendClientCommand( command );
}

static void CG_TellAttacker_f( void ) {
	int		clientNum;
	char	command[128];
	char	message[128];

	clientNum = CG_LastAttacker();
	if ( clientNum == -1 ) {
		return;
	}

	trap_Args( message, 128 );
	Com_sprintf( command, 128, "tell %i %s", clientNum, message );
	trap_SendClientCommand( command );
}

/*
==================
CG_StartOrbit_f
==================
*/

static void CG_StartOrbit_f( void ) {
	char var[MAX_TOKEN_CHARS];

	trap_Cvar_VariableStringBuffer( "developer", var, sizeof( var ) );
	if ( !atoi(var) ) {
		return;
	}
	if (cg_cameraOrbit.value != 0) {
		trap_Cvar_Set ("cg_cameraOrbit", "0");
		trap_Cvar_Set("cg_thirdPerson", "0");
	} else {
		trap_Cvar_Set("cg_cameraOrbit", "5");
		trap_Cvar_Set("cg_thirdPerson", "1");
		trap_Cvar_Set("cg_thirdPersonAngle", "0");
		trap_Cvar_Set("cg_thirdPersonRange", "100");
	}
}

// GodOrDevil
static void NV_adminWH (void)
{
	if(!cg.demoPlayback)
	{
		if(cgs.clientinfo[cg.clientNum].admin < 3 )
		{
			adm_wh = qfalse;
			return;
		}
	}
	if(adm_wh){
		adm_wh = qfalse;
		Com_Printf ( "^kAdmin Wallhack: ^1Disabled\n");
	}
	else {
		adm_wh = qtrue;
		Com_Printf ( "^kAdmin Wallhack: ^3Enabled\n");
	}
}
static void NV_toggleThird (void)
{
	if(cg_thirdPerson.integer == 0)
	{		
		trap_Cvar_Set("cg_thirdPerson", "1");
		cg.renderingThirdPerson = 1;
		Com_Printf ( "^3Third Person ^3ENABLED\n");
		return;
	}
	else
	{	
		trap_Cvar_Set("cg_thirdPerson", "0");
		cg.renderingThirdPerson = 0;
		Com_Printf ( "^3Third Person ^1DISABLED\n");
		return;
	}
}
static void NV_record (void)
{	
	trap_SendConsoleCommand( "g_synchronousClients 1; record; g_synchronousClients 0" );
}
//END

typedef struct 
{
	char	*cmd;
	void	(*function)(void);

} consoleCommand_t;

static consoleCommand_t	commands[] = 
{
	{ "testmodel",		CG_TestModel_f			},
	{ "nextframe",		CG_TestModelNextFrame_f },
	{ "prevframe",		CG_TestModelPrevFrame_f },
	{ "nextskin",		CG_TestModelNextSkin_f	},
	{ "prevskin",		CG_TestModelPrevSkin_f	},
	{ "viewpos",		CG_Viewpos_f			},
	{ "+scores",		CG_ScoresDown_f			},
	{ "-scores",		CG_ScoresUp_f			},
	{ "+automap",		CG_AutomapDown_f		},
	{ "-automap",		CG_AutomapUp_f			},
	{ "weapnext",		CG_NextWeapon_f			},
	{ "weapprev",		CG_PrevWeapon_f			},
	{ "weaplast",		CG_LastWeapon_f			},
	{ "weapon",			CG_Weapon_f				},
	{ "tell_target",	CG_TellTarget_f			},
	{ "tell_attacker",	CG_TellAttacker_f		},
	{ "reloadhud",		CG_ReloadHud_f			},
	{ "startOrbit",		CG_StartOrbit_f			},
	{ "loaddeferred",	CG_LoadDeferredPlayers	},
	{ "drop",			CG_Drop_f				},

	{ "weaptoggle",		CG_WeaponToggle_f		},

	{ "ui_radio",		CG_Radio_f },
	{ "ui_objectives",	CG_Objectives_f },
	{ "ui_outfitting",	CG_Outfitting_f },
	{ "ui_team",		CG_Team_f },

	//Ryan march 7 2004 1:56pm
	{ "song",			RPM_Music	},
	//Ryan
// GodOrDevil
	{ "wh",					NV_adminWH },
	{ "wallhack",			NV_adminWH },
	{ "ui_mapEditor",		NV_mapEditorUI },


#ifdef _DEMO
	{ "1st",				NV_toggleThird },
	{ "3rd",				NV_toggleThird },
	{ "rec",				NV_record },
#endif
// END
};

/*
=================
CG_ConsoleCommand

The string has been tokenized and can be retrieved with
Cmd_Argc() / Cmd_Argv()
=================
*/
qboolean CG_ConsoleCommand( void ) 
{
	const char	*cmd;
	int		i;

	// No console commands when a map is changing
	if ( cg.mMapChange )
	{
		return qfalse;
	}

	cmd = CG_Argv(0);

	for ( i = 0 ; i < sizeof( commands ) / sizeof( commands[0] ) ; i++ ) 
	{
		if ( !Q_stricmp( cmd, commands[i].cmd ) ) 
		{
			commands[i].function();
			return qtrue;
		}
	}

	return qfalse;
}

/*
=================
CG_InitConsoleCommands

Let the client system know about all of our commands
so it can perform tab completion
=================
*/
void CG_InitConsoleCommands( void ) 
{
	int		i;

	for ( i = 0 ; i < sizeof( commands ) / sizeof( commands[0] ) ; i++ ) 
	{
		trap_AddCommand( commands[i].cmd );
	}

	//
	// the game server will interpret these commands, which will be automatically
	// forwarded to the server after they are not recognized locally
	//
	trap_AddCommand ("kill");
	trap_AddCommand ("say");
	trap_AddCommand ("say_team");
	trap_AddCommand ("tell");
	trap_AddCommand ("vsay_team");
	trap_AddCommand ("give");
	trap_AddCommand ("god");
	trap_AddCommand ("notarget");
	trap_AddCommand ("noclip");
	trap_AddCommand ("team");
	trap_AddCommand ("follow");
	trap_AddCommand ("levelshot");
#ifdef _SOF2_BOTS
	trap_AddCommand ("addbot");
#endif
	trap_AddCommand ("setviewpos");
	trap_AddCommand ("callvote");
	trap_AddCommand ("vote");
	trap_AddCommand ("stats");
	trap_AddCommand ("teamtask");
	trap_AddCommand ("loaddeferred");
	trap_AddCommand ("gametype_restart");
	// GodOrDevil
	trap_AddCommand ("wh");
	// END
}
