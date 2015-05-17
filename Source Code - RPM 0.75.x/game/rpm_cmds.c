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

void Cmd_Kill_f( gentity_t *ent );
// GodOrDevil - 04.08.05 - 04:54pm - CmdSay for #scPlay scTeam
void Cmd_Say_f( gentity_t *ent, int mode, qboolean arg0 );

/*
=================
Cmd_Stats_f
=================
*/

void Cmd_Stats_f( gentity_t *ent ) 
{
	statinfo_t     *stat;
	char		   arg1[4];	
	float		   accuracy = 0/*, rankscore*/;
	int			   idnum, n, warncount, tkcount;
	char		   *altname, *plrName, *s, *r, *ip, *country;
	float			version;
	char			userinfo[MAX_INFO_STRING];


	trap_Argv( 1, arg1, sizeof( arg1 ) );  //get the number of the client that this client
										   //wanted to tsee the stats for
	if (arg1[0] < '0' || arg1[0] > '9')	//if no number was entered get the stats for the 
	{									//the client that called us
		stat = &ent->client->pers.statinfo;
		// GodOrDevil - 03.01.05 - 11:06pm #warnCount-#tkCount
		plrName   = ent->client->pers.netname;
		version   = ent->client->sess.rpmClient;
		tkcount   = ent->client->sess.tkCount;
		warncount = ent->client->sess.warnCount * 20;
		trap_GetUserinfo( ent->s.number, userinfo, sizeof( userinfo ) );
		ip		  = ent->client->nvSess.ip;
		r		  = Info_ValueForKey ( userinfo, "rate" );
		s		  = Info_ValueForKey ( userinfo, "snaps" );
		country	  = ent->client->sess.country;
		//End - 03.01.05 - 11:10pm 
	}


	else		//if a client number was entered
	{
		idnum = atoi (arg1);		//check to see that the id number of the client was a valid number

		if ( idnum < 0 || idnum >= g_maxclients.integer )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Invalid client number %d.\n\"", idnum ));
			return;
		}
		
		//Is the client connected?
		if ( g_entities[idnum].client->pers.connected == CON_DISCONNECTED )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"There is no client with the client number %d.\n\"", idnum ));
			return;
		}

		stat = &g_entities[idnum].client->pers.statinfo;
		plrName = g_entities[idnum].client->pers.netname;
		// GodOrDevil - 03.01.05 - 11:11pm #warnCount-#tkCount
		version		= g_entities[idnum].client->sess.rpmClient;
		tkcount		= g_entities[idnum].client->sess.tkCount;
		warncount	= g_entities[idnum].client->sess.warnCount * 20;
		trap_GetUserinfo( g_entities[idnum].s.number, userinfo, sizeof( userinfo ) );
		ip			= g_entities[idnum].client->nvSess.ip;
		country		= g_entities[idnum].client->sess.country;
		r			= Info_ValueForKey ( userinfo, "rate" );
		s			= Info_ValueForKey ( userinfo, "snaps" );
		//End - 03.01.05 - 011:15pm #warnCount-tkCount
	}
	// GodOrDevil - 03.01.05 - 11:04pm #Player Stats - TK&Warn Levels
	//trap_SendServerCommand( ent-g_entities, va("print \"\n^3Player Stats for: ^7[^3%s^7]^7 %s\n\"",  g_entities[idnum].client->sess.country, plrName ));
	trap_SendServerCommand( ent-g_entities, va("print \"\n^3Player Stats for: ^7[^3%s^7]^7 %s\n\"",  country, plrName ));
	trap_SendServerCommand( ent-g_entities, va("print \"--------------------------------------------------------\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \"^1Team Kill Penalties: ^7%i        -        ^1Warning Level: ^7%i\n\"", tkcount, warncount));
	trap_SendServerCommand( ent-g_entities, va("print \"--------------------------------------------------------\n\""));
	//End - 03.01.05 - 11:04pm 

	if(stat->shotcount)//dont bother displaying stats if the client hasn't fired a shot yet
	{
		//print the format of the stats so the client knows what the number are
		trap_SendServerCommand( ent-g_entities, va("print \"^4Weapon      ^7{ ^4Shots ^7/ ^4Hits  ^7}   { ^4Accuracy ^7}   { ^4HeadShots ^7}\n\""));
	
		//display the stats for each weapon that the client has fired
		for(n = 0; n < WP_NUM_WEAPONS; n++)
		{
			if(stat->weapon_shots[ATTACK_NORMAL][n] <= 0 && stat->weapon_shots[ATTACK_ALTERNATE][n] <=0)
			{
				continue;
			}

			//get the accuracy by dividing the number of hits by shots * 100
			accuracy = 0;
			//Use this if to be absolutely sure not to divde by 0
			if(stat->weapon_shots[ATTACK_NORMAL][n])
			{
				accuracy = (float)stat->weapon_hits[ATTACK_NORMAL][n] / (float)stat->weapon_shots[ATTACK_NORMAL][n] * 100;
			}

			trap_SendServerCommand( ent-g_entities, va("print \"^5%11s { ^7%5d ^5/ ^7%5d ^5}     {^7%3.2f ^5}     { ^7%5d ^5}\n\"",
				bg_weaponNames[n], 
				stat->weapon_shots[ATTACK_NORMAL][n], 
				stat->weapon_hits[ATTACK_NORMAL][n], 
				accuracy, 
				stat->weapon_headshots[ATTACK_NORMAL][n] ));	
			
			//did the client use the weapons alternate attack?
			if(stat->weapon_shots[ATTACK_ALTERNATE][n])
			{
				switch(n)
				{
					case WP_AK74_ASSAULT_RIFLE:
						altname = "Bayonette";
						break;

					case WP_M4_ASSAULT_RIFLE:
						altname = "M203";
						break;

					case WP_M590_SHOTGUN:
						altname = "Bludgeon";
						break;

					case WP_M1911A1_PISTOL:          
					case WP_USSOCOM_PISTOL: 
						altname = "Pistol Whip";
						break;

					default:
						altname = "none";
						break;
				}

				if(Q_stricmp (altname, "none") != 0)
				{
					accuracy = 0;
					if(stat->weapon_hits[ATTACK_ALTERNATE][n])
					{
						accuracy = (float)stat->weapon_hits[ATTACK_ALTERNATE][n] / (float)stat->weapon_shots[ATTACK_ALTERNATE][n] * 100;
					}

					trap_SendServerCommand( ent-g_entities, va("print \"^5%11s { ^7%5d ^5/ ^7%5d ^5}     {^7%3.2f ^5}     { ^7%5d ^5}\n\"",
						altname, 
						stat->weapon_shots[ATTACK_ALTERNATE][n], 
						stat->weapon_hits[ATTACK_ALTERNATE][n], 
						accuracy, 
						stat->weapon_headshots[ATTACK_ALTERNATE][n] ));
				}
			}
		}
	}

	trap_SendServerCommand( ent-g_entities, va("print \"^3Total Deaths: ^7%d    ^3Total Kills: ^7%d    ^3HeadShot Kills: ^7%d\n\"", 
		stat->deaths, 
		stat->kills, 
		stat->headShotKills)); 
	trap_SendServerCommand( ent-g_entities, va("print \"^3Damage Done: ^7%d    ^3Damage Taken: ^7%d\n\"", 
		stat->damageDone,
		stat->damageTaken));
	trap_SendServerCommand( ent-g_entities, va("print \"^1Total Shots: ^7%d\n^1Total Shots Hit: ^7%d\n\"",
		stat->shotcount, 
		stat->hitcount));
