
#include "../g_local.h"

#ifdef _NEW_VOTING
qboolean NV_canCallVote (gentity_t *ent)
{	
	if ( !g_allowVote.integer ) 
	{
		trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed here.\n\"" );
		return qfalse;
	}

	if ( level.intermissiontime || level.intermissionQueued )
	{
		trap_SendServerCommand( ent-g_entities, "print \"Voting not allowed during intermission.\n\"" );
		return qfalse;
	}
	if ( level.voteTime || level.voteExecuteTime ) 
	{
		trap_SendServerCommand( ent-g_entities, "print \"A vote is already in progress.\n\"" );
		return qfalse;
	}	

	if(!ent->client->nvSess.admin)
	{
		if ( level.time - level.startTime < 1000 * 30)
		{
			trap_SendServerCommand( ent-g_entities, "print \"Cannot vote within the first 30 seconds of a map change.\n\"" );
			return qfalse;
		}
#ifndef _DEBUG
		if ( level.numConnectedClients > 1 && level.numVotingClients >= 2) 
		{
			trap_SendServerCommand( ent-g_entities, "print \"You need at least 2 clients to call a vote.\n\"" );
			return qfalse;
		}
#endif

		if ( ent->client->pers.voteCount >= MAX_VOTE_COUNT ) 
		{
			trap_SendServerCommand( ent-g_entities, "print \"You have called the maximum number of votes.\n\"" );
			return qfalse;
		}

		if ( ent->client->sess.team == TEAM_SPECTATOR) 
		{
			trap_SendServerCommand( ent-g_entities, "print \"Not allowed to call a vote as spectator.\n\"" );
			return qfalse;
		}

#ifndef _DEBUG

		if ( ent->client->voteDelayTime > level.time )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"You are not allowed to vote within %d minute of a failed vote.\n\"", g_failedVoteDelay.integer ) );
			return qfalse;		
		}
#endif

	}
	return qtrue;
}

