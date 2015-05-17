
#include "../g_local.h"

#ifdef _CRS_BOTS	
#include "../q_shared.h"
#include "../botlib.h"
#include "../be_ea.h"
#include "zai_main.h"
#include "../chars.h"
#include "../inv.h"
#include "../syn.h"

// external functions
int WP_HistoryCost(wpHistory_t history[], wayPoint_t *point);
qboolean WP_CalcPath(wayPoint_t *start, wayPoint_t *goal, qboolean debug, wayPoint_t *path[], wpHistory_t history[]);
wayPoint_t *GetNearestVisibleWPToItem(vec3_t org, int ignore);
int BotSelectIdealWeapon(bot_state_t *bs);
void Cmd_Kill_f(gentity_t *ent);
qboolean Bot_GetIdealTeamDestination(bot_state_t *bs);
void Bot_TeamplayAI(bot_state_t *bs);
float Bot_IsBlind(bot_state_t *bs);
qboolean Bot_VisionObscured(bot_state_t *bs, vec3_t start, vec3_t end);

// functions
int BotGetWeaponRange(bot_state_t *bs);

// defines
//#define BOT_CTF_DEBUG		1
#define BOT_LAUNCH_ANGLES	1
#define BOT_MAX_CHECKOB		30

bot_state_t *botstates[MAX_CLIENTS];		// bot states
int numbots;					// number of bots
float floattime;				// floating point time
boteventtracker_t gBotEventTracker[MAX_CLIENTS];
botEntInfo_t bot_entInfo[MAX_GENTITIES];
int botLastEntInfoUpdate = 0;
botGametypeItemInfo_t bot_GTInfo[MAX_GAMETYPE_ITEMS];

/*
==============
AngleDifference
==============
*/
float AngleDifference(float ang1, float ang2) {
	float diff;

	diff = ang1 - ang2;
	if (ang1 > ang2) {
		if (diff > 180.0) diff -= 360.0;
	}
	else {
		if (diff < -180.0) diff += 360.0;
	}
	return diff;
}

void Bot_GetOriginFromBounds(gentity_t *ent, vec3_t origin)
{
	vec3_t mins, maxs;
	
	VectorAdd(ent->r.currentOrigin, ent->r.mins, mins);
	VectorAdd(ent->r.currentOrigin, ent->r.maxs, maxs);
	origin[0] = (maxs[0] + mins[0]) / 2;
	origin[1] = (maxs[1] + mins[1]) / 2;
	origin[2] = (maxs[2] + mins[2]) / 2;
}


/*
==============
BD - BotDebug
==============
*/
qboolean QDECL BotDebug(bot_state_t *bs, vmCvar_t *cvar, int value, const char *msg, ...)
{
	va_list		argptr;
	char		text[1024];
	gentity_t *player;
	
	// FIX ME - onlt debug certain bot?
	/*if ((bot_debugBot.integer != -1) && (bs->client != bot_debugBot.integer))
	{
	}*/

	player = &g_entities[0]; //only show info to the first client
	if (!player || !player->client)
	{
		return qfalse;
	}
	
	if (bs && player->client->sess.spectatorClient && (player->client->sess.spectatorClient != bs->client))
	{
		return qfalse;
	}
	
	if (cvar->integer != value)
	{
		return qfalse;
	}
	
	if (msg)
	{
		va_start (argptr, msg);
		vsprintf (text, msg, argptr);
		va_end (argptr);
		trap_Printf( text );
	}
	return qtrue;
}

/*
==============
BotTouchesPoint
==============
*/
void Bot_FuckedUP(bot_state_t *bs)
{
	bs->fuckUp++;
	bs->lastFuckUp = level.time;
}


/*
==============
Bot_AddWPHistoryList
==============
*/
wpHistory_t *Bot_AddWPHistoryList(bot_state_t *bs, wayPoint_t *point)
{
	wpHistory_t *wpHist, *best;
	int i, leastUpdated;
	
	leastUpdated = level.time;
	best = NULL;
	
	for(i = 0; i < MAX_WP_HISTORY; i++)
	{
		wpHist = &bs->wpHistory[i];
		
		// if we find this point already in the history, grab it
		if (wpHist->index == point->index)
		{
			best = wpHist;
			break;
		}
		
		// if we find a free spot in the history, grab it
		if (wpHist->index == 0)
		{
			best = wpHist;
			break;
		}
		
		// otherwise, find the least update history and get that
		if (wpHist->lastUpdated < leastUpdated)
		{
			best = wpHist;
			leastUpdated = wpHist->lastUpdated;
		}
	}
	
	return best;
}


/*
==============
Bot_AddToWPHistory
==============
*/
void Bot_AddToWPHistory(bot_state_t *bs, wayPoint_t *point, int extraCost)
{
	wpHistory_t *wpHist;
	
	wpHist = Bot_AddWPHistoryList(bs, point);
	if(!wpHist)
		return;
	if (wpHist->index != point->index)
	{
		wpHist->index = point->index;
		wpHist->costAdjustment = 0;
	}

	wpHist->lastUpdated = level.time;
	wpHist->costAdjustment += extraCost;
}


void BotRandomiseHistory(bot_state_t *bs)
{
	wayPoint_t *next;
	
	next = wpHead;
	
	while(next)
	{
		if (random() <= 0.5)
		{
			Bot_AddToWPHistory(bs, next, Q_irand(128, 1024));
		}
		next = next->next;
	}
}


/*
==============
BotTouchesPoint
==============
*/
qboolean BotTouchesPoint(bot_state_t *bs, vec3_t point) 
{
	if (bs->origin[0] - point[0] > 32
	 || bs->origin[0] - point[0] < -32
	  || bs->origin[1] - point[1] > 32
	   || bs->origin[1] - point[1] < -32
	    || bs->origin[2] - point[2] > 55
	     || bs->origin[2] - point[2] < -50)
	{
		return qfalse;
	}

	return qtrue;
}


/*
==================
BotAI_GetClientState
==================
*/
int BotAI_GetClientState(int clientNum, playerState_t *state)
{
	gentity_t	*ent;

	ent = &g_entities[clientNum];
	if (!ent->inuse)
	{
		return qfalse;
	}
	
	if (!ent->client)
	{
		return qfalse;
	}

	memcpy(state, &ent->client->ps, sizeof(playerState_t));
	
	return qtrue;
}


/*
==============
BotChangeViewAngle
==============
*/
float BotChangeViewAngle(float angle, float ideal_angle, float speed)
{
	float move;

	angle = AngleMod(angle);
	ideal_angle = AngleMod(ideal_angle);
	if (angle == ideal_angle)
	{
		return angle;
	}
	
	move = ideal_angle - angle;
	if (ideal_angle > angle)
	{
		if (move > 180.0)
		{
			move -= 360.0;
		}
	}
	else
	{
		if (move < -180.0)
		{
			move += 360.0;
		}
	}
	
	if (move > 0)
	{
		if (move > speed)
		{
			move = speed;
		}
	}
	else
	{
		if (move < -speed)
		{
			move = -speed;
		}
	}
	
	return AngleMod(angle + move);
}


/*
==============
BotChangeViewAngles
==============
*/
void BotChangeViewAngles(bot_state_t *bs, float thinktime)
{
	float diff, factor, maxchange, anglespeed, disired_speed;
	vec3_t kickAngles;
	int i;

	// Extract the kick angles 
	kickAngles[PITCH] = ((float)bs->cur_ps.kickPitch / 1000.0f);
	kickAngles[YAW] = kickAngles[ROLL] = 0;

	if (bs->ideal_viewangles[PITCH] > 180)
	{
		bs->ideal_viewangles[PITCH] -= 360;
	}
	
	if (bs->currentEnemy && bs->frame_Enemy_Vis)
	{
		factor = bs->skills.turnspeed_combat * bs->settings.skill;
	}
	else
	{
		factor = bs->skills.turnspeed;
	}
	
	maxchange = bs->skills.maxturn * thinktime;

	for (i = 0; i < 2; i++)
	{
		bs->ideal_viewangles[i] = AngleMod(bs->ideal_viewangles[i] + kickAngles[i]);
			
		diff = AngleDifference(bs->viewangles[i], bs->ideal_viewangles[i]);
			
		disired_speed = diff * factor;
			
		bs->viewanglespeed[i] += (bs->viewanglespeed[i] - disired_speed);
			
		if (bs->viewanglespeed[i] > 180)
		{
			bs->viewanglespeed[i] = maxchange;
		}
			
		if (bs->viewanglespeed[i] < -180)
		{
			bs->viewanglespeed[i] = -maxchange;
		}
			
		anglespeed = bs->viewanglespeed[i];
			
		if (anglespeed > maxchange)
		{
			anglespeed = maxchange;
		}
			
		if (anglespeed < -maxchange)
		{
			anglespeed = -maxchange;
		}
			
		bs->viewangles[i] += anglespeed;
		bs->viewangles[i] = AngleMod(bs->viewangles[i]);

		// demping
		bs->viewanglespeed[i] *= 0.45 * (1 - factor);
	}
	
	if (bs->viewangles[PITCH] > 180)
	{
		bs->viewangles[PITCH] -= 360;
	}
	
	//elementary action: view
	trap_EA_View(bs->client, bs->viewangles);
}


/*
==============
BotInputToUserCommand
==============
*/
void BotInputToUserCommand(bot_input_t *bi, usercmd_t *ucmd, int delta_angles[3], int time, int useTime)
{
	vec3_t angles, forward, right;
	short temp;
	int j;

	// clear the whole structure
	memset(ucmd, 0, sizeof(usercmd_t));

	// the duration for the user command in milli seconds
	ucmd->serverTime = time;

	if (bi->actionflags & ACTION_DELAYEDJUMP)
	{
		bi->actionflags |= ACTION_JUMP;
		bi->actionflags &= ~ACTION_DELAYEDJUMP;
	}

	// set the buttons
	if (bi->actionflags & ACTION_RESPAWN) 
	{
		ucmd->buttons = BUTTON_ATTACK;
	}
	
	if (bi->actionflags & ACTION_ATTACK)
	{
		ucmd->buttons |= BUTTON_ATTACK;
	}
	
	if (bi->actionflags & ACTION_ALT_ATTACK)
	{
		ucmd->buttons |= BUTTON_ALT_ATTACK;
	}

	if (bi->actionflags & ACTION_WALK)
	{
		ucmd->buttons |= BUTTON_WALKING;
	}

	// FIX ME - doing it this way be a stupid!
	/*if (useTime < level.time && Q_irand(1, 10) < 5)
	{ //for now just hit use randomly in case there's something useable around
		ucmd->buttons |= BUTTON_USE;
	}*/
	if (bi->actionflags & ACTION_USE)
	{
		ucmd->buttons |= BUTTON_USE;
		bi->actionflags &= ~ACTION_USE;
	}

	if (bi->actionflags & ACTION_RELOAD)
	{
		ucmd->buttons |= BUTTON_RELOAD;
		bi->actionflags &= ~ACTION_RELOAD;
	}

	if (bi->actionflags & ACTION_ZOOMIN)
	{
		ucmd->buttons |= BUTTON_ZOOMIN;
		bi->actionflags &= ~ACTION_ZOOMIN;
	}

	if (bi->actionflags & ACTION_ZOOMOUT)
	{
		ucmd->buttons |= BUTTON_ZOOMOUT;
		bi->actionflags &= ~ACTION_ZOOMOUT;
	}

	ucmd->weapon = bi->weapon;

	// set the view angles
	// NOTE: the ucmd->angles are the angles WITHOUT the delta angles
	ucmd->angles[PITCH] = ANGLE2SHORT(bi->viewangles[PITCH]);
	ucmd->angles[YAW] = ANGLE2SHORT(bi->viewangles[YAW]);
	ucmd->angles[ROLL] = ANGLE2SHORT(bi->viewangles[ROLL]);

	// subtract the delta angles
	for (j = 0; j < 3; j++)
	{
		temp = ucmd->angles[j] - delta_angles[j];
		ucmd->angles[j] = temp;
	}

	//NOTE: movement is relative to the REAL view angles
	// get the horizontal forward and right vector
	// get the pitch in the range [-180, 180]
	if (bi->dir[2])
	{
		angles[PITCH] = bi->viewangles[PITCH];
	}
	else
	{
		angles[PITCH] = 0;
	}
	
	angles[YAW] = bi->viewangles[YAW];
	angles[ROLL] = 0;
	AngleVectors(angles, forward, right, NULL);
	
	// bot input speed is in the range [0, 400]
	bi->speed = bi->speed * 127 / 400;
	
	// set the view independent movement
	ucmd->forwardmove = DotProduct(forward, bi->dir) * bi->speed;
	ucmd->rightmove = DotProduct(right, bi->dir) * bi->speed;
	ucmd->upmove = abs(forward[2]) * bi->dir[2] * bi->speed;
	
	// normal keyboard movement
	if (bi->actionflags & ACTION_MOVEFORWARD)
	{
		ucmd->forwardmove += 127;
	}
	
	if (bi->actionflags & ACTION_MOVEBACK)
	{
		ucmd->forwardmove -= 127;
	}
	
	if (bi->actionflags & ACTION_MOVELEFT)
	{
		ucmd->rightmove -= 127;
	}
	
	if (bi->actionflags & ACTION_MOVERIGHT)
	{
		ucmd->rightmove += 127;
	}
	
	// jump/moveup
	if (bi->actionflags & ACTION_JUMP)
	{
		ucmd->upmove += 127;
	}
	
	//crouch/movedown
	if (bi->actionflags & ACTION_CROUCH)
	{
		ucmd->upmove -= 127;
	}

	//Com_Printf("forward = %d right = %d up = %d\n", ucmd.forwardmove, ucmd.rightmove, ucmd.upmove);
	//Com_Printf("ucmd->serverTime = %d\n", ucmd->serverTime);
}


/*
==============
BotUpdateInput
==============
*/
void BotUpdateInput(bot_state_t *bs, int time, int elapsed_time)
{
	bot_input_t bi;
	int j;

	// add the delta angles to the bot's current view angles
	for (j = 0; j < 3; j++)
	{
		bs->viewangles[j] = AngleMod(bs->viewangles[j] + SHORT2ANGLE(bs->cur_ps.delta_angles[j]));
	}
	
	// change the bot view angles
	BotChangeViewAngles(bs, (float) elapsed_time / 1000);
	
	// retrieve the bot input
	trap_EA_GetInput(bs->client, (float) time / 1000, &bi);
	
	// respawn hack
	if (bi.actionflags & ACTION_RESPAWN) 
	{
		// IF already trying to respawn or a ghost then cancel the respawn
		if ((bs->lastucmd.buttons & BUTTON_ATTACK) || (bs->cur_ps.pm_flags&PMF_GHOST)) 
		{
			bi.actionflags &= ~(ACTION_RESPAWN|ACTION_ATTACK);
		}
	}
	
	// check for weapon buttons
	if (bs->doZoomIn)
	{
		if (bs->cur_ps.pm_flags & PMF_ZOOMED)
		{
			bs->doZoomIn = 0;
		}
		else
		{
			bi.actionflags |= ACTION_ZOOMIN;
		}
	}

	if (bs->doZoomOut)
	{
		if (!(bs->cur_ps.pm_flags & PMF_ZOOMED))
		{
			bs->doZoomOut = 0;
		}
		else
		{
			bi.actionflags |= ACTION_ZOOMOUT;
		}
	}

	if (bs->doReload)
	{
		if ((bs->cur_ps.weaponstate == WEAPON_RELOADING) || (bs->cur_ps.weaponstate == WEAPON_RELOADING_ALT))
		{
			bs->doReload = 0;
		}
		else
		{
			bi.actionflags |= ACTION_RELOAD;
		}
	}

	if (bs->useTime > level.time)
	{
		bi.actionflags |= ACTION_USE;
	}

	// convert the bot input to a usercmd
	BotInputToUserCommand(&bi, &bs->lastucmd, bs->cur_ps.delta_angles, time, bs->noUseTime);
	
	// subtract the delta angles
	for (j = 0; j < 3; j++)
	{
		bs->viewangles[j] = AngleMod(bs->viewangles[j] - SHORT2ANGLE(bs->cur_ps.delta_angles[j]));
	}
}


/*
==============
RemoveColorEscapeSequences
==============
*/
void RemoveColorEscapeSequences(char *text)
{
	int i, l;

	l = 0;
	for (i = 0; text[i]; i++)
	{
		if (Q_IsColorString(&text[i]))
		{
			i++;
			continue;
		}
		
		if (text[i] > 0x7E)
		{
			continue;
		}
		text[l++] = text[i];
	}
	
	text[l] = '\0';
}


/*
==============
BotAI
==============
*/
int BotAI(int client, float thinktime)
{
	char buf[1024], *args;
	bot_state_t *bs;
	int i;

	trap_EA_ResetInput(client);

	bs = botstates[client];
	
	if (!bs || !bs->inuse)
	{
		Com_Printf(S_COLOR_YELLOW"BotAI():- "S_COLOR_RED"ERROR"S_COLOR_YELLOW" bot %i is not setup\n", client);
		return qfalse;
	}

	// retrieve the current player state
	BotAI_GetClientState(client, &bs->cur_ps);

	// retrieve any waiting server commands
	while(trap_BotGetServerCommand(client, buf, sizeof(buf)))
	{
		// have buf point to the command and args to the command arguments
		args = strchr( buf, ' ');
		if (!args)
		{
			continue;
		}
		*args++ = '\0';

		// remove color espace sequences from the arguments
		RemoveColorEscapeSequences(args);

		//if (!Q_stricmp(buf, "cp "))
		//{
		//	/*CenterPrintf*/
		//}
		//else if (!Q_stricmp(buf, "cs"))
		//{
		//	/*ConfigStringModified*/
		//}
		//else if (!Q_stricmp(buf, "scores"))
		//{
		//	/*FIXME: parse scores?*/
		//}
		//else if (!Q_stricmp(buf, "clientLevelShot"))
		//{
		//	/*ignore*/
		//}
	}
	
	// add the delta angles to the bot's current view angles
	for (i = 0; i < 3; i++)
	{
		bs->viewangles[i] = AngleMod(bs->viewangles[i] + SHORT2ANGLE(bs->cur_ps.delta_angles[i]));
	}

	// increase the local time of the bot
	bs->ltime += thinktime;
	bs->thinktime = thinktime;

	// origin of the bot
	VectorCopy(bs->cur_ps.origin, bs->origin);
	
	// eye coordinates of the bot
	VectorCopy(bs->cur_ps.origin, bs->eye);
	bs->eye[2] += bs->cur_ps.viewheight;
	
	// perform standard bot functions
	// ie. find/follow path, shoot people
	StandardBotAI(bs, thinktime);
	
	// subtract the delta angles
	for (i = 0; i < 3; i++)
	{
		bs->viewangles[i] = AngleMod(bs->viewangles[i] - SHORT2ANGLE(bs->cur_ps.delta_angles[i]));
	}

	// check if we want basic bot nav debugging
	// highlight current path if we do
	if (BotDebug(bs, &bot_debugNav, 1, NULL))
	{
		vec3_t bottom, top;
		
		VectorCopy(bs->origin, top);
		top[2] += 64;
		VectorCopy(bs->origin, bottom);
		bottom[2] -= 64;
		G_TestLine(top, bottom, 0x0000ff, 5000);

		if (bs->lastPathDebug < level.time)
		{
			bs->lastPathDebug = level.time + 100;

			if (bs->wpCurrent)
			{
				VectorCopy(bs->wpCurrent->origin, top);
				top[2] += 4;
				VectorCopy(bs->wpCurrent->origin, bottom);
				bottom[2] += 32;
				G_TestLine(top, bottom, 0xff0000, 5000);;
			}

			if (bs->wpDestination)
			{
				VectorCopy(bs->wpDestination->origin, top);
				top[2] -= 4;
				VectorCopy(bs->wpDestination->origin, bottom);
				bottom[2] -= 32;
				G_TestLine(top, bottom, 0xff0000, 5000);;
			}
		
			for(i = 0; i < MAX_WPPATH - 1; i++)
			{
				if (bs->wpPath[i] && bs->wpPath[i + 1])
				{
					G_TestLine(bs->wpPath[i]->origin, bs->wpPath[i + 1]->origin, 0x0000ff, 5000);
				}
			}
		}
	}

	// see if this bot has stuffed up too many times
	if (bs->fuckUp > 1024)
	{
		Com_Printf(S_COLOR_YELLOW"BotAI():- "S_COLOR_RED"ERROR"S_COLOR_YELLOW" bot %i stuffed up - killing bot\n", client);
		Cmd_Kill_f(&g_entities[client]);
		bs->fuckUp = 0;
	}

	// if we haven't fucked up in a while, drop the fuckup check
	if (bs->fuckUp && (level.time - bs->lastFuckUp > 5000))
	{
		bs->fuckUp--;
	}

	// everything was ok
	return qtrue;
}


