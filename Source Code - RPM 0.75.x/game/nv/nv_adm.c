#include "../g_local.h"


#ifdef _NV_ADMIN


// Different from ClientBegin.  Skip shit i dont want.
void NV_ClientBegin( int clientNum ) 
{
	gentity_t	*ent;
	gclient_t	*client;
	int			flags;
	int			spawncount;

	ent = g_entities + clientNum;

	client = level.clients + clientNum;

	if ( ent->r.linked ) 
	{
		trap_UnlinkEntity( ent );
	}

	// Run a gametype check just in case the game hasnt started yet
	if ( !level.gametypeStartTime )
	{
		CheckGametype ( );
	}

	G_InitGentity( ent );
	ent->touch = 0;
	ent->pain = 0;
	ent->client = client;

	client->pers.connected = CON_CONNECTED;
	//client->pers.enterTime = level.time;
	client->pers.teamState.state = TEAM_BEGIN;


	// save eflags around this, because changing teams will
	// cause this to happen with a valid entity, and we
	// want to make sure the teleport bit is set right
	// so the viewpoint doesn't interpolate through the
	// world to the new position
	flags = client->ps.eFlags;
	spawncount = client->ps.persistant[PERS_SPAWN_COUNT];
	memset( &client->ps, 0, sizeof( client->ps ) );
	client->ps.eFlags = flags;
	client->ps.persistant[PERS_SPAWN_COUNT] = spawncount;

	// locate ent at a spawn point
	ClientSpawn( ent );
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


void NV_Finish (gentity_t *adm, gentity_t *ent, char *action, int flags)
{		
	int		i;			
	char	target[128] = "\0";

	if(ent && ent != NULL)
	{
#ifdef _TORTURE
		if(ent->client->nvSess.torture)
			return;
#endif
		if(ent->client->pers.camped)
		{
			ent->client->pers.camped = qfalse;
			return;
		}
		Com_sprintf(target, sizeof(target), "%s",  ent->client->pers.netname);	
	}	

	if(flags & FINISH_ADMIN_ONLY)
	{		
		for (i = 0; i < level.numConnectedClients; i++)	
		{
			if (level.clients[level.sortedClients[i]].nvSess.admin || level.clients[level.sortedClients[i]].nvSess.referee)
			{	
				if(adm != NULL)
					CPx(level.sortedClients[i], va("print \"^5[ADMIN] ^7%s^7: ^3[%s] ^7%s\n\"", adm->client->pers.netname, action, target));
				else 
					CPx(level.sortedClients[i], va("print \"^5[ADMIN] ^3[%s] ^7%s\n\"", action, target));
			}
		}
	}
	if((flags & FINISH_CP) && ent!=NULL)
	{
		//if(adm != NULL) 
		//{
		//	CP(va("cp \"^3%s ^7was ^3%s\n\"", ent->client->pers.netname, action, adm->client->pers.netname));
		//}
		//else 
		{
			CP(va("cp \"^3%s ^+was ^3%s^+!\n\"", ent->client->pers.netname, action));	
		}
	} 
	if((flags & FINISH_PRINT) && ent!=NULL)
	{
		if(adm != NULL) 
		{
			CP(va("print \"^5[ADMIN] ^3%s ^7was ^3%s ^7by ^3%s.\n\"", ent->client->pers.netname, action, adm->client->pers.netname));
			//CPx( ent->s.number, va("cp \"^3You ^7were ^1%s ^7by ^3%s\n", action, adm->client->pers.netname));
			CPx( ent->s.number, va("cp \"^3You ^+were ^1%s\n", action));
		}
		else 
		{
			CP(va("print \"^5[ADMIN] ^3%s ^7was ^3%s^7!\n\"",ent->client->pers.netname, action));	
			//CPx(ent->s.number, va("cp \"^3You ^7were ^1%s ^7by the ^6ADMIN\n", action));
			CPx(ent->s.number, va("cp \"^3You ^+were ^1%s\n", action));
		}
	}
	if(flags & FINISH_LOG)
	{	

#ifdef _DB_ENABLED
		if(g_enableDatabase.integer)
		{	
			char update[128] = "\0";
			int client = -1, admin = -1;

			if(ent && ent->client)
				client = ent->client->nvSess.dbID;
			if(adm && adm->client)
				admin = adm->client->nvSess.dbID;
			if(admin == -1 && (level.time - level.startTime) <= 2000)
				return;
			Com_sprintf(update, sizeof(update), "%i;%i;%s;", admin, client, action);
			NV_QueryDB(DB_LITE_ADMINLOG, update);
		//	return;
		}
#endif

#ifdef _COMPMODE
		if(g_compMode.integer)
		{	
			if(adm != NULL)
			{
				if(target[0])
					NV_Log (6, va("%s(%s) - %s: %s", adm->client->nvSess.cleanName, adm->client->nvSess.ip, action, target ));
				else
					NV_Log (6, va("%s(%s) - %s", adm->client->nvSess.cleanName, adm->client->nvSess.ip, action ));
			}
			else 	
				if(target[0])
					NV_Log (6, va("%s - %s: %s", "RCON", action, target )) ;
				else
					NV_Log (6, va("%s - %s", "RCON", action )) ;
			return;

		}
#endif
		if(adm != NULL && adm->client != NULL)
		{
			if( ent!=NULL)
				NV_Log (3, va("%s(%s) - %s: %s(%s)", adm->client->nvSess.cleanName, adm->client->nvSess.ip,  action, ent->client->nvSess.cleanName, ent->client->nvSess.ip )) ;
			else
				NV_Log (3, va("%s(%s) - %s: %s", adm->client->nvSess.cleanName, adm->client->nvSess.ip,  action, target )) ;
		}
		else
		{ 
			if( ent!=NULL)
				NV_Log (3, va("%s - %s: %s(%s)", "RCON", action, ent->client->nvSess.cleanName, ent->client->nvSess.ip )) ;
			else
				NV_Log (3, va("%s - %s: %s", "RCON", action, target )) ;
		}
	}
}
qboolean NV_extraCommands (gentity_t *ent, char *input)
{
	char			buffer[15000] = "\0";	
	const char		*token;
	char			cvar[32] =	"\0";
	char			shortName[128] = "\0";
	char			*p = "\0"; 

	NV_LoadFile( "settings/extraCmds.cfg", buffer, qfalse );
	token = buffer;
	
	while(token) 
	{	
		p	= COM_ParseExt(&token, qtrue);
		Q_strncpyz( cvar, p, strlen(p) + 1);
		p = COM_ParseExt(&token, qtrue);
		Q_strncpyz( shortName, p, strlen(p) + 1);
		p	= COM_ParseExt(&token, qtrue);
		
		/// -- Print out the commands at the bottom of the Admin command list
		if(!strcmp(input, "showExtra")){
			if(atoi(p) > ent->client->nvSess.admin)
				continue;
			if(!strcmp(shortName, ""))
				continue;
			if(ent != NULL)
				CPe(va("print \"@^7[^5%-12s^7%-22s^7][^3%s^7]\n\"", shortName, "<value>", p));
			else
				Com_Printf("^7[^5%-12s^7%-22s^7][^3%s^7]\n", shortName, "<value>", p);
			continue;
		}
		/// --

		if(!strcmp(input, shortName)){
			///char			*cmd1 = "\0";
			char			cmd1[128] = "\0";
			char			cmd2[32] = "\0";
			char			cmd3[32] = "\0";

			if(atoi(p) > ent->client->nvSess.admin)
				return qfalse;
			trap_Argv( 2, cmd1, sizeof( cmd1 ));
			///cmd1 = ConcatArgs(2);
			if(!cmd1[0])
			{
				trap_SendConsoleCommand( EXEC_APPEND, va("%s\n", cvar));
				NV_Finish (ent, NULL, cvar, FINISH_LOG|FINISH_ADMIN_ONLY);
			}
			else 
			{
				trap_Argv( 3, cmd2, sizeof( cmd2 ));
				trap_Argv( 4, cmd3, sizeof( cmd3 ));
				if(!cmd2[0])
				{
					trap_SendConsoleCommand( EXEC_APPEND, va("%s \"%s\"", cvar, cmd1) );
					Com_sprintf(cmd1, sizeof(cmd1), "%s %s", cvar, cmd1);
					NV_Finish (ent, NULL, cmd1, FINISH_LOG|FINISH_ADMIN_ONLY);
				}	
				else if(!cmd3[0])
				{
					trap_SendConsoleCommand( EXEC_APPEND, va("%s \"%s\" \"%s\"", cvar, cmd1, cmd2) );	
					Com_sprintf(cmd1, sizeof(cmd1), "%s %s %s", cvar, cmd1, cmd2);
					NV_Finish (ent, NULL, cmd1, FINISH_LOG|FINISH_ADMIN_ONLY);
				}
				else
				{
					trap_SendConsoleCommand( EXEC_APPEND, va("%s %s %s %s", cvar, cmd1, cmd2, cmd3) );
					Com_sprintf(cmd1, sizeof(cmd1), "%s %s %s %s", cvar, cmd1, cmd2, cmd3);
					NV_Finish (ent, NULL, cmd1, FINISH_LOG|FINISH_ADMIN_ONLY);
				}
			}
			return qtrue;
		}
	}
	return qfalse;
}
void NV_Pop			(int argNum, gentity_t *adm, int idnum)  
{
	gentity_t		*ent;

	if(!NV_canUseCommand(adm, idnum, "Pop", qfalse)) 
		return;	
	ent = &g_entities[idnum];

	G_Damage (ent, NULL, NULL, NULL, NULL, 10000, 0, MOD_POP, HL_HEAD|HL_LEG_UPPER_RT|HL_LEG_UPPER_LT|HL_ARM_RT|HL_ARM_LT);
		
	NV_Sound(idnum, "sound/npc/air1/guard02/laughs.mp3");

#ifdef _TORTURE
	if(!ent->client->nvSess.torture && !ent->client->pers.camped)
#else 
	if(!ent->client->pers.camped)
	{
#endif
		NV_Finish (adm, ent, "POPPED", FINISH_LOG|FINISH_PRINT);
	}
}
void NV_Mute		(int argNum, gentity_t *adm, int idnum)
{
	char *muted;
	gentity_t		*ent;

	if(!NV_canUseCommand(adm, idnum, "", qfalse)) return;
	ent = &g_entities[idnum];

	if(ent->client->sess.mute)
	{
		ent->client->sess.mute = qfalse;
		muted = "UNMUTED";
	}
	else 
	{
		ent->client->sess.mute = qtrue;
		muted = "MUTED";
	}
	NV_Finish (adm, ent, muted, FINISH_LOG|FINISH_CP|FINISH_PRINT);
}
void NV_AddAdmin	(gentity_t *ent, gentity_t *adm, int admin, char* append, char *file, char *type, char* colored)
{	
	char		id2[64];
	qboolean	added = qfalse;

	if(ent->client->nvSess.admin > admin && adm != NULL)
	{
		CPx( adm->s.number, va("print \"^5This player has a higher level of admin already\n"));
		return;
	}
	ent->client->nvSess.admin = admin;

#ifdef _DB_ENABLED
	if(g_enableDatabase.integer == 2)
	{
		char update[64] = "\0";
		Com_sprintf(update, sizeof(update), "%i;%s;%s;", admin, ent->client->nvSess.ip2, ent->client->nvSess.cleanName);
		NV_QueryDB(DB_LITE_ADDADMIN, update);
		added = qtrue;
	}	
	else
#endif	
	{
		RPM_Remove_from_list(ent->client->pers.rpm_id, g_adminfile.string,	 "admin", NULL, qfalse, qtrue);	
	#ifdef _BADMIN_FILE
		RPM_Remove_from_list(ent->client->pers.rpm_id, g_badminfile.string,	 "badmin", NULL, qfalse, qtrue);
	#endif

		Q_strncpyz (id2, ent->client->pers.rpm_id, 64);
		strcat ( id2, append );
		if(RPM_AddToList(id2, file, "Admin", NULL)) 
		{
			added = qtrue;
		}
	}
	
	//if(g_noRanks.integer)
	//	trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@^3%s now has ^6ADMIN ^1POWERS.", level.time + 5000,	ent->client->pers.netname));
	//else 
		trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@^3%s now has %s ^1POWERS.", level.time + 5000,	ent->client->pers.netname, colored));
	
	Com_sprintf(id2, sizeof(id2), "Granted %s", type);
	NV_Finish (adm, ent, id2, FINISH_LOG|FINISH_ADMIN_ONLY);


	//Com_Printf("^3%s ^7was added to the ADMIN ^7list. (^3S-Admin^7)\n",ent->client->pers.netname);
	//NV_AdminOnlyPrint (adm, idnum, "Granted S-ADMIN");
	//CPx(idnum, "cp \"^3You ^7have been ^3granted ^SS-ADMIN ^7Status\n\"" );	
}
void NV_addBAdmin	(int argNum, gentity_t *adm, int idnum)
{
	if(NV_canUseCommand(adm, idnum, "", qfalse))
#ifdef _BADMIN_FILE
		NV_AddAdmin(&g_entities[idnum], adm, 2, ":2", g_badminfile.string, "B-Admin", "^4B-ADMIN");
#else
		NV_AddAdmin(&g_entities[idnum], adm, 2, ":2", g_adminfile.string, "B-Admin", "^4B-ADMIN");
#endif
}
void NV_addAdmin	(int argNum, gentity_t *adm, int idnum)
{
	if(NV_canUseCommand(adm, idnum, "", qfalse))
		NV_AddAdmin(&g_entities[idnum], adm, 3, ":3", g_adminfile.string, "Admin", "^6ADMIN");
}
void NV_addSAdmin	(int argNum, gentity_t *adm, int idnum)
{
	if(NV_canUseCommand(adm, idnum, "", qfalse))
		NV_AddAdmin(&g_entities[idnum], adm, 4, ":4", g_adminfile.string, "S-Admin", "^gS-ADMIN");
}
#ifdef _RECRUITER
void NV_addRAdmin	(int argNum, gentity_t *adm, int idnum)
{
	if(NV_canUseCommand(adm, idnum, "", qfalse))
		NV_AddAdmin(&g_entities[idnum], adm, 5, ":5", g_adminfile.string, "Recruiter", "^3RECRUITER");
}
#endif
void NV_removeAdmin (int argNum, gentity_t *adm, int idnum)  
{
	gentity_t	*ent;

	if(!NV_canUseCommand(adm, idnum, "", qfalse)) return;

	ent = &g_entities[idnum];
	ent->client->nvSess.admin = 0;

#ifdef _DB_ENABLED
	if(g_enableDatabase.integer == 2)
	{
		char update[64] = "\0";
		Com_sprintf(update, sizeof(update), "%i;%s;%s;", ent->client->nvSess.admin, ent->client->nvSess.ip2, ent->client->nvSess.cleanName);
		NV_QueryDB(DB_LITE_ADDADMIN, update);
		trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@^3%s no longer has ^6ADMIN ^1POWERS.",	level.time + 5000, ent->client->pers.netname));
		NV_Finish (adm, ent, "Demoted", FINISH_LOG|FINISH_ADMIN_ONLY);
		return;
	}	
#endif

	RPM_Remove_from_list(g_entities[idnum].client->pers.rpm_id, g_adminfile.string, "admin", NULL, qfalse, qtrue);
#ifdef _BADMIN_FILE
	RPM_Remove_from_list(g_entities[idnum].client->pers.rpm_id, g_badminfile.string,"badmin", NULL, qfalse, qtrue);
#endif	

	NV_Finish (adm, ent, "Demoted", FINISH_LOG|FINISH_PRINT);	
	trap_SetConfigstring ( CS_GAMETYPE_MESSAGE, va("%i,@^3%s no longer has ^6ADMIN ^1POWERS.",	level.time + 5000, ent->client->pers.netname));
}
void NV_Ban			(int argNum, gentity_t *adm, int idnum)  
{	
	char            banid[128];
	char			reason[22] = "\0";
//	qboolean		added = qfalse;
	gentity_t*		ent;		
#ifdef _DB_ENABLED
	char				banTime[5] = "\0";
#endif

	if(!NV_canUseCommand(adm, idnum, "", qfalse)) 
		return;
		
	ent = &g_entities[idnum];
#ifdef _DB_ENABLED
	trap_Argv( argNum + 1, banTime, sizeof( banTime ) );
	trap_Argv( argNum + 2, reason, sizeof( reason ) );

	if(g_enableDatabase.integer == 2)
	{		
		char update[1024] = "\0";

		if(!banTime[0])
			Com_sprintf(banTime, sizeof(banTime), "0");

		NV_Finish (adm, ent, "BANNED", FINISH_LOG|FINISH_PRINT);

		if(!*reason)
		{	
			if(adm && adm->client)	
			{
				Com_sprintf(update, sizeof(update), "%i;%s;%s;%s;%s;%0.4f;%s", adm->s.number, adm->client->nvSess.ip2, adm->client->nvSess.cleanName, ent->client->nvSess.ip2, ent->client->nvSess.cleanName, atof(banTime), " ");
				NV_QueryDB(DB_LITE_ADDBAN, update);
				trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"BANNED FROM SERVER! by %s\"\n", idnum, adm->client->pers.netname));
			}
			else
			{
				Com_sprintf(update, sizeof(update), "%i;%s;%s;%s;%s;%0.4f;%s", -1, "RCON", "RCON", ent->client->nvSess.ip2, ent->client->nvSess.cleanName, atof(banTime), " ");
				NV_QueryDB(DB_LITE_ADDBAN, update);
				trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"BANNED FROM SERVER! by %s\"\n", idnum, "RCON"));
			}
		}
		else
		{	
			if(adm && adm->client)
			{
				Com_sprintf(update, sizeof(update), "%i;%s;%s;%s;%s;%0.4f;%s", adm->s.number, adm->client->nvSess.ip2, adm->client->nvSess.cleanName, ent->client->nvSess.ip2, ent->client->nvSess.cleanName, atof(banTime), reason);
				NV_QueryDB(DB_LITE_ADDBAN, update);
				trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"BANNED by %s for: %s\"\n", idnum, adm->client->pers.netname, reason));
			}
			else
			{
				Com_sprintf(update, sizeof(update), "%i;%s;%s;%s;%s;%0.4f;%s", -1, "RCON", "RCON", ent->client->nvSess.ip2, ent->client->nvSess.cleanName, atof(banTime), reason);
				NV_QueryDB(DB_LITE_ADDBAN, update);
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
			NV_Finish (adm, ent, "BANNED", FINISH_LOG|FINISH_PRINT);
			if(adm && adm->client)
				G_LogPrintf("%i %s (%s) BANNED BY %s (%s)\n", idnum, ent->client->nvSess.cleanName, ent->client->nvSess.ip,  adm->client->nvSess.cleanName, adm->client->nvSess.ip );
			else
				G_LogPrintf("%i %s (%s) BANNED BY %s\n", idnum, ent->client->nvSess.cleanName, ent->client->nvSess.ip,  "RCON" );

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
#ifdef _DB_ENABLED
	}
