// Copyright (C) 2001-2002 Raven Software
//
#include "g_local.h"


void InitTrigger( gentity_t *self ) {
	if (!VectorCompare (self->s.angles, vec3_origin))
		G_SetMovedir (self->s.angles, self->movedir);

	trap_SetBrushModel( self, self->model );
	self->r.contents = CONTENTS_TRIGGER;		// replaces the -1 from trap_SetBrushModel
	self->r.svFlags = SVF_NOCLIENT;
}


// the wait time has passed, so set back up for another activation
void multi_wait( gentity_t *ent ) {
	ent->nextthink = 0;
}


// the trigger was just activated
// ent->activator should be set to the activator so it can be held through a delay
// so wait for the delay time before firing
void multi_trigger( gentity_t *ent, gentity_t *activator ) {

	ent->activator = activator;
	if ( ent->nextthink ) {
		return;		// can't retrigger until the wait is over
	}

	// GodOrDevil - 03.25.07 - 09:43pm - for saving ent files easier, check for health 
	/// so doors with spawnflags can be accessed by both teams (unless health)
	///if ( activator->client ) {
	if ( activator->client && ent->health == 1000) {
		if ( ( ent->spawnflags & 1 ) && activator->client->sess.team != TEAM_RED ) 
		{
			return;
		}
		
		if ( ( ent->spawnflags & 2 ) && activator->client->sess.team != TEAM_BLUE ) 
		{
			return;
		}
	}

	G_UseTargets (ent, ent->activator);

	if ( ent->wait > 0 ) {
		ent->think = multi_wait;
		ent->nextthink = level.time + ( ent->wait + ent->random * crandom() ) * 1000;
	} else {
		// we can't just remove (self) here, because this is a touch function
		// called while looping through area links...
		ent->touch = 0;
		ent->nextthink = level.time + FRAMETIME;
		ent->think = G_FreeEntity;
	}
}

void Use_Multi( gentity_t *ent, gentity_t *other, gentity_t *activator ) {
	multi_trigger( ent, activator );
}

void Touch_Multi( gentity_t *self, gentity_t *other, trace_t *trace ) {
	if( !other->client ) {
		return;
	}
	multi_trigger( self, other );
}

/*QUAKED trigger_multiple (.5 .5 .5) ?
"wait" : Seconds between triggerings, 0.5 default, -1 = one time only.
"random"	wait variance, default is 0
Variable sized repeatable trigger.  Must be targeted at one or more entities.
so, the basic time between firing is a random time between
(wait - random) and (wait + random)
*/
void SP_trigger_multiple( gentity_t *ent ) 
{
	// This is a hack because the single player game has usable triggers
	// in shared bsp instances, since multiplayer doesnt have useable triggers
	// they end up just triggering when walked through.  So for now just toss them out
	// GodOrDevil - 03.15.07 - 03:10am
	//if ( ent->spawnflags & 0x2 )
	//{
	//	G_FreeEntity ( ent );
	//	return;
	//}
	///End  - 03.15.07 - 03:10am

	G_SpawnFloat( "wait", "0.5", &ent->wait );
	G_SpawnFloat( "random", "0", &ent->random );

	if ( ent->random >= ent->wait && ent->wait >= 0 ) 
	{
		ent->random = ent->wait - FRAMETIME;
		Com_Printf( "trigger_multiple has random >= wait\n" );
	}

	ent->touch = Touch_Multi;
	ent->use = Use_Multi;
	// GodOrDevil - 09.30.06 - 08:17pm #spMaps
	if(!Q_stricmp(ent->model, "BLOCKED_TRIGGER")){
		if (!VectorCompare (ent->s.angles, vec3_origin))
			G_SetMovedir (ent->s.angles, ent->movedir);
		ent->r.contents = CONTENTS_TRIGGER;
		ent->r.svFlags = SVF_NOCLIENT;
		trap_LinkEntity (ent);
		return;
	}
	///End  - 09.30.06 - 08:18pm

	InitTrigger( ent );
	trap_LinkEntity (ent);
}



