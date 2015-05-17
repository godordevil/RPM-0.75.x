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

#include "cg_local.h"
#include "../ui/ui_shared.h"

/*
==================
CG_TeamCount
==================
*/
int CG_TeamCount ( int team )
{
	int count;
	int i;

	count = 0;

	for ( i = 0 ; i < cgs.maxclients ; i++ )
	{
		clientInfo_t* cl;

		cl = &cgs.clientinfo[ i ];

		if ( !cl->infoValid )
		{
			continue;
		}

		if ( team != cl->team )
		{
			continue;
		}

		count++;
	}

	return count;
}

/*
=============
RPM_ParseTeamMemberInfo
=============
*/
void RPM_ParseTeamMemberInfo(void)
{
	int i, client, count;
	count = atoi(CG_Argv(1));
//GodOrDevil - 1.06.2005 - Display Status on Scoreboard
/*======================================
	Changed RPM Version to 0.7
=======================================*/
		//Henk 26/12/10 -> Add new 2k3/1fx servers.     
     if(cgs.rpmServer >= 1.0) 
	 {      
		for(i = 0; i < count; i++)   
		{                      
		 char string[24];   
		 int damage, weapon;       
		 client = atoi(CG_Argv(i * 9 + 2));     
		 if ((client < 0) || (client > cgs.maxclients))        
		 {                              
			 Com_Printf("^3Bad Info String.\n"); 
			 break;                    
		 }                       
		 // Henk 26/12/10 -> Parse 2k3's weird way of joining weapon and damage in 1 string.    
		 strcpy(string, va("%s", CG_Argv(i * 9 + 8)));                  
		 weapon = atoi(va("%c", string[strlen(string)-1])); // -> last digit = weapon    
		 if(strlen(string) <= 1){                              
			 damage = 0;                    
		 }else{        
			 //calculate damage     
			 string[strlen(string)-1] = '\0'; // we remove the weapon from the string and replace with a null terminator.   
			 damage = atoi(string);                   
		 }                    
		 cgs.clientinfo[client].health = atoi(CG_Argv(i * 9 + 3));  
		 cgs.clientinfo[client].armor = atoi(CG_Argv(i * 9 + 4));     
		 cgs.clientinfo[client].location = atoi(CG_Argv(i * 9 + 5));   
		 cgs.clientinfo[client].view = atoi(CG_Argv(i * 9 + 6)); // 1st/3rd person  
		 cgs.clientinfo[client].admin = atoi(CG_Argv(i * 9 + 7));           
		 cgs.clientinfo[client].damage = damage; // 2k3's weird way of joining it in strings   
		 cgs.clientinfo[client].weapon = weapon;                       
		 cgs.clientinfo[client].mute = atoi(CG_Argv(i * 9 + 9)); //CJJ - 1.04.2005 - For showing MUTED on scoreboard        
		 cgs.clientinfo[client].clanMember = atoi(CG_Argv(i * 9 + 10));  //GodOrDevil - 1.06.2005 - For showing CLAN on scoreboard
		}      
	 }
	else if(cgs.rpmServer > 0.6) //GodOrDevil - Display this if new server 1.06.2005 
	{
	for(i = 0; i < count; i++)
		{
			client = atoi(CG_Argv(i * 9 + 2));
			if ((client < 0) || (client > cgs.maxclients))
			{
				Com_Printf("^3Bad Info String.\n");
				break;
			}
			cgs.clientinfo[client].health = atoi(CG_Argv(i * 9 + 3));
			cgs.clientinfo[client].armor = atoi(CG_Argv(i * 9 + 4));
			cgs.clientinfo[client].weapon = atoi(CG_Argv(i * 9 + 5));
			cgs.clientinfo[client].location = atoi(CG_Argv(i * 9 + 6));
			cgs.clientinfo[client].admin = atoi(CG_Argv(i * 9 + 7));
			cgs.clientinfo[client].referee = atoi(CG_Argv(i * 9 + 8));
			cgs.clientinfo[client].mute = atoi(CG_Argv(i * 9 + 9));			//CJJ - 1.04.2005 - For showing MUTED on scoreboard
			cgs.clientinfo[client].clanMember = atoi(CG_Argv(i * 9 + 10));	//GodOrDevil - 1.06.2005 - For showing CLAN on scoreboard
		}
	}
	
	else if( cgs.rpmServer >= 0.5 && cgs.rpmServer <= 0.6) //GodOrDevil - Display this if its an older RPM 1.06.2005
		{
		for(i = 0; i < count; i++)
			{
			client = atoi(CG_Argv(i * 7 + 2));
			if ((client < 0) || (client > cgs.maxclients))
				{
				Com_Printf("^3Bad Info String.\n");
				break;
				}
			cgs.clientinfo[client].health = atoi(CG_Argv(i * 7 + 3));
			cgs.clientinfo[client].armor = atoi(CG_Argv(i * 7 + 4));
			cgs.clientinfo[client].weapon = atoi(CG_Argv(i * 7 + 5));
			cgs.clientinfo[client].location = atoi(CG_Argv(i * 7 + 6));
			cgs.clientinfo[client].admin = atoi(CG_Argv(i * 7 + 7));
			cgs.clientinfo[client].referee = atoi(CG_Argv(i * 7 + 8));
			}
		}
	else if(cgs.rpmServer == 0.4)
		{
		for(i = 0; i < count; i++)
			{
			client = atoi(CG_Argv(i * 5 + 2));
			if ((client < 0) || (client > cgs.maxclients))
				{
				Com_Printf(S_COLOR_YELLOW"bad info\n");
				break; // bad info
				}
			cgs.clientinfo[client].health = atoi(CG_Argv(i * 5 + 3));
			cgs.clientinfo[client].armor = atoi(CG_Argv(i * 5 + 4));
			cgs.clientinfo[client].weapon = atoi(CG_Argv(i * 5 + 5));
			cgs.clientinfo[client].location = atoi(CG_Argv(i * 5 + 6));
			}
		}
	else{
		}
	}

