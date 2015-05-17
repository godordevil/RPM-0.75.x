#include "cg_local.h"
#include "animtable.h"


/*
======================
CG_ParseAnimationFile

Read a configuration file containing animation counts and rates
models/players/visor/animation.cfg, etc
======================
*/
qboolean CG_ParseAnimationFile ( const char *filename, clientInfo_t *ci) 
{
	const char		*text_p;
	int				len;
	int				i;
	char			*token;
	float			fps;
	int				skip;
	char			text[20000];
	fileHandle_t	f;
	int				animNum;
	animation_t		*animations;

	animations = ci->animations;

	// load the file
	len = trap_FS_FOpenFile( filename, &f, FS_READ );
	if ( len <= 0 || len >= sizeof( text ) - 1 ) 
	{
		return qfalse;
	}

	trap_FS_Read( text, len, f );
	text[len] = 0;
	trap_FS_FCloseFile( f );

	// parse the text
	text_p = text;
	skip = 0;	// quiet the compiler warning

	//FIXME: have some way of playing anims backwards... negative numFrames?

	//initialize anim array so that from 0 to MAX_ANIMATIONS, set default values of 0 1 0 100
	for(i = 0; i < MAX_ANIMATIONS; i++)
	{
		animations[i].firstFrame = 0;
		animations[i].numFrames = 0;
		animations[i].loopFrames = -1;
		animations[i].frameLerp = 100;
		animations[i].initialLerp = 100;
	}

	// read information for each frame
	while(1) 
	{
		token = COM_Parse( &text_p );

		if ( !token || !token[0]) 
		{
			break;
		}

		animNum = GetIDForString(animTable, token);
		if(animNum == -1)
		{
//#ifndef FINAL_BUILD
#ifdef _DEBUG
			Com_Printf(S_COLOR_RED"WARNING: Unknown token %s in %s\n", token, filename);
#endif
			continue;
		}

		token = COM_Parse( &text_p );
		if ( !token ) 
		{
			break;
		}
		animations[animNum].firstFrame = atoi( token );

		token = COM_Parse( &text_p );
		if ( !token ) 
		{
			break;
		}
		animations[animNum].numFrames = atoi( token );

		token = COM_Parse( &text_p );
		if ( !token ) 
		{
			break;
		}
		animations[animNum].loopFrames = atoi( token );

		token = COM_Parse( &text_p );
		if ( !token ) 
		{
			break;
		}
		fps = atof( token );
		if ( fps == 0 ) 
		{
			fps = 1;//Don't allow divide by zero error
		}
		if ( fps < 0 )
		{//backwards
			animations[animNum].frameLerp = floor(1000.0f / fps);
		}
		else
		{
			animations[animNum].frameLerp = ceil(1000.0f / fps);
		}

		animations[animNum].initialLerp = ceil(1000.0f / fabs(fps));
	}

	return qtrue;
}
