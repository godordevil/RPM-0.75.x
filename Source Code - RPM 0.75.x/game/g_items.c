// Copyright (C) 2001-2002 Raven Software
//
#include "g_local.h"

/*

  Items are any object that a player can touch to gain some effect.

  Pickup will return the number of seconds until they should respawn.

  all items should pop when dropped in lava or slime

  Respawnable items don't actually go away when picked up, they are
  just made invisible and untouchable.  This allows them to ride
  movers and respawn apropriately.
*/


#define	RESPAWN_ARMOR		25
#define	RESPAWN_HEALTH		35
#define	RESPAWN_AMMO		40
#define RESPAWN_BACKPACK	40

extern gentity_t *droppedRedFlag;
extern gentity_t *droppedBlueFlag;

//======================================================================
void Add_Ammo (gentity_t *ent, int ammoindex, int count)
{
	ent->client->ps.ammo[ammoindex] += count;
	
	if ( ent->client->ps.ammo[ammoindex] > ammoData[ammoindex].max ) 
	{
		ent->client->ps.ammo[ammoindex] = ammoData[ammoindex].max;
	}
}

int Pickup_Ammo (gentity_t *ent, gentity_t *other)
{
	int	quantity;

	if ( ent->count ) 
	{
		quantity = ent->count;
	} 
	else 
	{
		quantity = ent->item->quantity;
	}

	Add_Ammo (other, ent->item->giTag, quantity);

	return RESPAWN_AMMO;
}

//======================================================================


int Pickup_Weapon (gentity_t *ent, gentity_t *other, qboolean* autoswitch ) 
{
	int			quantity;
	int			weaponNum = ent->item->giTag;
	qboolean	hasAltAmmo;

	hasAltAmmo = BG_WeaponHasAlternateAmmo ( weaponNum );

	if ( ent->count < 0 ) 
	{
		quantity = 0; // None for you, sir!
	} 
	else 
	{		
		if ( ent->count ) 
		{
			quantity = ent->count;
		} 
		else 
		{
			quantity = weaponData[weaponNum].attack[ATTACK_NORMAL].clipSize + (weaponData[weaponNum].attack[ATTACK_ALTERNATE].clipSize<<16);
		}
	}

	// Add the ammo in, dont use Add_Ammo because we may temporarily pass the max
	// ammo here and add_ammo wont let us do that.
	other->client->ps.ammo[weaponData[weaponNum].attack[ATTACK_NORMAL].ammoIndex] += (quantity&0xFF);
	other->client->ps.ammo[weaponData[weaponNum].attack[ATTACK_NORMAL].ammoIndex] += ((quantity>>8)&0xFF);

	if ( hasAltAmmo )
	{
		other->client->ps.ammo[weaponData[weaponNum].attack[ATTACK_ALTERNATE].ammoIndex] += ((quantity>>16)&0xFF);
		other->client->ps.ammo[weaponData[weaponNum].attack[ATTACK_ALTERNATE].ammoIndex] += ((quantity>>24)&0xFF);
	}

	// If just picked up move some ammo into the clip
	if ( !(other->client->ps.stats[STAT_WEAPONS]&(1<<weaponNum)) )
	{
		// Copy over the clips
		other->client->ps.clip[ATTACK_NORMAL][weaponNum] = (quantity&0xFF);
		other->client->ps.ammo[weaponData[weaponNum].attack[ATTACK_NORMAL].ammoIndex] -= other->client->ps.clip[ATTACK_NORMAL][weaponNum];

		if ( hasAltAmmo )
		{
			other->client->ps.clip[ATTACK_ALTERNATE][weaponNum] = ((quantity>>16)&0xFF);
			other->client->ps.ammo[weaponData[weaponNum].attack[ATTACK_ALTERNATE].ammoIndex] -= other->client->ps.clip[ATTACK_ALTERNATE][weaponNum];
		}
		
		if ( other->client->ps.weaponstate != WEAPON_CHARGING	  &&
			 other->client->ps.weaponstate != WEAPON_CHARGING_ALT    )
		{
			// Autoswitch the weapon
			*autoswitch = qtrue;
		}

		other->client->ps.firemode[weaponNum] = BG_FindFireMode ( weaponNum, ATTACK_NORMAL, WP_FIREMODE_AUTO );
	}

	// Call add ammo with 0 ammo to force it to cap it at max
	Add_Ammo( other, weaponData[weaponNum].attack[ATTACK_NORMAL].ammoIndex, 0 );

	if ( hasAltAmmo )
	{
		Add_Ammo( other, weaponData[weaponNum].attack[ATTACK_ALTERNATE].ammoIndex, 0 );
	}

	// add the weapon
	other->client->ps.stats[STAT_WEAPONS] |= ( 1 << weaponNum );

	// GodOrDevil - 01.11.06 - 07:22pm #weaponCamping #test
	///sess.lastNade crashes the dll.. donno why 
	//if(weaponNum >= 14 && weaponNum <= 21){
	//	Com_Printf("weaponNum: %i - GRENADE!!!\n", weaponNum);
	//	//if(ent->client->sess.lastNadeTime < (level.time - 2000))
	//	//	ent->client->sess.lastNadeTime = level.time;
	//	//else
	//		//Com_Printf("within 20 Seconds\n");
	//}
	///End  - 01.11.06 - 07:34pm

	return g_weaponRespawn.integer;
}


