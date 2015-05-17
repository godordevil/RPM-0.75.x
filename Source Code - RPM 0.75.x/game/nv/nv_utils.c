
#include "../g_local.h"

gentity_t*	LaunchItem		( gitem_t *item, vec3_t origin, vec3_t velocity );


void QDECL NV_Log( int type, const char *text, ... )
{
	char			string[400] = "\0";
	int				min, sec;
	va_list			argptr;
	qtime_t			q;
	fileHandle_t	f;

	trap_RealTime (&q);
	sec = level.time / 1000;
	min = sec / 60;

	if(strlen(text) >= 400)
	{
		return;
	}
	Com_sprintf( string, sizeof(string), "%02i-%02i-%02i %2i:%02i", 1+q.tm_mon,q.tm_mday, 1900+q.tm_year,q.tm_hour,q.tm_min);
	va_start( argptr, text );
	vsprintf( string , text, argptr );
	va_end( argptr );
	Com_sprintf( string, sizeof( string ), "%02i-%02i-%02i %2i:%02i - %s\n", 1+q.tm_mon,q.tm_mday, 1900+q.tm_year,q.tm_hour,q.tm_min, text );

	switch(type)
	{
		default:
		case 1:
			trap_FS_FOpenFile( "logs/sql.log", &f, FS_APPEND_TEXT);
			break;
		case 2:
			trap_FS_FOpenFile( "logs/ents.log", &f, FS_APPEND_TEXT);
			break;
		case 3:
			trap_FS_FOpenFile( "logs/adminlog.txt", &f, FS_APPEND_TEXT);
			break;
		case 5:
			trap_FS_FOpenFile( "logs/db_mod.log", &f, FS_APPEND_TEXT);
			break;
		case 6:
			trap_FS_FOpenFile( "logs/compmode.txt", &f, FS_APPEND_TEXT);
			break;
	}

	trap_FS_Write( string, strlen( string ), f );
	trap_FS_FCloseFile(f);
}
void NV_LoadFile( char *file, char *buf, qboolean caseSensitive )
{
	fileHandle_t	f;
	int				len;

	len = trap_FS_FOpenFile( file, &f, FS_READ_TEXT );
	if ( !f ) 
	{
		buf[0] = 0;
		return;
	}
	if ( len >= 16000) 
	{
		len = 16000;
	}

	trap_FS_Read( buf, len, f);
	trap_FS_FCloseFile(f);
	buf[len] = 0;
	if(!caseSensitive)
		Q_strlwr(buf);

}

// GodOrDevil :  Boeman's fix for connecting character lag - 1fx.uk.to
/*
================
trap_GetUserinfo
6/20/13 - 10:57 AM
Function with the weird movement fix, relays call to the real trap_GetUserinfo.
================
*/

void trap_GetUserinfo( int num, char *buffer, int bufferSize ) {
        char            buf2[MAX_INFO_STRING];
        qboolean        found = qfalse;
        int              i;

        trap_GetUserinfo2( num, buffer, bufferSize );

        // Boe!Man 1/31/13: Fix for weird movement when people connect/change teams etc. with unsupported characters in their names.
        Q_strncpyz(buf2, buffer, sizeof(buf2));
        for(i = 0; i < strlen(buffer); i++){
                if(buf2[i] <= 0){ // Boe!Man 1/31/13: Any ASCII character that is esentially malformed, we change by resetting this character to a dot.
                        buf2[i] = 46; // Convert to dot.
                        found = qtrue;
                }
        }

        if(found)
                trap_SetUserinfo(num, buf2);

        trap_GetUserinfo2( num, buffer, bufferSize );
}
// END

gclient_t* NV_FindClientByCleanName ( const char* name, int ignoreNum )
{
	int		i;

	for ( i = 0; i < level.numConnectedClients; i ++ ){
		gentity_t* ent = &g_entities[level.sortedClients[i]];

		if ( level.sortedClients[i] == ignoreNum )
			continue;

		if ( Q_stricmp ( name, ent->client->nvSess.cleanName ) == 0 )
			return ent->client;
	}
	return NULL;
}
void NV_adminPrint(char *input )
{
	int i;
	for (i = 0; i < level.numConnectedClients; i++)	
	{
		if (level.clients[level.sortedClients[i]].nvSess.admin || level.clients[level.sortedClients[i]].nvSess.referee)
		{	
			CPx(level.sortedClients[i], va("print \"^3%s\n\"", input));		
		}
	}
}
void NV_Sound(int clientNum, char *file )
{
	gentity_t *tent;

	if(!strcmp(file, ""))
		return;

	if(clientNum == -1) {
		tent = G_TempEntity( vec3_origin, EV_GLOBAL_SOUND );
		tent->r.svFlags |= SVF_BROADCAST;
		tent->s.eventParm = G_SoundIndex(file);
		return;
	}
	if(g_entities[clientNum].client->nvSess.timeLastsound < level.time)
	{
		//char	*s;
		char	userinfo[MAX_INFO_STRING];
		
		trap_GetUserinfo( clientNum , userinfo, sizeof( userinfo ) );
		//s = Info_ValueForKey (userinfo, "nvc_disableSounds");
		//if(s[0] == '1' || s[0] == '3')
		//{
		//	return;
		//}

		tent = G_TempEntity( g_entities[clientNum].client->ps.origin, EV_GLOBAL_SOUND);

		tent->r.singleClient = clientNum;
		tent->r.svFlags |= SVF_SINGLECLIENT;
		tent->s.eventParm = G_SoundIndex(file);
		g_entities[clientNum].client->nvSess.timeLastsound = level.time + 50;
		return;
	}
}
void TeamSound(int team, char *sound, char* chat)
{
	int i;

	for ( i = 0 ; i < level.maxclients ; i++ )
	{
		if ( level.clients[i].pers.connected != CON_CONNECTED )
			continue;
		if (level.clients[i].sess.team != team)
			continue;

		if(sound[0])
			NV_Sound(i, sound );

		if(chat[0])
			trap_SendServerCommand( i, va("chat -1 \"%s\n\"", chat ));
	}
}
void NV_playerDropItem (gentity_t *player, gentity_t *attacker, char *itemName)
{	
	gitem_t*	itemDrop;
	vec3_t		vel = {0,0,0};


	if(!player || player == NULL)
		return;

	if(itemName && itemName[1])
	{
		itemDrop = BG_FindClassnameItem ( itemName );
	}
	else
	{
		itemDrop = BG_FindClassnameItem ( "pickup_backpack" );
	}	
	if(itemDrop != NULL)
		LaunchItem( itemDrop, player->r.currentOrigin, vel);
}
void NV_playerDropRandomItem (gentity_t *player, gentity_t *attacker, qboolean announceItem)
{
	int i;

	i = irand(1, 34);
			
	NV_playerDropItem (player, NULL, bg_itemlist[i].classname);
	if(announceItem && attacker != NULL)
	{
		CP(va("print \"^3%s ^7was ^Gfreed ^7by ^3%s\n\"", player->client->pers.netname, attacker->client->pers.netname ));
		trap_GT_SendEvent ( GTEV_ITEM_DEFEND, level.time, attacker->health, attacker->s.clientNum, 0, 0, 0 );				
		trap_SendServerCommand( attacker-g_entities, va("cp \"@^1Grab ^3%s\"",bg_itemlist[i].pickup_name ) );
		trap_SendServerCommand( player-g_entities, va("cp \"@^1VIOLATED\"" ) );
	}
}
void NV_MissileDie ( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod, int filler, vec3_t filler2 )
{
	if (inflictor == self)
		return;

	self->think			=	G_ExplodeMissile;
	self->nextthink		=	level.time + 10;
}
void NV_ItemDie ( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod, int filler, vec3_t filler2 )
{
	if (inflictor == self)
		return;
			
	self->nextthink		=	level.time + 10;
}
void G_PlayEffect(int fxID, vec3_t org, vec3_t ang);
void NV_updateSpawnTeam (gentity_t *flagEnt)
{
	int i; 	
	gspawn_t *spawn;

	for (i = 0; i < level.spawnCount; i++)
	{
		spawn = &level.spawns[i];		
		if(spawn->linkedEnt == flagEnt->s.number)
		{
			//Com_Printf("NV_updateSpawnTeam(): Spawn Team: %i - static ent: %i",spawn->team, spawn->linkedEnt);
			spawn->team = flagEnt->flags;
			spawn->linkedEnt = flagEnt->s.number;
			//Com_Printf("NV_updateSpawnTeam(): Spawn Team: %i - static ent: %i",spawn->team, spawn->linkedEnt);
		}
	}
}
void NV_updateKothSpawnTeam (team_t team)
{
	int i; 	
	gspawn_t *spawn;

	for (i = 0; i < level.spawnCount; i++)
	{
		spawn = &level.spawns[i];
		// Set the 'FREE' Spawn as the team spawn
		if(spawn->team == TEAM_FREE)
		{
			spawn->linkedEnt = 1;
			spawn->team = team;
			continue;
		}
		// Now if it was a free spawn, give it to the team that controls the zone
		if(spawn->linkedEnt == 1)
		{
			spawn->team = team;
			continue;
		}
		if(team == TEAM_RED && spawn->team == team)
		{
			spawn->team = TEAM_BLUE;
			continue;
		}	
		if(team == TEAM_BLUE && spawn->team == team)
		{
			spawn->team = TEAM_RED;
			continue;
		}
	}
}
int NV_spawnLinked ( vec3_t pos )
{
	int		i;
	int		result;
	float	dist, dist2;
	gentity_t *ent;
	//qboolean	rush;

	dist   = 9999999.0f;
	result = -1;

	//rush = NV_isRush();
	ent = &g_entities[MAX_CLIENTS-1];
	for (i = MAX_CLIENTS-1; i < level.num_entities; i++, ent++) 
	{	
		if( !ent->classname)
			continue;
		if (Q_strncmp (ent->classname, "gametype_item_", 14))
			continue;		
		
		dist2 = DistanceSquared( pos, ent->s.origin );

		if ( dist2 < dist )
		{
			dist = dist2;
			result = ent->s.number;
		}		
	}

	return result;
}
void NV_reLinkSpawns (void)
{
	int i;
	gspawn_t* spawn = &level.spawns[0];

	for ( i = 0; i < level.spawnCount; i++, spawn++ )
	{
		spawn->linkedEnt = NV_spawnLinked(spawn->origin);
		if(spawn->linkedEnt && spawn->team <= 0){
			spawn->team = g_entities[spawn->linkedEnt].flags;
#ifdef _DEBUG
			//Com_Printf("NV_reLinkSpawns(): Spawn Team: %i - static ent: %i - Phase: ", spawn->team, spawn->linkedEnt, spawn->phase);
#endif
		}
	}
}
void NV_SetPhaseSpawns(void)
{
	int i;
	gspawn_t* spawn = &level.spawns[0];

	for ( i = 0; i < level.spawnCount; i++, spawn++ )
	{
		if(spawn->team == TEAM_BLUE)
		{
			//Com_Printf("NV_SetPhaseSpawns(): Spawn Team: %i -> %i  Phase: %i", spawn->team, TEAM_FREE, spawn->phase);
			spawn->team = TEAM_FREE;
			continue;
		}
		if(spawn->team == TEAM_RED)
		{
			//Com_Printf("NV_SetPhaseSpawns(): Spawn Team: %i -> %i  Phase: %i", spawn->team, TEAM_BLUE, spawn->phase);
			spawn->team = TEAM_BLUE;
			continue;
		}
		if(spawn->team == TEAM_FREE)
		{
			if(spawn->phase == level.extras.currentPhase)
			{
				//Com_Printf("NV_SetPhaseSpawns(): Spawn Team: %i -> %i  Phase: %i", spawn->team, TEAM_RED, spawn->phase);
				spawn->team = TEAM_RED;
				continue;
			}
		}
	}
}
void NV_spawnFlagMarker (gentity_t *ent)
{
	char		*origin;
	//char		*angles;
	
	//if(!strcmp(ent->target, "demolish1"))
	//	G_SpawnDebugCylinder ( ent->s.origin, 8, &g_entities[0], 2000000, COLOR_BLUE );	
	//else
	//	G_SpawnDebugCylinder ( ent->s.origin, 8, &g_entities[0], 2000000, COLOR_RED );	

	if(!ent || !ent->target || !ent->target[0])
		return;
	origin = va("%.0f %.0f %.0f",
		ent->s.origin[0], ent->s.origin[1], ent->s.origin[2] );
	//angles = va("%.0f %.0f %.0f",
	//	ent->s.angles[0], ent->s.angles[1], ent->s.angles[2] );
	
	AddSpawnField("classname", "nv_model");
	AddSpawnField("origin", origin);
	//AddSpawnField("angles", angles);
	if(!strcmp(ent->target, "demolish1"))
		AddSpawnField("model", "models/flags/flag_blue.md3");
	else
		AddSpawnField("model", "models/flags/flag_red.md3");

	G_SpawnGEntityFromSpawnVars(qfalse);
	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;

}
void NV_spawnBombEffect		(gentity_t *ent)
{
	char		*origin;

	if(!ent || !ent->target || !ent->target[0])
		return;
	origin = va("%.0f %.0f %.0f",
		ent->s.origin[0], ent->s.origin[1], ent->s.origin[2] );

	AddSpawnField("classname", "bomb_kill");
	AddSpawnField("origin", origin);
	AddSpawnField("targetname", ent->target);
	//AddSpawnField("effect", "effects/explosions/pra2_truck_explosion");
	AddSpawnField("effect", "effects/explosions/helicopter_explosion");
	AddSpawnField("splashDamage", "3000");
	AddSpawnField("splashRadius", "1000");
	G_SpawnGEntityFromSpawnVars(qfalse);
	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;

}

void NV_FlareEffect(team_t t)
{	
	gentity_t* find;			
	vec3_t o, up = {0,0,-1};	

	if(t == TEAM_BLUE)
	{
		find = NULL;
		while ( NULL != (find = G_Find ( find, FOFS(classname), "gametype_trigger" ) ) )
		{
			if ( Q_stricmp ( find->targetname, "briefcase_destination_red" ) )
			{
				continue;
			}
			VectorCopy(find->s.origin, o);
			o[2] = level.worldMaxs[2] - level.worldMins[2];
			G_PlayEffect(G_EffectIndex ( "flare_blue.efx"), o, up );
		}
	}
	else
	{	
		find = NULL;
		while ( NULL != (find = G_Find ( find, FOFS(classname), "gametype_trigger" ) ) )
		{
			if ( Q_stricmp ( find->targetname, "briefcase_destination_blue" ) )
			{
				continue;
			}
			VectorCopy(find->s.origin, o);
			o[2] = level.worldMaxs[2] - level.worldMins[2];
			G_PlayEffect(G_EffectIndex ( "flare_red.efx"), o, up );
		}
	}

}

