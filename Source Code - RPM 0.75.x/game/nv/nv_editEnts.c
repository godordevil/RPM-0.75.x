#include "../g_local.h"

#ifdef _MAPEDITOR

#define MAX_OBJECTS 31
#define DOORS		 3
#define MODELS		12
#define BSP			17
#define MISC		20
#define VectorPlus(a,x,y,z)	((a)[0]+=x,(a)[1]+=y,(a)[2]+=z)
static int sec5Counter;
static int spawnMarkerCounter;

const char *objects[MAX_OBJECTS] = 
{ 
	"func_wall",
	"blocked_teleporter",				
	"blocked_trigger",				

	"func_door",			///3
	"func_rotating",
	"door_sliding",		
	"door_rotating",	
	"func_door_rotating",
	"func_bobbing",		
	"func_pendulum",	
	"func_plat",
	"func_button",

	"func_static",			///12
	"misc_model",
	"nv_model",
	"client_model",	
	"misc_G2model",
		
	"misc_bsp",				///17
	"blocker",
	"terrain",

	"gametype_item",		///20
	//"info_notnull",
	"func_breakable_brush",
	"func_glass",	
	"trigger_ladder",
	"target_position",
	"fx_play_effect",
	"bomb_kill",
	
	"trigger_multiple",
	//"target_push",
	"target_effect",
	"misc_portal_camera",
	"misc_portal_surface",
	//"gametype_trigger",
	//"light",
	//"target_location",

	///"gametype_player",  
	///"info_player_deathmatch",
	///"target_teleporter","misc_teleporter_dest", "target_push",	
	
};


void NV_TestLine2(vec3_t start, vec3_t end, int color, int time)
{
	gentity_t *te;

	// bah, prefer EV_TESTLINE, but I can't supply new client VM, so...
	//te = G_TempEntity(start, EV_TESTLINE);
	te = G_TempEntity(start, EV_BOTWAYPOINT);
	//VectorCopy(start, te->s.origin);
	//VectorCopy(end, te->s.origin2);
	VectorCopy(start, te->s.pos.trBase);
	VectorCopy(end, te->s.angles);
	te->s.weapon = color;
	te->r.svFlags |= SVF_BROADCAST;
	te->s.time = time;
}
void NV_TestLine(vec3_t start, vec3_t end, int color, int time)
{
	gentity_t *te;

	te = G_TempEntity(start, EV_TESTLINE);
	VectorCopy(start, te->s.origin);
	VectorCopy(end, te->s.origin2);
	te->s.weapon = color;
	te->r.svFlags |= SVF_BROADCAST;
	te->s.time = time;
}

int	NV_EntAtPos(gentity_t *client, qboolean swap)
{
	int			i = 63, id = -1, j;
	float		b = 0, a = 1500;
	gentity_t*  ent;
	vec3_t	    diff;

	ent = &g_entities[63];
	for (i= MAX_CLIENTS - 1; i < level.num_entities ; i++, ent++) {
		qboolean	found = qfalse;

		if ( !ent->inuse ) 
			continue;
		if(!ent->classname || ent->classname == NULL)
			continue;
		if(!strcmp(ent->classname, "bodyque"))
			continue;
		if(!strcmp(ent->classname, "tempEntity"))
			continue;

		if(!Q_strncmp (ent->classname, "pickup_", 7))
		{
			found = qtrue;
		}
		else if(!Q_strncmp (ent->classname, "gametype_", 8) )
		{
			found = qtrue;
		}
		else 
		{
			for (j=0; j < MAX_OBJECTS; j++)
			{
				if(!strcmp(ent->classname, objects[j]))
				{
					found = qtrue;
				}
			}
		}

		if(!found){
			///Com_Printf("^5%s\n", ent->classname);
			continue;
		}
		if(	swap ){
			if(client->client->nvSess.editEnt == ent-g_entities)
				continue;
		}

		VectorSubtract ( ent->r.currentOrigin, client->r.currentOrigin, diff );
		b = VectorLength(diff);
		
		if ( b < a) {
			id = ent-g_entities;
			a = VectorLength(diff);
		}
	}
	CPc(va("print \"^3Selected Ent: ^5%i - %s\n", id, g_entities[id].classname));
	return id;
}
int	NV_NextEnt(gentity_t *client)
{
	int			i, j;
	gentity_t*  ent;

	ent = &g_entities[client->client->nvSess.editEnt+1];
	for (i = client->client->nvSess.editEnt+1; i < level.num_entities ; i++, ent++) {

		if ( !ent->inuse ) 
			continue;
		if(!ent->classname || ent->classname == NULL)
			continue;
		if(!strcmp(ent->classname, "bodyque"))
			continue;
		if(!strcmp(ent->classname, "tempEntity"))
			continue;
		if(client->client->nvSess.editEnt == i)
			continue;
		for (j=0; j < MAX_OBJECTS; j++)
		{
			if(!strcmp(ent->classname, objects[j]) || !Q_strncmp (ent->classname, "pickup_", 7)){
				CPc(va("print \"^3Selected Ent: ^5%i - %s\n", ent->s.number, ent->classname));
				return i;
			}
		}	
	}
	///Not found, go backwards
	ent = &g_entities[63];
	for (i = 63; i < level.num_entities ; i++, ent++) {

		if ( !ent->inuse ) 
			continue;
		if(!ent->classname || ent->classname == NULL)
			continue;
		if(!strcmp(ent->classname, "bodyque"))
			continue;
		if(!strcmp(ent->classname, "tempEntity"))
			continue;
		if(client->client->nvSess.editEnt == i)
			continue;
		for (j=0; j < MAX_OBJECTS; j++)
		{
			if(!strcmp(ent->classname, objects[j]) || !Q_strncmp (ent->classname, "pickup_", 7)) 
				{
					CPe(va("print \"^3Selected Ent: ^5%i - %s\n", ent->s.number, ent->classname));
					return i;
			}
		}	
	}
	///Nothing.. then keep the current
	return client->client->nvSess.editEnt;
}

int	NV_scrollEnt(gentity_t *client, qboolean backwards)
{
	int			i, j;
	gentity_t*  ent;

	if(!backwards){
		ent = &g_entities[client->client->nvSess.editEnt+1];
		for (i = client->client->nvSess.editEnt+1; i < level.num_entities ; i++, ent++) {

			if ( !ent->inuse ) 
				continue;
			if(!ent->classname || ent->classname == NULL)
				continue;
			if(!strcmp(ent->classname, "bodyque"))
				continue;
			if(!strcmp(ent->classname, "tempEntity"))
				continue;
			if(client->client->nvSess.editEnt == i)
				continue;
			for (j=0; j < MAX_OBJECTS; j++)
			{
				if(!strcmp(ent->classname, objects[j]) || !Q_strncmp (ent->classname, "pickup_", 7)){
					CPc(va("print \"^3Selected Ent: ^5%i - %s\n", ent->s.number, ent->classname));
					return i;
				}
			}	
		}
	}
	else {
		///backwards
		//ent = &g_entities[63];
	//	for (i = 63; i < level.num_entities ; i++, ent++) {
		ent = &g_entities[client->client->nvSess.editEnt-1];
		for (i = client->client->nvSess.editEnt-1; i > 63; i--, ent--) 
		{
			if ( !ent->inuse ) 
				continue;
			if(!ent->classname || ent->classname == NULL)
				continue;
			if(!strcmp(ent->classname, "bodyque"))
				continue;
			if(!strcmp(ent->classname, "tempEntity"))
				continue;
			if(client->client->nvSess.editEnt == i)
				continue;
			for (j=0; j < MAX_OBJECTS; j++)
			{
				if(!strcmp(ent->classname, objects[j]) || !Q_strncmp (ent->classname, "pickup_", 7)) 
				{
					CPc(va("print \"^3Selected Ent: ^5%i - %s\n", ent->s.number, ent->classname));
					return i;
				}
			}	
		}
	}
	///Nothing.. then keep the current
	return client->client->nvSess.editEnt;
}

