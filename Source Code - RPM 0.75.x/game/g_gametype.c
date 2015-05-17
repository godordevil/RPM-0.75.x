// Copyright (C) 2001-2002 Raven Software
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
// g_gametype.c -- dynamic gametype handling

#include "g_local.h"
#include "q_shared.h"
#include "g_team.h"
#include "inv.h"
// KRIS 16/04/2003 12:38AM

#ifdef _CRS_BOTS
#include "./crs/zg_waypoint.h"
extern botGametypeItemInfo_t bot_GTInfo[MAX_GAMETYPE_ITEMS];
#endif



// KRIS
#define	MAX_GAMETYPE_SPAWN_POINTS	32
// GodOrDevil - 01.18.06 - 07:34pm #logging
///CJJ - 1.1.2005 - Logging Flag Captures
//#define TRIGGER_REDFLAGCAPTURE      200
//#define TRIGGER_BLUEFLAGCAPTURE      201
//#define TRIGGER_CASECAPTURE      200
///CJJ - 1.1.2005 #END

int			g_gametypeItemCount = 0;
vec3_t		g_effectOrigin;

// GodOrDevil - GAMETYPES
static qboolean redNotify = qfalse;
static qboolean blueNotify = qfalse;
// END

/*QUAKED gametype_player (0 1 0) (-16 -16 -46) (16 16 48) REDTEAM BLUETEAM
Potential spawning position for red or blue team in custom gametype games.
*/
void SP_gametype_player ( gentity_t *ent ) 
{
	team_t	team;

	// Cant take any more spawns!!
	if ( level.spawnCount >= MAX_SPAWNS )
	{
		G_FreeEntity ( ent );
		return;
	}
	
	// If a team filter is set then override any team settings for the spawns
	if ( level.mTeamFilter[0] )
	{
		if ( Q_stricmp ( level.mTeamFilter, "red") == 0 )
		{
			team = TEAM_RED;
		}
		else if ( Q_stricmp ( level.mTeamFilter, "blue") == 0 )
		{
			team = TEAM_BLUE;
		}
		else
		{
			G_FreeEntity ( ent );
			return;
		}
	}
	else
	{
		// Red team only
		if ( ent->spawnflags & 0x1 )
		{
			team = TEAM_RED;
		}
		else if ( ent->spawnflags & 0x2 )
		{
			team = TEAM_BLUE;
		}
		else
		{
			team = TEAM_FREE;
		}
	}

	G_AddClientSpawn ( ent, team );

	G_FreeEntity ( ent );
}

void SP_mission_player ( gentity_t* ent )
{
	ent->classname = "gametype_player";

	SP_gametype_player ( ent );
}

// GodOrDevil 
/*
void gametype_trigger_use ( gentity_t *self, gentity_t *other, gentity_t *activator ) 
{
	if ( level.gametypeResetTime )
	{
		return;
	}
}

void gametype_trigger_touch ( gentity_t *self, gentity_t *other, trace_t *trace ) 
{
	if ( level.gametypeResetTime )
	{
		return;
	}

	if ( trap_GT_SendEvent ( GTEV_TRIGGER_TOUCHED, level.time, self->health, other->s.number, other->client->sess.team, 0, 0 ) )
	{
		G_UseTargets ( self, other );
	}
}

void SP_gametype_trigger ( gentity_t* ent )
{
	// The target name is only used by the gametype system, so it shouldnt
	// be adjusted by the RMG
	if ( level.mTargetAdjust )
	{
		if ( ent->targetname )
			ent->targetname = strchr ( ent->targetname, '-' ) + 1;
	}
	// GodOrDevil - 09.30.06 - 08:17pm #spMaps
	if(!Q_stricmp(ent->model, "NV_MODEL")){
		if (!VectorCompare (ent->s.angles, vec3_origin))
			G_SetMovedir (ent->s.angles, ent->movedir);
		ent->r.contents = CONTENTS_TRIGGER;
		ent->r.svFlags = SVF_NOCLIENT;
		ent->s.eType = ET_GRAPPLE;
		return;
	}
	///End  - 09.30.06 - 08:18pm

	InitTrigger (ent);
	ent->s.eType = ET_GRAPPLE;
}



static gentity_t* G_RealSpawnGametypeItem ( gentity_t* ent, qboolean dropped )
{
	gentity_t* it_ent;

	it_ent = G_Spawn();

	// KRIS 14/04/2003 5:19PM
	// for bot AI
	if (dropped)
	{
		it_ent->flags |= FL_DROPPED_ITEM;
	}
	// KRIS
	it_ent->item = ent->item;

	VectorCopy( ent->r.currentOrigin, it_ent->s.origin );
	VectorCopy ( ent->s.angles, it_ent->s.apos.trBase );
	VectorCopy ( ent->s.angles, it_ent->s.angles ); 
	it_ent->classname = ent->item->classname;
	G_SpawnItem ( it_ent, it_ent->item );
	FinishSpawningItem(it_ent);	
	
	VectorSet( it_ent->r.mins, -ITEM_RADIUS * 4 / 3, -ITEM_RADIUS * 4 / 3, -ITEM_RADIUS );
	VectorSet( it_ent->r.maxs, ITEM_RADIUS * 4 / 3, ITEM_RADIUS * 4 / 3, ITEM_RADIUS );

	// Red team only
	if ( ent->s.eFlags & EF_REDTEAM )
	{
		it_ent->s.eFlags |= EF_REDTEAM;
	}
	
	if ( ent->s.eFlags & EF_BLUETEAM )
	{
		it_ent->s.eFlags |= EF_BLUETEAM;
	}

	return it_ent;
}
gentity_t* G_SpawnGametypeItem ( const char* pickup_name, qboolean dropped )
{
	gentity_t* ent;

	// Look for the gametype item in the map
	ent = NULL;
	while ( NULL != (ent = G_Find ( ent, FOFS(classname), "gametype_item" ) ) )
	{
		// Match?
		if ( !Q_stricmp ( ent->item->pickup_name, pickup_name ) )
		{
			break;
		}
	}
	
	// If we couldnt find the item spawner then we have a problem	
	if ( !ent )
	{
		Com_Error ( ERR_FATAL, "Could not spawn gametype item '%s'\n", pickup_name );
		return NULL;
	}		

	return G_RealSpawnGametypeItem ( ent, dropped );
}

void G_GametypeItemThink ( gentity_t* ent )
{
	G_RealSpawnGametypeItem ( ent, qfalse );
}

void SP_gametype_item ( gentity_t* ent )
{
	// TEMPORARY HACK
	if ( level.mTargetAdjust )
	{
		if ( ent->targetname )
			ent->targetname = strchr ( ent->targetname, '-' ) + 1;
	
		if ( ent->target )
			ent->target = strchr ( ent->target, '-' ) + 1;
	}

	G_SetOrigin( ent, ent->s.origin );
}

*/

void gametype_trigger_use ( gentity_t *self, gentity_t *other, gentity_t *activator ) 
{
	if ( level.gametypeResetTime )
	{
		return;
	} 
	if ( trap_GT_SendEvent ( GTEV_TRIGGER_USED, level.time, self->health, other->s.number, other->client->sess.team, 0, 0 ) )
	{
		G_UseTargets ( self, other );
	}
	else if(NV_isDem() || NV_isSab() || NV_isRaze())
	{
		gitem_t* item;			

		//trap_GT_SendEvent ( GTEV_SETORIGIN, level.time, (int)other->client->ps.origin, 0, 0, 0, 0  );
		// Take items away
		item = BG_FindGametypeItemByID ( 100 );
		if ( item )	
		{	
			gentity_t* ent = LaunchItem ( item, other->client->ps.origin, vec3_origin );
			other->client->ps.stats[STAT_GAMETYPE_ITEMS] &= ~(1<<item->giTag);
			VectorCopy ( other->client->ps.viewangles, ent->s.angles );
			ent->chain = self; // link the item with the trigger
			self->armTime = gt_bombTimer.integer; // set the armTime just so we can mark the floating icons as needing to be updated
			//G_ClearGameTriggerConfigString();
			//G_SetGameTriggerConfigString();
		}
	}
}

