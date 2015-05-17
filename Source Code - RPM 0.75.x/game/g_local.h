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
// g_local.h -- local definitions for game module

#include "q_shared.h"
#include "bg_public.h"
#include "g_public.h"
#include "../gametype/gt_public.h"
#include "../ghoul2/g2.h"

#ifdef _DB_ENABLED
#include "./nv/nv_database.h"
#endif

// GodOrDevil 
#define CP(x)		trap_SendServerCommand(-1, x)					/// Print to all
#define CPe(x)		trap_SendServerCommand(ent-g_entities, x)		/// Print to an ent
#define CPx(x, y)	trap_SendServerCommand(x, y)					/// Print to id = x
#define CPa(x)		trap_SendServerCommand(adm-g_entities, x)		/// Print to an adm ent
#define GT(x)		trap_SetConfigstring(CS_GAMETYPE_MESSAGE, x)	/// Reg Gametype Message
#define CPc(x)		trap_SendServerCommand(client-g_entities, x)	/// Same as ent, but uses client ...
#define MAX_PHASES			4
#define EFFECT_GREEN_GAS 0
#define	EFFECT_GREEN_FLASH 1
#define	EFFECT_FLARE_RED 2
#define	EFFECT_FLARE_BLUE 3
#define	EFFECT_AIR_STRIKE 4
#define	EFFECT_AIR_STRIKE_BIG 5


#define MAX_KILLSPREE_REWARDS 20
#define MAX_PHASES			4
#define	EFFECT_MAX 10


///end

//==================================================================

// the "gameversion" client command will print this plus compile date
#define	GAMEVERSION	"sof2mp"

#define BODY_QUEUE_SIZE_MAX			MAX_CLIENTS
#define BODY_QUEUE_SIZE				8

//#ifndef _DLL_HOOK // used for dll hook only
//#define INFINITE					1000000
//#endif

#define Q3_INFINITE					16777216

#define	FRAMETIME					100					// msec

#define	INTERMISSION_DELAY_TIME		3000

// gentity->flags
#define	FL_GODMODE					0x00000010
#define	FL_NOTARGET					0x00000020
#define	FL_TEAMSLAVE				0x00000400	// not the first on the team
#define FL_NO_KNOCKBACK				0x00000800
#define FL_DROPPED_ITEM				0x00001000
#define FL_NO_BOTS					0x00002000	// spawn point not for bot use
#define FL_NO_HUMANS				0x00004000	// spawn point just for bots

#define	MAX_SPAWNS					128

// movers are things like doors, plats, buttons, etc
typedef enum
{
	MOVER_POS1,
	MOVER_POS2,
	MOVER_1TO2,
	MOVER_2TO1,
} moverState_t;

typedef struct gentity_s gentity_t;
typedef struct gclient_s gclient_t;

struct gentity_s
{
	entityState_t	s;				// communicated by server to clients
	entityShared_t	r;				// shared by both the server system and game

	// DO NOT MODIFY ANYTHING ABOVE THIS, THE SERVER
	// EXPECTS THE FIELDS IN THAT ORDER!
	//================================

	struct gclient_s	*client;			// NULL if not a client

	qboolean	inuse;

	char		*classname;			// set in QuakeEd
	int			spawnflags;			// set in QuakeEd

	qboolean	neverFree;			// if true, FreeEntity will only unlink
									// bodyque uses this

	int			flags;				// FL_* variables

	char		*model;
	char		*model2;
	int			freetime;			// level.time when the object was freed

	int			eventTime;			// events will be cleared EVENT_VALID_MSEC after set
	qboolean	freeAfterEvent;
	qboolean	unlinkAfterEvent;

	qboolean	physicsObject;		// if true, it can be pushed by movers and fall off edges
									// all game items are physicsObjects,
	float		physicsBounce;		// 1.0 = continuous bounce, 0.0 = no bounce
	int			clipmask;			// brushes with this content value will be collided against
									// when moving.  items and corpses do not collide against
									// players, for instance

	// movers
	moverState_t moverState;
	int			soundPos1;
	int			sound1to2;
	int			sound2to1;
	int			soundPos2;
	int			soundLoop;
	gentity_t	*parent;
	gentity_t	*nextTrain;
	vec3_t		pos1, pos2;

	char		*message;

	int			timestamp;		// body queue sinking, etc

	float		angle;			// set in editor, -1 = up, -2 = down
	char		*target;
	char		*targetname;
	char		*team;
	char		*targetShaderName;
	char		*targetShaderNewName;
	gentity_t	*target_ent;

	float		speed;
	vec3_t		movedir;
	float		mass;
	int			setTime;

//Think Functions
	int			nextthink;
	void		(*think)(gentity_t *self);
	void		(*reached)(gentity_t *self);	// movers call this when hitting endpoint
	void		(*blocked)(gentity_t *self, gentity_t *other);
	void		(*touch)(gentity_t *self, gentity_t *other, trace_t *trace);
	void		(*use)(gentity_t *self, gentity_t *other, gentity_t *activator);
	void		(*pain)(gentity_t *self, gentity_t *attacker, int damage);
	void		(*die)(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod, int hitLocation, vec3_t hitDir );

	int			pain_debounce_time;
	int			fly_sound_debounce_time;	// wind tunnel
	int			last_move_time;

//Health and damage fields
	int			health;
	qboolean	takedamage;

	int			damage;
	int			dflags;
	int			splashDamage;	// quad will increase this without increasing radius
	int			splashRadius;
	int			methodOfDeath;
	int			splashMethodOfDeath;

	int			count;
	float		bounceScale;

	gentity_t	*chain;
	gentity_t	*enemy;
	gentity_t	*activator;
	gentity_t	*teamchain;		// next entity in team
	gentity_t	*teammaster;	// master of the team

	int			watertype;
	int			waterlevel;

	int			noise_index;

	// timing variables
	float		wait;
	float		random;
	int			delay;

	gitem_t		*item;			// for bonus items

	// GodOrDevil - 03.10.05 - 05:30pm #antiCamping
	vec3_t camperOrigin;
	///End  - 03.10.05 - 05:31pm
	// GodOrDevil - 03.10.05 - 05:30pm 
	char		*message2;
	char		*bspmodel;
	
	float		distance;		// GodOrDevil - 03.13.07 - 07:53pm #rotating
	vec3_t		savedAngles;	/// Angles are cleared after load.. keeps doors from fucking up
	vec3_t		apos1;			/// Door angle for reverse rotations
	vec3_t		apos2;

	int			armTime;		// time to arm the bombs
	int			disarmTime;		// time needed to disarm bombs

	///End  - 03.10.05 - 05:31pm



};

typedef struct gspawn_s
{
	team_t		team;
	vec3_t		origin;
	vec3_t		angles;
	// GodOrDevil #dom
	int			linkedEnt;		// GodOrDevil - #dom - ent the spawn point links too.  So we can change the spawn team based on ents
	int			phase;
} gspawn_t;

typedef enum
{
	CON_DISCONNECTED,
	CON_CONNECTING,
	CON_CONNECTED

} clientConnected_t;

typedef enum
{
	SPECTATOR_NOT,
	SPECTATOR_FREE,
	SPECTATOR_FOLLOW,
	SPECTATOR_SCOREBOARD

} spectatorState_t;

typedef enum
{
	TEAM_BEGIN,		// Beginning a team game, spawn at base
	TEAM_ACTIVE		// Now actively playing

} playerTeamStateState_t;

typedef struct
{
	playerTeamStateState_t	state;

	int			location;

	int			captures;
	int			basedefense;
	int			carrierdefense;
	int			flagrecovery;
	int			fragcarrier;
	int			assists;

	float		lasthurtcarrier;
	float		lastreturnedflag;
	float		flagsince;
	float		lastfraggedcarrier;

} playerTeamState_t;

// the auto following clients don't follow a specific client
// number, but instead follow the first two active players
#define	FOLLOW_ACTIVE1	-1
#define	FOLLOW_ACTIVE2	-2