/*
	accuracy = 0;
	if(stat->hitcount)
	{
		accuracy = (float)stat->hitcount / (float)stat->shotcount * 100;
	}
*/
	trap_SendServerCommand( ent-g_entities, va("print \"^3Accuracy Percentage: ^7%3.2f\n\"", stat->accuracy));
	trap_SendServerCommand( ent-g_entities, va("print \"^1Hand Hits: ^7%d     ^1Foot Hits: ^7%d     ^1Arm Hits: ^7%d    ^1Torso Hits: ^7%d\n\"",
		stat->handhits,
		stat->foothits, 
		stat->armhits, 
		stat->leghits));
	trap_SendServerCommand( ent-g_entities, va("print \"^1Head Hits: ^7%d     ^1Neck hits: ^7%d     ^1Leg Hits: ^7%d    ^1Waist Hits: ^7%d\n\"",
		stat->headhits, 
		stat->neckhits, 
		stat->torsohits, 
		stat->waisthits));
	// GodOrDevil - 03.01.05 - 08:38pm #ExtraClientInfo
	trap_SendServerCommand( ent-g_entities, va("print \"--------------------------------------------------------\n\""));
	if (ent->client->nvSess.admin >= 4){
	trap_SendServerCommand( ent-g_entities, va("print \"^1IP^7:%s    ^1Rate^7:%s    ^1Snaps^7:%s\n\"", ip, r, s));
	}
	if (!version){
		trap_SendServerCommand( ent-g_entities, va("print \"^3%s is ^1NOT ^7running ^1R^3P^4M\n\"",plrName));
	}
	else if (version < RPM_VERSION){
		trap_SendServerCommand( ent-g_entities, va("print \"^3%s is running ^1OLDER VERSION^7 ^1R^3P^4M^7 %.1f\n\"", plrName, version));
	} else {
		trap_SendServerCommand( ent-g_entities, va("print \"^3%s ^7is running ^1R^3P^4M^7 %.1f\n\"", plrName, version));
	}
	//End - 03.01.05 - 08:50pm
/*
	if(stat->hitcount)		//here is a formula to figure out the clients skill depending on
	{						//their shot accuracy, number of headshots and death/kill ratio
		rankscore = (((float)stat->headhits + (float)stat->neckhits) / (float)stat->hitcount) * 100;
		rankscore += accuracy;

		if(rankscore > 100)
		{rankscore = 100;}

		if(!stat->deaths && stat->kills)
		{rankscore += 100;}

		else if(!stat->kills && stat->deaths)
		{rankscore -= stat->deaths;}

		else if(!stat->kills && !stat->deaths)
		{}

		else
		{rankscore += (((float)stat->kills + (float)stat->headShotKills) / (float)stat->deaths) * 10;}

		rankscore /= 2;

		if(rankscore > 100)
		{rankscore = 100;}

		if(rankscore < 0)
		{rankscore = 0;}


		//set their rank depending on their skill(rankscore)
		if(rankscore < 10)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"^2Skill: ^7%3.2f   ^6Rank: ^3NOOB^7, Dude your getting OWNED!\n\"", rankscore));
		}

		if(rankscore >= 10 && rankscore < 20)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"^2Skill: ^7%3.2f   ^6Rank: ^3Private^7, Doing good for a ROOKIE!\n\"", rankscore));
		}

		if(rankscore >= 20 && rankscore < 30)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"^2Skill: ^7%3.2f   ^6Rank: ^3Sergeant^7, Keep up the good work soldier.\n\"", rankscore));
		}

		if(rankscore >= 30 && rankscore < 40)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"^2Skill: ^7%3.2f   ^6Rank: ^3Lieutenant^7, Great aim you should be a sniper.\n\"", rankscore));
		}

		if(rankscore >= 40 && rankscore < 50)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"^2Skill: ^7%3.2f   ^6Rank: ^3Captain^7, Try just using a pistol for a greater challenge.\n\"", rankscore));
		}

		if(rankscore >= 50 && rankscore < 60)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"^2Skill: ^7%3.2f   ^6Rank: ^3Major^7, This game is too easy for you.\n\"", rankscore));
		}

		if(rankscore >= 60 && rankscore <75)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"^2Skill: ^7%3.2f   ^6Rank: ^3General^7, WOW your really OWNING these NOOBS!\n\"", rankscore));
		}

		if(rankscore >= 75)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"^2Skill: ^7%3.2f   ^6Rank: ^3SUPREME DICTATOR^7, GET A LIFE, YOU PLAY TOO MUCH!\n\"", rankscore));
		}

		else
		{
			return;
		}
	}
	return;
	*/
}

