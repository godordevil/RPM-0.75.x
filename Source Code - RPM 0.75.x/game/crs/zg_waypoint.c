#include "../g_local.h"

#ifdef _CRS_BOTS

#include "zg_waypoint.h"


int OrgVisibleBox(vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore);

int wp_totalCount;
wayPoint_t *wpHead;
wayPoint_t *wpEnd;
wayPoint_t *wp_free = NULL;
int wp_thinkTime;

// debug rendering
qboolean wp_DebugRender;
wayPoint_t *wpRender;
wayPoint_t *wp_current;

wayPoint_t *startPath = NULL;
wayPoint_t *endPath = NULL;

qboolean wp_GeneratePoints;
qboolean wp_CalcNeighbours;
wayPoint_t *wpLastNeighbour;
int wp_totalNeighbours;

void WP_BeginPointGeneration(void);
void WP_GenerateZPoints(void);
qboolean WP_PosValid(vec3_t pos);
void WP_FindNeighbours(wayPoint_t *point);
void WP_CalcNeighboursLoop(void);

// for calc'n points
vec3_t workPoint;
qtime_t wp_GenerateStart;
int wp_thinkTime;

wayPoint_t *GetNearestVisibleWP(vec3_t org, int ignore);
qboolean WP_CalcPath(wayPoint_t *start, wayPoint_t *goal, qboolean debug, wayPoint_t *path[], wpHistory_t history[]);
void G_SetFlags(void);

void G_TestPath(gentity_t *ent)
{
	if (startPath && endPath)
	{
		startPath = NULL;
		endPath = NULL;
		Com_Printf(S_COLOR_YELLOW"G_TestPath():- test path cleared\n");
		return;
	}

	if (startPath == NULL)
	{
		startPath = GetNearestVisibleWP(ent->client->ps.origin, ent->s.number);
		Com_Printf(S_COLOR_YELLOW"G_TestPath():- got start path\n");
	}

	if (endPath == NULL)
	{
		endPath = GetNearestVisibleWP(ent->client->ps.origin, ent->s.number);
	}
	
	if (endPath == startPath)
	{
		endPath = NULL;
	}

	if (endPath)
	{
		Com_Printf(S_COLOR_YELLOW"G_TestPath():- got end path\n");
	}
}

void G_TestLine(vec3_t start, vec3_t end, int color, int time)
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

/*
=================
WP_CreateWayPoint
=================
*/
wayPoint_t *WP_CreateWayPoint(vec3_t origin, vec3_t angle, int flags, int index)
{
	wayPoint_t *wp;
	int n;
	
	wp = NULL;
	if (wp_free)
	{
		wp = wp_free;
		wp_free = wp_free->next;
		wp->next = NULL;
	}
	
	if (wp == NULL)
	{
		wp = (wayPoint_t *)trap_VM_LocalAlloc (sizeof(*wp));
	}
	
	if (!wp)
	{
		Com_Printf(S_COLOR_YELLOW"WP_CreateWayPoint():- error - could not find or create WP\n");
	}

	wp->free = qfalse;
	wp->origin[0] = (int)origin[0];
	wp->origin[1] = (int)origin[1];
	wp->origin[2] = (int)origin[2];
	wp->next = NULL;

	if (index != -1)
	{
		wp->index = index;
	}
	else
	{
		wp->index = wp_totalCount + 1;
	}

	for(n = 0; n < MAX_NEIGHBOURS; n++)
	{
		wp->nIndexes[n] = 0;
		wp->neighBours[n] = NULL;
	}
	
	if (wpHead == NULL)
	{
		wpHead = wp;
	}
	else if (wpEnd)
	{	
		wpEnd->next = wp;
	}
	
	wp->flags = flags;
	wp->angle[0] = (int)angle[0];
	wp->angle[1] = (int)angle[1];
	wp->angle[2] = (int)angle[2];
	
	
	wpEnd = wp;
	wp_totalCount++;
	
	return wp;
}

/*
=================
WP_GetPointByIndex
=================
*/
wayPoint_t *WP_GetPointByIndex(int index)
{
	wayPoint_t *next;

	next = wpHead;
	
	while(next)
	{
		if (next->index == index)
		{
			return next;
		}

		next = next->next;
	}

	return NULL;
}

