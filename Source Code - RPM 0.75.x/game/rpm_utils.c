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

void ExitLevel( void );
gspawn_t* G_SelectRandomSafeSpawnPoint ( team_t team, float safeDistance );

chatSounds_t chatSounds[MAX_RPM_CHATS];

char* defaultChatSounds[][2] =
{
	{"Call for reinforcements",			"sound/enemy/english/male/call_for_reinforcements.mp3"	},
	{"Advance",							"sound/enemy/english/male/advance.mp3"					},
	{"Awaiting orders",					"sound/enemy/english/male/awaiting_orders.mp3"			},
	{"Check the perimiter",				"sound/enemy/english/male/check_peri.mp3"				},
	{"Go check that out",				"sound/enemy/english/male/check_that_out.mp3"			},
	{"He's dangerous",					"sound/enemy/english/male/dangerous.mp3"				},
	{"Did you hear that",				"sound/enemy/english/male/did_you_hear.mp3"			},
	{"He's disappeared",				"sound/enemy/english/male/disappeared.mp3"				},
	{"Drop your weapon",				"sound/enemy/english/male/drop_your_weapon.mp3"		},
	{"Eliminate target",				"sound/enemy/english/male/eliminate.mp3"				},
	{"GET HIM",							"sound/enemy/english/male/get_him.mp3"					},
	{"I got that bastard",				"sound/enemy/english/male/got_bastard.mp3"				},
	{"Hold here",						"sound/enemy/english/male/hold_here.mp3"				},
	{"Hurry",							"sound/enemy/english/male/hurry.mp3"					},
	{"I'm hit",							"sound/npc/col8/blakely/imhit.mp3"						},
	{"Investigate that area",			"sound/enemy/english/male/investigate_area.mp3"		},
	{"Keep looking",					"sound/enemy/english/male/keep_looking.mp3"			},
	{"KILL HIM",						"sound/enemy/english/male/kill_him"					},
	{"I killed him",					"sound/enemy/english/male/killed_him.mp3"				},
	{"I lost him",						"sound/enemy/english/male/lost_him.mp3"				},
	{"TAKING SERIOUS CASUALTIES",		"sound/enemy/english/male/man_down03.mp3"				},
	{"I need help here",				"sound/enemy/english/male/need_help.mp3"				},
	{"Medic Medic",						"sound/enemy/english/male/medic_medic.mp3"				},
	{"Open Fire",						"sound/enemy/english/male/open_fire.mp3"				},
	{"I'm out of Ammo",					"sound/enemy/english/fmale/out_ammo.mp3"				},
	{"He's over here",					"sound/enemy/english/male/over_here.mp3"				},
	{"Over there",						"sound/enemy/english/male/over_there.mp3"				},
	{"Plug him",						"sound/enemy/english/male/plug_him.mp3"				},
	{"Position reached",				"sound/enemy/english/male/pos_reached.mp3"				},
	{"Secure the area",					"sound/enemy/english/male/secure_the_area.mp3"			},
	{"We're getting slaughtered",		"sound/enemy/english/male/slaughtered.mp3"				},
	{"Somethings not right here",		"sound/enemy/english/male/something_not_right.mp3"		},
	{"Spread out",						"sound/enemy/english/male/spread_out.mp3"				},
	{"Surround him",					"sound/enemy/english/male/surround_him.mp3"			},
	{"Take cover",						"sound/enemy/english/male/take_cover2.mp3"				},
	{"Take him out",					"sound/enemy/english/male/take_him_out.mp3"			},
	{"Take Position",					"sound/enemy/english/male/take_position.mp3"			},
	{"Take them out",					"sound/enemy/english/male/take_them_out.mp3"			},
	{"Target has been eliminated",		"sound/enemy/english/male/target_eliminate.mp3"		},
	{"There he is",						"sound/enemy/english/male/there_he_is.mp3"				},
	{"Taking alot of fire",				"sound/enemy/english/male/underfire03.mp3"				},
	{"WATCH OUT",						"sound/enemy/english/male/watch_out.mp3"				},
	{"What are you doing here",			"sound/enemy/english/male/what_are_you_doing.mp3"		},
	{"What the...",						"sound/enemy/english/male/what_the.mp3"				},
	{"What was that",					"sound/enemy/english/male/what_was_that.mp3"			},
	{"Whats happening here",			"sound/enemy/english/male/whats_happening_here.mp3"	},
	{"Who are you",						"sound/enemy/english/male/who_are_you.mp3"				},
	{"You're not supposed to be here",	"sound/enemy/english/male/youre_not_supposed.mp3"		},
	{"Hey did you see that guy, He wet his pants ha haha ha ha..", "sound/npc/air1/guard01/01wetpants.mp3" },
	{"Hey honey..",						"sound/npc/air1/guard05/01honey.mp3"					},
	{"Huh I think i can help you",		"sound/npc/gladstone/arm1/03canhelp.mp3"				},
	{"If you look at me 1 more time, I swear I'll BLOW YOUR HEAD OFF", "sound/npc/air1/terrorists/blowheadoff.mp3" },
	{"How bout we see if you can dodge a BULLET", "sound/npc/air1/terrorists/dodgebullet.mp3"	},
	{"That was a close one",			"sound/npc/fritsch/closeone.mp3"						},
	{"What are you lookin at",			"sound/npc/air1/terrorists/lookingat.mp3"				},
	{"You scared?, You should be",		"sound/npc/air1/terrorists/scared02.mp3"				},
	{"Leave me alone",					"sound/npc/air1/woman01/01leaveme.mp3"					},
	{"Please just leave me be..",		"sound/npc/air1/woman01/02please.mp3"					},
	{"What? Please explain.. over",		"sound/npc/air4/hansen/04what.mp3"						},
	{"Attention",						"sound/npc/cem1/comm/01attention.mp3"					},
	{"Someone call 911",				"sound/npc/civ/english/male/call_911.mp3"				},
	{"Call the police",					"sound/npc/civ/english/male/callpolice.mp3"			},
	{"Comin thru..",					"sound/npc/civ/english/male/coming_thru.mp3"			},
	{"NICE SHOT",						"sound/npc/col8/washington/niceshot.mp3"				},
	{"Excuse me",						"sound/npc/civ/english/male/excuse_me.mp3"				},
	{"Don't hurt me",					"sound/npc/civ/english/male/dont_hurt.mp3"				},
	{"I don't understand",				"sound/npc/civ/english/male/dont_understand.mp3"		},
	{"I don't have any money",			"sound/npc/civ/english/male/no_money.mp3"				},
	{"Those men have guns",				"sound/npc/civ/english/male/thosemen.mp3"				},
	{"Take my wallet, Just don't hurt me", "sound/npc/civ/english/male/wallet.mp3"				},
	{"I'm unarmed",						"sound/npc/civ/english/male/unarmed.mp3"				},
	{"Watchout",						"sound/npc/civ/english/male/watchout.mp3"				},
	{"Pairup",							"sound/npc/col2/butch/pairup.mp3"						},
	{"Fanout",							"sound/npc/col2/butch/fanout.mp3"						},
	{"We got company, Take cover",		"sound/npc/col3/peterson/12company.mp3"				},
	{"Enemy has been neutralized, base camp is secure. over", "sound/npc/col3/radio/02enemy.mp3" },
	{"BANG! Your dead",					"sound/npc/col8/blakely/bang.mp3"						},
	{"DAMN! that was close",			"sound/npc/col8/blakely/close.mp3"						},
	{"GET DOWN",						"sound/npc/col8/blakely/getdown.mp3"					},
	{"Come get some",					"sound/npc/col8/blakely/getsome.mp3"					},
	{"Incoming",						"sound/npc/col8/blakely/incoming.mp3"					},
	{"Go cry to mama",					"sound/npc/col8/blakely/mama.mp3"						},
	{"We showed them",					"sound/npc/col8/blakely/showed.mp3"					},
	{"Take that",						"sound/npc/col8/blakely/takethat.mp3"					},
	{"That did it",						"sound/npc/col8/blakely/thatdidit.mp3"					},
	{"You want some of this",			"sound/npc/col8/blakely/wantsome.mp3"					},
	{"Yeah",							"sound/npc/col8/blakely/yeah.mp3"						},
	{"WOOOO",							"sound/npc/col8/blakely/woo.mp3"						},
	{"Don't worry girls it'll all be over soon", "sound/npc/col8/peterson/01girls.mp3"			},
	{"Cover my left",					"sound/npc/col8/peterson/coverleft.mp3"				},
	{"Cover my right",					"sound/npc/col8/peterson/coverright.mp3"				},
	{"Keep your eyes open boys",		"sound/npc/col8/peterson/eyesopen.mp3"					},
	{"Get moving",						"sound/npc/col8/peterson/getmoving02.mp3"				},
	{"Shudd-up man",					"sound/npc/col8/washington/02shuddup.mp3"				},
	{"Eat lead sukka",					"sound/npc/col8/washington/eatlead.mp3"				},
	{"Kickin ass, and takin names",		"sound/npc/col8/washington/takingnames.mp3"			},
	{"How'd you like that",				"sound/npc/col8/washington/youlike.mp3"				},
	{"WoOoOo",							"sound/npc/col8/washington/woo.mp3"					},
	{"O.K, lets do it",					"sound/npc/col9/pilot/03letsdoit.mp3"					},
	{"We can't take much more of this",	"sound/npc/col9/pilot/canttake.mp3"					},
	{NULL, NULL}
};

/*
=============
RPM_id
=============
*/
void RPM_id (int idnum)
{
	int             i = 0;
	char		    *ip = g_entities[idnum].client->nvSess.ip;
	char			*id = g_entities[idnum].client->pers.rpm_id;

		
	Q_strncpyz ( g_entities[idnum].client->nvSess.ip2, g_entities[idnum].client->nvSess.ip, 7 );
	// GodOrDevil - 09.02.06 - 08:18pm
#ifdef _SOF2_BOTS
	if(g_entities[idnum].r.svFlags & SVF_BOT){
		Com_sprintf(id, MAX_RPM_ID, "bot.%s", g_entities[idnum].client->nvSess.cleanName);
		return;
	}
#endif
	///End  - 09.02.06 - 08:18pm

	while(i < 6)	////And I did
	{
		*id++ = *ip++;
		i++;
	}

	*id++ = '\\';
    *id = '\0';

	strcat(id, g_entities[idnum].client->nvSess.cleanName);
	}