qboolean NV_ClientNameNeeded(gentity_t *ent, char *arg1, char* arg2)
{
	if ( !Q_stricmp ( arg1, "mute" )  
		|| !Q_stricmp ( arg1, "kick" )
		|| !Q_stricmp ( arg1, "ban" )
		|| !Q_stricmp ( arg1, "pop" )
		|| !Q_stricmp ( arg1, "torture" )
		|| !Q_stricmp ( arg1, "runover" )
		|| !Q_stricmp ( arg1, "strip" )
		|| !Q_stricmp ( arg1, "slow" )
		|| !Q_stricmp ( arg1, "twist" )
		|| !Q_stricmp ( arg1, "burn" )
		|| !Q_stricmp ( arg1, "slam" )
		|| !Q_stricmp ( arg1, "plant" )
		|| !Q_stricmp ( arg1, "blast" )
		|| !Q_stricmp ( arg1, "launch" )
		|| !Q_stricmp ( arg1, "flash" )
		|| !Q_stricmp ( arg1, "pbox" )
		|| !Q_stricmp ( arg1, "forcesay" )
		|| !Q_stricmp ( arg1, "rename" )
		|| !Q_stricmp ( arg1, "team" )
		//|| !Q_stricmp ( arg1, "addref" )
		)
	{	
		int n = -1;
		if(!strcmp(arg2, "")){
			trap_SendServerCommand( ent-g_entities, va("print \"^3usage: ^7%s #id\n\"", arg1 ) );
			return qfalse;
		}
		n = atoi ( arg2 );
		if ( n < 0 || n >= MAX_CLIENTS )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"invalid client number %d.\n\"", n ) );
			return qfalse;
		}
		if ( g_entities[n].client->pers.connected == CON_DISCONNECTED )
		{
			trap_SendServerCommand( ent-g_entities, va("print \"there is no client with the client number %d.\n\"", n ) );
			return qfalse;
		}
		//Com_sprintf ( level.voteString, sizeof(level.voteString ), "%s %s", arg1, g_entities[n].client->pers.netname );
		Com_sprintf ( level.voteString, sizeof(level.voteString ), "%s %i", arg1, n );
		Com_sprintf ( level.voteDisplayString, sizeof(level.voteDisplayString), "%s %s", arg1, g_entities[n].client->pers.netname );
		return qtrue;
	}
	else
	{
		// Special case because the developers were gay and decided to make names different
		if ( !Q_stricmp ( arg1, "friendlyfire" ) ) 
		{
			Com_sprintf( level.voteString, sizeof( level.voteString ), "g_friendlyFire \"%s\"\n", g_friendlyFire.integer?"0":"1" );
		}	
		else if ( !Q_stricmp ( arg1, "timeextension" ) ) 
		{
			Com_sprintf( level.voteString, sizeof( level.voteString ), "extendtime \"%s\"\n", arg2 );
		}
		else if ( !Q_stricmp ( arg1, "scoreextension" ) ) 
		{
			Com_sprintf( level.voteString, sizeof( level.voteString ), "extendscore \"%s\"\n", arg2 );
		}
				
		if ( !Q_stricmp ( arg1, "map" ) ) 
		{
			if ( !G_DoesMapExist ( arg2 ) )
			{
				trap_SendServerCommand( ent-g_entities, "print \"This server does not have that map!\n\"" );
				return;
			}
			Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%s\"\n", arg1, arg2 );
		}
		else // Default
		{
			Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%s\"\n", arg1, arg2 );
		}

	//	Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), level.voteString  );
	}
	Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), level.voteString  );
	return qtrue;
}
void NV_voteCommands (gentity_t *ent, char *input)
{
	char			buffer[15000] = "\0";	
	const char		*token;
	char			cvar[32] =	"\0";
//	char			shortName[128] = "\0";
	char			*p = "\0"; 
	qboolean		first = qtrue;
	qboolean		voteAllowed = qfalse;

	NV_LoadFile( "settings/voting.cfg", buffer, qfalse );
	token = buffer;

	if(strchr( input, ';' ))
	{
		CPe(va("print \" ^7invalid vote command\n\""));
		return;
	}
	if(!NV_canCallVote(ent)) // Check to see if voting is allowed at all
	{
		//continue;
		return;
	}

	while(token) 
	{	
		p	= COM_ParseExt(&token, qtrue);
		Q_strncpyz( cvar, Q_strlwr(p), strlen(p) + 1);
		p	= COM_ParseExt(&token, qtrue);
		
		/// -- Print out the commands at the bottom of the Admin command list
		if(!input[0] || !strcmp(input, "?"))
		{
			if(first)
			{
				CPe(va("print \"@^7Available votes are:\n--------------------\n\""));
				first = qfalse;
			}
			if(atoi(p) < 1)
			{
				continue;
			}
			if (atoi(p) > 1 && atoi(p) > (ent->client->nvSess.admin))
			{
				continue;
			}
			//if (atoi(p) > 2 && atoi(p) > ent->client->nvSess.admin && atoi(p) != 4)
			//{
			//	continue;
			//}
			CPe(va("print \"@ ^5%s\n\"", cvar));
			continue;
		}
		/// --

		//if(!NV_canCallVote(ent)) // Check to see if voting is allowed at all
		//{
		//	//continue;
		//	return;
		//}
		Q_strlwr(input);
		if(!strcmp(input, cvar))
		{		
			char	arg2[MAX_STRING_TOKENS];
			//char	arg3[MAX_STRING_TOKENS];
			//char	arg4[MAX_STRING_TOKENS];
			//char	arg5[MAX_STRING_TOKENS];
			int		i;

			voteAllowed = qfalse;
			i = atoi(p);
			if(i <= 0)
			{
				CPe(va("print \"^7This vote is not available to you.\n\""));
				return;
			}
			//if (i == 2 && !(ent->client->nvSess.clanMember || ent->client->nvSess.admin))
			//{
			//	CPe(va("print \"^7This vote is not available to you.\n\""));
			//	return;
			//}
			//if (i > 2 && i > ent->client->nvSess.admin)
			//{
			//	CPe(va("print \"^7This vote is not available to you.\n\""));
			//	return;
			//}
						
			if (i > 1 && !(ent->client->nvSess.admin))
			{
				CPe(va("print \"^7This vote is not available to you.\n\""));
				return;
			}
			// 4 = all, so skip the check
			if (i != 4 && i >= 2 && i > ent->client->nvSess.admin)
			{
				CPe(va("print \"^7This vote is not available to you.\n\""));
				return;
			}


			trap_Argv( 2, arg2, sizeof( arg2 ) );
			//trap_Argv( 3, arg3, sizeof( arg3 ) );
			
			if( strchr( arg2, ';' )/* || strchr( arg3, ';' ) || strchr( arg4, ';' ) || strchr( arg5, ';' )*/) 
			{
				trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
				return;
			}
			if ( !Q_stricmp( input, "rmgmap" ) ) 
			{
				char	arg3[MAX_STRING_TOKENS];
				char	arg4[MAX_STRING_TOKENS];

				trap_Argv( 3, arg3, sizeof( arg3 ) );
				trap_Argv( 4, arg4, sizeof( arg4 ) );

				if( strchr( arg3, ';' ) || strchr( arg4, ';' ) || strchr( ConcatArgs ( 5 ), ';' )) 
				{
					trap_SendServerCommand( ent-g_entities, "print \"Invalid vote string.\n\"" );
					return;
				}
				Com_sprintf( level.voteString, sizeof( level.voteString ), "rmgmap 1 %s 2 %s 3 %s 4 \"%s\" 0", arg2, arg3, arg4, ConcatArgs ( 5 ) );
				Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), "Randomly Generated Map" );
			}
			else
			{		
				if(!NV_ClientNameNeeded(ent, cvar, arg2))
					return;
				//Com_sprintf( level.voteString, sizeof( level.voteString ), "%s \"%s\"\n", cvar, arg2 );
				//Com_sprintf( level.voteDisplayString, sizeof( level.voteDisplayString ), level.voteString  );
			}


			//CPe(va("print \"^7%s - %s\n\"", cvar,  p));
			trap_SendServerCommand( -1, va("print \"%s ^7called a vote.\n\"", ent->client->pers.netname ) );
				
			// start the voting, the caller autoamtically votes yes
			level.voteTime = level.time;
			level.voteYes = 1;
			level.voteNo = 0;

			for ( i = 0 ; i < level.maxclients ; i++ ) 
			{
				level.clients[i].ps.eFlags &= ~EF_VOTED;
			}
			ent->client->ps.eFlags |= EF_VOTED;

			level.voteClient = ent->s.number; // Save the voting client ID

			trap_SetConfigstring( CS_VOTE_TIME, va("%i,%i", level.voteTime, g_voteDuration.integer*1000 ) );
			trap_SetConfigstring( CS_VOTE_STRING, level.voteDisplayString );	
			trap_SetConfigstring( CS_VOTE_YES, va("%i", level.voteYes ) );
			trap_SetConfigstring( CS_VOTE_NO, va("%i", level.voteNo ) );	
			trap_SetConfigstring( CS_VOTE_NEEDED, va("%i", level.numVotingClients / 2 ) );
			return;
		}
	}
	if(input[0])
		CPe(va("print \" ^3Invalid or restricted vote! \n\""));
}

#endif

