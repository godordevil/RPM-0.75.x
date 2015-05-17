
#include  "../g_local.h"

// GodOrDevil - 02.26.06 - 07:53pm #sp map
/// Modified version from MANDOWN sdk
static fileHandle_t entFile;

//extern char *buffer;


char buffz[131072];
char *buffer = buffz;

void NV_ParseMapShaders( char *buf )
{
	const char	*ptoken = "\0";
	char		*token = "\0";
	char		oldShader[MAX_QPATH]="\0";
	char		newShader[MAX_QPATH]="\0";
//	char		mapname[MAX_QPATH] = "\0";

	ptoken = buf;

	while(ptoken) 
	{	
		token = COM_ParseExt(&ptoken, qfalse);
		if (!token || token[0] == 0) 
			continue;
		Q_strncpyz(oldShader, token, MAX_QPATH - 1);

		token = COM_ParseExt(&ptoken, qfalse);
		if (!token || token[0] == 0) 
			continue;
		Q_strncpyz(newShader, token, MAX_QPATH - 1);

		AddRemap(oldShader, newShader, level.time * 0.001);
	}
	trap_SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
}

qboolean NV_ParseNewShaders( void )
{
	int			len;
	fileHandle_t f;
	char		buf[14000] = "\0";
	char		mapname[MAX_QPATH] = "\0";
	char		fileName[MAX_QPATH] = "\0";
	qboolean	found = qfalse;
	char *altDir = "maps\\shaders\\";
	int alt = 0;

	// First check Misc	
	Com_sprintf(fileName, sizeof(fileName), "%smisc.cfg", altDir);
	len = trap_FS_FOpenFile(fileName, &f, FS_READ );
	//len = trap_FS_FOpenFile(va("maps\\shaders\\misc.cfg"), &f, FS_READ );
	if(f)
	{
		memset( buf, 0, sizeof(buf) );
		trap_FS_Read( buf, len, f );

		if(len > 14000)	
			len = 14000;
		buf[len] = 0;
		trap_FS_FCloseFile( f );		
		NV_ParseMapShaders(buf);
		found = qtrue;
	}

	// Then check Gametype
	//len = trap_FS_FOpenFile(va("maps\\shaders\\%s.cfg", nv_gtShort.string), &f, FS_READ );
	Com_sprintf(fileName, sizeof(fileName), "%s%s.cfg", altDir, nv_gtShort.string );

	len = trap_FS_FOpenFile(fileName, &f, FS_READ );
	if(f)
	{
		memset( buf, 0, sizeof(buf) );
		trap_FS_Read( buf, len, f );

		if (len >= 14000)
			len = 14000;
		buf[len] = 0;
		trap_FS_FCloseFile( f );		
		NV_ParseMapShaders(buf);
		found = qtrue;
	}

	alt = trap_Cvar_VariableIntegerValue( "alt" );
	if(alt)
	{	
		altDir = "maps\\shaders\\alt\\";
	}

	// Now by map & gametype	
	trap_Cvar_VariableStringBuffer ( "mapname", mapname, MAX_QPATH );
	// First check by map name & gametype	
	Com_sprintf(fileName, sizeof(fileName),"%s%s_%s.cfg", altDir, mapname, nv_gtShort.string);
	len = trap_FS_FOpenFile(fileName, &f, FS_READ );

	// if not found, check by map name
	if(!f || len <= 0)
	{	
		trap_FS_FCloseFile( f );	
		Com_sprintf(fileName, sizeof(fileName), "%s%s.cfg", altDir, mapname);
		len = trap_FS_FOpenFile(fileName, &f, FS_READ );
	}	
	if(f)
	{
		memset( buf, 0, sizeof(buf) );
		trap_FS_Read( buf, len, f );

		if (len > 14000)
			len = 14000;
		buf[len] = 0;
		trap_FS_FCloseFile( f );		
		NV_ParseMapShaders(buf);
		found = qtrue;
	}

	return found;
}

