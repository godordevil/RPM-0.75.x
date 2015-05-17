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

//current punishment count
#define PUNISH_COUNT 9

//index number for the ref team command will be global just for ease of use
int		refTeamCmdIndex;
int		refTeamIndex;

//the text displayed in our punishment list
char* punishList[] =
{
	"Runover",
	"Pop",
	"Strip",
	"Mute",
	"Un-Mute",
	"Plant for Camping",
	"Plant for Spawn Killing",
	"Kick",
	"Ban",
};

//the actual command text sent to the server
char* punishCmds[] = 
{
	"runover",
	"pop",
	"strip",
	"mute",
	"unmute",
	"plant",
	"plantsk",
	"kick",
	"ban",
};

//currnet ref team command count
#define	REF_TEAM_COMMAND_COUNT 5

char* refTeamCmdList[] = 
{
	"Team Info",
	"Ready Team",
	"Lock Team",
	"Un-lock Team",
	"Reset Team",
};

char* refTeamCmd[] = 
{
	"info",
	"ready",
	"lock",
	"unlock",
	"reset",
};

char* teamList[] = 
{
	"Red",
	"Blue",
};

#define	VOTECOUNT	12

char* voteList[][2] =
{
	{ "Friendly Fire",					"friendlyfire"	},
	{ "Competition Mode",				"compmode"		},
	{ "Referee",						"referee"		},
	{ "Un-Referee",						"unreferee"		},
	{ "Swap Teams",						"swap_teams"	},
	{ "Start Match",					"start_match"	},
	{ "Modify weapon Damage",			"damagemod"		},
	{ "Default Damage amounts",			"defaultdamage"	},
	{ "Modify weapon Ammo",				"ammomod"		},
	{ "Default Ammo amounts",			"defaultammo"	},
	{ "Modify weapon Ammo & Damage",	"weaponmod"		},
	{ "Default Ammo & Damage",			"defaultweapons"},
};