static int oneSecTimer;
static int fiveSec, TwoSec, FourSec;
#ifdef _DB_ENABLED
static int fiveMinTimer = 20000;
static int TopStatTimer = 20000;
#endif
static qboolean spawnsRelinked = qfalse;

#ifdef _BOT_STRESS
static int tenSecTimer;
static qboolean swapKick = qtrue;
#endif

#ifdef _CRS_BOTS	
// KRIS 15/04/2003 11:28PM
void G_CheckClientWPEdit(gentity_t *ent);
// KRIS
#endif

qboolean NV_clientOnceSecondActions(gentity_t *ent, qboolean specOnly)
{	
	gclient_t	*client;
	int		distance;
	vec3_t 	diff;

	client = ent->client;
	
	if( level.time > client->sess.oneSecChecks)
	{	
		client->sess.oneSecChecks = level.time + 1000;
		
#ifdef _DB_ENABLED
		ent->client->nvSess.teamTime[ent->client->sess.team]++;
#endif // _DB

	}

	if(specOnly)
	{
		// This will add scores to the opposite team of a frozen client
		if(!level.extras.roundOver && level.firstBlood && 
			ent->client->nvSess.frozenTime > level.time && 
			level.time >= ent->client->nvSess.frozenFiveSecTimer + 5000
			&&  NV_isTFT() )
		{
			if(ent->client->sess.team == TEAM_BLUE)
				G_AddTeamScore ( TEAM_RED, 1 );		
			else if(ent->client->sess.team == TEAM_RED)
				G_AddTeamScore ( TEAM_BLUE, 1 );
 			ent->client->nvSess.frozenFiveSecTimer = level.time + 5000;
		}
			// END
		if ( client->nvSess.frozen > 0 && client->nvSess.frozenTime && client->nvSess.frozenTime < level.time && client->sess.team != TEAM_SPECTATOR )
		{		
			NV_respawn (ent, qfalse);
			return qtrue;
		}
		else if(client->sess.ghost && client->ps.respawnTimer <= level.time + 1000 && !client->nvSess.frozen && client->sess.team != TEAM_SPECTATOR && (NV_isFT() || NV_isTFT()))
		{	
			gentity_t	*tent;
			client->sess.ghost = qtrue;
			trap_UnlinkEntity (ent);
			G_StopFollowing( ent );
			client->sess.ghost = qfalse;
			ClientSpawn(ent);
			// Add a teleportation effect.
			tent = G_TempEntity( ent->client->ps.origin, EV_PLAYER_TELEPORT_IN );
			tent->s.clientNum = ent->s.clientNum;
			return qtrue;
		}
	}
	else
	{
#ifdef _OLDVIP
		if(NV_isVIP())
		{
			// do nothing
		}
		else
#endif
		/// #increment health
		if(!ent->client->nvSess.frozen && (ent->client->ps.stats[STAT_HEALTH] < MAX_HEALTH) && ( g_enableHealthGains.value || NV_isTFT() ) && !G_IsClientDead(ent->client))
		{
			if(ent->client->lasthurt_time < level.time + 3000)
			{
				if(g_enableHealthGains.integer)
				{
					ent->client->ps.stats[STAT_HEALTH] = ent->health += g_enableHealthGains.value;
					if(ent->client->ps.stats[STAT_HEALTH]>100)
						ent->client->ps.stats[STAT_HEALTH] = ent->health = MAX_HEALTH;
				}
				else
				{
					ent->client->ps.stats[STAT_HEALTH] = ent->health += 5.25;
					if(ent->client->ps.stats[STAT_HEALTH] > MAX_HEALTH)
						ent->client->ps.stats[STAT_HEALTH] = ent->health = MAX_HEALTH;
				}
			}
		}
#ifdef _MAPEDITOR
	if(nv_editMap.integer)
	{

	}
	else
#endif
		if ((level.time - level.startTime < 20000 && level.gametypeData->respawnType != RT_NONE) 
			|| (level.gametypeData->respawnType == RT_NONE && ((level.time - level.gametypeStartTime) < g_roundstartdelay.integer * 1000))
			|| (level.gametypeDelayTime && level.gametypeDelayTime > level.time)			
			|| ent->client->ps.ping > 900 )
		{

		}
#ifdef _TORTURE
		else if(g_camperPunish.integer	&& !ent->client->pers.planted && !ent->s.gametypeitems && !ent->client->nvSess.torture)
#else
		else if(g_camperPunish.integer	&& !ent->client->pers.planted && !ent->s.gametypeitems )
#endif
		{
			if(g_camperSniper.integer && ent->client->ps.weapon == WP_MSG90A1)
			{

			}
			else if(ent->client->sess.team != TEAM_SPECTATOR && ent->client->ps.pm_type != PM_DEAD 
				&& ent->client->ps.pm_type != PM_INTERMISSION && !ent->client->ps.respawnTimer
				&& !ent->client->sess.ghost )
			{

					if(g_camperRadius.integer < 20)
						g_camperRadius.integer = 20;
					
					VectorSubtract( ent->r.currentOrigin, ent->camperOrigin, diff );
					distance=(int)VectorLength(diff);
					
					if ((level.time - ent->client->sess.camperSeconds) / 1000 == 10)
						ent->client->sess.camperSeconds = 0;
					
					if (distance < g_camperRadius.integer)
					{
						ent->client->sess.camperSeconds++;

						if (ent->client->sess.camperSeconds >= g_camperAllowTime.integer - 5 && ent->client->sess.camperSeconds < g_camperAllowTime.integer)
						{
							trap_SendServerCommand(ent-g_entities, va("cp \"@^+You are about to be ^1Punished ^+for ^3Camping^7!\n\""));
							RPM_ClientSound(&g_entities[ent->s.number], G_SoundIndex("sound/movers/buttons/button03.mp3"));
							trap_SendServerCommand( ent - g_entities, va("chat -1 \"^+You are about to be ^1Punished ^+for ^3Camping^+! ^3Time until Camper ^1Punishment: ^1%i\n\"", g_camperAllowTime.integer - ent->client->sess.camperSeconds));
						}
						else if(ent->client->sess.camperSeconds == g_camperAllowTime.integer) 
						{
							//ent->client->sess.camperCount++;
							ent->client->pers.camped = qtrue;
							//trap_SendServerCommand(ent-g_entities, va("cp \"@\n\""));
							trap_SendServerCommand( ent - g_entities, va("chat -1 \"^+You have been ^1Punished ^+for ^3CAMPING ^+by the ^iServer\n\""));
														
							//if (ent->client->sess.camperCount == 1)
							//{
							//	trap_SendConsoleCommand( EXEC_INSERT, va("%s %i\n", g_camperPunishment.string, ent->s.number ) );
							//}
							//else if (ent->client->sess.camperCount == 2)
							//{
							//	trap_SendConsoleCommand( EXEC_INSERT, va("%s %i\n", g_camperPunishment2.string, ent->s.number ) );
							//}
							//else if (ent->client->sess.camperCount == 3)
							//{
							//	trap_SendConsoleCommand( EXEC_INSERT, va("%s %i\n", g_camperPunishment3.string, ent->s.number ) );
							//	ent->client->sess.camperCount = 0;
							//}
							
							trap_SendConsoleCommand( EXEC_INSERT, va("%s %i\n", g_camperPunishment.string, ent->s.number ) );
							trap_SendServerCommand( -1, va("print \"^3%s was punished for camping\n\"", ent->client->pers.netname ));
							VectorCopy(ent->r.currentOrigin, ent->camperOrigin);
							ent->client->sess.camperSeconds = 0;
						}
					}
					else
					{
						VectorCopy(ent->r.currentOrigin, ent->camperOrigin);
						ent->client->sess.camperSeconds = 0;
					}
			}
			else{
				VectorCopy(ent->r.currentOrigin, ent->camperOrigin);
				ent->client->sess.camperSeconds = 0;
			}
		}
		if(client->sess.penaltySeconds)
		{
			client->ps.respawnTimer = level.time + (client->sess.penaltySeconds * 1000);
			client->sess.penaltySeconds--;

			/// Tell the player the reason for the penalty and how much time is left in it
			trap_SendServerCommand(ent-g_entities, va("cp \"@^3You've been Thrown into the Penalty Box\n^4Reason: ^7%s\nTime left: %d seconds\n\"", client->sess.penaltyReason, client->sess.penaltySeconds));
			
			/// penalty over?
			if(!client->sess.penaltySeconds)
			{
				/// clean up some things
				ent->client->sess.mute = qfalse;
				ent->client->sess.noTeamChange = qfalse;
				
				if ( ent->client->sess.ghost )	
				{
					/// Disable being a ghost
					ent->client->ps.pm_flags &= ~PMF_GHOST;
					ent->client->ps.pm_type = PM_NORMAL;
					ent->client->sess.ghost = qfalse;
				}

				/// respawn the player
				trap_UnlinkEntity (ent);
				ClientSpawn(ent);

				trap_SendServerCommand(ent-g_entities, va("cp \"@Your penalty has Expired!\n\""));
				trap_SendServerCommand( -1, va("print \"^3%s's penalty has expired.\n\"", ent->client->pers.netname));		
			}
		}
#ifdef _TORTURE
		if(ent->client->nvSess.torture )
		{
			SC_doTorture(ent);
		}
#endif

		///03.30.05 - 12:52pm #Messages
		if(client->sess.msgSeconds )
		{
			client->sess.msgSeconds--;
			trap_SendServerCommand(ent-g_entities, va("cp \"@%s\n\"", client->sess.msgReason));
		}
#ifdef _TORTURE
		///03.30.05 - 12:52pm #Burn
		if ( client->sess.burnSeconds )
		{
			client->sess.burnSeconds--;
			if (ent->client->ps.stats[STAT_HEALTH] >= 35)
				G_Damage (ent, NULL, NULL, NULL, NULL, 48, 0, MOD_BURN, HL_NONE );
			G_ApplyKnockback ( ent, dir, 10 );	//knock them back
		}
#endif

	#ifdef _SPECIAL_NADES
		if(ent->client->nvSess.gasTime)
		{
			NV_checkGas(ent);
		}
	#endif

		if(g_maxSpawnKill.integer && ent->client->nvSess.spawnKillLast >= 1)
		{
			ent->client->nvSess.spawnKillLast--;
			if(ent->client->nvSess.spawnKillLast <= 0)
			{
				ent->client->nvSess.spawnKillCount = 0;
			}
		}
		if((level.time - level.startTime) >= FourSec && ent->client->ps.stats[STAT_GAMETYPE_ITEMS] && NV_isSab())
		{	
			NV_FlareEffect(ent->client->sess.team);
			FourSec = (level.time - level.startTime) + 4000;
		}	

#ifdef _OLDVIP	
		else if((level.time - level.startTime) >= FourSec && ent->client->nvSess.vip == 1 && NV_isVIP())
		{	
			NV_GiveDistanceHealth(ent);
			if(ent->client->lasthurt_time < level.time + 3000 )
			{
				ent->client->ps.painTime = level.time + 200;
				ent->pain_debounce_time = level.time + 200;
				G_Damage (ent, NULL, NULL, NULL, NULL, 20, DAMAGE_NO_KNOCKBACK, MOD_DUGUP, HL_NONE );
				if(ent->client->ps.stats[STAT_HEALTH] < 20 && ent->client->ps.stats[STAT_HEALTH] > 0)
				{
					trap_SendServerCommand(ent-g_entities, va("cp \"^3GET TO A REGEN ZONE!\n\""));
				}
			}
			FourSec = (level.time - level.startTime) + 5000;
		}
#endif
		///02.04.05 - 02:04pm #Warn Message
	
#ifdef _TORTURE
		if(client->sess.warnSeconds)
		{
			client->sess.warnSeconds--;
			trap_SendServerCommand(ent-g_entities, va("cp \"@^_**^7ADMIN WARNING^_**\n^1Reason: ^7%s\n\"", client->sess.warnReason));
			
			/// warn over?
			if(!client->sess.warnSeconds){
				warnings = client->sess.warnCount * 20;
				trap_SendServerCommand(ent-g_entities, va("cp \"@Your ^1WARNING ^7Level is now ^3%i^7!\n\"", warnings));
				ent->client->ps.origin[2] += 10;
				VectorCopy( ent->client->ps.origin, ent->s.origin );
			}
			/// If its their 4th warning, run over them.. alot
			else if (client->sess.warnCount == 4)
			{ 
				G_Damage (ent, NULL, NULL, NULL, NULL, 22, 0, MOD_CAR, HL_NONE );	//do the damage
				G_ApplyKnockback ( ent, dir, 600.0 );	//knock them back
				RPM_ClientSound(ent, G_SoundIndex("sound/ambience/vehicles/hit_scrape.mp3")); // GodOrDevil - Sound to client only.. its annyoing
			}
		}
#endif
	}
//#ifdef _MAPEDITOR
//	if(	nv_editMap.integer)
//	{
//		level.gametypeRoundTime = level.time + (g_roundtimelimit.integer * 60000);
//		client->ps.pm_flags |= PMF_MAPEDITOR;
//		NV_CheckClientEntEdit(ent);
//	}
//	else
//	{
//		// Always make sure the mapeditor flag is cleared if the cvar is off
//		client->ps.pm_flags &= ~PMF_MAPEDITOR;
//	}
//#endif

#ifdef _CRS_BOTS		
	// KRIS 28/03/2003 6:27PM
	G_CheckClientWPEdit(ent); 
	// Kris 
#endif

	return qfalse;
}
void NV_checkMisc (void)
{
	//int i;
	//gentity_t	*ent;
	int LTime = (level.time - level.startTime);

	if(level.intermissiontime || level.intermissionQueued)
		return;

	///Once a Second Checks
	if (oneSecTimer <= LTime )
	{		
		///Cleanup for mode changes
		if (level.modeChangeSec) 
		{
			level.modeChangeSec = 0;
		}
		
		// GodOrDevil - only show server messages durring warmup time in compmode
#ifdef _COMPMODE
		if(g_compMode.integer)
		{
			if(g_enableServerMsgs.integer && level.warmupTime && level.time > level.serverMsg && (LTime >= 1000))
			{
				SC_serverMsg();
			}
		}

		// END
		else 
#endif
		if ( g_enableServerMsgs.integer && level.time > level.serverMsg && (LTime >= 20000))
		{
			SC_serverMsg();
		}
			
#ifdef _MAPEDITOR
		if(nv_editMap.integer)
			NV_drawEntTimer();
#endif

#ifdef _STUPID_SHIT
		///Update admin/ref/clan count
		SC_admCount();
		///#autoUnlock Teams if no admin or ref is on
		if (level.adminCount == 0 && level.refCount == 0 && g_autoUnlock.integer 
			&& (level.extras.redLocked || level.extras.blueLocked)){
			level.extras.blueLocked = 0;
			level.extras.redLocked = 0;
			///Com_Printf("Teams Have Been UNLOCKED!\n");
		}
#endif
		if(level.gametypeData->teams && (level.gametypeData->respawnType != RT_NONE || NV_isSab() /*|| NV_isTFT()*/ ) )
		{
			if ( level.time > level.autoEvenTimer && g_autoEvenTeams.integer >= 1 && (!level.pause || !level.intermissiontime ) && (LTime > 25000)) 
			{
				int red, blue;
				if(g_autoEvenTeams.integer < 5)
					g_autoEvenTeams.integer = 5;
				level.autoEvenTimer = level.time + (g_autoEvenTeams.integer * 1000);

				red = TeamCount( -1, TEAM_RED, NULL );
				blue = TeamCount( -1, TEAM_BLUE, NULL );
					
				if ((red > blue + 1) || (blue > red + 1))
				{
					//RPM_Even_Teams(NULL, qtrue);
					NV_evenTeams(0, NULL, 99);
				}
			}
		}	

		// #dom
		if ( level.gametypeStartTime && !spawnsRelinked)
		{
			if(NV_isDom())
			{
				NV_reLinkSpawns();
			}
			spawnsRelinked = qtrue;
		}
#ifdef _DB_ENABLED
		if(LTime >= fiveMinTimer && level.time > 20000)
		{
			setMapStats();
			fiveMinTimer = oneSecTimer + (5*60000);
			//fiveMinTimer = oneSecTimer + (30000);
		}

#ifdef _DEBUG_OFF // 
		if(/*!level.warmupTime &&*/ g_enableDatabase.integer && g_enableTopStats.integer && LTime >= TopStatTimer)
		{
			NV_SaveMapStats(); // testing this shit
			NV_QueryDB(DB_SHOW_TOPSTATS, nv_gtShort.string);
			TopStatTimer = oneSecTimer + (g_enableTopStats.integer * 30000);
			trap_SendServerCommand(-1, va("print \"%s\n\"", "TOP STATS PRINTING" ));
		}
#else	
		if(!level.warmupTime && g_enableDatabase.integer && g_enableTopStats.integer && LTime >= TopStatTimer)
		{	
			//trap_SendServerCommand( -1, va("print \"^3LT= %i  TST = %i\n\"", LTime, TopStatTimer ));		
			NV_QueryDB(DB_SHOW_TOPSTATS, nv_gtShort.string);
			TopStatTimer = oneSecTimer + (g_enableTopStats.integer * 30000);
		}
#endif
#endif
		if(LTime >= fiveSec && (NV_isDom() || NV_isKoth()))
		{
			if(level.extras.domControlledFlags[TEAM_RED-1])
				G_AddTeamScore( TEAM_RED, level.extras.domControlledFlags[TEAM_RED-1]);
			if(level.extras.domControlledFlags[TEAM_BLUE-1])
				G_AddTeamScore( TEAM_BLUE, level.extras.domControlledFlags[TEAM_BLUE-1]);
		}	
		oneSecTimer = LTime + 1000;
	}
	
	if(LTime > fiveSec)
	{
		fiveSec = oneSecTimer + 5000;
	}
	if(LTime > TwoSec)
	{
		//if(gt_customType.integer >= NV_GT_DOM)
		{
			if ( level.gametypeDelayTime && level.gametypeDelayTime > level.time )
			{	
				NV_sendGametypeHelp(NULL, qfalse);
			}
		}
		TwoSec = (level.time - level.startTime) + 2000;
	}


#ifdef _BOT_STRESS	
	if(LTime >= tenSecTimer)
	{
	//	if (LTime < (g_timelimit.integer + level.timeExtension)*60000 + 5000 && LTime > 5000)
		{
			if(!swapKick)
			{
				
				trap_SendConsoleCommand( EXEC_INSERT, va("kick allbots;"));
				tenSecTimer = oneSecTimer + 5000;
			}
			else
			{
				trap_SendConsoleCommand( EXEC_INSERT, va("exec botfiles/10.cfg;"));
				tenSecTimer = oneSecTimer + 30000;
			}
		
			swapKick = (qboolean)!swapKick;
		}
	}
#endif

}
void PM_AddEvent( int newEvent );
void PM_Use( pmove_t *pm, int msec2 )
{
	int useTime = 0;
	//pmove_t		pm;
		
	//client = &g_entities[clientNum].client;

	//pm.ps = &client->ps;

	// don't allow attack until all buttons are up
	if ( pm->ps->pm_flags & PMF_RESPAWNED ) 
	{
		return;
	}

	// ignore if not a normal player
	if ( pm->ps->pm_type != PM_NORMAL ) 
	{
		return;
	}
	
	// check for dead player
	if ( pm->ps->stats[STAT_HEALTH] <= 0 ) 
	{
		//pm->ps->weapon = WP_NONE;
		return;
	}	
	
	// Cant use so dont bother letting them try
	if ((pm->ps->pm_flags & PMF_CAN_USE) && (NV_isDom()||NV_isKoth()))
	{
		pm->ps->stats[STAT_ARMOR] = pm->ps->stats[STAT_REALARMOR];
		return;
	}

	if(!(pm->ps->pm_flags & PMF_CAN_USE ))
	{
		if ( pm->ps->pm_debounce & PMD_USE )
		{
			pm->ps->pm_debounce &= ~PMD_USE;
			pm->ps->stats[STAT_USETIME] = 0;
		}
		pm->ps->stats[STAT_ARMOR] = pm->ps->stats[STAT_REALARMOR];
		return;
	}
	//if (g_crouchNeeded.integer && !(pm->ps->pm_flags & PMF_DUCKED))
	//{
	//	if ( pm->ps->pm_debounce & PMD_USE )
	//	{
	//		pm->ps->pm_debounce &= ~PMD_USE;
	//		pm->ps->stats[STAT_USETIME] = 0;
	//	}
	//	pm->ps->stats[STAT_ARMOR] = pm->ps->stats[STAT_REALARMOR];
	//	return;
	//}

	pm->ps->pm_debounce |= PMD_USE;
	//pm->ps->weaponstate = (WEAPON_DROPPING );
	//pm->ps->torsoAnim = -1;

	// GodOrDevil
	if(pm->ps->stats[STAT_USETIME_MAX] == 8121) // this means its NOT an actual 'use' item so dont do any armor shit.
	{
		pm->ps->stats[STAT_ARMOR] = pm->ps->stats[STAT_REALARMOR];
		pm->ps->stats[STAT_USETIME] = 0;
		pm->ps->stats[STAT_USETIME_MAX] = 0;
		return;
	}
	// End

	useTime = pm->ps->stats[STAT_USETIME_MAX];
	
	//pm->ps->stats[STAT_ARMOR] = (100 - (pm->ps->stats[STAT_USETIME_MAX] -  pm->ps->stats[STAT_USETIME]) / (pm->ps->stats[STAT_USETIME_MAX] / 100));

	if ( useTime )
	{
		int elapsedTime = pm->ps->stats[STAT_USETIME];
		
		pm->ps->stats[STAT_ARMOR] = (100 - (pm->ps->stats[STAT_USETIME_MAX] -  pm->ps->stats[STAT_USETIME]) / (pm->ps->stats[STAT_USETIME_MAX] / 100));

		if ( elapsedTime < useTime )
		{
			elapsedTime += msec2;
		}

		if ( elapsedTime >= useTime )
		{
			pm->ps->stats[STAT_USETIME] = 0;
			PM_AddEvent ( EV_USE );
			
			// add a little cooldown
			g_entities[pm->ps->clientNum].client->nvSess.useCoolDown = level.time + 2500;

		}
		else
		{

			//int useMax = pm->ps->stats[STAT_USETIME_MAX];
			//int use = pm->ps->stats[STAT_USETIME];
			//int ia = 0;
			
			pm->ps->stats[STAT_USETIME] = elapsedTime;
				
			if(g_entities[pm->ps->clientNum].client->nvSess.useLastTimeMsg >= level.time)
			{
				return;
			}		
					
		//	ia = (100 - (useMax - use) / (useMax / 100));
		//	trap_SendServerCommand(pm->ps->clientNum, va("chat -1 \"^2[^3%s^2] ^7Using Target: ^7(^3%i percent^7)\n\"", nv_gtShort.string, ia));

			g_entities[pm->ps->clientNum].client->nvSess.useLastTimeMsg = level.time + 2000;	
			NV_Sound(&g_entities[pm->ps->clientNum]-g_entities, "sound/misc/events/micro_beeps.mp3" );

		}
		return;
	}	

	if ( !(pm->ps->pm_debounce & PMD_USE) )
	{
		pm->ps->stats[STAT_ARMOR] = pm->ps->stats[STAT_REALARMOR];
		PM_AddEvent ( EV_USE );
	}
}

