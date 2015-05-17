
#include "../g_local.h"


static char *statColor = "^+";
char *DB_fadeColor(int color)
{	
	if(!color) 
		Q_irand(1, 6);

	switch(color)
	{
	case CT_BLACK:
		return "-,+7";
	case CT_RED:
		return  "$To+";
	default: 
	case CT_GREEN:
		return  "<j?+";
	case CT_BLUE:		
		return  "iyf+";
	case CT_YELLOW:
		return  "Nzkk";
	case CT_MAGENTA:
		return  "emrL";
	}		
}

char *NV_SexyColors(const char *text, int color) 
{
	char	out[192];
	char	*colors;
	int		len;
	int		i = 0, k = 0;

	len = strlen(text);	
	if(len >= 3)
	{
		colors = DB_fadeColor(color);
	}
	else return (char*)text;
	while (text[i] != 0 ) 
	{	
		if(i < 4)
		{
			out[k++] = '^';
			out[k++] = colors[i];
		}
		else if(len > 5 && i > len - 4)
		{
			out[k++] = '^';
			out[k++] = colors[len - i - 1];
		}
		out[k++] = text[i++];
	}
	out[k++] = '\0';
	return va("%s", out);
}

//typedef struct {
//	const char	*name;
//	int			color;
//	const char	*based;
//	const char	*longName;
//	const char	*help;
//
//} customGametypeInfo_t;

// Must match - customGameType_t - for gametype's

/*static*/ customGametypeInfo_t gtInfo[] = 
{
	{ "",				CT_YELLOW,			"",		"",					""},
	///Name				Name COLOR			based	Long name			help text	
	{"dm",				CT_BLACK,			"dm",	"Death Match",		""},	 
	{"tdm",				CT_MAGENTA,			"tdm",	"Team Death Match",	""},	 
	{"inf",				CT_YELLOW,			"inf",	"Infiltration",		""},	 
	{"elim",			CT_GREEN,			"elim",	"Elimination",		""},	 
	{"ctf",				CT_RED,				"ctf",	"Capture the Flag",	""},	 
	
	{"dom",				CT_MAGENTA,			"ctf",	"Domination",		"Dominate the map by controlling key points\n^1Capture: ^+Stand near the flag to gain control"},	 
	{"koth",			CT_YELLOW,			"ctf",	"King of the Hill",	"Controlling the Zone\n\n^3Fight for the zone marked by the flag!\n^1Capture: ^+Stand near the flag to gain control"},	 
	{"sab",				CT_GREEN,			"inf",	"Sabotage",			"^3Destroy ^+the ^1enemy target ^+with the ^3bomb!\n^3PLANT: ^+Stand near the target with the bomb\n^1DEFUSE: ^+Touch the Bomb until it deactivates!"},	 
	{"dem",				CT_BLACK,			"inf",	"Demolition",		"^4Blue: ^+Plant the bomb on a target\n^1Red: ^+Defend targets & defuse planted bombs"},	 
	{"rush",			CT_RED,				"ctf",	"Rush",				"^4Blue: ^+Destroy targets!\n^1Red: ^+Protect targets, kill ^4BLUE ^+Team."},	 
	{"cr",				CT_GREEN,			"inf",	"Case Run",			"^3Hold ^+the briefcase ^+to gain points.\n^+Keep the enemy from holding the case!"},	
	{"tft",				CT_BLUE,			"elim",	"Team Freeze Tag",	"^4Freeze ^+your enemy by killing them!\n^+Use ^3knives ^+to ^1rescue ^+frozen team mates!"},	
	{"ft",				CT_BLUE,			"dm",	"Freeze Tag",		"^4Freeze ^+your enemies to gain points!\n^3Stabbing or cutting ^iFrozen ^+players will \n^+make them ^1drop ^+a random item\n^1Be careful^+, they will ^3respawn ^+quicker!"},
	{"ctc",				CT_GREEN,			"inf",	"Capture the Case", "^+Teams must take the ^3case ^+to the ^3extraction zone^+!"},	
	
	{"cctf",			CT_RED,				"ctf",	"Classic Capture the Flag", "^+Teams can only capture the flag\n^+when their flag is at\n^+their base."},	
	{"tctf",			CT_BLACK,			"ctf",	"Tactical Capture the Flag", "^+Similar to CTF but with round limits\n^+Capture and Defend your teams Flags\n^+The team that wins the most rounds\n^+will achieve victory!"},	
	{"actf",			CT_BLUE,			"ctf",	"Any Capture The Flag", "^+Pickup ^3ANY ^+flag\n^1Capture: ^+Take the flag to the ^3OPPOSITE\n^+capture point!"},	
	{"octf",			CT_BLUE,			"ctf",	"One Capture The Flag", "^+Take the ^3flag ^+to your teams ^3capture point^+!"},	

	{"con",				CT_RED,				"ctf",	"Conquer",			""},		
	{"raze",			CT_BLUE,			"inf",	"Raze",				""},	 
	{"fort",			CT_RED,				"tdm",	"Fortress",			""},	

#ifdef _DCTF
	{"dctf",			CT_YELLOW,			"ctf",	"Dynamic Capture The Flag", "^2Take the ^2flag ^2to your teams ^3capture point^2!\nOnce a flag is captured\nFlags and Capture Points will be changed!"},	
#endif

	{"gg",				CT_BLACK,			"dm",	"Gun Game",			"^5Kill players to get weapons!\n^6The first person to get a ^3kill ^6with\n^6each weapon ^3WINS^+!"},	
#ifdef _OVERPOWER
	{"op",				CT_RED,				"ctf",	"Overpower",		"^5Take the flag to your base!\n^6Protect the flag at all costs\nYou gain points for the\nflag being at your base^+!"},	
#endif
#ifdef _ZOMBIES
	{"zom",				CT_GREEN,			"inf",	"Zombies",			"^1Survive"},	
#endif
	{"vip",				CT_GREEN,			"ctf",	"V.I.P.",			 "^3Protect and Kill!\n^6Kill the ENEMY VIP\n^3Protect your VIP\n^+Use ^3beacons ^+to find the ^3enemy VIP^+!"},	

	{ "",				CT_YELLOW,			"",		"",					""}
};

//int		gt_maxCustom = sizeof(gtInfo) / sizeof(gtInfo[0]) - 1;


void NV_sendGametypeHelp (gentity_t *ent, qboolean first)
{
#ifdef _DEBUG_OFF
	static int i = -1;
	if( i >= sizeof( gtInfo ) / sizeof( gtInfo[0]) - 1)
		i = -1;

	strcpy(out, NV_SexyColors( gtInfo[i+1].longName,  gtInfo[i+1].color ));
	strcpy(line, NV_SexyColors( "-------------------------------------------",  gtInfo[i+1].color ));

	if(first)
	{
		i++;
		while(!gtInfo[i].help)
		{
			i++;
			if( i >= sizeof( gtInfo ) / sizeof( gtInfo[0]) - 1)
				i=-1;
		}			
		CPx( ent->s.number, va("cp \"%s\n%s\n%s\n%s\n%s\n%s\n\n%s\n", server_motd1.string, server_motd2.string, line, out, line, gtInfo[i].help, server_motd3.string));
		CPx( ent->s.number, va("print \"^,------------------------------------------------------\n\""));
	}
	else // Dont print to the console
	{
		CPx( ent->s.number, va("cp \"@%s\n%s\n%s\n%s\n%s\n%s\n\n%s\n", server_motd1.string, server_motd2.string, line, out, line, gtInfo[i].help, server_motd3.string));
	}
	return;

#endif		

	int i;
	char line[256];
	strcpy(line, NV_SexyColors( "-------------------------------------------",  gtInfo[gt_customType.integer].color ));
	
	if(ent!=NULL)
	{
		CPx( ent->s.number, va("cp \"@%s\n%s\n%s\n%s\n",line, nv_gtName.string, line, gtInfo[gt_customType.integer].help));
	}
	else
	{
		for ( i = 0 ; i < level.maxclients ; i++ )
		{
			if (level.clients[i].pers.connected != CON_CONNECTED )
				continue;
			if(!level.gametypeStartTime)
				CPx( i, va("cp \"@%s%i\n%s\n%s\n%s\n%s\n", "STARTING IN: ", level.clients[i].ps.stats[STAT_FROZEN] / 1000, line, nv_gtName.string, line, gtInfo[gt_customType.integer].help));
			else
				CPx( i, va("cp \"@%s\n%s\n%s\n%s\n", line, nv_gtName.string, line, gtInfo[gt_customType.integer].help));
		}
	}
}
char *NV_getBasedGameType (char* gametype)
{
	int i;

	for ( i = 0 ; i < sizeof( gtInfo ) / sizeof( gtInfo[0] ) ; i++ )
	{
		if(!Q_stricmp(gametype, gtInfo[i].name))
		{
			return (char*)gtInfo[i].based;
		}
	}
	// Default shit just incase.. i donno, whatever happens
	return gametype;
}
char *NV_setGameTypeName ( void )
{	
	if(gt_customType.integer)
	{ 
		char out[256] = "";
		strcpy(out, NV_SexyColors( gtInfo[gt_customType.integer].longName,  gtInfo[gt_customType.integer].color ));
		trap_Cvar_Set("nv_gtName", out);
		//trap_Cvar_Set("nv_gtName", gtInfo[gt_customType.integer].longName );
		trap_Cvar_Update( &nv_gtName );
		trap_Cvar_Set("nv_gtShort", gtInfo[gt_customType.integer].name);
		trap_Cvar_Update( &nv_gtShort );
		return (char*)gtInfo[gt_customType.integer].name;	
	}
	trap_Cvar_Set("nv_gtName", level.gametypeData->displayName);
	trap_Cvar_Update( &nv_gtName );
	trap_Cvar_Set("nv_gtShort", level.gametypeData->name);
	trap_Cvar_Update( &nv_gtShort );
	return (char*)level.gametypeData->name;
}