int RPM_NameListCheck(int num, const char *name, const char *file, gentity_t *ent, qboolean banCheck, qboolean admCheck, qboolean subnetCheck)
{
	int             len;
	fileHandle_t	f;
	char            buf[15000] = "\0";
	char			ip[MAX_IP] = "\0";
	char			listName[64] = "\0";
	char			*listP = listName;
	char			*bufP = buf;

	///03.15.05 - 09:45pm
	char			x[64] = "\0";
	char			a[64] = "\0";
	qboolean		banline = qfalse;
	ent = g_entities + num;
	///End  - 03.15.05 - 09:45pm

	len = trap_FS_FOpenFile(file, &f, FS_READ_TEXT);

	///if no file exists
	if (!f) {
		///make one
		len = trap_FS_FOpenFile(file, &f, FS_APPEND_TEXT);

		///if error occured abort
		if (!f) {
			RPM_FileError(ent, file);
			return -1;
		}
		///if not close file
		trap_FS_FCloseFile(f);
		len = trap_FS_FOpenFile(file, &f, FS_READ_TEXT);

		///if error occured abort
		if (!f) {
			RPM_FileError(ent, file);
			return -1;
		}
	}

	// GodOrDevil - 09.09.06 - 08:23pm #FileHandle Debug
	/// Just gives heads up if file handles arent being closed
#ifdef _DEBUG
	if (f >= 10){
		Com_Printf("%i of 64 max handles used\n", f);
		G_LogPrintf("!=!=!=!=!=!=!=!=WARNING=!=!=!=!=!=!=!=!=!=! File handles are not closing properly  [handle count: ( %i )].   Current file: %s\n", f, file);
	}
#endif
	///End  - 09.09.06 - 08:23pm

	if (len > 15000)	{
		len = 15000;
	}
	else if (len < 2)
	{
		return 0;
	}

	// GodOrDevil - 09.07.06 - 03:29pm
	memset( buf, 0, sizeof(buf) );
	///End  - 09.07.06 - 03:29pm

	trap_FS_Read( buf, len, f );
	buf[len] = '\0';
	trap_FS_FCloseFile( f );

	while( *bufP != '\0')
	{
		while(*bufP != '\n' && *bufP != '\0')
		{
			*listP++ = *bufP++;
			 if((banCheck || subnetCheck) && *bufP == '\\')
			 { 
				 *listP = '\0';
				 Q_strncpyz(ip, listName, MAX_IP);
				 listP = listName;
				 banline = qtrue;
				 bufP++;
			 }
		}

		*listP = '\0';
		listP = listName;

		// GodOrDevil - 03.15.05 - 09:35pm				
		if (admCheck)
		{
			memset(a, 0, sizeof(a-1));
			strcpy(a, va("%s:2", name ));
			if (!Q_stricmp(listName, a)) 
			{
				return 2;
			}	
			memset(a, 0, sizeof(a-1));
			strcpy(a, va("%s:3", name ));
			if (!Q_stricmp(listName, a)) 
			{
				return 3;
			}
			memset(a, 0, sizeof(a-1));
			strcpy(a, va("%s:4", name ));
			if (!Q_stricmp(listName, a))
			{
				return 4;
			} 
		}
		///Check For Ban
		else if(banCheck && banline)
		{
			if(!Q_stricmp(ip, name))
				return 1;
		}
		else if (subnetCheck && banline)
		{
			if(strlen(ip) > 3){
				Q_strncpyz (x, name, strlen(ip) + 1);
				if(!Q_stricmp(ip, x))
					return 1;
			}
		}
		///Check Other, Clan, nokick, banned name, etc..
		if (!Q_stricmp(listName, name) && !admCheck && !subnetCheck)
		{
			return 1;
		}
		///End - 05.19.05 - 06:02am

		while(*bufP == '\n')
		{
			bufP++;
		}
	}	
	return 0;	///if not on the list return with 0
}

/*
=================
RPM_ClientNumFromArg
=================

int RPM_ClientNumFromArg (gentity_t *ent, int argNum, const char* usage, const char* action, qboolean aliveOnly)
{
	char	arg[8] = "\0";
	int		num = -1;

	trap_Argv( argNum, arg, sizeof( arg ) );
	///
	//if(!Q_stricmp(arg, "f") && ent->client->sess.spectatorState == SPECTATOR_FOLLOW )
	//	num = ent->client->sess.spectatorClient; ///use command on client they are following
	//else
	///
	if (arg[0] >= '0' && arg[0] <= '9')
	{
		num = atoi( arg );
	}
	else if(ent && ent->client)
	{
		///
		if(!Q_stricmp(arg, "f") && ent->client->sess.spectatorState == SPECTATOR_FOLLOW )
			num = ent->client->sess.spectatorClient; ///use command on client they are following
		else {
		///
			trap_SendServerCommand( ent-g_entities, va("print \"Bad client slot: %s\n\"", arg));
			trap_SendServerCommand( ent-g_entities, va("print \"Usage: adm %s\n\"", usage));
			return -1;
		}
	}
	else
	{
		Com_Printf("Bad client slot: %s\n", arg);
		Com_Printf("Usage: rcon %s\n", usage);
		return -1;
	}

	//is it a valid it number
	if ( num < 0 || num >= g_maxclients.integer )
	{
		if(ent && ent->client)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Invalid client number %d.\n\"", num));
		}
		else
		{
			Com_Printf("Invalid client number %d.\n", num );
		}
		return -1;
	}

	//is the client connected
	if ( g_entities[num].client->pers.connected == CON_DISCONNECTED )
	{
		if(ent && ent->client)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"There is no client with the client number %d.\n\"", num));
		}
		else
		{
			Com_Printf("There is no client with the client number %d.\n", num);
		}
		return -1;
	}

	// GodOrDevil - 10.23.05 - 04:41am #Punish Self and other admins 
	if(ent && ent->client && strncmp(action, "alias", 5))
	{
		if(num == ent->s.number ) //&& ent->client->nvSess.admin < 3)	//If your Admin.. fuck yourself over if you want. (or add to clanlist etc..)
		{															//Decided to go back to normal.  No need for that.
			trap_SendServerCommand( ent-g_entities, va("print \"^3Why would you want to %s yourself?\n\"", action));
			return -1;
		}
		else if (g_entities[num].client->nvSess.admin ){
			// GodOrDevil - 01.10.06 - 03:39pm
			if(g_entities[num].client->nvSess.admin >= ent->client->nvSess.admin){
			///End  - 01.10.06 - 03:39pm
				trap_SendServerCommand( ent-g_entities, va("print \"^3You cannot %s other Admin's.\n\"", action));
				return -1;
			}
		}
		//02.04.05 - 09:25am #Pbox
		else if (ent->client->sess.penaltySeconds){
			trap_SendServerCommand( ent-g_entities, va("print \"^3You cannot use Admin commands in the Penalty Box.\n\""));	
			return qfalse;
		}
	}
		//End - 02.04.05 - 09:25am


	if(aliveOnly)
	{
		if ( G_IsClientDead ( g_entities[num].client ) )
		{
			if(ent && ent->client)
			{
				trap_SendServerCommand( ent-g_entities, va("print \"You cannot %s dead Players.\n\"", action));
			}
			else
			{
				Com_Printf("You cannot %s dead Players.\n", action);
			}
			return -1;
		}
		if (G_IsClientSpectating(g_entities[num].client))
		{
			if(ent && ent->client)
			{
				trap_SendServerCommand( ent-g_entities, va("print \"You cannot %s a Spectator.\n\"", action));
			}
			else
			{
				Com_Printf("You cannot %s a Spectator.\n", action);
			}
			return - 1;
		}
	}
#ifdef _DB_ENABLED
	if(g_enableDatabase.integer && (!g_entities[num].client->nvSess.dbID || !g_entities[num].client->nvSess.dbnameID))
	{
		if(ent && ent->client)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"^7Please wait until ^3%s ^7client fully connects.\n\"", g_entities[num].client->pers.netname));
		}
		else
		{
			Com_Printf("^7Please wait until ^3%s ^7client fully connects.\n", g_entities[num].client->pers.netname);
		}
		return -1;
	}
#endif
	return num;
}
*/
/*
===================
RPM_AddToList
===================
*/
int RPM_AddToList(const char *string, const char* file, const char* type, gentity_t* ent)
{
	int				len;
	fileHandle_t	f;

	len = trap_FS_FOpenFile( file, &f, FS_APPEND_TEXT ); //open our file for adding to

    if (!f)  //if error abort
	{
		if(ent && ent->client)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"^1Error opening %s File\n\"", type));
		}
		else
		{
			Com_Printf("^1Error opening %s File\n", type);
		}
	    return -1;
	}

	if(len > 15000 - (strlen(string) + 1))
	{
		trap_FS_FCloseFile(f);

		if(ent && ent->client)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"^3List full, Unable to add %s.\n\"", type));
		}
		else
		{
			Com_Printf("^3List full, Unable to add %s.\n", type);
		}
		return 0;
	}

	trap_FS_Write(string, strlen(string), f);
	trap_FS_Write("\n", 1, f);
	trap_FS_FCloseFile(f);

	return 1;
}

