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

#include "g_local.h"

level_locals_t	level;

#ifdef _WIN32
#undef _WIN32
#endif

#ifdef _WIN32
what the mother fucking fuck
#endif

#ifdef _DB_ENABLED
//nv_topStats_t	topStats[20];
mapStats_t		mapStats;
#endif


typedef struct
{
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	int			cvarFlags;
	float		mMinValue, mMaxValue;
	int			modificationCount;  // for tracking changes
	qboolean	trackChange;	    // track this variable, and announce if changed
	//qboolean	teamShader;			// track and if changed, update shader state

} cvarTable_t;

gentity_t		g_entities[MAX_GENTITIES];
gclient_t		g_clients[MAX_CLIENTS];

vmCvar_t	g_gametype;
vmCvar_t	g_dmflags;
vmCvar_t	g_scorelimit;
vmCvar_t	g_timelimit;
vmCvar_t	g_friendlyFire;
vmCvar_t	g_password;
// GodOrDevil - privatepass overrides password if needed
vmCvar_t	sv_privatePassword;
//END
vmCvar_t	g_needpass;
vmCvar_t	g_maxclients;
vmCvar_t	g_maxGameClients;
vmCvar_t	g_dedicated;
vmCvar_t	g_speed;
vmCvar_t	g_gravity;
vmCvar_t	g_cheats;
vmCvar_t	g_knockback;
vmCvar_t	g_forcerespawn;
vmCvar_t	g_inactivity;
vmCvar_t	g_debugMove;
vmCvar_t	g_debugDamage;
vmCvar_t	g_debugAlloc;
vmCvar_t	g_weaponRespawn;
vmCvar_t	g_backpackRespawn;
vmCvar_t	g_motd;
vmCvar_t	g_synchronousClients;
vmCvar_t	g_warmup;
vmCvar_t	g_doWarmup;
vmCvar_t	g_restarted;
vmCvar_t	g_log;
vmCvar_t	g_logSync;
vmCvar_t	g_logHits;
vmCvar_t	g_allowVote;
vmCvar_t	g_voteDuration;
vmCvar_t	g_failedVoteDelay;
vmCvar_t	g_teamAutoJoin;
vmCvar_t	g_teamForceBalance;
vmCvar_t	g_banIPs;
vmCvar_t	g_filterBan;
vmCvar_t	g_smoothClients;
vmCvar_t	pmove_fixed;
vmCvar_t	pmove_msec;
vmCvar_t	g_rankings;
vmCvar_t	g_listEntity;
vmCvar_t	g_fps;
vmCvar_t	g_respawnInterval;
vmCvar_t	g_respawnInvulnerability;
vmCvar_t	g_roundtimelimit;
vmCvar_t	g_timeextension;
vmCvar_t	g_timeouttospec;
vmCvar_t	g_roundstartdelay;
vmCvar_t	g_availableWeapons;
vmCvar_t	g_forceFollow;
vmCvar_t	g_followEnemy;
vmCvar_t	g_mapcycle;
vmCvar_t	g_pickupsDisabled;				// Whether or not pickups are available in a map (uses outfitting if not)
vmCvar_t	g_suicidePenalty;				// Amount of score added for killing yourself (typically negative)
vmCvar_t	g_teamkillPenalty;				// Amount of score added for killing a teammates (typically negative)
vmCvar_t	g_teamkillDamageMax;			// max damage one can do to teammates before being kicked
vmCvar_t	g_teamkillDamageForgive;		// amount of teamkill damage forgiven each minute
vmCvar_t	g_voiceFloodCount;				// Number of voice messages in one minute to be concidered flooding
vmCvar_t	g_voiceFloodPenalty;			// Amount of time a void flooder must wait before they can use voice again

vmCvar_t	RMG;
vmCvar_t	g_debugRMG;

// GodOrDevil - 11.04.05 - 01:48am
vmCvar_t	g_talkingGhosts;			// Allow ghosts to talk to alive players
vmCvar_t	g_rules1;					// Allow ghosts to talk to alive players
vmCvar_t	g_rules2;					// Allow ghosts to talk to alive players



//Admin System #CVARs - 1.12.2005
#ifndef _NV_ADMIN
vmCvar_t g_plant;
vmCvar_t g_ban;
vmCvar_t g_pop;
vmCvar_t g_runover;
vmCvar_t g_kick;
vmCvar_t g_mute;
vmCvar_t g_strip;
vmCvar_t g_fvote;
vmCvar_t g_forcesay;
vmCvar_t g_namechange;
vmCvar_t g_eventeams;
vmCvar_t g_extracmds;
vmCvar_t g_addadmin;
vmCvar_t g_addbadmin;
vmCvar_t g_pause;
vmCvar_t g_lock;
vmCvar_t g_warn;
vmCvar_t g_swapteams;
vmCvar_t g_pbox;
vmCvar_t g_ff;
vmCvar_t g_subnetban;
vmCvar_t g_torture;
vmCvar_t g_modeChange;
//vmCvar_t g_sounds;
vmCvar_t g_nextMap;
vmCvar_t g_hide;
vmCvar_t	g_needWarn;

#ifdef _CLANMEMBER						//the file with the list of clan members
vmCvar_t g_addclan;
vmCvar_t g_clanvsall;
#endif

#endif

#ifdef _CLANMEMBER
vmCvar_t g_clanfile;						//the file with the list of clan members
#endif

vmCvar_t g_callVote;
vmCvar_t g_updatetmi;
vmCvar_t g_m203;
vmCvar_t g_weaponModFile;
vmCvar_t g_enableDoors;
vmCvar_t g_noDropOrigin;

vmCvar_t g_enableCustomGrenades;
vmCvar_t	g_admCmds;
///ExtraWeapons - 04.22.05 - 04:10am
//vmCvar_t	sc_extraWeapons;
//vmCvar_t	sc_Weapons;
//vmCvar_t	g_instagib;
//vmCvar_t	sc_forceWeapon;
vmCvar_t	g_maxIPConnections;

///--------Misc--------//
vmCvar_t	g_roundjointime;				// GodOrDevil - 09.09.06 - 11:53pm #roundjointime
vmCvar_t	g_Armor;						// GodOrDevil - 1.30.2005 - #Armor
#ifdef _WeaponTime
vmCvar_t	g_weapontime;					// GodOrDevil - 02.04.05 - 05:55am #WeaponTime
#endif
vmCvar_t	g_teamChangeDeath;				// GodOrDevil - 02.04.05 - 06:43am #teamChangeDeath
vmCvar_t	g_defaultPenaltyBoxTime;		//default time used when a plyer is put in the penalty box
vmCvar_t	g_autoEvenTeams;				//02.28.05 - 10:36pm - EvenTeams on a client disconnect
//vmCvar_t	g_pBoxTeamKill;					//03.01.05 - 09:41pm - #pBox Team Kill
#ifdef _TORTURE
vmCvar_t	g_torturedname;					//03.03.05 - 01:29am - #torturedName
#endif
vmCvar_t	g_autoUnlock;					//03.28.05 - 02.40pm - #autoUnlock so last ref/admin disc can/cant unlock teams
vmCvar_t	g_nextScore;					//03.28.05 - 02.58pm - #nextScore sets next maps score when nextMaps is used
//vmCvar_t	g_hideAdmins;					//04.06.05 - 09.40pm - #hideAdmins can hide all admins status
//vmCvar_t	g_allowSounds;					//04.08.05 - 05.40pm - #scPlay & scTeam sound restriction Cvar
vmCvar_t	g_allowTie;
//vmCvar_t	g_noRanks;
//vmCvar_t	sc_allowRefKick;	
//vmCvar_t	sc_adminSuspend;
vmCvar_t	sc_serverMsg;
vmCvar_t	g_setnextMap;					//03.28.05 - 02.58pm - #nextMap allows administrators to set a map to be loaded After
											//... The current map is finished.

vmCvar_t 	g_m4scope;

///#antiCamp
vmCvar_t	g_camperPunish;
vmCvar_t	g_camperAllowTime;
vmCvar_t	g_camperRadius;
vmCvar_t	g_camperSniper;
vmCvar_t	g_camperPunishment;
//vmCvar_t	g_camperPunishment2;
//vmCvar_t	g_camperPunishment3;

vmCvar_t	g_autoVoteTimeExtend;			///Vote to Auto-Extend time.

vmCvar_t	g_onlyWeapons;			
vmCvar_t	g_ForcedItems;			
///ServerMessages - 05.11.05 - 04.25am
vmCvar_t	g_serverMsgInterval;			//05.11.05 - 04.26am - #serverMsgInterval time between automessage display
vmCvar_t	g_enableServerMsgs;			//05.11.05 - 04.26am - #enableServerMsg enables/disables automessages
vmCvar_t	g_msg1;						//05.11.05 - 04.27am - #msg customizable strings to show in the automessages
vmCvar_t	g_msg2;						// ...
vmCvar_t	g_msg3;						// ...
vmCvar_t	g_msg4;						// ...
vmCvar_t	g_msg5;						// ...
vmCvar_t	g_serverMsgDelay;				//12.02.05 - 11.49pm - #serverMsgDelay sets time between each server msgs.
vmCvar_t	g_disableRPMObituary;
vmCvar_t	g_showGametypeHelp;
///ExtraWeapons - 04.22.05 - 04:10am
///vmCvar_t	sc_extraWeapons;
///vmCvar_t	sc_m19pistol;
///vmCvar_t	sc_socom;
///vmCvar_t	sc_shotgun;
///vmCvar_t	sc_uzi;
///vmCvar_t	sc_submachinegun;
///vmCvar_t	sc_AutoShotgun;
///vmCvar_t	sc_m4;
///vmCvar_t	sc_ak;
///vmCvar_t	sc_sniper;
///vmCvar_t	sc_m60;
///vmCvar_t	sc_mm1;
///vmCvar_t	sc_rpg;
///vmCvar_t	sc_m67;
///vmCvar_t	sc_flashnade;
///vmCvar_t	sc_firenade;
///vmCvar_t	sc_smokenade;
///vmCvar_t	sc_fragnade;
///vmCvar_t	sc_fragnade2;
///vmCvar_t	sc_fragnade3;
///vmCvar_t	sc_fragnade4;

///Chat Prefixes - 02.04.05 - 11:15am
vmCvar_t	g_clanChat;						
vmCvar_t	g_sadminChat;					
vmCvar_t	g_adminChat;					
vmCvar_t	g_refChat;						
vmCvar_t	g_caChat;						
vmCvar_t	g_adminTalk;					
vmCvar_t	g_refTalk;						
vmCvar_t	g_clanTalk;	
// GodOrDevil
vmCvar_t	g_adminNotify;
vmCvar_t	g_adminRule;
vmCvar_t	g_adminInfo;
vmCvar_t	g_adminWarning;
///
///antiCamp - 03.10.05 - 05.32pm
///vmCvar_t	sv_campDelay;
///vmCvar_t	sv_campCount;
///vmCvar_t	sv_campRange;

///End

///Ryan april 5 2003
vmCvar_t	g_colorTeamNames;				//sets weather or not colored names will be allowed in teamplay
vmCvar_t    g_allowSpecVote;			    //sets whether or not specs can call votes
vmCvar_t	g_allowMinuteVote;				//sets wether clients can call votes within 1 minute of map change
vmCvar_t	g_headShotMessage;				//this is the message that will be displayed when the player gets a headshot
vmCvar_t	g_allowDeathMessages;			//sets wheter or not deathmessages will be displayed
vmCvar_t	g_adminfile;					//this is the file with the list of server admins
#ifdef _BADMIN_FILE
 vmCvar_t	g_badminfile;
#endif
 #ifndef _NV_ADMIN
vmCvar_t	g_subnetbanlist;
#endif
vmCvar_t	g_banlist;						//The file that holds the list of banned people
#ifdef _STUPID_SHIT
vmCvar_t	g_friend1;						//The tag that identifies certain special people goes in here
vmCvar_t	g_friend1_motd1;				//and their special motd is here
vmCvar_t	g_friend1_motd2;
vmCvar_t	g_friend1_motd3;
vmCvar_t	g_friend1_motd4;
vmCvar_t	g_friend1_motd5;
vmCvar_t	g_friend1_motd6;
vmCvar_t	g_friend2;
vmCvar_t	g_friend2_motd1;
vmCvar_t	g_friend2_motd2;
vmCvar_t	g_friend2_motd3;
vmCvar_t	g_friend2_motd4;
vmCvar_t	g_friend2_motd5;
vmCvar_t	g_friend2_motd6;
vmCvar_t	g_friend3;
vmCvar_t	g_friend3_motd1;
vmCvar_t	g_friend3_motd2;
vmCvar_t	g_friend3_motd3;
vmCvar_t	g_friend3_motd4;
vmCvar_t	g_friend3_motd5;
vmCvar_t	g_friend3_motd6;
vmCvar_t	g_friend4;
vmCvar_t	g_friend4_motd1;
vmCvar_t	g_friend4_motd2;
vmCvar_t	g_friend4_motd3;
vmCvar_t	g_friend4_motd4;
vmCvar_t	g_friend4_motd5;
vmCvar_t	g_friend4_motd6;
vmCvar_t	g_friendsonly;					// only people who have the appropriate signs that match
vmCvar_t	g_rpmClientsOnly;
vmCvar_t	g_showTkMessage;
vmCvar_t	g_readyPercent;
#endif

#ifdef _COMPMODE
vmCvar_t	g_compMode;					//enables competiton only options
vmCvar_t	g_timeoutLimit;
vmCvar_t	g_enableTeamCmds;
vmCvar_t	g_autoSpecLock;
#endif

vmCvar_t	server_motd1;					//The default MOTD that is shown when a player first
vmCvar_t	server_motd2;					//gets into the game
vmCvar_t	server_motd3;
vmCvar_t	server_motd4;
vmCvar_t	server_motd5;
vmCvar_t	server_motd6;
										//the friend1 -4 tags will be allowed to enter if this is set to 1
vmCvar_t	g_enableAdmins;
vmCvar_t	g_extraCommands;
vmCvar_t	g_globalVoiceChat;
vmCvar_t	g_allowThirdPerson;
vmCvar_t	g_showKillStreak;
vmCvar_t	g_fastRestart;
// KRIS 15/04/2003 11:20PM
#ifdef _CRS_BOTS
vmCvar_t	bot_editWPS;
vmCvar_t	bot_debug;
vmCvar_t	bot_debugNav;
vmCvar_t	bot_debugTeamplay;
vmCvar_t	bot_debugGameType;
#endif
// KRIS


//Ryan

// KRIS 7/08/2003 11:11AM
//vmCvar_t	g_debugWeapons;
//vmCvar_t	g_debugAdjBBox;
//vmCvar_t	g_adjLeanOffset;
//vmCvar_t	g_adjLBMaxsZ;
//vmCvar_t	g_adjLBMinsZ;
//vmCvar_t	g_adjDuckedLBMinsZ;
//vmCvar_t	g_adjPLBMaxsZ;
// KRIS

//Ryan Temporary
//vmCvar_t	g_botLean;
//Ryan

//Ryan april 14 2004 9:28pm
#ifdef _RPM_WEAPONMOD
vmCvar_t	g_weaponModFlags;
vmCvar_t	g_weaponModFile;
#endif
//Ryan