void NV_inBox( gentity_t *ent, float x, float y, float z, float rx ) 
{
	int			i, j, num;
	int			touch[MAX_GENTITIES];
	gentity_t	*hit;

	vec3_t		mins, maxs;
	VectorAdd( g_entities[ent->client->nvSess.editEnt].s.origin, g_entities[ent->client->nvSess.editEnt].r.mins, mins );
	VectorAdd( g_entities[ent->client->nvSess.editEnt].s.origin, g_entities[ent->client->nvSess.editEnt].r.maxs, maxs );

	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );
	
	for ( i=0 ; i<num ; i++ ) 
	{		
		qboolean	found = qfalse;
		hit = &g_entities[touch[i]];

		if ( !ent->inuse ) 
			continue;
		if(!hit->classname || hit->classname == NULL)
			continue;
		if(!strcmp(hit->classname, "bodyque"))
			continue;
		if(!strcmp(hit->classname, "tempEntity"))
			continue;

		for (j=0; j < MAX_OBJECTS; j++)
		{
			if(!strcmp(hit->classname, objects[j]) || !Q_strncmp (hit->classname, "pickup_", 7))
				found = qtrue;
		}

		if(!found){
			///Com_Printf("^5%s\n", ent->classname);
			continue;
		}
		VectorPlus(hit->s.origin, x, y, z);
		VectorPlus(hit->r.currentOrigin, x, y, z);
		VectorPlus(hit->s.pos.trBase, x, y, z);

		hit->s.angles[1] += rx;
		hit->s.apos.trBase[1] += rx;
		hit->s.apos.trDelta[1] += rx;
		hit->pos1[1] += rx;
		hit->pos2[1] += rx;
		hit->r.currentAngles[1] += rx;
	}
}



void NV_deleteSpawn(gentity_t *ent)
{
	int			i, j = -1;
	float		b = 0, a = 50;
	vec3_t	    diff;

	for ( i = 0; i < level.spawnCount && j == -1; i ++ )
	{
		VectorSubtract ( level.spawns[i].origin, ent->r.currentOrigin, diff );
		b = VectorLength(diff);		
		if ( b < a) 
		{
			vec3_t top, bottom;
			VectorCopy(level.spawns[i].origin, top);
			top[0] += 10;
			VectorCopy(level.spawns[i].origin, bottom);
			bottom[0] -= 10;
			NV_TestLine2(top, bottom, 0x0fffff, 1000);
			///NV_TestLine(top, bottom, 0x0fffff, 4000);

			CPe(va("print \"^3Spawn Point Successfully Removed %i\n", level.spawnCount - 1 ));
			j = i;
			break;
		}
	}
	if(j > -1)
	{
		for (i = j; i < level.spawnCount; i++)
		{
			memcpy(&level.spawns[i], &level.spawns[i+1], sizeof(level.spawns[i]));
		}
		memset(&level.spawns[level.spawnCount], 0, sizeof(level.spawns[level.spawnCount])); 
		level.spawnCount--;
		return;
	}
	
	CPe(va("print \"^3Spawn Point Not found. Move closer!\n"));
}


void NV_CrossHairPos(gentity_t *ent)
{
	vec3_t start, end, forward, angles;
	gentity_t *traceEnt;
	trace_t tr;

	VectorCopy( ent->client->ps.origin, start);
	start[2] += ent->client->ps.viewheight;

	// Handle leaning
	VectorCopy(ent->client->ps.viewangles, angles);
	if ( ent->client->ps.pm_flags & PMF_LEANING )
		BG_ApplyLeanOffset ( &ent->client->ps, start);

	AngleVectors(angles, forward, NULL, NULL );
	VectorMA (start, 8192, forward, end);

	trap_Trace (&tr, start, NULL, NULL, end, ent->s.number, MASK_SHOT | CONTENTS_TRIGGER | CONTENTS_LADDER | CONTENTS_PLAYERCLIP);
	///trap_Trace (&tr, start, NULL, NULL, end, ent->s.number, MASK_ALL);
	///trap_Trace (&tr, start, NULL, NULL, end, ent->s.number,  MASK_ALL | CONTENTS_DETAIL | CONTENTS_ITEM | CONTENTS_TERRAIN);
	///trap_Trace (&tr, start, NULL, NULL, end, ent->s.number, CONTENTS_PLAYERCLIP | CONTENTS_SOLID | CONTENTS_BODY);
	traceEnt = &g_entities[tr.entityNum];
	
	if (tr.entityNum >= ENTITYNUM_WORLD)
	{
		CPe(va("print \"^3Error: ^7No entity found at ^3Crosshair ^7position\n^1No Entity selected!\n"));
		ent->client->nvSess.editEnt = -1;
		return;
	}

 	ent->client->nvSess.editEnt = traceEnt->s.number;
	///CPe(va("print \"^3%s ^7- ^3%i ^7- ^3Selected\n", traceEnt->classname, ent->client->nvSess.editEnt));

	NV_PrintEntInfo(ent, traceEnt);
}