/*
============
RPM_Players
============
*/
void RPM_Players (gentity_t *ent)
{										//displays a list of clients names and id numbers
	int i;

	char *ability = "";
	char *color = "";
	char *you = "";
	char *ready = "";
	char *muted = "";///CJJ - 1.2.2005 - Show if a player is muted
	// GodOrDevil - 1.04.2005 - #ClanMember - #Rate
	char *clan = ""; 
	char *s;
	char userinfo[MAX_INFO_STRING];
	///End
	trap_SendServerCommand( ent-g_entities, va("print \"----------------------------------------------------------\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^3%s ^7- %s - ^3%21s ^7- ^1%s - %s - %s\n\"", "Id", "Name", "Loc", "Ping", "Rate", "Admin"));
	//trap_SendServerCommand( ent-g_entities, va("print \" ^3%s ^7-           %15s - ^3%s ^7- ^1%s\n\"", "Id#", "Name", "Ping", "Rate"));
	///trap_SendServerCommand( ent-g_entities, va("print \"\n ^3%s ^7-           %15s - ^3%s\n\"", "Id#", "Name", "Ping"));
	trap_SendServerCommand( ent-g_entities, va("print \"-----------------------------------------------------------\n\""));
	for ( i = 0; i < level.maxclients; i ++ )
	{	
		you = "";
		ability = ""; 
		ready = "";
		muted = "";
		clan = ""; // GodOrDevil

		if(g_entities[i].client->pers.connected == CON_DISCONNECTED )
		{
			continue;
		}	

#ifdef _CLANMEMBER
		if(g_entities[i].client->sess.clanMember) {
			clan = "^7<-[^3C^7]";
		}
#endif
		///End
		//if(g_entities[i].client->nvSess.referee ) {
		//	ability = "^7[^3R^7]";
		//}
		//else 
		//{
			switch(g_entities[i].client->nvSess.admin)
			{
			default:
				ability = "";
				break;
			case 2:
				ability = "^7[^4B^7]";
				break;
			case 3:
				ability = "^7[^6A^7]";
				break;
			case 4:
				ability = "^7[^1S^7]";
				break;
#ifdef _RECRUITER
			case 5:
				ability = "^7[^3R^7]";	
				break;
#endif

			}
	//	}
		if(g_entities[i].s.number == ent->s.number)	{
			you = "^7<-^2You";
		}
		if(g_entities[i].client->pers.ready) {
			ready = "7<-^1Ready";
		}
		if(g_entities[i].client->sess.mute)	{
			muted = "^7<-[^1M^7]";
		}
		switch ( g_entities[i].client->sess.team )
		{
			case TEAM_RED:
				color = S_COLOR_RED;
				break;

			case TEAM_BLUE:
				color = S_COLOR_BLUE;
				break;
			default:
				color = S_COLOR_WHITE;
				break;
		}
		// GodOrDevil - 1.20.2005 - Display #Rate
		trap_GetUserinfo( g_entities[i].s.number, userinfo, sizeof( userinfo ) );
		s = Info_ValueForKey( userinfo, "rate" );
		///End
		///CJJ & GodOrDevil - 1.2.2005 - Show if a player is muted and/or a #ClanMember
#ifdef _CLANMEMBER
		trap_SendServerCommand( ent-g_entities, va("print \"[^3%2d^7 - %s%-25s %s ^7- ^3%3d  ^7- %5s]%s %s %s %s %s\n\"",
			i,
			color,
			g_entities[i].client->nvSess.cleanName, 
			g_entities[i].client->sess.country,
			g_entities[i].client->ps.ping,
			s,	
			you,
			clan,	
			ability,
			muted,
			ready));
#else
		trap_SendServerCommand( ent-g_entities, va("print \"[^3%2d^7 - %s%-25s %s ^7- ^3%3d  ^7- %5s] %s %s %s %s\n\"",
			i,
			color,
			g_entities[i].client->nvSess.cleanName, 
			g_entities[i].client->sess.country,
			g_entities[i].client->ps.ping,
			s,
			ability,
			you,
			muted,
			ready));
#endif
	///End
	}
}
/* 
================
SC_Server_Info
GodOrDevil - 03.01.05 - 05:22pm
================
*/
void SC_Server_Info (gentity_t *ent)
{	
	char	*third, *ff, *vote;
#ifdef _COMPMODE 
	char	*comp;
#endif 

	//int		k = 0, ref = 0, adm = 0, clan = 0;
#ifdef _RPM_WEAPONMOD
	char	*ammo, *damage;
	switch (g_weaponModFlags.integer){
		case AMMO_MOD:
			ammo = "ENABLED ";
			damage = "DISABLED";
			break;
		case DAMAGE_MOD:
			ammo = "DISABLED";
			damage = "ENABLED ";
			break;
		case 3:
			ammo = "ENABLED ";
			damage = "ENABLED ";
			break;
		default:
			ammo = "DISABLED";
			damage = "DISABLED";
	}
#endif
	switch (g_allowThirdPerson.integer){
		case 0:
			third = "DISABLED";
			break;
		case 1:
			third = "ENABLED ";
			break;
		default:
			third = "DISABLED";
	}
		switch (g_friendlyFire.integer){
		case 0:
			ff = "DISABLED";
			break;
		case 1:
			ff = "ENABLED ";
			break;
		default:
			ff = "DISABLED";
	}
#ifdef _COMPMODE
	switch (g_compMode.integer){
		case 0:
			comp = "DISABLED";
			break;
		case 1:
			comp = "ENABLED ";
			break;
		default:
			comp = "DISABLED";
	}
#endif
	switch (g_allowVote.integer){
		case 0:
			vote = "DISABLED";
			break;
		case 1:
			vote = "ENABLED ";
			break;
		default:
			vote = "DISABLED";
	}

	
	trap_SendServerCommand( ent-g_entities, va("print \"\n----------------------------------------------------------\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^1Server Version^7: [%.1f]    -  [%s^7]\n\"", RPM_VERSION, RPM_VERSION_STRING_COLORED));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4Client Version^7: [%.1f]\n\"", ent->client->sess.rpmClient));
