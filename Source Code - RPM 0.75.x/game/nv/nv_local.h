//////////////////////////////////////////////
//	Prototypes - Server
/////////////////////////////////////////////


#define FINISH_ADMIN_ONLY			0x00000001
#define FINISH_LOG					0x00000002
#define FINISH_PRINT				0x00000004
#define FINISH_CP					0x00000008
void NV_Finish (gentity_t *adm, gentity_t *ent, char *action, int flags);

void NV_ToggleTeamGTItemDraws ( int flag1, int flag2 );
void NV_GTItemNoDraw(void);


void NV_swapTeams	(int argNum, gentity_t *adm, int idnum);
void NV_evenTeams	(int argNum, gentity_t *adm, int idnum);
void NV_setRestriction	(char *cmd);
qboolean NV_rconCmds	(char *cmd);

void NV_sendGametypeHelp (gentity_t *ent, qboolean first);


void NV_playerDropItem (gentity_t *player, gentity_t *attacker, char *itemName);
void NV_playerDropRandomItem (gentity_t *player, gentity_t *attacker, qboolean announceItem);

///nv_utils.c

void NV_spawnBombEffect		(gentity_t *ent);
void NV_ClientBegin			( int clientNum );
void NV_bomb_kill			( gentity_t *self );


/// #gametypes
qboolean NV_CheckTouchItem  (gentity_t *ent, gentity_t *other);
qboolean NV_CheckTouchTrigger(gentity_t *ent, gentity_t *other);
qboolean NV_CheckBombTimer	(gentity_t *ent);
qboolean NV_disArmBomb		(gentity_t *ent, gentity_t *other);

qboolean NV_isDom			(void);
qboolean NV_isKoth			(void);
qboolean NV_isSab			(void);
qboolean NV_isTFT			(void);
qboolean NV_isFT			(void);
qboolean NV_isDem			(void);
qboolean NV_isRaze			(void);
qboolean NV_isRush			(void);
qboolean NV_isCTC			(void);
qboolean NV_isCR			(void);
qboolean NV_isVIP			(void);
qboolean NV_isInf			(void);
qboolean NV_isTctf			(void);
qboolean NV_isOctf			(void);
qboolean NV_isGG			(void);
qboolean NV_isDctf			(void);
qboolean NV_isOP			(void);
#ifdef _ZOMBIES
qboolean NV_isZom			(void);

void ZOM_SetTeam( gentity_t *ent, char *s, const char* identity );
#endif

#ifdef _FAIR_WEAPON_STATS
qboolean NV_WeaponStatsOk(void);
#endif

void NV_checkMisc (void);
void NV_SQLAdminLog(gentity_t *adm, gentity_t *ent, char* cmd);

#ifdef _MAPEDITOR

void NV_duplicateEnt		(gentity_t *ent);
void NV_createEntFile		(gentity_t *ent);
void G_GametypeItemThink	(gentity_t* ent);
void gametype_trigger_touch (gentity_t *self, gentity_t *other, trace_t *trace);
void NV_TestBox				(gentity_t *ent, int color, int time);
void G_TestLine				(vec3_t start, vec3_t end, int color, int time);
char *ConcatArgs			(int start);
void NV_drawEntMarkers		(void);
void NV_CheckClientEntEdit	(gentity_t *ent);
qboolean NV_mapEntCommands	(gentity_t* ent);
void NV_drawEntTimer		(void);
void NV_saveShaders			(gentity_t *ent); 
void NV_entFields			(fileHandle_t f, gentity_t *ent);
void NV_PrintEntInfo(		gentity_t *ent, gentity_t *traceEnt);
void NV_showShaders (gentity_t *ent);

#endif

void NV_voteCommands		(gentity_t *ent, char *input);

///#spMaps
void NV_ChangeEntField(gentity_t *ent, const char *key, const char *value);
void ROT_SetMoverState		(gentity_t *ent, moverState_t moverState, int time);
void SP_func_door_rotating	(gentity_t *ent);
char *G_GetEntFileToken		(void);
qboolean G_LoadEntFile		(void);
qboolean G_ReadingFromEntFile(qboolean inSubBSP);
void NV_EditEntInfo			(gentity_t *ent);
void NV_viewEntFieldInfo	(gentity_t *ent);
void SP_func_breakable		(gentity_t *ent);
//void NV_Breakable_bsp		(gentity_t *ent); 
void G_BreakableDie			(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod, int hitLocation, vec3_t hitDir);

void bomb_target_red_use	( gentity_t *self, gentity_t *other, gentity_t *activator ); 
void bomb_target_blue_use	( gentity_t *self, gentity_t *other, gentity_t *activator ); 


void NV_FlareEffect			(team_t t);