#ifdef _NEW_MAP_SETUP
nv_mapList_t		mapList[MAX_MAPLIST];

int static nextMapNum = 2;
int static maxMapCount = 0;


static void NV_setLastMaps( void )
{	
	char		map0[MAX_QPATH] = "\0";
	char		map1[MAX_QPATH] = "\0";
	char		map2[MAX_QPATH] = "\0";
	
	trap_Cvar_VariableStringBuffer ( "mapname", map0, MAX_QPATH );
	trap_Cvar_VariableStringBuffer ( "rlast1", map1, MAX_QPATH );		
	trap_Cvar_VariableStringBuffer ( "rlast2", map2, MAX_QPATH );		
		
	trap_SendConsoleCommand( EXEC_APPEND, va("seta rlast1 %s; seta rlast2 %s; seta rlast3 %s\n", map0, map1, map2 ) );
}

static void NV_randomMapSelect(char* map1, char* map2, char* map3, int i, qboolean vote)
{	
	const char	*ptoken = "\0";
	char		*token = "\0";
	char		map[8] = "\0";
	char		maplist[MAX_STRING_CHARS] = "\0";
	
#ifdef _ALTVOTEMAP	 // setting rmap1 with vstr means we need to grab the variables from the string.	
	if(vote)
	{
		trap_Cvar_VariableStringBuffer ( map1, maplist, MAX_STRING_CHARS );	
	}
	else
#endif
	{
		Com_sprintf(map, sizeof(map), "rmap%i", i);
		trap_Cvar_VariableStringBuffer ( map, maplist, MAX_STRING_CHARS );
		Q_strncpyz(level.randVSTR, map, 32);		
	}

	ptoken = maplist;

	while(ptoken) 
	{		
		token = COM_ParseExt(&ptoken, qtrue);
#ifdef _ALTVOTEMAP	 // set vote map based on 'vote' variable

		if(vote && !Q_strncmp(token, "vote", 4))
		{
			if(!Q_strncmp(token, "vstr", 4))	
			{
				token = COM_ParseExt(&ptoken, qfalse);
			}		
			token = COM_ParseExt(&ptoken, qfalse);
			if(token[strlen(token)-1] == ';')
			{
				token[strlen(token)-1] = 0;
			}
			if(*token)
			{
				Q_strncpyz(level.randVSTR, token, 32);			
				trap_Cvar_VariableStringBuffer ( token, maplist, MAX_STRING_CHARS );
				ptoken = maplist;
			}
			continue;
		}

		if(!vote && !Q_strncmp(token, "vstr", 4))
		{		
			token = COM_ParseExt(&ptoken, qfalse);
			if(token[strlen(token)-1] == ';')
			{
				token[strlen(token)-1] = 0;
			}
			trap_Cvar_VariableStringBuffer ( token, maplist, MAX_STRING_CHARS );
			ptoken = maplist;	
			continue;
		}
#endif
		//  Min/Max players needed for map votes
		if(!strcmp(token, "minplayers"))
		{
			token = COM_ParseExt(&ptoken, qtrue);
			if (!token || token[0] == 0) 
				continue;
			if(token[strlen(token)-1] == ';')
			{
				token[strlen(token)-1] = 0;
			}
			if(level.numConnectedClients <= atoi(token))
			{
				return;
			}
		}
		if(!strcmp(token, "maxplayers"))
		{
			token = COM_ParseExt(&ptoken, qtrue);
			if (!token || token[0] == 0) 
				continue;
			if(token[strlen(token)-1] == ';')
			{
				token[strlen(token)-1] = 0;
			}
			if(level.numConnectedClients > atoi(token))
			{
				return;
			}
		}

		if(!strcmp(token, "map") || !strcmp(token, "devmap"))
		{
			token = COM_ParseExt(&ptoken, qtrue);
			if (!token || token[0] == 0) 
				continue;

			if(!vote)
			{
				if(!strcmp(token, map1)|| !strcmp(token, map2) || !strcmp(token, map3))
					return;

				if(!strcmp(token, level.nextMap)) // dont use it if its the same map.
					return;
			}
			Q_strncpyz(level.randMap, token, MAX_QPATH - 1);
			if(level.randMap[strlen(level.randMap)-1] == ';')
			{
				level.randMap[strlen(level.randMap)-1] = '\0';
			}
			level.randMapNum = i;
		}
		else if(!strcmp(token, "rmgmap") || !strcmp(token, "rmgdevmap"))
		{		
			if(!vote)
			{
				if(!strcmp(token, map1)|| !strcmp(token, map2) || !strcmp(token, map3))
					return;

			}

			level.randMapNum = i;
			Q_strncpyz(level.randMap, "RMG", MAX_QPATH - 1);
		}
		else if(!strcmp(token, "g_gametype"))
		{	
			token = COM_ParseExt(&ptoken, qtrue);
			if (!token || token[0] == 0) 
				continue;
			if(!vote)
			{
				if(!strcmp(token, level.nextGameType)) // dont use it if its the same gametype.
					return;
			}
			Q_strncpyz(level.randGameType, token, 8 - 1);
			if(level.randGameType[strlen(level.randGameType)-1] == ';')
			{
				level.randGameType[strlen(level.randGameType)-1] = '\0';
			}
		}
	}			
}
void NV_getRandomMapList (qboolean restart)
{
	int	i, count = 0;	

	//if(!restart)
	//{
	//	NV_setLastMaps();  // Get the last 3 maps played
	//}

	for (i = 1; i <= MAX_MAPLIST; i++)
	{		
		char		map[8] = "\0";
		char		maplist[MAX_STRING_CHARS] = "\0";

		Com_sprintf(map, sizeof(map), "rmap%i", i);
		trap_Cvar_VariableStringBuffer ( map, maplist, MAX_STRING_CHARS );

		if(maplist[5]) 
		{	
			count++;
		}		
	}	

	if(count >= 1)
	{
		int			num = 1;
		char		map1[MAX_QPATH] = "\0";
		char		map2[MAX_QPATH] = "\0";
		char		map3[MAX_QPATH] = "\0";

		trap_Cvar_VariableStringBuffer ( "rlast1", map1, MAX_QPATH );		
		trap_Cvar_VariableStringBuffer ( "rlast2", map2, MAX_QPATH );		
		trap_Cvar_VariableStringBuffer ( "rlast3", map3, MAX_QPATH );

		num = Q_irand(1, count);

		i = 0;
		while(i < 40)
		{	
			if(level.randMapNum)  // we found something
				return;
			NV_randomMapSelect(map1, map2, map3, num, qfalse);
			num++;
			if(num > count)
				num = 1;
			i++;
		}
	}
}
qboolean NV_doRandomMap (void)
{	
	if(level.randVSTR)
	{	
		// Increment the mapcycle
		nextMapNum++;
		if(nextMapNum > maxMapCount)
		{	
			nextMapNum = 1;
		}
		//trap_SendConsoleCommand( EXEC_APPEND, va("set next vstr map%i\n", nextMapNum ) );
		trap_SendConsoleCommand( EXEC_APPEND, va("set next vstr map%i; exec settings/default.cfg; vstr %s\n", nextMapNum, level.randVSTR) );	
		//trap_SendConsoleCommand( EXEC_APPEND, va("exec mapcycle/default.cfg") );
		return qtrue;
	}
	
	return qfalse;
}