// client data that stays across multiple levels or map restarts
// this is achieved by writing all the data to cvar strings at game shutdown
// time and reading them back at connection time.  Anything added here
// MUST be dealt with in G_InitSessionData() / G_ReadSessionData() / G_WriteSessionData()
typedef struct
{
	team_t				team;					// current team
	int					spectatorTime;			// for determining next-in-line to play
	spectatorState_t	spectatorState;
	int					spectatorClient;		// for chasecam and follow mode
	int					score;					// total score
	int					kills;					// number of kills
	int					deaths;					// number of deaths
	qboolean			ghost;					// Whether or not the player is a ghost (sticks past team change)
	int					teamkillDamage;			// amount of damage death to teammates
	int					teamkillForgiveTime;	// time when team damage will be forgivin
	qboolean			noTeamChange;			// cant change teams when this is true (rt_none only)

	///Ryan april 5 2003
	qboolean			mute;					// if set to 1 this client is muted
	qboolean			nokick;					// if set to 1 this player cant be kicked
	qboolean			firstTime;				// if set to 1 this is the client's first time connecting
	int					motdStartTime;			// the time the server motd will start being sent
	int					motdStopTime;			// time the server motd will stop being sent
	qboolean			invitedByRed;
	qboolean			invitedByBlue;
	float				rpmClient;
	qboolean			spectatorFirstPerson;
	int					totalSpectatorTime;
	int					ghostStartTime;
	
	// GodOrDevil - 02.04.05 - 09:01am
	int					penaltySeconds;         /// holds the duration of the penalty in seconds
	char				penaltyReason[40];		/// holds the reason the player is in the penalty box
	int					msgSeconds;				/// ... Stay on the screen for a few seconds.
	char				msgReason[200];			 
	int					oneSecChecks;
	int					lastSpamTime;
	int					warnCount;					//Warn counter, counts up to a penalty....
	int					tkCount;					// 03.01.05 - 09:58pm - TeamKill Counter
	int					oldStatus;					// 03.02.05 - 12:25am - Stores admin status for suspending
	qboolean			fileChecked;				//used to check admin only at connect (or name change?) instead of team change etc...
#ifdef _CLANMEMBER
	qboolean			clanMember;	
#endif
	//qboolean			drop;					//used for drop cmd if fallingdamage is off

#ifdef _TORTURE
	int					burnSeconds;			//used for burning them w/knockback

	///#Warn message - 02.04.05 - 02:22pm
	int					warnSeconds;			/// holds the duration of the penalty in seconds
	char				warnReason[40];			/// holds the reason the player is in the penalty box
	
	///#Torture - 03.02.05 - 08:53pm 
	qboolean			torture;					// cant change teams when this is true (rt_none only)
	int					tortureCheckTimer;
	char				tortureReason[40];		/// holds the reason the player is being tortured
#endif

	///03.10.05 - 05:30pm #antiCamping
	int					camperSeconds;
	//int					camperCount;

	int	lastIdentityChange;						///Keeps track of last identity change time so we can limit it every 5 seconds

	char				country[3];
	
} clientSession_t;


//
//Ryan
#define MAX_IP				16
#define MAX_RPM_ID			64
//Ryan

#define MAX_NETNAME			36
#define MAX_IDENTITY		64
#define	MAX_VOTE_COUNT		3

// client data that stays across multiple respawns, but is cleared
// on each level change or team change at ClientBegin()
typedef struct
{
	clientConnected_t	connected;
	usercmd_t			cmd;						// we would lose angles if not persistant
	qboolean			localClient;				// true if "ip" info key is "localhost"
	qboolean			initialSpawn;				// the first spawn should be at a cool location
	qboolean			predictItemPickup;			// based on cg_predictItems userinfo
	qboolean			pmoveFixed;					//
	qboolean			antiLag;					// anti-lag on or off
	qboolean			autoReload;					// auto rellaod turned on or off
	char				netname[MAX_NETNAME];
	char				deferredname[MAX_NETNAME];
	int					netnameTime;				// Last time the name was changed
	TIdentity*			identity;					// Current identity
	goutfitting_t		outfitting;					// Current outfitting
	int					enterTime;					// level.time the client entered the game
	playerTeamState_t	teamState;					// status in teamplay games
	int					voteCount;					// to prevent people from constantly calling votes
	int					firemode[MAX_WEAPONS];		// weapon firemodes

	qboolean			camped;

	//Ryan april 7 2003
	qboolean			planted;
	statinfo_t			statinfo;					//the structure that holds this clients stats
	char				talkname[MAX_NETNAME];		//the clients talkname for teamgames
	int					ready;
	int					readyMessageTime;			// used for displaying the please ready up message every 3 seconds
	char				rpm_id[MAX_RPM_ID];
	//Ryan

	// GodOrDevil
	// Stores weapons for #TFT game types
	int					ammo[MAX_AMMO];
	int					clip[ATTACK_MAX][MAX_WEAPONS];
	int					weapons;
	int					currentWeapon;	
	int					rank; // GG
	// End

} clientPersistant_t;
/*
typedef struct
{
	char				ip[MAX_IP];
	char				ip2[MAX_IP];
	char				cleanName[MAX_NETNAME];		//a copy of the clients name without colors	
	int					admin;					// if set to 1 this clien is an admin
	int					ban;
	float				banTime;
	char				bannedBy[36];
	char				reason[24];
	qboolean			scrim;
	int					dbID;
	int					dbnameID;
	//int					stats[DB_MAX_TOP_STATS];
} dbConnectInfo_t;
*/
// GodOrDevil - nvSess

#ifdef _PLAYERSTATS
typedef enum 
{
	PSTATS_HS,
	PSTATS_HIT,
	PSTATS_SHOT,	
	
	PSTATS_WIN,
	PSTATS_LOSS,
	PSTATS_TE,
	PSTATS_SK,
	PSTATS_CAP,
	PSTATS_DEF,
	PSTATS_SC,
	PSTATS_KI,
	PSTATS_DE,
	PSTATS_PTIME,
	PSTATS_A1,
	PSTATS_A2,
	PSTATS_A3,
	PSTATS_A4,
	PSTATS_A5,
	PSTATS_A6,
	PSTATS_A7,
	PSTATS_KS,
	PSTATS_KR,
	PSTATS_RE,
	PSTATS_SU,
	PSTATS_BP,

	PSTATS_MAX
} playerStats_t;
#endif
typedef struct 
{
	//dbConnectInfo_t		db;
	int					capturing;					/// #dom capturing (or defending) a dom flag
	int					frozen;
	int					frozenTime;
	int					frozenFiveSecTimer;
	vec3_t				frozenOrigin;
	int					useLastTimeMsg;
	int					useCoolDown;
	int					spawnKillCount;				// #SPAWNKILLING
	int					spawnKillLast;				// last time they spawn killed
	int					timeLastsound;
	int					chatAbuse;
	int					teamTime[TEAM_NUM_TEAMS];	
	int					referee;
	int					vip;
	int					vipLastTime;
#ifdef _SPECIAL_NADES
	int					gasTime;					// last time to touch the gas
	int					gasOwner;					// He who dealt it.
#endif
	
#ifdef _MAPEDITOR
	int					editEnt;					///Map Editor: closes entity selected.	
#endif

#ifdef _KILLSPREE
	int					timeLastKill;
	int					timeLastKSsound;
#endif // _KILLSPREE
	
#ifdef _SPREEREWARD
	int					killSpreeItem[MAX_KILLSPREE_REWARDS];
	int					killSpreeClip[ATTACK_MAX][MAX_KILLSPREE_REWARDS];
	int					killSpreeExtraClips[ATTACK_MAX][MAX_KILLSPREE_REWARDS];
#endif // _SPREEREWARD
	
	int					earnedWeapons;
	
	char				ip[MAX_IP];
	char				ip2[MAX_IP];
	char				cleanName[MAX_NETNAME];		//a copy of the clients name without colors	
	int					admin;					// if set to 1 this clien is an admin
	int					ban;
	float				banTime;
	char				bannedBy[36];
	char				reason[32];
	qboolean			scrim;
	int					dbID;
	int					dbnameID;
	//int					stats[DB_MAX_TOP_STATS];
	int					noNameChange;
	int					teamlock;
	
	int					speed;	
	qboolean			slow;
	qboolean			stripped;
	//qboolean			planted;
	qboolean			drop;
	
#ifdef _TORTURE
	///#Torture - 03.02.05 - 08:53pm 
	qboolean			torture;					// cant change teams when this is true (rt_none only)
	int					tortureCheckTimer;
	char				tortureReason[40];		/// holds the reason the player is being tortured
	int					tortureCount;				//03.04.05 - 03:42am - used to get next punishment in torture
#endif

#ifdef _ZOMBIES
	qboolean			zombie;
#endif

#ifdef _PLAYERSTATS
	//int			hs;
	//int			hits, shots;
	//int			wins, losses;
	//int			sc;
	//int			team;		// most team played on this map
	//int			def, cap;
	int			playerStats[PSTATS_MAX];
#endif


} nvSession_t;
//End


#define MAX_SERVER_FPS		40
#define MAX_ANTILAG			MAX_SERVER_FPS

// Antilag information
typedef struct gantilag_s
{
	vec3_t	rOrigin;				// entity.r.currentOrigin
	vec3_t	rAngles;				// entity.r.currentAngles
	vec3_t	mins;					// entity.r.mins
	vec3_t	maxs;					// entity.r.maxs

	vec3_t	legsAngles;				// entity.client.ghoulLegsAngles
	vec3_t	lowerTorsoAngles;		// entity.client.ghoulLowerTorsoAngles
	vec3_t	upperTorsoAngles;		// entity.client.ghoulUpperTorsoAngles
	vec3_t	headAngles;				// entity.client.ghoulHeadAngles

	int		time;					// time history item was saved
	int		leveltime;

	int		torsoAnim;				// entity.client.ps.torsoAnim
	int		legsAnim;				// entity.client.ps.legsAnim

	float	leanTime;				// entity.client.ps.leanOffset
	int		pm_flags;				// entity.client.ps.pm_flags

} gantilag_t;


// this structure is cleared on each ClientSpawn(),
// except for 'client->pers' and 'client->sess'
struct gclient_s
{
	// ps MUST be the first element, because the server expects it
	playerState_t	ps;				// communicated by server to clients