void gametype_item_use ( gentity_t* self, gentity_t* other )
{
	if ( level.gametypeResetTime )
	{
		return;
	}

	if ( trap_GT_SendEvent ( GTEV_ITEM_USED, level.time, self->item->quantity, other->s.number, other->client->sess.team, 0, 0 ) )
	{
		G_UseTargets ( self, other );
		//NV_checkTriggerIcons(1); // 1 = defused
		self->armTime = 0;
		if(self->chain)
			self->chain->armTime = 0;
		//G_ClearGameTriggerConfigString();
		//G_SetGameTriggerConfigString();
	}
}

void gametype_trigger_touch ( gentity_t *self, gentity_t *other, trace_t *trace ) 
{
	if ( level.gametypeResetTime )
	{
		return;
	}
		
#ifdef _MAPEDITOR
	if(nv_editMap.integer > 1)
	{
		return;
	}
#endif

	if ( trap_GT_SendEvent ( GTEV_TRIGGER_TOUCHED, level.time, self->health, other->s.number, other->client->sess.team, 0, 0 ) )
	{
		G_UseTargets ( self, other );
	}
}

void SP_gametype_trigger ( gentity_t* ent )
{
	// The target name is only used by the gametype system, so it shouldnt
	// be adjusted by the RMG
	if ( level.mTargetAdjust )
	{
		if ( ent->targetname )
			ent->targetname = strchr ( ent->targetname, '-' ) + 1;
	}

	// GodOrDevil - 09.30.06 - 08:17pm #spMaps	
	// Need target names on these or the bombeffect will get FUCKED UP!
	if(!ent->target && (NV_isDem() || NV_isSab() || NV_isRaze() || NV_isRush()))
	{
		G_FreeEntity( ent );
		return;
	}
	if(!Q_stricmp(ent->model, "NV_MODEL"))
	{
		if (!VectorCompare (ent->s.angles, vec3_origin))
			G_SetMovedir (ent->s.angles, ent->movedir);
		ent->r.contents = CONTENTS_TRIGGER;
		ent->r.svFlags = SVF_NOCLIENT;
		ent->s.eType = ET_GRAPPLE;
		if(NV_isRush())
		{
			if(ent->spawnflags >=1 && ent->spawnflags < MAX_PHASES)
			{
				level.extras.phaseTargets[ent->spawnflags]++;
				NV_Spawn_Target_Models (ent ); 
			}
		}
		else if(NV_isDem() || NV_isSab() || NV_isRaze())
		{
			NV_spawnBombEffect(ent);
			NV_spawnFlagMarker(ent);
		}
		return;
	}
	///End  - 09.30.06 - 08:18pm

	InitTrigger (ent);

	ent->s.eType = ET_GRAPPLE;
}


/*static*/ gentity_t* G_RealSpawnGametypeItem ( gitem_t* item, vec3_t origin, vec3_t angles, qboolean dropped )
{
	gentity_t* it_ent;

	it_ent = G_Spawn();

#ifdef _CRS_BOTS
	// KRIS 14/04/2003 5:19PM
	// for bot AI
	if (dropped)
	{
		it_ent->flags |= FL_DROPPED_ITEM;
	}
	// KRIS
#endif

	it_ent->item = item;

	VectorCopy( origin, it_ent->s.origin );
	VectorCopy ( angles, it_ent->s.apos.trBase );
	VectorCopy ( angles, it_ent->s.angles );
	// GodOrDevil - 27.06.09 - set GTItem angles - using 1.01 G_RealSpawnGametypeItem, and they had angles defined, but not ent angles. lets see how it goes
	//VectorCopy ( ent->s.angles, it_ent->s.angles );
	//END
	it_ent->classname = item->classname;
	G_SpawnItem ( it_ent, it_ent->item );
	FinishSpawningItem(it_ent);	
	
	VectorSet( it_ent->r.mins, -ITEM_RADIUS * 4 / 3, -ITEM_RADIUS * 4 / 3, -ITEM_RADIUS );
	VectorSet( it_ent->r.maxs, ITEM_RADIUS * 4 / 3, ITEM_RADIUS * 4 / 3, ITEM_RADIUS );

	return it_ent;
}

// GodOrDevil - 1.01 code
gentity_t* G_SpawnGametypeItem ( const char* pickup_name, qboolean dropped, vec3_t origin )
//gentity_t* G_SpawnGametypeItem ( const char* pickup_name, qboolean dropped )
// END
{
	gentity_t* ent;

	// GodOrDevil - #dem 1.01 code: so items dont have to be in the ent file if they are registered in the gt file
	if ( dropped )
	{
		gitem_t* item = BG_FindItem ( pickup_name );
		if ( item )
		{
			return G_RealSpawnGametypeItem ( item, origin, vec3_origin, dropped );
		}

		return NULL;
	}
	//END
	// Look for the gametype item in the map
	ent = NULL;
	while ( NULL != (ent = G_Find ( ent, FOFS(classname), "gametype_item" ) ) )
	{
		// Match?
		if ( !Q_stricmp ( ent->item->pickup_name, pickup_name ) )
		{
			break;
		}
	}
	
	// If we couldnt find the item spawner then we have a problem	
	if ( !ent )
	{
		Com_Error ( ERR_FATAL, "Could not spawn gametype item '%s'\n", pickup_name );
		return NULL;
	}		

	// GodOrDevil - 1.01 code
	//return G_RealSpawnGametypeItem ( ent, dropped );
	return G_RealSpawnGametypeItem ( ent->item, ent->r.currentOrigin, ent->s.angles, dropped );
	// END
}

void G_GametypeItemThink ( gentity_t* ent )
{
	// GodOrDevil - 1.01 code
	//G_RealSpawnGametypeItem ( ent, qfalse );
	G_RealSpawnGametypeItem ( ent->item, ent->r.currentOrigin, ent->s.angles, qfalse );
	// END

}

void SP_gametype_item ( gentity_t* ent )
{	
	// GodOrDevil - GAMETYPES #DEM

	if(NV_isDem() || NV_isVIP())
	{	
		trap_UnlinkEntity( ent );
		G_FreeEntity( ent );
		return;
	}
	// END
	// TEMPORARY HACK
	if ( level.mTargetAdjust )
	{
		if ( ent->targetname )
			ent->targetname = strchr ( ent->targetname, '-' ) + 1;
	
		if ( ent->target )
			ent->target = strchr ( ent->target, '-' ) + 1;
	}
	// GodOrDevil #dom - keep track of total dom flags
		level.extras.domMaxFlags++;	
	///end

	VectorSet( ent->r.mins, -5, -5, -10 );
	VectorSet( ent->r.maxs, 5, 5, 10);
	G_SetOrigin( ent, ent->s.origin );

#ifdef _OVERPOWER
	if (NV_isOP() && ent->targetname)
	{	
		if (!strcmp(ent->targetname, "briefcase")) // marks starting flag
		{
			VectorCopy(ent->r.currentOrigin, level.extras.neutralFlagOrigin);
			VectorCopy(ent->r.currentAngles, level.extras.neutralFlagAngles);
			strcpy(ent->targetname, "blue_flag");
			return;
		}
		else if(!strcmp(ent->targetname, "red_flag"))
		{
			VectorCopy(ent->r.currentOrigin, level.extras.redFlagOrigin);
			VectorCopy(ent->r.currentAngles, level.extras.redFlagAngles);
		}
		else if(!strcmp(ent->targetname, "blue_flag"))
		{
			VectorCopy(ent->r.currentOrigin, level.extras.blueFlagOrigin);
			VectorCopy(ent->r.currentAngles, level.extras.blueFlagAngles);
		}
		trap_UnlinkEntity( ent );
		G_FreeEntity( ent );
	}
#endif

}