#endif

}
#ifdef _DB_ENABLED
void NV_DoUnban(gentity_t *adm, char*ip, int userID)
{
	char banstring[32] = "\0";
	char update[64] = "\0";

	if (userID > 0)
	{
		if (adm != NULL)
			Com_sprintf(update, sizeof(update), "%i;%s;%i;", userID, "0", adm->s.number);
		else
			Com_sprintf(update, sizeof(update), "%i;%s;%i;", userID, "0", -1);
		NV_QueryDB(DB_LITE_UNBAN, update);

		Com_sprintf(banstring, sizeof(banstring), "UNBAN ID: %i", userID);
		NV_Finish(adm, NULL, banstring, FINISH_LOG);
		return;
	}

	if (adm != NULL)
		Com_sprintf(update, sizeof(update), "%i;%s;%i;", -1, ip, adm->s.number);
	else
		Com_sprintf(update, sizeof(update), "%i;%s;%i;", -1, ip, -1);	
	NV_QueryDB(DB_LITE_UNBAN, update);

	Com_sprintf(banstring, sizeof(banstring), "UNBAN IP: %s", ip);
	NV_Finish(adm, NULL, banstring, FINISH_LOG);
}
#endif
void NV_Unban		(int argNum, gentity_t *adm, int idnum)  
{
	int		count = 0, count2 = 0;
	char	ip[16] = "\0";

	trap_Argv( argNum, ip, sizeof(ip) );

#ifdef _DB_ENABLED
	if(g_enableDatabase.integer == 2)
	{	
#ifdef _SOF2_BOTS
		if(!strcmp(ip, "bot"))
		{
			NV_DoUnban(adm, ip, -1 );
			return;
		}
#endif
		if(strstr(ip, "."))
		{	
			while (ip[count] != '\0')
			{
				if(ip[count] == ' ')
				{
					count++;
					continue;
				}
				if(ip[count] == '.')
				{
					count++;
					count2++;
				}

				if(ip[count] < '0' || ip[count] > '9')	
				{
					if(adm != NULL)
						trap_SendServerCommand( adm-g_entities, va("print \"^5Invalid ip address: ^7%s\n\"", ip));
					else
						Com_Printf("Invalid ip address: %s\n", ip);
					return;
				}
				count++;
			}
			NV_DoUnban(adm, ip, -1 );
		}
		else			
		{
			int id = atoi(ip);
			if(id > 0)
			{
				NV_DoUnban(adm, "999", id );
			}
		}
		return;
	}
#endif

	while (ip[count] != '\0')
	{
		if(ip[count] == ' ')
		{
			count++;
			continue;
		}

		if(ip[count] == '.')
		{
			count++;
			count2++;
		}
		if(ip[count] < '0' || ip[count] > '9')	
		{
			if(adm != NULL)
				trap_SendServerCommand( adm-g_entities, va("print \"^5Invalid ip address: ^7%s\n\"", ip));
			else
				Com_Printf("Invalid ip address: %s\n", ip);
			return;
		}
		count++;
	}

	if(count2 < 3)
	{
		if(adm != NULL)
			trap_SendServerCommand( adm-g_entities, va("print \"^5Invalid ip address: ^7%s\n\"", ip));
		else
			Com_Printf("Invalid ip address: %s\n", ip);
		return;
	}

	if(RPM_Remove_from_list(ip, g_banlist.string, "Ban", adm, qtrue, qfalse ))
	{	
		char banstring[32] = "\0"; 			
		Com_sprintf(banstring, sizeof(banstring), "UNBAN IP: %s", ip);			
		NV_Finish (adm, NULL, banstring, FINISH_LOG);
		return;
	}
}
void NV_Team		(int argNum, gentity_t *adm, int idnum)
{	
	gentity_t		*ent;
	char	team[5] = "\0";
	char	lock[2] = "\0";

	if(!NV_canUseCommand(adm, idnum, "", qtrue)) return;
	ent = &g_entities[idnum];

	trap_Argv( argNum + 1, team, sizeof( team ) );
	trap_Argv( argNum + 2, lock, sizeof( lock ) );

	if(team[0])
	{
		ent->client->nvSess.teamlock = 1;
		SetTeam( ent, team, NULL );
		ent->client->nvSess.teamlock = 0;
		switch(ent->client->sess.team)
		{
			case TEAM_RED:
				NV_Finish (adm, ent, "Moved to RED", FINISH_LOG|FINISH_PRINT);
				if(atoi(lock) == 0)
					ent->client->nvSess.teamlock = 2;
				break;		
			case TEAM_BLUE:
				NV_Finish (adm, ent, "Moved to Blue", FINISH_LOG|FINISH_PRINT);
				if(atoi(lock) == 0)
					ent->client->nvSess.teamlock = 2;
				break;		
			case TEAM_FREE:
				NV_Finish (adm, ent, "Moved", FINISH_LOG|FINISH_PRINT);
				break;
			default:
			case TEAM_SPECTATOR:
				NV_Finish (adm, ent, "Moved to Spec", FINISH_LOG|FINISH_PRINT);
				break;
		}
	}	
}
void NV_UpperCut	(int argNum, gentity_t *adm, int idnum)
{
	gentity_t		*ent;

	if(!NV_canUseCommand(adm, idnum, "Launched", qfalse)) return;
	ent = &g_entities[idnum];
	
	//G_Damage (ent, NULL, NULL, NULL, NULL, 10, 0, MOD_ADMIN, HL_NONE );

	ent->client->ps.velocity[2] = 1200;
	
	NV_Finish (adm, ent, "Launched", FINISH_LOG|FINISH_PRINT);

}
void NV_Twist		(int argNum, gentity_t *adm, int idnum)  
{
	gentity_t	*ent;
	vec3_t		lookdown;
	char		a[4], b[4], c[4];

	if(!NV_canUseCommand(adm, idnum, "Twist", qfalse)) return;
	ent = &g_entities[idnum];

	trap_Argv( argNum + 1, c, sizeof(c) );
	trap_Argv( argNum + 2, b, sizeof(b) );
	trap_Argv( argNum + 3, a, sizeof(a) );
	
	if ( !c[0] ) VectorSet(lookdown, 100, 0, 130);
	else  VectorSet(lookdown, atoi(a), atoi(b), atoi(c));
	SetClientViewAngle(ent, lookdown);
	

	 NV_Sound(idnum, "sound/player/steps/mud/mud1.wav" );
	
#ifdef _TORTURE
	if(!ent->client->nvSess.torture)
#endif
	NV_Finish (adm, ent, "Twisted", FINISH_LOG|FINISH_ADMIN_ONLY);
}
void NV_Slow		(int argNum, gentity_t *adm, int idnum)  
{
	gentity_t		*ent;
	char			speed[5];
	int				slow;

	if(!NV_canUseCommand(adm, idnum, "Slow", qfalse)) return;
	ent = &g_entities[idnum];

	trap_Argv( argNum + 1, speed, sizeof( speed ) );
	slow = atoi(speed);

	if((slow <= 0 || slow == g_speed.integer) && ent->client->nvSess.slow){
		ent->client->nvSess.slow = qfalse;
		CP(va("print \"%s's speed has been restored.\n\"", ent->client->pers.netname));
		return;
	}
	/// Make sure they arent using it to their advantage
	if( slow > g_speed.value){
		if(adm != NULL)	CPa(va("print \"This command is a ^3Punisment ^7Command!\nSelect a value between ^30 ^7and ^3%i^7!\n\"", g_speed.integer));
		else  Com_Printf("This command is a ^3Punishment ^7Command!\nSelect a value between ^30 ^7and ^3%i^7!\n\"", g_speed.integer);
		return;
	}

	ent->client->nvSess.slow = qtrue;
	ent->client->nvSess.speed = slow;
	ent->client->ps.speed = slow;
	NV_Sound(idnum, "sound/misc/outtakes/todd_s.mp3" );
	NV_Finish (adm, ent, "Slowed", FINISH_LOG|FINISH_PRINT);
}
void NV_Rename		(int argNum, gentity_t *adm, int idnum)  
{
	char		*s;
	char		name[MAX_NETNAME] = "\0";
	char		oldname[MAX_NETNAME] = "\0";
//	char		unlock[8] = "\0";
	gentity_t	*ent;
	gclient_t	*client;

	if(!NV_canUseCommand(adm, idnum, "", qfalse)) return;	
	ent = &g_entities[idnum];
		
	trap_Argv( argNum + 1, name, sizeof( name ) );
	//name = ConcatArgs( argNum + 1 );
	if(!Q_stricmp ( name, "" )){
		ent->client->nvSess.noNameChange = 0;
		return;
	}

	client = ent->client;
	
	Com_sprintf(oldname, sizeof(oldname), "Renamed (%s)", name);
	NV_Finish (adm, ent, oldname, FINISH_LOG|FINISH_PRINT);
		
	switch(g_colorTeamNames.integer)
	{
	case 1:		
		G_ClientCleanName( name, client->pers.netname, sizeof(client->pers.netname), qtrue );
		G_ClientCleanName( name, client->pers.talkname, sizeof(client->pers.talkname), level.gametypeData->teams?qfalse:qtrue );
		break;
	case 2:		
		G_ClientCleanName( name, client->pers.netname, sizeof(client->pers.netname), qtrue );
		G_ClientCleanName( name, client->pers.talkname, sizeof(client->pers.talkname), qtrue );
		break;
	default:
	case 0:
		G_ClientCleanName( name, client->pers.netname, sizeof(client->pers.netname), level.gametypeData->teams?qfalse:qtrue );
		G_ClientCleanName( name, client->pers.talkname, sizeof(client->pers.talkname), level.gametypeData->teams?qfalse:qtrue );
		break;
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
		
	//trap_GetUserinfo( ent->s.number, userinfo, sizeof( userinfo ) );
	//Info_SetValueForKey( userinfo, "name", name );
 	//trap_SetUserinfo ( ent->s.number, userinfo );
#ifdef _SOF2_BOTS
	if ( ent->r.svFlags & SVF_BOT )
	{
		char		userinfo[MAX_INFO_STRING];
		s = va("n\\%s\\t\\%i\\identity\\%s\\skill\\%s",	name, client->sess.team, client->pers.identity->mName,	Info_ValueForKey( userinfo, "skill" ) );
	}
	else
#endif
	{	// mullinssuit
		s = va("n\\%s\\t\\%i\\identity\\%s", name,  client->sess.team, client->pers.identity->mName );
	}
	ent->client->nvSess.noNameChange = 0;
	trap_SetConfigstring( CS_PLAYERS+ent->s.number, s );
	ent->client->nvSess.noNameChange = 1;


}
#ifdef _TORTURE
void NV_unTorture	(int argNum, gentity_t *adm, int idnum)  
{
	gentity_t	*ent;

	if(!NV_canUseCommand(adm, idnum, "", qfalse)) return;	
	ent = &g_entities[idnum];

	if (!ent->client->nvSess.torture)	return;

	//trap_SendConsoleCommand( EXEC_INSERT, va("pop %i\n", ent->s.number ) );
	G_Damage (ent, NULL, NULL, NULL, NULL, 10000, 0, MOD_POP, HL_HEAD|HL_LEG_UPPER_RT|HL_LEG_UPPER_LT|HL_ARM_RT|HL_ARM_LT);
	ent->client->nvSess.torture = qfalse;
	ent->client->nvSess.tortureCount = 0;	
	NV_Finish (adm, ent, "Untortured ", FINISH_LOG|FINISH_PRINT);
}
void NV_Torture		(int argNum, gentity_t *adm, int idnum)  
{
	char		reason[MAX_TOKEN_CHARS];
	char		ban[4];
	gentity_t	*ent;

	if(!NV_canUseCommand(adm, idnum, "", qfalse)) return;	
	ent = &g_entities[idnum];
	
	if(ent->client->nvSess.torture){
		NV_unTorture(argNum, adm, idnum);
		return;
	}
	/// get the reason/time for torture
	trap_Argv( argNum + 1, reason, sizeof( reason ) );
	trap_Argv( argNum + 2, ban, sizeof( ban ) );

	if(ent->client->ps.pm_type == PM_DEAD){
		//ent->client->nvSess.penaltyBox = qfalse;
		ent->client->ps.respawnTimer = 0;
		G_StopFollowing( ent );
		G_StopGhosting( ent );
		trap_UnlinkEntity ( ent );
		ClientSpawn( ent );
	}

	///Take care of specing & ghost victims $FIX ???
	if (ent->client->sess.team == TEAM_SPECTATOR){
		G_StopFollowing( ent );
		G_StopGhosting( ent );
		SetTeam( ent, "", NULL );
		ent->client->ps.respawnTimer = 0;
		ClientSpawn( ent );
	}

	///Do a little screaming
	if ( strstr ( ent->client->pers.identity->mCharacter->mModel, "female" ) )
		NV_Sound(idnum, "sound/ambience/colombia/battlebackground/scream01.mp3" );
	else 
		NV_Sound(idnum, "sound/ambience/colombia/battlebackground/scream03.mp3" );

	G_DropGametypeItems ( ent, 10000 );

	NV_Finish (adm, ent, "Tortured ", FINISH_LOG|FINISH_PRINT);
	///set this AFTER nv_finish so we can log and announce the shit
	ent->client->nvSess.torture = qtrue;
	ent->client->sess.noTeamChange = qtrue;

	ent->client->ps.stats[STAT_ARMOR] = 0;
	trap_SendConsoleCommand( EXEC_INSERT, va("strip %i\n", ent->s.number ) );	
	trap_SendConsoleCommand( EXEC_INSERT, va("rename %i \"%s\"\n", ent->s.number, g_torturedname.string ) );	

	if (!ent->client->sess.mute)
		trap_SendConsoleCommand( EXEC_INSERT, va("mute %i\n", ent->s.number ) );

	Q_strncpyz(ent->client->nvSess.tortureReason, *reason?reason:"Not Following The Server Rules!", sizeof(ent->client->nvSess.tortureReason));
	ent->client->nvSess.tortureCount = 1;
}
#endif
void NV_Forcesay	(int argNum, gentity_t *adm, int idnum)  
{
	char  *text;

	if(!NV_canUseCommand(adm, idnum, "", qfalse)) return;	

	text = ConcatArgs( argNum + 1 );
	G_Say( g_entities + idnum, NULL, SAY_ALL, text );
#ifdef _TORTURE
	if(g_entities[idnum].client->nvSess.torture)	
		return;	
#endif
	NV_Finish (adm, g_entities + idnum, "Force say", FINISH_LOG|FINISH_ADMIN_ONLY);
}
void NV_nextMap		(int argNum, gentity_t *adm, int idnum)
{
	char		map[64];
	char		gt[6], sl[4], *b;
	int			a;
	
	trap_Argv( argNum	, map, sizeof( map ) );
	trap_Argv( argNum + 1, gt, sizeof( gt ) );
	trap_Argv( argNum + 2, sl, sizeof( sl ) );
	
	///So if nextMap was used before it will tell the nextMap scorelimit and gt.
	if (!g_nextScore.integer) a = g_scorelimit.integer;
	else a = g_nextScore.integer;
	b = g_gametype.string;

	if ( !map[0] ){
		CPa(va("print \"^3Usage^7: nextmap <map name> <gametype> <scorelimit>\n\"" ));
		return;
	}
	///does map exist on server?
	if ( !G_DoesMapExist ( map ) ){
		CPa(va("print \"This server does not have that map!\n\"" ));
		return;
	}
	///Sets the value of the Cvar(s).
	trap_Cvar_Set("g_setnextMap", map);
	trap_Cvar_Update( &g_setnextMap );
	if ( gt[0]) {
		//trap_SendConsoleCommand( EXEC_APPEND, va("g_gametype \"none\"\n", gt) ); // do this so changing from 'based' gametypes wont fuck up (ie inf to sab would be inf again)
		trap_SendConsoleCommand( EXEC_APPEND, va("g_gametype \"%s\"\n", gt) );
		b = gt;
		if ( sl[0] ) {
			trap_Cvar_Set("g_nextScore", sl);
			a = atoi(sl);;
		}
	}
	GT(va("%i,^1Next Map^+: %s ^3| ^1Gametype^+: %s ^3| ^1Scorelimit^+: %i", level.time + 5000, g_setnextMap.string, b, a));
	level.modeChangeSec = 1;
	NV_Sound(-1, "sound/misc/events/tut_door01.mp3" );
	Com_sprintf(map, sizeof(map), "nextmap: %s %s", b, g_setnextMap.string);
	NV_Finish (adm, NULL, map, FINISH_LOG|FINISH_ADMIN_ONLY);
}
void NV_friendlyFire(int argNum, gentity_t *adm, int idnum) 
{	
	char str[32] = "\0"; 			

	if (g_friendlyFire.integer)	trap_Cvar_Set("g_friendlyFire",	 "0");
		else trap_Cvar_Set("g_friendlyFire", "1");	

	Com_sprintf(str, sizeof(str), "FRIENDLY FIRE: %s", g_friendlyFire.integer?"OFF":"ON");
	NV_Finish (adm, NULL, str, FINISH_LOG|FINISH_ADMIN_ONLY);
}
void NV_ClipSize	(int argNum, gentity_t *adm, int idnum)
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
			char str[32] = "\0"; 	
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
			}
			else 
			{
				Com_Printf("^1%s ^3clipSize ^7set to ^3%s\n", bg_weaponNames[i], clipSize);
			}
			Com_sprintf(str, sizeof(str), "clipsize %s %s", bg_weaponNames[i], clipSize);	
			NV_Finish (adm, NULL, str, FINISH_LOG);
			return;
		}
	}

	if(adm != NULL)
		trap_SendServerCommand( adm-g_entities, va("print \"^1Invalid Weapon: ^3%s^7\ntype ^3/adm clipsize ? ^7to see the number of the weapon\n\"", cmd));
	else
		Com_Printf("Invalid Weapon: ^3%s^7\ntype ^3/adm clipsize ? ^7to see the number of the weapon\n");
}
void NV_ExtraClips	(int argNum, gentity_t *adm, int idnum)
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
			char str[32] = "\0"; 		
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
			}
			else 
			{
				Com_Printf("^1%s ^3extraClips ^7set to ^3%s\n", bg_weaponNames[i], extraClips);
			}	
					
			Com_sprintf(str, sizeof(str), "extraClips %s %s", bg_weaponNames[i], extraClips);	
			NV_Finish (adm, NULL, str, FINISH_LOG|FINISH_ADMIN_ONLY);
			return;
		}
	}

	if(adm != NULL)
		trap_SendServerCommand( adm-g_entities, va("print \"^1Invalid Weapon: ^3%s^7\ntype ^3/adm extraClips ? ^7to see the number of the weapon\n\"", cmd));
	else
		Com_Printf("Invalid Weapon: ^3%s^7\ntype ^3/adm extraClips ? ^7to see the number of the weapon\n");
}
void NV_WeaponOnly	(int argNum, gentity_t *adm, int idnum)
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
		if(adm && adm->client)
		{
			trap_SendServerCommand( adm-g_entities, va("print \"------------------\n\""));
			trap_SendServerCommand( adm-g_entities, va("print \"^3Usage: ^7/adm only <wpn>\n\""));
		}
		else 
		{
			Com_Printf("------------------\n");
			Com_Printf("^3Usage: ^7/only <wpn>\n");
		}
		return;
	}
	if ((Q_stricmp(cmd, "off") == 0) || (Q_stricmp(cmd, "0") == 0)){
		g_onlyWeapons.integer = 0; 				
		trap_Cvar_Set("g_onlyWeapons", "0");
		if(adm && adm->client)
		{
			trap_SendServerCommand( adm-g_entities, va("print \"^3Selected weapons have been disabled\n\""));
		}
		else 
		{
			Com_Printf("^3Selected weapons have been disabled\n");
		}		
		NV_Finish (adm, NULL, "WeaponOnly OFF", FINISH_LOG|FINISH_ADMIN_ONLY);
		return;

	}

	for(i = WP_KNIFE; i < WP_NUM_WEAPONS; i++)
	{
		if (Q_stricmp(bg_weaponNames[i], cmd) == 0 || atoi(cmd) == i)
		{		
			char str[64] = "\0"; 	
			if(g_onlyWeapons.integer & (1<<i))
			{
				g_onlyWeapons.integer &= ~(1<<i); 				
				trap_Cvar_Set("g_onlyWeapons",va("%i", g_onlyWeapons.integer));
				if(adm && adm->client)
				{
					trap_SendServerCommand( adm-g_entities, va("print \"^1%s ^3Disabled\n\"", bg_weaponNames[i]));
				}
				else 
				{
					Com_Printf("^1%s ^3Disabled\n", bg_weaponNames[i]);
				}		
				Com_sprintf(str, sizeof(str), "WeaponOnly %s OFF", bg_weaponNames[i]);	
				NV_Finish (adm, NULL, str, FINISH_LOG|FINISH_ADMIN_ONLY);
			}
			else 
			{
				g_onlyWeapons.integer |= (1<<i);
				trap_Cvar_Set("g_onlyWeapons",va("%i", g_onlyWeapons.integer));
				if(adm && adm->client)
				{
					trap_SendServerCommand( adm-g_entities, va("print \"^1%s ^3Enabled\n\"", bg_weaponNames[i]));
				}
				else 
				{
					Com_Printf("^1%s ^3Enabled\n", bg_weaponNames[i]);
				}	
				Com_sprintf(str, sizeof(str), "WeaponOnly %s ON", bg_weaponNames[i]);	
				NV_Finish (adm, NULL, str, FINISH_LOG|FINISH_ADMIN_ONLY);
			}
		}
	}
}
void NV_ForceWeapons(int argNum, gentity_t *adm, int idnum)
{
	int				i;
	char			cmd[16] = "";

	trap_Argv( argNum, cmd, sizeof( cmd ) );

	if ((Q_stricmp(cmd, "") == 0) || Q_stricmp(cmd, "?") == 0)
	{
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
		}
		else 
		{
			Com_Printf("^3Forced weapons have been disabled\n");
		}
		NV_Finish (adm, NULL, "Weapons: OFF", FINISH_LOG|FINISH_ADMIN_ONLY);
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
			char str[64] = "\0"; 	

			if(g_ForcedItems.integer & (1<<i)){
				g_ForcedItems.integer &= ~(1<<i); 				
				trap_Cvar_Set("g_ForcedItems",va("%i", g_ForcedItems.integer));
				if(adm && adm->client)
				{
					trap_SendServerCommand( adm-g_entities, va("print \"^1%s ^3Disabled\n\"", bg_weaponNames[i]));
				}
				else 
				{
					Com_Printf("^1%s ^3Disabled\n", bg_weaponNames[i]);
				}
				Com_sprintf(str, sizeof(str), "Weapon %s OFF", bg_weaponNames[i]);	
				NV_Finish (adm, NULL, str, FINISH_LOG|FINISH_ADMIN_ONLY);
			}
			else 
			{
				g_ForcedItems.integer |= (1<<i);
				trap_Cvar_Set("g_ForcedItems",va("%i", g_ForcedItems.integer));
				if(adm && adm->client)
				{
					trap_SendServerCommand( adm-g_entities, va("print \"^1%s ^3Enabled\n\"", bg_weaponNames[i]));
				}
				else 
				{
					Com_Printf("^1%s ^3Enabled\n", bg_weaponNames[i]);
				}
				Com_sprintf(str, sizeof(str), "Weapon %s ON", bg_weaponNames[i]);	
				NV_Finish (adm, NULL, str, FINISH_LOG|FINISH_ADMIN_ONLY);	
			}
		}
	}
}
void NV_swapTeams	(int argNum, gentity_t *adm, int idnum)
{
	int		i;
	int		rs = level.teamScores[TEAM_RED];
	int		bs = level.teamScores[TEAM_BLUE];
	int		rl = level.extras.redLocked;
	int		bl = level.extras.blueLocked;
	int		rrw = level.extras.roundWinsRed;
	char	score[2];

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
#ifdef _COMPMODE
	if (!score[0] || g_compMode.integer)
#else
	if (!score[0])
#endif
	{
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
#ifdef _COMPMODE
	if(g_compMode.integer)
	{
		trap_SendServerCommand(-1, va("chat -1 \"^7SERVER: ^3SWAPPING SIDES!\n\""));
	}
#endif

	trap_SendServerCommand( -1, va("cp \"^_**^+Admin Action^_**\n^3Swap Teams\n\"") );
	RPM_GlobalSound(G_SoundIndex("sound/misc/events/tut_lift02.mp3"));
	
	if(idnum != -99)
	{
		NV_Finish (adm, NULL, "Swap Teams", FINISH_LOG|FINISH_ADMIN_ONLY);
	}


	///CalculateRanks();
}
void SC_SwapTeams(gentity_t *adm)
{
	NV_swapTeams(0, NULL, -99);
}
void NV_shuffleTeams(int argNum, gentity_t *adm, int idnum)
{
	int i, j= 0, team = 0;
	int count[TEAM_NUM_TEAMS];
	int	sortClients[MAX_CLIENTS];
	gclient_t *cl;

	if(!level.gametypeData->teams){
		if(adm != NULL) CPa( va("print \"Not playing a team game.\n\"") );
		else  Com_Printf("Not playing a team game.\n");
		return;
	}

	for( i = 0; i < TEAM_NUM_TEAMS; i++ )
		count[i] = 0;

	for( i = 0; i < level.numConnectedClients; i++ ) {
		cl = level.clients + level.sortedClients[ i ];

		if( cl->sess.team != TEAM_RED && cl->sess.team != TEAM_BLUE )
			continue;

		sortClients[j++] = level.sortedClients[i];
	}

	qsort( level.sortedClients, level.numConnectedClients, 
		   sizeof(level.sortedClients[0]), SortRanks );

	for( i = 0; i < j; i++ ) {
		cl = level.clients + sortClients[i];

		team = (i % 2) + TEAM_RED;

		if(cl->sess.team == team)
			continue;

		if (g_entities[level.sortedClients[i]].r.svFlags & SVF_BOT){
			char userinfo[MAX_INFO_STRING];
			trap_GetUserinfo( g_entities[level.sortedClients[i]].s.number, userinfo, sizeof( userinfo ) );
			Info_SetValueForKey( userinfo, "team", team == TEAM_RED?"red":"blue");
			trap_SetUserinfo( g_entities[level.sortedClients[i]].s.number, userinfo );
		}
		cl->sess.team = (team_t)team;

		g_entities[level.sortedClients[i]].client->pers.identity = NULL;
		ClientUserinfoChanged( sortClients[i]  );
		NV_ClientBegin(sortClients[i]);

	}
	level.extras.redLocked = 0; 
	level.extras.blueLocked = 0; 

	/// tell them what happened
	NV_Sound(-1, "sound/misc/events/tut_lift02.mp3" );
	GT(va("%i,@^3Teams ^+have been ^3Shuffled", level.time + 5000));
	
	NV_Finish (adm, NULL, "Shuffle Teams", FINISH_LOG|FINISH_ADMIN_ONLY);
	CalculateRanks();
}
void NV_evenTeams	(int argNum, gentity_t *adm, int idnum)  
{
	int		counts[TEAM_NUM_TEAMS];
	int		diff = 0;
	int		highTeam, i, j, lastConnectedTime;
	int		adminChecked = 1; // Start with 1 (gay refs)
	qboolean	autoeven = qfalse;
	///gentity_t *lastConnected, *ent;
	clientSession_t	*sess;
	///qboolean canBeMoved = qfalse;

	// GodOrDevil - 10.23.05 - 01:45am
	if(level.intermissiontime)
		return;		
	
	if (level.time - level.startTime >= (g_timelimit.integer + level.timeExtension)*60000 )
		return;
	///End  - 10.23.05 - 01:45am
	if(idnum == 99)
		autoeven = qtrue;
	else 
		autoeven = qfalse;

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
		
#ifdef _TORTURE
			if(sess->torture)
				continue;
#endif
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

#ifdef _SOF2_BOTS
		if (lastConnected->r.svFlags & SVF_BOT)	{
			char	userinfo[MAX_INFO_STRING];
			trap_GetUserinfo( lastConnected->s.number, userinfo, sizeof( userinfo ) );
			Info_SetValueForKey( userinfo, "team", lastConnected->client->sess.team == TEAM_RED?"red":"blue");
			trap_SetUserinfo( lastConnected->s.number, userinfo );
		}
#endif

		///01.24.06 - 07:43pm
		lastConnected->client->pers.identity = NULL;
		ClientUserinfoChanged( lastConnected->s.number );		
		CalculateRanks();

		G_StopFollowing( lastConnected );
		G_StopGhosting( lastConnected );
		trap_UnlinkEntity ( lastConnected );
		//ClientSpawn( lastConnected);
		NV_ClientBegin( lastConnected->s.number);
		///End  - 01.24.06 - 07:43pm
	}

	/// tell them what happened
	trap_SendServerCommand( -1, va("cp \"^-** ^3Evening Teams ^-**\n\"") );
	RPM_GlobalSound(G_SoundIndex("sound/misc/events/tut_lift02.mp3"));
			
	if(autoeven)
		return;
	NV_Finish (adm, NULL, "Even Teams", FINISH_LOG|FINISH_ADMIN_ONLY);
}
void NV_lockTeam	(int argNum, gentity_t *adm, int idnum)
{
	char		*a="", *teamName = "", *logName = "", team[2], str[32] = "\0";

	trap_Argv( argNum, team, sizeof( team ) );

	if ( !level.gametypeData->teams ){
		CPa(va("print \"Not playing a team game!\n\""));
		return;
	}

	if(adm != NULL)
	{
		if (adm->client->sess.team != TEAM_RED && adm->client->sess.team != TEAM_BLUE &&
			!(adm->client->nvSess.referee || adm->client->nvSess.admin) ){
				CPa(va("print \"^+You are ^3NOT ^+allowed to Lock or Unlock the ^3Spectators\n\""));
				return;
		}
	}
	if(!(adm != NULL) || adm->client->nvSess.referee || adm->client->nvSess.admin ) {
		if (team[0] == 'r' || team[0] == 'R'){
			if( level.extras.redLocked ) {
				level.extras.redLocked = qfalse; 
				a = "UN";
			}
			else level.extras.redLocked = qtrue; 
			teamName = "^1RED ^7Team";
			logName = "Red Team";
		}
		else if(team[0] == 'b'|| team[0] == 'B') {
			if( level.extras.blueLocked  ) {
				level.extras.blueLocked = qfalse;
				a = "UN";
			}
			else level.extras.blueLocked = 1; 
			teamName = "^4BLUE ^7Team";
			logName = "Blue Team";
		}
		else if(team[0] == 's' || team[0] == 'S'){
			if( level.extras.specsLocked ) {
				level.extras.specsLocked  = qfalse;
				a = "UN";
			}
			else level.extras.specsLocked = qfalse; 
			teamName = "^3SPECTATORS";
			logName = "Spectators";
		}		
		else {
			CPa(va("print \"Usage: lock/unlock <team>\n\""));
			return;
		}
	}	

	GT(va("%i,@^+The %s ^+has been ^3%sLOCKED", level.time + 5000, teamName, a ));
		
	Com_sprintf(str, sizeof(str), "%sLOCKED %s", a, logName);
	NV_Finish (adm, NULL, str, FINISH_LOG|FINISH_ADMIN_ONLY);
}
void NV_VotePass		(int argNum, gentity_t *adm, int idnum) 
{
	if ( !level.voteTime )
	{	
		if(adm && adm != NULL)
			trap_SendServerCommand( adm-g_entities, "print \"No vote in progress.\n\"" );
		else
			Com_Printf("^5No vote in progress!\n");
		return;
	}
	level.voteYes = level.numVotingClients / 2 + 1;
	NV_Finish (adm, NULL, "Vote Passed", FINISH_LOG|FINISH_ADMIN_ONLY);	
}
void NV_VoteCancel	(int argNum, gentity_t *adm, int idnum) 
{	
	if ( !level.voteTime )
	{	
		if(adm && adm != NULL)
			trap_SendServerCommand( adm-g_entities, "print \"No vote in progress.\n\"" );
		else
			Com_Printf("^5No vote in progress!\n");
		return;
	}
	level.voteTime = 0;
	trap_SetConfigstring( CS_VOTE_TIME, "" );	
	NV_Finish (adm, NULL, "Vote Cancelled", FINISH_LOG|FINISH_ADMIN_ONLY);
}
void NV_Strip		(int argNum, gentity_t *adm, int idnum)  
{
	gentity_t		*ent;
	int	idle;

	if(!NV_canUseCommand(adm, idnum, "Strip", qfalse)) return;
	ent = &g_entities[idnum];

	ent->client->ps.zoomFov = 0;
	ent->client->ps.pm_flags &= ~(PMF_GOGGLES_ON|PMF_ZOOM_FLAGS);
	ent->client->ps.stats[STAT_WEAPONS] = 0;
	ent->client->ps.stats[STAT_GOGGLES] = GOGGLES_NONE; 
	TossClientItems( ent );
	memset ( ent->client->ps.ammo, 0, sizeof( ent->client->ps.ammo) );
	memset (  ent->client->ps.clip, 0, sizeof( ent->client->ps.clip) );

	///Com_Printf("%02f %02f %02f\n", ent->client->ps.origin[0], ent->client->ps.origin[1], ent->client->ps.origin[2]);

//#ifdef _DEBUG
	ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_NONE );
	ent->client->ps.clip[ATTACK_NORMAL][WP_NONE]=weaponData[WP_NONE].attack[ATTACK_NORMAL].clipSize;
	ent->client->ps.firemode[WP_NONE] = BG_FindFireMode ( WP_NONE, ATTACK_NORMAL, WP_FIREMODE_AUTO );
	ent->client->ps.weapon = WP_NONE;
	BG_GetInviewAnim(ent->client->ps.weapon,"idle",&idle);
	ent->client->ps.weaponAnimId = idle;