#ifdef _COMPMODE
	trap_SendServerCommand( ent-g_entities, va("print \"\n ^3Third Person^7: %s  -  ^3Competition Mode^7: %s\n", third, comp));
#else
	trap_SendServerCommand( ent-g_entities, va("print \"\n ^3Third Person^7: %s\n", third));
#endif
	trap_SendServerCommand( ent-g_entities, va("print \" ^3FriendlyFire^7: %s  -  ^3Voting^7:           %s\n", ff, vote));
#ifdef _RPM_WEAPONMOD
	trap_SendServerCommand( ent-g_entities, va("print \" ^3Damage Mod^7:   %s  -  ^3Ammo Mod^7:         %s\n", damage, ammo));
#endif
	trap_SendServerCommand( ent-g_entities, va("print \" ^3Game Speed^7:   %.0f       -  ^3Gravity^7:          %.0f\n", g_speed.value, g_gravity.value));
#ifdef _CLANMEMBER
//	trap_SendServerCommand( ent-g_entities, va("print \"\n ^6Admins^7: [%i] - ^3Refs^7: [%i] - ^4Clan^7: [%i] - ^2Total Clients^7 [%i]  \n\"", level.adminCount, level.refCount, level.clanCount, level.numConnectedClients - k));
#else
//	trap_SendServerCommand( ent-g_entities, va("print \"\n ^6Admins^7: [%i] - ^3Refs^7: [%i] - ^2Total Clients^7 [%i]  \n\"", level.adminCount, level.refCount, level.numConnectedClients - k));
#endif
	trap_SendServerCommand( ent-g_entities, va("print \"----------------------------------------------------------\n\""));
	if ( !ent->client->sess.rpmClient){
		trap_SendServerCommand( ent-g_entities, va("print \" You do ^1NOT ^7have %s^7! For best results ^3install ^7the ^1MOD.\n\"", RPM_VERSION_STRING_LONG));
		trap_SendServerCommand( ent-g_entities, va("print \" ^1Download ^7it at: ^3www.sof2.org\n\""));
	}
	else if ( RPM_VERSION > ent->client->sess.rpmClient){
		trap_SendServerCommand( ent-g_entities, va("print \" You are running an ^1OLDER VERSION^7! ^3UPGRADE ^7for best results.\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" ^1Download ^7it at: ^3www.sof2.org\n\""));
	}
}

// GodOrDevil - 04.23.05 - 07:33pm #NOTE - removed for ]DA[ update