void NV_getMapList (qboolean restart)
{
	int		i;	
	char	maplist[MAX_STRING_CHARS] = "\0";
	char		map[8] = "\0";

	maxMapCount = 0;
	nextMapNum = 2;

	if(!restart)
	{
		NV_setLastMaps();  // Get the last 3 maps played
	}

	trap_Cvar_VariableStringBuffer ( "next", maplist, MAX_STRING_CHARS );
	if(maplist[8])
	{
		Q_strncpyz(maplist, 8+maplist, sizeof(maplist)-1);
		nextMapNum = atoi(maplist);
	}	
	
#ifdef _ALTVOTEMAP
	// set random here
	Com_sprintf(map, sizeof(map), "map%i", nextMapNum);
	//NV_randomMapSelect(map, "","", nextMapNum, qtrue);	
#endif

	for (i = 1; i <= MAX_MAPLIST; i++)
	{		
		const char	*ptoken = "\0";
		char		*token = "\0";
		char		prevToken[32] = "\0";

		memset(maplist, 0, sizeof(maplist));
		memset(map, 0, sizeof(map));
		Com_sprintf(map, sizeof(map), "map%i", i);
		trap_Cvar_VariableStringBuffer ( map, maplist, MAX_STRING_CHARS );
		ptoken = maplist;			

		while(ptoken) 
		{	
			if(*token)
			{	
				Q_strncpyz(prevToken, token, 32 - 1);					
			}

			token = COM_ParseExt(&ptoken, qfalse);
#ifdef _ALTVOTEMAP
			if(!Q_strncmp(token, "vstr", 4))
			{		
				token = COM_ParseExt(&ptoken, qfalse);
				if(!Q_strncmp(token, "map", 3))
					continue;
				if(!Q_strncmp(prevToken, "vote", 4))
				{
					continue;
				}			
				if(token[strlen(token)-1] == ';')
				{
					token[strlen(token)-1] = 0;
				}
				trap_Cvar_VariableStringBuffer ( token, maplist, MAX_STRING_CHARS );
				ptoken = maplist;	
				continue;
			}
#endif
			if(!strcmp(token, "map") || !strcmp(token, "devmap"))
			{
				token = COM_ParseExt(&ptoken, qtrue);
				if (!token || token[0] == 0) 
					continue;
				Q_strncpyz(mapList[i].map, token, MAX_QPATH - 1);
				if(mapList[i].map[strlen(mapList[i].map)-1] == ';')
				{
					mapList[i].map[strlen(mapList[i].map)-1] = '\0';
				}
				if(nextMapNum == i)
				{
					Q_strncpyz(level.nextMap, mapList[i].map, sizeof(level.nextMap) - 1);
				}
				maxMapCount++;
			}
			else if(!strcmp(token, "rmgmap") || !strcmp(token, "rmgdevmap"))
			{
				Q_strncpyz(mapList[i].map, "RMG", MAX_QPATH - 1);
				if(nextMapNum == i)
				{
					Q_strncpyz(level.nextMap, mapList[i].map, sizeof(level.nextMap) - 1);
				}
				maxMapCount++;
			}
			else if(!strcmp(token, "g_gametype"))
			{	
				token = COM_ParseExt(&ptoken, qtrue);
				if (!token || token[0] == 0) 
					continue;
				Q_strncpyz(mapList[i].gametype, token, 8 - 1);
				if(mapList[i].gametype[strlen(mapList[i].gametype)-1] == ';')
				{
					mapList[i].gametype[strlen(mapList[i].gametype)-1] = '\0';
				}
				if(nextMapNum == i)
				{
					Q_strncpyz(level.nextGameType, mapList[i].gametype, sizeof(level.nextGameType) - 1);
				}
			}	
		}			
	}	
	
	if(maxMapCount <= 1)
	{
		char	mapname[MAX_QPATH];
		trap_Cvar_VariableStringBuffer ( "mapname", mapname, MAX_QPATH );
		Q_strncpyz(level.nextMap, mapname, sizeof(level.nextMap) - 1);
		Q_strncpyz(level.nextGameType, nv_gtShort.string, sizeof(level.nextGameType) - 1);
	}
	else if(!level.nextMap[0] && maxMapCount > 1)
	{
		Q_strncpyz(level.nextMap, mapList[2].map, sizeof(level.nextMap) - 1);
		Q_strncpyz(level.nextGameType, mapList[2].gametype, sizeof(level.nextGameType) - 1);
	}	
	
	if(!level.nextMap[0])
	{		
		char	mapname[MAX_QPATH];
		trap_Cvar_VariableStringBuffer ( "mapname", mapname, MAX_QPATH );
		Q_strncpyz(level.nextMap, mapname, sizeof(level.nextMap) - 1);
		Q_strncpyz(level.nextGameType, nv_gtShort.string, sizeof(level.nextGameType) - 1);
	}


	if(maxMapCount > 2)
	{	
		i = 0;
		while (1)
		{
			level.randMapNum = Q_irand(1, maxMapCount);
			i++;
			if(i > 99) // dont do this forever
				break;
			if(level.randMapNum == nextMapNum)		// skip next
				continue;
			if(level.randMapNum == nextMapNum - 1)  // skip current
				continue;
			if(level.randMapNum == nextMapNum - 2)  // skip previous
				continue;
			break;
		}
	}
	//if(level.randMapNum <= 0)
	//{
	//	NV_getRandomMapList(restart);
	//}
}
void NV_printMapList (gentity_t *ent)
{
	int		i;	
	char	info[700] = "";


	if(ent != NULL)
	{
		CPe(va("print \"@^2-----^3Mapcycle^2-------\n\n"));
		for (i = 1; i <= maxMapCount; i++)
		{
			if(mapList[i].map && mapList[i].map[0])
			{
				//if(nextMapNum - 1 == i ||  nextMapNum == 1 && i == 1 )
				if(nextMapNum - 1 == i || (nextMapNum == 1 && maxMapCount == i) )
				{	
					char	mapname[MAX_QPATH] = "\0";
					trap_Cvar_VariableStringBuffer ( "mapname", mapname, MAX_QPATH );
					//Com_sprintf(info, sizeof(info), "%s^5[%2i] %-4s :  %s   <== CURRENT MAP^3\n", info, i, mapList[i].gametype, mapList[i].map);
					Com_sprintf(info, sizeof(info), "%s^5[%2i] %-4s :  %s   <== CURRENT MAP^3\n", info, i, nv_gtShort.string, mapname);				
				}	
				else
				{		
					Com_sprintf(info, sizeof(info), "^3%s[%2i] %-4s :  %s\n", info, i, mapList[i].gametype, mapList[i].map);
				}
				if(i % 5 == 1)
				{
					CPe(va("print \"@^3%s", info));
					memset(&info, 0, sizeof(info));
				}
			}
		}
		if(info[0])
			CPe(va("print \"@%s\n", info));
	}
	else
	{
		Com_Printf(S_COLOR_YELLOW"-----^3Mapcycle^2------\n");
		for (i = 1; i <= maxMapCount; i++)
		{
			if(mapList[i].map && mapList[i].map[0])
			{
				//if(nextMapNum - 1 == i || nextMapNum == 1 && i == 1 )
				if(nextMapNum - 1 == i || (nextMapNum == 1 && maxMapCount == i) )
				{		
					char	mapname[MAX_QPATH] = "\0";
					trap_Cvar_VariableStringBuffer ( "mapname", mapname, MAX_QPATH );
					//Com_sprintf(info, sizeof(info), "%s^5[%2i] %-4s :  %s   <== CURRENT^3\n", info, i, mapList[i].gametype, mapList[i].map);
					Com_sprintf(info, sizeof(info), "%s^5[%2i] %-4s :  %s   <== CURRENT MAP^3\n", info, i, nv_gtShort.string, mapname);
				}	
				else
				{		
					Com_sprintf(info, sizeof(info), "^3%s[%2i] %-4s :  %s\n", info, i, mapList[i].gametype, mapList[i].map);
				}
				if(i % 5 == 1)
				{
					Com_Printf(S_COLOR_YELLOW"%s",info);
					memset(&info, 0, sizeof(info));
				}
			}			
			else
			{
				break;
			}		
		}
		if(info[0])
			Com_Printf(S_COLOR_YELLOW"%s\n", info);
	}
}
#ifdef _CHAT_KEYWORDS

#define MAX_HUD_STRING  256
void NV_parseKeywordTokens ( gentity_t *ent, char *chatText )
{
	char		newText[MAX_SAY_TEXT] = "\0";
	char		*newTextp, *chatTextS;

	newTextp = newText;
	chatTextS = chatText;

	while(*chatText && newTextp < (newText + MAX_SAY_TEXT) )
	{
		switch (*chatText)
		{
		case '#':
			chatText++;
			switch(*chatText)
			{	
			case 'u':
			case 'U':
				if(ent->client){
					Com_sprintf(newText, MAX_SAY_TEXT,"%s%s^2", newText, ent->client->pers.netname );
					chatText++;
					break;
				}
				chatText--;	
				break;
			case 's':
			case 'S':
				Com_sprintf(newText, MAX_SAY_TEXT,"%s%s^2", newText, g_scorelimit.string );
				chatText++;
				break;
			case 't':
			case 'T':
				Com_sprintf(newText, MAX_SAY_TEXT,"%s%s^2", newText, g_timelimit.string );
				chatText++;
				break;
			case 'r':
			case 'R':
				Com_sprintf(newText, MAX_SAY_TEXT,"%s%s^2", newText, g_roundtimelimit.string );
				chatText++;	
				break;		
			case 'f':
			case 'F':
				Com_sprintf(newText, MAX_SAY_TEXT,"%s%s^2", newText, g_friendlyFire.string );
				chatText++;
				break;
			case 'a':
				Com_sprintf(newText, MAX_SAY_TEXT,"%s%s^2", newText, g_camperPunish.string );
				chatText++;
				break;		
			case 'A':
				Com_sprintf(newText, MAX_SAY_TEXT,"%s%s^2", newText, g_camperAllowTime.string );
				chatText++;
				break;			
			case 'm':
				// Current map
				if(ent->client)
				{
					char mapname[MAX_QPATH] = "\0";
					trap_Cvar_VariableStringBuffer ( "mapname", mapname, MAX_QPATH );
					Com_sprintf(newText, MAX_SAY_TEXT,"%s%s^2", newText, mapname );
					chatText++;
					break;
				}
				chatText--;	
				break;
			case 'M':  // Next map
				Com_sprintf(newText, MAX_SAY_TEXT,"%s%s^2", newText, level.nextMap );
				chatText++;
				break;
			case 'g': // Current Gametype
				Com_sprintf(newText, MAX_SAY_TEXT,"%s%s^2", newText, Q_strupr(nv_gtName.string) );
				chatText++;
				break;
			case 'G': // Next Gametype
				Com_sprintf(newText, MAX_SAY_TEXT,"%s%s^2", newText, Q_strupr(level.nextGameType) );
				chatText++;
				break;

			default:
				chatText--;
				break;

			}
		}
		while(*newTextp)
			newTextp++;

		if(*chatText)
			*newTextp++ = *chatText++;
	}
	*newTextp = '\0';

	chatText = chatTextS;
	Q_strncpyz (chatText, newText, MAX_SAY_TEXT);
	
}

nv_keywords_t		keyword[MAX_KEYWORDTOKENS];

