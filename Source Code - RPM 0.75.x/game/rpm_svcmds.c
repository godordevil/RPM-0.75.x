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
gspawn_t* G_SelectClientSpawnPoint ( gentity_t* ent,  qboolean plantsk );
char		*ConcatArgs		(int start);
void		G_Say			(gentity_t *ent, gentity_t *target, int mode, const char *chatText ); 
//===================================================================
//	ADMIN COMMANDS
//===================================================================

/*
======================
Svcmd_Mute
======================
*/
void Svcmd_Mute (int argNum, gentity_t *adm)
{
	int		idnum;
	char	*status;

	idnum = RPM_ClientNumFromArg(adm, argNum, "mute/unmute <idnumber>", "Mute", qfalse);

	if(idnum < 0) return;

	if(g_entities[idnum].client->sess.mute)	{
		status = "UN-MUTED";
		g_entities[idnum].client->sess.mute = qfalse;
	}
	else {
		status = "MUTED";
		g_entities[idnum].client->sess.mute = qtrue;
	}

#ifdef _TORTURE
	if(g_entities[idnum].client->nvSess.torture)
		return;
#endif
	
	// GodOrDevil - 02.26.05 - 02:59am #adminLog
	if(adm && adm->client){
		trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,^3%s was ^g%s ^7by ^6%s", level.time + 5000,	g_entities[idnum].client->pers.netname,	status,	adm->client->pers.netname));
		SC_adminLog (va("%s (%s) - %s: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip,  g_entities[idnum].client->sess.mute?"MUTE":"UNMUTE", g_entities[idnum].client->nvSess.cleanName,  g_entities[idnum].client->nvSess.ip  )) ;
	}
	else {
		trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,^3%s was ^g%s ^7by ^6ADMIN!", level.time + 5000, g_entities[idnum].client->pers.netname,	status));
		SC_adminLog (va("%s - %s: %s (%s)", "RCON", g_entities[idnum].client->sess.mute?"MUTE":"UNMUTE", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip   )) ;
	}
	NV_SQLAdminLog(adm, &g_entities[idnum], "MUTE");

	///End  - 02.26.05 - 02:59am
}

/*
======================
Svcmd_Strip
======================
*/
void Svcmd_Strip (int argNum, gentity_t *adm)
{
	gentity_t	*ent;
	gclient_t	*client;
	int			idnum;

	// GodOrDevil - 01.10.06 - 12:07pm
	int			idle = 0;
	///End  - 01.10.06 - 12:07pm

	idnum = RPM_ClientNumFromArg(adm, argNum, "strip <idnumber>", "Strip", qtrue);

	if(idnum < 0)
		return;

	ent    = g_entities + idnum;
	client = ent->client;
	
	// GodOrDevil - 01.10.06 - 12:07pm
	/*
	// Clear all ammo, clips, and weapons
	ent->client->ps.zoomFov = 0;	//if they are looking through a scope go to normal view
	client->ps.stats[STAT_WEAPONS] = 0;
	memset ( client->ps.ammo, 0, sizeof(client->ps.ammo) );
	memset ( client->ps.clip, 0, sizeof(client->ps.clip) );

	if(ent->client->nvSess.torture){
		ent->client->ps.weapon = WP_NONE;
		client->ps.weaponstate = WEAPON_READY;
		client->ps.weaponTime = 0;
		client->ps.weaponAnimTime = 0;
		return;
	}

	ent->client->ps.pm_flags &= ~(PMF_GOGGLES_ON|PMF_ZOOM_FLAGS);
	client->ps.stats[STAT_GOGGLES] = GOGGLES_NONE;
	
	///give the client 1 knife thats it
	client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
	client->ps.clip[ATTACK_NORMAL][WP_KNIFE]=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
	client->ps.firemode[WP_KNIFE] = BG_FindFireMode ( WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO );
	///set the knife for them
	client->ps.weapon = WP_KNIFE;

	client->ps.weaponstate = WEAPON_READY;
	client->ps.weaponTime = 0;
	client->ps.weaponAnimTime = 0;
	///dont give them grenades if the pickup a backpack
	client->ps.stats[STAT_OUTFIT_GRENADE] = bg_itemlist[bg_outfittingGroups[-1][client->pers.outfitting.items[-1]]].giTag;
	*/

	/// Clear all ammo, clips, and weapons
	ent->client->ps.zoomFov = 0;	///if they are looking through a scope go to normal view
	ent->client->ps.pm_flags &= ~(PMF_GOGGLES_ON|PMF_ZOOM_FLAGS);
	client->ps.stats[STAT_WEAPONS] = 0;
	client->ps.stats[STAT_GOGGLES] = GOGGLES_NONE;
	   
	memset ( client->ps.ammo, 0, sizeof(client->ps.ammo) );
	memset ( client->ps.clip, 0, sizeof(client->ps.clip) );
#ifdef _DEBUG_OFF
	/// - Give them a nade to test shooting nades in hand
	if(1)
	{
		char	arg1[2] = "\0";
		int		plus = 0;
		trap_Argv( 2, arg1, sizeof( arg1 ) );
		plus = atoi(arg1);
		if(plus < 0 || plus > 7)
			plus = 0;
		client->ps.stats[STAT_WEAPONS] |= ( 1 << (WP_M67_GRENADE + plus));
		client->ps.clip[ATTACK_NORMAL][WP_M67_GRENADE+plus]=weaponData[WP_M67_GRENADE+plus].attack[ATTACK_NORMAL].clipSize;
		client->ps.firemode[WP_M67_GRENADE+plus] = BG_FindFireMode ( (weapon_t) WP_M67_GRENADE+plus, ATTACK_NORMAL, WP_FIREMODE_AUTO );
		client->ps.weapon = WP_M67_GRENADE+plus;
		client->ps.weaponstate = WEAPON_READY;
	}
#else
	///give the client 1 knife thats it
	client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
	client->ps.clip[ATTACK_NORMAL][WP_KNIFE]=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
	client->ps.firemode[WP_KNIFE] = BG_FindFireMode ( WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO );

	client->ps.weapon = WP_KNIFE;
	BG_GetInviewAnim(client->ps.weapon,"idle",&idle);
	client->ps.weaponAnimId = idle;


	client->ps.weaponstate = WEAPON_READY;
	client->ps.weaponTime = 0;
	client->ps.weaponAnimTime = 0;
	///dont give them grenades if the pickup a backpack
	client->ps.stats[STAT_OUTFIT_GRENADE] = bg_itemlist[bg_outfittingGroups[-1][client->pers.outfitting.items[-1]]].giTag;
#endif

	if(ent->client->pers.camped){
		ent->client->pers.camped = qfalse;
		return;
	}

#ifdef _TORTURE
	if(ent->client->nvSess.torture)
		return;
#endif

	if(adm && adm->client)	{
		trap_SendServerCommand(idnum, va("cp \"^7You have been ^4STRIPPED\n^7by ^6%s!\n\"", adm->client->pers.netname));
		trap_SendServerCommand(-1, va("print\"^3%s WAS ^4STRIPPED ^7BY ^6%s!\n\"", g_entities[idnum].client->pers.netname, adm->client->pers.netname));
		SC_adminLog (va("%s (%s) - STRIP: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName,  g_entities[idnum].client->nvSess.ip  )) ;
	}
	else{
		trap_SendServerCommand(idnum, va("cp \"^7You have been ^4STRIPPED ^7by ^6ADMIN!\n\""));
		trap_SendServerCommand(-1, va("print\"^3%s WAS ^4STRIPPED ^7BY ^6ADMIN!\n\"", g_entities[idnum].client->pers.netname));
		SC_adminLog (va("%s - STRIP: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName,  g_entities[idnum].client->nvSess.ip  )) ;
	}	
	NV_SQLAdminLog(adm, &g_entities[idnum], "STRIP");

	///End  - 02.26.05 - 02:59am
}

/*
======================
Svcmd_Runover
======================
*/
void Svcmd_Runover (int argNum, gentity_t *adm)
{
	gentity_t	*ent;
	//gentity_t   *tent;
	vec3_t	dir;
	vec3_t  fireAngs;
	float   knockback = 400.0;
	int		idnum;
	int		anim = 0;

	idnum = RPM_ClientNumFromArg(adm, argNum, "runover <idnumber>", "Runover", qtrue);

	if(idnum < 0)
	{
		return;
	}

	ent = g_entities + idnum;

	VectorCopy(ent->client->ps.viewangles, fireAngs);	//which way is the client facing
	AngleVectors( fireAngs, dir, NULL, NULL );			//so we can knock them back
	dir[0] *= -1.0;
	dir[1] *= -1.0;
	dir[2] = 0.0;
	VectorNormalize ( dir );

	RPM_ClientSound(ent, G_SoundIndex("sound/ambience/vehicles/hit_scrape.mp3")); // GodOrDevil - Sound to client only.. its annyoing
///	RPM_GlobalSound(G_SoundIndex("sound/ambience/vehicles/hit_scrape.mp3"));
/*	tent = G_TempEntity( vec3_origin, EV_GLOBAL_SOUND );
	tent->s.eventParm = G_SoundIndex("sound/ambience/vehicles/hit_scrape.mp3");
	tent->r.svFlags = SVF_BROADCAST;
*/


	// GodOrDevil - 03.03.05 - 12:31am 
	ent->client->ps.weaponstate = WEAPON_FIRING;
	//ent->client->ps.firemode[ent->s.weapon] = WP_FIREMODE_SINGLE;
	///ent->client->ps.pm_flags &= ~PMF_JUMPING;
	ent->client->ps.velocity[2] = 20;

	anim = LEGS_RUN + (rand()%LEGS_LEANRIGHT_CROUCH_WALKRIGHT - LEGS_RUN);
	ent->client->ps.legsAnim = 
		( ( ent->client->ps.legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
	
	anim = ANIM_START_DEATHS + (rand()%29);
	ent->client->ps.torsoAnim = 
		( ( ent->client->ps.torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
	
	ent->client->ps.weaponTime = 1000;

	///If they're being tortured, dont take that much damage, Play the sound again too :)
#ifdef _TORTURE
	if(ent->client->nvSess.torture || ent->client->pers.camped){
#else
	if (ent->client->pers.camped){
#endif
		///RPM_ClientSound(ent, G_SoundIndex("sound/ambience/vehicles/hit_scrape.mp3"));
		G_Damage (ent, NULL, NULL, NULL, NULL, 10, 0, MOD_CAR, HL_NONE );
		G_ApplyKnockback ( ent, dir, knockback );	///knock them back
		return;
	}
	if(adm && adm->client){
		trap_SendServerCommand( -1, va("print \"^3%s was runover by ^3%s^7's ^SCAR^7!\n\"", ent->client->pers.netname, adm->client->pers.netname));
		SC_adminLog (va("%s (%s) - RUNOVER: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName,g_entities[idnum].client->nvSess.ip  )) ;
	}
	else{
		trap_SendServerCommand( -1, va("print \"^3%s was runover by the admins ^SCAR!\n\"", ent->client->pers.netname));
		SC_adminLog (va("%s - RUNOVER: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName,  g_entities[idnum].client->nvSess.ip )) ;
	}	
	///End  - 02.26.05 - 02:59am

	G_Damage (ent, NULL, NULL, NULL, NULL, 15, 0, MOD_CAR, HL_NONE );	//do the damage
	G_ApplyKnockback ( ent, dir, knockback );	//knock them back
	NV_SQLAdminLog(adm, ent, "RUNOVER");
}

/*
===============
Svcmd_pop
===============
*/

void Svcmd_pop (int argNum, gentity_t *adm)
{
	gentity_t		*ent;
	int				idnum;

	idnum = RPM_ClientNumFromArg(adm, argNum, "pop <idnumber>", "Pop", qtrue);

	if(idnum < 0)
	{
		return;
	}

	ent = g_entities + idnum;

	///We use g_damage since the animations dont work if we use player_die
	// GodOrDevil - 03.03.05 - 12:33am - #Tortured/#camped message #POP: less gibs
	///G_Damage (ent, NULL, NULL, NULL, NULL, 10000, 0, MOD_POP, HL_HEAD|HL_FOOT_RT|HL_FOOT_LT|HL_LEG_UPPER_RT|HL_LEG_UPPER_LT|HL_LEG_LOWER_RT|HL_LEG_LOWER_LT|HL_HAND_RT|HL_HAND_LT|HL_ARM_RT|HL_ARM_LT|HL_WAIST|HL_BACK_RT|HL_BACK_LT|HL_BACK|HL_CHEST_RT|HL_CHEST_LT|HL_CHEST|HL_NECK);
	G_Damage (ent, NULL, NULL, NULL, NULL, 10000, 0, MOD_POP, HL_HEAD|HL_FOOT_RT|HL_FOOT_LT|HL_LEG_UPPER_RT|HL_LEG_UPPER_LT|HL_HAND_RT|HL_HAND_LT|HL_WAIST|HL_CHEST|HL_NECK);
#ifdef _TORTURE
	if(ent->client->pers.camped || ent->client->nvSess.torture)
#else
	if (ent->client->pers.camped )
#endif 
	{
		ent->client->pers.camped = qfalse;
		return;
	}	
	
	RPM_GlobalSound(G_SoundIndex("sound/npc/air1/guard02/laughs.mp3"));

	if(adm && adm->client)
	{
		trap_SendServerCommand( -1, va("print \"%s WAS POPED BY ^3%s.\n\"", ent->client->pers.netname, adm->client->pers.netname));
		SC_adminLog (va("%s (%s) - POP: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip  )) ;
	}
	else
	{
		SC_adminLog (va("%s - POP: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip )) ;
		trap_SendServerCommand( -1, va("print \"%s WAS POPED BY ^6ADMIN.\n\"", ent->client->pers.netname));
	}
	NV_SQLAdminLog(adm, ent, "POP");
}
void RPM_Plant (int argNum, gentity_t *adm, qboolean plantsk)
{
	gentity_t *ent;
	int		idnum;

	if(plantsk)
		idnum = argNum;
	else
		idnum = RPM_ClientNumFromArg(adm, argNum, "plant <idnumber>", "Plant", qtrue);

	if(idnum < 0)
		return;

	ent = g_entities + idnum;

	if(ent->client->pers.planted){
		if(adm && adm->client)
			trap_SendServerCommand( adm-g_entities, va("print \"^3%s is already Planted\n\"", ent->client->pers.netname));
		else
			Com_Printf("^3%s is already Planted\n", ent->client->pers.netname);
		return;
	}

	if ( ent->client->ps.pm_flags & PMF_DUCKED )
		ent->client->ps.origin[2] -=40;
	else
		ent->client->ps.origin[2] -= 65;

	VectorCopy( ent->client->ps.origin, ent->s.origin );

	ent->client->pers.planted = qtrue;

	// GodOrDevil - 03.03.05 - 12:27am - Dont play sound globally if in #Torture 
#ifdef _TORTURE

	if(ent->client->nvSess.torture){
		RPM_ClientSound(ent, G_SoundIndex("sound/misc/confused/wood_break.mp3"));
		return;
	}
	else
#endif
		RPM_GlobalSound(G_SoundIndex("sound/misc/confused/wood_break.mp3"));

	if(ent->client->pers.camped){
		ent->client->pers.camped = qfalse;
		return;
	}
	// GodOrDevil - 02.26.05 - 02:59am #adminLog
	if(adm && adm->client){
		SC_adminLog (va("%s (%s) - PLANT: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip  )) ;
		trap_SendServerCommand( -1, va("print \"%s WAS PLANTED BY ^3%s.\n\"", ent->client->pers.netname, adm->client->pers.netname));
	}
	else{
		trap_SendServerCommand( -1, va("print \"%s WAS PLANTED BY ^6ADMIN.\n\"", ent->client->pers.netname));
		SC_adminLog (va("%s - PLANT: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip   )) ;
	}
	///End  - 02.26.05 - 02:59am
	NV_SQLAdminLog(adm, &g_entities[idnum], "PLANT");

}
void RPM_unPlant (int argNum, gentity_t *adm)
{
	gentity_t *ent;
	int		idnum;

	idnum = RPM_ClientNumFromArg(adm, argNum, "unplant <idnumber>", "Unplant", qtrue);
	if(idnum < 0){return;}

	ent = g_entities + idnum;

	if(!ent->client->pers.planted){
		if(adm && adm->client)
			trap_SendServerCommand( adm-g_entities, va("print \"^3%s is not Planted\n\"", ent->client->pers.netname));
		 else
			Com_Printf("^3%s is not Planted\n", ent->client->pers.netname);
		return;
	}

	if(ent->client->pers.planted){
		ent->client->ps.origin[2] += 65;
		VectorCopy( ent->client->ps.origin, ent->s.origin );
		ent->client->pers.planted = qfalse;
	}

	///If they are being tortured, dont fuck em up too bad with this. So we can fuck em with other shit too :)
#ifdef _TORTURE
	if(ent->client->nvSess.torture){
		RPM_ClientSound(ent, G_SoundIndex("sound/misc/confused/wood_break.mp3"));
		G_Damage (ent, NULL, NULL, NULL, NULL, 5, 0, MOD_DUGUP, HL_NONE );
		return;
	}
	else 
#endif
	{
		RPM_GlobalSound(G_SoundIndex("sound/misc/confused/wood_break.mp3"));
		G_Damage (ent, NULL, NULL, NULL, NULL, 70, 0, MOD_DUGUP, HL_NONE );
	}
	
	if(ent->client->pers.camped){
		ent->client->pers.camped = qfalse;
		return;
	}	
		
	// GodOrDevil - 02.26.05 - 02:59am #adminLog
	if(adm && adm->client){
		SC_adminLog (va("%s (%s) - UNPLANT: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip )) ;
		trap_SendServerCommand( -1, va("print \"%s WAS UNPLANTED BY ^3%s.\n\"", ent->client->pers.netname, adm->client->pers.netname));
	} else {
		trap_SendServerCommand( -1, va("print \"%s WAS UNPLANTED BY ^6ADMIN.\n\"", ent->client->pers.netname));
		SC_adminLog (va("%s - UNPLANT: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip  )) ;
	}
	///End  - 02.26.05 - 02:59am 
	NV_SQLAdminLog(adm, &g_entities[idnum], "UNPLANT");

}
void RPM_PlantSK (int argNum, gentity_t *adm)
{
	int		  idnum;
	gentity_t *ent;
	gspawn_t  *spawnPoint;

	idnum = RPM_ClientNumFromArg(adm, argNum, "plantsk <idnumber>", "Plant", qtrue);
	if(idnum < 0){ return; }

	ent = g_entities + idnum;

	if(ent->client->pers.planted){
		if(adm && adm->client)
			trap_SendServerCommand( adm-g_entities, va("print \"^3%s is already Planted\n\"", ent->client->pers.netname));
		else
			Com_Printf("^3%s is already Planted\n", ent->client->pers.netname);
		return;
	}

	/// Clear all ammo, clips, and weapons
	ent->client->ps.zoomFov = 0;	///if they are looking through a scope go to normal view
	ent->client->ps.pm_flags &= ~(PMF_GOGGLES_ON|PMF_ZOOM_FLAGS);
	ent->client->ps.stats[STAT_WEAPONS] = 0;
	ent->client->ps.stats[STAT_GOGGLES] = GOGGLES_NONE;
	memset ( ent->client->ps.ammo, 0, sizeof(ent->client->ps.ammo) );
	memset ( ent->client->ps.clip, 0, sizeof(ent->client->ps.clip) );

	ent->client->ps.weapon = WP_NONE;
	ent->client->ps.weaponstate = WEAPON_READY;
	ent->client->ps.weaponTime = 0;
	ent->client->ps.weaponAnimTime = 0;

	/// Where do we spawn?
	spawnPoint = G_SelectClientSpawnPoint ( ent, qtrue );

	if (!spawnPoint ) {
		return;
	}
	VectorCopy( spawnPoint->origin, ent->client->ps.origin );
	SetClientViewAngle( ent, spawnPoint->angles );

	RPM_Plant(idnum, adm, qtrue);
}

void Svcmd_Ban_f (int argNum, gentity_t *adm)
{
	int				idnum;
	char            banid[128];
	char			reason[22] = "\0";
	qboolean		added = qfalse;
	gentity_t*		ent;

#ifdef _DB_DLL
	char				banTime[5] = "\0";
#endif

	idnum = RPM_ClientNumFromArg(adm, argNum, "ban <idnumber> <reason>", "Ban", qfalse);

	if(idnum < 0)
		return;
		
	ent = &g_entities[idnum];
#ifdef _DB_DLL
	trap_Argv( argNum + 1, banTime, sizeof( banTime ) );
	trap_Argv( argNum + 2, reason, sizeof( reason ) );

	if(g_enableDatabase.integer == 2)
	{		
		if(!banTime[0])
			Com_sprintf(banTime, sizeof(banTime), "0");
		added = qtrue;
		if(!*reason)
		{	
			if(adm && adm->client)	
			{
				NV_QueryDB (DB_LITE_ADDBAN, va("%i;%s;%s;%s;%s;%0.4f;%s", adm->s.number, adm->client->nvSess.ip2, adm->client->nvSess.cleanName, ent->client->nvSess.ip2, ent->client->nvSess.cleanName, atof(banTime), " "));
				trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"BANNED FROM SERVER! by %s\"\n", idnum, adm->client->pers.netname));
			}
			else
			{
				NV_QueryDB (DB_LITE_ADDBAN, va("%i;%s;%s;%s;%s;%0.4f;%s", -1, "RCON","RCON", ent->client->nvSess.ip2, ent->client->nvSess.cleanName, atof(banTime), " "));
				trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"BANNED FROM SERVER! by %s\"\n", idnum, "RCON"));
			}
		}
		else
		{	
			if(adm && adm->client)
			{		
				NV_QueryDB (DB_LITE_ADDBAN, va("%i;%s;%s;%s;%s;%0.4f;%s", adm->s.number, adm->client->nvSess.ip2, adm->client->nvSess.cleanName, ent->client->nvSess.ip2, ent->client->nvSess.cleanName, atof(banTime), reason));
				trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"BANNED by %s for: %s\"\n", idnum, adm->client->pers.netname, reason));
			}
			else
			{		
				NV_QueryDB (DB_LITE_ADDBAN, va("%i;%s;%s;%s;%s;%0.4f;%s", -1, "RCON","RCON", ent->client->nvSess.ip2, ent->client->nvSess.cleanName, atof(banTime), reason));
				trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"BANNED by %s for: %s\"\n", idnum, "RCON", reason));
			}
		}
	}	
	else
	{
#endif
		trap_Argv( argNum + 1, reason, sizeof( reason ) );
		
		Com_sprintf (banid, sizeof(banid), "%s\\%s", ent->client->nvSess.ip,	ent->client->nvSess.cleanName);

		if(RPM_AddToList(banid, g_banlist.string, "Ban", adm))
		{	
			added = qtrue;
		}
		if(!*reason)
		{	
			if(adm && adm->client)	
				trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"BANNED FROM SERVER! by %s\"\n", idnum, adm->client->pers.netname));
			else
				trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"BANNED FROM SERVER! by %s\"\n", idnum, "RCON"));
		}
		else
		{	
			if(adm && adm->client)
				trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"BANNED by %s for: %s\"\n", idnum, adm->client->pers.netname, reason));
			else
				trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"BANNED by %s for: %s\"\n", idnum, "RCON", reason));
		}
#ifdef _DB_DLL
	}
#endif

	if(added)
	{
		if(adm && adm->client)	
		{
			///kick them with a generic reason or personal
			trap_SendServerCommand( -1, va("print \"%s was BANNED by %s\n\"", ent->client->pers.netname, adm->client->pers.netname));
			SC_adminLog (va("%s (%s) - BAN: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, ent->client->nvSess.cleanName, ent->client->nvSess.ip  )) ;
		}
		else	
		{					
			trap_SendServerCommand( -1, va("print \"%s was BANNED by RCON!\n\"", ent->client->pers.netname));
			Com_Printf("%s was BANNED!\n", ent->client->pers.netname);
			SC_adminLog (va("%s - BAN: %s (%s)", "RCON", ent->client->nvSess.cleanName, ent->client->nvSess.ip  )) ;
		}
	}
}

void NV_RPMUnban(gentity_t *adm, char*ip, int userID)
{		
	if(userID > 0)
	{
		NV_QueryDB (DB_LITE_UNBAN, va("%i;%s;%i;", userID, "0", adm->s.number));

		if(adm && adm->client)
			SC_adminLog (va("%s (%s)- UNBAN ID: %i", adm->client->nvSess.cleanName, adm->client->nvSess.ip, userID  )) ;
		else 
			SC_adminLog (va("%s - UNBAN ID: %i", "RCON", userID  )) ;
		NV_SQLAdminLog(adm, NULL, va("UNBAN ID: %i", userID));
		return;
	}
	NV_QueryDB (DB_LITE_UNBAN, va("%i;%s;%i;", -1, ip, adm->s.number));

	if(adm && adm->client)
		SC_adminLog (va("%s (%s)- UNBAN IP: %s", adm->client->nvSess.cleanName, adm->client->nvSess.ip, ip  )) ;
	else 
		SC_adminLog (va("%s - UNBAN IP: %s", "RCON", ip  )) ;
	
	NV_SQLAdminLog(adm, NULL, va("UNBAN IP: %s", ip));
}

void RPM_Unban(gentity_t *adm, char *ip, qboolean subnet)
{
	int		count = 0, count2 = 0;

#ifdef _DB_DLL
	if(g_enableDatabase.integer == 2)
	{	
		if(strstr(ip, "."))
		{	
			while (ip[count] != '\0'){
				if(ip[count] == ' '){
					count++;
					continue;
				}

				if(ip[count] == '.'){
					count++;
					count2++;
				}
				if(ip[count] < '0' || ip[count] > '9')	{
					trap_SendServerCommand( adm-g_entities, va("print \"^3%s ^7is an invalid ip address\n\"", ip));
					return;
				}
				count++;
			}
			NV_RPMUnban(adm, ip, -1 );
		}
		else			
		{
			int id = atoi(ip);
			if(id > 0)
			{
				NV_RPMUnban(adm, "999", id );
			}
		}
		return;
	}
#endif

	while (ip[count] != '\0'){
		if(ip[count] == ' '){
			count++;
			continue;
		}

		if(ip[count] == '.'){
			count++;
			count2++;
		}
		if(ip[count] < '0' || ip[count] > '9')	{
			trap_SendServerCommand( adm-g_entities, va("print \"^3%s ^7is an invalid ip address\n\"", ip));
			return;
		}
		count++;
	}

	if(!subnet){
		if(count2 < 3){
			trap_SendServerCommand( adm-g_entities, va("print \"^3%s ^7is an invalid ip address\n\"", ip));
			return;
		}

			if(RPM_Remove_from_list(ip, g_banlist.string, "Ban", adm, qtrue, qfalse ))
			{				
				trap_SendServerCommand( adm-g_entities, va("print \"^3%s ^7has been Unbanned.\n\"", ip));
				// GodOrDevil - 02.26.05 - 02:59am #adminLog
				if(adm && adm->client)
					SC_adminLog (va("%s (%s)- UNBAN: %s", adm->client->nvSess.cleanName, adm->client->nvSess.ip, ip  )) ;
				else 
					SC_adminLog (va("%s - UNBAN: %s", "RCON", ip  )) ;
				///End  - 02.26.05 - 02:59am
				NV_SQLAdminLog(adm, NULL, va("UNBAN IP: %s", ip));				
				return;
			}
	}
	else {
		if(count2 < 1){
			trap_SendServerCommand( adm-g_entities, va("print \"^3%s ^7is an invalid ip address\n\"", ip));
			return;
		}
		// GodOrDevil - 01.22.06 - 05:51pm #subnetUnban
		if(RPM_Remove_from_list(ip, g_subnetbanlist.string, "SubnetBan", adm, qtrue, qfalse ))
		{
			trap_SendServerCommand( adm-g_entities, va("print \"^3%s's Subnet ^7has been Unbanned.\n\"", ip));
			if(adm && adm->client)
				SC_adminLog (va("%s (%s) - SUBNET UNBAN: %s", adm->client->nvSess.cleanName, adm->client->nvSess.ip, ip  )) ;
			else 
				SC_adminLog (va("%s - SUBNET UNBAN: %s", "RCON", ip  )) ;
			NV_SQLAdminLog(adm, NULL, va("SUBNET UNBAN: %s", ip));	
			return;
		}
		///End  - 01.22.06 - 05:51pm
	}
}

//===================================================================
//		RCON ADMIN COMMANDS
//		GodOrDevil - Most are controlled by the admin system now
//===================================================================

void NV_AddAdmin(gentity_t *ent, gentity_t *adm, int admin, char* append, char *file, char *type, char* colored)
{	
	char		id2[64];
	qboolean	added = qfalse;

	RPM_Remove_from_list(ent->client->pers.rpm_id, g_adminfile.string,	 "admin", NULL, qfalse, qtrue);	
#ifdef _BADMIN_FILE
	RPM_Remove_from_list(ent->client->pers.rpm_id, g_badminfile.string,	 "badmin", NULL, qfalse, qtrue);
#endif

	ent->client->nvSess.admin = admin;

#ifdef _DB_DLL
	if(g_enableDatabase.integer == 2)
	{	
		if(adm && adm->client)
			NV_QueryDB (DB_LITE_ADDADMIN, va("%i;%s;%s;", admin, ent->client->nvSess.ip2, ent->client->nvSess.cleanName));
		else
			NV_QueryDB (DB_LITE_ADDADMIN, va("%i;%s;%s;", admin, ent->client->nvSess.ip2, ent->client->nvSess.cleanName));

		added = qtrue;
	}	
	else
#endif	
	{
		Q_strncpyz (id2, ent->client->pers.rpm_id, 64);
		strcat ( id2, append );
		if(RPM_AddToList(id2, file, "Admin", NULL)) 
		{
			added = qtrue;
		}
	}
	
	Com_Printf("%s was added to the ^6Admin ^7list. Status %s\n", ent->client->pers.netname, type);
	if(g_noRanks.integer)
		trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@^3%s now has ^6ADMIN ^1POWERS.", level.time + 5000,	ent->client->pers.netname));
	else 
		trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@^3%s now has %s ^1POWERS.", level.time + 5000,	ent->client->pers.netname, colored));
	if(adm && adm->client)
		SC_adminLog (va("%s (%s) - Add%s: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, type, ent->client->nvSess.cleanName, ent->client->nvSess.ip  )) ;
	else 
		SC_adminLog (va("%s - Add%s: %s (%s)", "RCON", type, ent->client->nvSess.cleanName, ent->client->nvSess.ip )) ;
		
	NV_SQLAdminLog(adm, ent, va("Add%s", type));	
}
void Svcmd_Add_sAdmin_f(int argNum, gentity_t *adm)
{
	int             idnum;

	idnum = RPM_ClientNumFromArg(adm, argNum, "addsadmin <idnumber>", "do this to", qfalse);
	if(idnum < 0) return;

	NV_AddAdmin(&g_entities[idnum], adm, 4, ":4", g_adminfile.string, "SAdmin", "^gS-ADMIN");
}
void Svcmd_Add_Admin_f(int argNum, gentity_t *adm)
{
	int             idnum;

	idnum = RPM_ClientNumFromArg(adm, argNum, "addadmin <idnumber>", "do this to", qfalse);
	if(idnum < 0) return;

	NV_AddAdmin(&g_entities[idnum], adm, 3, ":3", g_adminfile.string, "Admin", "^6ADMIN");
}
void Svcmd_Add_bAdmin_f(int argNum, gentity_t *adm)
{
	int             idnum;

	idnum = RPM_ClientNumFromArg(adm, argNum, "addbadmin <idnumber>", "do this to", qfalse);
	if(idnum < 0) return;

	NV_AddAdmin(&g_entities[idnum], adm, 2, ":2", g_adminfile.string, "BAdmin", "^4B-ADMIN");
}

void Svcmd_Remove_Admin_f (void)

{
	int				idnum;
	char			*id;
	gentity_t		*ent;
	idnum = RPM_ClientNumFromArg(NULL, 1, "removeadm <idnumber>", "", qfalse);
	if(idnum < 0) return;

	ent = &g_entities[idnum];
		
	ent->client->nvSess.admin = 0;

#ifdef _DB_DLL
	if(g_enableDatabase.integer == 2)
	{			
		NV_QueryDB (DB_LITE_ADDADMIN, va("%i;%s;%s;", ent->client->nvSess.admin, ent->client->nvSess.ip2, ent->client->nvSess.cleanName));
		Com_Printf("^7%s ^7was ^3removed ^7from the admin list.\n", ent->client->pers.netname);
		trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@^3%s no longer has ^6ADMIN ^1POWERS.",	level.time + 5000, ent->client->pers.netname));
		SC_adminLog (va("%s - RemoveAdmin: %s (%s)", "RCON", ent->client->nvSess.cleanName, ent->client->nvSess.ip )) ;
		NV_SQLAdminLog(NULL, ent, "RemoveAdmin");	
		return;
	}	
#endif

	id = g_entities[idnum].client->pers.rpm_id;
		
	RPM_Remove_from_list(id, g_adminfile.string, "admin", NULL, qfalse, qtrue);
#ifdef _BADMIN_FILE
	RPM_Remove_from_list(id, g_badminfile.string,	 "badmin", NULL, qfalse, qtrue);
#endif
	
	// GodOrDevil - 02.26.05 - 02:59am #adminLog
	SC_adminLog (va("%s - RemoveAdmin: %s (%s)", "RCON", ent->client->nvSess.cleanName, ent->client->nvSess.ip )) ;
	NV_SQLAdminLog(NULL, ent, "RemoveAdmin");	
	///End  - 02.26.05 - 02:59am
	
	trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@^3%s no longer has ^6ADMIN ^1POWERS.",	level.time + 5000, ent->client->pers.netname));
}



#ifdef _CLANMEMBER
void RPM_Add_Clan_Member(int argNum, gentity_t *adm)
{
	int             idnum, onlist;
	char			*id;

	idnum = RPM_ClientNumFromArg(adm, argNum, "addclan <idnumber>", "do this to", qfalse);

	if(idnum < 0)
		return;

	g_entities[idnum].client->sess.clanMember = 1;

	id = g_entities[idnum].client->pers.rpm_id;

	onlist = RPM_NameListCheck (0, id, g_clanfile.string, NULL, qfalse, qfalse, qfalse);

	if(onlist) {
		Com_Printf("%s is already a ^6Clan Member.\n", g_entities[idnum].client->pers.netname);
		return;
	}
	if(onlist == -1)
		return;

	if(RPM_AddToList(id, g_clanfile.string, "Clan", NULL))
	{
		trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@^3%s ^7is now a ^1Clan Member.",
		level.time + 5000,
		g_entities[idnum].client->pers.netname));
	///	trap_SendServerCommand(-1, va("print \"^3%s was added to the ^6Clan Member List^7!\n\"", g_entities[idnum].client->pers.netname ));
		Com_Printf("^3%s was added to the ^6Clan Member List^7!\n", g_entities[idnum].client->pers.netname );
		
		// GodOrDevil - 02.26.05 - 02:59am #adminLog
		if(adm && adm->client)
			SC_adminLog (va("%s (%s) - AddClan: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip )) ;
		else 
			SC_adminLog (va("%s - AddClan: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip )) ;
	///End  - 02.26.05 - 02:59am
	}
}
// GodOrDevil - Remove #ClanMember
/*
======================
RPM_Remove_Clan_Member
======================
*/

void RPM_Remove_Clan_Member(int argNum, gentity_t *adm)
{
	int			idnum;
	char		*id;

	idnum = RPM_ClientNumFromArg(adm, argNum, "removeclan <idnumber>", "remove", qfalse);

	///if invalid client etc.. idnum will == -1 and we abort
	if(idnum < 0)
		return;

	///make sure they are not admins even if not on list
	g_entities[idnum].client->sess.clanMember = 0;

	id = g_entities[idnum].client->pers.rpm_id;


	if(RPM_Remove_from_list(id, g_clanfile.string, "Clan", NULL, qfalse, qfalse))
	{
		trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@^3%s is no longer a ^1Clan Member.",
			level.time + 5000,
			g_entities[idnum].client->pers.netname));
	///	trap_SendServerCommand( -1, va("print \"^3%s was removed from the ^6Clan Member List^7!\n\"", g_entities[idnum].client->pers.netname ));
		Com_Printf("^3%s was removed from the ^6Clan Member List^7!\n", g_entities[idnum].client->pers.netname );
		
		// GodOrDevil - 02.26.05 - 02:59am #adminLog		
		if(adm && adm->client)
			SC_adminLog (va("%s (%s) - RemoveClan: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip )) ;
		else 
			SC_adminLog (va("%s - RemoveClan: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip  )) ;
		///End  - 02.26.05 - 02:59am
	}
}
#endif
void Svcmd_admWarn_cmd(int argNum, gentity_t *adm)
{
	int		idnum;
	char	reason[MAX_TOKEN_CHARS];

	gentity_t	*ent;
	gclient_t	*client;

	idnum = RPM_ClientNumFromArg(adm, argNum, "warn <idnumber> <reason>", "", qfalse);
	if(idnum < 0) return;

	ent    = g_entities + idnum;
	client = ent->client;

	trap_Argv( argNum + 1, reason, sizeof( reason ) );
	RPM_ClientSound(ent, G_SoundIndex("sound/misc/events/tut_lift02.mp3"));
	///CJJ - 1.17.2005 - Warn Counter
	client->sess.warnCount++;

	if (!G_IsClientSpectating(client) || !G_IsClientDead ( client )){
		if (ent->client->sess.team != TEAM_SPECTATOR && !ent->client->sess.ghost && ent->client->ps.pm_type != PM_DEAD){
			switch(client->sess.warnCount){
				default:
				case 1:
					client->ps.origin[2] -=20;
					break;
				case 2:
					SC_Twist(argNum, NULL);
					Svcmd_Strip(argNum, NULL);
					break;
				case 3:
					ent->client->sess.mute = qtrue;
					SC_Slam(argNum, NULL);
					break;
				case 4:
					Svcmd_Strip(argNum, NULL);
					SC_Slow(argNum, NULL);
					break;
				case 5:
					trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"KICKED! Follow the rules or dont come back!\"\n", ent->s.number ) );
					break;
			}
		}
	}

	ent->client->sess.warnSeconds = 10;
	Q_strncpyz(ent->client->sess.warnReason, *reason?reason:"Not Following The Server Rules!", sizeof(ent->client->sess.warnReason));

	if(ent->client->pers.camped){
		ent->client->pers.camped = qfalse;
		return;
	}

	if(adm && adm->client) {
		SC_adminLog (va("%s (%s) - Warn: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip )) ;
		trap_SendServerCommand(-1, va("print \"^3%s ^7has been ^1WARNED ^7by ^6%s\n^4REASON:^3 %s\n\"", g_entities[idnum].client->pers.netname, adm->client->pers.netname, *reason?reason:"Not Following The Server Rules!"));
	} else	{
		SC_adminLog (va("%s - Warn: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip  )) ;
		trap_SendServerCommand(-1, va("print \"^3%s ^7has been ^1WARNED ^7by ^6%s\n^4REASON:^3 %s\n\"", g_entities[idnum].client->pers.netname, "ADMIN", *reason?reason:"Not Following The Server Rules!"));
	}
	NV_SQLAdminLog(adm, ent, "Warn");	
}
void SC_Torture(int argNum, gentity_t *adm)
{
	int			idnum;
	char		reason[MAX_TOKEN_CHARS];
	char		ban[4];
	char		banid[128];
	gentity_t	*ent;
	gclient_t	*client;
	
	idnum = RPM_ClientNumFromArg(adm, argNum, "torture <id> <reason> <ban>", "Torture", qtrue);
	if(idnum < 0) return;

	ent    = g_entities + idnum;
	client = ent->client;

	if (client->nvSess.torture){
		SC_unTorture(argNum, adm);
		///trap_SendServerCommand( adm - g_entities, va("print \"^3%s^7 is already being ^1Tortured^7!.\n\"", ent->client->pers.netname));
		return;
	}

	/// get the reason/time for torture
	trap_Argv( argNum + 1, reason, sizeof( reason ) );
	trap_Argv( argNum + 2, ban, sizeof( ban ) );

	///Take care of specing victims
	///if (ent->client->sess.team == TEAM_SPECTATOR){
	///	//if(	level.gametypeData->respawnType == RT_INTERVAL ){
	///	if(	!level.gametypeData->respawnType == RT_INTERVAL){
	///		if(counts[TEAM_BLUE] > counts[TEAM_RED]) ent->client->sess.team = TEAM_RED;
	///		else ent->client->sess.team = TEAM_BLUE;
	///	}
	///	else ent->client->sess.team = TEAM_FREE;
	/// //get and distribute relevent paramters
	///client->pers.identity = NULL;
	///ClientUserinfoChanged( ent->s.number );
	///}
	///End  - 05.03.05 - 12:40am

	if ( strstr ( client->pers.identity->mCharacter->mModel, "female" ) )
		RPM_GlobalSound(G_SoundIndex("sound/ambience/colombia/battlebackground/scream01.mp3"));
	 else 
		RPM_GlobalSound(G_SoundIndex("sound/ambience/colombia/battlebackground/scream03.mp3"));

	client->nvSess.torture = qtrue;

	///05.05.05 - 07.31pm - Only for testing certain parts of torture
	///client->nvSess.tortureCount = 34;

	///drop any GT items, take their armor and weapons
	if(ent->s.gametypeitems > 0){
		G_DropGametypeItems ( ent, 10000 );
	}
	///fuck 'em up ratio-strip-mute-rename-acc
	client->ps.stats[STAT_REALARMOR] = 0;
	trap_SendConsoleCommand( EXEC_INSERT, va("strip %d\n", ent->s.number ) );
	///trap_SendConsoleCommand( EXEC_INSERT, va("chrat %i 3 193 -190\n", ent->s.number ) );	
	trap_SendConsoleCommand( EXEC_INSERT, va("rename %i \"%s\"\n", ent->s.number, g_torturedname.string ) );
	///trap_SendConsoleCommand( EXEC_INSERT, va("chacc %i 5366 10\n", ent->s.number) );
	///trap_SendConsoleCommand( EXEC_INSERT, va("forceteam %i b\n", ent->s.number ) );
	
	if (!client->sess.mute){
		trap_SendConsoleCommand( EXEC_INSERT, va("mute %i\n", ent->s.number ) );
	}

	///04.02.05 - 10.12pm - adds their name to the banlist without kicking them
	if(!Q_stricmp ( ban, "ban" ) && adm->client->nvSess.admin >= g_ban.integer){		
		Com_sprintf (banid, sizeof(banid), "%s\\%s",
			g_entities[idnum].client->nvSess.ip,
			g_entities[idnum].client->nvSess.cleanName);
			if(adm && adm->client)
			{		
				trap_SendServerCommand( -1, va("print \"%s was BANNED by %s\n\"", g_entities[idnum].client->pers.netname, adm->client->pers.netname));
				SC_adminLog (va("%s (%s) - BAN: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip  )) ;
			}
			else
			{
				trap_SendServerCommand( -1, va("print \"%s ^7was BANNED by %s\n\"", g_entities[idnum].client->pers.netname, "the server Admin"));
				SC_adminLog (va("%s - BAN: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip  )) ;				
			}
			if(RPM_AddToList(banid, g_banlist.string, "Ban", adm));
	}

	if(adm && adm->client){
		SC_adminLog (va("%s (%s) - TORTURE: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip  )) ;
		trap_SendServerCommand(-1, va("print \"^3%s ^7is going to be ^1TORTURED ^7by ^6%s\n^4REASON:^3 %s\n\"", g_entities[idnum].client->pers.netname, adm->client->pers.netname, *reason?reason:"Not Following The Server Rules!"));
	}
	else {
		SC_adminLog (va("%s - TORTURE: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip   )) ;
		trap_SendServerCommand(-1, va("print \"^3%s ^7is going to be ^1TORTURED ^7by ^6%s\n^4REASON:^3 %s\n\"", g_entities[idnum].client->pers.netname, "ADMIN", *reason?reason:"Not Following The Server Rules!"));
	}
	NV_SQLAdminLog(adm, ent, "Torture");	

	Q_strncpyz(ent->client->nvSess.tortureReason, *reason?reason:"Not Following The Server Rules!", sizeof(ent->client->nvSess.tortureReason));
}
void SC_unTorture(int argNum, gentity_t *adm)
{
	int		idnum;

	gentity_t	*ent;
	gclient_t	*client;

	idnum = RPM_ClientNumFromArg(adm, argNum, "untorture <idnumber>", "Torture", qfalse);
	if(idnum < 0) return;

	ent    = g_entities + idnum;
	client = ent->client;

	if (!client->nvSess.torture){
		return;
	}

	client->nvSess.torture = qfalse;
	client->nvSess.tortureCount = 0;
	///ent->client->nvSess.tortureTime = level.time;
	///ent->client->sess.msgTime = level.time;
	ent->client->sess.msgSeconds = 7;
	
	trap_SendConsoleCommand( EXEC_INSERT, va("pop %i\n", ent->s.number ) );
	Q_strncpyz(ent->client->sess.msgReason,"You have been ^_Tortured^7!\nIf you wish to remain on the server\n^1FOLLOW THE RULES^7\nand ^3RESPECT ALL^7 the players!\n", sizeof(ent->client->sess.msgReason));
	trap_SendServerCommand( -1, va("print \"^3%s's torture has ended.\n\"", ent->client->pers.netname));
	NV_SQLAdminLog(adm, ent, "Untorture");	

}
void Svcmd_NameChange (int argNum, gentity_t *adm)
{
	int			idnum, j;
	char		*s;
	char		userinfo[MAX_INFO_STRING];
	char		name[MAX_NETNAME] = "\0";
	char		oldname[MAX_NETNAME] = "\0";
	char		unlock[8] = "\0";
	gentity_t	*ent;
	gclient_t	*client;
///	qboolean	namelock;

	idnum = RPM_ClientNumFromArg(adm, argNum, "namechange <idnumber> <nameWithQuotes> <unlock>", "Name Change", qfalse);
	if(idnum < 0) {	return; }

	ent = g_entities + idnum;
	client = ent->client;
	
	///Clients Warning Level has to be atleast 3 to use this unless they are being tortured!
	if ((ent->client->sess.warnCount < 3 && g_needWarn.integer) /*&& !ent->client->nvSess.tortureSeconds*/){
		trap_SendServerCommand( adm-g_entities, va("print \"Clients ^1Warning Level^7 must at ^33^7 to be subjected to this punishment!\n^4%s ^7current ^1Warning Level^7: ^3%i\n\"", ent->client->pers.netname, ent->client->sess.warnCount));
		return;
	}

	// GodOrDevil - 03.05.05 - 01:50am
	///Changed so Rcon can use it too
	trap_Argv( argNum + 1, name, sizeof( name ) );
	trap_Argv( argNum + 2, unlock, sizeof( unlock ) );

	strcpy(oldname, client->pers.netname);

	trap_GetUserinfo( ent->s.number, userinfo, sizeof( userinfo ) );
	Info_SetValueForKey( userinfo, "name", name );
 	trap_SetUserinfo ( ent->s.number, userinfo );

//CJJ - 1.3.2005 - Added Clean Name Function
/* Stole this from rpm_cmds.c so that we can reuse the cleanname when we force a rename... */
	if(!g_colorTeamNames.integer){
		G_ClientCleanName( name, client->pers.netname, sizeof(client->pers.netname), level.gametypeData->teams?qfalse:qtrue );
		G_ClientCleanName( name, client->pers.talkname, sizeof(client->pers.talkname), level.gametypeData->teams?qfalse:qtrue );
	}
	else if(g_colorTeamNames.integer == 1){
		G_ClientCleanName( name, client->pers.netname, sizeof(client->pers.netname), qtrue );
		G_ClientCleanName( name, client->pers.talkname, sizeof(client->pers.talkname), level.gametypeData->teams?qfalse:qtrue );
	}
	else if(g_colorTeamNames.integer == 2){
		G_ClientCleanName( name, client->pers.netname, sizeof(client->pers.netname), qtrue );
		G_ClientCleanName( name, client->pers.talkname, sizeof(client->pers.talkname), qtrue );
	}
	else{
		G_ClientCleanName( name, client->pers.netname, sizeof(client->pers.netname), level.gametypeData->teams?qfalse:qtrue );
		G_ClientCleanName( name, client->pers.talkname, sizeof(client->pers.talkname), level.gametypeData->teams?qfalse:qtrue );
	}
	if(strlen(client->pers.netname) > MAX_NETNAME - 3) {
		client->pers.netname[MAX_NETNAME - 3] = '\0';
	}
	if(strlen(client->pers.talkname) > MAX_NETNAME - 3) {
		client->pers.talkname[MAX_NETNAME - 3] = '\0';
	}
	strcat(client->pers.netname, S_COLOR_WHITE);
	strcat(client->pers.talkname, S_COLOR_WHITE);

	G_ClientCleanName( name, client->nvSess.cleanName, sizeof(client->nvSess.cleanName), qfalse );
	

	//ClientUserinfoChanged ( ent->s.number );
	// Do this way instead.. better then checking (and setting) db and admin and stuff again for a rename.
	if ( ent->r.svFlags & SVF_BOT )
	{
		s = va("n\\%s\\t\\%i\\identity\\%s\\skill\\%s",
			name, client->sess.team, client->pers.identity->mName,
			Info_ValueForKey( userinfo, "skill" ) );
	}
	else
	{	// mullinssuit
		s = va("n\\%s\\t\\%i\\identity\\%s",
				name,  client->sess.team, client->pers.identity->mName );
	}
	trap_SetConfigstring( CS_PLAYERS+ent->s.number, s );
	//

	if(!Q_stricmp ( unlock, "unlock" ))
		client->sess.noNameChange = qfalse;
	else client->sess.noNameChange = qtrue;

	if (client->nvSess.torture)
		return;

	if(ent->client->pers.camped){
		ent->client->pers.camped = qfalse;
		return;
	}
	
	for (j = 0; j < level.numConnectedClients; j++)	{
		if (level.clients[level.sortedClients[j]].nvSess.admin >= 2){
			if(adm && adm->client)
				trap_SendServerCommand(level.sortedClients[j], va("print \"^3Name Change^7: %s to %s ^7- ^3%s\n\"", oldname, g_entities[idnum].client->nvSess.cleanName, adm->client->pers.netname));
			else
				trap_SendServerCommand(level.sortedClients[j], va("print \"^3Name Change^7: %s to %s^7- ^6SERVER\n\"",oldname, g_entities[idnum].client->nvSess.cleanName));
		}
	}

	if(adm && adm->client)
	{
		SC_adminLog (va("%s (%s) - Rename: %s to %s (%s)",  adm->client->nvSess.cleanName, adm->client->nvSess.ip, oldname, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip )) ;
	}
	else
	{
		SC_adminLog (va("%s - Rename:  %s to %s (%s)", "RCON", oldname, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip )) ;
	}
	NV_SQLAdminLog(adm, ent, va("Rename: %s to %s", oldname, g_entities[idnum].client->nvSess.cleanName ));	

}
// GodOrDevil
void RPM_Even_Teams (gentity_t *adm, qboolean autoeven)
{
	int		counts[TEAM_NUM_TEAMS];
	int		diff = 0;
	int		highTeam, i, j, lastConnectedTime;
	int		adminChecked = 1; // Start with 1 (gay refs)
	///gentity_t *lastConnected, *ent;
	clientSession_t	*sess;
	///qboolean canBeMoved = qfalse;

	// GodOrDevil - 10.23.05 - 01:45am
	if(level.intermissiontime)
		return;		
	
	if (level.time - level.startTime >= (g_timelimit.integer + level.timeExtension)*60000 )
		return;
	///End  - 10.23.05 - 01:45am

	if(!level.gametypeData->teams && !autoeven){
		if(adm && adm->client)
			trap_SendServerCommand( adm - g_entities, va("print \"Not playing a team game.\n\"") );
		else
			Com_Printf("Not playing a team game.\n");
		return;
	}

	if(level.extras.blueLocked || level.extras.redLocked){
		if(autoeven)
			return;
		if(adm && adm->client)
			trap_SendServerCommand( adm - g_entities, va("print \"Teams are Locked.\n\"") );
		else
			Com_Printf("Teams are Locked.\n");
		return;

	}
	/// get the number of players on each team
	counts[TEAM_BLUE] = TeamCount(-1, TEAM_BLUE, NULL );
	counts[TEAM_RED] = TeamCount(-1, TEAM_RED, NULL );

	/// find the team with more players and
	/// calculate the number of players that need to be moved
	if(counts[TEAM_BLUE] > counts[TEAM_RED]){
		highTeam = TEAM_BLUE;
		diff = (counts[TEAM_BLUE] - counts[TEAM_RED]);
	}
	else if(counts[TEAM_BLUE] < counts[TEAM_RED]){
		highTeam = TEAM_RED;
		diff = (counts[TEAM_RED] - counts[TEAM_BLUE]);
	}
	else {
		if(autoeven)
			return;
		if(adm && adm->client)
			trap_SendServerCommand( adm - g_entities, va("print \"Teams are as even as possible.\n\"") );
		else 
			Com_Printf("Teams are as even as possible.\n");
		return;
	}
	/// if less than 2 players difference, you cant make it any more even
	if(diff < 2){
		if(autoeven)
			return;
		if(adm && adm->client)
			trap_SendServerCommand( adm - g_entities, va("print \"Teams are as even as possible.\n\"") );
		else 
			Com_Printf("Teams are as even as possible.\n");
		return;
	}

	/// difference divided by 2 is the number of players we will move
	diff /= 2;

	for(i = 0; i < diff; i++)
	{
		///qboolean canBeMoved = qfalse;
		gentity_t	*lastConnected = NULL, *ent = NULL;
		lastConnectedTime = 0;
		/// find the last client to join the high team and move them to the other
		for ( j = 0; j < level.numConnectedClients ; j++ )	
		{
			ent = &g_entities[level.sortedClients[j]];
			sess = &ent->client->sess;

			if (!ent->client)
				continue;
			if (ent->client->pers.connected != CON_CONNECTED 
				&& ent->client->pers.connected != CON_CONNECTING )
				continue;
			if(sess->team != TEAM_RED && sess->team != TEAM_BLUE)
				continue;
			if(sess->team != highTeam)
				continue;
			// GodOrDevil - We dont use Team Captains.. so i changed it to #admin
			///if(ent->client->sess.teamCaptain)
			if(ent->client->nvSess.admin >= adminChecked)
				continue;
			if(sess->torture)
				continue;
			/// Don't force them to change if they have a gt item
			if(ent->s.gametypeitems)
				continue;
			///End

			/// are they the last to connect?
			if(ent->client->pers.enterTime > lastConnectedTime)	
			{
				lastConnectedTime = ent->client->pers.enterTime;
				lastConnected = ent;
			}		
		}

		///if(!canBeMoved || !highTeam){
		if(!lastConnected || !lastConnected->client || !highTeam)
		{
			if(adminChecked <= 4) // No one moved, so check next admin level
			{
				adminChecked++;
				i--;
				continue;
			}
			else
			{
				if(autoeven)
					return;
				if(adm && adm->client)
					trap_SendServerCommand( adm-g_entities, va("print \"Teams cannot be evened [all admin or item holders].\n\"") );
				else 
					Com_Printf("Teams cannot be evened [all admin or item holders]\n");
				return;
			}
		}

		// GodOrDevil - 01.24.06 - 07:42pm - remove their weapons
		///and set them as a ghost
		lastConnected->client->ps.stats[STAT_WEAPONS] = 0;
		TossClientItems( lastConnected );
		G_StartGhosting( lastConnected );
		///End  - 01.24.06 - 07:43pm

		/// move them
		if(highTeam == TEAM_RED) lastConnected->client->sess.team = TEAM_BLUE;
		else lastConnected->client->sess.team = TEAM_RED;

		if (lastConnected->r.svFlags & SVF_BOT)	{
			char	userinfo[MAX_INFO_STRING];
			trap_GetUserinfo( lastConnected->s.number, userinfo, sizeof( userinfo ) );
			Info_SetValueForKey( userinfo, "team", lastConnected->client->sess.team == TEAM_RED?"red":"blue");
			trap_SetUserinfo( lastConnected->s.number, userinfo );
		}

		///01.24.06 - 07:43pm
		lastConnected->client->pers.identity = NULL;
		ClientUserinfoChanged( lastConnected->s.number );		
		CalculateRanks();

		G_StopFollowing( lastConnected );
		G_StopGhosting( lastConnected );
		trap_UnlinkEntity ( lastConnected );
		ClientSpawn( lastConnected);
		///End  - 01.24.06 - 07:43pm
	}

	/// tell them what happened
	trap_SendServerCommand( -1, va("cp \"^-** ^3Evening Teams ^-**\n\"") );
	RPM_GlobalSound(G_SoundIndex("sound/misc/events/tut_lift02.mp3"));
			
	if(autoeven)
		return;
	// GodOrDevil - 02.26.05 - 02:59am #adminLog
	if(adm && adm->client) {
		SC_adminLog (va("%s (%s) - EvenTeams", adm->client->nvSess.cleanName, adm->client->nvSess.ip)) ;
		trap_SendServerCommand( -1, va("print \"^-Admin^7: Even Teams - ^3%s\n\"", adm->client->pers.netname));
	} else	{
		///Not going to log it.. takes up to much space in the logs
		SC_adminLog (va("%s - EvenTeams", "RCON")) ;
		trap_SendServerCommand( -1, va("print \"^3Teams have been evened\n\"" ));
	}
	NV_SQLAdminLog(adm, NULL, "Eventeams");	
	///End  - 02.26.05 - 02:59am
}


//void RPM_Even_Teams (gentity_t *adm, qboolean autoeven)
//{
//	int		counts[TEAM_NUM_TEAMS];
//	int		diff = 0;
//	int		highTeam, i, j, lastConnectedTime;
//	///gentity_t *lastConnected, *ent;
//	clientSession_t	*sess;
//	///qboolean canBeMoved = qfalse;
//
//	// GodOrDevil - 10.23.05 - 01:45am
//	if(level.intermissiontime)
//		return;
//	///End  - 10.23.05 - 01:45am
//
//	if(!level.gametypeData->teams && !autoeven){
//		if(adm && adm->client)
//			trap_SendServerCommand( adm - g_entities, va("print \"Not playing a team game.\n\"") );
//		else
//			Com_Printf("Not playing a team game.\n");
//		return;
//	}
//
//	if(level.extras.blueLocked || level.extras.redLocked){
//		if(autoeven)
//			return;
//		if(adm && adm->client)
//			trap_SendServerCommand( adm - g_entities, va("print \"Teams are Locked.\n\"") );
//		else
//			Com_Printf("Teams are Locked.\n");
//		return;
//
//	}
//	/// get the number of players on each team
//	counts[TEAM_BLUE] = TeamCount(-1, TEAM_BLUE, NULL );
//	counts[TEAM_RED] = TeamCount(-1, TEAM_RED, NULL );
//
//	/// find the team with more players and
//	/// calculate the number of players that need to be moved
//	if(counts[TEAM_BLUE] > counts[TEAM_RED]){
//		highTeam = TEAM_BLUE;
//		diff = (counts[TEAM_BLUE] - counts[TEAM_RED]);
//	}
//	else if(counts[TEAM_BLUE] < counts[TEAM_RED]){
//		highTeam = TEAM_RED;
//		diff = (counts[TEAM_RED] - counts[TEAM_BLUE]);
//	}
//	else {
//		if(autoeven)
//			return;
//		if(adm && adm->client)
//			trap_SendServerCommand( adm - g_entities, va("print \"Teams are as even as possible.\n\"") );
//		else 
//			Com_Printf("Teams are as even as possible.\n");
//		return;
//	}
//	/// if less than 2 players difference, you cant make it any more even
//	if(diff < 2){
//		if(autoeven)
//			return;
//		if(adm && adm->client)
//			trap_SendServerCommand( adm - g_entities, va("print \"Teams are as even as possible.\n\"") );
//		else 
//			Com_Printf("Teams are as even as possible.\n");
//		return;
//	}
//
//	/// difference divided by 2 is the number of players we will move
//	diff /= 2;
//
//	for(i = 0; i < diff; i++){
//		///qboolean canBeMoved = qfalse;
//		gentity_t	*lastConnected = NULL, *ent = NULL;
//		lastConnectedTime = 0;
//		/// find the last client to join the high team and move them to the other
//		for ( j = 0; j < level.numConnectedClients ; j++ )	{
//			ent = &g_entities[level.sortedClients[j]];
//			sess = &ent->client->sess;
//
//			if (!ent->client)
//				continue;
//			if (ent->client->pers.connected != CON_CONNECTED 
//				&& ent->client->pers.connected != CON_CONNECTING )
//				continue;
//			if(sess->team != TEAM_RED && sess->team != TEAM_BLUE)
//				continue;
//			if(sess->team != highTeam)
//				continue;
//			// GodOrDevil - We dont use Team Captains.. so i changed it to #admin
//			///if(ent->client->sess.teamCaptain)
//			if(nvSess->admin)
//				continue;
//			if(sess->torture)
//				continue;
//			/// Don't force them to change if they have a gt item
//			if(ent->s.gametypeitems)
//				continue;
//			///End
//
//			/// are they the last to connect?
//			if(ent->client->pers.enterTime > lastConnectedTime)	{
//				lastConnectedTime = ent->client->pers.enterTime;
//				lastConnected = ent;
//			}
// 			///canBeMoved = qtrue;
//			///canBeMoved++;
//		}
//
//		///if(!canBeMoved || !highTeam){
//		if(!lastConnected || !lastConnected->client || !highTeam){
//			if(autoeven)
//				return;
//			if(adm && adm->client)
//				trap_SendServerCommand( adm-g_entities, va("print \"Teams cannot be evened [all admin or item holders].\n\"") );
//			else 
//				Com_Printf("Teams cannot be evened [all admin or item holders]\n");
//			return;
//		}
//
//		// GodOrDevil - 01.24.06 - 07:42pm - remove their weapons
//		///and set them as a ghost
//		lastConnected->client->ps.stats[STAT_WEAPONS] = 0;
//		TossClientItems( lastConnected );
//		G_StartGhosting( lastConnected );
//		///End  - 01.24.06 - 07:43pm
//
//		/// move them
//		if(highTeam == TEAM_RED) lastConnected->client->sess.team = TEAM_BLUE;
//		else lastConnected->client->sess.team = TEAM_RED;
//
//		if (lastConnected->r.svFlags & SVF_BOT)	{
//			char	userinfo[MAX_INFO_STRING];
//			trap_GetUserinfo( lastConnected->s.number, userinfo, sizeof( userinfo ) );
//			Info_SetValueForKey( userinfo, "team", lastConnected->client->sess.team == TEAM_RED?"red":"blue");
//			trap_SetUserinfo( lastConnected->s.number, userinfo );
//		}
//
//		///01.24.06 - 07:43pm
//		lastConnected->client->pers.identity = NULL;
//		ClientUserinfoChanged( lastConnected->s.number );		
//		CalculateRanks();
//
//		G_StopFollowing( lastConnected );
//		G_StopGhosting( lastConnected );
//		trap_UnlinkEntity ( lastConnected );
//		ClientSpawn( lastConnected);
//		///End  - 01.24.06 - 07:43pm
//	}
//
//	/// tell them what happened
//	trap_SendServerCommand( -1, va("cp \"^-** ^3Evening Teams ^-**\n\"") );
//	RPM_GlobalSound(G_SoundIndex("sound/misc/events/tut_lift02.mp3"));
//			
//	if(autoeven)
//		return;
//	// GodOrDevil - 02.26.05 - 02:59am #adminLog
//	if(adm && adm->client) {
//		SC_adminLog (va("%s - EvenTeams", adm->client->nvSess.cleanName )) ;
//		trap_SendServerCommand( -1, va("print \"^-Admin^7: Even Teams - ^3%s\n\"", adm->client->pers.netname));
//	} else	{
//		///Not going to log it.. takes up to much space in the logs
//		SC_adminLog (va("%s - EvenTeams", "RCON")) ;
//		trap_SendServerCommand( -1, va("print \"^3Teams have been evened\n\"" ));
//	}
//	///End  - 02.26.05 - 02:59am
//}


void SC_SwapTeams(gentity_t *adm)
{
	int		i;
	// GodOrDevil - 04.22.05 - 02:56am - #swapScore
	int		rs = level.teamScores[TEAM_RED];
	int		bs = level.teamScores[TEAM_BLUE];
	int		rl = level.extras.redLocked;
	int		bl = level.extras.blueLocked;
	int		rrw = level.extras.roundWinsRed;
	char	score[2];
	///End  - 04.22.05 - 02:57am

	clientSession_t	*sess;
	gentity_t *ent, *find;

	if(!level.gametypeData->teams) {
		if(adm && adm->client) {
			trap_SendServerCommand( adm - g_entities, va("print \"Not playing a team game.\n\"") );
			} else {
			Com_Printf("Not playing a team game.\n");}
		return;
	}

	/// used to swap scores (or not)
	if(!adm || adm == NULL)
		score[0] = 0;
	else if(adm && adm->client) trap_Argv( 2, score, sizeof( score ) );
	else trap_Argv( 1, score, sizeof( score ) );

	for(i = 0; i < level.numConnectedClients; i++){
		ent = &g_entities[level.sortedClients[i]];
		sess = &ent->client->sess;
		
		///Do the team changing
		if (ent->client->sess.team == TEAM_SPECTATOR)	    continue;
		if (ent->client->pers.connected != CON_CONNECTED )	continue;


		/// drop any gt items they might have
		if(ent->s.gametypeitems > 0){
				G_DropGametypeItems ( ent, 0 );
		}

		///01.24.06e - 07:42pm - remove their weapons
		///and set them as a ghost
		ent->client->ps.stats[STAT_WEAPONS] = 0;
		TossClientItems( ent );
		G_StartGhosting( ent );
		///End  - 01.24.06 - 07:43pm

		if (ent->client->sess.team == TEAM_RED) 	   ent->client->sess.team = TEAM_BLUE;
		else if(ent->client->sess.team == TEAM_BLUE)   ent->client->sess.team = TEAM_RED;

		///Take care of the bots
		if (ent->r.svFlags & SVF_BOT){
			char	userinfo[MAX_INFO_STRING];
			trap_GetUserinfo( ent->s.number, userinfo, sizeof( userinfo ) );
			Info_SetValueForKey( userinfo, "team", sess->team == TEAM_RED?"red":"blue");
			trap_SetUserinfo( ent->s.number, userinfo );
		}
		
		///Prepare the clients for team change then repawn
		///01.24.06 - 07:43pm
		ent->client->pers.identity = NULL;
		ClientUserinfoChanged( ent->s.number );		
		CalculateRanks();

		G_StopFollowing( ent );
		G_StopGhosting( ent );
		trap_UnlinkEntity ( ent );
		ClientSpawn( ent);

		///ent->client->pers.identity = NULL;
		///ClientUserinfoChanged( ent->s.number );
		///ent->client->sess.ghost = qfalse;
		///ClientBegin( ent->s.number, qfalse );
		///End  - 01.24.06 - 07:45pm
	}

	///Reset #Gametype Item
	find = NULL;
	while ( NULL != (find = G_Find ( find, FOFS(classname), "gametype_item" ) ) ){
		G_ResetGametypeItem ( find->item );	
	}
	
	///04.22.05 - 02:44am - swap scores & locks
	if (!score[0] || g_compMode.integer){
		level.teamScores[TEAM_BLUE] = rs;
		level.teamScores[TEAM_RED] = bs;
		level.extras.roundWinsRed = level.extras.roundWinsBlue;
		level.extras.roundWinsBlue = rrw;
	}
	level.extras.redLocked = bl;
	level.extras.blueLocked = rl;
	///End  - 04.22.05 - 02:45am

	///Enable roundtime for Gametypes w/out respawn intervals	
	if (level.gametypeData->respawnType != RT_INTERVAL ){
		level.gametypeDelayTime = level.time + g_roundstartdelay.integer * 1000;
		level.gametypeRoundTime = level.time + (g_roundtimelimit.integer * 60000);
		if ( level.gametypeDelayTime != level.time ){
			trap_SetConfigstring ( CS_GAMETYPE_TIMER, va("%i", level.gametypeRoundTime) );
		}
	}
	///Tell Everyone what happend
	if(g_compMode.integer)
	{
		trap_SendServerCommand(-1, va("chat -1 \"^7SERVER: ^3SWAPPING SIDES!\n\""));
	}

	trap_SendServerCommand( -1, va("cp \"^_**^7Admin Action^_**\n^3Swap Teams\n\"") );
	RPM_GlobalSound(G_SoundIndex("sound/misc/events/tut_lift02.mp3"));
	
	// GodOrDevil - 02.26.05 - 02:59am #adminLog	
	if(adm && adm->client) {
		trap_SendServerCommand( -1, va("print \"^3Swap Teams ^7- ^3%s\n\"", adm->client->pers.netname));
		SC_adminLog (va("%s (%s) - SwapTeams", adm->client->nvSess.cleanName, adm->client->nvSess.ip )) ;
	} else	{
		trap_SendServerCommand( -1, va("print \"^3Swap Teams ^7- ^6Admin\n\""));
		SC_adminLog (va("%s - SwapTeams", "RCON" )) ;
	}
	///End  - 02.26.05 - 02:59am
	NV_SQLAdminLog(adm, NULL, "SwapTeams");	

	///CalculateRanks();
}
#ifdef _CLANMEMBER
void RPM_Clan_Vs_All(gentity_t *adm)
{
	int		counts[TEAM_NUM_TEAMS] = {0};
	int		i, clanTeam, othersTeam;
	char	team[6];	

	clientSession_t	*sess;
	gentity_t	*ent;

	if(!level.gametypeData->teams){
		if(adm && adm->client){
			trap_SendServerCommand( adm - g_entities, va("print \"Not playing a team game.\n\"") );
		}
		else {
			Com_Printf("Not playing a team game.\n");
		}
		return;
	}
	if(adm && adm->client)	trap_Argv( 2, team, sizeof( team ) );
	else trap_Argv( 1, team, sizeof( team ) );
	
	/// which team has the most clan members on it?
	for (i = 0; i < level.numConnectedClients; i++) {
		sess = &g_entities[level.sortedClients[i]].client->sess;
		if(!sess->clanMember) continue;
		if(sess->team != TEAM_RED && sess->team != TEAM_BLUE) continue;
		counts[sess->team]++;
	}
	// GodOrDevil - 05.03.05 - 12:58am - #pickClanteam
	if (team[0] == 'r' || team[0] == 'R'){
		clanTeam = TEAM_RED;
		othersTeam = TEAM_BLUE;
	}
	else if (team[0] == 'b' || team[0] == 'B'){
		clanTeam = TEAM_BLUE;
		othersTeam = TEAM_RED;
	}
	else {
		/// set which team the clan will be on
		if(counts[TEAM_RED] >= counts[TEAM_BLUE]){
			clanTeam = TEAM_RED;
			othersTeam = TEAM_BLUE;
		}
		else {
			clanTeam = TEAM_BLUE;
			othersTeam = TEAM_RED;
		}
		//End  - 05.03.05 - 12:58am
	}

	for(i = 0; i < level.numConnectedClients; i++){
		ent = &g_entities[level.sortedClients[i]];
		sess = &ent->client->sess;

		if(sess->team != TEAM_RED && sess->team != TEAM_BLUE) continue;

		/// clan member not on the clan team?
		if(sess->clanMember){
			if(sess->team != clanTeam){
				///01.24.06e - 07:42pm - remove their weapons
				///and set them as a ghost
				
				/// drop any gt items they might have
				///if(ent->s.gametypeitems > 0){
				///	G_DropGametypeItems ( ent, 0 );
				///}

				ent->client->ps.stats[STAT_WEAPONS] = 0;
				TossClientItems( ent );
				G_StartGhosting( ent );
				///End  - 01.24.06 - 07:43pm
				sess->team = clanTeam;

			}
			else {
				continue;
			}
		}
		/// non clan member thats on the clan team?
		else {
			if(sess->team != othersTeam) {
				
				/// drop any gt items they might have
				///if(ent->s.gametypeitems > 0){
				///	G_DropGametypeItems ( ent, 0 );
				///}

				///01.24.06e - 07:42pm - remove their weapons
				///and set them as a ghost
				ent->client->ps.stats[STAT_WEAPONS] = 0;
				TossClientItems( ent );
				G_StartGhosting( ent );
				///End  - 01.24.06 - 07:43pm

				sess->team = othersTeam;

			}
			else continue;
		}
		
		if (ent->r.svFlags & SVF_BOT){
			char	userinfo[MAX_INFO_STRING];
			trap_GetUserinfo( ent->s.number, userinfo, sizeof( userinfo ) );
			Info_SetValueForKey( userinfo, "team", sess->team == TEAM_RED?"red":"blue");
			trap_SetUserinfo( ent->s.number, userinfo );
		}		
		/// start them at their new base without delay
		///Prepare the clients for team change then repawn
		///01.24.06 - 07:43pm
		ent->client->pers.identity = NULL;
		ClientUserinfoChanged( ent->s.number );		
		CalculateRanks();

		G_StopFollowing( ent );
		G_StopGhosting( ent );
		trap_UnlinkEntity ( ent );
		ClientSpawn( ent);

		///ent->client->pers.identity = NULL;
		///ClientUserinfoChanged( ent->s.number );
		/// ///ent->client->sess.ghost = qfalse;
		///ClientBegin( ent->s.number, qfalse );
	}
	// GodOrDevil - 02.28.05 - 10:24pm #lockClanTean
	///Lock the clanTeam when switch is complete
	if (clanTeam == TEAM_BLUE){	
		level.extras.blueLocked = 1;
		level.extras.redLocked = 0;
		}
	else {
		level.extras.redLocked = 1;
		level.extras.blueLocked = 0;
	}

	/// tell everyone what just happened
	trap_SendServerCommand( -1, va("cp \"^_**^7Admin Action^_**\n^1CLAN ^7vs ^1ALL\n^4Clan Team ^7has been ^3LOCKED\n\"") );
	RPM_GlobalSound(G_SoundIndex("sound/misc/events/tut_lift02.mp3"));
			
	// GodOrDevil - 02.26.05 - 02:59am #adminLog	
	if(adm && adm->client) {
		SC_adminLog (va("%s (%s) - ClanVsAll", adm->client->nvSess.cleanName, adm->client->nvSess.ip )) ;
		trap_SendServerCommand( -1, va("print \"^3Clan Vs All ^7- ^3%s\n\"",  adm->client->pers.netname));
	} else	{
		SC_adminLog (va("%s - ClanVsAll","RCON" )) ;
		trap_SendServerCommand( -1, va("print \"^3Clan Vs All ^7- ^6Admin\n\""));
	}
	///End  - 02.26.05 - 02:59am
	///CalculateRanks();
}
#endif
void RPM_Penalty(int argNum, gentity_t *adm)
{
	gentity_t *ent;
	int		idnum/*,i*/;
	int		seconds; 
	char	time[4];
	char	reason[40];

	idnum = RPM_ClientNumFromArg(adm, argNum, "Usage: penalty <idnumber> <time> <reason>", "Penalty", qfalse);
	if(idnum < 0){return;}

	ent = g_entities + idnum;

	/// Keeps Player from setting a pbox time to a lower value, so they dont cheat.
	if(ent->client->sess.penaltySeconds){
		if(adm && adm->client)	{
			trap_SendServerCommand( adm-g_entities, va("print \"^3%s is already in the Penalty Box\n\"", ent->client->pers.netname));
		} else {
			Com_Printf("^3%s is already in the Penalty Box\n", ent->client->pers.netname);
		}
		return;
	}

	/// make them drop any gametype items they may have
	G_DropGametypeItems ( ent, 0 );

	/// make them dead so it shows that they are not playing on the scoreboard etc...
	ent->s.eFlags |= EF_DEAD;
	ent->client->ps.pm_type = PM_DEAD;

	/// mute them so they can't whine about it and spam the server
	ent->client->sess.mute = qtrue;

	/// make them not ghosting so calling G_startGhosting will actually do something
	ent->client->sess.ghost = qfalse;
	G_StartGhosting(ent);

	/// Make sure the don't leave a body or anything 
	G_FreeEnitityChildren ( ent );

	/// find the intermission point
	FindIntermissionPoint ( );

	/// and move there
	VectorCopy( level.intermission_origin,	ent->s.origin );
	VectorCopy( level.intermission_origin,	ent->r.currentOrigin );
	VectorCopy( level.intermission_origin,	ent->client->ps.pvsOrigin );
	VectorCopy( level.intermission_origin,	ent->client->ps.origin );
	VectorCopy(	level.intermission_angle,	ent->client->ps.viewangles);
	
	/// get the specified penalty time
	trap_Argv( argNum + 1, time, sizeof( time ) );
	seconds = atoi(time);

	/// get the reason for penalty
	trap_Argv( argNum + 2, reason, sizeof( reason ) );

	/// save them for use later
	///ent->client->sess.penaltyBoxTime = level.time;
	ent->client->sess.penaltySeconds = seconds?seconds:g_defaultPenaltyBoxTime.integer;
	Q_strncpyz(ent->client->sess.penaltyReason, *reason?reason:"Bad Behavior", sizeof(ent->client->sess.penaltyReason));

	/// Play a sound for them
	RPM_ClientSound(ent, G_SoundIndex("sound/movers/doors/jail_door/jail_doorstop.mp3"));

	/// Tell all the players about it
	trap_SendServerCommand( -1, va("print \"^3%s has been thrown into the Penalty Box.\n\"", ent->client->pers.netname));		
	
	if(ent->client->pers.camped){
		ent->client->pers.camped = qfalse;
		return;
	}
		
	if (ent->client->nvSess.torture)
		return;

	// GodOrDevil - 02.26.05 - 02:59am #adminLog	
	if(adm && adm->client)
		SC_adminLog (va("%s (%s) - PBOX: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip  )) ;
	else
		SC_adminLog (va("%s - PBOX(Auto?): %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip  )) ;
	///End  - 02.26.05 - 02:59am
	NV_SQLAdminLog(adm, ent, "PBOX");	
}
void SC_Forcesay(int argNum, gentity_t *adm)
{
	gentity_t *ent;
	int	  idnum, j;
	char  *text;

	idnum = RPM_ClientNumFromArg(adm, argNum, "Usage: ForceSay <idnumber> <message>", "Forcesay", qfalse);
	if(idnum < 0){return;}

	ent = g_entities + idnum;

	///Clients Warning Level has to be atleast 3 to use this unless they are being tortured!
	///if ((ent->client->sess.warnCount < 3 && g_needWarn.integer) /*&& !ent->client->nvSess.tortureSeconds*/){
	///	trap_SendServerCommand( adm-g_entities, va("print \"Clients ^1Warning Level^7 must at ^33^7 to be subjected to this punishment!\n^4%s ^7current ^1Warning Level^7: ^3%i\n\"", ent->client->pers.netname, ent->client->sess.warnCount));
	///	Com_Printf("Clients ^1Warning Level^7 must at ^33^7 to be subjected to this punishment!\n^4%s ^7current ^1Warning Level^7: ^3%i\n\"", ent->client->pers.netname, ent->client->sess.warnCount);
	///	return;
	///}

	text = ConcatArgs( argNum + 1 );
	G_Say( g_entities + idnum, NULL, SAY_ALL, text );
	// GodOrDevil - 03.03.05 - 12:32am - Dont show message if being #Tortured
	///if(ent->client->nvSess.torture)
	///	return;

	///if(ent->client->pers.camped){
	///	ent->client->pers.camped = qfalse;
	///	return;
	///}
	for (j = 0; j < level.numConnectedClients; j++)	{
		if (level.clients[level.sortedClients[j]].nvSess.admin >= 2){
			if(adm && adm->client)
				trap_SendServerCommand(level.sortedClients[j], va("print \"^1%s^7 has used ^3forcesay ^7on ^1%s\n\"", adm->client->pers.netname, g_entities[idnum].client->pers.netname));
			else
				trap_SendServerCommand(level.sortedClients[j], va("print \"^1%s^7 has used ^3forcesay ^7on ^1%s\n\"", "^6ADMIN", g_entities[idnum].client->pers.netname));
		}
	}
	if(adm && adm->client)
		SC_adminLog (va("%s (%s) - FORCESAY: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip   )) ;
	else
		SC_adminLog (va("%s - FORCESAY: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip  )) ;
	NV_SQLAdminLog(adm, ent, "FORCESAY");	
}
void SC_Slam (int argNum, gentity_t *adm)
{
	gentity_t		*ent;
	int				idnum;

	idnum = RPM_ClientNumFromArg(adm, argNum, "Slam <idnumber>", "Slam", qtrue);
	if(idnum < 0) return;
	
	ent = g_entities + idnum;
	
	///Do the Jumping
	///ent->client->ps.pm_debounce |= PMD_JUMP;
	ent->client->ps.pm_flags |= PMF_JUMPING;

	ent->client->ps.groundEntityNum = ENTITYNUM_NONE;
	ent->client->ps.velocity[2] = 2000;

	ent->client->nvSess.drop = qtrue;

	RPM_ClientSound(ent, G_SoundIndex("sound/ambience/vehicles/telephone_pole.mp3"));

	if(ent->client->nvSess.torture)
		return;

	if(ent->client->pers.camped){
		ent->client->pers.camped = qfalse;
		return;
	}
	
	if(adm && adm->client){
		SC_adminLog (va("%s (%s) - SLAM: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip   )) ;
		trap_SendServerCommand( -1, va("print \"%s was slammed to the ground by ^3%s.\n\"", ent->client->pers.netname, adm->client->pers.netname));
	}
	else
	{
		SC_adminLog (va("%s - SLAM: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName,  g_entities[idnum].client->nvSess.ip   )) ;
		trap_SendServerCommand( -1, va("print \"%s was slammed to the ground by an ^6ADMIN.\n\"", ent->client->pers.netname));
	}
	NV_SQLAdminLog(adm, ent, "SLAM");	
}
void SC_Twist (int argNum, gentity_t *adm)
{
	gentity_t		*ent;
	int				idnum, i;
	vec3_t			lookdown;
	//char			*a, *b, *c;
	char			a[4], b[4], c[4];


	idnum = RPM_ClientNumFromArg(adm, argNum, "twist <idnumber>", "Twist", qtrue);
	if(idnum < 0)  return;

	ent = g_entities + idnum;

	trap_Argv( argNum + 1, c, sizeof(c) );
	trap_Argv( argNum + 2, b, sizeof(b) );
	trap_Argv( argNum + 3, a, sizeof(a) );

	////////Clients Warning Level has to be atleast 3 to use this unless they are being tortured!
	////if (ent->client->sess.warnCount < 3 && !ent->client->nvSess.tortureSeconds){
	////	trap_SendServerCommand( adm-g_entities, va("print \"Clients ^1Warning Level^7 must at ^33^7 to be subjected to this punishment!\n^4%s ^7current ^1Warning Level^7: ^3%i\n""", ent->client->pers.netname, ent->client->sess.warnCount));
	////	Com_Printf("Clients ^1Warning Level^7 must at ^33^7 to be subjected to this punishment!\n^4%s ^7current ^1Warning Level^7: ^3%i\n""", ent->client->pers.netname, ent->client->sess.warnCount);
	////	return;
	////}

	////"c" is the first arg. If its not defined, use the default setting
	if ( !c[0] ) {
	VectorSet(lookdown, 100, 0, 130);
	}
	else {
	VectorSet(lookdown, atoi(a), atoi(b), atoi(c));
	}

	SetClientViewAngle(ent, lookdown);

	if(ent->client->nvSess.torture){
		RPM_ClientSound(ent, G_SoundIndex("sound/player/steps/mud/mud1.wav"));
		return;
	}
	else
		RPM_GlobalSound(G_SoundIndex("sound/player/steps/mud/mud1.wav"));
	
	if(ent->client->pers.camped){
		ent->client->pers.camped = qfalse;
		return;
	}
	
	for (i = 0; i < level.numConnectedClients; i++)	{
		if (level.clients[level.sortedClients[i]].nvSess.admin){	
			if(adm != NULL)
				//CPx(level.sortedClients[i], va("print \"^7%s^7: ^3[%s] %s\n\"", adm->client->pers.netname, action, target));
				trap_SendServerCommand(level.sortedClients[i], va("print \"^7%s^7: ^3[Twisted] ^7%s\n\"", adm->client->pers.netname, g_entities[idnum].client->pers.netname));
			else 
				trap_SendServerCommand(level.sortedClients[i], va("print \"^7ADMIN^7: ^3[Twisted] ^7%s\n\"", g_entities[idnum].client->pers.netname));
		}
	}

	if (ent->client->nvSess.torture)
		return;

	if(adm && adm->client)
		SC_adminLog (va("%s (%s) - TWIST: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip   )) ;
	else
		SC_adminLog (va("%s - TWIST: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip  )) ;
	NV_SQLAdminLog(adm, ent, "TWIST");	
}
void SC_unTwist (int argNum, gentity_t *adm)
{
	gentity_t	*ent;
	int			idnum;
	vec3_t		lookdown;

	idnum = RPM_ClientNumFromArg(adm, argNum, "untwist <idnumber>", "unTwist", qtrue);
	if(idnum < 0) {	return;	}

	ent = g_entities + idnum;

	VectorSet(lookdown, 0, 0, 0);	
	SetClientViewAngle(ent, lookdown);
	
	if (ent->client->nvSess.torture)
		return;

	if(adm && adm->client)
		SC_adminLog (va("%s (%s) - UNTWIST: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip   )) ;
	else 
		SC_adminLog (va("%s - UNTWIST: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip  )) ;
	NV_SQLAdminLog(adm, ent, "UNTWIST");

}
/////#NOTE - MOVED SO ALL PLAYERS CAN USE
/////* 
////================
////SC_Sound
////Allows any sound in the game to be played
/// GodOrDevil - 03.26.05 - 03:28pm #sound
////================
////*/
////void SC_Sound ( gentity_t *adm)
////{
////	char		sound[100];
////	char		p[100];
////
////	trap_Argv( 2, sound, sizeof( sound ) );
////	trap_Argv( 3, p, sizeof( p ) );
////
////	if ( !sound[0] ) {
////		trap_SendServerCommand( adm-g_entities, va("print \"Usage: play <dir/soundfile> <chat text> | i.e. play sound/misc/confused/moose MOOO!!\n\""));	
////		return;
////	}
////	if (p[0]) Cmd_Say_f (adm, 9, qfalse);
////
//// 	RPM_GlobalSound(G_SoundIndex(sound));
////}
////
////
/////* 
////================
////SC_TeamSound
////Allows any sound in the game to be played TeamOnly
/// GodOrDevil - 03.27.05 - 06:02pm #teamSound
////================
////*/
////void SC_TeamSound ( gentity_t *adm)
////{
////	char		sound[100];
////	char		p[100];
////	int			j;
////
////	trap_Argv( 2, sound, sizeof( sound ) );
////	trap_Argv( 3, p, sizeof( p ) );
////	
////	if ( !sound[0] ) {
////		trap_SendServerCommand( adm-g_entities, va("print \"Usage: playTeam <dir/soundfile> <chat text> | i.e. play sound/misc/confused/moose MOOO!!\n\""));	
////		return;
////	}
////	if (p[0])	Cmd_Say_f (adm, PLAY_CHAT, qfalse);
////
////	for (j = 0; j < level.numConnectedClients; j++)	{
////		if (level.clients[level.sortedClients[j]].sess.team == adm->client->sess.team ) {
////			RPM_ClientSound(&g_entities[level.sortedClients[j]], G_SoundIndex(sound));
////		}
////	}
////}
////

void SC_nextMap ( gentity_t	*adm )
{
	char		map[32]="\0", gt[6] = "\0", sl[6] = "\0", b[6] = "\0";
	int			a;
	
	trap_Argv( 2, map, sizeof( map ));
	trap_Argv( 3, gt, sizeof( gt ) );
	trap_Argv( 4, sl, sizeof( sl ) );
	
	///So if nextMap was used before it will tell the nextMap scorelimit and gt.
	if (!g_nextScore.integer)
		a = g_scorelimit.integer;
	else 
		a = g_nextScore.integer;

	strcpy(b, g_gametype.string);

	if ( !map[0] ){
		trap_SendServerCommand( adm-g_entities, "print \"^3Usage^7: nextmap <map name> <gametype> <scorelimit>\n\"" );
		return;
	}

	///does map exist on server?
	if ( !G_DoesMapExist ( map ) ){
		trap_SendServerCommand( adm-g_entities, "print \"This server does not have that map!\n\"" );
		return;
	}

	///Sets the value of the Cvar(s).
	trap_Cvar_Set("g_setnextMap", map);
	trap_Cvar_Update( &g_setnextMap );

	if ( gt[0]) {
		///level.modeChangeSec = 1;
		trap_SendConsoleCommand( EXEC_APPEND, va("g_gametype \"%s\"\n", gt) );
		strcpy(b, gt);

		if ( sl[0] ) {
			trap_Cvar_Set("g_nextScore", sl);
			a = atoi(sl);;
		}
	}

	///trap_SendServerCommand( -1, va("print \"^1Next Map^7: %s ^3| ^1Gametype^7: %s ^3| ^1Scorelimit^7: %i\n\"", g_setnextMap.string, b, a ));
	trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,^1Next Map^7: %s ^3| ^1Gametype^7: %s ^3| ^1Scorelimit^7: %i", level.time + 5000, g_setnextMap.string, b, a));
	RPM_GlobalSound(G_SoundIndex("sound/misc/events/tut_door01.mp3"));

	if(adm && adm->client)
		SC_adminLog (va("%s (%s) - Next", adm->client->nvSess.cleanName, adm->client->nvSess.ip )) ;
	else
		SC_adminLog (va("%s - Next", "RCON" )) ;
	NV_SQLAdminLog(adm, NULL, va("Next: [%s]%s", b, g_setnextMap.string));	
}
void SC_Slow (int argNum, gentity_t *adm)
{
	gentity_t		*ent;
	int				idnum;
	char			speed[5];
	int				slow;

	idnum = RPM_ClientNumFromArg(adm, argNum, "slow <id> <speed>", "Slow", qtrue);
	if(idnum < 0) {	return;	}

	ent = g_entities + idnum;

	///Clients Warning Level has to be atleast 3 to use this unless they are being tortured!
	if (( ent->client->sess.warnCount < 3 && g_needWarn.integer) /*&& !ent->client->nvSess.tortureSeconds*/){
		trap_SendServerCommand( adm-g_entities, va("print \"Clients ^1Warning Level^7 must at ^33^7 to be subjected to this punishment!\n^4%s ^7current ^1Warning Level^7: ^3%i\n\"", ent->client->pers.netname, ent->client->sess.warnCount));
		Com_Printf("Clients ^1Warning Level^7 must at ^33^7 to be subjected to this punishment!\n^4%s ^7current ^1Warning Level^7: ^3%i\n\"", ent->client->pers.netname, ent->client->sess.warnCount);
		return;
	}

	trap_Argv( argNum + 1, speed, sizeof( speed ) );
	slow = atoi(speed);

	if((slow <= 0 || slow == g_speed.integer) && ent->client->nvSess.slow){
		ent->client->nvSess.slow = qfalse;
		trap_SendServerCommand( -1, va("print \"%s's speed was restored.\n\"", ent->client->pers.netname));
		return;
	}

	/// Make sure they arent using it to their advantage
	if( slow > g_speed.value)
	{
		if(adm && adm->client)
			trap_SendServerCommand( adm-g_entities, va("print \"This command is a ^1Punisment ^7Command!\nSelect a value between ^30 ^7and ^3%i^7!\n\"", g_speed.integer));
		else 
			Com_Printf("This command is a ^1Punishment ^7Command!\nSelect a value between ^30 ^7and ^3%i^7!\n\"", g_speed.integer);
		return;
	}

	ent->client->nvSess.speed = slow;
	ent->client->nvSess.slow = qtrue;
	ent->client->ps.speed = slow;

	RPM_ClientSound(ent, G_SoundIndex("sound/misc/outtakes/todd_s.mp3"));

	if(ent->client->pers.camped){
		ent->client->pers.camped = qfalse;
		return;
	}

	if( ent->client->nvSess.torture || ent->client->sess.warnSeconds){
		return;
	}

	if(ent->client->pers.camped){
		ent->client->pers.camped = qfalse;
		return;
	}

	if(adm && adm->client){
		SC_adminLog (va("%s (%s) - SLOW: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip   )) ;
		trap_SendServerCommand( -1, va("print \"%s's speed\n was Altered by ^3%s.\n\"", ent->client->pers.netname, adm->client->pers.netname));
	}else{
		SC_adminLog (va("%s - SLOW: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip  )) ;
		trap_SendServerCommand( -1, va("print \"%s's speed\n was Altered by ^6ADMIN.\n\"", ent->client->pers.netname));
	}
	NV_SQLAdminLog(adm, ent, "SLOW");	
}


/* 
================
SC_Burn
GodOrDevil - 05.14.05
Modded code From 
(Ryan) - XYZ
================
*/
void SC_Burn (int argNum, gentity_t *adm)
{
	gentity_t		*ent;
	int				idnum;
	gentity_t *tent; 

	idnum = RPM_ClientNumFromArg(adm, argNum, "burn <idnumber>", "Burn", qtrue);
	if(idnum < 0) return;
	
	ent = g_entities + idnum;
	
	ent->client->sess.burnSeconds = 4;

	tent = G_TempEntity( g_entities[ent->s.number].r.currentOrigin, EV_EXPLOSION_HIT_FLESH ); 
	tent->s.eventParm = 0; 
	tent->s.otherEntityNum2 = g_entities[ent->s.number].s.number; 
	tent->s.time = WP_ANM14_GRENADE + ((((int)g_entities[ent->s.number].s.apos.trBase[YAW]&0x7FFF) % 360) << 16); 
	VectorCopy ( g_entities[ent->s.number].r.currentOrigin, tent->s.angles ); 
	SnapVector ( tent->s.angles ); 

	RPM_ClientSound(ent, G_SoundIndex("/sound/weapons/incendiary_grenade/incen01.mp3"));
		
	if (ent->client->nvSess.torture)
		return;

	if(ent->client->pers.camped){
		ent->client->pers.camped = qfalse;
		return;
	}

	if(adm && adm->client){
		trap_SendServerCommand( adm-g_entities, va("print \"%s ^7was ^1Burnt ^7to a crisp by ^3%s\n\"", ent->client->pers.netname, adm->client->pers.netname));
		SC_adminLog (va("%s (%s) - BURN: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip   )) ;
	}
	else{
		SC_adminLog (va("%s - BURN: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip  )) ;
		Com_Printf("^3%s ^7was ^1Burnt ^7to a crisp\n", ent->client->pers.netname);
	}
	NV_SQLAdminLog(adm, ent, "BURN");
}
void SC_subnetBan (int argNum, gentity_t *adm)  
{
	gentity_t		*ent;
	char			ip[16];
	char			reason[16] = "\0";
	char			size[4];
	char			info[96];
	int				idnum, s;

	if(adm && adm->client) argNum = 2;
	else argNum = 1;

	idnum = RPM_ClientNumFromArg(adm, argNum, "subnetban <id> <size> <reason>", "Subnet Ban", qfalse);
	if(idnum < 0) return;
	
	ent = g_entities + idnum;

	trap_Argv( argNum + 1, size, sizeof( size ) );
	trap_Argv( argNum + 2, reason, sizeof( reason ) );
	s = atoi(size) + 1;
	
	if (!size[0] || s <= 4 ) s = 8;
	if (s > 16) s = 16;		

	Q_strncpyz(ip, ent->client->nvSess.ip, s);
	strcpy(info, va("%s\\%s - [%s]", ip, ent->client->nvSess.cleanName, *reason?reason:"HACK"));

	if(RPM_AddToList(info, g_subnetbanlist.string, "Subnet ban", adm)){
		if(adm && adm->client)	{
			if(!ent->client->nvSess.torture){
				trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"SUBNET BANNED by %s for: %s\"\n", idnum, adm->client->pers.netname, *reason?reason:"NOT FOLLOWING THE RULES!"));
			}
			trap_SendServerCommand( adm-g_entities, va("print \"%s's ^4SUBNET ^7WAS ^1BANNED^7!\n\"", ent->client->pers.netname));
			SC_adminLog (va("%s (%s) - SUBNETBAN: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip   )) ;
		}
		else {
			if(!ent->client->nvSess.torture){
				trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"SUBNET BANNED for: %s\"\n", idnum, *reason?reason:"NOT FOLLOWING THE RULES!"));
			}
			SC_adminLog (va("%s - SUBNETBAN: %s (%s)", "RCON", ent->client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip )) ;
			Com_Printf("%s's ^4SUBNET ^7WAS ^1BANNED^7!\n", ent->client->pers.netname);
		}
		NV_SQLAdminLog(adm, ent, "SUBNET");
	}
}
gentity_t *NV_projectile (gentity_t *ent, vec3_t start, vec3_t dir, int weapon, int damage) {
	gentity_t	*missile;

	missile = G_Spawn();

	if(weapon == WP_M84_GRENADE || weapon == WP_M15_GRENADE){
		missile->r.singleClient = ent->client->ps.clientNum;
		missile->r.svFlags |= SVF_SINGLECLIENT;
	}
	else {
		missile->r.svFlags |= SVF_BROADCAST;
		missile->damage				= damage;
		missile->splashDamage		= 300;
	}
	missile->nextthink				= level.time + 1000;
	missile->think					= G_ExplodeMissile;
	missile->s.eType				= ET_MISSILE;
	missile->s.weapon				= weapon;
	missile->r.ownerNum				= ent->s.number;
	missile->parent					= ent;
	missile->classname				= "grenade";
	missile->splashRadius			= 500;
	missile->methodOfDeath			= MOD_M84_GRENADE;
	missile->splashMethodOfDeath	= MOD_M84_GRENADE;
	missile->s.eFlags				= EF_BOUNCE_HALF | EF_BOUNCE;
	missile->clipmask				= MASK_SHOT | CONTENTS_MISSILECLIP;
	//missile->s.pos.trType			= TR_INTERPOLATE;
	missile->s.pos.trType			= TR_HEAVYGRAVITY;
	missile->s.pos.trTime			= level.time - 50;
	
	if(weapon == WP_ANM14_GRENADE){
		missile->splashRadius		= 1000;
		missile->dflags				= DAMAGE_AREA_DAMAGE;
		missile->splashDamage		= 70;
	}

	VectorCopy( start,	missile->s.pos.trBase );
	VectorCopy( dir,	missile->s.pos.trDelta );

	if ( ent->client->ps.pm_flags & PMF_JUMPING )
		VectorAdd( missile->s.pos.trDelta, ent->s.pos.trDelta, missile->s.pos.trDelta );

	SnapVector( missile->s.pos.trDelta );			// save net bandwidth

	VectorCopy (start, missile->r.currentOrigin);
	return missile;
}
void NV_clusterNade( gentity_t *ent, int weapon, int numNades, int damage )
{
	vec3_t dir;
	int it, nadeDir;
	float x, y;
	gentity_t *missile;

	if(numNades > 1)
		nadeDir = 360 / numNades;
	else
		nadeDir = 1;
	for( it = 0; it < numNades; it++ ) {
		x = 100 * cos( DEG2RAD(nadeDir * it));  
		y = 100 * sin( DEG2RAD(nadeDir * it));
		VectorSet( dir, x, y, 100 );
		dir[2] = 300;
		missile = NV_projectile( ent, ent->r.currentOrigin, dir, weapon, damage );
		if(weapon == WP_M84_GRENADE || weapon == WP_M15_GRENADE || weapon == WP_ANM14_GRENADE)
			missile->nextthink = level.time + 250;
		else
			missile->nextthink = level.time + 1000;
	}
}
void SC_Flash		(int argNum, gentity_t *adm)
{	
	int		idnum;

	if(adm && adm->client) argNum = 2;
	else argNum = 1;

	idnum = RPM_ClientNumFromArg(adm, argNum, "flash <id>", "Flash", qtrue);
	if(idnum < 0 || idnum > 63)
		return;
	
	NV_clusterNade( g_entities + idnum, WP_M84_GRENADE, 1, 0 );

#ifdef _TORTURE
	if (g_entities[idnum].client->nvSess.torture)
		return;
#endif

	if(adm && adm->client)
		SC_adminLog (va("%s (%s) - FLASH: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip   )) ;
	else
		SC_adminLog (va("%s - FLASH: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip   )) ;
	NV_SQLAdminLog(adm, &g_entities[idnum], "FLASH");
}
void NV_Launch	(int argNum, gentity_t *adm)
{
	gentity_t		*ent;
	int				idnum;

	idnum = RPM_ClientNumFromArg(adm, argNum, "launch <idnumber>", "Launch", qtrue);
	if(idnum < 0) return;
	
	ent = g_entities + idnum;

	G_Damage (ent, NULL, NULL, NULL, NULL, 20, 0, MOD_FALLING, HL_NONE );

	ent->client->ps.velocity[2] = 800;

#ifdef _TORTURE
	if (ent->client->nvSess.torture)
		return;
#endif

	if(adm && adm->client)
	{
		trap_SendServerCommand( adm-g_entities, va("print \"^3%s ^7was ^1LAUNCHED ^7into the air by ^3%s\n\"", ent->client->pers.netname, adm->client->pers.netname));
		SC_adminLog (va("%s (%s) - Launch: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, g_entities[idnum].client->nvSess.cleanName, g_entities[idnum].client->nvSess.ip   )) ;
	}
	else
	{
		SC_adminLog (va("%s - Launch: %s (%s)", "RCON", g_entities[idnum].client->nvSess.cleanName,  g_entities[idnum].client->nvSess.ip   )) ;
		Com_Printf("^3%s ^7was ^1LAUNCHED ^7into the air\n", ent->client->pers.netname);
	}
	NV_SQLAdminLog(adm, ent, "LAUNCHED");
}
void NV_ClipSize (int argNum, gentity_t *adm)
{
	int				i;
	char			cmd[16] = "", clipSize[5] = "";
	attackData_t	*attack;

	
	trap_Argv( argNum, cmd, sizeof( cmd ) );
	trap_Argv( argNum + 1, clipSize, sizeof( clipSize ) );
	
	if ((Q_stricmp(cmd, "") == 0) || Q_stricmp(cmd, "?") == 0){
		if(adm && adm->client)
			trap_SendServerCommand( adm-g_entities, va("print \"------------------------\n\""));
		else
			Com_Printf("------------------------\n");
		for(i = WP_KNIFE; i < WP_NUM_WEAPONS; i++){
			if(adm && adm->client)
				trap_SendServerCommand( adm-g_entities, va("print \"[^3%-2i^7] ^7%-15s\n\"", i, bg_weaponNames[i]));
			else
				Com_Printf("[^3%-2i^7] ^7%-15s\n", i, bg_weaponNames[i]);
		}
		if(adm && adm->client){
			trap_SendServerCommand( adm-g_entities, va("print \"------------------\n\""));
			trap_SendServerCommand( adm-g_entities, va("print \"^3Usage: ^7/adm clipsize <wpn> <size>\n\""));
		}
		else {
			Com_Printf("------------------\n");
			Com_Printf("^3Usage: ^7/clipsize <wpn> <size>\n");
		}
		return;
	}

	for(i = 1; i < WP_NUM_WEAPONS; i++)
	{
		if (Q_stricmp(bg_weaponNames[i], cmd) == 0 || atoi(cmd) == i)
		{			
			attack = &weaponData[i].attack[ATTACK_NORMAL];
			if(!clipSize[0]){
				if(adm && adm->client)
					trap_SendServerCommand( adm-g_entities, va("print \"^1%s ^7current ^3clipSize^7: ^3%i\n\"", bg_weaponNames[i], attack->clipSize));
				else 
					Com_Printf("^1%s ^7current ^3clipSize^7: ^3%i\n", bg_weaponNames[i], attack->clipSize);
				return;
			}
			ammoData[attack->ammoIndex].max -= attack->clipSize * attack->extraClips ;
			attack->clipSize = atoi(clipSize);
			ammoData[attack->ammoIndex].max += attack->clipSize * attack->extraClips ;
			if(adm && adm->client)
			{
				trap_SendServerCommand( adm-g_entities, va("print \"^1%s ^3clipSize ^7set to ^3%s\n\"", bg_weaponNames[i], clipSize));
				SC_adminLog (va("%s (%s) - Clipsize: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, bg_weaponNames[i], clipSize  )) ;
			}
			else 
			{
				Com_Printf("^1%s ^3clipSize ^7set to ^3%s\n", bg_weaponNames[i], clipSize);
				SC_adminLog (va("%s - Clipsize: %s (%s)", "RCON", bg_weaponNames[i], clipSize  )) ;
			}
			NV_SQLAdminLog(adm, NULL, va("Clipsize: %s (%s)", bg_weaponNames[i], clipSize));
			return;
		}
	}

	if(adm != NULL)
		trap_SendServerCommand( adm-g_entities, va("print \"^1Invalid Weapon: ^3%s^7\ntype ^3/adm clipsize ? ^7to see the number of the weapon\n\"", cmd));
	else
		Com_Printf("Invalid Weapon: ^3%s^7\ntype ^3/adm clipsize ? ^7to see the number of the weapon\n");
}
void NV_ExtraClips (int argNum, gentity_t *adm)
{
	int				i;
	char			cmd[16] = "", extraClips[5] = "";
	attackData_t	*attack;

	
	trap_Argv( argNum, cmd, sizeof( cmd ) );
	trap_Argv( argNum + 1, extraClips, sizeof( extraClips ) );
	
	if ((Q_stricmp(cmd, "") == 0) || Q_stricmp(cmd, "?") == 0){
		if(adm && adm->client)
			trap_SendServerCommand( adm-g_entities, va("print \"------------------------\n\""));
		else
			Com_Printf("------------------------\n");
		for(i = WP_KNIFE; i < WP_NUM_WEAPONS; i++){
			if(adm && adm->client)
				trap_SendServerCommand( adm-g_entities, va("print \"[^3%-2i^7] ^7%-15s\n\"", i, bg_weaponNames[i]));
			else
				Com_Printf("[^3%-2i^7] ^7%-15s\n", i, bg_weaponNames[i]);
		}
		if(adm && adm->client){
			trap_SendServerCommand( adm-g_entities, va("print \"------------------\n\""));
			trap_SendServerCommand( adm-g_entities, va("print \"^3Usage: ^7/adm extraClips <wpn> <size>\n\""));
		}
		else {
			Com_Printf("------------------\n");
			Com_Printf("^3Usage: ^7/extraClips <wpn> <size>\n");
		}
		return;
	}

	for(i = 1; i < WP_NUM_WEAPONS; i++)
	{
		if (Q_stricmp(bg_weaponNames[i], cmd) == 0 || atoi(cmd) == i)
		{			
			attack = &weaponData[i].attack[ATTACK_NORMAL];
			if(!extraClips[0]){
				if(adm && adm->client)
					trap_SendServerCommand( adm-g_entities, va("print \"^1%s ^7current ^3extraClips^7: ^3%i\n\"", bg_weaponNames[i], attack->extraClips));
				else 
					Com_Printf("^1%s ^7current ^3extraClips^7: ^3%i\n", bg_weaponNames[i], attack->extraClips);
				return;
			}
			ammoData[attack->ammoIndex].max -= attack->clipSize * attack->extraClips;
			attack->extraClips = atoi(extraClips);
			ammoData[attack->ammoIndex].max += attack->clipSize * attack->extraClips;
			if(adm && adm->client)
			{
				trap_SendServerCommand( adm-g_entities, va("print \"^1%s ^3extraClips ^7set to ^3%s\n\"", bg_weaponNames[i], extraClips));
				SC_adminLog (va("%s (%s) - ExtraClips: %s (%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip, bg_weaponNames[i], extraClips  )) ;
			}
			else 
			{
				Com_Printf("^1%s ^3extraClips ^7set to ^3%s\n", bg_weaponNames[i], extraClips);
				SC_adminLog (va("%s - ExtraClips: %s (%s)", "RCON", bg_weaponNames[i], extraClips  )) ;
			}
			NV_SQLAdminLog(adm, NULL, va("ExtraClips: %s (%s)", bg_weaponNames[i], extraClips));
			return;
		}
	}

	if(adm != NULL)
		trap_SendServerCommand( adm-g_entities, va("print \"^1Invalid Weapon: ^3%s^7\ntype ^3/adm extraClips ? ^7to see the number of the weapon\n\"", cmd));
	else
		Com_Printf("Invalid Weapon: ^3%s^7\ntype ^3/adm extraClips ? ^7to see the number of the weapon\n");
}
void NV_WeaponOnly (int argNum, gentity_t *adm)
{
	int				i;
	char			cmd[16] = "";

	trap_Argv( argNum, cmd, sizeof( cmd ) );

	if ((Q_stricmp(cmd, "") == 0) || Q_stricmp(cmd, "?") == 0){
		if(adm && adm->client)
			trap_SendServerCommand( adm-g_entities, va("print \"------------------------\n\""));
		else
			Com_Printf("------------------------\n");
		for(i = WP_KNIFE; i < WP_NUM_WEAPONS; i++){
			int j = 0; 
			if(g_onlyWeapons.integer & (1<<i)) 
				j = 1;
			if(adm && adm->client)
				trap_SendServerCommand( adm-g_entities, va("print \"[^3%-2i^7] ^7%-15s ^7[%i]\n\"", i, bg_weaponNames[i], j));
			else
				Com_Printf("[^3%-2i^7] ^7%-15s ^7[%i]\n", i, bg_weaponNames[i], j);
		}
		if(adm && adm->client){
			trap_SendServerCommand( adm-g_entities, va("print \"------------------\n\""));
			trap_SendServerCommand( adm-g_entities, va("print \"^3Usage: ^7/adm only <wpn>\n\""));
		}
		else {
			Com_Printf("------------------\n");
			Com_Printf("^3Usage: ^7/only <wpn>\n");
		}
		return;
	}
	if ((Q_stricmp(cmd, "off") == 0) || (Q_stricmp(cmd, "0") == 0)){
		nv_onlyWeapons.integer = 0; 				
		trap_Cvar_Set("nv_onlyWeapons", "0");
		if(adm && adm->client)
		{
			trap_SendServerCommand( adm-g_entities, va("print \"^3Selected weapons have been disabled\n\""));
			SC_adminLog (va("%s (%s) - WeaponOnly: OFF", adm->client->nvSess.cleanName, adm->client->nvSess.ip )) ;
		}
		else 
		{
			Com_Printf("^3Selected weapons have been disabled\n");
			SC_adminLog (va("%s - WeaponOnly: OFF", "RCON" )) ;
		}		
		NV_SQLAdminLog(adm, NULL, "WeaponOnly: OFF");
		return;

	}

	for(i = WP_KNIFE; i < WP_NUM_WEAPONS; i++)
	{
		if (Q_stricmp(bg_weaponNames[i], cmd) == 0 || atoi(cmd) == i)
		{
			if(nv_onlyWeapons.integer & (1<<i))
			{
				nv_onlyWeapons.integer &= ~(1<<i); 				
				trap_Cvar_Set("nv_onlyWeapons",va("%i", nv_onlyWeapons.integer));
				if(adm && adm->client)
				{
					trap_SendServerCommand( adm-g_entities, va("print \"^1%s ^3Disabled\n\"", bg_weaponNames[i]));
					SC_adminLog (va("%s (%s) - WeaponOnly: %s - OFF", adm->client->nvSess.cleanName, adm->client->nvSess.ip, bg_weaponNames[i] )) ;
				}
				else 
				{
					Com_Printf("^1%s ^3Disabled\n", bg_weaponNames[i]);
					SC_adminLog (va("%s - WeaponOnly: %s - OFF", "RCON", bg_weaponNames[i] )) ;
				}
				NV_SQLAdminLog(adm, NULL, va("WeaponOnly: %s - OFF", bg_weaponNames[i]));
			}
			else 
			{
				nv_onlyWeapons.integer |= (1<<i);
				trap_Cvar_Set("nv_onlyWeapons",va("%i", nv_onlyWeapons.integer));
				if(adm && adm->client)
				{
					trap_SendServerCommand( adm-g_entities, va("print \"^1%s ^3Enabled\n\"", bg_weaponNames[i]));
					SC_adminLog (va("%s (%s) - WeaponOnly: %s - ON", adm->client->nvSess.cleanName, adm->client->nvSess.ip, bg_weaponNames[i] )) ;
				}
				else 
				{
					Com_Printf("^1%s ^3Enabled\n", bg_weaponNames[i]);
					SC_adminLog (va("%s - WeaponOnly: %s - ON", "RCON", bg_weaponNames[i] )) ;
				}
				NV_SQLAdminLog(adm, NULL, va("WeaponOnly: %s - ON", bg_weaponNames[i]));
			}
		}
	}
}
void NV_ForcedWeapons (int argNum, gentity_t *adm)
{
	int				i;
	char			cmd[16] = "";

	trap_Argv( argNum, cmd, sizeof( cmd ) );

	if ((Q_stricmp(cmd, "") == 0) || Q_stricmp(cmd, "?") == 0){
		if(adm && adm->client)
			trap_SendServerCommand( adm-g_entities, va("print \"------------------------\n\""));
		else
			Com_Printf("------------------------\n");
		for(i = WP_KNIFE; i < WP_NUM_WEAPONS; i++){
			int j = 0; 
			if(g_ForcedItems.integer & (1<<i)) 
				j = 1;
			if(adm && adm->client)
				trap_SendServerCommand( adm-g_entities, va("print \"[^3%-2i^7] ^7%-15s ^7[%i]\n\"", i, bg_weaponNames[i], j));
			else
				Com_Printf("[^3%-2i^7] ^7%-15s ^7[%i]\n", i, bg_weaponNames[i], j);
		}
		if(adm && adm->client){
			trap_SendServerCommand( adm-g_entities, va("print \"------------------\n\""));
			trap_SendServerCommand( adm-g_entities, va("print \"^3Usage: ^7/adm force <wpn>\n\""));
			trap_SendServerCommand( adm-g_entities, va("print \"^3Toggle feature: ^7/adm force on/off\n\""));
		}
		else {
			Com_Printf("------------------\n");
			Com_Printf("^3Usage: ^7/force <wpn>\n");
			Com_Printf("^3Toggle feature: ^7/force on/off\n");
		}
		return;
	}
	if ((Q_stricmp(cmd, "off") == 0) || (Q_stricmp(cmd, "disable") == 0)){
		g_ForcedItems.integer = 0; 				
		trap_Cvar_Set("g_ForcedItems", "0");
		if(adm && adm->client)
		{
			trap_SendServerCommand( adm-g_entities, va("print \"^3Forced weapons have been disabled\n\""));
			SC_adminLog (va("%s (%s) - ForcedWeapons: OFF", adm->client->nvSess.cleanName, adm->client->nvSess.ip )) ;
		}
		else 
		{
			Com_Printf("^3Forced weapons have been disabled\n");
			SC_adminLog (va("%s - ForcedWeapons: OFF", "RCON" )) ;
		}
		NV_SQLAdminLog(adm, NULL, "ForcedWeapons: OFF");
		return;
	}
	//if ((Q_stricmp(cmd, "on") == 0) || (Q_stricmp(cmd, "0") == 0)){
	//	if(adm && adm->client)
	//		trap_SendServerCommand( adm-g_entities, va("print \"^3Forced weapons have been enabled\n\""));
	//	else 
	//		Com_Printf("^3Forced weapons have been enabled\n");
	//	return;
	//}


	for(i = WP_KNIFE; i < WP_NUM_WEAPONS; i++)
	{
		if (Q_stricmp(bg_weaponNames[i], cmd) == 0 || atoi(cmd) == i)
		{
			if(g_ForcedItems.integer & (1<<i)){
				g_ForcedItems.integer &= ~(1<<i); 				
				trap_Cvar_Set("g_ForcedItems",va("%i", g_ForcedItems.integer));
				if(adm && adm->client)
				{
					trap_SendServerCommand( adm-g_entities, va("print \"^1%s ^3Disabled\n\"", bg_weaponNames[i]));
					SC_adminLog (va("%s (%s) - ForcedWeapons: %s - OFF", adm->client->nvSess.cleanName, adm->client->nvSess.ip,  bg_weaponNames[i] )) ;
				}
				else 
				{
					Com_Printf("^1%s ^3Disabled\n", bg_weaponNames[i]);
					SC_adminLog (va("%s - ForcedWeapons: %s - OFF", "RCON", bg_weaponNames[i] )) ;
				}
				NV_SQLAdminLog(adm, NULL, va("ForcedWeapons: %s - OFF", bg_weaponNames[i]));

			}
			else 
			{
				g_ForcedItems.integer |= (1<<i);
				trap_Cvar_Set("g_ForcedItems",va("%i", g_ForcedItems.integer));
				if(adm && adm->client)
				{
					trap_SendServerCommand( adm-g_entities, va("print \"^1%s ^3Enabled\n\"", bg_weaponNames[i]));
					SC_adminLog (va("%s (%s) - ForcedWeapons: %s - ON", adm->client->nvSess.cleanName, adm->client->nvSess.ip,  bg_weaponNames[i] )) ;
				}
				else 
				{
					Com_Printf("^1%s ^3Enabled\n", bg_weaponNames[i]);
					SC_adminLog (va("%s - ForcedWeapons: %s - ON", "RCON", bg_weaponNames[i] )) ;
				}		
				NV_SQLAdminLog(adm, NULL, va("ForcedWeapons: %s - ON", bg_weaponNames[i]));
			}
		}
	}
}





#endif