/* 
================
SC_Sound
Allows any sound in the game to be played
GodOrDevil - 03.26.05 - 03:28pm #sound
================

void SC_Sound ( gentity_t *ent)
{
	char		sound[MAX_QPATH];
	char		p[120];

	////Check for mute and any Cvar restrictions
	if	(ent->client->sess.mute){
		trap_SendServerCommand( ent-g_entities, va("print \"You are currently muted by admin.\n\"") );
		return;
	}
	if (g_allowSounds.integer >= 3) {
		trap_SendServerCommand( ent-g_entities, va("print \"Sounds are ^1DISABLED ^7on this server\n\""));	
		return;
	}
	if (!ent->client->nvSess.admin && g_allowSounds.integer == 2) {
		trap_SendServerCommand( ent-g_entities, va("print \"This Feature is set in ^6Admin ^3Only ^7mode\n\""));	
		return;
	}
	if (g_allowSounds.integer == 1) {
		trap_SendServerCommand( ent-g_entities, va("print \"This Feature is set in ^1TEAM ^3Only ^7mode\nUse ^3/scteam ^7instead!\n\""));	
		return;
	}

	trap_Argv( 1, sound, sizeof( sound ) );
	trap_Argv( 2, p, sizeof( p ) );
	if ( !sound[0] ) {
		trap_SendServerCommand( ent-g_entities, va("print \"Usage: scplay <dir/soundfile> <chat text> \n^1i.e.^7 scplay sound/misc/confused/moose MOOO!!\n\""));	
		return;
	}
	//04.30.05 - 08.07pm #TEST
	if ( sound[63] ){  //return; ////keep it from crashing :)
		trap_SendServerCommand( ent-g_entities, va("print \"Sound Exceeded Max Lenght!\n\""));	
	return;
	}
	//End  - 04.30.05 - 08:08pm
	if (p[0]) Cmd_Say_f (ent, 8, qfalse);
	//RPM_GlobalSound(G_AmbientSoundSetIndex(sound));
	RPM_GlobalSound (G_SoundIndex(va("%s", sound)));
 	//RPM_GlobalSound(G_SoundIndex(sound));
}


/* 
================
SC_TeamSound
Allows any sound in the game to be played TeamOnly
GodOrDevil - 03.27.05 - 06:02pm #teamSound
================

void SC_TeamSound ( gentity_t *ent)
{
	char		sound[MAX_QPATH];
	char		p[120];
	int			j;

	////Check for mute and any Cvar restrictions
	if	(ent->client->sess.mute){
		trap_SendServerCommand( ent-g_entities, va("print \"You are currently muted by admin.\n\"") );
		return;
	}
	if (g_allowSounds.integer >= 3) {
		trap_SendServerCommand( ent-g_entities, va("print \"Sounds are ^1DISABLED ^7on this server\n\""));	
		return;
	}
	if (!ent->client->nvSess.admin && g_allowSounds.integer == 2) {
		trap_SendServerCommand( ent-g_entities, va("print \"This Feature is set in ^6Admin ^3Only ^7mode\n\""));	
		return;
	}

	trap_Argv( 1, sound, sizeof( sound ) );
	trap_Argv( 2, p, sizeof( p ) );

	if ( !sound[0] ) {
		trap_SendServerCommand( ent-g_entities, va("print \"Usage: scteam <dir/soundfile> <chat text> | i.e. play sound/misc/confused/moose MOOO!!\n\""));	
		return;
	}
	//04.30.05 - 08.08pm #TEST
	if ( sound[63] ){  //return; ////keep it from crashing :)
		trap_SendServerCommand( ent-g_entities, va("print \"Sound Exceeded Max Lenght!\n\""));	
	return;
	}
	//End  - 04.30.05 - 08:08pm
	if (p[0])	Cmd_Say_f (ent, PLAY_CHAT, qfalse);

	for (j = 0; j < level.numConnectedClients; j++)	{
		if (level.clients[level.sortedClients[j]].sess.team == ent->client->sess.team ) {
			RPM_ClientSound(&g_entities[level.sortedClients[j]], G_SoundIndex(sound));
		}
	}
}

///* 
//================
//SC_weaponCvarDisplay
// GodOrDevil - 04.23.05 - 06:44pm
//================
//*/
//void SC_weaponCvarDisplay ( gentity_t *ent )
//{
//	trap_SendServerCommand( ent-g_entities, va("print \"\n [^3New Server Cvars^7]\n\""));
//	trap_SendServerCommand( ent-g_entities, va("print \" ===================================\n\""));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^3Cvar           ^1Value  ^4Discription^7]\n\""));
//	trap_SendServerCommand( ent-g_entities, va("print \" ===================================\n\""));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_extraWeapons  ^S%i ^7- Enable/Disable Extra Weapons ]\n\"", sc_extraWeapons.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_m19pistol     ^S%i ^7- M1911A1 PISTOL               ]\n\"", sc_m19pistol.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_socom         ^S%i ^7- USSOCOM PISTOL               ]\n\"", sc_socom.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_shotgun       ^S%i ^7- M590 SHOTGUN                 ]\n\"", sc_shotgun.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_uzi           ^S%i ^7- MICRO UZI                    ]\n\"", sc_uzi.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_submachinegun ^S%i ^7- M3A1 SUBMACHINEGUN           ]\n\"", sc_submachinegun.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_AutoShotgun   ^S%i ^7- USAS-12 SHOTGUN              ]\n\"", sc_AutoShotgun.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_m4            ^S%i ^7- M4 ASSAULT RIFLE             ]\n\"", sc_m4.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_ak            ^S%i ^7- AK74 ASSAULT RIFLE           ]\n\"", sc_ak.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_sniper        ^S%i ^7- MSG90A1 SNIPER RIFLE         ]\n\"", sc_sniper.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_m60           ^S%i ^7- M60 MACHINEGUN               ]\n\"", sc_m60.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_mm1           ^S%i ^7- MM1 GRENADE LAUNCHER         ]\n\"", sc_mm1.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_rpg           ^S%i ^7- RPG7 LAUNCHER                ]\n\"", sc_rpg.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_m67           ^S%i ^7- M67 GRENADE                  ]\n\"", sc_m67.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_flashnade     ^S%i ^7- M84 GRENADE                  ]\n\"", sc_flashnade.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_firenade      ^S%i ^7- ANM14 GRENADE                ]\n\"", sc_firenade.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_smokenade     ^S%i ^7- M15 GRENADE                  ]\n\"", sc_smokenade.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_fragnade      ^S%i ^7- SMOHG92 GRENADE              ]\n\"", sc_fragnade.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_fragnade2     ^S%i ^7- F1 GRENADE                   ]\n\"", sc_fragnade2.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_fragnade3     ^S%i ^7- L2A2 GRENADE                 ]\n\"", sc_fragnade3.integer));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4sc_fragnade4     ^S%i ^7- MDN11 GRENADE                ]\n\"", sc_fragnade4.integer));
//}
/* 
================
SC_miscCvarDisplay
GodOrDevil - 04.23.05 - 07:21pm
================
*/
void SC_miscCvarDisplay ( gentity_t *ent )
{
	trap_SendServerCommand( ent-g_entities, va("print \"\n [^3New Server Cvars^7]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" =========================================\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^3Cvar                  ^1Value ^4Discription^7]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" =========================================\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_Armor                 ^S%i ^7- Allow Ammor and Goggles                 \n\"", g_Armor.integer));