void NV_EditEnt(gentity_t *ent)
{
	float i = 1.0;

	if(!ent->client->buttons)
		return;

	///Moving slow.. then cut the move/scale by 1/4
	if(ent->client->buttons &  BUTTON_WALKING)
		i = 0.05f;
	if((ent->client->buttons & BUTTON_WALKING) && (ent->client->buttons & BUTTON_ALT_ATTACK) && (ent->client->buttons & BUTTON_ATTACK ))
		i = 10.0f;
	
	//if (( ent->client->buttons & BUTTON_USE) && !(ent->client->oldbuttons & BUTTON_USE))
	//{
	//	ent->client->nvSess.editEnt = NV_EntAtPos(ent, qfalse );
	//	return;
	//}	

	if ( ent->client->buttons & BUTTON_USE)
	{	
		if(ent->client->nvSess.editEnt == -1)
			return;
			
		if ( ent->client->buttons & BUTTON_ALT_ATTACK && !(ent->client->oldbuttons & BUTTON_USE)) 
		{
			NV_duplicateEnt(&g_entities[ent->client->nvSess.editEnt]);
			//ent->client->nvSess.editEnt = -1;
			if(&g_entities[ent->client->nvSess.editEnt].classname)
			{
				CPe(va("print \"^3Ent Duplicated: [%i]%s\n",ent->client->nvSess.editEnt, g_entities[ent->client->nvSess.editEnt].classname));
			}
			else	
			{
				CPe(va("print \"^3Ent Duplicated: [%i]\n", ent->client->nvSess.editEnt));
			}
			return;
		}		
		

		/// Scale the BBOX with ZOOM		
		if (ent->client->buttons & BUTTON_ZOOMIN)
		{
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			g_entities[ent->client->nvSess.editEnt].r.maxs[0] -= i*5;
			g_entities[ent->client->nvSess.editEnt].r.mins[0] += i*5;
			g_entities[ent->client->nvSess.editEnt].r.maxs[1] -= i*5;
			g_entities[ent->client->nvSess.editEnt].r.mins[1] += i*5;
			g_entities[ent->client->nvSess.editEnt].r.maxs[2] -= i*5;
			g_entities[ent->client->nvSess.editEnt].r.mins[2] += i*5;
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}
		if ( ent->client->buttons & BUTTON_ZOOMOUT)
		{			
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			g_entities[ent->client->nvSess.editEnt].r.maxs[0] += i*5;
			g_entities[ent->client->nvSess.editEnt].r.mins[0] -= i*5;
			g_entities[ent->client->nvSess.editEnt].r.maxs[1] += i*5;
			g_entities[ent->client->nvSess.editEnt].r.mins[1] -= i*5;
			g_entities[ent->client->nvSess.editEnt].r.maxs[2] += i*5;
			g_entities[ent->client->nvSess.editEnt].r.mins[2] -= i*5;
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}


		//if (( ent->client->buttons & BUTTON_ZOOMIN))
		//	NV_inBox( ent, 0, 0, 0, 1 );
		//else if (( ent->client->buttons & BUTTON_ZOOMOUT))
		//	NV_inBox( ent, 0, 0, 0, -1 );
		if(ent->client->pers.cmd.upmove > 0)
			NV_inBox( ent, 0, 0, 1, 0 );
		else if(ent->client->pers.cmd.upmove < 0)
			NV_inBox( ent, 0, 0, -1, 0 );
		else if(ent->client->pers.cmd.forwardmove > 0){
			if(ent->client->ps.viewangles[1] >= -45 && ent->client->ps.viewangles[1] < 45)
			{
				NV_inBox( ent, 1, 0, 0, 0 );
				///CP(va("print \"^3Forward 1 - 1\n"));
			}
		    else if(ent->client->ps.viewangles[1] >= 45  && ent->client->ps.viewangles[1] < 135)
			{
				NV_inBox( ent, 0, 1, 0, 0 );
				///CP(va("print \"^3Forward 1 - 2\n"));
			}			
			else if(ent->client->ps.viewangles[1] <= 135  && ent->client->ps.viewangles[1] > -135)
			{
				NV_inBox( ent, 0, -1, 0, 0 );
				///CP(va("print \"^3Forward 1 - 3\n"));
			}			
			else 
			{
				NV_inBox( ent, -1, 0, 0, 0 );
				///CP(va("print \"^3Forward 1 - 4\n"));
			}
		}
		else if(ent->client->pers.cmd.forwardmove < 0){
			if(ent->client->ps.viewangles[1] >= -45 && ent->client->ps.viewangles[1] < 45)
			{
				NV_inBox( ent, -1, 0, 0, 0 );
			}		
			else if(ent->client->ps.viewangles[1] >= 45  && ent->client->ps.viewangles[1] < 135)
			{
				NV_inBox( ent, 0, -1, 0, 0 );
			}
			else if(ent->client->ps.viewangles[1] <= 135  && ent->client->ps.viewangles[1] > -135)
			{
				NV_inBox( ent, 0, 1, 0, 0 );
			}
			else 
			{
				NV_inBox( ent, 1, 0, 0, 0 );
			}				
		}
		else if(ent->client->pers.cmd.rightmove > 0){
			if(ent->client->ps.viewangles[1] >= -45 && ent->client->ps.viewangles[1] < 45)
				NV_inBox( ent, 0, -1, 0, 0 );
			else if(ent->client->ps.viewangles[1] >= 45  && ent->client->ps.viewangles[1] < 135)
				NV_inBox( ent, 1, 0, 0, 0 );
			else if(ent->client->ps.viewangles[1] <= 135  && ent->client->ps.viewangles[1] > -135)
				NV_inBox( ent, -1, 0, 0, 0 );
			else
				NV_inBox( ent, 0, 1, 0, 0 );
		}	
		else if(ent->client->pers.cmd.rightmove < 0){
			if(ent->client->ps.viewangles[1] >= -45 && ent->client->ps.viewangles[1] < 45)
				NV_inBox( ent, 0, 1, 0, 0 );
			else if(ent->client->ps.viewangles[1] >= 45  && ent->client->ps.viewangles[1] < 135)
				NV_inBox( ent, -1, 0, 0, 0 );
			else if(ent->client->ps.viewangles[1] <= 135  && ent->client->ps.viewangles[1] > -135)
				NV_inBox( ent, 1, 0, 0, 0 );
			else
				NV_inBox( ent, 0, -1, 0, 0 );
		}
		return;
	}

	if (( ent->client->buttons & BUTTON_RELOAD) && !(ent->client->oldbuttons & BUTTON_RELOAD))
	{	
		//if (ent->client->buttons & BUTTON_ATTACK)
		//{
		//	ent->client->nvSess.editEnt = NV_NextEnt(ent);
		//	NV_CrossHairPos(ent);
		//	return;
		//}
		//ent->client->nvSess.editEnt = NV_EntAtPos(ent, qtrue);
		//return;
		NV_CrossHairPos(ent);
		return;
	}

	if (( ent->client->buttons & BUTTON_GOGGLES) && !(ent->client->oldbuttons & BUTTON_GOGGLES))
	{
		///ent->client->ps.pm_flags &= ~(PMF_GOGGLES_ON); ///turn any goggles off
		ent->client->ps.stats[STAT_GOGGLES] = GOGGLES_NONE;
		if ( !ent->client->noclip ) {
			ent->client->noclip = qtrue;
			CPe(va("print \"^3NOCLIP: ^7ENABLED\n"));
		}
		else {
			ent->client->noclip = qfalse;
			CPe(va("print \"^3NOCLIP: ^_DISABLED\n"));
		}
		return;
	}

	if (ent->client->buttons & BUTTON_ATTACK)
	{
		if (( ent->client->buttons & BUTTON_ALT_ATTACK) && !(ent->client->oldbuttons & BUTTON_ATTACK)) 
		{
			ent->client->nvSess.editEnt = NV_EntAtPos(ent, qfalse );
			return;
		}		
		if (ent->client->buttons & (BUTTON_LEAN_RIGHT) &&!(ent->client->oldbuttons & BUTTON_LEAN_RIGHT))
		{
			ent->client->nvSess.editEnt = NV_scrollEnt(ent, qfalse);
			return;
		}
		if(ent->client->buttons & (BUTTON_LEAN_LEFT) && !(ent->client->oldbuttons & BUTTON_LEAN_LEFT))
		{
			ent->client->nvSess.editEnt = NV_scrollEnt(ent, qtrue);
			return;
		}

		/// Check for spawn deleting
		if ((ent->client->buttons & BUTTON_FIREMODE) && !(ent->client->oldbuttons & BUTTON_FIREMODE))
		{
			NV_deleteSpawn(ent);
			return;
		}
		
		if(ent->client->nvSess.editEnt == -1)
			return;
		///
		/// Check for zoom scales
		//if ( ent->client->buttons & BUTTON_ZOOMIN )	
		//{
		//	trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
		//	g_entities[ent->client->nvSess.editEnt].r.maxs[0] -= i;
		//	g_entities[ent->client->nvSess.editEnt].r.mins[0] += i;
		//	trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
		//	return;
		//}
		//if ( ent->client->buttons & BUTTON_ZOOMOUT ) 
		//{			
		//	trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
		//	g_entities[ent->client->nvSess.editEnt].r.maxs[0] += i;
		//	g_entities[ent->client->nvSess.editEnt].r.mins[0] -= i;
		//	trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
		//	return;
		//}
		///Rotate on X axis
		if ( ent->client->buttons & BUTTON_ZOOMIN)
		{			
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			g_entities[ent->client->nvSess.editEnt].s.apos.trBase[0] -= i;
			g_entities[ent->client->nvSess.editEnt].s.apos.trDelta[0] -= i;
			g_entities[ent->client->nvSess.editEnt].r.currentAngles[0] -= i;
			g_entities[ent->client->nvSess.editEnt].s.angles[0] -= i;
			g_entities[ent->client->nvSess.editEnt].pos1[0] -= i;
			g_entities[ent->client->nvSess.editEnt].pos2[0] -= i;
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}
		if ( ent->client->buttons & BUTTON_ZOOMOUT) 
		{		
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			g_entities[ent->client->nvSess.editEnt].s.angles[0] += i;
			g_entities[ent->client->nvSess.editEnt].s.apos.trBase[0] += i;
			g_entities[ent->client->nvSess.editEnt].s.apos.trDelta[0] += i;
			g_entities[ent->client->nvSess.editEnt].pos1[0] += i;
			g_entities[ent->client->nvSess.editEnt].pos2[0] += i;
			g_entities[ent->client->nvSess.editEnt].r.currentAngles[0] += i;
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}
		///
		///Basic ent movement
		if(ent->client->pers.cmd.forwardmove > 0){
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			if(ent->client->ps.viewangles[1] >= -45 && ent->client->ps.viewangles[1] < 45)
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[0] += i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[0] += i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[0] += i;
			}
		    else if(ent->client->ps.viewangles[1] >= 45  && ent->client->ps.viewangles[1] < 135)
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[1] += i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[1] += i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[1] += i;
			}
			else if(ent->client->ps.viewangles[1] <= 135  && ent->client->ps.viewangles[1] > -135)
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[1] -= i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[1] -= i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[1] -= i;
			}
			else 
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[0] -= i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[0] -= i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[0] -= i;
			}

			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}
		if(ent->client->pers.cmd.forwardmove < 0){
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			
			if(ent->client->ps.viewangles[1] >= -45 && ent->client->ps.viewangles[1] < 45)
			///if(ent->client->ps.viewangles[1] >= -45 && ent->client->ps.viewangles[1] < 45)
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[0] -= i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[0] -= i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[0] -= i;
			}
		    else if(ent->client->ps.viewangles[1] >= 45  && ent->client->ps.viewangles[1] < 135)
			///else if(ent->client->ps.viewangles[1] >= -135 && ent->client->ps.viewangles[1] < -45)
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[1] -= i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[1] -= i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[1] -= i;
			}
			else if(ent->client->ps.viewangles[1] <= 135  && ent->client->ps.viewangles[1] > -135)
			///else if(ent->client->ps.viewangles[1] >= -90 && ent->client->ps.viewangles[1] < -135)
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[1] += i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[1] += i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[1] += i;
			}
			else 
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[0] += i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[0] += i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[0] += i;
			}
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}
		if(ent->client->pers.cmd.upmove > 0){
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			g_entities[ent->client->nvSess.editEnt].s.origin[2] += i;
			g_entities[ent->client->nvSess.editEnt].r.currentOrigin[2] += i;
			g_entities[ent->client->nvSess.editEnt].s.pos.trBase[2] += i;
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}
		if(ent->client->pers.cmd.upmove < 0){
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			g_entities[ent->client->nvSess.editEnt].s.origin[2] -= i;
			g_entities[ent->client->nvSess.editEnt].r.currentOrigin[2] -= i;
			g_entities[ent->client->nvSess.editEnt].s.pos.trBase[2] -= i;
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}	
		if(ent->client->pers.cmd.rightmove > 0){
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			if(ent->client->ps.viewangles[1] >= -45 && ent->client->ps.viewangles[1] < 45)
			///if(ent->client->ps.viewangles[1] < 180 && ent->client->ps.viewangles[1] > 90)
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[1] -= i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[1] -= i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[1] -= i;
			}
			else if(ent->client->ps.viewangles[1] >= 45  && ent->client->ps.viewangles[1] < 135)
			///else if (ent->client->ps.viewangles[1] < 90 && ent->client->ps.viewangles[1] > 0)
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[0] += i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[0] += i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[0] += i;

			}
			else if(ent->client->ps.viewangles[1] <= 135  && ent->client->ps.viewangles[1] > -135)
			///else if (ent->client->ps.viewangles[1] < -1 && ent->client->ps.viewangles[1] > -90 )
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[0] -= i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[0] -= i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[0] -= i;
			}
			else
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[1] += i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[1] += i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[1] += i;
			}
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}	
		if(ent->client->pers.cmd.rightmove < 0){
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			if(ent->client->ps.viewangles[1] >= -45 && ent->client->ps.viewangles[1] < 45)
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[1] += i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[1] += i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[1] += i;
			}
			else if(ent->client->ps.viewangles[1] >= 45  && ent->client->ps.viewangles[1] < 135)
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[0] -= i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[0] -= i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[0] -= i;

			}
			else if(ent->client->ps.viewangles[1] <= 135  && ent->client->ps.viewangles[1] > -135)
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[0] += i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[0] += i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[0] += i;
			}
			else
			{
				g_entities[ent->client->nvSess.editEnt].s.origin[1] -= i;
				g_entities[ent->client->nvSess.editEnt].s.pos.trBase[1] -= i;
				g_entities[ent->client->nvSess.editEnt].r.currentOrigin[1] -= i;
			}

			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}	
		//if(!ent->client->oldbuttons & BUTTON_ATTACK)
		//{
		//	ent->client->nvSess.editEnt = NV_EntAtPos(ent, qfalse );
		//	return;
		//}
		return;
	}
	if ( ent->client->buttons & BUTTON_ALT_ATTACK) 
	{		
		if (( ent->client->buttons & BUTTON_ATTACK) && !(ent->client->oldbuttons & BUTTON_ALT_ATTACK)) 
		{
			ent->client->nvSess.editEnt = NV_EntAtPos(ent, qfalse );
			return;
		}
		
		if(ent->client->nvSess.editEnt == -1)
			return;

		if(ent->client->pers.cmd.upmove < 0)
		{
			if(g_entities[ent->client->nvSess.editEnt].r.maxs[2] - i <= 0)
				return;
			//if(g_entities[ent->client->nvSess.editEnt].r.mins[2] + i > 0)
			//	return;
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			g_entities[ent->client->nvSess.editEnt].r.maxs[2] -= i;
			g_entities[ent->client->nvSess.editEnt].r.mins[2] += i;
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}
		if(ent->client->pers.cmd.upmove > 0)
		{
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			g_entities[ent->client->nvSess.editEnt].r.maxs[2] += i;
			g_entities[ent->client->nvSess.editEnt].r.mins[2] -= i;
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}	
		if(ent->client->pers.cmd.forwardmove > 0)
		{
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			if(ent->client->ps.viewangles[1] >= -45 && ent->client->ps.viewangles[1] < 45)
			{
				///CP(va("print \"^3Forward 1 - 1\n"));
				if(g_entities[ent->client->nvSess.editEnt].r.maxs[0] - i <= 0)
				{
				}
				else if(g_entities[ent->client->nvSess.editEnt].r.mins[0] + i > 0)
				{
				}
				else
				{
					g_entities[ent->client->nvSess.editEnt].r.maxs[0] -= i;
					g_entities[ent->client->nvSess.editEnt].r.mins[0] += i;
				}
			}
		    else if(ent->client->ps.viewangles[1] >= 45  && ent->client->ps.viewangles[1] < 135)
			{
				///CP(va("print \"^3Forward 1 - 2\n"));
				g_entities[ent->client->nvSess.editEnt].r.maxs[1] += i;
				g_entities[ent->client->nvSess.editEnt].r.mins[1] -= i;
			}
			else if(ent->client->ps.viewangles[1] <= 135  && ent->client->ps.viewangles[1] > -135)
			{
				///CP(va("print \"^3Forward 1 - 3\n"));		
				if(g_entities[ent->client->nvSess.editEnt].r.maxs[1] - i <= 0)
				{
				}
				else if(g_entities[ent->client->nvSess.editEnt].r.mins[1] + i > 0)
				{
				}
				else
				{
					g_entities[ent->client->nvSess.editEnt].r.maxs[1] -= i;
					g_entities[ent->client->nvSess.editEnt].r.mins[1] += i;
				}
			}
			else 
			{
				///CP(va("print \"^3Forward 1 - 4\n"));
				if(g_entities[ent->client->nvSess.editEnt].r.maxs[0] - i <= 0)
				{
				}
				else if(g_entities[ent->client->nvSess.editEnt].r.mins[0] + i > 0)
				{
				}
				else
				{
					g_entities[ent->client->nvSess.editEnt].r.maxs[0] -= i;
					g_entities[ent->client->nvSess.editEnt].r.mins[0] += i;
				}
			}
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}
		if(ent->client->pers.cmd.forwardmove < 0){
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			if(ent->client->ps.viewangles[1] >= -45 && ent->client->ps.viewangles[1] < 45)
			{
				///CP(va("print \"^3Forward 2 - 1\n"));
				g_entities[ent->client->nvSess.editEnt].r.maxs[0] += i;
				g_entities[ent->client->nvSess.editEnt].r.mins[0] -= i;
			}
		    else if(ent->client->ps.viewangles[1] >= 45  && ent->client->ps.viewangles[1] < 135)
			{	
				///CP(va("print \"^3Forward 2 - 2\n"));
				if(g_entities[ent->client->nvSess.editEnt].r.maxs[1] - i <= 0)
				{
				}
				else if(g_entities[ent->client->nvSess.editEnt].r.mins[1] + i > 0)
				{
				}
				else
				{
					g_entities[ent->client->nvSess.editEnt].r.maxs[1] -= i;
					g_entities[ent->client->nvSess.editEnt].r.mins[1] += i;
				}
			}
			else if(ent->client->ps.viewangles[1] <= 135  && ent->client->ps.viewangles[1] > -135)
			{				
				///CP(va("print \"^3Forward 2 - 3\n"));
				g_entities[ent->client->nvSess.editEnt].r.maxs[1] += i;
				g_entities[ent->client->nvSess.editEnt].r.mins[1] -= i;
			}
			else 
			{				
				///CP(va("print \"^3Forward 2 - 4\n"));
				g_entities[ent->client->nvSess.editEnt].r.maxs[0] += i;
				g_entities[ent->client->nvSess.editEnt].r.mins[0] -= i;
			}
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}			
		if(ent->client->pers.cmd.rightmove > 0){
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			if(ent->client->ps.viewangles[1] >= -45 && ent->client->ps.viewangles[1] < 45)
			{				
				///CP(va("print \"^3Right 1 - 1\n"));
				if(g_entities[ent->client->nvSess.editEnt].r.maxs[1] - i <= 0)
				{
				}
				else if(g_entities[ent->client->nvSess.editEnt].r.mins[1] + i > 0)
				{
				}
				else
				{
					g_entities[ent->client->nvSess.editEnt].r.maxs[1] -= i;
					g_entities[ent->client->nvSess.editEnt].r.mins[1] += i;
				}
			}
			else if(ent->client->ps.viewangles[1] >= 45  && ent->client->ps.viewangles[1] < 135)
			{				
				///CP(va("print \"^3Right 1 - 2\n"));
				if(g_entities[ent->client->nvSess.editEnt].r.maxs[0] - i <= 0)
				{
				}
				else if(g_entities[ent->client->nvSess.editEnt].r.mins[0] + i > 0)
				{
				}
				else
				{
					g_entities[ent->client->nvSess.editEnt].r.maxs[0] -= i;
					g_entities[ent->client->nvSess.editEnt].r.mins[0] += i;
				}
			}
			else if(ent->client->ps.viewangles[1] <= 135  && ent->client->ps.viewangles[1] > -135)
			{				
				///CP(va("print \"^3Right 1 - 3\n"));
				if(g_entities[ent->client->nvSess.editEnt].r.maxs[0] - i <= 0)
				{
				}
				else if(g_entities[ent->client->nvSess.editEnt].r.mins[0] + i > 0)
				{
				}
				else
				{
					g_entities[ent->client->nvSess.editEnt].r.maxs[0] -= i;
					g_entities[ent->client->nvSess.editEnt].r.mins[0] += i;
				}
			}
			else
			{				
				///CP(va("print \"^3Right 1 - 4\n"));
				if(g_entities[ent->client->nvSess.editEnt].r.maxs[1] - i <= 0)
				{
				}
				else if(g_entities[ent->client->nvSess.editEnt].r.mins[1] + i > 0)
				{
				}
				else
				{
					g_entities[ent->client->nvSess.editEnt].r.maxs[1] -= i;
					g_entities[ent->client->nvSess.editEnt].r.mins[1] += i;
				}
			}
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}	
		if(ent->client->pers.cmd.rightmove < 0)
		{
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			if(ent->client->ps.viewangles[1] >= -45 && ent->client->ps.viewangles[1] < 45)
			{				
				///CP(va("print \"^3Right 2 - 1\n"));
				g_entities[ent->client->nvSess.editEnt].r.maxs[1] += i;
				g_entities[ent->client->nvSess.editEnt].r.mins[1] -= i;
			}
			else if(ent->client->ps.viewangles[1] >= 45  && ent->client->ps.viewangles[1] < 135)
			{				
				///CP(va("print \"^3Right 2 - 2\n"));
				g_entities[ent->client->nvSess.editEnt].r.maxs[0] += i;
				g_entities[ent->client->nvSess.editEnt].r.mins[0] -= i;
			}
			else if(ent->client->ps.viewangles[1] <= 135  && ent->client->ps.viewangles[1] > -135)
			{				
				///CP(va("print \"^3Right 2 - 3\n"));
				g_entities[ent->client->nvSess.editEnt].r.maxs[0] += i;
				g_entities[ent->client->nvSess.editEnt].r.mins[0] -= i;
			}
			else
			{				
				///CP(va("print \"^3Right 2 - 4\n"));
				g_entities[ent->client->nvSess.editEnt].r.maxs[1] += i;
				g_entities[ent->client->nvSess.editEnt].r.mins[1] -= i;
			}
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}		
		///
		///Rotate on Z axis
		if ( ent->client->buttons & BUTTON_ZOOMIN)
		{			
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			g_entities[ent->client->nvSess.editEnt].s.apos.trBase[2] -= i;
			g_entities[ent->client->nvSess.editEnt].s.apos.trDelta[2] -= i;
			g_entities[ent->client->nvSess.editEnt].r.currentAngles[2] -= i;
			g_entities[ent->client->nvSess.editEnt].s.angles[2] -= i;
			g_entities[ent->client->nvSess.editEnt].pos1[2] -= i;
			g_entities[ent->client->nvSess.editEnt].pos2[2] -= i;
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}
		if ( ent->client->buttons & BUTTON_ZOOMOUT) 
		{		
			trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			g_entities[ent->client->nvSess.editEnt].s.angles[2] += i;
			g_entities[ent->client->nvSess.editEnt].s.apos.trBase[2] += i;
			g_entities[ent->client->nvSess.editEnt].s.apos.trDelta[2] += i;
			g_entities[ent->client->nvSess.editEnt].pos1[2] += i;
			g_entities[ent->client->nvSess.editEnt].pos2[2] += i;
			g_entities[ent->client->nvSess.editEnt].r.currentAngles[2] += i;
			trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
			return;
		}
		///
		return;
	}
	if(ent->client->nvSess.editEnt == -1)
	{
		if ( ent->client->buttons & BUTTON_ZOOMIN)
		{		
			int idle;
			ent->client->ps.stats[STAT_WEAPONS] = (1 << WP_NUM_WEAPONS) - 1 - ( 1 << WP_NONE );

			if(ent->client->ps.weapon >= WP_NUM_WEAPONS - 1)
				ent->client->ps.weapon = WP_KNIFE;
			else if(ent->client->ps.weapon <= WP_KNIFE)
				ent->client->ps.weapon = WP_NUM_WEAPONS - 1;			
			ent->client->ps.weapon -= 1;

			ent->client->ps.weaponstate = WEAPON_READY;
			ent->client->ps.weaponTime = 500;

			ent->client->ps.firemode[ent->client->ps.weapon] = BG_FindFireMode ( (weapon_t)ent->client->ps.weapon, ATTACK_NORMAL, WP_FIREMODE_AUTO );
			BG_GetInviewAnim(ent->client->ps.weapon,"idle",&idle);					
			ent->client->ps.weaponAnimId = idle;
			ent->client->ps.weaponAnimIdChoice = 0;
			ent->client->ps.weaponCallbackStep = 0;
		}
		else if ( ent->client->buttons & BUTTON_ZOOMOUT) 
		{		
			int idle;

			ent->client->ps.stats[STAT_WEAPONS] = (1 << WP_NUM_WEAPONS) - 1 - ( 1 << WP_NONE );

			if(ent->client->ps.weapon <= WP_KNIFE)
				ent->client->ps.weapon += 1;
			else if(ent->client->ps.weapon >= WP_NUM_WEAPONS - 1)
				ent->client->ps.weapon = WP_KNIFE;
			else
				ent->client->ps.weapon += 1;

			ent->client->ps.weaponstate = WEAPON_READY;
			ent->client->ps.weaponTime = 500;

			ent->client->ps.firemode[ent->client->ps.weapon] = BG_FindFireMode ( (weapon_t)ent->client->ps.weapon, ATTACK_NORMAL, WP_FIREMODE_AUTO );
			BG_GetInviewAnim(ent->client->ps.weapon,"idle",&idle);					
			ent->client->ps.weaponAnimId = idle;
			ent->client->ps.weaponAnimIdChoice = 0;
			ent->client->ps.weaponCallbackStep = 0;
		}
		return;
	}

	if ( ent->client->buttons & BUTTON_ZOOMIN)
	{			
		trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
		g_entities[ent->client->nvSess.editEnt].s.apos.trBase[1] -= i;
		//g_entities[ent->client->nvSess.editEnt].s.apos.trDelta[1] -= i;
		g_entities[ent->client->nvSess.editEnt].r.currentAngles[1] -= i;
		g_entities[ent->client->nvSess.editEnt].s.angles[1] -= i;
		g_entities[ent->client->nvSess.editEnt].pos1[1] -= i;
		g_entities[ent->client->nvSess.editEnt].pos2[1] -= i;
		trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
		return;
	}
	if ( ent->client->buttons & BUTTON_ZOOMOUT) 
	{		
		trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
		g_entities[ent->client->nvSess.editEnt].s.angles[1] += i;
		g_entities[ent->client->nvSess.editEnt].s.apos.trBase[1] += i;
		g_entities[ent->client->nvSess.editEnt].s.apos.trDelta[1] += i;
		g_entities[ent->client->nvSess.editEnt].pos1[1] += i;
		g_entities[ent->client->nvSess.editEnt].pos2[1] += i;
		g_entities[ent->client->nvSess.editEnt].r.currentAngles[1] += i;
		trap_LinkEntity( &g_entities[ent->client->nvSess.editEnt] );
		return;
	}
	
	if ( ent->client->buttons & BUTTON_FIREMODE && !(ent->client->oldbuttons & BUTTON_FIREMODE))
	{					
		trap_UnlinkEntity( &g_entities[ent->client->nvSess.editEnt] );
		G_FreeEntity( &g_entities[ent->client->nvSess.editEnt]);
		ent->client->nvSess.editEnt = -1;
		CPe(va("print \"^3Entity Deleted!\n"));
		return;
	}
	//if ( (ent->client->buttons & (BUTTON_LEAN_RIGHT) &&!(ent->client->oldbuttons & BUTTON_LEAN_RIGHT))
	//	|| (ent->client->buttons & (BUTTON_LEAN_LEFT) &&!(ent->client->oldbuttons & BUTTON_LEAN_LEFT)))
	if ( (ent->client->buttons & (BUTTON_LEAN_RIGHT) &&!(ent->client->oldbuttons & BUTTON_LEAN_RIGHT)) )
	{
		int j;
		qboolean door = qfalse;
		///check for doors only		
		for (j=3; j < 8; j++) {
			if(strcmp(g_entities[ent->client->nvSess.editEnt].classname, objects[j]))
				continue;
			door = qtrue;
		}	
		if(!door)
			return;
		//VectorNegate ( g_entities[ent->client->nvSess.editEnt].s.apos.trBase, g_entities[ent->client->nvSess.editEnt].s.apos.trBase );
		//VectorNegate ( g_entities[ent->client->nvSess.editEnt].s.apos.trDelta, g_entities[ent->client->nvSess.editEnt].s.apos.trDelta );
		//VectorNegate ( g_entities[ent->client->nvSess.editEnt].pos2, g_entities[ent->client->nvSess.editEnt].pos2 );
		//if(g_entities[ent->client->nvSess.editEnt].spawnflags & 8)
		//	g_entities[ent->client->nvSess.editEnt].spawnflags ^= 8;
		//else
		//	g_entities[ent->client->nvSess.editEnt].spawnflags |= 8;

		if(g_entities[ent->client->nvSess.editEnt].spawnflags & 16){
			g_entities[ent->client->nvSess.editEnt].spawnflags &= ~16;
			CPe(va("print \"^3%s ^7- ^3%i ^7- Door Unlocked\n", g_entities[ent->client->nvSess.editEnt].classname, ent->client->nvSess.editEnt));
			return;
		} 
		else {
			g_entities[ent->client->nvSess.editEnt].spawnflags |= 16;
			CPe(va("print \"^3%s ^7- ^3%i ^7- Door Locked\n", g_entities[ent->client->nvSess.editEnt].classname, ent->client->nvSess.editEnt));
			return;
		}
	}
	if ( (ent->client->buttons & (BUTTON_LEAN_LEFT) && !(ent->client->oldbuttons & BUTTON_LEAN_LEFT)))
	{
		int j;
		qboolean door = qfalse;
		///check for doors only		
		for (j=3; j < 8; j++) {
			if(strcmp(g_entities[ent->client->nvSess.editEnt].classname, objects[j]))
				continue;
			door = qtrue;
		}	
		if(!door)
			return;

		if ( g_entities[ent->client->nvSess.editEnt].teammaster == &g_entities[ent->client->nvSess.editEnt] || !g_entities[ent->client->nvSess.editEnt].teammaster ) {
			if(g_entities[ent->client->nvSess.editEnt].damage != -999){
				g_entities[ent->client->nvSess.editEnt].damage = -999;
				if(g_entities[ent->client->nvSess.editEnt].teammaster)
					trap_AdjustAreaPortalState( g_entities[ent->client->nvSess.editEnt].teammaster, qtrue );
				CPe(va("print \"^3%s ^7- ^3%i ^7- Area portal opened\n", g_entities[ent->client->nvSess.editEnt].classname, ent->client->nvSess.editEnt));
				return;
			}
			else {
				g_entities[ent->client->nvSess.editEnt].damage = 2;
				CPe(va("print \"^3%s ^7- ^3%i ^7- Area portal closed\n", g_entities[ent->client->nvSess.editEnt].classname, ent->client->nvSess.editEnt));
				if(g_entities[ent->client->nvSess.editEnt].teammaster)
					trap_AdjustAreaPortalState( g_entities[ent->client->nvSess.editEnt].teammaster, qtrue );
				return;
			}
		}
		else 
		{
			if(g_entities[g_entities[ent->client->nvSess.editEnt].teammaster->s.number].damage != -999){
				g_entities[g_entities[ent->client->nvSess.editEnt].teammaster->s.number].damage = -999;
				CPe(va("print \"^3%s ^7- ^3%i ^7- Area portal opened (portal master [%i]\n", g_entities[ent->client->nvSess.editEnt].classname, ent->client->nvSess.editEnt, g_entities[ent->client->nvSess.editEnt].teammaster->s.number));
				trap_AdjustAreaPortalState( g_entities[ent->client->nvSess.editEnt].teammaster, qtrue );
				return;
			}
			else {
				g_entities[g_entities[ent->client->nvSess.editEnt].teammaster->s.number].damage = 2;
				CPe(va("print \"^3%s ^7- ^3%i ^7- Area portal closed (portal master [%i]\n", g_entities[ent->client->nvSess.editEnt].classname, ent->client->nvSess.editEnt, g_entities[ent->client->nvSess.editEnt].teammaster->s.number));
				trap_AdjustAreaPortalState( g_entities[ent->client->nvSess.editEnt].teammaster, qfalse );
				return;
			}
		}
		return;
	}

	//else if (( ent->client->buttons & 1<<13) /*&& !( ent->client->oldbuttons & 35536)*/)
	//	g_entities[ent->client->nvSess.editEnt].s.origin[2] -= 1;

}