team_t NV_PickTeam( int ignoreClientNum, team_t TEAM_WANTED, team_t TEAM_CURRENT );

gentity_t *LaunchItem( gitem_t *item, vec3_t origin, vec3_t velocity ); 
//void NV_GiveRoundItem ( void );
void NV_GiveGTIToTeam ( team_t team, qboolean announce );
//void NV_GiveBombToPlayer ( int player );
//void NV_GiveVIPItemToTeam ( team_t team );

//void NV_setVIPIdent(void);


void NV_ResetBreakable		(void);
qboolean NV_ParseSpawnVars	(qboolean inSubBSP); 

// ----  GAMETYPES -----  //
///#dom
void NV_updateSpawnTeam (gentity_t *flagEnt);
//void NV_updateKothSpawnTeam (gentity_t *flagEnt);
void NV_updateKothSpawnTeam (team_t team);
int NV_spawnLinked		(vec3_t pos);
void NV_reLinkSpawns	(void);
//void NV_SetTeamSpawns  (gentity_t *ent, team_t team, qboolean disabled);
//void NV_linkSpawnToEnt	(void);
void NV_SetPhaseSpawns	(void);


//qboolean NV_TeamDeadNoLives (team_t team);
void NV_checkGametypePhase (void);


//void NV_setGameTypeName (void);
char *NV_setGameTypeName(void);
void player_freeze		( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod, int hitLocation, vec3_t hitDir );
void NV_unFreeze		( gentity_t *targ, gentity_t *attacker, int mod );
void NV_Freeze			( gentity_t *targ, gentity_t *attacker );
void NV_freeFrozenModel ( gentity_t *ent );
void NV_teamSkin		( gentity_t *targ, char *skin, int team);
//void NV_CopyToBodyQue	( gentity_t *ent, int hitLocation, vec3_t direction );
void NV_spawnFrozen		( gentity_t* ent );
void Cmd_DropItem_f		( gentity_t* ent );
//void NV_createFrozenSpawn(gentity_t *ent);
void NV_respawn			(gentity_t *ent, qboolean rescued );
void NV_setTrueGameType	(qboolean restart);
char *NV_getBasedGameType	(char* gametype);

void NV_Spawn_Target_Models ( gentity_t *ent );
void NV_Spawn_Target_Die(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod, int blank, vec3_t a);

/// ----          -----  //
#ifdef _CHAT_KEYWORDS
void NV_ParseKeywordChats	(char *filename);
#endif // _CHAT_KEYWORDS

void TeamSound			(int team, char *sound, char* chat);

void NV_entFields			(fileHandle_t f, gentity_t *ent);
void NV_MissileDie			(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod, int filler, vec3_t filler2);
void NV_ItemDie				(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod, int filler, vec3_t filler2);
qboolean NV_RemoveFromFile	(char *file, char *ip, qboolean caseSensitive);
qboolean NV_canUseCommand	(gentity_t *adm, int idnum, const char* useDead, qboolean useOnAdmin);
void NV_clusterNade			(gentity_t *ent, int weapon, int numNades, int damage);
void NV_Sound(int clientNum, char *file );
void NV_loadSettingFiles	(void);
void NV_getMapList			(qboolean restart);
void NV_getRandomMapList	(qboolean restart);
void NV_SetTriggerByName			(const char* name, int type);
void NV_printMapList		(gentity_t *ent);
void NV_printConsoleTokenList		(gentity_t *ent);
void NV_Print_File (gentity_t *ent, char *file);


void NV_spawnFlagMarker		(gentity_t *ent);

qboolean NV_doRandomMap (void);

char *NV_SexyColors(const char *text, int color);

void hurt_touch( gentity_t *self, gentity_t *other, trace_t *trace );
void hurt_use( gentity_t *self, gentity_t *other, gentity_t *activator );
void G_ShutdownGame				( int restart );
void Svcmd_EntityList_f			(void);

int			trap_RealTime	(qtime_t *qtime);
void		G_Say			(gentity_t *ent, gentity_t *target, int mode, const char *chatText ); 
void		Cmd_Say_f		(gentity_t *ent, int mode, qboolean arg0 );
char		*ConcatArgs		(int start);
void		G_ApplyKnockback(gentity_t *targ, vec3_t newDir, float knockback);
int			QDECL SortRanks	(const void *a, const void *b );
qboolean	BG_InitWeaponStats( void ); ///added for g_main (weaponModFile)
const char *G_GetArenaInfoByMap( const char *map );
gspawn_t*	G_SelectRandomSafeSpawnPoint ( team_t team, float safeDistance);
void		hurt_use( gentity_t *self, gentity_t *other, gentity_t *activator );
void		target_effect_delayed_use ( gentity_t* self );