//======================================================================

int Pickup_Health (gentity_t *ent, gentity_t *other) 
{
	int			quantity;

	if ( ent->count ) 
	{
		quantity = ent->count;
	} 
	else 
	{
		quantity = ent->item->quantity;
	}

	other->health += quantity;

	if (other->health > MAX_HEALTH ) 
	{
		other->health = MAX_HEALTH;
	}

	other->client->ps.stats[STAT_HEALTH] = other->health;

	return RESPAWN_HEALTH;
}

//======================================================================

int Pickup_Armor( gentity_t *ent, gentity_t *other ) 
{
	other->client->ps.stats[STAT_REALARMOR] += ent->item->quantity;

	if ( other->client->ps.stats[STAT_REALARMOR] > MAX_ARMOR ) 
	{
		other->client->ps.stats[STAT_REALARMOR] = MAX_ARMOR;
	}

	return RESPAWN_ARMOR;
}

//======================================================================

int Pickup_Gametype (gentity_t *ent, gentity_t *other) 
{
	other->client->ps.stats[STAT_GAMETYPE_ITEMS] |= (1<<ent->item->giTag);
	// KRIS 14/04/2003 5:20PM
	// for bot AI
	// had to make slight adjustment to gametype code so bots
	// could tell difference between real GTI's and dropped ones...
#ifdef _CRS_BOTS
	if ( ent->flags & FL_DROPPED_ITEM ) 
	{
		return 1;
	}
#endif
	// KRIS
	return -1;
}

int Pickup_Backpack ( gentity_t* ent, gentity_t* other )
{
	float			percent = (float)ent->item->quantity / 100.0f;
	int				i;
	playerState_t	*ps;

	ps = &other->client->ps;

#ifdef _DB_ENABLED
	other->client->pers.statinfo.packer++;
#endif
	// Fill up their health
	ps->stats[STAT_HEALTH] += MAX_HEALTH * percent;
	if ( ps->stats[STAT_HEALTH] > MAX_HEALTH )
	{
		ps->stats[STAT_HEALTH] = MAX_HEALTH;
	}
	other->health = ps->stats[STAT_HEALTH];

	// Cant get armor when you have goggles -  Ahh but yes you can ... AND WILL!!
	// GodOrDevil - 1.30.2005 - #Armor
	if ( !ps->stats[STAT_GOGGLES] || g_Armor.integer >= 1 )
//	if ( !ps->stats[STAT_GOGGLES] )
	//End
	{
		ps->stats[STAT_REALARMOR] += MAX_HEALTH * percent;
		if ( ps->stats[STAT_REALARMOR] > MAX_ARMOR )
		{
			ps->stats[STAT_REALARMOR] = MAX_ARMOR;
		}
	}

	// Give them some ammo
	for ( i = 0; i < MAX_AMMO; i ++ )
	{
		int	maxammo;

		maxammo = BG_GetMaxAmmo ( ps, i);

		if ( !maxammo || ps->ammo[i] >= maxammo )
		{
			continue;
		}

		ps->ammo[i] += Com_Clamp ( 1, maxammo, maxammo * percent );
		if ( ps->ammo[i] >= maxammo )
		{
			ps->ammo[i] = maxammo;
		}
	}

	// Make sure you alwasy get grenades
	// GodOrDevil - GAMETYPES
	//if ( level.pickupsDisabled )
	if ( level.pickupsDisabled && !NV_isFT())
	// NE
	{
		weapon_t weapon = ps->stats[STAT_OUTFIT_GRENADE];

		// If the client doesnt even have a greande then we need to give them one
		// and fill their clip.  They should have already been give ammo 
		if ( !(ps->stats[STAT_WEAPONS] & (1<<weapon)) )
		{
			int ammoIndex;

			ps->stats[STAT_WEAPONS] |= (1<<weapon);

			// Move over the ammo to a clip
			ammoIndex = weaponData[weapon].attack[ATTACK_NORMAL].ammoIndex;
			ps->ammo[ammoIndex] -= weaponData[weapon].attack[ATTACK_NORMAL].clipSize;
			ps->clip[ATTACK_NORMAL][weapon] = weaponData[weapon].attack[ATTACK_NORMAL].clipSize;
		}
	}

	return g_backpackRespawn.integer;
}