void NV_ParseKeywordChats( char *filename ) 
{
	char	*token;
	int		count;
	char	key[MAX_TOKEN_CHARS];
	int	len;
	fileHandle_t f;
	char buf[14000];
	const char *ptoken = "";
	int responses, keywords;

	Com_Printf ("----------Loading Auto Responses--------\n");

	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if(!f){
		Com_Printf("ERROR: Response File not found: %s\n",filename);
		return;
	}

	trap_FS_Read( buf, len, f );
	if (len > 14000)
		len = 14000;
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	count = 0;
	ptoken = buf;

	while ( 1 ) 
	{
		token = COM_Parse( &ptoken );
		if ( !token[0] ) 
		{
			break;
		}
		token = COM_Parse( &ptoken );
		if ( strcmp( token, "{" ) ) 
		{
			Com_Printf( "Missing { in info file\n" );
			break;
		}

		if ( count == MAX_KEYWORDTOKENS ) 
		{
			Com_Printf( "Max infos exceeded\n" );
			break;
		}

		responses = 0;
		keywords = 0;
		while ( 1 ) 
		{
			token = COM_ParseExt( &ptoken, qtrue );
			if ( !token[0] ) 
			{
				Com_Printf( "Unexpected end of info file\n" );
				break;
			}
			if ( !strcmp( token, "}" ) ) 
			{
				break;
			}
			
			Q_strncpyz( key, token, sizeof( key ) );

			token = COM_ParseExt( &ptoken, qfalse );
			if ( !token[0] ) 
			{
				strcpy( token, "<NULL>" );
			}
			if(!strcmp(key, "keyword") && keywords < MAX_KEYWORDS )
			{
				Q_strncpyz(keyword[count].keyword[keywords++], va("%s ", token), 32);
				keyword[count].count[0]++;
			}
			//	Com_sprintf ( keyword[count].keyword[keywords++], 32, token );
			if(!strcmp(key, "response") && responses < MAX_RESPONSES )
			{
				Q_strncpyz( keyword[count].reponse[responses++], token, MAX_SAY_TEXT - 1 );
				keyword[count].count[1]++;
			}
			if(!strcmp(key, "importance"))
				keyword[count].level = atoi(token);
			if(!strcmp(key, "clientOnly"))
			{
				if(atoi(token) >= 1)
					keyword[count].clientOnly = qtrue;
			}
			//Info_SetValueForKey( infos[count], key, token );
		}
		count++;
	}
}
void NV_ServerKeyWords(gentity_t *ent, char *chatText)
{
	int i, j;
	int worst = -1;

	if (!ent || ent == NULL)
		return;

	Q_strlwr(chatText);
	Q_CleanStr(chatText);

	for (i = 0; i < MAX_KEYWORDTOKENS; i++)
	{
		for (j = 0; j < keyword[i].count[0]; j++)
		{
			if (keyword[i].keyword[j][0] && strstr(chatText, keyword[i].keyword[j]))
			{
				//if (!strcmp(keyword[i].keyword[j], "lag") && strstr(chatText, "flag"))
				//	continue;
				if (worst == -1)
					worst = i;
				if (keyword[i].level >= keyword[worst].level)
					worst = i;
			}
		}
	}

	if (worst == -1) 
		return;

	if(keyword[worst].lastTime > level.time - level.startTime )
	{
		trap_SendServerCommand(-1, va("chat -1 \"^2Server: ^7***^3Please dont abuse the server ^_%s ^3or you will be punished^7***\n\"", ent->client->pers.netname));
		if(g_enableChatReponses.integer > 1)
			ent->client->nvSess.chatAbuse++;
		return;
	}

	if(g_enableChatReponses.integer > 1)
	{
		switch (ent->client->nvSess.chatAbuse)
		{
		default:
		case 0:
		case 1:
		case 3:
		case 4:
			/* just skip it for now and continue code but add some abuse if they already have some*/
			if(ent->client->nvSess.chatAbuse > 3 )
				ent->client->nvSess.chatAbuse++; 
			break;
		case 5:
			trap_SendServerCommand(-1, va("chat -1 \"^2Server: ^7Im warning you ^3%s^7!\n\"", ent->client->pers.netname));
			ent->client->nvSess.chatAbuse++;
			return;
		case 6:
			trap_SendServerCommand(-1, va("chat -1 \"^2Server: ^7You are really testing my patience ^3%s\n\"", ent->client->pers.netname));
			ent->client->nvSess.chatAbuse++;
			return;
		case 7:
			ent->client->nvSess.chatAbuse++;
			if(!G_IsClientDead(ent->client))
			{	
				trap_SendServerCommand(-1, va("chat -1 \"^2Server: That does it ^3%s^7, time to play!\n\"", ent->client->pers.netname));
				trap_SendConsoleCommand( EXEC_INSERT, va("runover %i\n", ent->s.number ) );
			}
			else
			{
				trap_SendServerCommand(-1, va("chat -1 \"^2Server: ^7Yeah, just keep running your mouth kid.\n\""));
				trap_SendConsoleCommand( EXEC_INSERT, va("forcesay %i \"I think I better shut up now, before the server gets angry and ass rapes me again!\"\n", ent->s.number ) );
			}
			return;
		case 8:
			ent->client->nvSess.chatAbuse++;
			if(!G_IsClientDead(ent->client))
			{
				trap_SendServerCommand(-1, va("chat -1 \"^2Server: ^7So you want more? I have more!\n\""));
				trap_SendConsoleCommand( EXEC_INSERT, va("strip %i\n", ent->s.number ) );
			}
			else
			{		
				trap_SendServerCommand(-1, va("chat -1 \"^2Server: ^7You're just making yourself look stupid.  I'd advise you to keep your comments to yourself.\n\""));
			}
			return;
		case 9:
			ent->client->nvSess.chatAbuse++;
			if(!G_IsClientDead(ent->client))
			{
				trap_SendServerCommand(-1, va("chat -1 \"^2Server: ^7I can do this all day long\n\""));
				trap_SendConsoleCommand( EXEC_INSERT, va("slam %i\n", ent->s.number ) );
			}
			else
			{
				trap_SendServerCommand(-1, va("chat -1 \"^2Server: ^7Im really going to have to ask you to shut the hell up already!\n\""));
			}
			return;
		case 10:
			trap_SendServerCommand(-1, va("chat -1 \"^2Server: ^7Ok, Im starting to get bored of you now*\n\""));
			ent->client->nvSess.chatAbuse++;
			if(!G_IsClientDead(ent->client))
				trap_SendConsoleCommand( EXEC_INSERT, va("pop %i\n", ent->s.number ) );
			return;	
		case 11:
			trap_SendServerCommand(-1, va("chat -1 \"^2Server: ^7STFU already %s^7!\n\"", ent->client->pers.netname));
			ent->client->nvSess.chatAbuse++;
			trap_SendConsoleCommand( EXEC_INSERT, va("mute %i\n", ent->s.number ) );
			return;	
		case 12:
			trap_SendServerCommand(-1, va("chat -1 \"^2Server: ^7Haven't we been down this road before %s^7!\n\"", ent->client->pers.netname));
			trap_SendConsoleCommand( EXEC_INSERT, va("mute %i\n", ent->s.number ) );
			return;	
		}
	}

	//j = rand()%keyword[worst].count[1];
	//if(!keyword[worst].reponse[j][0])
	//	j=0;
	if(keyword[worst].count[2] > 5 || !keyword[worst].reponse[keyword[worst].count[2]][0])
	{
		keyword[worst].count[2] = 0;
	}
	j = keyword[worst].count[2]++;
	
	if(keyword[worst].reponse[j])
	{
		char text[MAX_SAY_TEXT] = "\0";
		Q_strncpyz( text, keyword[worst].reponse[j], MAX_SAY_TEXT - 1 );
		NV_parseKeywordTokens(ent, text );
		if(keyword[worst].clientOnly)
			trap_SendServerCommand(ent-g_entities, va("chat -1 \"%s\n\"", text));
		else 
		{
			trap_SendServerCommand(-1, va("chat -1 \"%s\n\"", text ));
		}
	}
	
	/// Execute a specific function ... good for spamming muli-lines such as rules or even punishments :P
//	if(keyWord[worst].funcName)
//	{
//		keyWord[worst].funcName(1, NULL, ent->s.number, keyWord[worst].lastTime);
//	}
	if(keyword[worst].level == 0)
		keyword[worst].lastTime = level.time - level.startTime + 5000;
	else
		ent->client->nvSess.chatAbuse++;	
}

#else

void NV_ServerKeyWords(gentity_t *ent, char *chatText)
{
	if (!ent || ent == NULL)
		return;

	if ((stristr(chatText, "nextmap")) || (stristr(chatText, "!next")))
	{
		if (level.doRandom && level.randMapNum && level.randGameType[0] && level.randMap[0])
			trap_SendServerCommand(-1, va("chat -1 \"^7server: ^3*^2Next map is: ^1%s ^2- ^S%s^3*\"", level.randMap, Q_strupr(level.randGameType)));
		else
			trap_SendServerCommand(-1, va("chat -1 \"^7server: ^3*^2Next map is: ^1%s ^2- ^S%s^3*\"", level.nextMap, Q_strupr(level.nextGameType)));
		return;
	}

	if (stristr(chatText, "scorelimit") || stristr(chatText, "timelimit") || stristr(chatText, "!sl") || stristr(chatText, "!tl"))
	{
		trap_SendServerCommand(-1, va("chat -1 \"^7server: ^5scorelimit: ^3%i   ^5timelimit: ^3%i\"", g_scorelimit.integer, g_timelimit.integer + level.timeExtension));
		return;
	}
#ifdef _DEBUG
	if (stristr(chatText, "!help"))
	{
		//	//RPM_Motd(ent);
		NV_sendGametypeHelp(ent, qtrue);
		trap_SendServerCommand(ent->s.number, va("chat -1 \"^7server: %s^2, Open the console (press ^3~^2) to see the gametype Help information\"", ent->client->pers.netname));
		return;
	}
#endif
	if (stristr(chatText, "!rules"))
	{
		static int lastTime = 0;

		if (lastTime > level.time)
			return;
		lastTime = level.time + 10000;
		if (g_rules1.string[1])
		{
			//trap_SendServerCommand( -1, va("chat -1 \"%s\"", g_rules1.string ));
			NV_Sound(-1, "sound/movers/buttons/button02.mp3");
			trap_SendConsoleCommand(EXEC_INSERT, va("say %s\n", g_rules1.string));
		}
		if (g_rules2.string[1])
			//trap_SendServerCommand( -1, va("chat -1 \"%s\"", g_rules2.string ));
			trap_SendConsoleCommand(EXEC_INSERT, va("say %s\n", g_rules2.string));
		return;
	}
}
#endif //_CHAT_KEYWORDS

#ifdef _SPECIAL_NADES

void NV_checkGas ( gentity_t *ent )
{
	if(ent->client->nvSess.gasOwner < 0 || ent->client->nvSess.gasOwner >= MAX_CLIENTS )
	{
		return;
	}

	if(ent->client->ps.pm_flags & PMF_GOGGLES_ON)
	{
		ent->client->nvSess.gasTime--;
		return;
	}
	else
	{
		//vec3_t viewAngle;
		//SetClientViewAngle(ent, viewAngle);
	}
	switch (ent->client->nvSess.gasTime)
	{
		case 0:
			ent->client->nvSess.gasTime = 0;
			ent->client->nvSess.gasOwner = -1;
			return;
		case 9: // First
		{		
			int cough;
			cough = G_SoundIndex("sound/misc/models/cough04");
			G_EntitySound( ent, 0, cough);
			G_Damage (ent, NULL, &g_entities[ent->client->nvSess.gasOwner], NULL, NULL, ent->client->ps.stats[STAT_HEALTH] * .05, DAMAGE_NO_ARMOR|DAMAGE_NO_KNOCKBACK|DAMAGE_NO_GORE, MOD_F1_GRENADE, HL_NONE );
			//G_Damage (ent, NULL, &g_entities[ent->client->nvSess.gasOwner], NULL, NULL, 10, DAMAGE_NO_ARMOR|DAMAGE_NO_KNOCKBACK|DAMAGE_NO_GORE, MOD_F1_GRENADE, HL_NONE );
			ent->client->nvSess.gasTime--;
			return;
		}		
		case 6:
		{		
			int cough;
			cough = G_SoundIndex("sound/misc/models/cough03");
			G_EntitySound( ent, 0, cough);
			G_Damage (ent, NULL, &g_entities[ent->client->nvSess.gasOwner], NULL, NULL, ent->client->ps.stats[STAT_HEALTH] * .10, DAMAGE_NO_ARMOR|DAMAGE_NO_KNOCKBACK|DAMAGE_NO_GORE, MOD_F1_GRENADE, HL_NONE );
			//G_Damage (ent, NULL, &g_entities[ent->client->nvSess.gasOwner], NULL, NULL, 10, DAMAGE_NO_ARMOR|DAMAGE_NO_KNOCKBACK|DAMAGE_NO_GORE, MOD_F1_GRENADE, HL_NONE );
			ent->client->nvSess.gasTime--;
			return;
		}	
		case 3:
		{		
			int cough;
			cough = G_SoundIndex("sound/misc/models/cough03");
			G_EntitySound( ent, 0, cough);
			G_Damage (ent, NULL, &g_entities[ent->client->nvSess.gasOwner], NULL, NULL, ent->client->ps.stats[STAT_HEALTH] * .20, DAMAGE_NO_ARMOR|DAMAGE_NO_KNOCKBACK|DAMAGE_NO_GORE, MOD_F1_GRENADE, HL_NONE );
			ent->client->nvSess.gasTime--;
			return;
		}	
		//case 2:
		//{
		//	G_Damage (ent, NULL, &g_entities[ent->client->nvSess.gasOwner], NULL, NULL, , 0, MOD_F1_GRENADE, HL_NONE );
		//	ent->client->nvSess.gasTime--;
		//	return;
		//}
		case 1: // KILL
		{		
			int cough;
			cough = G_SoundIndex("sound/misc/models/cough02");
			G_EntitySound( ent, 0, cough);
			//G_Damage (ent, NULL, &g_entities[ent->client->nvSess.gasOwner], NULL, NULL, ent->client->ps.stats[STAT_HEALTH] * .10, 0, MOD_F1_GRENADE, HL_NONE );
			//G_Damage (ent, NULL, &g_entities[ent->client->nvSess.gasOwner], NULL, NULL, ent->client->ps.stats[STAT_HEALTH] + 10, DAMAGE_NO_ARMOR, DAMAGE_NO_ARMOR|DAMAGE_NO_KNOCKBACK|DAMAGE_NO_GORE, HL_NONE );
			G_Damage (ent, NULL, &g_entities[ent->client->nvSess.gasOwner], NULL, NULL, 25, DAMAGE_NO_ARMOR, DAMAGE_NO_ARMOR|DAMAGE_NO_KNOCKBACK|DAMAGE_NO_GORE, HL_NONE );
			ent->client->nvSess.gasTime--;
			return;
		}
		default:
		{	
			ent->client->nvSess.gasTime--;
			return;
		}
	}	
}
void NV_Touch_Gas ( gentity_t *ent, gentity_t *other, trace_t *trace )
{
	if(!other || !other->client)
		return;
	if(other->client->nvSess.gasTime > 4)
		return;
	other->client->nvSess.gasTime = 9;
	other->client->nvSess.gasOwner = ent->parent->s.number;
}
void NV_GAS_THINK ( gentity_t *ent ) 
{

	ent->s.time2--;

	if ( ent->s.time2 <= 0 )
	{	
		G_FreeEntity ( ent );
		return;
	}

	ent->nextthink = level.time + 350;
	trap_LinkEntity( ent );
}
void NV_Spawn_Gas ( gentity_t *gas, gentity_t *owner)
{
	gentity_t	*damageArea;

	damageArea = G_Spawn();
	
	damageArea->nextthink = level.time + 350;
	damageArea->think = NV_GAS_THINK;
	damageArea->s.eType = ET_GENERAL;
	damageArea->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	damageArea->parent = owner;
	damageArea->r.ownerNum = owner->s.number;

	damageArea->touch = NV_Touch_Gas;
	//damageArea->s.eType=ET_MOVER;
	//damageArea->clipmask = CONTENTS_SOLID;


	VectorSet (damageArea->r.mins, -150, -150, -150);
	VectorSet (damageArea->r.maxs, 150, 150, 150);
	damageArea->r.contents = CONTENTS_TRIGGER;

	damageArea->s.pos.trType = TR_STATIONARY;
	damageArea->s.pos.trTime = level.time;
	damageArea->s.time2 = 27;
	damageArea->target_ent = NULL;

	damageArea->classname = "GasArea";

	//VectorSet( damageArea->r.maxs, 1, 1, 1 );
	//VectorScale( damageArea->r.maxs, -1, damageArea->r.mins );

	VectorCopy( gas->r.currentOrigin, damageArea->s.pos.trBase );
	VectorCopy( gas->r.currentOrigin, damageArea->r.currentOrigin);
	SnapVector( damageArea->r.currentOrigin  );

}

