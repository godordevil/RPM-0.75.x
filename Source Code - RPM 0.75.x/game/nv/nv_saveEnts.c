#include "../g_local.h"

#ifdef _MAPEDITOR

field_t savefields[] = 
{	
	{"model",				FOFS(model),				F_LSTRING},
	{"model2",				FOFS(model2),				F_LSTRING},
	{"classname",			FOFS(classname),			F_LSTRING},
	{"origin",				FOFS(s.origin),				F_VECTOR},
	{"spawnflags",			FOFS(spawnflags),			F_INT},
	{"speed",				FOFS(speed),				F_FLOAT},
	{"target",				FOFS(target),				F_LSTRING},
	{"targetname",			FOFS(targetname),			F_LSTRING},
	{"message",				FOFS(message),				F_LSTRING},
	{"message2",			FOFS(message2),				F_LSTRING},
	{"team",				FOFS(team),					F_LSTRING},
	{"wait",				FOFS(wait),					F_FLOAT},
	{"random",				FOFS(random),				F_FLOAT},
	{"count",				FOFS(count),				F_INT},
	{"health",				FOFS(health),				F_INT},
	{"dmg",					FOFS(damage),				F_INT},
	{"angles",				FOFS(s.angles),				F_VECTOR},
	///{"angle",				FOFS(s.angles),				F_ANGLEHACK},
	{"angles",				FOFS(savedAngles),			F_ANGLEHACK}, 
	{"targetShaderName",	FOFS(targetShaderName),		F_LSTRING},
	{"targetShaderNewName", FOFS(targetShaderNewName),	F_LSTRING},
	{"mins",				FOFS(r.mins),				F_VECTOR},
	{"maxs",				FOFS(r.maxs),				F_VECTOR},
	{"origin2",				FOFS(s.origin2),			F_VECTOR},
	{"bspmodel",			FOFS(bspmodel),				F_LSTRING},
	{"distance",			FOFS(distance),				F_FLOAT},
	{"effect",				FOFS(message2),				F_LSTRING},
#ifdef _DEBUG
	{"apos1",				FOFS(apos1),				F_VECTOR},
	{"apos2",				FOFS(apos2),				F_VECTOR},
#endif
#ifdef _MOUNTED
	{"weapon",				FOFS(mounted_weapon),		F_INT},
	{"weapon_alt",			FOFS(mounted_weapon_alt),	F_INT},
	{"ammo",				FOFS(ammo[0]),				F_INT},
	{"ammo_alt",			FOFS(altAmmo[0]),			F_INT},
	{"firedelay",			FOFS(firedelay),			F_INT},
	{"firedelay_alt",		FOFS(firedelay_alt),		F_INT},
#endif
	{NULL}
};

void NV_saveToFile( const void *buffer, int len, fileHandle_t f ) {
	trap_FS_Write( buffer, len, f );
}

void NV_checkType (fileHandle_t f, field_t *field, byte *base)
{
	void		*p;
	char		line[128] = "\0";
	
	p = (void *)(base + field->ofs);

	switch (field->type)
	{
	case F_LSTRING:
		if ( *(char **)p ){
			strcpy (line, va("\"%s\" \"%s\"\n", field->name, *(char **)p));
			NV_saveToFile (line,  strlen(line), f);
		}
		break;	
	case F_INT:
		if(*(int *)p){
			strcpy (line, va("\"%s\" \"%i\"\n", field->name, *(int *)p));
			NV_saveToFile (line,  strlen(line), f);
		}
		break;
	case F_ANGLEHACK:
	case F_VECTOR:
		///if(*(float *)p){
		if(*(vec3_t*)p){
			vec3_t v;
			VectorCopy( *(vec3_t*)p, v);
			if(v[0] == 0 && v[1] == 0 && v[2] == 0)
				break;
			strcpy (line, va("\"%s\" \"%.0f %.0f %.0f\"\n", field->name, v[0], v[1], v[2]));
			NV_saveToFile (line,  strlen(line), f);
		}
		break;
	case F_FLOAT:
		if(*(float *)p){
			if(!strcmp(field->name, "wait")){
				float  w = (*(float *)p);
				if(w >= 1000)
					strcpy (line, va("\"%s\" \"%.1f\"\n", field->name, w/1000));
				else
					strcpy (line, va("\"%s\" \"%.1f\"\n", field->name, w));
			}
			else
				strcpy (line, va("\"%s\" \"%.1f\"\n", field->name, *(float *)p));
			NV_saveToFile (line,  strlen(line), f);
		}
		break;		
	default:
		break;
	}
}