/*
==============================================================================

trigger_always

==============================================================================
*/

void trigger_always_think( gentity_t *ent ) {
	G_UseTargets(ent, ent);
	G_FreeEntity( ent );
}

/*QUAKED trigger_always (.5 .5 .5) (-8 -8 -8) (8 8 8)
This trigger will always fire.  It is activated by the world.
*/
void SP_trigger_always (gentity_t *ent) {
	// we must have some delay to make sure our use targets are present
	ent->nextthink = level.time + 300;
	ent->think = trigger_always_think;
}


/*
==============================================================================

trigger_push

==============================================================================
*/

void trigger_push_touch (gentity_t *self, gentity_t *other, trace_t *trace ) 
{
}


/*
=================
AimAtTarget

Calculate origin2 so the target apogee will be hit
=================
*/
void AimAtTarget( gentity_t *self ) {
	gentity_t	*ent;
	vec3_t		origin;
	float		height, gravity, time, forward;
	float		dist;

	VectorAdd( self->r.absmin, self->r.absmax, origin );
	VectorScale ( origin, 0.5, origin );

	ent = G_PickTarget( self->target );
	if ( !ent ) {
		G_FreeEntity( self );
		return;
	}

	height = ent->s.origin[2] - origin[2];
	gravity = g_gravity.value;
	time = sqrt( height / ( .5 * gravity ) );
	if ( !time ) {
		G_FreeEntity( self );
		return;
	}

	// set s.origin2 to the push velocity
	VectorSubtract ( ent->s.origin, origin, self->s.origin2 );
	self->s.origin2[2] = 0;
	dist = VectorNormalize( self->s.origin2);

	forward = dist / time;
	VectorScale( self->s.origin2, forward, self->s.origin2 );

	self->s.origin2[2] = time * gravity;
}


/*QUAKED trigger_push (.5 .5 .5) ?
Must point at a target_position, which will be the apex of the leap.
This will be client side predicted, unlike target_push
*/
void SP_trigger_push( gentity_t *self ) {
	InitTrigger (self);

	// unlike other triggers, we need to send this one to the client
	self->r.svFlags &= ~SVF_NOCLIENT;

	// make sure the client precaches this sound
	G_SoundIndex("sound/world/jumppad.wav");
	self->s.eType = ET_PUSH_TRIGGER;
	self->touch = trigger_push_touch;
	self->think = AimAtTarget;
	self->nextthink = level.time + FRAMETIME;
	trap_LinkEntity (self);
}


void Use_target_push( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	if ( !activator->client ) {
		return;
	}

	if ( activator->client->ps.pm_type != PM_NORMAL ) {
		return;
	}

	VectorCopy (self->s.origin2, activator->client->ps.velocity);

	// play fly sound every 1.5 seconds
	if ( activator->fly_sound_debounce_time < level.time ) {
		activator->fly_sound_debounce_time = level.time + 1500;
		G_Sound( activator, CHAN_AUTO, self->noise_index );
	}
}

/*QUAKED target_push (.5 .5 .5) (-8 -8 -8) (8 8 8) bouncepad
Pushes the activator in the direction.of angle, or towards a target apex.
"speed"		defaults to 1000
if "bouncepad", play bounce noise instead of windfly
*/
void SP_target_push( gentity_t *self ) {
	if (!self->speed) {
		self->speed = 1000;
	}
	G_SetMovedir (self->s.angles, self->s.origin2);
	VectorScale (self->s.origin2, self->speed, self->s.origin2);

	if ( self->spawnflags & 1 ) {
		self->noise_index = G_SoundIndex("sound/world/jumppad.wav");
	} else {
	//	self->noise_index = G_SoundIndex("sound/misc/windfly.wav");
		self->noise_index = G_SoundIndex("movers/doors/hitech_door02/hitech_open.mp3");
	}
	if ( self->target ) {
		VectorCopy( self->s.origin, self->r.absmin );
		VectorCopy( self->s.origin, self->r.absmax );
		self->think = AimAtTarget;
		self->nextthink = level.time + FRAMETIME;
	}
	self->use = Use_target_push;
}