/*
=================
WP_DebugRender
=================
*/
void WP_DebugRender(void)
{
	gentity_t *player;
	wayPoint_t *next;
	float dist;
	vec3_t vec, top, bottom;
	int i, n, count, check;
	gspawn_t *spawn;

	if (startPath && endPath)
	{
		WP_CalcPath(startPath, endPath, qtrue, NULL, NULL);

		VectorCopy(startPath->origin, top);
		top[2] += 16;
		VectorCopy(startPath->origin, bottom);
		bottom[2] -= 16;
		G_TestLine(top, bottom, 0x0000ff, 500);

		VectorCopy(endPath->origin, top);
		top[2] += 16;
		VectorCopy(endPath->origin, bottom);
		bottom[2] -= 16;
		G_TestLine(top, bottom, 0x0000ff, 500);
		return;
	}
	
	player = &g_entities[0]; //only show info to the first client
	if (!player || !player->client)
	{
		return;
	}

	for (i = 0; i < level.spawnCount; i++)
	{
		spawn = &level.spawns[i];
		VectorSubtract(player->client->ps.origin, spawn->origin, vec);
		dist = VectorLength(vec);

		if (dist < 1536)
		{
			VectorCopy(spawn->origin, top);
			top[2] += 32;
			VectorCopy(spawn->origin, bottom);
			bottom[2] -= 32;
			G_TestLine(top, bottom, 0x00ff00, 500);
		}
	}
	
	next = wpRender;
	count = 0;
	check = 0;

	while((check < wp_totalCount) && (count < 16))
	{
		if (next == NULL)
		{
			next = wpHead;
		}
		
		VectorSubtract(player->client->ps.origin, next->origin, vec);
		dist = VectorLength(vec);

		if (dist < 384)
		{
			for(i = 0; i < MAX_NEIGHBOURS; i++)
			{
				if (!next->neighBours[i])
				{
					continue;
				}
				
				//G_TestLine(best->origin, best->neighBours[i]->origin, 0xff0000, 500);
				for(n = 0; n < MAX_NEIGHBOURS; n++)
				{
					if (next->neighBours[i]->neighBours[n] == next)
					{
						break;
					}
				}

				// one way link
				if (n == MAX_NEIGHBOURS)
				{
					VectorSubtract(next->neighBours[i]->origin, next->origin, vec);
					dist = VectorLength(vec);			
					VectorNormalize(vec);
					VectorMA(next->origin, dist - 32, vec, top);
					G_TestLine(next->origin, top, 0xff0000, 500);
				}					
				else if (next->neighBours[i]->index > next->index)
				{
					G_TestLine(next->origin, next->neighBours[i]->origin, 0x0000ff, 500);
				}
			}

			VectorCopy(next->origin, top);
			VectorCopy(next->origin, bottom);

			if (wp_current && (next == wp_current))
			{
				top[2] += 32;
				bottom[2] -= 32;
				G_TestLine(top, bottom, 0xff0000, 500);
			}
			else
			{
				top[2] += 2;
				bottom[2] -= 2;
				G_TestLine(top, bottom, 0x0000ff, 500);
			}
			
			wpRender = next;
			count++;
		}
		
		next = next->next;
		check++;
	}
}
/*{
	gentity_t *player;
	wayPoint_t *next;
	vec3_t vec, top, bottom;
	int i, n, count, check;
	float dist;
	gspawn_t *spawn;

	if (startPath && endPath)
	{
		wayPoint_t *path[MAX_WPPATH];		
		WP_CalcPath(startPath, endPath, qtrue, path);

		VectorCopy(startPath->origin, top);
		top[2] += 16;
		VectorCopy(startPath->origin, bottom);
		bottom[2] -= 16;
		G_TestLine(top, bottom, 0x0000ff, 500);

		VectorCopy(endPath->origin, top);
		top[2] += 16;
		VectorCopy(endPath->origin, bottom);
		bottom[2] -= 16;
		G_TestLine(top, bottom, 0x0000ff, 500);
		return;
	}
	
	player = &g_entities[0]; //only show info to the first client
	if (!player || !player->client)
	{
		return;
	}

	for (i = 0; i < level.spawnCount; i++)
	{
		spawn = &level.spawns[i];
		VectorSubtract(player->client->ps.origin, spawn->origin, vec);
		dist = VectorLength(vec);

		if (dist < 1536)
		{
			VectorCopy(spawn->origin, top);
			top[2] += 32;
			VectorCopy(spawn->origin, bottom);
			bottom[2] -= 32;
			G_TestLine(top, bottom, 0x00ff00, 500);
		}
	}
	
	next = wpRender;
	count = 0;
	check = 0;

	while((check < wp_totalCount) && (count < 8))
	{
		if (next == NULL)
		{
			next = wpHead;
		}
		
		VectorCopy(next->origin, top);
		top[2] += 4;
		VectorCopy(next->origin, bottom);
		bottom[2] -= 4;
		G_TestLine(top, bottom, 0xff0000, 500);
			
		for(i = 0; i < MAX_NEIGHBOURS; i++)
		{
			if (next->neighBours[i])
			{
				for(n = 0; n < MAX_NEIGHBOURS; n++)
				{
					if (next->neighBours[i]->neighBours[n] == next)
					{
						break;
					}
				}

				// one way link
				if (n == MAX_NEIGHBOURS)
				{
					VectorSubtract(next->neighBours[i]->origin, next->origin, vec);
					dist = VectorLength(vec);			
					VectorNormalize(vec);
					VectorMA(next->origin, dist - 32, vec, top);
					G_TestLine(next->origin, top, 0xff0000, 500);
				}					
				else if (next->neighBours[i]->index > next->index)
				{
					G_TestLine(next->origin, next->neighBours[i]->origin, 0x0000ff, 500);
				}
			}
		}
		
		wpRender = next;
		count++;
		
		next = next->next;
		check++;
	}
	
	if (wp_current)
	{
		VectorCopy(wp_current->origin, top);
		top[2] += 32;
		VectorCopy(wp_current->origin, bottom);
		bottom[2] -= 32;
		G_TestLine(top, bottom, 0xff0000, 500);
	}
}
*/

/*
=================
WP_DebugRenderSwitch
=================
*/
void WP_DebugRenderSwitch(void)
{
	if (wp_DebugRender)
	{
		wp_DebugRender = qfalse;
		Com_Printf(S_COLOR_YELLOW "WP_DebugRenderSwitch(): wp_DebugRender = qfalse\n");
	}
	else
	{
		wp_DebugRender = qtrue;
		Com_Printf(S_COLOR_YELLOW "WP_DebugRenderSwitch(): wp_DebugRender = qtrue\n");
	}
}