#endif //_SPECIAL_NADES

#ifdef _KILLSPREE

nv_spreeRewards_t	spreeRewards[MAX_KILLSPREE_REWARDS];
void NV_parseKillSpreeTokens ( gentity_t *ent, char *chatText, int spree )
{
	char		newText[MAX_SAY_TEXT] = "\0";
	char		*newTextp, *chatTextS;

	newTextp = newText;
	chatTextS = chatText;

	while(*chatText && newTextp < (newText + MAX_SAY_TEXT) )
	{
		switch (*chatText)
		{
		case '#':
			chatText++;
			switch(*chatText)
			{	
			case 'u':
			case 'U':
			case 'n':
			case 'N':
				if(ent->client)
				{
					Com_sprintf(newText, MAX_SAY_TEXT,"%s%s", newText, ent->client->pers.netname );
					chatText++;
					break;
				}
			case 'k':
			case 'K':
				Com_sprintf(newText, MAX_SAY_TEXT,"%s%i", newText, spree );
				chatText++;
				break;	
			case 'r':
			case 'R':			
				if (ent->client)
				{
					Com_sprintf(newText, MAX_SAY_TEXT, "%s%i", newText, ent->client->pers.rank);
					chatText++;
				}
				break;
			case 't':
			case 'T':
				if(ent->client)
				{
					if(ent->client->sess.team == TEAM_RED)
					{
						Com_sprintf(newText, MAX_SAY_TEXT,"%s%s", newText, "^7the ^4Blue ^7team" );
						chatText++;
						break;
					}			
					if(ent->client->sess.team == TEAM_BLUE)
					{
						Com_sprintf(newText, MAX_SAY_TEXT,"%s%s", newText, "^7the ^1Red ^7team" );
						chatText++;
						break;
					}			
					if(ent->client->sess.team == TEAM_FREE)
					{
						Com_sprintf(newText, MAX_SAY_TEXT,"%s^1Everyone", newText );
						chatText++;
						break;
					}
				}
			default:
				chatText--;
				break;

			}
		}
		while(*newTextp)
			newTextp++;

		if(*chatText)
			*newTextp++ = *chatText++;
	}
	*newTextp = '\0';

	chatText = chatTextS;
	Q_strncpyz (chatText, newText, MAX_SAY_TEXT);	
}
void NV_ParseKillSpreeFile( char *filename ) 
{
	char	*token;
	int		count;
	char	key[MAX_TOKEN_CHARS];

	int	len;
	fileHandle_t f;
	char buf[14000];
	const char *ptoken = "";
	
	//level.killSpreeFile = qfalse;

	Com_Printf ("----------Loading Kill Spree File --------\n");

	len = trap_FS_FOpenFile( filename, &f, FS_READ );

	if(!f){
		Com_Printf("ERROR: Kill Spree File not found: %s\n", filename);
		return;
	}

	trap_FS_Read( buf, len, f );
	if (len > 14000)
		len = 14000;
	buf[len] = 0;
	trap_FS_FCloseFile( f );

	count = 0;

	ptoken = buf;
	while ( 1 ) 
	{
		token = COM_Parse( &ptoken );
		if ( !token[0] ) 
		{
			break;
		}
		token = COM_Parse( &ptoken );
		if ( strcmp( token, "{" ) ) 
		{
			Com_Printf( "Missing { in info file\n" );
			break;
		}
		if ( count == MAX_KILLSPREE_REWARDS ) 
		{
			Com_Printf( "Max infos exceeded\n" );
			break;
		}

		spreeRewards[count].extraclipsAlt = -1;
		spreeRewards[count].extraclips = -1;

		while ( 1 ) 
		{
			token = COM_ParseExt( &ptoken, qtrue );
			if ( !token[0] )
			{
				Com_Printf( "Unexpected end of info file\n" );
				break;
			}
			if ( !strcmp( token, "}" ) ) 
			{
				break;
			}	

			Q_strncpyz( key, token, sizeof( key ) );
			token = COM_ParseExt( &ptoken, qfalse );
			if ( !token[0] ) 
			{
				continue;
			}			
			if(!strcmp(key, "kill"))
			{
				spreeRewards[count].killsNeeded = atoi(token);
				if(spreeRewards[count].killsNeeded >= level.topSpreeReward)
					level.topSpreeReward = spreeRewards[count].killsNeeded;
				continue;
			}			
			if(!strcmp(key, "GlobalMsg")  )
			{
				Q_strncpyz( spreeRewards[count].globalMsg, token, MAX_SAY_TEXT - 1 );
				continue;
			}	
			if(!strcmp(key, "CenterMsg")  )
			{
				Q_strncpyz( spreeRewards[count].centerMsg, token, MAX_SAY_TEXT - 1 );
				continue;
			}
			if(!strcmp(key, "ConsoleMsg"))
			{
				Q_strncpyz( spreeRewards[count].consoleMsg, token, MAX_SAY_TEXT - 1 );
				continue;
			}		
			if(!strcmp(key, "sound"))
			{
				Q_strncpyz( spreeRewards[count].sound, token, MAX_QPATH - 1 );
				continue;
			}	
			if(!strcmp(key, "reward"))
			{
				spreeRewards[count].reward = atoi(token);
				continue;
			}				
			if(!strcmp(key, "clipsize"))
			{
				spreeRewards[count].clipsize = atoi(token);
				continue;
			}
			if(!strcmp(key, "extraclips"))
			{
				spreeRewards[count].extraclips = atoi(token);
				continue;
			}							
			if(!strcmp(key, "clipsizeAlt"))
			{
				spreeRewards[count].clipsizeAlt = atoi(token);
				continue;
			}
			if(!strcmp(key, "extraclipsAlt"))
			{
				spreeRewards[count].extraclipsAlt = atoi(token);
				continue;
			}

		}
		//level.killSpreeFile = qtrue;
		count++;
	}

	if(NV_isGG())
	{
		level.topSpreeReward = count;
	}
}

#define REWARD_HEALTH 100
#define REWARD_AMMO   101
void NV_KillSpreeSound(int clientNum, char *file )
{
	gentity_t *tent;

	if(!strcmp(file, "") || !file[0])
		return;

	if(g_entities[clientNum].client->nvSess.timeLastKSsound < level.time)
	{
		//char	*s;
		//char	userinfo[MAX_INFO_STRING];
		//
		//trap_GetUserinfo( clientNum , userinfo, sizeof( userinfo ) );
		//s = Info_ValueForKey (userinfo, "nvc_disableSounds");
		//if(s[0] == '2' || s[0] == '3')
		//{
		//	return;
		//}

		///tent = G_TempEntity( g_entities[clientNum].client->ps.origin, EV_CHAT_SOUND);
		tent = G_TempEntity( g_entities[clientNum].client->ps.origin, EV_GLOBAL_SOUND);

		tent->r.singleClient = clientNum;
		tent->r.svFlags |= SVF_SINGLECLIENT;
		tent->s.eventParm = G_SoundIndex(file);
		g_entities[clientNum].client->nvSess.timeLastKSsound = level.time + 1000;
	}
}

void NV_killSpreeNotify (gentity_t *ent, int spree, nv_spreeRewards_t info)
{
	if(info.globalMsg[0])
	{	
		char text[MAX_SAY_TEXT] = "\0";
		Q_strncpyz( text, info.globalMsg, MAX_SAY_TEXT - 1 );
		NV_parseKillSpreeTokens(ent, text, spree  );
		trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, %s", level.time + 5000, text));
	}
	if(info.consoleMsg[0])
	{		
		char text[MAX_SAY_TEXT] = "\0";
		Q_strncpyz( text, info.consoleMsg, MAX_SAY_TEXT - 1 );
		NV_parseKillSpreeTokens(ent, text, spree  );
		CPx(-1, va("print \"%s\n", text));
	}
	if(info.centerMsg[0])
	{		
		char text[MAX_SAY_TEXT] = "\0";
		Q_strncpyz( text, info.centerMsg, MAX_SAY_TEXT - 1 );
		NV_parseKillSpreeTokens(ent, text, spree );
		CPe(va("cp \"%s\n\"", text));
	}
	if(info.sound[2])
	{
		NV_KillSpreeSound(ent->s.number, info.sound );
	}
}
	

#ifdef _SPREEREWARD
void NV_ClearReward(gentity_t *ent)
{
	memset ( ent->client->nvSess.killSpreeItem, 0, sizeof(ent->client->nvSess.killSpreeItem) );
	memset ( ent->client->nvSess.killSpreeClip, 0, sizeof(ent->client->nvSess.killSpreeClip) );
	memset ( ent->client->nvSess.killSpreeExtraClips, 0, sizeof(ent->client->nvSess.killSpreeExtraClips) );
}

void NV_GiveRewardSpawn(gentity_t *ent)
{
	int i;
	int w = 0;
	playerState_t	*ps;

	ps = &ent->client->ps;
	for(i=0; i < MAX_KILLSPREE_REWARDS; i++)
	{	
		if(	ent->client->nvSess.killSpreeItem[i] > WP_NONE && ent->client->nvSess.killSpreeItem[i] < WP_NUM_WEAPONS)
		{
			attackType_t a;
			ps->stats[STAT_WEAPONS] |= (1<<	ent->client->nvSess.killSpreeItem[i]);	

			for ( a = ATTACK_NORMAL; a < ATTACK_MAX; a++ )
			{
				ent->client->ps.clip[a][ent->client->nvSess.killSpreeItem[i]] = weaponData[w].attack[a].clipSize;
				ent->client->ps.ammo[weaponData[ent->client->nvSess.killSpreeItem[i]].attack[a].ammoIndex] = ammoData[weaponData[w].attack[a].ammoIndex].max;
			}
		}	
	}
}

#endif // _SPREEREWARDS