void NV_setTrueGameType (qboolean restart)
{
	int i;
		
	if(restart)
	{
		trap_Cvar_Set("g_gametype", va("%s", gtInfo[gt_customType.integer].based));
		trap_Cvar_Update (&g_gametype);
		return;
	}

	// Starting at dom (first true custom gametype)
	for ( i = 0 ; i < sizeof( gtInfo ) / sizeof( gtInfo[0] ) ; i++ )
	{
		if(!Q_stricmp(g_gametype.string, gtInfo[i].name))
		{	
			trap_Cvar_Set("g_gametype", va("%s", gtInfo[i].based));
			trap_Cvar_Update (&g_gametype);
			trap_Cvar_Set("gt_customType", va("%i", i) );
			trap_Cvar_Update (&gt_customType);
			return;
		}
	}
}

void NV_giveItem (gentity_t *ent, const char *name)
{	
	gitem_t		*it;
	gentity_t	*it_ent;
	trace_t		trace;

	it = BG_FindItem (name);
	if (!it) 
		return;

	it_ent = G_Spawn();
	VectorCopy( ent->r.currentOrigin, it_ent->s.origin );
	it_ent->classname = it->classname;
	G_SpawnItem (it_ent, it);
	FinishSpawningItem(it_ent );
	memset( &trace, 0, sizeof( trace ) );
	Touch_Item (it_ent, ent, &trace);

	if (it_ent->inuse) 
	{
		G_FreeEntity( it_ent );
	}
}
qboolean NV_isInf (void)
{
	if(gt_customType.integer != NV_GT_INF)
		return qfalse;
	//if (strcmp(g_gametype.string, "inf"))
	//	return qfalse;
	return qtrue;
}
qboolean NV_isOctf (void)
{
	if(gt_customType.integer != NV_GT_OCTF)
		return qfalse;
	//if (strcmp(g_gametype.string, "ctf"))
	//	return qfalse;
	return qtrue;
}

qboolean NV_isTctf (void)
{
	if(gt_customType.integer != NV_GT_TCTF)
		return qfalse;
	//if (strcmp(g_gametype.string, "ctf"))
	//	return qfalse;
	return qtrue;
}
qboolean NV_isSab (void)
{
	if(gt_customType.integer != NV_GT_SAB)
		return qfalse;
	//if (strcmp(g_gametype.string, "inf"))
	//	return qfalse;
	return qtrue;
}
qboolean NV_isRaze (void)
{
	if(gt_customType.integer != NV_GT_RAZE)
		return qfalse;
	//if (strcmp(g_gametype.string, "inf"))
	//	return qfalse;
	return qtrue;
}
qboolean NV_isRush (void)
{
	if(gt_customType.integer != NV_GT_RUSH)
		return qfalse;
	//if (strcmp(g_gametype.string, "ctf"))
	//	return qfalse;
	return qtrue;
}
qboolean NV_isCTC (void)
{
	if(gt_customType.integer != NV_GT_CTC)
		return qfalse;
	//if (strcmp(g_gametype.string, "inf"))
	//	return qfalse;
	return qtrue;
}
qboolean NV_isCR (void)
{
	if(gt_customType.integer != NV_GT_CR)
		return qfalse;
	//if (strcmp(g_gametype.string, "inf"))
	//	return qfalse;
	return qtrue;
}
qboolean NV_isVIP (void)
{
	if(gt_customType.integer != NV_GT_VIP)
		return qfalse;
	//if (strcmp(g_gametype.string, "inf"))
	//	return qfalse;
	return qtrue;
}
qboolean NV_isTFT (void)
{
	if(gt_customType.integer != NV_GT_TFT)
		return qfalse;
	//if (strcmp(g_gametype.string, "elim"))
	//	return qfalse;
	return qtrue;
}
qboolean NV_isFT (void)
{
	if(gt_customType.integer != NV_GT_FT)
		return qfalse;
	//if (strcmp(g_gametype.string, "dm"))
	//	return qfalse;
	return qtrue;
}
qboolean NV_isDom (void)
{
	if(gt_customType.integer != NV_GT_DOM)
		return qfalse;
	//if (strcmp(g_gametype.string, "ctf"))
	//	return qfalse;
	return qtrue;
}
qboolean NV_isKoth (void)
{
	if(gt_customType.integer != NV_GT_KOTH)
		return qfalse;
	//if (strcmp(g_gametype.string, "ctf"))
	//	return qfalse;
	return qtrue;
}
qboolean NV_isDem (void)
{
	if(gt_customType.integer != NV_GT_DEM)
		return qfalse;
	//if (strcmp(g_gametype.string, "inf"))
	//	return qfalse;
	return qtrue;
}
qboolean NV_isCon (void)
{
	if(gt_customType.integer != NV_GT_CON)
		return qfalse;
	//if (strcmp(g_gametype.string, "ctf"))
	//	return qfalse;
	return qtrue;
}

qboolean NV_isFor (void)
{
	if(gt_customType.integer != NV_GT_FOR)
		return qfalse;
	//if (strcmp(g_gametype.string, "tdm"))
	//	return qfalse;
	return qtrue;
}

#ifdef _DCTF
qboolean NV_isDctf (void)
{
	if(gt_customType.integer != NV_GT_DCTF)
		return qfalse;
	//if (strcmp(g_gametype.string, "ctf"))
	//	return qfalse;
	return qtrue;
}
#endif

qboolean NV_isGG (void)
{
	if(gt_customType.integer != NV_GT_GG)
		return qfalse;
	//if (strcmp(g_gametype.string, "dm"))
	//	return qfalse;
	return qtrue;
}

#ifdef _OVERPOWER
qboolean NV_isOP (void)
{
	if(gt_customType.integer != NV_GT_OP)
		return qfalse;
	//if (strcmp(g_gametype.string, "ctf"))
	//	return qfalse;
	return qtrue;
}
#endif

#ifdef _ZOMBIES
qboolean NV_isZom (void)
{
	if(gt_customType.integer != NV_GT_ZOM)
		return qfalse;
	//if (strcmp(g_gametype.string, "ctf"))
	//	return qfalse;
	return qtrue;
}
#endif
//void NV_GTItemNoDraw(void)
//{	
//	gentity_t *find;	
//	//find = NULL;	
//	//while ( NULL != (find = G_Find ( find, FOFS(classname), "gametype_item" ) ) )
//	//{
//	//	find->s.eFlags |= EF_NODRAW;
//	//	trap_UnlinkEntity (find);
//	//}
//	find = NULL;
//	while ( NULL != (find = G_Find ( find, FOFS(classname), "red_flag" ) ) )
//	{
//		find->s.eFlags |= EF_NODRAW;
//		trap_UnlinkEntity (find);
//	}	
//
//	find = NULL;
//	while ( NULL != (find = G_Find ( find, FOFS(classname), "blue_flag" ) ) )
//	{
//		find->s.eFlags |= EF_NODRAW;
//		trap_UnlinkEntity (find);
//	}	
//}
gentity_t* G_RealSpawnGametypeItem ( gitem_t* item, vec3_t origin, vec3_t angles, qboolean dropped );
gentity_t *CreateWeaponPickup(vec3_t pos,weapon_t weapon);
#ifdef _OVERPOWER

void NV_ToggleTeamGTItemDraws ( int flag1, int flag2 )
{
	gentity_t *find;
	int		  i;
	gitem_t* item;
	vec3_t dir = {10,10,100};

	item = BG_FindGametypeItemByID ( flag2 );

	// Convience check
	if ( !item )
	{
		return;
	}

	for(i = 0; i < TEAM_NUM_TEAMS; i++)
	{
		level.extras.spawnExtras[i] = qfalse;
	}

	// Remove all spawned instances of the item on the map
	find = NULL;
	while ( NULL != (find = G_Find ( find, FOFS(classname), item->classname ) ) )
	{
		// Free this entity soon
		find->nextthink = level.time + 10;
		find->think = G_FreeEntity;
	}

	// Strip the item from all connected clients
	for ( i = 0; i < level.numConnectedClients; i ++ )
	{
		g_entities[level.sortedClients[i]].client->ps.stats[STAT_GAMETYPE_ITEMS] &= ~(1<<item->giTag);
	}
	switch(flag1 - 100)
	{
	case TEAM_RED:
		G_RealSpawnGametypeItem ( item, level.extras.redFlagOrigin, level.extras.redFlagAngles, qfalse );
		////CreateWeaponPickup(level.extras.redFlagOrigin, WP_RPG7_LAUNCHER);
		//G_ApplyKnockback ( CreateWeaponPickup(level.extras.redFlagOrigin, WP_RPG7_LAUNCHER), dir, 800 );
		level.extras.spawnExtras[TEAM_RED] = qtrue;
		break;
	case TEAM_BLUE:
		G_RealSpawnGametypeItem ( item, level.extras.blueFlagOrigin, level.extras.blueFlagAngles, qfalse );
		////CreateWeaponPickup(level.extras.blueFlagOrigin, WP_RPG7_LAUNCHER);	
		//G_ApplyKnockback ( CreateWeaponPickup(level.extras.blueFlagOrigin, WP_RPG7_LAUNCHER), dir, 800 );
		level.extras.spawnExtras[TEAM_BLUE] = qtrue;
		break;
	case TEAM_FREE:
		G_RealSpawnGametypeItem ( item, level.extras.neutralFlagOrigin, level.extras.neutralFlagAngles, qfalse );
		break;
	}
}
#endif