/*
==============
RPM_Tokens
==============
*/
void RPM_Tokens(gentity_t *ent, char *chatText, int mode)
{
	int  i = 0, n = 0;
	qboolean playedSound = qfalse, ghost = qfalse;
	char location[64] = "\0";
	char newText[MAX_SAY_TEXT] = "\0";
	char *newTextp;
	char *chatTextS;
	gentity_t *tent;
	qboolean text = qtrue;
	// GodOrDevil - 03.02.05 - 05:00pm #Tokens
	gitem_t* item;			
	// no need to continue if they are muted
	if(ent->client->sess.mute)
	{
		return;
	}
	//End - 03.02.05 - 05:00pm 
	newTextp = newText;
	chatTextS = chatText;

	while(*chatText && newTextp < newText + MAX_SAY_TEXT )
	{
	///CJJ - 1.1.2005 - Take the new chat tokens out.
	// GodOrDevil - 02.04.05 - 12:44pm - Added a few more		
		//if (*chatText == '!') {
		//	chatText++;
		//	///Admin Talk/Chat
		//	switch(*chatText){
		//		case 'a':
		//		case 'A':
		//			chatText++;
		//			switch(*chatText)
		//			{
		//			case 'c':
		//			case 'C':
		//			case 'T':
		//			case 't':
		//				chatText++;
		//				chatText++;
		//				G_Say( ent, NULL, ADM_CHAT, chatText );
		//				return;
		//			}
		//			break;
		//		case 's':
		//		case 'S':
		//			chatText++;
		//			switch(*chatText)
		//			{
		//			case 'c':
		//			case 'C':
		//				chatText++;
		//				chatText++;
		//				G_Say( ent, NULL, ADM_CHAT, chatText );
		//				return;
		//			}
		//			continue;
		//		case 'r':
		//		case 'R':
		//			chatText++;
		//			switch(*chatText)
		//			{
		//			case 'c':
		//			case 'C':
		//			case 'T':
		//			case 't':
		//				chatText++;
		//				chatText++;
		//			}
		//			continue;
		//		case 'c':
		//		case 'C':
		//			chatText++;
		//			switch(*chatText)
		//			{
		//			case 'c':
		//			case 'C':
		//			case 't':
		//			case 'T':
		//			case 'a':
		//			case 'A':
		//				chatText++;
		//				chatText++;
		//			}
		//			continue;
		//	}			
		//	chatText--;	
		//}		
		//if (*chatText == '/') {
		//	chatText++;
		//	switch(*chatText){
		//		case 'm':
		//		case 'M':
		//		case 'c':
		//		case 'C':
		//			chatText++;
		//			switch(*chatText)
		//			{
		//			case 'e':
		//			case 'E':					
		//			case 'p':
		//			case 'P':
		//				chatText++;
		//				chatText++;
		//			}
		//			continue;
		//		case 'b':
		//		case 'B':
		//			chatText++;
		//			switch(*chatText)
		//			{
		//			case 'c':
		//			case 'C':
		//				chatText++;
		//				chatText++;
		//			}
		//			continue;
		//	}	
		//	chatText--;
		//}	
		//End  - 11.01.05 - 03:43pm

		if (*chatText == '#')
		{
			chatText++;

			switch(*chatText)
			{
				case 'h':	//color coded health in numbers
					Q_strcat(newText, MAX_SAY_TEXT, va("%s%d^2",RPM_StatColor(ent->health), ent->health));
					//strcat(newText, va("%s%d^2",RPM_StatColor(ent->health), ent->health));
					chatText++;
					continue;

				case 'H':	//not color coded health in numbers
					Q_strcat(newText, MAX_SAY_TEXT, va("%d", ent->health));
					chatText++;
					continue;

				case 'a':	//color coded armor in numbers
					Q_strcat(newText, MAX_SAY_TEXT, va("%s%d^2",RPM_StatColor(ent->client->ps.stats[STAT_REALARMOR]), ent->client->ps.stats[STAT_REALARMOR]));
					chatText++;
					continue;

				case 'A':	//not color coded armor in numbers
					Q_strcat(newText, MAX_SAY_TEXT, va("%d", ent->client->ps.stats[STAT_REALARMOR]));
					chatText++;
					continue;

				case 'b':	//health in a bar format
					Q_strcat(newText, MAX_SAY_TEXT, va("^1%s^2", RPM_BarStat(ent->health)));
					chatText++;
					continue;

				case 'B':	//armor in a bar format
					Q_strcat(newText, MAX_SAY_TEXT, va("^-%s^2", RPM_BarStat(ent->client->ps.stats[STAT_REALARMOR])));
					chatText++;
					continue;

				case 'd':	//name of last client that hurt this client
				case 'D':
					Q_strcat(newText, MAX_SAY_TEXT, va("%s", g_entities[ent->client->pers.statinfo.lasthurtby].client->pers.netname));
					chatText++;
					continue;

				case 't':	//name of last person this client hurt
				case 'T':
					Q_strcat(newText, MAX_SAY_TEXT, va("%s", g_entities[ent->client->pers.statinfo.lastclient_hurt].client->pers.netname));
					chatText++;
					continue;

				case 'f':	//name of closest teamate
				case 'F':
					i = RPM_FindClosestTeammate(ent, qfalse);
					if(i > -1)
					{
						Q_strcat(newText, MAX_SAY_TEXT, va("%s", g_entities[i].client->pers.netname));
					}
					chatText++;
					continue;

				case 'l':	//your current location
				case 'L':
					if(Team_GetLocationMsg(ent, location, sizeof(location)))
					{
						Q_strcat(newText, MAX_SAY_TEXT, va("%s", location));
					}
					chatText++;
					continue;


			// GodOrDevil - 03.02.05 - 04:58pm #Tokens
			/*==================== New Tokens ====================*/
				case 'P': // primary weapon name
					//{
					//	if(level.pickupsDisabled)
					//	{
					//		// GodOrDevil - 09.02.06 - 05:00pm #bugFix - make sure we are allowed nades 
					//		///item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_PRIMARY][ent->client->pers.outfitting.items[OUTFITTING_GROUP_PRIMARY]]];
					//		///Q_strcat(newText, MAX_SAY_TEXT, bg_weaponNames[item->giTag]);
					//		int equipWeapon;
					//		for(equipWeapon = WP_USAS_12_SHOTGUN; equipWeapon < WP_RPG7_LAUNCHER + 1; equipWeapon++){
					//			if ( BG_IsWeaponAvailableForOutfitting ( equipWeapon, 2 )){
					//				item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_PRIMARY][ent->client->pers.outfitting.items[OUTFITTING_GROUP_PRIMARY]]];
					//				Q_strcat(newText, MAX_SAY_TEXT, bg_weaponNames[item->giTag]);
					//				equipWeapon = 55;
					//				break;
					//			}
					//		}
					//		if(equipWeapon != 55){
					//			Q_strcat(newText, MAX_SAY_TEXT, "none");
					//		}
					//		///End  - 09.02.06 - 05:02pm
					//	}
					//	else
						{ //if pickups are enabled there might be more than 1 primary weapon
							// in the players outfitting, just display the first 1 we find
							int weaponNum = WP_USAS_12_SHOTGUN;
							while(weaponNum < WP_M67_GRENADE)
							{
								if ( ent->client->ps.stats[STAT_WEAPONS]&(1<<weaponNum))
								{
									Q_strcat(newText, MAX_SAY_TEXT, bg_weaponNames[weaponNum]);
									break;
								}
								else if(weaponNum == WP_RPG7_LAUNCHER)
								{
									Q_strcat(newText, MAX_SAY_TEXT, "none");
									break;
								}
								weaponNum++;
							}
						}
						chatText++;
						continue;
					//}
					
				case 'p': // primary weapon short name
					//{
					//	if(level.pickupsDisabled)
					//	{
					//		// GodOrDevil - 09.02.06 - 05:00pm #bugFix - make sure we are allowed nades 
					//		///item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_PRIMARY][ent->client->pers.outfitting.items[OUTFITTING_GROUP_PRIMARY]]];
					//		///Q_strcat(newText, MAX_SAY_TEXT, rpm_weaponLongNames[item->giTag]);
					//		int equipWeapon;
					//		for(equipWeapon = WP_USAS_12_SHOTGUN; equipWeapon < WP_RPG7_LAUNCHER + 1; equipWeapon++){
					//			if ( BG_IsWeaponAvailableForOutfitting ( equipWeapon, 2 )){
					//				item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_PRIMARY][ent->client->pers.outfitting.items[OUTFITTING_GROUP_PRIMARY]]];
					//				Q_strcat(newText, MAX_SAY_TEXT, rpm_weaponLongNames[item->giTag]);
					//				equipWeapon = 55;
					//				break;
					//			}
					//		}
					//		if(equipWeapon != 55){
					//			Q_strcat(newText, MAX_SAY_TEXT, "none");
					//		}
					//		///End  - 09.02.06 - 05:02pm
					//	}
					//	else
						{	//if pickups are enabled there might be more than 1 primary weapon
							// in the players outfitting, just display the first 1 we find
							int weaponNum = WP_USAS_12_SHOTGUN;
							while(weaponNum < WP_M67_GRENADE)
							{
								if ( ent->client->ps.stats[STAT_WEAPONS]&(1<<weaponNum))
								{
									Q_strcat(newText, MAX_SAY_TEXT, rpm_weaponLongNames[weaponNum]);
									break;
								}
								else if(weaponNum == WP_RPG7_LAUNCHER)
								{
									Q_strcat(newText, MAX_SAY_TEXT, "none");
									break;
								}
								weaponNum++;
							}
						}
						chatText++;
						continue;
					//}

				case 'S': // secondary weapon name
					//{
					//	if(level.pickupsDisabled)
					//	{
					//		// GodOrDevil - 09.02.06 - 05:00pm #bugFix - make sure we are allowed nades 
					//		///item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_SECONDARY][ent->client->pers.outfitting.items[OUTFITTING_GROUP_SECONDARY]]];
					//		///Q_strcat(newText, MAX_SAY_TEXT, bg_weaponNames[item->giTag]);
					//		int equipWeapon;
					//		for(equipWeapon = WP_M590_SHOTGUN; equipWeapon < WP_M3A1_SUBMACHINEGUN + 1; equipWeapon++){
					//			if ( BG_IsWeaponAvailableForOutfitting ( equipWeapon, 2 )){
					//				item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_SECONDARY][ent->client->pers.outfitting.items[OUTFITTING_GROUP_SECONDARY]]];
					//				Q_strcat(newText, MAX_SAY_TEXT, bg_weaponNames[item->giTag]);
					//				equipWeapon = 55;
					//				break;
					//			}
					//		}
					//		if(equipWeapon != 55){
					//			Q_strcat(newText, MAX_SAY_TEXT, "none");
					//		}
					//		///End  - 09.02.06 - 05:02pm
					//	}
					//	else
						{	//if pickups are enabled there might be more than 1 secondary weapon
							// in the players outfitting, just display the first 1 we find
							int weaponNum = WP_M590_SHOTGUN;
							while(weaponNum < WP_USAS_12_SHOTGUN )
							{
								if ( ent->client->ps.stats[STAT_WEAPONS]&(1<<weaponNum))
								{
									Q_strcat(newText, MAX_SAY_TEXT, bg_weaponNames[weaponNum]);
									break;
								}
								else if(weaponNum == WP_M3A1_SUBMACHINEGUN)
								{
									Q_strcat(newText, MAX_SAY_TEXT, "none");
									break;
								}
								weaponNum++;
							}
						}
						chatText++;
						continue;
					//}
					
				case 's': // secondary weapon short name
					//{
					//	if(level.pickupsDisabled)
					//	{		
					//		// GodOrDevil - 09.02.06 - 05:00pm #bugFix - make sure we are allowed nades 
					//		///item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_SECONDARY][ent->client->pers.outfitting.items[OUTFITTING_GROUP_SECONDARY]]];
					//		///Q_strcat(newText, MAX_SAY_TEXT, rpm_weaponLongNames[item->giTag]);
					//		int equipWeapon;
					//		for(equipWeapon = WP_M590_SHOTGUN; equipWeapon < WP_M3A1_SUBMACHINEGUN + 1; equipWeapon++){
					//			if ( BG_IsWeaponAvailableForOutfitting ( equipWeapon, 2 )){
					//				item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_SECONDARY][ent->client->pers.outfitting.items[OUTFITTING_GROUP_SECONDARY]]];
					//				Q_strcat(newText, MAX_SAY_TEXT, rpm_weaponLongNames[item->giTag]);
					//				equipWeapon = 55;
					//				break;
					//			}
					//		}
					//		if(equipWeapon != 55){
					//			Q_strcat(newText, MAX_SAY_TEXT, "none");
					//		}
					//		///End  - 09.02.06 - 05:02pm
					//	}
					//	else
						{	//if pickups are enabled there might be more than 1 secondary weapon
							// in the players outfitting, just display the first 1 we find
							int weaponNum = WP_M590_SHOTGUN;
							while(weaponNum < WP_USAS_12_SHOTGUN)
							{
								if ( ent->client->ps.stats[STAT_WEAPONS]&(1<<weaponNum))
								{
									Q_strcat(newText, MAX_SAY_TEXT, rpm_weaponLongNames[weaponNum]);
									break;
								}
								else if(weaponNum == WP_M3A1_SUBMACHINEGUN)
								{
									Q_strcat(newText, MAX_SAY_TEXT, "none");
									break;
								}
								weaponNum++;
							}
						}
						chatText++;
						continue;
					//}

				case 'I': // pistol name long
					//{
					//	if(level.pickupsDisabled)
					//	{	
					//		// GodOrDevil - 09.02.06 - 05:00pm #bugFix - make sure we are allowed nades 
					//		///item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_PISTOL][ent->client->pers.outfitting.items[OUTFITTING_GROUP_PISTOL]]];
					//		///Q_strcat(newText, MAX_SAY_TEXT, bg_weaponNames[item->giTag]);
					//		int equipWeapon = 0;
					//		if ( BG_IsWeaponAvailableForOutfitting ( WP_M1911A1_PISTOL, 2 ) ||BG_IsWeaponAvailableForOutfitting ( WP_USSOCOM_PISTOL, 2 ) ){
					//			item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_PISTOL][ent->client->pers.outfitting.items[OUTFITTING_GROUP_PISTOL]]];
					//			Q_strcat(newText, MAX_SAY_TEXT, bg_weaponNames[item->giTag]);
					//				equipWeapon = 55;
					//				break;
					//		}
					//		if(equipWeapon != 55){
					//			Q_strcat(newText, MAX_SAY_TEXT, "none");
					//		}
					//		///End  - 09.02.06 - 05:02pm
					//	}
					//	else
						{   //if pickups are enabled there might be more than 1 pistol
							//in the players outfitting, just display the first 1 we find
							int weaponNum = WP_M1911A1_PISTOL;
							while(weaponNum < WP_M590_SHOTGUN)
							{
								if ( ent->client->ps.stats[STAT_WEAPONS]&(1<<weaponNum))
								{
									Q_strcat(newText, MAX_SAY_TEXT, bg_weaponNames[weaponNum]);
									break;
								}
								else if(weaponNum == WP_USSOCOM_PISTOL)
								{
									Q_strcat(newText, MAX_SAY_TEXT, "none");
									break;
								}
								weaponNum++;
							}
						}
						chatText++;
						continue;
					//}
					
				case 'i': // pistol name short
					//{
						//if(level.pickupsDisabled)
						//{
						//	// GodOrDevil - 09.02.06 - 05:00pm #bugFix - make sure we are allowed nades 
						//	///item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_PISTOL][ent->client->pers.outfitting.items[OUTFITTING_GROUP_PISTOL]]];
						//	///Q_strcat(newText, MAX_SAY_TEXT, rpm_weaponLongNames[item->giTag]);
						//	int equipWeapon = 0;
						//	if ( BG_IsWeaponAvailableForOutfitting ( WP_M1911A1_PISTOL, 2 ) ||BG_IsWeaponAvailableForOutfitting ( WP_USSOCOM_PISTOL, 2 ) ){
						//			item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_PISTOL][ent->client->pers.outfitting.items[OUTFITTING_GROUP_PISTOL]]];
						//			Q_strcat(newText, MAX_SAY_TEXT, rpm_weaponLongNames[item->giTag]);
						//			equipWeapon = 55;
						//			break;
						//	}
						//	if(equipWeapon != 55){
						//		Q_strcat(newText, MAX_SAY_TEXT, "none");
						//	}
						//	///End  - 09.02.06 - 05:02pm
						//}
						//else
						{	//if pickups are enabled there might be more than 1 pistol
							// in the players outfitting, just display the first 1 we find
							int weaponNum = WP_M1911A1_PISTOL;
							while(weaponNum < WP_M590_SHOTGUN)
							{
								if ( ent->client->ps.stats[STAT_WEAPONS]&(1<<weaponNum))
								{
									Q_strcat(newText, MAX_SAY_TEXT, rpm_weaponLongNames[weaponNum]);
									break;
								}
								else if(weaponNum == WP_USSOCOM_PISTOL)
								{
									Q_strcat(newText, MAX_SAY_TEXT, "none");
									break;
								}
								weaponNum++;
							}
						}
						chatText++;
						continue;
					//}

				case 'G': // grenade weapon name
					//{
					//	if(level.pickupsDisabled)
					//	{
					//		// GodOrDevil - 09.02.06 - 05:00pm #bugFix - make sure we are allowed nades 
					//		///item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_GRENADE][ent->client->pers.outfitting.items[OUTFITTING_GROUP_GRENADE]]];
					//		///Q_strcat(newText, MAX_SAY_TEXT, bg_weaponNames[item->giTag]);
					//		int equipWeapon;
					//		for(equipWeapon = WP_M67_GRENADE; equipWeapon < WP_M15_GRENADE + 1; equipWeapon++){
					//			if ( BG_IsWeaponAvailableForOutfitting ( equipWeapon, 2 )){
					//				item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_GRENADE][ent->client->pers.outfitting.items[OUTFITTING_GROUP_GRENADE]]];
					//				Q_strcat(newText, MAX_SAY_TEXT, bg_weaponNames[item->giTag]);
					//				equipWeapon = 55;
					//				break;
					//			}
					//		}
					//		if(equipWeapon != 55){
					//			Q_strcat(newText, MAX_SAY_TEXT, "none");
					//		}
					//		///End  - 09.02.06 - 05:02pm
					//	}
					//	else
						{	//if pickups are enabled there might be more than 1 grenade type
							 //in the players outfitting, just display the first 1 we find
							int weaponNum = WP_M67_GRENADE;
							while(weaponNum < WP_NUM_WEAPONS)
							{
								if ( ent->client->ps.stats[STAT_WEAPONS]&(1<<weaponNum))
								{
									Q_strcat(newText, MAX_SAY_TEXT, bg_weaponNames[weaponNum]);
									break;
								}
								else if(weaponNum == WP_M15_GRENADE)
								{
									Q_strcat(newText, MAX_SAY_TEXT, "none");
									break;
								}
								weaponNum++;
							}
						}
						chatText++;
						continue;
					//}
					
				case 'g': // grenade weapon short name
					//{
					//	if(level.pickupsDisabled)
					//	{	
						//	// GodOrDevil - 09.02.06 - 05:00pm #bugFix - make sure we are allowed nades 
						//	///item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_GRENADE][ent->client->pers.outfitting.items[OUTFITTING_GROUP_GRENADE]]];
						//	///Q_strcat(newText, MAX_SAY_TEXT, rpm_weaponLongNames[item->giTag]);
						//	int equipWeapon;
						//	for(equipWeapon = WP_M67_GRENADE; equipWeapon < WP_M15_GRENADE + 1; equipWeapon++){
						//		if ( BG_IsWeaponAvailableForOutfitting ( equipWeapon, 2 )){
						//			item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_GRENADE][ent->client->pers.outfitting.items[OUTFITTING_GROUP_GRENADE]]];
						//			Q_strcat(newText, MAX_SAY_TEXT, rpm_weaponLongNames[item->giTag]);
						//			equipWeapon = 55;
						//			break;
						//		}
						//	}
						//	if(equipWeapon != 55){
						//		Q_strcat(newText, MAX_SAY_TEXT, "none");
						//	}
						//	///End  - 09.02.06 - 05:02pm
						//}
						//else
						{	//if pickups are enabled there might be more than 1 grenade type
							// in the players outfitting, just display the first 1 we find
							int weaponNum = WP_M67_GRENADE;
							while(weaponNum < WP_NUM_WEAPONS)
							{
								if ( ent->client->ps.stats[STAT_WEAPONS]&(1<<weaponNum))
								{
									Q_strcat(newText, MAX_SAY_TEXT, rpm_weaponLongNames[weaponNum]);
									break;
								}
								else if(weaponNum == WP_M15_GRENADE)
								{
									Q_strcat(newText, MAX_SAY_TEXT, "none");
									break;
								}
								weaponNum++;
							}
						}
						chatText++;
						continue;
					//}

				case 'E': // name of current equipment
				case 'e':
					item = &bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_ACCESSORY][ent->client->pers.outfitting.items[OUTFITTING_GROUP_ACCESSORY]]];
					Q_strcat(newText, MAX_SAY_TEXT, item->pickup_name);
					chatText++;
					continue;

				case 'Z':
				case 'z': // name of the last player to connect
					Q_strcat(newText, MAX_SAY_TEXT, g_entities[level.lastConnectedClient].client->pers.netname );
					chatText++;
					continue;
				// GodOrDevil
#ifdef _NEW_MAP_SETUP

				case 'n':
				case 'N':
					if(level.doRandom && level.randMapNum && level.randGameType[0] && level.randMap[0])
					{
						Q_strcat(newText, MAX_SAY_TEXT, va( "%s %s", level.randMap, Q_strupr(level.randGameType)));
						chatText++;
					}
					else
					{
						Q_strcat(newText, MAX_SAY_TEXT, va( "%s %s", level.nextMap, Q_strupr(level.nextGameType)));
						chatText++;
					}
					continue;
				case 'r':
				case 'R':
					if(level.randMapNum && level.randGameType[0] && level.randMap[0])
					{
						Q_strcat(newText, MAX_SAY_TEXT, va( "%s %s", level.randMap, Q_strupr(level.randGameType)));
						chatText++;
					}
					continue;
#endif
					// End
				default:
					// check for a number from 0 to 63 if it is then display the name of the player with that id number
					if(*chatText >= '0' && *chatText <= '9')
					{
						n = atoi(chatText);
						if(n > -1 && n < MAX_CLIENTS)
						{
							tent = g_entities + n;
							// if they are a connected client inject their name
							if(tent && tent->client)
							{
								Q_strcat(newText, MAX_SAY_TEXT, tent->client->pers.netname);
							}
							// if not...
							else
							{
								Q_strcat(newText, MAX_SAY_TEXT, "none");
							}
							// if it was a 2 number client slot advance the pointer 1 extra
							if ( n > 9)
							{
								chatText++;
							}
							chatText++;
							continue;
						}
					}
					// if not then write the original text and continue on
					chatText--;
			}
		}

		/*#REMOVED
				default:
					chatText--;
			}
		}
		*/
		/*==================== End New Tokens ====================*/

		if((*chatText == '@' || *chatText == '!') && !playedSound)
		{
			if ( g_voiceFloodCount.integer )
			{
				if ( ent->client->voiceFloodPenalty )
				{
					if ( ent->client->voiceFloodPenalty > level.time )
					{
						playedSound = qtrue;
					//	continue;
					}
					else
					{
					ent->client->voiceFloodPenalty = 0;
					}
				}
				if ( ent->client->voiceFloodCount >= g_voiceFloodCount.integer )
				{
					ent->client->voiceFloodCount = 0;
					ent->client->voiceFloodTimer = 0;
					ent->client->voiceFloodPenalty = level.time + g_voiceFloodPenalty.integer * 1000;
					trap_SendServerCommand( ent-g_entities, va("print \"Voice chat flooded, you will be able use voice chats again in (%d) seconds\n\"", g_voiceFloodPenalty.integer ) );
					playedSound = qtrue;
				//	continue;
				}
			}
			if(*chatText == '!') {
				text = qfalse;
			}
			chatText++;
			i = 0;
			n = atoi(chatText) - 1;
			if(n > -1 && n < MAX_RPM_CHATS && chatSounds[n].sound && *chatSounds[n].text) {
				i = chatSounds[n].sound;
				if(text)
					Q_strcat(newText, MAX_SAY_TEXT, chatSounds[n].text);
				chatText++;
			}
			else
			{
				chatText--;
			}
			if(i)
			{
				if( level.gametypeData->respawnType == RT_NONE)	
				{
					if ( G_IsClientDead ( ent->client ) )
							ghost = qtrue;
				}
				// GodOrDevil - 09.01.06 - 08:19pm
				///if(g_globalVoiceChat.integer && mode == SAY_TEAM){
				if(g_globalVoiceChat.integer  && !playedSound)		
				{
					if(mode == SAY_TEAM) 
					{
				///End  - 09.01.06 - 08:19pm
						for (n = 0; n < level.maxclients; n++) 
						{
							tent = &g_entities[n];

							if (!tent || !tent->inuse || !tent->client)
								continue;
							if ( !OnSameTeam(ent, tent) )
								continue;
							if (!G_IsClientDead ( tent->client ) && !G_IsClientSpectating( tent->client) && ghost)
								continue;
							RPM_ClientSound(tent, i);
						}
						ent->client->voiceFloodCount++;
						playedSound = qtrue;
					}	
					// GodOrDevil - 09.01.06 - 08:18pm #voiceToken
					///else if(g_globalVoiceChat.integer && mode == SAY_ALL) {
					else if(mode != SAY_TELL  && !playedSound )
					{
					///End  - 09.01.06 - 08:18pm
						if(ghost) 
						{
							for (n = 0; n < level.maxclients; n++)
							{
								tent = &g_entities[n];
								if (!tent || !tent->inuse || !tent->client)
									continue;
								if (!G_IsClientDead ( tent->client ) && !G_IsClientSpectating( tent->client))
									continue;
								RPM_ClientSound(tent, i);
							}
						}
						else if(!playedSound)
						{
							RPM_GlobalSound(i);
						}							
						ent->client->voiceFloodCount++;
						playedSound = qtrue;
					}
				// GodOrDevil - 09.01.06 - 08:18pm #voiceToken
				///else if(!g_globalVoiceChat.integer)	{
				}
				else if (!playedSound)
				{
				///End  - 09.01.06 - 08:18pm
					RPM_ClientSound(ent, i);
					playedSound = qtrue;
				} 
				while (*chatText >= '0' && *chatText <= '9')
					chatText++;
			}
		}

		while(*newTextp)
		{
			newTextp++;
		}

		if(*chatText)
		{
			*newTextp++ = *chatText++;
		}
		*newTextp = '\0';
	}

	chatText = chatTextS;
	Q_strncpyz (chatText, newText, MAX_SAY_TEXT);
	return;
}

