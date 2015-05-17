// Copyright (C) 2001-2002 Raven Software.
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
// g_combat.c

#include "g_local.h"

#ifdef _SOF2_BOTS
void BotDamageNotification	( gclient_t *bot, gentity_t *attacker );
#endif
/*
============
G_AddScore

Adds score to both the client and his team
============
*/
void G_AddScore( gentity_t *ent, int score )
{
	if ( !ent->client )
	{
		return;
	}

	// no scoring during pre-match warmup
	if ( level.warmupTime )
	{
		return;
	}

	ent->client->sess.score += score;
	ent->client->ps.persistant[PERS_SCORE] = ent->client->sess.score;

	CalculateRanks();
}

/*
=================
TossClientItems

Toss the weapon and custom gametype items for the killed player
=================
*/
void TossClientItems( gentity_t *self )
{
	gitem_t		*item;
	int			weapon;
	float		angle;
	int			i;
	gentity_t	*drop;

	// drop the weapon if not a gauntlet or machinegun
	weapon = self->s.weapon;

	// make a special check to see if they are changing to a new
	// weapon that isn't the mg or gauntlet.  Without this, a client
	// can pick up a weapon, be killed, and not drop the weapon because
	// their weapon change hasn't completed yet and they are still holding the MG.
	if ( self->client->ps.weaponstate == WEAPON_DROPPING )
	{
		weapon = self->client->pers.cmd.weapon;
	}

	if ( !( self->client->ps.stats[STAT_WEAPONS] & ( 1 << weapon ) ) )
	{
		weapon = WP_NONE;
	}

	// If we have a valid weapon to drop and it has ammo then drop it
	if ( weapon > WP_KNIFE && weapon < WP_NUM_WEAPONS &&
		 (self->client->ps.ammo[ weaponData[weapon].attack[ATTACK_NORMAL].ammoIndex ] + self->client->ps.clip[weapon]) )
	{
		G_DropWeapon ( self, weapon, 0 );
	}

	// drop all custom gametype items
	angle = 45;
	for ( i = 0 ; i < MAX_GAMETYPE_ITEMS ; i++ )
	{
		// skip this gametype item if the client doenst have it
		if ( !(self->client->ps.stats[STAT_GAMETYPE_ITEMS] & (1<<i)) )
		{
			continue;
		}

		item = BG_FindGametypeItem ( i );
		if ( !item )
		{
			continue;
		}

		drop = G_DropItem( self, item, angle );
		drop->count = 1;
		angle += 45;

		// TAke it away from the client just in case
		self->client->ps.stats[STAT_GAMETYPE_ITEMS] &= ~(1<<i);

		if ( self->enemy && self->enemy->client && !OnSameTeam ( self->enemy, self ) )
		{
			trap_GT_SendEvent ( GTEV_ITEM_DEFEND, level.time, item->quantity, self->enemy->s.clientNum, self->enemy->client->sess.team, 0, 0  );
		}
	}

	self->client->ps.stats[STAT_GAMETYPE_ITEMS] = 0;
}

/*
==================
LookAtKiller
==================
*/
void LookAtKiller( gentity_t *self, gentity_t *inflictor, gentity_t *attacker ) {
	vec3_t		dir;
	vec3_t		angles;

	if ( attacker && attacker != self ) {
		VectorSubtract (attacker->s.pos.trBase, self->s.pos.trBase, dir);
	} else if ( inflictor && inflictor != self ) {
		VectorSubtract (inflictor->s.pos.trBase, self->s.pos.trBase, dir);
	} else {
		self->client->ps.stats[STAT_DEAD_YAW] = self->s.angles[YAW];
		return;
	}

	self->client->ps.stats[STAT_DEAD_YAW] = vectoyaw ( dir );

	angles[YAW] = vectoyaw ( dir );
	angles[PITCH] = 0;
	angles[ROLL] = 0;
}

/*
==================
body_die
==================
*/

// GodOrDevil - GAMETYPES
/*
void body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath, int hitLocation, vec3_t hitDir )
{
	return;
}
*/

void body_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath, int hitLocation, vec3_t hitDir ) 
{
	// GodOrDevil #TFT - unfreeze if the corpse is hit
	if(!NV_isTFT() && !NV_isFT())
		return;
	if(attacker==NULL || !attacker->client)
		return;
	if(attacker == self)
		return;
	
	if (self && self->classname != NULL && !strcmp(self->classname, "nv_model"))
	{	
		if((((meansOfDeath & 0xFF) == MOD_KNIFE) || ((meansOfDeath & 0xFF) == MOD_AK74_ASSAULT_RIFLE)) && NV_isFT())
		{
			if (&g_entities[self->r.ownerNum] && &g_entities[self->r.ownerNum].client && g_entities[self->r.ownerNum].client->nvSess.frozenTime >= level.time + 2000)
			{
				NV_playerDropRandomItem(&g_entities[self->r.ownerNum], attacker, qtrue);
				g_entities[self->r.ownerNum].client->nvSess.frozenTime = level.time + 2000;
				g_entities[self->r.ownerNum].client->invulnerableTime = level.time + 2500;
				g_entities[self->r.ownerNum].client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
				g_entities[self->r.ownerNum].client->pers.weapons = g_entities[self->r.ownerNum].client->ps.stats[STAT_WEAPONS];
				g_entities[self->r.ownerNum].client->pers.currentWeapon = WP_KNIFE;
				g_entities[self->r.ownerNum].client->ps.respawnTimer = level.time + 1000;
				g_entities[self->r.ownerNum].client->ps.weaponstate = 1;
				g_entities[self->r.ownerNum].client->ps.weaponTime = 0;
			}
			return;
		}
		if(g_entities[self->r.ownerNum].client->sess.team == attacker->client->sess.team)
		{
			NV_unFreeze( &g_entities[self->r.ownerNum], attacker, meansOfDeath);
			//NV_unFreeze( &g_entities[self->r.ownerNum], attacker, (meansOfDeath >> 8) & 0xFF);
			return;
		}

	}
	return;
}
// END


// these are just for logging, the client prints its own messages
char *modNames[] =
{
	"MOD_UNKNOWN",

	"MOD_KNIFE",

	"MOD_M1911A1_PISTOL",
	"MOD_US_SOCOM_PISTOL",

	"MOD_M590_SHOTGUN",
	"MOD_MICRO_UZI_SUBMACHINEGUN",
	"MOD_M3A1_SUBMACHINEGUN",

	"MOD_USAS_12_SHOTGUN",
	"MOD_M4_ASSAULT_RIFLE",
	"MOD_AK74_ASSAULT_RIFLE",
	"MOD_MSG90A1_SNIPER_RIFLE",
	"MOD_M60_MACHINEGUN",
	"MOD_MM1_GRENADE_LAUNCHER",
	"MOD_RPG7_LAUNCHER",

	"MOD_M67_GRENADE",
	"MOD_M84_GRENADE",
	"MOD_F1_GRENADE",
	"MOD_L2A2_GRENADE",
	"MOD_MDN11_GRENADE",
	"MOD_SMOHG92_GRENADE",
	"MOD_ANM14_GRENADE",
	"MOD_M15_GRENADE",

	"MOD_WATER",
	"MOD_CRUSH",
	"MOD_TELEFRAG",
	"MOD_FALLING",
	"MOD_SUICIDE",
	"MOD_TEAMCHANGE",
	"MOD_TARGET_LASER",
	"MOD_TRIGGER_HURT",
	//Ryan
	"MOD_CAR",
	"MOD_POP",
	"MOD_REFRESH",
	//Ryan

	// GodOrDevil #MOD
	"MOD_DUGUP",
	"MOD_BURN",
	"MOD_ADMIN",
	"MOD_SPAWNKILL"
	//End
};