/*
=================
WP_Think
=================
*/
void WP_Think(void)
{
	if (!wp_GeneratePoints && !wp_CalcNeighbours && !wp_DebugRender)
	{
		return;
	}
	
	if (wp_thinkTime > level.time - 250)
	{
		return;
	}
	
	wp_thinkTime = level.time;

	if (wp_GeneratePoints)
	{
		int i;
		for (i = 0; i < 512; i++)
		{
			WP_GenerateZPoints();
			if (!wp_GeneratePoints)
			{
				// finished generating
				break;
			}
		}

		if (!wp_GeneratePoints)
		{
			qtime_t current;
			int m, s;

			trap_RealTime(&current);
		
			m = (current.tm_min - wp_GenerateStart.tm_min) * 60;
			s = (current.tm_sec - wp_GenerateStart.tm_sec) + m;
			
			if (s < 0)
			{
				s += 60;
				m -= 1; 
			}
			
			Com_Printf(S_COLOR_YELLOW "WP_Think() : Finished WP_GenerateZPoints() loop in %2i sec - wp_totalCount = %i\n", s, wp_totalCount);
			trap_RealTime(&wp_GenerateStart);
			wp_CalcNeighbours = qtrue;
			wpLastNeighbour = wpHead;
		}
		return;
	}
	
	if (wp_CalcNeighbours)
	{
		WP_CalcNeighboursLoop();

		if (!wp_CalcNeighbours)
		{
			qtime_t current;
			int m, s;

			trap_RealTime(&current);
		
			m = (current.tm_min - wp_GenerateStart.tm_min) * 60;
			s = (current.tm_sec - wp_GenerateStart.tm_sec) + m;
			
			if (s < 0)
			{
				s += 60;
				m -= 1; 
			}
			
			Com_Printf(S_COLOR_YELLOW "WP_Think() : Finished WP_CalcNeighboursLoop() in %2i sec - wp_totalNeighbours = %i\n", s, wp_totalNeighbours);
			trap_RealTime(&wp_GenerateStart);
		}
		return;
	}

	if (wp_DebugRender)
	{
		WP_DebugRender();
	}
}


/*
=================
G_WPAtPos

Get wp at crosshair pos or current ent pos
=================
*/
wayPoint_t *G_WPAtPos(gentity_t *ent)
{
	vec3_t start, end, vec, angles;
	wayPoint_t *next, *best;
	trace_t tr;
	float dist, best_dist;

	VectorCopy( ent->client->ps.origin, start);
	start[2] += ent->client->ps.viewheight;

	// Handle leaning
	VectorCopy(ent->client->ps.viewangles, angles);
	if ( ent->client->ps.pm_flags & PMF_LEANING )
	{
		BG_ApplyLeanOffset ( &ent->client->ps, start);
	}

	AngleVectors(angles, vec, NULL, NULL );
	
	if (angles[PITCH] < 80)
	{
		VectorMA (start, 8192, vec, end);

		trap_Trace (&tr, start, NULL, NULL, end, ent->s.number, MASK_SHOT);

		next = wpHead;
		best = NULL;
		best_dist = 64;
	
		while(next)
		{
			VectorSubtract(tr.endpos, next->origin, vec);
			dist = VectorLength(vec);
			if (dist < best_dist)
			{
				best_dist = dist;
				best = next;
			}
			next = next->next;
		}
	
		if (best != NULL)
		{
			return best;
		}
	}
	
	next = wpHead;
	best = NULL;
	best_dist = 64;
	
	while(next)
	{
		VectorSubtract(start, next->origin, vec);
		dist = VectorLength(vec);
		if (dist < best_dist)
		{
			best_dist = dist;
			best = next;
		}
		next = next->next;
	}
	
	return best;
}


/*
=================
WP_Link
=================
*/
qboolean WP_Link(wayPoint_t *a, wayPoint_t *b)
{
	int i;
	qboolean unlinked;
	
	unlinked = qfalse;
	
	
	if (!a || !b)
	{
		Com_Printf(S_COLOR_YELLOW"WP_Link():- !a || !b\n");
		return qfalse;
	}

	for(i = 0; i < MAX_NEIGHBOURS; i++)
	{
		if (a->neighBours[i] == b)
		{
			a->neighBours[i] = NULL;
			unlinked = qtrue;
			break;
		}
	}

	for(i = 0; i < MAX_NEIGHBOURS; i++)
	{
		if (b->neighBours[i] == a)
		{
			b->neighBours[i] = NULL;
			unlinked = qtrue;
			break;
		}
	}
	
	if (unlinked)
	{
		return qtrue;
	}
	
	for(i = 0; i < MAX_NEIGHBOURS; i++)
	{
		if (a->neighBours[i] == NULL)
		{
			a->neighBours[i] = b;
			break;
		}
	}

	for(i = 0; i < MAX_NEIGHBOURS; i++)
	{
		if (b->neighBours[i] == NULL)
		{
			b->neighBours[i] = a;
			break;
		}
	}

	return qfalse;
}


/*
=================
WP_OneWayLink
=================
*/
qboolean WP_OneWayLink(wayPoint_t *from, wayPoint_t *to)
{
	int i;
	
	if (!from || !to)
	{
		Com_Printf(S_COLOR_YELLOW"WP_OneWayLink():- !from || !to\n");
		return qfalse;
	}

	for(i = 0; i < MAX_NEIGHBOURS; i++)
	{
		if (from->neighBours[i] == to)
		{
			return qtrue; // already linked
		}
	}

	for(i = 0; i < MAX_NEIGHBOURS; i++)
	{
		if (from->neighBours[i] == NULL)
		{
			from->neighBours[i] = to;
			return qtrue;
		}
	}

	return qfalse;
}