// GodOrDevil
vmCvar_t	gt_customType;
vmCvar_t	gt_targetHealth;
vmCvar_t	nv_gtName;
vmCvar_t	nv_gtShort;
vmCvar_t	g_swapsides;
vmCvar_t	gt_bombTimer;
vmCvar_t	g_freezeTime;
//vmCvar_t	nv_reinforcementsBlue;
//vmCvar_t	nv_reinforcementsRed;
//vmCvar_t	nv_reinforcementsRedTime;
//vmCvar_t	nv_reinforcementsBlueTime;
vmCvar_t	g_enableHealthGains;
vmCvar_t	g_maxSpawnKill;
//vmCvar_t	g_crouchNeeded;
vmCvar_t	g_enableChatReponses;

#ifdef _MAPEDITOR
vmCvar_t	nv_editMap;
vmCvar_t	nv_enableThirdPerson;
vmCvar_t	nv_enableThirdCrosshair;
#endif

#ifdef _KILLSPREE
vmCvar_t	g_enableKillSpreeReward;
vmCvar_t	g_killSpreeFile;
#endif // _KILLSPREE

vmCvar_t	g_enableVoteSkipNextMap;
vmCvar_t	g_enableVoteNextMap;

#ifdef _DB_ENABLED
#ifdef _DB_STATS_DLL
vmCvar_t	statsloaded;
#endif
vmCvar_t	g_enableDatabase;
vmCvar_t	g_enableTopStats;
vmCvar_t	g_banIpSize;
vmCvar_t	g_databasename;
#endif

vmCvar_t	g_ggRankType;  // For Gun Game resetting