qboolean G_LoadEntFile(void)
{
	char entPath[128];
	vmCvar_t mapname;
	int len;
	char    buf[15000] = "\0";
	int alt = 0;

	trap_Cvar_Register( &mapname, "mapname", "", CVAR_SERVERINFO | CVAR_ROM, 0.0, 0.0 );

	alt = trap_Cvar_VariableIntegerValue( "alt" );
	if(alt)
	{	
	//	trap_SendConsoleCommand( EXEC_APPEND, va("seta alt \"0\";" ) );
		Com_sprintf(entPath, 128, "maps/alt/%s_%s.ent\0", mapname.string, NV_setGameTypeName());			

		len = trap_FS_FOpenFile(entPath, &entFile, FS_READ);
		if (!entFile)
		{
			int i;
			extern customGametypeInfo_t	gtInfo[];
			len = trap_FS_FOpenFile(entPath, &entFile, FS_READ);

			for ( i = 0 ; i < 5; i++ )
			{
				if(!Q_stricmp(gtInfo[i].based, g_gametype.string))
				{
					Com_sprintf(entPath, 128, "maps/alt/%s_%s.ent\0", mapname.string, gtInfo[i].name);
					len = trap_FS_FOpenFile(entPath, &entFile, FS_READ);
					break;
				}
			}
		}
		if (!entFile)
		{ 
			Com_sprintf(entPath, 128, "maps/alt/%s.ent\0", mapname.string); 
			len = trap_FS_FOpenFile(entPath, &entFile, FS_READ);
			//if (!entFile)
			//	return qfalse;
		}
		if (!entFile)  // fuck it, try the non-alt maps now.
		{
			Com_sprintf(entPath, 128, "maps/%s_%s.ent\0", mapname.string, NV_setGameTypeName());		
			len = trap_FS_FOpenFile(entPath, &entFile, FS_READ);
		}
	}
	else
	{
		Com_sprintf(entPath, 128, "maps/%s_%s.ent\0", mapname.string, NV_setGameTypeName());		
		len = trap_FS_FOpenFile(entPath, &entFile, FS_READ);
	}
	if (!entFile)
	{
		int i;
		extern customGametypeInfo_t	gtInfo[];
		len = trap_FS_FOpenFile(entPath, &entFile, FS_READ);

		for ( i = 0 ; i < 5; i++ )
		{
			if(!Q_stricmp(gtInfo[i].based, g_gametype.string))
			{
				Com_sprintf(entPath, 128, "maps/%s_%s.ent\0", mapname.string, gtInfo[i].name);
				len = trap_FS_FOpenFile(entPath, &entFile, FS_READ);
				break;
			}
		}
	}

	if (!entFile)
	{ /// failing that, just try by map name
		Com_sprintf(entPath, 128, "maps/%s.ent\0", mapname.string); 
		len = trap_FS_FOpenFile(entPath, &entFile, FS_READ);
		if (!entFile)
			return qfalse;
	}
	Com_Printf(S_COLOR_YELLOW "Loading ent data from \"%s\"\n", entPath);

	if (len >= 131072) {
		Com_Printf( S_COLOR_RED "file too large: %s is %i, max allowed is %i", entPath, len, 131072);
		trap_FS_FCloseFile(entFile);
		return qfalse;
	}	
	trap_FS_Read((char*)buffer, len, entFile);
//	Com_sprintf(entPath, 128, "maps/%s_extras.ent\0", mapname.string);
	buffer[len] = 0;
	trap_FS_FCloseFile(entFile);

	return qtrue;
}
qboolean G_ReadingFromEntFile(qboolean inSubBSP)
{
	if (inSubBSP)
		return qfalse;
//#RMG TESTING
//#ifndef _DEBUG
//	if (RMG.integer)
//		return qfalse;
//#endif
//END
	if(entFile)
		return qtrue;
	return qfalse;
}
static char token[MAX_TOKEN_CHARS];

