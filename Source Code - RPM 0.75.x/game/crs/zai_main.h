#ifdef _CRS_BOTS	
#include "zg_waypoint.h"

void G_TestLine(vec3_t start, vec3_t end, int color, int time);

// zai_main.c & ai_main.c defs
#define ACTION_RELOAD		0x1000000
#define ACTION_ZOOMIN		0x2000000
#define ACTION_ZOOMOUT		0x8000000

#define MAX_CHAT_BUFFER_SIZE 65536
#define MAX_CHAT_LINE_SIZE 128

#define BWEAPONRANGE_MELEE			1
#define BWEAPONRANGE_SHORT			2
#define BWEAPONRANGE_MID			3
#define BWEAPONRANGE_LONG			4
#define BWEAPONRANGE_VERY_LONG			5

#define MELEE_ATTACK_RANGE			256
#define MAX_CHICKENWUSS_TIME		10000 //wait 10 secs between checking which run-away path to take

#define BOT_RUN_HEALTH				40
#define ENEMY_FORGET_MS				10000
//if our enemy isn't visible within 10000ms (aprx 10sec) then "forget" about him and treat him like every other threat, but still look for
//more immediate threats while main enemy is not visible


#define MAX_PROJECTILE_DISTANCE		256

typedef enum
{
	TEAMPLAYSTATE_NONE,
	TEAMPLAYSTATE_BUSY,
	TEAMPLAYSTATE_GUARDING_GTI,
	TEAMPLAYSTATE_GUARDING_TRIGGER,
	TEAMPLAYSTATE_GETTING_GTI,
	TEAMPLAYSTATE_FOLLOWING,
	TEAMPLAYSTATE_ASSISTING,
	TEAMPLAYSTATE_REGROUP,
	TEAMPLAYSTATE_MAXTPSTATES
} bot_teamplay_state_t;

typedef struct boteventtracker_s
{
	int			eventSequence;
	int			events[MAX_PS_EVENTS];
	float		eventTime;
} boteventtracker_t;

typedef struct botskills_s
{
	int					reflex;
	float				accuracy;
	float				turnspeed;
	float				turnspeed_combat;
	float				maxturn;
	int					perfectaim;
} botskills_t;

