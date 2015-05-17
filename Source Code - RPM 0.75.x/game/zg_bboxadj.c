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
#include "g_local.h"
#ifdef _MANDOWN_HITBOX

/*
================
G_AdjustClientBBox
================
*/
void G_AdjustClientBBox(gentity_t *other)
{
	other->client->minSave[0] = other->r.mins[0];
	other->client->minSave[1] = other->r.mins[1];
	other->client->minSave[2] = other->r.mins[2];

	other->client->maxSave[0] = other->r.maxs[0];
	other->client->maxSave[1] = other->r.maxs[1];
	other->client->maxSave[2] = other->r.maxs[2];

	// Adjust the hit box to account for hands and such 
	// that are sticking out of the normal bounding box
	if (other->client->ps.pm_flags & PMF_LEANING)
	{
		other->r.maxs[0] *= 3.0f;
		other->r.maxs[1] *= 3.0f;
		other->r.mins[0] *= 3.0f;
		other->r.mins[1] *= 3.0f;
		other->r.svFlags |= SVF_DOUBLED_BBOX;
	}

	// Relink the entity into the world
	trap_LinkEntity (other);
}


/*
================
G_AdjustClientBBoxs

Inflates every clients bbox to take leaning/jumping etc into account this frame.
This is all very nasty really...
================
*/

void G_AdjustClientBBoxs(void)
{
	int i;

	// Move all the clients back into the reference clients time frame.
	for (i = 0; i < level.numConnectedClients; i++)
	{
		gentity_t* other = &g_entities[level.sortedClients[i]];

		if (other->client->pers.connected != CON_CONNECTED)
		{
			continue;
		}

		// Skip entities not in use
		if (!other->inuse)
		{
			continue;
		}

		// Skip clients that are spectating
		if (G_IsClientSpectating(other->client) || G_IsClientDead(other->client))
		{
			continue;
		}

		G_AdjustClientBBox(other);
	}	
}



/*
================
G_UndoAdjustedClientBBox
================
*/
void G_UndoAdjustedClientBBox(gentity_t *other)
{
	// Put the hitbox back the way it was
	other->r.maxs[0] = other->client->maxSave[0];
	other->r.maxs[1] = other->client->maxSave[1];
	other->r.maxs[2] = other->client->maxSave[2];

	other->r.mins[0] = other->client->minSave[0];
	other->r.mins[1] = other->client->minSave[1];
	other->r.mins[2] = other->client->minSave[2];

	other->r.svFlags &= (~SVF_DOUBLED_BBOX);
}


/*
================
G_UndoAdjustedClientBBoxs
================
*/
void G_UndoAdjustedClientBBoxs(void)
{
	int i;

	for ( i = 0; i < level.numConnectedClients; i ++ )
	{
		gentity_t* other = &g_entities[level.sortedClients[i]];
		
		if ( other->client->pers.connected != CON_CONNECTED )
		{
			continue;
		}

		// Skip clients that are spectating
		if ( G_IsClientSpectating ( other->client ) || G_IsClientDead ( other->client ) )
		{
			continue;
		}

		G_UndoAdjustedClientBBox(other);

		// Relink the entity into the world
		trap_LinkEntity ( other );
	}
}

/*
================
G_SetClientPreLeaningBBox
================
*/
void G_SetClientPreLeaningBBox(gentity_t *ent)
{
	//Ryan
	//ent->r.maxs[2] += g_adjPLBMaxsZ.value;
	ent->r.maxs[2] += PLB_MAXZ;
	//Ryan
}

/*
================
G_SetClientLeaningBBox
================
*/