void		G_DropGametypeItems	( gentity_t* self, int delayPickup );
qboolean	G_DoesMapExist( const char* mapname );
void		BG_ApplyLeanOffset ( playerState_t* ps, vec3_t origin );
void		G_FreeEnitityChildren( gentity_t* ent );
void		gametype_item_use	( gentity_t* self, gentity_t* other );

gclient_t*	NV_FindClientByCleanName(const char* name, int ignoreNum);
void NV_GiveRewardSpawn(gentity_t *ent);
qboolean NV_mapEditCmds(gentity_t *ent);


qboolean NV_ParseNewShaders	(void);

#ifdef _KILLSPREE

typedef struct nv_spreeRewards_s
{
	int		killsNeeded;
	int		reward;
	int		clipsize;
	int		extraclips;
	int		clipsizeAlt;
	int		extraclipsAlt;
	char	sound[MAX_QPATH];
	char	centerMsg[MAX_SAY_TEXT];
	char	globalMsg[MAX_SAY_TEXT];
	char	consoleMsg[MAX_SAY_TEXT];
//	char	CPMsg[MAX_SAY_TEXT];
} nv_spreeRewards_t;

void NV_killSpreeNotify (gentity_t *ent, int spree, nv_spreeRewards_t info);
qboolean NV_checkKillSpree(gentity_t *killer, gentity_t *target, int type);

#endif // _KILLSPREE



#define MAX_NV_GAMETYPES 20

typedef struct customGametypeInfo_s { 
	const char	*name;
	int			color;
	const char	*based;
	const char	*longName;
	const char	*help;

} customGametypeInfo_t;

extern	customGametypeInfo_t gtInfo[];
//

#ifdef _NEW_MAP_SETUP
#define MAX_MAPLIST		40
typedef struct nv_mapList_t
{
	char map[MAX_QPATH];
	char gametype[8];
} nv_mapList_t;


#define MAX_KEYWORDS		10
#define MAX_RESPONSES		6
#define MAX_KEYWORDTOKENS	32


typedef struct
{
	//char		*keyword[MAX_KEYWORDS];
	char		keyword[MAX_KEYWORDS][32];
	char		reponse[MAX_RESPONSES][MAX_SAY_TEXT];
	int			level;
	int			lastTime;
	int			count[3];	//0 = keyword count, 1 = response count; 2 = last response said; 
	qboolean	clientOnly;
} nv_keywords_t;

#endif


void NV_ServerKeyWords		(gentity_t *ent, char *chatText);

#ifdef _SPECIAL_NADES
void NV_Touch_Gas			(gentity_t *ent, gentity_t *other, trace_t *trace );
void NV_Spawn_Gas			(gentity_t *gas, gentity_t *owner);
void NV_checkGas			(gentity_t *ent);
#endif //_SPECIAL_NADES

char *stristr(char *str, char *charset);

void QDECL NV_Log( int type, const char *text, ... );

void NV_killThreads (void);


qboolean NV_clientOnceSecondActions(gentity_t *ent, qboolean specOnly);


void NV_GunGameGiveWeapon(gentity_t *ent);
void NV_GunGamePromote (gentity_t* ent, gentity_t* victim, int weapon);
void NV_GunGameDemote (gentity_t* ent, gentity_t* attacker);


#ifdef _CLEAR_DROPPED
void NV_clearDroppedItems (void);
#endif


#ifdef _EFFECTS
void NV_getEffectsList (void);
void NV_effectsInc(gentity_t *ent);
void NV_effectsDec(gentity_t *ent);
#endif

#ifdef _NV_ADMIN
void NV_cmdsAreAllowed		(gentity_t *adm, char *arg);
#endif


void NV_adminPrint(char *input );



int NV_CheckAdmin( int clientNum );
void NV_LoadFile( char *file, char *buf, qboolean caseSensitive );
qboolean NV_extraCommands	(gentity_t *ent, char *input);

#ifdef _DB_ENABLED
void setMapStats(void);
void NV_SaveMapStats(void);


void DB_BAN					(gentity_t* ent);
void NV_SaveStats			(gentity_t* ent);
void NV_doGameStats			(void);
void NV_doWeaponStats		(gentity_t *ent);


void NV_ClientStats			(void);


#endif



#ifdef _OLDVIP
void NV_GiveDistanceHealth (gentity_t* self);
#endif


void vip_marker_wait	(gentity_t *self);
void vip_marker_touch	( gentity_t *self, gentity_t *other, trace_t *trace );
void vip_marker_think	( gentity_t *self );

gspawn_t* VIP_GetVIPSpawnPoints (gentity_t* ent, team_t team);

void NV_ip2CountryFileSelect ( int clientNum);


void NV_setHudIcon(gentity_t *ent, qboolean touching);



#ifdef _DB_WEBCMDS
void NV_WebAdmin (void);
#endif


