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
qboolean Bot_WantsToGuardGTI(bot_state_t *bs, gentity_t *ent);
qboolean Bot_WantsToPickupGTI(bot_state_t *bs, gentity_t *ent);
qboolean WP_CalcPath(wayPoint_t *start, wayPoint_t *goal, qboolean debug, wayPoint_t *path[], wpHistory_t history[]);
wayPoint_t *GetNearestVisibleWPToItem(vec3_t org, int ignore);
int InFieldOfVision(vec3_t viewangles, float fov, vec3_t angles);

extern bot_state_t *botstates[MAX_CLIENTS];		// bot states

/*
==============
Bot_GetIdealTeamDestination
==============
*/
qboolean Bot_EntNeedsAssistance(gentity_t *ent)
{
	if (!ent->client)
	{
		return qfalse;
	}
	
	if (ent->client->ps.stats[STAT_GAMETYPE_ITEMS])
	{
		return qtrue;
	}
	
	return qfalse;
}


/*
==============
Bot_GetIdealTeamDestination
==============
*/
qboolean Bot_GetIdealTeamDestination(bot_state_t *bs)
{
	vec3_t usethisvec;
	wayPoint_t *tempWP;
	int i;
	vec3_t vec;
	int updateTime;
	gentity_t *teamMate, *guard;
	
	// if we've got a gametype item, forgo all else and just
	// high tail it to where we should be taking the bloody thing...
	if (bs->cur_ps.stats[STAT_GAMETYPE_ITEMS])
	{
		return qfalse;
	}

	// if it isn't time to update our path, just assume we're good and keep going
	if (bs->wpDestSwitchTime >= level.time)
	{
		return qtrue;
	}

	// first up, see if we've got a team mate to help our / follow	
	teamMate = NULL;
	
	if (bs->assistTeamMate)
	{	// if we were supposed to help out someone, but they're dead / gone
		// don't bother helping them out anymore
		if (G_IsClientDead(bs->assistTeamMate->client) || G_IsClientSpectating(bs->assistTeamMate->client) || !Bot_EntNeedsAssistance(bs->assistTeamMate))
		{
			bs->assistTeamMate = NULL;
			bs->teamplayState = TEAMPLAYSTATE_NONE;
			return qfalse;
		}
		teamMate = bs->assistTeamMate;
	}
	else if (bs->squadLeader)
	{	// if we have a live squad leader, stick with them
		if (!G_IsClientDead(bs->squadLeader->client) && !G_IsClientSpectating(bs->squadLeader->client))
		{
			teamMate = bs->squadLeader;
		}
	}

	// we're supposed to follow a buddie or help them out
	if (teamMate)
	{
		VectorCopy(teamMate->client->ps.origin, usethisvec);

		updateTime = Q_irand(1000, 3000);
		VectorSubtract(usethisvec, bs->eye, vec);
		vectoangles(vec, vec);
		if (InFieldOfVision(bs->viewangles, 90, vec) && OrgVisible(bs->eye, usethisvec, -1))
		{
			updateTime = 100;
		}

		if (botstates[teamMate->s.number] && botstates[teamMate->s.number]->wpCurrent)
		{
			tempWP = botstates[teamMate->s.number]->wpCurrent;
		}
		else
		{
			tempWP = GetNearestVisibleWP(usethisvec, teamMate->s.number);
		}

		if (!tempWP)
		{	// bah?
			return qfalse;
		}
			
		if ((bs->wpCurrent == tempWP) && (updateTime == 100))
		{	// close enough, head to em'
			VectorSubtract(bs->origin, usethisvec, vec);
			VectorNormalize(vec);
			VectorMA(usethisvec, 64, vec, bs->goalPosition); 
			bs->wpDestSwitchTime = level.time + updateTime;
			return qtrue;
		}
			
		for(i = 0; i < MAX_NEIGHBOURS; i++)
		{
			if (bs->wpCurrent->neighBours[i] == tempWP)
			{
				bs->wpCurrent = tempWP;
				bs->wpDestSwitchTime = level.time + updateTime;
				return qtrue;
			}
		}
			
		// update path
		if (WP_CalcPath(bs->wpCurrent, tempWP, qfalse, bs->wpPath, bs->wpHistory))
		{
			bs->wpDestination = tempWP;
			bs->wpDestSwitchTime = level.time + updateTime;
			BotDebug(bs, &bot_debugTeamplay, 3, S_COLOR_YELLOW"Bot_GetIdealTeamDestination():- bot[%i] calc'd path to teammate\n", bs->client);
		}
		return qtrue;
	}

	// see if we should be guarding something
	guard = NULL;
	
	if (bs->guardGTI)
	{
		if (bs->guardGTI->r.svFlags & SVF_NOCLIENT)
		{
			bs->guardGTI = NULL;
			bs->teamplayState = TEAMPLAYSTATE_NONE;
			return qfalse;
		}
		guard = bs->guardGTI;
	}
	else if (bs->guardTrigger)
	{
		guard = bs->guardTrigger;
	}

	// we're supposed to go somewhere and guard it
	if (guard)
	{
		if (guard->s.eType == ET_GRAPPLE)
		{
			usethisvec[0] = (guard->r.absmax[0] + guard->r.absmin[0]) / 2;
			usethisvec[1] = (guard->r.absmax[1] + guard->r.absmin[1]) / 2;
			usethisvec[2] = (guard->r.absmax[2] + guard->r.absmin[2]) / 2;
		}
		else
		{
			VectorCopy(guard->s.pos.trBase, usethisvec);
		}

		updateTime = Q_irand(1000, 3000);
		VectorSubtract(usethisvec, bs->eye, vec);
		vectoangles(vec, vec);
		if (InFieldOfVision(bs->viewangles, 90, vec) && OrgVisible(bs->eye, usethisvec, -1))
		{
			updateTime = 100;
		}

		if (bot_entInfo[guard->s.number].wpClosest)
		{
			tempWP = bot_entInfo[guard->s.number].wpClosest;
		}
		else
		{
			tempWP = GetNearestVisibleWP(usethisvec, guard->s.number);
		}
		
		if (!tempWP)
		{	// bah?
			return qfalse;
		}
			
		if ((bs->wpCurrent == tempWP) && (updateTime == 100))
		{	// close enough, head to em'
			VectorSubtract(bs->origin, usethisvec, vec);
			VectorNormalize(vec);
			VectorMA(usethisvec, 64, vec, bs->goalPosition); 
			bs->wpDestSwitchTime = level.time + updateTime;
			return qtrue;
		}
			
		for(i = 0; i < MAX_NEIGHBOURS; i++)
		{
			if (bs->wpCurrent->neighBours[i] == tempWP)
			{
				bs->wpCurrent = tempWP;
				bs->wpDestSwitchTime = level.time + updateTime;
				return qtrue;
			}
		}
			
		// update path
		if (WP_CalcPath(bs->wpCurrent, tempWP, qfalse, bs->wpPath, bs->wpHistory))
		{
			bs->wpDestination = tempWP;
			bs->wpDestSwitchTime = level.time + updateTime;
			BotDebug(bs, &bot_debugTeamplay, 3, S_COLOR_YELLOW"Bot_GetIdealTeamDestination():- bot[%i] calc'd path to %s\n", bs->client, guard->classname);
		}
		return qtrue;
	}
	
	if (bs->getGTI)
	{
		if (bs->getGTI->r.svFlags & SVF_NOCLIENT)
		{
			bs->getGTI = NULL;
			bs->teamplayState = TEAMPLAYSTATE_NONE;
			return qfalse;
		}

		VectorCopy(bs->getGTI->s.pos.trBase, usethisvec);

		if (bot_entInfo[bs->getGTI->s.number].wpClosest)
		{
			tempWP = bot_entInfo[bs->getGTI->s.number].wpClosest;
		}
		else
		{
			tempWP = GetNearestVisibleWP(usethisvec, bs->getGTI->s.number);
		}

		// update path
		if (tempWP && WP_CalcPath(bs->wpCurrent, tempWP, qfalse, bs->wpPath, bs->wpHistory))
		{
			bs->touchGoal = bs->getGTI;
			bs->wpDestination = tempWP;
			bs->wpDestSwitchTime = level.time + Q_irand(1000, 3000);
			BotDebug(bs, &bot_debugGameType, 3, S_COLOR_YELLOW"Bot_GetIdealTeamDestination():- bot[%i] off to guard %s\n", bs->client, bs->getGTI->classname);
			return qtrue;
		}
	}
	
	return qfalse;
}