//void NV_ToggleTeamGTItemDraws ( int flag1, int flag2 )
//{
//	gentity_t *find;
//	gitem_t* item, *item2;
//	static qboolean remap = qfalse;
//
//	//G_SpawnGametypeItem("blue_flag", qfalse, level.extras.redFlagOrigin);
//	return;
//	item = BG_FindGametypeItemByID ( flag1 );
//	item2 = BG_FindGametypeItemByID ( flag2 );
//	if ( !item || !item2 )
//	{
//		return;
//	}
//	
//	if(!remap)
//	{
//		AddRemap("models/flags/flag_blue", "models/flags/flag_blue", .001);
//		AddRemap("models/flags/flag_red", "models/flags/flag_red", .001);
//		trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
//		remap = qtrue;
//	}
//
//	find = NULL;
//	while ( NULL != (find = G_Find ( find, FOFS(classname), item->classname ) ) )
//	{
//		find->s.eFlags |= EF_NODRAW;
//		trap_UnlinkEntity (find);
//
//	}	
//	find = NULL;
//	while ( NULL != (find = G_Find ( find, FOFS(classname), item2->classname ) ) )
//	{
//		find->s.eFlags &= ~EF_NODRAW;
//		trap_LinkEntity (find);
//	}	
//}

/*
void NV_ToggleTeamGTItemDraws ( int flag1, int flag2 )
{
	//gitem_t* item;
	//item = BG_FindGametypeItemByID ( flag2 );
	//if ( !item )
	//	return;
	
	if(flag2-MAX_QPATH == TEAM_RED)
		AddRemap("models/flags/flag_red", "models/flags/flag_blue", .001);
	else
		AddRemap("models/flags/flag_blue", "models/flags/flag_blue", .001);
	trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
	g_entities[level.sortedClients[flag1]].client->ps.stats[STAT_GAMETYPE_ITEMS] = 0;
	G_SpawnGametypeItem ( "blue_flag", qfalse, g_entities[flag1].client->ps.origin );
}
*/
// GodOrDevil - should move all this to the propper gt file one day.  Still a bug somewhere too :/
void NV_domTouchItem ( gentity_t *ent, gentity_t *other )
{
	if(ent->count < 0)
		ent->count = 0;

	if (ent->flags != other->client->sess.team)
	{
		if(ent->count < 500)
		{
			other->client->nvSess.capturing = ent->s.number;
			
			//For nv capture bar
			//pm->ps->stats[STAT_USETYPE] = 1; // 1 = 'capturing'; 2 = 'planting/defusing'; 
			other->client->ps.stats[STAT_USETYPE] = 1; // 1 = 'capturing'; 2 = 'planting/defusing'; 
			other->client->ps.stats[STAT_USETIME_MAX]	= MAX_HEALTH;
			other->client->ps.stats[STAT_USETIME]		= ent->count/5;
			//
			other->client->ps.stats[STAT_ARMOR] = (100 - (other->client->ps.stats[STAT_USETIME_MAX] -  other->client->ps.stats[STAT_USETIME]) / (other->client->ps.stats[STAT_USETIME_MAX] / 100));

			if (ent->count == 90 /*&& ent->flags*/)
			{
				char		location[64];
				Team_GetLocationMsg(ent, location, sizeof(location));
				switch(other->client->sess.team)
				{
					case TEAM_BLUE:
						TeamSound(TEAM_RED, "sound/ctf_flag.mp3", "");
						if(location) /// Flag Name from location
							trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^4BLUE ^7Team is ^3TAKING ^7control of the ^$%s", level.time + 5000, location));
						else if(ent->flags) ///Flag belongs to a team, but not named
							trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^4BLUE ^7Team is ^3TAKING ^7control of a ^$RED ^7flag",	level.time + 5000));
						else /// No team & no name
							trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^4BLUE ^7Team is ^3TAKING ^7control of a ^3flag!", level.time + 5000));
						break;
					case TEAM_RED:	
						TeamSound(TEAM_BLUE, "sound/ctf_flag.mp3", "");
						if(location) /// Flag Name from location
							trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^$RED ^7Team is ^3TAKING ^7control of the ^4%s!",	level.time + 5000, location));
						else if(ent->flags) ///Flag belongs to a team, but not named
							trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^$RED ^7Team is ^3TAKING ^7control of a ^4BLUE ^7flag!",	level.time + 5000));
						else /// No team & no name
							trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^$RED ^7Team is ^3TAKING ^7control of a ^3flag!",	level.time + 5000));					
						break;	
					default:
						break;
				}
				trap_GT_SendEvent ( GTEV_TRIGGER_TOUCHED, level.time, ent->health, other->s.number, other->client->sess.team, 0, 0 );
			}
			if ( ( other->r.svFlags & SVF_BOT ))
				ent->count+=3;
			else
				ent->count++;
		}
		else
		{	
			//extern domMaxFlags;
			ent->s.eFlags &= ~EF_NODRAW;

			if(other->client->sess.team == TEAM_RED)
			{
				level.extras.domControlledFlags[TEAM_RED-1]++;
				if(level.extras.domControlledFlags[TEAM_BLUE-1] && ent->flags == TEAM_BLUE)
					level.extras.domControlledFlags[TEAM_BLUE-1]--;
				ent->s.modelindex = MODELINDEX_GAMETYPE_ITEM_2;
			}
			else
			{
				ent->s.modelindex = MODELINDEX_GAMETYPE_ITEM;
				level.extras.domControlledFlags[TEAM_BLUE-1]++;
				if(level.extras.domControlledFlags[TEAM_RED-1] && ent->flags == TEAM_RED)
					level.extras.domControlledFlags[TEAM_RED-1]--;
			}

			// Check that something didnt fuck up and go out of our bounds..
			if(level.extras.domControlledFlags[TEAM_BLUE-1] > level.extras.domMaxFlags)
				level.extras.domControlledFlags[TEAM_BLUE-1] = level.extras.domMaxFlags;
			if(level.extras.domControlledFlags[TEAM_RED-1] > level.extras.domMaxFlags)
				level.extras.domControlledFlags[TEAM_RED-1] = level.extras.domMaxFlags;
					
			if(level.extras.domControlledFlags[TEAM_BLUE-1] < 0)
				level.extras.domControlledFlags[TEAM_BLUE-1] = 0;
			if(level.extras.domControlledFlags[TEAM_RED-1] < 0)
				level.extras.domControlledFlags[TEAM_RED-1] = 0;

			if(level.extras.domMaxFlags && level.extras.domControlledFlags[other->client->sess.team-1] == level.extras.domMaxFlags)
			{
				char		location[64] = "\0";
				Team_GetLocationMsg(ent, location, sizeof(location));	
				//trap_GT_SendEvent ( GTEV_TEAM_ELIMINATED, level.time, ent->health, other->s.number, other->client->sess.team, 1, 0 );
				switch(other->client->sess.team)
				{
				case TEAM_BLUE:	
					trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^4BLUE ^7Team is ^3DOMINATING ^7the ^$RED ^7Team!",	level.time + 5000));
					Com_sprintf(location, sizeof(location),"^7We are being ^$DOMINATED ^7by the ^1enemy!");
					TeamSound(TEAM_RED, "sound/enemy/english/male/slaughtered.mp3", location);
					break;
				case TEAM_RED:	
					trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^$RED ^7Team is ^3DOMINATING ^7the ^4BLUE ^7Team!",	level.time + 5000));
					Com_sprintf(location, sizeof(location),"^7We are being ^4DOMINATED ^7by the ^4ENEMY!");
					TeamSound(TEAM_BLUE, "sound/enemy/english/male/slaughtered.mp3", location);
					break;
				}
			}
			else
			{
				char		location[128];
				Team_GetLocationMsg(ent, location, sizeof(location));
				//trap_GT_SendEvent ( GTEV_TEAM_ELIMINATED, level.time, ent->health, other->s.number, other->client->sess.team, 0, 0 );
						
				switch(other->client->sess.team)
				{
				case TEAM_BLUE:
					if(location)
					{
						trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^4BLUE ^7Team has the ^4%s^7!  ^2-  ^4Blue ^7controls ^4%i ^7of ^3%i",	level.time + 5000,	location, level.extras.domControlledFlags[TEAM_BLUE-1], level.extras.domMaxFlags));
						Com_sprintf(location, sizeof(location),"^7The ^1ENEMY ^7has taken ^3%s", location );
						TeamSound(TEAM_RED, "sound/ctf_base.mp3", location);
						TeamSound(TEAM_BLUE, "sound/ctf_win.mp3", "");
						//
						break;
					}
					else
					{
						trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^4BLUE ^7Team has ^4a ^4control point!  ^2-  ^4Blue ^7controls ^4%i ^7of ^3%i",	level.time + 5000, level.extras.domControlledFlags[TEAM_BLUE-1], level.extras.domMaxFlags));
						Com_sprintf(location, sizeof(location),"^7The ^4ENEMY ^7has taken a ^3control point");
						TeamSound(TEAM_RED, "sound/ctf_base.mp3", location);
						TeamSound(TEAM_BLUE, "sound/ctf_win.mp3", "");
						//
						break;
					}
					break;
				case TEAM_RED:	
					if(location)
					{
						trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^$RED ^7Team has the ^$%s^7!  ^2-  ^$RED ^7controls ^$%i ^7of ^3%i",	level.time + 5000,	location, level.extras.domControlledFlags[TEAM_RED-1], level.extras.domMaxFlags));
						Com_sprintf(location, sizeof(location),"^7The ^1ENEMY ^7has taken ^3%s", location );
						TeamSound(TEAM_BLUE, "sound/ctf_base.mp3", location);
						TeamSound(TEAM_RED, "sound/ctf_win.mp3", "");
						break;
					}
					else
					{
						trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^$RED ^7Team has a ^1control point!  ^2-  ^$RED ^7controls ^$%i ^7of ^3%i",	level.time + 5000, level.extras.domControlledFlags[TEAM_RED-1], level.extras.domMaxFlags ));
						Com_sprintf(location, sizeof(location),"^7The ^1ENEMY ^7has taken a ^3control point");
						TeamSound(TEAM_BLUE, "sound/ctf_base.mp3", location);
						TeamSound(TEAM_RED, "sound/ctf_win.mp3", "");
						//
						break;
					}
					break;
				}
			}
			//NV_DomInfo(-1);

			ent->flags = other->client->sess.team; /// $todo: trap_GT_SendEvent(GTEV_TRIGGER_CAPTURE ...)
			ent->count = 0;
			other->client->nvSess.capturing = 0;
			NV_updateSpawnTeam(ent);
#ifdef _DB_ENABLED
			other->client->pers.statinfo.itemCaps++;
#endif
		}
	}
	else if(ent->flags == other->client->sess.team && ent->count > 1) 
	{						
		if ( ( other->r.svFlags & SVF_BOT ))
			ent->count-=3;
		else
			ent->count-=1;

		//For nv capture bar
		pm->ps->stats[STAT_USETYPE] = 1; // 1 = 'capturing'; 2 = 'planting/defusing'; 
		other->client->ps.stats[STAT_USETIME_MAX]	= MAX_HEALTH;
		other->client->ps.stats[STAT_USETIME]		= ent->count/5;
		//

		other->client->nvSess.capturing = ent->s.number;
	}
}
void NV_kothTouchItem ( gentity_t *ent, gentity_t *other )
{
	if(ent->count < 0)
		ent->count = 0;

	if (ent->flags != other->client->sess.team)
	{
		if(ent->count < 500)
		{
			other->client->nvSess.capturing = ent->s.number;
			
			//For nv capture bar
		//	pm->ps->stats[STAT_USETYPE] = 1; // 1 = 'capturing'; 2 = 'planting/defusing'; 
			other->client->ps.stats[STAT_USETYPE] = 1; // 1 = 'capturing'; 2 = 'planting/defusing'; 
			other->client->ps.stats[STAT_USETIME_MAX]	= MAX_HEALTH;
			other->client->ps.stats[STAT_USETIME]		= ent->count/5;
			//					
			other->client->ps.stats[STAT_ARMOR] = (100 - (other->client->ps.stats[STAT_USETIME_MAX] -  other->client->ps.stats[STAT_USETIME]) / (other->client->ps.stats[STAT_USETIME_MAX] / 100));

			if (ent->count == 90 )
			{
				char		location[64];
				Team_GetLocationMsg(ent, location, sizeof(location));
				switch(other->client->sess.team)
				{
					case TEAM_BLUE:
							trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^4BLUE ^7Team is ^3TAKING ^7control of the ^3Zone!", level.time + 5000));
						break;
					case TEAM_RED:	
							trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^$RED ^7Team is ^3TAKING ^7control of the ^3Zone!",	level.time + 5000));					
						break;	
					default:
						break;
				}
				trap_GT_SendEvent ( GTEV_TRIGGER_TOUCHED, level.time, ent->health, other->s.number, other->client->sess.team, 0, 0 );
			}
			if ( ( other->r.svFlags & SVF_BOT ))
				ent->count+=3;
			else
				ent->count++;
		}
		else
		{		
			char		location[128];
			Team_GetLocationMsg(ent, location, sizeof(location));

			//extern domMaxFlags;
			ent->s.eFlags &= ~EF_NODRAW;

			if(other->client->sess.team == TEAM_RED){
				level.extras.domControlledFlags[TEAM_RED-1]++;
				if(level.extras.domControlledFlags[TEAM_BLUE-1] && ent->flags == TEAM_BLUE)
					level.extras.domControlledFlags[TEAM_BLUE-1]--;
				ent->s.modelindex = MODELINDEX_GAMETYPE_ITEM_2;
			}
			else{
				ent->s.modelindex = MODELINDEX_GAMETYPE_ITEM;
				level.extras.domControlledFlags[TEAM_BLUE-1]++;
				if(level.extras.domControlledFlags[TEAM_RED-1] && ent->flags == TEAM_RED)
					level.extras.domControlledFlags[TEAM_RED-1]--;
			}
				
			// Check that something didnt fuck up and go out of our bounds..
			if(level.extras.domControlledFlags[TEAM_BLUE-1] > level.extras.domMaxFlags)
				level.extras.domControlledFlags[TEAM_BLUE-1] = level.extras.domMaxFlags;
			if(level.extras.domControlledFlags[TEAM_RED-1] > level.extras.domMaxFlags)
				level.extras.domControlledFlags[TEAM_RED-1] = level.extras.domMaxFlags;
					
			if(level.extras.domControlledFlags[TEAM_BLUE-1] < 0)
				level.extras.domControlledFlags[TEAM_BLUE-1] = 0;
			if(level.extras.domControlledFlags[TEAM_RED-1] < 0)
				level.extras.domControlledFlags[TEAM_RED-1] = 0;

			trap_GT_SendEvent ( GTEV_TEAM_ELIMINATED, level.time, ent->health, other->s.number, other->client->sess.team, 0, 0 );
			switch(other->client->sess.team)
			{
			case TEAM_BLUE:
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^4BLUE ^7Team has ^4a ^4control point!", level.time + 5000));
				Com_sprintf(location, sizeof(location),"^2[^3%s^2] ^1HURRY! ^7- The ^1enemy ^7took the ^3control point", nv_gtShort.string);
				TeamSound(TEAM_RED, "sound/enemy/english/male/hurry.mp3", location);
				break;
			case TEAM_RED:	
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, The ^$RED ^7Team has the ^1control point!",	level.time + 5000));
				Com_sprintf(location, sizeof(location),"^2[^3%s^2] ^1HURRY! ^7- The ^4enemy ^7took the ^3control point", nv_gtShort.string);
				TeamSound(TEAM_BLUE, "sound/enemy/english/male/hurry.mp3", location);
				break;
			}
			
			//NV_DomInfo(-1);

			ent->flags = other->client->sess.team; /// $todo: trap_GT_SendEvent(GTEV_TRIGGER_CAPTURE ...)
			ent->count = 0;
			other->client->nvSess.capturing = 0;
			//NV_updateKothSpawnTeam(ent);
			NV_updateKothSpawnTeam(other->client->sess.team);
#ifdef _DB_ENABLED
			other->client->pers.statinfo.itemCaps++;
#endif
		}
	}
	else if(ent->flags == other->client->sess.team && ent->count > 1) 
	{						
		if ( ( other->r.svFlags & SVF_BOT ))
			ent->count-=3;
		else
			ent->count-=1;

		//For nv capture bar
		pm->ps->stats[STAT_USETYPE] = 1; // 1 = 'capturing'; 2 = 'planting/defusing'; 
		other->client->ps.stats[STAT_USETIME_MAX]	= MAX_HEALTH;
		other->client->ps.stats[STAT_USETIME]		= ent->count/5;
		//

		other->client->nvSess.capturing = ent->s.number;
	}
}
qboolean NV_CheckTouchItem ( gentity_t *ent, gentity_t *other )
{
	if ( level.gametypeDelayTime && level.gametypeDelayTime >= level.time )
		return qfalse;

	switch(gt_customType.integer)
	{
	case NV_GT_DOM:
		NV_domTouchItem	(ent, other);
		return qtrue;		
	case NV_GT_KOTH:
		NV_kothTouchItem (ent, other);
		return qtrue;
	case NV_GT_CON:
		return qfalse;
	default:
		return qfalse;
	}
}