/*
===============
RespawnItem
===============
*/
void RespawnItem( gentity_t *ent ) 
{
	// randomly select from teamed entities
	if (ent->team) 
	{
		gentity_t	*master;
		int	count;
		int choice;

		if ( !ent->teammaster ) 
		{
			Com_Error( ERR_FATAL, "RespawnItem: bad teammaster");
		}
		master = ent->teammaster;

		for (count = 0, ent = master; ent; ent = ent->teamchain, count++)
			;

		choice = rand() % count;

		for (count = 0, ent = master; count < choice; ent = ent->teamchain, count++)
			;
	}

	ent->r.contents = CONTENTS_TRIGGER;
	ent->s.eFlags &= ~EF_NODRAW;
	ent->r.svFlags &= ~SVF_NOCLIENT;
	trap_LinkEntity (ent);

	// play the normal respawn sound only to nearby clients
	G_AddEvent( ent, EV_ITEM_RESPAWN, 0 );

	ent->nextthink = 0;
}


/*
===============
Touch_Item
===============
*/
void Touch_Item (gentity_t *ent, gentity_t *other, trace_t *trace) 
{
	int			respawn;
	qboolean	predict;
	qboolean	autoswitch;
	
#ifdef _MAPEDITOR
	if(nv_editMap.integer)
		return;
#endif

	if (!other->client)
		return;

	// dead people can't pickup
	if (other->health < 1)
		return;	

	// GodOrDevil - 04.01.06 - 10:07pm #Tortured cant pick shit up
#ifdef _TORTURE
	if(other->client->nvSess.torture)
		return;
#endif
	
	if(other->client->nvSess.stripped)
	{
		switch( ent->item->giType ) 
		{
			case IT_WEAPON:
				return;
			case IT_AMMO:
				return;
			case IT_ARMOR:
				return;
			case IT_HEALTH:
				return;
			case IT_BACKPACK:
				return;
			default:
				break;
		}
	}
	///End  - 04.01.06 - 10:09pm

	// See if teh item can be picked up
	if( ent->s.eFlags & EF_NOPICKUP )
	{
		return;
	}
	// KRIS 10/04/2003 12:35PM
#ifdef _CRS_BOTS
	if (bot_editWPS.integer  && (other->client->ps.clientNum == 0))
	{
		return;
	}
#endif
	// KRIS
	
#ifdef _MAPEDITOR
	if(nv_editMap.integer )
	{
		//G_FreeEntity(ent);
		return;
	}
#endif

	// If its a gametype item the gametype handles it
	if ( ent->item->giType == IT_GAMETYPE )
	{
		// Let the gametype decide if it can be picked up
 		if ( !trap_GT_SendEvent ( GTEV_ITEM_TOUCHED, level.time, ent->item->quantity, other->s.number, other->client->sess.team, 0, 0 ) )
		{
			return;
		}
	}
	// the same pickup rules are used for client side and server side
	else if ( !BG_CanItemBeGrabbed( level.gametype, &ent->s, &other->client->ps ) ) 
	{
		return;
	}

#ifdef _DEBUG
	G_LogPrintf( "Item: %i %s\n", other->s.number, ent->item->classname );
#endif

	// Initialize booleans
	predict    = other->client->pers.predictItemPickup;
	autoswitch = qfalse;

	// call the item-specific pickup function
	switch( ent->item->giType ) 
	{
		case IT_WEAPON:
			// GodOrDevil - No picking up dropped nades if m203 only.
			if(g_m203.integer && !g_m4scope.integer && ent->item->outfittingGroup == 3 && (other->client->ps.stats[STAT_WEAPONS] & ( 1 << WP_M4_ASSAULT_RIFLE )))
				return;
			///End
			respawn = Pickup_Weapon(ent, other, &autoswitch );
			break;
		case IT_AMMO:
			respawn = Pickup_Ammo(ent, other);
			break;
		case IT_ARMOR:
			respawn = Pickup_Armor(ent, other);
			break;
		case IT_HEALTH:
			respawn = Pickup_Health(ent, other);
			break;
		case IT_GAMETYPE:
			respawn = Pickup_Gametype(ent, other);
			predict = qfalse;
			break;
		case IT_BACKPACK:
			respawn = Pickup_Backpack(ent,other);
			predict = qfalse;
			break;
		default:
			return;
	}

	if ( !respawn ) 
	{
		return;
	}

	// play the normal pickup sound
	if (predict) 
	{
		G_AddPredictableEvent( other, EV_ITEM_PICKUP, ent->s.modelindex | (autoswitch?ITEM_AUTOSWITCHBIT:0) );
	} 
	else 
	{
		G_AddEvent( other, EV_ITEM_PICKUP, ent->s.modelindex | (autoswitch?ITEM_AUTOSWITCHBIT:0) );
	}

	// fire item targets
	G_UseTargets (ent, other);

	// wait of -1 will not respawn
	if ( ent->wait == -1 ) 
	{
		ent->r.svFlags |= SVF_NOCLIENT;
		ent->s.eFlags |= EF_NODRAW;
		ent->r.contents = 0;
		ent->unlinkAfterEvent = qtrue;
		return;
	}

	// non zero wait overrides respawn time
	if ( ent->wait ) 
	{
		respawn = ent->wait;
	}

	// random can be used to vary the respawn time
	if ( ent->random ) 
	{
		respawn += crandom() * ent->random;
		
		if ( respawn < 1 ) 
		{
			respawn = 1;
		}
	}

	// dropped items will not respawn
	if ( ent->flags & FL_DROPPED_ITEM ) 
	{
		ent->freeAfterEvent = qtrue;
	}

	// picked up items still stay around, they just don't
	// draw anything.  This allows respawnable items
	// to be placed on movers.
	ent->r.svFlags |= SVF_NOCLIENT;
	ent->s.eFlags |= EF_NODRAW;
	ent->r.contents = 0;

	// ZOID
	// A negative respawn times means to never respawn this item (but don't 
	// delete it).  This is used by items that are respawned by third party events
	if ( respawn <= 0 ) 
	{
		ent->nextthink = 0;
		ent->think = 0;
	} 
	else 
	{
		ent->nextthink = level.time + respawn * 1000;
		ent->think = RespawnItem;
	}

	trap_LinkEntity( ent );
}


