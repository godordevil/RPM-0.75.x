
int trap_RealTime ( qtime_t *qtime );

#define MAX_NEIGHBOURS 	8
#define MAX_WPPATH	16

#define	MAX_WP_STEP		48
#define	MAX_WP_STEP_Z		64
#define	MAX_RMG_WP_STEP		256
#define	MAX_RMG_WP_STEP_Z	128

typedef struct wayPoint_s
{
	vec3_t origin;
	vec3_t angle;
	qboolean free;
	int flags;
	int index;
	int ladder;
	struct wayPoint_s *next;
	int nIndexes[MAX_NEIGHBOURS];
	struct wayPoint_s *neighBours[MAX_NEIGHBOURS];
	struct wayPoint_s *parent;
	struct wayPoint_s *open;
	int pathcost;
	int heuristic;
} wayPoint_t;

typedef struct
{
	qboolean 	valid; 			// has been updated this frame
	wayPoint_t 	*wpClosest;		// closest way point
	qboolean 	dangerous;		// can hurt us (ie. grenade or knife)
	qboolean 	obscures;
	int		GTI;
} botEntInfo_t;

extern	botEntInfo_t bot_entInfo[MAX_GENTITIES];

extern int wp_totalCount;
extern wayPoint_t *wpHead;

typedef struct
{
	int index;
	char name[32];
	int team;
	char target[32];
	char target2[32];
	char target3[32];
	int droppedTeam;
	qboolean use;
	qboolean useTarget;
	int guard;
	int guardDropped;
} botGametypeItemInfo_t;

extern botGametypeItemInfo_t bot_GTInfo[MAX_GAMETYPE_ITEMS];

typedef struct
{
	int index;		// index of waypoint this history is for
	int lastUpdated;	// last time we updated this waypoints history
	int costAdjustment;	// amout of we're going to adjust this waypoint for
} wpHistory_t;

#define MAX_WP_HISTORY	512

#define WPFLAG_LADDER			0x00000010

#define WPFLAG_DUCK			0x00000020 //duck while moving around here
#define WPFLAG_NOVIS			0x00000400 //go here for a bit even with no visibility
#define WPFLAG_SNIPEORCAMPSTAND		0x00000800 //a good position to snipe or camp - stand
#define WPFLAG_WAITFORFUNC		0x00001000 //wait for a func brushent under this point before moving here
#define WPFLAG_SNIPEORCAMP		0x00002000 //a good position to snipe or camp - crouch
#define WPFLAG_ONEWAY_FWD		0x00004000 //can only go forward on the trial from here (e.g. went over a ledge)
#define WPFLAG_ONEWAY_BACK		0x00008000 //can only go backward on the trail from here
#define WPFLAG_GOALPOINT		0x00010000 //make it a goal to get here.. goal points will be decided by setting "weight" values
#define WPFLAG_RED_FLAG			0x00020000 //red flag
#define WPFLAG_BLUE_FLAG		0x00040000 //blue flag
#define WPFLAG_NOMOVEFUNC		0x00200000 //don't move over if a func is under

#define WP_KEEP_FLAG_DIST			128
