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

/*
==================
RPM_Obituary
==================
*/
void RPM_Obituary ( gentity_t *target, gentity_t *attacker, int mod, attackType_t attack, int hitLocation ) 
{
	int				targ, killer, gender, attackt, weapon;
	char			targetName[64];
	char			killerName[64];
	const char		*targetColor;
	const char		*killerColor;
	char			*message;
	char			*message2;
	char			*message3;
	qboolean		headShot = qfalse;
	qboolean		statOk = qfalse;
	statinfo_t		*atrstat = &attacker->client->pers.statinfo;

	killerColor = S_COLOR_WHITE;
	targetColor   = S_COLOR_WHITE;
	message2 = "";
	message3 = "";
	// GodOrDevil - 01.08.06 - 09:14pm
	///attackt = attacker->client->pers.statinfo.attack;
	///weapon = attacker->client->pers.statinfo.weapon;

#ifdef _TORTURE
	if(target->client->nvSess.drop)
		return;
#endif
	///End  - 01.08.06 - 09:14pm

	if(!level.gametypeData->teams || (level.gametypeData->teams && !OnSameTeam ( target, attacker )))
	{
		statOk = qtrue;
	}

	//was the kill hit a HEADSHOT?
	hitLocation = hitLocation & (~HL_DISMEMBERBIT);
	if (hitLocation == HL_HEAD && statOk)
	{
		// GodOrDevil - 01.08.06 - 09:14pm
		attackt = attacker->client->pers.statinfo.attack;
		weapon = attacker->client->pers.statinfo.weapon;
		///End  - 01.08.06 - 09:14pm
		headShot = qtrue;
		//add to the total headshot count for this player
		atrstat->headShotKills++;
		atrstat->weapon_headshots[attackt][weapon]++;
	}

	targ = target->s.number;

	if ( targ< 0 || targ >= MAX_CLIENTS ) 
	{
		Com_Error( ERR_FATAL, "RPM_Obituary: target out of range" );
	}

	Q_strncpyz( targetName, target->client->pers.netname, sizeof(targetName));
	strcat( targetName, S_COLOR_WHITE );

	//find out who or what killed the client
	if ( attacker->client ) 
	{
		killer = attacker->s.number;
		Q_strncpyz( killerName, attacker->client->pers.netname, sizeof(killerName));
		strcat( killerName, S_COLOR_WHITE );
	}	
	else
	{
		killer = ENTITYNUM_WORLD;
	}
	
	// Play the death sound, water if they drowned
	if ( mod == MOD_WATER )
	{
		RPM_ClientSound(target, G_SoundIndex("sound/pain_death/mullins/drown_dead.mp3"));
	}
	else		//play a random death sound out of 3 possible sounds
	{
		switch(level.time % 3)
		{
		case 0:
			RPM_ClientSound(target, G_SoundIndex("sound/pain_death/male/die01.mp3"));
			break;

		case 1:
			RPM_ClientSound(target, G_SoundIndex("sound/pain_death/male/die04.mp3"));
			break;

		case 2:
			RPM_ClientSound(target, G_SoundIndex("sound/pain_death/male/die05.mp3"));
			break;

		default:
			RPM_ClientSound(target, G_SoundIndex("sound/pain_death/male/die01.mp3"));
			break;
		}
	}

	// Play the frag sound, and make sure its not played more than every 250ms
	if ( attacker->client && level.time - attacker->client->lastKillTime > 250 )
	{
		//If the attacker killed themselves play the selffrag sound
		if ( killer == targ )
		{
			RPM_ClientSound(target, G_SoundIndex("sound/self_frag.mp3"));
		}
		//Or if they killed a teammate
		else if(level.gametypeData->teams && OnSameTeam ( target, attacker ))
		{
			RPM_ClientSound(attacker, G_SoundIndex("sound/self_frag.mp3"));
		
#ifdef _STUPID_SHIT
			if(g_showTkMessage.integer)
			{
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,%s ^1killed ^7a ^3teamate!", level.time + 3000, killerName ) );
			}
#endif
		}
		else if(headShot)
		{
			//if they use rpm client-side the client will 
			//handle the sound etc...
			if(attacker->client->sess.rpmClient)
			{
				trap_SendServerCommand( attacker->s.number, va("headshot \"%s\n\"", g_headShotMessage.string));
			}
			//if not we'll send them the sound etc..
			else if(g_allowDeathMessages.integer)
			{
				trap_SendServerCommand( attacker->s.number, va("cp \"%s\n\"", g_headShotMessage.string));
				RPM_ClientSound(attacker, G_SoundIndex("sound/npc/col8/blakely/niceshot.mp3"));
			}
			
			if(g_allowDeathMessages.integer)
			{
				//if we can show kills we'll display the 
				//heashot message with the "you killed" message
				if ( level.gametypeData->showKills )
				{	
					message2 = va("%s\n", g_headShotMessage.string );
				}

				//We'll tack this on to the end of the kill broadcast to all
				message3 = "{^3HeaDShoT^7}";
			}
			
		}
		//if not headshot, suicide or tk just play the normal sound
		else
		{	
			RPM_ClientSound(attacker, G_SoundIndex("sound/frag.mp3"));
		}
		
		//set the time here now 
		attacker->client->lastKillTime = level.time;
	}	
	
	//set the teamcolor of the killed client
	switch ( target->client->sess.team )
	{
		case TEAM_RED:
			targetColor = S_COLOR_RED;
			break;

		case TEAM_BLUE:
			targetColor = S_COLOR_BLUE;
			break;
	}
	
	gender = GENDER_MALE;
	//is the client female?, check the name of the model they're using
	if ( strstr ( target->client->pers.identity->mCharacter->mModel, "female" ) )
	{
		gender = GENDER_FEMALE;
	}

	switch( mod ) 
	{
		case MOD_SUICIDE:
			message = "suicides";
			break;
		case MOD_FALLING:
			if ( gender == GENDER_FEMALE )
				message = "fell to her death";
			else
				message = "fell to his death";
			break;
		case MOD_CRUSH:
			message = "was squished";
			break;
		case MOD_WATER:
			message = "sank like a rock";
			break;
		case MOD_TARGET_LASER:
			message = "saw the light";
			break;
		case MOD_TRIGGER_HURT:
			message = "was in the wrong place";
			break;
		case MOD_TEAMCHANGE:
			return;
		case MOD_CAR:
			message = "was killed in a terrible car accident";
			break;
		case MOD_POP:
			message = "was popped";
			break;
// GodOrDevil - 1.13.2005	- Dugup (Unplant) #MOD
		case MOD_DUGUP:
			message = "didnt make it out alive";
			break;
		case MOD_BURN:
			message = "was burnt to a Crisp!";
			break;
#ifdef _NV_ADMIN
		case MOD_ADMIN:
			return;
#endif
//End
		default:
			message = NULL;
			break;
	}

	// Attacker killed themselves.  Ridicule them for it.
	if (killer == targ) 
	{
		switch (mod) 
		{
			case MOD_MM1_GRENADE_LAUNCHER:    
			case MOD_RPG7_LAUNCHER:           
			case MOD_M67_GRENADE:            
			case MOD_M84_GRENADE:
			case MOD_F1_GRENADE:
			case MOD_L2A2_GRENADE:
			case MOD_MDN11_GRENADE:
			case MOD_SMOHG92_GRENADE:
			case MOD_ANM14_GRENADE:
			case MOD_M15_GRENADE:
				if ( gender == GENDER_FEMALE )
					message = "blew herself up";
				else if ( gender == GENDER_NEUTER )
					message = "blew itself up";
				else
					message = "blew himself up";
				break;

			case MOD_REFRESH:
				message = "Refreshed";
				break;

			case MOD_SPAWNKILL:
				message = "^3was punished for spawn killing";
				break;

			default:
				if ( gender == GENDER_FEMALE )
					message = "killed herself";
				else if ( gender == GENDER_NEUTER )
					message = "killed itself";
				else
					message = "killed himself";
				break;
		}
	}

	if (message) 
	{
		trap_SendServerCommand( -1, va("print \"%s%s %s.\n\"", targetColor, targetName, message));
		return;
	}

	// check for kill messages
	if ( level.gametypeData->showKills )
	{
		if ( attacker && attacker->client ) 
		{
			//if not a team game display the kill and the rank
			if ( !level.gametypeData->teams ) 
			{
				trap_SendServerCommand( attacker->s.number, va("cp \"%sYou killed %s%s\n%s place with %i\n\"",
				message2,
				targetColor,
				targetName, 
				G_PlaceString( attacker->client->ps.persistant[PERS_RANK] + 1 ),
				attacker->client->ps.persistant[PERS_SCORE] ));
			} 
			else //if team just display the kill
			{
				trap_SendServerCommand( attacker->s.number, va("cp \"%sYou killed %s%s\n\"", message2, targetColor, targetName ));
			}
		}
	}

	// check for double client messages
	message2 = "";

	// GodOrDevil - 01.08.06 - 09:22pm
		//switch ( attacker->client->sess.team )
		//{
		//	case TEAM_RED:
		//		killerColor = S_COLOR_RED;
		//		break;

		//	case TEAM_BLUE:
		//		killerColor = S_COLOR_BLUE;
		//		break;
		//}
	///End  - 01.08.06 - 09:31pm
			
	if (killer != ENTITYNUM_WORLD)
	{
		// GodOrDevil - 01.08.06 - 09:31pm
		if (attacker && attacker->client)
		{
			switch (attacker->client->sess.team)
			{
				case TEAM_RED:
					killerColor = S_COLOR_RED;
					break;

				case TEAM_BLUE:
					killerColor = S_COLOR_BLUE;
					break;
			}
		}
		///End  - 01.08.06 - 09:31pm

		switch (mod) 
		{
			case MOD_KNIFE:
				message = "was sliced by";
				if(statOk)
				{
					atrstat->knifeKills++;
				}
				break;

			case MOD_USAS_12_SHOTGUN:
			case MOD_M590_SHOTGUN:
				if ( attack == ATTACK_ALTERNATE )
				{
					message = "was bludgeoned by";
					message2 = va("'s %s", weaponParseInfo[mod].mName );
				}
				else
				{
					message = "was pumped full of lead by";
					message2 = va("'s %s", weaponParseInfo[mod].mName );
				}
				break;

			case MOD_M1911A1_PISTOL:
			case MOD_USSOCOM_PISTOL: 
				if ( attack == ATTACK_ALTERNATE )
				{
					message = "was pistol whipped by";
					message2 = va("'s %s", weaponParseInfo[mod].mName );
				}
				else
				{
					message = "was shot by";
					message2 = va("'s %s", weaponParseInfo[mod].mName );
				}
				break;

			case MOD_AK74_ASSAULT_RIFLE:
				if ( attack == ATTACK_ALTERNATE )
				{
					message = "was stabbed by";
					message2 = va("'s %s", weaponParseInfo[mod].mName );
				}
				else
				{
					message = "was shot by";
					message2 = va("'s %s", weaponParseInfo[mod].mName );
				}
				break;

			case MOD_M4_ASSAULT_RIFLE:
				if ( attack == ATTACK_ALTERNATE )
				{
					message = "was detonated by";
					message2 = "'s M203";

					if(statOk)
					{
						atrstat->explosiveKills++;
					}
				}
				else if(g_m4scope.integer && (attacker->client->ps.pm_flags & PMF_ZOOMED))
				{
					message = "was scoped by";
					message2 = va("'s %s", weaponParseInfo[mod].mName );
#ifdef _DB_ENABLED
					//atrstat->m4scoped++;
					//atrstat->wpnKills[MOD_M4_ASSAULT_RIFLE][ATTACK_ALTERNATE]++;
#endif
				}
				else
				{
					message = "was shot by";
					message2 = va("'s %s", weaponParseInfo[mod].mName );
				}
				break;

			case MOD_M60_MACHINEGUN:
			case MOD_MICRO_UZI_SUBMACHINEGUN:
			case MOD_M3A1_SUBMACHINEGUN:
				message = "was shot by";
				message2 = va("'s %s", weaponParseInfo[mod].mName );
				break;

			case MOD_MSG90A1_SNIPER_RIFLE:    
				message = "was sniped by";
				message2 = va("'s %s", weaponParseInfo[mod].mName );
				break;

			case MOD_MM1_GRENADE_LAUNCHER:    
			case MOD_RPG7_LAUNCHER:           
			case MOD_M67_GRENADE:            
			case MOD_M84_GRENADE:
			case MOD_F1_GRENADE:
			case MOD_L2A2_GRENADE:
			case MOD_MDN11_GRENADE:
			case MOD_SMOHG92_GRENADE:
			case MOD_ANM14_GRENADE:
			case MOD_M15_GRENADE:
				message = "was detonated by";
				message2 = va("'s %s", weaponParseInfo[mod].mName );
				if(statOk)
				{
					if(mod == MOD_ANM14_GRENADE)
					{
						atrstat->hitcount++;
						atrstat->accuracy = (float)atrstat->hitcount / (float)atrstat->shotcount * 100;
						atrstat->weapon_hits[ATTACK_NORMAL][mod]++;
					}

					atrstat->explosiveKills++;
				}
				break;

			case MOD_TELEFRAG:
				message = "tried to invade";
				message2 = "'s personal space";
				break;

			default:
				message = "was killed by";
				break;
		}

		if (message) {
			trap_SendServerCommand( -1, va("print \"%s%s %s %s%s%s %s\n\"", targetColor, targetName, message, killerColor, killerName, message2, message3));
			return;
		}
	}

	// we don't know what it was
	trap_SendServerCommand( -1, va("print \"%s%s died.\n\"", targetColor, targetName ));
}