/*
==============================================================================

trigger_teleport

==============================================================================
*/

void trigger_teleporter_touch (gentity_t *self, gentity_t *other, trace_t *trace ) {
	gentity_t	*dest;

	if ( !other->client ) {
		return;
	}
	if ( other->client->ps.pm_type == PM_DEAD ) {
		return;
	}
	// Spectators only?
	if ( ( self->spawnflags & 1 ) && !G_IsClientSpectating ( other->client ) ) 
	{
		return;
	}


	dest = 	G_PickTarget( self->target );
	if (!dest) {
		Com_Printf ("Couldn't find teleporter destination\n");
		return;
	}

	TeleportPlayer( other, dest->s.origin, dest->s.angles );
}


/*QUAKED trigger_teleport (.5 .5 .5) ? SPECTATOR
Allows client side prediction of teleportation events.
Must point at a target_position, which will be the teleport destination.

If spectator is set, only spectators can use this teleport
Spectator teleporters are not normally placed in the editor, but are created
automatically near doors to allow spectators to move through them
*/
void SP_trigger_teleport( gentity_t *self ) {
	InitTrigger (self);

	// unlike other triggers, we need to send this one to the client
	// unless is a spectator trigger
	if ( self->spawnflags & 1 ) {
		self->r.svFlags |= SVF_NOCLIENT;
	} else {
		self->r.svFlags &= ~SVF_NOCLIENT;
	}

	// make sure the client precaches this sound
	G_SoundIndex("sound/world/jumppad.wav");

	self->s.eType = ET_TELEPORT_TRIGGER;
	self->touch = trigger_teleporter_touch;

	trap_LinkEntity (self);
}



// GodOrDevil - 10.01.06
void NV_blocked_trigger	(gentity_t *ent)
{	
	ent->r.svFlags = SVF_NOCLIENT;
	ent->s.eType =	ET_TERRAIN;	
	ent->r.contents = MASK_SHOT;

	if ( ent->damage ) 
	{
		ent->noise_index = 0;
		ent->touch = hurt_touch;
		ent->use = hurt_use;
		ent->methodOfDeath = MOD_TRIGGER_HURT;
	}

	trap_LinkEntity (ent);
}

void NV_blocked_Teleport	(gentity_t *ent)
{
	ent->r.contents = CONTENTS_PLAYERCLIP;
	ent->r.svFlags = SVF_NOCLIENT;
	ent->s.eType = ET_TELEPORT_TRIGGER;

	ent->touch = trigger_teleporter_touch;
	trap_LinkEntity (ent);
}
	