static cvarTable_t gameCvarTable[] =
{
	/// don't override the cheat state set by the system
	{ &g_cheats, "sv_cheats", "", 0, 0.0, 0.0, 0, qfalse },

	/// noset vars
	{ NULL, "gamename", GAMEVERSION , CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0, 0, qfalse  },
	{ NULL, "gamedate", __DATE__ , CVAR_ROM, 0.0, 0.0, 0, qfalse  },
	{ &g_restarted, "g_restarted", "0", CVAR_ROM, 0.0, 0.0, 0, qfalse  },
	{ NULL, "sv_mapname", "", CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0, 0, qfalse  },

	{ &g_fps, "sv_fps", "", CVAR_ROM, 0.0, 0.0, 0, qfalse },

	/// latched vars
	{ &g_gametype, "g_gametype", "dm", CVAR_SERVERINFO | CVAR_LATCH, 0.0, 0.0, 0, qfalse  },

	{ &g_maxclients, "sv_maxclients", "8", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
	{ &g_maxGameClients, "g_maxGameClients", "0", CVAR_SERVERINFO | CVAR_LATCH | CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },

	{ &g_dmflags, "dmflags", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue  },
	{ &g_scorelimit, "scorelimit", "20", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0.0, 0.0, 0, qtrue },
	{ &g_timelimit, "timelimit", "0", CVAR_SERVERINFO | CVAR_ARCHIVE | CVAR_NORESTART, 0.0, 0.0, 0, qtrue },

	{ &g_synchronousClients, "g_synchronousClients", "0", CVAR_SYSTEMINFO, 0.0, 0.0, 0, qfalse  },

	{ &g_friendlyFire, "g_friendlyFire", "0", CVAR_SERVERINFO|CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue  },

	{ &g_teamAutoJoin, "g_teamAutoJoin", "0", CVAR_ARCHIVE,  0.0, 0.0, 0, qfalse  },
	{ &g_teamForceBalance, "g_teamForceBalance", "0", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },

	{ &g_warmup, "g_warmup", "20", CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue  },
	{ &g_doWarmup, "g_doWarmup", "0", 0, 0.0, 0.0, 0, qtrue  },
	{ &g_log, "g_log", "games.log", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
	{ &g_logSync, "g_logSync", "0", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
	{ &g_logHits, "g_logHits", "0", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },

	{ &g_password, "g_password", "", CVAR_USERINFO, 0.0, 0.0, 0, qfalse  },
	{ &sv_privatePassword, "sv_privatePassword", "", CVAR_INTERNAL, 0.0, 0.0, 0, qfalse  },

	{ &g_banIPs, "g_banIPs", "", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
	{ &g_filterBan, "g_filterBan", "1", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },

	{ &g_needpass, "g_needpass", "0", CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0, 0, qtrue },

	{ &g_dedicated, "dedicated", "0", 0, 0.0, 0.0, 0, qfalse  },

	{ &g_speed, "g_speed", "280", 0, 0.0, 0.0, 0, qtrue  },
	{ &g_gravity, "g_gravity", "800", 0, 0.0, 0.0, 0, qtrue  },
	{ &g_knockback, "g_knockback", "700", 0, 0.0, 0.0, 0, qtrue  },
	{ &g_weaponRespawn, "g_weaponrespawn", "15", 0, 0.0, 0.0, 0, qtrue  },
	{ &g_backpackRespawn, "g_backpackrespawn", "40", 0, 0.0, 0.0, 0, qtrue  },
	{ &g_forcerespawn, "g_forcerespawn", "20", 0, 0.0, 0.0, 0, qtrue },
	{ &g_inactivity, "g_inactivity", "180", CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue },
	{ &g_debugMove, "g_debugMove", "0", 0, 0.0, 0.0, 0, qfalse },
	{ &g_debugDamage, "g_debugDamage", "0", 0, 0.0, 0.0, 0, qfalse },
	{ &g_debugAlloc, "g_debugAlloc", "0", 0, 0.0, 0.0, 0, qfalse },
	{ &g_motd, "g_motd", "", 0, 0.0, 0.0, 0, qfalse },

	{ &g_allowVote, "g_allowVote", "1", CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue },
	{ &g_voteDuration, "g_voteDuration", "60", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
	{ &g_failedVoteDelay, "g_failedVoteDelay", "1", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
	{ &g_listEntity, "g_listEntity", "0", 0, 0.0, 0.0, 0, qfalse },

	{ &g_smoothClients, "g_smoothClients", "1", 0, 0.0, 0.0, 0, qfalse},
	{ &pmove_fixed, "pmove_fixed", "0", CVAR_SYSTEMINFO, 0.0, 0.0, 0, qfalse},
	{ &pmove_msec, "pmove_msec", "8", CVAR_SYSTEMINFO, 0.0, 0.0, 0, qfalse},

	{ &g_rankings, "g_rankings", "0", 0, 0.0, 0.0, 0, qfalse},

	{ &g_respawnInterval, "g_respawnInterval", "15", CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue },
	{ &g_respawnInvulnerability, "g_respawnInvulnerability", "5", CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue },

	{ &RMG, "RMG", "0", 0, 0.0, 0.0, 0, qfalse },
	{ &g_debugRMG, "g_debugRMG", "0", 0, 0.0f, 0.0f, 0, qfalse },

	{ &g_timeouttospec,		"g_timeouttospec",	"15",		CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },
	{ &g_roundtimelimit,	"g_roundtimelimit",	"5",		CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue },
	{ &g_timeextension,		"g_timeextension",	"15",		CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue },

	{ &g_roundstartdelay,	"g_roundstartdelay", "5",		CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue },

	{ &g_availableWeapons,	"g_availableWeapons", "2222222222211", CVAR_ARCHIVE|CVAR_SERVERINFO|CVAR_LATCH, 0.0, 0.0, 0, qtrue },

	{ &g_forceFollow,		"g_forceFollow",	 "0",			  CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue },
	{ &g_followEnemy,		"g_followEnemy",	 "1",			  CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue },

	{ &g_mapcycle,			"sv_mapcycle",		 "none",		CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },

	{ &g_pickupsDisabled,	"g_pickupsDisabled", "0",					CVAR_ARCHIVE|CVAR_LATCH, 0.0, 0.0, 0, qtrue },

	{ &g_suicidePenalty,	"g_suicidePenalty",  "-1",					CVAR_ARCHIVE,	0.0f,	0.0f,	0,	qfalse },

	{ &g_voiceFloodCount,	"g_voiceFloodCount",	"6",				CVAR_ARCHIVE,	0.0f,	0.0f,	0,  qfalse },
	{ &g_voiceFloodPenalty,	"g_voiceFloodPenalty",	"60",				CVAR_ARCHIVE,	0.0f,	0.0f,	0,  qfalse },

	{ &g_teamkillPenalty,		"g_teamkillPenalty",		"-1",		CVAR_ARCHIVE,	0.0f,	0.0f,	0,	qfalse },
	{ &g_teamkillDamageMax,		"g_teamkillDamageMax",		"300",		CVAR_ARCHIVE,	0.0f,	0.0f,	0,  qtrue },
	{ &g_teamkillDamageForgive,	"g_teamkillDamageForgive",	"50",		CVAR_ARCHIVE,	0.0f,	0.0f,	0,  qfalse },
	
#ifdef _CLANMEMBER
	{ &g_clanfile,					"g_clanfile",			"users/clanmembers.txt",	CVAR_ARCHIVE,	0.0,	0.0,  0, qfalse  },
#endif

	{ &g_m4scope,					"g_m4scope",			"0",		0,	0.0,	0.0,  0, qtrue  },
	{ &g_m203,						"g_m203",				"0",				0,	0.0f,   0.0f, 0,  qtrue	 },	
	{ &g_callVote,					"g_callVote",			"3",				0,	0.0f,   0.0f, 0,  qfalse },
	{ &g_updatetmi,				"g_updatetmi",			"1000",				0,	0.0f,   0.0f, 0,  qfalse },
	{ &g_rules1,					"g_rules1",				"^6You can use all weapons available.  ^5The full map is always open unless blocked by the server!",			0, 0, 0, 0, qfalse},
	{ &g_rules2,					"g_rules2",				"^6DO NOT: ^53RD, WH, Cheat, Map Exploit, Spam chat/sounds, SPAWN CAMP, PACK CAMP, or accuse players of cheating/hacking!",	0, 0, 0, 0, qfalse},

	/*==================== Misc Cvars ====================*/
#ifdef _TORTURE
	{ &g_torturedname,				"g_torturedname",       "ToRTurEd FoOL",    CVAR_ARCHIVE,	0.0f,   0.0f, 0,  qfalse },
#endif
	{ &g_autoUnlock,				"g_autoUnlock",			"1",				0,	0.0f,   0.0f, 0,  qtrue },
	{ &g_Armor,						"g_Armor",				"0",				0,	0.0f,	0.0f, 0,  qtrue },
#ifdef _WeaponTime
	{ &g_weapontime,				"g_weapontime",			"0",				0,	0.0f,	0.0f, 0,  qfalse },
#endif
	{ &g_teamChangeDeath,			"g_teamChangeDeath",	"0",				0,	0.0f,	0.0f, 0,  qtrue },
	{ &g_autoEvenTeams,				"g_autoEvenTeams",		"8",				0,	0.0f,   0.0f, 0,  qtrue },
	{ &g_allowTie,					"g_allowTie",			"0",				0,	0.0,	0.0,  0,  qtrue },
	{ &g_maxIPConnections,			"g_maxIPConnections",	"4",				0,	0.0f,   0.0f, 0,  qfalse},
	{ &g_roundjointime,				"g_roundjointime",		"5",				0,	0.0f,	0.0f, 0,  qtrue },
	{ &g_weaponModFile,				"g_weaponModFile",		"sof2",				0,	0.0f,   0.0f, 0,  qtrue},
	{ &g_enableDoors,				"g_enableDoors",		"0",				0,  0,		0,	  0, qfalse },

	/*==================== Server Msg Cvars ====================*/
	{ &g_serverMsgInterval,			"g_serverMsgInterval",		    "1",		CVAR_ARCHIVE,	0.0,	0.0,  0,  qfalse  },
	{ &g_enableServerMsgs,			"g_enableServerMsgs",		    "1",		CVAR_ARCHIVE,	0.0,	0.0,  0,  qfalse  },
	{ &g_serverMsgDelay,			"g_serverMsgDelay",			"10",		CVAR_ARCHIVE,	0.0,	0.0,  0,  qfalse },
	{ &g_msg1,						"g_msg1",						"",			0,	0.0,	0.0,  0,  qfalse  },
	{ &g_msg2,						"g_msg2",						"",			0,	0.0,	0.0,  0,  qfalse  },
	{ &g_msg3,						"g_msg3",						"",			0,	0.0,	0.0,  0,  qfalse  },
	{ &g_msg4,						"g_msg4",						"",			0,	0.0,	0.0,  0,  qfalse  },
	{ &g_msg5,						"g_msg5",						"",			0,	0.0,	0.0,  0,  qfalse  },
	{ &g_onlyWeapons,				"g_onlyWeapons",				"0",		0,	0.0f,	0.0f,  0,  qfalse },
	{ &g_ForcedItems,				"g_ForcedItems",				"0",		0,	0.0f,	0.0f,  0,  qfalse },
	
	{ &g_disableRPMObituary,		"g_disableRPMObituary",		"0",		0,	0.0f,	0.0f,  0,  qfalse },
	
	//This is not Archived, otherwise it will always play the selected map.
	{ &g_setnextMap,				"g_setnextMap",			"none",				0,	0.0f,   0.0f, 0,  qfalse },
	{ &g_nextScore,					"g_nextScore",			"",					0,	0.0f,   0.0f, 0,  qfalse },
	
	/*==================== #antiCamp ====================*/
	{ &g_camperAllowTime,		"g_camperAllowTime",	"30",					0, 0, 0, 0, qtrue },
	{ &g_camperPunish,			"g_camperPunish",		"0",					0, 0, 0, 0, qtrue },
	{ &g_camperRadius,			"g_camperRadius",		"300",					0, 0, 0, 0, qtrue },
	{ &g_camperPunishment,		"g_camperPunishment",	"pop",					0, 0, 0, 0, qtrue },
	//{ &g_camperPunishment2,	"g_camperPunishment2",	"twist",				0, 0, 0, 0, qtrue },
	//{ &g_camperPunishment3,	"g_camperPunishment3",	"burn",					0, 0, 0, 0, qtrue },
	{ &g_camperSniper,			"g_camperSniper",		"1",					0, 0, 0, 0, qtrue },
	{ &g_autoVoteTimeExtend,	"g_autoVoteTimeExtend","1",					0, 0, 0, 0, qtrue },

	/*==================== Shortcut Chat Prefixes ====================*/
	{ &g_adminTalk,			"g_adminTalk",	"^_**^7ADMIN^_**^-",		0,	0.0f,	0.0f, 0,  qfalse },
	{ &g_sadminChat,		"g_sadminChat",	"^7**^gS-ADMIN ONLY^7**^7",	0,	0.0f,	0.0f, 0,  qfalse },
	{ &g_adminChat,			"g_adminChat",	"^7**^1ADMIN ONLY^7**^7",	0,	0.0f,	0.0f, 0,  qfalse },
	{ &g_clanTalk,			"g_clanTalk",	"^R**^1CLAN^R**^7",			0,	0.0f,	0.0f, 0,  qfalse },
	{ &g_clanChat,			"g_clanChat",	"^R**^7CLAN ONLY^R**^7",	0,	0.0f,	0.0f, 0,  qfalse },
	{ &g_refTalk,			"g_refTalk",	"^_**^3REFEREE^_**^-",		0,	0.0f,	0.0f, 0,  qfalse },
	{ &g_refChat,			"g_refChat",	"^7**^3REF ONLY^7**^7",		0,	0.0f,	0.0f, 0,  qfalse },
	{ &g_caChat,			"g_caChat",		"^7**^RHEY ADMIN^7**^7",	0,	0.0f,	0.0f, 0,  qfalse },
	///	
	{ &g_adminNotify,		"g_adminNotify",	"^$",					0,	0.0f,	0.0f, 0,  qfalse },
	{ &g_adminRule,			"g_adminRule",		"^3*^_RULE^3*",			0,	0.0f,	0.0f, 0,  qfalse },
	{ &g_adminInfo,			"g_adminInfo",		"^7*^iINFO^7*",			0,	0.0f,	0.0f, 0,  qfalse },
	{ &g_adminWarning,		"g_adminWarning",	"^_*^3WARNING^_*",		0,	0.0f,	0.0f, 0,  qfalse },
	///
	{ &g_defaultPenaltyBoxTime, "g_defaultPenaltyBoxTime", "20",		0,	0.0,	0.0,  0,  qtrue  },
	{ &g_talkingGhosts,			"g_talkingGhosts",			"1",		0,	0.0f,	0.0f,	0,  qtrue },


	{ NULL,					"modname", RPM_VERSION_STRING_LONG, CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0, 0, qfalse  },
	{ &g_colorTeamNames,	"g_colorTeamNames", "0", CVAR_LATCH | 0, 0.0, 0.0, 0, qtrue  },
	{ &g_allowSpecVote,		"g_allowSpecVote", "0", 0, 0.0, 0.0, 0, qtrue  },
	{ &g_allowMinuteVote,	"g_allowMinuteVote", "1", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_headShotMessage,	"g_headShotMessage", "^1-^3=^1H^3E^1A^3D^1S^3H^1O^3T^1=^3-", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_allowDeathMessages,"g_allowDeathMessages", "1", 0, 0.0, 0.0, 0, qtrue  },
	{ &g_adminfile,			"g_adminfile", "users/admins.txt", 0, 0.0, 0.0, 0, qfalse  },
#ifdef _BADMIN_FILE
	{ &g_badminfile,			"g_badminfile", "users/badmins.txt", 0, 0.0, 0.0, 0, qfalse  },
#endif
	{ &g_banlist,			"g_banlist", "users/bannedclients.txt", 0, 0.0, 0.0, 0, qfalse  },
	{ &server_motd1, "server_motd1", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &server_motd2, "server_motd2", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &server_motd3, "server_motd3", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &server_motd4, "server_motd4", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &server_motd5, "server_motd5", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &server_motd6, "server_motd6", "", 0, 0.0, 0.0, 0, qfalse  },
#ifdef _STUPID_SHIT
	{ &g_friend1, "g_friend1", "", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
	{ &g_friend1_motd1, "g_friend1_motd1", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend1_motd2, "g_friend1_motd2", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend1_motd3, "g_friend1_motd3", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend1_motd4, "g_friend1_motd4", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend1_motd5, "g_friend1_motd5", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend1_motd6, "g_friend1_motd6", "", 0, 0.0, 0.0, 0, qfalse  },

	{ &g_friend2, "g_friend2", "", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
	{ &g_friend2_motd1, "g_friend2_motd1", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend2_motd2, "g_friend2_motd2", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend2_motd3, "g_friend2_motd3", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend2_motd4, "g_friend2_motd4", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend2_motd5, "g_friend2_motd5", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend2_motd6, "g_friend2_motd6", "", 0, 0.0, 0.0, 0, qfalse  },

	{ &g_friend3, "g_friend3", "", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
	{ &g_friend3_motd1, "g_friend3_motd1", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend3_motd2, "g_friend3_motd2", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend3_motd3, "g_friend3_motd3", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend3_motd4, "g_friend3_motd4", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend3_motd5, "g_friend3_motd5", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend3_motd6, "g_friend3_motd6", "", 0, 0.0, 0.0, 0, qfalse  },

	{ &g_friend4, "g_friend4", "", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
	{ &g_friend4_motd1, "g_friend4_motd1", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend4_motd2, "g_friend4_motd2", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend4_motd3, "g_friend4_motd3", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend4_motd4, "g_friend4_motd4", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend4_motd5, "g_friend4_motd5", "", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_friend4_motd6, "g_friend4_motd6", "", 0, 0.0, 0.0, 0, qfalse  },

	{ &g_friendsonly, "g_friendsonly", "0", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
	{ &g_rpmClientsOnly, "g_rpmClientsOnly", "0", CVAR_LATCH | CVAR_SERVERINFO|CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue  },
	{ &g_showTkMessage, "g_showTkMessage", "1", CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue  },
	{ &g_readyPercent, "g_readyPercent", "100", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse },

#endif

#ifdef _COMPMODE
	{ &g_autoSpecLock, "g_autoSpecLock", "1", CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue  },
	{ &g_timeoutLimit, "g_timeoutLimit", "3", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
	{ &g_enableTeamCmds, "g_enableTeamCmds", "1", CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue  },
	{ &g_compMode,			"g_compMode",	"0",	CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue  },	
#endif

	{ &g_enableAdmins, "g_enableAdmins", "1", CVAR_LATCH | CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue  },
	{ &g_extraCommands, "g_extraCommands", "ExtraAdminCommands.txt", 0, 0.0, 0.0, 0, qfalse  },
	{ &g_globalVoiceChat, "g_globalVoiceChat", "1", 0, 0.0, 0.0, 0, qtrue  },
	{ &g_allowThirdPerson, "g_allowThirdPerson", "0", CVAR_SERVERINFO | CVAR_ARCHIVE, 0.0, 0.0, 0, qtrue  },
	{ &g_showKillStreak, "g_showKillStreak", "1", 0, 0.0, 0.0, 0, qtrue  },

	{ &g_fastRestart, "g_fastRestart", "1", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
	///Ryan
	// KRIS 15/04/2003 11:20PM
#ifdef _CRS_BOTS
	{ &bot_editWPS, "bot_editWPS", "0", 0, 0.0, 0.0, 0, qfalse },
	{ &bot_debugNav, "bot_debugNav", "0", 0, 0.0, 0.0, 0, qfalse },
	{ &bot_debug, "bot_debug", "0", 0, 0.0, 0.0, 0, qfalse },
	{ &bot_debugNav, "bot_debugNav", "0", 0, 0.0, 0.0, 0, qfalse },
	{ &bot_debugTeamplay, "bot_debugTeamplay", "0", 0, 0.0, 0.0, 0, qfalse },
	{ &bot_debugGameType, "bot_debugGameType", "0", 0, 0.0, 0.0, 0, qfalse },
#endif
	// KRIS
	
#ifdef _RPM_WEAPONMOD
	///Ryan april 14 2004 9:25pm
	{ &g_weaponModFile, "g_weaponModFile", "RPM_weaponMods.wpn", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
	{ &g_weaponModFlags, "g_weaponModFlags", "0", CVAR_ARCHIVE, 0.0, 0.0, 0, qfalse  },
	///Ryan
#endif

	{ &gt_targetHealth,				"gt_targetHealth",			"3",				CVAR_ROM,	0.0,	0.0,  0, qfalse },
	{ &gt_customType,				"gt_customType",			"0",				CVAR_ROM,	0.0,	0.0,  0, qfalse },
	{ &nv_gtName,					"nv_gtName",				"Deathmatch",		CVAR_ROM,	0.0,	0.0,  0,  qfalse },
	{ &nv_gtShort,					"nv_gtShort",				"DM",				CVAR_ROM,	0.0,	0.0,  0,  qfalse },	
	{ &gt_bombTimer,				"gt_bombTimer",				"30",				0,	0.0,	0.0,  0, qtrue },
	{ &g_freezeTime,				"g_freezeTime",			"20",				0,	0,		0,	  0,  qtrue},
	{ &g_swapsides,					"g_swapsides",				"0",				0,	0.0,	0.0,  0, qtrue },
	{ &g_enableHealthGains,			"g_enableHealthGains",		"0",				0,	0,		0,	  0,  qfalse},
	{ &g_maxSpawnKill,				"g_maxSpawnKill",		"5",				0, 0, 0, 0, qtrue },
	{ &g_showGametypeHelp,			"g_showGametypeHelp",		"1",		0,	0.0f,	0.0f,  0,  qfalse },

	{ &g_enableChatReponses,		"g_enableChatReponses",	"1",				0,  0.0, 0.0, 0,	qtrue },
	{ &g_enableCustomGrenades,		"g_enableCustomGrenades",	"0",				CVAR_LATCH,	0.0f,	0.0f, 0,  qtrue},
	{ &g_admCmds,						"g_admCmds",			"323344341435344334354443544544344544421134323",	CVAR_ARCHIVE,	0.0f,   0.0f, 0,  qfalse },

#ifdef _MAPEDITOR
	{ &nv_editMap,						"nv_editMap",			"0",					CVAR_SERVERINFO|CVAR_ARCHIVE,	0.0f,   0.0f, 0,  qtrue},
	//{ &nv_enableThirdPerson,			"nv_enableThirdPerson",	"2",				CVAR_SERVERINFO|CVAR_ARCHIVE,	0.0f,   0.0f, 0,  qtrue},
	{ &nv_enableThirdPerson,			"nv_enableThirdPerson",	"2",				CVAR_SERVERINFO|CVAR_ROM,	0.0f,   0.0f, 0,  qtrue},
	{ &nv_enableThirdCrosshair,			"nv_enableThirdCrosshair","1",				CVAR_SERVERINFO|CVAR_ARCHIVE,	0.0f,   0.0f, 0,  qtrue},

#endif

#ifdef _KILLSPREE
	{ &g_enableKillSpreeReward,	"g_enableKillSpreeReward",	"0",				0,	0.0f,	0.0f, 0,  qtrue},
	{ &g_killSpreeFile,			"g_killSpreeFile",		"settings/killspree.cfg",	CVAR_LATCH,	0.0f,   0.0f, 0,  qtrue },

#endif // _KILLSPREE
	
		{ &g_noDropOrigin,			"g_noDropOrigin",	"0",						CVAR_ROM,	0.0f,	0.0f, 0,  qfalse},

	{ &g_enableVoteSkipNextMap,		"g_enableVoteSkipNextMap",	"0",			CVAR_ARCHIVE,	0,		0,	  0,  qtrue},
	{ &g_enableVoteNextMap,			"g_enableVoteNextMap",	"1",				CVAR_ARCHIVE,	0,		0,	  0,  qtrue},
	
		{ &g_ggRankType,				"g_ggRankType",			"0",						0,	0,		0,	  0,  qtrue},

#ifdef _DB_ENABLED
#ifdef _DB_STATS_DLL
	{ &statsloaded,						"statsloaded",				"0", CVAR_ROM, 0.0, 0.0, 0, qfalse },
#endif
	{ &g_databasename,					"g_databasename", "",  CVAR_ROM, 0.0, 0.0, 0, qfalse  },
	{ &g_enableTopStats,				"g_enableTopStats",	"1",					CVAR_ARCHIVE,	0.0f,	0.0f, 0,  qtrue},
	{ &g_enableDatabase,				"g_enableDatabase",	"1",					CVAR_ARCHIVE,	0.0f,	0.0f, 0,  qtrue},
	{ &g_banIpSize,						"g_banIpSize",			"7",					CVAR_ARCHIVE,	4,		15, 0,  qfalse}, 
#endif

};




// bk001129 - made static to avoid aliasing
static int gameCvarTableSize = sizeof( gameCvarTable ) / sizeof( gameCvarTable[0] );


void G_InitGame					( int levelTime, int randomSeed, int restart );
void G_RunFrame					( int levelTime );
void G_ShutdownGame				( int restart );
void CheckExitRules				( void );
void G_InitGhoul				( void );
void G_ShutdownGhoul			( void );

/*
================
vmMain

This is the only way control passes into the module.
This must be the very first function compiled into the .q3vm file
================
*/
int vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11  )
{
	switch ( command )
	{
		case GAME_GHOUL_INIT:
			G_InitGhoul ( );
			return 0;
		case GAME_GHOUL_SHUTDOWN:
			G_ShutdownGhoul ( );
			return 0;
		case GAME_INIT:	
			G_InitGame( arg0, arg1, arg2 );
			return 0;
		case GAME_SHUTDOWN:
			G_ShutdownGame( arg0 );
			return 0;
		case GAME_CLIENT_CONNECT:
			return (int)ClientConnect( arg0, arg1, arg2 );
		case GAME_CLIENT_THINK:
			ClientThink( arg0 );
			return 0;
		case GAME_CLIENT_USERINFO_CHANGED:
			ClientUserinfoChanged( arg0 );
			return 0;
		case GAME_CLIENT_DISCONNECT:
			ClientDisconnect( arg0 );
			return 0;
		case GAME_CLIENT_BEGIN:
			//Ryan april 10 2004 10:04am
			//ClientBegin( arg0 );
			ClientBegin( arg0, qtrue );
			//Ryan
			return 0;
		case GAME_CLIENT_COMMAND:
			ClientCommand( arg0 );
			return 0;
		case GAME_GAMETYPE_COMMAND:
			return G_GametypeCommand ( arg0, arg1, arg2, arg3, arg4, arg5 );
		case GAME_RUN_FRAME:
			G_RunFrame( arg0 );
			return 0;
		case GAME_CONSOLE_COMMAND:
			return ConsoleCommand();
		case BOTAI_START_FRAME:
#ifdef _SOF2_BOTS
			return BotAIStartFrame( arg0 );
#else
			return qfalse;
#endif
		case GAME_SPAWN_RMG_ENTITY:
			if (G_ParseSpawnVars(qfalse))
			{
				G_SpawnGEntityFromSpawnVars(qfalse);
			}
			return 0;
	}
	return -1;
}

/*
================
G_FindTeams

Chain together all entities with a matching team field.
Entity teams are used for item groups and multi-entity mover groups.

All but the first will have the FL_TEAMSLAVE flag set and teammaster field set
All but the last will have the teamchain field set to the next one
================
*/
void G_FindTeams( void ) {
	gentity_t	*e, *e2;
	int		i, j;
	int		c, c2;

	c = 0;
	c2 = 0;
	for ( i=1, e=g_entities+i ; i < level.num_entities ; i++,e++ ){
		if (!e->inuse)
			continue;
		if (!e->team)
			continue;
		if (e->flags & FL_TEAMSLAVE)
			continue;
		e->teammaster = e;
		c++;
		c2++;
		for (j=i+1, e2=e+1 ; j < level.num_entities ; j++,e2++)
		{
			if (!e2->inuse)
				continue;
			if (!e2->team)
				continue;
			if (e2->flags & FL_TEAMSLAVE)
				continue;
			if (!strcmp(e->team, e2->team))
			{
				c2++;
				e2->teamchain = e->teamchain;
				e->teamchain = e2;
				e2->teammaster = e;
				e2->flags |= FL_TEAMSLAVE;

				// make sure that targets only point at the master
				if ( e2->targetname ) {
					e->targetname = e2->targetname;
					e2->targetname = NULL;
				}
			}
		}
	}

	Com_Printf ("%i teams with %i entities\n", c, c2);
}

/*
=================
G_RemapTeamShaders
=================
*/
void G_RemapTeamShaders(void)
{
	trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
}

/*
=================
G_RegisterCvars
=================
*/
void G_RegisterCvars( void )
{
	int			i;
	cvarTable_t	*cv;
	qboolean	remapped = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ )
	{
		trap_Cvar_Register( cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags, cv->mMinValue, cv->mMaxValue );

		if ( cv->vmCvar )
		{
			cv->modificationCount = cv->vmCvar->modificationCount;
		}

		//if (cv->teamShader)
		//{
		//	remapped = qtrue;
		//}
	}

	//if (remapped)
	//{
	//	G_RemapTeamShaders();
	//}

	level.warmupModificationCount = g_warmup.modificationCount;
}

/*
=================
G_UpdateCvars
=================
*/
qboolean BG_InitWeaponStats(void);
void G_UpdateCvars( void )
{
	int			i;
	cvarTable_t	*cv;
	qboolean	remapped = qfalse;

	for ( i = 0, cv = gameCvarTable ; i < gameCvarTableSize ; i++, cv++ )
	{
		if ( cv->vmCvar )
		{
			trap_Cvar_Update( cv->vmCvar );

			if ( cv->modificationCount != cv->vmCvar->modificationCount )
			{
				// Handle any modified cvar checks
				if ( !Q_stricmp ( cv->cvarName, "sv_fps" ) )
				{
					if ( cv->vmCvar->integer > 50 )
					{
						trap_Cvar_Set ( "sv_fps", "50" );
						trap_Cvar_Update ( cv->vmCvar );
					}
					else if ( cv->vmCvar->integer < 10 )
					{
						trap_Cvar_Set ( "sv_fps", "10" );
						trap_Cvar_Update ( cv->vmCvar );
					}
				}
						
#ifdef _MAPEDITOR
				//if ( !g_cheats.integer )
				//{
				//	if(nv_editMap.integer) 
				//	{
				//		trap_Cvar_Set ( "nv_editMap", "0" );
				//		trap_Cvar_Update ( &nv_editMap );
				//	}
				//}
				//if(!Q_stricmp(cv->cvarName,"nv_editMap")) 
				//{
				//	trap_Cvar_Set ( "nv_enableThirdPerson", "2" );
				//	trap_Cvar_Update ( &nv_enableThirdPerson );
				//}	
#endif

				cv->modificationCount = cv->vmCvar->modificationCount;

				///03.22.07 - 03.46pm - update weapon mod file
				if (!Q_stricmp(cv->cvarName,"g_weaponModFile"))
					BG_InitWeaponStats( );
				///End
				// GodOrDevil - 05.15.05 - 08:34pm #cvarAnnouce #serverMode
				///if (!Q_stricmp ( cv->cvarName, "nv_serverMode" )) {
				///	if (!Q_stricmp ( cv->vmCvar->string, "0" )){
				///		trap_SendConsoleCommand( EXEC_INSERT, va("exec settings/publicmode.cfg\n" ) );
				///		GT (va("%i,^3PUBLIC ^7SETTINGS ^1LOADED", level.time + 5000));
				///	}
				///	else if (!Q_stricmp ( cv->vmCvar->string, "1" )){
				///		trap_SendConsoleCommand( EXEC_INSERT, va("exec settings/compmode.cfg\n" ) );
				///		GT (va("%i,^3COMPETITON ^7SETTINGS ^1LOADED", level.time + 5000));
				///	}
				///	else if (!Q_stricmp ( cv->vmCvar->string, "2" )){
				///		trap_SendConsoleCommand( EXEC_INSERT, va("exec settings/trainingmode.cfg\n" ) );
				///		GT (va("%i,^3TRAINING ^7SETTINGS ^1LOADED", level.time + 5000));
				///	}
				///	level.modeChangeSec = 2;
				///	NV_Sound(-1, "sound/misc/events/tut_door01.mp3" );
				///}
#ifdef _MAPEDITOR
				//if (!Q_stricmp(cv->cvarName,"nv_editMap") && !nv_editMap.integer)
				//{
				//	if ( !g_cheats.integer )
				//		continue;
				//}
#endif

				if (cv->trackChange){
					if(level.modeChangeSec){
						trap_SendServerCommand( -1, va("print \"Server: %s changed to %s\n\"", cv->cvarName, cv->vmCvar->string ) );
					}
					else {
						// GodOrDevil - 01.22.06 - 07:47pm
						char		cvar[64] = "\0";
						///End  - 01.22.06 - 07:47pm
						Com_sprintf(cvar, sizeof(cvar), "%s", cv->cvarName);
						level.modeChangeSec = 1;
						///NV_Sound(-1, "sound/misc/events/tut_door01.mp3" );
						///GT (va("%i,^SSETTING CHANGED^7: ^1%s^7: ^3%s", level.time + 5000, Q_strupr(cvar), cv->vmCvar->string));
						trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,^SSETTING CHANGED^7: ^1%s^7: ^3%s", level.time + 5000, Q_strupr(cvar), cv->vmCvar->string));
						RPM_GlobalSound(G_SoundIndex("sound/misc/events/tut_door01.mp3"));

					}
				}
				/// wut ?
				///else {
				///	trap_SendServerCommand( -1, va("print \"Server: %s changed to %s\n\"", cv->cvarName, cv->vmCvar->string ) );
				///}
				/// Original Code
				///if (cv->trackChange)
				///{
				///		trap_SendServerCommand( -1, va("print \"Server: %s changed to %s\n\"", cv->cvarName, cv->vmCvar->string ) );
				///}	
				///End  - 05.15.05 - 08:49pm


				//if (cv->teamShader)
				//{
				//	remapped = qtrue;
				//}
			}
		}
	}

	//if (remapped)
	//{
	//	G_RemapTeamShaders();
	//}
}


void G_SetGametype ( const char* gametype )
{	
	// Make sure the gametype is valid, if not default to deathmatch
	level.gametype = BG_FindGametype ( gametype );

	// First make sure its supported on this map
#ifdef _MAPEDITOR
	if(!nv_editMap.integer)
	{
#endif
		if ( !G_DoesMapSupportGametype ( gametype ) || level.gametype == -1 )
		{
			int i = 0;
			
			// Find a gametype it does support
			for ( i = 0; i < bg_gametypeCount; i ++ )
			{
				if ( G_DoesMapSupportGametype ( bg_gametypeData[i].name ) )
				{
					break;
				}
			}

			// This is bad, this means the map doesnt support any gametypes
			if ( i >= bg_gametypeCount )
			{
				Com_Error ( ERR_FATAL, "map does not support any of the available gametypes" );
			}

			G_LogPrintf ( "gametype '%s' is not supported on this map and was defaulted to '%s'\n", 
							gametype,
							bg_gametypeData[i].name );

			gametype = bg_gametypeData[i].name;
			trap_Cvar_Set( "g_gametype", gametype );
			level.gametype = BG_FindGametype ( gametype );
			
			trap_Cvar_Update( &g_gametype );


		// GodOrDevil - #gametype not supported, so reset nv type to the default
			NV_setTrueGameType(qfalse);
		//End

		}
#ifdef _MAPEDITOR
	}
#endif

	level.gametypeData = &bg_gametypeData[level.gametype];	

	// Copy the backpack percentage over
	bg_itemlist[MODELINDEX_BACKPACK].quantity = level.gametypeData->backpack;

	// Set the pickup state
	// GodOrDevil - GAMETYPES
	if (RMG.integer || g_pickupsDisabled.integer || level.gametypeData->pickupsDisabled || NV_isFT())
	//if (RMG.integer || g_pickupsDisabled.integer || level.gametypeData->pickupsDisabled )
	// END
	{
		level.pickupsDisabled = qtrue;
		trap_SetConfigstring ( CS_PICKUPSDISABLED, "1" );
	}
	else
	{
		level.pickupsDisabled = qfalse;
		trap_SetConfigstring ( CS_PICKUPSDISABLED, "0" );
	}
}


//void G_SetGametype(const char* gametype)
//{
//	NV_setTrueGameType(qfalse);
//
//	// Make sure the gametype is valid, if not default to deathmatch
//	level.gametype = BG_FindGametype(gametype);
//
//	level.gametypeData = &bg_gametypeData[level.gametype];
//
//	// Copy the backpack percentage over
//	bg_itemlist[MODELINDEX_BACKPACK].quantity = level.gametypeData->backpack;
//
//	// Set the pickup state
//	if (RMG.integer || g_pickupsDisabled.integer || level.gametypeData->pickupsDisabled || NV_isFT())
//	{
//		level.pickupsDisabled = qtrue;
//		trap_SetConfigstring(CS_PICKUPSDISABLED, "1");
//	}
//	else
//	{
//		level.pickupsDisabled = qfalse;
//		trap_SetConfigstring(CS_PICKUPSDISABLED, "0");
//	}
//}

/*
============
G_InitGame
============
*/
void G_InitGame( int levelTime, int randomSeed, int restart )
{
	int	i;

	Com_Printf ("------- Game Initialization -------\n");
	Com_Printf ("gamename: %s\n", GAMEVERSION);
	Com_Printf ("gamedate: %s\n", __DATE__);

	srand( randomSeed );

	// set some level globals
	memset( &level, 0, sizeof( level ) );
	level.time = levelTime;
	level.startTime = levelTime;

	// GodOrDevil - so cvar changes wont show when map restart/change
	level.modeChangeSec = 10;
	///End
	
	// GodOrDevil - RMG
	if ( !RMG.integer )
	{
		char temp[MAX_INFO_STRING];

		// start the music
		trap_Cvar_VariableStringBuffer("mapname", temp, MAX_QPATH);
		if(!strcmp(temp, "arioche/mp_small") || !strcmp(temp, "arioche/mp_medium") || !strcmp(temp, "arioche/small"))
		{
			trap_Cvar_Set("RMG", "1");
			trap_Cvar_VariableStringBuffer("RMG_sky", temp, MAX_QPATH);
			if(strlen(temp) < 4)
			{
				trap_Cvar_Set("RMG_sky", "rmg_sky_desert_day");
			}		
			trap_Cvar_VariableStringBuffer("RMG_fog", temp, MAX_QPATH);
			if(strlen(temp) < 4)
			{
				trap_Cvar_Set("RMG_fog", "textures/airport/lightfog");
			}	
		}	
	}
	// END 

	G_RegisterCvars();

	G_ProcessIPBans();

	// Load the list of arenas
	G_LoadArenas ( );

	// Build the gametype list so we can verify the given gametype
	BG_BuildGametypeList ( );

#ifdef _NEW_MAP_SETUP
	NV_setTrueGameType(restart);
#endif

	// Set the current gametype
	G_SetGametype(g_gametype.string);

	// Give the game a uniqe id
	trap_SetConfigstring ( CS_GAME_ID, va("%d", randomSeed ) );

	if ( g_log.string[0] )
	{
		if ( g_logSync.integer )
		{

//#ifdef _DEBUG // GodOrDevil - new log eerrday.
			if(!Q_stricmp(g_log.string, "date"))
			{
				char string[64] = "\0";
				qtime_t			  q;
				trap_RealTime	 (&q);
				Com_sprintf( string, sizeof(string), "logs\\game\\%i%02i%02i.log", q.tm_year+1900,1+q.tm_mon, q.tm_mday);
				trap_FS_FOpenFile( string, &level.logFile, FS_APPEND_SYNC_TEXT );
			}
			else
//#endif
			trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND_SYNC_TEXT );
		}
		else
		{
//#ifdef _DEBUG // GodOrDevil - new log eerrday.
		if(!Q_stricmp(g_log.string, "date"))
		{
			char string[64] =	"\0";
			qtime_t				q;
			trap_RealTime	(&q);
			Com_sprintf( string, sizeof(string), "logs\\game\\%i%02i%02i.log", q.tm_year+1900,1+q.tm_mon, q.tm_mday);
			trap_FS_FOpenFile( string, &level.logFile, FS_APPEND_TEXT );
		}
		else
//#endif
			trap_FS_FOpenFile( g_log.string, &level.logFile, FS_APPEND_TEXT );
		}

		if ( !level.logFile )
		{
			Com_Printf( "WARNING: Couldn't open logfile: %s\n", g_log.string );
		}
		else
		{
			char	serverinfo[MAX_INFO_STRING];

			trap_GetServerinfo( serverinfo, sizeof( serverinfo ) );

			G_LogPrintf("------------------------------------------------------------\n" );
			G_LogPrintf("InitGame: %s\n", serverinfo );
		}
	}
	else
	{
		Com_Printf( "Not logging to disk.\n" );
	}

	// GodOrDevil - Open the needed files
	trap_FS_FOpenFile( "logs\\adminlog.txt", &level.adminLogFile, FS_APPEND_TEXT );
	trap_FS_FOpenFile( "logs\\crashlog.txt", &level.crashLogFile, FS_APPEND_TEXT );
	///end

	G_InitWorldSession();

	// initialize all entities for this game
	memset( g_entities, 0, MAX_GENTITIES * sizeof(g_entities[0]) );
	level.gentities = g_entities;

	// initialize all clients for this game
	level.maxclients = g_maxclients.integer;
	memset( g_clients, 0, MAX_CLIENTS * sizeof(g_clients[0]) );
	level.clients = g_clients;

	// set client fields on player ents
	for ( i=0 ; i<level.maxclients ; i++ )
	{
		g_entities[i].client = level.clients + i;
	}

	// always leave room for the max number of clients,
	// even if they aren't all used, so numbers inside that
	// range are NEVER anything but clients
	level.num_entities = MAX_CLIENTS;

	// let the server system know where the entites are
	trap_LocateGameData( level.gentities,
						 level.num_entities,
						 sizeof( gentity_t ),
						 &level.clients[0].ps,
						 sizeof( level.clients[0] ) );

	// Get the boundaries of the world
	trap_GetWorldBounds ( level.worldMins, level.worldMaxs );

	// reserve some spots for dead player bodies
	G_InitBodyQueue();

	ClearRegisteredItems();

	// GodOrDevil - register effects for special shit
	// if its not a restart, load it, otherwise set the effects by number because they are already registered on the client
	if(!restart)
	{
		level.effects[EFFECT_GREEN_GAS]		= G_EffectIndex ( "levels/shop7_toxic_smoke.efx");
		level.effects[EFFECT_GREEN_FLASH]	= G_EffectIndex ( "levels/shop7_toxiic_explosion.efx");
		level.effects[EFFECT_FLARE_RED]		= G_EffectIndex ( "flare_red.efx");
		level.effects[EFFECT_FLARE_BLUE]	= G_EffectIndex ( "flare_blue.efx");
		//level.effects[EFFECT_AIR_STRIKE]	= G_EffectIndex ( "explosions/mushroom_explosion.efx");
		//level.effects[EFFECT_AIR_STRIKE_BIG] = G_EffectIndex( "explosions/finca4_explosions.efx");
		level.effects[EFFECT_AIR_STRIKE]	= G_EffectIndex ( "explosion_medium_noscorch.efx");
		level.effects[EFFECT_AIR_STRIKE_BIG] = G_EffectIndex( "explosions/mushroom_explosion.efx");
	}
	else
	{
		level.effects[EFFECT_GREEN_GAS]		= EFFECT_GREEN_GAS+1;
		level.effects[EFFECT_GREEN_FLASH]	= EFFECT_GREEN_FLASH+1;
		level.effects[EFFECT_FLARE_RED]		= EFFECT_FLARE_RED+1;
		level.effects[EFFECT_FLARE_BLUE]	= EFFECT_FLARE_BLUE+1;
		level.effects[EFFECT_AIR_STRIKE]	= EFFECT_AIR_STRIKE+1;
		level.effects[EFFECT_AIR_STRIKE_BIG] = EFFECT_AIR_STRIKE_BIG+1;
		//level.effects[EFFECT_AIR_STRIKE]	= G_EffectIndex ( "explosions/mushroom_explosion.efx");
		//level.effects[EFFECT_AIR_STRIKE_BIG] = G_EffectIndex( "explosions/finca4_explosions.efx");
	}
	//END
#ifdef _NEW_MAP_SETUP
	NV_getMapList((qboolean)restart);
#endif

	// parse the key/value pairs and spawn gentities
	G_SpawnEntitiesFromString(qfalse);

	// Now parse the gametype information that we need.  This needs to be
	// done after the entity spawn so that the items and triggers can be
	// linked up properly
	G_ParseGametypeFile ( );

	BG_ParseInviewFile();

	//Ryan april 14 2004 9:33pm
	//We'll load the weapon mods (if the server has it set)
	//right after we parse the inview files just to keep the theme
#ifdef _RPM_WEAPONMOD
	RPM_WeaponMod(qfalse, -1);
#endif
	//Ryan
	// Load in the identities
	BG_ParseNPCFiles ( );

	// general initialization
	G_FindTeams();

	SaveRegisteredItems();

	Com_Printf ("-----------------------------------\n");

	if( trap_Cvar_VariableIntegerValue( "com_buildScript" ) )
	{
		G_SoundIndex( "sound/player/gurp1.wav" );
		G_SoundIndex( "sound/player/gurp2.wav" );
	}

#ifdef _SOF2_BOTS
	if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) )
	{
		BotAISetup( restart );
		BotAILoadMap( restart );
		G_InitBots( restart );
	}
#endif

	// GodOrDevil - GAMETYPES
	Com_Printf ("-----------Loading Shaders-----------\n");
	// Freeze Tags get some shaders remade.. do it before Remap so customization can be done if wanted.
	if(NV_isTFT())
	{
		AddRemap("models/objects/armory/dummy.jpg", "gfx/misc/jk_flare_red", .001);
		AddRemap("models/objects/kamchatka/misc/chem_suit.jpg", "textures/common/water_pool_finca", .001);
		//AddRemap("models/objects/kamchatka/misc/chem_suit.jpg", "models/chunks/glass/glass", .001);
		//AddRemap("models/objects/Common/body_bag.jpg", "textures/colors/blue_light", .001);
		//trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
	}
	else if (NV_isFT())
	{
		//AddRemap("models/objects/armory/dummy.jpg", "gfx/menus/icons/icon_return_glow", .001);
		//AddRemap("models/objects/kamchatka/misc/chem_suit.jpg", "textures/common/water_pool_finca", .001);
		//AddRemap("models/objects/kamchatka/misc/chem_suit.jpg", "gfx/misc/bp_electric_line.jpg", .001);
		//AddRemap("models/objects/armory/dummy.jpg", "gfx/misc/bp_electric_line.jpg", .001);
		AddRemap("models/objects/armory/dummy.jpg", "textures/sprites/viral_tent_temp", .001);
	}
	else if(NV_isDem())
	{   
		AddRemap("models/objects/armory/virus", "textures/common/water_pool_finca", .001);
		//AddRemap("models/objects/jordan/misc/monitor_alpha", "textures/common/water_pool_finca", .001);
	}
	else if(NV_isSab())
	{
		AddRemap("models/objects/armory/virus", "textures/common/water_pool_finca", .001);
		AddRemap("models/objects/jordan/misc/monitor_alpha", "models/objects/shop/misc/helipad_glass", .001);
	}
	else if (NV_isRush())
	{
		//level.teamScores[TEAM_BLUE] = nv_maxLivesBlue.integer;
		//AddRemap("models/objects/armory/virus", "textures/common/water_pool_finca", .001);
		AddRemap("models/objects/jordan/misc/monitor_alpha", "models/objects/shop/misc/helipad_glass", .001);
		level.extras.currentPhase = 1;
	}	
	
	if(!NV_ParseNewShaders())
		G_RemapTeamShaders();
	// END

	// Set the available outfitting
	BG_SetAvailableOutfitting ( g_availableWeapons.string );

	// Initialize the gametype
	trap_GT_Init ( g_gametype.string, (qboolean)restart );
	
	// GodOrDevil - 02.07.09 
#ifdef _DB_QVM
	if (g_enableDatabase.integer)
	{
		level.usingDatabase = DB_CreateThread();
	}
#endif

	//End

	// Music
	if ( RMG.integer )
	{
		char temp[MAX_INFO_STRING];

		// start the music
		trap_Cvar_VariableStringBuffer("RMG_music", temp, MAX_QPATH);
		trap_SetConfigstring( CS_MUSIC, temp );
	}

	trap_SetConfigstring( CS_VOTE_TIME, "" );

	//Ryan april 11 2004 10:40am
	//Get the chat sounds ready for use
	RPM_ParseChatSounds();
	//Ryan

#ifdef _DB_ENABLED
	// get TOP stats and set the map_id
#ifdef _DB_STATS_DLL
	if (statsloaded.integer && !restart /*&& !level.warmupTime*/)
#else
	if (g_enableDatabase.integer && !restart /*&& !level.warmupTime*/)
#endif
	{
		NV_QueryDB (DB_LITE_INIT_NEW, nv_gtShort.string);	
	#ifdef _DEBUG		
		if(!restart && !level.warmupTime)
			NV_Log (5, "\n----------------------\n") ;		
	#endif
	}
#endif

}

/*
=================
G_ShutdownGame
=================
*/
void G_ShutdownGame( int restart )
{
	Com_Printf ("==== ShutdownGame ====\n");
	// GodOrDevil
	if ( level.adminLogFile )
		trap_FS_FCloseFile( level.adminLogFile );
	if ( level.crashLogFile )
		trap_FS_FCloseFile( level.crashLogFile );
	// End

	if ( level.logFile )
	{
		G_LogPrintf("ShutdownGame:\n" );
		G_LogPrintf("------------------------------------------------------------\n" );
		trap_FS_FCloseFile( level.logFile );
	}

	// write all the client session data so we can get it back
	G_WriteSessionData();
	
	// GodOrDevil
	if(!restart)
	{
		trap_SendConsoleCommand( EXEC_APPEND, va("seta alt \"0\";" ) );
	}
	// End

#ifdef _SOF2_BOTS
	if ( trap_Cvar_VariableIntegerValue( "bot_enable" ) )
	{
		BotAIShutdown( restart );
	}
#endif



}


#ifndef GAME_HARD_LINKED

void QDECL Com_Error ( int level, const char *fmt, ... )
{
	va_list		argptr;
	char		text[1024];
	// GodOrDevil - #crashlog
	char		mapname[MAX_QPATH];
	trap_Cvar_VariableStringBuffer ( "mapname", mapname, MAX_QPATH );
	///End

	va_start (argptr, fmt);
	vsprintf (text, fmt, argptr);
	va_end (argptr);

	// GodOrDevil - #crashlog
	NV_crashLog (va("%s -- map: %s - %s", text, mapname, g_gametype.string )) ;
	///End
	trap_Error( text );
}

void QDECL Com_Printf( const char *msg, ... )
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr, msg);
	vsprintf (text, msg, argptr);
	va_end (argptr);

	trap_Printf( text );
}

#endif

/*
=============
SortRanks
=============
*/
int QDECL SortRanks( const void *a, const void *b )
{
	gclient_t	*ca;
	gclient_t	*cb;

	ca = &level.clients[*(int *)a];
	cb = &level.clients[*(int *)b];

	// sort special clients last
	if ( ca->sess.spectatorState == SPECTATOR_SCOREBOARD )
	{
		return 1;
	}

	if ( cb->sess.spectatorState == SPECTATOR_SCOREBOARD )
	{
		return -1;
	}

	// then connecting clients
	if ( ca->pers.connected == CON_CONNECTING )
	{
		return 1;
	}

	if ( cb->pers.connected == CON_CONNECTING )
	{
		return -1;
	}

	// then spectators
	if ( ca->sess.team == TEAM_SPECTATOR && cb->sess.team == TEAM_SPECTATOR )
	{
		if ( ca->sess.spectatorTime < cb->sess.spectatorTime )
		{
			return -1;
		}
		if ( ca->sess.spectatorTime > cb->sess.spectatorTime )
		{
			return 1;
		}
		return 0;
	}

	if ( ca->sess.team == TEAM_SPECTATOR )
	{
		return 1;
	}

	if ( cb->sess.team == TEAM_SPECTATOR )
	{
		return -1;
	}

	// then sort by score
	if ( ca->sess.score > cb->sess.score )
	{
		return -1;
	}

	if ( ca->sess.score < cb->sess.score )
	{
		return 1;
	}

	// then sort by kills
	if ( ca->sess.kills > cb->sess.kills )
	{
		return -1;
	}

	if ( ca->sess.kills < cb->sess.kills )
	{
		return 1;
	}

	// then sort by deaths
	if ( ca->sess.deaths > cb->sess.deaths )
	{
		return -1;
	}

	if ( ca->sess.deaths < cb->sess.deaths )
	{
		return 1;
	}

	return 0;
}

/*
============
CalculateRanks

Recalculates the score ranks of all players
This will be called on every client connect, begin, disconnect, death,
and team change.
============
*/
void CalculateRanks( void )
{
	int			i;
	int			rank;
	int			score;
	int			newScore;
	gclient_t	*cl;

	level.follow1 = -1;
	level.follow2 = -1;
	level.numConnectedClients = 0;
	level.numNonSpectatorClients = 0;
	level.numPlayingClients = 0;
	level.numVotingClients = 0;		// don't count bots

	for ( i = 0 ; i < level.maxclients ; i++ )
	{
		if ( level.clients[i].pers.connected != CON_DISCONNECTED )
		{
			level.sortedClients[level.numConnectedClients] = i;
			level.numConnectedClients++;

			if ( level.clients[i].sess.team != TEAM_SPECTATOR )
			{
				level.numNonSpectatorClients++;

				// decide if this should be auto-followed
				if ( level.clients[i].pers.connected == CON_CONNECTED )
				{
					level.numPlayingClients++;
#ifdef _DEBUG
					if (1)
#else
					if ( !(g_entities[i].r.svFlags & SVF_BOT) )
#endif
					{
						level.numVotingClients++;
					}
					if ( level.follow1 == -1 )
					{
						level.follow1 = i;
					}
					else if ( level.follow2 == -1 )
					{
						level.follow2 = i;
					}
				}
			}
		}
	}

	qsort( level.sortedClients, level.numConnectedClients,
		   sizeof(level.sortedClients[0]), SortRanks );

	// set the rank value for all clients that are connected and not spectators
	if ( level.gametypeData->teams )
	{
		int rank;
		if ( level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE] )
		{
			rank = 2;
		}
		else if ( level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE] )
		{
			rank = 0;
		}
		else
		{
			rank = 1;
		}

		// in team games, rank is just the order of the teams, 0=red, 1=blue, 2=tied
		for ( i = 0;  i < level.numConnectedClients; i++ )
		{
			cl = &level.clients[ level.sortedClients[i] ];
			cl->ps.persistant[PERS_RANK] = rank;
		}
	}
	else
	{
		rank = -1;
		score = 0;
		for ( i = 0;  i < level.numPlayingClients; i++ )
		{
			cl = &level.clients[ level.sortedClients[i] ];
			newScore = cl->sess.score;
			if ( i == 0 || newScore != score )
			{
				rank = i;
				// assume we aren't tied until the next client is checked
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank;
			}
			else
			{
				// we are tied with the previous client
				level.clients[ level.sortedClients[i-1] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
				level.clients[ level.sortedClients[i] ].ps.persistant[PERS_RANK] = rank | RANK_TIED_FLAG;
			}
			score = newScore;
		}
	}


	// see if it is time to end the level
	CheckExitRules();

	// if we are at the intermission, send the new info to everyone
	//Ryan june 15 2003
		//if ( level.intermissiontime )
	if ( level.intermissiontime || level.pause)
	//Ryan
	{
		SendScoreboardMessageToAllClients();
	}
}


/*
========================================================================

MAP CHANGING

========================================================================
*/

/*
========================
SendScoreboardMessageToAllClients

Do this at BeginIntermission time and whenever ranks are recalculated
due to enters/exits/forced team changes
========================
*/
void SendScoreboardMessageToAllClients( void ) {
	int		i;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if( g_entities[i].r.svFlags & SVF_BOT)
				continue;
		if ( level.clients[ i ].pers.connected == CON_CONNECTED ) {
			DeathmatchScoreboardMessage( g_entities + i );
		}
	}
}

/*
========================
MoveClientToIntermission

When the intermission starts, this will be called for all players.
If a new client connects, this will be called after the spawn function.
========================
*/
void MoveClientToIntermission( gentity_t *ent )
{
	// take out of follow mode if needed
	if ( ent->client->sess.spectatorState == SPECTATOR_FOLLOW )
	{
		G_StopFollowing( ent );
	}

	// move to the spot
	VectorCopy( level.intermission_origin, ent->s.origin );
	VectorCopy( level.intermission_origin, ent->client->ps.origin );
	VectorCopy (level.intermission_angle, ent->client->ps.viewangles);

	// Reset some client variables
	ent->client->ps.pm_type		  = PM_INTERMISSION;
	ent->client->ps.stats[STAT_GAMETYPE_ITEMS] = 0;
	ent->client->ps.eFlags		  = 0;
	ent->s.eFlags				  = 0;
	ent->s.eType				  = ET_GENERAL;
	ent->s.modelindex			  = 0;
	ent->s.loopSound			  = 0;
	ent->s.event				  = 0;
	ent->r.contents				  = 0;
}

/*
==================
FindIntermissionPoint

This is also used for spectator spawns
==================
*/
void FindIntermissionPoint( void )
{
	gentity_t	*ent, *target;
	vec3_t		dir;

	// find the intermission spot
	ent = G_Find (NULL, FOFS(classname), "info_player_intermission");
	if ( !ent )
	{
		gspawn_t* spawn = G_SelectRandomSpawnPoint ( -1 );
		if ( spawn )
		{
			VectorCopy (spawn->origin, level.intermission_origin);
			VectorCopy (spawn->angles, level.intermission_angle);
		}

		return;
	}

	VectorCopy (ent->s.origin, level.intermission_origin);
	VectorCopy (ent->s.angles, level.intermission_angle);

	// if it has a target, look towards it
	if ( ent->target )
	{
		target = G_PickTarget( ent->target );
		if ( target )
		{
			VectorSubtract( target->s.origin, level.intermission_origin, dir );
			vectoangles( dir, level.intermission_angle );
		}
	}
}

/*
==================
BeginIntermission
==================
*/
void BeginIntermission( void )
{
	int			i;
	gentity_t	*ent;

	if ( level.intermissiontime )
	{
		// already active
		return;
	}

	// Kill any votes
	level.voteTime = 0;
	trap_SetConfigstring( CS_VOTE_TIME, "" );

	level.intermissiontime  = level.time;
	level.gametypeResetTime = level.time;
	FindIntermissionPoint();

#ifdef _DB_ENABLED
	NV_SaveMapStats();
#endif

	// move all clients to the intermission point
	for (i=0 ; i< level.maxclients ; i++)
	{
		ent = g_entities + i;
		if (!ent->inuse)
		{
			continue;
		}

		//Ryan april 10 2004
		//total these up for the awards
		if(ent->client->sess.team == TEAM_SPECTATOR)
		{
			ent->client->sess.totalSpectatorTime += level.time - ent->client->sess.spectatorTime;
		}
		else if(ent->client->sess.ghostStartTime)
		{
			ent->client->sess.totalSpectatorTime += level.time - ent->client->sess.ghostStartTime;
		}
		//Ryan

		// take out of follow mode
		G_StopFollowing( ent );

		// Get rid of ghost state
		G_StopGhosting ( ent );

		// respawn if dead
		if ( G_IsClientDead ( ent->client ) )
		{
			respawn ( ent );
		}

		MoveClientToIntermission( ent );
	}

	// send the current scoring to all clients
	SendScoreboardMessageToAllClients();
}


/*
=============
ExitLevel

When the intermission has been exited, the server is either killed
or moved to a new level based on the "nextmap" cvar

=============
*/
void ExitLevel (void)
{
	int			i;
	gclient_t	*cl;

	///Ryan march 21 2004 9:19am
	if ((!*g_mapcycle.string || !Q_stricmp ( g_mapcycle.string, "none" )) &&
		g_fastRestart.integer)
	{
		trap_SendServerCommand( -1, va("cp \"@ \n\""));
		trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
		return;
	}
	///Ryan

#ifdef _SOF2_BOTS
	trap_SendConsoleCommand( EXEC_APPEND, va("kick allbots; wait 50;\n") );
#endif

	// GodOrDevil - 03.28.05 - 02:52pm #nextMap
	///if cvar set by rcon, make sure its a vaid map, if not clear it.
	if (Q_stricmp ( g_setnextMap.string, "none" )){
		if (!G_DoesMapExist ( g_setnextMap.string )){
			trap_Cvar_Set("g_setnextMap", "none");
		}
	}
	///If its not none.. set it as the map
	if (Q_stricmp ( g_setnextMap.string, "none" ) /*&& !g_setnextMap.string == "none"*/){
		if(g_nextScore.integer)
		{
			trap_SendConsoleCommand( EXEC_APPEND, va("exec settings/default.cfg; scorelimit \"%i\"; map \"%s\"\n", g_nextScore.integer,  g_setnextMap.string ) );
		}
		else
		{
			//trap_SendConsoleCommand( EXEC_APPEND, va("map \"%s\"\n", g_setnextMap.string ) );
			trap_SendConsoleCommand( EXEC_APPEND, va("exec settings/default.cfg; map \"%s\"\n", g_setnextMap.string ) );
		}

		trap_Cvar_Set("g_setnextMap", "none");
		trap_Cvar_Set("g_nextScore", "");
		return;
	}
	///End  - 03.28.05 - 10:00pm
	
#ifdef _COMPMODE
	if(g_compMode.integer == 2)
	{
		RPM_KickAll();
	}	
#endif

	// GodOrDevil - Check random map
#ifdef _NEW_MAP_SETUP
	
	if(level.doRandom /*|| !Q_stricmp ( g_mapcycle.string, "random" )*/)
	{
		// Do the random, if it cant be done, just mapcycle it
		if(!NV_doRandomMap())
			trap_SendConsoleCommand( EXEC_APPEND, "mapcycle\n" );
	}
	else
#endif
	{
		trap_SendConsoleCommand( EXEC_APPEND, "mapcycle\n" );
	}
	// End

	level.changemap = NULL;
	level.intermissiontime = 0;


	// reset all the scores so we don't enter the intermission again
	level.teamScores[TEAM_RED] = 0;
	level.teamScores[TEAM_BLUE] = 0;
	for ( i=0 ; i< g_maxclients.integer ; i++ )
	{
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED )
		{
			continue;
		}

		cl->sess.score = 0;
		cl->pers.ready = 0;
		cl->ps.persistant[PERS_SCORE] = 0;
	}

	// we need to do this here before chaning to CON_CONNECTING
	G_WriteSessionData();

	// change all client states to connecting, so the early players into the
	// next level will know the others aren't done reconnecting
	for (i=0 ; i< g_maxclients.integer ; i++)
	{
		if ( level.clients[i].pers.connected == CON_CONNECTED )
		{
			level.clients[i].pers.connected = CON_CONNECTING;
		}
	}

}

/*
=================
G_LogPrintf

Print to the logfile with a time stamp if it is open
=================
*/
void QDECL G_LogPrintf( const char *fmt, ... ) {
	va_list		argptr;
	char		string[1024];

	// GodOrDevil - 04.28.06 - 03:01am #timeLog
	///int			min, tens, sec;
   ///End  - 04.28.06 - 03:015am


	///CJJ - 1.1.2005 - Logging the time #logging
	qtime_t			q;
	trap_RealTime	(&q);
	///END - 1.1.2005

	// GodOrDevil - 04.28.06 - 03:01am #timeLog
	///sec = level.time / 1000;
	///min = sec / 60;
	///sec -= min * 60;
	///tens = sec / 10;
	///sec -= tens * 10;

	///Com_sprintf( string, sizeof(string), "%4i:%i%i ", min, tens, sec );
   Com_sprintf( string, sizeof(string), "%02i/%02i/%i %02i:%02i - ", 1+q.tm_mon,q.tm_mday, q.tm_year+1900,q.tm_hour,q.tm_min);
   ///End  - 04.28.06 - 03:01am

	va_start( argptr, fmt );
	// GodOrDevil - 04.28.06 - 03:01am #timeLog
	///vsprintf( string +8 , fmt,argptr );
	vsprintf( string + 19 , fmt,argptr );
	///End  - 04.28.06 - 03:02am
	va_end( argptr );

#ifndef _DEBUG
   if ( g_dedicated.integer ) {
#endif
		// GodOrDevil - 04.28.06 - 03:02am #timelog
		///Com_Printf( "%s", string + 8 );
		Com_Printf( "%s", string + 19 );
		///End  - 04.28.06 - 03:02am

#ifndef _DEBUG
	}
#endif

	if ( !level.logFile ) {
		return;
	}

	trap_FS_Write( string, strlen( string ), level.logFile );
}

/* 
================
LogExit

Append information about this game to the log file
================
*/
void LogExit( const char *string )
{
	int				i;
	int				numSorted;
	gclient_t		*cl;

	G_LogPrintf( "Exit: %s\n", string );

	// GodOrDevil - remind users to take a screenshot for SC tourny
//#ifdef _COMPMODE
//	if(g_compMode.integer)
//	{
//		trap_SendServerCommand( -1, va("chat -1 \"^6GAME ^3OVER^7:  ^2Remember to take a ^3screenshot ^2of the final scores!\n\""));
//	}
//#endif
	level.intermissionQueued = level.time;

	// this will keep the clients from playing any voice sounds
	// that will get cut off when the queued intermission starts
	trap_SetConfigstring( CS_INTERMISSION, "1" );

	// don't send more than 32 scores (FIXME?)
	numSorted = level.numConnectedClients;
	if ( numSorted > 32 )
	{
		numSorted = 32;
	}

	if ( level.gametypeData->teams )
	{
		G_LogPrintf( "red:%i  blue:%i\n", level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE] );
	}

	for (i=0 ; i < numSorted ; i++)
	{
		int	ping;

		cl = &level.clients[level.sortedClients[i]];

		if ( cl->sess.team == TEAM_SPECTATOR )
		{
			continue;
		}

		if ( cl->pers.connected == CON_CONNECTING )
		{
			continue;
		}

		ping = cl->ps.ping < 999 ? cl->ps.ping : 999;

		//if(g_compMode.integer)
		//{
		//	RPM_WriteFinalScores(cl);
		//}
		G_LogPrintf( "score: %i  ping: %i  client: %i %s\n", cl->sess.score, ping, level.sortedClients[i],	cl->pers.netname );

	}
}


///*
//================
//LogExit
//
//Append information about this game to the log file
//================
//*/
//void LogExit( const char *string ) 
//{
//	int				i;
//	int				numSorted;
//	gclient_t		*cl;
//
//	G_LogPrintf( "Exit: %s\n", string );
//
//	level.intermissionQueued = level.time;
//
//	// this will keep the clients from playing any voice sounds
//	// that will get cut off when the queued intermission starts
//	trap_SetConfigstring( CS_INTERMISSION, "1" );
//
//	// don't send more than 32 scores (FIXME?)
//	numSorted = level.numConnectedClients;
//	if ( numSorted > 32 ) 
//	{
//		numSorted = 32;
//	}
//
//	if ( level.gametypeData->teams ) 
//	{
//		G_LogPrintf( "red:%i  blue:%i\n", level.teamScores[TEAM_RED], level.teamScores[TEAM_BLUE] );
//	}
//
//	for (i=0 ; i < numSorted ; i++) 
//	{
//		int	ping;
//
//		cl = &level.clients[level.sortedClients[i]];
//
//		if ( cl->sess.team == TEAM_SPECTATOR ) 
//		{
//			continue;
//		}
//
//		if ( cl->pers.connected == CON_CONNECTING ) 
//		{
//			continue;
//		}
//
//		ping = cl->ps.ping < 999 ? cl->ps.ping : 999;
//
//		G_LogPrintf( "score: %i  ping: %i  client: %i %s\n", cl->sess.score, ping, level.sortedClients[i],	cl->pers.netname );
//	}
//}



/*
=================
CheckIntermissionExit

The level will stay at the intermission for a minimum of 5 seconds
If all players wish to continue, the level will then exit.
If one or more players have not acknowledged the continue, the game will
wait 10 seconds before going on.
=================
*/
void CheckIntermissionExit( void )
{
//Ryan
	if ( !level.exitTime )
	{
		level.exitTime = level.time;
	}

#ifdef _COMPMODE
	if(g_compMode.integer) // show scoreboard to 10 seconds in compmode 
	{
		if ( level.time < level.exitTime + 10000)
		{
			return;
		}
	}
	else 
#endif
	if ( level.time < level.exitTime + 5000 )
	{
		return;
	}

	if(!level.awardTime)
	{
		RPM_Awards();
		level.awardTime = level.time;
		level.lastAwardSent = level.time;
		return;
	}

	if(level.awardTime && (level.time > level.lastAwardSent + 3000))
	{
		RPM_Awards();
		level.lastAwardSent = level.time;
	}
#ifdef _COMPMODE
	if(g_compMode.integer) // show awards longer longer in comp mode;
	{
		if(level.time < level.awardTime + 20000)
		{
			return;
		}
	} else 
#endif
	if(level.time < level.awardTime + 15000)
	{
		return;
	}
#ifdef _DB_ENABLED
	if(g_enableDatabase.integer)
		NV_QueryDB (DB_LITE_MYSQLUPDATES, "");	
#endif
	ExitLevel();
}
/*
	int			ready, notReady;
	int			i;
	gclient_t	*cl;
	int			readyMask;

	// see which players are ready
	ready = 0;
	notReady = 0;
	readyMask = 0;
	for (i=0 ; i< g_maxclients.integer ; i++) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		if ( g_entities[cl->ps.clientNum].r.svFlags & SVF_BOT ) {
			continue;
		}

		if ( cl->readyToExit ) {
			ready++;
			if ( i < 16 ) {
				readyMask |= 1 << i;
			}
		} else {
			notReady++;
		}
	}

	// copy the readyMask to each player's stats so
	// it can be displayed on the scoreboard
	for (i=0 ; i< g_maxclients.integer ; i++) {
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED ) {
			continue;
		}
		cl->ps.stats[STAT_CLIENTS_READY] = readyMask;
	}

	// never exit in less than five seconds
	if ( level.time < level.intermissiontime + 5000 ) {
		return;
	}

	// if nobody wants to go, clear timer
	if ( !ready ) {
		level.readyToExit = qfalse;
		return;
	}

	// if everyone wants to go, go now
	if ( !notReady ) {
		ExitLevel();
		return;
	}

	// the first person to ready starts the ten second timeout
	if ( !level.readyToExit ) {
		level.readyToExit = qtrue;
		level.exitTime = level.time;
	}

	// if we have waited ten seconds since at least one player
	// wanted to exit, go ahead
	if ( level.time < level.exitTime + 10000 ) {
		return;
	}

	ExitLevel();
}
*/
//Ryan

/*
=============
ScoreIsTied
=============
*/
qboolean ScoreIsTied( void )
{
	int	a;
	int b;

	if ( level.numPlayingClients < 2 )
	{
		return qfalse;
	}

	if ( level.gametypeData->teams )
	{
		return level.teamScores[TEAM_RED] == level.teamScores[TEAM_BLUE];
	}

	a = level.clients[level.sortedClients[0]].sess.score;
	b = level.clients[level.sortedClients[1]].sess.score;

	return a == b;
}

/*
=================
CheckExitRules

There will be a delay between the time the exit is qualified for
and the time everyone is moved to the intermission spot, so you
can see the last frag.
=================
*/
/*
void CheckExitRules( void )
{
 	int			i;
	gclient_t	*cl;

	// if at the intermission, wait for all non-bots to
	// signal ready, then go to next level
	if ( level.intermissiontime )
	{
		CheckIntermissionExit ();
		return;
	}

	if ( level.intermissionQueued )
	{
		int time = INTERMISSION_DELAY_TIME;
		if ( level.time - level.intermissionQueued >= time )
		{
			level.intermissionQueued = 0;
			BeginIntermission();
		}

		return;
	}

	// GodOrDevil - 04.29.05 - 07:19pm - #Endgame cmd
	// check for sudden death
	if ( ScoreIsTied() && !g_allowTie.integer )
	{
		// always wait for sudden death
		return;
	}

	//trap_Cvar_Set("g_allowTie",	 "0"); //SC logfile #stats
	//End  - 04.29.05 - 07:19pm

	// Check to see if the timelimit was hit
	if ( g_timelimit.integer && !level.warmupTime )
	{


		// GodOrDevil - 04.21.06 - 07:33pm #vote - call time extend vote
		if ( level.gametypeData->respawnType != RT_NONE || level.gametypeResetTime )
		{
			if(g_autoVoteTimeExtend.integer && level.time - level.startTime + 1000*60 == g_timelimit.integer * 60000){
				///if ( level.numConnectedClients > 1 && level.numVotingClients >= 2 && !level.voteTime){
				if (level.numVotingClients > 1 && !level.voteTime){
					trap_SendServerCommand( -1, va("print \"Auto vote: Extend Time\n\"") );
					Com_sprintf ( level.voteString, sizeof(level.voteString ), "extendtime %d", g_timeextension.integer );
					Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), "Extend timelimit by %d minutes", g_timeextension.integer );
					level.voteTime = level.time;
					level.voteYes = 0;
					level.voteNo = 0;

					for ( i = 0 ; i < level.maxclients ; i++ )
						level.clients[i].ps.eFlags &= ~EF_VOTED;

					trap_SetConfigstring( CS_VOTE_TIME, va("%i,%i", level.voteTime, g_voteDuration.integer*1000 ) );
					trap_SetConfigstring( CS_VOTE_STRING, level.voteDisplayString );	
					trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
					trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );	
					trap_SetConfigstring( CS_VOTE_NEEDED, va("%i", level.numVotingClients / 2) );
				}
			}
		}
		///End  - 04.21.06 - 07:50pm

		if ( level.time - level.startTime >= (g_timelimit.integer + level.timeExtension)*60000 )
		{
			gentity_t* tent;
			tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
			tent->s.eventParm = GAME_OVER_TIMELIMIT;
			tent->r.svFlags = SVF_BROADCAST;

			LogExit( "Timelimit hit." );
			return;
		}
	}

	// Check to see if the score was hit
	if ( g_scorelimit.integer )
	{
		if ( level.gametypeData->teams )
		{
			// GodOrDevil - 05.03.05 - 03:11am #halfTime
			////if (count == 0){
			////	if (( level.teamScores[TEAM_RED] >= g_scorelimit.integer / 2) ||	
			////		( level.teamScores[TEAM_BLUE] >= g_scorelimit.integer / 2)){
			////			trap_SendConsoleCommand( EXEC_INSERT, va("swapteams\n" ) );
			////			Com_Printf("****HALF-TIME****\n");
			////			count++;
			////	}
			////}
			if ( level.teamScores[TEAM_RED] >= g_scorelimit.integer )
			{
				gentity_t* tent;
				tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
				tent->s.eventParm = GAME_OVER_SCORELIMIT;
				tent->r.svFlags = SVF_BROADCAST;
				tent->s.otherEntityNum = TEAM_RED;

				LogExit( "Red team hit the score limit." );
				return;
			}

			if ( level.teamScores[TEAM_BLUE] >= g_scorelimit.integer )
			{
				gentity_t* tent;
				tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
				tent->s.eventParm = GAME_OVER_SCORELIMIT;
				tent->r.svFlags = SVF_BROADCAST;
				tent->s.otherEntityNum = TEAM_BLUE;

				LogExit( "Blue team hit the score limit." );
				return;
			}
		}
		else
		{
			// Check to see if any of the clients scores have crossed the scorelimit
			for ( i = 0 ; i < level.numConnectedClients ; i++ )
			{
				cl = g_entities[level.sortedClients[i]].client;

				if ( cl->pers.connected != CON_CONNECTED )
				{
					continue;
				}

				if ( cl->sess.team != TEAM_FREE )
				{
					continue;
				}

				if ( cl->sess.score >= g_scorelimit.integer )
				{
					gentity_t* tent;
					tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
					tent->s.eventParm = GAME_OVER_SCORELIMIT;
					tent->r.svFlags = SVF_BROADCAST;
					tent->s.otherEntityNum = level.sortedClients[i];

					LogExit( "Scorelimit hit." );
					return;
				}
			}
		}

	}
}
*/

// GodOrDevil - GAMETYPES - EXIT RULES
void CheckExitRules( void ) 
{
 	int			i;
	gclient_t	*cl;

	// if at the intermission, wait for all non-bots to
	// signal ready, then go to next level
	if ( level.intermissiontime ) 
	{
		CheckIntermissionExit ();
		return;
	}

	if ( level.intermissionQueued ) 
	{
		int time = INTERMISSION_DELAY_TIME;
		if ( level.time - level.intermissionQueued >= time ) 
		{
			level.intermissionQueued = 0;
			BeginIntermission();
		}

		return;
	}

	// Check to see if the timelimit was hit
	if ( g_timelimit.integer /*&& !level.warmupTime */) 
	{
		// GodOrDevil - #remaining
		if(!level.warmupTime)
		{
			if(level.time - level.startTime + 2000*60 == (g_timelimit.integer  + level.timeExtension )* 60000)
				GT(va("%i,^12 ^3MINUTES REMAINING!", level.time + 5000));
			else  if(level.time - level.startTime + 1000*60 == (g_timelimit.integer  + level.timeExtension )* 60000)
				GT(va("%i,^11 ^3MINUTE REMAINING!", level.time + 5000));
			else  if(level.time - level.startTime + 1000*30 == (g_timelimit.integer  + level.timeExtension )* 60000)
				GT(va("%i,^130 ^3SECONDS REMAINING", level.time + 5000));
			
			///#vote - call time extend vote
			//Com_Printf("%i  %i\n", level.time - level.startTime + 60000, (g_timelimit.integer -1) * 60000);
#ifdef _COMPMODE
			if(!level.voteTime && g_compMode.integer == 0 && level.numConnectedClients > 1 && level.numVotingClients >= 2)
#else
			if(!level.voteTime && level.numConnectedClients > 1 && level.numVotingClients >= 2)
#endif
			{

				if(g_autoVoteTimeExtend.integer && level.time - level.startTime + 1000*60 == (g_timelimit.integer - 0.25) * 60000)
				{					
					trap_SendServerCommand( -1, va("print \"Auto vote called.\n\"") );
					Com_sprintf ( level.voteString, sizeof(level.voteString ), "extendtime %d", g_timeextension.integer );
					Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), "Extend timelimit by %d minutes", g_timeextension.integer );
					level.voteTime = level.time;
					level.voteYes = 0;
					level.voteNo = 0;

					for ( i = 0 ; i < level.maxclients ; i++ )
					{
						level.clients[i].ps.eFlags &= ~EF_VOTED;
					}

					trap_SetConfigstring( CS_VOTE_TIME, va("%i,%i", level.voteTime, g_voteDuration.integer*1000 ) );
					trap_SetConfigstring( CS_VOTE_STRING, level.voteDisplayString );	
					trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
					trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );	
					trap_SetConfigstring( CS_VOTE_NEEDED, va("%i", level.numVotingClients / 2) );
				
#ifdef _DB_ENABLED // Vote called sound
					RPM_GlobalSound(G_SoundIndex("sound/movers/buttons/button02.mp3"));
#endif
				}
				else if(g_enableVoteNextMap.integer && level.time - level.startTime == 120000)
				{
					NV_voteNextMap();
				}
				//else if(g_enableVoteSkipNextMap.integer && level.time - level.startTime + 3000*60 == (g_timelimit.integer - 0.25) * 60000)
				//else if(g_enableVoteSkipNextMap.integer && level.time - level.startTime == (g_timelimit.integer * 60000) / 2)
				else if(g_enableVoteSkipNextMap.integer )
				{
					NV_voteSkipNextMap();
				}
			}
		}
		///End  - 04.21.06 - 07:50pm
		/// GodOrDevil - Dont do timelimit if INF/ELIM - Updated: removed this because it was making the timelimit not be hit in sab (possibly inf?).  My code btw ...
		//if ( level.time > level.gametypeRoundTime )
		//{
		////END
			if ( level.time - level.startTime >= (g_timelimit.integer + level.timeExtension)*60000 ) 
			{	
				gentity_t* tent;
				
				if(NV_isTctf())
				{
					if(level.extras.roundWinsBlue > level.extras.roundWinsRed )
					{
						tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
						tent->s.eventParm = GAME_OVER_SCORELIMIT;
						tent->r.svFlags = SVF_BROADCAST;	
						tent->s.otherEntityNum = TEAM_BLUE;
						level.teamScores[TEAM_BLUE] = level.extras.roundWinsBlue;
						level.teamScores[TEAM_RED] = level.extras.roundWinsRed;
						LogExit( "Blue team Wins." );
#ifdef _DB_ENABLED
						mapStats.winner = TEAM_BLUE;
#endif // _DB
						return;
					}
					if(level.extras.roundWinsRed > level.extras.roundWinsBlue)
					{
						tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
						tent->s.eventParm = GAME_OVER_SCORELIMIT;
						tent->r.svFlags = SVF_BROADCAST;	
						tent->s.otherEntityNum = TEAM_RED;
						level.teamScores[TEAM_BLUE] = level.extras.roundWinsBlue;
						level.teamScores[TEAM_RED] = level.extras.roundWinsRed;
						LogExit( "Red team Wins." );
#ifdef _DB_ENABLED
						mapStats.winner = TEAM_RED;
#endif // _DB
						return;
					}
				}

				tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
				tent->s.eventParm = GAME_OVER_TIMELIMIT;
				tent->r.svFlags = SVF_BROADCAST;	
				LogExit( "Timelimit hit." );
#ifdef _DB_ENABLED
				//mapStats.winner = -1;
				if ( level.gametypeData->teams )
				{
					if(level.teamScores[TEAM_BLUE] > level.teamScores[TEAM_RED])
						mapStats.winner = TEAM_BLUE;
					else if(level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE])
						mapStats.winner = TEAM_RED;
					else  
						mapStats.winner = -1;
				}
#endif // _DB
				return;
			}
		}
	//}

	// GodOrDevil - #rush - dont bother checking scorelimits in rush.
	if(NV_isRush() /*|| level.warmupTime*/)
	{
		return;
	}

	// Check to see if the score was hit
	if ( g_scorelimit.integer && !level.warmupTime) 
	{		
		if ( level.gametypeData->teams )
		{
			if ( level.teamScores[TEAM_RED] >= g_scorelimit.integer ) 
			{
				gentity_t* tent;
				tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
				tent->s.eventParm = GAME_OVER_SCORELIMIT;
				tent->r.svFlags = SVF_BROADCAST;	
				tent->s.otherEntityNum = TEAM_RED;
				level.extras.roundWinsRed++;
				LogExit( "Red team hit the score limit." );
#ifdef _DB_ENABLED
				mapStats.winner = TEAM_RED;
#endif // _DB
				return;
			}

			if ( level.teamScores[TEAM_BLUE] >= g_scorelimit.integer ) 
			{
				gentity_t* tent;
				tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
				tent->s.eventParm = GAME_OVER_SCORELIMIT;
				tent->r.svFlags = SVF_BROADCAST;	
				tent->s.otherEntityNum = TEAM_BLUE;
				level.extras.roundWinsBlue++;
				LogExit( "Blue team hit the score limit." );
#ifdef _DB_ENABLED
				mapStats.winner = TEAM_BLUE;
#endif // _DB
				return;
			}
		}
		else
		{
			// GodOrDevil
			if(NV_isGG())
			{
				for ( i = 0 ; i < level.numConnectedClients ; i++ ) 
				{
					cl = g_entities[level.sortedClients[i]].client;

					if ( cl->pers.connected != CON_CONNECTED )
					{
						continue;
					}

					if ( cl->sess.team != TEAM_FREE ) 
					{
						continue;
					}
					
					if(cl->pers.rank >= level.topSpreeReward)
					{
						gentity_t* tent;
						tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
						tent->s.eventParm = GAME_OVER_SCORELIMIT;
						tent->r.svFlags = SVF_BROADCAST;	
						tent->s.otherEntityNum = level.sortedClients[i];
						LogExit( "Scorelimit hit." );
#ifdef _DB_ENABLED
						mapStats.winner = level.sortedClients[i];
#endif // _DB
						return;
					}
				}
				return;
			}
			// End


			// Check to see if any of the clients scores have crossed the scorelimit
			for ( i = 0 ; i < level.numConnectedClients ; i++ ) 
			{
				cl = g_entities[level.sortedClients[i]].client;

				if ( cl->pers.connected != CON_CONNECTED )
				{
					continue;
				}

				if ( cl->sess.team != TEAM_FREE ) 
				{
					continue;
				}

				if ( cl->sess.score >= g_scorelimit.integer ) 
				{
					gentity_t* tent;
					tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
					tent->s.eventParm = GAME_OVER_SCORELIMIT;
					tent->r.svFlags = SVF_BROADCAST;	
					tent->s.otherEntityNum = level.sortedClients[i];
					LogExit( "Scorelimit hit." );
#ifdef _DB_ENABLED
					mapStats.winner = level.sortedClients[i];
#endif // _DB
					return;
				}
			}
		}
	}
}