void BodySink( gentity_t *ent ) ;

void NV_spawnFrozen (gentity_t *ent)
{
	char		*origin;
	char		*angles;
	//char		cmd[MAX_QPATH] = "\0";
	int			entNum;
	int			offset = 0;

	angles = va("%.0f %.0f %.0f", 0.0, ent->client->ps.viewangles[1], 0.0);
	origin = va("%.0f %.0f %.0f",
		ent->client->ps.origin[0], 	ent->client->ps.origin[1], 	ent->client->ps.origin[2]);

	//if(strlen(cmd) < 4 ) // beams for CTF/INF markers
	if(ent->client->sess.team == TEAM_RED)
	{
		AddSpawnField("model", "models/objects/Armory/dummy.md3");
		offset = 20;
	}
	else if(ent->client->sess.team == TEAM_BLUE)
	{
		AddSpawnField("model", "models/objects/Kamchatka/misc/chem_suit.md3");
		offset = 60;
	}
	else
	{
		AddSpawnField("model", "models/objects/Armory/dummy.md3");
		offset = 20;
	}
	AddSpawnField("classname", "nv_model");
	AddSpawnField("origin", origin);
	AddSpawnField("angles", angles);
	entNum = G_SpawnGEntityFromSpawnVars(qfalse);
	

	trap_UnlinkEntity (ent);
	ent->client->nvSess.frozenTime = level.time + (g_freezeTime.value - 1)* 1000;

	if(entNum != -1)
	{
		int			contents;
		gentity_t	*frozen = &g_entities[entNum];
		
		trap_UnlinkEntity (frozen);
		contents = trap_PointContents( ent->r.currentOrigin, -1 );
		if ( contents & CONTENTS_NODROP ) 
		{
			ent->client->nvSess.frozen = 0;
			return;
		}

		VectorCopy (ent->r.mins, frozen->r.mins);
		VectorCopy (ent->r.maxs, frozen->r.maxs);
		VectorCopy (ent->r.absmin, frozen->r.absmin);
		VectorCopy (ent->r.absmax, frozen->r.absmax);
		frozen->r.absmin[2] += 15;
		frozen->r.absmax[2] -= 15;
		frozen->r.maxs[2] += 15;
		frozen->r.mins[2] -= 15;
		frozen->r.ownerNum = ent->s.number;

		frozen->clipmask	= CONTENTS_SHOTCLIP;
		frozen->r.contents	= CONTENTS_CORPSE;

		ent->client->nvSess.frozen = frozen->s.number; // store this bodys ent number on the client
	
		frozen->s.eType				= ET_BODY;
		frozen->die					= body_die;	
		frozen->think				= G_FreeEntity;
		frozen->nextthink			= ent->client->nvSess.frozenTime;
		frozen->takedamage			= qtrue;
		frozen->timestamp			= level.time;
		frozen->physicsObject		= qtrue;
		frozen->physicsBounce		= 0;
		//frozen->s.otherEntityNum	= ent->s.clientNum;

		frozen->s.pos.trType = TR_STATIONARY;
		frozen->s.event = 0;

		VectorCopy ( frozen->s.pos.trBase, frozen->r.currentOrigin );
		VectorCopy ( frozen->s.pos.trBase, ent->client->nvSess.frozenOrigin);
		frozen->s.pos.trBase[2] = ent->client->ps.origin[2] + offset;
		trap_LinkEntity (frozen);
	}

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}
void NV_ClientRespawn (gentity_t *ent)
{
	int					index;
	gclient_t			*client;
	int					i;
	clientPersistant_t	saved;
	clientSession_t		savedSess;
	nvSession_t			savedNVSess;
	int					persistant[MAX_PERSISTANT];
	int					flags;
	int					savedPing;
	int					eventSequence;
	char				userinfo[MAX_INFO_STRING];
	int					idle;
	gentity_t			*tent;
	int					ammo[MAX_AMMO];
	int					clip[ATTACK_MAX][MAX_WEAPONS];
	int					weapons;
	vec3_t	playerMins = {-15, -15, -46};
	vec3_t	playerMaxs = {15, 15, 48};

	index  = ent - g_entities;
	client = ent->client;

	trap_UnlinkEntity (ent);
	G_StopGhosting(ent);
#ifdef _MOUNTED
	if(ent->client->ps.stats[STAT_SPECIAL] == SPECIAL_MOUNTED_M60)
	{	
		NV_Mounted_Free_Client(&g_entities[ent->mountedGun]);
	}
#endif
	client->pers.teamState.state = TEAM_ACTIVE;

	// toggle the teleport bit so the client knows to not lerp
	// and never clear the voted flag
	flags = ent->client->ps.eFlags & (EF_TELEPORT_BIT | EF_VOTED);
	flags ^= EF_TELEPORT_BIT;

	// clear everything but the persistant data
	saved = client->pers;
	savedSess = client->sess;
	savedNVSess = client->nvSess;
	savedPing = client->ps.ping;

	//WEAPONS
	weapons = client->ps.stats[STAT_WEAPONS];
	memcpy(ammo, &client->ps.ammo, sizeof(ammo));
	memcpy(clip, &client->ps.clip, sizeof(clip));
	//END

	for ( i = 0 ; i < MAX_PERSISTANT ; i++ ) 
	{
		persistant[i] = client->ps.persistant[i];
	}
	eventSequence = client->ps.eventSequence;

	memset (client, 0, sizeof(*client)); 

	client->pers = saved;
	client->sess = savedSess;
	client->nvSess = savedNVSess;

	client->ps.ping = savedPing;
	client->lastkilled_client = -1;

	for ( i = 0 ; i < MAX_PERSISTANT ; i++ ) 
	{
		client->ps.persistant[i] = persistant[i];
	}
	client->ps.eventSequence = eventSequence;

	// increment the spawncount so the client will detect the respawn
	client->ps.persistant[PERS_SPAWN_COUNT]++;
	client->ps.persistant[PERS_TEAM] = client->sess.team;
	client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
	
	client->airOutTime = level.time + 12000;

	trap_GetUserinfo( index, userinfo, sizeof(userinfo) );

	// clear entity values
	client->ps.eFlags = flags;

	ent->s.groundEntityNum = ENTITYNUM_NONE;
	ent->client = &level.clients[index];
	ent->takedamage = qtrue;
	ent->inuse = qtrue;
	ent->classname = "player";
	ent->r.contents = CONTENTS_BODY;
	ent->clipmask = MASK_PLAYERSOLID;
	ent->die = player_die;

	ent->waterlevel = 0;
	ent->watertype = 0;
	ent->flags = 0;
	
	VectorCopy (playerMins, ent->r.mins);
	VectorCopy (playerMaxs, ent->r.maxs);

	client->ps.clientNum = index;
	ent->s.number = index;

	// Bring back the saved firemodes & weapon info
	memcpy ( client->ps.firemode, client->pers.firemode, sizeof(client->ps.firemode) );
	memcpy( client->ps.ammo, client->pers.ammo, sizeof(client->ps.ammo));
	memcpy( client->ps.clip, client->pers.clip, sizeof(client->ps.clip));
	client->ps.stats[STAT_WEAPONS] = client->pers.weapons;
	client->ps.weapon = client->pers.currentWeapon;

	//client->ps.stats[STAT_HEALTH] = ent->health = MAX_HEALTH;
	client->ps.stats[STAT_HEALTH] = ent->health = MAX_HEALTH / 2;
	client->ps.stats[STAT_REALARMOR] = MAX_HEALTH;

	//G_SetOrigin( ent, g_entities[ ent->client->nvSess.frozen ].s.pos.trBase );
	//VectorCopy( g_entities[ ent->client->nvSess.frozen ].s.pos.trBase, client->ps.origin );
	if(!ent->client->nvSess.frozenOrigin || !ent->client->nvSess.frozen )
		return;
	G_SetOrigin(ent, ent->client->nvSess.frozenOrigin);
	VectorCopy( ent->client->nvSess.frozenOrigin, client->ps.origin );
	
	// the respawned flag will be cleared after the attack and jump keys come up
	client->ps.pm_flags |= PMF_RESPAWNED;
	if ( client->pers.autoReload || (  ent->r.svFlags & SVF_BOT))
	{
		client->ps.pm_flags |= PMF_AUTORELOAD;
	}
	else
	{
		client->ps.pm_flags &= ~PMF_AUTORELOAD;
	}

	trap_GetUsercmd( client - level.clients, &ent->client->pers.cmd );

	//VectorCopy ( spawnPoint->angles, spawn_angles );
	//VectorCopy ( spawnPoint->origin, spawn_origin );	
	//VectorCopy ( ent->client->nvSess., spawn_angles);
	//spawn_origin[2] += 9;	
	//SetClientViewAngle( ent, spawn_angles );
	VectorCopy( g_entities[ ent->client->nvSess.frozen ].s.angles, ent->s.angles );
	VectorCopy (g_entities[ ent->client->nvSess.frozen ].s.angles, ent->client->ps.viewangles);


	//G_KillBox( ent ); // dont do a kill box here.. telefrags suck for rescues
 	trap_LinkEntity (ent);

	client->ps.firemode[client->ps.weapon] = BG_FindFireMode ( (weapon_t)client->ps.weapon, ATTACK_NORMAL, WP_FIREMODE_AUTO );
	BG_GetInviewAnim(client->ps.weapon,"idle",&idle);					
	client->ps.weaponAnimId = idle;
	client->ps.weaponAnimIdChoice = 0;
	client->ps.weaponCallbackStep = 0;

	// don't allow full run speed for a bit
	client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
	client->ps.pm_time = 100;

	client->respawnTime = level.time;
	client->invulnerableTime = level.time;
	client->ps.eFlags |= EF_INVULNERABLE;
	client->inactivityTime = level.time + g_inactivity.integer * 1000;
	client->latched_buttons = 0;
	client->sess.camperSeconds = 0;

	// set default animations
	client->ps.weaponstate = WEAPON_READY;
	client->ps.torsoAnim = -1;
	client->ps.legsAnim = LEGS_IDLE;

	client->ps.weaponAnimIdChoice = 0;
	client->ps.weaponCallbackStep = 0;
	client->ps.weaponCallbackTime = 0;

	// Not on a ladder
	client->ps.ladder = -1;

	// Not leaning
	client->ps.leanTime = LEAN_TIME;

	if ( level.intermissiontime ) 
	{
		MoveClientToIntermission( ent );
	} 

	// Frozen?
	if ( level.gametypeDelayTime > level.time )
	{
		ent->client->ps.stats[STAT_FROZEN] = level.gametypeDelayTime - level.time;
	}

	// run a client frame to drop exactly to the floor,
	// initialize animations and other things
	client->ps.commandTime = level.time - 100;
	ent->client->pers.cmd.serverTime = level.time;
	ClientThink( ent-g_entities );

	// positively link the client, even if the command times are weird
	if ( ent->client->sess.team != TEAM_SPECTATOR ) 
	{
		BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );
		VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
	}

	// run the presend to set anything else
	ClientEndFrame( ent );

	// clear entity state values
	BG_PlayerStateToEntityState( &client->ps, &ent->s, qtrue );

	// Add a teleportation effect.
	tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_IN );
	tent->s.clientNum = ent->s.clientNum;
	trap_LinkEntity( ent );
	
	// Mark as PMF_SIAMESETWINS incase the rescuer is inside the frozen corpse.
	ent->client->ps.pm_flags |= PMF_SIAMESETWINS;

}

