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
#ifndef _NV_ADMIN

void Cmd_Say_f( gentity_t *ent, int mode, qboolean arg0 );

/*
===========
Cmd_Adm_f
All Admin commands will be handled here
===========
*/
void Cmd_adm_f ( gentity_t *ent )
{

	int		id, adm;
	char	arg1[MAX_STRING_TOKENS];
	char	arg2[MAX_STRING_TOKENS];
	char	arg3[MAX_STRING_TOKENS];
	gclient_t	*client;
	///gentity_t *tent;  // GodOrDevil - 09.01.06 - 05:29pm not needed
	client = ent->client;
	trap_Argv( 1, arg1, sizeof( arg1 ) );
	trap_Argv( 2, arg2, sizeof( arg2 ) );
	
	adm = ent->client->nvSess.admin;
	if(!adm){
		trap_SendServerCommand( ent-g_entities, va("print \"^3You do not have admin powers!\n\""));
		return;
	}

	if(ent->client->sess.team != TEAM_SPECTATOR && g_compMode.integer)
	{
		trap_SendServerCommand( ent-g_entities, va("print \"^3Admin disabled for Competition!  Join Spec to use your powers.\n\""));
		return;
	}
	// GodOrDevil - Adjusted for #AdminLevels - 1.13.2005
	if (!Q_stricmp ( arg1, "?" )||!Q_stricmp ( arg1, "" ))
	{
	if (adm > 1){
		if (adm >= g_plant.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3plant ^5id        ^7Puts player into the ground                  [^4%i^7]\n\"", g_plant.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3unplant ^5id      ^7UNPlants player                              [^4%i^7]\n\"", g_plant.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3plantsk ^5id      ^7Plants player at enemies spawn               [^4%i^7]\n\"", g_plant.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3launch ^5id       ^7Launches player in the air                   [^4%i^7]\n\"", g_plant.integer));
			}
		if (adm >= g_pop.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3pop ^5id          ^7Dismembers player                            [^4%i^7]\n\"", g_pop.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3slam ^5id         ^7Slams player into the ground                 [^4%i^7]\n\"", g_pop.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3slow ^5id         ^7Reduce/Restore players speed                 [^4%i^7]\n\"", g_pop.integer));
			}
		if (adm >= g_runover.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3runover ^5id      ^7Runover player with your car                 [^4%i^7]\n\"", g_runover.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3twist ^5id        ^7Twists player like a pretzel                 [^4%i^7]\n\"", g_runover.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3untwist ^5id      ^7Removes player from the Twist state          [^4%i^7]\n\"", g_runover.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3burn ^5id         ^7Sets player on fire                          [^4%i^7]\n\"", g_runover.integer));
			}
		if (adm >= g_kick.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3kick ^5id         ^7Kicks player from the server                 [^4%i^7]\n\"", g_kick.integer));
			}
		if (adm >= g_mute.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3mute ^5id         ^7Mutes the player                             [^4%i^7]\n\"", g_mute.integer));
			}
		if (adm >= g_strip.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3strip ^5id        ^7Remove weapons from the player               [^4%i^7]\n\"", g_strip.integer));
			}
		if (adm >= g_pbox.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3pbox ^5id         ^7Sends player to the penalty box              [^4%i^7]\n\"", g_pbox.integer));
			}
		if (adm >= g_forcesay.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3forcesay ^5id     ^7Send a message as another player             [^4%i^7]\n\"", g_forcesay.integer));
			}
		if (adm >= g_namechange.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3rename ^5id       ^7Changes players name                         [^4%i^7]\n\"", g_namechange.integer));
			}
		if (adm >= g_warn.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3warn ^5id         ^7Sends a warning message to player            [^4%i^7]\n\"", g_warn.integer));
			}
#ifdef _CLANMEMBER
		if (adm >= g_addclan.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3addclan ^5id      ^7Adds Clan Status to the player               [^4%i^7]\n\"", g_addclan.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3removeclan ^5id   ^7Removes Clan Status from the player          [^4%i^7]\n\"", g_addclan.integer));
		}