// END


/*
=============
CheckWarmup
=============
*/
void CheckWarmup ( void )
{
	int			counts[TEAM_NUM_TEAMS];
	qboolean	notEnough = qfalse;

	// check because we run 3 game frames before calling Connect and/or ClientBegin
	// for clients on a map_restart
	if ( level.numPlayingClients == 0 )
	{
		return;
	}

	if ( !level.warmupTime  )
	{
		return;
	}

	//Ryan   if there are any players that are not ready do no start the warmup countdown untill they are
#ifdef _STUPID_SHIT
	if (g_doWarmup.integer == 2 && level.warmupTime < 0)
	{
		int		i, ready = 0;

		for ( i = 0; i < level.numPlayingClients; i ++ )
		{
			if (g_entities[level.sortedClients[i]].client->sess.team == TEAM_SPECTATOR)
			{
				continue;
			}

			if ( g_entities[level.sortedClients[i]].client->pers.connected != CON_CONNECTED )
			{
				continue;
			}

			if (!g_entities[level.sortedClients[i]].client->pers.ready)
			{
				continue;
			}

			ready++;
		}
		if( (float)ready / (float)level.numPlayingClients * 100 < g_readyPercent.integer)
		{
			return;
		}
	}
#endif
	//Ryan

	if ( level.gametypeData->teams )
	{
		counts[TEAM_BLUE] = TeamCount( -1, TEAM_BLUE, NULL );
		counts[TEAM_RED] = TeamCount( -1, TEAM_RED, NULL );

		if (counts[TEAM_RED] < 1 || counts[TEAM_BLUE] < 1)
		{
			notEnough = qtrue;
		}
	}
	else if ( level.numPlayingClients < 2 )
	{
		notEnough = qtrue;
	}

	//Ryan if only 1 player and the swap_teams vote passes let it execute
	//if(level.swapteams)
	//{
	//	notEnough = qfalse;
	//}
	//Ryan

	if ( notEnough )
	{
		if ( level.warmupTime != -1 )
		{
			level.warmupTime = -1;
			trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
			G_LogPrintf( "Warmup:\n" );
		}

		return; // still waiting for team members
	}

	if ( level.warmupTime == 0 )
	{
		return;
	}

	// if the warmup is changed at the console, restart it
	if ( g_warmup.modificationCount != level.warmupModificationCount )
	{
		level.warmupModificationCount = g_warmup.modificationCount;
		level.warmupTime = -1;
	}

	// if all players have arrived, start the countdown
	if ( level.warmupTime < 0 )
	{
		// fudge by -1 to account for extra delays
		level.warmupTime = level.time + ( g_warmup.integer - 1 ) * 1000;
		trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
		return;
	}

	// if the warmup time has counted down, restart
	if ( level.time > level.warmupTime )
	{
		level.warmupTime += 10000;
		trap_Cvar_Set( "g_restarted", "1" );
		trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
		level.restarted = qtrue;
		level.extras.roundSwapCount = 0;
		level.extras.roundCount = 0;
		level.extras.roundWinsBlue = 0;
		level.extras.roundWinsRed = 0;

		// GodOrDevil - GAMETYPES #TCTF
		if(NV_isTctf())
		{
			level.extras.roundNumber = 0;
		}
		else
		{
			level.extras.roundNumber = 1;
		}
		// END
		return;
	}
}