static qboolean NV_SpreeReward(gentity_t *ent, int spree, qboolean earned)
{
	int				i;
	int				spreeLoop;
	playerState_t	*ps;

	ps = &ent->client->ps;
	
	// Kill spree rewards disabled or limited
#ifdef _COMPMODE
	if(g_compMode.integer || g_showKillStreak.integer < 2)
#else
	if(g_showKillStreak.integer < 2)
#endif
		return qfalse;

#ifdef _DB_ENABLED
	if(spree == 3)
	{
		ent->client->pers.statinfo.killSpreeCount++;
	}
#endif

	// Lets me loop back so we can keep giving them shit if they pass the max killspree number
	if(level.topSpreeReward && level.topSpreeReward < spree)
	{
		spreeLoop = (spree % level.topSpreeReward) + 1;
	}
	else
	{
		spreeLoop = spree;
	}

	if(!g_enableKillSpreeReward.integer || !level.topSpreeReward)
	{
		return qfalse;	
	}
	for(i=0; i < MAX_KILLSPREE_REWARDS; i++)
	{	
		if(!spreeRewards[i].killsNeeded)
			continue;
		if(spreeRewards[i].killsNeeded != spreeLoop)
			continue;

		// The 1st Kill spree message.  Only do this once!
		if(spreeRewards[i].reward == -1)
		{
			if(spree != spreeRewards[i].killsNeeded)
				continue;
			NV_killSpreeNotify(ent, spree, spreeRewards[i]);
			return qtrue;
		}

		// Give Health
		if(spreeRewards[i].reward == REWARD_HEALTH)
		{
			ps->stats[STAT_HEALTH] = MAX_HEALTH;
			ent->health = ps->stats[STAT_HEALTH];
			if ( !ps->stats[STAT_GOGGLES] || g_Armor.integer)
			{
				ps->stats[STAT_REALARMOR] = MAX_ARMOR;
			}
			NV_killSpreeNotify(ent, spree, spreeRewards[i]);
			return qtrue;
		}	

		// Give Ammo		
		if(spreeRewards[i].reward == REWARD_AMMO)
		{
			for ( i = 0; i < MAX_AMMO; i ++ )
			{
				int	maxammo;
				maxammo = BG_GetMaxAmmo ( ps, i);
				if ( !maxammo || ps->ammo[i] >= maxammo )
					continue;
				ps->ammo[i] += Com_Clamp ( 1, maxammo, maxammo );
				if ( ps->ammo[i] >= maxammo )
					ps->ammo[i] = maxammo;
			}
			NV_killSpreeNotify(ent, spree, spreeRewards[i]);
			return qtrue;
		}
		// Give a weapon based on reward
		if(spreeRewards[i].reward > WP_NONE && spreeRewards[i].reward < WP_NUM_WEAPONS)
		{
			int w = spreeRewards[i].reward;
			attackType_t a;
			ps->stats[STAT_WEAPONS] |= (1<<w);	

#ifdef _120314_2
			ent->client->nvSess.earnedWeapons |= (1<<w);	
#endif

#ifndef _SPREEREWARD			
			for ( a = ATTACK_NORMAL; a < ATTACK_MAX; a++ )
			{
				ent->client->ps.clip[a][w] = weaponData[w].attack[a].clipSize;
				ent->client->ps.ammo[weaponData[w].attack[a].ammoIndex] = ammoData[weaponData[w].attack[a].ammoIndex].max;
				//ent->client->nvSess.killSpreeItems[i][a + 1] = ent->client->ps.ammo[weaponData[w].attack[a].ammoIndex];
			}
#else
			ent->client->nvSess.killSpreeItem[i] = spreeRewards[i].reward;

			if(spreeRewards[i].clipsize)
			{
				ent->client->ps.clip[ATTACK_NORMAL][w] += spreeRewards[i].clipsize;
				ent->client->nvSess.killSpreeClip[ATTACK_NORMAL][w] = spreeRewards[i].clipsize;
			}
			else
			{
				ent->client->ps.clip[ATTACK_NORMAL][w] += weaponData[w].attack[ATTACK_NORMAL].clipSize;
				ent->client->nvSess.killSpreeClip[ATTACK_NORMAL][w] =  weaponData[w].attack[ATTACK_NORMAL].clipSize;
			}

			if(spreeRewards[i].extraclips != -1)
			{
				ent->client->ps.ammo[weaponData[w].attack[ATTACK_NORMAL].ammoIndex] += spreeRewards[i].extraclips * weaponData[i].attack[ATTACK_NORMAL].clipSize;
				ent->client->nvSess.killSpreeExtraClips[ATTACK_NORMAL][w] = spreeRewards[i].extraclips * weaponData[i].attack[ATTACK_NORMAL].clipSize;
			}
			else
			{
				ent->client->ps.ammo[weaponData[w].attack[ATTACK_NORMAL].ammoIndex] += weaponData[i].attack[ATTACK_NORMAL].extraClips * weaponData[i].attack[ATTACK_NORMAL].clipSize;
				ent->client->nvSess.killSpreeExtraClips[ATTACK_NORMAL][w] = weaponData[i].attack[ATTACK_NORMAL].extraClips * weaponData[i].attack[ATTACK_NORMAL].clipSize;
			}
			// Alt-Attack		
			if(spreeRewards[i].clipsizeAlt)
			{
				ent->client->ps.clip[ATTACK_ALTERNATE][w] += spreeRewards[i].clipsizeAlt;
				ent->client->nvSess.killSpreeClip[ATTACK_ALTERNATE][w] = spreeRewards[i].clipsize;
			}
			else
			{
				ent->client->ps.clip[ATTACK_ALTERNATE][w] += weaponData[w].attack[ATTACK_ALTERNATE].clipSize;
				ent->client->nvSess.killSpreeClip[ATTACK_ALTERNATE][w] =  weaponData[w].attack[ATTACK_ALTERNATE].clipSize;
			}

			if(spreeRewards[i].extraclipsAlt != -1)
			{
				ent->client->ps.ammo[weaponData[w].attack[ATTACK_ALTERNATE].ammoIndex] += spreeRewards[i].extraclipsAlt * weaponData[i].attack[ATTACK_ALTERNATE].clipSize;
				ent->client->nvSess.killSpreeExtraClips[ATTACK_ALTERNATE][w] = spreeRewards[i].extraclips * weaponData[i].attack[ATTACK_ALTERNATE].clipSize;
			}
			else
			{
				ent->client->ps.ammo[weaponData[w].attack[ATTACK_ALTERNATE].ammoIndex] += weaponData[i].attack[ATTACK_ALTERNATE].extraClips * weaponData[i].attack[ATTACK_ALTERNATE].clipSize;
				ent->client->nvSess.killSpreeExtraClips[ATTACK_ALTERNATE][w] = weaponData[i].attack[ATTACK_ALTERNATE].extraClips * weaponData[i].attack[ATTACK_ALTERNATE].clipSize;
			}
#endif // _SPREEREWARD

			NV_killSpreeNotify(ent, spree, spreeRewards[i]);
			return qtrue;
		}	
		//if(!spreeRewards[i].reward || spreeRewards[i].reward == -2)
		//{
		//	NV_killSpreeNotify(ent, spree, spreeRewards[i]);
		//	return qtrue;
		//}
	}

	return qfalse;
}
qboolean NV_checkKillSpree(gentity_t *killer, gentity_t *target, int type)
{
	int		spree = killer->client->pers.statinfo.killsinarow;

#ifdef _DB_ENABLED
	if(spree >= killer->client->pers.statinfo.killSpreeRecord)
		killer->client->pers.statinfo.killSpreeRecord = spree;
#endif
	
#ifdef _COMPMODE
	if(g_compMode.integer || g_showKillStreak.integer < 2)
#else
	if(g_showKillStreak.integer < 2)
#endif

		return qfalse;

	switch(type)
	{
	case 1: ///team killer
		killer->client->pers.statinfo.killsinarow = 0;
		GT(va("%i,^3%s's ^7killing ^3Spree ^7was ended for ^3TEAM KILLING", level.time + 5000, killer->client->pers.netname));
		return qtrue;
	case 2:
		GT(va("%i,^3%s ^7has ended ^3%s's ^7killing ^3Spree^7",  level.time + 5000, killer->client->pers.netname, target->client->pers.netname));
		return qtrue;
	default:
		return NV_SpreeReward(killer, spree, qfalse);
	}

}

#endif // _KILLSPREE

void NV_GunGameGiveWeapon(gentity_t *ent)
{
	int		i;
	//int		ammoIndex;
	int		idle;
	//int		oldWeap = ent->client->ps.weapon;
	attackType_t a;

	ent->client->ps.stats[STAT_WEAPONS] = 0;
	memset ( ent->client->ps.ammo, 0, sizeof(ent->client->ps.ammo) );
	memset ( ent->client->ps.clip, 0, sizeof(ent->client->ps.clip) );
/*
	ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
	ammoIndex=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].ammoIndex;
	ent->client->ps.ammo[ammoIndex]=ammoData[ammoIndex].max;
	ent->client->ps.clip[ATTACK_NORMAL][WP_KNIFE]=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
	ent->client->ps.firemode[WP_KNIFE] = BG_FindFireMode ( WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO );
*/	
	i = ent->client->pers.rank;
//#ifdef _DEBUG
//	ent->client->pers.currentWeapon = WP_KNIFE;
//#else
	if(!spreeRewards[i].reward 	|| 
		spreeRewards[i].reward <= WP_NONE || 
		spreeRewards[i].reward >= WP_NUM_WEAPONS)
	{
		return;
	}

	ent->client->ps.stats[STAT_WEAPONS] |= (1 << spreeRewards[i].reward);
	ent->client->pers.currentWeapon = spreeRewards[i].reward;

	//ammoIndex = weaponData[item->giTag].attack[ATTACK_NORMAL].ammoIndex;
	//client->ps.ammo[ammoIndex] += weaponData[item->giTag].attack[ATTACK_NORMAL].extraClips * weaponData[item->giTag].attack[ATTACK_NORMAL].clipSize;
	//client->ps.clip[ATTACK_NORMAL][item->giTag] = weaponData[item->giTag].attack[ATTACK_NORMAL].clipSize;


	for ( a = ATTACK_NORMAL; a < ATTACK_MAX; a++ )
	{
		ent->client->ps.clip[a][spreeRewards[i].reward] = weaponData[spreeRewards[i].reward].attack[a].clipSize;
		//ent->client->ps.ammo[weaponData[spreeRewards[i].reward].attack[a].ammoIndex] = ammoData[weaponData[spreeRewards[i].reward].attack[a].ammoIndex].max;
		ent->client->ps.ammo[weaponData[spreeRewards[i].reward].attack[a].ammoIndex] = (ammoData[weaponData[spreeRewards[i].reward].attack[a].ammoIndex].max + 1 ) * 5;
	}
	if ( ent->client->ps.firemode[spreeRewards[i].reward] == WP_FIREMODE_NONE )
	{
		ent->client->ps.firemode[spreeRewards[i].reward] = BG_FindFireMode ( (weapon_t)spreeRewards[i].reward, ATTACK_NORMAL, WP_FIREMODE_AUTO );
	}
//#endif
	ent->client->ps.zoomFov	  = 0;
	ent->client->ps.zoomTime  = 0;
	ent->client->ps.pm_flags &= ~(PMF_ZOOM_FLAGS);
		
	ent->client->ps.weapon = ent->client->pers.currentWeapon;
	ent->client->ps.weaponstate = WEAPON_RAISING; 
 	ent->client->ps.weaponTime = 500;
	ent->client->ps.weaponAnimTime = 500;
	
	BG_GetInviewAnim(ent->client->ps.weapon,"ready",&idle);

	ent->client->ps.weaponAnimId = idle;
	ent->client->ps.weaponAnimIdChoice = 0;
	ent->client->ps.weaponCallbackStep = 0;

#ifdef _SOF2_BOTS
	if ( ent->r.svFlags & SVF_BOT )
	{
		ent->client->ps.weaponstate = WEAPON_RAISING;
	}
#endif

}
void NV_GunGameDemote (gentity_t* ent, gentity_t* attacker)
{
	if (!ent || ent==NULL )
		return;

	if(ent->client->pers.rank >= 1)
	{
		ent->client->pers.rank--;
	}
	else
	{
		return;
	}

	if(attacker != NULL && attacker != ent)
	{
		//trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^7%s ^2was ^1Humiliated ^2by ^7%s", level.time + 5000, ent->client->pers.netname, attacker->client->pers.netname));
		trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^3%s ^+was ^1destroyed ^+by ^3%s", level.time + 5000, ent->client->pers.netname, attacker->client->pers.netname));
		NV_KillSpreeSound(ent->s.number, "sound/npc/civ/english/male/mygod.mp3" );
		return;
	}

	//trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^7%s ^2has ^1Humiliated ^7%sself", level.time + 5000, ent->client->pers.netname,  strstr ( ent->client->pers.identity->mCharacter->mModel, "female" )?"Her":"Him"));
	trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^3%s ^+has ^1Humiliated ^+%sself", level.time + 5000, ent->client->pers.netname,  strstr ( ent->client->pers.identity->mCharacter->mModel, "female" )?"Her":"Him"));
	NV_KillSpreeSound(ent->s.number, "sound/npc/civ/english/fmale/mygod.mp3" );
}
void NV_GunGamePromote (gentity_t* ent, gentity_t* victim, int weapon)
{
	int i = ent->client->pers.rank;

	//if(weapon == WP_KNIFE && victim != NULL)
	//{
	//	NV_GunGameDemote (victim, ent);
	//}
	if(ent == victim) // nothing for yourself !
		return;

	if(spreeRewards[i].reward && spreeRewards[i].reward != weapon)
	{
		return;	
	}

	if(ent->client->pers.rank >= level.topSpreeReward)
	{
		return;
	}

	ent->client->pers.rank++;
	NV_GunGameGiveWeapon(ent);
		
	i = ent->client->pers.rank;

	if(!spreeRewards[i].reward 	|| 
		spreeRewards[i].reward <= WP_NONE || 
		spreeRewards[i].reward >= WP_NUM_WEAPONS)
	{
		return;
	}
	NV_killSpreeNotify(ent, ent->client->pers.rank, spreeRewards[ent->client->pers.rank]);
}


#ifdef _DB_ENABLED

void NV_SQLAdminLog(gentity_t *adm, gentity_t *ent, char* cmd)
{
	int client = -1, admin = -1;

	if (!g_enableDatabase.integer)
		return;
	if (ent && ent->client)
		client = ent->client->nvSess.dbID;
	if (adm && adm->client)
		admin = adm->client->nvSess.dbID;

	trap_SendConsoleCommand(DB_LITE_ADMINLOG, va("%i;%i;%s;", admin, client, cmd));
}
#endif
#ifndef _DB_ENABLED

//void NV_ClientStats(void)
//{
//	gentity_t		*ent;
//	char	arg[8] = "\0";
//	int		num = -1;
//
//	if (!g_enableDatabase.integer)
//		return;
//
//	trap_Argv(1, arg, sizeof(arg));
//
//	if (arg[0] >= '0' && arg[0] <= '9')
//		num = atoi(arg);
//	else
//		return;
//
//	if (num < 0 || num >= g_maxclients.integer)
//		return;
//
//	ent = g_entities + num;
//	ent->client->nvSess.dbID = atoi(Info_ValueForKey(ConcatArgs(2), "u"));
//	ent->client->nvSess.dbnameID = atoi(Info_ValueForKey(ConcatArgs(2), "n"));
//	if (g_enableDatabase.integer == 2)
//	{
//		ent->client->nvSess.admin = atoi(Info_ValueForKey(ConcatArgs(2), "a"));
//		ent->client->nvSess.scrim = (qboolean)atoi(Info_ValueForKey(ConcatArgs(2), "s"));
//		ent->client->nvSess.ban = atoi(Info_ValueForKey(ConcatArgs(2), "b"));
//		Com_sprintf(ent->client->nvSess.reason, 50, "%s", Info_ValueForKey(ConcatArgs(2), "r"));
//	}
//
//#ifdef _DEBUG
//
//	if (!level.warmupTime)
//		NV_Log(5, va("[%-2i]%-24s - %s", num, ent->client->nvSess.cleanName, ConcatArgs(2)));
//
//	if (ent->client->nvSess.dbnameID == 0 || ent->client->nvSess.dbID == 0)
//	{
//		Com_Printf("\n%s - %s\n", ent->client->nvSess.cleanName, ConcatArgs(2));
//	}
//#endif
//
//}
#endif

void NV_loadSettingFiles ( void )
{	
	Com_Printf ("---------- Loading NV Config Files ---------\n");
#ifdef _CHAT_KEYWORDS
	NV_ParseKeywordChats ("settings/autoresponse.cfg");
#endif //_CHAT_KEYWORDS
	
#ifdef _KILLSPREE
		if(NV_isGG())
			NV_ParseKillSpreeFile ("settings//gungame.cfg");
		else
			NV_ParseKillSpreeFile (g_killSpreeFile.string);
#endif
	Com_Printf ("----------Finished Loading Config Files ---------\n");
}

char *stristr(char *str, char *charset) {
	int i;

	while(*str) {
		for (i = 0; charset[i] && str[i]; i++) {
			if (toupper(charset[i]) != toupper(str[i])) break;
		}
		if (!charset[i]) return str;
		str++;
	}
	return NULL;
}