// END



/*
===============
G_ResetGametypeItem
===============
*/
void G_ResetGametypeItem ( gitem_t* item )
{
	gentity_t *find;
	int		  i;

	// Convience check
	if ( !item )
	{
		return;
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

	// Respawn the item in all of its locations
	find = NULL;
	while ( NULL != (find = G_Find ( find, FOFS(classname), "gametype_item" ) ) )
	{
		if ( find->item != item )
		{
			continue;
		}

		// GodOrDevil - 1.01 code
		//G_RealSpawnGametypeItem ( find, qfalse );
		G_RealSpawnGametypeItem ( find->item, find->r.currentOrigin, find->s.angles, qfalse );
				// End	
	}
}

/*
===============
G_ResetGametypeEntities
===============
*/
void G_ResetGametypeEntities ( void )
{
	gentity_t* find;
	qboolean   initVisible;

	// Show total time
	initVisible = qtrue;
	trap_SetConfigstring ( CS_GAMETYPE_TIMER, "0" );

	// Reset all of the gametype items.  This must be done last because the spawn
	// function may alter enabled states of triggers or scripts
	find = NULL;
	while ( NULL != (find = G_Find ( find, FOFS(classname), "gametype_item" ) ) )
	{
		G_ResetGametypeItem ( find->item );
	}
}

/*
===============
G_RespawnClients
===============
*/
void G_RespawnClients ( qboolean force, team_t team )
{
	int i;

	// Respawn all clients back at a spawn pointer
	for ( i = 0; i < level.numConnectedClients; i ++ )
	{
		// Save the clients weapons
		playerState_t	ps;
		gentity_t*		ent;
		qboolean		ghost;

		ent = &g_entities[level.sortedClients[i]];

		// Make sure they are actually connected
		if ( ent->client->pers.connected != CON_CONNECTED )
		{
			continue;
		}

		//Ryan june 7 2003    dont respawn them if they are admins and using the adminspec feature
		if(ent->client->adminspec && !force)
		{
			continue;
		}
		//Ryan

		// GodOrDevil - 02.04.05 - 09:22am #Pbox
		// Ryan Nov 20 2004
		// don't respawn them if they are in the penalty box
		if(ent->client->sess.penaltySeconds)
		{
			continue;
		}
		//End - 02.04.05 - 09:22am

		// Save the entire player state so certain things
		// can be maintained across rounds
		ps = ent->client->ps;

		ghost = ent->client->sess.ghost;

		// Spawn the requested team
		if ( ent->client->sess.team != team )
		{
			continue;
		}

		// Dont force them to respawn if they arent a ghost
		if ( !ghost && !force )
		{
			continue;
		}

		// GodOrDevil - GAMETYPES
		if(ent->client->nvSess.frozen && !force)
		{
			ent->client->ps.pm_flags &= ~PMF_GHOST;
			ent->client->ps.pm_type = PM_NORMAL;
			ent->client->sess.ghost = qfalse;
			continue;
		}

		// #TFT - clear timers
		ent->client->nvSess.frozen = 0;
		ent->client->nvSess.frozenTime = level.time;
		//END

		// If the client is a ghost then revert them
		if ( ent->client->sess.ghost )
		{
			// Clean up any following monkey business
			G_StopFollowing ( ent );

			// Disable being a ghost
			ent->client->ps.pm_flags &= ~PMF_GHOST;
			ent->client->ps.pm_type = PM_NORMAL;
			ent->client->sess.ghost = qfalse;

			//Ryan april 10 2004
			//make sure we add up the ghost time in inf, elim games
			if(force && ent->client->sess.ghostStartTime)
			{
				ent->client->sess.totalSpectatorTime += level.time - ent->client->sess.ghostStartTime;
				ent->client->sess.ghostStartTime = 0;
			}
			//Ryan
		}
		
		ent->client->sess.noTeamChange = qfalse;
		if(NV_isVIP())
		{
			if(ent->client->nvSess.vip == 1)
			{
				ent->client->nvSess.vip = 2;
				ent->client->sess.lastIdentityChange = level.time - 1000;
				ClientUserinfoChanged( ent->s.number );
			}
		}
		trap_UnlinkEntity (ent);
		ClientSpawn ( ent );
	}
}

/*
===============
G_ResetPickups
===============
*/
void G_WallUse(gentity_t *self, gentity_t *other, gentity_t *activator);
void G_ResetEntities ( void )
{
	int i;

	// Run through all the entities in the level and reset those which
	// need to be reset
	for ( i = 0; i < level.num_entities; i ++ )
	{
		gentity_t* ent;

		ent = &g_entities[i];

		// Skip entities not in use
		if ( !ent->inuse )
		{	
			continue;
		}

		// If this is a player then unlink it so then clients
		// spawned in dont telefrag
		if ( ent->s.eType == ET_PLAYER || ent->s.eType == ET_BODY )
		{
			trap_UnlinkEntity ( ent );
		}
		// If this is a missile
		else if ( ent->s.eType == ET_MISSILE )
		{
			G_FreeEntity ( ent );
		}
		// If the dropped flag is set then free it
		else if ( ent->flags & FL_DROPPED_ITEM )
		{
			G_FreeEntity ( ent );
		}
		// If this item is waiting to be respawned, then respawn it
		else if ( ent->think == RespawnItem )
		{
			RespawnItem ( ent );
		}
		else if ( ent->s.eType == ET_DAMAGEAREA )
		{
			G_FreeEntity ( ent );
		}
		// GodOrDevil - GAMETYPES #sab 
		else if (ent->s.eType == ET_GRAPPLE)
		{
 			ent->disarmTime = 0;
			ent->armTime = 0;
			ent->flags = 0;
		}
		else if ( ent->use == hurt_use )
		{
			if ( ent->spawnflags & 1 )
			{
				trap_UnlinkEntity ( ent );
			}
		}
		else if ( ent->think == target_effect_delayed_use )
		{
			ent->think = 0;
			ent->nextthink = 0;
		}

		// GodOrDevil
		else if ( ent->s.eType == ET_MOVER && ent->use != NULL && ent->use == G_WallUse )
		{
			if ( ent->spawnflags & 1 )
			{
				trap_UnlinkEntity ( ent );
			}
			else
			{
				trap_LinkEntity ( ent );
			}
		}
		// End
	}
}

/*
===============
G_ResetGametype
===============
*/
void G_ResetGametype ( void )
{
	gentity_t*	tent;

	// Reset the glass in the level
	G_ResetGlass ( );

	// Reset all pickups in the world
	G_ResetEntities ( );

	// Reset the gametype itself
	G_ResetGametypeEntities ( );
	
	// GodOrDevil - 09.09.06 - 11:23pm #goldsource roundlimits
	// Cant have a 0 roundtimelimit
	if ( g_roundtimelimit.integer < 1 )
	{
		trap_Cvar_Set ( "g_roundtimelimit", "1" );
		trap_Cvar_Update ( &g_roundtimelimit );
	}
	///End  - 09.09.06 - 11:24pm

	// Initialize the respawn interval since this is a interval gametype
	//if(gt_customType.integer >= NV_GT_DOM)
#ifdef _MAPEDITOR
	if(nv_editMap.integer)
	{
	}
	else
#endif
	if( g_showGametypeHelp.integer && !level.warmupTime)
	{
		if(!level.gametypeStartTime) // game hasnt started yet, so extend our delay for the help info
		{
		//	level.gametypeDelayTime = level.time + g_warmup.integer;
			level.gametypeDelayTime = level.time + g_roundstartdelay.integer * 3000;
			if(level.gametypeData->respawnType == RT_NONE)
			{
				level.gametypeDelayTime = level.time + g_roundstartdelay.integer * 5000;
			}
		}
		else
		{
			level.gametypeDelayTime = level.time + g_roundstartdelay.integer * 1000;
		}
	}
	switch ( level.gametypeData->respawnType )
	{
		case RT_INTERVAL:
			level.gametypeRespawnTime[TEAM_RED]  = 0;
			level.gametypeRespawnTime[TEAM_BLUE] = 0;
			level.gametypeRespawnTime[TEAM_FREE] = 0;
			break;

		case RT_NONE:
			if( !g_showGametypeHelp.integer)
			{
				level.gametypeDelayTime = level.time + g_roundstartdelay.integer * 1000;
			}

			level.gametypeRoundTime = level.time + (g_roundtimelimit.integer * 60000);

			if ( level.gametypeDelayTime != level.time )
			{	
				level.extras.roundCount++;
#ifdef _COMPMODE
				if(g_compMode.integer && !level.warmupTime)
				{
					trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@Get Ready for ^1ROUND ^3%i", level.gametypeDelayTime, level.extras.roundCount ) );
				}
				else
#endif
				if(!g_showGametypeHelp.integer)
				{
					trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@Get Ready", level.gametypeDelayTime ) );
				}
				// GodOrDevil - 04.15.06 - 05:29pm #autoEven for INF/Elim
				if(g_autoEvenTeams.integer){
					int red, blue;
					red = TeamCount( -1, TEAM_RED, NULL );
					blue = TeamCount( -1, TEAM_BLUE, NULL );
					if ((red > blue + 1) || (blue > red + 1))
					{
						//RPM_Even_Teams(NULL, qtrue);
						NV_evenTeams(0, NULL, 99);
					}
				}
				///End  - 04.15.06 - 05:30pm

			}

			trap_SetConfigstring ( CS_GAMETYPE_TIMER, va("%i", level.gametypeRoundTime) );
			break;
	}

	level.gametypeJoinTime  = 0;
	
	// Allow people to join 20 seconds after both teams have people on them
	if ( !level.gametypeData->teams )
	{
		level.gametypeJoinTime = level.time;
	}

	// Respawn all clients
	if(NV_isVIP())
	{
		level.extras.bombGivenClient[TEAM_RED] = 0;
		level.extras.bombGivenClient[TEAM_BLUE] = 0;
	}
	if(g_swapsides.integer > 0 && level.extras.roundSwapCount >= g_swapsides.integer)
	{
		level.extras.roundSwapCount = 1;
		SC_SwapTeams( NULL);
	}	
	else if(!level.gametypeData->teams)
	{
		G_RespawnClients ( qtrue, TEAM_FREE );
		level.extras.roundSwapCount++;
	}
	else
	{
		G_RespawnClients ( qtrue, TEAM_RED );
		G_RespawnClients ( qtrue, TEAM_BLUE );
		level.extras.roundSwapCount++;
	}


	// GodOrDevil - GAMETYPES
	//level.extras.blueReinforcements = 0;
	//level.extras.redReinforcements = 0;
	level.extras.domControlledFlags[0] = 0;
	level.extras.domControlledFlags[1] = 0;
	level.extras.roundOver = qfalse;

	if(NV_isSab() || NV_isRaze() || NV_isRush() || NV_isVIP())
	{
		level.gametypeRespawnTime[TEAM_RED]  = 0;
		level.gametypeRespawnTime[TEAM_BLUE] = 0;
		level.gametypeRespawnTime[TEAM_FREE] = 0;
	}
	// #resetBomb #sab  - this resets the bomb and the timer.
	if(NV_isSab() || NV_isDem())
	{
		trap_GT_SendEvent ( GTEV_RESET_BOMB, level.time, 0, 0, 0, 0, 0 );
	}
	// END


	level.gametypeStartTime = level.time;
	level.gametypeResetTime = 0;

	// Reset the clients local effects
	tent = G_TempEntity( vec3_origin, EV_GAMETYPE_RESTART );
	tent->r.svFlags |= SVF_BROADCAST;

	// Start the gametype
	trap_GT_Start ( level.gametypeStartTime );

}

/*
===============
G_ParseGametypeItems
===============
*/
qboolean G_ParseGametypeItems ( TGPGroup* itemsGroup )
{
	TGPGroup	itemGroup;
	int			itemCount;
	char		temp[MAX_QPATH];
	gentity_t	*ent;
	// KRIS 12/04/2003 12:48AM
	// for bot AI
#ifdef _CRS_BOTS
	botGametypeItemInfo_t *GTInfo;
	char text[MAX_QPATH];
#endif
	// KRIS

	// Handle NULL for convienience
	if ( !itemsGroup )
	{
		return qfalse;
	}

	// Loop over all the items and add each 
	itemGroup = trap_GPG_GetSubGroups ( itemsGroup );
	itemCount = 0;

	while ( itemGroup )
	{	
		gitem_t*   item;
		
		// Parse out the pickup name
		trap_GPG_GetName ( itemGroup, temp );
		// KRIS 12/04/2003 12:49AM
		// GodOrDevil - WTF!  This dont make sense.  Most of this info is NOT stored in the gt scripts!
		/// need to set the values somewhere else - I added some code, but its probably not much better.
		/// These bots still are worthless!
		// for bot AI
		//memset(GTInfo, 0, sizeof(botGametypeItemInfo_t));
#ifdef _CRS_BOTS
		GTInfo = NULL;
		if ((itemCount >= 0) && (itemCount < MAX_GAMETYPE_ITEMS))
		{
			GTInfo = &bot_GTInfo[itemCount];

			strcpy(GTInfo->name, temp);
			
			memset(GTInfo, 0, sizeof(botGametypeItemInfo_t));

			Com_Printf(S_COLOR_YELLOW"G_ParseGametypeItems():- parsing gametype item = %i\n", itemCount);
			GTInfo->index = MODELINDEX_GAMETYPE_ITEM + itemCount;

			// which team wants this item?
			// defaults to both
			trap_GPG_FindPairValue(itemGroup, "team", "both", text);
			if (text[0])
			{
				Com_Printf(S_COLOR_YELLOW"team = %s\n", text);
			}

			if (!Q_stricmp(text, "red"))
			{
				GTInfo->team = (1<<TEAM_RED);
			}
			else if (!Q_stricmp(text, "blue"))
			{
				GTInfo->team = (1<<TEAM_BLUE);
			}
			else
			{
				GTInfo->team = ((1<<TEAM_RED) | (1<<TEAM_BLUE));
			}
			// GodOrDevil - Bot item destinations
			if (!Q_stricmp(temp, "red_flag"))
			{
				//strcpy(GTInfo->target, "red_capture_point");
				strcpy(GTInfo->target, "blue_capture_point");
			}
			else if (!Q_stricmp(temp, "blue_flag"))
			{
				//strcpy(GTInfo->target, "blue_capture_point");
				strcpy(GTInfo->target, "red_capture_point");
			}
			else if (!Q_stricmp(temp, "briefcase"))
			{
				strcpy(GTInfo->target, "briefcase_destination");
			}
			
			/// // set this items target gametype_trigger
			///trap_GPG_FindPairValue(itemGroup, "target", "", GTInfo->target);
			///if (GTInfo->target[0])
			///{
			///	Com_Printf(S_COLOR_YELLOW"target = %s\n", GTInfo->target);
			///}
			///End

			// set this items target2 gametype_trigger
			trap_GPG_FindPairValue(itemGroup, "target2", "", GTInfo->target2);
			if (GTInfo->target2[0])
			{
				Com_Printf(S_COLOR_YELLOW"target2 = %s\n", GTInfo->target2);
			}
			
			// set this items target3 gametype_trigger
			trap_GPG_FindPairValue(itemGroup, "target3", "", GTInfo->target3);
			if (GTInfo->target2[0])
			{
				Com_Printf(S_COLOR_YELLOW"target3 = %s\n", GTInfo->target3);
			}
			
			
			// do we have to use this item when near it?
			trap_GPG_FindPairValue(itemGroup, "use", "no", text);

			if (!Q_stricmp(text, "yes"))
			{
				GTInfo->use = qtrue;
				Com_Printf(S_COLOR_YELLOW"use = TRUE\n");
			}
			else
			{
				GTInfo->use = qfalse;
			}
		
			// do we have to use this items gametype_trigger when near it?
			trap_GPG_FindPairValue(itemGroup, "useTarget", "no", text);

			if (!Q_stricmp(text, "yes"))
			{
				GTInfo->useTarget = qtrue;
				Com_Printf(S_COLOR_YELLOW"useTarget = TRUE\n");
			}
			else
			{
				GTInfo->useTarget = qfalse;
			}
		
			// which team wants this item when it is dropped?
			// defaults to both
			trap_GPG_FindPairValue(itemGroup, "droppedTeam", "both", text);
			if (text[0])
			{
				Com_Printf(S_COLOR_YELLOW"droppedTeam = %s\n", text);
			}
			
			if (!Q_stricmp(text, "red"))
			{
				GTInfo->droppedTeam = (1<<TEAM_RED);
			}
			else if (!Q_stricmp(text, "blue"))
			{
				GTInfo->droppedTeam = (1<<TEAM_BLUE);
			}
			else
			{
				GTInfo->droppedTeam = ((1<<TEAM_RED) | (1<<TEAM_BLUE));
			}

			// which team wants to guard this item?
			// defaults to both
			trap_GPG_FindPairValue(itemGroup, "guard", "both", text);
			if (text[0])
			{
				Com_Printf(S_COLOR_YELLOW"guard = %s\n", text);
			}
			
			if (!Q_stricmp(text, "red"))
			{
				GTInfo->guard = (1<<TEAM_RED);
			}
			else if (!Q_stricmp(text, "blue"))
			{
				GTInfo->guard = (1<<TEAM_BLUE);
			}
			else
			{
				GTInfo->guard = ((1<<TEAM_RED) | (1<<TEAM_BLUE));
			}

			// which team wants to guard this item when its dropped?
			// defaults to both
			trap_GPG_FindPairValue(itemGroup, "guardDropped", "both", text);
			if (text[0])
			{
				Com_Printf(S_COLOR_YELLOW"guardDropped = %s\n", text);
			}
			
			if (!Q_stricmp(text, "red"))
			{
				GTInfo->guardDropped = (1<<TEAM_RED);
			}
			else if (!Q_stricmp(text, "blue"))
			{
				GTInfo->guardDropped = (1<<TEAM_BLUE);
			}
			else
			{
				GTInfo->guardDropped = ((1<<TEAM_RED) | (1<<TEAM_BLUE));
			}
		}
		// KRIS
#endif
		item = BG_FindItem ( temp );
		if ( !item )
		{
			item = &bg_itemlist[ MODELINDEX_GAMETYPE_ITEM + itemCount ];
			item->pickup_name = (char *)trap_VM_LocalStringAlloc ( temp );
			itemCount++;
		}

		// Handle the entity specific stuff by finding all matching items that 
		// were spawned.
		ent = NULL;
		while ( NULL != (ent = G_Find ( ent, FOFS(targetname), item->pickup_name ) ) )
		{
			// If not a gametype item then skip it
			if ( Q_stricmp ( ent->classname, "gametype_item" ) )
			{
				continue;
			}

			// Setup the gametype data
			ent->item	   = item;
			ent->nextthink = level.time + 200;
			ent->think     = G_GametypeItemThink;
		}

		// Next sub group
		itemGroup = trap_GPG_GetNext(itemGroup);
	}

#ifdef _SOF2_BOTS
	// GRIM 10/06/2002 6:18PM
	level.gtItemCount = itemCount;
	// GRIM
#endif

	return qtrue;
}

/*
===============
G_ParseGametypeFile
===============
*/
qboolean G_ParseGametypeFile ( void )
{
	TGenericParser2 GP2;
	TGPGroup		topGroup;
	TGPGroup		gametypeGroup;
	char			value[4096];

	// Default the gametype config strings
	trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, "0,0" );

	// Open the given gametype file
	GP2 = trap_GP_ParseFile ( (char*)level.gametypeData->script, qtrue, qfalse );
	if (!GP2)
	{
		return qfalse;
	}

	// Grab the top group and the list of sub groups
	topGroup = trap_GP_GetBaseParseGroup(GP2);
	gametypeGroup = trap_GPG_FindSubGroup(topGroup, "gametype" );
	if ( !gametypeGroup )
	{
		trap_GP_Delete(&GP2);
		return qfalse;
	}

	// Look for the respawn type
	trap_GPG_FindPairValue ( gametypeGroup, "respawn", "normal", value );
#ifdef _ZOMBIES
	//if(NV_isZom())
	//{
	//	level.gametypeData->respawnType = RT_INTERVAL;
	//}
	//else
#endif

	if ( !Q_stricmp ( value, "none" ) )
	{
		level.gametypeData->respawnType = RT_NONE;
	}
	else if ( !Q_stricmp ( value, "interval" ) )
	{
		level.gametypeData->respawnType = RT_INTERVAL;
	}
	else
	{
		level.gametypeData->respawnType = RT_NORMAL;
	}

	// Grab the defined items
	G_ParseGametypeItems ( trap_GPG_FindSubGroup ( gametypeGroup, "items" ) );

	// Free up the parser
	trap_GP_Delete(&GP2);

	return qtrue;
}