/*
=============
RPM_First_Run
=============
*/
void RPM_FirstRun(void)
{
	trap_Cvar_Set("cg_drawCrosshairNames", "2");
	trap_Cvar_Set("cg_rpmFirstRun", "0");
}

/*
==================
RPM_ParseStats
==================
*/
void RPM_ParseStats (void)
{
	cg.stats.mins = atoi(CG_Argv(1));
	cg.stats.secs = atoi(CG_Argv(2));
	cg.stats.damageDone = atoi(CG_Argv(3));
	cg.stats.damageTaken =	atoi(CG_Argv(4));
	cg.stats.ratio = atof(CG_Argv(5));
	cg.stats.shotCount = atoi(CG_Argv(6));
	cg.stats.hitCount = atoi(CG_Argv(7));
	cg.stats.explosiveKills = atoi(CG_Argv(8));
	cg.stats.knifeKills = atoi(CG_Argv(9));
}

/*
==================
RPM_ParseAwardInfo
==================
*/
void RPM_ParseAwardInfo (void)
{
	int		i;

	for(i = 0; i < MAX_AWARDS; i++)
	{
		award[i].clientNum = atoi( CG_Argv( i * 2 + 1 ));
		Q_strncpyz(award[i].scoreValue, CG_Argv( i * 2 + 2 ), 16);
	}
}

/*
===========
RPM_ClientCleanName
Ripped from server code and alterd for RPM's needs
Removes colors from a players name for use in special functions
============
*/
void RPM_ClientCleanName ( const char *in, char *out, int outSize)
{
	int		len;
	char	ch;

	//save room for trailing null byte
	outSize--;

	len = 0;

	while( 1 )
	{
		ch = *in++;
		if( !ch )
		{
			break;
		}

		// check colors
		if( ch == Q_COLOR_ESCAPE )
		{
			// solo trailing carat is not a color prefix
			if( !*in )
			{
				break;
			}

			in++;
			continue;
		}

		if( len > outSize - 1 )
		{
			break;
		}

		*out++ = ch;
		len++;
	}

	*out = 0;
}



void NV_loadBSP	( const char *str)
{
	int			sub, j;
	char		temp[MAX_QPATH];
	vec3_t		mins, maxs;
	int			breakPoint = 0;

	for ( breakPoint = 1; breakPoint < MAX_MODELS - 1; breakPoint++ ){
		if (!cgs.inlineDrawModel[breakPoint])
			break;
	}
	if(breakPoint >= MAX_MODELS)
		return;

	trap_CM_LoadMap( str, qtrue );

	cgs.inlineDrawModel[breakPoint] = trap_R_RegisterModel( str );

	if(!cgs.inlineDrawModel[breakPoint])
		return;

	trap_R_ModelBounds( cgs.inlineDrawModel[breakPoint], mins, maxs );
	for ( j = 0 ; j < 3 ; j++ ) 
		cgs.inlineModelMidpoints[breakPoint][j] = mins[j] + 0.5 * ( maxs[j] - mins[j] );

	breakPoint++;
	cgs.numInlineModels++;
	cgs.bspCount++;

	///Load the models
	for(sub=1;sub<MAX_MODELS;sub++)
	{
		Com_sprintf(temp, MAX_QPATH, "*%d-%d", cgs.bspCount, sub);
		cgs.inlineDrawModel[breakPoint] = trap_R_RegisterModel( temp );
		
		if (!cgs.inlineDrawModel[breakPoint])
			continue;
		
		trap_R_ModelBounds( cgs.inlineDrawModel[breakPoint], mins, maxs );
			
		for ( j = 0 ; j < 3 ; j++ ) 
			cgs.inlineModelMidpoints[breakPoint][j] = mins[j] + 0.5 * ( maxs[j] - mins[j] );

		breakPoint++;
		cgs.numInlineModels++;
	}
}