/*
==============
BotAISetupClient
==============
*/
int BotAISetupClient(int client, struct bot_settings_s *settings, qboolean restart)
{
	bot_state_t *bs;

	if (!botstates[client])
	{
		botstates[client] = B_Alloc(sizeof(bot_state_t));
	}

	memset(botstates[client], 0, sizeof(bot_state_t));

	bs = botstates[client];

	if (bs && bs->inuse)
	{
		Com_Printf(S_COLOR_RED"BotAISetupClient():- client %d already setup\n", client);
		return qfalse;
	}

	memcpy(&bs->settings, settings, sizeof(bot_settings_t));

	// need to know the client number before doing personality stuff
	bs->client = client;

	// initialize default weapon weights
	bs->botWeaponWeights[WP_NONE] = 0;
	bs->botWeaponWeights[WP_KNIFE] = 1;
	bs->botWeaponWeights[WP_M1911A1_PISTOL] = 3;
	bs->botWeaponWeights[WP_USSOCOM_PISTOL] = 2;
	bs->botWeaponWeights[WP_M4_ASSAULT_RIFLE] = 14;
	bs->botWeaponWeights[WP_AK74_ASSAULT_RIFLE] = 14;
	bs->botWeaponWeights[WP_M60_MACHINEGUN] = 14;
	bs->botWeaponWeights[WP_MICRO_UZI_SUBMACHINEGUN] = 8;
	bs->botWeaponWeights[WP_MSG90A1] = 11;
	bs->botWeaponWeights[WP_USAS_12_SHOTGUN] = 13;
	bs->botWeaponWeights[WP_M590_SHOTGUN] = 12;
	bs->botWeaponWeights[WP_MM1_GRENADE_LAUNCHER] = 8;
	bs->botWeaponWeights[WP_RPG7_LAUNCHER] = 16;
	bs->botWeaponWeights[WP_M84_GRENADE] = 6;
	bs->botWeaponWeights[WP_SMOHG92_GRENADE] = 2;
	bs->botWeaponWeights[WP_ANM14_GRENADE] = 2;
	bs->botWeaponWeights[WP_M15_GRENADE] = 2;

	BotUtilizePersonality(bs);
	
	// add some random waypoint history so we
	// don't follow the same points all the tiem
	BotRandomiseHistory(bs);

	bs->inuse = qtrue;
	bs->entitynum = client;
	bs->setupcount = 4;
	bs->entergame_time = FloatTime();
	numbots++;

	if (level.numPlayingClients)
	{ 	// don't talk to yourself
		BotDoChat(bs, "GeneralGreetings", 0);
	}

	return qtrue;
}


/*
==============
BotAIShutdownClient
==============
*/
int BotAIShutdownClient(int client, qboolean restart)
{
	bot_state_t *bs;

	bs = botstates[client];
	if (!bs || !bs->inuse)
	{
		Com_Printf(S_COLOR_YELLOW"BotAIShutdownClient():- client %d already shutdown\n", client);
		return qfalse;
	}

	//clear the bot state
	memset(bs, 0, sizeof(bot_state_t));
	
	// set the inuse flag to qfalse
	bs->inuse = qfalse;
	
	// there's one bot less
	numbots--;
	
	// everything went ok
	return qtrue;
}


/*
==============
BotResetState

called when a bot enters the intermission or observer mode and
when the level is changed
==============
*/
void BotResetState(bot_state_t *bs)
{
	int client, entitynum, inuse;
	int movestate, goalstate, weaponstate;
	bot_settings_t settings;
	playerState_t ps; 		// current player state
	float entergame_time;

	// save some things that should not be reset here
	memcpy(&settings, &bs->settings, sizeof(bot_settings_t));
	memcpy(&ps, &bs->cur_ps, sizeof(playerState_t));
	inuse = bs->inuse;
	client = bs->client;
	entitynum = bs->entitynum;
	movestate = bs->ms;
	goalstate = bs->gs;
	weaponstate = bs->ws;
	entergame_time = bs->entergame_time;
	
	// reset the whole state
	memset(bs, 0, sizeof(bot_state_t));
	
	// copy back some state stuff that should not be reset
	bs->ms = movestate;
	bs->gs = goalstate;
	bs->ws = weaponstate;
	memcpy(&bs->cur_ps, &ps, sizeof(playerState_t));
	memcpy(&bs->settings, &settings, sizeof(bot_settings_t));
	bs->inuse = inuse;
	bs->client = client;
	bs->entitynum = entitynum;
	bs->entergame_time = entergame_time;
}

/*
==============
BotAILoadMap
==============
*/
int BotAILoadMap( int restart )
{
	int			i;

	for (i = 0; i < MAX_CLIENTS; i++)
	{
		if (botstates[i] && botstates[i]->inuse)
		{
			BotResetState(botstates[i]);
			botstates[i]->setupcount = 4;
		}
	}

	return qtrue;
}

/*
==============
OrgVisible
==============
*/
int OrgVisible(vec3_t org1, vec3_t org2, int ignore)
{
	trace_t tr;
	int mask;
	
	mask = (ignore == -1) ? MASK_SOLID : MASK_PLAYERSOLID;

	trap_Trace(&tr, org1, NULL, NULL, org2, ignore, mask);

	if (tr.fraction == 1)
	{
		return 1;
	}

	return 0;
}


/*
==============
OrgVisibleBox
==============
*/
int OrgVisibleBox(vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore)
{
	trace_t tr;
	int mask;
	
	mask = (ignore == -1) ? MASK_SOLID : MASK_PLAYERSOLID;

	trap_Trace(&tr, org1, mins, maxs, org2, ignore, mask);

	if (tr.fraction == 1 && !tr.startsolid && !tr.allsolid)
	{
		return 1;
	}

	return 0;
}


/*
==============
CheckForFunc
==============
*/
int CheckForFunc(vec3_t org, int ignore)
{
	gentity_t *fent;
	vec3_t under;
	trace_t tr;

	VectorCopy(org, under);

	under[2] -= 64;

	trap_Trace(&tr, org, NULL, NULL, under, ignore, MASK_SOLID);

	if (tr.fraction == 1)
	{
		return 0;
	}

	fent = &g_entities[tr.entityNum];

	if (!fent)
	{
		return 0;
	}

	if (strstr(fent->classname, "func_"))
	{
		return 1; //there's a func brush here
	}

	return 0;
}


/*
==============
GetNearestVisibleWP
==============
*/
wayPoint_t *GetNearestVisibleWP(vec3_t org, int ignore)
{
	float bestdist;
	float flLen;
	vec3_t a, mins, maxs;
	wayPoint_t *next, *best;

	bestdist = 800;

	mins[0] = -15;
	mins[1] = -15;
	mins[2] = -1;
	maxs[0] = 15;
	maxs[1] = 15;
	maxs[2] = 1;

	next = wpHead;
	best = NULL;
	
	while(next)
	{
		VectorSubtract(org, next->origin, a);
		flLen = VectorLength(a);

		if (flLen < bestdist && trap_InPVS(org, next->origin) && OrgVisibleBox(org, mins, maxs, next->origin, ignore))
		{
			bestdist = flLen;
			best = next;
		}

		next = next->next;
	}

	return best;
}


/*
==============
Bot_ClearPath
==============
*/
void Bot_ClearPath(bot_state_t *bs)
{
	int i;
	
	for(i = 0; i < MAX_WPPATH; i++)
	{
		bs->wpPath[i] = NULL;
	}
}


/*
==============
WPTouchRoutine
==============
*/
void WPTouchRoutine(bot_state_t *bs)
{
	int i, j;
	wayPoint_t *prevPoint;
	vec3_t vec;
	
	if (!bs->wpCurrent)
	{
		return;
	}

	bs->wpTravelTime = level.time + 10000;

	/*if (bs->wpCurrent->flags & WPFLAG_NOMOVEFUNC)
	{
		bs->noUseTime = level.time + 4000;
	}*/

	// if we've reached our destination, clear it...
	if (bs->wpDestination && (bs->wpCurrent == bs->wpDestination))
	{
		bs->wpDestination = NULL;
	}

	// if we're supposed to be heading to an item and
	// our current point is associated with that item, head there now...
	if (bs->touchGoal && bot_entInfo[bs->touchGoal->s.number].wpClosest && (bot_entInfo[bs->touchGoal->s.number].wpClosest == bs->wpCurrent))
	{
		BotDebug(bs, &bot_debugNav, 1, S_COLOR_YELLOW"WPTouchRoutine():- heading for touchGoal %s\n", bs->touchGoal->classname);

		bs->touchGoalTime = level.time + 1500;
		Bot_ClearPath(bs);

		// if we can't pick our touch goal up,
		// perhaps we need to drop something?
		if (bs->touchGoal->item && !BG_CanItemBeGrabbed(level.gametype, &bs->touchGoal->s, &bs->cur_ps))
		{
			// see if we'd rather have this item then one we already have
			if ((bs->touchGoal->item->giType == IT_WEAPON)
			 && (bs->cur_ps.weapon >= WP_M590_SHOTGUN)
			  && (bs->cur_ps.weapon <= WP_RPG7_LAUNCHER)
			   && (bs->touchGoal->item->outfittingGroup == OUTFITTING_GROUP_PRIMARY)
			    && (bs->botWeaponWeights[bs->cur_ps.weapon] < bs->botWeaponWeights[bs->touchGoal->item->giTag])
			     && (G_DropWeapon (&g_entities[bs->client], bs->cur_ps.weapon, 3000) != NULL))
			{
				BotDebug(bs, &bot_debugNav, 1, S_COLOR_YELLOW"WPTouchRoutine():- I'd rather have %s the my current weapon\n", bs->touchGoal->classname);

				level.clients[bs->client].ps.weapon = WP_KNIFE;
				bs->cur_ps.weapon = WP_KNIFE;
				bs->virtualWeapon = WP_KNIFE;
			}
			else
			{
				BotDebug(bs, &bot_debugNav, 1, S_COLOR_YELLOW"WPTouchRoutine():- can't grab %s\n", bs->touchGoal->classname);
			}
		}
	}

	if (bs->touchGoalTime > level.time)
	{
		if ((bs->touchGoal == NULL) || !bot_entInfo[bs->touchGoal->s.number].wpClosest)
		{
			bs->touchGoal = NULL;
			bs->touchGoalTime = level.time;
		}
		else
		{
			gentity_t *as;

			as = bs->touchGoal;

			if (as->s.eType == ET_GRAPPLE)
			{
				Bot_GetOriginFromBounds(as, vec);
			}
			else
			{
				VectorCopy(as->s.pos.trBase, vec);
			}
			
			if (bot_entInfo[as->s.number].wpClosest && (BotTouchesPoint(bs, vec) || (as->item && (as->r.svFlags & SVF_NOCLIENT))))
			{
				BotDebug(bs, &bot_debugNav, 1, S_COLOR_YELLOW"WPTouchRoutine():- touchGoal %s touched it or non-existant\n", as->classname);
				if (as->item)
				{
					if (as->item->giType == IT_WEAPON)
					{
						BotSelectIdealWeapon(bs);
					}
					else if (as->item->giType == IT_GAMETYPE)
					{
						botGametypeItemInfo_t *GTInfo;
						j = as->s.modelindex - MODELINDEX_GAMETYPE_ITEM;
						
						GTInfo = &bot_GTInfo[j];
						if (GTInfo->use)
						{
							BotDebug(bs, &bot_debugNav, 1, S_COLOR_YELLOW"WPTouchRoutine():- have to use %s\n", as->classname);
							VectorCopy(vec, bs->goalPosition);
							bs->beStill = level.time + 150;
							bs->useTime = level.time + 150;
							bs->wpDestSwitchTime = level.time + 1000;
							return;
						}
					}
				}
				else if (as->s.eType == ET_GRAPPLE)
				{
					botGametypeItemInfo_t *GTInfo;

					if (bs->cur_ps.stats[STAT_GAMETYPE_ITEMS])
					{
						j = -1;
						for (i = 0; i < MAX_GAMETYPE_ITEMS; i++)
						{
							if (bs->cur_ps.stats[STAT_GAMETYPE_ITEMS] & (1<<i))
							{
								j = i;
								break;
							}
						}

						if (j != -1)
						{
							GTInfo = &bot_GTInfo[j];
							if (GTInfo->useTarget)
							{
								BotDebug(bs, &bot_debugNav, 1, S_COLOR_YELLOW"WPTouchRoutine():- have to use %s\n", as->classname);
								VectorCopy(vec, bs->goalPosition);
								bs->beStill = level.time + 150;
								bs->useTime = level.time + 150;
								bs->wpDestSwitchTime = level.time + 1000;
								return;
							}	
						}
					}
				}
				
				bs->touchGoalTime = level.time;
				bs->wpCurrent = bot_entInfo[as->s.number].wpClosest;
				bs->touchGoal = NULL;
			}
			else
			{
				VectorCopy(vec, bs->goalPosition);
				return; 
			}
		}
	}
	else
	{
		// add to the cost of this waypoint so we end up here less
		Bot_AddToWPHistory(bs, bs->wpCurrent, 64);
	}
	
	prevPoint = bs->wpLastPoint;
	bs->wpLastPoint = bs->wpCurrent;

	for(i = 0; i < MAX_WPPATH; i++)
	{
		if (bs->wpPath[i] && (bs->wpCurrent == bs->wpPath[i]))
		{
			// clear this part of the path
			bs->wpPath[i] = NULL;

			// see if there is a next bit to the path
			if (i + 1 < MAX_WPPATH)
			{
				if (bs->wpPath[i + 1])
				{
					bs->wpCurrent = bs->wpPath[i + 1];
					BotDebug(bs, &bot_debugNav, 2, S_COLOR_YELLOW"WPTouchRoutine():- found current wp at path index %i and set new one\n", i);
					return;
				}
			}
		}
	}
	
	if (bs->wpDestination)
	{
		for(i = 0; i < MAX_NEIGHBOURS; i++)
		{
			if (bs->wpCurrent->neighBours[i] == bs->wpDestination)
			{
				bs->wpCurrent = bs->wpDestination;
				
				// grab a new destination ASAP
				// basically, we should have a destination
				// before WPTouchRoutinue() is called again...
				// ... if not, we fucked up somehow, which is bad
				bs->wpDestination = NULL;
				bs->destinationGrabTime = level.time;
				BotDebug(bs, &bot_debugNav, 2, S_COLOR_YELLOW"WPTouchRoutine():- wpCurrent had destination as neighbour, calc new destination\n");
				return;
			}
		}
	}

	for(i = 0; i < MAX_WPPATH; i++)
	{
		for(j = 0; j < MAX_NEIGHBOURS; j++)
		{
			if (bs->wpPath[i] && (bs->wpCurrent->neighBours[j] == bs->wpPath[i]))
			{
				bs->wpCurrent = bs->wpPath[i];
				BotDebug(bs, &bot_debugNav, 2, S_COLOR_YELLOW"WPTouchRoutine():- wpCurrent had bs->wpPath[%i] as neighbour\n", i);
				return;
			}					
		}
	}

	if (bs->wpDestination && WP_CalcPath(bs->wpCurrent, bs->wpDestination, qfalse, bs->wpPath, bs->wpHistory))
	{
		bs->wpCurrent = bs->wpPath[0];
		BotDebug(bs, &bot_debugNav, 2, S_COLOR_YELLOW"WPTouchRoutine():- calc'd more of path to destination\n");
		return;
	}
	
	bs->destinationGrabTime = level.time;

	// we've fucked up - we should have a place to go by now
	Bot_FuckedUP(bs);
	if (bs->fuckUp > 1024)
	{
		//Com_Printf(S_COLOR_YELLOW"WPTouchRoutine():- wpCurrent == NULL, calc a new path ASAP\n");
		Com_Printf(S_COLOR_YELLOW"WPTouchRoutine():- "S_COLOR_RED"ERROR"S_COLOR_YELLOW" bot[%i] -> wpCurrent == NULL and couldn't calc new path!!!\n", bs->client);
	}

	// mother fucker - still no where to go?
	// fuck that, just pick a neighbour and start running..
	// son of a bitch!!
	if (bs->wpCurrent)
	{
		wayPoint_t *neighbours[MAX_NEIGHBOURS];
		int count;
		count = 0;

		for(i = 0; i < MAX_NEIGHBOURS; i++)
		{
			if ((bs->wpCurrent->neighBours[i] != NULL) && (bs->wpCurrent->neighBours[i] != prevPoint))
			{
				neighbours[count++] = bs->wpCurrent->neighBours[i];
			}
		}

		if (count)
		{
			bs->wpCurrent = neighbours[rand() % count];
			BotDebug(bs, &bot_debugNav, 1, S_COLOR_YELLOW"WPTouchRoutine():- wpCurrent == wander round hack\n");
		}
	}
}


/*
==============
MoveTowardIdealAngles
==============
*/
void MoveTowardIdealAngles(bot_state_t *bs)
{
	VectorCopy(bs->goalAngles, bs->ideal_viewangles);
}


/*
==============
PassStandardEnemyChecks
==============
*/
qboolean PassStandardEnemyChecks(bot_state_t *bs, gentity_t *en)
{
	if (!bs || !en)
	{
		return qfalse;
	}

	if (bs->client == en->s.number)
	{
		return qfalse;
	}

	if (!en->client)
	{
		return qfalse;
	}

	if (en->health < 1)
	{
		return qfalse;
	}

	if (!en->takedamage)
	{
		return qfalse;
	}

	if (en->flags & FL_NOTARGET)
	{
		return qfalse;
	}

	if (!en->s.solid)
	{
		return qfalse;
	}

	if (en->client->ps.pm_type != PM_NORMAL)
	{
		return qfalse;
	}

	if (G_IsClientSpectating(en->client))
	{
		return qfalse;
	}

	if (OnSameTeam(&g_entities[bs->client], en))
	{
		return qfalse;
	}

	return qtrue;
}