//======================================================================

/*
================
LaunchItem

Spawns an item and tosses it forward
================
*/
gentity_t *LaunchItem( gitem_t *item, vec3_t origin, vec3_t velocity ) 
{
	gentity_t	*dropped;

	// Gametype items must be spawned using the spawn mission item function
	if ( item->giType == IT_GAMETYPE )
	{
		// GodOrDevil - 1.01 code
		//dropped = G_SpawnGametypeItem ( item->pickup_name, qtrue );
		dropped = G_SpawnGametypeItem ( item->pickup_name, qtrue, origin );
		// END
		dropped->nextthink = 0;
	}
	else
	{
		dropped = G_Spawn();
		dropped->think = G_FreeEntity;
		dropped->nextthink = level.time + 30000;
	}

	if ( !dropped )
	{
		return NULL;
	}

	dropped->s.eType = ET_ITEM;
	dropped->s.modelindex = item - bg_itemlist;	// store item number in modelindex
	dropped->s.modelindex2 = 1; // This is non-zero is it's a dropped item

	dropped->classname = item->classname;
	dropped->item = item;
	VectorSet (dropped->r.mins, -ITEM_RADIUS, -ITEM_RADIUS, -ITEM_RADIUS);
	VectorSet (dropped->r.maxs, ITEM_RADIUS, ITEM_RADIUS, ITEM_RADIUS);
	dropped->r.contents = CONTENTS_TRIGGER;

	dropped->touch = Touch_Item;

	G_SetOrigin( dropped, origin );
	dropped->s.pos.trType = TR_GRAVITY;
	dropped->s.pos.trTime = level.time;
	VectorCopy( velocity, dropped->s.pos.trDelta );

	dropped->s.eFlags |= EF_BOUNCE_HALF;

	dropped->flags = FL_DROPPED_ITEM;

	trap_LinkEntity (dropped);

	return dropped;
}

/*
================
G_DropItem

Spawns an item and tosses it forward
================
*/
gentity_t *G_DropItem( gentity_t *ent, gitem_t *item, float angle ) 
{
	vec3_t		velocity;
	vec3_t		angles;
	gentity_t*	dropped;

	VectorCopy( ent->s.apos.trBase, angles );
	angles[YAW] += angle;
	angles[PITCH] = 0;	// always forward

	AngleVectors( angles, velocity, NULL, NULL );
	VectorScale( velocity, 150, velocity );
	velocity[2] += 200 + crandom() * 50;
	
	dropped = LaunchItem( item, ent->r.currentOrigin, velocity );

	if ( item->giType == IT_GAMETYPE )
	{
		if ( level.gametypeResetTime && NV_isVIP() )
			return dropped;

		if(ent->client)
			trap_GT_SendEvent ( GTEV_ITEM_DROPPED, level.time, item->quantity, ent->s.number, ent->client->sess.team, 0, 0 );
		else
			trap_GT_SendEvent ( GTEV_ITEM_DROPPED, level.time, item->quantity, ent->s.number, 0, 0, 0 );
	}

	return dropped;
}

/*
=================
G_EnablePickup

Re-enables pickup on a entity that has it disabled
=================
*/