char *G_GetEntFileToken(void)
{
	qboolean hasNewLines = qfalse;
	const char *data;
	int c = 0, len;

	data = buffer;
	len = 0;
	token[0] = 0;

	/// make sure incoming data is valid
	if (!data){
		buffer = NULL;
		//*buffer = NULL;
		return NULL;
	}

	while (1){
		/// skip whitespace
		data = SkipWhitespace(buffer, &hasNewLines);
		if ( !data ){
			buffer = NULL;
			//*buffer = NULL;
			return NULL; /// EOF
			///return token;
		}

		c = *data;

		/// skip double slash comments
		if (c == '/' && data[1] == '/') {
			data += 2;
			while (*data && *data != '\n'){
				data++;
			}
		}
		/// skip /* */ comments
		else if ( c=='/' && data[1] == '*') {
			data += 2;
			while (*data && (*data != '*' || data[1] != '/' )) {
				data++;
			}

			if (*data)
				data += 2;
		}
		else
			break;
	}
	///handle quoted strings
	if (c == '\"') {
		data++;
		while (1){
			c = *data++;
			if (c=='\"' || !c) {
				token[len] = 0;
				buffer = ( char * ) data;
				//*buffer = ( char * ) data;

				return token;
			}
			if (len < MAX_TOKEN_CHARS) {
				token[len] = c;
				len++;
			}
		}
	}
	///parse a regular word
	do {
		if (len < MAX_TOKEN_CHARS) {
			token[len] = c;
			len++;
		}
		data++;
		c = *data;
	} while (c>32);

	if (len == MAX_TOKEN_CHARS) {
		//Com_Printf ("Token exceeded %i chars, discarded.\n", MAX_TOKEN_CHARS);
		len = 0;
	} 
	token[len] = 0;
	buffer = (char *)data;
	//*buffer = ( char * ) data;

	if (token[0] == 0 || token[0] == ' '){
		return NULL; /// EOF
	} 
	return token;
}


/*
void NV_ParseBrushModels ( void ) 
{
	char	keyname[MAX_TOKEN_CHARS];
	char	com_token[MAX_TOKEN_CHARS];

	// parse the opening brace
	if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) ) 
		return;
	
	if ( com_token[0] != '{' ) 
		Com_Error( ERR_FATAL, "G_ParseSpawnVars: found %s when expecting {",com_token );

	// go through all the key / value pairs
	while ( 1 ) 
	{
		// parse key
		if ( !trap_GetEntityToken( keyname, sizeof( keyname ) ) ) 
			Com_Error( ERR_FATAL, "G_ParseSpawnVars: EOF without closing brace" );

		if ( keyname[0] == '}' ) 
			break;
			
		// parse value	
		if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) ) 
			Com_Error( ERR_FATAL, "G_ParseSpawnVars: EOF without closing brace" );

		if ( com_token[0] == '}' ) 
			Com_Error( ERR_FATAL, "G_ParseSpawnVars: closing brace without data" );
	}
}
*/
void HandleEntityAdjustment(void);
qboolean NV_ParseSpawnVars( qboolean inSubBSP ) 
{
	char	keyname[MAX_TOKEN_CHARS];
	char	com_token[MAX_TOKEN_CHARS];
	char	*token;

	
	if(!inSubBSP)
	{
		level.numSpawnVars = 0;
		level.numSpawnVarChars = 0;
	}

	//NV_ParseBrushModels();

	token = G_GetEntFileToken();
	if (!token)
		return qfalse;
	Com_sprintf(com_token, sizeof(com_token), "%s", token);
	
	if ( com_token[0] != '{' ) 
		Com_Error( ERR_FATAL, "G_ParseSpawnVars: found %s when expecting {",com_token );

	// go through all the key / value pairs
	while ( 1 ) 
	{
		token = G_GetEntFileToken();
		if (!token)
			Com_Error( ERR_FATAL, "G_ParseSpawnVars: EOF without closing brace" );
		Com_sprintf(keyname, sizeof(keyname), "%s", token);

		if ( keyname[0] == '}' ) 
			break;

		token = G_GetEntFileToken();
		if (!token)
			Com_Error( ERR_FATAL, "G_ParseSpawnVars: EOF without closing brace" );
		Com_sprintf(com_token, sizeof(com_token), "%s", token);

		if ( com_token[0] == '}' ) 
			Com_Error( ERR_FATAL, "G_ParseSpawnVars: closing brace without data" );
		
		if ( level.numSpawnVars == MAX_SPAWN_VARS ) 
			Com_Error( ERR_FATAL, "G_ParseSpawnVars: MAX_SPAWN_VARS" );

		if(inSubBSP && !strcmp(keyname, "worldspawn"))
			continue;
		
		if(level.numSpawnVarChars > 1000)
			continue;
		AddSpawnField(keyname, com_token);
	}

	if (inSubBSP)
		HandleEntityAdjustment();

	return qtrue;
}