void NV_entFields(fileHandle_t f, gentity_t *ent)
{
	field_t	*field;
	for (field=savefields; field->name; field++)
	{
#ifdef _MOUNTED
		// Skip count for #mounted_weapons
		if(!strcmp(ent->classname, "mounted_gun") && !strcmp(field->name, "count"))
			continue;
#endif
		NV_checkType (f, field, (byte *)ent);
	}
}


void NV_copyFields (field_t *field, byte *base)
{
	void		*p;
	char		line[128] = "\0";
	
	p = (void *)(base + field->ofs);

	switch (field->type)
	{
	case F_LSTRING:
		if ( *(char **)p )
		{
			strcpy (line, va("%s", *(char **)p));
			AddSpawnField( field->name, *(char **)p);
		}
		break;	
	case F_INT:
		if(*(int *)p)
		{
			strcpy (line, va("%i", *(int *)p));
			AddSpawnField( field->name, line);
		}
		break;
	case F_ANGLEHACK:
	case F_VECTOR:
		if(*(vec3_t*)p)
		{
			vec3_t v;
			VectorCopy( *(vec3_t*)p, v);
			if(v[0] == 0 && v[1] == 0 && v[2] == 0)
				return;
			strcpy (line, va("%.0f %.0f %.0f", v[0], v[1], v[2]));
			AddSpawnField( field->name, line);
		}
		break;
	case F_FLOAT:
		if(*(float *)p)
		{
			if(!strcmp(field->name, "wait"))
			{
				float  w = (*(float *)p);
				if(w >= 1000)
				{
					strcpy (line, va("%.1f",  w/1000));
				}
				else
				{
					strcpy (line, va("%.1f", w));
				}
			}
			else
			{
				strcpy (line, va("%.1f", *(float *)p));
			}
			AddSpawnField( field->name, line);
		}
		break;		
	default:
		return;
	}
}


void NV_duplicateEnt (gentity_t *ent)
{
	field_t	*field;

	for (field=savefields; field->name; field++)
	{
		NV_copyFields (field, (byte *)ent);
	}

	G_SpawnGEntityFromSpawnVars(qfalse);

	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;
}



void NV_writeLevelSpawns(fileHandle_t f)
{	
	char			*string;
	int				i;
	
	///Create a worldSpawn
	string = va("\n{\n\"classname\" \"worldspawn\"\n");
	trap_FS_Write( string, strlen( string ), f );
	if(level.gametypeData->teams)
	{
		string = va("\"redteam\" \"%s\"\n\"blueteam\"\t\"%s\"\n", level.gametypeTeam[TEAM_RED], level.gametypeTeam[TEAM_BLUE]);
		trap_FS_Write( string, strlen( string ), f );
	}
	
	if(g_noDropOrigin.integer != 0)
	{
		string = va("\"nodrop\" \"%i\"\n", g_noDropOrigin.integer);
		trap_FS_Write( string, strlen( string ), f );
	}		
	string = va("\"doors\" \"%i\"\n", g_enableDoors.integer);
	trap_FS_Write( string, strlen( string ), f );

	trap_FS_Write( "}", 1, f );


	///

	///Get player spawnpoints
	for ( i = 0; i < level.spawnCount; i ++ )
	{
		//gspawn_t* spawn = &level.spawns[i];

		if (level.gametypeData->respawnType != RT_NORMAL)
		{
			if(!level.spawns[i].team && !(NV_isDom() || NV_isKoth() || NV_isRush() || NV_isVIP()) ) ///#dom write TEAM_FREE Spawns
				continue; ///Skip if inf/elim and no team set.
		}

		trap_FS_Write( "\n{\n", 3, f );

		if (level.gametypeData->respawnType == RT_NORMAL )
		{
			string = va("\"classname\" \"info_player_deathmatch\"\n");
			trap_FS_Write( string, strlen( string ), f );
		}
		else 
		{
			string = va("\"classname\" \"gametype_player\"\n");
			trap_FS_Write( string, strlen( string ), f );
			string = va("\"spawnflags\" \"%i\"\n", level.spawns[i].team);
			trap_FS_Write( string, strlen( string ), f );
			string = va("\"count\" \"%i\"\n", level.spawns[i].phase);
			trap_FS_Write( string, strlen( string ), f );
		}

		string = va("\"origin\" \"%.0f %.0f %.0f\"\n", level.spawns[i].origin[0], level.spawns[i].origin[1], level.spawns[i].origin[2]);
		trap_FS_Write( string, strlen( string ), f );
				
		string = va("\"angles\" \"%.0f %.0f %.0f\"\n", level.spawns[i].angles[0], level.spawns[i].angles[1], level.spawns[i].angles[2]);
		trap_FS_Write( string, strlen( string ), f );

		trap_FS_Write( "}", 1, f );
	}	
	trap_FS_Write( "\n\n", 2, f );
}
void NV_writeRMGInfo(fileHandle_t f,gentity_t *ent2 )
{	
	char			*string;
//	char			s[MAX_STRING_CHARS];
//	int				i;

	///Create a worldSpawn
	//string = va("\n{\n\"classname\" \"worldspawn\"\n");
	//trap_FS_Write( string, strlen( string ), f );
	//if(level.gametypeData->teams){
	//	string = va("\"redteam\" \"%s\"\n\"blueteam\"\t\"%s\"\n", level.gametypeTeam[TEAM_RED], level.gametypeTeam[TEAM_BLUE]);
	//	trap_FS_Write( string, strlen( string ), f );
	//}
	//trap_FS_Write( "}", 1, f );
	///
	//trap_FS_Write( "{\n", 2, f );
	string = va("\n{\n"
		"\"densitymap\" \"%s\"\n "
		"\"heightmap\" \"%s\"\n" 
		"\"numpatches\" \"%d\"\n"
		"\"shader\" \"%d\"\n"
		"\"terxels\" \"%d\"\n"
		"\"texturescale\" \"%f\"\n", 

		"heightmaps/density_small", 
		"heightmaps/random_small", 
		1024, 
		0, 
		4, 
		.0375
		);

	//for ( i=1 ; i<level.rmgCount; i++ ) 
	//{
	//	trap_GetConfigstring( CS_TERRAINS + level.rmgCount, s, sizeof( s ) );
	//	if ( !s[0] ) 
	//	{
	//		break;
	//	}
	//	if ( !strcmp( s, name ) ) 
	//	{
	//		return i;
	//	}
	//}
	trap_FS_Write( string, strlen( string ), f );
	NV_entFields (f, ent2);
	trap_FS_Write( "}\n", 2, f );
}