void G_EnablePickup ( gentity_t* ent )
{
	ent->s.eFlags &= ~EF_NOPICKUP;

	// If this is a no respawn game we can keep the weapons around till round end
	// GodOrDevil - GAMETYPES
	//if ( level.gametypeData->respawnType == RT_NONE )
	if ( level.gametypeData->respawnType == RT_NONE && !NV_isSab() && !NV_isRaze() /* && !NV_isTFT()*/)
	{	
	// END	
		return;
	}

	// Go away in 30 seconds
	ent->think = G_FreeEntity;
	ent->nextthink = level.time + 30000;
}

/*
=================
G_IsAmmoBeingShared

determins if the given ammo index is being shared by any other weapon in the players
inventory.  Excluding the specified weapon
=================
*/
qboolean G_IsAmmoBeingShared ( gentity_t* ent, int ammoIndex, weapon_t exclude )
{
	weapon_t weapon;

	// Need to figure out if this guy has any other guns that use the ammo for the gun
	// being dropped.
	for ( weapon = WP_KNIFE + 1; weapon < WP_NUM_WEAPONS; weapon ++ )
	{
		// Does the player have this weapon?
		if ( !(ent->client->ps.stats[STAT_WEAPONS] & (1<<weapon) ) )
		{
			continue;
		}

		// Dont include the weapon being dropped
		if ( exclude == weapon )
		{
			continue;
		}

		// Does this attack use the specified ammo?
		if ( weaponData[weapon].attack[ATTACK_NORMAL].ammoIndex == ammoIndex )
		{
			return qtrue;
		}

		// Does this attack use the specified ammo?
		if ( weaponData[weapon].attack[ATTACK_ALTERNATE].ammoIndex == ammoIndex )
		{
			return qtrue;
		}
	}

	return qfalse;
}

/*
=================
G_DropWeapon

Drops the weapon and all its ammo
=================
*/
gentity_t* G_DropWeapon ( gentity_t* ent, weapon_t weapon, int pickupDelay )
{
	gentity_t*	dropped;
	gitem_t*	item;
	vec3_t		angles;
	
	if ( weapon <= WP_KNIFE || weapon >= WP_NUM_WEAPONS )
	{
		return NULL;
	}

	// GodOrDevil
	if (NV_isGG())
		return NULL;
	// End

	// No more outfitting changes
	ent->client->noOutfittingChange = qtrue;
	
	// find the item type for this weapon
	item = BG_FindWeaponItem ( weapon );

	// spawn the item
	dropped = G_DropItem( ent, item, 0 );

	// Pack all the ammo into the count field
	dropped->s.angles[YAW] = rand()%360;
	dropped->count  = (ent->client->ps.clip[ATTACK_NORMAL][weapon]&0xFF);

	// If the ammo isnt being shared then send it all with the gun
	if ( !G_IsAmmoBeingShared ( ent, weaponData[weapon].attack[ATTACK_NORMAL].ammoIndex, weapon ) )
	{
		dropped->count += ((ent->client->ps.ammo[weaponData[weapon].attack[ATTACK_NORMAL].ammoIndex]<<8) & 0xFF00);
		ent->client->ps.ammo[weaponData[weapon].attack[ATTACK_NORMAL].ammoIndex] = 0;
	}

	// Dont bother if the weapon doesnt have alternate ammo
	if ( BG_WeaponHasAlternateAmmo ( weapon ) )
	{
		dropped->count += ((ent->client->ps.clip[ATTACK_ALTERNATE][weapon] << 16) & 0xFF0000 );	

		// If the ammo isnt being shared then send it all with the gun
		if ( !G_IsAmmoBeingShared ( ent, weaponData[weapon].attack[ATTACK_ALTERNATE].ammoIndex, weapon ) )
		{
			dropped->count += ((ent->client->ps.ammo[weaponData[weapon].attack[ATTACK_ALTERNATE].ammoIndex] << 24) & 0xFF000000 );
			ent->client->ps.ammo[weaponData[weapon].attack[ATTACK_ALTERNATE].ammoIndex] = 0;
		}
	}

	// Clear the clips
	ent->client->ps.clip[ATTACK_NORMAL][weapon] = 0;
	ent->client->ps.clip[ATTACK_ALTERNATE][weapon] = 0;	

	// Take the weapon away
	ent->client->ps.stats[STAT_WEAPONS] &= ~(1<<weapon);

	// if the gun is empty then just kill it soon after its dropped
#ifdef _PISTOLDROPPED
	if ( !dropped->count || weapon ==  WP_USSOCOM_PISTOL || weapon ==  WP_M1911A1_PISTOL )
#else
	if ( !dropped->count )
#endif
	{
		dropped->nextthink = level.time + 2500;
		dropped->think     = G_FreeEntity;
		dropped->s.eFlags |= EF_NOPICKUP;
	}
	// Dont allow the item to be picked up againt for 3 seconds if in a no pickup game, otherwise
	// let them pick it up immediately
	else if ( pickupDelay )
	{
		dropped->nextthink = level.time + 3000;	
		dropped->s.eFlags |= EF_NOPICKUP;
		dropped->think = G_EnablePickup;
	}
	// Always need a tad bit of delay on pickup for prediction issues
	else
	{
		dropped->nextthink = level.time + 200;	
		dropped->s.eFlags |= EF_NOPICKUP;
		dropped->think = G_EnablePickup;
	}

	// Throw the gun forward
	VectorCopy( ent->s.apos.trBase, angles );
	angles[PITCH] = 0;

	// Some random velocity
	AngleVectors( angles, ent->s.pos.trDelta, NULL, NULL );
	VectorScale( ent->s.pos.trDelta, 150, ent->s.pos.trDelta );
	ent->s.pos.trDelta[2] += 200 + random() * 50;

	return dropped;
}