void NV_freeFrozenModel ( gentity_t *ent )
{
	if( ent->client->nvSess.frozen > 0 && &g_entities[ ent->client->nvSess.frozen ] != NULL  && strcmp(g_entities[ ent->client->nvSess.frozen ].classname, "freed"))
	{
		G_FreeEntity( &g_entities[ ent->client->nvSess.frozen ] );
	}
}

void NV_respawn( gentity_t *ent, qboolean rescued )
{
	// No respawning when intermission is queued
	if ( level.intermissionQueued )
		return;
	
	trap_UnlinkEntity (ent);

	NV_freeFrozenModel(ent);

	if(ent->client->pers.teamState.state == TEAM_BEGIN)
	{		
		G_StopGhosting( ent );
		ClientSpawn(ent);
	}	
	else if (ent->client->nvSess.frozen)
	{
		if(!rescued)
			CP(va("print \"^3%s ^7has ^zThawed\n\"", ent->client->pers.netname ));
		NV_ClientRespawn(ent);
	}

	ent->client->sess.camperSeconds = 0;
	ent->client->nvSess.frozen = 0;
	ent->client->nvSess.frozenTime = level.time;
	ent->client->invulnerableTime = level.time + 1500;
		
	// #SPAWNKILL
	if(g_maxSpawnKill.integer && level.gametypeData->teams)
		ent->client->spawn = qtrue;
	else
		ent->client->spawn = qfalse;

//	NV_teamSkin (ent, NULL, ent->client->sess.team);

}
void NV_unFreeze( gentity_t *targ, gentity_t *attacker, int mod )
{
	if( !targ->client->nvSess.frozen )
		return;

	if(((mod & 0xFF) != MOD_KNIFE) && ((mod & 0xFF) != MOD_AK74_ASSAULT_RIFLE))
	{			
		return;
	}
	
	if(attacker && attacker->client)
	{
		attacker->client->ps.stats[STAT_HEALTH] = attacker->health += 20;
		if(attacker->client->ps.stats[STAT_HEALTH]>100)
			attacker->client->ps.stats[STAT_HEALTH] = attacker->health = MAX_HEALTH;
		
		attacker->client->ps.stats[STAT_REALARMOR] += 20;
		if(attacker->client->ps.stats[STAT_REALARMOR] > MAX_HEALTH)
			attacker->client->ps.stats[STAT_REALARMOR] = MAX_HEALTH;

		if(NV_isTFT())
		{
			CP(va("print \"^3%s ^7was ^Grescued ^7by ^3%s\n\"", targ->client->pers.netname, attacker->client->pers.netname ));
#ifdef _DB_ENABLED
			attacker->client->pers.statinfo.itemDefs++;
#endif
			//trap_GT_SendEvent ( GTEV_ITEM_DEFEND, level.time, attacker->health, attacker->s.clientNum, 0, 0, 0 );
			G_AddScore( attacker, 1 );
		}
		else
		{

		}
	//	NV_Sound(targ-g_entities, "sound/player/bullet_impacts/ice/ice1.wav" );
	//	NV_Sound(attacker-g_entities, "sound/player/bullet_impacts/ice/ice0.wav" );

		NV_respawn (targ, qtrue);
	}
}
void LogExit( const char *string );
/*
qboolean NV_TeamDeadNoLives (team_t team)
{
	int i;
	int alive;
	int dead;

	if(level.intermissionQueued || level.intermissiontime)
		return qfalse;
	if(!level.firstBlood)
		return qfalse;
	//if(!NV_isRush())
	//	return qfalse;
	switch(team)
	{
		case TEAM_BLUE:
			if(nv_reinforcementsBlue.integer < 1)
				return qfalse;
			
			level.extras.blueReinforcements++;
			if(nv_reinforcementsBlue.integer == level.extras.blueReinforcements )
			{		
				GT(va("%i,%s ^7is low on ^1Reinforcements!", level.time + 5000, "^4BLUE" ));
				return qfalse;
			}
			if(level.extras.blueReinforcements < nv_reinforcementsBlue.integer)
			{	
				return qfalse;
			}
			break;

		case TEAM_RED:
			if(nv_reinforcementsRed.integer < 1)
				return qfalse;

			level.extras.redReinforcements++;
			if(nv_reinforcementsRed.integer == level.extras.redReinforcements)
			{	
				GT(va("%i,%s ^7is low on ^1Reinforcements!", level.time + 5000, "^1RED" ));
				return qfalse;
			}
			if(level.extras.redReinforcements < nv_reinforcementsRed.integer)
			{	
				return qfalse;
			}
			break;

		default:
			break;
	}

	alive = 0;
	dead = 0;
	for ( i = 0; i < level.numConnectedClients; i ++ )
	{
		gentity_t* ent = &g_entities[level.sortedClients[i]];

		if ( ent->client->pers.connected != CON_CONNECTED )
			continue;			
		if ( ent->client->sess.team != team )
			continue;
#ifdef _TORTURE
		if ( G_IsClientDead ( ent->client ) || ent->client->nvSess.torture)
#else	
		if ( G_IsClientDead ( ent->client ))
#endif
			dead++;
		else
			alive++;
	}

	if ( !alive && dead )
	{
		gentity_t* tent;
		tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
		tent->s.eventParm = GAME_OVER_SCORELIMIT;
		tent->r.svFlags = SVF_BROADCAST;	
		tent->s.otherEntityNum = team;

		if(team == TEAM_RED)
		{		
			tent->s.otherEntityNum = TEAM_BLUE;
			trap_GT_SendEvent ( GTEV_TEAM_ELIMINATED, level.time, team, 0, 0, 0, 0 );
			LogExit( "Blue team hit the score limit." );
#ifdef _DB_ENABLED
			mapStats.winner = TEAM_BLUE;
#endif // _DB
		}
		else if(team == TEAM_BLUE)
		{		
			tent->s.otherEntityNum = TEAM_RED;
			trap_GT_SendEvent ( GTEV_TEAM_ELIMINATED, level.time, team, 0, 0, 0, 0 );
			LogExit( "Red team hit the score limit." );
#ifdef _DB_ENABLED
			mapStats.winner = TEAM_BLUE;
#endif // _DB
		}
		return qtrue;		
	}
	else
	{	
		if(team == TEAM_RED)
		{
			GT(va("%i,%s ^7has ^3%i Reinforcements ^7remaining!", level.time + 5000, "^1RED", alive ));
			level.gametypeRespawnTime[team] = 0;
		}
		if(team == TEAM_BLUE)
		{
			GT(va("%i,%s ^7has ^3%i Reinforcements ^7remaining!", level.time + 5000, "^4BLUE", alive ));
			level.gametypeRespawnTime[team] = 0;
		}
	}
	return qtrue;
}
*/
void NV_checkGametypePhase (void)
{
	level.extras.phaseTargets[level.extras.currentPhase]--;
	if(level.extras.phaseTargets[level.extras.currentPhase] <= 0)
	{
		level.extras.currentPhase++;
		if(level.extras.currentPhase >= MAX_PHASES || level.extras.phaseTargets[level.extras.currentPhase] <= 0 )
		{				
			gentity_t* tent;
			tent = G_TempEntity( vec3_origin, EV_GAME_OVER );
			tent->s.eventParm = GAME_OVER_SCORELIMIT;
			tent->r.svFlags = SVF_BROADCAST;	
			tent->s.otherEntityNum = TEAM_BLUE;
			GT(va("%i, ^+Objective Complete! ^3%s ^+WINS!", level.time + 5000, "^4BLUE" ));
			LogExit( "Blue team destoryed all targets." );
			return;
		}
		else
		{
			GT(va("%i, ^3PHASE %i ^+has begun!", level.time + 5000, level.extras.currentPhase ));
			trap_GT_SendEvent ( GTEV_TEAM_ELIMINATED, level.time, TEAM_RED, 0, 0, 0, 0 );
			NV_SetPhaseSpawns();
		}
	}
	else
	{
		GT(va("%i, ^+A ^3Target ^+has been ^1Destroyed!", level.time + 5000 ));
	}
}
void bomb_target_activated_use ( gentity_t* self );
void NV_Spawn_Target_Die(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod, int blank, vec3_t a)
{
	vec3_t dif; 
	gentity_t *te;

	// Barrel = self
	if (self->chain) // chain = target
	{	
		if(self->chain->chain) // target->chain = tnt 
		{
			G_FreeEntity(self->chain->chain);
		}		
		self->chain->armTime = gt_bombTimer.integer + 100;
		//self->chain->health = 200;
		//self->chain->use = bomb_target_red_use;
		self->chain->think = bomb_target_activated_use;
		self->chain->nextthink = level.time + 10;  // Let them all spawn first
	}


	// TODO: Spawn 'damage' area models
	G_RadiusDamage( self->r.currentOrigin, attacker, 20, 1000000, self, 3, MOD_TRIGGER_HURT );

	dif[0] = (self->r.absmax[0]+self->r.absmin[0])/2;
	dif[1] = (self->r.absmax[1]+self->r.absmin[1])/2;
	dif[2] = (self->r.absmax[2]+self->r.absmin[2])/2;

	te = G_TempEntity( dif, EV_GLASS_SHATTER );
	te->s.eventParm = self->s.number;
	VectorCopy(self->r.maxs, te->s.origin);
	VectorCopy(self->r.mins, te->s.angles);

	//G_UseTargetsByName ( self->target, NULL, NULL );	
	trap_UnlinkEntity(self);

	//self->s.weapon=WP_RPG7_LAUNCHER; 
	//G_AddEvent( self, EV_MISSILE_MISS, DirToByte( dir ) ); 
	self->freeAfterEvent = qtrue; 
}


