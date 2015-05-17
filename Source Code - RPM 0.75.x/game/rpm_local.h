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

#ifndef _NV_ADMIN
void	RPM_Plant (int argNum, gentity_t *adm, qboolean plantsk);
void	RPM_PlantSK (int argNum, gentity_t *adm);
void SC_Burn			(int argNum, gentity_t *adm); 
void NV_Launch			(int argNum, gentity_t *adm);
void NV_ClipSize		(int argNum, gentity_t *adm);
void NV_ExtraClips		(int argNum, gentity_t *adm);
void NV_WeaponOnly		(int argNum, gentity_t *adm);
void NV_ForcedWeapons	(int argNum, gentity_t *adm);
void	Svcmd_Mute (int argNum, gentity_t *adm);
void	Svcmd_Strip (int argNum, gentity_t *adm);
void	Svcmd_Runover (int argNum, gentity_t *adm);
void	Svcmd_CancelVote_f ( void );
void	RPM_Even_Teams (gentity_t *adm, qboolean autoeven);
void	Svcmd_Add_sAdmin_f (int argNum, gentity_t *adm);
void	Svcmd_Add_bAdmin_f (int argNum, gentity_t *adm);
void	Svcmd_Add_Admin_f (int argNum, gentity_t *adm);
void	Svcmd_Add_No_Kick_f (int argNum, gentity_t *adm);
void	Svcmd_Remove_No_Kick_f (int argNum, gentity_t *adm);
void	Svcmd_admWarn_cmd (int argNum, gentity_t *adm);
void	RPM_unPlant (int argNum, gentity_t *adm);
void	SC_SwapTeams (gentity_t *adm);
void	RPM_Penalty(int argNum, gentity_t *adm);			//#Pbox
void	SC_Torture (int argNum, gentity_t *adm);			//#Torture
void	SC_unTorture (int argNum, gentity_t *adm);			//#unTorture
void	SC_Forcesay(int argNum, gentity_t *adm);			//#Forcesay
void	SC_Slam (int argNum, gentity_t *adm);				//#Drop
void	SC_Twist (int argNum, gentity_t *adm);				//#Twist
void	SC_unTwist (int argNum, gentity_t *adm);			//#unTwist
void	SC_Slow (int argNum, gentity_t *adm);				//#slow
void	SC_nextMap( gentity_t *adm );						//#mapNext
void	SC_subnetBan (int argNum, gentity_t *adm);
void	SC_Flash	  (int argNum, gentity_t *adm);
///void SC_playEffects(int argNum, gentity_t *adm);			//#playEffects
///void	SC_Sound ( gentity_t *adm);							//#Sound
///void	SC_TeamSound ( gentity_t *adm);						//#TeamSound
///void	Svcmd_Add_No_Kick_f (void);
///void	Svcmd_Remove_No_Kick_f (void);
void 	Svcmd_NameChange (int argNum, gentity_t *adm);
void	Svcmd_Remove_Admin_f (void);
void	Cmd_adm_f ( gentity_t *ent );
void	Svcmd_Ban_f (int argNum, gentity_t *adm);
void	Svcmd_pop (int argNum, gentity_t *adm);
void	RPM_Unban(gentity_t *adm, char *ip, qboolean subnet);

#endif

void	SC_SwapTeams (gentity_t *adm);
int		RPM_NameListCheck (int num, const char *name, const char *file, gentity_t *ent, qboolean banCheck, qboolean admCheck, qboolean subnetCheck);
void	RPM_FileError (gentity_t * ent, const char *file);
void	RPM_id (int);
void	RPM_Obituary( gentity_t *target, gentity_t *attacker, int mod, attackType_t attack, int hitLocation);
void	Cmd_Stats_f( gentity_t *ent );
const char	*G_PlaceString( int rank );
void	RPM_Tokens(gentity_t *ent, char *chatText, int mode);
char	*RPM_BarStat(int stat);
char	*RPM_StatColor(int stat);
int		RPM_FindClosestTeammate(gentity_t *ent, qboolean bot);
void	G_ApplyKnockback( gentity_t *targ, vec3_t newDir, float knockback );
#ifdef _CLANMEMBER
void	RPM_Clan_Vs_All (gentity_t *adm);	
void	RPM_Add_Clan_Member (int argNum, gentity_t *adm);		
void	RPM_Remove_Clan_Member (int argNum, gentity_t *adm);
#endif
void	SC_Server_Info (gentity_t *ent);					//Gives basic server Info
void	SC_weaponCvarDisplay ( gentity_t *ent );			//#weaponCvarDisplay
void	SC_miscCvarDisplay ( gentity_t *ent );				//#miscCvarDisplay
void    SC_serverMsg   (void);
void	CheckExitRules (void);								//#CheckExitRules allows for endgame cmd