/*
==============
BotScanForTeamLeader

Bots will only automatically obtain a leader if it's another bot using this method.
==============
*/
void BotScanForTeamLeader(bot_state_t *bs)
{
	int i;
	gentity_t *ent;

	ent = &g_entities[0];
	for(i = 0; i < MAX_CLIENTS; i++, ent++)
	{
		if (!ent->inuse)
		{
			continue;
		}
		
		if (bs->client == i)
		{
			continue;
		}
			
		if (!botstates[i])
		{
			continue;
		}

		if (botstates[i]->isTeamLeader && OnSameTeam(&g_entities[bs->client], ent))
		{
			bs->teamLeader = ent;
			BotDebug(bs, &bot_debugTeamplay, 1, S_COLOR_YELLOW"BotScanForTeamLeader():- bot[%i] found %s as my team leader!\n", bs->client, bs->teamLeader->client->pers.netname);
			break;
		}
	}
}

/*
==============
Bot_CommanderAI
==============
*/
void Bot_CommanderAI(bot_state_t *bs)
{
	gentity_t *squad[MAX_CLIENTS];
	bot_state_t *teammate, *sleader;
	gentity_t *ent, *trigger;
	int i, j, k, teammembers, follow;
	gentity_t *assist[MAX_CLIENTS];
	qboolean assisting[MAX_CLIENTS];
	int assneeded;
	gentity_t *guardGTI[16];
	int guardingGTI[16];
	int guardGTIcount;
	gentity_t *getGTI[16];
	int grabbingGTI[16];
	int grabGTIcount;
	gentity_t *guardTrigger[16];
	int guardingTrigger[16];
	int guardTriggerCount;

	if (bs->teamLeader_giveOrders > level.time)
	{
		return;
	}

	bs->teamLeader_giveOrders = level.time + 500;

	BotDebug(bs, &bot_debugTeamplay, 2, S_COLOR_YELLOW"Bot_CommanderAI():- bot[%i]\n", bs->client);

	assneeded = 0;
	guardGTIcount = 0;
	grabGTIcount = 0;
	guardTriggerCount = 0;

	// go through entInfos looking for gametype items
	ent = &g_entities[MAX_CLIENTS];
	for (i = MAX_CLIENTS; i < level.num_entities; i++, ent++) 
	{
		if (!ent->inuse || !ent->item)
		{
			continue;
		}
		
		if (ent->item->giType != IT_GAMETYPE)
		{
			continue;
		}

		// if item is valid, its either at home base or dropped
		// FIX ME - stick with magically knowledge of dropped GTI locations??
		if (bot_entInfo[i].valid)
		{
			// see if we want to guard this item
			if (Bot_WantsToGuardGTI(bs, ent))
			{
				if (guardGTIcount < 16)
				{
					guardGTI[guardGTIcount] = ent;
					guardingGTI[guardGTIcount] = 0;
					guardGTIcount++;
				}
			}

			// see if we want to get this item
			if (Bot_WantsToPickupGTI(bs, ent))
			{
				if (grabGTIcount < 16)
				{
					getGTI[grabGTIcount] = ent;
					grabbingGTI[grabGTIcount] = 0;
					grabGTIcount++;
				}
			}
			continue;
		}

		// if we want to guard this, but it is gone...
		if (Bot_WantsToGuardGTI(bs, ent))
		{
			// ... then go guard we're you'd capture this
			if (guardTriggerCount >= 16)
			{
				continue;
			}

			j = ITEM_INDEX(ent->item) - MODELINDEX_GAMETYPE_ITEM;

			trigger = NULL;
			while (NULL != (trigger = G_Find (trigger, FOFS(classname), "gametype_trigger")))
			{
				//if (strstr(ent->targetname, bot_GTInfo[j].target))
				if (!Q_stricmp (ent->targetname, bot_GTInfo[j].target)
				 || !Q_stricmp (ent->targetname, bot_GTInfo[j].target2)
				  || !Q_stricmp (ent->targetname, bot_GTInfo[j].target3))
				{
					guardTrigger[guardTriggerCount] = trigger;
					guardingTrigger[guardTriggerCount] = 0;
					guardTriggerCount++;		
				}
			}
			continue;
		}
		
		// FIX ME - have magically knowledge of GTI thief locations??
		/*
		// ignore invalid dropped items - they'll be gone soon
		if (ent->flags & FL_DROPPED_ITEM)
		{
			continue;
		}
		
		// ignore anything that our team doesn't want
		if (!Bot_WantsToPickupGTI(bs, ent))
		{
			continue;
		}
		*/
	}

	squad[0] = &g_entities[bs->client];
	teammembers = 1;
	
	ent = &g_entities[0];
	for (i = 0; i < MAX_CLIENTS; i++, ent++) 
	{
		if (!ent->inuse)
		{
			continue;
		}

		if (!ent->client)
		{
			continue;
		}
		
		// already asigned our self somewhere
		if (i == bs->client)
		{
			continue;
		}

		// skip ghosts/specators
		if (G_IsClientSpectating(ent->client))
		{
			continue;
		}

		if (!OnSameTeam(&g_entities[bs->client], ent))
		{
			continue;
		}
		// FIX - skip dead people?
		/*
		if (G_IsClientDead(ent->client))
		{
			continue;
		}
		*/

		// this guy has something important and
		// should be escorted or some such thing
		if (Bot_EntNeedsAssistance(ent))
		{
			assist[assneeded] = ent;
			assisting[assneeded] = 0;
			assneeded++;
			continue;
		}

		if (!botstates[ent->s.number])
		{
			continue;
		}
		
		squad[teammembers] = ent;
		teammembers++;
	}
	
	/*
	if (teammembers < 2)
	{
		BotDebug(bs, &bot_debugTeamplay, 2, S_COLOR_YELLOW"Bot_CommanderAI():- bot[%i] not enough team members\n", bs->client);
		return;
	}
	*/

	// every so often tell the squad to regroup for the sake of variation...
	// ... unless we're busy, in which case, keep going for a while
	if ((bs->teamplayState == TEAMPLAYSTATE_NONE) && (bs->squadRegroupInterval < level.time) && Q_irand(1, 10) < 5)
	{
		bs->teamplayState = TEAMPLAYSTATE_REGROUP;
		bs->isTeamLeader = qfalse;
		bs->teamLeader = NULL;
		bs->findTeamLeader = level.time + Q_irand(500, 4000);
		bs->squadRegroupInterval = level.time + Q_irand(45000, 65000);
		BotDebug(bs, &bot_debugTeamplay, 1, S_COLOR_YELLOW"Bot_CommanderAI():- bot[%i] -> tell the squad to regroup \n", bs->client);
	}

	follow = -1;

	// figure out who is supposed to be doing what (including ourselves) 
	// and then tell them to go do it...
	for(i = 0; i < teammembers; i++)
	{
		if (!squad[i])
		{
			break;
		}

		teammate = botstates[squad[i]->s.number];
		
		sleader = NULL;
		if (teammate->squadLeader)
		{
			sleader = botstates[teammate->squadLeader->s.number];
		}

		// we're shuffling up who is leader
		if (bs->teamplayState == TEAMPLAYSTATE_REGROUP)
		{
		 	if (teammate->teamplayState == TEAMPLAYSTATE_NONE)
		 	{
				teammate->teamplayState = TEAMPLAYSTATE_REGROUP;
				teammate->squadLeader = NULL;
			}
			teammate->findTeamLeader = level.time + Q_irand(500, 4000);
			teammate->teamLeader = NULL;
			continue;
		}

		if (teammate->cur_ps.stats[STAT_GAMETYPE_ITEMS])
		{
			// this guy has a gametype items and should just get a move on
			teammate->teamplayState = TEAMPLAYSTATE_BUSY;
		}

		switch(teammate->teamplayState)
		{
		case TEAMPLAYSTATE_BUSY:
			// we've doing something that overrides any other needs, continue
			break;
			
		case TEAMPLAYSTATE_ASSISTING:
			// as long as they still need assisting, we're busy
			if (!teammate->assistTeamMate || !Bot_EntNeedsAssistance(teammate->assistTeamMate))
			{
				teammate->assistTeamMate = NULL;
				teammate->teamplayState = TEAMPLAYSTATE_NONE;
			}
			break;

		case TEAMPLAYSTATE_FOLLOWING:
			// if there are higher prioritys and our squad leader isn't 
			// doing something important, we can go do something important
			if (sleader)
			{
				teammate->assistTeamMate = NULL;
				
				if ((sleader->teamplayState == TEAMPLAYSTATE_ASSISTING) && sleader->assistTeamMate)
				{	// squad leader is assisting, so we should assist too
					teammate->assistTeamMate = sleader->assistTeamMate;
				}
				else if ((sleader->teamplayState == TEAMPLAYSTATE_BUSY) || Bot_EntNeedsAssistance(teammate->squadLeader))
				{
					teammate->assistTeamMate = teammate->squadLeader;
				}
				else
				{
					sleader = NULL;
				}
			}
			
			if (!sleader)
			{
				teammate->squadLeader = NULL;
				teammate->teamplayState = TEAMPLAYSTATE_NONE;
			}				
			break;

		case TEAMPLAYSTATE_GUARDING_GTI:
			// if the item is still there, keep guarding it
			if (!teammate->guardGTI || !bot_entInfo[teammate->guardGTI->s.number].valid)
			{
				teammate->guardGTI = NULL;
				teammate->teamplayState = TEAMPLAYSTATE_NONE;
			}
			break;

		case TEAMPLAYSTATE_GUARDING_TRIGGER:
			// if we no longer need to guard the GTI trigger, stop
			if (teammate->guardTrigger)
			{
				for(j = 0; j < guardGTIcount; j++)
				{
					k = ITEM_INDEX(guardGTI[j]->item) - MODELINDEX_GAMETYPE_ITEM;
					//if (strstr(ent->targetname, bot_GTInfo[k].target))
					if (!Q_stricmp (ent->targetname, bot_GTInfo[k].target)
				 	 || !Q_stricmp (ent->targetname, bot_GTInfo[k].target2)
				  	  || !Q_stricmp (ent->targetname, bot_GTInfo[k].target3))
					{
						break;
					}
				}
				if (j == guardGTIcount)
				{
					teammate->guardTrigger = NULL;
				}
			}
			
			if (!teammate->guardTrigger)
			{
				teammate->guardTrigger = NULL;
				teammate->teamplayState = TEAMPLAYSTATE_NONE;
			}
			break;
		
		case TEAMPLAYSTATE_GETTING_GTI:
			// if the item is still there, keep guarding it
			if (!teammate->getGTI || !bot_entInfo[teammate->getGTI->s.number].valid)
			{
				teammate->getGTI = NULL;
				teammate->teamplayState = TEAMPLAYSTATE_NONE;
			}
			break;

		case TEAMPLAYSTATE_REGROUP:
			// if we've got a leader, stop trying to regropu
			if (bs->isTeamLeader && (bs != teammate))
			{
				teammate->isTeamLeader = qfalse;
				teammate->teamplayState = TEAMPLAYSTATE_NONE;
		 	}
			break;
		
		default:
		case TEAMPLAYSTATE_NONE:
			break;
		}

		if (teammate->teamplayState == TEAMPLAYSTATE_BUSY)
		{
			continue;
		}

		if (teammate->assistTeamMate)
		{
			for(j = 0; j < assneeded; j++)
			{
				if (teammate->assistTeamMate == assist[j])
				{
					assisting[j]++;
					break;
				}
			}
			// we're helping a team mate, so we're busy
			continue;
		}

		if (teammate->guardGTI)
		{
			for(j = 0; j < guardGTIcount; j++)
			{
				if (teammate->guardGTI == guardGTI[j])
				{
					guardingGTI[j]++;
					break;
				}
			}
			// we're guarding something important, keep doing it
			continue;
		}
		
		if (teammate->guardTrigger)
		{
			for(j = 0; j < guardTriggerCount; j++)
			{
				if (teammate->guardTrigger == guardTrigger[j])
				{
					guardingTrigger[j]++;
					break;
				}
			}
			// we're guarding something important, keep doing it
			continue;
		}

		if (teammate->getGTI)
		{
			for(j = 0; j < grabGTIcount; j++)
			{
				if (teammate->getGTI == getGTI[j])
				{
					grabbingGTI[j]++;
					break;
				}
			}
			// we're getting something important, keep doing it
			continue;
		}
		

		// see if we need to help anyone
		for(k = 0; k < 3; k++)
		{
			for(j = 0; j < assneeded; j++)
			{
				if (!assist[j])
				{
					break;
				}
				
				if (assisting[j] > k)
				{
					// got enough help already
					continue;
				}
			
				teammate->assistTeamMate = assist[j];
				teammate->teamplayState = TEAMPLAYSTATE_ASSISTING;
				assisting[j]++;
				break;
			}
			if (teammate->assistTeamMate)
			{
				break;
			}
		}
		
		// we're now helping someone
		if (teammate->assistTeamMate)
		{
			BotDebug(bs, &bot_debugTeamplay, 2, S_COLOR_YELLOW"Bot_CommanderAI():- told teammate %i to assist a team mate\n", i);
			continue;
		}

		// see if we need to go guard a GTI trigger
		for(k = 0; k < 3; k++)
		{
			for(j = 0; j < guardTriggerCount; j++)
			{
				if (!guardTrigger[j])
				{
					break;
				}
				
				if (guardingTrigger[j] > k)
				{
					// got enough help already
					continue;
				}
			
				teammate->guardTrigger = guardTrigger[j];
				teammate->teamplayState = TEAMPLAYSTATE_GUARDING_TRIGGER;
				guardingTrigger[j]++;
				break;
			}
			
			if (teammate->guardTrigger)
			{
				break;
			}
		}
			
		// we're now guarding some GTI trigger
		if (teammate->guardTrigger)
		{
			BotDebug(bs, &bot_debugTeamplay, 2, S_COLOR_YELLOW"Bot_CommanderAI():- told teammate %i to guard trigger %s\n", i, teammate->guardTrigger->classname);
			continue;
		}

		// see if we need to go guard a GTI
		for(k = 0; k < 3; k++)
		{
			for(j = 0; j < guardGTIcount; j++)
			{
				if (!guardGTI[j])
				{
					break;
				}
				
				if (guardingGTI[j] > k)
				{
					// got enough help already
					continue;
				}
			
				teammate->guardGTI = guardGTI[j];
				teammate->teamplayState = TEAMPLAYSTATE_GUARDING_GTI;
				guardingGTI[j]++;
				break;
			}
			
			if (teammate->guardGTI)
			{
				break;
			}
		}
			
		// we're now guarding some GTI trigger
		if (teammate->guardGTI)
		{
			BotDebug(bs, &bot_debugTeamplay, 2, S_COLOR_YELLOW"Bot_CommanderAI():- told teammate %i to guard %s\n", i, teammate->guardGTI->classname);
			continue;
		}

		// see if we need to go get a GTI
		for(k = 0; k < 3; k++)
		{
			for(j = 0; j < grabGTIcount; j++)
			{
				if (!getGTI[j])
				{
					break;
				}
				
				if (grabbingGTI[j] > k)
				{
					// got enough help already
					continue;
				}
			
				teammate->getGTI = getGTI[j];
				teammate->teamplayState = TEAMPLAYSTATE_GUARDING_GTI;
				grabbingGTI[j]++;
				break;
			}
			
			if (teammate->getGTI)
			{
				break;
			}
		}

		// we're now going to get some GTI
		if (teammate->getGTI)
		{
			BotDebug(bs, &bot_debugTeamplay, 2, S_COLOR_YELLOW"Bot_CommanderAI():- told teammate %i to get %s\n", i, teammate->getGTI->classname);
			continue;
		}

		if (follow != -1)
		{	// follow the person in front of us making groups of two
			teammate->teamplayState = TEAMPLAYSTATE_FOLLOWING;
			teammate->squadLeader = squad[follow];
			BotDebug(bs, &bot_debugTeamplay, 2, S_COLOR_YELLOW"Bot_CommanderAI():- told teammate %i to follow %i\n", i, follow);
			follow = -1;
			continue;
		}

		// wander around as we need
		teammate->teamplayState = TEAMPLAYSTATE_NONE;
		follow = i;
		BotDebug(bs, &bot_debugTeamplay, 2, S_COLOR_YELLOW"Bot_CommanderAI():- told teammate %i to be a squad leader and wander around\n", i);
	}
}