/*
=============
RPM_BarStat		//called for heath and armor bar tokens
=============
*/
char *RPM_BarStat(int stat)
{
	char	*bar;

	stat /= 10; //only have 10 "-"'s in our bar so make the stat 10 or less

	if(stat >= 10)
	{
		bar = "----------";
		return bar;
	}

	switch(stat)
	{
		case 9:
			bar = "---------^0-";
			break;
		case 8:
			bar = "--------^0--";
			break;
		case 7:
			bar = "-------^0---";
			break;
		case 6:
			bar = "------^0----";
			break;
		case 5:
			bar = "-----^0-----";
			break;
		case 4:
			bar = "----^0------";
			break;
		case 3:
			bar = "---^0-------";
			break;
		case 2:
			bar= "--^0--------";
			break;
		case 1:
			bar = "-^0---------";
			break;
		default:
			bar = "^0----------";
	}
	return bar;
}

/*
===============
RPM_StatColor	// called for color coded health and armor
===============
*/
char *RPM_StatColor(int stat)
{
	char *color;

	if(stat>=80)
	{
		color = "^g";
	}
	else if(stat>=70)
	{
		color = "^&";
	}
	else if(stat>=60)
	{
		color = "^3";
	}
	else if(stat>=50)
	{
		color = "^b";
	}
	else if(stat>=40)
	{
		color = "^d";
	}
	else if(stat>=30)
	{
		color = "^@";
	}
	else if(stat>=20)
	{
		color = "^6";
	}
	else
	{
		color = "^1";
	}

	return color;
}