	// the rest of the structure is private to game
	clientPersistant_t	pers;
	clientSession_t		sess;
		
	// GodOrDevil - 05.18.06 - 04:15pm
	nvSession_t			nvSess;
	///End  - 05.18.06 - 04:15pm

	qboolean	readyToExit;		// wishes to leave the intermission

	qboolean	noclip;

	int			lastCmdTime;		// level.time of last usercmd_t, for EF_CONNECTION
									// we can't just use pers.lastCommand.time, because
									// of the g_sycronousclients case
	int			buttons;
	int			oldbuttons;
	int			latched_buttons;

	vec3_t		oldOrigin;

	// sum up damage over an entire frame, so
	// shotgun blasts give a single big kick
	int			damage_armor;		// damage absorbed by armor
	int			damage_blood;		// damage taken out of health
	int			damage_knockback;	// impact damage
	vec3_t		damage_from;		// origin for vector calculation
	qboolean	damage_fromWorld;	// if true, don't use the damage_from vector

	int			accurateCount;		// for "impressive" reward sound

	//
	int			lastkilled_client;	// last client that this client killed
	int			lasthurt_client;	// last client that damaged this client
	int			lasthurt_time;		// time this client was last hurt
	int			lasthurt_mod;		// type of damage the client did

	// timers
	int			respawnTime;		// can respawn when time > this, force after g_forcerespwan
	int			invulnerableTime;	// invulnerable time
	int			inactivityTime;		// kick players when time > this
	qboolean	inactivityWarning;	// qtrue if the five seoond warning has been given
	int			rewardTime;			// clear the EF_AWARD_IMPRESSIVE, etc when time > this

	int			airOutTime;

	int			lastKillTime;		// for multiple kill rewards

	qboolean	fireHeld;			// used for hook
	gentity_t	*hook;				// grapple hook if out

	int			switchTeamTime;		// time the player switched teams
	int			voteDelayTime;		// time when this client can vote again

	char		*areabits;

	int				voiceFloodTimer;		// Timer used to forgive voice chat flooding
	int				voiceFloodCount;		// Amount of voice chats that need to be forgivin
	int				voiceFloodPenalty;		// Time when a client can voice chat again

	// Anti-lag information
	gantilag_t		antilag[MAX_ANTILAG];
	gantilag_t		antilagUndo;
	int				antilagHead;

	qboolean		noOutfittingChange;

	animInfo_t		torso;
	animInfo_t		legs;
	vec3_t			ghoulLegsAngles;
	vec3_t			ghoulLowerTorsoAngles;
	vec3_t			ghoulUpperTorsoAngles;
	vec3_t			ghoulHeadAngles;

	gentity_t		*siameseTwin;
	//Ryan june 7 2003
	qboolean		adminspec;
	//Ryan

	//Ryan april 4 2004
	vec3_t			maxSave;
	vec3_t			minSave;
	//Ryan

	// GodOrDevil - GAMETYPES - #use
	gentity_t		*useEntity;

	// #spawnKill
	vec3_t			spawnOrigin;
	qboolean		spawn;

	// END

};

// GodOrDevil - GAMETYPES
typedef struct gtitem_s
{
	int		id;
	//int		useIcon;
	//int		useSound;
	int		useTime;
} gtitem_t;
// End

// GodOrDevil - GAMETYPES
typedef struct nvgt_s
{
	int				roundSwapCount;
	int				roundCount;
	int				roundNumber;
	int				roundWinsBlue;
	int				roundWinsRed;
	//qboolean		roundPausedWait;
	int				domControlledFlags[2];		/// keep track of team contolled flags
	int				domMaxFlags;
	int				phaseTargets[MAX_PHASES];
	int				currentPhase;
	//int				blueReinforcements;
	//int				redReinforcements;
	int				bombGivenClient[TEAM_NUM_TEAMS];
	int				redLocked;				// when set to 1 no one will be allowed to join team
	int				blueLocked;
	int				specsLocked;
	int				redtimeouts;
	int				bluetimeouts;
	int				timeout;
	qboolean		swapteams;
	qboolean		roundOver;
#ifdef _OVERPOWER
	vec3_t			redFlagOrigin, blueFlagOrigin, neutralFlagOrigin;
	vec3_t			redFlagAngles, blueFlagAngles, neutralFlagAngles;
	qboolean		spawnExtras[TEAM_NUM_TEAMS];
#endif
} nvgt_t;

// End

//
// this structure is cleared as each map is entered
//
#define	MAX_SPAWN_VARS			64
#define	MAX_SPAWN_VARS_CHARS	4096

typedef struct
{
	struct gclient_s	*clients;		// [maxclients]

	struct gentity_s	*gentities;

	int				gentitySize;
	int				num_entities;		// current number, <= MAX_GENTITIES

	int				warmupTime;			// restart match at this time

	int				gametype;			// Current gametype index (gametypeData[gametype])
	gametypeData_t*	gametypeData;		// Current gametype data for easier access

	vec3_t			worldMins;			// Absolute min coords of the world
	vec3_t			worldMaxs;			// Absolute max coords of the world

	fileHandle_t	logFile;

	// store latched cvars here that we want to get at often
	int			maxclients;

	int			framenum;
	int			time;					// in msec
	int			previousTime;			// so movers can back up when blocked
	int			frameStartTime;

	int			startTime;				// level.time the map was started
	int			globalVoiceTime;		// last global voice

	int			teamScores[TEAM_NUM_TEAMS];

	//Ryan & Dragon
	int			teamAliveCount[TEAM_NUM_TEAMS];
	//Ryan & Dragon

	int			lastTeamLocationTime;		// last time of client team location update

	qboolean	newSession;				// don't use any old session data, because
										// we changed gametype

	qboolean	restarted;				// waiting for a map_restart to fire

	int			numConnectedClients;
	int			numNonSpectatorClients;	// includes connecting clients
	int			numPlayingClients;		// connected, non-spectators
	int			sortedClients[MAX_CLIENTS];		// sorted by score
	int			follow1, follow2;		// clientNums for auto-follow spectators

	int			warmupModificationCount;	// for detecting if g_warmup is changed

	// voting state
	char		voteString[MAX_STRING_CHARS];
	char		voteDisplayString[MAX_STRING_CHARS];
	int			voteTime;				// level.time vote was called
	int			voteExecuteTime;		// time the vote is executed
	int			voteYes;
	int			voteNo;
	int			voteClient;				// client who initiated vote
	int			numVotingClients;		// set by CalculateRanks

	// spawn variables
	qboolean	spawning;				// the G_Spawn*() functions are valid
	int			numSpawnVars;
	char		*spawnVars[MAX_SPAWN_VARS][2];	// key / value pairs
	int			numSpawnVarChars;
	char		spawnVarChars[MAX_SPAWN_VARS_CHARS];

	// intermission state
	int			intermissionQueued;		// intermission was qualified, but
										// wait INTERMISSION_DELAY_TIME before
										// actually going there so the last
										// frag can be watched.  Disable future
										// kills during this delay
	int			intermissiontime;		// time the intermission was started
	char		*changemap;
	qboolean	readyToExit;			// at least one client wants to exit
	int			exitTime;
	vec3_t		intermission_origin;	// also used for spectator spawns
	vec3_t		intermission_angle;
	//Ryan june 15 2003
	int			pause; // when 1 all players in intermission type state
	int			unpausetime;			// time for the restart countdown sequence
	int			awardTime;
	int			lastAwardSent;
	int			lastTMIupdate;

	//Ryan
	int			lastConnectedClient;	// GodOrDevil - 03.02.05 - 05:29pm #Tokens
	
	// GodOrDevil - 02.28.05 - 11:11pm #counts
	int				adminCount;					/// 02.28.05 - 11:11pm - #adminCount to count # of admins.. duh!
#ifdef _CLANMEMBER
	int				clanCount;					/// 02.28.05 - 11:11pm - #clanCount to count # of clan
#endif
	int				refCount;					/// 02.28.05 - 11:11pm - #refCount to count # of refs
	int				redMsgCount;				/// 03.27.05 - 08.50pm - #redMsgCount to count last alive message
	int				blueMsgCount;				/// 03.27.05 - 08.50pm - #blueMsgCount to count last alive message
	//int				oneSecTimer;				/// 01.08.06 - 11.24pm - #scOneSecTimer sets up 1 second intervals
	int				autoEvenTimer;				/// 01.22.06 - 07.21pm - #oldLevelTime sets up 1 second intervals
	int				modeChangeSec;				/// 01.22.06 - 07.22pm - #modeChangeSec
	///int				tortureCheckTimer;

	fileHandle_t	adminLogFile;
	fileHandle_t	crashLogFile;
	///End - 03.02.05 - 01:44am


#ifdef _SOF2_BOTS
	// GRIM 10/06/2002 6:20PM
	int			gtItemCount;
	char		entExt[MAX_QPATH];
	// GRIM
#endif

	qboolean	locationLinked;			// target_locations get linked
	gentity_t	*locationHead;			// head of the location list

	// body queue
	int			bodyQueIndex;			// dead bodies
	int			bodyQueSize;			// how many dead bodies can there be
	int			bodySinkTime;
	gentity_t	*bodyQue[BODY_QUEUE_SIZE_MAX];

	int			portalSequence;

	int			mNumBSPInstances;
	int			mBSPInstanceDepth;
	vec3_t		mOriginAdjust;
	float		mRotationAdjust;
	char		*mTargetAdjust;
	qboolean	hasBspInstances;
	char		mFilter[MAX_QPATH];
	char		mTeamFilter[MAX_QPATH];

	struct gentity_s	*landScapes[MAX_TERRAINS];

	int				gametypeStartTime;
	int				gametypeJoinTime;
	int				gametypeResetTime;
	int				gametypeRoundTime;
	int				gametypeRespawnTime[TEAM_NUM_TEAMS];
	int				gametypeDelayTime;
	const char*		gametypeTeam[TEAM_NUM_TEAMS];

	void*			serverGhoul2;
	animation_t		ghoulAnimations[MAX_ANIMATIONS];

	int				ladderCount;

	gspawn_t		spawns[MAX_SPAWNS];
	int				spawnCount;

	qboolean		pickupsDisabled;

	int				timeExtension;

	// GodOrDevil - 05.11.05 - 4:24am #serverMessages
	int				serverMsg;
	int				serverMsgCount;
	qboolean		firstBlood;
	//End  - 05.11.05 - 4:25am

	gtitem_t		gametypeItems[MAX_GAMETYPE_ITEMS];
	
	int				effects[EFFECT_MAX];					// effects

	// GodOrDevil - level_locals
#ifdef _DB_ENABLED
	qboolean		usingDatabase;
#endif

#ifdef _NEW_MAP_SETUP
	char			nextMap[64];
	char			nextGameType[8];
	int				randMapNum;
	char			randMap[64];
	char			randGameType[8];
	char			randVSTR[32];		// stores random vote identifier.  ie, rmap2 or mp_kam2_ctf, depending on the mapcycle setup.
	qboolean		randomVoteCalled;
	qboolean		doRandom;
#endif
	

#ifdef _KILLSPREE
	int				topSpreeReward;				// Used for looping killing spree rewards
#endif

	nvgt_t			extras;
// END

} level_locals_t;


