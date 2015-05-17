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
#ifdef _DB_ENABLED
#define MAX_AWARDS 7
#endif

typedef struct statinfo_s
{
	int weapon_shots[2][WP_NUM_WEAPONS];
	int weapon_hits[2][WP_NUM_WEAPONS];
	int weapon_headshots[2][WP_NUM_WEAPONS];
	int			weapon;
	int			attack;
	int         shotcount;
	int		    hitcount;
	float		accuracy;
	float		ratio;
	int         handhits;
	int         foothits;
	int         armhits;
	int         leghits;
	int         headhits;
	int         neckhits;
	int         torsohits;
	int         waisthits;	
	int			headShotKills;
	int			kills;
	int			killsinarow;
	int			deaths;
	int			damageDone;
	int			damageTaken;
	int			lastclient_hurt;
	int			lasthurtby;
	int			overallScore;
	int			explosiveKills;
	int			knifeKills;

#ifdef _DB_ENABLED
	int		refresh;
	int		itemCaps;
	int		itemDefs;
	int		suicides;
	int		killSpreeCount;
	int		killSpreeRecord;
	int		packer;
	int		m4scoped;  // m4 scope kills
	int		awards[MAX_AWARDS];
	int		wpnKills[WP_NUM_WEAPONS][ATTACK_MAX];
	int		wpnDeaths[WP_NUM_WEAPONS];
	int		weapon_killLoc[WP_NUM_WEAPONS][8];
	int		spawnKills;

#endif

} statinfo_t;

typedef struct chatSounds_s
{
	char text[MAX_SAY_TEXT];
	int	 sound;
} chatSounds_t;

#define	MAX_RPM_CHATS	200

#define AMMO_MOD	0x00000001
#define DAMAGE_MOD	0x00000002