// GodOrDevil - 02.03.05 - 11:30am - #DropGametypeItems #GOLD
void G_EnableGametypeItemPickup ( gentity_t* ent )
{
	ent->s.eFlags &= ~EF_NOPICKUP;
}
/*
=================
G_DropGametypeItems

Drops all of the gametype items held by the player
=================
*/
void G_DropGametypeItems ( gentity_t* self, int delayPickup )
{
	float		angle;
	int			i;
	gentity_t	*drop;
	gitem_t		*item;

	// drop all custom gametype items
	angle = 0;
	for ( i = 0 ; i < MAX_GAMETYPE_ITEMS ; i++ ) 
	{
		// skip this gametype item if the client doenst have it
		if ( !(self->client->ps.stats[STAT_GAMETYPE_ITEMS] & (1<<i)) ) 
		{
			continue;
		}

		item = BG_FindGametypeItem ( i );
		if ( !item ) 
		{
			continue;
		}

		drop = G_DropItem( self, item, angle );
		drop->count = 1;
		angle += 45;

		if ( delayPickup )
		{
			drop->nextthink = level.time + delayPickup;	
			drop->s.eFlags |= EF_NOPICKUP;
			drop->think = G_EnableGametypeItemPickup;
		}
		
		// TAke it away from the client just in case
		self->client->ps.stats[STAT_GAMETYPE_ITEMS] &= ~(1<<i);

		if ( self->enemy && self->enemy->client && !OnSameTeam ( self->enemy, self ) )
		{	//05.07.05 - 07:15pm
			//trap_GT_SendEvent ( GTEV_ITEM_DEFEND, level.time, level.gametypeItems[item->giTag].id, self->enemy->s.clientNum, self->enemy->client->sess.team, 0, 0  );
			trap_GT_SendEvent ( GTEV_ITEM_DEFEND, level.time, item->quantity, self->enemy->s.clientNum, self->enemy->client->sess.team, 0, 0  );
			//End  - 05.07.05 - 07:15pm
		}
	}

	self->client->ps.stats[STAT_GAMETYPE_ITEMS] = 0;
}
//End - 02.03.05 - 11:31am