/*
================
Use_Item

Respawn the item
================
*/
void Use_Item( gentity_t *ent, gentity_t *other, gentity_t *activator ) 
{
	RespawnItem( ent );
}

//======================================================================

/*
================
FinishSpawningItem

Traces down to find where an item should rest, instead of letting them
free fall from their spawn points
================
*/
void FinishSpawningItem( gentity_t *ent ) 
{
	trace_t		tr;
	vec3_t		dest;
	vec3_t		src;
//	gitem_t		*item;

	VectorSet( ent->r.mins, -ITEM_RADIUS, -ITEM_RADIUS, -ITEM_RADIUS );
	VectorSet( ent->r.maxs, ITEM_RADIUS, ITEM_RADIUS, ITEM_RADIUS );

	ent->s.eType = ET_ITEM;
	ent->s.modelindex = ent->item - bg_itemlist;		// store item number in modelindex
	ent->s.modelindex2 = 0; // zero indicates this isn't a dropped item

	ent->r.contents = CONTENTS_TRIGGER;
	ent->touch = Touch_Item;
	// useing an item causes it to respawn
	ent->use = Use_Item;

	// create a Ghoul2 model if the world model is a glm
/*	item = &bg_itemlist[ ent->s.modelindex ];
	if (!stricmp(&item->world_model[0][strlen(item->world_model[0]) - 4], ".glm"))
	{
		trap_G2API_InitGhoul2Model(&ent->s, item->world_model[0], G_ModelIndex(item->world_model[0] ), 0, 0, 0, 0);
		ent->s.radius = 60;
	}
*/
	if ( ent->item->giType != IT_GAMETYPE && (ent->spawnflags & 1) ) 
	{
		// suspended
		G_SetOrigin( ent, ent->s.origin );
	} 
	else 
	{
		// drop to floor
		VectorSet( src, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2] + 1 );
		VectorSet( dest, ent->s.origin[0], ent->s.origin[1], ent->s.origin[2] - 4096 );
		trap_Trace( &tr, src, ent->r.mins, ent->r.maxs, dest, ent->s.number, MASK_SOLID );
		if ( tr.startsolid ) 
		{
			Com_Printf ("FinishSpawningItem: %s startsolid at %s\n", ent->classname, vtos(ent->s.origin));
			G_FreeEntity( ent );
			return;
		}

		// GodOrDevil - GAMETYPES #dom - update the mins/maxs for dom flags
		/// ... change min/max here to avoid "starting in solid" message
		/// ... since they are a pretty big area's in DOM
		if(ent->item->giType == IT_GAMETYPE)
		{
			if( NV_isDom() || NV_isKoth())
			{	
				ent->s.eFlags |= EF_NODRAW;
				VectorSet( ent->r.mins, -ITEM_RADIUS * 10, -ITEM_RADIUS * 10, -ITEM_RADIUS );
				VectorSet( ent->r.maxs, ITEM_RADIUS * 10, ITEM_RADIUS * 10, ITEM_RADIUS * 5 );
			} 
			else if (NV_isVIP())
			{
				ent->nextthink = level.time + 20000;
				ent->think = G_FreeEntity;
			}
		}
		// END

		// allow to ride movers
		ent->s.groundEntityNum = tr.entityNum;

		G_SetOrigin( ent, tr.endpos );
	}

	// team slaves and targeted items aren't present at start
	if ( ( ent->flags & FL_TEAMSLAVE ) || ent->targetname ) {
		ent->s.eFlags |= EF_NODRAW;
		ent->r.contents = 0;
		return;
	}

	// Gametype items are broadcast
	if ( ent->item->giType == IT_GAMETYPE )
	{
		ent->r.svFlags |= SVF_BROADCAST;
	}

	trap_LinkEntity (ent);
}