#ifdef _MAPEDITOR
// pheno: moved from g_spawn.c
typedef enum {
	F_INT, 
	F_FLOAT,
	F_LSTRING,			// string on disk, pointer in memory, TAG_LEVEL
	F_GSTRING,			// string on disk, pointer in memory, TAG_GAME
	F_VECTOR,
	F_ANGLEHACK,
	F_ENTITY,			// index on disk, pointer in memory
	F_ITEM,				// index on disk, pointer in memory
	F_CLIENT,			// index on disk, pointer in memory
	F_IGNORE
} fieldtype_t;

typedef struct
{
	char		*name;
	int			ofs;
	fieldtype_t	type;
	int			flags;
} field_t;

#endif

//
// g_spawn.c
//
qboolean	G_SpawnString( const char *key, const char *defaultString, char **out );
// spawn string returns a temporary reference, you must CopyString() if you want to keep it
qboolean	G_SpawnFloat( const char *key, const char *defaultString, float *out );
qboolean	G_SpawnInt( const char *key, const char *defaultString, int *out );
qboolean	G_SpawnVector( const char *key, const char *defaultString, float *out );
qboolean	G_ParseSpawnVars( qboolean inSubBSP );
// GodOrDevil		int now to return ent number aftre its spawn
//void		G_SpawnGEntityFromSpawnVars( qboolean inSubBSP );
int			G_SpawnGEntityFromSpawnVars( qboolean inSubBSP );
//END

void		G_SpawnEntitiesFromString( qboolean inSubBSP );
char		*G_NewString( const char *string );
void		AddSpawnField(char *field, char *value);

//
// g_cmds.c
//
void		Cmd_Score_f			( gentity_t *ent );
void		G_StopFollowing		( gentity_t *ent );
void		G_StopGhosting		( gentity_t* ent );
void		G_StartGhosting		( gentity_t* ent );

void		BroadcastTeamChange( gclient_t *client, int oldTeam );
void		SetTeam( gentity_t *ent, char *s, const char* identity );
void		Cmd_FollowCycle_f( gentity_t *ent, int dir );
qboolean	CheatsOk				( gentity_t *ent );
void		G_SpawnDebugCylinder	( vec3_t origin, float radius, gentity_t* clientent, float viewRadius, int colorIndex );
void		G_Voice					( gentity_t *ent, gentity_t *target, int mode, const char *id, qboolean voiceonly );
void		G_VoiceGlobal			( gentity_t* ent, const char* id, qboolean force );
qboolean	G_CanVoiceGlobal		( void );

//
// g_items.c
//
void ItemUse_Shield(gentity_t *ent);
void ItemUse_Sentry(gentity_t *ent);
void ItemUse_Seeker(gentity_t *ent);
void ItemUse_MedPack(gentity_t *ent);

void G_RunItem( gentity_t *ent );
void RespawnItem( gentity_t *ent );

void		PrecacheItem		( gitem_t *it );
gentity_t*	G_DropItem			( gentity_t *ent, gitem_t *item, float angle );
gentity_t*	G_LaunchItem		( gitem_t *item, vec3_t origin, vec3_t velocity );
gentity_t*	G_DropWeapon		( gentity_t* ent, weapon_t weapon, int pickupDelay );

void SetRespawn (gentity_t *ent, float delay);
void G_SpawnItem (gentity_t *ent, gitem_t *item);
void FinishSpawningItem( gentity_t *ent );
void Think_Weapon (gentity_t *ent);
int ArmorIndex (gentity_t *ent);
void	Add_Ammo (gentity_t *ent, int weapon, int count);
void Touch_Item (gentity_t *ent, gentity_t *other, trace_t *trace);

void ClearRegisteredItems( void );
void RegisterItem( gitem_t *item );
void SaveRegisteredItems( void );

//
// g_utils.c
//
int		G_ModelIndex		( char *name );
int		G_SoundIndex		( char *name );
int		G_AmbientSoundSetIndex( char *name );
int		G_BSPIndex			( char *name );
int		G_IconIndex			( char *name );
int		G_EffectIndex		( char *name );

void	G_TeamCommand( team_t team, char *cmd );
void	G_KillBox (gentity_t *ent);
gentity_t *G_Find (gentity_t *from, int fieldofs, const char *match);
int		G_RadiusList ( vec3_t origin, float radius,	gentity_t *ignore, qboolean takeDamage, gentity_t *ent_list[MAX_GENTITIES]);
gentity_t *G_PickTarget (char *targetname);
void	G_UseTargets (gentity_t *ent, gentity_t *activator);
void	G_UseTargetsByName( const char* name, gentity_t *exclude, gentity_t *activator );
void	G_SetMovedir ( vec3_t angles, vec3_t movedir);
void	G_SetAngles( gentity_t *ent, vec3_t angles );

void	G_InitGentity( gentity_t *e );
gentity_t	*G_Spawn (void);
gentity_t *G_TempEntity( vec3_t origin, int event );
void	G_PlayEffect(int fxID, vec3_t org, vec3_t ang);
void	G_Sound( gentity_t *ent, int channel, int soundIndex );
void	G_SoundAtLoc( vec3_t loc, int channel, int soundIndex );
void	G_EntitySound( gentity_t *ent, int channel, int soundIndex );
void	G_FreeEntity( gentity_t *e );
qboolean	G_EntitiesFree( void );

void	G_TouchTriggers (gentity_t *ent);
void	G_TouchSolids (gentity_t *ent);

//
// g_object.c
//

extern void G_RunObject			( gentity_t *ent );


float	*tv (float x, float y, float z);
char	*vtos( const vec3_t v );

float vectoyaw( const vec3_t vec );

void G_AddPredictableEvent( gentity_t *ent, int event, int eventParm );
void G_AddEvent( gentity_t *ent, int event, int eventParm );
void G_SetOrigin( gentity_t *ent, vec3_t origin );
void AddRemap(const char *oldShader, const char *newShader, float timeOffset);
const char *BuildShaderStateConfig(void);

// CG specific API access

void		trap_G2API_CleanGhoul2Models	( void **ghoul2Ptr );
void		trap_G2API_CollisionDetect		( CollisionRecord_t *collRecMap, void* ghoul2, const vec3_t angles, const vec3_t position,int frameNumber, int entNum, vec3_t rayStart, vec3_t rayEnd, vec3_t scale, int traceFlags, int useLod );
qhandle_t	trap_G2API_RegisterSkin			( const char *skinName, int numPairs, const char *skinPairs);
qboolean	trap_G2API_SetSkin				( void* ghoul2, int modelIndex, qhandle_t customSkin);
qboolean	trap_G2API_GetAnimFileNameIndex ( void* ghoul2, qhandle_t modelIndex, const char* name );