/*
=================
G_EditWPS
=================
*/
void G_EditWPS(gentity_t *ent)
{
	gclient_t *client;
	wayPoint_t *point;
	int i;
	
	client = ent->client;

	if ((client->buttons & BUTTON_RELOAD) && !(client->oldbuttons & BUTTON_RELOAD))
	{
		point = WP_CreateWayPoint(ent->client->ps.origin, ent->client->ps.viewangles, 0, -1);
		
		if (wp_current && point && (point != wp_current))
		{
			if (WP_Link(wp_current, point))
			{
				Com_Printf(S_COLOR_YELLOW"G_EditWPS():- UNlinking point %i with point %i\n", wp_current->index, point->index);
			}
			else
			{
				Com_Printf(S_COLOR_YELLOW"G_EditWPS():- linking point %i with point %i\n", wp_current->index, point->index);
			}
		}
		else
		{
			Com_Printf(S_COLOR_YELLOW"G_EditWPS():- creating current point ready for linking\n");
		}
		wp_current = point;
	}
	
	if ((client->buttons & BUTTON_USE) && !(client->oldbuttons & BUTTON_USE))
	{
		if (wp_current)
		{
			point = G_WPAtPos(ent);
			if (point && (point != wp_current))
			{
				if (client->buttons & BUTTON_WALKING)
				{
					if (WP_OneWayLink(wp_current, point))
					{
						Com_Printf(S_COLOR_YELLOW"G_EditWPS():- one way link from point %i to point %i\n", wp_current->index, point->index);
					}
					else
					{
						Com_Printf(S_COLOR_YELLOW"G_EditWPS():- couldn't one way link point %i to point %i\n", wp_current->index, point->index);
					}
				}					
				else if (WP_Link(wp_current, point))
				{
					Com_Printf(S_COLOR_YELLOW"G_EditWPS():- UNlinking point %i with point %i\n", wp_current->index, point->index);
				}
				else
				{
					Com_Printf(S_COLOR_YELLOW"G_EditWPS():- linking point %i with point %i\n", wp_current->index, point->index);
				}
			}
			else
			{
				Com_Printf(S_COLOR_YELLOW"G_EditWPS():- coudldn't find wp to link with\n");
			}
		}
		else
		{
			point = WP_CreateWayPoint(ent->client->ps.origin, ent->client->ps.viewangles, 0, -1);
			if (wp_current == NULL)
			{
				wp_current = point;
				Com_Printf(S_COLOR_YELLOW"G_EditWPS():- creating current point\n");
			}
			else
			{
				Com_Printf(S_COLOR_YELLOW"G_EditWPS():- creating point\n");
			}
		}
	}
	
	if ((client->buttons & BUTTON_ATTACK) && !(client->oldbuttons & BUTTON_ATTACK))
	{
		Com_Printf(S_COLOR_YELLOW"G_EditWPS():- selecting current point\n");
		wp_current = G_WPAtPos(ent);
	}

	if ((client->buttons & BUTTON_ALT_ATTACK) && !(client->oldbuttons & BUTTON_ALT_ATTACK))
	{
		Com_Printf(S_COLOR_YELLOW"G_EditWPS():- clearing current point\n");
		wp_current = NULL;
	}

	if ((client->buttons & BUTTON_FIREMODE) && !(client->oldbuttons & BUTTON_FIREMODE))
	{
		if (wp_current)
		{
			Com_Printf(S_COLOR_YELLOW"G_EditWPS():- deleting current point\n");
			point = wpHead;
			while(point)
			{
				for(i = 0; i < MAX_NEIGHBOURS; i++)
				{
					if (point->neighBours[i] == wp_current)
					{
						point->neighBours[i] = NULL;
					}
				}
				
				if (point->index > wp_current->index)
				{
					point->index--;
				}
				point = point->next;
			}

			point = wpHead;
			while(point)
			{
				if (point->next == wp_current)
				{
					point->next = wp_current->next;
					if (wpHead == wp_current)
					{
						wpHead = point;
					}

					if (wpEnd == wp_current)
					{
						wpEnd = point;
					}
					break;
				}
				point = point->next;
			}

			wp_current->free = qtrue;
			wp_current->next = NULL;
			if (wp_free)
			{
				wp_current->next = wp_free;
			}
			VectorClear(wp_current->origin);
			VectorClear(wp_current->angle);

			wp_totalCount--;
			wp_current->index = -1;
			
			wp_free = wp_current;
			wp_current = NULL;
		}
	}
}

/*
=================
G_CheckClientWPEdit
=================
*/
void G_CheckClientWPEdit(gentity_t *ent)
{

	if (!g_cheats.integer)
		return;

	if (ent != &g_entities[0])
	{
		return;
	}
		
	if (bot_editWPS.integer)
	{
		G_EditWPS(ent);
	}
}


/*
=================
G_SavePoints
=================
*/
void G_SavePoints(void)
{
	char storeString[1024];
	char routePath[128];
	wayPoint_t *next;
	vmCvar_t mapname;
	fileHandle_t f;
	int check, i;

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0 );
	Com_sprintf(routePath, 128, "botroutes/%s_%s.wps\0", mapname.string, level.gametypeData->name);

	trap_FS_FOpenFile(routePath, &f, FS_WRITE);

	if (!f)
	{
		Com_Printf(S_COLOR_RED "ERROR: Could not open file \"%s\"to write point data\n", routePath);
		return;
	}

	// clear file first
	Com_sprintf(storeString, 1024, "");
	trap_FS_Write(routePath, strlen(storeString), f);
	trap_FS_FCloseFile(f);

	// reopen it
	trap_FS_FOpenFile(routePath, &f, FS_APPEND_TEXT);
	Com_sprintf(storeString, 1024, "points\n[\n");
	trap_FS_Write(storeString, strlen(storeString), f);

	check = 0;
	next = wpHead;
	while(check < wp_totalCount && next)
	{
		Com_sprintf(storeString, 1024, "%i %i %i %i %i %i %i", (int)next->origin[0], (int)next->origin[1], (int)next->origin[2], (int)next->angle[0], (int)next->angle[1], (int)next->angle[2], next->flags);
		trap_FS_Write(storeString, strlen(storeString), f);

		for(i = 0; i < 8; i++)
		{
			if (next->neighBours[i] == NULL)
			{
				Com_sprintf(storeString, sizeof(storeString), " 0");
			}		
			else
			{
				Com_sprintf(storeString, sizeof(storeString), " %i", next->neighBours[i]->index);
			}		
			trap_FS_Write(storeString, strlen(storeString), f);
		}
		
		Com_sprintf(storeString, sizeof(storeString), "\n");
		trap_FS_Write(storeString, strlen(storeString), f);

		check++;
		next = next->next;
	}

	Com_sprintf(storeString, 1024, "]\n");
	trap_FS_Write(storeString, strlen(storeString), f);
	trap_FS_FCloseFile(f);

	Com_Printf(S_COLOR_YELLOW "Point data has been saved\n");
}


static void *pointsFile;
static void *pointsGroup;