///#ExtraWeapons - 04.22.05 - 04:07am
//extern vmCvar_t	sc_extraWeapons;
//extern vmCvar_t	sc_Weapons;
//extern vmCvar_t	g_instagib;
//extern vmCvar_t	sc_forceWeapon;
///End

// GodOrDevil - 01.18.06 - 08:14pm #logging
int 	trap_RealTime ( qtime_t *qtime );



void	RPM_Motd (gentity_t *ent);
void	RPM_Players (gentity_t *ent);
void	RPM_ClientSound (gentity_t *ent, int soundIndex);
void	RPM_GlobalSound (int soundIndex);
void	RPM_Display_Tokens ( gentity_t *ent );
#ifdef _COMPMODE
void	RPM_Pause (gentity_t *adm);
void	RPM_Unpause (gentity_t *adm);
void    RPM_DisplayPlayer_Cmds( gentity_t *ent );
void	RPM_ReadyUp (gentity_t *ent);
void	RPM_ReadyCheck (gentity_t *ent);
void	RPM_Tcmd ( gentity_t *ent );
void	RPM_ReadyTeam(gentity_t *ent, qboolean referee, char *team);
void	RPM_TeamInfo (gentity_t *ent, char *team);
void	RPM_lockTeam(gentity_t *ent, qboolean referee, char *team);
void	RPM_ref_cmd( gentity_t *ent);
void	RPM_Invite_Spec(gentity_t *ent, char *arg2);
void	RPM_Team_Reset(gentity_t *ent, qboolean referee, char *team);
void	RPM_Timeout(gentity_t *ent, qboolean referee);
void	RPM_Timein (gentity_t *ent);
void	RPM_ReadyAll (void);
void	RPM_KickAll (void);
#endif
void	RPM_Awards(void);
void	Bot_SetGametypeItems(void);
void	RPM_Display_Sounds( gentity_t *ent );
void	RPM_Display_Sounds2( gentity_t *ent );
void	RPM_Print_File (gentity_t *ent, char *file);
void	QDECL NV_crashLog		(const char *text, ... );

int RPM_Remove_from_list( char *key, const char *file, const char* type, gentity_t *ent, qboolean unban, qboolean removeAdmin);

#ifndef _NV_ADMIN
void QDECL SC_adminLog	(const char *text, ... );
#endif

#ifdef _TORTURE
void SC_doTorture		(gentity_t *ent);
#endif

////int		RPM_Remove_from_list ( char *key, const char *file, const char* type, gentity_t *ent, qboolean unban);
#ifdef _STUPID_SHIT
void SC_admCount		(void);
#endif
//void SC_extraWeapons	( void );
//void SC_forceWeapons			(int argNum, gentity_t *adm);

//End  - 04.05.05 - 11:27pm

void	RPM_Refresh(gentity_t *ent);
void	RPM_UpdateTMI(void);
#ifndef _NV_ADMIN
int		RPM_ClientNumFromArg (gentity_t *ent, int argNum, const char* usage, const char* type, qboolean aliveOnly);
#endif
int		RPM_AddToList(const char *string, const char* file, const char* type, gentity_t* ent);
void	RPM_ParseChatSounds (void);
void	RPM_WeaponMod (qboolean broadcast, int mod);
void	RPM_Undo_WeaponMods (void);
void	RPM_UpdateLoadScreenMessage (void);
void	RPM_Display_ExtraSounds (gentity_t *ent);
void	RPM_Display_RoundLimits (gentity_t *ent);
void	NV_Weapons				(gclient_t	*client);

//KRIS
#ifdef _MANDOWN_HITBOX
void G_AdjustClientBBox(gentity_t *other);
void G_AdjustClientBBoxs(void);
void G_UndoAdjustedClientBBox(gentity_t *other);
void G_UndoAdjustedClientBBoxs(void);
void G_SetClientPreLeaningBBox(gentity_t *ent);
void G_SetClientLeaningBBox(gentity_t *ent);
///void G_ShowClientBBox(gentity_t *ent);
#endif
//KRIS



extern  vmCvar_t	g_colorTeamNames;
extern  vmCvar_t    g_allowSpecVote;
extern  vmCvar_t	g_allowMinuteVote;
extern  vmCvar_t	g_headShotMessage;
extern  vmCvar_t	g_allowDeathMessages;