void		trap_G2_ListModelSurfaces(void *ghlInfo);
void		trap_G2_ListModelBones(void *ghlInfo, int frame);
int			trap_G2API_AddBolt(void *ghoul2, const int modelIndex, const char *boneName);
qboolean	trap_G2API_RemoveBolt(void *ghlInfo, const int modelIndex, const int index);
qboolean	trap_G2API_AttachG2Model(void *ghoul2From, int modelFrom, void *ghoul2To, int toBoltIndex, int toModel);
void		trap_G2_SetGhoul2ModelIndexes(void *ghoul2, qhandle_t *modelList, qhandle_t *skinList);
qboolean	trap_G2_HaveWeGhoul2Models(void *ghoul2);
qboolean	trap_G2API_GetBoltMatrix(void *ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix,
								const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, vec3_t scale);
int			trap_G2API_InitGhoul2Model(void **ghoul2Ptr, const char *fileName, int modelIndex, qhandle_t customSkin,
						  qhandle_t customShader, int modelFlags, int lodBias);

int			trap_G2API_CopyGhoul2Instance(void *g2From, void *g2To, int modelIndex);
int			trap_G2API_CopySpecificGhoul2Model(void *g2From, int modelFrom, void *g2To, int modelTo);
void		trap_G2API_DuplicateGhoul2Instance(void *g2From, void **g2To);
qboolean	trap_G2API_RemoveGhoul2Model(void **ghlInfo, int modelIndex);

qboolean	trap_G2API_SetBoneAngles(void *ghoul2, int modelIndex, const char *boneName, const vec3_t angles, const int flags,
								const int up, const int right, const int forward, qhandle_t *modelList,
								int blendTime , int currentTime );
char		*trap_G2API_GetGLAName(void *ghoul2, int modelIndex);
qboolean	trap_G2API_SetBoneAnim(void *ghoul2, const int modelIndex, const char *boneName, const int startFrame, const int endFrame,
							  const int flags, const float animSpeed, const int currentTime, const float setFrame , const int blendTime );


//
// g_combat.c
//
qboolean	CanDamage			( gentity_t *targ, vec3_t origin);
int			G_GetHitLocation	( gentity_t *target, vec3_t ppoint, vec3_t dir );
int			G_Damage			( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker, vec3_t dir, vec3_t point, int damage, int dflags, int mod, int location );
qboolean	G_RadiusDamage		( vec3_t origin, gentity_t *attacker, float damage, float radius, gentity_t *ignore, int power, int mod);
void		body_die			( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath, int hitLocation, vec3_t hitDir );
void		TossClientItems		( gentity_t *self );

// damage flags
#define DAMAGE_RADIUS				0x00000001	// damage was indirect
#define DAMAGE_NO_ARMOR				0x00000002	// armour does not protect from this damage
#define DAMAGE_NO_KNOCKBACK			0x00000004	// do not affect velocity, just view angles
#define DAMAGE_NO_PROTECTION		0x00000008  // armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_NO_TEAM_PROTECTION	0x00000010  // armor, shields, invulnerability, and godmode have no effect
#define DAMAGE_DEATH_KNOCKBACK		0x00000080	// only does knockback on death of target
#define DAMAGE_AREA_DAMAGE			0x00000400	// spawn area damage
#define DAMAGE_NO_GORE				0x00000800	// dont spawn gore pieces with this damage
#define	DAMAGE_FORCE_GORE			0x00001000	// force something to pop off

//
// g_missile.c
//
void		G_RunMissile		( gentity_t *ent );
gentity_t*	G_CreateMissile		( vec3_t org, vec3_t dir, float vel, int life, gentity_t *owner, attackType_t attack );
gentity_t*	G_CreateDamageArea	( vec3_t origin, gentity_t* attacker, float damage, float radius, int duration, int mod );
void		G_BounceProjectile	( vec3_t start, vec3_t impact, vec3_t dir, vec3_t endout );
void		G_ExplodeMissile	( gentity_t *ent );
void		G_GrenadeThink		( gentity_t* ent );

//
// g_mover.c
//
void		G_RunMover			( gentity_t *ent );
void		Touch_DoorTrigger	( gentity_t *ent, gentity_t *other, trace_t *trace );
void		G_ResetGlass		( void );


//
// g_trigger.c
//
void		trigger_teleporter_touch		( gentity_t *self, gentity_t *other, trace_t *trace );
void		InitTrigger						( gentity_t *self );

//
// g_misc.c
//
void		TeleportPlayer					( gentity_t *player, vec3_t origin, vec3_t angles );


//
// g_weapon.c
//
void		SnapVectorTowards				( vec3_t v, vec3_t to );
void		G_InitHitModels					( void );

//
// g_client.c
//
qboolean	G_IsClientSpectating			( gclient_t* client );
qboolean	G_IsClientDead					( gclient_t* client );
void		G_ClientCleanName				( const char *in, char *out, int outSize, qboolean colors );
int			TeamCount						( int ignoreClientNum, team_t team, int* alive );
int			G_GhostCount					( team_t team );
team_t		PickTeam						( int ignoreClientNum );
void		SetClientViewAngle				( gentity_t *ent, vec3_t angle );
void		CopyToBodyQue					( gentity_t *ent, int hitLocation, vec3_t hitDirection );
void		respawn							( gentity_t *ent);
void		BeginIntermission				( void);
void		InitClientPersistant			( gclient_t *client);
void		InitClientResp					( gclient_t *client);
void		ClientSpawn						( gentity_t *ent );
void		player_die						( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod, int hitLocation, vec3_t hitDir );
void		G_InitBodyQueue					( void);
void		G_AddScore						( gentity_t *ent, int score );
void		CalculateRanks					( void );
qboolean	G_SpotWouldTelefrag				( gspawn_t* spawn );
void		G_UpdateClientAnimations		( gentity_t* ent );
void		G_SetRespawnTimer				( gentity_t* ent );
gentity_t*	G_FindNearbyClient				( vec3_t origin, team_t team, float radius, gentity_t* ignore );
void		G_AddClientSpawn				( gentity_t* ent, team_t team );

//
// g_svcmds.c
//
qboolean	ConsoleCommand					( void );
void		G_ProcessIPBans					( void );
qboolean	G_FilterPacket					( char *from );

//
// g_weapon.c
//
gentity_t*	G_FireWeapon					( gentity_t *ent, attackType_t attack );
gentity_t*	G_FireProjectile				( gentity_t *ent, weapon_t weapon, attackType_t attack, int projectileLifetime, int flags );

//
// p_hud.c
//
void		MoveClientToIntermission		( gentity_t *client );
void		DeathmatchScoreboardMessage		( gentity_t *client );

//
// g_cmds.c
//

//
// g_main.c
//
void		FindIntermissionPoint				( void );
void		SetLeader							( int team, int client );
void		G_RunThink							( gentity_t *ent );
void QDECL	G_LogPrintf							( const char *fmt, ... );
void		SendScoreboardMessageToAllClients	( void );
void		CheckGametype						( void );

//
// g_client.c
//
char*		ClientConnect						( int clientNum, qboolean firstTime, qboolean isBot );
void		ClientUserinfoChanged				( int clientNum );
void		ClientDisconnect					( int clientNum );
//Ryan april 10 2004 10:05am
//void		ClientBegin							( int clientNum );
void		ClientBegin							( int clientNum, qboolean setTime );
//Ryan
void		ClientCommand						( int clientNum );
gspawn_t*	G_SelectRandomSpawnPoint			( team_t team );
int			G_GametypeCommand					( int cmd, int arg0, int arg1, int arg2, int arg3, int arg4 );

//
// g_active.c
//
void G_CheckClientTimeouts	( gentity_t *ent );
void G_CheckClientTeamkill	( gentity_t *ent );
void ClientThink			( int clientNum );
void ClientEndFrame			( gentity_t *ent );
void G_RunClient			( gentity_t *ent );


//void Camping_Think			(gentity_t *ent);	// GodOrDevil - 03.10.05 - 06:12pm #antiCamp


//
// g_team.c
//
qboolean OnSameTeam( gentity_t *ent1, gentity_t *ent2 );
void Team_CheckDroppedItem( gentity_t *dropped );

//
// g_session.c
//
void G_ReadSessionData( gclient_t *client );
void G_InitSessionData( gclient_t *client, char *userinfo );

void G_InitWorldSession( void );
void G_WriteSessionData( void );

//
// g_bot.c
//
#ifdef _SOF2_BOTS

void G_InitBots( qboolean restart );
char *G_GetBotInfoByNumber( int num );
char *G_GetBotInfoByName( const char *name );
void G_CheckBotSpawn( void );
void G_RemoveQueuedBotBegin( int clientNum );
qboolean G_BotConnect( int clientNum, qboolean restart );
void Svcmd_AddBot_f( void );
void Svcmd_BotList_f( void );
void BotInterbreedEndMatch( void );
#endif
qboolean G_DoesMapSupportGametype ( const char* gametype );
//Ryan
qboolean G_DoesMapExist				( const char* mapname );
//Ryan
void G_LoadArenas ( void );