/*
==================
player_die
==================
*/
void player_die(
	gentity_t	*self,
	gentity_t	*inflictor,
	gentity_t	*attacker,
	int			damage,
	int			mod,
	int			hitLocation,
	vec3_t		hitDir
	)
{
	int				anim;
	int				contents;
	int				killer;
	int				i;
	char			*killerName, *obit;
	attackType_t	attack;
	int				meansOfDeath;

#ifdef _DB_ENABLED
	int weap = 0;
#endif

	attack		 = (attackType_t)((mod >> 8) & 0xFF);
	meansOfDeath = mod & 0xFF;
	
#ifdef _DB_ENABLED
	weap = meansOfDeath;
#endif

	if ( self->client->ps.pm_type == PM_DEAD )
	{
		return;
	}

	if ( level.intermissiontime)
	{
		return;
	}

	// Let the gametype know about the player death so it can adjust anything
	// it needs to adjust
	// GodOrDevil - VIP : if player is a vip and dies, mark them as being vip and send a notification to the gt
	if(self->client->nvSess.vip == 1 && NV_isVIP())
	{
		G_DropGametypeItems(self, 0);
	}
	// END
	if ( attacker && attacker->client )
	{
		trap_GT_SendEvent ( GTEV_CLIENT_DEATH, level.time, self->s.number, self->client->sess.team, attacker->s.number, attacker->client->sess.team, 0 );
	}
	else
	{
		trap_GT_SendEvent ( GTEV_CLIENT_DEATH, level.time, self->s.number, self->client->sess.team, -1, -1, 0 );
	}
	// Add to the number of deaths for this player

	//Ryan March 30 2004 5:28pm
	if(g_showKillStreak.integer && attacker != self && attacker->client && self->client->pers.statinfo.killsinarow >= 3)
	{
		trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, ^3%s's ^1Killing Spree ^7was ended by ^3%s",
				level.time + 5000,
				self->client->pers.netname,
				attacker->client->pers.netname));
	}

	self->client->pers.statinfo.deaths++;
	self->client->pers.statinfo.killsinarow = 0;
	// Ryan
	self->client->sess.deaths++;

	//Ryan april 22 2003
	//If they are planted unplant them before we kill them
	if(self->client->pers.planted)
	{
		self->client->ps.origin[2] += 65;
		VectorCopy( self->client->ps.origin, self->s.origin );
		self->client->pers.planted = qfalse;
	}
	//Ryan

	// This is just to ensure that the player wont render for even a single frame
	self->s.eFlags |= EF_DEAD;

	self->client->ps.pm_type = PM_DEAD;

	if ( attacker )
	{
		killer = attacker->s.number;
		if ( attacker->client )
		{
			killerName = attacker->client->pers.netname;
		}
		else
		{
			killerName = "<non-client>";
		}
	}
	else
	{
		killer = ENTITYNUM_WORLD;
		killerName = "<world>";
	}

	if ( killer < 0 || killer >= MAX_CLIENTS )
	{
		killer = ENTITYNUM_WORLD;
		killerName = "<world>";
	}

	if ( meansOfDeath < 0 || meansOfDeath >= sizeof( modNames ) / sizeof( modNames[0] ) )
	{
		obit = "<bad obituary>";
	}
	else
	{
		if ( attack == ATTACK_ALTERNATE )
		{
			obit = va ( "%s_ALT", modNames[ meansOfDeath ] );
		}
		else
		{
			obit = modNames[ meansOfDeath ];
		}
	}

	// If the weapon was charging then drop it with no forward velocity
	if ( self->client->ps.grenadeTimer )
	{
		gentity_t* missile;
		missile = G_FireWeapon( self, ATTACK_NORMAL );
		if ( missile )
		{
			VectorClear ( missile->s.pos.trDelta );
		}
	}

	G_LogPrintf("Kill: %i %i %i: %s killed %s by %s\n",
		killer, self->s.number, meansOfDeath, killerName,
		self->client->pers.netname, obit );

	// broadcast the death event to everyone
	//Ryan april 18 2003
	//Don't let the client print the obituary messages anymore. This will now be handled
	//by the server so we can change them.	
	if(g_disableRPMObituary.integer && !(hitLocation & HL_HEAD)) // GodOrDevil - added cvar to toggle to reduce any extra lag.
	{	
		gentity_t		*ent;
		ent = G_TempEntity( self->r.currentOrigin, EV_OBITUARY );
		ent->s.eventParm = mod;
		ent->s.otherEntityNum = self->s.number;
		ent->s.otherEntityNum2 = killer;
		ent->r.svFlags = SVF_BROADCAST;	// send to everyone
	}
	//Ryan

	self->enemy = attacker;

	if (attacker && attacker->client)
	{
		attacker->client->lastkilled_client = self->s.number;

#ifdef _DB_ENABLED
		if(self->client->ps.weapon && mod != MOD_TEAMCHANGE)
		{
			//self->client->pers.statinfo.wpnDeaths[attacker->client->ps.weapon]++;
			if(self->client->ps.weapon >= WP_KNIFE && self->client->ps.weapon < WP_NUM_WEAPONS)
				self->client->pers.statinfo.wpnDeaths[self->client->ps.weapon]++;
		}
#endif
		if ( attacker == self )
		{
			if ( mod != MOD_TEAMCHANGE )
			{
#ifdef _DB_ENABLED
				attacker->client->pers.statinfo.suicides++;
#endif
				G_AddScore( attacker, g_suicidePenalty.integer );
				// GodOrDevil
				if(mod == MOD_SPAWNKILL)
				{
					attacker->client->nvSess.spawnKillLast = 30;
					attacker->client->nvSess.spawnKillCount++;
					trap_SendServerCommand( attacker-g_entities, "cp \"\n^6**^3**^1No Spawn Killing^3**^6**\n^3Play nice or you will be punished!\n\"" );
#ifdef _DB_ENABLED
					attacker->client->pers.statinfo.spawnKills++;
#endif
				}
				// End
			}
		}
		else if ( OnSameTeam ( self, attacker ) )
		{
			if ( mod != MOD_TELEFRAG )
			{	
#ifdef _KILLSPREE						
				if(attacker->client->pers.statinfo.killsinarow >= 3)
					NV_checkKillSpree(attacker, self, 1);
				else 
					attacker->client->pers.statinfo.killsinarow = 0;
#endif // _KILLSPREE
				G_AddScore( attacker, g_teamkillPenalty.integer );
			}
		}
		else if ( mod != MOD_TELEFRAG )
		{
			G_AddScore( attacker, 1 );

			/// GodOrDevil #FT gets a percent of the total score the victim has.  - NOT SURE ABOUT USING THIS.. might be too much??
			//if(self->client->sess.score >= 4 && NV_isFT())
			//{
			//	G_AddScore( attacker, (self->client->sess.score * .20f) );
			//}
			//// END

			attacker->client->sess.kills++;

			// GodOrDevil - 03.10.06 - 11:43pm #killingSpree
			if(!level.firstBlood)
			{
				level.firstBlood = qtrue; 
				GT(va("%i, %s ^7has drawn first ^_blood!",level.time + 5000, attacker->client->pers.netname));
				///NV_Sound(attacker-g_entities, "sound/nv/firstblood.mp3" );
			}
		
#ifdef _DB_ENABLED	
			if(self && self->client && attacker != self && self->client->ps.stats[STAT_GAMETYPE_ITEMS])
			{
				attacker->client->pers.statinfo.itemDefs++;
			}
			
			if(weap > 0 && weap < WP_NUM_WEAPONS)
			{	
				switch ( hitLocation & (~HL_DISMEMBERBIT) )
				{
					default:
						break;

					case HL_HEAD:
						attacker->client->pers.statinfo.weapon_killLoc[weap][0]++;
						break;
					case HL_NECK:
						attacker->client->pers.statinfo.weapon_killLoc[weap][1]++;
						break;
					case HL_CHEST:
					case HL_CHEST_RT:
					case HL_CHEST_LT:
					case HL_BACK_LT:
					case HL_BACK_RT:
					case HL_BACK:
						attacker->client->pers.statinfo.weapon_killLoc[weap][2]++;
						break;
					case HL_ARM_LT:
					case HL_ARM_RT:
						attacker->client->pers.statinfo.weapon_killLoc[weap][3]++;
						break;
					case HL_HAND_LT:
					case HL_HAND_RT:
						attacker->client->pers.statinfo.weapon_killLoc[weap][4]++;
						break;						
					case HL_WAIST:
						attacker->client->pers.statinfo.weapon_killLoc[weap][5]++;
						break;
					case HL_LEG_UPPER_LT:
					case HL_LEG_LOWER_LT:
					case HL_LEG_UPPER_RT:
					case HL_LEG_LOWER_RT:
						attacker->client->pers.statinfo.weapon_killLoc[weap][6]++;
						break;
					case HL_FOOT_LT:
					case HL_FOOT_RT:
						attacker->client->pers.statinfo.weapon_killLoc[weap][7]++;
						break;				
				}
			}
#endif
			//Ryan april 22 2003
			//add the kill to our stats and display messages if necessary
			attacker->client->pers.statinfo.kills++;
			attacker->client->pers.statinfo.killsinarow++;

			// GodOrDevil
			if(NV_isGG())
			{		
				if(attacker->client->pers.statinfo.killsinarow >= 3 && attacker->client->pers.statinfo.killsinarow < 5)
				{
					trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, %s is on ^1FIRE ^7with ^3%d ^7Kills in a Row!",
					level.time + 5000,
					attacker->client->pers.netname,
					attacker->client->pers.statinfo.killsinarow) );
				}
				else if(attacker->client->pers.statinfo.killsinarow >= 5 && level.gametypeData->teams)
				{
					trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, %s is ^3OWNING^7 the %s ^7team with ^3%d ^7Kills in a Row!",
					level.time + 5000,
					attacker->client->pers.netname,
					// GodOrDevil - 10.29.05 - 06:00am #Team Names
					attacker->client->sess.team == TEAM_BLUE ? "^1RED" : "^4BLUE",
					//attacker->client->sess.team == TEAM_BLUE ? "^1Purple" : "^4Green",
					attacker->client->pers.statinfo.killsinarow) );
				}
				else if(attacker->client->pers.statinfo.killsinarow >= 5 && !level.gametypeData->teams)
				{
					trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, %s is ^3OWNING ^7you All with ^3%d ^7Kills in a Row!",
					level.time + 5000,
					attacker->client->pers.netname,
					attacker->client->pers.statinfo.killsinarow) );
				}
			}
			else
			// End