/*
=====================
RPM_FindClosestTeammate
=====================
*/
int	RPM_FindClosestTeammate(gentity_t *ent, qboolean bot)
{
	int			i;
	int			client = -1;
	gentity_t*  other;
	float		dist, bestDist = 99999;
	vec3_t	    diff;

	for ( i = 0; i < level.numConnectedClients; i ++ )
	{
		other = &g_entities[ level.sortedClients[i] ];

		if ( other->client->pers.connected != CON_CONNECTED )
		{
			continue;
		}

		if ( other == ent)
		{
			continue;
		}

		if ( G_IsClientSpectating ( other->client ) || G_IsClientDead ( other->client ) )
		{
			continue;
		}

		if ( !OnSameTeam(ent, other) )
		{
			continue;
		}

		if(bot)
		{
			if (!other->r.svFlags & SVF_BOT)
			{
				continue;
			}
		}

		VectorSubtract ( other->r.currentOrigin, ent->r.currentOrigin, diff );
		dist = VectorLength(diff);

		//is this person closer than the last?
		if ( dist < bestDist)
		{
			client = other->s.number;
			bestDist = VectorLength(diff);
		}
	}

	return client;
}

#ifdef _COMPMODE
void RPM_Pause (gentity_t *adm)
{
	int			i;
	gentity_t	*ent;
//	gentity_t	*tent;

	if ( level.intermissiontime || level.pause || level.intermissionQueued)
	{
		return;
	}

	level.pause = 6;

	for (i=0 ; i< level.maxclients ; i++)
	{
		ent = g_entities + i;
		if (!ent->inuse)
		{
			continue;
		}
		ent->client->ps.pm_type = PM_INTERMISSION;
	}
	// send the current scoring to all clients
	SendScoreboardMessageToAllClients();

	RPM_GlobalSound(G_SoundIndex("sound/misc/events/buzz02.wav"));
/*	tent = G_TempEntity( vec3_origin, EV_GLOBAL_SOUND );
	tent->s.eventParm = G_SoundIndex("sound/misc/events/buzz02.wav");
	tent->r.svFlags = SVF_BROADCAST;
*/
}

void RPM_Unpause (gentity_t *adm)
{
	int			i;
	gentity_t	*ent;
//	gentity_t	*tent;

	if(!level.pause)
	{
		trap_SendServerCommand( adm-g_entities, va("print \"The game is not currently paused.\n\"") );
		return;
	}

	if(level.time >= level.unpausetime + 1000)
	{
		level.unpausetime = level.time;
		level.pause--;

		trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@^1Resume ^7game in: %d ^4seconds", level.time + 2000, level.pause ) );

		RPM_GlobalSound(G_SoundIndex("sound/misc/events/buzz02.wav"));
/*		tent = G_TempEntity( vec3_origin, EV_GLOBAL_SOUND );
		tent->s.eventParm = G_SoundIndex("sound/misc/events/buzz02.wav");
		tent->r.svFlags = SVF_BROADCAST;
*/
		if(!level.pause)
		{
			level.unpausetime = 0;
			
			// GodOrDevil - 08.30.06 - 03:06pm #reset clients (scoreboard) display time
			trap_SetConfigstring( CS_LEVEL_START_TIME, va("%i", level.startTime ) );
			trap_SetConfigstring ( CS_GAMETYPE_TIMER, va("%i", level.gametypeRoundTime) );
			///End  - 08.30.06 - 03:06pm

			if( level.gametypeData->respawnType == RT_INTERVAL)
			{
				level.gametypeRespawnTime[TEAM_RED]  = level.time + g_respawnInterval.integer * 1000;
				level.gametypeRespawnTime[TEAM_BLUE] = level.time + g_respawnInterval.integer * 1000;
				level.gametypeRespawnTime[TEAM_FREE] = level.time + g_respawnInterval.integer * 1000;
			}

			for (i=0 ; i< level.maxclients ; i++)
			{
				ent = g_entities + i;
				if (!ent->inuse)
				{
					continue;
				}
				ent->client->ps.pm_type = PM_NORMAL;
			}
			trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@GO!", level.time + 2000) );
		}
	}
}
#endif
/*
=============
RPM_Motd
=============
*/
void RPM_Motd (gentity_t *ent)
{
	//	int		n = 0;
    char    gmotd[1024] = "\0";
	char	motd[1024] = "\0";
	char	*s = motd;
	char	*gs = gmotd;
	char	*name = ent->client->pers.netname;
	// standard header spam
	char	*header1 = va("@%s ^7- ^3%s\n", RPM_VERSION_STRING_COLORED, RPM_VERSION_DATE );
	// GodOrDevil - 09.26.06 - 03:58am #motd
	char	*header2 = va("^6*^3* ^2%s ^3*^6*\n", nv_gtName.string);
	//char	*header2 = "";
	///End  - 09.26.06 - 03:58am
	
#ifdef _STUPID_SHIT	
	if(strstr(name, g_friend1.string) && g_friend1.string[0] > '\0')
	{
		Com_sprintf(gmotd, 1024, "%s%s%s\n%s\n%s\n%s\n%s\n%s\n",
			header1,
			header2,
			g_friend1_motd1.string,
			g_friend1_motd2.string,
			g_friend1_motd3.string,
			g_friend1_motd4.string,
			g_friend1_motd5.string,
			g_friend1_motd6.string);
	}
	else if(strstr(name, g_friend2.string) && g_friend2.string[0] > '\0')
	{
		Com_sprintf(gmotd, 1024, "%s%s%s\n%s\n%s\n%s\n%s\n%s\n",
			header1,
			header2,
			g_friend2_motd1.string,
			g_friend2_motd2.string,
			g_friend2_motd3.string,
			g_friend2_motd4.string,
			g_friend2_motd5.string,
			g_friend2_motd6.string);
	}
	else if(strstr(name, g_friend3.string) && g_friend3.string[0] > '\0')
	{
		Com_sprintf(gmotd, 1024, "%s%s%s\n%s\n%s\n%s\n%s\n%s\n",
			header1,
			header2,
			g_friend3_motd1.string,
			g_friend3_motd2.string,
			g_friend3_motd3.string,
			g_friend3_motd4.string,
			g_friend3_motd5.string,
			g_friend3_motd6.string);
	}
	else if(strstr(name, g_friend4.string) && g_friend4.string[0] > '\0')
	{
		Com_sprintf(gmotd, 1024, "%s%s%s\n%s\n%s\n%s\n%s\n%s\n",
			header1,
			header2,
			g_friend4_motd1.string,
			g_friend4_motd2.string,
			g_friend4_motd3.string,
			g_friend4_motd4.string,
			g_friend4_motd5.string,
			g_friend4_motd6.string);
	}
	else
#endif

	{
		Com_sprintf(gmotd, 1024, "%s%s%s\n%s\n%s\n%s\n%s\n%s\n",
			header1,
			header2,
			server_motd1.string,
			server_motd2.string,
			server_motd3.string,
			server_motd4.string,
			server_motd5.string,
			server_motd6.string);
	}
	//{
	//	Com_sprintf(gmotd, 1024, "%s\n%s\n%s\n%s\n%s\n%s\n",
	//		server_motd1.string,
	//		server_motd2.string,
	//		server_motd3.string,
	//		server_motd4.string,
	//		server_motd5.string,
	//		server_motd6.string
	//		);
	//}
	gmotd[strlen(gmotd)+1] = '\0';

	while(*gs)
	{
		if(*gs == '#')
		{
			if(*++gs == 'u')
			{
				strcat(motd, name);
				strcat(motd, "^7");
				s += strlen(name) +2;
				gs++;
			}
			else
			{
				gs--;
			}
		}

		*s++ = *gs++;
	}

	*s = '\0';
	trap_SendServerCommand(ent-g_entities, va("cp \"%s\"", motd));
	//CPx( ent->s.number, va("cp \"%s^-* ^3%s^- *\n^,------------------------------------------------------\n\n%s\n\n^2Type ^3'!help' ^2in ^3chat ^2to see this message again!\n\"", motd, gtInfo[gt_customType.integer].longName, gtInfo[gt_customType.integer].help));
}
//#endif

/*
=============
RPM_ClientSound
=============
*/
void RPM_ClientSound (gentity_t *ent, int soundIndex)
{
	gentity_t *tent;

	tent = G_TempEntity( ent->r.currentOrigin, EV_GLOBAL_SOUND);
	tent->s.eventParm = soundIndex;
	tent->r.svFlags |= SVF_SINGLECLIENT;
	tent->r.singleClient = ent->s.number;
}

/*
=============
RPM_GlobalSound
=============
*/
void RPM_GlobalSound (int soundIndex)
{
	gentity_t *tent;

	tent = G_TempEntity( vec3_origin, EV_GLOBAL_SOUND );
	tent->s.eventParm = soundIndex;
	tent->r.svFlags = SVF_BROADCAST;
}

