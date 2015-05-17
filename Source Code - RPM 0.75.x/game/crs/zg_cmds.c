#include "../g_local.h"

#ifdef _CRS_BOTS	

void G_SavePoints(void);
void WP_DebugRenderSwitch(void);
void G_LoadPoints(void);
void G_TestPath(gentity_t *ent);
void WP_BeginPointGeneration(void);

/*
=================
zClientCommand
=================
*/
qboolean zClientCommand(int clientNum)
{
	gentity_t *ent;
	char cmd[MAX_TOKEN_CHARS];

	trap_Argv( 0, cmd, sizeof( cmd ) );

	ent = g_entities + clientNum;
	if (!ent->client )
	{
		return qtrue;		// not fully in game yet
	}
	
	if (Q_stricmp (cmd, "wpsave") == 0)
	{
		if (g_cheats.integer)
		{
			G_SavePoints();
		}
		return qtrue;
	}
	else if (Q_stricmp (cmd, "wprender") == 0)
	{
		if (g_cheats.integer)
		{
			WP_DebugRenderSwitch();
		}
		return qtrue;
	}
	else if (Q_stricmp (cmd, "wpload") == 0)
	{
		if (g_cheats.integer)
		{
			G_LoadPoints();
		}
		return qtrue;
	}
	else if (Q_stricmp (cmd, "wptestpath") == 0)
	{
		if (g_cheats.integer)
		{
			G_TestPath(ent);
		}
		return qtrue;
	}
	else if (Q_stricmp(cmd, "wpgen" ) == 0)
	{
		if (g_cheats.integer)
		{
			WP_BeginPointGeneration();
		}
		return qtrue;
	}
	

	return qfalse;
}

#endif