gentity_t* G_RealSpawnGametypeItem ( gitem_t* item, vec3_t origin, vec3_t angles, qboolean dropped );
gentity_t *NV_GetTeamVIP(int team)
{
	char		location[64], *s, *c, *a;
	char		chat[128];

	int			i;

	for ( i = 0; i < level.numConnectedClients; i ++ )
	{
		gentity_t* eVIP = &g_entities[level.sortedClients[i]];

		if ( eVIP->client->pers.connected != CON_CONNECTED )
			continue;
				
		if(eVIP->client->nvSess.vip != 1)
			continue;

		if ( G_IsClientDead ( eVIP->client ) )
			continue;
		
		if (eVIP->client->sess.team == team)
			continue;

		if (!(eVIP->client->sess.team == TEAM_RED || eVIP->client->sess.team == TEAM_BLUE))
			continue;

		Team_GetLocationMsg(eVIP, location, sizeof(location));
		if(team == TEAM_BLUE)
		{
			s = "^iR^ya^fd^+a^7r";
			c = "^y";
			a = "sound/enemy/english/male/investigate_area.mp3";
		}
		else
		{
			s = "^$R^Ta^od^+a^7r";	
			c = "^1";
			a = "sound/enemy/english/fmale/investigate_area.mp3";

		}
			
		if(location)
			Com_sprintf(chat, 128, "%s has detected enemy movement at %s%s!", s, c, location);
		else	
			Com_sprintf(chat, 128, "%s has detected enemy movement!", s);
		
		TeamSound(team, a, chat);

		return eVIP;
	}
	return NULL;
}
void vip_marker_wait (gentity_t *self)
{
	self->nextthink = 0;
	self->think = 0;
	trap_LinkEntity (self);
}
void vip_marker_scantargets( gentity_t *self ) 
{
	gentity_t *find, *eVIP;
	gitem_t* item;
	vec3_t dir = {10,10,100};
	vec3_t o, up;	
	int		flareColor = EFFECT_FLARE_RED;

	eVIP = NV_GetTeamVIP(self->spawnflags);

	if(eVIP == NULL)
	{	
		//G_FreeEntity ( self );
		return;
	}

	if(self->spawnflags == TEAM_BLUE)
		item = BG_FindGametypeItemByID ( ITEM_REDFLAG );
	else if(self->spawnflags == TEAM_RED)
		item = BG_FindGametypeItemByID ( ITEM_BLUEFLAG );
	else return;

	if ( !item )
		return;

	// Remove all spawned instances of the item on the map
	find = NULL;
	while ( NULL != (find = G_Find ( find, FOFS(classname), item->classname ) ) )
	{
		if(find->spawnflags == self->spawnflags)
			continue;
		// Free this entity soon
		find->nextthink = level.time + 10;
		find->think = G_FreeEntity;
	}

	if(eVIP->client->sess.team == TEAM_RED)
	{
		flareColor = EFFECT_FLARE_RED;
	}
	else
	{
		flareColor = EFFECT_FLARE_BLUE;
	}

	VectorSet(up, 0, 0, -1);
	VectorCopy(eVIP->r.currentOrigin, o);
	//o[2] = level.worldMaxs[2] - level.worldMins[2] - 300;
	o[2] += 400;
	G_PlayEffect(level.effects[flareColor], o, up);

	// spawn the item and set the new item ent as dropped so we can clear on round reset
	find = G_RealSpawnGametypeItem ( item, eVIP->r.currentOrigin, vec3_origin, qfalse );
	find->flags |= FL_DROPPED_ITEM;
	find->spawnflags = eVIP->client->sess.team;

	//G_FreeEntity ( self );	
 	self->nextthink = level.time + 20000;
	self->think     = vip_marker_wait;
	trap_UnlinkEntity (self);
}

	
void vip_marker_touch( gentity_t *self, gentity_t *other, trace_t *trace ) 
{
#ifdef _MAPEDITOR
	if(nv_editMap.integer)
		return;
#endif
	if(!other || !other->client)
		return;
	//if(other->client->nvSess.vip != 1)
	//	return;
	if(self->nextthink > level.time)
		return;	
	if (G_IsClientDead ( other->client ) || G_IsClientSpectating( other->client))
		return;

	if(other->client->sess.team == TEAM_BLUE)
		RPM_ClientSound(other, G_SoundIndex("sound/enemy/english/male/report_targets.mp3"));
	else
		RPM_ClientSound(other, G_SoundIndex("sound/enemy/english/fmale/report_targets.mp3"));
	
	self->spawnflags = other->client->sess.team;
	self->nextthink = level.time + 5000;
	self->think     = vip_marker_scantargets;
}

void NV_model( gentity_t *ent );
void NV_trigger_vip_marker( gentity_t *ent ) 
{		
	if (!VectorCompare (ent->s.angles, vec3_origin))
		G_SetMovedir (ent->s.angles, ent->movedir);

	if(ent->model)
	{
		NV_model(ent);
	}
	ent->r.contents = CONTENTS_TRIGGER;
	//ent->r.svFlags = SVF_NOCLIENT;
	trap_LinkEntity (ent);
	ent->noise_index = 0; // G_SoundIndex( "sound/world/electro.wav" );
	ent->touch = vip_marker_touch;
}

#ifdef _OLDVIP

void health_use( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	if ( self->r.linked ) {
		trap_UnlinkEntity( self );
	} else {
		trap_LinkEntity( self );
	}
}