void G_SetClientLeaningBBox(gentity_t *ent)
{
	float	leanOffset;
	vec3_t	up;
	vec3_t	right;
	
	// adjust origin for leaning
	//BG_ApplyLeanOffset(&ent->client->ps, org);//ent->r.currentOrigin);
	leanOffset = (float)(ent->client->ps.leanTime - LEAN_TIME) / LEAN_TIME * LEAN_OFFSET;
	
	//Ryan
	//leanOffset *= g_adjLeanOffset.value;
	leanOffset *= BBOX_LEAN_OFFSET;
	//Ryan

	AngleVectors( ent->client->ps.viewangles, NULL, right, up);
	VectorMA(ent->r.currentOrigin, leanOffset, right, ent->r.currentOrigin);
	VectorMA(ent->r.currentOrigin, Q_fabs(leanOffset) * -0.20f, up, ent->r.currentOrigin);

	//Ryan
	//ent->r.maxs[2] += g_adjLBMaxsZ.value;
	ent->r.maxs[2] += LB_MAXZ;
	//Ryan
	
	if (ent->client->ps.pm_flags & PMF_DUCKED)
	{
		//Ryan
		//ent->r.mins[2] += g_adjDuckedLBMinsZ.value;
		ent->r.mins[2] += DUCKED_LB_MINZ;
		//Ryan
	}
	else
	{
		//Ryan
		//ent->r.mins[2] += g_adjLBMinsZ.value;
		ent->r.mins[2] += LB_MINZ;
		//Ryan
	}
}				
#endif
// GodOrDevil - 01.10.06 - 03:09am #bbox
//Ryan
/*
================
G_ShowClientBBox

Ripped from unlagged code by Neil “haste” Toronto 
================

void G_ShowClientBBox(gentity_t *ent)
{
	vec3_t corners[8];
	float extx, exty, extz;
	gentity_t *tent;
	int i;

	// get the extents (size)
	extx = ent->r.maxs[0] - ent->r.mins[0];
	exty = ent->r.maxs[1] - ent->r.mins[1];
	extz = ent->r.maxs[2] - ent->r.mins[2];

	VectorAdd(ent->r.currentOrigin, ent->r.maxs, corners[3]);

	VectorCopy(corners[3], corners[2]);
	corners[2][0] -= extx;

	VectorCopy(corners[2], corners[1]);
	corners[1][1] -= exty;

	VectorCopy(corners[1], corners[0]);
	corners[0][0] += extx;

	for (i = 0; i < 4; i++)
	{
		VectorCopy(corners[i], corners[i + 4]);
		corners[i + 4][2] -= extz;
	}

	// top
	tent = G_TempEntity(corners[0], EV_BOTWAYPOINT);
	VectorCopy(corners[1], tent->s.angles);
	tent->r.svFlags |= SVF_BROADCAST;

	tent = G_TempEntity(corners[2], EV_BOTWAYPOINT);
	VectorCopy(corners[3], tent->s.angles);
	tent->r.svFlags |= SVF_BROADCAST;

	tent = G_TempEntity(corners[2], EV_BOTWAYPOINT);
	VectorCopy(corners[1], tent->s.angles);
	tent->r.svFlags |= SVF_BROADCAST;

	tent = G_TempEntity(corners[0], EV_BOTWAYPOINT);
	VectorCopy(corners[3], tent->s.angles);
	tent->r.svFlags |= SVF_BROADCAST;

	// bottom
	tent = G_TempEntity(corners[7], EV_BOTWAYPOINT);
	VectorCopy(corners[6], tent->s.angles);
	tent->r.svFlags |= SVF_BROADCAST;

	tent = G_TempEntity(corners[5], EV_BOTWAYPOINT);
	VectorCopy(corners[4], tent->s.angles);
	tent->r.svFlags |= SVF_BROADCAST;

	tent = G_TempEntity(corners[5], EV_BOTWAYPOINT);
	VectorCopy(corners[6], tent->s.angles);
	tent->r.svFlags |= SVF_BROADCAST;

	tent = G_TempEntity(corners[7], EV_BOTWAYPOINT);
	VectorCopy(corners[4], tent->s.angles);
	tent->r.svFlags |= SVF_BROADCAST;
}
*/