/*
==============
Bot_TeamplayAI
==============
*/
void Bot_TeamplayAI(bot_state_t *bs)
{
	if (!level.gametypeData->teams)
	{
		return;
	}
	
	if (bs->teamLeader && G_IsClientDead(bs->teamLeader->client))
	{	// get a new leader ASAP
		bs->teamLeader = NULL;
		bs->findTeamLeader = level.time + Q_irand(150, 500);
		bs->teamplayState = TEAMPLAYSTATE_REGROUP;
	}

	if (bs->teamplayState == TEAMPLAYSTATE_REGROUP)
	{ 	// force to find a new leader
		bs->squadLeader = NULL;
		bs->teamLeader = NULL;
		bs->isTeamLeader = qfalse;
	}

	if (!bs->teamLeader && !bs->isTeamLeader)
	{
		BotScanForTeamLeader(bs);
	}

	if (!bs->teamLeader && (bs->findTeamLeader < level.time))
	{
		// if still no leader after scanning, then become team leader
		bs->isTeamLeader = qtrue;
		bs->teamLeader = &g_entities[bs->client];
		bs->squadRegroupInterval = level.time + Q_irand(45000, 65000);
		bs->teamLeader_giveOrders = 0;
		bs->teamplayState = TEAMPLAYSTATE_NONE;

		BotDebug(bs, &bot_debugTeamplay, 1, S_COLOR_YELLOW"Bot_TeamplayAI():- bot[%i] still no leader after scanning, becoming team leader\n", bs->client);
	}

	// if I am the squad leader, tell everyone what to do
	if (bs->isTeamLeader)
	{
		if (bs->squadLeader)
		{	// we don't follow anyone if we are a team leader
			bs->squadLeader = NULL;
		}
		Bot_CommanderAI(bs);
		return;
	}

	if (bs->state_Forced)
	{
		bs->teamplayState = bs->state_Forced;
	}
}

#endif