//#else
//	ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_KNIFE );
//	ent->client->ps.clip[ATTACK_NORMAL][WP_KNIFE]=weaponData[WP_KNIFE].attack[ATTACK_NORMAL].clipSize;
//	ent->client->ps.firemode[WP_KNIFE] = BG_FindFireMode ( WP_KNIFE, ATTACK_NORMAL, WP_FIREMODE_AUTO );
//	ent->client->ps.weapon = WP_KNIFE;
//	//ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_M84_GRENADE );
//	//ent->client->ps.clip[ATTACK_NORMAL][WP_M84_GRENADE]=weaponData[WP_M84_GRENADE].attack[ATTACK_NORMAL].clipSize + 10;
//	//ent->client->ps.firemode[WP_M84_GRENADE] = BG_FindFireMode ( WP_M84_GRENADE, ATTACK_NORMAL, WP_FIREMODE_AUTO );
//	//ent->client->ps.weapon = WP_M84_GRENADE;
//
//	BG_GetInviewAnim(ent->client->ps.weapon,"idle",&idle);
//	ent->client->ps.weaponAnimId = idle;
//#endif
	//ent->client->ps.stats[STAT_WEAPONS] |= ( 1 << WP_NONE );
	//ent->client->ps.weapon = WP_NONE;

	ent->client->nvSess.stripped = qtrue;

	/// Announce what happened
	NV_Finish (adm, ent, "Stripped", FINISH_LOG|FINISH_PRINT);	
	NV_Sound(idnum, "sound/misc/menus/apply_changes" );
}
void NV_Runover		(int argNum, gentity_t *adm, int idnum)  
{
	gentity_t	*ent;
	vec3_t		fwd, right, up, fireAngs;
	int			anim = 0;

	if(!NV_canUseCommand(adm, idnum, "Runover", qfalse)) return;
	ent = &g_entities[idnum];

	VectorCopy(ent->client->ps.viewangles, fireAngs);
	AngleVectors( fireAngs, fwd, right, up);
	ent->client->ps.velocity[2] += 200;
	VectorMA ( ent->client->ps.velocity, -600, fwd, ent->client->ps.velocity );

	ent->client->ps.weaponstate = WEAPON_FIRING;
	//ent->client->ps.firemode[ent->s.weapon] = WP_FIREMODE_SINGLE;
	ent->client->ps.pm_flags &= ~PMF_JUMPING;

	anim = LEGS_RUN + (rand()%LEGS_LEANRIGHT_CROUCH_WALKRIGHT - LEGS_RUN);
	ent->client->ps.legsAnim = 
		( ( ent->client->ps.legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
	
	anim = ANIM_START_DEATHS + (rand()%29);
	ent->client->ps.torsoAnim = 
		( ( ent->client->ps.torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
	ent->client->ps.weaponTime = 3000;
	
	NV_Sound(idnum, "sound/ambience/vehicles/hit_scrape.mp3" );
#ifdef _TORTURE
	if(ent->client->nvSess.torture)
	{
		G_Damage (ent, NULL, NULL, NULL, NULL, 10, 0, MOD_CAR, HL_NONE );
		return;
	}
#endif
	G_Damage (ent, NULL, NULL, NULL, NULL, 20, 0, MOD_ADMIN, HL_NONE );
	NV_Finish (adm, ent, "Runover", FINISH_LOG|FINISH_PRINT);	
}
#ifdef _TORTURE
void NV_Burn		(int argNum, gentity_t *adm, int idnum)  
{
	if(!NV_canUseCommand(adm, idnum, "Burn", qfalse)) return;
	NV_clusterNade( g_entities + idnum, WP_ANM14_GRENADE, 1, 20 );
	NV_Finish (adm, g_entities + idnum, "Burned", FINISH_LOG|FINISH_PRINT);	
}
#endif
void NV_Blast   	(int argNum, gentity_t *adm, int idnum)  
{
	if(!NV_canUseCommand(adm, idnum, "Blasted", qfalse)) 
		return;
 	NV_clusterNade( g_entities + idnum, WP_SMOHG92_GRENADE, 6, 99 );
	NV_Finish (adm, g_entities + idnum, "Blasted", FINISH_LOG|FINISH_PRINT);	
}
void NV_Flash		(int argNum, gentity_t *adm, int idnum)  
{
	if(!NV_canUseCommand(adm, idnum, "Flash", qfalse)) return;
	NV_clusterNade( g_entities + idnum, WP_M84_GRENADE, 1, 0 );
	NV_Finish (adm, g_entities + idnum, "Flashed", FINISH_LOG|FINISH_ADMIN_ONLY);	
}
void NV_Plant		(int argNum, gentity_t *adm, int idnum)  
{
	gentity_t *ent;
	char	b[2] = "", *a = ""; 
	team_t	team;
	gspawn_t  *spawnPoint;	
	
	if(!NV_canUseCommand(adm, idnum, "Plant", qfalse)) return;
	ent = &g_entities[idnum];

	trap_Argv( argNum + 1, b, sizeof( b ) );

	if (b[0]) // bait
	{
		///Strip them
		ent->client->ps.pm_flags &= ~(PMF_ZOOM_FLAGS);
		memset ( ent->client->ps.ammo, 0, sizeof(ent->client->ps.ammo) );
		memset ( ent->client->ps.clip, 1, sizeof(ent->client->ps.clip) );
		ent->client->ps.stats[STAT_WEAPONS] = 0;
		ent->client->ps.weaponstate = WEAPON_DROPPING;

		/// Get opposite team
		if(ent->client->sess.team == TEAM_RED)
			team = TEAM_BLUE;
		else if (ent->client->sess.team == TEAM_BLUE)
			team = TEAM_RED;
		else team = TEAM_FREE;
		
		///Get a safe spawn to plant them in
		spawnPoint = G_SelectRandomSafeSpawnPoint ( team, 1500 );

		///move them to the new spawn origin then set the view direction
		VectorCopy( spawnPoint->origin, ent->client->ps.origin );
		SetClientViewAngle( ent, spawnPoint->angles );
	}

	if(ent->client->pers.planted && !b[0]) {
		ent->client->ps.origin[2] += 65;
		VectorCopy( ent->client->ps.origin, ent->s.origin );
		ent->client->pers.planted = qfalse;
#ifdef _TORTURE
		if(ent->client->nvSess.torture) 
			G_Damage (ent, NULL, NULL, NULL, NULL, 5, 0, MOD_ADMIN, HL_NONE );
		else 
#endif
			G_Damage (ent, NULL, NULL, NULL, NULL, 70, 0, MOD_ADMIN, HL_NONE );
		a = "UNPLANTED";
	}
	else {
		if ( ent->client->ps.pm_flags & PMF_DUCKED )
			ent->client->ps.origin[2] -=40;
		else
			ent->client->ps.origin[2] -= 65;
		
		VectorCopy( ent->client->ps.origin, ent->s.origin );
		ent->client->pers.planted = qtrue;
		a = "PLANTED";
	}

	NV_Finish (adm, ent, a, FINISH_LOG|FINISH_PRINT);	
	NV_Sound(idnum, "sound/misc/confused/wood_break.mp3" );
}
#ifdef _TORTURE
void NV_Slam		(int argNum, gentity_t *adm, int idnum)
{
	gentity_t		*ent;

	if(!NV_canUseCommand(adm, idnum, "Slam", qfalse)) return;

	ent = &g_entities[idnum];
	
	ent->client->ps.pm_debounce |= PMD_JUMP;
	ent->client->ps.pm_flags |= PMF_JUMPING;

	ent->client->ps.groundEntityNum = ENTITYNUM_NONE;
	ent->client->ps.velocity[2] = 2000;

	ent->client->nvSess.drop = qtrue;

	if(ent->client->nvSess.torture)
		return;

	/// Dont know why I added this. Not deleting incase something fucks up might be for OLD 'drop'
	///if(ent->client->pers.camped){
	///	ent->client->pers.camped = qfalse;
	///	return;
	///}
	
	NV_Sound(idnum, "sound/ambience/vehicles/telephone_pole.mp3" );
	NV_Finish (adm, ent, "Slammed", FINISH_LOG|FINISH_PRINT);	
}
#endif
void NV_Kick		(int argNum, gentity_t *adm, int idnum)  
{
	char	reason[MAX_STRING_TOKENS];
	if(!NV_canUseCommand(adm, idnum, "", qfalse)) return;
	trap_Argv( argNum + 2, reason, sizeof( reason ) );
	NV_Finish (adm, g_entities + idnum, "Kicked", FINISH_LOG|FINISH_PRINT);	
	trap_SendConsoleCommand( EXEC_INSERT, va("clientkick \"%d\" \"%s\"\n", idnum, reason));
}
void NV_PBOX		(int argNum, gentity_t *adm, int idnum)  
{
	gentity_t *ent;
	int		seconds; 
	char	time[4];
	char	reason[40];
	
	if(!NV_canUseCommand(adm, idnum, "PBox", qfalse)) return;
	ent = &g_entities[idnum];

	/// Keeps Player from setting a pbox time to a lower value, so they dont cheat.
	if(ent->client->sess.penaltySeconds)
	{
		if(adm && adm->client)	
		{
			trap_SendServerCommand( adm-g_entities, va("print \"^3%s is already in the Penalty Box\n\"", ent->client->pers.netname));
		} 
		else
		{
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
	//trap_SendServerCommand( -1, va("print \"^3%s has been thrown into the Penalty Box.\n\"", ent->client->pers.netname));		
	
	NV_Finish (adm, g_entities + idnum, "Penalized", FINISH_LOG|FINISH_PRINT);	
}

#ifdef _DB_ENABLED
void NV_getIPAlias(int argNum, gentity_t *adm, int idnum)
{
	gentity_t		*ent;
	char update[64] = "\0";

	if(!g_enableDatabase.integer)
		return;

	if(adm == NULL)
	{
		Com_Printf("You must be a client to use this command\n");
		return;
	}

	if(!NV_canUseCommand(adm, idnum, "", qtrue)) 
		return;

	ent = &g_entities[idnum];
	
	Com_sprintf(update, sizeof(update), "%i;%s;%s;", adm->s.number, ent->client->nvSess.ip2, ent->client->nvSess.cleanName);
	NV_QueryDB(DB_LITE_GETALIASES, update);

}
#endif

void NV_ViewBanList (int argNum, gentity_t *adm, int idnum)
{
#ifdef _DB_ENABLED
	if(g_enableDatabase.integer == 2)
	{
		char update[10] = "\0";

		if(adm == NULL)
		{
			Com_Printf("You must be a client to use this command\n");
			return;
		}		
		Com_sprintf(update, sizeof(update), "%i", adm->s.number);
		NV_QueryDB(DB_LITE_GETBANS, update);
		return;
	}
#endif
	trap_SendServerCommand( adm-g_entities, va("print \"[^3Banlist^7]\n\""));
	NV_Print_File( adm, g_banlist.string);
}
#ifdef _DEBUG
void NV_killAll	(int argNum, gentity_t *adm, int idnum)   
{
	int i, c = 0;
	gentity_t *ent;
	
	while(c < 2) // Doesnt kill everyone the first time around.
	{
		for(i = 0; i < level.numConnectedClients; i++)
		{
			ent = &g_entities[level.sortedClients[i]];

			if (G_IsClientSpectating(ent->client))
				continue;
			if (ent->client->nvSess.vip == 1)
				continue;
			if(adm != NULL && adm == ent)
				continue;
			G_Damage (ent, NULL, NULL, NULL, NULL, 10000, 0, MOD_ADMIN, HL_NONE);
		}
		i = 0;
		c++;
	}	
}
#endif


void NV_PrintAdminCommands (int argNum, gentity_t *adm, int idnum);
typedef struct {
	const char	*cmd;
	void		(*funcName)(int argNum, gentity_t *adm, int idnum );
	const char	*usage;
	const char	*help;
} commands_t;

static /*const*/ commands_t cmdInfo[] = {
	///console cmd		function        usage				help info
	{"pop",				NV_Pop,			"#id",				"Kills a player by dismembering them"					},	                     				
	{"mute",			NV_Mute,		"#id",				"Mutes or unmutes the player"							},              	
	{"strip",			NV_Strip,	 	"#id",				"Removes all but 1 knife from a player"			        },          		
	{"runover",			NV_Runover,		"#id", 				"Injuries the player by simulating them being hit by a car"},     			
	{"slow",			NV_Slow,	    "#id <speed>", 		"Reduces/Restores a players speed"                      },             
	{"twist",			NV_Twist,	  	"#id",				"Twist a player like a pretzel (0 will untwist)"		},                           
	{"plant",			NV_Plant,	   	"#id <bait>",       "Plants/UnPlants a player in the ground. Use an extra argument to bait them (ex. adm plant #id b)"},         	
	{"blast",			NV_Blast,	   	"#id",				"Blasts a player with explosives"						},         	
	{"launch",			NV_UpperCut,   	"#id",				"Knocks a player into the air"							},         	
	{"flash",			NV_Flash,   	"#id",				"Uses a flash grenade on the player"					},         	
	{"pbox" ,			NV_PBOX,		"#id <time> <reason>","Sends a player to the penalty box"					},                      		
	{"kick" ,			NV_Kick,	    "#id <reason>",		"Kicks the player from the server"						},
	{"forcesay",		NV_Forcesay,	"#id <message>",	"Send a message as another player"						},
	{"rename" ,    		NV_Rename,		"#id <name> <unlock>","Changes a players name and locks it"					},
	{"move",			NV_Team,		"#id <team> <unlock>", "Forces a player onto a team. (opt. 1 to unlock)"	},
#ifdef _TORTURE
	{ "burn", NV_Burn, "#id", "Sets a player on fire, draining their health" },
	{ "slam", NV_Slam, "#id", "Slams a player into the ground" },
	{ "torture", NV_Torture, "#id <reason> <ban>", "Abuses a player non-stop player (issue the command again to end it)" },
#endif
	{"eventeams",		NV_evenTeams,	"",					"Balances out the teams by player count"				},
	{"swapteams",		NV_swapTeams,  	"",					"Moves all players to the opposite team"				},
	{"lock",			NV_lockTeam,	"team",				"Locks/Unlocks a team"									},
	{"shuffle",			NV_shuffleTeams,"",					"Attempts to even the teams by player scores"	        },
	{"nextmap",			NV_nextMap, 	"map <gt> <score>",	"Choose next map w/out ending the currnet one (gametype and score are optional)"},
#ifdef _DB_ENABLED
	{"alias",			NV_getIPAlias,	"#id",				"Gets a list of aliases used by a player"				},
#endif
	{"addSadmin",		NV_addSAdmin, 	"#id",				"Grants Server Admin powers to a player."				},
#ifdef _RECRUITER
	{"addRadmin",		NV_addRAdmin, 	"#id",				"Grants Recruiter powers to a player."					},
#endif
	{"addadmin",		NV_addAdmin, 	"#id",				"Grants Admin powers to a player."						},
	{"addBadmin",		NV_addBAdmin, 	"#id",				"Grants Basic Admin Powers to a player."				},
	{"removeadmin",		NV_removeAdmin,	"#id",				"Removes all levels of admin status from a player"		},
	{"ban",				NV_Ban,			"#id <time><reason>","Kicks the player and bans them from the server"		},
	{"unban",     		NV_Unban,	 	"<ip or id>",        "Removes a ban by IP or stats ID"						},
	{"banlist",    		NV_ViewBanList,	 "",				 "View the list of banned players on the server"		},
	{"passvote",		NV_VotePass,	"",					"Passes an active vote"								},
	{"cancelvote",		NV_VoteCancel,	"",					"Cancels  an active vote"								},
	{"weapForce",		NV_ForceWeapons,"<weapon> <value>", "Adds extra weapons to all players. Value: 1:ON, 0:OFF, 2+:Extra Ammo (# times clipsize)"},
	{"weapOnly",		NV_WeaponOnly,  "<weapon>",			"Only enable the selected weapon(s)"},
	{"clipsize",		NV_ClipSize,    "<weapon> <value>", "Change a weapons clip size"},
	{"extraclips",		NV_ExtraClips,  "<weapon> <value>", "Change how many extra clips a weapon has."},
#ifdef _COMPMODE
//	{"pause",			NV_Pause,		"",					"Pauses/Unpauses the game"								},
#endif
#ifdef _DEBUG
	{"list",			NV_PrintAdminCommands,	"<text>",	"show List of commands for condump"},
	{"killall",			NV_killAll,				"<text>",	"Kill all alive players"},
//	{"newName",			NV_changeClientDBName,				"<old name> <new name>",	"Renames a client in the DB"},
#endif
	{ "",				0,			"",					""}
};

#ifdef _DEBUG
void NV_PrintAdminCommands (int argNum, gentity_t *adm, int idnum) 
{
	int i;
	char	*restricted = "\0";
	char	a[2] = "\0";
	
	restricted = g_admCmds.string;

	for ( i = 0 ; i < sizeof( cmdInfo ) / sizeof( cmdInfo[0] ) ; i++ ) 
	{	
		a[0] = restricted[i];
		if(a[0] == '0') a[0] = 'N';
		if(a[0] == '1') a[0] = 'B';
		if(a[0] == '2') a[0] = 'B';
		if(a[0] == '3') a[0] = 'A';
		if(a[0] == '4') a[0] = 'S';
		if(a[0] == '5') a[0] = 'R';

		if(adm != NULL)
			CPa(va("print \"@ %s ^3%s: ^7%-22s\n", a, cmdInfo[i].cmd, cmdInfo[i].help ));
		else
			Com_Printf("%s ^3%-18s ^7%s\n",  a, cmdInfo[i].cmd, cmdInfo[i].help);
	}		
	//trap_SendConsoleCommand( EXEC_APPEND, va("condump \"adminCommands.doc\""));
}
#endif
qboolean NV_canUseCommand(gentity_t *adm, int idnum, const char* useDead, qboolean useOnAdmin)
{
	if ( idnum < 0 || idnum >= MAX_CLIENTS || !g_entities[idnum].client ){
		if(adm != NULL)	CPa(va("print \"^3Invalid client number: %i.\n\"", idnum));
		else Com_Printf("^3Invalid client number %i\n", idnum );
		return qfalse;
	}
	///is the client connected
	if ( g_entities[idnum].client->pers.connected == CON_DISCONNECTED ) {
		if(adm != NULL)	CPa(va("print \"^3No client with id %i.\n\"", idnum));
		else Com_Printf("^3There is no client with id %i.\n", idnum);
		return qfalse;
	}

	if(adm != NULL) 
	{
		if(g_entities[idnum].client->nvSess.admin)
		{
			///Cant it be used on Admins
			if(!useOnAdmin)
			{
				if(idnum == adm->s.number )
				{
					CPa(va("print \"^3You cannot use this command on yourself!\n\""));
					return qfalse;
				}
				else if (g_entities[idnum].client->nvSess.admin >= adm->client->nvSess.admin && !adm->client->nvSess.referee)
				{
					CPa(va("print \"^3You cannot use this command on high-level Admins!\n\""));
					return qfalse;
				}
				//else {
				//	CPa(va("print \"^3You cannot use this command on Admins!\n\""));
				//	return 0;
				//}
			}
			else
			{
				if (g_entities[idnum].client->nvSess.admin > adm->client->nvSess.admin && !adm->client->nvSess.referee)
				{
					CPa(va("print \"^3You cannot use this command on high-level Admins!\n\""));
					return qfalse;
				}
			}
		}
		///The admin cant use commands in pbox
		else if (adm->client->sess.penaltySeconds)
		{
			CPa(va("print \"^3You cannot use Admin commands while in the Penalty Box!\n\""));	
			return qfalse;
		}
	}
	if(strcmp(useDead,""))
	{
		if ( G_IsClientDead ( g_entities[idnum].client ) )
		{
			if(adm != NULL) CPa(va("print \"^3You cannot %s dead Players.\n\"", useDead));
			else Com_Printf("^3You cannot %s dead Players.\n", useDead);
			return qfalse;
		}
		if (G_IsClientSpectating(g_entities[idnum].client)) 
		{
			if(adm != NULL) CPa(va("print \"^3You cannot %s a Spectator.\n\"", useDead));
			else Com_Printf("^3You cannot %s a Spectator.\n", useDead);
			return qfalse;
		}
	}
#ifdef _DB_ENABLED
	if(g_enableDatabase.integer && (!g_entities[idnum].client->nvSess.dbID || !g_entities[idnum].client->nvSess.dbnameID))
	{
		if (adm && adm->client)
		{
			trap_SendServerCommand(adm - g_entities, va("print \"^7Please wait until ^3%s ^7client fully connects.\n\"", g_entities[idnum].client->pers.netname));
		}
		else 
		{
			Com_Printf("^7Please wait until ^3%s ^7client fully connects.\n", g_entities[idnum].client->pers.netname);
		}
		return qfalse;
	}
#endif
	return qtrue;
}
qboolean NV_admCmds	(gentity_t *adm, char *cmd)
{	
	int i, idnum = -1, b = adm->client->nvSess.admin, a;
	char *restricted = g_admCmds.string, x[2] = "\0";
	char arg[MAX_SAY_TEXT] = "\0";
	int argNum = 2;
	
	///see if 1st arg is a status chat cmd
	trap_Argv( 1, arg, sizeof(arg));
	if(!Q_stricmp(arg, "chat") || !Q_stricmp(arg, "talk") /*|| !Q_stricmp(arg, "schat")*/){
		trap_Argv( argNum+=1, arg, sizeof(arg));
	}
	else 
		trap_Argv( argNum, arg, sizeof(arg));
	
	if(!Q_stricmp(arg, "f") && adm->client->sess.spectatorState == SPECTATOR_FOLLOW )
		idnum = adm->client->sess.spectatorClient; ///use command on client they are following
	else if (arg[0] >= '0' && arg[0] <= '9')	
		idnum = atoi( arg );
	else 
		idnum = -1;

	for ( i = 0 ; i < sizeof( cmdInfo ) / sizeof( cmdInfo[0] ) ; i++ )
	{
		if(!Q_stricmp(cmd,  cmdInfo[i].cmd) && cmdInfo[i].funcName != NULL)
		{	
			x[0] = restricted[i];
			x[1] = '\0';
			if (x[0] > '0' && x[0] <= '9')	
				a = atoi( &x[0] );
			else 
				return qtrue;

			if(b >= a)
			{
				if(!Q_stricmp(arg, "?"))
				{
					CPa(va("print \"@ ^3%s: ^7%s\n\n", cmdInfo[i].cmd, cmdInfo[i].help ));
					return qfalse;
				}
				cmdInfo[i].funcName(argNum, adm, idnum);
			}
			return qfalse;
		}
	}
	return qtrue;
}
qboolean NV_rconCmds(char *cmd)
{
	int			 i, idnum = -1;
	char		 arg[3];

	trap_Argv( 1, arg, sizeof(arg));
	if (arg[0] >= '0' && arg[0] <= '9')	
		idnum = atoi( arg );
	else 
		idnum = -1;

	for ( i = 0 ; i < sizeof( cmdInfo ) / sizeof( cmdInfo[0] ) ; i++ )
	{
		if(!Q_stricmp(cmd,  cmdInfo[i].cmd) && cmdInfo[i].funcName != NULL)
		{	
			if(!Q_stricmp(arg, "?"))
			{
				Com_Printf(" ^3%s: ^7%s\n\n", cmdInfo[i].cmd, cmdInfo[i].help );
				return qfalse;
			}
			cmdInfo[i].funcName(1, NULL, idnum);
			return qfalse;
		}
	}
	return qtrue;
}

void NV_setRestriction(char *cmd)
{
	int			i;
	char		arg[2];
	char		string[64] = "\0";

	trap_Argv( 2, arg, sizeof(arg));

	if (arg[0] < '0' || arg[0] > '9')
	{
		Com_Printf("^3Error: ^7Use only single digit numbers\n^3Usage^7: /admcmds <^4cmd^7> <^4admin level^7>\n[  ^30 ^7or ^31^7 = All Admins   ^32^7 = Admin+   ^33^7 = S-Admin only   ^34+ ^7= All Denied   ]");
		return;
	}

	for ( i = 0; i < sizeof(cmdInfo)/sizeof(cmdInfo[0]); i++ ) 
	{
		if(!Q_stricmp(cmd, cmdInfo[i].cmd))
		{
			Com_sprintf( string, sizeof(string), "%s", g_admCmds.string );
			string[i] = arg[0];
			trap_Cvar_Set("g_admCmds",	 string);			

			switch(arg[0])
			{
			case '1':
			case '2':
				Com_Printf("^3Admin Restriction updated: ^7%s [%s] - [B-ADMIN+]\n", cmdInfo[i].cmd, arg);
				return;
			case '3':
				Com_Printf("^3Admin Restriction updated: ^7%s [%s] - [ADMIN+]\n", cmdInfo[i].cmd, arg);
				return;
			case '4':
				Com_Printf("^3Admin Restriction updated: ^7%s [%s] - [S-ADMIN]\n", cmdInfo[i].cmd, arg);
				return;
			case '5':
				Com_Printf("^3Admin Restriction updated: ^7%s [%s] - [RECRUITER]\n", cmdInfo[i].cmd, arg);
				return;
			default:
				Com_Printf("^3Admin Restriction updated: ^7%s [%s] - [UNAVAILABLE]\n", cmdInfo[i].cmd, arg);
				return;
			}
		}		
	}
	return;
}
void NV_cmdList		(gentity_t *adm)
{
	int		i = 0, b = adm->client->nvSess.admin;
	char	*restricted = "\0";
	char	a[2] = "\0";
	int		len = 0;
	char	string[1024] = "";
	char	*line;

	restricted = g_admCmds.string;
	CPa(va("print \"@\n[^3%-12s^3%-22s^7][^3%s^7]\n\"", "Commands","Arguments","L"));
	CPa(va("print \"@---------------------------------------\n\""));

	for( i = 0; i < sizeof(cmdInfo)/sizeof(cmdInfo[0]) - 1; i++) 
	{
		a[0] = restricted[i];

		if(b < atoi(a) || atoi(a) <= 0 || atoi(a) > 9)
			continue;

		line = va("^7[^5%-12s^7%-22s^7][^3%s^7]\n", cmdInfo[i].cmd, cmdInfo[i].usage, a);

		strncpy( &string[len], line, 128 );
		len += strlen( line );

		if ( len > 750 ){
			string[len++] = 0;
			CPa(va("print \"@%s\"", string));
			len = 0;
		}
	}
	if ( len ){
		string[len++] = 0;
		CPa(va("print \"@%s\"", string));
	}
	if(adm->client->nvSess.admin){
		CPa(va("print \"@------------Extra Commands-------------\n\""));
		NV_extraCommands (adm, "showExtra");
	}

	CPa(va("print \"@\nNOTE: Anything between ^3< >^7 is ^1OPTIONAL^7!\nTo see more info on command type a '?' after the cmd (ex. /adm pop ?)\n\n\""));
}
void NV_cmdsAreAllowed (gentity_t *adm, char *arg)
{		
	char	arg2[MAX_TOKEN_CHARS];

	if (!adm || !adm->client)
		return;

	if(adm != NULL)	
		trap_Argv( 2, arg2, sizeof( arg2 ));
	else 
		trap_Argv( 3, arg2, sizeof( arg2 ));

	if (adm && adm->client && !adm->client->nvSess.admin)
	{ 
		CPa( va("print \"YOU ARE NOT AN ADMIN!\n\"" ) );
		return;
	}

#ifdef _COMPMODE
	if((Q_stricmp (arg, "mode") && Q_stricmp (arg2, "mode")) && g_compMode.integer)
	{
		//if(!nv_enableAdmins.integer && adm->client->nvSess.admin )
		//{
		//	//if(adm->client->sess.team == TEAM_SPECTATOR){
		//		CPa(va("print \"^6Admins's ^7are ^1DISABLED ^7in ^3Competition Mode\n"""));
		//		return;
		//	//}
		//}
		//else
		if (adm->client->sess.team != TEAM_SPECTATOR)
		{
			CPa(va("print \"YOUR ^1POWERS ^7ARE ^3SUSPENDED ^7WHILE ^3INGAME^7!!\n"""));
			return;
		}
	}
#endif
	if (!Q_stricmp (arg, "") || !Q_stricmp (arg, "?"))
	{
		NV_cmdList (adm);
		return;
	}	
	
	if(!Q_stricmp (arg, "chat"))
	{
		Cmd_Say_f (adm, ADM_CHAT, qtrue);
		return;
	}
	if(!Q_stricmp (arg, "talk"))
	{
		Cmd_Say_f (adm, ADM_TALK, qtrue);
		return;
	}
	if(!Q_stricmp (arg, "schat") && adm->client->nvSess.admin == 3)
	{
		Cmd_Say_f (adm, SADM_CHAT, qtrue);
		return;
	}
	
	if(!NV_admCmds(adm, arg))
	{
		return;
	}
	if(strcmp(arg, "") && adm->client->nvSess.admin && NV_extraCommands(adm, arg))
	{
		return;
	}
	if (!arg2[0] || !Q_stricmp (arg2, " "))
	{
		CPa(va("print \"^3Unknown or Restricted Command\n\"" ));
	}
}

#endif