void NV_createEntFile(gentity_t *ent)
{
	fileHandle_t	f;
	int				i;
	gentity_t		*ent2;
	char			mapname[MAX_QPATH];
	char			arg[MAX_STRING_TOKENS];	
	char *altDir = "maps\\";
	int alt = 0;

	alt = trap_Cvar_VariableIntegerValue( "alt" );
	if(alt)
	{	
		altDir = "maps\\alt\\";
	}

	trap_Cvar_VariableStringBuffer ( "mapname", mapname, MAX_QPATH );
	trap_Argv( 2, arg, sizeof( arg ) );

	if(!arg[0] || strlen(arg) < 2)
		Q_strncpyz(arg, NV_setGameTypeName(), 6);
	 if (trap_FS_FOpenFile( va("%s%s_%s.ent",altDir, mapname, arg), &f, FS_WRITE ) < 0) 
	 {
		CPe(va("cp \"^3Cannot open file!\n^3ENTITIES WILL ^1NOT ^3BE ^1SAVED^3!\n"));
		return;
	}

	/// write worldSpawn & player Spawns (based on GT)
	NV_writeLevelSpawns(f);

	/// write level entities		
	for (i=MAX_CLIENTS-1 ; i<level.num_entities ; i++)
	{
		ent2 = &g_entities[i];
		if (!ent2->inuse || ent2->s.number == ENTITYNUM_WORLD)
			continue;
		if ( ent2->freeAfterEvent )	
			continue;
		if ( ent2->s.weapon == 999 )	
			continue;
		if ( !ent2->r.linked && ent2->neverFree ) 
			continue;
		if ( ent2->s.eType == ET_MISSILE )
			continue;
		//if ( ent2->client)	///Dont need
		//	continue;
		if( ent2->bspmodel) ///skip bsp 
			continue;
		if( ent2->classname )
		{
			if( !strcmp(ent2->classname, "freed"))
				continue;
			if( !strcmp(ent2->classname, "bodyque"))
				continue;
			if(!Q_strncmp (ent2->classname, "gametype_item_", 14))
				continue;
			if(!Q_strncmp (ent2->classname, "pickup_weapon_", 14))
				continue;
			// These are extras spawned, so skip them
			if( !strcmp(ent2->classname, "bomb_kill"))
				continue;
			if( !strcmp(ent2->classname, "mounted_base"))
				continue;
			if( !strcmp(ent2->classname, "target_base"))
				continue;
			//End

			//if(!strcmp (ent2->classname, "target_effect"))
			//	continue;
			if((!strcmp(ent2->classname, "red_capture_point") 
			|| !strcmp(ent2->classname, "blue_capture_point") 
			|| !strcmp(ent2->classname, "briefcase_destination")
			|| !strcmp(ent2->classname, "briefcase_destination_red")
			|| !strcmp(ent2->classname, "briefcase_destination_blue")) )
			{
				VectorSet (ent2->r.maxs, 15, 15, 30);
				VectorSet (ent2->r.mins, -15, -15, -15);
			}
			if( !strcmp(ent2->classname, "terrain"))
			{	
				NV_writeRMGInfo(f, ent2);
				continue;
			}

		}
		trap_FS_Write( "{\n", 2, f );
		NV_entFields (f, ent2);
		trap_FS_Write( "}\n", 2, f );
	}

	/// Now, lets save the bsp's to the end of the file.
	for (i=MAX_CLIENTS-1 ; i<level.num_entities ; i++)
	{
		ent2 = &g_entities[i];
		if(!ent2->bspmodel)
			continue;
		trap_FS_Write( "{\n", 2, f );
		NV_entFields (f, ent2);
		trap_FS_Write( "}\n", 2, f );
	}

	trap_FS_FCloseFile( f );

	//NV_SaveArenaFileInfo(mapname, arg);
	CPe(va("cp \"^3Save Successful: ^7%s%s_%s.ent\n", altDir, mapname, arg));
	CP(va("print \"^5Ent file saved by ^3%s\n",   ent->client->pers.netname));
}