qboolean	itemRegistered[MAX_ITEMS];

/*
==============
ClearRegisteredItems
==============
*/
void ClearRegisteredItems( void ) 
{
	memset( itemRegistered, 0, sizeof( itemRegistered ) );

	// players always start with the base weapon
	RegisterItem( BG_FindWeaponItem ( WP_FIRST_RANGED_WEAPON ) );
	RegisterItem( BG_FindWeaponItem ( WP_KNIFE ) );
}

/*
===============
RegisterItem

The item will be added to the precache list
===============
*/
void RegisterItem( gitem_t *item ) 
{
	if ( !item ) 
	{
		Com_Printf( "RegisterItem: NULL" );
	}

	itemRegistered[ item - bg_itemlist ] = qtrue;
}

/*
===============
SaveRegisteredItems

Write the needed items to a config string
so the client will know which ones to precache
===============
*/
void SaveRegisteredItems( void ) 
{
	char	string[MAX_ITEMS+1];
	int		i;
	int		count;

	count = 0;
	for ( i = 0 ; i < bg_numItems ; i++ ) 
	{
		if ( itemRegistered[i] ) 
		{
			count++;
			string[i] = '1';
		}
		else 
		{
			string[i] = '0';
		}
	}
	
	string[ bg_numItems ] = 0;

	Com_Printf( "%i items registered\n", count );
	trap_SetConfigstring(CS_ITEMS, string);
}

/*
============
G_ItemDisabled
============
*/
int G_ItemDisabled( gitem_t *item ) 
{
	char name[128];
	Com_sprintf(name, sizeof(name), "disable_%s", item->classname);
	return trap_Cvar_VariableIntegerValue( name );
}

/*
============
G_SpawnItem

Sets the clipping size and plants the object on the floor.

Items can't be immediately dropped to floor, because they might
be on an entity that hasn't spawned yet.
============
*/
void G_SpawnItem (gentity_t *ent, gitem_t *item) 
{
	// Weapons can be disabled
	if ( item->giType == IT_WEAPON )
	{
		if ( !BG_IsWeaponAvailableForOutfitting ( item->giTag, 1 ) )
		{
			return;
		}
	}	

	G_SpawnFloat( "random", "0", &ent->random );
	G_SpawnFloat( "wait", "0", &ent->wait );

	RegisterItem( item );
	if ( G_ItemDisabled(item) )
		return;

	ent->item = item;
	// some movers spawn on the second frame, so delay item
	// spawns until the third frame so they can ride trains
	ent->nextthink = level.time + FRAMETIME * 2;
	ent->think = FinishSpawningItem;

	ent->physicsBounce = 0.50;		// items are bouncy
}


/*
================
G_BounceItem

================
*/
void G_BounceItem( gentity_t *ent, trace_t *trace ) {
	vec3_t	velocity;
	float	dot;
	int		hitTime;

	// reflect the velocity on the trace plane
	hitTime = level.previousTime + ( level.time - level.previousTime ) * trace->fraction;
	BG_EvaluateTrajectoryDelta( &ent->s.pos, hitTime, velocity );
	dot = DotProduct( velocity, trace->plane.normal );
	VectorMA( velocity, -2*dot, trace->plane.normal, ent->s.pos.trDelta );

	// cut the velocity to keep from bouncing forever
	VectorScale( ent->s.pos.trDelta, ent->physicsBounce, ent->s.pos.trDelta );

	// check for stop
	if ( trace->plane.normal[2] > 0 && ent->s.pos.trDelta[2] < 40 ) {
		trace->endpos[2] += 1.0;	// make sure it is off ground
		SnapVector( trace->endpos );
		G_SetOrigin( ent, trace->endpos );
		ent->s.groundEntityNum = trace->entityNum;
		return;
	}

	VectorAdd( ent->r.currentOrigin, trace->plane.normal, ent->r.currentOrigin);
	VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
	ent->s.pos.trTime = level.time;
}