//
// g_gametype.c
//
gentity_t*	G_SelectGametypeSpawnPoint			( team_t team, vec3_t origin, vec3_t angles );
// GodOrDevil - GAMETYPES
//gentity_t*	G_SpawnGametypeItem					( const char* pickup_name, qboolean dropped);
gentity_t*	G_SpawnGametypeItem					( const char* pickup_name, qboolean dropped, vec3_t origin );
// END
gentity_t*	G_SelectRandomGametypeSpawnPoint	( team_t team );
qboolean	G_ParseGametypeFile					( void );
qboolean	G_ExecuteGametypeScript				( gentity_t* activator, const char* name );
void		G_ResetGametype						( void );
qboolean	G_CanGametypeTriggerBeUsed			( gentity_t* self, gentity_t* activator );
void		G_ResetGametypeItem					( gitem_t* item );
// GodOrDevil - 02.03.05 - 11:32am #DropGTItems
void		G_DropGametypeItems					( gentity_t* self, int delayPickup );
void		G_FreeEnitityChildren				( gentity_t* ent );		
//End - 02.03.05 - 11:32am

// ai_main.c
#define MAX_FILEPATH			144
int OrgVisible(vec3_t org1, vec3_t org2, int ignore);

//bot settings
#ifdef _SOF2_BOTS
typedef struct bot_settings_s
{
	char personalityfile[MAX_FILEPATH];
	float skill;
	char team[MAX_FILEPATH];
} bot_settings_t;

int BotAISetup( int restart );
int BotAIShutdown( int restart );
int BotAILoadMap( int restart );
int BotAISetupClient(int client, struct bot_settings_s *settings, qboolean restart);
int BotAIShutdownClient( int client, qboolean restart );
int BotAIStartFrame( int time );
#endif
#include "g_team.h" // teamplay specific stuff


extern	level_locals_t	level;
extern	gentity_t		g_entities[MAX_GENTITIES];

#ifdef _DB_ENABLED
//extern nv_topStats_t	topStats[20];
extern mapStats_t		mapStats;
#endif

#define	FOFS(x) ((int)&(((gentity_t *)0)->x))


#ifndef _NV_ADMIN
// GodOrDevil - 1.12.2005 - External #CVARs for #AdminLevels
extern vmCvar_t g_plant;
extern vmCvar_t g_ban;
extern vmCvar_t g_pop;
extern vmCvar_t g_runover;
extern vmCvar_t g_kick;
extern vmCvar_t g_mute;
extern vmCvar_t g_strip;
extern vmCvar_t g_fvote;
extern vmCvar_t g_forcesay;
extern vmCvar_t g_namechange;
extern vmCvar_t g_eventeams;
extern vmCvar_t g_clanvsall;
extern vmCvar_t g_extracmds;
extern vmCvar_t g_addadmin;
extern vmCvar_t g_addbadmin;
#ifdef _CLANMEMBER
extern vmCvar_t g_addclan;
#endif
extern vmCvar_t g_pause;
extern vmCvar_t g_lock;
extern vmCvar_t g_warn;
extern vmCvar_t g_swapteams;
extern vmCvar_t g_pbox;
extern vmCvar_t g_ff;

extern vmCvar_t g_subnetban;

extern vmCvar_t g_torture;
extern vmCvar_t g_modeChange;
//extern vmCvar_t g_sounds;
extern vmCvar_t g_nextMap;
extern vmCvar_t g_hide;
extern vmCvar_t g_needWarn;
#endif
extern vmCvar_t g_updatetmi;
extern vmCvar_t g_callVote;
extern vmCvar_t g_m203;

///ServerMessages - 05.11.05 - 04.25am
extern vmCvar_t	g_serverMsgInterval;			//05.11.05 - 04.26am - #serverMsgInterval time between automessage display
extern vmCvar_t	g_enableServerMsgs;			//05.11.05 - 04.26am - #enableServerMsg enables/disables automessages
extern vmCvar_t	g_msg1;						//05.11.05 - 04.27am - #msg customizable strings to show in the automessages
extern vmCvar_t	g_msg2;						// ...
extern vmCvar_t	g_msg3;						// ...
extern vmCvar_t	g_msg4;						// ...
extern vmCvar_t	g_msg5;						// ...
extern vmCvar_t	g_serverMsgDelay;				///12.02.05 - 11.49pm - #serverMsgDelay sets time between each server msgs.
extern vmCvar_t g_disableRPMObituary;
//#AntiCamp
extern vmCvar_t		g_camperAllowTime;
extern vmCvar_t		g_camperPunish;
extern vmCvar_t		g_camperRadius;
extern vmCvar_t		g_camperSniper;
extern vmCvar_t		g_camperPunishment;
//extern vmCvar_t		g_camperPunishment2;
//extern vmCvar_t		g_camperPunishment3;

extern vmCvar_t		g_autoVoteTimeExtend;		///Vote to Auto-Extend time.

// GodOrDevil - 11.04.05 - 01:49am
extern	vmCvar_t	g_talkingGhosts;

//---------------------Misc Cvars-----------------/
extern vmCvar_t g_Armor;				// GodOrDevil - 1.30.2005 - #Armor
extern vmCvar_t g_teamChangeDeath;		// GodOrDevil - 02.04.05 - 06:43am - #teamChangeDeath
extern vmCvar_t g_autoEvenTeams;		//02.28.05 - 10:34pm - #evenTeams if a player disconnects
//extern vmCvar_t g_pBoxTeamKill;			//03.01.05 - 09:40pm - #pBox for Teamkill
#ifdef _TORTURE
extern vmCvar_t g_torturedname;			//03.03.05 - 01:29am - #torturedName
#endif
extern vmCvar_t g_autoUnlock;			//03.28.05 - 02.40pm - #autoUnlock so last ref/admin disc can/cant unlock teams
extern vmCvar_t	g_nextScore;			//03.28.05 - 02.58pm - #nextScore sets score of the next map when nextMap is used
extern vmCvar_t	g_setnextMap;			//03.28.05 - 02.58pm - #nextMap allows admins to set a map to be loaded After current map
//extern vmCvar_t g_hideAdmins;	
//extern vmCvar_t g_noRanks;
//extern vmCvar_t g_allowSounds;
extern vmCvar_t g_allowTie;
//extern vmCvar_t	sc_allowRefKick;
//extern vmCvar_t	sc_adminSuspend;
extern vmCvar_t	g_roundjointime;
extern vmCvar_t	g_weaponModFile;
extern vmCvar_t	g_enableDoors;
extern vmCvar_t	g_enableCustomGrenades;
extern vmCvar_t	g_admCmds;
extern vmCvar_t	g_noDropOrigin;
extern vmCvar_t g_m4scope;

#ifdef _MAPEDITOR
extern vmCvar_t	nv_editMap;
extern vmCvar_t	nv_enableThirdPerson;
extern vmCvar_t	nv_enableThirdCrosshair;
#endif

#ifdef _DB_ENABLED
#ifdef _DB_STATS_DLL
extern vmCvar_t	statsloaded;
#endif
extern vmCvar_t	g_enableDatabase;
extern vmCvar_t	g_enableTopStats;
extern vmCvar_t	g_banIpSize;
extern vmCvar_t	g_databasename;
#endif

//ExtraWeapons - 04.22.05 - 04:07am
extern vmCvar_t	g_onlyWeapons;
extern vmCvar_t	g_ForcedItems;

//Chat Prefixes - 02.04.05 - 11:16am
extern vmCvar_t	g_clanChat;						
extern vmCvar_t	g_sadminChat;									
extern vmCvar_t	g_adminChat;					
extern vmCvar_t	g_refChat;						
extern vmCvar_t	g_caChat;						
extern vmCvar_t	g_adminTalk;					
extern vmCvar_t	g_refTalk;						
extern vmCvar_t	g_clanTalk;
extern vmCvar_t g_adminNotify;
extern vmCvar_t g_adminRule;
extern vmCvar_t g_adminInfo;
extern vmCvar_t g_adminWarning;
//End



extern	vmCvar_t	g_gametype;
extern	vmCvar_t	g_dedicated;
extern	vmCvar_t	g_cheats;
extern	vmCvar_t	g_maxclients;			// allow this many total, including spectators
extern	vmCvar_t	g_maxGameClients;		// allow this many active
extern	vmCvar_t	g_restarted;

extern	vmCvar_t	g_logHits;