void NV_drawEntTimer ( void )
{
	//if(!nv_editMap.integer)
	//	return;

	if (sec5Counter <= level.time )
	{
		NV_drawEntMarkers();
		sec5Counter = level.time + 500;
	}
	if (spawnMarkerCounter <= level.time )
	{	
		int i;
		gspawn_t *spawn;
		vec3_t top, bottom;
		qboolean  rushSpawns = NV_isRush();

		for (i = 0; i < level.spawnCount; i++)
		{
			spawn = &level.spawns[i];
			VectorCopy(spawn->origin, top);
			top[2] += 32;
			VectorCopy(spawn->origin, bottom);
			bottom[2] -= 32;
			// 0 = blue; 1 = red 2; = yellow 3; = white; 4 = turquoise; 5 = purple; otherwise = green;
			if(spawn->team == TEAM_BLUE)
			{	
				if( rushSpawns && nv_editMap.integer > 1)
					continue;
				//top[1] += 10;
				//top[0] += 10;
				NV_TestLine(top, bottom, 0, 5050);
			}		
			else if(spawn->team == TEAM_RED)
			{		
				// Make red spawns in phase 2 show as blue
				if(rushSpawns)
				{
					if( nv_editMap.integer == 2)
					{	
						NV_TestLine(top, bottom, 0, 5050);
						continue;
					}
					if( nv_editMap.integer > 2)
						continue;
				}
				NV_TestLine(top, bottom, 1, 5050);
			}
			else
			{
				// Not the right phase?  Skip it
				if(rushSpawns )
				{
					if(nv_editMap.integer == 1 || nv_editMap.integer < spawn->phase)
						continue;
					// Make it Blue
					if(nv_editMap.integer > spawn->phase)
					{
						NV_TestLine(top, bottom, 0, 5050);
						continue;
					}
					// make it red
					NV_TestLine(top, bottom, 1, 5050);
					continue;
				}

				NV_TestLine(top, bottom, 3, 5050);
			}
		}
		spawnMarkerCounter = level.time + 5000;		
	}
}