#ifdef _WeaponTime
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_weapontime            ^S%i ^7- Weapons fire faster after switch        \n\"", g_weapontime.integer));
#endif
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_teamChangeDeath       ^S%i ^7- Switching Teams doesnt count as a death\n\"",			g_teamChangeDeath.integer));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_defaultPenaltyBoxTime ^S%i ^7- Pbox time used if no value is given\n\"",				g_defaultPenaltyBoxTime.integer));
	//trap_SendServerCommand( ent-g_entities, va("print \" ^4g_pBoxTeamKill          ^S%i ^7- Max times in pbox before kicked for TK\n\"",			g_pBoxTeamKill.integer));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_camperPunish         ^S%i ^7- Enable/Disable AntiCamp Feature\n\"",					g_camperPunish.integer));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_camperAllowTime      ^S%i ^7- Time allowed until anticamper punishment\n\"",			g_camperAllowTime.integer));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_camperRadius         ^S%i ^7- Anticamp 'check' area\n\"",								g_camperRadius.integer));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_camperSniper         ^S%i ^7- Snipers dont get punished for camping\n\"",				g_camperSniper.integer));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_autoUnlock            ^S%i ^7- Unlock teams when last admin disconnects\n\"",			g_autoUnlock.integer));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_autoEvenTeams         ^S%i ^7- Evens teams every X amount of seconds\n\"",				g_autoEvenTeams.integer));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_allowTie             ^S%i ^7- Allows the game to end on a tied score\n\"",			g_allowTie.integer));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_talkingGhosts        ^S%i ^7- Ghost chat can be seen by alive players\n\"",			g_talkingGhosts.integer));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_enableServerMsgs     ^S%i ^7- Auto chat messages sent by the server\n\"",				g_enableServerMsgs.integer));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_serverMsgInterval    ^S%i ^7- How often the server messages are shown\n\"",			g_serverMsgInterval.integer));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_serverMsgDelay       ^S%i ^7- Time (secs) between each server message\n\"",			g_serverMsgDelay.integer));
	//trap_SendServerCommand( ent-g_entities, va("print \" ^4g_noRanks               ^S%i ^7- All levels of admin will show as normal ^6ADMIN\n\"",	g_noRanks.integer));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_swapsides            ^S%i ^7- Swap teams at X amount of rounds in INF/ELIM \n\"",		g_swapsides.integer));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_m4scope               ^S%i ^7- Enable the m4 Scope\n\"",								g_m4scope.integer));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4g_autoVoteTimeExtend   ^S%i ^7- Auto vote to extend the timelimit at the end of the map\n\"",g_autoVoteTimeExtend.integer));
}
	
