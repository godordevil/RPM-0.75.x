// Copyright (C) 2001-2002 Raven Software.
//
// gt_syscalls.h -- system call function definitions

// print message on the local console
void		trap_Print						( const char *fmt );

// abort the game
void		trap_Error						( const char *fmt );

// Current system milliseconds
int			trap_Milliseconds				( void );

void		trap_Cvar_Register				( vmCvar_t *cvar, const char *var_name, const char *value, int flags, float MinValue, float MaxValue );
void		trap_Cvar_Update				( vmCvar_t *cvar );
void		trap_Cvar_Set					( const char *var_name, const char *value );
int			trap_Cvar_VariableIntegerValue	( const char *var_name );
float		trap_Cvar_VariableValue			( const char *var_name );
void		trap_Cvar_VariableStringBuffer	( const char *var_name, char *buffer, int bufsize );


// General commands
void		trap_Cmd_Restart				( int delay );
void		trap_Cmd_TextMessage			( int client, const char* message );
void		trap_Cmd_RadioMessage			( int client, const char* message );
int			trap_Cmd_RegisterGlobalSound	( const char* sound );
void		trap_Cmd_StartGlobalSound		( int sound );
void		trap_Cmd_AddTeamScore			( team_t team, int score );
int			trap_Cmd_RegisterEffect			( const char* effect );
void		trap_Cmd_PlayEffect				( int effect, vec3_t origin, vec3_t angles );

// Item commands
qboolean	trap_Cmd_RegisterItem			( int itemid, const char* name, gtItemDef_t* def );
void		trap_Cmd_ResetItem				( int itemid );

// Trigger commands
qboolean	trap_Cmd_RegisterTrigger		( int trigid, const char* name, gtTriggerDef_t* def );

// Client commands
void		trap_Cmd_GetClientName			( int clientid, const char* buffer, int buffersize );
void		trap_Cmd_GetClientItems			( int clientid, int* buffer, int buffersize );
qboolean	trap_Cmd_DoesClientHaveItem		( int clientid, int itemid );
void		trap_Cmd_AddClientScore			( int clientid, int score );