/*
==================
CheckVote
==================
*/
void CheckVote( void )
{
	if ( level.voteExecuteTime && level.voteExecuteTime < level.time )
	{
		level.voteExecuteTime = 0;
		//Ryan
#ifdef _COMPMODE
		if(strstr (level.voteDisplayString, "UnReferee"))
		{
			int n = atoi (level.voteString);
			if(g_entities[n].client->nvSess.referee)
			{
				g_entities[n].client->nvSess.referee = 0;
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,^3%s is no longer a Referee", level.time + 5000, g_entities[n].client->pers.netname ) );
				return;
			}
		}
		else if(strstr (level.voteDisplayString, "Referee"))
		{
			int n = atoi (level.voteString);
			if(!g_entities[n].client->nvSess.referee)
			{
				g_entities[n].client->nvSess.referee = 1;
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,^3%s is now a Referee", level.time + 5000, g_entities[n].client->pers.netname ) );
				return;
			}
		}
		else if(strstr (level.voteString, "swap_teams"))
		{
			//level.swapteams = qtrue;

			//if(g_doWarmup.integer)
			//{
			//	trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );
			//}
			//else
			//{
			//	trap_SendConsoleCommand( EXEC_APPEND, "map_restart 3\n" );
			//}
			SC_SwapTeams( NULL);
			//NV_swapTeams(0, NULL, -99);
			return;
		}
		else if(strstr (level.voteString, "start_match"))
		{
			RPM_ReadyAll();
			return;
		}
		//Ryan
		else
#endif
		{
			trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", level.voteString ) );
			return;
		}
	}

	if ( !level.voteTime )
	{
		return;
	}



	// GodOrDevil
		if(!Q_strncmp (level.voteString, "setRandom", 9))
		{
			
#ifdef _DB_ENABLED
			mapStats.voteYes = level.voteYes;
			mapStats.voteNo = level.voteNo;
#endif
#ifdef _MAPVOTE_MOST
		//	trap_SetConfigstring ( CS_VOTE_NEEDED, 0 );
#endif
		}
#ifdef _MAPVOTE_MOST
		//else 
#endif

	
	// Update the needed clients
	trap_SetConfigstring ( CS_VOTE_NEEDED, va("%i", (level.numVotingClients / 2) + 1 ) );
	/// GodOrDevil - update this for people joining after its called.
	//trap_SetConfigstring( CS_VOTE_TIME, va("%i,%i", level.voteTime, g_voteDuration.integer*1000 ) );
	//trap_SetConfigstring( CS_VOTE_STRING, level.voteDisplayString );	
	//trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
	//trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );


	if ( level.time - level.voteTime >= g_voteDuration.integer*1000 )
	{			
#ifdef _NEW_MAP_SETUP

		if(!Q_strncmp (level.voteString, "setRandom", 9))
		{	
#ifdef _MAPVOTE_MOST
			if ( level.voteYes > level.voteNo  )
			{
				trap_SendServerCommand( -1, va("print \"^5More voted YES!  Vote passed:  Next - [%s] %s\n\"",  level.randGameType, level.randMap) );
				level.voteExecuteTime = level.time + 2000;
			}
			else
			{
				trap_SendServerCommand( -1, va("print \"^5Vote Failed:  Next - [%s] %s\n\"",  level.nextGameType, level.nextMap) );
			}
#else
			trap_SendServerCommand( -1, va("print \"^5Vote Failed:  Next - [%s] %s\n\"",  level.nextGameType, level.nextMap) );
#endif
			// End

		}
		else
#endif
		{
			trap_SendServerCommand( -1, "print \"^5Vote failed.\n\"" );
		}
		level.clients[level.voteClient].voteDelayTime = level.time + g_failedVoteDelay.integer * 60000;
	}
	else
	{
		if ( level.voteYes > level.numVotingClients/2 )
		{
			// execute the command, then remove the vote
#ifdef _NEW_MAP_SETUP
			if(!Q_strncmp (level.voteString, "setRandom", 9))
			{	
				trap_SendServerCommand( -1, va("print \"^5Vote passed:  Next - [%s] %s\n\"",  level.randGameType, level.randMap) );
			}
			else
#endif
			{
				trap_SendServerCommand( -1, "print \"^5Vote passed.\n\"" );
			}
			level.voteExecuteTime = level.time + 3000;
				

			//Ryan if a team swap  change the teams and use the 3 second vote pass delay
			//to avoid any problems
			//if(strstr (level.voteString, "swap_teams"))
			//{
			//	int i;
			//	gentity_t *ent;

			//	level.voteExecuteTime = level.time;
			//	for ( i = 0; i < level.numPlayingClients; i ++ )
			//	{
			//		ent = &g_entities[level.sortedClients[i]];

			//		if (ent->client->sess.team == TEAM_SPECTATOR)
			//		{
			//			continue;
			//		}

			//		if ( ent->client->pers.connected != CON_CONNECTED )
			//		{
			//			continue;
			//		}

			//		if (ent->client->sess.team == TEAM_RED)
			//		{
			//			ent->client->sess.team = TEAM_BLUE;
			//		}
			//		else if(ent->client->sess.team == TEAM_BLUE)
			//		{
			//			ent->client->sess.team = TEAM_RED;
			//		}
			//	}
			//}
			////Ryan
		}
		else if ( level.voteNo >= level.numVotingClients/2 )
		{
			// same behavior as a timeout
#ifdef _NEW_MAP_SETUP

			if(!Q_strncmp (level.voteString, "setRandom", 9))
			{
				trap_SendServerCommand( -1, va("print \"^5Vote Failed:  Next - [%s] %s\n\"",  level.nextGameType, level.nextMap) );
			}
			else
#endif
			{
				trap_SendServerCommand( -1, "print \"^5Vote failed.\n\"" );
			}
			level.clients[level.voteClient].voteDelayTime = level.time + g_failedVoteDelay.integer * 60000;
		}
		else
		{
			// still waiting for a majority
			return;
		}
	}

	level.voteTime = 0;
	trap_SetConfigstring( CS_VOTE_TIME, "" );
}