void CheckGametype ( void )
{			
	//int i;
	//int counts[TEAM_NUM_TEAMS][3];
	static int	bombTime = 0;

	// If the level is over then forget checking gametype stuff.
	//Ryan june 15 2003
	//if ( level.intermissiontime )
	if ( level.intermissiontime || level.pause )
	//End Ryan
	{
		return;
	}

	if(level.intermissionQueued)
		return;

 	if ( !level.gametypeStartTime )
	{
		int i;
		int counts[TEAM_NUM_TEAMS];

		memset ( counts, 0, sizeof(counts) );

		for ( i = 0; i < level.numConnectedClients; i ++ )
		{
			gentity_t* other = &g_entities[level.sortedClients[i]];

			if ( other->client->pers.connected != CON_CONNECTED )
			{
				continue;
			}
				
			if ( other->client->pers.connected != CON_CONNECTED )
			{
				continue;
			}

			counts[other->client->sess.team]++;
		}

		if ( counts[TEAM_RED] || counts[TEAM_BLUE] || counts[TEAM_FREE] )
		{
			G_ResetGametype ( );
			return;
		}
	}	
	
	// Check for delayed gametype reset
	if ( level.gametypeResetTime )
	{
		if ( level.time > level.gametypeResetTime )
		{			
			// Dont do this again
			level.gametypeResetTime = 0;
			G_ResetGametype ( );
		}

		return;
	}

	// GodOrDevil - GAMETYPE #Dem - give random player the item
	
	if((level.time > level.gametypeStartTime + 5000) && level.numPlayingClients && bombTime < level.time && level.gametypeDelayTime <= level.time)
	{
		if(NV_isDem() && level.extras.bombGivenClient[TEAM_BLUE] <= 0)
		{
			NV_GiveGTIToTeam(TEAM_BLUE, qtrue);
			bombTime = level.time + 5000;
		}
		else if(NV_isVIP())
		{
			if( level.extras.bombGivenClient[TEAM_BLUE] <= 0)
			{
				NV_GiveGTIToTeam(TEAM_BLUE, qfalse);
			}			
			if( level.extras.bombGivenClient[TEAM_RED] <= 0)
			{
				NV_GiveGTIToTeam(TEAM_RED, qfalse);
			}
			bombTime = level.time + 5000;
		}
	}
	// GodOrDevil #sab #respawn - sab has different spawning.  server mimics, but RT_NONE set so the client isnt confused
#ifdef _ZOMBIES
	if(level.time > level.gametypeDelayTime && level.gametypeStartTime && (NV_isSab() || NV_isTFT() || NV_isRaze() || NV_isCR() || NV_isZom() ))
#else
	if(level.time > level.gametypeDelayTime && level.gametypeStartTime && (NV_isSab() || NV_isTFT() || NV_isRaze() || NV_isCR() ))
#endif
	{
		//if(level.suddenDeath)
		//{
		//	NV_SuddenDeath();
		//	return;
		//}
		//else
		{
			team_t team;
			for ( team = TEAM_RED; team < TEAM_SPECTATOR; team ++ )
			{
				if ( level.gametypeRespawnTime[team] && level.time > level.gametypeRespawnTime[team] )
				{
					if(NV_isTFT())
					{
					//	G_RespawnClients ( qfalse, team );
					//	level.gametypeRespawnTime[team] = level.time + g_freezeTime.integer * 1000;
					}
					else
					{
						// Respawn all dead clients
						G_RespawnClients ( qfalse, team );

						// Next interval
						level.gametypeRespawnTime[team] = 0;	
					}

				}
			}
						
			// #TFT #ELIMITATED
			if(NV_isTFT())
			{
				if(!level.firstBlood)		
				{
					if(!level.gametypeRespawnTime[TEAM_RED])
						level.gametypeRespawnTime[TEAM_RED] = level.time + 5000;
					if(!level.gametypeRespawnTime[TEAM_BLUE])
						level.gametypeRespawnTime[TEAM_BLUE] = level.time + 5000;
				}
				else
				{
					int i;
					int alive[TEAM_NUM_TEAMS];
					int dead[TEAM_NUM_TEAMS];
					int	player[TEAM_NUM_TEAMS];

					memset ( &alive[0], 0, sizeof(alive) );
					memset ( &dead[0], 0, sizeof(dead) );
					memset ( &player[0], -1, sizeof(player) );

					for ( i = 0; i < level.numConnectedClients; i ++ )
					{
						gentity_t* ent = &g_entities[level.sortedClients[i]];

						if ( ent->client->pers.connected != CON_CONNECTED )
							continue;

						if ( G_IsClientDead ( ent->client ) )
							dead[ent->client->sess.team] ++;
						else
						{
							alive[ent->client->sess.team] ++;
							player[ent->client->sess.team] = ent->s.number;
						}
					}
					// GodOrDevil - 03.27.05 - 8:50pm #lastAlive
					/*==================== Start Last Alive Msg ====================*/ 				
					if ( alive[TEAM_BLUE] == 1 && !blueNotify && dead[TEAM_BLUE] && player[TEAM_BLUE] >= 0)
					{
						CPx(player[TEAM_BLUE], "cp \"^7You are the\n^1LAST ALIVE ^7on the ^4BLUE ^7TEAM!\n\"" );
						NV_Sound(player[TEAM_BLUE], "sound/misc/events/tut_door01.mp3");
						blueNotify = qtrue;
					}		
					else if ( alive[TEAM_RED] == 1 && !redNotify && dead[TEAM_RED] && player[TEAM_RED] >= 0)
					{
						CPx(player[TEAM_RED], "cp \"^7You are the\n^1LAST ALIVE ^7on the ^1RED ^7TEAM!\n\"" );
						NV_Sound(player[TEAM_RED], "sound/misc/events/tut_door01.mp3");
						redNotify = qtrue;
					}

					// Reset the last alive message
					if( alive[TEAM_RED] > 1)
						redNotify = qfalse;
					if( alive[TEAM_BLUE] > 1)
						blueNotify = qfalse;

					/*==================== End Alive Massage ====================*/
					//End  - 03.27.05 - 09:24pm
					if ( !alive[TEAM_BLUE] && dead[TEAM_BLUE] )
					{
						trap_GT_SendEvent ( GTEV_TEAM_ELIMINATED, level.time, TEAM_BLUE, 0, 0, 0, 0 );		
						redNotify = qfalse;
						blueNotify = qfalse;
						level.extras.roundOver = qtrue;
					}
					else if ( !alive[TEAM_RED] && dead[TEAM_RED] )
					{
						trap_GT_SendEvent ( GTEV_TEAM_ELIMINATED, level.time, TEAM_RED, 0, 0, 0, 0 );
						redNotify = qfalse;
						blueNotify = qfalse;
						level.extras.roundOver = qtrue;
					}
				}
			}
			//

			// See if the time has expired
			if ( level.time > level.gametypeRoundTime )
			{
				// GodOrDevil
				//trap_GT_SendEvent ( GTEV_TIME_EXPIRED, level.time, 0, 0, 0, 0, 0 );
				if (!trap_GT_SendEvent(GTEV_TIME_EXPIRED, level.time, 0, 0, 0, 0, 0))
					level.extras.roundOver = qtrue;
				// End
			} 
		}
		return;
	}
	
	// Handle respawn interval spawning
	// GodOrDevil #Warmup Respawn - INF/ELIM will just spawn when they die.  Fuck the waiting bullshit!
	else if ( level.warmupTime	|| (level.gametypeData->respawnType == RT_INTERVAL))
	//END
	{	
		team_t team;
		//if(level.suddenDeath)
		//{
		//	NV_SuddenDeath();
		//	return;
		//}
		for ( team = TEAM_RED; team < TEAM_SPECTATOR; team ++ )
		{
			if( level.warmupTime )
			{		
				// Respawn all dead clients
				G_RespawnClients ( qfalse, team );
				// Next interval
				level.gametypeRespawnTime[team] = 0;
				continue;
			}	

			if (level.gametypeRespawnTime[team] && level.time > level.gametypeRespawnTime[team] )
			{					

				//if(NV_TeamDeadNoLives(team))
				//	return;

				level.gametypeRespawnTime[team] = 0;
				G_RespawnClients ( qfalse, team );
			}
		}

		if(level.gametypeRoundTime && NV_isTctf()) // check some roundlimit stuff
		{
			if ( level.time == (level.gametypeRoundTime - 60000))
			{
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^31 ^2minute ^3remaining",level.time + 5000) );
				NV_Sound(-1, "sound/movers/doors/jail_door/jail_doorstop.mp3" );
			}
			else if ( level.time == (level.gametypeRoundTime - 30000))
			{
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^330 ^2seconds ^3remaining", level.time + 5000) );
				NV_Sound(-1, "sound/movers/doors/jail_door/jail_doorstop.mp3" );
			}		
			else if ( level.time == (level.gametypeRoundTime - 10000))
			{
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^310 ^2seconds ^3remaining", level.time + 5000) );
				NV_Sound(-1, "sound/movers/doors/jail_door/jail_doorstop.mp3" );
			}

			else if ( level.time > level.gametypeRoundTime )
			{
				//if( level.teamScores[TEAM_RED] > level.teamScores[TEAM_BLUE])
				//{		
				//	NV_Sound(-1, "sound/ctf_win.mp3");
				//	level.gametypeResetTime = level.time + 5000;
				//	trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^1%s ^2has ^won ^2the round!", level.time + 5000, "RED") );
				//	level.extras.roundSwapCount++;
				//	level.roundPausedWait = qtrue;
				//	level.extras.roundNumber++;
				//}	
				//else if( level.teamScores[TEAM_BLUE] > level.teamScores[TEAM_RED])
				//{
				//	NV_Sound(-1, "sound/ctf_win.mp3");
				//	level.gametypeResetTime = level.time + 5000;
				//	trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^4%s ^2has ^3won ^2the round!", level.time + 5000, "BLUE") );
				//	level.extras.roundSwapCount++;
				//	level.roundPausedWait = qtrue;
				//	level.extras.roundNumber++;
				//}
				//else
				{
					NV_Sound(-1, "sound/ctf_return.mp3");
					level.gametypeResetTime = level.time + 5000;
					trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^3~ ^2DRAW ^3~", level.time + 5000) );
					level.extras.roundSwapCount++;
					level.extras.roundNumber++;
				}
			}
		}
	}	
	//else if(!level.intermissionQueued && NV_isFT())
	//{
	//	int		i, alive = 0, dead = 0;
	//	int		winnerID = -1;

	//	for ( i = 0; i < level.numConnectedClients; i ++ )
	//	{
	//		gentity_t* ent = &g_entities[level.sortedClients[i]];

	//		if ( ent->client->pers.connected != CON_CONNECTED )
	//			continue;

	//		if ( G_IsClientDead ( ent->client ) )
	//			dead++;
	//		else
	//		{
	//			alive++;
	//			winnerID = level.sortedClients[i];
	//		}
	//	}
	//	if(alive == 1 && dead && winnerID >=0 )
	//	{
	//		// WINNER! - do something
	//		//g_entities[winnerID].client->sess.score = g_scorelimit.integer;
	//		G_AddScore ( &g_entities[winnerID], (dead + alive) * 2 );
	//	}
	//}
	// If we are in RT_NONE respawn mode then we need to look for everyone being dead
	else if ( level.time > level.gametypeDelayTime && level.gametypeData->respawnType == RT_NONE && level.gametypeStartTime)
	{
		int i;
		int alive[TEAM_NUM_TEAMS];
		int dead[TEAM_NUM_TEAMS];
		int	player[TEAM_NUM_TEAMS];

		memset ( &alive[0], 0, sizeof(alive) );
		memset ( &dead[0], 0, sizeof(dead) );
		memset ( &player[0], -1, sizeof(player) );

		//if(level.firstBlood)
		//{
			for ( i = 0; i < level.numConnectedClients; i ++ )
			{
				gentity_t* ent = &g_entities[level.sortedClients[i]];

				if ( ent->client->pers.connected != CON_CONNECTED )
				{
					continue;
				}

				if ( G_IsClientDead ( ent->client ) )
				{
					dead[ent->client->sess.team] ++;
				}
				else
				{
					alive[ent->client->sess.team] ++;
					player[ent->client->sess.team] = ent->s.number;
				}
			}
			// GodOrDevil - 03.27.05 - 8:50pm #lastAlive
			/*==================== Start Last Alive Msg ====================*/ 				
			if ( alive[TEAM_BLUE] == 1 && !blueNotify && dead[TEAM_BLUE] && player[TEAM_BLUE] >= 0)
			{
				CPx(player[TEAM_BLUE], "cp \"^7You are the\n^1LAST ALIVE ^7on the ^4BLUE ^7TEAM!\n\"" );
				NV_Sound(player[TEAM_BLUE], "sound/misc/events/tut_door01.mp3");
				blueNotify = qtrue;
			}		
			else if ( alive[TEAM_RED] == 1 && !redNotify && dead[TEAM_RED] && player[TEAM_RED] >= 0)
			{
				CPx(player[TEAM_RED], "cp \"^7You are the\n^1LAST ALIVE ^7on the ^1RED ^7TEAM!\n\"" );
				NV_Sound(player[TEAM_RED], "sound/misc/events/tut_door01.mp3");
				redNotify = qtrue;
			}		
			/*==================== End Alive Massage ====================*/
			//End  - 03.27.05 - 09:24pm

			// If everyone is dead on a team then reset the gametype, but only if 
			// there was someone on that team to begin with.
			if ( !alive[TEAM_RED] && dead[TEAM_RED] )
			{			
				trap_GT_SendEvent ( GTEV_TEAM_ELIMINATED, level.time, TEAM_RED, 0, 0, 0, 0 );
				redNotify = qfalse;
				blueNotify = qfalse;
				return;
			}
			if ( !alive[TEAM_BLUE] && dead[TEAM_BLUE] )
			{			
				trap_GT_SendEvent ( GTEV_TEAM_ELIMINATED, level.time, TEAM_BLUE, 0, 0, 0, 0 );
				redNotify = qfalse;
				blueNotify = qfalse;
				return;
			}
		//}

		// See if the time has expired
		if ( level.time > level.gametypeRoundTime )
		{
			trap_GT_SendEvent ( GTEV_TIME_EXPIRED, level.time, 0, 0, 0, 0, 0 );
			if(NV_isDem() || NV_isInf())
				level.extras.roundWinsRed++;
		} 
	}
}