void health_touch( gentity_t *self, gentity_t *other, trace_t *trace ) 
{
	int startHealth = 100;

	if(!other->client || (other->client->ps.stats[STAT_HEALTH]>=100 && other->client->ps.stats[STAT_REALARMOR]>=100))
	{
		return;
	}

	if ( self->timestamp > level.time ) {
		return;
	}

	if ( self->spawnflags & 16 ) {
		self->timestamp = level.time + 1000;
	} else {
		self->timestamp = level.time + FRAMETIME;
	}

/*
	// play sound
	if ( !(self->spawnflags & 4) ) {
		G_Sound( other, CHAN_AUTO, self->noise_index );
	}
*/		
	startHealth = other->client->ps.stats[STAT_HEALTH];
	other->client->lasthurt_time = level.time + 5000;
	if(g_enableHealthGains.integer)
	{
		other->client->ps.stats[STAT_HEALTH] = other->health += g_enableHealthGains.value;
		other->client->ps.stats[STAT_REALARMOR] += g_enableHealthGains.value;
		if(other->client->ps.stats[STAT_HEALTH]>100)
			other->client->ps.stats[STAT_HEALTH] = other->health = MAX_HEALTH;
		if(other->client->ps.stats[STAT_REALARMOR]>100)
			other->client->ps.stats[STAT_REALARMOR] = MAX_ARMOR;
		self->damage-=g_enableHealthGains.value;
	}
	else
	{
		other->client->ps.stats[STAT_HEALTH] = other->health += 5.25;
		 other->client->ps.stats[STAT_REALARMOR] += 5;
		if(other->client->ps.stats[STAT_HEALTH] > MAX_HEALTH)
			other->client->ps.stats[STAT_HEALTH] = other->health = MAX_HEALTH;
		if(other->client->ps.stats[STAT_REALARMOR]>100)
			other->client->ps.stats[STAT_REALARMOR] = MAX_ARMOR;
	}

	startHealth = other->client->ps.stats[STAT_HEALTH] - startHealth;
	self->damage -= startHealth;
	if(self->damage <= 0)
	{
		self->r.linked = qfalse;
		trap_UnlinkEntity( self );
	}
}
void NV_trigger_health( gentity_t *ent ) 
{		
	if (!VectorCompare (ent->s.angles, vec3_origin))
		G_SetMovedir (ent->s.angles, ent->movedir);
	ent->r.contents = CONTENTS_TRIGGER;
	ent->r.svFlags = SVF_NOCLIENT;
	trap_LinkEntity (ent);


	ent->noise_index = 0; // G_SoundIndex( "sound/world/electro.wav" );
	ent->touch = health_touch;

	if ( !ent->damage ) {
		ent->damage = 200;
	}

	ent->r.contents = CONTENTS_TRIGGER;

	if ( ent->spawnflags & 2 ) {
		ent->use = health_use;
	}

	// link in to the world if starting active
	if ( ! (ent->spawnflags & 1) ) {
		trap_LinkEntity (ent);
	}
}
#endif
///END

/*
==============================================================================

trigger_hurt

==============================================================================
*/

/*QUAKED trigger_hurt (.5 .5 .5) ? START_OFF - SILENT NO_PROTECTION SLOW
Any entity that touches this will be hurt.
It does dmg points of damage each server frame
Targeting the trigger will toggle its on / off state.

SILENT			supresses playing the sound
SLOW			changes the damage rate to once per second
NO_PROTECTION	*nothing* stops the damage

"dmg"			default 5 (whole numbers only)

*/
void hurt_use( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	if ( self->r.linked ) {
		trap_UnlinkEntity( self );
	} else {
		trap_LinkEntity( self );
	}
}