/*
==================
PrintTeam
==================
*/
void PrintTeam(int team, char *message)
{
	int i;

	for ( i = 0 ; i < level.maxclients ; i++ )
	{
		if (level.clients[i].sess.team != (team_t)team)
		{
			continue;
		}

		trap_SendServerCommand( i, message );
	}
}

/*
==================
CheckCvars
==================
*/
void CheckCvars( void )
{
	static int lastMod = -1;

	if ( g_password.modificationCount != lastMod )
	{
		lastMod = g_password.modificationCount;
		if ( *g_password.string && Q_stricmp( g_password.string, "none" ) )
		{
			trap_Cvar_Set( "g_needpass", "1" );
		} else
		{
			trap_Cvar_Set( "g_needpass", "0" );
		}
	}
}

/*
=============
G_RunThink

Runs thinking code for this frame if necessary
=============
*/
void G_RunThink (gentity_t *ent)
{
	float	thinktime;
	thinktime = ent->nextthink;

	if (thinktime <= 0)
	{
		return;
	}
	if (thinktime > level.time)
	{
		return;
	}

#ifdef _DB_STATS_DLL
	if (!statsloaded.integer)
	{
		LoadStats();
	}
#endif

	ent->nextthink = 0;
	if (!ent->think)
	{
		Com_Error ( ERR_FATAL, "NULL ent->think");
	}
	ent->think (ent);
}

