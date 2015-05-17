#include "g_local.h"
gspawn_t* G_SelectClientSpawnPoint ( gentity_t* ent,  qboolean plantsk );
/*
=================
RPM_UpdateTMI

//Used to only send teammate info hence "TMI" but I have decided to
//send all player infos so specs can see health etc.. of both teams
//and its also kinda cool in deathmatches to see the health of the enemy
=================
*/
void RPM_UpdateTMI(void)
{
	int			i, j;
	char		entry[1024];
	char		infoString[2048];
	int			infoStringLength;
	gclient_t	*cl;
	int			numAdded, location;
	gentity_t	*bestLoc;
	int			adm = 0;
/*	if (!level.gametypeData->teams)
	{	// only bother if a league game with teams
		return;
	}
*/
	// if we are waiting for the level to restart, do nothing, even if forced
	if (level.restarted){
		return;
	}

	if(level.pause)
		return;

	if(level.exitTime)
		return;

	// GodOrDevil - 11.01.05 - 07:03am - #updatetmi - added cvar to help control lag server side
	if (g_updatetmi.integer < 1000)
		return;

	if (level.time - level.lastTMIupdate < g_updatetmi.integer)
	///if (level.time - level.lastTMIupdate < 1000)
	{
		return;
	}

	level.lastTMIupdate = level.time;

	entry[0] = 0;
	infoString[0] = 0;
	infoStringLength = 0;
	numAdded = 0;
	
	for (i = 0; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[level.sortedClients[i]];
		if (G_IsClientSpectating(cl) || G_IsClientDead (cl))
		{
			continue;
		}

		bestLoc = Team_GetLocation(&g_entities[i]);

		location = 0;
		if (bestLoc)
		{
			location = bestLoc->health;
		}

	// GodOrDevil - 01.20.05 #HiddenAdmin #ClanMembers #Mute
		/*	Com_sprintf (entry, sizeof(entry),
				" %i %i %i %i %i %i %i",
				level.sortedClients[i],
				cl->ps.stats[STAT_HEALTH],
				cl->ps.stats[STAT_REALARMOR],
				cl->ps.weapon,
				location,
				cl->nvSess.admin,
				cl->nvSess.referee
				);
		*/
		/*==================== Start Hidden Admin ====================*/

			//if (g_noRanks.integer && cl->nvSess.admin > 1)
			//adm = 3;
			//else
			adm = cl->nvSess.admin;

			Com_sprintf (entry, sizeof(entry),
				" %i %i %i %i %i %i %i %i %i",
				level.sortedClients[i],
				cl->ps.stats[STAT_HEALTH],
				cl->ps.stats[STAT_REALARMOR],
				cl->ps.weapon,
				location,
				////cl->nvSess.admin,
				adm,
				cl->nvSess.admin==5?1:0,
				cl->sess.mute,		//CJJ - Mute
#ifdef _CLANMEMBER
				cl->sess.clanMember	// GodOrDevil - #ClanMember
#else
				0
#endif
				);
		/*==================== End Hidden Admin ====================*/
	//End  - 01.20.05

		j = strlen(entry);

		if (infoStringLength + j > 1010)
		{
			break;
		}
		strcpy (infoString + infoStringLength, entry);
		infoStringLength += j;
		numAdded++;

	/*	if (cl->sess.team == TEAM_RED)
		{
			if (stringlengthRed + j > 1022)
			{
				continue;
			}
			strcpy (stringRed + stringlengthRed, entry);
			stringlengthRed += j;
			numAddedRed++;
		}
		else
		{
			if (stringlengthBlue + j > 1022)
			{
				break;
			}
			strcpy (stringBlue + stringlengthBlue, entry);
			stringlengthBlue += j;
			numAddedBlue++;
		}
		*/
	}
	for (i = 0 ; i < level.numConnectedClients; i++)
	{
		cl = &level.clients[level.sortedClients[i]];
		

// GodOrDevil - 1.20.2005 - #scoreboard #Version compatiblity

		if(cl->sess.rpmClient > 0.6 /*&& cl->ps.ping < 500*/)	
		{

		///if(cl->sess.rpmClient >= 0.7 && cl->ps.ping < 600)
		///if(cl->sess.rpmClient >= RPM_VERSION)
			trap_SendServerCommand(level.sortedClients[i], va("tmi %i%s", numAdded, infoString));

		///if (cl->sess.team == TEAM_RED && cl->sess.rpmClient)
			///trap_SendServerCommand(level.sortedClients[i], va("tmi %i%s", numAddedRed, stringRed));
		///else if (cl->sess.team == TEAM_BLUE && cl->sess.rpmClient)
			///trap_SendServerCommand(level.sortedClients[i], va("tmi %i%s", numAddedBlue, stringBlue));
		}
	}
}

#ifdef _STUPID_SHIT

void SC_admCount(void){
	int i;
	level.adminCount	= 0;	
#ifdef _CLANMEMBER
	level.clanCount		= 0;
#endif
	level.refCount		= 0;

	for ( i = 0 ; i < level.maxclients ; i++ ) {
		if ( level.clients[i].pers.connected != CON_CONNECTED )
			continue;

#ifdef _CLANMEMBER
		if ( level.clients[i].sess.clanMember)
			level.clanCount++;
#endif
		if ( level.clients[i].nvSess.referee)
			level.refCount++;
	}
}
#endif
//End - 03.02.05 - 02:40am 


//void RPM_WriteFinalScores ()
//{
//	statinfo_t     *stat;
//	int time = 0;
//
//	stat = &g_entities[level.sortedClients[i]].client->pers.statinfo;		
//	time += ((level.time - g_entities[level.sortedClients[i]].client->pers.enterTime) - g_entities[level.sortedClients[i]].client->sess.totalSpectatorTime) / 1000;
//
//	G_LogPrintf( "%-5s [%-2i] %s  [ %s ]  ping: %-3i  time: %i  score: %i  kills: %i  deaths: %i\n",
//		cl->sess.team==1?"RED":"BLUE",
//		level.sortedClients[i],
//		cl->nvSess.cleanName,
//		cl->nvSess.ip,
//		cl->sess.score,
//		ping,		
//		time,
//		stat->deaths,
//		stat->kills
//	);
//}