/*
===================
G_PlaceString	//stole code from CG_Placestring
===================
*/
const char	*G_PlaceString( int rank ) {
	static char	str[64];
	char	*s, *t;

	if ( rank & RANK_TIED_FLAG ) {
		rank &= ~RANK_TIED_FLAG;
		t = "Tied for ";
	} else {
		t = "";
	}

	if ( rank == 1 ) {
		s = S_COLOR_BLUE "1st" S_COLOR_WHITE;		// draw in blue
	} else if ( rank == 2 ) {
		s = S_COLOR_RED "2nd" S_COLOR_WHITE;		// draw in red
	} else if ( rank == 3 ) {
		s = S_COLOR_YELLOW "3rd" S_COLOR_WHITE;		// draw in yellow
	} else if ( rank == 11 ) {
		s = "11th";
	} else if ( rank == 12 ) {
		s = "12th";
	} else if ( rank == 13 ) {
		s = "13th";
	} else if ( rank % 10 == 1 ) {
		s = va("%ist", rank);
	} else if ( rank % 10 == 2 ) {
		s = va("%ind", rank);
	} else if ( rank % 10 == 3 ) {
		s = va("%ird", rank);
	} else {
		s = va("%ith", rank);
	}

	Com_sprintf( str, sizeof( str ), "%s%s", t, s );
	return str;
}