/*
==============
BotDamageNotification
==============
*/
void BotDamageNotification(gclient_t *bot, gentity_t *attacker)
{
	bot_state_t *bs;
	bot_state_t *bs_a;
	int i;

	if (!bot || !attacker || !attacker->client)
	{
		return;
	}

	bs_a = botstates[attacker->s.number];

	if (bs_a)
	{
		bs_a->lastAttacked = &g_entities[bot->ps.clientNum];
		for(i = 0; i < MAX_CLIENTS; i++)
		{
			if (botstates[i] && i != bs_a->client
			 && (botstates[i]->lastAttacked == &g_entities[bot->ps.clientNum]))
			{
				botstates[i]->lastAttacked = NULL;
			}
		}
	}
	else
	{ 	// got attacked by a real client, so no one gets rights to lastAttacked
		for(i = 0; i < MAX_CLIENTS; i++)
		{
			if (botstates[i] && (botstates[i]->lastAttacked == &g_entities[bot->ps.clientNum]))
			{
				botstates[i]->lastAttacked = NULL;
			}

			i++;
		}
	}

	bs = botstates[bot->ps.clientNum];

	if (!bs)
	{
		return;
	}

	bs->lastHurt = attacker;

	if (bs->currentEnemy)
	{
		return;
	}

	if (PassStandardEnemyChecks(bs, attacker))
	{
		bs->currentEnemy = attacker;
		bs->enemySeenTime = level.time + ENEMY_FORGET_MS;
	}
}


/*
==============
BotCanHear
==============
*/
int BotCanHear(bot_state_t *bs, gentity_t *en, float distance)
{
	float minlen;

	// if we're deaf from a flash bang, we can't here shit
	if (bs->flashbangDeafTime + bs->flashbangDeafFadeTime > level.time)
	{
		return 0;
	}

	if (!en || !en->client)
	{
		return 0;
	}

	if (gBotEventTracker[en->s.number].eventTime < level.time)
	{
		return 0;
	}

	switch(gBotEventTracker[en->s.number].events[gBotEventTracker[en->s.number].eventSequence & (MAX_PS_EVENTS-1)])
	{
	case EV_GLOBAL_SOUND:
		minlen = 256;
		break;

	case EV_FIRE_WEAPON:
	case EV_ALT_FIRE:
		minlen = 512;
		break;

	case EV_STEP_4:
	case EV_STEP_8:
	case EV_STEP_12:
	case EV_STEP_16:
	case EV_FOOTSTEP:
	case EV_SWIM:
		minlen = 256;
		break;

	case EV_JUMP:
		minlen = 256;
		break;

	case EV_ITEM_PICKUP:
	case EV_PAIN:
	case EV_PAIN_WATER:
	case EV_WEAPON_CALLBACK:
	case EV_GOGGLES:
	case EV_NOAMMO:
	case EV_FALL_SHORT:
	case EV_FALL_MEDIUM:
	case EV_FALL_FAR:
		minlen = 256;
		break;

	case EV_WATER_FOOTSTEP:
	case EV_WATER_TOUCH:
	case EV_WATER_LAND:
	case EV_WATER_CLEAR:
		minlen = 384;
		break;

	default:
		// FIX ME - I don't think so - this would mean any-non sound event
		//	is treated like a really loud sound... that don't figure!!
		//minlen = 999999;
		minlen = 0;
		break;
	}

	if (distance <= minlen)
	{
		return 1;
	}

	return 0;
}


/*
==============
Bot_UpdateEventTracker
==============
*/
void Bot_UpdateEventTracker(void)
{
	int i;

	for(i = 0; i < MAX_CLIENTS; i++)
	{
		// updated event
		if (gBotEventTracker[i].eventSequence != level.clients[i].ps.eventSequence)
		{
			gBotEventTracker[i].eventSequence = level.clients[i].ps.eventSequence;
			gBotEventTracker[i].events[0] = level.clients[i].ps.events[0];
			gBotEventTracker[i].events[1] = level.clients[i].ps.events[1];
			gBotEventTracker[i].eventTime = level.time + 0.5;
		}
	}
}


/*
==============
InFieldOfVision
==============
*/
int InFieldOfVision(vec3_t viewangles, float fov, vec3_t angles)
{
	int i;
	float diff, angle;

	for (i = 0; i < 2; i++)
	{
		angle = AngleMod(viewangles[i]);
		angles[i] = AngleMod(angles[i]);
		diff = angles[i] - angle;
		if (angles[i] > angle)
		{
			if (diff > 180.0)
			{
				diff -= 360.0;
			}
		}
		else
		{
			if (diff < -180.0)
			{
				diff += 360.0;
			}
		}
		if (diff > 0)
		{
			if (diff > fov * 0.5)
			{
				return 0;
			}
		}
		else
		{
			if (diff < -fov * 0.5)
			{
				return 0;
			}
		}
	}
	return 1;
}


/*
==============
Bot_ScanForEnemies
==============
*/
int Bot_ScanForEnemies(bot_state_t *bs)
{
	float distcheck, closest, hasEnemyDist;
	int bestindex, i;
	vec3_t a;

	closest = 999999;
	hasEnemyDist = 0;
	bestindex = -1;
	i = 0;

	if (bs->currentEnemy)
	{
		hasEnemyDist = bs->frame_Enemy_Len;
	}

	for(i = 0; i < MAX_CLIENTS; i++)
	{
		if (!PassStandardEnemyChecks(bs, &g_entities[i]))
		{
			continue;
		}
		
		if (!trap_InPVS(g_entities[i].client->ps.origin, bs->eye))
		{
			continue;
		}

		VectorSubtract(g_entities[i].client->ps.origin, bs->eye, a);
		distcheck = VectorLength(a);
		vectoangles(a, a);

		if (distcheck > closest)
		{
			continue;
		}
		
		// if we can't hear them, try and see them
		if (!BotCanHear(bs, &g_entities[i], distcheck))
		{
			// if we're blind, we can't see shit
			if (Bot_IsBlind(bs) > 0.85f)
			{
				continue;
			}
			
			// if not in our field of vision, continue
			if (!InFieldOfVision(bs->viewangles, 90, a))
			{
				continue;
			}

			// if we can't see through the smoke and haze, continue
			if (Bot_VisionObscured(bs, bs->eye, g_entities[i].client->ps.origin))
			{
				continue;
			}
		}

		if (!OrgVisible(bs->eye, g_entities[i].client->ps.origin, -1))
		{
			continue;
		}

		// if we have an enemy, only switch to closer if they
		// are at least 128 units closer to avoid flipping out
		if (!hasEnemyDist || (distcheck < (hasEnemyDist - 128)))
		{
			closest = distcheck;
			bestindex = i;
		}
	}
	
	return bestindex;
}


/*
==============
WaitingForNow

Checks if the bot is doing something along the lines of 
waiting for an elevator to raise up
==============
*/
int WaitingForNow(bot_state_t *bs, vec3_t goalpos)
{
	vec3_t xybot, xywp, a;

	if (!bs->wpCurrent)
	{
		return 0;
	}

	if ((int)goalpos[0] != (int)bs->wpCurrent->origin[0]
	 || (int)goalpos[1] != (int)bs->wpCurrent->origin[1]
	  || (int)goalpos[2] != (int)bs->wpCurrent->origin[2])
	{
		return 0;
	}

	VectorCopy(bs->origin, xybot);
	VectorCopy(bs->wpCurrent->origin, xywp);

	xybot[2] = 0;
	xywp[2] = 0;

	VectorSubtract(xybot, xywp, a);

	if (VectorLength(a) < 16 && bs->frame_Waypoint_Len > 100)
	{
		if (CheckForFunc(bs->origin, bs->client))
		{
			// we're probably standing on an elevator and riding up/down. 
			// Or at least we hope so.
			return 1;
		}
	}
	else if (VectorLength(a) < 64 && bs->frame_Waypoint_Len > 64 && CheckForFunc(bs->origin, bs->client))
	{
		bs->noUseTime = level.time + 2000;
	}

	return 0;
}


/*
==============
BotGetWeaponRange
==============
*/
int BotGetWeaponRange(bot_state_t *bs)
{
	switch (weaponData[bs->cur_ps.weapon].category)
	{
	case CAT_KNIFE:
		return BWEAPONRANGE_MELEE;
	case CAT_PISTOL:
		return BWEAPONRANGE_MID; //short
	case CAT_SHOTGUN: 	
		return BWEAPONRANGE_SHORT; //short
	case CAT_SUB:     	
		return BWEAPONRANGE_MID;
	case CAT_ASSAULT: 	
		return BWEAPONRANGE_LONG;
	case CAT_SNIPER:  	
		return BWEAPONRANGE_VERY_LONG;
	case CAT_HEAVY:   	
		return BWEAPONRANGE_LONG;
	case CAT_GRENADE: 	
		return BWEAPONRANGE_MID; //short
	default:
		return BWEAPONRANGE_MID;
	}
}


/*
==============
BotIsAChickenWuss
==============
*/
int BotIsAChickenWuss(bot_state_t *bs)
{
	int bWRange;

	if (bs->chickenWussCalculationTime > level.time)
	{
		// don't want to keep going between two points...
		return 2;
	}

	bs->chickenWussCalculationTime = level.time + MAX_CHICKENWUSS_TIME;

	if (g_entities[bs->client].health < BOT_RUN_HEALTH)
	{
		return 1;
	}

 	// if we're reloading, keep our distance
	if ((bs->cur_ps.weaponstate == WEAPON_RELOADING) || (bs->cur_ps.weaponstate == WEAPON_RELOADING_ALT))
	{
		return 1;
	}

	bWRange = BotGetWeaponRange(bs);

	if (bWRange == BWEAPONRANGE_MELEE)
	{
		if (!bs->meleeSpecialist)
		{
			return 1;
		}
	}

	if (bs->cur_ps.weapon == WP_NONE)
	{
		return 1;
	}
	
	if (bs->currentEnemy && bs->currentEnemy->client)
	{
		switch(bs->currentEnemy->client->ps.weapon)
		{
		case WP_USAS_12_SHOTGUN:         
		case WP_M590_SHOTGUN:            
		case WP_KNIFE:
			if (bs->frame_Enemy_Len < 1024)
			{
				switch(bs->cur_ps.weapon)
				{
				case WP_USAS_12_SHOTGUN:         
				case WP_M590_SHOTGUN:            
				case WP_KNIFE:
					break;
				default:
					return 1;
				}
			}
			break;

		default:
			break;
		}
	}

	// didn't run, reset the timer
	bs->chickenWussCalculationTime = 0;

	return 0;
}


/*
==============
Bot_GetNearestBadThing
==============
*/
gentity_t *Bot_GetNearestBadThing(bot_state_t *bs)
{
	gentity_t *ent, *best;
	float dist, best_dist, factor;
	vec3_t vec;
	int i;
	
	best_dist = 9999;
	best = NULL;
	
	ent = &g_entities[0];
	for (i = 0; i < level.num_entities; i++, ent++) 
	{
		if (!ent->inuse)
		{
			continue;
		}

		if (!bot_entInfo[i].valid)
		{
			continue;
		}
		
		if (!bot_entInfo[i].dangerous)
		{
			continue;
		}
		
		// don't be afraid of teammates' explosives if team damage off
		if (ent->r.ownerNum && g_entities[ent->r.ownerNum].client && OnSameTeam(&g_entities[bs->client], &g_entities[ent->r.ownerNum]) && (g_friendlyFire.value <= 0.0))
		{
			continue;
		}
	
		VectorSubtract(bs->origin, ent->r.currentOrigin, vec);
		dist = VectorLength(vec);

		if (ent->s.eType == ET_DAMAGEAREA)
		{
			factor = 1.5;
		}
		else if (!ent->splashDamage)
		{
			factor = 0.5;
		}
		else
		{
			factor = 1;
		}

		if (dist >= best_dist * factor)
		{
			continue;
		}
		
		if (trap_InPVS(bs->origin, ent->s.pos.trBase) && OrgVisible(bs->origin, ent->s.pos.trBase, bs->client) && !Bot_VisionObscured(bs, bs->origin, ent->s.pos.trBase))
		{
			best = ent;
			best_dist = dist;
		}
	}

	if (best)
	{
		bs->dontGoBack = level.time + 1500;
	}
	
	return best;
}


/*
==============
EntityVisibleBox
==============
*/
int EntityVisibleBox(vec3_t org1, vec3_t mins, vec3_t maxs, vec3_t org2, int ignore, int ignore2)
{
	trace_t tr;

	trap_Trace(&tr, org1, mins, maxs, org2, ignore, MASK_SOLID);

	if (tr.fraction == 1 && !tr.startsolid && !tr.allsolid)
	{
		return 1;
	}
	else if (tr.entityNum != ENTITYNUM_NONE && tr.entityNum == ignore2)
	{
		return 1;
	}

	return 0;
}


/*
==============
Bot_WantsToGuardGTI
==============
*/
qboolean Bot_WantsToGuardGTI(bot_state_t *bs, gentity_t *ent)
{
	botGametypeItemInfo_t *GTInfo;
	
	GTInfo = &bot_GTInfo[bot_entInfo[ent->s.number].GTI];
	
	if (ent->flags & FL_DROPPED_ITEM)
	{
		if (GTInfo->guardDropped & (1<<level.clients[bs->client].sess.team))
		{
			return qtrue;
		}
	}
	
	return (GTInfo->guard & (1<<level.clients[bs->client].sess.team));
}


/*
==============
Bot_WantsToPickupGTI
==============
*/
qboolean Bot_WantsToPickupGTI(bot_state_t *bs, gentity_t *ent)
{
	botGametypeItemInfo_t *GTInfo;
	
	GTInfo = &bot_GTInfo[bot_entInfo[ent->s.number].GTI];
	
	if (ent->flags & FL_DROPPED_ITEM)
	{
		if (GTInfo->droppedTeam & (1<<level.clients[bs->client].sess.team))
		{
			return qtrue;
		}
	}
	
	return (GTInfo->team & (1<<level.clients[bs->client].sess.team));
}


/*
==============
BotDoesWant
==============
*/
qboolean BotDoesWant(bot_state_t *bs, gentity_t *ent)
{
	if (!ent || !ent->item)
	{
		return qfalse;
	}

	if (ent->r.svFlags & SVF_NOCLIENT)
	{
		return qfalse;
	}
	
	// See if teh item can be picked up
	if (ent->s.eFlags & EF_NOPICKUP)
	{
		return qfalse;
	}

	if (ent->item->giType == IT_GAMETYPE)
	{
		if (Bot_WantsToPickupGTI(bs, ent))
		{
			BotDebug(bs, &bot_debugGameType, 1, S_COLOR_YELLOW"BotDoesWant():- wants %sgti -> %s\n", (ent->flags & FL_DROPPED_ITEM) ? "dropped " : "", ent->classname);
			return qtrue;
		}
		return qfalse;
	}
	// the same pickup rules are used for client side and server side
	else if (!BG_CanItemBeGrabbed(level.gametype, &ent->s, &bs->cur_ps)) 
	{
		// see if we'd rather have this item then one we already have
		if ((ent->item->giType == IT_WEAPON) && (bs->cur_ps.weapon >= WP_M590_SHOTGUN) && (bs->cur_ps.weapon <= WP_RPG7_LAUNCHER) && (ent->item->outfittingGroup == OUTFITTING_GROUP_PRIMARY))
		{
			if (bs->botWeaponWeights[bs->cur_ps.weapon] < bs->botWeaponWeights[ent->item->giTag])
			{
				return qtrue;
			}
		}
		return qfalse;
	}

	return qtrue;
}


/*
==============
BotSetNeedsAndWants
==============
*/
void BotSetNeedsAndWants(bot_state_t *bs)
{
	int i;

	bs->wantsNewWeapon = qfalse;
	bs->wantsMoreAmmo = qfalse;

	bs->needDecentWeapon = qfalse;
	bs->needExtraAmmo = qfalse;
	
	for(i = WP_M590_SHOTGUN; i < WP_M84_GRENADE; i++)
	{
		if (bs->cur_ps.stats[STAT_WEAPONS] & (1<<i))
		{
			break;
		}	
	}

	if (i == WP_M84_GRENADE)
	{
		// no primary weapon yet - get one asap!
		bs->needDecentWeapon = qtrue;
	}

	// if we don't have at least one extra clip, we like this more
	if (bs->cur_ps.ammo[weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].ammoIndex] < weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].clipSize)
	{
		bs->needExtraAmmo = qtrue;
	}
			
	if (bs->cur_ps.clip[ATTACK_NORMAL][bs->cur_ps.weapon] < (weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].clipSize / 2))
	{
		bs->needExtraAmmo = qtrue;
	}

	bs->needHealth = (bs->cur_ps.stats[STAT_HEALTH] < 30);
	bs->needArmor = (bs->cur_ps.stats[STAT_REALARMOR] < 30);
	bs->wantsHealth = (bs->cur_ps.stats[STAT_HEALTH] != MAX_HEALTH);
	bs->wantsArmor = (bs->cur_ps.stats[STAT_REALARMOR] != MAX_HEALTH);
}


/*
==============
BotHasNeeds
==============
*/
qboolean BotHasNeeds(bot_state_t *bs)
{
	return (bs->needDecentWeapon || bs->needExtraAmmo || bs->needHealth || bs->needArmor);
}


/*
==============
BotHasWants
==============
*/
qboolean BotHasWants(bot_state_t *bs)
{
	if (bs->wantsNewWeapon || bs->wantsMoreAmmo || bs->wantsHealth || bs->wantsArmor);
	{
		// 50/50 we go for what we want
		if (random() < 0.5)
		{
			return qtrue;
		}
	}
	
	return qfalse;
}


/*
==============
BotGetItemWeight
==============
*/
int BotGetItemWeight(bot_state_t *bs, gentity_t *ent)
{
	float weight, f;
	int i;
	
	if (!ent || !ent->item)
	{
		return 0;
	}
	
	weight = 0;
	f = 1.0;
	if (strcmp(ent->classname, "pickup_armor_big") == 0)
	{
		weight = 5;
	}
	else if (strcmp(ent->classname, "pickup_armor_medium") == 0)
	{
		weight = 4;
	}
	else if (strcmp(ent->classname, "pickup_armor_small") == 0)
	{
		weight = 3;
	}
	else if (strcmp(ent->classname, "pickup_health_big") == 0)
	{
		weight = 8;
	}
	else if (strcmp(ent->classname, "pickup_health_small") == 0)
	{
		weight = 4;
	}
	else if (ent->item->giType == IT_WEAPON)
	{
		weight = bs->botWeaponWeights[ent->item->giTag];
		for(i = WP_M590_SHOTGUN; i < WP_M84_GRENADE; i++)
		{
			if (bs->cur_ps.stats[STAT_WEAPONS] & (1<<i))
			{
				break;
			}	
		}
		
		if (i == WP_M84_GRENADE)
		{
			// no primary weapon yet - get one asap!
			f = 3;
		}
	}
	else if (ent->item && ent->item->giType == IT_AMMO)
	{
		weight = 3;

		if (weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].ammoIndex == ent->item->giTag)
		{
			// if we don't have at least one extra clip, we like this more
			if (bs->cur_ps.ammo[ent->item->giTag] < weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].clipSize)
			{
				f = 3;
			}
			else
			{
				f = 2;
			}
			
			if (bs->cur_ps.clip[ATTACK_NORMAL][bs->cur_ps.weapon] < (weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].clipSize / 2))
			{
				f += 1;
			}
		}
	}
	else if (ent->item->giType == IT_BACKPACK)
	{
		weight = 10;
	}
	else if (ent->item->giType == IT_GAMETYPE)
	{
		if (bot_GTInfo[bot_entInfo[ent->s.number].GTI].team & (1<<level.clients[bs->client].sess.team))
		{
			weight = 25;
			BotDebug(bs, &bot_debugGameType, 1, S_COLOR_YELLOW"BotGetItemWeight():- got weight for gti -> %s\n", ent->classname);
		}
	}

	if (ent->item->giType == IT_HEALTH)
	{
		f = 1 + (1 - (bs->cur_ps.stats[STAT_HEALTH] / 100));
		if (bs->cur_ps.stats[STAT_HEALTH] < 30)
		{
			f += 1;
		}
	}
	else if (ent->item->giType == IT_ARMOR)
	{
		f = 1 + (1 - (bs->cur_ps.stats[STAT_REALARMOR] / 100));
		if (bs->cur_ps.stats[STAT_REALARMOR] < 30)
		{
			f += 1;
		}
	}
	
	weight *= f;

	if (bs->needHealth && (ent->item->giType != IT_HEALTH) && (ent->item->giType != IT_BACKPACK))
	{
		f *= 0.5;
	}
	else if (bs->needDecentWeapon && ((ent->item->giType != IT_WEAPON) || (ent->item->outfittingGroup != OUTFITTING_GROUP_PRIMARY)))
	{
		f *= 0.5;
	}
	
	return (int)weight;
}