#ifdef _STUPID_SHIT
extern  vmCvar_t	g_friend1;
extern  vmCvar_t	g_friend1_motd1;
extern  vmCvar_t	g_friend1_motd2;
extern  vmCvar_t	g_friend1_motd3;
extern  vmCvar_t	g_friend1_motd4;
extern  vmCvar_t	g_friend1_motd5;
extern  vmCvar_t	g_friend1_motd6;
extern  vmCvar_t	g_friend2;
extern  vmCvar_t	g_friend2_motd1;
extern  vmCvar_t	g_friend2_motd2;
extern  vmCvar_t	g_friend2_motd3;
extern  vmCvar_t	g_friend2_motd4;
extern  vmCvar_t	g_friend2_motd5;
extern  vmCvar_t	g_friend2_motd6;
extern  vmCvar_t	g_friend3;
extern  vmCvar_t	g_friend3_motd1;
extern  vmCvar_t	g_friend3_motd2;
extern  vmCvar_t	g_friend3_motd3;
extern  vmCvar_t	g_friend3_motd4;
extern  vmCvar_t	g_friend3_motd5;
extern  vmCvar_t	g_friend3_motd6;
extern  vmCvar_t	g_friend4;
extern  vmCvar_t	g_friend4_motd1;
extern  vmCvar_t	g_friend4_motd2;
extern  vmCvar_t	g_friend4_motd3;
extern  vmCvar_t	g_friend4_motd4;
extern  vmCvar_t	g_friend4_motd5;
extern  vmCvar_t	g_friend4_motd6;
extern  vmCvar_t	g_friendsonly;
extern	vmCvar_t	g_rpmClientsOnly;
extern  vmCvar_t	g_showTkMessage;
extern  vmCvar_t	g_readyPercent;
#endif 

#ifdef _COMPMODE
extern	vmCvar_t	g_autoSpecLock;
extern  vmCvar_t	g_enableTeamCmds;
extern	vmCvar_t	g_compMode;
extern  vmCvar_t	g_timeoutLimit;
#endif

extern  vmCvar_t	server_motd1;
extern  vmCvar_t	server_motd2;
extern  vmCvar_t	server_motd3;
extern  vmCvar_t	server_motd4;
extern  vmCvar_t	server_motd5;
extern  vmCvar_t	server_motd6;

extern  vmCvar_t	g_enableAdmins;
extern	vmCvar_t	g_extraCommands;
extern	vmCvar_t	g_globalVoiceChat;
extern  vmCvar_t	g_allowThirdPerson;
extern  vmCvar_t	g_showKillStreak;
extern	vmCvar_t	g_fastRestart;
// GodOrDevil - #Admin Levels
extern  vmCvar_t	g_sadminfile;		//S-Admin file - 1.11.2005
extern  vmCvar_t	g_adminfile;
#ifdef _BADMIN_FILE
extern  vmCvar_t	g_badminfile;		//B-Admin file - 1.11.2005
#endif
#ifdef _CLANMEMBER
extern  vmCvar_t	g_clanfile;			//Clanfile - RPM GOLD - 1.04.2005
#endif
#ifndef _NV_ADMIN
extern  vmCvar_t	g_subnetbanlist;
#endif
extern	vmCvar_t	g_banlist;

// GodOrDevil - 02.04.05 - 09:04am #Pbox
extern  vmCvar_t	g_defaultPenaltyBoxTime;
extern vmCvar_t		g_maxIPConnections;

// GodOrDevil - 03.10.05 - 05:27pm #antiCamping
//End

//KRIS
//extern vmCvar_t	g_adjLeanOffset;
//extern vmCvar_t	g_adjLBMaxsZ;
//extern vmCvar_t	g_adjLBMinsZ;
//extern vmCvar_t	g_adjDuckedLBMinsZ;
//extern vmCvar_t	g_adjPLBMaxsZ;
//extern vmCvar_t	g_debugAdjBBox;
//extern vmCvar_t	g_debugWeapons;

#define BBOX_LEAN_OFFSET	0.65f
#define LB_MAXZ				-3
#define LB_MINZ				54
#define DUCKED_LB_MINZ		25
#define PLB_MAXZ			-39


//KRIS

//extern vmCvar_t	g_botLean;
#ifdef _RPM_WEAPONMOD
extern vmCvar_t	g_weaponModFlags;
extern vmCvar_t	g_weaponModFile;
#endif

extern chatSounds_t chatSounds[MAX_RPM_CHATS];
extern char	*rpm_weaponLongNames[WP_NUM_WEAPONS];

// GodOrDevil - 04.20.06 - 03:40pm
qboolean G_LoadEntFile(void);
qboolean G_ReadingFromEntFile(qboolean inSubBSP);
char *G_GetEntFileToken(void);

void trap_SendServerCommand2( int clientNum, const char *text );

#include "nv/nv_local.h"


void NV_voteSkipNextMap (void);
void NV_callVoteSkipNextMap (void);
void NV_voteNextMap		(void);