//bot state
typedef struct bot_state_s
{
	int inuse;										//true if this state is used by a bot client
	int client;										//client number of the bot
	int entitynum;									//entity number of the bot
	playerState_t cur_ps;							//current player state
	usercmd_t lastucmd;								//usercmd from last frame
	bot_settings_t settings;						//several bot settings
	float thinktime;								//time the bot thinks this frame
	vec3_t origin;									//origin of the bot
	vec3_t velocity;								//velocity of the bot
	vec3_t eye;										//eye coordinates of the bot
	int setupcount;									//true when the bot has just been setup
	float ltime;									//local bot time
	float entergame_time;							//time the bot entered the game
	int ms;											//move state of the bot
	int gs;											//goal state of the bot
	int ws;											//weapon state of the bot
	vec3_t viewangles;								//current view angles
	vec3_t ideal_viewangles;						//ideal view angles
	vec3_t viewanglespeed;

	//rww - new AI values
	gentity_t			*currentEnemy;

	gentity_t			*squadLeader;
	gentity_t			*teamLeader;
	int					findTeamLeader;
	qboolean 			isTeamLeader;

	gentity_t			*lastHurt;
	gentity_t			*lastAttacked;

	gentity_t			*touchGoal;
	gentity_t			*shootGoal;

	gentity_t			*dangerousObject;

	qboolean			isSquadLeader;

	int				squadRegroupInterval;
	int				squadCannotLead;

	int				lastDeadTime;

	wayPoint_t			*wpCurrent;
	wayPoint_t			*wpDestination;
	wayPoint_t			*wpStoreDest;
	vec3_t				goalAngles;
	vec3_t				goalMovedir;
	vec3_t				goalPosition;

	vec3_t				lastEnemySpotted;
	vec3_t				hereWhenSpotted;
	int				lastVisibleEnemyIndex;

	float				destinationGrabTime;
	float				wpSeenTime;
	float				wpTravelTime;
	float				wpDestSwitchTime;
	float				wpSwitchTime;
	float				wpDestIgnoreTime;

	float				timeToReact;

	float				enemySeenTime;

	float				chickenWussCalculationTime;

	float				beStill;
	float				duckTime;
	float				jumpTime;
	float				jDelay;

	float				aimOffsetTime;
	float				aimOffsetAmtYaw;
	float				aimOffsetAmtPitch;

	float				frame_Waypoint_Len;
	int				frame_Waypoint_Vis;
	float				frame_Enemy_Len;
	int				frame_Enemy_Vis;

	int				isCamper;
	float				isCamping;
	wayPoint_t			*wpCamping;
	wayPoint_t			*wpCampingTo;
	qboolean			campStanding;

	int				randomNavTime;
	int				randomNav;

	int				meleeSpecialist;

	int				canChat;
	int				chatFrequency;
	char				currentChat[MAX_CHAT_LINE_SIZE];
	float				chatTime;
	float				chatTime_stored;
	int				doChat;
	int				chatTeam;
	gentity_t			*chatObject;
	gentity_t			*chatAltObject;

	float				meleeStrafeTime;
	int				meleeStrafeDir;
	float				meleeStrafeDisable;

	int				altChargeTime;

	float				escapeDirTime;

	float				dontGoBack;

	int				doAttack;
	int				doAltAttack;

	int				forceWeaponSelect;
	int				virtualWeapon;

	int				runningLikeASissy;
	int				runningToEscapeThreat;

	//char				chatBuffer[MAX_CHAT_BUFFER_SIZE];
	//Since we're once again not allocating bot structs dynamically,
	//shoving a 64k chat buffer into one is a bad thing.

	botskills_t			skills;

	int				deathActivitiesDone;

	float				botWeaponWeights[WP_NUM_WEAPONS];
	int				teamplayState;
	int				state_Forced; //set by player ordering menu
	int				noUseTime;

	// KRIS 28/02/2003 5:22PM
	float launchAngle;
	int doReload;
	int doZoomIn;
	int doZoomOut;
	float lastAimUp;
	//vec3_t				launchAngle;
	wayPoint_t			*wpLastPoint;
	wayPoint_t			*wpNextPoint;
	wayPoint_t 			*wpPath[MAX_WPPATH];
	int					lastPathDebug;
	int				debugPathIndex;
	int				touchGoalTime;
	int			fuckUp;
	int			lastFuckUp;
	qboolean	needDecentWeapon;
	qboolean	needExtraAmmo;
	qboolean	needHealth;
	qboolean	needArmor;
	qboolean	wantsNewWeapon;
	qboolean	wantsMoreAmmo;
	qboolean	wantsHealth;
	qboolean	wantsArmor;
	int 		teamLeader_giveOrders;

	gentity_t	*assistTeamMate;
	gentity_t	*guardGTI;
	gentity_t	*guardTrigger;
	gentity_t	*getGTI;
	int 		strafeTime;
	int 		strafeDir;
	wpHistory_t 	wpHistory[MAX_WP_HISTORY];	// keep track of the last wayPoints we visited
	int		useTime;
	int		flashbangTime;
	int		flashbangFadeTime;
	float		flashbangAlpha;
	int		flashbangDeafTime;
	int		flashbangDeafFadeTime;
	// KRIS
	//end rww
} bot_state_t;

void *B_TempAlloc(int size);
void B_TempFree(int size);

void *B_Alloc(int size);
void B_Free(void *ptr);

//resets the whole bot state
void BotResetState(bot_state_t *bs);

void BotUtilizePersonality(bot_state_t *bs);
int BotDoChat(bot_state_t *bs, char *section, int always);
void StandardBotAI(bot_state_t *bs, float thinktime);
int OrgVisibleBox(vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore);
int BotIsAChickenWuss(bot_state_t *bs);
wayPoint_t *GetNearestVisibleWP(vec3_t org, int ignore);
wayPoint_t *GetBestIdleGoal(bot_state_t *bs);

extern char gBotChatBuffer[MAX_CLIENTS][MAX_CHAT_BUFFER_SIZE];

extern float floattime;
#define FloatTime() floattime

qboolean QDECL BotDebug(bot_state_t *bs, vmCvar_t *cvar, int value, const char *msg, ...);



#endif 

