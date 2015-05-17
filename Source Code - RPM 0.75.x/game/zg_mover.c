// GodOrDevil

#include "g_local.h"

void MatchTeam			(gentity_t *teamLeader, int moverState, int time);
void Use_BinaryMover	(gentity_t *ent, gentity_t *other, gentity_t *activator);
void SP_func_door		(gentity_t *ent);
void Think_MatchTeam	(gentity_t *ent);
void InitRotator		(gentity_t *ent);
void ROT_SetMoverState( gentity_t *ent, moverState_t moverState, int time ) 
{
	vec3_t			delta;
	float			f;

	ent->s.apos.trTime = time;

	switch( moverState ) 
	{
	case MOVER_POS1:
		VectorCopy( ent->apos1, ent->s.apos.trBase );
		ent->s.apos.trType = TR_STATIONARY;
		break;

	case MOVER_POS2:
		VectorCopy( ent->apos2, ent->s.apos.trBase );
		ent->s.apos.trType = TR_STATIONARY;
		break;

	case MOVER_1TO2:
		VectorCopy( ent->apos1, ent->s.apos.trBase );
		VectorSubtract( ent->apos2, ent->apos1, delta );
		f = 1000.0 / ent->s.apos.trDuration;
		VectorScale( delta, f, ent->s.apos.trDelta );
		ent->s.apos.trType = TR_LINEAR_STOP;
		break;

	case MOVER_2TO1:
		VectorCopy( ent->apos2, ent->s.apos.trBase );
		VectorSubtract( ent->apos1, ent->apos2, delta );
		f = 1000.0 / ent->s.apos.trDuration;
		VectorScale( delta, f, ent->s.apos.trDelta );
		ent->s.apos.trType = TR_LINEAR_STOP;
		break;
	}

	BG_EvaluateTrajectory(&ent->s.apos, level.time, ent->r.currentAngles);	
}


void Touch_2WayRotDoorTrigger( gentity_t *ent, gentity_t *other, trace_t *trace ) 
{
	vec3_t dir, angles;
	vec3_t forward, right;
	float dot;

	if ((ent->parent->moverState != MOVER_POS1) && (ent->parent->moverState != MOVER_POS2))
		return;
	if (!other->client)
		return;

	/// Skip locked doors
	//if(ent->parent->damage == -999)
	//	return;
	if (ent->parent->spawnflags & 16)
		return;

	// figure out which way the door should open
	VectorSubtract(ent->s.origin, other->client->ps.origin, forward);
	VectorNormalize(forward);

	// make sure we're facing the door
	AngleVectors(other->client->ps.viewangles, dir, NULL, NULL);
	dot = DotProduct(dir, forward);
	if (dot < 0.3)
		return;

	// LOCKED DOOR
	//if (ent->parent->spawnflags & 16)
	//	return;
	
	// if already open, just return to default position
	if ((ent->parent->moverState == MOVER_POS2)) {
		Use_BinaryMover(ent->parent, ent, other);
		return;
	}
		
	VectorSubtract(ent->parent->s.origin, ent->s.origin, dir);
	VectorNormalize(dir);
	vectoangles(dir, angles);
	AngleVectors(angles, NULL, right, NULL);
		
	// figure out we're roughly facing with or against it
	dot = DotProduct(forward, right);

	if (dot > 0.3) {
		VectorAdd(ent->parent->apos1, ent->apos2, ent->parent->apos2);
		Use_BinaryMover(ent->parent, ent, other);
	}
	else if (dot < -0.3) {
		VectorSubtract(ent->parent->apos1, ent->apos2, ent->parent->apos2);
		Use_BinaryMover(ent->parent, ent, other);
	}
}


void Think_Spawn2WayRotDoorTrigger(gentity_t *ent)
{
	gentity_t *other;
	vec3_t mins, maxs;
	int i, best;

	// find the bounds of everything on the team
	VectorCopy(ent->r.absmin, mins);
	VectorCopy(ent->r.absmax, maxs);

	for (other = ent->teamchain; other; other=other->teamchain){
		AddPointToBounds (other->r.absmin, mins, maxs);
		AddPointToBounds (other->r.absmax, mins, maxs);
	}

	// find the thinnest axis, which will be the one we expand
	best = 0;
	for (i = 1 ; i < 3 ; i++){
		if (maxs[i] - mins[i] < maxs[best] - mins[best])
			best = i;
	}

	maxs[best] += 120;
	mins[best] -= 120;

	// create a trigger with this size
	other = G_Spawn();
	other->classname = "door_trigger";
	VectorCopy (mins, other->r.mins);
	VectorCopy (maxs, other->r.maxs);
	other->parent = ent;
	other->r.contents = CONTENTS_TRIGGER;
	other->touch = Touch_2WayRotDoorTrigger;
	VectorCopy(ent->s.angles, other->apos2);

	// figure out the real origin of the door
	VectorAdd(mins, maxs, other->s.origin);
	VectorScale(other->s.origin, 0.5, other->s.origin);

	trap_LinkEntity (other);

	// remember the thinnest axis
	other->count = best;

	trap_LinkEntity (other);
	MatchTeam( ent, ent->moverState, level.time );
}

void Blocked_RotDoor( gentity_t *ent, gentity_t *other ) 
{
	// remove anything other than a client
	if (!other->client)
	{
		// except mission items
		if (other->s.eType == ET_ITEM && other->item->giType == IT_GAMETYPE)
			return;
		G_TempEntity(other->s.origin, EV_ITEM_POP);
		G_FreeEntity(other);
		return;
	}	
	Use_BinaryMover(ent, ent, other);
}

void SP_func_door_rotating (gentity_t *ent)
{
	int i, best, b;

	SP_func_door (ent);
	// GodOrDevil - remove area portals & doors. disable (weapon == 999 for SUB BSP in RMGs)
	if(g_enableDoors.integer == 0 || (g_enableDoors.integer == 2 && ent->s.weapon == 999))
	{	
		trap_UnlinkEntity( ent );
		return;
	}
	// End

	if (VectorCompare(ent->apos2, vec3_origin))
		VectorSet(ent->apos2, 0, 90, 0);

	// figure out 
	for (best = 0, i = 0; i < 3; i++){
		// record angle change for later use (ie. 2-way doors)
		ent->s.angles[i] = abs(ent->apos2[i]);		
		ent->apos1[i] = ent->s.apos.trBase[i];
		b = abs(ent->apos2[i]);
		ent->apos2[i] += ent->apos1[i];
		if (b > best)
		        best = b;
	}

    ent->s.apos.trDuration = best * 1000 / abs(ent->speed);
	VectorCopy(ent->s.apos.trBase, ent->r.currentAngles);
	ent->blocked = Blocked_RotDoor;
	trap_LinkEntity(ent);
	
	//if ( ent->teammaster == ent || !ent->teammaster ) {
	//	if(ent->damage == -999){
	//		trap_AdjustAreaPortalState( ent, qtrue );
	//		return;
	//	}
	//}
	// if a team slave, we're done
	if (ent->flags & FL_TEAMSLAVE)
		return;
	
	if (ent->targetname)
		ent->think = Think_MatchTeam;
	else
		ent->think = Think_Spawn2WayRotDoorTrigger;
}

// End