/*
==================
RPM_Display_tokens
==================
*/
void RPM_Display_Tokens ( gentity_t *ent )
{
	trap_SendServerCommand( ent-g_entities, va("print \"\n [^3Chat Tokens^7]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" =======================================================\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ** Type these tokens in your chat messages to get the effect **\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^3Token    Effect^7]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#h, #H  ^7Inserts your current numerical health with color, nocolor ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#B      ^7Inserts your current health in a nice bar format.         ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#a, #A  ^7Inserts your current numerical armor with color, nocolor  ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#b      ^7Inserts your current armor in a nice bar format.          ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#d, #D  ^7Name of the last player that hurt you.                    ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#t, #T  ^7Name of the last player that you hurt.                    ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#f, #F  ^7Name of the closest teammate.                             ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#l, #L  ^7Name of your current location.                            ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#P      ^7Name of your primary weapon (short)                       ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#p      ^7Name of your primary weapon (long)                        ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#S      ^7Name of your secondary weapon (short)                     ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#s      ^7Name of your secondary weapon (long)                      ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#I      ^7Name of your pistol (short)                               ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#i      ^7Name of your pistol (long)                                ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#G      ^7Name of your grenade (short)                              ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#g      ^7Name of your grenade (long)                               ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#e, #E  ^7Name of your equipment (Armor, Goggles, etc.. )           ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4#z, #Z  ^7Name of the last player to join the game                  ]\n\""));
	// GodOrDevil - 04.21.05 - 11:07pm #Chat shortCuts
	trap_SendServerCommand( ent-g_entities, va("print \"\n [^3Shortcut Chat Tokens^7]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" =======================================================\n\""));
	if (!ent->client->nvSess.admin){
	trap_SendServerCommand( ent-g_entities, va("print \" ^4!ca    ^7Sends a message that ^6ADMINS ^7can see (^3from non-admins^7)\n\""));
	}
	if (ent->client->nvSess.admin >= 2){
	trap_SendServerCommand( ent-g_entities, va("print \" ^4!at    ^7Sends a Global message as Admin that Everyone can see\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4!ac    ^7Sends a message that ONLY ^6ADMINS ^7can see\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4!aw    ^7Sends a message as a ^3warning\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4!ar    ^7Sends a message as a server ^3rule\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4!ai    ^7Sends a message as ^4server information\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4!an    ^7Sends a anonymous message with no prefix\n\""));
	}
	if (ent->client->nvSess.admin == 4){
	trap_SendServerCommand( ent-g_entities, va("print \" ^4!sc    ^7Sends a message that ONLY ^6S-ADMINS ^7can see\n\""));
	}
#ifdef _CLANMEMBER
	if (ent->client->sess.clanMember == 1){
	trap_SendServerCommand( ent-g_entities, va("print \" ^4!cc    ^7Sends a message that ONLY ^4Clan Members ^7can see\n\""));
	}
#endif
	if (ent->client->nvSess.referee || ent->client->nvSess.admin == 1){
	trap_SendServerCommand( ent-g_entities, va("print \" ^4!rt    ^7Sends a Global message as REFEREE that Everyone can see\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4!rc    ^7Sends a message that ONLY REFEREES can see\n\""));
	}
	trap_SendServerCommand( ent-g_entities, va("print \" ^4!n     ^7Votes NO on an active vote\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4!y     ^7Votes YES on an active vote\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4!sl    ^7Displays current scorelimit and timelimit in the chat\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4!next  ^7Displays information about the next map on the server\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^4maplist ^7Shows the servers mapcycle in the console ^3(use in the console)\n\""));

	//End  - 04.21.05 - 11:23pm
	trap_SendServerCommand( ent-g_entities, va("print \" ^3** Press PgUp and PgDn keys to scroll up and down the list in console! **\n\""));
}
/*
=======================
RPM_DisplayPlayer_Cmds
=======================
*/

void RPM_DisplayPlayer_Cmds( gentity_t *ent )
{
	trap_SendServerCommand( ent-g_entities, va("print \"\n [^3Player commands^7]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ** All commands must start with a ^3/ ^7in front of them! **\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4sounds      ^7displays a list available voice chat tokens           ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4sounds2     ^7displays another list available voice chat tokens     ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4extrasounds ^7displays the list of extra sounds on the server       ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4refresh     ^7use this instead of recon, resets your score, stats   ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4stats id#   ^7displays your stats or the stats of the player id#    ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4motd        ^7displays the servers message of the day               ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4players     ^7displays a list of all players and their status       ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4tokens      ^7displays the list of chat tokens you can use          ]\n\""));
//	trap_SendServerCommand( ent-g_entities, va("print \" [^4weapon,     ^7displays Extra Weapon Cvars and their settings        ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4misc,       ^7displays Miscellaneous Cvars and their settings       ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4info        ^7displays basic information about the server           ]\n\""));	
#ifdef _COMPMODE
	trap_SendServerCommand( ent-g_entities, va("print \" [^4ready       ^7sets your ready status for match play                 ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4roundLimits    ^7see the servers Timelimit/timeleft and scorelimit  ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4tcmd <command> ^7type ^6/tcmd ? ^7to see a list of team commands        ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4ref <command>  ^7type ^6/ref ? ^7to see a list of referee commands      ]\n\""));
#endif
	trap_SendServerCommand( ent-g_entities, va("print \" [^4adm <command>  ^7type ^6/adm ? ^7to see a list of admin commands        ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" [^4callvote       ^7type ^6/callvote ? ^7to see available vote calls       ]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^3** Press PgUp and PgDn keys to scroll up and down the list in console! **\n\""));
}

/*
====================
RPM_Display_Sounds
====================
*/
void RPM_Display_Sounds( gentity_t *ent )
{
	trap_SendServerCommand( ent-g_entities, va("print \"\n[^3RPM Sounds^7]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \"** Type @ then a number to say the message eg.. @21 **\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^41]^1reinforcement", "^413]^1Hold here", "^425]^1out of Ammo", "^437]^1Take Position"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^42]^1Advance", "^414]^1Hurry", "^426]^1He's here", "^438]^1Take out"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^43]^1Await orders", "^415]^1I'm hit", "^427]^1Over there", "^439]^1eliminated"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^44]^1Check..", "^416]^1Investigate", "^428]^1Plug him", "^440]^1There he is"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^45]^1Go check", "^417]^1Keep looking", "^429]^1Pos. reached", "^441]^1Taking alot"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^46]^1dangerous", "^418]^1Kill him", "^430]^1Secure area", "^442]^1Watch out"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^47]^1hear that", "^419]^1killed him", "^431]^1slaughtered", "^443]^1doing here?"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^48]^1disappeared", "^420]^1lost him", "^432]^1not right", "^444]^1What the.."));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^49]^1Drop weapon", "^421]^1Casualties", "^433]^1Spread out", "^445]^1What that?"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^410]^1Eliminate", "^422]^1help here", "^434]^1Surround", "^446]^1happening?"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^411]^1Get him", "^423]^1Medic!", "^435]^1Take cover", "^447]^1Who are you?"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^412]^1Got that..", "^424]^1Open Fire", "^436]^1Take him", "^448]^1not supposed to"));
	trap_SendServerCommand( ent-g_entities, va("print \"Type ^6/sounds2 ^7in console or ^6/extraSounds ^7to see the rest of the sound tokens.\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^3** Press PgUp and PgDn keys to scroll up and down the list in console! **\n\""));
}

/*
====================
RPM_Display_Sounds2
====================
*/
void RPM_Display_Sounds2( gentity_t *ent )
{
	trap_SendServerCommand( ent-g_entities, va("print \"\n[^3RPM Sounds 2^7]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \"** Type @ then a number to say the message eg.. @21 **\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^449]^1Wet pants", "^462]^1Call police", "^475]^1Company", "^488]^1Woo"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^450]^1Hey honey", "^463]^1Comin thru", "^476]^1Base secure", "^489]^1Worry girls"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^451]^1Can help you", "^464]^1I'm busy", "^477]^1Your dead", "^490]^1Cover left"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^452]^1Head off", "^465]^1Excuse me", "^478]^1Close", "^491]^1Cover right"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^453]^1Dodge bullet", "^466]^1Don't hurt", "^479]^1Get down", "^492]^1Eyes open"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^454]^1Close one", "^467]^1understand", "^480]^1Get some", "^493]^1Get moving"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^455]^1Lokin at", "^468]^1No money", "^481]^1Incoming", "^494]^1Shut up"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^456]^1You scared", "^469]^1Have guns", "^482]^1cry to mama", "^495]^1Eat lead"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^457]^1Leave alone", "^470]^1Take wallet", "^483]^1Showed them", "^496]^1Kickin ass"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^458]^1Leave be", "^471]^1Unarmed", "^484]^1Take that", "^497]^1Like that"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^459]^1What?", "^472]^1Watchout", "^485]^1That did it", "^498]^1WoOoOo!"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^460]^1Attention", "^473]^1Pairup", "^486]^1Some of this", "^499]^1Lets do it"));
	trap_SendServerCommand( ent-g_entities, va("print \"%19s  %19s  %19s %s\n\"", "^461]^1Call 911", "^474]^1Fanout", "^487]^1Yeah!", "^4100]^1Can't take more"));
	trap_SendServerCommand( ent-g_entities, va("print \"Type ^6/sounds1 ^7in console or ^6/extraSounds ^7to see the rest of the sound tokens.\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \" ^3** Press PgUp and PgDn keys to scroll up and down the list in console! **\n\""));
}