/*
==============
Bot_UpdateEntInfo
==============
*/
void Bot_UpdateEntInfo(void)
{
	gentity_t *ent;
	int i, j;
	vec3_t vec;
	
	if (botLastEntInfoUpdate > level.time)
	{
		return;
	}
	
	// only update 10 times per second
	// for performances sake
	// FIX ME - improve code you slack arse!!
	botLastEntInfoUpdate = level.time + 100;
	
	ent = &g_entities[0];
	for (i = 0; i < level.num_entities; i++, ent++) 
	{
		if (!ent->inuse)
		{
			continue;
		}

		bot_entInfo[i].valid = qfalse;
		bot_entInfo[i].obscures = qfalse;
		bot_entInfo[i].dangerous = qfalse;

		if (ent->freeAfterEvent)
		{
			bot_entInfo[i].wpClosest = NULL;
			continue;
		}

		if (ent->s.eType == ET_GRAPPLE)
		{
			bot_entInfo[i].valid = qtrue;
			Bot_GetOriginFromBounds(ent, vec);

			if (bot_entInfo[i].wpClosest == NULL)
			{
				bot_entInfo[i].wpClosest = GetNearestVisibleWP(vec, ent->s.number);
				BotDebug(NULL, &bot_debugGameType, 2, S_COLOR_YELLOW"Bot_UpdateEntInfo():- GTI Trigger org = %s, wp = %i\n", vtos(vec), bot_entInfo[i].wpClosest ? bot_entInfo[i].wpClosest->index : -1);
			}
			//BotDebug(NULL, &bot_debugGameType, 2, S_COLOR_YELLOW"r.mins = %s / r.maxs = %s\n", vtos(ent->r.mins), vtos(ent->r.maxs));
			//BotDebug(NULL, &bot_debugGameType, 2, S_COLOR_YELLOW"r.absmin = %s / r.absmax = %s\n", vtos(ent->r.absmin), vtos(ent->r.absmax));
			continue;
		}

		// set if it obscures vision or not
		if ((ent->s.eType == ET_GENERAL) && (ent->s.weapon == WP_M15_GRENADE))
		{
			if ((ent->r.svFlags & SVF_BROADCAST) || (ent->r.svFlags & SVF_NOCLIENT))
			{
				bot_entInfo[i].obscures = qtrue;
				//BotDebug(NULL, &bot_debug, 1, S_COLOR_YELLOW"Bot_UpdateEntInfo():- ent %i obscures!\n", i);
			}
		}

		if (ent->r.svFlags & SVF_NOCLIENT)
		{
			continue;
		}

		if (ent->s.eType == ET_ITEM)
		{	// items we may, may not be able to pick up
			if (!ent->item || (ent->s.eFlags & EF_NOPICKUP))
			{
				continue;
			}
		
			bot_entInfo[i].valid = qtrue;
			if (bot_entInfo[i].wpClosest == NULL)
			{
				bot_entInfo[i].wpClosest = GetNearestVisibleWPToItem(ent->s.pos.trBase, ent->s.number);
			}
			
			// if this is a gametype item, set it as a goal
			// for the appropriate team
			if (ent->item->giType == IT_GAMETYPE)
			{
				for(j = 0; j < MAX_GAMETYPE_ITEMS; j++)
				{
					if (bot_GTInfo[j].index == ITEM_INDEX(ent->item))
					{
						bot_entInfo[i].GTI = j;
						BotDebug(NULL, &bot_debugGameType, 3, S_COLOR_YELLOW"Bot_UpdateEntInfo():- found bot_GTInfo for %s\n", ent->classname);
						break;
					}
				}
			}
		}
		else if (ent->s.eType == ET_MISSILE)
		{
			// all missiles are considered dangerous
			bot_entInfo[i].valid = qtrue;
			bot_entInfo[i].dangerous = qtrue;
		}
	}
}


/*
==============
Bot_GetBestIdleGoal
==============
*/
wayPoint_t *Bot_GetBestIdleGoal(bot_state_t *bs)
{
	int i, weight, highestweight;
	wayPoint_t *point, *desired;
	wayPoint_t *best, *next;
	gentity_t *ent;
	vec3_t vec;
	float dist, best_dist;

	if (bs->isCamper != 2)
	{
		if (bs->randomNavTime < level.time)
		{
			if (!BotHasNeeds(bs) && !BotHasWants(bs) && (Q_irand(1, 10) < 5))
			{
				bs->randomNav = 1;
			}
			else
			{
				bs->randomNav = 0;
			}
			
			bs->randomNavTime = level.time + Q_irand(5000, 15000);
		}
	}

	desired = NULL;
	highestweight = 0;

	if (bs->cur_ps.stats[STAT_GAMETYPE_ITEMS])
	{
		weight = -1;
		for (i = 0; i < MAX_GAMETYPE_ITEMS; i++)
		{
			if (bs->cur_ps.stats[STAT_GAMETYPE_ITEMS] & (1<<i))
			{
				weight = i;
				break;
			}
		}
		
		if (bs->useTime > level.time)
		{
			BotDebug(bs, &bot_debugNav, 1, S_COLOR_YELLOW"Bot_GetBestIdleGoal():- busy using something\n");
			return NULL;
		}

		if (weight != -1)
		{
			gentity_t *gtiTriggers[4];
			int gtiTrigcount;
			
			gtiTrigcount = 0;
			BotDebug(bs, &bot_debugGameType, 1, S_COLOR_YELLOW"Bot_GetBestIdleGoal():- have gametype item %i, capture point = %s\n", weight, bot_GTInfo[weight].target);

			ent = &g_entities[0];
			for (i = 0; i < level.num_entities; i++, ent++) 
			{
				if (ent->s.eType != ET_GRAPPLE)
				{
					continue;
				}	

				if (ent->targetname)
				{
					BotDebug(bs, &bot_debugGameType, 1, S_COLOR_YELLOW"Bot_GetBestIdleGoal():- found GT trigger with targetname %s\n", ent->targetname);
				}
				
				if (!bot_entInfo[i].valid)
				{
					BotDebug(bs, &bot_debugGameType, 1, S_COLOR_YELLOW"Bot_GetBestIdleGoal():- !bot_entInfo[i].valid\n");
					continue;
				}

				if (!bot_entInfo[i].wpClosest)
				{
					BotDebug(bs, &bot_debugGameType, 1, S_COLOR_YELLOW"Bot_GetBestIdleGoal():- !bot_entInfo[i].wpClosest\n");
					continue;
				}
				
				// if we've already got a touch goal and this is it,
				// we don't want to suddenly head off to another one
				if (bs->touchGoal && (bs->touchGoal == ent))
				{
					gtiTriggers[0] = ent;
					gtiTrigcount = 1;
					break;
				}
								
				//if (strstr(ent->targetname, bot_GTInfo[weight].target))
				if (!Q_stricmp (ent->targetname, bot_GTInfo[weight].target)
				 || !Q_stricmp (ent->targetname, bot_GTInfo[weight].target2)
				  || !Q_stricmp (ent->targetname, bot_GTInfo[weight].target3))
				{
					gtiTriggers[gtiTrigcount] = ent;
					gtiTrigcount++;
					if (gtiTrigcount >= 5)
					{
						break;
					}
				}
			}
			
			// pick a random point
			if(gtiTrigcount){
				ent = gtiTriggers[rand() % gtiTrigcount];
				desired = bot_entInfo[ent->s.number].wpClosest;
				bs->touchGoal = ent;
				BotDebug(bs, &bot_debugGameType, 1, S_COLOR_YELLOW"Bot_GetBestIdleGoal():- now heading for gametype trigger %s\n", ent->targetname);
			}
		}
	}
	else if (bs->randomNav)
	{ 	// stop looking for items and/or camping on them
		BotDebug(bs, &bot_debugNav, 1, S_COLOR_YELLOW"Bot_GetBestIdleGoal():- stop looking for items and/or camping on them\n");
	}
	else if (bs->wpCurrent)
	{
		ent = &g_entities[0];
		for (i = 0; i < level.num_entities; i++, ent++) 
		{
			if (!bot_entInfo[i].valid || !bot_entInfo[i].wpClosest)
			{
				continue;
			}

			if (!BotDoesWant(bs, ent))
			{
				continue;
			}

			weight = BotGetItemWeight(bs, ent);

			// switch to CHEAP calc'n of weapon weights / distances
			// the TotatlTrailDistance() function is too expensive!!
			VectorSubtract(bs->wpCurrent->origin, bot_entInfo[i].wpClosest->origin, vec);
			dist = VectorLength(vec);
			best_dist = 1 - (dist / 8192);
			weight = (int)((float)weight * best_dist);

			if (weight > highestweight)
			{
				highestweight = weight;
				desired = bot_entInfo[i].wpClosest;
				bs->touchGoal = ent;
				BotDebug(bs, &bot_debugGameType, 1, S_COLOR_YELLOW"Bot_GetBestIdleGoal():-  weight %i > highestweight %i for %s\n", weight, highestweight, ent->classname);
			}
		}
		
		if (desired && !WP_CalcPath(bs->wpCurrent, desired, qfalse, NULL, NULL))
		{
			if (bs->touchGoal)
			{
				BotDebug(bs, &bot_debugGameType, 1, S_COLOR_YELLOW"Bot_GetBestIdleGoal():- couldn't figure out path to %s\n", bs->touchGoal->classname);
			}
			desired = NULL;
		}
	}
	
	if (desired == NULL)
	{
		point = NULL;		
		best = NULL;
		if (bs->wpCurrent)
		{
			point = bs->wpCurrent;
		}
		else if (bs->wpLastPoint)
		{
			point = bs->wpLastPoint;
		}
		
		BotDebug(bs, &bot_debugNav, 2, S_COLOR_YELLOW"Bot_GetBestIdleGoal():- desired == NULL - find somewhere to go!!\n");

		next = wpHead;
		best_dist = 0;

		for(i = 0; i < 4; i++)
		{
			while(next)
			{
				VectorSubtract(next->origin, bs->origin, vec);
				dist = VectorLength(vec);

				// avoid picking random points we don't like
				dist += WP_HistoryCost(bs->wpHistory, next);
			
				if ((dist > best_dist) && (dist < 1024.0f + random() * 2048))
				{
					if (point && (i != 0))
					{
						if (WP_CalcPath(point, next, qfalse, NULL, NULL))
						{
							best_dist = dist;
							best = next;
						}
					}
					else
					{				
						best_dist = dist;
						best = next;
					}
				}
			
				next = next->next;
			}

			if (best && bs->wpCurrent && WP_CalcPath(bs->wpCurrent, best, qfalse, NULL, NULL))
			{
				BotDebug(bs, &bot_debugNav, 1, S_COLOR_YELLOW"Bot_GetBestIdleGoal():- found random point %i on try %i\n", best->index, i);
				break;
			}
		}		

		if (best)
		{
			desired = best;
		}
	}

	// mother fucker - still no where to go?
	// fuck that, just pick a neighbour and start running..
	// son of a bitch!!
	if (desired == NULL)
	{
		point = bs->wpCurrent;

		// we've fucked up - we should have found an idle point by now
		Bot_FuckedUP(bs);
		if (bs->fuckUp > 1024)
		{
			Com_Printf(S_COLOR_YELLOW"Bot_GetBestIdleGoal():- "S_COLOR_RED"ERROR"S_COLOR_YELLOW" bot[%i] -> , desired == NULLwe should have found an idle point by now!\n", bs->client);
		}
		
		if (point)
		{
			wayPoint_t *neighbours[MAX_NEIGHBOURS];
			int count;
			count = 0;

			for(i = 0; i < MAX_NEIGHBOURS; i++)
			{
				if (point->neighBours[i] != NULL)
				{
					neighbours[count++] = point->neighBours[i];
				}
			}

			if (count)
			{
				desired = neighbours[rand() % count];
				Com_Printf(S_COLOR_YELLOW"Bot_GetBestIdleGoal():- desired == wander round hack\n");
			}
		}
	}

	return desired;
}


/*
==============
Bot_AvoidBadThings
==============
*/
qboolean Bot_AvoidBadThings(bot_state_t *bs)
{
	float dist, best;
	gentity_t *badthing;
	wayPoint_t *runto[4];
	vec3_t vec;
	int i, j;

	if (!bs->wpCurrent)
	{
		return qfalse;
	}

	if ((level.time - bs->escapeDirTime) > 4000)
	{
		badthing = Bot_GetNearestBadThing(bs);
	}
	else
	{
		badthing = NULL;
	}

	if (badthing && badthing->inuse && (badthing->health > 0) && badthing->takedamage)
	{
		bs->dangerousObject = badthing;
	}
	else
	{
		bs->dangerousObject = NULL;
	}

	if (!badthing && (bs->wpDestIgnoreTime > level.time))
	{
		return qtrue;
	}

	if (!badthing && (bs->dontGoBack > level.time))
	{
		if (bs->wpDestination)
		{
			bs->wpStoreDest = bs->wpDestination;
		}
		
		bs->wpDestination = NULL;
		return qtrue;
	}
	else if (!badthing && bs->wpStoreDest)
	{ 	// after we finish running away, switch back to our original destination
		bs->wpDestination = bs->wpStoreDest;
		bs->wpStoreDest = NULL;
	}

	if (badthing && bs->wpCamping)
	{
		bs->wpCamping = NULL;
	}

	if (badthing == NULL)
	{
		return qfalse;
	}
	
	// ahh, run away!
	bs->runningLikeASissy = level.time + 100;

	if (bs->wpDestination)
	{
		bs->wpStoreDest = bs->wpDestination;
	}
	
	bs->wpDestination = NULL;
		
	if (bs->escapeDirTime > level.time)
	{
		return qtrue;
	}

	VectorSubtract(badthing->s.pos.trBase, bs->wpCurrent->origin, vec);
	best = VectorLength(vec);
	for(j = 0; j < 4; j++)
	{
		runto[j] = NULL;
	}
	runto[0] = bs->wpCurrent;

	for(i = 0; i < MAX_NEIGHBOURS; i++)
	{
		if (bs->wpCurrent->neighBours[i] == NULL)
		{
			continue;
		}

		VectorSubtract(badthing->s.pos.trBase, bs->wpCurrent->neighBours[i]->origin, vec);
		dist = VectorLength(vec);

		if (dist > best)
		{
			best = dist;
			for(j = 3; j > 0; j--)
			{
				runto[j] = runto[j - 1];
			}				
			runto[0] = bs->wpCurrent->neighBours[i];
		}
	}
		
	j = 0;
	for(i = 0; i < 4; i++)
	{
		if (runto[i])
		{
			j++;
		}
	}
	
	if (j)
	{
		bs->wpCurrent = runto[rand() % j];
		bs->escapeDirTime = level.time + Q_irand(500, 1000);
		BotDebug(bs, &bot_debugNav, 2, "Escaping from scary bad thing [%s]\n", badthing->classname);
		return qtrue;
	}

	return qfalse;
}


/*
==============
Bot_GetIdealDestination
==============
*/
void Bot_GetIdealDestination(bot_state_t *bs)
{
	float dist, best;
	vec3_t usethisvec, vec;
	wayPoint_t *tempWP;
	wayPoint_t *idleWP;
	wayPoint_t *runto[4];
	int i, j;

	if (!bs->wpCurrent)
	{
		return;
	}

	if (bs->wpCamping)
	{
		bs->wpDestination = bs->wpCamping;
		return;
	}

	if (bs->currentEnemy)
	{
		switch(BotGetWeaponRange(bs))
		{
		case BWEAPONRANGE_MELEE:
			dist = 1;
			break;

		case BWEAPONRANGE_SHORT:
			dist = 256;
			break;

		default:
		case BWEAPONRANGE_MID:
			dist = 512;
			break;

		case BWEAPONRANGE_LONG:
			dist = 1024;
			break;

		case BWEAPONRANGE_VERY_LONG:
			dist = 2048;
			break;
		}
		
		if (bs->currentEnemy->client)
		{
			VectorCopy(bs->currentEnemy->client->ps.origin, usethisvec);
		}
		else
		{
			VectorCopy(bs->currentEnemy->s.origin, usethisvec);
		}

		bs->runningToEscapeThreat = BotIsAChickenWuss(bs);

		if ((bs->frame_Enemy_Len < dist) || (bs->runningToEscapeThreat && (bs->runningToEscapeThreat != 2)))
		{
			if (bs->frame_Enemy_Len > 400)
			{ 	
				// good distance away, start running toward a 
				// good place for an item or powerup or whatever
				idleWP = Bot_GetBestIdleGoal(bs);

				if (idleWP && WP_CalcPath(bs->wpCurrent, idleWP, qfalse, bs->wpPath, bs->wpHistory))
				{
					bs->wpDestination = idleWP;
				}
			}
			else
			{
				tempWP = NULL;

				VectorSubtract(usethisvec, bs->wpCurrent->origin, vec);
				best = VectorLength(vec);
				for(j = 0; j < 4; j++)
				{
					runto[j] = NULL;
				}
				runto[0] = bs->wpCurrent;

				for(i = 0; i < MAX_NEIGHBOURS; i++)
				{
					if (bs->wpCurrent->neighBours[i] == NULL)
					{
						continue;
					}

					VectorSubtract(usethisvec, bs->wpCurrent->neighBours[i]->origin, vec);
					dist = VectorLength(vec);

					if (dist > best)
					{
						best = dist;
						for(j = 3; j > 0; j--)
						{
							runto[j] = runto[j - 1];
						}
				
						runto[0] = bs->wpCurrent->neighBours[i];
					}
				}
		
				j = 0;
				for(i = 0; i < 4; i++)
				{
					if (runto[i])
					{
						j++;
					}
				}
		
				if (j)
				{
					bs->wpCurrent = runto[rand() % j];
				}
			}
		}
		else if ((bs->runningToEscapeThreat != 2) && (bs->wpDestSwitchTime < level.time))
		{
			tempWP = GetNearestVisibleWP(usethisvec, bs->client);

			// update path
			if (tempWP && WP_CalcPath(bs->wpCurrent, tempWP, qfalse, bs->wpPath, bs->wpHistory))
			{
				bs->wpDestination = tempWP;
				bs->wpDestSwitchTime = level.time + Q_irand(1000, 5000);
			}
		}
	}

	if (!bs->wpDestination && (bs->wpDestSwitchTime < level.time))
	{
		BotDebug(bs, &bot_debugNav, 1, S_COLOR_YELLOW"Bot_GetIdealDestination():- I need something to do\n");

		idleWP = Bot_GetBestIdleGoal(bs);

		if (idleWP && WP_CalcPath(bs->wpCurrent, idleWP, qfalse, bs->wpPath, bs->wpHistory))
		{
			bs->wpDestination = idleWP;
		}
	}
}