#endif
		if (adm >= g_addbadmin.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3addbadmin ^5id    ^7Gives BASIC Admin to the player              [^4%i^7]\n\"", g_addbadmin.integer));
			}
		if (adm >= g_addadmin.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3addadmin ^5id     ^7Gives Admin to the player                    [^4%i^7]\n\"", g_addadmin.integer));
			//trap_SendServerCommand( ent-g_entities, va("print \" ^3addfake ^5id      ^7Adds player to the hidden admin list         [^4%i^7]\n\"", g_addadmin.integer));
			//trap_SendServerCommand( ent-g_entities, va("print \" ^3removefake ^5id   ^7Removes player from the hidden admin list    [^4%i^7]\n\"", g_addadmin.integer));
			}
		if (adm >= g_torture.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3torture ^5id      ^7Torture/unTortures player                    [^4%i^7]\n\"", g_torture.integer));
			}
		if (adm >= g_ban.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3ban ^5id          ^7Bans player from the server                  [^4%i^7]\n\"", g_ban.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3unban ^5ip        ^7Unban an ip from the server                  [^4%i^7]\n\"", g_ban.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3alias ^5id        ^7Lists the names used from the clients IP     [^4%i^7]\n\"", g_ban.integer));
			}
		if (adm >= g_subnetban.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3subnetban ^5id    ^7Ban players subnet (opt <#> <reason>)        [^4%i^7]\n\"", g_subnetban.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3subnetUnban ^5ip  ^7Removes an IP from the subnet banlist        [^4%i^7]\n\"", g_subnetban.integer));
			}
		if (adm >= g_lock.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3lock ^5team       ^7Locks the team so players cant join it       [^4%i^7]\n\"", g_lock.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3unlock ^5team     ^7Unlocks the team                             [^4%i^7]\n\"", g_lock.integer));
			}
		if (adm >= g_eventeams.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3eventeams       ^7Evens the teams                              [^4%i^7]\n\"", g_eventeams.integer));
			}
#ifdef _CLANMEMBER	
		if (adm >= g_clanvsall.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3clanvsall       ^7Puts all clan on one team                    [^4%i^7]\n\"", g_clanvsall.integer));
			}