#ifdef _KILLSPREE
			if(self->client->pers.statinfo.killsinarow >= 3 && g_showKillStreak.integer >= 2)
				NV_checkKillSpree(attacker, self, 2);
			else if(!NV_checkKillSpree(attacker, self, 0) && g_showKillStreak.integer)
#endif // _KILLSPREE
			{
				if(attacker->client->pers.statinfo.killsinarow >= 3 && attacker->client->pers.statinfo.killsinarow < 5)
				{
					trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, %s is on ^1FIRE ^7with ^3%d ^7Kills in a Row!",
					level.time + 5000,
					attacker->client->pers.netname,
					attacker->client->pers.statinfo.killsinarow) );
				}
				else if(attacker->client->pers.statinfo.killsinarow >= 5 && level.gametypeData->teams)
				{
					trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, %s is ^3OWNING^7 the %s ^7team with ^3%d ^7Kills in a Row!",
					level.time + 5000,
					attacker->client->pers.netname,
					// GodOrDevil - 10.29.05 - 06:00am #Team Names
					attacker->client->sess.team == TEAM_BLUE ? "^1RED" : "^4BLUE",
					//attacker->client->sess.team == TEAM_BLUE ? "^1Purple" : "^4Green",
					attacker->client->pers.statinfo.killsinarow) );
				}
				else if(attacker->client->pers.statinfo.killsinarow >= 5 && !level.gametypeData->teams)
				{
					trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, %s is ^3OWNING ^7you All with ^3%d ^7Kills in a Row!",
					level.time + 5000,
					attacker->client->pers.netname,
					attacker->client->pers.statinfo.killsinarow) );
				}
			}


			// GodOrDevil - GAMETYPES - #dom #koth - killed someone taking your team flag then you get points.
			if(self->client->nvSess.capturing && (NV_isDom() || NV_isKoth()))
			{
				//if( g_entities[self->client->nvSess.capturing].flags == attacker->client->sess.team){
				//	trap_GT_SendEvent ( GTEV_ITEM_DEFEND, level.time, 0, attacker->s.clientNum, 0, 0, 0  );
				//}
				self->client->nvSess.capturing = 0;
#ifdef _DB_ENABLED	
				if(self->client->ps.stats[STAT_GAMETYPE_ITEMS] && attacker != self)
				{
					attacker->client->pers.statinfo.itemDefs++;
				}
#endif
			}
			// END

			//Ryan

			//Ryan april 22 2003
			//We'll set the last kill time when we do the obituary
			//attacker->client->lastKillTime = level.time;
			//Ryan
		}
	}
	else if ( mod != MOD_TEAMCHANGE )
	{
#ifdef _DB_ENABLED
		self->client->pers.statinfo.suicides++;
#endif
		G_AddScore( self, g_suicidePenalty.integer );
	}

	
	// GodOrDevil - GAMETYPES - #SPAWNKILL #Reinforcement timer
	//if(level.gametypeData->teams && level.gametypeRespawnTime[self->client->sess.team] <= level.time && mod != MOD_TEAMCHANGE ) 
	//{
	//	if(self->client->sess.team == TEAM_RED && nv_reinforcementsRedTime.integer >= 1)
	//		level.gametypeRespawnTime[TEAM_RED] = level.time + (nv_reinforcementsRedTime.integer * 1000);
	//	if(self->client->sess.team == TEAM_BLUE && nv_reinforcementsBlueTime.integer >= 1)
	//		level.gametypeRespawnTime[TEAM_BLUE] = level.time + (nv_reinforcementsBlueTime.integer * 1000);
	//}

	if( mod != MOD_TELEFRAG && self->client->spawn && attacker && attacker->client && attacker != self)
	{	
		attacker->client->nvSess.spawnKillLast = 30;
		attacker->client->nvSess.spawnKillCount++;
		trap_SendServerCommand( attacker-g_entities, "cp \"\n^1No Spawn Killing!\n\"" );
		NV_Sound(attacker-g_entities, "sound/movers/buttons/button03.mp3" );
		trap_SendServerCommand( attacker-g_entities, va("chat -1 \"^3*WARNING* ^7NO ^$SPAWN KILLING ^7or you will be ^$PUNISHED! ^3*WARNING*\n\""));
		self->client->spawn = qfalse;
#ifdef _DB_ENABLED
		attacker->client->pers.statinfo.spawnKills++;
#endif
	}
	// END

	//Ryan april 18 2003
	//This is where the server handles the obituary functions now (after the scores are done)
	if(!g_disableRPMObituary.integer || (hitLocation & HL_HEAD))
	{
		RPM_Obituary (self, attacker, meansOfDeath, attack, hitLocation);
	}
	//Ryan
			
	if ( mod != MOD_TELEFRAG )
	{

		//Ryan march 22 2004 9:20pm   calculate ratio's
		if(attacker && attacker->client && attacker != self )
		{
			if(attacker->client->pers.statinfo.deaths)
			{
				attacker->client->pers.statinfo.ratio = (float)attacker->client->pers.statinfo.kills / (float)attacker->client->pers.statinfo.deaths;
			}
			else
			{
				attacker->client->pers.statinfo.ratio = attacker->client->pers.statinfo.kills;
			}
		}
		//Careful if this "if" isnt here and they die from changing teams
		//before they have any other deaths we will divide by 0 since
		//we dont add deaths incurred from changing teams
		if(self->client->pers.statinfo.deaths)
		{
			self->client->pers.statinfo.ratio = (float)self->client->pers.statinfo.kills / (float)self->client->pers.statinfo.deaths;
		}
		else
		{
			self->client->pers.statinfo.ratio = (float)self->client->pers.statinfo.kills;
		}
	}
	//Ryan


	// If client is in a nodrop area, don't drop anything
	// GodOrDevil - GAMETYPES
	if(mod == MOD_TEAMCHANGE || !(NV_isTFT() || NV_isFT())) // GodOrDevil #TFT - when dying skip these checks on tft EXCEPT if its a team change death!
	{
	//
		contents = trap_PointContents( self->r.currentOrigin, -1 );
		if ( !( contents & CONTENTS_NODROP ) )
		{
			// People who kill themselves dont drop guns
			if ( attacker == self )
			{
				self->client->ps.stats[STAT_WEAPONS] = 0;
			}

			TossClientItems( self );
		}
		else
		{
			// Any gametype items that are dropped into a no drop area need to be reported
			// to the gametype so it can handle it accordingly
			for ( i = 0 ; i < MAX_GAMETYPE_ITEMS ; i++ )
			{
				gitem_t* item;

				// skip this gametype item if the client doenst have it
				if ( !(self->client->ps.stats[STAT_GAMETYPE_ITEMS] & (1<<i)) )
				{
					continue;
				}

				item = BG_FindGametypeItem ( i );

				// Let the gametype handle the problem, if it doenst handle it and return 1 then
				// just reset the gametype item
				if ( !trap_GT_SendEvent ( GTEV_ITEM_STUCK, level.time, item->quantity, 0, 0, 0, 0 ) )
				{
					G_ResetGametypeItem ( item );
				}
			}
		}
	}

	Cmd_Score_f( self );

	// send updated scores to any clients that are following this one,
	// or they would get stale scoreboards
	for ( i = 0 ; i < level.numConnectedClients; i++ )
	{
		gclient_t	*client;

		client = g_entities[level.sortedClients[i]].client;

		if ( client->pers.connected != CON_CONNECTED )
		{
			continue;
		}

		if ( !G_IsClientSpectating ( client ) )
		{
			continue;
		}

		if ( client->sess.spectatorClient == self->s.number )
		{
			Cmd_Score_f( g_entities + i );
		}
	}
	if( (NV_isTFT() || NV_isFT()) 
		&& mod != MOD_WATER 
		&& mod != MOD_CRUSH 
		&& mod != MOD_TELEFRAG 
		&& mod != MOD_TEAMCHANGE 
		&& mod != MOD_TARGET_LASER 
		&& mod != MOD_TRIGGER_HURT 
		)	
		// #TFT check for death #FREEZE #newway
	{
		self->r.contents				= CONTENTS_CORPSE;
		self->takedamage				= qfalse;
		self->client->ps.zoomFov		= 0;			// Turn off zooming when we die
		self->client->ps.pm_flags &= ~(PMF_GOGGLES_ON|PMF_ZOOM_FLAGS);
		self->s.angles[0]				= 0;
		self->s.angles[2]				= 0;
		self->s.loopSound				= 0;
		//self->r.maxs[2]					= -8;
		self->r.maxs[2]					= 50;
		self->client->ps.respawnTimer	= level.time + g_freezeTime.value * 1000;
			
		// set default animations
	//client->ps.weaponstate = WEAPON_READY;

		self->client->ps.weaponstate = -1;
		self->client->ps.torsoAnim = -1;
		self->client->ps.legsAnim = -1;
		self->client->ps.weaponAnimIdChoice = 0;
		self->client->ps.weaponCallbackStep = 0;
		self->client->ps.weaponCallbackTime = 0;

		//#GAS
		//self->client->nvSess.gasTime = 0;
		//self->client->nvSess.gasOwner = -1;
		//END

		memcpy( self->client->pers.ammo, self->client->ps.ammo, sizeof(self->client->pers.ammo));
		memcpy( self->client->pers.clip, self->client->ps.clip, sizeof(self->client->pers.clip));
		self->client->pers.weapons = self->client->ps.stats[STAT_WEAPONS];
		self->client->pers.currentWeapon = self->s.weapon;

		//NV_CopyToBodyQue (self, HL_NONE, hitDir );
		NV_spawnFrozen(self);
		
		//"sound/player/bullet_impacts/ice/ice1.wav"
		NV_Sound(self-g_entities, "sound/enemy/english/male/stop.mp3" );
	}
	else
	{
		self->s.weapon					= WP_NONE;
		self->s.gametypeitems			= 0;

		// no gibbing right now
	//	self->r.contents				= CONTENTS_CORPSE;
	//	self->takedamage				= qtrue;			// can still be gibbed
		self->r.contents				= CONTENTS_CORPSE;
		self->takedamage				= qfalse;

		self->client->ps.zoomFov		= 0;			// Turn off zooming when we die
		self->client->ps.stats[STAT_GAMETYPE_ITEMS] = 0;
		self->client->ps.pm_flags &= ~(PMF_GOGGLES_ON|PMF_ZOOM_FLAGS);

		self->s.angles[0]				= 0;
		self->s.angles[2]				= 0;
		self->s.loopSound				= 0;
		self->r.maxs[2]					= -8;

		LookAtKiller (self, inflictor, attacker);

		VectorCopy( self->s.angles, self->client->ps.viewangles );

		// don't allow respawn until the death anim is done
		// g_forcerespawn may force spawning at some later time
		self->client->respawnTime = level.time + 1700;

		switch ( hitLocation & (~HL_DISMEMBERBIT) )
		{
			case HL_WAIST:
				if ( rand() %2 )
				{
					anim = BOTH_DEATH_GROIN_1 + (rand()%2);
				}
				else
				{
					anim = BOTH_DEATH_GUT_1 + (rand()%2);	// GUT2 is being shot from the back.
				}
				break;

			default:
			case HL_CHEST:
				anim = BOTH_DEATH_CHEST_1 + (rand()%2);
				break;

			case HL_CHEST_RT:
				if ( irand(1,10) < 8 )
				{
					anim = BOTH_DEATH_SHOULDER_RIGHT_1 + (rand()%2);
				}
				else
				{
					anim = BOTH_DEATH_CHEST_1 + (rand()%2);
				}
				break;

			case HL_CHEST_LT:

				if ( irand(1,10) < 8 )
				{
					anim = BOTH_DEATH_SHOULDER_LEFT_1 + (rand()%2);
				}
				else
				{
					anim = BOTH_DEATH_CHEST_1 + (rand()%2);
				}

				break;

			case HL_NECK:
				anim = BOTH_DEATH_NECK;
				break;

			case HL_HEAD:
				anim = BOTH_DEATH_HEAD_1 + (rand()%2);
				break;

			case HL_LEG_UPPER_LT:
				anim = BOTH_DEATH_THIGH_LEFT_1 + (rand()%2);
				break;

			case HL_LEG_LOWER_LT:
			case HL_FOOT_LT:
				anim = BOTH_DEATH_LEGS_LEFT_1 + (rand()%3);
				break;

			case HL_ARM_LT:

				if ( rand()%2 )
					anim = BOTH_DEATH_ARMS_LEFT_1 + (rand()%2);
				else
					anim = BOTH_DEATH_SHOULDER_LEFT_1 + (rand()%2);

				break;

			case HL_ARM_RT:

				if ( rand()%2 )
					anim = BOTH_DEATH_ARMS_RIGHT_1 + (rand()%2);
				else
					anim = BOTH_DEATH_SHOULDER_RIGHT_1 + (rand()%2);

				break;

			case HL_LEG_UPPER_RT:
				anim = BOTH_DEATH_THIGH_RIGHT_1 + (rand()%2);
				break;

			case HL_LEG_LOWER_RT:
			case HL_FOOT_RT:
				anim = BOTH_DEATH_LEGS_RIGHT_1 + (rand()%3);
				break;
		}

		self->client->ps.legsAnim =
			( ( self->client->ps.legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
		self->client->ps.torsoAnim =
			( ( self->client->ps.torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;

		// If the dismember bit is set then make sure the body queue dismembers
		// the location that was hit
		if ( hitLocation & HL_DISMEMBERBIT )
		{
			CopyToBodyQue (self, hitLocation & (~HL_DISMEMBERBIT), hitDir );
		}
		else
		{
			CopyToBodyQue (self, HL_NONE, hitDir );
		}

		self->client->nvSess.stripped = qfalse;
		self->client->pers.planted = qfalse;
		//self->client->nvSess.fired = qfalse;
		self->client->nvSess.slow = qfalse;


		// GodOrDevil - GAMETYPES #RAZE
		//if(self->client->sess.team == TEAM_BLUE && NV_isRaze())
		//{
		//	if ( rand()%2 )
		//	{
		//		NV_playerDropItem (self, attacker, "gametype_item_1");
		//	}
		//}
	}

	// the body can still be gibbed
	//self->die = body_die;

	trap_LinkEntity (self);

	// GodOrDevil
	if(NV_isGG() && mod != MOD_TELEFRAG )
	{	
		if(attacker && attacker->client) 
		{
			NV_GunGamePromote(attacker, self, meansOfDeath);
		}
		switch (g_ggRankType.integer)
		{		
		case 0:
		default:
			if(attacker && attacker->client && (meansOfDeath == WP_KNIFE || self == attacker))
				NV_GunGameDemote (self, attacker);
			break;
		case 1:
			self->client->pers.rank = 0;
			break;
		case 2:
			if(self->client->pers.rank >= 1)
				self->client->pers.rank--;
			break;
		}
		self->client->sess.score = self->client->pers.rank;
		if(attacker && attacker->client)
			attacker->client->sess.score = attacker->client->pers.rank;
	}
	// End

#ifdef _DB_ENABLED
	if (mod != MOD_TELEFRAG && attacker && attacker->client && (weap >= WP_KNIFE && weap < WP_NUM_WEAPONS) && attacker != self && !OnSameTeam ( self, attacker ))
	{

		if(weap == WP_M4_ASSAULT_RIFLE && g_m4scope.integer && (attacker->client->ps.pm_flags & PMF_ZOOMED)) // m4 scope == alt attack
			attacker->client->pers.statinfo.wpnKills[weap][ATTACK_ALTERNATE]++;
		else if(weap == WP_MSG90A1 && !(attacker->client->ps.pm_flags & PMF_ZOOMED)) // sniper no scope == alt attack
			attacker->client->pers.statinfo.wpnKills[weap][ATTACK_ALTERNATE]++;
		else
			attacker->client->pers.statinfo.wpnKills[weap][attack]++;
	}
#endif

	return;
}

/*
================
CheckArmor
================
*/
int CheckArmor (gentity_t *ent, int damage, int dflags)
{
	gclient_t	*client;
	int			save;
	int			count;

	if (!damage)
		return 0;

	client = ent->client;

	if (!client)
		return 0;

	if (dflags & DAMAGE_NO_ARMOR)
		return 0;

	// armor
	count = client->ps.stats[STAT_REALARMOR];
	save = ceil( damage * ARMOR_PROTECTION );
	if (save >= count)
		save = count;

	if (!save)
		return 0;

	client->ps.stats[STAT_REALARMOR] -= save;

	return save;
}


void G_ApplyKnockback( gentity_t *targ, vec3_t newDir, float knockback )
{
	vec3_t	kvel;
	float	mass;

	if ( targ->physicsBounce > 0 )	//overide the mass
		mass = targ->physicsBounce;
	else
		mass = 200;

	if ( g_gravity.value > 0 )
	{
		VectorScale( newDir, g_knockback.value * (float)knockback / mass * 0.8, kvel );
//		kvel[2] = newDir[2] * g_knockback.value * (float)knockback / mass * 1.5;
	}
	else
	{
		VectorScale( newDir, g_knockback.value * (float)knockback / mass, kvel );
	}

	if ( targ->client )
	{
		VectorAdd( targ->client->ps.velocity, kvel, targ->client->ps.velocity );
	}
	else if ( targ->s.pos.trType != TR_STATIONARY && targ->s.pos.trType != TR_LINEAR_STOP )
	{
		VectorAdd( targ->s.pos.trDelta, kvel, targ->s.pos.trDelta );
		VectorCopy( targ->r.currentOrigin, targ->s.pos.trBase );
		targ->s.pos.trTime = level.time;
	}

	// set the timer so that the other client can't cancel
	// out the movement immediately
	if ( targ->client && !targ->client->ps.pm_time )
	{
		int		t;

		t = knockback * 2;
		if ( t < 50 ) {
			t = 50;
		}
		if ( t > 200 ) {
			t = 200;
		}
		targ->client->ps.pm_time = t;
		targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
	}
}

/*
================
RaySphereIntersections
================
*/
int RaySphereIntersections( vec3_t origin, float radius, vec3_t point, vec3_t dir, vec3_t intersections[2] ) {
	float b, c, d, t;

	//	| origin - (point + t * dir) | = radius
	//	a = dir[0]^2 + dir[1]^2 + dir[2]^2;
	//	b = 2 * (dir[0] * (point[0] - origin[0]) + dir[1] * (point[1] - origin[1]) + dir[2] * (point[2] - origin[2]));
	//	c = (point[0] - origin[0])^2 + (point[1] - origin[1])^2 + (point[2] - origin[2])^2 - radius^2;

	// normalize dir so a = 1
	VectorNormalize(dir);
	b = 2 * (dir[0] * (point[0] - origin[0]) + dir[1] * (point[1] - origin[1]) + dir[2] * (point[2] - origin[2]));
	c = (point[0] - origin[0]) * (point[0] - origin[0]) +
		(point[1] - origin[1]) * (point[1] - origin[1]) +
		(point[2] - origin[2]) * (point[2] - origin[2]) -
		radius * radius;

	d = b * b - 4 * c;
	if (d > 0) {
		t = (- b + sqrt(d)) / 2;
		VectorMA(point, t, dir, intersections[0]);
		t = (- b - sqrt(d)) / 2;
		VectorMA(point, t, dir, intersections[1]);
		return 2;
	}
	else if (d == 0) {
		t = (- b ) / 2;
		VectorMA(point, t, dir, intersections[0]);
		return 1;
	}
	return 0;
}

int G_GetHitLocation(gentity_t *target, vec3_t ppoint, vec3_t dir )
{
	float		fdot;
	float		rdot;
	vec3_t		tangles;
	vec3_t		forward;
	vec3_t		up;
	vec3_t		right;
	vec3_t		distance;
	vec3_t		tcenter;
	vec3_t		temp;
	vec3_t		hit;

	// We are only interested in the YAW angle of the target
	VectorSet( tangles, 0, target->client->ps.viewangles[YAW], 0);

	// Extract the forward, right, and up vectors
	AngleVectors ( tangles, forward, right, up );

	// Determine the center of the target entity
	VectorAdd(target->r.absmin, target->r.absmax, tcenter);
	VectorScale(tcenter, 0.5, tcenter);


/* NOTE: This would work to figure out shots that go across the front of someone and
         hit the opposite side, but had an error in it when a shot came from either
		 the immediate left or right of the player.
*/

	// Calculate the distnace from the shooter to the target
	VectorCopy ( dir, temp );
	VectorSubtract ( tcenter, ppoint, distance );

	// Use that distnace to determine the point of tangent in relation to
	// the center of the player entity
	VectorMA ( ppoint, DotProduct ( temp, distance ), temp, hit );

	// Create a vector from the tangent point to the center.  This will
	// be used to determine which side was hit
	VectorSubtract ( tcenter, hit, temp );
	VectorCopy ( temp, distance );

	VectorSubtract ( tcenter, ppoint, temp );
	VectorNormalize ( temp );

	// Determine the shot in relation to the forward vector
	fdot = DotProduct ( forward, temp );

	// Determine the shot in relation to the right vector
	rdot = DotProduct ( right, temp );

	if ( distance[2] < -35 )
	{
		return HL_HEAD;
	}
	else if ( distance[2] < -32 )
	{
		return HL_NECK;
	}
	else if ( distance[2] < -27 )
	{
		if ( rdot > 0 )
			return HL_ARM_LT;

		return HL_ARM_RT;
	}
	else if ( distance[2] < -3 )
	{
		if ( fdot > 0 )
		{
			if ( rdot > 0 )
			{
				return HL_CHEST_LT;
			}

			return HL_CHEST_RT;
		}

		if ( rdot > 0 )
		{
			return HL_BACK_LT;
		}

		return HL_BACK_RT;
	}
	else if ( distance[2] < 4 )
	{
		return HL_WAIST;
	}
	else if ( distance[2] < 18 )
	{
		if ( rdot > 0 )
			return HL_LEG_UPPER_LT;

		return HL_LEG_UPPER_RT;
	}
	else if ( distance[2] < 33 )
	{
		if ( rdot > 0 )
			return HL_LEG_LOWER_LT;

		return HL_LEG_LOWER_RT;
	}

	if ( rdot > 0 )
		return HL_FOOT_LT;

	return HL_FOOT_RT;
}

/*
============
T_Damage

targ		entity that is being damaged
inflictor	entity that is causing the damage
attacker	entity that caused the inflictor to damage targ
	example: targ=monster, inflictor=rocket, attacker=player

dir			direction of the attack for knockback
point		point at which the damage is being inflicted, used for headshots
damage		amount of damage being inflicted
knockback	force to be applied against targ as a result of the damage

inflictor, attacker, dir, and point can be NULL for environmental effects

dflags		these flags are used to control how T_Damage works
	DAMAGE_RADIUS			damage was indirect (from a nearby explosion)
	DAMAGE_NO_ARMOR			armor does not protect from this damage
	DAMAGE_NO_KNOCKBACK		do not affect velocity, just view angles
	DAMAGE_NO_PROTECTION	kills godmode, armor, everything
============
*/

int G_Damage (
	gentity_t		*targ,
	gentity_t		*inflictor,
	gentity_t		*attacker,
	vec3_t			dir,
	vec3_t			point,
	int				damage,
	int				dflags,
	int				mod,
	int				location
	)
{
	gclient_t		*client;
	int				take;
	int				save;
	int				asave;
	int				knockback;
	//Ryan
	int				actualtake;
	//Ryan

	if (!targ->takedamage)
	{
		return 0;
	}

	// See if they are invulnerable
	if ( (mod&0xFF) < MOD_WATER )
	{
		if ( targ->client && (level.time - targ->client->invulnerableTime < g_respawnInvulnerability.integer * 1000) )
		{
			return 0;
		}
	}

	// the intermission has allready been qualified for, so don't
	// allow any extra scoring
	if ( level.intermissionQueued )
	{
		return 0;
	}

	if ( level.gametypeResetTime && NV_isVIP() )
	{
		return 0;
	}

	// GodOrDevil - #GAS - if goggles are active then dont do damage	
#ifdef _SPECIAL_NADES
	if(mod == WP_F1_GRENADE && g_enableCustomGrenades.integer && targ && targ->client && targ->client->ps.pm_flags & PMF_GOGGLES_ON)
	{
		return 0;
	}	
#endif // _SPECIAL_NADES
	//End

	// Cant change outfitting after being shot
	if ( targ->client )
	{
		targ->client->noOutfittingChange = qtrue;
	}

	if ( !inflictor )
	{
		inflictor = &g_entities[ENTITYNUM_WORLD];
	}
	if ( !attacker )
	{
		attacker = &g_entities[ENTITYNUM_WORLD];
	}

	// shootable doors / buttons don't actually have any health
	if ( targ->s.eType == ET_MOVER )
	{
		if ( targ->use && targ->moverState == MOVER_POS1 )
		{
			targ->use( targ, inflictor, attacker );
		}
		return 0;
	}

	client = targ->client;

	if ( client )
	{
		if ( client->noclip )
		{
			return 0;
		}
	}

	// GodOrDevil - I liked this idea!	
	// KRIS
	// GRIM 30/08/2002 2:46PM 
	// Make the incendiary grenades less of a 'spam' weapon
	// by allowing people to run through the fire as apposed
	// to being suddenly stopped by it - gets rather annoying
	// (unless sof2defaults is set)
	if (((mod & 0xFF) == MOD_ANM14_GRENADE))
	{
		dflags |= DAMAGE_NO_KNOCKBACK;
	}
	// GRIM

	if ( !dir )
	{
		dflags |= DAMAGE_NO_KNOCKBACK;
	}
	else
	{
		VectorNormalize(dir);
	}

	knockback = damage;
	if ( knockback > 200 )
	{
		knockback = 200;
	}

	if ( targ->flags & FL_NO_KNOCKBACK ) {
		knockback = 0;
	}
	if ( dflags & DAMAGE_NO_KNOCKBACK ) {
		knockback = 0;
	}

/*
	// figure momentum add, even if the damage won't be taken
	if ( knockback && targ->client )
	{
		G_ApplyKnockback ( targ, dir, knockback );
		vec3_t	kvel;
		float	mass;

		mass = 200;

		VectorScale (dir, g_knockback.value * (float)knockback / mass, kvel);
		VectorAdd (targ->client->ps.velocity, kvel, targ->client->ps.velocity);

		// set the timer so that the other client can't cancel
		// out the movement immediately
		if ( !targ->client->ps.pm_time ) {
			int		t;

			t = knockback * 2;
			if ( t < 50 ) {
				t = 50;
			}
			if ( t > 200 ) {
				t = 200;
			}
			targ->client->ps.pm_time = t;
			targ->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
		}
	}
*/

	// check for completely getting out of the damage
	if ( !(dflags & DAMAGE_NO_PROTECTION) ) {

		// if TF_NO_FRIENDLY_FIRE is set, don't do damage to the target
		// if the attacker was on the same team
		if ( targ != attacker && OnSameTeam (targ, attacker)  )
		{
			if ( !g_friendlyFire.integer )
			{
				return 0;
			}
		}

		// check for godmode
		if ( targ->flags & FL_GODMODE )
		{
			return 0;
		}
	}

	if ( damage < 1 )
	{
		damage = 1;
	}

	take = damage;
	save = 0;

	// Be careful with grenades
	if ( attacker == targ )
	{
		take *= 2;
	}
	
	// GodOrDevil
	if (g_maxSpawnKill.integer && level.gametypeData->teams && attacker && attacker->client && targ && targ->client && targ->client->spawn)
	{

		if(attacker->client->nvSess.spawnKillCount >= g_maxSpawnKill.integer)
		{
			if((attacker->client->ps.stats[STAT_GAMETYPE_ITEMS] == 0 && !attacker->client->nvSess.capturing) || attacker->client->nvSess.spawnKillCount > 15)
			{
				targ = attacker;
				mod = MOD_SPAWNKILL;
			}
		}
	}
	// End

	// save some from armor
	asave = CheckArmor (targ, take, dflags);
	take -= asave;

	//Ryan
	actualtake = Com_Clamp ( 0, targ->health, take );
	//Ryan

	// Teamkill dmage thats not caused by a telefrag?
	if ( g_teamkillDamageMax.integer && mod != MOD_TELEFRAG )
	{
		if ( level.gametypeData->teams && targ && attacker && targ != attacker )
		{
			// Hurt your own team?
			if ( OnSameTeam ( targ, attacker ) )
			{
				// Dont count more than one damage call per frame (grenades!)
				if ( level.time != attacker->client->sess.teamkillForgiveTime )
				{
					/*
					//Ryan june 7 2003
					//admins are not effected by tk damage
					// GodOrDevil - 03.01.05 - 10:13pm - Decided to let admin have tk but use pBox.. no kick still.
					//if(attacker->client->nvSess.admin != 1)
					//if(attacker->client->nvSess.admin < 2) //03.01.05 - 10:13pm 
					{										
						attacker->client->sess.teamkillDamage	   += actualtake;
						attacker->client->sess.teamkillForgiveTime  = level.time;
					}
					//Ryan
					*/
					// GodOrDevil - 03.01.05 - 10:13pm #pBoxTeamKill
					//Decided to let admin have tk and use pBox to punish abusers
					//Admins will NOT get kicked!
					attacker->client->sess.teamkillDamage	   += actualtake;
					attacker->client->sess.teamkillForgiveTime  = level.time;
				}
			}
		}
	}

	// Output hits
	// GodOrDevil - 04.21.06 - 10:31pm #warmup - dont log hits durring warmup
	//if ( g_logHits.integer && attacker && targ && attacker->client && targ->client )
	if ( !level.warmupTime && g_logHits.integer && attacker && targ && attacker->client && targ->client )
	///End  - 04.21.06 - 10:31pm
	{
		G_LogPrintf ( "hit: %i %i %i %i %i: %s hit %s at location %i for %i\n",
						  attacker->s.number,
						  targ->s.number,
						  location,
						  take,
						  asave,
						  attacker->client->pers.netname,
						  targ->client->pers.netname,
						  location,
						  (int)((float)take) );
	}

	if ( g_debugDamage.integer )
	{
		Com_Printf( "%i: client:%i health:%i damage:%i armor:%i\n", level.time, targ->s.number, targ->health, take, asave );
	}

	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	if ( client )
	{
		if ( attacker )
		{
			client->ps.persistant[PERS_ATTACKER] = attacker->s.number;
		}
		else
		{
			client->ps.persistant[PERS_ATTACKER] = ENTITYNUM_WORLD;
		}

#ifdef _SPECIAL_NADES
		if ( mod != MOD_WATER && !(mod == WP_F1_GRENADE && g_enableCustomGrenades.integer ) ) // #GAS
#else
		if ( mod != MOD_WATER )
#endif //_SPECIAL_NADES
		{
			client->damage_armor += asave;
			client->damage_blood += take;
		}

		client->damage_knockback += knockback;

		if ( dir )
		{
			VectorCopy ( dir, client->damage_from );
			client->damage_fromWorld = qfalse;
		}
		else
		{
			VectorCopy ( targ->r.currentOrigin, client->damage_from );
			client->damage_fromWorld = qtrue;
		}
#ifdef _SOF2_BOTS
		if (attacker && attacker->client)
		{
			BotDamageNotification(client, attacker);
		}
		else if (inflictor && inflictor->client)
		{
			BotDamageNotification(client, inflictor);
		}
#endif
	}

	if (targ->client)
	{
		// set the last client who damaged the target
		targ->client->lasthurt_client = attacker->s.number;
		targ->client->lasthurt_time = level.time;
		targ->client->lasthurt_mod = mod;

		//Ryan april 28 2003
		targ->client->pers.statinfo.lasthurtby = attacker->s.number;
		if(attacker->client)
		{
			attacker->client->pers.statinfo.lastclient_hurt = targ->s.number;
		}
		//Ryan
	}

	// do the damage
	if (take)
	{
		targ->health = targ->health - take;

		if ( targ->client )
		{
			//Ryan
			//Punish them for killing themselves by not adding to
			//their damagedone
			if(targ == attacker)
			{
				targ->client->pers.statinfo.damageTaken += actualtake;
			}
			////Punish them for killing a teammate and dont add to the
			////victims damage taken
			// GodOrDevil - 11.01.05 - 06:21am - #DLL barfs on this
			else if(level.gametypeData->teams && OnSameTeam(targ,attacker))
			{
				attacker->client->pers.statinfo.damageDone -= actualtake;
			}
			// GodOrDevil - 01.08.06 - 09:50pm 
			///else if (attacker && attacker->s.number != ENTITYNUM_WORLD)
			///{
			///Com_Printf("DAMAGEDONE: %i, AT: %i, D+AT: %i\n", attacker->client->pers.statinfo.damageDone, actualtake, attacker->client->pers.statinfo.damageDone += actualtake);
			else if(attacker && attacker->client){
			///else { //RPM 0.6 ELSE
			///End  - 01.10.06 - 02:26pm
				attacker->client->pers.statinfo.damageDone += actualtake;
				targ->client->pers.statinfo.damageTaken += actualtake;
				
			}

			///else
			///{
			///	attacker->client->pers.statinfo.damageDone += actualtake;
			///	targ->client->pers.statinfo.damageTaken += actualtake;
			///}
			///End  - 11.01.05 - 06:21am
			//End - R

			targ->client->ps.stats[STAT_HEALTH] = targ->health;

//#ifdef _OLDVIP
//			if ( NV_isVIP() && targ->client->nvSess.vip == 1 && !knockback)
//			{
//
//			}
//			else
//#endif
#ifdef _OLDVIP
			if ( targ->health > 0 && (mod&0xFF) != MOD_DUGUP)
#else
			if ( targ->health > 0 )
#endif
			{
				// Slow down the client at bit when they get hit
				targ->client->ps.velocity[0] *= 0.25f;
				targ->client->ps.velocity[1] *= 0.25f;

				// figure momentum add, even if the damage won't be taken
				if ( knockback )
				{	
					// GodOrDevil - #GAS
#ifdef _SPECIAL_NADES
					if(mod == WP_F1_GRENADE && g_enableCustomGrenades.integer)
					{
						// DO NOTHING
					}
					else
#endif
					//END
					G_ApplyKnockback ( targ, dir, knockback );
				}

				// Friendly fire
				if ( g_friendlyFire.integer && targ != attacker && OnSameTeam ( targ, attacker ) )
				{
					vec3_t diff;

					// Make sure the attacker is close enough to hear the guy whining
					VectorSubtract ( targ->r.currentOrigin, attacker->r.currentOrigin, diff );
					if ( VectorLengthSquared ( diff ) < 800 * 800 )
					{
						G_VoiceGlobal ( targ, "check_fire", qfalse );
					}
				}
			}
		}

		if ( targ->health <= 0 )
		{
			// Something dismembered?
			if ( (targ->health < DISMEMBER_HEALTH && !(dflags&DAMAGE_NO_GORE)) || (dflags&DAMAGE_FORCE_GORE) )
			{
				location |= HL_DISMEMBERBIT;
			}

			if ( client )
				targ->flags |= FL_NO_KNOCKBACK;

			if (targ->health < -999)
				targ->health = -999;

			targ->enemy = attacker;
			targ->die(targ, inflictor, attacker, take, mod, location, dir );
		}
		else if ( targ->pain )
		{
			targ->pain (targ, attacker, take);
		}
	}

	return take;
}

/*
============
CanDamage

Returns qtrue if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
qboolean CanDamage (gentity_t *targ, vec3_t origin) {
	vec3_t	dest;
	trace_t	tr;
	vec3_t	midpoint;

	// use the midpoint of the bounds instead of the origin, because
	// bmodels may have their origin is 0,0,0
	VectorAdd (targ->r.absmin, targ->r.absmax, midpoint);
	VectorScale (midpoint, 0.5, midpoint);

	VectorCopy (midpoint, dest);
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0 || tr.entityNum == targ->s.number)
		return qtrue;

	// this should probably check in the plane of projection,
	// rather than in world coordinate, and also include Z
	VectorCopy (midpoint, dest);
	dest[0] += 15.0;
	dest[1] += 15.0;
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0)
		return qtrue;

	VectorCopy (midpoint, dest);
	dest[2] = targ->r.absmax[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0)
		return qtrue;

	VectorCopy (midpoint, dest);
	dest[2] = targ->r.absmin[2];
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0)
		return qtrue;

	VectorCopy (midpoint, dest);
	dest[0] += 15.0;
	dest[1] -= 15.0;
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0)
		return qtrue;

	VectorCopy (midpoint, dest);
	dest[0] -= 15.0;
	dest[1] += 15.0;
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0)
		return qtrue;

	VectorCopy (midpoint, dest);
	dest[0] -= 15.0;
	dest[1] -= 15.0;
	trap_Trace ( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID);
	if (tr.fraction == 1.0)
		return qtrue;


	return qfalse;
}
/*
============
G_MultipleDamageLocations
============
*/
int G_MultipleDamageLocations(int hitLocation)
{

	switch ( hitLocation & (~HL_DISMEMBERBIT) )
	{
		case HL_FOOT_RT:
		case HL_FOOT_LT:
			hitLocation |= (HL_FOOT_RT | HL_FOOT_LT);
			break;
		case HL_LEG_UPPER_RT:
			hitLocation |= (HL_LEG_UPPER_RT | HL_LEG_LOWER_LT);
			if ( rand() %2 )
			{
				hitLocation |= HL_HAND_RT;
			}
			break;
		case HL_LEG_UPPER_LT:
			hitLocation |= (HL_LEG_UPPER_LT | HL_LEG_LOWER_RT);
			if ( rand() %2 )
			{
				hitLocation |= HL_HAND_LT;
			}
			break;
		case HL_LEG_LOWER_RT:
			hitLocation |= (HL_LEG_LOWER_RT | HL_FOOT_LT);
			break;
		case HL_LEG_LOWER_LT:
			hitLocation |= (HL_LEG_LOWER_LT | HL_FOOT_RT);
			break;
		case HL_HAND_RT:
			hitLocation |= HL_HAND_RT;
			break;
		case HL_HAND_LT:
			hitLocation |= HL_HAND_LT;
			break;
		case HL_ARM_RT:
			hitLocation |= (HL_ARM_RT | HL_LEG_UPPER_RT) ;
			break;
		case HL_ARM_LT:
			hitLocation |= (HL_ARM_LT | HL_LEG_UPPER_LT) ;
			break;
		case HL_HEAD:
			hitLocation |= HL_HEAD ;
			if ( rand() %2 )
			{
				hitLocation |= HL_ARM_RT;
			}
			else
			{
				hitLocation |= HL_ARM_LT;
			}
			break;
		case HL_WAIST:
			hitLocation |= (HL_LEG_UPPER_RT | HL_LEG_UPPER_LT) ;

			if ( rand() %2 )
			{
				if ( rand() %2 )
				{
					hitLocation |= HL_HAND_RT;
				}
				else
				{
					hitLocation |= HL_HAND_LT;
				}
			}
			break;
		case HL_BACK_RT:
		case HL_CHEST_RT:
			hitLocation |= HL_ARM_RT;
			hitLocation |= HL_HEAD;
			break;
		case HL_BACK_LT:
		case HL_CHEST_LT:
			hitLocation |= HL_ARM_LT;
			hitLocation |= HL_HEAD;
			break;
		case HL_BACK:
		case HL_CHEST:
			hitLocation |= (HL_ARM_RT | HL_ARM_LT);
			hitLocation |= HL_HEAD;
			break;

	}

	return (hitLocation);
}


/*
============
G_RadiusDamage
============
*/
qboolean G_RadiusDamage (
	vec3_t		origin,
	gentity_t*	attacker,
	float		damage,
	float		radius,
	gentity_t*	ignore,
	int			power,
	int			mod
	)
{
	float		points, dist;
	gentity_t	*ent, *tent;
	int			entityList[MAX_GENTITIES];
	int			numListedEntities;
	vec3_t		mins, maxs;
	vec3_t		v;
	vec3_t		dir;
	int			i, e;
	qboolean	hitClient = qfalse;

	if ( radius < 1 )
	{
		radius = 1;
	}

	for ( i = 0 ; i < 3 ; i++ )
	{
		mins[i] = origin[i] - radius;
		maxs[i] = origin[i] + radius;
	}

	numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

	for ( e = 0 ; e < numListedEntities ; e++ )
	{
		ent = &g_entities[entityList[ e ]];

		if (ent == ignore)
		{
			continue;
		}

		if (!ent->takedamage)
		{
			continue;
		}

		// find the distance from the edge of the bounding box
		for ( i = 0 ; i < 3 ; i++ )
		{
			if ( origin[i] < ent->r.absmin[i] )
			{
				v[i] = ent->r.absmin[i] - origin[i];
			}
			else if ( origin[i] > ent->r.absmax[i] )
			{
				v[i] = origin[i] - ent->r.absmax[i];
			}
			else
			{
				v[i] = 0;
			}
		}

		dist = VectorLength( v );
		if ( dist >= radius )
		{
			continue;
		}

		points = damage * ( 1.0 - powf((dist / radius), power));

		if( CanDamage (ent, origin) )
		{
			int		location;
			int		weapon;
			vec3_t	hitdir;
			int		d;

			VectorSubtract (ent->r.currentOrigin, origin, dir);
			// push the center of mass higher than the origin so players
			// get knocked into the air more

			location = HL_NONE;
			if ( ent->client )
			{
				VectorNormalize ( dir );
				VectorCopy(dir, hitdir);
				dir[2] = 0;
				location = G_GetHitLocation ( ent, origin, dir );
				location = G_MultipleDamageLocations ( location );
			}
			// GodOrDevil - GAMETYPES #bomb damage					
			else if ( ent->health && attacker && !Q_stricmpn ( ent->classname, "blocker", 7) /*&& !Q_stricmpn ( ent->classname, "func_breakable", 14) */)
			{
				//ent->damage	+=	(int)points * 4;
				// BOMB
				G_BreakableDie ( ent, NULL, attacker, (int)points, mod, location, NULL );
				return hitClient;
			}

			d = G_Damage (ent, NULL, attacker, dir, origin, (int)points, DAMAGE_RADIUS|DAMAGE_NO_ARMOR, mod, location );

			if ( d && ent->client )
			{
				//Ryan may 16 2004
				//log the hit into our stats
				statinfo_t *stat = &attacker->client->pers.statinfo;

				//Fire nades only count a hit if they kill
				//if(attacker->client && mod != MOD_ANM14_GRENADE && (!level.gametypeData->teams || (level.gametypeData->teams && !OnSameTeam ( attacker, ent ))))
				if(/*!stat->hitcount &&*/ attacker->client && mod != MOD_ANM14_GRENADE && attacker != ent && (!level.gametypeData->teams || (level.gametypeData->teams && !OnSameTeam ( attacker, ent ))))
				{
					stat->hitcount++;
					stat->accuracy = (float)stat->hitcount / (float)stat->shotcount * 100;

					if(mod - 256 == MOD_M4_ASSAULT_RIFLE)
					{
						stat->weapon_hits[ATTACK_ALTERNATE][WP_M4_ASSAULT_RIFLE]++;
					}
					else
					{
						stat->weapon_hits[ATTACK_NORMAL][mod]++;
					}
				}
				//Ryan

				// Put some procedural gore on the target.
				tent = G_TempEntity( origin, EV_EXPLOSION_HIT_FLESH );

				// send entity and direction
				tent->s.eventParm = DirToByte( hitdir );
				if (ignore && ignore->s.weapon)
				{
					weapon = ignore->s.weapon;		// Weapon type number
				}
				else if (points >= 10)
				{	// dangerous weapon
					weapon = WP_M67_GRENADE;
				}
				else
				{	// Just a flesh wound
					weapon = WP_M84_GRENADE;
				}
				tent->s.otherEntityNum2 = ent->s.number;			// Victim entity number

				// Pack the shot info into the temp end for gore
				tent->s.time  = weapon + ((((int)ent->s.apos.trBase[YAW]&0x7FFF) % 360) << 16);
				if ( attacker->s.eFlags & EF_ALT_FIRING )
				{
					tent->s.time += (ATTACK_ALTERNATE<<8);
				}
				VectorCopy ( ent->r.currentOrigin, tent->s.angles );
				SnapVector ( tent->s.angles );
			}
		}
	}

	return hitClient;
}