// These are just for displaying and editing.. but putting them here now
void NV_ChangeEntField(gentity_t *ent, const char *key, const char *value)
{
	field_t	*f;
	byte	*b;
	float	v;
	vec3_t	vec;

	for ( f=savefields ; f->name ; f++ ) {
		if ( !Q_stricmp(f->name, key) ) {
			// found it
			b = (byte *)ent;
			trap_UnlinkEntity( ent );
			switch( f->type ) {
			case F_LSTRING:
				*(char **)(b+f->ofs) = G_NewString (value);
				break;
			case F_VECTOR:
				sscanf (value, "%f %f %f", &vec[0], &vec[1], &vec[2]);
				((float *)(b+f->ofs))[0] = vec[0];
				((float *)(b+f->ofs))[1] = vec[1];
				((float *)(b+f->ofs))[2] = vec[2];
				break;
			case F_INT:
				*(int *)(b+f->ofs) = atoi(value);
				break;
			case F_FLOAT:
				*(float *)(b+f->ofs) = atof(value);
				break;
			case F_ANGLEHACK:
				v = atof(value);
				((float *)(b+f->ofs))[0] = 0;
				((float *)(b+f->ofs))[1] = v;
				((float *)(b+f->ofs))[2] = 0;
				break;
			default:
			case F_IGNORE:
				break;
			}
			trap_LinkEntity( ent );
			return;
		}
	}
}

void NV_viewEntFieldInfo(gentity_t *ent)
{
	field_t	*f;
	byte	*b;

	if(!nv_editMap.integer || !ent->client->nvSess.admin >= 4)
		return;
	if(!ent->client->nvSess.editEnt)
		return;

	for ( f=savefields ; f->name ; f++ ) 
	{
		b = (byte *)&g_entities[ent->client->nvSess.editEnt];
		switch( f->type ) {
		case F_LSTRING:
			//*(char **)(b+f->ofs) = G_NewString (value);
			trap_SendServerCommand( ent-g_entities, va("print \"^3%-22s^7%s\n",  f->name, *(char **)(b+f->ofs)) );
			break;
		case F_VECTOR:
			trap_SendServerCommand( ent-g_entities, va("print \"^3%-22s^7%.2f %.2f %.2f\n",  f->name, ((float *)(b+f->ofs))[0], ((float *)(b+f->ofs))[1], ((float *)(b+f->ofs))[2] ) );
			break;
		case F_INT:
			trap_SendServerCommand( ent-g_entities, va("print \"^3%-22s^7%d\n",  f->name, *(int *)(b+f->ofs) ) );
			break;
		case F_FLOAT:
			trap_SendServerCommand( ent-g_entities, va("print \"^3%-22s^7%.2f\n",  f->name, *(float *)(b+f->ofs) ) );
			break;
		case F_ANGLEHACK:
			trap_SendServerCommand( ent-g_entities, va("print \"^3%-22s^7%.2f %.2f %.2f\n",  f->name, ((float *)(b+f->ofs))[0], ((float *)(b+f->ofs))[1], ((float *)(b+f->ofs))[2] ) );
			break;
		default:
		case F_IGNORE:
			break;
		}
	}
}


#endif