/*
=================
G_LoadPointsFile
=================
*/
static qboolean G_LoadPointsFile(char *filename)
{
	pointsFile = trap_GP_ParseFile(filename, qtrue, qfalse);

	if (!pointsFile)
	{
		return qfalse;
	}

	//pointsGroup = trap_GP_GetBaseParseGroup(pointsFile);

	return qtrue;
}


/*
=================
G_LinkNeighbours
=================
*/
void G_LinkNeighbours(void)
{
	wayPoint_t *next;
	int i;

	next = wpHead;
	
	while(next)
	{
		for(i = 0; i < MAX_NEIGHBOURS; i++)
		{
			if (next->nIndexes[i] == 0)
				continue;
					
			next->neighBours[i] = WP_GetPointByIndex(next->nIndexes[i]);
		}
			
		next = next->next;
	}
}


/*
=================
G_LoadPoints
=================
*/
void G_LoadPoints(void)
{
	char routePath[128];
	vmCvar_t mapname;
	char temp[1024];
	const char *s;
	char *token;
	vec3_t origin, angle;
	wayPoint_t *point;
	TGPGroup pairs;
	TGPValue list;
	int i, j;

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0 );
	Com_sprintf(routePath, 128, "botroutes/%s_%s.wps\0", mapname.string, level.gametypeData->name);

	pointsFile = NULL;
	
	if (!G_LoadPointsFile(routePath) || (pointsFile == NULL))
	{
		pointsFile = NULL;
		Com_sprintf(routePath, 128, "botroutes/%s.wps\0", mapname.string);
		if (!G_LoadPointsFile(routePath) || (pointsFile == NULL))
		{
			Com_Printf(S_COLOR_RED "ERROR: Could not open points file \"%s\" to load point data\n", routePath);
			return;
		}
	}
	
	Com_Printf(S_COLOR_YELLOW "Loading points \"%s\"\n", routePath);
	wp_totalCount = 0;

	pointsGroup = trap_GP_GetBaseParseGroup(pointsFile);

	pairs = trap_GPG_FindPair (pointsGroup, "points");
		
	// Run through the list
	list = trap_GPV_GetList(pairs);
	if (list == NULL)
	{
		Com_Printf(S_COLOR_YELLOW "G_LoadPoints - list == NULL\n");
	}
	
	while (list)
	{
		VectorClear(origin);

		trap_GPV_GetName (list, temp);
		//Com_Printf(S_COLOR_YELLOW "G_LoadPoints - temp = %s\n", temp);
		s = temp;

		while(1)
		{
			token = COM_Parse(&s);
			if (!token || !token[0])
			{
				break;
			}
			origin[0] = atoi(token);

			token = COM_Parse(&s);
			if (!token || !token[0])
			{
				break;
			}
			origin[1] = atoi(token);
			
			token = COM_Parse(&s);
			if (!token || !token[0])
			{
				break;
			}
			origin[2] = atoi(token);

			token = COM_Parse(&s);
			if (!token || !token[0])
			{
				break;
			}
			angle[0] = atoi(token);

			token = COM_Parse(&s);
			if (!token || !token[0])
			{
				break;
			}
			angle[1] = atoi(token);
			
			token = COM_Parse(&s);
			if (!token || !token[0])
			{
				break;
			}
			angle[2] = atoi(token);
			
			token = COM_Parse(&s);
			if (!token || !token[0])
			{
				break;
			}
			j = atoi(token);

			point = WP_CreateWayPoint(origin, angle, j, -1);
			
			for(i = 0; i < MAX_NEIGHBOURS; i++)
			{
				point->nIndexes[i] = 0;

				token = COM_Parse(&s);
				if (token)
				{
					point->nIndexes[i] = atoi(token);
				}
			}				
		}
		
		list = trap_GPV_GetNext(list);
	}

	G_LinkNeighbours();
	G_SetFlags();

	trap_GP_Delete(&pointsFile);
}


/*
=================
WP_CalcPath
=================
*/
int lastDebugPath = 0;
static wayPoint_t *openList;
static qboolean totallyFuckedUP = qfalse;

void WP_AddToOpenList(wayPoint_t *point)
{
	wayPoint_t *open, *prev;
	int ourCost, cost;
	int fuckUp = 0;

	// first, clear open - we're NOT on the list
	point->open = NULL;
	
	// if openlist NOT present, just tack us on the end
	if (openList == NULL)
	{
		//Com_Printf(S_COLOR_YELLOW"WP_AddToOpenList():- openList NULL, openList = %i\n", point->index);
		openList = point;
		return;
	}

	// sort open list by total length (pathcost + heuristic)
	ourCost = point->pathcost + point->heuristic;

	open = openList;
	prev = NULL;
	
	while(open)
	{
		fuckUp++;
		if (fuckUp > (wp_totalCount * 2))
		{
			Com_Printf(S_COLOR_YELLOW"WP_AddToOpenList():- loop fucked up\n");
			openList = NULL;
			prev = NULL;
			totallyFuckedUP = qtrue;
			break;
		}

		cost = open->pathcost + open->heuristic;
		
		if (ourCost < cost)
		{
			if (prev == NULL)
			{
				openList = point;
			}
			else
			{
				prev->open = point;
			}

			point->open = open;
			//Com_Printf(S_COLOR_YELLOW"WP_AddToOpenList():- added to openList PREV %i POINT %i NEXT %i\n", prev->index, point->index, open->index);
			break;
		}
		
		prev = open;
		open = open->open;
	}

	// couldn't find a place to insert it, must have highest cost - add to end
	if (prev && (point->open == NULL))
	{
		prev->open = point;
		//Com_Printf(S_COLOR_YELLOW"WP_AddToOpenList():- highest cost - added to openList PREV %i POINT %i NEXT <NULL>\n", prev->index, point->index);
	}
}