void NV_Spawn_Target_Chunks ( gentity_t *ent ) 
{
	gentity_t	*tnt;
	vec3_t origin;

	/////////////////
	// BAR MODEL
	////////////////
	tnt=G_Spawn();

	tnt->s.eType=ET_GENERAL;
	
	tnt->s.modelindex = G_ModelIndex("models/chunks/monitor_cart_chunks/monitor_cart_chunk1.md3");
	tnt->model = "models/chunks/monitor_cart_chunks/monitor_cart_chunk1.md3";
	tnt->s.modelindex2 = G_ModelIndex("models/chunks/monitor_cart_chunks/monitor_cart_chunk1.md3");

	VectorCopy(ent->r.currentOrigin, origin);
	origin[2] -= 20;
	G_SetOrigin(tnt,origin);

	VectorSet(tnt->s.apos.trBase,0, ent->s.apos.trBase[1] ,0);
	G_SetAngles(tnt, ent->s.angles );

	trap_LinkEntity (tnt);
}

void NV_Spawn_Target_Models ( gentity_t *ent ) 
{
	gentity_t	*tnt, *barrel;
	vec3_t origin;

	/////////////////
	// BAR MODEL
	////////////////
	tnt=G_Spawn();
	ent->chain=tnt;

	tnt->s.eType=ET_GENERAL;
	tnt->classname="target_base";	
	//tnt->s.modelindex = G_ModelIndex("models/pick_ups/rmg_tnt.md3");
	//tnt->model = "models/pick_ups/rmg_tnt.md3";
	//tnt->s.modelindex2 = G_ModelIndex("models/pick_ups/rmg_tnt.md3");
	tnt->s.modelindex = G_ModelIndex("models/objects/Common/beam.md3");
	tnt->model = "models/objects/Common/beam.md3";
	tnt->s.modelindex2 = G_ModelIndex("models/objects/Common/beam.md3");

	tnt->r.contents = CONTENTS_SOLID;
	tnt->clipmask = CONTENTS_SOLID;

	VectorSet( tnt->r.mins, -5, -5, -20 );
	VectorSet( tnt->r.maxs, 5, 5, 20);

	VectorCopy(ent->r.currentOrigin, origin);
	//origin[2] += 20;
	G_SetOrigin(tnt,origin);

	VectorSet(tnt->s.apos.trBase,0, ent->s.apos.trBase[1] ,0);
	G_SetAngles(tnt, ent->s.angles );

	trap_LinkEntity (tnt);

	/////////////////
	// BARREL MODEL
	////////////////
	barrel=G_Spawn();

	// Link ent to bags & barrel to ent
	tnt->chain=barrel;
	barrel->chain=ent;

	barrel->r.contents = CONTENTS_SOLID;
	barrel->clipmask = MASK_SHOT;

	barrel->s.eType=ET_GENERAL;
	barrel->classname="target_base";	
	//barrel->s.modelindex = G_ModelIndex("models/objects/common/barrels/barrel_hk.md3");
	//barrel->model = "models/objects/common/barrels/barrel_hk.md3";
	//barrel->s.modelindex2 = G_ModelIndex("models/objects/common/barrels/barrel_hk.md3");
	
	barrel->s.modelindex = G_ModelIndex("models/objects/Hospital/carts/monitor_cart.md3");
	barrel->model = "models/objects/Hospital/carts/monitor_cart.md3";
	barrel->s.modelindex2 = G_ModelIndex("models/objects/Hospital/carts/monitor_cart.md3");
	
	//barrel->health = ent->health;
	barrel->health = gt_targetHealth.integer * 1000;

	if(barrel->health > 0)
	{
		barrel->takedamage = qtrue;
		barrel->die = NV_Spawn_Target_Die;
	}

	VectorSet( barrel->r.mins, -16, -16, -40 );
	VectorSet( barrel->r.maxs, 16, 16, 40);

	VectorCopy(ent->r.currentOrigin, origin);
	origin[2] -= 50;
	G_SetOrigin(barrel,origin);	

	//ent->s.apos.trBase[1] += 90;
	VectorSet(barrel->s.apos.trBase, 0, ent->s.apos.trBase[1] ,0);

	// origin = angles now
	VectorCopy(ent->s.angles, origin);
	origin[1] += 90; 
	G_SetAngles(barrel, origin );
	
	trap_LinkEntity (barrel);
}