/*
#ifdef _DEBUG
static int thirtysecs = 10000;
#endif
*/
/*
================
G_RunFrame

Advances the non-player objects in the world
================
*/
#ifdef _CRS_BOTS
void WP_Think(void);
#endif
void G_RunFrame( int levelTime )
{
	int			i;
	gentity_t	*ent;
	int			msec;
	// GodOrDevil - GAMETYPES
	qboolean	isRush;
	// END

	//NT - store the time the frame started
	level.frameStartTime = trap_Milliseconds();

	// if we are waiting for the level to restart, do nothing
	if ( level.restarted )
	{
		return;
	}

	level.framenum++;
	level.previousTime = level.time;
	level.time = levelTime;
	msec = level.time - level.previousTime;
	
#ifdef _COMPMODE
	//Ryan
	if(level.pause)
	{
		// GodOrDevil - 08.30.06 - 03:33pm #paused - add to level.startTime so pausing wont take away from timelimit
		level.startTime += msec;
		if(level.gametypeRoundTime)
			level.gametypeRoundTime += msec;
		///End  - 08.30.06 - 03:34pm

		if(level.unpausetime)
		{
			RPM_Unpause(NULL);
		}
	}	
	//Ryan
#endif


	// get any cvar changes
	G_UpdateCvars();

	// go through all allocated objects
	ent = &g_entities[0];

	// GodOrDevil - GAMETYPES
	isRush = NV_isRush();
	// END

	for (i=0 ; i<level.num_entities ; i++, ent++)
	{
		if ( !ent->inuse )
		{
			continue;
		}

		// GodOrDevil - GAMETYPES - #RUSH #Phase - unlink all GT Triggers that dont match up to the phase.
		if ( ent->s.eType == ET_GRAPPLE && isRush) 
		{
			if(ent->spawnflags != level.extras.currentPhase && ent->r.linked)
			{
				trap_UnlinkEntity( ent );
			}
			else if(!ent->r.linked && ent->spawnflags == level.extras.currentPhase)
			{
				if(ent->armTime != 999999)
					trap_LinkEntity( ent );
			}
		}
		// END
		// GodOrDevil - 07.30.06 - 03:33pm #BLOCKED_TRIGGER
		if(ent->model && ent->model != NULL && (ent->s.eType == ET_TERRAIN || ent->s.eType == ET_MOVER || ent->s.eType == ET_TELEPORT_TRIGGER) && !strcmp(ent->model, "BLOCKED_TRIGGER"))
		{
			if(ent->count)
			{		
				// GodOrDevil - GAMETYPES #RUSH
				if(isRush)
				{
				
#ifdef _MAPEDITOR
					if(nv_editMap.integer)
					{
						if(ent->count < nv_editMap.integer)
						{
							if (ent->r.linked)
							{
								trap_UnlinkEntity( ent );
							}
						}
						if(ent->count >= nv_editMap.integer)
						{
							if (!ent->r.linked)
							{
								trap_LinkEntity( ent );
							}
						}
					}
#endif
					if(ent->count < level.extras.currentPhase)
					{
						if (ent->r.linked)
						{
							trap_UnlinkEntity( ent );
						}
					}
				}
				// END
				else
				{
					///Team Games
					if(level.gametypeData->teams)
					{
						if(	ent->count <= (TeamCount( -1, TEAM_RED, NULL )) && ent->count <= (TeamCount( -1, TEAM_BLUE, NULL )))
						{
							if (ent->r.linked)	
							{
								trap_UnlinkEntity( ent );
								if(ent->message != NULL)
								{
									CP(va("cp \"%s\n\"", ent->message));
								}
							}
						}
						else if(!ent->r.linked)
						{
							trap_LinkEntity( ent );
							if(ent->message2 != NULL)
							{
								CP(va("cp \"%s\n\"", ent->message2));
							}
						}
					}
					///Non-Team Games
					else if(ent->count >= level.numPlayingClients)
					{
						if (ent->r.linked)
						{
							trap_UnlinkEntity( ent );
							if(ent->message != NULL)
							{
								CP(va("cp \"%s\n\"", ent->message));
							}
						}
					}
					else if(!ent->r.linked)
					{
						trap_LinkEntity( ent );
						if(ent->message2 != NULL)
						{
							CP(va("cp \"%s\n\"", ent->message2));
						}
					}
				}
			}
			continue;
		}	
	///End  - 07.30.06 - 03:33pm


		// clear events that are too old
		if ( level.time - ent->eventTime > EVENT_VALID_MSEC )
		{
			if ( ent->s.event )
			{
				ent->s.event = 0;	// &= EV_EVENT_BITS;
				if ( ent->client )
				{
					ent->client->ps.externalEvent = 0;
					// predicted events should never be set to zero
					//ent->client->ps.events[0] = 0;
					//ent->client->ps.events[1] = 0;
				}
			}

			if ( ent->freeAfterEvent )
			{
				// tempEntities or dropped items completely go away after their event
				G_FreeEntity( ent );
				continue;
			}
			else if ( ent->unlinkAfterEvent )
			{
				// items that will respawn will hide themselves after their pickup event
				ent->unlinkAfterEvent = qfalse;
				trap_UnlinkEntity( ent );
			}
		}

		// temporary entities don't think
		if ( ent->freeAfterEvent )
		{
			continue;
		}

		if ( !ent->r.linked && ent->neverFree )
		{
			continue;
		}

		if ( ent->s.eType == ET_MISSILE )
		{
			G_RunMissile( ent );
			continue;
		}

		if ( ent->s.eType == ET_ITEM || ent->physicsObject )
		{
			G_RunItem( ent );
			continue;
		}

		if ( ent->s.eType == ET_MOVER )
		{
			G_RunMover( ent );
			continue;
		}

		if ( i < MAX_CLIENTS )
		{
			G_CheckClientTimeouts ( ent );
			G_RunClient( ent );
			G_CheckClientTeamkill ( ent );
			continue;
		}

		G_RunThink( ent );
	}

	// perform final fixups on the players
	ent = &g_entities[0];
	for (i=0 ; i < level.maxclients ; i++, ent++ )
	{
		if ( ent->inuse )
		{
			ClientEndFrame( ent );
		}
	}
#ifdef _CRS_BOTS
	// KRIS 8/04/2003 1:39PM
	if (g_cheats.integer)
	{
		WP_Think();
	}
	// KRIS
#endif

		
	// GodOrDevil - GAMETYPES #misc checks
	NV_checkMisc();
	// End  - 05.11.05 - 04:38am

#ifdef _MAPEDITOR   /// dont check warmup shit.. better then restarting each time someone joins
	if(!nv_editMap.integer) // Skip warmup and exit checks when mapeditor is enabled
	{	
		// Check warmup rules
		CheckWarmup();
		// see if it is time to end the level
		CheckExitRules();
	}
#else	
	// Check warmup rules
	CheckWarmup();
	// see if it is time to end the level
	CheckExitRules();
#endif
	// Check warmup rules

	//Ryan Dec 3 2003
	RPM_UpdateTMI();
	//Ryan

	// Update gametype stuff
	CheckGametype ();

	trap_GT_RunFrame ( level.time );

	// cancel vote if timed out
	CheckVote();

	// for tracking changes
	CheckCvars();
	
	if (g_listEntity.integer)
	{
		for (i = 0; i < MAX_GENTITIES; i++)
		{
			Com_Printf("%4i: %s\n", i, g_entities[i].classname);
		}
		trap_Cvar_Set("g_listEntity", "0");
	}
}

void G_InitGhoul ( void )
{
	G_InitHitModels ( );
}

void G_ShutdownGhoul ( void )
{
	if ( !level.serverGhoul2 )
	{
		return;
	}

	trap_G2API_CleanGhoul2Models ( &level.serverGhoul2 );
	level.serverGhoul2 = NULL;
}