void WP_RemoveFromOpenList(void)//wayPoint_t *point)
{
	wayPoint_t *point;

	if (openList == NULL)
	{
		Com_Printf(S_COLOR_YELLOW"WP_RemoveFromOpenList():- openList == NULL - that aint right\n");
		totallyFuckedUP = qtrue;
		return;
	}
			
	point = openList->open;
	openList->open = NULL;
	openList = point;

	/*if (openList == NULL)
	{
		Com_Printf(S_COLOR_YELLOW"WP_RemoveFromOpenList():- removed %i - openList now NULL\n", point->index);
	}
	else
	{
		Com_Printf(S_COLOR_YELLOW"WP_RemoveFromOpenList():- removed %i\n", point->index);
	}*/
}

void WP_ClearAll(void)
{
	wayPoint_t *next;
	
	next = wpHead;
	while(next)
	{
		next->parent = NULL;
		next->open = NULL;
		next->pathcost = 0;
		next->heuristic = 0;
		next = next->next;
	}
}

void WP_RenderGoalPath(wayPoint_t *goal)
{
	wayPoint_t *path;
	vec3_t top, bottom;
	int fuckUp = 0;
	
	path = goal;
	while(path)
	{
		fuckUp++;
		if (fuckUp > (wp_totalCount * 2))
		{
			Com_Printf(S_COLOR_YELLOW"WP_RenderGoalPath():- loop fucked up\n");
			break;
		}

		if (path->parent)
		{
			VectorCopy(path->origin, top);
			top[2] += 16;
			VectorCopy(path->origin, bottom);
			bottom[2] -= 16;
			G_TestLine(top, bottom, 0x0000ff, 110);
			G_TestLine(path->origin, path->parent->origin, 0xff0000, 110);
		}
		
		path = path->parent;
	}
}

void WP_MakePath(wayPoint_t *goal, wayPoint_t *path[])
{
	wayPoint_t *point;
	int i;
	int fuckUp = 0;

	for(i = 0; i < MAX_WPPATH; i++)
	{
		path[i] = NULL;
	}
	
	point = goal;
	while(point)
	{
		fuckUp++;
		if (fuckUp > (wp_totalCount * 2))
		{
			Com_Printf(S_COLOR_YELLOW"WP_MakePath():- loop fucked up\n");
			break;
		}

		for(i = MAX_WPPATH - 1; i > 0; i--)
		{
			path[i] = path[i - 1];
		}
		path[0] = point;
		
		point = point->parent;
		
		// HACK to stop infinite loop
		// FIX ME - you cheap bastard!
		//point->parent = NULL;
	}
}

/*
==============
WP_HistoryCost
==============
*/
int WP_HistoryCost(wpHistory_t history[], wayPoint_t *point)
{
	wpHistory_t *wpHist;
	int i;
	
	for(i = 0; i < MAX_WP_HISTORY; i++)
	{
		wpHist = &history[i];
		
		// if we find this point already in the history, grab it
		if (wpHist->index == point->index)
		{
			return wpHist->costAdjustment;
		}
	}
	
	return 0;
}

qboolean WP_CalcPath(wayPoint_t *start, wayPoint_t *goal, qboolean debug, wayPoint_t *path[], wpHistory_t history[])
{
	wayPoint_t *current, *temp;
	vec3_t vec;
	int i;
	int fuckUp = 0;
	
	if (totallyFuckedUP)
	{
		return qfalse;
	}	
	
	if ((start == NULL) || (goal == NULL))
	{
		Com_Printf(S_COLOR_YELLOW"WP_CalcPath():- (start == NULL) || (goal == NULL)\n");
		return qfalse;
	}

	// first up, all points costs, parents etc
	// FIX ME - really needed?
	WP_ClearAll();

	// open list - points we CAN look at
	openList = NULL;
	
	// add start to the open list, calc path cost and heuristic
	WP_AddToOpenList(start);

	start->parent = NULL;
	start->pathcost = 0;
	VectorSubtract(goal->origin, start->origin, vec);
	start->heuristic = (int)VectorLength(vec);
	
	current = NULL;
	
	// while we have points we can look at, keep trying
	while(openList)
	{
		fuckUp++;
		if (fuckUp > (wp_totalCount * 2))
		{
			Com_Printf(S_COLOR_YELLOW"WP_CalcPath():- loop fucked up\n");
			current = NULL;
			totallyFuckedUP = qtrue;
			break;
		}

		// grab the first point from the open list
		// openlist is sorted by distance, so this SHOULD be the shortest path
		current = openList;

		// remove the current point from the openlist
		//WP_RemoveFromOpenList(current);
		WP_RemoveFromOpenList();
		
		// if the first point on the openList is our goal, we made it!
		if (current == goal)
		{
			//Com_Printf(S_COLOR_YELLOW"WP_CalcPath():- first point on the openList is our goal\n");
			break;
		}
		
		// go through all our neighbours and put them in the open list
		for(i = 0; i < MAX_NEIGHBOURS; i++)
		{
			temp = current->neighBours[i];
			if (temp == NULL)
			{
				continue;
			}
			
			// already been put in path
			if (temp->parent)
			{
				continue;
			}
			
			// skip any pathes that loop back to previous points
			if ((temp == start) || (temp == current->parent))
			{
				continue;
			}
			
			// add this point to the open list
			temp->parent = current;

			// calculate cost of path to here
			VectorSubtract(current->origin, temp->origin, vec);
			temp->pathcost = current->pathcost + (int)VectorLength(vec);

			// try to avoid ladders... they suck
			if (temp->flags & WPFLAG_LADDER)
			{
				temp->pathcost += 384;
			}
			
			// add to this any cost adjustments maid in its history
			if (history)
			{
				temp->pathcost += WP_HistoryCost(history, temp);
			}
			
			// calculate direction dist cost to goal
			VectorSubtract(goal->origin, temp->origin, vec);
			temp->heuristic = (int)VectorLength(vec);

			// add this point to the open list
			// NOTE : WP_AddToOpenList() sorts points by total length
			WP_AddToOpenList(temp);
		}
	}
	
	if (startPath && endPath && (goal == endPath))
	{
		WP_RenderGoalPath(goal);
	}

	if ((current == goal) && path)
	{
		WP_MakePath(goal, path);
	}
	/*if (current == goal)
	{
		//WP_RenderGoalPath(goal);
		Com_Printf(S_COLOR_YELLOW"WP_CalcPath():- PATH FOUND\n");
	}
	else
	{
		Com_Printf(S_COLOR_YELLOW"WP_CalcPath():- COULD NOT FIND PATH\n");
	}*/
	
	return (current == goal);
}