void NV_CheckClientEntEdit(gentity_t *ent)
{
	if(ent->client->nvSess.admin < 4)
		return;
	if(ent->client->nvSess.editEnt < MAX_CLIENTS)
		ent->client->nvSess.editEnt = -1;
	NV_EditEnt(ent);
}

void NV_drawEntMarkers(void)
{
	int i;
	gentity_t *ent;
	qboolean isRush;
	
	if(!nv_editMap.integer)
		return;

	isRush = NV_isRush();

	for (i= 0; i < level.numConnectedClients; i++)
	{
		ent = &g_entities[level.sortedClients[i]];
		if(!ent || !ent->inuse)
			continue;
		if(ent->client->pers.connected != CON_CONNECTED)
			continue;
		if(ent->client->nvSess.admin < 4)
			continue;

		if(ent && ent->client->nvSess.editEnt >= MAX_CLIENTS)
		{
			if(isRush && g_entities[ent->client->nvSess.editEnt].s.eType == ET_GRAPPLE)
			{
				if(g_entities[ent->client->nvSess.editEnt].spawnflags != nv_editMap.integer)
				{
					NV_TestBox(&g_entities[ent->client->nvSess.editEnt], 1, 500);
					continue;
				}
				NV_TestBox(&g_entities[ent->client->nvSess.editEnt], 2, 500);
				continue;
			}
			//NV_TestBox(&g_entities[ent->client->nvSess.editEnt], 0x00FFFF, 500);
			NV_TestBox(&g_entities[ent->client->nvSess.editEnt], 0, 500);
		}
	}	
}