extern	vmCvar_t	g_dmflags;
extern	vmCvar_t	g_scorelimit;
extern	vmCvar_t	g_timelimit;
extern	vmCvar_t	g_friendlyFire;
extern	vmCvar_t	g_password;
// GodOrDevil - privatepass overrides password if needed
extern	vmCvar_t	sv_privatePassword;
//END
extern	vmCvar_t	g_needpass;
extern	vmCvar_t	g_gravity;
extern	vmCvar_t	g_speed;
extern	vmCvar_t	g_knockback;
extern	vmCvar_t	g_forcerespawn;
extern	vmCvar_t	g_inactivity;
extern	vmCvar_t	g_debugMove;
extern	vmCvar_t	g_debugAlloc;
extern	vmCvar_t	g_debugDamage;
extern	vmCvar_t	g_weaponRespawn;
extern	vmCvar_t	g_backpackRespawn;
extern	vmCvar_t	g_synchronousClients;
extern	vmCvar_t	g_motd;
extern	vmCvar_t	g_warmup;
extern	vmCvar_t	g_doWarmup;
extern	vmCvar_t	g_allowVote;
extern	vmCvar_t	g_voteDuration;
extern	vmCvar_t	g_failedVoteDelay;
extern	vmCvar_t	g_teamAutoJoin;
extern	vmCvar_t	g_teamForceBalance;
extern	vmCvar_t	g_banIPs;
extern	vmCvar_t	g_filterBan;
extern	vmCvar_t	g_smoothClients;
extern	vmCvar_t	pmove_fixed;
extern	vmCvar_t	pmove_msec;
extern	vmCvar_t	g_rankings;
extern	vmCvar_t	g_respawnInterval;
extern	vmCvar_t	g_respawnInvulnerability;
extern	vmCvar_t	RMG;
extern  vmCvar_t	g_debugRMG;
extern	vmCvar_t	g_timeouttospec;
extern  vmCvar_t	g_roundtimelimit;
extern  vmCvar_t	g_timeextension;
extern  vmCvar_t	g_roundstartdelay;
extern  vmCvar_t	g_availableWeapons;
extern	vmCvar_t	g_forceFollow;
extern	vmCvar_t	g_followEnemy;
extern  vmCvar_t	g_mapcycle;
extern	vmCvar_t	g_pickupsDisabled;
extern	vmCvar_t	g_suicidePenalty;
extern	vmCvar_t	g_teamkillPenalty;
extern	vmCvar_t	g_teamkillDamageMax;
extern	vmCvar_t	g_teamkillDamageForgive;
extern	vmCvar_t	g_voiceFloodCount;
extern	vmCvar_t	g_voiceFloodPenalty;

extern	vmCvar_t	g_rules1;
extern	vmCvar_t	g_rules2;

// GodOrDevil - GAMETYPES
extern vmCvar_t	gt_targetHealth;
extern vmCvar_t	gt_customType;
extern vmCvar_t nv_gtName;
extern vmCvar_t nv_gtShort;
extern vmCvar_t	gt_bombTimer;
extern vmCvar_t	g_freezeTime;
extern vmCvar_t	nv_reinforcementsBlue;
extern vmCvar_t	nv_reinforcementsRed;
extern vmCvar_t	nv_reinforcementsBlueTime;
extern vmCvar_t	nv_reinforcementsRedTime;
extern vmCvar_t g_swapsides;
extern vmCvar_t	g_enableHealthGains;
extern vmCvar_t	g_maxSpawnKill;

//extern vmCvar_t g_crouchNeeded;
extern vmCvar_t g_enableChatReponses;

#ifdef _KILLSPREE
extern vmCvar_t g_enableKillSpreeReward;
extern vmCvar_t g_killSpreeFile;
#endif // _KILLSPREE

extern vmCvar_t	g_enableVoteSkipNextMap;
extern vmCvar_t	g_enableVoteNextMap;
extern vmCvar_t	g_ggRankType;  // For Gun Game resetting
extern vmCvar_t	g_showGametypeHelp; 


//END

void	trap_Printf( const char *fmt );
void	trap_Error( const char *fmt );
int		trap_Milliseconds( void );
int		trap_Argc( void );
void	trap_Argv( int n, char *buffer, int bufferLength );
void	trap_Args( char *buffer, int bufferLength );
int		trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode );
void	trap_FS_Read( void *buffer, int len, fileHandle_t f );
void	trap_FS_Write( const void *buffer, int len, fileHandle_t f );
void	trap_FS_FCloseFile( fileHandle_t f );
int		trap_FS_GetFileList	( const char *path, const char *extension, char *listbuf, int bufsize );
void	trap_SendConsoleCommand( int exec_when, const char *text );
void	trap_Cvar_Register( vmCvar_t *cvar, const char *var_name, const char *value, int flags, float MinValue, float MaxValue );
void	trap_Cvar_Update( vmCvar_t *cvar );
void	trap_Cvar_Set( const char *var_name, const char *value );
int		trap_Cvar_VariableIntegerValue( const char *var_name );
float	trap_Cvar_VariableValue( const char *var_name );
void	trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize );
void	trap_LocateGameData( gentity_t *gEnts, int numGEntities, int sizeofGEntity_t, playerState_t *gameClients, int sizeofGameClient );
void	trap_RMG_Init(int terrainID);
void	trap_DropClient( int clientNum, const char *reason );
void	trap_SendServerCommand( int clientNum, const char *text );
//void	trap_SendServerCommand2( int clientNum, const char *text );
void	trap_SetConfigstring( int num, const char *string );
void	trap_GetConfigstring( int num, char *buffer, int bufferSize );
void	trap_GetUserinfo( int num, char *buffer, int bufferSize );
void    trap_GetUserinfo2( int num, char *buffer, int bufferSize ); // GodOrDevil :  Boeman's fix for connecting character lag - 1fx.uk.to
void	trap_SetUserinfo( int num, const char *buffer );
void	trap_GetServerinfo( char *buffer, int bufferSize );
void	trap_SetBrushModel( gentity_t *ent, const char *name );
void	trap_SetActiveSubBSP(int index);
void	trap_Trace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask );
int		trap_PointContents( const vec3_t point, int passEntityNum );
qboolean trap_InPVS( const vec3_t p1, const vec3_t p2 );
qboolean trap_InPVSIgnorePortals( const vec3_t p1, const vec3_t p2 );
void	trap_AdjustAreaPortalState( gentity_t *ent, qboolean open );
qboolean trap_AreasConnected( int area1, int area2 );
void	trap_LinkEntity( gentity_t *ent );
void	trap_UnlinkEntity( gentity_t *ent );
int		trap_EntitiesInBox( const vec3_t mins, const vec3_t maxs, int *entityList, int maxcount );
qboolean trap_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );
int		trap_BotAllocateClient( void );
void	trap_BotFreeClient( int clientNum );
void	trap_GetUsercmd( int clientNum, usercmd_t *cmd );
qboolean	trap_GetEntityToken( char *buffer, int bufferSize );
void		trap_GetWorldBounds ( vec3_t mins, vec3_t maxs );

void	*trap_BotGetMemoryGame(int size);
void	trap_BotFreeMemoryGame(void *ptr);

int		trap_DebugPolygonCreate(int color, int numPoints, vec3_t *points);
void	trap_DebugPolygonDelete(int id);

int		trap_BotLibSetup( void );
int		trap_BotLibShutdown( void );
int		trap_BotLibVarSet(char *var_name, char *value);
int		trap_BotLibVarGet(char *var_name, char *value, int size);
int		trap_BotLibDefine(char *string);
int		trap_BotLibStartFrame(float time);
int		trap_BotLibLoadMap(const char *mapname);
int		trap_BotLibUpdateEntity(int ent, void /* struct bot_updateentity_s */ *bue);
int		trap_BotLibTest(int parm0, char *parm1, vec3_t parm2, vec3_t parm3);

int		trap_BotGetSnapshotEntity( int clientNum, int sequence );
int		trap_BotGetServerCommand(int clientNum, char *message, int size);
void	trap_BotUserCommand(int client, usercmd_t *ucmd);

int		trap_AAS_BBoxAreas(vec3_t absmins, vec3_t absmaxs, int *areas, int maxareas);
int		trap_AAS_AreaInfo( int areanum, void /* struct aas_areainfo_s */ *info );
void	trap_AAS_EntityInfo(int entnum, void /* struct aas_entityinfo_s */ *info);

int		trap_AAS_Initialized(void);
void	trap_AAS_PresenceTypeBoundingBox(int presencetype, vec3_t mins, vec3_t maxs);
float	trap_AAS_Time(void);

int		trap_AAS_PointAreaNum(vec3_t point);
int		trap_AAS_PointReachabilityAreaIndex(vec3_t point);
int		trap_AAS_TraceAreas(vec3_t start, vec3_t end, int *areas, vec3_t *points, int maxareas);

int		trap_AAS_PointContents(vec3_t point);
int		trap_AAS_NextBSPEntity(int ent);
int		trap_AAS_ValueForBSPEpairKey(int ent, char *key, char *value, int size);
int		trap_AAS_VectorForBSPEpairKey(int ent, char *key, vec3_t v);
int		trap_AAS_FloatForBSPEpairKey(int ent, char *key, float *value);
int		trap_AAS_IntForBSPEpairKey(int ent, char *key, int *value);

int		trap_AAS_AreaReachability(int areanum);

int		trap_AAS_AreaTravelTimeToGoalArea(int areanum, vec3_t origin, int goalareanum, int travelflags);
int		trap_AAS_EnableRoutingArea( int areanum, int enable );
int		trap_AAS_PredictRoute(void /*struct aas_predictroute_s*/ *route, int areanum, vec3_t origin,
							int goalareanum, int travelflags, int maxareas, int maxtime,
							int stopevent, int stopcontents, int stoptfl, int stopareanum);

int		trap_AAS_AlternativeRouteGoals(vec3_t start, int startareanum, vec3_t goal, int goalareanum, int travelflags,
										void /*struct aas_altroutegoal_s*/ *altroutegoals, int maxaltroutegoals,
										int type);