wayPoint_t *GetNearestVisibleWPToItem(vec3_t org, int ignore)
{
	vec3_t dir, mins, maxs;
	wayPoint_t *next, *best;
	float dist, bestdist;

	bestdist = 256;

	mins[0] = -15;
	mins[1] = -15;
	mins[2] = -1;
	maxs[0] = 15;
	maxs[1] = 15;
	maxs[2] = 1;

	next = wpHead;
	best = NULL;
	
	while (next)
	{
		VectorSubtract(org, next->origin, dir);
		dist = VectorLength(dir);

		if ((dist < bestdist) && trap_InPVS(org, next->origin) && OrgVisibleBox(org, mins, maxs, next->origin, ignore))
		{
			bestdist = dist;
			best = next;
		}

		next = next->next;
	}

	return best;
}


void G_SetFlags(void)
{
	wayPoint_t *next;
	//int ladder; 
	
	next = wpHead;
	while(next)
	{
		// check for any ladders and adjust our position to be centered on it
		// FIX ME - woah, what about bit arse weird ladders?
		// shit... I hate ladders... don't you?
		next->ladder = BG_FindLadder (next->origin);
		
		if (next->ladder > -1)
		{
			next->flags |= WPFLAG_LADDER;
		}
		next = next->next;
	}
}

// AUTO GEN 
/*
=================
WP_PosValid

Makes sure vec pos is with-in world limits
=================
*/
qboolean WP_PosValid(vec3_t pos)
{
	if (pos[0] < level.worldMins[0] || pos[1] < level.worldMins[1]
	 || pos[0] > level.worldMaxs[0] || pos[1] > level.worldMaxs[1]
	  || pos[2] < level.worldMins[2] || pos[2] > level.worldMaxs[2])
		return qfalse;

	return qtrue;
}


/*
=================
WP_BeginPointGeneration
=================
*/
void WP_BeginPointGeneration(void)
{
	// figure out the first workPoint
	workPoint[0] = (int)level.worldMaxs[0];
	workPoint[1] = (int)level.worldMaxs[1];
	workPoint[2] = (int)level.worldMaxs[2];
	
	// delay first think just a tad
	wp_thinkTime = level.time + 500;

	trap_RealTime(&wp_GenerateStart);

	wp_totalCount = 0;
	wpHead = NULL;
	wpEnd = NULL;
	wp_GeneratePoints = qtrue;
	wp_CalcNeighbours = qfalse;
	wpLastNeighbour = NULL;
	wp_totalNeighbours = 0;

	Com_Printf(S_COLOR_YELLOW "WP_BeginPointGeneration()\n");
}


/*
=================
WP_GenerateZPoints

Creates way points that occupy X + Y at every possible Z point
=================
*/
void WP_GenerateZPoints(void)
{
	float pointMaxs, minWalkNormal;
	int mask, maxStep, maxZStep;
	vec3_t point, end, roof;
	vec3_t mins, maxs;
	trace_t trace;
	
	mask = MASK_DEADSOLID;

	// don't be as accurate on RMG maps, since they're generally MUCH larger
	if (RMG.integer)
	{
		maxStep = MAX_RMG_WP_STEP;
		maxZStep = MAX_RMG_WP_STEP_Z;
	}
	else
	{
		maxStep = MAX_WP_STEP;
		maxZStep = MAX_WP_STEP_Z;
	}
	
	mins[0] = -15;
	mins[1] = -15;
	maxs[0] = 15;
	maxs[1] = 15;
	
	// make the initial roof vec higher than start pos
	VectorCopy(workPoint, roof);
	roof[2] += 8;
	while(workPoint[2] > level.worldMins[2])
	{
		qboolean canOccupy = qtrue;

		// find the ceiling
		trap_Trace(&trace, workPoint, NULL, NULL, roof, -1, mask);
		if ((trace.fraction == 1.0f) || trace.allsolid)
		{
			canOccupy = qfalse; // no ceiling
		}
				
		if (canOccupy)
		{
			// find the floor
			VectorCopy(workPoint, end);
			end[2] = level.worldMins[2];
			mins[2] = 0;
			maxs[2] = 0;
			trap_Trace(&trace, workPoint, mins, maxs, end, -1, mask);

			if ((trace.fraction == 1.0f) || trace.allsolid)
			{
				// no floor
				canOccupy = qfalse;
			}
			else
			{
				// found ground - make sure we can walk on it
				minWalkNormal = 0.7f;
				if (trace.contents & CONTENTS_TERRAIN)
				{
					minWalkNormal = 0.625f; //MIN_WALK_NORMAL_TERRAIN;
				}

				if (trace.plane.normal[2] < minWalkNormal)
				{
					// too steep
					canOccupy = qfalse;
				}
			}
		}

		if (canOccupy)
		{
			// this X/Y pos has a floor and a ceiling - now make sure we
			// can actually occupy this X/Y pos properly (standing or ducked)
			VectorCopy(trace.endpos, point);
			point[2] -= MINS_Z;

			mins[2] = MINS_Z;
			maxs[2] = DEFAULT_PLAYER_Z_MAX;
			trap_Trace(&trace, point, mins, maxs, point, -1, mask);
			if ((trace.fraction != 1.0f) || trace.allsolid)
			{
				maxs[2] = CROUCH_PLAYER_Z_MAX;
				trap_Trace(&trace, point, mins, maxs, point, -1, mask);
				if ((trace.fraction != 1.0f) || trace.allsolid)
				{
					// there is not enough room, skip this X/Y pos
					canOccupy = qfalse;
				}
			}
		
			pointMaxs = maxs[2];
		}

		VectorCopy(workPoint, roof);

		if (canOccupy)
		{
			VectorCopy(point, workPoint);
			WP_CreateWayPoint(point, vec3_origin, 0, -1);
		}

		workPoint[2] -= maxZStep;
	}

	workPoint[2] = (int)level.worldMaxs[2];
	workPoint[0] -= (int)maxStep;

	// check if new point is valid, and move if not
	if (!WP_PosValid(workPoint))
	{
		workPoint[2] = (int)level.worldMaxs[2];
		workPoint[0] = (int)level.worldMaxs[0];
		workPoint[1] -= (int)maxStep;

		// if this point is invalid, we're completely done!!
		if (!WP_PosValid(workPoint))
		{
			// stop trying to generate points
			wp_GeneratePoints = qfalse;
		}
	}
}