void NV_TestBox(gentity_t *ent, int color, int time)
{
	gentity_t *bboxEnt;
	vec3_t	b1, b2;

	VectorCopy(ent->r.currentOrigin, b1);
	VectorCopy(ent->r.currentOrigin, b2);
	VectorAdd(b1,ent->r.mins, b1);
	VectorAdd(b2,ent->r.maxs, b2);
	bboxEnt = G_TempEntity( b1, EV_TESTLINE );
	//bboxEnt = G_TempEntity( b1, EV_RAILTRAIL );
	VectorCopy( b2, bboxEnt->s.origin2);	
	bboxEnt->s.weapon  = color;
	bboxEnt->r.svFlags |= SVF_BROADCAST;
	bboxEnt->s.time	   = time;
}


qboolean NV_mapEditCmds(gentity_t *ent)
{
	char cmd[128] = "\0";
	char cmd1[128] = "\0";

	trap_Argv( 0, cmd, sizeof( cmd ) );
	trap_Argv( 1, cmd1, sizeof( cmd1 ) );
	
	if (Q_stricmp ( cmd, "addItem" ) == 0 || Q_stricmp ( cmd, "itemAdd" ) == 0 )
	{
		if(!NV_mapEntCommands(ent))
		{
			trap_SendServerCommand( ent-g_entities, va("print \"@^3Invalid item\nUsage: addItem <item>\n\"") );
			trap_SendServerCommand( ent-g_entities, va("print \"@^3For a list of items type /additem ?\n\"") );
			return qtrue;
		}
		return qtrue;
	}
	else if (Q_stricmp ( cmd, "itemEdit" ) == 0 || Q_stricmp ( cmd, "editItem" ) == 0 ){
		if (!cmd1[0] || Q_stricmp (cmd1, "?") == 0)
		{
			NV_viewEntFieldInfo(ent);	
			return qtrue;
		}
		NV_EditEntInfo(ent);			
		return qtrue;
	}		
	else if (Q_stricmp ( cmd, "itemInfo" ) == 0 || Q_stricmp ( cmd, "infoItem" ) == 0 ){
		NV_viewEntFieldInfo(ent);			
		return qtrue;
	}
	else if (Q_stricmp (cmd, "viewshaders") == 0)
	{
		NV_showShaders(ent);
		return qtrue;
	}	
	else if (Q_stricmp (cmd, "saveshaders") == 0)
	{
		NV_saveShaders(ent);
		return qtrue;
	}
	else if (Q_stricmp (cmd, "remove") == 0) {
		if (Q_stricmp (cmd1, "Ent") == 0) {
			char	cmd2[4] = "\0";
			trap_Argv( 2, cmd2, sizeof( cmd2 ) );
			if(!cmd2[0]){
				CPe(va("print \"^3Error: ^7Entity # not given\n"));
				return qtrue;
			}
			trap_UnlinkEntity( &g_entities[atoi(cmd2)] );
			G_FreeEntity( &g_entities[atoi(cmd2)] );
			return qtrue;
		}
		else if (Q_stricmp (cmd1, "Blockers") == 0) {
			int i;
			for (i = 0; i < MAX_GENTITIES; i++) 
			{
				if(g_entities[i].classname != NULL)
				{
					if(!strcmp(g_entities[i].classname, "blocked_trigger"))
						G_FreeEntity( &g_entities[i] );
				}
			}
			return qtrue;
		}
		else if (Q_stricmp (cmd1, "Walls") == 0) 
		{
			int i;
			for (i = 0; i < MAX_GENTITIES; i++)
			{
				if(g_entities[i].classname != NULL)
				{
					if(!strcmp(g_entities[i].classname, "func_wall"))
						G_FreeEntity( &g_entities[i] );
				}
			}
			return qtrue;
		}
		else if (Q_stricmp (cmd1, "Teleports") == 0) 
		{
			int i;
			for (i = 0; i < MAX_GENTITIES; i++) 
			{
				if(g_entities[i].classname != NULL)
				{
					if(!strcmp(g_entities[i].classname, "blocked_teleporter"))
						G_FreeEntity( &g_entities[i] );
				}
			}
			return qtrue;
		}
		else if (Q_stricmp (cmd1, "spawns") == 0)
		{
			int i;
			for (i = 0; i < level.spawnCount; i++)
			{
				memset(&level.spawns[i], 0, sizeof(level.spawns[i]));
				level.spawnCount--;
			}
			return qtrue;
		}
		else if (Q_stricmp (cmd1, "Bsps") == 0) 
		{
			int i;
			for (i = 0; i < MAX_GENTITIES; i++) 
			{
				if(g_entities[i].classname != NULL)
				{
					if(!strcmp(g_entities[i].classname, "blocker"))
						G_FreeEntity( &g_entities[i] );
				}
			}
			return qtrue;
		}
		else 
		{
			CPe(va("print \"@^3Usage: ^3/remove <type>\n  - ^3Valid Types:  ^7Spawns - Bsps - Teleports - Blockers - Walls - Ent\n"));
			return qtrue;
		}
	}
	else if (Q_stricmp ( cmd, "list" ) == 0 ){
		if (Q_stricmp ( cmd1, "blockers" ) == 0 ){
			int		i;
			CPe(va("print \"@\n------- ^5Blockers ^7 -------\n^3 Ent #  ^7|  ^3Origin\n^7-------------------------\n"));
			for (i = 0; i < MAX_GENTITIES; i++) 
			{
				if(g_entities[i].classname != NULL)
				{
					if(!strcmp(g_entities[i].classname, "blocked_trigger"))
						CPe(va("print \"@^5%4i    ^7| ^3%6.0f %6.0f %6.0f\n", i, g_entities[i].s.origin[0], g_entities[i].s.origin[1], g_entities[i].s.origin[2] ));
				}
			}		
			CPe(va("print \"@------ End of List ------\n^3 Player:^7 %6.0f %6.0f %6.0f\n", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2]));
			return qtrue;
		}
		else if (Q_stricmp ( cmd1, "walls" ) == 0 ){
			int		i;
			CPe(va("print \"@\n--------- ^5Walls ^7---------\n^3 Ent #  ^7|  ^3Origin\n^7-------------------------\n"));
			for (i = 0; i < MAX_GENTITIES; i++)
			{
				if(g_entities[i].classname != NULL)
				{
					if(!strcmp(g_entities[i].classname, "func_wall"))
						CPe(va("print \"@^5%4i    ^7| ^3%6.0f %6.0f %6.0f\n", i, g_entities[i].s.origin[0], g_entities[i].s.origin[1], g_entities[i].s.origin[2] ));
				}
			}
			CPe(va("print \"@------ End of List ------\n^3 Player:^7 %6.0f %6.0f %6.0f\n", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2]));
			return qtrue;
		}
		else if (Q_stricmp ( cmd1, "teleports" ) == 0 ){
			int		i;
			CPe(va("print \"@\n------- ^5Teleports^7 -------\n^3 Ent #  ^7|  ^3Origin\n^7-------------------------\n"));
			for (i = 0; i < MAX_GENTITIES; i++)
			{
				if(g_entities[i].classname != NULL)
				{
					if(!strcmp(g_entities[i].classname, "blocked_teleporter"))
						CPe(va("print \"@^5%4i    ^7| ^3%6.0f %6.0f %6.0f\n", i, g_entities[i].s.origin[0], g_entities[i].s.origin[1], g_entities[i].s.origin[2] ));
				}
			}
			CPe(va("print \"@------ End of List ------\n^3 Player:^7 %6.0f %6.0f %6.0f\n", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2]));
			return qtrue;
		}
		else if (Q_stricmp ( cmd1, "bsps" ) == 0 ){
			int		i;
			CPe(va("print \"@\n--------- ^5BSP's ^7---------\n^3 Ent #  ^7|  ^3Origin\n^7-------------------------\n"));
			for (i = 0; i < MAX_GENTITIES; i++) 
			{
				if(g_entities[i].classname != NULL)
				{
					if(!strcmp(g_entities[i].classname, "blocker"))
						CPe(va("print \"@^5%4i    ^7| ^3%6.0f %6.0f %6.0f\n", i, g_entities[i].s.origin[0], g_entities[i].s.origin[1], g_entities[i].s.origin[2] ));
				}
			}
			CPe(va("print \"@------ End of List ------\n^3 Player:^7 %6.0f %6.0f %6.0f\n", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2]));
			return qtrue;
		}
		else if (Q_stricmp ( cmd1, "models" ) == 0 ){
			int		i;
			CPe(va("print \"@\n--------- ^5Models ^7--------\n^3 Ent #  ^7|  ^3Origin\n^7-------------------------\n"));
			for (i = 0; i < MAX_GENTITIES; i++) 
			{
				if(g_entities[i].classname != NULL)
				{
					if(!strcmp(g_entities[i].classname, "model_static"))
						CPe(va("print \"@^5%4i    ^7| ^3%6.0f %6.0f %6.0f\n", i, g_entities[i].s.origin[0], g_entities[i].s.origin[1], g_entities[i].s.origin[2] ));
				}
			}
			CPe(va("print \"@------ End of List ------\n^3 Player:^7 %6.0f %6.0f %6.0f\n", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2]));
			return qtrue;
		}		
		else if (Q_stricmp ( cmd1, "triggers" ) == 0 ){
			int		i;
			CPe(va("print \"@\n--------- ^5Models ^7--------\n^3 Ent #  ^7|  ^3Origin\n^7-------------------------\n"));
			for (i = 0; i < MAX_GENTITIES; i++) 
			{
				if(g_entities[i].classname != NULL)
				{
					if(!Q_strncmp(g_entities[i].classname, "gametype", 8))
					{
						if(g_entities[i].targetname != NULL)
							CPe(va("print \"@^5%4i    ^7| ^3%6.0f %6.0f %6.0f  (%s)\n", i, g_entities[i].s.origin[0], g_entities[i].s.origin[1], g_entities[i].s.origin[2],g_entities[i].targetname ));
						else
							CPe(va("print \"@^5%4i    ^7| ^3%6.0f %6.0f %6.0f\n", i, g_entities[i].s.origin[0], g_entities[i].s.origin[1], g_entities[i].s.origin[2] ));
					}
				}
			}
			CPe(va("print \"@------ End of List ------\n^3 Player:^7 %6.0f %6.0f %6.0f\n", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2]));
			return qtrue;
		}
		else if (Q_stricmp ( cmd1, "notnull" ) == 0 ){
			int		i;
			CPe(va("print \"@\n--------- ^5Not Null ^7--------\n^3 Ent #  ^7|  ^3Origin\n^7-------------------------\n"));
			for (i = 0; i < MAX_GENTITIES; i++) 
			{
				if(g_entities[i].classname != NULL)
				{
					if(!strcmp(g_entities[i].classname, "info_notnull"))
						CPe(va("print \"@^5%4i    ^7| ^3%6.0f %6.0f %6.0f\n", i, g_entities[i].s.origin[0], g_entities[i].s.origin[1], g_entities[i].s.origin[2] ));
				}
			}
			CPe(va("print \"@------ End of List ------\n^3 Player:^7 %6.0f %6.0f %6.0f\n", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2]));
			return qtrue;
		}
		else
			CPe(va("print \"@^3Usage: ^3/list <type>\n  - ^3Valid Types:  ^7Bsps - Teleports - Blockers - Walls - Models\n"));
		return qtrue;
	}
	return qfalse;
}
	