/*
=============
RPM_Awards
=============
*/
void RPM_Awards(void)
{
	static int overallScore = 0, headshots = 0, damage = 0, explosiveKills = 0, knifeKills = 0;
	static float  accuracy = 0, ratio = 0;
	unsigned int i, numPlayers = 0;
	//I changed this to an integer so players who connect
	//faster than the rest don't bump up the avg. time
	//float avgtime = 0.0, playerTime = 0.0;
	int avgtime = 0, playerTime = 0;
	statinfo_t     *stat;
	static gentity_t *bestOverall = NULL, *headshooter = NULL, *killer = NULL;
	static gentity_t *accurate = NULL, *bestRatio = NULL, *explosive = NULL, *knifer = NULL;
	gentity_t *ent;

	// GodOrDevil - 01.12.06 - 09:26pm #TEST - for award debug :p
	//char	*a, *b;
	///End  - 01.12.06 - 09:26pm

	if(!level.awardTime)
	{
		//find the average time player by all connected clients
		for (i=0; i < level.maxclients ; i++)
		{
			ent = g_entities + i;
			if (!ent->inuse)
			{
				continue;
			}
			numPlayers++;
			//avgtime += ((level.time - ent->client->pers.enterTime) - ent->client->sess.totalSpectatorTime) / 60000;
			avgtime += (ent->client->nvSess.teamTime[TEAM_RED] + ent->client->nvSess.teamTime[TEAM_BLUE] + ent->client->nvSess.teamTime[TEAM_FREE]) / 60;

		}
		//incase timelimit runs out with nobody in the server
		//I'll use an "if" here
		if(numPlayers)
		{
			avgtime /= numPlayers;
			//Com_Printf("^3Averagetime %d\n", avgtime);
		}

		for (i = 0; i < level.maxclients ; i++)
		{
			ent = g_entities + i;
			if (!ent->inuse)
			{
				continue;
			}

			stat = &ent->client->pers.statinfo;

			//playerTime = ((level.time - ent->client->pers.enterTime) - ent->client->sess.totalSpectatorTime) / 60000;
			playerTime = (ent->client->nvSess.teamTime[TEAM_RED] + ent->client->nvSess.teamTime[TEAM_BLUE] + ent->client->nvSess.teamTime[TEAM_FREE]) / 60;
#ifdef _DEBUG
			//Com_Printf("%s playtime %d, Level time: %d, Enter Time: %d SPECTIME: %d\n", ent->client->pers.netname, playerTime, level.time, ent->client->pers.enterTime, ent->client->sess.totalSpectatorTime);
#endif
			// GodOrDevil - 1.20.2005 - #Version 0.5 compatible. Right?
			if(ent->client->sess.rpmClient >= 0.5)
//			if(ent->client->sess.rpmClient >= RPM_VERSION)
			{
				trap_SendServerCommand( i, va("stats %i %i %i %i %.2f %i %i %i %i",
				playerTime,
				//(((level.time - ent->client->pers.enterTime) - ent->client->sess.totalSpectatorTime) % 60000) / 1000,
				((ent->client->nvSess.teamTime[TEAM_RED] + ent->client->nvSess.teamTime[TEAM_BLUE] + ent->client->nvSess.teamTime[TEAM_FREE]) % 60),
				stat->damageDone,
				stat->damageTaken,
				stat->ratio,
				stat->shotcount,
				stat->hitcount,
				stat->explosiveKills,
				stat->knifeKills));
			}

			//this will remove the SPECTATOR Press ESC etc.. from the specs screen
			if(ent->client->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR)
			{
				ent->client->ps.persistant[PERS_TEAM] = TEAM_FREE;
			}

			//Make sure they can't move
			ent->client->ps.pm_type = PM_FREEZE;

			//This will remove the HUD icons etc.. from the players screen
			ent->client->ps.stats[STAT_HEALTH] = -1;

#ifdef _DB_ENABLED		
			if(playerTime < 1 || ent->client->pers.statinfo.shotcount < 10)
			{
				continue;
			}

			if(ent->client->pers.statinfo.shotcount < 10) // at least play a little instead of whoring the acc award
			{
				stat->overallScore = ent->client->sess.score + (int)(100 * (stat->accuracy + stat->ratio)) + (stat->damageDone - stat->damageTaken) + ((ent->client->pers.statinfo.itemCaps + ent->client->pers.statinfo.itemCaps ) * 10);
			}
			else
			{
				stat->overallScore = ent->client->sess.score + (stat->damageDone - stat->damageTaken) + ((ent->client->pers.statinfo.itemCaps + ent->client->pers.statinfo.itemCaps ) * 10);
			}	
#else
			stat->overallScore = ent->client->sess.score + (int)(100 * (stat->accuracy + stat->ratio)) + (stat->damageDone - stat->damageTaken);
#endif

			if(stat->overallScore > overallScore)
			{
				overallScore = stat->overallScore;
				bestOverall = ent;
			}
			else if(stat->overallScore == overallScore && overallScore)
			{
				//if they got the same amout of points in less time
				//make them mvp
				if(ent->client->pers.enterTime > bestOverall->client->pers.enterTime)
				{
					overallScore = stat->overallScore;
					bestOverall = ent;
				}
			}
			

			if(stat->headShotKills > headshots)
			{
				headshots = stat->headShotKills;
				headshooter = ent;
			}
			else if(stat->headShotKills == headshots && headshots)
			{
				if(ent->client->pers.enterTime > headshooter->client->pers.enterTime)
				{
					headshots = stat->headShotKills;
					headshooter = ent;
				}
			}

			if(stat->damageDone > damage)
			{
				damage = stat->damageDone;
				killer = ent;
			}
			else if(stat->damageDone == damage && damage)
			{
				if(ent->client->pers.enterTime > killer->client->pers.enterTime)
				{
					damage = stat->damageDone;
					killer = ent;
				}
			}

			if(stat->explosiveKills > explosiveKills)
			{
				explosiveKills = stat->explosiveKills;
				explosive = ent;
			}
			else if(stat->explosiveKills == explosiveKills && explosiveKills)
			{
				if(ent->client->pers.enterTime > explosive->client->pers.enterTime)
				{
					explosiveKills = stat->explosiveKills;
					explosive = ent;
				}
			}

			if(stat->knifeKills > knifeKills )
			{
				knifeKills = stat->knifeKills;
				knifer = ent;
			}
			else if(stat->knifeKills == knifeKills && knifeKills)
			{
				if(ent->client->pers.enterTime > knifer->client->pers.enterTime)
				{
					knifeKills = stat->knifeKills;
					knifer = ent;
				}
			}

			//The awards below here are time sensitive so players
			//with 1/0 for ratio etc.. dont get the award
			if(playerTime < avgtime)
			{
				continue;
			}	

			if(ent->client->pers.statinfo.shotcount > 10)
			{
				if(stat->accuracy > accuracy)
				{
					if(ent->client->pers.statinfo.shotcount > 10)
					{
						accuracy = stat->accuracy;
						accurate = ent;
					}
				}
				else if(stat->accuracy == accuracy && accuracy)
				{
					//if this player held the accuracy longer give him the award
					if(ent->client->pers.enterTime < accurate->client->pers.enterTime)
					{
						accuracy = stat->accuracy;
						accurate = ent;
					}
				}
			}

			if(stat->ratio > ratio)
			{
				ratio = stat->ratio;
				bestRatio = ent;
			}
			else if(stat->ratio == ratio && ratio)
			{
				if(ent->client->pers.enterTime < bestRatio->client->pers.enterTime)
				{
					ratio = stat->ratio;
					bestRatio = ent;
				}
			}
		}
	
#ifdef _DB_ENABLED
		if(bestOverall != NULL)
			g_entities[bestOverall->s.number].client->pers.statinfo.awards[0] = 1;
		if(headshooter != NULL)
			g_entities[headshooter->s.number].client->pers.statinfo.awards[1] = 1;
		if(killer != NULL)
			g_entities[killer->s.number].client->pers.statinfo.awards[2] = 1;
		if(accurate != NULL)
			g_entities[accurate->s.number].client->pers.statinfo.awards[3] = 1;
		if(bestRatio != NULL)
			g_entities[bestRatio->s.number].client->pers.statinfo.awards[4] = 1;
		if(explosive != NULL)
			g_entities[explosive->s.number].client->pers.statinfo.awards[5] = 1;
		if(knifer != NULL)
			g_entities[knifer->s.number].client->pers.statinfo.awards[6] = 1;

		if(!level.warmupTime)
		{
			NV_doGameStats();
		}
#endif

	}
	
	for (i = 0; i < level.maxclients ; i++)
	{
		ent = g_entities + i;

		if (!ent->inuse)
		{
			continue;
		}
		
//#ifdef _DB_ENABLED	
//		if(!level.awardTime && !level.warmupTime)
//		{
//			NV_SaveClientStats(ent);
//		}
//#endif
		// GodOrDevil - 01.12.06 - 03:03pm - dont send to bots
#ifdef _SOF2_BOTS
		if (ent->r.svFlags & SVF_BOT)
			continue;
#endif
		///End  - 01.12.06 - 03:03pm

		// GodOrDevil - #Version 0.5 Had awards too #Version
		///if(ent->client->sess.rpmClient < RPM_VERSION)
		if(ent->client->sess.rpmClient < 0.5)		
		{	
			// GodOrDevil - 01.12.06 - 09:21pm
			///CJJ - 2.5.2005 - Added a text based awards system for older RPM clients
			///We should spam them instead
			///trap_SendServerCommand( i, va("cp \"You are ^1NOT ^7using ^1R^4P^3M^7 %s.\nYou cannot see the awards without it.\nGet the ^$NEWEST ^7[SC] ^4VERSION ^7at\n^1http://clanforums.ath.cx/\n\nFor more information check out the\n^1R^4P^3M^7 OFFICIAL SITE: ^1www.rpm-mod.tk\n\"", RPM_VERSION_STRING));
			trap_SendServerCommand( i, va("cp \"^1Best Overall: ^7%s - %i\n^1Headshots: ^7%s - %i\n^1Killer: ^7%s - %i\n^1Accurate: ^7%s - %.2f percent\n^1Best Ratio: ^7%s - %.2f\n^1Nades: ^7%s - %i detonated\n^1Knifer: ^7%s - %i shanked",
				bestOverall != NULL ? g_entities[bestOverall->s.number].client->pers.netname:"NONE",
				overallScore,
				headshooter != NULL ? g_entities[headshooter->s.number].client->pers.netname:"NONE",
				headshots,
				killer != NULL ? g_entities[killer->s.number].client->pers.netname:"NONE",
				damage,
				accurate != NULL ? g_entities[accurate->s.number].client->pers.netname:"NONE",
				accuracy,
				bestRatio != NULL ? g_entities[bestRatio->s.number].client->pers.netname:"NONE",
				ratio,
				explosive != NULL ? g_entities[explosive->s.number].client->pers.netname:"NONE",
				explosiveKills,
				knifer != NULL ? g_entities[knifer->s.number].client->pers.netname:"NONE",
				knifeKills ));
			continue;
		}
		else if(!level.awardTime)
		{
//#ifdef Q3_VM
			///End  - 01.12.06 - 09:23pm
			trap_SendServerCommand( -1, va("awards %i %i %i %i %i %i %i %.2f %i %.2f %i %i %i %i", 
				//bestOverall->s.number,
				bestOverall != NULL?bestOverall->s.number:-1,
				overallScore,
				//headshooter->s.number,
				headshooter != NULL?headshooter->s.number:-1,
				headshots, 
				//killer->s.number, 
				killer != NULL?killer->s.number:-1,
				damage, 
				//accurate->s.number, 
				accurate != NULL?accurate->s.number:-1,
				accuracy, 
				//bestRatio->s.number, 
				bestRatio != NULL?bestRatio->s.number:-1,
				ratio, 
				//explosive->s.number,
				explosive != NULL?explosive->s.number:-1,
				explosiveKills,	
				//knifer->s.number, 
				knifer != NULL?knifer->s.number:-1,
				knifeKills 
				));
//#endif
		}
		
	}
	

}

#ifndef _NV_ADMIN
void RPM_Print_File (gentity_t *ent, char *file)
{
	int             len = 0;
	fileHandle_t	f;
	char            buf[15000] = "\0";
	char			packet[512];
	char			*bufP = buf;

	len = trap_FS_FOpenFile( file, &f, FS_READ_TEXT);

	if (!f)
	{
		len = trap_FS_FOpenFile( file, &f, FS_APPEND_TEXT);

		if (!f)
		{
			RPM_FileError(ent, file);
			return;
		}

		trap_FS_FCloseFile( f );

		len = trap_FS_FOpenFile( file, &f, FS_READ_TEXT);

		if (!f)
		{
			RPM_FileError(ent, file);
			return;
		}
	}

	if(len > 15000)
	{
		len = 15000;
	}

	// GodOrDevil - 09.07.06 - 03:29pm
	memset( buf, 0, sizeof(buf) );
	///End  - 09.07.06 - 03:29pm

	trap_FS_Read( buf, len, f );
	buf[len] = '\0';
	trap_FS_FCloseFile( f );

	while(bufP <= &buf[len + 500])
	{
		Q_strncpyz(packet, bufP, 501);
		trap_SendServerCommand( ent-g_entities, va("print \"%s\"", packet));
		bufP += 500;
	}
	//trap_SendServerCommand( ent-g_entities, va("print \"%s\n\"", buf));
	trap_SendServerCommand( ent-g_entities, va("print \"^3** Press PgUp and PgDn keys to scroll up and down the list in console! **\n\""));
	return;
}
#endif