/*
=================
WP_AlreadyLinked
=================
*/
qboolean WP_AlreadyLinked(wayPoint_t *a, wayPoint_t *b)
{
	int i, j;
	
	if (!a || !b)
	{
		return qfalse;
	}

	for(i = 0; i < MAX_NEIGHBOURS; i++)
	{
		if (a->neighBours[i] == b)
		{
			for(j = 0; j < MAX_NEIGHBOURS; j++)
			{
				if (b->neighBours[j] == a)
				{
					return qtrue;
				}
			}
			break;
		}
	}

	return qfalse;	
}

/*
=================
WP_ForceLink
=================
*/
qboolean WP_ForceLink(wayPoint_t *a, wayPoint_t *b)
{
	int i;
	qboolean linkA;
	qboolean linkB;
	
	linkA = qtrue;
	linkB = qtrue;
	
	
	if (!a || !b)
	{
		return qfalse;
	}

	for(i = 0; i < MAX_NEIGHBOURS; i++)
	{
		if (a->neighBours[i] == b)
		{
			linkA = qfalse;
			break;
		}
	}

	for(i = 0; i < MAX_NEIGHBOURS; i++)
	{
		if (b->neighBours[i] == a)
		{
			linkB = qfalse;
			break;
		}
	}
	
	if (linkA)
	{
		for(i = 0; i < MAX_NEIGHBOURS; i++)
		{
			if (a->neighBours[i] == NULL)
			{
				a->neighBours[i] = b;
				break;
			}
		}
	}
	
	if (linkB)
	{
		for(i = 0; i < MAX_NEIGHBOURS; i++)
		{
			if (b->neighBours[i] == NULL)
			{
				b->neighBours[i] = a;
				break;
			}
		}
	}
	
	if (linkA)
	{
		return qtrue;
	}
	else if (linkB)
	{
		return qtrue;
	}

	return qfalse;
}

/*
=================
WP_FindNeighbours
=================
*/
void WP_FindNeighbours(wayPoint_t *point)
{
	int i, j, n, maxStep;
	float x, y;
	vec3_t nPoints[MAX_NEIGHBOURS];
	wayPoint_t *next;
	wayPoint_t *best[MAX_NEIGHBOURS];
	trace_t trace;
	float calc;
	vec3_t mins, maxs;
	VectorSet(mins, -4, -4, -4);
	VectorSet(maxs, 4, 4, 4);

	if (RMG.integer)
	{
		maxStep = MAX_RMG_WP_STEP;
	}
	else
	{
		maxStep = MAX_WP_STEP;
	}

	for(i = 0; i < MAX_NEIGHBOURS; i++)
	{
		VectorClear(nPoints[i]);
		best[i] = NULL;
	}
		
	n = 0;
	for(i = -1; i <= 1; i++)
	{
		x = (int)point->origin[0] + (maxStep * i);
		for(j = -1; j <= 1; j++)
		{
			if ((i == 0) && (j == 0))
				continue; // skip center
				
			y = (int)point->origin[1] + (maxStep * j);
			VectorSet(nPoints[n], x, y, (int)point->origin[2]);
			n++;
		}
	}

	next = wpHead;
	
	while(next)
	{
		for(i = 0; i < MAX_NEIGHBOURS; i++)
		{
			if ((int)next->origin[0] != (int)nPoints[i][0])
			{	// wrong pos
				continue;
			}

			if ((int)next->origin[1] != (int)nPoints[i][1])
			{	// wrong pos
				continue;
			}
			
			/*
			VectorSubtract(point->origin, next->origin, dir);
			VectorNormalize(dir);
			if (dir[2] < 0.7f)
			{	// too steep
				continue;
			}
			*/
			if (next->origin[2] > point->origin[2])
			{
				calc = next->origin[2] - point->origin[2];
				if (calc > 64)
				{
					continue;
				}
			}
			
			//trap_Trace(&trace, next->origin, NULL, NULL, point->origin, -1, MASK_DEADSOLID);
			trap_Trace(&trace, next->origin, mins, maxs, point->origin, -1, MASK_DEADSOLID);
			if ((trace.fraction < 1.0f) || trace.allsolid)//|| trace.startsolid)
			{	// can't reach
				continue;
			}

			if ((best[i] == NULL) || (abs(next->origin[2] - point->origin[2]) < abs(best[i]->origin[2] - point->origin[2])))
			{
				best[i] = next;
			}
		}

		next = next->next;
	}

	for(i = 0; i < MAX_NEIGHBOURS; i++)
	{
		if (best[i] != NULL)
		{
			point->neighBours[i] = best[i];
			wp_totalNeighbours++;
		}
	}
}


/*
=================
WP_CalcNeighboursLoop
=================
*/
void WP_CalcNeighboursLoop(void)
{
	wayPoint_t *next;
	int check;

	next = wpLastNeighbour;
	check = 0;
	
	while(next && (check < 128))
	{
		WP_FindNeighbours(next);
		next = next->next;
		check++;
	}
	
	wpLastNeighbour = next;

	if (wpLastNeighbour == NULL)
	{
		wp_CalcNeighbours = qfalse;
	}
}

#endif