void NV_callVoteSkipNextMap (void)
{
	if (level.randMapNum <= 0)
	{
		NV_getRandomMapList(qfalse);
	}

	//if(level.randMapNum && level.nextMap[0] && level.nextGameType[0] && mapList[level.randMapNum].map[0] && mapList[level.randMapNum].gametype[0])
	if(level.randMapNum && level.nextMap[0] && level.nextGameType[0] && level.randMap[0] && level.randGameType[0])
	{
		int i;

		trap_SendServerCommand( -1, "print \"Auto vote: Pick Next Map\n\"");
		Com_sprintf ( level.voteString, sizeof(level.voteString ), "setRandom");
	//	Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), " ^2What should we play next?\n\n\n\n\n ^3F1 - ^7YES: ^3%s ^d%s^2\n ^3F2 - ^7NO: ^1%s ^d%s ^2(mapcycle)", level.randMap, level.randGameType, level.nextMap, level.nextGameType);
		Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), " ^2What should we play next?\n\n\n\n\n(^5F1^2)^7 YES: ^3%s ^d%s^2\n(^5F2^2)^7 ^7NO: ^1%s ^d%s ^2(mapcycle)", level.randMap, level.randGameType, level.nextMap, level.nextGameType);
	//	Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), " ^2What should we play next?\n\n\n\n\n ^7F1 (Y): ^3%s ^d%s^2\n ^7F2 (N): ^1%s ^d%s ^2(mapcycle)", level.randMap, level.randGameType, level.nextMap, level.nextGameType);
	//	Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), " ^2What should we play next?\n\n\n\n\n ^7F1: ^3%s ^d%s^2\n ^7F2: ^1%s ^d%s ^2(mapcycle)", level.randMap, level.randGameType, level.nextMap, level.nextGameType);
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
	
#ifdef _MAPVOTE_MOST
		trap_SetConfigstring( CS_VOTE_NEEDED, 0);
#else
		trap_SetConfigstring( CS_VOTE_NEEDED, va("%i", level.numVotingClients / 2) );
#endif

#ifdef _DB_ENABLED // Vote called sound
	RPM_GlobalSound(G_SoundIndex("sound/movers/buttons/button02.mp3"));
#endif

	}
}

#ifdef _NEW_MAP_SETUP
void NV_voteSkipNextMap(void)
{
	if(level.randomVoteCalled == qtrue)
		return;
	if((level.time - level.startTime >= ((g_timelimit.integer * 60000) *0.80f)))
	{
		NV_callVoteSkipNextMap();
		level.randomVoteCalled = qtrue;
		return;
	}
	if ( g_scorelimit.integer && !level.warmupTime) 
	{	
		int score = (g_scorelimit.integer * 0.80f);
		if ( level.gametypeData->teams )
		{
			if ( level.teamScores[TEAM_RED] >= score ) 
			{
				NV_callVoteSkipNextMap();
				level.randomVoteCalled = qtrue;
				return;		
			}
			if ( level.teamScores[TEAM_BLUE] >= score ) 
			{
				NV_callVoteSkipNextMap();
				level.randomVoteCalled = qtrue;
				return;			
			}
		}
		else
		{
			int i;
			gclient_t	*cl;

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

				if ( cl->sess.score >= score) 
				{
					NV_callVoteSkipNextMap();
					level.randomVoteCalled = qtrue;
					return;	
				}
			}
		}
	}
}
void NV_voteNextMap (void)
{
		int i;
		trap_SendServerCommand( -1, va("print \"Auto vote: next map\n\"") );
		Com_sprintf ( level.voteString, sizeof(level.voteString ), "mapcycle");
		Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), "Next Map (%s %s)", level.nextMap, level.nextGameType);
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

#endif
#endif

#ifdef _CLEAR_DROPPED
void NV_clearDroppedItems (void)
{	
	int			i, missile, freed, total, players, bodies;
	gentity_t	*e;
	char	mapname[MAX_QPATH];

	e = NULL;	// shut up warning
	i = 0;		// shut up warning
	missile = 0;
	freed = 0;
	total = 0;
	players = 0;
	bodies = 0;
	
	trap_Cvar_VariableStringBuffer ( "mapname", mapname, MAX_QPATH );
	//NV_Log (2, va("------------------ %s - %s ------------------------\n", mapname, nv_gtShort.string)) ;
	
	//e = &g_entities[MAX_CLIENTS];
	//for ( i = MAX_CLIENTS ; i<level.num_entities ; i++, e++) 
	e = &g_entities[0];
	for ( i = 0 ; i<level.num_entities ; i++, e++) 
	{
		if (!e->inuse ) 
		{
			continue;
		}
		total++;
		
		if ( (e->flags & FL_DROPPED_ITEM) && e->item && e->item->giType == IT_WEAPON)
		{	
			//Com_Printf("Freeing Dropped Item: %4i: %s\n", i, e->classname);
			//NV_Log (2, va("Freeing Dropped Item: [%4i] %s", i,  e->classname)) ;
			G_FreeEntity ( e );
			freed++;
		}		
		else if ( ( e->s.eType == ET_MISSILE ) )
		{	
			missile++;
		}	
		else if ( ( e->s.eType == ET_PLAYER ) )
		{	
			players++;
		}	
		else if ( ( e->s.eType == ET_BODY ) )
		{	
			bodies++;
		}
	}

	NV_Log (2, va("\n"
		"------------------\n"
		"Map: %s    Gametype: %s\n"
		"Total Entities before: %i  --  After: %i\n"
		"Freed: %i\n"
		"Missiles: %i\n"
		"Players: %i\n"
		"Bodies: %i\n"
		"------------------------\n", 
		mapname, nv_gtShort.string,
		total, total -  freed,  
		freed, 
		missile, 
		players, 
		bodies
		)) ;
		
	Com_Printf("\n"
		"------------------\n"
		"Map: %s    Gametype: %s\n"
		"Total Entities before: %i  --  After: %i\n"
		"Freed: %i\n"
		"Missiles: %i\n"
		"Players: %i\n"
		"Bodies: %i\n"
		"------------------------\n", 
		mapname, nv_gtShort.string,
		total, total -  freed,  
		freed, 
		missile, 
		players, 
		bodies
		) ;


}
#endif // 120317

#ifdef _EFFECTS

typedef struct effects_s
{
	char effect[MAX_QPATH];
	int	 id;
} effects_t;

#define	MAX_EFFECTS	700

effects_t efx[MAX_EFFECTS];

static int efxID = 0;
static int maxEfx = 0;

qboolean efxReg = qfalse;
void NV_getEffectsList (void)
{
	char		filelist[MAX_QPATH * 400];
	char		*fileptr;
	int			filelen;
	int			filecount;
	int			i, j = 0;
	unsigned long fileIP = 0;

	filecount = trap_FS_GetFileList("", ".efx", filelist, sizeof(filelist) );
	fileptr   = filelist;
	
	filelen = strlen(fileptr);
	for ( i = 0; i < filecount; i++, fileptr += filelen+1) 
	{	
		filelen = strlen(fileptr);

		//s  = strchr ( fileptr, '.' );
		//*s = '\0';

		if(filelen <= 2)
			continue;
		Q_strncpyz(efx[j].effect, fileptr, MAX_QPATH);

		Com_Printf("[%3i]%s\n", j,  efx[j].effect  );
		j++;
	}
	maxEfx = j;
	efxReg = qtrue;
	efxID = 600;
}

void NV_effectsSend(gentity_t *ent)
{
	const char* info;
		
	if(!efxReg)
		NV_getEffectsList();
	info = va("efx %s %.2f %.2f %.2f", efx[efxID].effect, ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2] + (ent->client->ps.viewheight * 1.25));
	trap_SendServerCommand( ent->s.number, info);
	Com_Printf("[%3i]%s\n\n", efxID,  efx[efxID].effect  );
}

void NV_effectsInc(gentity_t *ent)
{	
	if(efxID >= maxEfx)
		efxID = -1;
	efxID++;
	NV_effectsSend(ent);
}
void NV_effectsDec(gentity_t *ent)
{
	if(efxID <= 1)
		efxID = maxEfx;
	efxID--;
	NV_effectsSend(ent);
}
#endif

team_t NV_PickTeam( int ignoreClientNum, team_t TEAM_WANTED, team_t TEAM_CURRENT )
{
	int		counts[TEAM_NUM_TEAMS];

	if(TEAM_WANTED == TEAM_SPECTATOR)
	{
		return TEAM_SPECTATOR;
	}
	if(level.extras.redLocked && level.extras.blueLocked)
	{
		return TEAM_CURRENT;
	}
	if(level.extras.redLocked  && !level.extras.blueLocked)
	{
		return TEAM_BLUE;
	}
	if(level.extras.blueLocked  && !level.extras.redLocked )
	{
		return TEAM_RED;
	}

	counts[TEAM_BLUE] = TeamCount( ignoreClientNum, TEAM_BLUE, NULL );
	counts[TEAM_RED] = TeamCount( ignoreClientNum, TEAM_RED, NULL );

	if ( TEAM_WANTED == TEAM_BLUE && counts[TEAM_BLUE] > counts[TEAM_RED] )
	{
		trap_SendServerCommand( ignoreClientNum, "cp \"Blue team has too many players.\n\"" );
		return TEAM_RED;
	}
	if ( TEAM_WANTED == TEAM_RED && counts[TEAM_RED] > counts[TEAM_BLUE] )
	{	
		trap_SendServerCommand( ignoreClientNum, "cp \"Red team has too many players.\n\"" );
		return TEAM_BLUE;
	}
	// equal team count, so join the team with the lowest score
	if ( TEAM_WANTED == TEAM_RED && level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE] )
	{	
		trap_SendServerCommand( ignoreClientNum, "cp \"Red team is Winning.\n\"" );
		return TEAM_BLUE;
	}
	if (TEAM_WANTED == TEAM_BLUE && level.teamScores[TEAM_BLUE] > level.teamScores[TEAM_RED] )
	{
		trap_SendServerCommand( ignoreClientNum, "cp \"Blue team is Winning.\n\"" );
		return TEAM_RED;
	}
	return TEAM_WANTED;
}


void BG_ApplyLeanOffset ( playerState_t* ps, vec3_t origin )
{
	float	leanOffset;
	vec3_t	up;
	vec3_t	right;

	leanOffset = (float)(ps->leanTime - LEAN_TIME) / LEAN_TIME * LEAN_OFFSET;
	AngleVectors( ps->viewangles, NULL, right, up);
	VectorMA( origin, leanOffset, right, origin );
	VectorMA( origin, Q_fabs(leanOffset) * -0.20f, up, origin );
}


void NV_Print_File (gentity_t *ent, char *file)
{
	int		i = 0, len = 0, total = 0;
	char	buffer[14000] = "";
	char	listName[128] = "";
	char	*listP = listName;
	char	*bufchar = buffer;
	char	str[1024];
	char	*text;

	NV_LoadFile( file, buffer, qfalse );
	
	// Doing this to get total lines
	if(strlen(buffer) > 8000)
	{
		while( *bufchar)
		{
			if(*bufchar == '\n')
			{
				SkipRestOfLine( (const char **) &bufchar );
				continue;
			}
			while(*bufchar != '\n' && *bufchar != '\0')
				bufchar++;
			total++;
		}

		bufchar = buffer;
		if(total >= 500)
		{
			total = 350;
		}
	}

	// Now do it and send to the requester
	while( *bufchar)
	{
		if(*bufchar == '\n')
		{
			SkipRestOfLine( (const char **) &bufchar );
			continue;
		}
		while(*bufchar != '\n' && *bufchar != '\0')
			*listP++ = *bufchar++;

		*listP = '\0';
		listP = listName;

		i++;
		if(i < total)
		{
			continue;
		}
		text = va("[%i] %s\n", i, listP);
		
		strncpy( &str[len], text, 128 );
		len += strlen( text );
		if ( len > 750 )
		{
			str[len++] = 0;
			if(ent != NULL)
			{
				CPe(va("print \"@%s\"", str));
			}
			else
			{
				Com_Printf("@%s\n", str);
			}
			strncpy( str,"", 128 );
			len = 0;
		}
		SkipRestOfLine( (const char **) &bufchar );
	}
	if (len)
	{
		str[len++] = 0;
		if(ent != NULL)
		{
			CPe(va("print \"@%s\"", str));
		}
		else
		{
			Com_Printf("@%s\n", str);
		}
	}
}

#ifdef _FAIR_WEAPON_STATS
qboolean NV_WeaponStatsOk(void)
{
	if(g_ForcedItems.integer || g_onlyWeapons.integer)
		return qfalse;
	return qtrue;
}
#endif



void NV_setHudIcon(gentity_t *ent, qboolean touching)
{
	// nv mod will be seen as 0.213 for 2.13, etc.. just for this purpose so we dont show the capturing nonsense, so skip it if its nv client
	if(ent->client->sess.rpmClient && ent->client->sess.rpmClient < 0.4f) 
		return;
	ent->s.generic1 = 0;
	if(!touching)
	{
		if(ent->client->nvSess.vip == 1)
		{
			ent->s.generic1 =  G_IconIndex("gfx/menus/misc/gold_star.png");
			ent->client->ps.generic1 =  G_IconIndex("gfx/menus/misc/gold_star.png");

		}
		//else if (/*kill spree weapons?*/) { }
		//else if (/*holding pickups or items */) { }
		else 
		{	
			//ent->s.generic1 =  G_IconIndex("gfx/menus/weapon_renders/rpg7");
			//ent->client->ps.generic1 =  G_IconIndex("gfx/menus/weapon_renders/rpg7");
		}
		
		if(ent->s.generic1 != 0)
		{
			ent->client->ps.pm_flags |= PMF_CAN_USE;
			ent->client->ps.stats[STAT_USETIME_MAX] = 8121;
		}
	}
	else
	{
		if(NV_isSab())
		{
			ent->s.generic1 =  G_IconIndex("gfx/menus/hud/c4");
			ent->client->ps.generic1 =  G_IconIndex("gfx/menus/hud/c4");
		}
		else if(NV_isDom())
		{		
			ent->s.generic1 =  G_IconIndex("gfx/menus/hud/info_walpha");
			ent->client->ps.generic1 =  G_IconIndex("gfx/menus/hud/info_walpha");
		}
	}

}



#ifdef _DB_ENABLED
#ifdef _DB_WEBCMDS

// For the admin backend on the forums.  Handing out admin, bans, ranks, titles, etc.. via the website.
void NV_WebAdmin (void)
{	
	char		 playerID[6];
	char		 status[2];
	int			 idnum = -1;
	//char update[64] = "\0";


	if(!g_enableDatabase.integer)
		return;
	trap_Argv( 1, playerID, sizeof(playerID));
	if (playerID[0] >= '0' && playerID[0] <= '9')	
		idnum = atoi( playerID );
	else 
		return;
	trap_Argv(2, status, sizeof(status));

	//Com_sprintf(update, sizeof(update), "%i;%i;", playerID, status);
	//NV_QueryDB(DB_LITE_ADDWEBADMIN, update);	
	//Com_sprintf(id2, sizeof(id2), "Granted %s", type);
	//NV_Finish (adm, ent, id2, FINISH_LOG|FINISH_ADMIN_ONLY);

}
#endif
#endif