void hurt_touch( gentity_t *self, gentity_t *other, trace_t *trace ) {
	int		dflags;

	if ( !other->takedamage ) {
		return;
	}

	if ( self->timestamp > level.time ) {
		return;
	}

	if ( self->spawnflags & 16 ) {
		self->timestamp = level.time + 1000;
	} else {
		self->timestamp = level.time + FRAMETIME;
	}

/*
	// play sound
	if ( !(self->spawnflags & 4) ) {
		G_Sound( other, CHAN_AUTO, self->noise_index );
	}
*/

	if (self->spawnflags & 8)
		dflags = DAMAGE_NO_PROTECTION;
	else
		dflags = 0;
	G_Damage (other, self, self, NULL, NULL, self->damage, dflags, MOD_TRIGGER_HURT, HL_NONE );
}

void SP_trigger_hurt( gentity_t *self ) {
	InitTrigger (self);

	self->noise_index = 0; // G_SoundIndex( "sound/world/electro.wav" );
	self->touch = hurt_touch;

	if ( !self->damage ) {
		self->damage = 5;
	}

	self->r.contents = CONTENTS_TRIGGER;

	if ( self->spawnflags & 2 ) {
		self->use = hurt_use;
	}

	// link in to the world if starting active
	if ( ! (self->spawnflags & 1) ) {
		trap_LinkEntity (self);
	}
}

/*QUAKED trigger_ladder (.5 .5 .5) ? 
Indicates a ladder and its normal

"angles"		angle ladder faces
*/
void SP_trigger_ladder ( gentity_t* self )
{
	vec3_t fwd;

	trap_LinkEntity ( self );

	trap_SetConfigstring( CS_LADDERS + level.ladderCount++, 
						  va("%i,%i,%i,%i,%i,%i,%i", 
							 (int)self->r.absmin[0],(int)self->r.absmin[1],(int)self->r.absmin[2],
							 (int)self->r.absmax[0],(int)self->r.absmax[1],(int)self->r.absmax[2],
							 (int)self->s.angles[YAW] ) );

	// Only need integer resolution
	self->r.absmin[0] = (int)self->r.absmin[0];
	self->r.absmin[1] = (int)self->r.absmin[1];
	self->r.absmin[2] = (int)self->r.absmin[2];
	self->r.absmax[0] = (int)self->r.absmax[0];
	self->r.absmax[1] = (int)self->r.absmax[1];
	self->r.absmax[2] = (int)self->r.absmax[2];
	
	self->s.angles[PITCH] = self->s.angles[ROLL] = 0;
	self->s.angles[YAW] = (int)self->s.angles[YAW];
	AngleVectors( self->s.angles, fwd, 0, 0);

	BG_AddLadder ( self->r.absmin, self->r.absmax, fwd );

	G_FreeEntity ( self );
}	

/*
==============================================================================

timer

==============================================================================
*/


/*QUAKED func_timer (0.3 0.1 0.6) (-8 -8 -8) (8 8 8) START_ON
This should be renamed trigger_timer...
Repeatedly fires its targets.
Can be turned on or off by using.

"wait"			base time between triggering all targets, default is 1
"random"		wait variance, default is 0
so, the basic time between firing is a random time between
(wait - random) and (wait + random)

*/
void func_timer_think( gentity_t *self ) {
	G_UseTargets (self, self->activator);
	// set time before next firing
	self->nextthink = level.time + 1000 * ( self->wait + crandom() * self->random );
}

void func_timer_use( gentity_t *self, gentity_t *other, gentity_t *activator ) {
	self->activator = activator;

	// if on, turn it off
	if ( self->nextthink ) {
		self->nextthink = 0;
		return;
	}

	// turn it on
	func_timer_think (self);
}

void SP_func_timer( gentity_t *self ) {
	G_SpawnFloat( "random", "1", &self->random);
	G_SpawnFloat( "wait", "1", &self->wait );

	self->use = func_timer_use;
	self->think = func_timer_think;

	if ( self->random >= self->wait ) {
		self->random = self->wait - FRAMETIME;
		Com_Printf( "func_timer at %s has random >= wait\n", vtos( self->s.origin ) );
	}

	if ( self->spawnflags & 1 ) {
		self->nextthink = level.time + FRAMETIME;
		self->activator = self;
	}

	self->r.svFlags = SVF_NOCLIENT;
}