//void NV_GiveBombToPlayer ( int player )
//{
//	gitem_t* item;		
//	level.extras.bombGivenClient[g_entities[player].client->sess.team] = -1;
//		
//	item = &bg_itemlist[35];
//	if ( item )	
//	{
//		//g_entities[player].client->nvSess.vip = 1;
//		g_entities[player].client->ps.stats[STAT_GAMETYPE_ITEMS] |= (1<<item->giTag);			
//		//level.extras.bombGivenClient[g_entities[player].client->sess.team] = player + 1;
//	}
//}

//
//void NV_setVIPTeamIdent(team_t)
//{
//	int  i, count;
//	
//	for ( i = 0, count = 0; i < level.numConnectedClients && count < MAX_CLIENTS; i ++ )
//	{	
//		gclient_t* client = &level.clients[level.sortedClients[i]];	
//		if ( client->pers.connected != CON_CONNECTED )
//				continue;
//			if ( client->sess.team != team )
//				continue;
//		if(client->sess.team == TEAM_BLUE)
//
//		{
//			if(level.extras.bombGivenClient[client->sess.team] -1 == level.sortedClients[i] && strcmp(client->pers.identity->mName,"NPC_Taylor_Lab/taylor_lab" )
//				|| level.extras.bombGivenClient[client->sess.team] -1 != level.sortedClients[i] && !strcmp(client->pers.identity->mName,"NPC_Taylor_Lab/taylor_lab" )
//				)
//			{
//				//g_entities[level.sortedClients[i]].client->nvSess.capturing = 1;
//				ClientUserinfoChanged( level.sortedClients[i] );
//				//g_entities[level.sortedClients[i]].client->nvSess.capturing = 2;
//
//			}
//		}	
//		else if(client->sess.team == TEAM_RED)
//		{
//			if(level.extras.bombGivenClient[client->sess.team] -1 == level.sortedClients[i] && strcmp(client->pers.identity->mName,"NPC_HongKong_Civilian/hk_ped_a4" )
//				|| level.extras.bombGivenClient[client->sess.team] -1 != level.sortedClients[i] && !strcmp(client->pers.identity->mName,"NPC_HongKong_Civilian/hk_ped_a4" )
//				)
//			{
//				ClientUserinfoChanged( level.sortedClients[i] );
//			}
//		}
//	}
//}
//void NV_setVIPIdent(void)
//{
//	int  i, count;
//	
//	for ( i = 0, count = 0; i < level.numConnectedClients && count < MAX_CLIENTS; i ++ )
//	{	
//		gclient_t* client = &level.clients[level.sortedClients[i]];	
//		if(client->sess.team == TEAM_BLUE)
//		{
//			if(level.extras.bombGivenClient[client->sess.team] -1 == level.sortedClients[i] && strcmp(client->pers.identity->mName,"NPC_Taylor_Lab/taylor_lab" )
//				|| level.extras.bombGivenClient[client->sess.team] -1 != level.sortedClients[i] && !strcmp(client->pers.identity->mName,"NPC_Taylor_Lab/taylor_lab" )
//				)
//			{
//				//g_entities[level.sortedClients[i]].client->nvSess.capturing = 1;
//				ClientUserinfoChanged( level.sortedClients[i] );
//				//g_entities[level.sortedClients[i]].client->nvSess.capturing = 2;
//
//			}
//		}	
//		else if(client->sess.team == TEAM_RED)
//		{
//			if(level.extras.bombGivenClient[client->sess.team] -1 == level.sortedClients[i] && strcmp(client->pers.identity->mName,"NPC_HongKong_Civilian/hk_ped_a4" )
//				|| level.extras.bombGivenClient[client->sess.team] -1 != level.sortedClients[i] && !strcmp(client->pers.identity->mName,"NPC_HongKong_Civilian/hk_ped_a4" )
//				)
//			{
//				ClientUserinfoChanged( level.sortedClients[i] );
//			}
//		}
//	}
//}