/*
================
RPM_Display_ExtraSounds
================
*/
void RPM_Display_ExtraSounds (gentity_t *ent)
{
	int	soundIndex = 100;

	trap_SendServerCommand( ent-g_entities, va("print \"\n[^3RPM Extra Sounds^7]\n\""));

	//while(*chatSounds[soundIndex].text)
	while(soundIndex < MAX_RPM_CHATS)
	{
		if(*chatSounds[soundIndex].text)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"%d ^1%s\n\"", soundIndex + 1, chatSounds[soundIndex].text));
		}
		soundIndex++;
	}

	trap_SendServerCommand( ent-g_entities, va("print \" ^3** Press PgUp and PgDn keys to scroll up and down the list in console! **\n\""));
}

#ifdef _COMPMODE

/*
================
RPM_ReadyUp
================
*/

void RPM_ReadyUp (gentity_t *ent)
{
	if(ent->client->sess.team == TEAM_SPECTATOR)
	{
		return;
	}

	if(ent->client->pers.ready)
	{
		return;
	}

	if(level.warmupTime > -1)
	{
		return;
	}

	ent->client->pers.ready = 1;

	if(ent->client->sess.team == TEAM_BLUE)
		trap_SendServerCommand(-1, va("chat -1 \"^7SERVER: ^4%s ^2is ^4ready!!\n\"", ent->client->pers.netname));
	else
		trap_SendServerCommand(-1, va("chat -1 \"^7SERVER: ^1%s ^2is ^1ready!!\n\"", ent->client->pers.netname));
	trap_SendServerCommand( ent-g_entities, va("cp \"@ \n\"")); //send a blank message to clear the readyup message
}
/*
================
RPM_ReadyAll
================
*/
void RPM_ReadyAll (void)
{
	int i;
	gentity_t *ent;

	for ( i = 0; i < level.numPlayingClients; i ++ )
	{
		ent = &g_entities[level.sortedClients[i]];
		ent->client->pers.ready = 1;
	}
}

void RPM_KickAll (void)
{
	int i;
	gentity_t *ent;

	for ( i = 0; i < level.numPlayingClients; i ++ )
	{
		ent = &g_entities[level.sortedClients[i]];
		trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%i\"\n", ent->client->ps.clientNum) );
	}
}
#endif

/*
================
RPM_Refresh
================
*/
void RPM_Refresh(gentity_t *ent)
{
	if (!G_IsClientSpectating ( ent->client ) )
	{
		ent->flags &= ~FL_GODMODE;
		ent->client->ps.stats[STAT_HEALTH] = ent->health = -999;
		player_die (ent, ent, ent, 100000, MOD_REFRESH, HL_NONE, vec3_origin );
	}
	
	memset(&ent->client->pers.statinfo, 0, sizeof(ent->client->pers.statinfo));
	//ent->client->pers.enterTime = level.time; // NOPE, no thanks... not with database stats!
	ent->client->ps.persistant[PERS_SCORE] = 0;
	ent->client->sess.score = 0;
	ent->client->sess.deaths = 0;
	ent->client->sess.kills = 0;
#ifdef _DB_ENABLED
	ent->client->pers.statinfo.refresh++;
#endif
	//ent->client->sess.teamkillDamage      = 0;
	//ent->client->sess.teamkillForgiveTime = 0;

	// GodOrDevil
	ent->client->pers.rank = 0;
	// End

#ifdef _120314_2
	ent->client->nvSess.earnedWeapons = 0;
#endif

}

/*
================
RPM_Display_RoundLimits
================
*/
void RPM_Display_RoundLimits (gentity_t *ent)
{
	int	minsLeft, secsLeft, timeLeft;

	trap_SendServerCommand( ent-g_entities, va("print \"\n[^3Round Limits^7]\n\""));
	trap_SendServerCommand( ent-g_entities, va("print \"=======================================================\n\""));

	if(!g_timelimit.integer)
	{
		trap_SendServerCommand( ent-g_entities, va("print \"^3No Timelimit on this server.\n\""));
	}
	else
	{
		timeLeft = ((g_timelimit.integer + level.timeExtension) * 60000) - (level.time - level.startTime); 
		minsLeft = timeLeft / 60000;
		secsLeft = (timeLeft % 60000) / 1000;
		trap_SendServerCommand( ent-g_entities, va("print \"^5Timelimit is: %d\n^3There is %d minutes and %d seconds ^7left in this round.\n\"", g_timelimit.integer, minsLeft, secsLeft));
	}

	if(!g_scorelimit.integer)
	{
		trap_SendServerCommand( ent-g_entities, va("print \"\n^3No Scorelimit on this server.\n\""));
	}
	else
	{
		// GodOrDevil - 10.29.05 - 06:00am #Team Names
		trap_SendServerCommand( ent-g_entities, va("print \"\n^5Scorelimit is: %d\n^4Blue team needs %d ^7and ^1Red team needs %d ^7points to win this round.\n\"", g_scorelimit.integer, g_scorelimit.integer - level.teamScores[TEAM_BLUE], g_scorelimit.integer - level.teamScores[TEAM_RED]));
		//trap_SendServerCommand( ent-g_entities, va("print \"\n^5Scorelimit is: %d\n^4GREEN team needs %d ^7and ^1PURPLE team needs %d ^7points to win this round.\n\"", g_scorelimit.integer, g_scorelimit.integer - level.teamScores[TEAM_BLUE], g_scorelimit.integer - level.teamScores[TEAM_RED]));
	}
}

