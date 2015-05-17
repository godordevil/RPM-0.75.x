#include "g_local.h"

/*
================
G_CheckBox

Same as G_SpotWouldTelefrag, cept origin is based off ent's position (a player)
================
*/
qboolean G_CheckBox(gentity_t *ent) 
{
	vec3_t	playerMins = {-15, -15, -46};
	vec3_t	playerMaxs = {15, 15, 48};
	int i, num;
	int touch[MAX_GENTITIES];
	gentity_t *hit;
	vec3_t mins, maxs;

	VectorAdd(ent->client->ps.origin, playerMins, mins);
	VectorAdd(ent->client->ps.origin, playerMaxs, maxs);
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );

	for (i=0 ; i<num ; i++) 
	{
		hit = &g_entities[touch[i]];

		if (hit->client)
		{
			if (hit == ent)
			{
				continue;
			}
		
			if (G_IsClientSpectating(hit->client))
			{
				continue;
			}

			if (G_IsClientDead(hit->client))
			{
				continue;
			}

			return qtrue;
		}
	}

	return qfalse;
}