#endif

		if (adm >= g_swapteams.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3swapteams       ^7Moves all players to the opposite team       [^4%i^7]\n\"", g_swapteams.integer));
			}
		if (adm >= g_ff.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3ff              ^7Toggles friendlyfire                         [^4%i^7]\n\"", g_ff.integer));
			}

		if (adm >= g_fvote.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3cancelvote      ^7Cancels current vote                         [^4%i^7]\n\"", g_fvote.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3passvote        ^7Passes current vote                          [^4%i^7]\n\"", g_fvote.integer));
			}

		if (adm >= g_modeChange.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3compmode        ^7Toggles Competition mode                     [^4%i^7]\n\"", g_modeChange.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3anticamp        ^7Toggles Anticamp                             [^4%i^7]\n\"", sc_camperPunish.integer));
			}
		if (adm >= g_nextMap.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3next            ^7Sets the next map w/out ending the currnet   [^4%i^7]\n\"", g_nextMap.integer));
			}	
		//if (adm >= g_hide.integer){
		//	trap_SendServerCommand( ent-g_entities, va("print \" ^3hide            ^7Hides your ^6ADMIN ^7Status                     [^4%i^7]\n\"", g_hide.integer));
		//	trap_SendServerCommand( ent-g_entities, va("print \" ^3hideall         ^7Hides status of ALL ^6ADMINS^7                   [^4%i^7]\n\"", g_hide.integer));
		//	trap_SendServerCommand( ent-g_entities, va("print \" ^3unhideall       ^7Reveals status of ALL ^6ADMINS^7                 [^4%i^7]\n\"", g_hide.integer));
		//}
		if (adm >= sc_forceWeapon.integer){
			trap_SendServerCommand( ent-g_entities, va("print \" ^3force           ^7Force weapons in the player outfittings      [^4%i^7]\n\"", sc_forceWeapon.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3clipsize        ^7Change a weapons clip size                   [^4%i^7]\n\"", sc_forceWeapon.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3extraclips      ^7Change how many extra clips a weapon has     [^4%i^7]\n\"", sc_forceWeapon.integer));
			//trap_SendServerCommand( ent-g_entities, va("print \" ^3altammo         ^7Change a weapons alternate ammo ammount      [^4%i^7]\n\"", sc_forceWeapon.integer));
			trap_SendServerCommand( ent-g_entities, va("print \" ^3only            ^7Only enable the selected weapon(s)           [^4%i^7]\n\"", sc_forceWeapon.integer));
		}
		if (adm >= sc_adminSuspend.integer)
		{
			trap_SendServerCommand( ent-g_entities, va("print \" ^3suspend         ^7Suspends/resumes admin status\n\"" ));
		}
		
		trap_SendServerCommand( ent-g_entities, va("print \" ^3banlist          ^7Displays a list of the banned clients\n\"" ));
		trap_SendServerCommand( ent-g_entities, va("print \" ^3subnetBanlist    ^7Displays a list of the Subnet banned clients\n\"" ));
		//trap_SendServerCommand( ent-g_entities, va("print \" ^3hide             ^7Hides or Reveals your admin status\n\"" ));
		//trap_SendServerCommand( ent-g_entities, va("print \" ^3fake             ^7Fakes admin disconnecting from server\n\"" ));

		if (adm >= g_extracmds.integer)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"--------------- ^1Extra Admin commands Available ^7-------------  [^4%i^7]\n\"", g_extracmds.integer));
			NV_extraCommands (ent, "showExtra");
			//RPM_Print_File( ent, g_extraCommands.string);
			///trap_SendServerCommand( ent-g_entities, va("print \"---------------------------------------------------------------\n\"", g_extracmds.integer));
		}
	///return;
		}
	else if (adm == 1){
		trap_SendServerCommand( ent-g_entities, va("print \"\n [^3Referee commands ^7(^-ADMIN SUSPEND^7)]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" ** All commands must start with a ^3/ref ^7in front of them! **\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" ^4info <team>   ^7displays the specified teams info (whos ready, locked etc.)\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" ^4ready <team>  ^7readys up the specified team so you can start the match\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" ^4lock <team>   ^7locks the specified team so nobody else can join it\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" ^4unlock <team> ^7unlocks the specified team to allow people to join it\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" ^4reset <team>  ^7resets the specified teams settings (ready status etc...)\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" ^4kick    id#   ^7kicks the player id#\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" ^4chat          ^7sends a chat message to referees only\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" ^4talk          ^7sends a referee message to all players\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" ^4timeout       ^7pauses the game for a timeout\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" ^4timein        ^7starts the match from a timeout\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" ^3** Press PgUp and PgDn keys to scroll up and down the list in console! **\n\""));
		}

		/*
		trap_SendServerCommand( ent-g_entities, va("print \"\n [^3Admin commands^7]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" ** All commands must start with a ^3/adm ^7in front of them! **\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4plant   id#  ^7Plants the player id# firmly in his current position        ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4plantsk id#  ^7Plants player id# in his enemys spawn without weapons       ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4banlist      ^7displayes a list of the server current banned clients       ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4unban   ip   ^7the ip address and associated name will be unbanned         ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4pop     id#  ^7Pop's the player with the specified id#                     ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4runover id#  ^7runover player id# with your car                            ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4kick    id#  ^7kicks the player id#                                        ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4ban     id#  ^7kicks the player id# and bans him from returning            ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4mute    id#  ^7mutes the player id#                                        ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4unmute  id#  ^7removes mute from the player id#                            ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4strip   id#  ^7strips player id# of all weapons except 1 knife             ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4cancelvote   ^7cancels the current vote                                    ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4passvote     ^7forces the current vote to pass                             ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4suspend      ^7suspends/resumes admin status^3 (referee when suspended)^7      ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4chat         ^7sends a chat message to admins only                         ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \" [^4talk         ^7sends an admin message to all players                       ]\n\""));
		trap_SendServerCommand( ent-g_entities, va("print \"^1Extra Admin commands Available\n\""));
		RPM_Print_File( ent, g_extraCommands.string);
		*/
	return;
	///End
	}

	if(!ent->client->nvSess.admin){
		trap_SendServerCommand( ent-g_entities, va("print \"YOU DON'T HAVE ^6ADMIN ^1POWERS\n\""));
		return;
	}

	if(!g_enableAdmins.integer){
		trap_SendServerCommand( ent-g_entities, va("print \"Admins's are disabled on this server\n\""));
		return;
	}
	if(ent->client->sess.penaltySeconds){
		trap_SendServerCommand( ent-g_entities, va("print \"You ^1CANNOT ^7use your ^6POWERS ^7while being ^1Punished^7!\n\""));
		return;
	}
	
	// GodOrDevil - 03.02.05 - 01:05am #suspend
	if((ent->client->nvSess.admin >= sc_adminSuspend.integer) || ent->client->sess.oldStatus){
		if (!Q_stricmp ( arg1, "suspend" ))	{ 		   		  	
			///Currently Ref
			if(ent->client->nvSess.admin == 1){
				ent->client->nvSess.admin = ent->client->sess.oldStatus;
				ent->client->nvSess.referee = 0;
				///trap_SendServerCommand( ent-g_entities, va("print \"admin: %i - oldStatus: %i\n\"",ent->client->nvSess.admin, ent->client->sess.oldStatus));
				trap_SendServerCommand( -1, va("print \"%s ENABLED\nHIS ^6ADMIN ^7POWERS!\n\"", ent->client->pers.netname));
				return;
			}
			///Currently Admin
			else {
			ent->client->sess.oldStatus = ent->client->nvSess.admin; 
				ent->client->nvSess.admin = 1;							
				ent->client->adminspec = qfalse;
				ent->client->nvSess.referee = 1;
				if ( G_IsClientSpectating ( ent->client ) )	{
					if ( ent->client->sess.spectatorState != SPECTATOR_FOLLOW && g_forceFollow.integer ) {
						Cmd_FollowCycle_f( ent, 1 );
					}
				}
				trap_SendServerCommand( -1, va("print \"%s SUSPENDED\nHis ^6ADMIN ^7POWERS!\n\"", ent->client->pers.netname));
				return;
			}
		}
	}
	if(ent->client->nvSess.admin == 1) {
		trap_SendServerCommand( ent-g_entities, va("print \"Your ^6Admin ^7Powers are currently suspended. \nUse ^3/adm suspend ^7to enable them\n\""));
		return;
	}
///END - 1.12.2005.

// GodOrDevil - Let the #AdminLevels determine who gets to use these cmds - 1.12.2005
	if (!Q_stricmp ( arg1, "passvote" )	&& ent->client->nvSess.admin >= g_fvote.integer){
		if ( !level.voteTime ){
			trap_SendServerCommand( ent-g_entities, "print \"No vote in progress.\n\"" );
			return;
		}
		level.voteYes = level.numVotingClients / 2 + 1;
		trap_SendServerCommand( -1, va("print \"^2Vote was ^3passed ^2by ^3%s\n\"",  ent->client->nvSess.cleanName));
		SC_adminLog (va("%s (%s) - Passvote", ent->client->nvSess.cleanName, ent->client->nvSess.ip )) ;
		NV_SQLAdminLog(ent, NULL, "Passvote");
		return;
	}
	if (!Q_stricmp ( arg1, "cancelvote" ) && ent->client->nvSess.admin >= g_fvote.integer){
		Svcmd_CancelVote_f();
		trap_SendServerCommand( -1, va("print \"^2Vote was ^1canceled ^2by ^3%s\n\"",  ent->client->nvSess.cleanName));
		SC_adminLog (va("%s (%s) - Cancelvote", ent->client->nvSess.cleanName, ent->client->nvSess.ip )) ;
		NV_SQLAdminLog(ent, NULL, "Cancelvote");
		return;
	}
	if (!Q_stricmp ( arg1, "strip" ) && ent->client->nvSess.admin >= g_strip.integer){
		Svcmd_Strip(2, ent);
		return;
	}
	if (!Q_stricmp ( arg1, "runover" ) && ent->client->nvSess.admin >= g_runover.integer){
		Svcmd_Runover(2, ent);
		return;
	}
	if (!Q_stricmp ( arg1, "pop" ) && ent->client->nvSess.admin >= g_pop.integer)
	{
		Svcmd_pop(2, ent);
		return;
	}
	if (!Q_stricmp ( arg1, "plant" ) && ent->client->nvSess.admin >= g_plant.integer){
		RPM_Plant(2, ent, qfalse);
		return;
	}
	if (!Q_stricmp ( arg1, "plantsk" ) && ent->client->nvSess.admin >= g_plant.integer){
		RPM_PlantSK(2, ent);
		return;
	}
	if (!Q_stricmp ( arg1, "mute" )	&& ent->client->nvSess.admin >= g_mute.integer){
		Svcmd_Mute(2, ent);
		return;
	}
	if (!Q_stricmp ( arg1, "unmute" ) && ent->client->nvSess.admin >= g_mute.integer){
		Svcmd_Mute(2, ent);
		return;
	}
	if (!Q_stricmp ( arg1, "ban" ) && ent->client->nvSess.admin >= g_ban.integer)	{
		Svcmd_Ban_f(2, ent);
		return;
	}
	if (!Q_stricmp ( arg1, "unban" ) && ent->client->nvSess.admin >= g_ban.integer){
		trap_Argv( 2, arg2, sizeof( arg2 ) );
		RPM_Unban(ent, arg2, qfalse);
		return;
	}
	if(!Q_stricmp ( arg1, "banlist" )){
#ifdef _DB_DLL
		if(g_enableDatabase.integer == 2)
		{		
			NV_QueryDB (DB_LITE_GETBANS, va("%i", ent->s.number));
			return;
		}
#endif
		trap_SendServerCommand( ent-g_entities, va("print \"[^3Banlist^7]\n\""));
		RPM_Print_File( ent, g_banlist.string);
		return;
	}
	if(!Q_stricmp ( arg1, "adminlist" ) && ent->client->nvSess.admin >= 4 ){
#ifdef _DB_DLL
		if(g_enableDatabase.integer == 2)
		{		
			if(!*arg2)
				NV_QueryDB (DB_LITE_GETADMINS, va("%i;0;", ent->s.number));
			else
				NV_QueryDB (DB_LITE_GETADMINS, va("%i;%s;", ent->s.number,arg2));
			return;
		}
#endif
		trap_SendServerCommand( ent-g_entities, va("print \"[^3Admin List^7]\n\""));
		RPM_Print_File( ent, g_adminfile.string);
		return;
	}
#ifdef _CLANMEMBER
	if(!Q_stricmp ( arg1, "clanlist" ) && ent->client->nvSess.admin >= 4){
		trap_SendServerCommand( ent-g_entities, va("print \"[^3Clan List^7]\n\""));
		RPM_Print_File( ent, g_clanfile.string);
		return;
	}
#endif
	if (!Q_stricmp ( arg1, "chat" )){
		if(ent->client->sess.mute){
			trap_SendServerCommand( ent-g_entities, va("print \"You are currently muted by admin.\n\"") );
			return;
		}
		// GodOrDevil - 09.01.06 - 05:27pm - not needed anymore
		//for(i = 0; i < level.maxclients; i++){
		//	tent = &g_entities[i];
		//	if (!tent->inuse){
		//		continue;
		//	}
		//	if(tent->client->nvSess.admin){
		//		RPM_ClientSound(tent, G_SoundIndex("sound/misc/confused/sec_camera2.mp3"));
		//	}
		//}
		Cmd_Say_f (ent, ADM_CHAT, qfalse);
		///End  - 09.01.06 - 05:27pm
		return;
	}
	if (!Q_stricmp ( arg1, "talk" )){
		if(ent->client->sess.mute){
			trap_SendServerCommand( ent-g_entities, va("print \"You are currently muted by admin.\n\"") );
			return;
		}
		// GodOrDevil - 09.01.06 - 05:27pm - not needed anymore
		///RPM_GlobalSound(G_SoundIndex("sound/misc/confused/sec_camera2.mp3"));
		Cmd_Say_f (ent, ADM_TALK, qfalse);
		///End  - 09.01.06 - 05:27pm
		return;
	}
	if (!Q_stricmp ( arg1, "kick" ) && ent->client->nvSess.admin >= g_kick.integer){
		id = RPM_ClientNumFromArg(ent, 2, "kick <idnumber> <reason>", "Kick", qfalse);
		if(id < 0 || id > 64)
			return;
		trap_Argv( 3, arg3, sizeof( arg3 ) );
		trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"%s\"\n", id, arg3));
		trap_SendServerCommand( -1, va("print \"^3%s ^2was kicked off the server by ^3%s\n\"",  g_entities[id].client->nvSess.cleanName, ent->client->nvSess.cleanName));
		if(ent && ent->client)
			SC_adminLog (va("%s (%s) - Kick: %s (%s)", ent->client->nvSess.cleanName, ent->client->nvSess.ip, g_entities[id].client->nvSess.cleanName, g_entities[id].client->nvSess.ip   )) ;
		else
			SC_adminLog (va("%s - kick: %s (%s)", "RCON", g_entities[id].client->nvSess.cleanName, g_entities[id].client->nvSess.ip  )) ;
		NV_SQLAdminLog(ent, &g_entities[id], "Kick");
		return;
	}
	if (!Q_stricmp ( arg1, "pause" ) && ent->client->nvSess.admin >= g_pause.integer)
	{
		if(!g_compMode.integer)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Can only pause during Competition mode\n\""));
			return;
		}
		if(level.warmupTime != 0)
		{
			trap_SendServerCommand( ent-g_entities, va("print \"Cannot pause in warmup period\n\""));
			return;
		}
		trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@^1Paused ^7by ^6ADMIN!", level.time + 10000 ) );
		RPM_Pause(ent);
		
		// GodOrDevil - 02.26.05 - 02:59am #adminLog
		if(ent && ent->client)
			SC_adminLog (va("%s (%s) - PAUSE", ent->client->nvSess.cleanName, ent->client->nvSess.ip ));
		else 
			SC_adminLog (va("%s - PAUSE", "RCON" )) ;
		NV_SQLAdminLog(ent, NULL, "PAUSE");
		///End  - 02.26.05 - 02:59am
		return;
	}
	if (!Q_stricmp ( arg1, "unpause" )	&& ent->client->nvSess.admin >= g_pause.integer){
		if(level.extras.timeout)
			level.extras.timeout = 0;

		RPM_Unpause(ent);
		// GodOrDevil - 02.26.05 - 02:59am #adminLog
		if(ent && ent->client)
			SC_adminLog (va("%s (%s) - UNPAUSE", ent->client->nvSess.cleanName, ent->client->nvSess.ip ));
		else 
			SC_adminLog (va("%s - UNPAUSE", "RCON" )) ;
		///End  - 02.26.05 - 02:59am
		NV_SQLAdminLog(ent, NULL, "UNPAUSE");
		return;
	}

	// GodOrDevil - Added Admin Commands - 1.02.2005
	if (!Q_stricmp ( arg1, "eventeams" ) && ent->client->nvSess.admin >= g_eventeams.integer){
		RPM_Even_Teams(ent, qfalse);
		return;
		}
	///#SwapTeams 1.2.2005
	if (!Q_stricmp ( arg1, "swapteams" ) && ent->client->nvSess.admin >= g_swapteams.integer){
		SC_SwapTeams(ent);
		return;
		}
	///02.03.05 - 11:35am - #Pbox
	if ((!Q_stricmp ( arg1, "penalty" )) || (!Q_stricmp ( arg1, "pbox" )) && ent->client->nvSess.admin >= g_pbox.integer){
		RPM_Penalty(2, ent);
		return;
	}
	///03.02.05 - 09:07pm #Torture
	if ((!Q_stricmp ( arg1, "torture" )) && ent->client->nvSess.admin >= g_torture.integer) {
		SC_Torture(2, ent);
		return;
	}
	if ((!Q_stricmp ( arg1, "untorture" )) && ent->client->nvSess.admin >= g_torture.integer) {
		SC_unTorture(2, ent);
		return;
	}
	///03.09.05 - 07:41pm #Drop
	if ((!Q_stricmp ( arg1, "slam" )) && ent->client->nvSess.admin >= g_runover.integer) {
		SC_Slam(2, ent);
		return;
	}
	///03.09.05 - 9:32pm #Twist
	if ((!Q_stricmp ( arg1, "twist" )) && ent->client->nvSess.admin >= g_runover.integer) {
		SC_Twist(2, ent);
		return;
	}
	if ((!Q_stricmp ( arg1, "untwist" )) && ent->client->nvSess.admin >= g_runover.integer) {
		SC_unTwist(2, ent);
		return;
	}
	///03.09.05 - 10:20pm #Slow
	if ((!Q_stricmp ( arg1, "slow" )) && ent->client->nvSess.admin >= g_runover.integer) {
		SC_Slow(2, ent);
		return;
	}
	if ((!Q_stricmp ( arg1, "forcesay" )) && ent->client->nvSess.admin >= g_forcesay.integer ){
		SC_Forcesay(2, ent);
		return;
	}
	///01.22.06 - 05:38pm - #subnetban
	if (!Q_stricmp ( arg1, "subnetban" ) && ent->client->nvSess.admin >= g_subnetban.integer){
		SC_subnetBan(2, ent);
		return;
	}
	if (!Q_stricmp ( arg1, "flash" ) && ent->client->nvSess.admin >= 3){
		SC_Flash(2, ent);
		return;
	}
	if (!Q_stricmp ( arg1, "launch" ) && ent->client->nvSess.admin >= g_plant.integer){
		NV_Launch(2, ent);
		return;
	}
	if (!Q_stricmp ( arg1, "clipsize" ) && ent->client->nvSess.admin >= sc_forceWeapon.integer){
		NV_ClipSize(2, ent);
		return;
	}	
	if (!Q_stricmp ( arg1, "extraClips" ) && ent->client->nvSess.admin >= sc_forceWeapon.integer){
		NV_ExtraClips(2, ent);
		return;
	}	
	//if (!Q_stricmp ( arg1, "altammo" ) && ent->client->nvSess.admin >= sc_forceWeapon.integer){
	//	NV_AltAmmo(2, ent);
	//	return;
	//}
	if (!Q_stricmp ( arg1, "only" ) && ent->client->nvSess.admin >= sc_forceWeapon.integer){
		NV_WeaponOnly(2, ent);
		return;
	}
	
	///#unplant
	if (!Q_stricmp ( arg1, "unplant" ) && ent->client->nvSess.admin >= g_plant.integer){
		RPM_unPlant(2, ent);
		return;
	}
	///#nextMap
	if (!Q_stricmp(arg1, "next" ) && ent->client->nvSess.admin >= g_nextMap.integer){
		SC_nextMap(ent);
		return;
	}
	///#force
	if(!Q_stricmp(arg1, "force" )	&& ent->client->nvSess.admin >= sc_forceWeapon.integer){
		//SC_forceWeapons(2, ent);
		NV_ForcedWeapons(2, ent);
		return;	
	}
	if(!Q_stricmp(arg1, "burn" )	&& ent->client->nvSess.admin >= g_runover.integer){
		SC_Burn(2, ent);
		return;	
	}
	if(!Q_stricmp ( arg1, "subnetbanlist" )){
		trap_SendServerCommand( ent-g_entities, va("print \"[^3Subnet Banlist^7]\n\""));
		RPM_Print_File( ent, g_subnetbanlist.string);
		return;
	}
	if (!Q_stricmp ( arg1, "subnetunban" ) && ent->client->nvSess.admin >= g_subnetban.integer){
		trap_Argv( 2, arg2, sizeof( arg2 ) );
		RPM_Unban(ent, arg2, qtrue);
		return;
	}
	if (!Q_stricmp ( arg1, "addadmin" ) && ent->client->nvSess.admin >= g_addadmin.integer){
		Svcmd_Add_Admin_f(2, ent);
		return;
	}
	if (!Q_stricmp ( arg1, "addbadmin" ) && ent->client->nvSess.admin >= g_addbadmin.integer){
		Svcmd_Add_bAdmin_f(2, ent);
		return;
	}