/*
=================
G_GametypeCommand

Handles a command coming from the current gametype VM
=================
*/
int G_GametypeCommand ( int cmd, int arg0, int arg1, int arg2, int arg3, int arg4 )
{
	switch ( cmd )
	{
		case GTCMD_RESTART:
			if ( arg0 <= 0 )
			{
				G_ResetGametype ( );
			}
			else
			{
				level.gametypeResetTime = level.time + arg0 * 1000;
			}
			break;

		case GTCMD_TEXTMESSAGE:
			// GodOrDevil
			//if(arg0 == -999)
			////if(!strcmp((const char*)arg1, "gametype_trigger"))
			//{
			//	gentity_t	*find;
			//	find = NULL;
			//	while ( NULL != (find = G_Find ( find, FOFS(classname), "gametype_trigger" ) ) )
			//	{
			//		if ( find->health == arg1 )
			//		{
			//			G_UseTargetsByName ( find->target, NULL, NULL );
			//			//G_PlayEffect ( arg0, (float*) arg1, (float*) arg2 );
			//			break;
			//		}
			//	}
			//	break;
			//}
			//END
			trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,%s", level.time + 5000, (const char*)arg1 ) );
			break;		

		case GTCMD_RADIOMESSAGE:

			// #DEM
			if(arg0 == -999) 
			{			
				G_UseTargetsByName ( (const char*) arg1, NULL, NULL );
				break;
			}	
			// also added captures and defuses to sab, raze, dem
			if(arg0 == -777)  // used in NV gametype shit.. remove it and expect a crash!!!!!!!
			{	
#ifdef _DB_ENABLED
				int client = atoi((const char*)arg1);
				if(client >= 0 && client < MAX_CLIENTS)
					g_entities[client].client->pers.statinfo.itemCaps++;
#endif
				break;
			}
			// Give roundWin.
			if(arg0 == -888) 
			{	
				if(atoi((const char*) arg1) == TEAM_BLUE)
					level.extras.roundWinsBlue++;
				else if(atoi((const char*) arg1) == TEAM_RED)
					level.extras.roundWinsRed++;
				break;
			}
			// Give player Defends
			if(arg0 == -555) 
			{
#ifdef _DB_ENABLED
				int client = atoi((const char*)arg1);
				if(client >= 0 && client < MAX_CLIENTS)
					g_entities[client].client->pers.statinfo.itemDefs++;
#endif		
				break;
			}		
					
			if(arg0 == -333) // RED_TEAM			
			{					
				int team = atoi((const char*)arg1);
				if(team == TEAM_RED)
				{
					TeamSound(TEAM_BLUE, "sound/ctf_base.mp3", "");
					//TeamSound(TEAM_BLUE, "sound/movers/buttons/button04.mp3", "");
					TeamSound(TEAM_RED, "sound/ctf_return.mp3", "");
				}
				else if(team == TEAM_BLUE)
				{
					TeamSound(TEAM_RED, "sound/ctf_base.mp3", "");
					//TeamSound(TEAM_BLUE, "sound/movers/buttons/button04.mp3", "");
					TeamSound(TEAM_BLUE, "sound/ctf_return.mp3", "");
					
				}				
				break;
			}	

			if(arg0 == -444) 			
			{
				int team = atoi((const char*)arg1);
				NV_FlareEffect((team_t)team);
				break;
			}
			//if(arg0 == -444) 
			//{
			//	NV_SetTriggerByName ( (const char*) arg1, IMG_TRIGGER_ARMED );
			//	break;
			//}	
			//END

			G_Voice ( &g_entities[arg0], NULL, SAY_TEAM, (const char*) arg1, qfalse );
			break;

		case GTCMD_REGISTERGLOBALSOUND:
			return G_SoundIndex ( (char*) arg0 );

		case GTCMD_STARTGLOBALSOUND:
		{
			gentity_t* tent;
			tent = G_TempEntity( vec3_origin, EV_GLOBAL_SOUND );
			tent->s.eventParm = arg0;
			tent->r.svFlags = SVF_BROADCAST;	
			break;
		}	

		case GTCMD_REGISTEREFFECT:
			return G_EffectIndex ( (char*) arg0 );

		case GTCMD_PLAYEFFECT: 
			G_PlayEffect ( arg0, (float*) arg1, (float*) arg2 );
			break;

		case GTCMD_ADDCLIENTSCORE:
			G_AddScore ( &g_entities[arg0], arg1 );
			break;

		case GTCMD_ADDTEAMSCORE:
			G_AddTeamScore ( (team_t) arg0, arg1 );
			// GodOrDevil
			if( NV_isTctf() )
			{
				level.extras.roundNumber++;
				if( (team_t) arg0 == TEAM_RED )
				{
					level.gametypeResetTime = level.time + 5000;
					trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^1%s ^3WINS ^2the ^3ROUND!",level.time + 5000, "RED") );
					level.extras.roundWinsRed++;
					//level.extras.roundSwapCount++;
				}	
				else if( (team_t) arg0 == TEAM_BLUE )
				{
					level.gametypeResetTime = level.time + 5000;
					trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^4%s ^3WINS ^2the ^3ROUND!", level.time + 5000, "BLUE") );
					level.extras.roundWinsBlue++;
					//level.extras.roundSwapCount++;					
				}
			}
			else if(g_swapsides.integer && g_swapsides.integer <= level.extras.roundNumber && !(level.intermissiontime || level.intermissionQueued))
			{
				level.gametypeResetTime = level.time + 3000;
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^-* ^3SWAPPING SIDES ^-*", level.time + 5000) );
			}
			// End
			break;
		case GTCMD_RESETITEM:
		{
			gitem_t* item;

			item = BG_FindGametypeItemByID ( arg0 );
			if ( item )
			{
				G_ResetGametypeItem ( item );
				return qtrue;
			}
			//else
			//{
			//	Com_Printf("^3GTCMD_RESETITEM: item not found [%i]\n", arg0);
			//}
			break;
		}

		case GTCMD_GETCLIENTNAME:
 			if(arg0 >= MAX_CLIENTS) 
			{
#ifdef _OVERPOWER	
				if(NV_isOP())	// red flag, NULL, blue flag	
				{
					NV_ToggleTeamGTItemDraws ( arg0, arg2 );
					break;
				}			
				else //if(NV_isDem()) // trap_Cmd_GetTriggerTarget() for DEM; .. not JUST for dem..
#endif	
				{
					gentity_t	*find;
					Com_sprintf ( (char*) arg1, arg2, "" );

					find = NULL;
					while ( NULL != (find = G_Find ( find, FOFS(classname), "gametype_trigger" ) ) )
					{
						if ( find->health == arg0 )
						{			
							//trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, GTCMD_GETCLIENTNAME: find->target %s", level.time + 5000, find->target ) );
							Com_sprintf ( (char*) arg1, arg2, "%s", find->target );
							break;
						}
					}					
				}		
				break;
			}
			Com_sprintf ( (char*) arg1, arg2, "%s", g_entities[arg0].client->pers.netname );
			break;
		case GTCMD_DOESCLIENTHAVEITEM:
		{
			gitem_t*	item;
			gentity_t*	ent;

			ent  = &g_entities[arg0];
			item = BG_FindGametypeItemByID ( arg1 );

			if ( item )
			{
				if ( ent->client->ps.stats[STAT_GAMETYPE_ITEMS] & (1<<item->giTag) )
				{
					return 1;
				}
			}
			return 0;
		}

		case GTCMD_REGISTERITEM:
		{
			gitem_t		*item;
			gtItemDef_t	*def;

			def = (gtItemDef_t*)arg2;

			item = BG_FindItem ( (const char*) arg1 );
			if ( item )
			{
				gtitem_t *gtitem;

				gtitem = &level.gametypeItems[item->giTag];

				gtitem->id = arg0;
				item->quantity = arg0;

				// See if the trigger needs to be used.
				if ( def && /*def->size == sizeof(gtItemDef_t) && */ def->use )
				{
					//gtitem->useIcon  = G_IconIndex("gfx/menus/hud/tnt");
					gtitem->useTime  = def->useTime;
					
					//gtitem->useSound = def->useSound;
				}

				return qtrue;
			}
			return qfalse;
		}

		case GTCMD_REGISTERTRIGGER:
		{
			gentity_t* find;

			// GodOrDevil - #use
			gtTriggerDef_t	*def;
			def = (gtTriggerDef_t*)arg2;
			//END
			// GodOrDevil #dom - dont bother, DOM is handled different
			if(NV_isDom() || NV_isKoth())
				return 0;
			///End

			find = NULL;
			while ( NULL != (find = G_Find ( find, FOFS(classname), "gametype_trigger" ) ) )
			{
				if ( Q_stricmp ( find->targetname, (const char*) arg1 ) )
				{
					continue;
				}

				// Assign the id to it.
				find->health = arg0;
				find->touch  = gametype_trigger_touch;

			// GodOrDevil - #use
				// See if the trigger needs to be used.
				if ( def && /*def->size == sizeof(gtTriggerDef_t) &&*/ def->use )
				{
					if(NV_isRush())
					{
						// Set the use and make it a blue target for default (201 = blue target)
						find->use	 = bomb_target_blue_use;
						find->health = 201;
					}
					else
					{
						find->use		= gametype_trigger_use;
					}
					//find->delay		= def->useIcon;
					//find->delay		= G_IconIndex("gfx/menus/hud/tnt");
					find->soundPos1 = def->useTime;
					//find->soundLoop = def->useSound;

				}
			//END
				trap_LinkEntity (find);
			}

			return 0;
		}
		default:
			break;
	}

	return -1;
}