int		trap_AAS_Swimming(vec3_t origin);
int		trap_AAS_PredictClientMovement(void /* aas_clientmove_s */ *move, int entnum, vec3_t origin, int presencetype, int onground, vec3_t velocity, vec3_t cmdmove, int cmdframes, int maxframes, float frametime, int stopevent, int stopareanum, int visualize);


void	trap_EA_Say(int client, char *str);
void	trap_EA_SayTeam(int client, char *str);
void	trap_EA_Command(int client, char *command);

void	trap_EA_Action(int client, int action);
void	trap_EA_Gesture(int client);
void	trap_EA_Talk(int client);
void	trap_EA_Attack(int client);
void	trap_EA_Use(int client);
void	trap_EA_Respawn(int client);
void	trap_EA_Crouch(int client);
void	trap_EA_MoveUp(int client);
void	trap_EA_MoveDown(int client);
void	trap_EA_MoveForward(int client);
void	trap_EA_MoveBack(int client);
void	trap_EA_MoveLeft(int client);
void	trap_EA_MoveRight(int client);
void	trap_EA_SelectWeapon(int client, int weapon);
void	trap_EA_Jump(int client);
void	trap_EA_DelayedJump(int client);
void	trap_EA_Move(int client, vec3_t dir, float speed);
void	trap_EA_View(int client, vec3_t viewangles);
void	trap_EA_Alt_Attack(int client);
void	trap_EA_ForcePower(int client);

void	trap_EA_EndRegular(int client, float thinktime);
void	trap_EA_GetInput(int client, float thinktime, void /* struct bot_input_s */ *input);
void	trap_EA_ResetInput(int client);


int		trap_BotLoadCharacter(char *charfile, float skill);
void	trap_BotFreeCharacter(int character);
float	trap_Characteristic_Float(int character, int index);
float	trap_Characteristic_BFloat(int character, int index, float min, float max);
int		trap_Characteristic_Integer(int character, int index);
int		trap_Characteristic_BInteger(int character, int index, int min, int max);
void	trap_Characteristic_String(int character, int index, char *buf, int size);

int		trap_BotAllocChatState(void);
void	trap_BotFreeChatState(int handle);
void	trap_BotQueueConsoleMessage(int chatstate, int type, char *message);
void	trap_BotRemoveConsoleMessage(int chatstate, int handle);
int		trap_BotNextConsoleMessage(int chatstate, void /* struct bot_consolemessage_s */ *cm);
int		trap_BotNumConsoleMessages(int chatstate);
void	trap_BotInitialChat(int chatstate, char *type, int mcontext, char *var0, char *var1, char *var2, char *var3, char *var4, char *var5, char *var6, char *var7 );
int		trap_BotNumInitialChats(int chatstate, char *type);
int		trap_BotReplyChat(int chatstate, char *message, int mcontext, int vcontext, char *var0, char *var1, char *var2, char *var3, char *var4, char *var5, char *var6, char *var7 );
int		trap_BotChatLength(int chatstate);
void	trap_BotEnterChat(int chatstate, int client, int sendto);
void	trap_BotGetChatMessage(int chatstate, char *buf, int size);
int		trap_StringContains(char *str1, char *str2, int casesensitive);
int		trap_BotFindMatch(char *str, void /* struct bot_match_s */ *match, unsigned long int context);
void	trap_BotMatchVariable(void /* struct bot_match_s */ *match, int variable, char *buf, int size);
void	trap_UnifyWhiteSpaces(char *string);
void	trap_BotReplaceSynonyms(char *string, unsigned long int context);
int		trap_BotLoadChatFile(int chatstate, char *chatfile, char *chatname);
void	trap_BotSetChatGender(int chatstate, int gender);
void	trap_BotSetChatName(int chatstate, char *name, int client);
void	trap_BotResetGoalState(int goalstate);
void	trap_BotRemoveFromAvoidGoals(int goalstate, int number);
void	trap_BotResetAvoidGoals(int goalstate);
void	trap_BotPushGoal(int goalstate, void /* struct bot_goal_s */ *goal);
void	trap_BotPopGoal(int goalstate);
void	trap_BotEmptyGoalStack(int goalstate);
void	trap_BotDumpAvoidGoals(int goalstate);
void	trap_BotDumpGoalStack(int goalstate);
void	trap_BotGoalName(int number, char *name, int size);
int		trap_BotGetTopGoal(int goalstate, void /* struct bot_goal_s */ *goal);
int		trap_BotGetSecondGoal(int goalstate, void /* struct bot_goal_s */ *goal);
int		trap_BotChooseLTGItem(int goalstate, vec3_t origin, int *inventory, int travelflags);
int		trap_BotChooseNBGItem(int goalstate, vec3_t origin, int *inventory, int travelflags, void /* struct bot_goal_s */ *ltg, float maxtime);
int		trap_BotTouchingGoal(vec3_t origin, void /* struct bot_goal_s */ *goal);
int		trap_BotItemGoalInVisButNotVisible(int viewer, vec3_t eye, vec3_t viewangles, void /* struct bot_goal_s */ *goal);
int		trap_BotGetNextCampSpotGoal(int num, void /* struct bot_goal_s */ *goal);
int		trap_BotGetMapLocationGoal(char *name, void /* struct bot_goal_s */ *goal);
int		trap_BotGetLevelItemGoal(int index, char *classname, void /* struct bot_goal_s */ *goal);
float	trap_BotAvoidGoalTime(int goalstate, int number);
void	trap_BotSetAvoidGoalTime(int goalstate, int number, float avoidtime);
void	trap_BotInitLevelItems(void);
void	trap_BotUpdateEntityItems(void);
int		trap_BotLoadItemWeights(int goalstate, char *filename);
void	trap_BotFreeItemWeights(int goalstate);
void	trap_BotInterbreedGoalFuzzyLogic(int parent1, int parent2, int child);
void	trap_BotSaveGoalFuzzyLogic(int goalstate, char *filename);
void	trap_BotMutateGoalFuzzyLogic(int goalstate, float range);
int		trap_BotAllocGoalState(int state);
void	trap_BotFreeGoalState(int handle);

void	trap_BotResetMoveState(int movestate);
void	trap_BotMoveToGoal(void /* struct bot_moveresult_s */ *result, int movestate, void /* struct bot_goal_s */ *goal, int travelflags);
int		trap_BotMoveInDirection(int movestate, vec3_t dir, float speed, int type);
void	trap_BotResetAvoidReach(int movestate);
void	trap_BotResetLastAvoidReach(int movestate);
int		trap_BotReachabilityArea(vec3_t origin, int testground);
int		trap_BotMovementViewTarget(int movestate, void /* struct bot_goal_s */ *goal, int travelflags, float lookahead, vec3_t target);
int		trap_BotPredictVisiblePosition(vec3_t origin, int areanum, void /* struct bot_goal_s */ *goal, int travelflags, vec3_t target);
int		trap_BotAllocMoveState(void);
void	trap_BotFreeMoveState(int handle);
void	trap_BotInitMoveState(int handle, void /* struct bot_initmove_s */ *initmove);
void	trap_BotAddAvoidSpot(int movestate, vec3_t origin, float radius, int type);

int		trap_BotChooseBestFightWeapon(int weaponstate, int *inventory);
void	trap_BotGetWeaponInfo(int weaponstate, int weapon, void /* struct weaponinfo_s */ *weaponinfo);
int		trap_BotLoadWeaponWeights(int weaponstate, char *filename);
int		trap_BotAllocWeaponState(void);
void	trap_BotFreeWeaponState(int weaponstate);
void	trap_BotResetWeaponState(int weaponstate);

int		trap_GeneticParentsAndChildSelection(int numranks, float *ranks, int *parent1, int *parent2, int *child);

void	trap_SnapVector( float *v );

int			trap_CM_RegisterTerrain(const char *config);

void		*trap_VM_LocalAlloc ( int size );
void		*trap_VM_LocalAllocUnaligned ( int size );			// WARNING!!!! USE WITH CAUTION!!! BEWARE OF DOG!!!
void		*trap_VM_LocalTempAlloc( int size );
void		trap_VM_LocalTempFree( int size );					// free must be in opposite order of allocation!
const char	*trap_VM_LocalStringAlloc ( const char *source );

// Gametype traps
void		trap_GT_Init		( const char* gametype, qboolean restart );
void		trap_GT_RunFrame	( int time );
void		trap_GT_Start		( int time );
int			trap_GT_SendEvent	( int event, int time, int arg0, int arg1, int arg2, int arg3, int arg4 );

void G_UpdateClientAntiLag	( gentity_t* ent );
void G_UndoAntiLag			( void );
void G_ApplyAntiLag			( gentity_t* ref, qboolean enlargeHitBox );

// KRIS 16/04/2003 3:54PM
#ifdef _CRS_BOTS
extern vmCvar_t	bot_editWPS;
extern vmCvar_t	bot_debug;
extern vmCvar_t	bot_debugNav;
extern vmCvar_t	bot_debugTeamplay;
extern vmCvar_t	bot_debugGameType;
#endif
// KRIS

//Ryan april 28 2003
#include "rpm_local.h"
//Ryan

//void NV_ip2CountryFileSelect ( int clientNum);
//qboolean NV_QueryDB(int type, void *extra);