int RPM_Remove_from_list ( char *key, const char *file, const char* type, gentity_t *ent, qboolean unban, qboolean removeAdmin)
{
	int				len = 0, removed = 0;
	// GodOrDevil - 10.23.05 - 11:17pm
	char			buf[15000] = "\0";
	///End  - 10.23.05 - 11:17pm
	char            listName[64] = "\0";
	char			ip[MAX_IP];
	char			*bufP, *listP;
	fileHandle_t	f;
	char			a[64] = "\0";

	while(*key == ' '){
		if(*key == '\0'){
			return 0;
		}
		key++;
	}

	len = trap_FS_FOpenFile( file, &f, FS_READ_TEXT);

	if (!f)	{
		RPM_FileError(ent, file);
		return -1;
	}

	if(len > 15000)	{
		len = 15000;
	}

	// GodOrDevil - 09.07.06 - 03:29pm
	memset( buf, 0, sizeof(buf) );
	///End  - 09.07.06 - 03:29pm

	trap_FS_Read( buf, len, f );
	buf[len] = '\0';
	trap_FS_FCloseFile( f );

	bufP = buf;
	listP = listName;

	len = trap_FS_FOpenFile( file, &f, FS_WRITE_TEXT);

	while(*bufP) {
		while(*bufP != '\n' && *bufP != '\0') {
			*listP++ = *bufP++;
			if(unban && *bufP == '\\') {
				*listP = '\0';
				Q_strncpyz(ip, listName, MAX_IP);
			}
		}

		*listP = '\0';

		while (*bufP == '\n') {
			bufP++;
		}

		if(unban && !Q_stricmp(ip, key)) {
			removed = 1;
			listP = listName;
			continue;
		}
		
		///03.15.05 - 09:35pm				
		if (removeAdmin){
			///192.16/[SC]GodOrDevil:2
			strcpy(a, va("%s:2", key ));
			///Q_strncpyz (a, key, 64);
			///strcat ( a, ":2" );
			if (!Q_stricmp(listName, a)) {
				removed = 1;
				listP = listName;
				continue;
			}
			///192.16/[SC]GodOrDevil:3
			strcpy(a, va("%s:3", key ));
			///Q_strncpyz (a, key, 64);
			///strcat ( a, ":3" );
			if (!Q_stricmp(listName, a)) {
				removed = 1;
				listP = listName;
				continue;
			}
			///192.16/[SC]GodOrDevil:4
			strcpy(a, va("%s:4", key ));
			///Q_strncpyz (a, key, 64);
			///strcat ( a, ":4" );
			if (!Q_stricmp(listName, a)) {
				removed = 1;
				listP = listName;
				continue;
			} 
		}
		///End  - 03.15.05 - 09:57pm


		if(!Q_stricmp(listName, key)){
			removed = 1;
			listP = listName;
			continue;
		}

		trap_FS_Write( listName, strlen(listName), f);
		trap_FS_Write( "\n", 1, f);
		listP = listName;
	}

	trap_FS_FCloseFile( f );

	if(!removed){
		if(ent && ent->client) {
			trap_SendServerCommand( ent-g_entities, va("print \"^3%s ^7was not on the %s list\n\"", key, type));
		}
		else {
			Com_Printf("^3%s ^7was not on the %s list\n", key, type);
		}
	}
	

	return removed;
}

void RPM_FileError (gentity_t * ent, const char *file)
{
	if(ent && ent->client)
	{
		trap_SendServerCommand( ent-g_entities, va("print \"^1Error opening %s\n\"", file));
	}
	else
	{
		Com_Printf("^1Error opening %s\n", file);
	}
	return;
}
void RPM_ParseChatSounds (void)
{
	int			fileCount, filelen, i, numSounds, number;
	char		chatFiles[1024];
	char		*filePtr;
	char		*file;
	char		text[MAX_SAY_TEXT];
	char		sound[MAX_QPATH];
	char		numString[8];
	void		*GP2, *group;

	//First we load the default RPM sounds into the chatSounds data buffer
	G_LogPrintf("Loading Default RPM chat Sounds\n");
	for( numSounds = 0; defaultChatSounds[numSounds][0]; numSounds++)
	{
		Q_strncpyz(chatSounds[numSounds].text, defaultChatSounds[numSounds][0], MAX_SAY_TEXT);
		chatSounds[numSounds].sound = G_SoundIndex(va("%s", defaultChatSounds[numSounds][1]));
	}

	// Find out how many .vchat files we have
	fileCount = trap_FS_GetFileList( "", ".vchat", chatFiles, 1024 );
	filePtr = chatFiles;

	G_LogPrintf("Number of Extended Chat sound files to Parse: %d\n", fileCount);

	//Parse each file
	for( i = 0; i < fileCount && numSounds < MAX_RPM_CHATS; i++, filePtr += filelen+1 )
	{
		filelen = strlen(filePtr);
		file = va("%s", filePtr);

		GP2 = trap_GP_ParseFile(file, qtrue, qfalse);
		if (!GP2)
		{
			G_LogPrintf("Error in file: \"%s\" or file not found\n", file);
			continue;
		}

		G_LogPrintf("Parsing chat file: %s\n", file);
		group = trap_GPG_GetSubGroups(GP2);

		//extract the sound and text data from each file
		while(group)
		{
			trap_GPG_FindPairValue(group, "number", "0", numString);
			trap_GPG_FindPairValue(group, "text", "", text);
			trap_GPG_FindPairValue(group, "sound", "", sound);

			number = atoi(numString) -1;

			if(number < 0 || number > MAX_RPM_CHATS - 1)
			{
				G_LogPrintf("Error in chat file: %s\nNumber %d out of range must be 101 - 200.\n", file, number);
				group = trap_GPG_GetNext(group);
				continue;
			}
			if(*chatSounds[number].text || chatSounds[number].sound)
			{
				G_LogPrintf("Error chat token #%d already in use by:\n%s\n", number+1, chatSounds[number].text);
				group = trap_GPG_GetNext(group);
				continue;
			}

			if(*text && *sound)
			{
				//store the sound and text data
				Q_strncpyz(chatSounds[number].text, text, MAX_SAY_TEXT - 1);
				chatSounds[number].sound = G_SoundIndex(va("%s", sound));
				numSounds++;
			}
			if(numSounds == MAX_RPM_CHATS)
			{
				break;
			}

			group = trap_GPG_GetNext(group);
		}

		trap_GP_Delete(&GP2);
	}

	G_LogPrintf("Loaded %d sounds.\n", numSounds);
}
#ifdef _RPM_WEAPONMOD
void RPM_WeaponMod (qboolean broadcast, int mod)
{
	void			*GP2, *group, *attackType;
	char			name[64], tmpStr[64];
	int				i, n, clipSize, numExtraClips, damage;
	int				ammoMaxs[AMMO_MAX] = {0};
	attackData_t	*attack;

	if(g_weaponModFlags.integer == 0)
	{
		G_LogPrintf("Not modifying weapon stats\n");
		return;
	}

	GP2 = trap_GP_ParseFile(g_weaponModFile.string, qtrue, qfalse);
	if (!GP2)
	{
		Com_Printf("^1Error in file: \"%s\" or file not found\n", g_weaponModFile.string);
		G_LogPrintf("Error in file: \"%s\" or file not found\n", g_weaponModFile.string);
		return;
	}

	G_LogPrintf("Loading weapon mods from: %s\n", g_weaponModFile.string);

	group = trap_GPG_GetSubGroups(GP2);

	while(group)
	{
		trap_GPG_FindPairValue(group, "name", "", name);

		for(i = 0; i < WP_NUM_WEAPONS; i++)
		{
			if (Q_stricmp(bg_weaponNames[i], name) == 0)
			{
				for(n = ATTACK_NORMAL; n < ATTACK_MAX; n++)
				{
					if(n == ATTACK_NORMAL)
					{
						attackType = trap_GPG_FindSubGroup(group, "attack");
					}
					else if(n == ATTACK_ALTERNATE)
					{
						attackType = trap_GPG_FindSubGroup(group, "altattack");
					}

					if ( NULL == attackType )
					{
						continue;
					}

					attack = &weaponData[i].attack[n];

					trap_GPG_FindPairValue(attackType, "mp_clipSize||clipSize", "-1", tmpStr);
					clipSize = atoi(tmpStr);
					if(clipSize < 0) //If no value specified in the file use default vaule
					{
						clipSize = attack->clipSize;
					}

					trap_GPG_FindPairValue(attackType, "mp_extraClips", "-1", tmpStr );
					numExtraClips = atoi ( tmpStr );
					if(numExtraClips < 0)
					{
						numExtraClips = attack->extraClips;
					}

					trap_GPG_FindPairValue(attackType, "mp_damage||damage", "-1", tmpStr);
					damage = atoi(tmpStr);
					if(damage < 0)
					{
						damage = attack->damage;
					}

					ammoMaxs[attack->ammoIndex] += clipSize * numExtraClips;

					if(g_weaponModFlags.integer & AMMO_MOD)
					{
						attack->clipSize = clipSize;
						attack->extraClips = numExtraClips;
						ammoData[attack->ammoIndex].max = ammoMaxs[attack->ammoIndex];
					}

					if(g_weaponModFlags.integer & DAMAGE_MOD)
					{
						attack->damage = damage;
					}
				}
			}
		}

		group = trap_GPG_GetNext(group);
	}

	trap_GP_Delete(&GP2);

	if(broadcast)
	{
		switch(mod)
		{
			case AMMO_MOD:
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, Servers ^3Ammo amounts ^7have been Modified", level.time + 5000));
				G_LogPrintf("Modified Ammos loaded\n");
				break;
			case DAMAGE_MOD:
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, Servers ^3Damage amounts ^7have been Modified", level.time + 5000));
				G_LogPrintf("Modified Damage loaded\n");
				break;
			case 3: // = AMMO_MOD & DAMAGE_MOD
				trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i, Servers ^3Damage & Ammos ^7have been Modified", level.time + 5000));
				G_LogPrintf("Modified Ammos & Damage loaded\n");
				break;
		}
	}
}
void RPM_Undo_WeaponMods (void)
{
	int				i, n;
	attackData_t	*attack;

	for(i = 0; i < WP_NUM_WEAPONS; i++)
	{
		for(n = ATTACK_NORMAL; n < ATTACK_MAX; n++)
		{
			attack = &weaponData[i].attack[n];

			if(!(g_weaponModFlags.integer & AMMO_MOD))
			{
				attack->clipSize = attack->defaultClipSize;
				attack->extraClips = attack->defaultExtraClips;
				ammoData[attack->ammoIndex].max = ammoData[attack->ammoIndex].defaultMax;
			}
			if(!(g_weaponModFlags.integer & DAMAGE_MOD))
			{
				attack->damage = attack->defaultDamage;
			}
		}
	}
}
#endif