/*
==============
MeleeCombatHandling
==============
*/
void MeleeCombatHandling(bot_state_t *bs)
{
	vec3_t usethisvec;
	vec3_t downvec;
	vec3_t midorg;
	vec3_t a;
	vec3_t fwd;
	vec3_t mins, maxs;
	trace_t tr;
	int en_down;
	int me_down;
	int mid_down;

	if (!bs->currentEnemy)
	{
		return;
	}

	if (bs->currentEnemy->client)
	{
		VectorCopy(bs->currentEnemy->client->ps.origin, usethisvec);
	}
	else
	{
		VectorCopy(bs->currentEnemy->s.origin, usethisvec);
	}

	if (bs->meleeStrafeTime < level.time)
	{
		if (bs->meleeStrafeDir)
		{
			bs->meleeStrafeDir = 0;
		}
		else
		{
			bs->meleeStrafeDir = 1;
		}

		bs->meleeStrafeTime = level.time + Q_irand(500, 1800);
	}

	mins[0] = -15;
	mins[1] = -15;
	mins[2] = -24;
	maxs[0] = 15;
	maxs[1] = 15;
	maxs[2] = 32;

	VectorCopy(usethisvec, downvec);
	downvec[2] -= 4096;

	trap_Trace(&tr, usethisvec, mins, maxs, downvec, -1, MASK_SOLID);

	en_down = (int)tr.endpos[2];

	VectorCopy(bs->origin, downvec);
	downvec[2] -= 4096;

	trap_Trace(&tr, bs->origin, mins, maxs, downvec, -1, MASK_SOLID);

	me_down = (int)tr.endpos[2];

	VectorSubtract(usethisvec, bs->origin, a);
	vectoangles(a, a);
	AngleVectors(a, fwd, NULL, NULL);

	midorg[0] = bs->origin[0] + fwd[0]*bs->frame_Enemy_Len/2;
	midorg[1] = bs->origin[1] + fwd[1]*bs->frame_Enemy_Len/2;
	midorg[2] = bs->origin[2] + fwd[2]*bs->frame_Enemy_Len/2;

	VectorCopy(midorg, downvec);
	downvec[2] -= 4096;

	trap_Trace(&tr, midorg, mins, maxs, downvec, -1, MASK_SOLID);

	mid_down = (int)tr.endpos[2];

	if (me_down == en_down && en_down == mid_down)
	{
		VectorCopy(usethisvec, bs->goalPosition);
	}
}


/*
==============
BotWeaponCanLead
==============
*/
float BotWeaponCanLead(bot_state_t *bs)
{
	switch (bs->cur_ps.weapon)
	{
	default:
		return 0;
	case WP_KNIFE:
		return 0.5;

	// no leading needed for any bullet weapons
	case WP_M1911A1_PISTOL:          
	case WP_USSOCOM_PISTOL:         
	case WP_M4_ASSAULT_RIFLE:        
	case WP_AK74_ASSAULT_RIFLE:
	case WP_M60_MACHINEGUN:          
	case WP_MICRO_UZI_SUBMACHINEGUN: 
	case WP_MSG90A1:    
	case WP_USAS_12_SHOTGUN:         
	case WP_M590_SHOTGUN:            
		return 0;
	// projectile weapons lead
	case WP_MM1_GRENADE_LAUNCHER:    
		return 0.5;
	case WP_RPG7_LAUNCHER:           
		return 0.5;
	case WP_M84_GRENADE:             
	case WP_SMOHG92_GRENADE:         
	case WP_ANM14_GRENADE:           
	case WP_M15_GRENADE:             
		return 0.7;
	}
}


/*
==============
BotWeaponCanLead

Calculate proper angle for ballistic weapon
==============
*/
static float CalcWeaponAngle(float vel, float gravity, float targetRange)
{
	float angle = 0;
	float val =  (gravity * targetRange) / (vel * vel);

	if (val >= -1 && val <= 1)
	{
		angle = RAD2DEG(asin(val)/2);
	}
	else
	{
		angle = 90;
	}
	
	return angle;
}


/*
==============
BotAimLeading
==============
*/
void BotAimLeading(bot_state_t *bs, vec3_t headlevel, float leadAmount)
{
	int x;
	vec3_t predictedSpot;
	vec3_t movementVector;
	vec3_t a, ang;
	float vtotal;

	if (!bs->currentEnemy ||
		!bs->currentEnemy->client)
	{
		return;
	}

	if (!bs->frame_Enemy_Len)
	{
		return;
	}

	vtotal = 0;

	if (bs->currentEnemy->client->ps.velocity[0] < 0)
	{
		vtotal += -bs->currentEnemy->client->ps.velocity[0];
	}
	else
	{
		vtotal += bs->currentEnemy->client->ps.velocity[0];
	}

	if (bs->currentEnemy->client->ps.velocity[1] < 0)
	{
		vtotal += -bs->currentEnemy->client->ps.velocity[1];
	}
	else
	{
		vtotal += bs->currentEnemy->client->ps.velocity[1];
	}

	if (bs->currentEnemy->client->ps.velocity[2] < 0)
	{
		vtotal += -bs->currentEnemy->client->ps.velocity[2];
	}
	else
	{
		vtotal += bs->currentEnemy->client->ps.velocity[2];
	}

	//G_Printf("Leadin target with a velocity total of %f\n", vtotal);

	VectorCopy(bs->currentEnemy->client->ps.velocity, movementVector);

	VectorNormalize(movementVector);

	x = bs->frame_Enemy_Len*leadAmount; //hardly calculated with an exact science, but it works

	if (vtotal > 400)
	{
		vtotal = 400;
	}

	if (vtotal)
	{
		x = (bs->frame_Enemy_Len*0.9)*leadAmount*(vtotal*0.0012); //hardly calculated with an exact science, but it works
	}
	else
	{
		x = (bs->frame_Enemy_Len*0.9)*leadAmount; //hardly calculated with an exact science, but it works
	}

	predictedSpot[0] = headlevel[0] + (movementVector[0]*x);
	predictedSpot[1] = headlevel[1] + (movementVector[1]*x);
	predictedSpot[2] = headlevel[2] + (movementVector[2]*x);

	VectorSubtract(predictedSpot, bs->eye, a);
	vectoangles(a, ang);
	VectorCopy(ang, bs->goalAngles);
}


/*
==============
BotAimOffsetGoalAngles
==============
*/
void BotAimOffsetGoalAngles(bot_state_t *bs)
{
	int i;
	float accVal;
	float blind;
	
	i = 0;

	blind = Bot_IsBlind(bs);

	if (bs->skills.perfectaim && (blind < 0.5))
	{
		return;
	}

	if (bs->aimOffsetTime > level.time)
	{
		if (bs->aimOffsetAmtYaw)
		{
			bs->goalAngles[YAW] += bs->aimOffsetAmtYaw;
		}

		if (bs->aimOffsetAmtPitch)
		{
			bs->goalAngles[PITCH] += bs->aimOffsetAmtPitch;
		}
		
		while (i <= 2)
		{
			if (bs->goalAngles[i] > 360)
			{
				bs->goalAngles[i] -= 360;
			}

			if (bs->goalAngles[i] < 0)
			{
				bs->goalAngles[i] += 360;
			}

			i++;
		}
		return;
	}

	accVal = bs->skills.accuracy/bs->settings.skill;
	
	// less accurate when blinded
	accVal *= 1 + blind;

	if (bs->currentEnemy && bs->frame_Enemy_Vis)
	{ //assume our goal is aiming at the enemy, seeing as he's visible and all
		if (!bs->currentEnemy->s.pos.trDelta[0] &&
			!bs->currentEnemy->s.pos.trDelta[1] &&
			!bs->currentEnemy->s.pos.trDelta[2])
		{
			accVal = 0; //he's not even moving, so he shouldn't really be hard to hit.
		}
		else
		{
			accVal += accVal*0.25; //if he's moving he's this much harder to hit
		}

		if (g_entities[bs->client].s.pos.trDelta[0] ||
			g_entities[bs->client].s.pos.trDelta[1] ||
			g_entities[bs->client].s.pos.trDelta[2])
		{
			accVal += accVal*0.15; //make it somewhat harder to aim if we're moving also
		}

		// see if we're aiming through smoke
		if (Bot_VisionObscured(bs, bs->eye, bs->currentEnemy->s.pos.trBase))
		{	// kinda hard to hit things through smoke
			accVal += accVal * 0.15;
		}
	}

	if (accVal > 90)
	{
		accVal = 90;
	}
	if (accVal < 1)
	{
		accVal = 0;
	}

	if (!accVal)
	{
		bs->aimOffsetAmtYaw = 0;
		bs->aimOffsetAmtPitch = 0;
		return;
	}

	if (rand()%10 <= 5)
	{
		bs->aimOffsetAmtYaw = rand()%(int)accVal;
	}
	else
	{
		bs->aimOffsetAmtYaw = -(rand()%(int)accVal);
	}

	if (rand()%10 <= 5)
	{
		bs->aimOffsetAmtPitch = rand()%(int)accVal;
	}
	else
	{
		bs->aimOffsetAmtPitch = -(rand()%(int)accVal);
	}

	bs->aimOffsetTime = level.time + rand()%500 + 200;
}


/*
==============
Bot_ShouldSecondaryFire
==============
*/
int Bot_ShouldSecondaryFire(bot_state_t *bs, vec3_t eorg, vec3_t dir)
{
	weaponData_t *weapon = &weaponData[bs->cur_ps.weapon];
	attackData_t *attack = &weapon->attack[ATTACK_ALTERNATE];
	float range2D;

	// no alt fire
	if (attack->damage == 0)
	{
		return 0;
	}
	
	if (attack->ammoIndex != AMMO_NONE)
	{	// no ammo for alt fire
		if ((bs->cur_ps.ammo[attack->ammoIndex] < 1)
	  	 && (bs->cur_ps.clip[ATTACK_ALTERNATE][bs->cur_ps.weapon] < 1))
		{
			return 0;
		}
	}

 	// don't forget to make sure the bot doesn't shoot it off in his own face!
	if ((attack->weaponFlags & PROJECTILE_FIRE) && (bs->frame_Enemy_Len > MAX_PROJECTILE_DISTANCE))
	{
		// only 2D range since Z is for the ballistic path only
		range2D = max(0, SQRTFAST(dir[0]*dir[0] + dir[1]*dir[1]) - attack->splashRadius); 

		if (attack->weaponFlags & PROJECTILE_TIMED)
		{	
			// if timed projectile, let bounce for 1 sec.
			range2D = max(0, range2D - attack->rV.velocity);
		}
			
		bs->launchAngle = CalcWeaponAngle(attack->rV.velocity, DEFAULT_GRAVITY, range2D);
		if (bs->launchAngle < 90)
		{
			if (!OrgVisible(bs->eye, eorg, bs->client))
			{
				bs->launchAngle = 90 - bs->launchAngle;
			}

			return 1;
		}
		else
		{
			bs->launchAngle = 0;
		}
	}
	else if (bs->frame_Enemy_Len < attack->rV.range)
	{	// alt-fire is bullet
		return 1;
	}
	
	return 0;
}


/*
==============
CombatBotAI
==============
*/
int CombatBotAI(bot_state_t *bs, float thinktime)
{
	vec3_t eorg, a, dir;
	int secFire;
	float fovcheck;

	if (weaponData[bs->cur_ps.weapon].category == CAT_GRENADE)
	{
		if (bs->cur_ps.grenadeTimer)// < 100)
		{	// just throw the fucking thing away
			// FIX ME - dangerous
			bs->doAttack = 0;
			bs->doAltAttack = 0;
			return 1;
		}
	}

	if (!bs->currentEnemy)
	{
		return 0;
	}

	if (bs->currentEnemy->client)
	{
		VectorCopy(bs->currentEnemy->client->ps.origin, eorg);
	}
	else
	{
		VectorCopy(bs->currentEnemy->s.origin, eorg);
	}

	VectorSubtract(eorg, bs->eye, dir);
	vectoangles(dir, a);

	if (BotGetWeaponRange(bs) == BWEAPONRANGE_MELEE )
	{
		if (bs->frame_Enemy_Len <= MELEE_ATTACK_RANGE)
		{
			bs->doAttack = 1;
			return 1;
		}
		// GodOrDevil - 10.02.06 - 04:27pm #bot use alt attack with knifes
		else if (bs->cur_ps.weapon == WP_KNIFE && (InFieldOfVision(bs->viewangles, 10, a)) 
			&& bs->frame_Enemy_Len > weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].rV.range)
		{
			bs->doAltAttack = 1;
			return 1;
		}
		///End  - 10.02.06 - 04:28pm
		return 0;
	}
	else if (bs->frame_Enemy_Len > weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].rV.range + 256)
	{
		return 0;
	}
	else
	{
		if (bs->cur_ps.weapon == WP_RPG7_LAUNCHER || bs->cur_ps.weapon == WP_MM1_GRENADE_LAUNCHER)
		{ 	// be careful with the hurty weapons
			fovcheck = 10;
		}
		else
		{
			fovcheck = 60;
		}

		if (bs->frame_Enemy_Len < 128)
		{
			fovcheck *= 2;
		}

		if (InFieldOfVision(bs->viewangles, fovcheck, a))
		{
			weaponData_t *weapon = &weaponData[bs->cur_ps.weapon];

			if ((weapon->category == CAT_GRENADE) || (bs->cur_ps.weapon == WP_MM1_GRENADE_LAUNCHER))
			{
				attackData_t *attack;
				float range2D;

				// are we using a grenade type weapon?
				if ((weapon->category != CAT_GRENADE) || (bs->frame_Enemy_Len < weapon->attack[ATTACK_NORMAL].rV.velocity * weapon->attack[ATTACK_NORMAL].projectileLifetime * 0.001))
				{
					attack = &weapon->attack[ATTACK_NORMAL];
				}
				else
				{
					attack = &weapon->attack[ATTACK_ALTERNATE];
				}

				// only 2D range since Z is for the ballistic path only
				range2D = max(0, SQRTFAST(dir[0]*dir[0] + dir[1]*dir[1]) - attack->splashRadius); 
				if (attack->weaponFlags & PROJECTILE_TIMED)
				{	
					// if timed projectile, let bounce for 1 sec.
					range2D = max(0, range2D - attack->rV.velocity);
				}
				
				bs->launchAngle = CalcWeaponAngle(attack->rV.velocity, DEFAULT_GRAVITY, range2D);
				if (bs->launchAngle < 90)
				{
					if (!OrgVisible(bs->eye, eorg, bs->client))
					{
						bs->launchAngle = 90 - bs->launchAngle;
					}

					bs->doAttack = 1;
				}
				else
				{
					bs->launchAngle = 0;
				}
			}
			else
			{
				bs->launchAngle = 0;
				secFire = Bot_ShouldSecondaryFire(bs, eorg, dir);

				if (bs->cur_ps.weaponstate != WEAPON_CHARGING_ALT)
				{
					bs->altChargeTime = Q_irand(500, 1000);
				}

				if (secFire)
				{
					bs->doAltAttack = 1;
					return 1;
				}
				else
				{	// if our accuracy is way out, stop firing for a bit
					//if (bs->cur_ps.inaccuracy > (weapon->attack[ATTACK_NORMAL].maxInaccuracy / 3))
					if (bs->cur_ps.kickPitch > 8000)
					{
						bs->doAttack = 0;
					}
					else
					{
						bs->doAttack = 1;
						return 1;
					}
				}
			}
		}
	}

	return 0;
}


/*
==============
BotFallbackNavigation
==============
*/
int BotFallbackNavigation(bot_state_t *bs)
{
	vec3_t b_angle, fwd, trto, mins, maxs;
	trace_t tr;

	if (bs->currentEnemy && bs->frame_Enemy_Vis)
	{
		return 2; //we're busy
	}

	mins[0] = -15;
	mins[1] = -15;
	mins[2] = 0;
	maxs[0] = 15;
	maxs[1] = 15;
	maxs[2] = 32;

	bs->goalAngles[PITCH] = 0;
	bs->goalAngles[ROLL] = 0;

	VectorCopy(bs->goalAngles, b_angle);

	AngleVectors(b_angle, fwd, NULL, NULL);

	trto[0] = bs->origin[0] + fwd[0]*16;
	trto[1] = bs->origin[1] + fwd[1]*16;
	trto[2] = bs->origin[2] + fwd[2]*16;

	trap_Trace(&tr, bs->origin, mins, maxs, trto, -1, MASK_PLAYERSOLID);

	if (tr.fraction == 1)
	{
		VectorCopy(trto, bs->goalPosition);
		return 1; //success!
	}
	else
	{
		bs->goalAngles[YAW] = rand()%360;
	}

	return 0;
}


/*
==============
BotTryAnotherWeapon

Out of ammo, resort to the first weapon we come across that has ammo
==============
*/
int BotTryAnotherWeapon(bot_state_t *bs)
{
	int i;

	for(i = WP_NUM_WEAPONS - 1; i > WP_KNIFE; i--)
	{
		if (!(bs->cur_ps.stats[STAT_WEAPONS] & (1 << i)))
		{
			continue;
		}
		
		if (bs->cur_ps.ammo[weaponData[i].attack[ATTACK_NORMAL].ammoIndex] > 0)
		{
			bs->virtualWeapon = i;
			trap_EA_SelectWeapon(bs->client, i);
			return 1;
		}

		if (bs->cur_ps.clip[ATTACK_NORMAL][i] > 0)
		{
			bs->virtualWeapon = i;
			trap_EA_SelectWeapon(bs->client, i);
			return 1;
		}
	}

	if (bs->cur_ps.weapon != WP_KNIFE && bs->virtualWeapon != WP_KNIFE && (bs->cur_ps.stats[STAT_WEAPONS] & (1 << WP_KNIFE)))
	{ 	// should always have this.. shouldn't we?
		bs->virtualWeapon = 1;
		trap_EA_SelectWeapon(bs->client, 1);
		return 1;
	}

	return 0;
}


/*
==============
BotSelectIdealWeapon
==============
*/
int BotSelectIdealWeapon(bot_state_t *bs)
{
	int i;
	int bestweight = -1;
	int bestweapon = 0;

	for(i = WP_KNIFE; i < WP_NUM_WEAPONS; i++)
	{
		if ((bs->cur_ps.ammo[weaponData[i].attack[ATTACK_NORMAL].ammoIndex] > 0
		 || bs->cur_ps.clip[ATTACK_NORMAL][i] > 0)
		  &&  (bs->botWeaponWeights[i] > bestweight)
		   && (bs->cur_ps.stats[STAT_WEAPONS] & (1 << i)))
		{
			bestweight = bs->botWeaponWeights[i];
			bestweapon = i;
		}
	}

	if ((bestweapon != WP_NONE) && (bestweight != -1) && (bs->cur_ps.weapon != bestweapon) && (bs->virtualWeapon != bestweapon))
	{
		bs->virtualWeapon = bestweapon;
		trap_EA_SelectWeapon(bs->client, bestweapon);
		
		return 1;
	}

	return 0;
}