void NV_EditEntInfo (gentity_t *ent)
{
	char cmd[128] = "\0";
	char cmd1[128] = "\0";

	if(!nv_editMap.integer || !ent->client->nvSess.admin >= 4)
		return;
	if(!ent->client->nvSess.editEnt)
		return;

	trap_Argv( 1, cmd, sizeof( cmd ) );
	trap_Argv( 2, cmd1, sizeof( cmd1 ) );

	NV_ChangeEntField(&g_entities[ent->client->nvSess.editEnt], cmd, cmd1);
}

/*
qboolean NV_changeEntInfo( gentity_t *ent ) 
{
	char arg[16] = "\0";
	int		i;

	/// Hault and give a warning because the game will crash. Allow 50 ents
	/// ... for safety incase some are created w/out freeing (missles, players, etc) 
	if(level.num_entities == MAX_GENTITIES - 50){
		CPe(va("cp \"^1WARNING! - Adding more ^3ENTITIES ^1could crash the server!!\n\""));
		return qfalse;
	}
	
	trap_Argv( 1, arg, sizeof(arg));

	for ( i = 0 ; i < sizeof( entFields ) / sizeof( entFields[0] ) ; i++ ) {
		if ( !Q_stricmp( arg, entFields[i].cmd ) ) {
			entFields[i].function(ent);
			return qtrue;
		}
	}
	return qfalse;
}*/




/*
void NV_itemList		(gentity_t *ent)
{
	int		i;
	int		len = 0;
	char	string[1024] = "";
	char	*line;

	CPe(va("print \"---------------------------------------\n\""));

	for( i = 0; i < sizeof(commands)/sizeof(commands[0]) - 1; i++) {
		line = va("  ^4%-12s ^3- ^7%s\n", fields[i].cmd, fields[i].usage);
		strncpy( &string[len], line, 128 );
		len += strlen( line );

		if ( len > 750 ){
			string[len++] = 0;
			CPe(va("print \"%s\"", string));
			len = 0;
		}
	}
	if ( len ){
		string[len++] = 0;
		CPe(va("print \"%s\"", string));
	}
	CPe(va("print \"---------------------------------------\n\""));

	///CPe(va("print \"NOTE: Anything between ^3< >^7 is ^1OPTIONAL^7!\nTo see more info on command type a '?' after the cmd (ex. /adm pop ?)\n\""));
}*/


#endif