void RPM_UpdateLoadScreenMessage (void)
{
	if(gt_customType.integer >= NV_GT_DOM)  // Below DOM are default GT's
	{
		//trap_SetConfigstring( CS_MESSAGE, va("^6Gametype:  ^3%s", Q_strupr(nv_gtName.string)));
		trap_SetConfigstring( CS_MESSAGE, va("^+Gametype:  ^3%s", nv_gtName.string));
	}
#ifdef _RPM_WEAPONMOD
	else
	{
		char	*ammo, *damage;
		switch (g_weaponModFlags.integer)
		{
			case AMMO_MOD:
				ammo = "ON";
				damage = "OFF";
				break;
			case DAMAGE_MOD:
				ammo = "OFF";
				damage = "ON";
				break;
			case 3:
				ammo = "ON";
				damage = "ON";
				break;
			default:
				ammo = "OFF";
				damage = "OFF";
		}
		trap_SetConfigstring( CS_MESSAGE, va("^3Modified Damage: ^7%s  ^3Modified Ammo: ^7%s\n", damage, ammo));
	}
#endif
}
#ifndef _NV_ADMIN

void QDECL SC_adminLog( const char *text, ... )
{
	char		string[1024] = "\0";
	va_list		argptr;
	qtime_t		q;
	fileHandle_t	f;

	trap_RealTime (&q);

	Com_sprintf( string, sizeof(string), "%02i/%02i/%i %02i:%02i - ", 1+q.tm_mon,q.tm_mday, q.tm_year+1900,q.tm_hour,q.tm_min);
	va_start( argptr, text );
	vsprintf( string + 19, text, argptr );
	va_end( argptr );
		
	trap_FS_FOpenFile( "logs/adminlog.txt", &f, FS_APPEND_TEXT);
	
	if ( !f )
		return;

	trap_FS_Write( string, strlen( string ), f );
	trap_FS_Write( "\n", 1, f);
	trap_FS_FCloseFile(f);
}
#endif
#ifdef _TORTURE
void SC_doTorture (gentity_t *ent)
{
	vec3_t		dir;
	float		knockback = 600.0;
	gspawn_t	*spawnPoint = G_SelectRandomSpawnPoint (/*TEAM_FREE*/-1);

	trap_SendServerCommand(ent-g_entities, va("cp \"@^3You ^7are being ^_ToRTuRed^+!\n^4Reason: ^7%s\n\"", ent->client->nvSess.tortureReason ));

	///If they're dead respawn them.
	if ( G_IsClientDead ( ent->client ) ){
		trap_UnlinkEntity (ent);
		ClientSpawn(ent);
		return;
	}

	switch (ent->client->nvSess.tortureCount)
	{
	case 1:
		if(ent->client->pers.planted)
			ent->client->ps.stats[STAT_GOGGLES] = GOGGLES_NIGHTVISION;
		trap_SendConsoleCommand( EXEC_INSERT, va("plant %i\n", ent->s.number ) );
		ent->client->ps.pm_flags = PMF_GOGGLES_ON;
		break;
	case 5:
		trap_SendConsoleCommand( EXEC_INSERT, va("burn %i\n", ent->s.number ) );
		break;
	case 6:
	case 7:
	case 8:
		trap_SendConsoleCommand( EXEC_INSERT, va("runover %i\n", ent->s.number ) );
		ent->client->ps.stats[STAT_GOGGLES] = GOGGLES_NIGHTVISION;
		break;
	case 9:
		trap_SendConsoleCommand( EXEC_INSERT, va("twist %i\n", ent->s.number ) );
		break;
	case 10:
	case 11:
		trap_SendConsoleCommand( EXEC_INSERT, va("slam %i\n", ent->s.number ) );
		break;
	case 12:
		trap_SendConsoleCommand( EXEC_INSERT, va("twist %i\n", ent->s.number ) );
		break;
	case 13:
		ent->client->ps.stats[STAT_GOGGLES] = GOGGLES_NIGHTVISION;
		trap_SendConsoleCommand( EXEC_INSERT, va("runover %i\n", ent->s.number ) );
		ent->client->ps.pm_flags = PMF_GOGGLES_ON;
		break;
	case 14:
		trap_SendConsoleCommand( EXEC_INSERT, va("twist %i\n", ent->s.number ) );
		ent->client->ps.stats[STAT_GOGGLES] = GOGGLES_NONE;
		ent->client->ps.pm_flags &= ~PMF_GOGGLES_ON;
		break;
	case 15:
		trap_SendConsoleCommand( EXEC_INSERT, va("runover %i\n", ent->s.number ) );
		ent->client->ps.stats[STAT_GOGGLES] = GOGGLES_INFRARED;
		ent->client->ps.pm_flags = PMF_GOGGLES_ON;
		break;
	case 20:
	case 23:
	case 26:
	case 48:
		trap_SendConsoleCommand( EXEC_INSERT, va("pop %i\n", ent->s.number ) );
		break;
	case 29:
	case 32:
	case 35:
		trap_SendConsoleCommand( EXEC_INSERT, va("slam %i\n", ent->s.number ) );
		break;
	case 38:
	case 41:
	case 44:
		trap_SendConsoleCommand( EXEC_INSERT, va("slam %i\n", ent->s.number ) );
		break;
	case 47:
	case 49:
	case 51:
		trap_SendConsoleCommand( EXEC_INSERT, va("plant %i\n", ent->s.number ) );
		break;
	case 54:
	case 56:
	case 58:
	case 60:
		G_PlayEffect ( 100, ent->client->ps.origin, ent->pos1 );
		G_Damage (ent, NULL, NULL, NULL, NULL, 999, 0, MOD_SMOHG92_GRENADE, HL_NONE );
		break;
	case 55:
	case 57:
	case 59:	
	case 61:
		break;
	case 62:
		trap_SendConsoleCommand( EXEC_INSERT, va("pop %d\n", ent->s.number ) );
		ent->client->nvSess.tortureCount = 0; ///Restart the counter
		break;
	default:
		if (!spawnPoint ) return;
		VectorCopy( spawnPoint->origin, ent->client->ps.origin );
		SetClientViewAngle( ent, spawnPoint->angles );
		G_Damage (ent, NULL, NULL, NULL, NULL, 5, 0, MOD_CAR, HL_NONE );	///do the damage
		G_ApplyKnockback ( ent, dir, knockback );	///knock them back
		break;
	}
	ent->client->nvSess.tortureCount++;
}
#endif
void SC_serverMsg (void)
{
	char	*message = "\0";

	level.serverMsgCount++;

	switch (level.serverMsgCount){
		case 1:
			message = g_msg1.string;
			break;
		case 2:
			message = g_msg2.string;
			break;
		case 3:
			message = g_msg3.string;
			break;
		case 4:
			message = g_msg4.string;
			break;
		case 5:
			message = g_msg5.string;
			break;		
		default:
			message = "";
			level.serverMsgCount = 0;
			level.serverMsg = level.time + (g_serverMsgInterval.integer * 60000);
			break;
	}
	if ( !message || message[0] == '\0' )
		return;

	level.serverMsg = level.time + (g_serverMsgDelay.integer * 1000);
	trap_SendServerCommand( -1, va("chat -1 \"%s\n\"", message ) );

}
void QDECL NV_crashLog( const char *text, ... )
{
	char		string[1024] = "\0";
	va_list		argptr;
	qtime_t q;
	///fileHandle_t	f;

	if ( !level.crashLogFile )
		return;

	if ( !text )
		return;

	trap_RealTime (&q);

	Com_sprintf( string, sizeof(string), "%02i/%02i/%i %02i:%02i - ", 1+q.tm_mon,q.tm_mday, q.tm_year+1900,q.tm_hour,q.tm_min);
	va_start( argptr, text );
	vsprintf( string + 19, text, argptr );
	va_end( argptr );

	///trap_FS_FOpenFile( "logs/crashlog.txt", &f, FS_APPEND_TEXT);
	
	trap_FS_Write( string, strlen( string ), level.crashLogFile );
	trap_FS_Write( "\n", 1, level.crashLogFile);
	///trap_FS_FCloseFile(f);
}	
void NV_Weapons (gclient_t	*client)
{
	int			group;
	int			ammoIndex;
	int			i;
	int			equipWeapon;
	int			equipWeaponGroup;
	int					idle;

	client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
	ammoIndex								 = weaponData[WP_KNIFE].attack[ATTACK_NORMAL].ammoIndex;
	client->ps.clip[ATTACK_NORMAL][WP_KNIFE] = weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
	client->ps.firemode[WP_KNIFE]            = BG_FindFireMode ( WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO );
	
	equipWeaponGroup = OUTFITTING_GROUP_KNIFE;
	equipWeapon = WP_KNIFE;

	for(i = WP_KNIFE; i < WP_NUM_WEAPONS; i++)
	{
		if(g_onlyWeapons.integer & (1<<i))
		{
			///Can hold Nades + M203 
			if(g_m203.integer && !g_m4scope.integer && i >= MOD_M84_GRENADE){
				if(client->ps.stats[STAT_WEAPONS] & ( 1 << WP_M4_ASSAULT_RIFLE ) 
					&& client->ps.clip[ATTACK_ALTERNATE][WP_M4_ASSAULT_RIFLE]){
						continue;
				}
			}
			
			for ( group = 0; group < OUTFITTING_GROUP_ACCESSORY; group ++ )
			{
				if ( group < equipWeaponGroup )
				{
					equipWeaponGroup = group;
					equipWeapon = i;
				}
			}

			client->ps.stats[STAT_WEAPONS] |= ( 1 << (i));
			ammoIndex = weaponData[i].attack[ATTACK_NORMAL].ammoIndex ;
			client->ps.ammo[ammoIndex] += weaponData[i].attack[ATTACK_NORMAL].extraClips * weaponData[i].attack[ATTACK_NORMAL].clipSize;
			client->ps.clip[ATTACK_NORMAL][i] = weaponData[i].attack[ATTACK_NORMAL].clipSize;
			ammoData[ammoIndex].max = weaponData[i].attack[ATTACK_NORMAL].extraClips * weaponData[i].attack[ATTACK_NORMAL].clipSize;
				
									
			// alt-fire ammo
			ammoIndex = weaponData[i].attack[ATTACK_ALTERNATE].ammoIndex;
			if ( weaponData[i].attack[ATTACK_ALTERNATE].fireAmount && AMMO_NONE != ammoIndex )
			{
				client->ps.clip[ATTACK_ALTERNATE][i] = weaponData[i].attack[ATTACK_ALTERNATE].clipSize;
				client->ps.ammo[ammoIndex] += weaponData[i].attack[ATTACK_ALTERNATE].extraClips * weaponData[i].attack[ATTACK_ALTERNATE].clipSize;
			}
			// Set the default firemode for this weapon
			if ( client->ps.firemode[i] == WP_FIREMODE_NONE )
				client->ps.firemode[i] = BG_FindFireMode ( i, ATTACK_NORMAL, WP_FIREMODE_AUTO );

			if(i >= MOD_M84_GRENADE)
				///client->ps.stats[STAT_OUTFIT_GRENADE] = bg_itemlist[bg_outfittingGroups[OUTFITTING_GROUP_GRENADE][client->pers.outfitting.items[OUTFITTING_GROUP_GRENADE]]].giTag;
				client->ps.stats[STAT_OUTFIT_GRENADE] = i;
		}
	}

	// Disable zooming
	client->ps.zoomFov	  = 0;
	client->ps.zoomTime  = 0;
	client->ps.pm_flags &= ~(PMF_ZOOM_FLAGS);

	client->ps.weapon = equipWeapon;
	client->ps.weaponstate = WEAPON_READY; //WEAPON_SPAWNING;
	client->ps.weaponTime = 0;
	client->ps.weaponAnimTime = 0;

	// Bot clients cant use the spawning state
#ifdef _SOF2_BOTS
	if ( g_entities[client->ps.clientNum].r.svFlags & SVF_BOT )
		client->ps.weaponstate = WEAPON_READY;
#endif

	// Default to auto (or next available fire mode).
	BG_GetInviewAnim(client->ps.weapon,"idle",&idle);
	client->ps.weaponAnimId = idle;
	client->ps.weaponAnimIdChoice = 0;
	client->ps.weaponCallbackStep = 0;
}