/*
==============
BotTrace_ObstacleCheck
==============
*/
qboolean BotTrace_ObstacleCheck(bot_state_t *bs)
{
	vec3_t fwd, right, start, end;
	vec3_t angles, mins, maxs;
	trace_t tr, clearTr;
	float minWalkNormal;
	int i;
	qboolean tryJumpingOverClient;

	// don't do anything if not on the ground
	if (bs->cur_ps.groundEntityNum == ENTITYNUM_NONE)
	{
		return qfalse;
	}
	
	// don't do anything if already jumping / ducking
	if ((bs->jumpTime >= level.time) || (bs->duckTime >= level.time))
	{
		return qfalse;
	}

	if (bs->cur_ps.pm_flags & PMF_DUCKED)
	{
		return qfalse;
	}

	VectorCopy(bs->goalMovedir, fwd);

	VectorCopy(bs->origin, start);
	VectorMA(start, BOT_MAX_CHECKOB * 2, fwd, end);

	trap_Trace(&clearTr, start, NULL, NULL, end, bs->client, MASK_PLAYERSOLID);

	// if we're blocked by some glass...
	if ((clearTr.entityNum >= MAX_CLIENTS) && !Q_stricmp(g_entities[clearTr.entityNum].classname, "func_glass"))
	{
		bs->shootGoal = &g_entities[clearTr.entityNum];
		return qfalse; // ...blow it away!!
	}

	fwd[2] = 0;

	mins[0] = -15;
	mins[1] = -15;
	mins[2] = MINS_Z + 18;
	maxs[0] = 15;
	maxs[1] = 15;
	maxs[2] = DEFAULT_PLAYER_Z_MAX;

	VectorMA(start, BOT_MAX_CHECKOB, fwd, end);
	trap_Trace(&clearTr, start, mins, maxs, end, bs->client, MASK_PLAYERSOLID);

	if ((clearTr.fraction >= 1.0f) && !clearTr.allsolid && !clearTr.startsolid)
	{
		return qfalse;
	}

	// Not clear, try strafing around it first
	vectoangles(fwd, angles);
	
	// make sure we're facing the direction we're heading, else it'd be stupid to strafe
	if (!(bs->cur_ps.pm_flags & PMF_LADDER) && (AngleDifference(bs->viewangles[YAW], angles[YAW]) <= 40 && AngleDifference(bs->viewangles[YAW], angles[YAW]) >= -40))
	{
		AngleVectors(angles, NULL, right, NULL);
		for(i = -1; i < 2; i++)
		{
			if (i == 0)
			{
				continue;
			}

			VectorCopy(bs->origin, start);
			VectorMA(bs->origin, (BOT_MAX_CHECKOB / 2) * i, right, start);
			VectorMA(start, BOT_MAX_CHECKOB / 2, fwd, end);

			trap_Trace(&tr, start, mins, maxs, end, bs->client, MASK_PLAYERSOLID);

			if ((tr.fraction >= 1.0f) && !tr.allsolid && !tr.startsolid)
			{
				// make sure we're not strafing over a ledge or something
				VectorCopy(start, end);
				end[2] -= 32;
				trap_Trace(&tr, start, mins, maxs, end, bs->client, MASK_PLAYERSOLID);
				if (tr.fraction < 1.0f)
				{
					break;
				}
			}

			BotDebug(bs, &bot_debugNav, 4, S_COLOR_YELLOW"BotTrace_ObstacleCheck():- bot[%i] -> can't strafe %s\n", bs->client, (i == -1) ? "LEFT" : "RIGHT");
		}

		if (i == -1)
		{	// strafe left
			trap_EA_MoveLeft(bs->client);
			BotDebug(bs, &bot_debugNav, 4, S_COLOR_YELLOW"BotTrace_ObstacleCheck():- bot[%i] -> strafe LEFT\n", bs->client);
			bs->strafeTime = level.time + 150;
			bs->strafeDir = -1;
			return qfalse;
		}
		else if (i == 1)
		{	// strafe right
			BotDebug(bs, &bot_debugNav, 4, S_COLOR_YELLOW"BotTrace_ObstacleCheck():- bot[%i] -> strafe RIGHT\n", bs->client);
			bs->strafeTime = level.time + 150;
			bs->strafeDir = 1;
			return qfalse;
		}
	}

	// if we traced into a client, only try jumping over 
	// them if they are currently ducked
	tryJumpingOverClient = qfalse;
	if (clearTr.entityNum >= 0 && clearTr.entityNum < MAX_CLIENTS)
	{
		if (g_entities[clearTr.entityNum].client)
		{
			if (g_entities[clearTr.entityNum].client->ps.pm_flags & PMF_LADDER)
			{
				return qtrue; // they're on a ladder... wait
			}				
			else if (g_entities[clearTr.entityNum].client->ps.pm_flags & PMF_DUCKED)
			{
				tryJumpingOverClient = qtrue;
			}
		}
		
		if (!tryJumpingOverClient)
		{
			goto skipjumpandduck;
		}
	}

	// don't do anything if on a ladder
	if (bs->cur_ps.pm_flags & PMF_LADDER)
	{
		return qtrue;	// we're just blocked, ok?
	}

	// ok, we might need to jump... check the angle
	// slopes that are too steep will not be considered onground
	if (clearTr.contents & CONTENTS_TERRAIN)
	{
		minWalkNormal = 0.625f;//MIN_WALK_NORMAL_TERRAIN;
	}
	else
	{
		minWalkNormal = 0.7f;//MIN_WALK_NORMAL;
	}

	if (tryJumpingOverClient || (clearTr.plane.normal[2] < minWalkNormal))
	{	// we may need to jump
		BotDebug(bs, &bot_debugNav, 4, S_COLOR_YELLOW"BotTrace_ObstacleCheck():- bot[%i] -> obstacle ahead too steep - we need to jump\n", bs->client);

		mins[2] = MINS_Z;
		maxs[2] = CROUCH_PLAYER_Z_MAX;

		for(i = 0; i <= 4; i++)
		{
			VectorCopy(bs->origin, start);
			VectorMA(start, BOT_MAX_CHECKOB, fwd, end);
			start[2] += 27 + (11 * i);
			end[2] += 27 + (11 * i);
			trap_Trace(&tr, start, mins, maxs, end, bs->client, MASK_PLAYERSOLID);

			if ((tr.fraction >= 1.0f) && !tr.allsolid && !tr.startsolid)
			{
				bs->jumpTime = level.time + 150;
				return qfalse;
			}
		}
		BotDebug(bs, &bot_debugNav, 4, S_COLOR_YELLOW"BotTrace_ObstacleCheck():- bot[%i] -> something above prevents us jumping\n", bs->client);
		
		// can't avoid it - its not clear below, so why bother
		// seeing if we can duck under it?
		goto skipjumpandduck;
	}

	VectorCopy(bs->origin, start);
	VectorMA(start, BOT_MAX_CHECKOB, fwd, end);
	mins[2] = MINS_Z;
	maxs[2] = CROUCH_PLAYER_Z_MAX;
	trap_Trace(&tr, start, mins, maxs, end, bs->client, MASK_PLAYERSOLID);

	if ((tr.fraction < 1.0f) || tr.allsolid)
	{
		BotDebug(bs, &bot_debugNav, 4, S_COLOR_YELLOW"BotTrace_ObstacleCheck():- bot[%i] -> couldn't just duck - may be a step, so recheck slightly step units up\n", bs->client);
		start[2] += 18;
		end[2] += 18;
		trap_Trace(&tr, start, mins, maxs, end, bs->client, MASK_PLAYERSOLID);
	}

	// ok, so its clear enough for us to duck... so, err, duck	
	if ((tr.fraction >= 1.0f) && !tr.allsolid)
	{
		BotDebug(bs, &bot_debugNav, 4, S_COLOR_YELLOW"BotTrace_ObstacleCheck():- bot[%i] -> its clear enough for us to duck\n", bs->client);
		bs->duckTime = level.time + 500;
	}

skipjumpandduck:
	// make sure we're facing the direction we're heading, else it'd be stupid to strafe
	// this is the same check as above, but we'll only try to strafe a little bit
	if (AngleDifference(bs->viewangles[YAW], angles[YAW]) <= 40 && AngleDifference(bs->viewangles[YAW], angles[YAW]) >= -40)
	{
		AngleVectors(angles, NULL, right, NULL);
		for(i = -1; i < 2; i++)
		{
			if (i == 0)
			{
				continue;
			}

			VectorCopy(bs->origin, start);
			VectorMA(bs->origin, 4 * i, right, start);
			VectorMA(start, 4, fwd, end);

			trap_Trace(&tr, start, mins, maxs, end, bs->client, MASK_PLAYERSOLID);

			if ((tr.fraction >= 1.0f) && !tr.allsolid && !tr.startsolid)
			{
				// make sure we're not strafing over a ledge or something
				VectorCopy(start, end);
				end[2] -= 32;
				trap_Trace(&tr, start, mins, maxs, end, bs->client, MASK_PLAYERSOLID);
				if (tr.fraction < 1.0f)
				{
					break;
				}
				break;
			}

			BotDebug(bs, &bot_debugNav, 4, S_COLOR_YELLOW"BotTrace_ObstacleCheck():- bot[%i] -> can't strafe %s a little bit\n", bs->client, (i == -1) ? "LEFT" : "RIGHT");
		}
		
		if (i == -1)
		{	// strafe left
			trap_EA_MoveLeft(bs->client);
			BotDebug(bs, &bot_debugNav, 4, S_COLOR_YELLOW"BotTrace_ObstacleCheck():- bot[%i] -> strafe LEFT a little bit\n", bs->client);
			bs->strafeTime = level.time + 50;
			bs->strafeDir = -1;
			return qfalse;
		}
		else if (i == 1)
		{	// strafe right
			trap_EA_MoveRight(bs->client);
			BotDebug(bs, &bot_debugNav, 4, S_COLOR_YELLOW"BotTrace_ObstacleCheck():- bot[%i] -> strafe RIGHT a little bit\n", bs->client);
			bs->strafeTime = level.time + 50;
			bs->strafeDir = 1;
			return qfalse;
		}
	}

	// totally blocked!!
	return qtrue;
}


/*
==============
BotHasBetterLongRangeWeapon
==============
*/
int BotHasBetterLongRangeWeapon(bot_state_t *bs)
{
	int i, best, best_range;
	
	best = bs->cur_ps.weapon;
	best_range = weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].rV.range;
	
	for(i = WP_KNIFE + 1; i < WP_MM1_GRENADE_LAUNCHER; i++)
	{
		if (!(bs->cur_ps.stats[STAT_WEAPONS] & (1 << i)))
		{
			continue;
		}
		
		if (weaponData[i].attack[ATTACK_NORMAL].rV.range == 0)
		{
			continue;
		}
		
		if ((bs->cur_ps.ammo[weaponData[i].attack[ATTACK_NORMAL].ammoIndex] < 1) && (bs->cur_ps.clip[ATTACK_NORMAL][i] < 1))
		{
			continue;
		}

		if (weaponData[i].attack[ATTACK_NORMAL].rV.range >= best_range)
		{
			best = i;
			best_range = weaponData[i].attack[ATTACK_NORMAL].rV.range;
		}
	}
	
	if (best != bs->cur_ps.weapon)
	{
		return best;
	}
	
	return 0;
}


/*
==============
BotHasBetterShortRangeWeapon
==============
*/
int BotHasBetterShortRangeWeapon(bot_state_t *bs)
{
	int i, best, best_range;
	
	best = bs->cur_ps.weapon;
	best_range = weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].rV.range;
	
	for(i = WP_KNIFE + 1; i < WP_MM1_GRENADE_LAUNCHER; i++)
	{
		if (!(bs->cur_ps.stats[STAT_WEAPONS] & (1 << i)))
		{
			continue;
		}

		if (weaponData[i].attack[ATTACK_NORMAL].rV.range == 0)
		{
			continue;
		}
		
		if ((bs->cur_ps.ammo[weaponData[i].attack[ATTACK_NORMAL].ammoIndex] < 1) && (bs->cur_ps.clip[ATTACK_NORMAL][i] < 1))
		{
			continue;
		}

		if (weaponData[i].attack[ATTACK_NORMAL].rV.range <= best_range)
		{
			best = i;
			best_range = weaponData[i].attack[ATTACK_NORMAL].rV.range;
		}
	}
	
	if (best != bs->cur_ps.weapon)
	{
		return best;
	}
	
	return 0;
}

/*
==============
Bot_HandleWeapons
==============
*/
void PM_HandleWeaponAction(int weaponAction);
void Bot_HandleWeapons(bot_state_t *bs)
{
	int enemy, weapon;
	
	weapon = 0;	

	// see if we have a better long range weapon
	if (bs->currentEnemy)
	{
		weapon = BotHasBetterLongRangeWeapon(bs);
			
		if (weapon)
		{	
			// must change weapons - we can't hit them right now!!
			if (bs->frame_Enemy_Len > weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].rV.range + 256)
			{
				bs->virtualWeapon = weapon;
				trap_EA_SelectWeapon(bs->client, weapon);
				return;
			}

			// if the weapon we want to switch to is a sniper rifle...
			if (weapon == WP_MSG90A1)
			{	// ... make sure our enemy is fairly far away first
				if (bs->frame_Enemy_Len > 3072)
				{
					bs->virtualWeapon = weapon;
					trap_EA_SelectWeapon(bs->client, weapon);
					return;
				}
			}// if the weapon we found is better, change weapon
			else if ((bs->cur_ps.weapon < WP_MICRO_UZI_SUBMACHINEGUN + 1) && (weapon > WP_MICRO_UZI_SUBMACHINEGUN))
			{
				bs->virtualWeapon = weapon;
				trap_EA_SelectWeapon(bs->client, weapon);
				return;
			}
		}
	}

	// see if we have a better close range weapon
	if (bs->currentEnemy || (bs->enemySeenTime < level.time + (ENEMY_FORGET_MS / 2)))
	{
		weapon = BotHasBetterShortRangeWeapon(bs);

		// this weapon can't reach the distance required so forget it
		if (weapon && bs->currentEnemy && (bs->frame_Enemy_Len > weaponData[weapon].attack[ATTACK_NORMAL].rV.range))
		{
			weapon = 0;
		}
		
		if (weapon)
		{
			// if using a sniper rifle...
			if (/*(bs->cur_ps.weapon == WP_SILENCED_MSG90A1) || */(bs->cur_ps.weapon == WP_MSG90A1))
			{	// ... and we have no enemy / enemy is pretty close, change
				if (!bs->currentEnemy || (bs->frame_Enemy_Len <= 3072))
				{
					bs->virtualWeapon = weapon;
					trap_EA_SelectWeapon(bs->client, weapon);
					return;
				}
			} // if the weapon we found is better, change weapon
			else if ((bs->cur_ps.weapon < WP_MICRO_UZI_SUBMACHINEGUN + 1) && (weapon > WP_MICRO_UZI_SUBMACHINEGUN))
			{
				bs->virtualWeapon = weapon;
				trap_EA_SelectWeapon(bs->client, weapon);
				return;
			}
		}
	}

	if ((bs->cur_ps.weapon == WP_KNIFE) || ((bs->cur_ps.weapon >= WP_RPG7_LAUNCHER) && (bs->cur_ps.weapon <= WP_M15_GRENADE)))
	{
		if (bs->cur_ps.clip[ATTACK_NORMAL][bs->cur_ps.weapon] < 1)
		{
			if (bs->cur_ps.ammo[weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].ammoIndex] < 1)
			{		
				// GodOrDevil - 10.05.06 - 11:19pm - #FIXED - This way would cause a NULL animID and crash the server
				/// ... Lets drop the weapon after its safe.  
				//if (bs->cur_ps.weapon == WP_RPG7_LAUNCHER)
				//{
				//	if (G_DropWeapon (&g_entities[bs->client], bs->cur_ps.weapon, 3000) != NULL)
				//	{	// drop the empty weapon
				//		level.clients[bs->client].ps.weapon = WP_KNIFE;
				//		bs->cur_ps.weapon = WP_KNIFE;
				//		bs->virtualWeapon = WP_KNIFE;
				//	}
				//}
				//BotSelectIdealWeapon(bs);
				//return;

				if(	!BotSelectIdealWeapon(bs))
					trap_EA_SelectWeapon(bs->client, WP_KNIFE);
				//if(bs->virtualWeapon && bs->virtualWeapon != bs->cur_ps.weapon)
				//	G_DropWeapon (&g_entities[bs->client], bs->cur_ps.weapon, 3000);
				return;
			///End  - 10.05.06 - 11:23pm

			}
		}
	}
	else if (bs->cur_ps.clip[ATTACK_NORMAL][bs->cur_ps.weapon] < 1)
	{
		if (bs->cur_ps.ammo[weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].ammoIndex] < 1)
		{	// drop the empty weapon
			// GodOrDevil - 10.05.06 - 11:19pm - #FIXED - This way would cause a NULL animID and crash the server
				/// ... Lets drop the weapon after its safe.  
			//if (G_DropWeapon (&g_entities[bs->client], bs->cur_ps.weapon, 3000) != NULL)
			//{
			//	level.clients[bs->client].ps.weapon = WP_KNIFE;
			//	bs->cur_ps.weapon = WP_KNIFE;
			//	bs->virtualWeapon = WP_KNIFE;
			//}

			//BotSelectIdealWeapon(bs);
			//return;
			if(	!BotSelectIdealWeapon(bs))
				trap_EA_SelectWeapon(bs->client, WP_KNIFE);
			//
			//if(bs->virtualWeapon && bs->virtualWeapon != bs->cur_ps.weapon)
			//	G_DropWeapon (&g_entities[bs->client], bs->cur_ps.weapon, 3000);
			return;
			///End  - 10.05.06 - 11:23pm
		}
		else 
		{	// reload
			if ((bs->cur_ps.weaponstate != WEAPON_RELOADING) && (bs->cur_ps.weaponstate != WEAPON_RELOADING_ALT) && (rand()%10 < 5))
			{
				trap_EA_Attack(bs->client);
			}
			return;
		}
	}
	else if ((bs->cur_ps.weapon > WP_KNIFE) && (rand()%10 < 4))
	{
		if ((bs->cur_ps.ammo[weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].ammoIndex] > 0)
		 && (bs->cur_ps.clip[ATTACK_NORMAL][bs->cur_ps.weapon] < weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].clipSize)
		 && (bs->cur_ps.weaponstate != WEAPON_RELOADING) && (bs->cur_ps.weaponstate != WEAPON_RELOADING_ALT))
		{
			if (bs->enemySeenTime < level.time + (ENEMY_FORGET_MS / 2))
			{
				//Com_Printf(S_COLOR_GREEN "Bot_HandleWeapons : (bs->enemySeenTime < level.time + (ENEMY_FORGET_MS / 2)), doreload\n");
				bs->doReload = 1;
				return;
			}
		}
	}

	if (bs->currentEnemy)
	{
		float range;
		vec3_t a;
		
		range = 0;
		
		if (/*(bs->cur_ps.weapon == WP_SILENCED_MSG90A1) || */(bs->cur_ps.weapon == WP_MSG90A1))
		{
			if (bs->frame_Enemy_Len > 1536)
			{
				if ((!(bs->cur_ps.pm_flags & PMF_ZOOMED)) && (bs->lastAimUp < level.time))
				{
					bs->doZoomIn = 1;

					enemy = Bot_ScanForEnemies(bs);

					if (enemy != -1)
					{
						VectorSubtract(level.clients[enemy].ps.origin, bs->eye, a);
						range = VectorLength(a);
					}
					
					if ((enemy == -1) || (range > 1536))
					{
						bs->lastAimUp = level.time + 3000 + random() * 5000;
						bs->beStill = level.time + 1500 + random() * 3000;
						bs->duckTime = level.time + 2000 + random() * 2500;
					}
				}
			}
			else if (bs->cur_ps.pm_flags & PMF_ZOOMED)
			{
				bs->doZoomOut = 1;
			}
		}
		else if ((weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].pellets <= 1) && (bs->lastAimUp < level.time) && (rand()%10 < 2))
		{
			if (bs->frame_Enemy_Len > 1536)//(weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].rV.range / 3))
			{
				enemy = Bot_ScanForEnemies(bs);
				if (enemy != -1)
				{
					VectorSubtract(level.clients[enemy].ps.origin, bs->eye, a);
					range = VectorLength(a);
				}

				if ((enemy == -1) || (range > 1536))
				{
					bs->lastAimUp = level.time + 7500 + random() * 15000;
					if (range > 1536)
					{
						bs->beStill = level.time + 500 + random() * 2000;
						bs->duckTime = level.time + 1000 + random() * 1500;
					}
				}
			}
		}
	}
}