#ifdef _CLANMEMBER	
	if (!Q_stricmp ( arg1, "addclan" ) && ent->client->nvSess.admin >= g_addclan.integer){
		RPM_Add_Clan_Member(2, ent);
		return;
	}
	if (!Q_stricmp ( arg1, "removeclan" ) && ent->client->nvSess.admin >= g_addclan.integer){
		RPM_Remove_Clan_Member(2, ent);
		return;
	}
	if (!Q_stricmp ( arg1, "clanvsall" ) && ent->client->nvSess.admin >= g_clanvsall.integer){
		RPM_Clan_Vs_All(ent);
		return;
	}
#endif
	if (!Q_stricmp ( arg1, "warn" ) && ent->client->nvSess.admin >= g_warn.integer){
		Svcmd_admWarn_cmd(2, ent);
		return;
	}
	///#Namechange
	if ((!Q_stricmp ( arg1, "namechange" )) || (!Q_stricmp ( arg1, "rename" ))  && ent->client->nvSess.admin >= g_namechange.integer){
		Svcmd_NameChange(2, ent);
		return;
	}
	if ( (!Q_stricmp ( arg1, "lock" )) || (!Q_stricmp ( arg1, "unlock" ))  && ent->client->nvSess.admin >= g_lock.integer){
		if ( !level.gametypeData->teams ){
			trap_SendServerCommand( ent-g_entities, va("print \"Not playing a team game!\n\""));
			return;
		}
		RPM_lockTeam(ent, qtrue, arg2);
		return;
	}
	///04.06.05 - 05.48pm #adminHidden
	
	///#Friendly Fire
	if ( !Q_stricmp(arg1, "ff" ) && ent->client->nvSess.admin >= g_ff.integer){
		if (g_friendlyFire.integer) 
			trap_Cvar_Set("g_friendlyFire",	 "0"); 
		else 
			trap_Cvar_Set("g_friendlyFire",	 "1");
		if(ent && ent->client)
			SC_adminLog (va("%s (%s) - FriendlyFire: %i", ent->client->nvSess.cleanName, ent->client->nvSess.ip, g_friendlyFire.integer?0:1 ));
		else 
			SC_adminLog (va("%s - FriendlyFire: %i", "RCON", g_friendlyFire.integer?0:1 )) ;
		NV_SQLAdminLog(ent, NULL, va("FriendlyFire: %i",  g_friendlyFire.integer?0:1));
		return;
	}
	///#antiCamp
	if (!Q_stricmp(arg1, "anticamp" ) && ent->client->nvSess.admin >= g_modeChange.integer){
		if (sc_camperPunish.integer) 
			trap_Cvar_Set("sc_camperPunish",	 "0");
		else 
			trap_Cvar_Set("sc_camperPunish",	 "1");
		if(ent && ent->client)
			SC_adminLog (va("%s (%s) - AntiCamp: %i", ent->client->nvSess.cleanName, ent->client->nvSess.ip, sc_camperPunish.integer?0:1 ));
		else 
			SC_adminLog (va("%s - AntiCamp: %i", "RCON", sc_camperPunish.integer?0:1 )) ;
		NV_SQLAdminLog(ent, NULL, va("AntiCamp: %i",  sc_camperPunish.integer?0:1));
		return;
	}

	///#compMode
	if (!Q_stricmp(arg1, "compmode" ) && ent->client->nvSess.admin >= g_modeChange.integer){
		if (g_compMode.integer) 
			trap_Cvar_Set("g_compMode",	 "0");
		else 
			trap_Cvar_Set("g_compMode",	 "1");		
		///trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,^3COMPETITION MODE ^7has been %s", level.time + 5000, g_compMode.integer ? "^1DISABLED" : "^gENABLED"));
		///RPM_GlobalSound(G_SoundIndex("sound/misc/events/tut_door01.mp3"));
		if(ent && ent->client)
			SC_adminLog (va("%s (%s) - Compmode: %i", ent->client->nvSess.cleanName, ent->client->nvSess.ip, g_compMode.integer?0:1 ));
		else 
			SC_adminLog (va("%s - Compmode: %i", "RCON", g_compMode.integer?0:1 )) ;
		NV_SQLAdminLog(ent, NULL, va("Compmode: %i",  g_compMode.integer?0:1));
		return;
	}
	///#ChangeRat
	///	if ((!Q_stricmp ( arg1, "chrat" ))  && ent->client->nvSess.admin >= g_namechange.integer) {
	///		Svcmd_ChangeRatio(2, ent);
	///		return;
	///	}
	///#ChangeAcc
	///if ((!Q_stricmp ( arg1, "chacc" ))  && ent->client->nvSess.admin >= g_namechange.integer){
	///	Svcmd_ChangeAcc(2, ent);
	///	return;
	///}	
	///if (!Q_stricmp ( arg1, "effects" )	&& ent->client->nvSess.admin >= g_torture.integer)
	///{
	///	SC_playEffects(2, ent);
	///	return;
	///}
	///if ((!Q_stricmp(arg1, "endgame" )) /*&& ent->client->nvSess.admin >= g_endgame.integer*/){
	///	trap_Cvar_Set("sc_allowTie",	 "1");
	///	trap_SendConsoleCommand( EXEC_INSERT, va("timelimit %d\n", 1) );
	///	trap_SendServerCommand( -1, va("chat -1 \"^1                  ^3Server ^7Stats ^3Updating! ^4- ^7Mapcycle ^3command issued!^7!\n\""));
	///	CheckExitRules();
	///	return;
	///}
	///03.26.05 - 03.33pm #Sound
	///	if (!Q_stricmp ( arg1, "play" ) && ent->client->nvSess.admin >= g_sounds.integer) {
	///		SC_Sound(ent);
	///		return;
	///	}
	///	if (!Q_stricmp ( arg1, "playteam" ) && ent->client->nvSess.admin >= g_sounds.integer) {
	///		SC_TeamSound(ent);
	///		return;
	///	}
#ifdef _DB_DLL
		if (!Q_stricmp(arg1, "alias" ) && ent->client->nvSess.admin >= g_ban.integer)
		{	
			NV_getIPAliase(2, ent);
			return;
		}
#endif

	if(strcmp(arg1, "") && ent->client->nvSess.admin && NV_extraCommands(ent, arg1))
	{
		return;
	}
	//if(RPM_NameListCheck (0, arg1, g_extraCommands.string, ent, qfalse, qfalse, qfalse) && ent->client->nvSess.admin >= g_extracmds.integer){
	//	trap_Argv( 2, arg2, sizeof( arg2 ) );
	//	trap_Argv( 3, arg3, sizeof( arg3 ) );
	//	trap_SendConsoleCommand( EXEC_APPEND, va("%s %s %s\n", arg1, arg2, arg3 ) );
	//	return;
	//}

	else
	{
		trap_SendServerCommand( ent-g_entities, va("print \"Unknown Command %s.\n\"", arg1));
		trap_SendServerCommand( ent-g_entities, va("print \"Usage: adm <command> <variable>\n\""));
		return;
	}
}

#endif