void NV_GiveGTIToTeam ( team_t team, qboolean announce )
{
	gitem_t* item;		
	level.extras.bombGivenClient[team] = 0;

	if(NV_isVIP())
	{
		switch(team)
		{
			case TEAM_RED:
				item = BG_FindGametypeItemByID ( ITEM_REDFLAG ); 
				break;
			case TEAM_BLUE:	
				item = BG_FindGametypeItemByID ( ITEM_BLUEFLAG );
				break;
		}
	}
	else
	{
		item = &bg_itemlist[35];
	}

	if ( item )	
	{		
		int  i, count, randPlayer, hasBeen = -1, vipLastTime = 0;
		int	 teamClients[MAX_CLIENTS];
		
		//G_ResetGametypeItem ( item );

		for ( i = 0, count = 0; i < level.numConnectedClients && count < MAX_CLIENTS; i ++ )
		{
			gclient_t* client = &level.clients[level.sortedClients[i]];				
			if ( client->pers.connected != CON_CONNECTED )
				continue;
			if ( client->sess.team != team )
				continue;
			if ( G_IsClientDead ( client ) )
				continue;
			if( client->nvSess.vip ) // if this player was VIP, skip them for now but mark them as being able.
			{
				client->nvSess.vip = 2;
				if(client->nvSess.vipLastTime <= vipLastTime || vipLastTime == 0)
				{
					vipLastTime = client->nvSess.vipLastTime;
					hasBeen = level.sortedClients[i];
				}
				continue;	
			}

#ifdef _SOF2_BOTS
			//if(g_entities[level.sortedClients[i]].r.svFlags & SVF_BOT)
			//	continue;
#endif
			teamClients[count++] = level.sortedClients[i];
		}
		if(!count)
		{
			if(hasBeen >= 0) // we have a player that has been before, so give it to them.
				randPlayer = hasBeen;
			else	
				return;
		}
		else
		{
			randPlayer = teamClients[irand(0, count-1)];
		}
		if(announce)
		{
			GT(va("%i, %s ^+has received the ^1bomb", level.time + 5000, &g_entities[randPlayer].client->pers.netname ));
		}
		g_entities[randPlayer].client->ps.stats[STAT_GAMETYPE_ITEMS] |= (1<<item->giTag);			
		level.extras.bombGivenClient[team] = randPlayer + 1;
		if(NV_isVIP())
		{	
			char chat[128];
			Com_sprintf(chat, 128, "^3%s ^+is the team ^3VIP!", g_entities[randPlayer].client->pers.netname);
			TeamSound(team, "sound/ambience/generic/alarm04.mp3", chat );
			g_entities[randPlayer].client->nvSess.vip = 1;
			g_entities[randPlayer].client->sess.lastIdentityChange = level.time - 1000;
			ClientUserinfoChanged( randPlayer );
			g_entities[randPlayer].client->nvSess.vipLastTime = level.time;
		}
	}
}


/*
void NV_GiveRoundItem ( void )
{
	gitem_t* item;		

	item = BG_FindGametypeItemByID ( 301 );
	G_ResetGametypeItem ( item );

	item = BG_FindGametypeItemByID ( 100 ); // BOMB
	if ( item )	
	{		
		int  i, count, randPlayer;
		int	 blueClients[MAX_CLIENTS];
		
		G_ResetGametypeItem ( item );

		for ( i = 0, count = 0; i < level.numConnectedClients && count < MAX_CLIENTS; i ++ )
		{
			gclient_t* client = &level.clients[level.sortedClients[i]];				
			if ( client->pers.connected != CON_CONNECTED )
				continue;
			if ( client->sess.team != TEAM_BLUE )
				continue;
			if ( G_IsClientDead ( client ) )
				continue;
#ifdef _SOF2_BOTS
			if(g_entities[level.sortedClients[i]].r.svFlags & SVF_BOT)
				continue;
#endif
			blueClients[count++] = level.sortedClients[i];
		}
		if(!count)
			return;

		randPlayer = blueClients[irand(0, count-1)];
		GT(va("%i, %s ^7has received the ^1bomb", level.time + 5000, &g_entities[randPlayer].client->pers.netname ));
		//level.clients[(count+2)%count].ps.stats[STAT_GAMETYPE_ITEMS] |= 1;
		g_entities[randPlayer].client->ps.stats[STAT_GAMETYPE_ITEMS] |= (1<<item->giTag);
		level.extras.bombGivenClient[TEAM_BLUE] = randPlayer + 1;		
	}
}
*/






#ifdef _OLDVIP

void NV_GiveDistanceHealth (gentity_t* self)
{
	float		points, dist, radius = 800;
	gentity_t	*ent, *tent;
	int			entityList[MAX_GENTITIES];
	int			numListedEntities;
	vec3_t		mins, maxs;
	vec3_t		v;
	vec3_t		dir;
	int			i, e;
	qboolean	hitClient = qfalse;
	vec3_t		origin;	

	VectorCopy(self->client->ps.origin, origin);

	for ( i = 0 ; i < 3 ; i++ )
	{
		mins[i] = origin[i] - radius;
		maxs[i] = origin[i] + radius;
	}

	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_CLIENTS );

	for ( e = 0 ; e < numListedEntities ; e++ )
	{
		ent = &g_entities[entityList[ e ]];

		if (ent == self)
		{
			continue;
		}
		if(!OnSameTeam(ent, self))
		{
			continue;
		}
		if ( ent->client->pers.connected == CON_CONNECTING ) 
		{
			continue;
		}
		if ( G_IsClientSpectating ( ent->client ) || G_IsClientDead ( ent->client ) )
		{
			continue;
		}
		// find the distance from the edge of the bounding box
		for ( i = 0 ; i < 3 ; i++ )
		{
			if ( origin[i] < ent->r.absmin[i] )
			{
				v[i] = ent->r.absmin[i] - origin[i];
			}
			else if ( origin[i] > ent->r.absmax[i] )
			{
				v[i] = origin[i] - ent->r.absmax[i];
			}
			else
			{
				v[i] = 0;
			}
		}

		dist = VectorLength( v );
		if ( dist >= radius )
		{
			continue;
		}	
		if(ent->client->ps.stats[STAT_HEALTH] <= 80)
		{
			ent->client->ps.stats[STAT_HEALTH] = ent->health += 20;
			GT(va("%i, %s ^7is close enough for health (%i)", level.time + 3000, ent->client->pers.netname, ent->client->ps.stats[STAT_HEALTH] ));
		}
		else if(ent->client->ps.stats[STAT_HEALTH] < 100)
		{	
			ent->client->ps.stats[STAT_HEALTH] = ent->health = 100;
			GT(va("%i, %s ^7is close enough for health (FULL)", level.time + 3000, ent->client->pers.netname ));
		}
		else
		{
			GT(va("%i, %s ^7is close enough for health but already FULL", level.time + 3000, ent->client->pers.netname ));
			continue;
		}
		
	}
}

#endif



gspawn_t* VIP_SelectRandomSafeSpawnPoint ( team_t team, float safeDistance )
{
	gspawn_t*	spawns[MAX_SPAWNS];
	float		safeDistanceSquared;
	int			count;
	int			i;

	// Square the distance for faster comparisons
	safeDistanceSquared = safeDistance * safeDistance;

	// Build a list of spawns
	for ( i = 0, count = 0; i < level.spawnCount; i ++ )
	{
		gspawn_t* spawn = &level.spawns[i];
		int		  j;

		//if ( spawn->team == enemy )
		if ( spawn->team != team && spawn->team != TEAM_FREE )
		{
			continue;
		}	

		// Make sure this spot wont kill another player
		if ( G_SpotWouldTelefrag( spawn ) )
		{
			continue;
		}


		// Loop through connected clients
		for ( j = 0; j < level.numConnectedClients && count < MAX_SPAWNS; j ++ )
		{
			gentity_t	*other = &g_entities[level.sortedClients[j]];
			vec3_t		diff;
			int			max, min;

			if ( other->client->pers.connected != CON_CONNECTED )
			{
				continue;
			}

			// Skip clients that are spectating or dead
			if ( G_IsClientSpectating ( other->client ) || G_IsClientDead ( other->client ) )
			{
				continue;
			}
				
			if ( other->client->nvSess.vip != 1 )
			{
				continue;
			}

			// on safe team, dont count this guy
			if ( team != other->client->sess.team )
			{
				continue;
			}
					
			max = maximum(other->r.currentOrigin[2], spawn->origin[2] );
			min = minimum(other->r.currentOrigin[2], spawn->origin[2] );

			// dont spawn too high or low above the player so we arent spawning on roofs, lower tunnles, etc
			if(max - min > 250)
			{
				break;
			}

			VectorSubtract ( other->r.currentOrigin, spawn->origin, diff );

			// Far enough away to qualify
			if ( VectorLengthSquared ( diff ) > safeDistanceSquared )
			{
				break;
			}
		}

		// If we didnt go through the whole list of clients then we must
		// have hit one that was too close.  But if we did go through teh whole
		// list then this spawn point is good to go
		if ( j >= level.numConnectedClients )
		{
			spawns[count++] = spawn;
		}
	}

	// Nothing found, try it at half the safe distance
	if ( !count )
	{
		return NULL;
	}

	// Spawn them at one of the spots
	return spawns[ rand() % count ];
}

gspawn_t* VIP_GetVIPSpawnPoints (gentity_t* ent, team_t team)
{
	gspawn_t*	spawnPoint;

	spawnPoint = VIP_SelectRandomSafeSpawnPoint ( team, 800 );	
		
	// go a little further
	//if ( !spawnPoint )
	//{
	//	spawnPoint = VIP_SelectRandomSafeSpawnPoint ( team, 300 );	
	//}	

	// Spawn at any gametype spawn, telefrag if needed
	if ( !spawnPoint )
	{
		spawnPoint = G_SelectRandomSpawnPoint ( team );
	}
	if ( !spawnPoint )
	{
		spawnPoint = G_SelectRandomSpawnPoint ( (team_t)-1 );
	}

	return spawnPoint;
}