/*
==============
Bot_StrafeTracing
==============
*/
void Bot_StrafeTracing(bot_state_t *bs)
{
	vec3_t mins, maxs;
	vec3_t right, rorg, drorg;
	trace_t tr;

	mins[0] = -15;
	mins[1] = -15;
	mins[2] = -22;
	maxs[0] = 15;
	maxs[1] = 15;
	maxs[2] = 32;

	AngleVectors(bs->viewangles, NULL, right, NULL);

	if (bs->meleeStrafeDir)
	{
		rorg[0] = bs->origin[0] - right[0]*32;
		rorg[1] = bs->origin[1] - right[1]*32;
		rorg[2] = bs->origin[2] - right[2]*32;
	}
	else
	{
		rorg[0] = bs->origin[0] + right[0]*32;
		rorg[1] = bs->origin[1] + right[1]*32;
		rorg[2] = bs->origin[2] + right[2]*32;
	}

	trap_Trace(&tr, bs->origin, mins, maxs, rorg, bs->client, MASK_PLAYERSOLID);

	if (tr.fraction != 1)
	{
		bs->meleeStrafeDisable = level.time + Q_irand(500, 1500);
	}

	VectorCopy(rorg, drorg);

	drorg[2] -= 32;

	trap_Trace(&tr, rorg, NULL, NULL, drorg, bs->client, MASK_PLAYERSOLID);

	if (tr.fraction == 1)
	{ 	// this may be a dangerous ledge, so don't strafe over it just in case
		bs->meleeStrafeDisable = level.time + Q_irand(500, 1500);
	}
}


/*
==============
Bot_PrimFiring
==============
*/
int Bot_PrimFiring(bot_state_t *bs)
{
	if (bs->cur_ps.weaponstate != WEAPON_CHARGING && bs->doAttack)
	{
		return 1;
	}

	if (bs->cur_ps.weaponstate == WEAPON_CHARGING && !bs->doAttack)
	{
		return 1;
	}

	return 0;
}


/*
==============
Bot_KeepPrimFromFiring
==============
*/
void Bot_KeepPrimFromFiring(bot_state_t *bs)
{
	if (bs->cur_ps.weaponstate != WEAPON_CHARGING && bs->doAttack)
	{
		bs->doAttack = 0;
	}

	if (bs->cur_ps.weaponstate == WEAPON_CHARGING && !bs->doAttack)
	{
		bs->doAttack = 1;
	}
}


/*
==============
Bot_AltFiring
==============
*/
int Bot_AltFiring(bot_state_t *bs)
{
	if (bs->cur_ps.weaponstate != WEAPON_CHARGING_ALT && bs->doAltAttack)
	{				
		return 1;
	}

	if (bs->cur_ps.weaponstate == WEAPON_CHARGING_ALT && !bs->doAltAttack)
	{	
		return 1;
	}

	return 0;
}


/*
==============
Bot_KeepAltFromFiring
==============
*/
void Bot_KeepAltFromFiring(bot_state_t *bs)
{
	if (bs->cur_ps.weaponstate != WEAPON_CHARGING_ALT && bs->doAltAttack)
	{
		bs->doAltAttack = 0;
	}

	if (bs->cur_ps.weaponstate == WEAPON_CHARGING_ALT && !bs->doAltAttack)
	{
		bs->doAltAttack = 1;
	}
}


/*
==============
Bot_CheckForFriendInLOF
==============
*/
gentity_t *Bot_CheckForFriendInLOF(bot_state_t *bs)
{
	vec3_t mins, maxs;
	vec3_t fwd, end;
	gentity_t *trent;
	trace_t tr;
	
	VectorSet(mins, -3, -3, -3);
	VectorSet(maxs, 3, 3, 3);

	AngleVectors(bs->viewangles, fwd, NULL, NULL);
	VectorMA(bs->eye, 2048, fwd, end);

	trap_Trace(&tr, bs->eye, mins, maxs, end, bs->client, MASK_PLAYERSOLID);

	if ((tr.fraction != 1.0f) && (tr.entityNum <= MAX_CLIENTS))
	{
		trent = &g_entities[tr.entityNum];

		if (trent && trent->client)
		{
			if (level.gametypeData->teams && OnSameTeam(&g_entities[bs->client], trent))
			{
				return trent;
			}
		}
	}

	return NULL;
}


/*
==============
BotTrace_AvoidFriendlyInLOF
==============
*/
qboolean BotTrace_AvoidFriendlyInLOF(bot_state_t *bs)
{
	vec3_t start, right, end;
	vec3_t angles, mins, maxs;
	trace_t tr;
	int i;

	// don't do anything if not on the ground
	if (bs->cur_ps.groundEntityNum == ENTITYNUM_NONE)
	{
		return qfalse;
	}
	
	// don't do anything if already jumping / ducking
	if ((bs->jumpTime >= level.time) || (bs->duckTime >= level.time))
	{
		return qfalse;
	}
	
	// don't do anything if on a ladder
	if (bs->cur_ps.pm_flags & PMF_LADDER)
	{
		return qfalse;
	}

	mins[0] = -15;
	mins[1] = -15;
	mins[2] = MINS_Z + 18;
	maxs[0] = 15;
	maxs[1] = 15;
	maxs[2] = DEFAULT_PLAYER_Z_MAX;

	VectorClear(angles);
	angles[YAW] = bs->viewangles[YAW];
	AngleVectors(angles, NULL, right, NULL);
	
	for(i = -1; i < 2; i++)
	{
		if (i == 0)
		{
			continue;
		}

		VectorCopy(bs->origin, start);
		VectorMA(bs->origin, 18 * i, right, end);

		trap_Trace(&tr, start, mins, maxs, end, bs->client, MASK_PLAYERSOLID);

		if ((tr.fraction >= 1.0f) && !tr.allsolid && !tr.startsolid)
		{
			// make sure we're not strafing over a ledge or something
			VectorCopy(end, start);
			end[2] -= 32;
			trap_Trace(&tr, start, mins, maxs, end, bs->client, MASK_PLAYERSOLID);
			if (tr.fraction < 1.0f)
			{
				VectorCopy(start, bs->goalPosition);
				break;
			}
		}

		BotDebug(bs, &bot_debugNav, 4, S_COLOR_YELLOW"BotTrace_AvoidFriendlyInLOF():- bot[%i] -> can't strafe %s\n", bs->client, (i == -1) ? "LEFT" : "RIGHT");
	}
	
	if (i == -1)
	{	// strafe left
		trap_EA_MoveLeft(bs->client);
		BotDebug(bs, &bot_debugNav, 4, S_COLOR_YELLOW"BotTrace_AvoidFriendlyInLOF():- bot[%i] -> strafe LEFT\n", bs->client);
		return qtrue;
	}
	else if (i == 1)
	{	// strafe right
		trap_EA_MoveRight(bs->client);
		BotDebug(bs, &bot_debugNav, 4, S_COLOR_YELLOW"BotTrace_AvoidFriendlyInLOF():- bot[%i] -> strafe RIGHT\n", bs->client);
		return qtrue;
	}
	
	return qfalse;
}


/*
==============
BotReplyGreetings
==============
*/
void BotReplyGreetings(bot_state_t *bs)
{
	int numhello, i;

	numhello = 0;
	
	for(i = 0; i < MAX_CLIENTS; i++)
	{
		if (botstates[i] && botstates[i]->canChat && (i != bs->client))
		{
			botstates[i]->chatObject = &g_entities[bs->client];
			botstates[i]->chatAltObject = NULL;

			if (BotDoChat(botstates[i], "ResponseGreetings", 0))
			{
				numhello++;
			}
		}

		if (numhello > 3)
		{ 	// don't let more than 4 bots say hello at once
			return;
		}
	}
}


/*
==============
StandardBotAI
==============
*/
void StandardBotAI(bot_state_t *bs, float thinktime)
{
	int enemy;
	int doingFallback;
	vec3_t a, ang, headlevel, eorg, dif;
	float reaction;
	float bLeadAmount;
	int meleestrafe = 0;
	int cBAI = 0;
	gentity_t *friendInLOF = NULL;
	int visResult = 0;
	//int selResult = 0;

	if (g_entities[bs->client].inuse && g_entities[bs->client].client
	 && G_IsClientSpectating (g_entities[bs->client].client))
	{
		bs->currentEnemy = NULL;
		bs->wpCurrent = NULL;
		bs->wpDestination = NULL;
		bs->wpLastPoint = NULL;
		bs->fuckUp = 0;
		bs->touchGoal = NULL;
		bs->isTeamLeader = qfalse;
		bs->teamLeader = NULL;
		bs->squadLeader = NULL;
		return;
	}

	if ((!bs->lastDeadTime) || (bs->cur_ps.pm_flags & PMF_RESPAWNED))
	{ 	// just spawned in?
		bs->lastDeadTime = level.time;
		bs->fuckUp = 0;
		bs->touchGoal = NULL;
		bs->wpCurrent = NULL;
		bs->wpDestination = NULL;
		bs->findTeamLeader = level.time + Q_irand(500, 4000);
		bs->teamplayState = TEAMPLAYSTATE_NONE;
		bs->assistTeamMate = NULL;
		bs->guardGTI = NULL;
		bs->guardTrigger = NULL;
		bs->getGTI = NULL;
		bs->teamLeader = NULL;
		bs->squadLeader = NULL;
		bs->isTeamLeader = qfalse;
		Bot_ClearPath(bs);
	}

	if (g_entities[bs->client].health < 1)
	{
		bs->lastDeadTime = level.time;

		if (bs->wpCurrent)
		{	// we don't like this point so much... we died here
			Bot_AddToWPHistory(bs, bs->wpCurrent, 128);
		}
		
		bs->wpCurrent = NULL;
		bs->wpDestination = NULL;
		bs->currentEnemy = NULL;
		bs->wpLastPoint = NULL;

		bs->wpCamping = NULL;
		bs->wpCampingTo = NULL;
		bs->wpStoreDest = NULL;
		bs->wpDestIgnoreTime = 0;
		bs->wpDestSwitchTime = 0;
		bs->wpSeenTime = 0;
		bs->fuckUp = 0;
		bs->touchGoal = NULL;

		if (rand()%10 < 5 && (!bs->doChat || bs->chatTime < level.time))
		{
			trap_EA_Attack(bs->client);
		}

		return;
	}

	BotSetNeedsAndWants(bs);

	// reset the attack states
	bs->doAttack = 0;
	bs->doAltAttack = 0;

	// set up any teamplay stuff here
	Bot_TeamplayAI(bs);

	if (!bs->currentEnemy)
	{
		bs->frame_Enemy_Vis = 0;
	}

	if (bs->currentEnemy && bs->currentEnemy->client
	 && (bs->currentEnemy->client->pers.connected != CON_CONNECTED))
	{
		bs->currentEnemy = NULL;
	}

	doingFallback = 0;

	// if we don't have a weapon out, get one out ASAP
	if (bs->cur_ps.weapon == WP_NONE)
	{
		BotSelectIdealWeapon(bs);		
	}

	reaction = bs->skills.reflex/bs->settings.skill;

	if (reaction < 0)
	{
		reaction = 0;
	}
	
	if (reaction > 2000)
	{
		reaction = 2000;
	}

	if (!bs->currentEnemy)
	{
		bs->timeToReact = level.time + reaction;
	}

	if (bs->wpCamping)
	{
		if (bs->isCamping < level.time)
		{
			bs->wpCamping = NULL;
			bs->isCamping = 0;
		}

		if (bs->currentEnemy && bs->frame_Enemy_Vis)
		{
			bs->wpCamping = NULL;
			bs->isCamping = 0;
		}
	}

	if (bs->wpCurrent && (bs->wpSeenTime < level.time || bs->wpTravelTime < level.time))
	{
		bs->wpCurrent = NULL;
	}

	if (bs->currentEnemy)
	{
		if (bs->enemySeenTime < level.time || !PassStandardEnemyChecks(bs, bs->currentEnemy))
		{
			if ((bs->currentEnemy->health < 1) && bs->lastAttacked && (bs->lastAttacked == bs->currentEnemy))
			{	// CHAT: Killed
				bs->chatObject = bs->currentEnemy;
				bs->chatAltObject = NULL;
				BotDoChat(bs, "Killed", 0);
			}

			bs->currentEnemy = NULL;
		}
	}

	if (!bs->wpCurrent)
	{
		bs->wpCurrent = GetNearestVisibleWP(bs->origin, bs->client);

		if (bs->wpCurrent)
		{
			bs->wpSeenTime = level.time + 1500;
			// never take more than 10 seconds to travel to a waypoint
			bs->wpTravelTime = level.time + 10000;
		}
	}

	if (bs->enemySeenTime < level.time || !bs->frame_Enemy_Vis || !bs->currentEnemy
	 || (bs->currentEnemy && (bs->cur_ps.weapon > WP_NONE) && (bs->frame_Enemy_Len > weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].rV.range + 256))
	  || (rand()%10 < 1))
	{
		enemy = Bot_ScanForEnemies(bs);

		if (enemy != -1)
		{
			bs->currentEnemy = &g_entities[enemy];
			bs->enemySeenTime = level.time + ENEMY_FORGET_MS;
		}
	}

	// ESTABLISH VISIBILITIES AND DISTANCES FOR THE WHOLE FRAME HERE
	if (bs->wpCurrent)
	{
		VectorSubtract(bs->wpCurrent->origin, bs->origin, a);
		bs->frame_Waypoint_Len = VectorLength(a);

		visResult = OrgVisible(bs->origin, bs->wpCurrent->origin, bs->client);
		
		// if we can't see where we are going properly...
		if (Bot_VisionObscured(bs, bs->origin, bs->wpCurrent->origin))
		{
			if (random() < 0.5)
			{
				visResult = 0;
			}
		}
		
		if (visResult)
		{
			bs->frame_Waypoint_Vis = 1;
		}
		else
		{
			bs->frame_Waypoint_Vis = 0;
		}
	}

	if (bs->currentEnemy)
	{
		if (bs->currentEnemy->client)
		{
			VectorCopy(bs->currentEnemy->client->ps.origin, eorg);
			eorg[2] += bs->currentEnemy->client->ps.viewheight;
		}
		else
		{
			VectorCopy(bs->currentEnemy->s.origin, eorg);
		}

		VectorSubtract(eorg, bs->eye, a);
		bs->frame_Enemy_Len = VectorLength(a);

		if (OrgVisible(bs->eye, eorg, -1) && !Bot_VisionObscured(bs, bs->eye, eorg))
		{
			bs->frame_Enemy_Vis = 1;
			VectorCopy(eorg, bs->lastEnemySpotted);
			VectorCopy(bs->origin, bs->hereWhenSpotted);
			bs->lastVisibleEnemyIndex = bs->currentEnemy->s.number;
		}
		else
		{
			bs->frame_Enemy_Vis = 0;
		}
	}
	else
	{
		bs->lastVisibleEnemyIndex = ENTITYNUM_NONE;
	}
	// END
	
	if (bs->frame_Enemy_Vis)
	{
		bs->enemySeenTime = level.time + ENEMY_FORGET_MS;
	}
	
	// START WEAPON HANDLING
	Bot_HandleWeapons(bs);
	// END WEAPON HANDLING
	if (bs->wpCurrent)
	{
		if (bs->frame_Waypoint_Vis)
		{
			// if we lose sight of the point, we have 1.5 seconds to regain it before we drop it
			bs->wpSeenTime = level.time + 1500;
		}
		
		VectorCopy(bs->wpCurrent->origin, bs->goalPosition);

		/*
		if (bs->wpCamping && bs->wpCampingTo)
		{
			VectorSubtract(bs->wpCampingTo->origin, bs->origin, a);
			vectoangles(a, ang);
			VectorCopy(ang, bs->goalAngles);

			VectorSubtract(bs->origin, bs->wpCamping->origin, a);
			if (VectorLength(a) < 64)
			{
				VectorCopy(bs->wpCamping->origin, bs->goalPosition);
				bs->beStill = level.time + 1000;

				if (!bs->campStanding)
				{
					bs->duckTime = level.time + 1000;
				}
			}
		}
		// This code usually causes horrible issues in SoF2 because the levels are more cramped and tight.
		// So I guess for now they'll just have snappier viewangles.
		else*/
		{
			VectorSubtract(bs->wpCurrent->origin, bs->origin, a);
			
			if (bs->cur_ps.pm_flags & PMF_LADDER)
			{
				// if on a ladder and we're not supposed to be, jump...
				if (!(bs->wpCurrent->flags & WPFLAG_LADDER))
				{
					bs->jumpTime = level.time + 100;
				}
				else
				{
					vec3_t ladderMod;

					VectorCopy(bs->goalPosition, ladderMod);

					if (bs->goalPosition[2] > bs->origin[2])
					{
						ladderMod[2] += 400;
					}
					else if (bs->cur_ps.groundEntityNum == ENTITYNUM_NONE)
					{	// keep heading down until our feet touch the ground proper like
						ladderMod[2] -= 200;
					}

					VectorSubtract(ladderMod, bs->origin, a);
				}
			}
			// if our current point is a ladder, but we aint on it...
			else if ((bs->wpCurrent->flags & WPFLAG_LADDER) && (bs->wpCurrent->ladder > -1))
			{
				gentity_t *ent;
				int i;
				
				// see if anyone else is... if they are, stay put
				ent = &g_entities[0];
				for(i = 0; i < MAX_CLIENTS; i++, ent++)
				{
					if (!ent->inuse || !ent->client)
					{
						continue;
					}
					
					if (i == bs->client)
					{
						continue;
					}
					
					if (!(ent->client->ps.pm_flags & PMF_LADDER))
					{
						continue;
					}
					
					if (ent->client->ps.ladder == -1)
					{
						continue;
					}
					
					if (ent->client->ps.ladder == bs->wpCurrent->ladder)
					{
						if (fabs(ent->client->ps.origin[2] - bs->cur_ps.origin[2]) < 90)
						{
							break;
						}
					}
				}
				
				// someone is on our ladder - wait for them to get off
				if (i != MAX_CLIENTS)
				{
					bs->beStill = level.time + 150;
				}
			}
			
			vectoangles(a, ang);
			VectorCopy(ang, bs->goalAngles);
		}

		if (!Bot_AvoidBadThings(bs))
		{
			// if we're following our squad leader or some order, don't do anything else
			if (Bot_GetIdealTeamDestination(bs))
			{
				bs->destinationGrabTime = level.time + 2000;
			}
			else if (bs->destinationGrabTime < level.time && (!bs->wpDestination || (bs->currentEnemy && bs->frame_Enemy_Vis)))
			{
				Bot_GetIdealDestination(bs);
			}
		}
		
		if (bs->touchGoalTime > level.time)
		{
			WPTouchRoutine(bs);
		}
		else if (BotTouchesPoint(bs, bs->wpCurrent->origin))
		{
			WPTouchRoutine(bs);
		}
	}
	else //We can't find a waypoint, going to need a fallback routine.
	{
		doingFallback = BotFallbackNavigation(bs);
	}

	if (bs->timeToReact < level.time && bs->currentEnemy && bs->enemySeenTime > level.time + (ENEMY_FORGET_MS - (ENEMY_FORGET_MS*0.2)))
	{
		if (bs->frame_Enemy_Vis)
		{
			cBAI = CombatBotAI(bs, thinktime);
		}
		else if (bs->cur_ps.weaponstate == WEAPON_CHARGING_ALT)
		{ 	// keep charging in case we see him again 
			// before we lose track of him
			bs->doAltAttack = 1;
		}

		if (bs->destinationGrabTime > level.time + 100)
		{
			// assures that we will continue staying within a 
			// general area of where we want to be in a combat situation
			bs->destinationGrabTime = level.time + 100;
		}

		if (bs->cur_ps.pm_flags & PMF_LADDER)
		{ 	// keep navigating the ladder but if anyone 
			// happens in front of us on it, shoot them up good.
			goto skipviewchecks;
		}

		if (bs->currentEnemy->client)
		{
			VectorCopy(bs->currentEnemy->client->ps.origin, headlevel);
			
			// aim at upper chest if at a distance
			if (bs->frame_Enemy_Len > (weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].rV.range / 2))
			{
				headlevel[2] += 8;
			}
			else
			{
				headlevel[2] += bs->currentEnemy->client->ps.viewheight;
			}
		}
		else
		{
			VectorCopy(bs->currentEnemy->client->ps.origin, headlevel);
		}

		bLeadAmount = BotWeaponCanLead(bs);
		if ((bs->skills.accuracy/bs->settings.skill) <= 8 && bLeadAmount)
		{
			BotAimLeading(bs, headlevel, bLeadAmount);
		}
		else
		{
			VectorSubtract(headlevel, bs->eye, a);
			vectoangles(a, ang);
			VectorCopy(ang, bs->goalAngles);
		}

		if ((bs->launchAngle != 0) && (bs->launchAngle < 90))
		{
			bs->goalAngles[PITCH] = bs->goalAngles[PITCH]-bs->launchAngle;
		}
		bs->launchAngle = 0;

		BotAimOffsetGoalAngles(bs);
	}