/*
================
G_RunItem
================
*/
void G_RunItem( gentity_t *ent ) 
{
	vec3_t		origin;
	trace_t		tr;
	int			contents;
	int			mask;

		// if groundentity has been set to -1, it may have been pushed off an edge
	if ( ent->s.groundEntityNum == -1 ) 
	{
		if ( ent->s.pos.trType != TR_GRAVITY ) 
		{
			ent->s.pos.trType = TR_GRAVITY;
			ent->s.pos.trTime = level.time;
		}
	}
		
	// GodOrDevil - GAMETYPES #dom
#ifdef _MAPEDITOR
	if(!nv_editMap.integer &&  ent->item != NULL && ent->item->giType == IT_GAMETYPE && (NV_isDom() || NV_isKoth()))
#else
	if(ent->item != NULL && ent->item->giType == IT_GAMETYPE && (NV_isDom() || NV_isKoth()))
#endif
	{
		int		i;
		qboolean isTouched = qfalse;
		for ( i = 0; i < level.numConnectedClients; i ++ )
		{
			if(level.clients[ level.sortedClients[i]].nvSess.capturing == ent->s.number && !G_IsClientDead ( &level.clients[ level.sortedClients[i]] ))
			{
				//Com_Printf("Touching: %s - %i/%i", level.clients[level.sortedClients[i]].pers.netname, ent->s.number, level.clients[ level.sortedClients[i]].sess.capturing  );
				isTouched = qtrue;
				continue;
			}
			//Com_Printf("Not Touching: %i",  ent->s.number );
		}
		if(!isTouched && ent->count){
			ent->count = 0;
		}		
	}
	///End

	if ( ent->s.pos.trType == TR_STATIONARY ) 
	{
		// GodOrDevil
		if( g_noDropOrigin.integer != 0 && ent->item && ent->item->giType == IT_GAMETYPE && ent->r.currentOrigin[2] <= g_noDropOrigin.integer)
		{	
			if ( !trap_GT_SendEvent ( GTEV_ITEM_STUCK, level.time, ent->item->quantity, 0, 0, 0, 0 ) )
			{
				G_ResetGametypeItem ( ent->item );
			}
			G_FreeEntity( ent );
			return;
		}
		// End
		// check think function
		G_RunThink( ent );
		return;
	}

	// get current position
	BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );

	// trace a line from the previous position to the current position
	if ( ent->clipmask ) 
	{
		mask = ent->clipmask;
	} 
	else 
	{
		mask = MASK_PLAYERSOLID & ~CONTENTS_BODY;//MASK_SOLID;
	}

	trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, ent->r.ownerNum, mask );

	VectorCopy( tr.endpos, ent->r.currentOrigin );

	if ( tr.startsolid ) 
	{
		tr.fraction = 0;
	}

	// FIXME: avoid this for stationary?
	trap_LinkEntity( ent );	

	// check think function
	G_RunThink( ent );

	if ( tr.fraction == 1 ) 
	{
		return;
	}

	// if it is in a nodrop volume, remove it
	contents = trap_PointContents( ent->r.currentOrigin, -1 );
	if ( contents & CONTENTS_NODROP ) 
	{
		// Gametype items are reported to the gametype when they are stuck like this
		if ( ent->item && ent->item->giType == IT_GAMETYPE )
		{
			// Let the gametype handle the problem, if it doenst handle it and return 1 then 
			// just reset the gametype item
			if ( !trap_GT_SendEvent ( GTEV_ITEM_STUCK, level.time, ent->item->quantity, 0, 0, 0, 0 ) )
			{
				G_ResetGametypeItem ( ent->item );
			}
		}

		G_FreeEntity( ent );
		return;
	}

	G_BounceItem( ent, &tr );
}

gentity_t *CreateWeaponPickup(vec3_t pos,weapon_t weapon)
{
	gentity_t	*dropped;
	gitem_t		*item;
	
	if ( weapon < WP_KNIFE || weapon >= WP_NUM_WEAPONS )
	{
		return 0;
	}

//#ifdef _DEBUG
	//if(weapon == WP_KNIFE)
	//{
	//	Com_Printf("[^3%-2i^7] ^7%-15s  %i\n", weapon, "ent count", level.num_entities);
	//	return 0;
	//}
//#endif

	item = BG_FindWeaponItem (weapon);
	if(!item)
	{
		return(0);
	}

	dropped = G_Spawn();
	dropped->s.eType = ET_ITEM;
	dropped->s.modelindex = item - bg_itemlist;	// store item number in modelindex
	dropped->s.modelindex2 = 1;					// This is non-zero is it's a dropped item

	dropped->classname = item->classname;
	dropped->item = item;
	VectorSet (dropped->r.mins, -ITEM_RADIUS, -ITEM_RADIUS, -ITEM_RADIUS);
	VectorSet (dropped->r.maxs, ITEM_RADIUS, ITEM_RADIUS, ITEM_RADIUS);
	dropped->r.contents = CONTENTS_TRIGGER;

	dropped->touch = Touch_Item;

	G_SetOrigin( dropped, pos );
	dropped->s.pos.trType = TR_GRAVITY;
	dropped->s.pos.trTime = level.time;
	VectorSet( dropped->s.pos.trDelta,0.0,0.0,0.0);

	dropped->s.eFlags |= EF_BOUNCE_HALF;
	dropped->flags = FL_DROPPED_ITEM;

	trap_LinkEntity (dropped);

//#ifdef _DEBUG
//	Com_Printf("[^3%-2i^7] ^7%-15s\n", weapon, dropped->classname);
//#endif

	return dropped;
}