skipviewchecks:
	if (bs->currentEnemy)
	{
		if (BotGetWeaponRange(bs) == BWEAPONRANGE_MELEE)
		{
			if (bs->frame_Enemy_Len <= MELEE_ATTACK_RANGE)
			{
				MeleeCombatHandling(bs);
				meleestrafe = 1;
			}
		}
	}

	// just stand and fire if we have no idea where we are
	if (doingFallback && bs->currentEnemy)
	{
		VectorCopy(bs->origin, bs->goalPosition);
	}

	if (bs->doChat && bs->chatTime > level.time && (!bs->currentEnemy || !bs->frame_Enemy_Vis))
	{
		return;
	}
	else if (bs->doChat && bs->currentEnemy && bs->frame_Enemy_Vis)
	{
		//bs->chatTime = level.time + bs->chatTime_stored;
		bs->doChat = 0; //do we want to keep the bot waiting to chat until after the enemy is gone?
		bs->chatTeam = 0;
	}
	else if (bs->doChat && bs->chatTime <= level.time)
	{
		if (bs->chatTeam)
		{
			trap_EA_SayTeam(bs->client, bs->currentChat);
			bs->chatTeam = 0;
		}
		else
		{
			trap_EA_Say(bs->client, bs->currentChat);
		}

		if (bs->doChat == 2)
		{
			BotReplyGreetings(bs);
		}
		bs->doChat = 0;
	}

	if (bs->shootGoal && (!bs->shootGoal->inuse || bs->shootGoal->health <= 0 || !bs->shootGoal->takedamage))
	{
		bs->shootGoal = NULL;
	}		
		
	if (bs->shootGoal)
	{
		dif[0] = (bs->shootGoal->r.absmax[0]+bs->shootGoal->r.absmin[0])/2;
		dif[1] = (bs->shootGoal->r.absmax[1]+bs->shootGoal->r.absmin[1])/2;
		dif[2] = (bs->shootGoal->r.absmax[2]+bs->shootGoal->r.absmin[2])/2;

		if (!bs->currentEnemy || (bs->frame_Enemy_Len > 256))
		{ 	// if someone is close then don't stop shooting them for this
			VectorSubtract(dif, bs->eye, a);
			vectoangles(a, a);
			VectorCopy(a, bs->goalAngles);

			if (InFieldOfVision(bs->viewangles, 30, a)
			 && EntityVisibleBox(bs->origin, NULL, NULL, dif, bs->client, bs->shootGoal->s.number))
			{
				bs->doAttack = 1;
			}
		}
	}

	if (bs->dangerousObject && bs->dangerousObject->inuse && (bs->dangerousObject->health > 0)
	 && bs->dangerousObject->takedamage && (!bs->frame_Enemy_Vis || !bs->currentEnemy)
	  && (BotGetWeaponRange(bs) == BWEAPONRANGE_MID || BotGetWeaponRange(bs) == BWEAPONRANGE_LONG) && !bs->shootGoal)
	{
		float danLen;

		VectorSubtract(bs->dangerousObject->r.currentOrigin, bs->eye, a);

		danLen = VectorLength(a);

		if (danLen > 256)
		{
			vectoangles(a, a);
			VectorCopy(a, bs->goalAngles);

			if (Q_irand(1, 10) < 5)
			{
				bs->goalAngles[YAW] += Q_irand(0, 3);
				bs->goalAngles[PITCH] += Q_irand(0, 3);
			}
			else
			{
				bs->goalAngles[YAW] -= Q_irand(0, 3);
				bs->goalAngles[PITCH] -= Q_irand(0, 3);
			}

			if (InFieldOfVision(bs->viewangles, 30, a)
			 && EntityVisibleBox(bs->origin, NULL, NULL, bs->dangerousObject->r.currentOrigin, bs->client, bs->dangerousObject->s.number))
			{
				bs->doAttack = 1;
			}			
		}
	}

	// if reloading, stop trying to attack
	if ((bs->cur_ps.weaponstate == WEAPON_RELOADING)
	 || (bs->cur_ps.weaponstate == WEAPON_RELOADING_ALT))
	{
		bs->doAttack = 0;
		bs->doAltAttack = 0;
	}

	if (Bot_PrimFiring(bs) || Bot_AltFiring(bs))
	{
		friendInLOF = Bot_CheckForFriendInLOF(bs);
		
		if (friendInLOF)
		{
			// see if we can move left / right a bit so we can shoot
			BotTrace_AvoidFriendlyInLOF(bs);
			
			if (Bot_PrimFiring(bs))
			{
				Bot_KeepPrimFromFiring(bs);
			}

			if (Bot_AltFiring(bs))
			{
				Bot_KeepAltFromFiring(bs);
			}
		}
	}

	if (bs->beStill < level.time && !WaitingForNow(bs, bs->goalPosition))
	{
		VectorSubtract(bs->goalPosition, bs->origin, bs->goalMovedir);
		VectorNormalize(bs->goalMovedir);

		/*if (bs->jumpTime > level.time)
		{
			trap_EA_Move(bs->client, bs->goalMovedir, 5000);
			bs->beStill = level.time + 200;
		}
		else*/
		{
			trap_EA_Move(bs->client, bs->goalMovedir, 5000);
		}

		if (meleestrafe)
		{
			Bot_StrafeTracing(bs);
			
			if (bs->meleeStrafeDisable < level.time)
			{
				if (bs->meleeStrafeDir)
				{
					trap_EA_MoveRight(bs->client);
				}
				else
				{
					trap_EA_MoveLeft(bs->client);
				}
			}
		}

		// check whether we should be strafing, jumping or ducking to
		// avoid something in our way (like a pole, edge or beam)
		if (BotTrace_ObstacleCheck(bs))
		{
			VectorCopy(bs->goalMovedir, a);
			VectorScale(a, -1, bs->goalMovedir);
			
			if (!(bs->cur_ps.pm_flags & PMF_LADDER) && !BotTrace_ObstacleCheck(bs))
			{
				// finish turning towards our goal
				VectorCopy(a, bs->goalMovedir);
				/*
				// head back the way we came and then find new way to go
				if (bs->wpLastPoint && (bs->touchGoalTime < level.time))
				{
					bs->wpCurrent = bs->wpLastPoint;
					VectorCopy(bs->wpCurrent->origin, bs->goalPosition);
					VectorSubtract(bs->goalPosition, bs->origin, bs->goalMovedir);
					VectorNormalize(bs->goalMovedir);
				}

				bs->destinationGrabTime = level.time;
				*/
			}
			else
			{
				// shit, totally stuck
				// hold still for a little while and duck...
				// should let people jump over you (if need be)
				bs->beStill = level.time + 400 + random() * 500;
				bs->duckTime = bs->beStill;
				
				// we really don't like this point... we got stuck here
				if (bs->wpCurrent)
				{
					Bot_AddToWPHistory(bs, bs->wpCurrent, 256);
				}
			}
		}
	}

	if (bs->strafeTime > level.time)
	{
		if (bs->strafeDir == 1)
		{
			trap_EA_MoveRight(bs->client);
		}
		else if (bs->strafeDir == -1)
		{
			trap_EA_MoveLeft(bs->client);
		}
	}


	if (bs->jumpTime > level.time)
	{
		if (!(bs->cur_ps.pm_debounce & PMD_JUMP))
		{
			trap_EA_Jump(bs->client);
		}

		// SoF2 default hack - always jump dump
		// MD = auto crouch jump... hope this hack works right, heh
		if ((bs->cur_ps.groundEntityNum == ENTITYNUM_NONE) && (bs->cur_ps.pm_flags & PMF_JUMPING) && !(bs->cur_ps.pm_flags & PMF_CROUCH_JUMP))
		{
			trap_EA_Crouch(bs->client);
		}
	}

	if (bs->duckTime > level.time)
	{
		if (!(bs->cur_ps.pm_flags & PMF_DUCKED))
		{
			trap_EA_Crouch(bs->client);
		}	
	}
	else if (bs->cur_ps.pm_flags & PMF_DUCKED)
	{
		// if not in crouch jumping / ducking through window etc
		// hit jump to stand back up - crouch is not momentary
		if (!(bs->cur_ps.pm_flags & PMF_JUMPING) && !(bs->cur_ps.pm_flags & PMF_CROUCH_JUMP))
		{
			trap_EA_Jump(bs->client);
		}
	}

	if (bs->doAttack)
	{
		// SOF2 defaults - if not a fully automatic weapon, only hit attack when ready to fire
		// MD no longer has firemodes
		if (bs->cur_ps.firemode[bs->cur_ps.weapon] == WP_FIREMODE_SINGLE)
		{
			if (bs->cur_ps.weaponstate == WEAPON_READY)
			{
				trap_EA_Attack(bs->client);
			}
		}
		/*if (weaponData[bs->cur_ps.weapon].attack[ATTACK_NORMAL].weaponFlags & (1<<WP_FIREMODE_SINGLE))
		{
			if (bs->cur_ps.weaponstate == WEAPON_READY)
			{
				trap_EA_Attack(bs->client);
			}
		}*/
		else
		{
			trap_EA_Attack(bs->client);
		}
	}
	else if (bs->doAltAttack)
	{
		trap_EA_Alt_Attack(bs->client);
	}

	MoveTowardIdealAngles(bs);
}


/*
==================
BotAIStartFrame
==================
*/
int BotAIStartFrame(int time) 
{
	static int local_time;
	int i, elapsed_time;

	G_CheckBotSpawn();

	Bot_UpdateEventTracker();
	Bot_UpdateEntInfo();

	elapsed_time = time - local_time;
	local_time = time;

	// execute bot AI and user commands every frame
	for(i = 0; i < MAX_CLIENTS; i++)
	{
		if (!botstates[i] || !botstates[i]->inuse)
		{
			continue;
		}
		
		if (g_entities[i].client->pers.connected != CON_CONNECTED)
		{
			continue;
		}

		BotAI(i, (float)elapsed_time / 1000);
		BotUpdateInput(botstates[i], time, elapsed_time);
		trap_BotUserCommand(botstates[i]->client, &botstates[i]->lastucmd);
	}

	return qtrue;
}


/*
==============
BotAISetup
==============
*/
int BotAISetup(int restart)
{
	// if the game is restarted for a tournament
	if (restart)
	{
		return qtrue;
	}

	//initialize the bot states
	memset(botstates, 0, sizeof(botstates));

	if (!trap_BotLibSetup())
	{
		return qfalse;
	}

	return qtrue;
}

/*
==============
BotAIShutdown
==============
*/
int BotAIShutdown(int restart)
{
	int i;

	// don't shutdown the bot library, just the bot states
	if (restart)
	{
		for (i = 0; i < MAX_CLIENTS; i++)
		{
			if (botstates[i] && botstates[i]->inuse)
			{
				BotAIShutdownClient(botstates[i]->client, restart);
			}
		}
		
	}
	else
	{
		trap_BotLibShutdown();
	}
	
	return qtrue;
}

#define	MAX_FLASHBANG_AFFECT_DISTANCE	1750
#define MAX_FLASHBANG_DISTANCE			3000
#define MAX_FLASHBANG_TIME				11000
#define	MAX_FLASHBANG_DEAFNESS_DISTANCE	512


void Bot_CheckFlashBang(bot_state_t *bs, vec3_t origin)
{
	trace_t trace;
	vec3_t	start;
	vec3_t	end;
	vec3_t	delta;
	float	distance;
	vec3_t	fromangles;
	int		angle;
	int		time;
	float ddist;

	// Dont flash grenade dead people
	if (bs->cur_ps.pm_type != PM_NORMAL)
	{
		return;
	}

	VectorCopy (origin, start);
	start[2] += bs->cur_ps.viewheight;				

	VectorCopy (bs->cur_ps.origin, end);
	end[2] += bs->cur_ps.viewheight;				

	VectorSubtract(bs->cur_ps.origin, origin, delta);
					
	// distance to the flash bang
	distance = VectorLength(delta);

	// Make sure its not too far away
	if (distance > MAX_FLASHBANG_DISTANCE)
	{
		return;
	}

	// close enough to deafen?
	if (distance < MAX_FLASHBANG_DEAFNESS_DISTANCE)
	{
		ddist = (MAX_FLASHBANG_DEAFNESS_DISTANCE - distance);
		bs->flashbangDeafTime = level.time;
		bs->flashbangDeafFadeTime = (MAX_FLASHBANG_TIME * ddist / MAX_FLASHBANG_DEAFNESS_DISTANCE);
		BotDebug(bs, &bot_debug, 1, S_COLOR_YELLOW"Bot_CheckFlashBang():- I'm deaf!\n");
	}

	// Can the player see the flashbang?
	trap_Trace(&trace, start, NULL, NULL, end, ENTITYNUM_NONE, MASK_SHOT);
	if (trace.contents & (CONTENTS_SOLID|CONTENTS_TERRAIN))
	{
		return;
	}

	VectorNormalize(delta);
	vectoangles(delta, fromangles);

	angle = bs->viewangles[1] - fromangles[1];
	angle += 180;

	if (angle < 0)
	{
		angle += 360;
	}

	// add distance if not facing the grenade.. 
	if (!(angle > 300 || angle < 60))
	{
		angle = 120 - abs(angle-180);
		distance += (MAX_FLASHBANG_AFFECT_DISTANCE * angle / 240);
	}

	BotDebug(bs, &bot_debug, 1, S_COLOR_YELLOW"Bot_CheckFlashBang():- I'm blind!\n");

	if(distance > MAX_FLASHBANG_AFFECT_DISTANCE - 50 )
	{
		distance = MAX_FLASHBANG_AFFECT_DISTANCE - 50;				
	}

	distance = MAX_FLASHBANG_AFFECT_DISTANCE - distance;
	time	 = MAX_FLASHBANG_TIME;

	bs->flashbangTime     = level.time;
	bs->flashbangFadeTime = (MAX_FLASHBANG_TIME * distance / MAX_FLASHBANG_AFFECT_DISTANCE);
	bs->flashbangAlpha    = 1.0f * distance / MAX_FLASHBANG_AFFECT_DISTANCE;

	// If NVG or thermals were on then blast them a bit more
	if ((bs->cur_ps.pm_flags & PMF_GOGGLES_ON))
	{
		bs->flashbangFadeTime += (bs->flashbangFadeTime / 4);
	}
}

void Bot_FlashBang(vec3_t origin)
{
	bot_state_t *bs;
	int i;
				
	for(i = 0; i < MAX_CLIENTS; i++)
	{
		bs = botstates[i];

		if (bs && bs->inuse)
		{
			Bot_CheckFlashBang(bs, origin);
		}			
	}
}

float Bot_IsBlind(bot_state_t *bs)
{
	float alpha;
	
	alpha = 0.0f;
	
	// Is there an active flash bang?
	if (bs->flashbangTime + bs->flashbangFadeTime <= level.time)
	{
		return alpha;
	}

	// Spectators and dead people dont need to see a flash so it can stop here
	if ( (bs->cur_ps.pm_flags & (PMF_GHOST|PMF_FOLLOW)) || bs->cur_ps.pm_type != PM_NORMAL )
	{
		bs->flashbangTime = 0;
		return alpha;
	}
	
	alpha = bs->flashbangAlpha * (1.0f - ((float)(level.time - bs->flashbangTime) / (float)bs->flashbangFadeTime));

	alpha *=  2.0;
	if (alpha < 0)
	{
		alpha = 0;
	}
	if (alpha > 1.0f)
	{
		alpha = 1.0f;
	}

	return alpha;
}


qboolean Bot_VisionObscured(bot_state_t *bs, vec3_t start, vec3_t end)
{
	gentity_t *ent;
	int i;
	
	ent = &g_entities[MAX_CLIENTS];
	for (i = MAX_CLIENTS; i < level.num_entities; i++, ent++) 
	{
		if (!ent->inuse)
		{
			continue;
		}

		if (!bot_entInfo[i].obscures)
		{
			continue;
		}
		
		BotDebug(bs, &bot_debug, 1, S_COLOR_YELLOW"Bot_VisionObscured():- found %s with weapon %i that obscures\n", ent->classname, ent->s.weapon);

		if (Q_TestRaySphere(ent->r.currentOrigin, ent->splashRadius * 0.5, start, end))
		{
			BotDebug(bs, &bot_debug, 1, S_COLOR_YELLOW"Bot_VisionObscured():- My vision is obscured!\n");
			return qtrue;
		}
	}
	
	return qfalse;
}

#endif
