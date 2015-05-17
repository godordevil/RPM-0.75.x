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


awards_t award[MAX_AWARDS];

extern int		sb_lineHeight;
extern int		sb_maxClients;
extern float	sb_nameFontScale;
extern float	sb_numberFontScale;
extern float	sb_readyFontScale;
extern float    sb_statFontScale;
extern float    sb_statFontSize;

/*
==============
RPM_DrawTime
==============
*/
void RPM_DrawTime (void)
{
	qtime_t currrent;
	char time[16];
	int s;

	trap_RealTime (&currrent);

	if (cg_drawTime.integer == 2)
	{
		Com_sprintf(time, 16, "%2i:%02i", currrent.tm_hour, currrent.tm_min);
	}
	else
	{
		if(currrent.tm_hour == 12)
		{
			Com_sprintf(time, 16, "%2i:%02iPM", currrent.tm_hour, currrent.tm_min);
		}

		else if (currrent.tm_hour > 12)
		{
			Com_sprintf(time, 16, "%2i:%02iPM", currrent.tm_hour - 12, currrent.tm_min);
		}

		else if(currrent.tm_hour == 0)
		{
			Com_sprintf(time, 16, "%2i:%02iAM", currrent.tm_hour + 12, currrent.tm_min);
		}

		else
		{
			Com_sprintf(time, 16, "%2i:%02iAM", currrent.tm_hour, currrent.tm_min);
		}
	}

	s = trap_R_GetTextWidth(time, cgs.media.hudFont, 0.53f, 0);
	CG_DrawText ( 635 - s, 175, cgs.media.hudFont, 0.53f, g_color_table[ColorIndex(COLOR_GREEN)], time, 0, DT_OUTLINE);
	return;
}
/*
==============
RPM_DrawHud
==============
*/
void RPM_DrawHud (void)
{
	static vec4_t	colorSelected	= { 1.0f, 1.0f, 1.0f, 1.0f };
	static vec4_t	colorShadow		= { 0.0f, 0.0f, 0.0f, 0.25f };

	qhandle_t  icon;
	int		   x = 0,y = 0,h = 0,w = 0;
	int        value;
	float      ammo_x, ammo_y;
	const char *ammo;
	const char *mode;
	int		   curMode;

	if((cg.predictedPlayerState.weapon < WP_KNIFE) || (cg.predictedPlayerState.weapon >= WP_NUM_WEAPONS))
	{
		return;
	}

	icon = cg_weapons[ cg.predictedPlayerState.weapon ].weaponIcon;

	if (CAT_GRENADE <= weaponData[cg.predictedPlayerState.weapon].category)
	{
		x = 310;
		y = 453;
		w = 25;
		h = 25;
	}
	else
	{
		switch (cg.predictedPlayerState.weapon)
		{
			case WP_AK74_ASSAULT_RIFLE:
				 {
					 x = 285;
					 y = 446;
					 w = 70;
					 h = 35;
					 break;
				 }
			case WP_M4_ASSAULT_RIFLE:
			case WP_M60_MACHINEGUN:
			case WP_MSG90A1:
			case WP_RPG7_LAUNCHER:
			case WP_M590_SHOTGUN:
				 {
					 x = 285;
					 y = 448;
					 w = 70;
					 h = 35;
					 break;
				 }
			case WP_USAS_12_SHOTGUN:
			case WP_MM1_GRENADE_LAUNCHER:
				{
					 x = 290;
					 y = 453;
					 w = 60;
					 h = 30;
					 break;
				 }
			case WP_MICRO_UZI_SUBMACHINEGUN:
			case WP_M3A1_SUBMACHINEGUN:
			case WP_KNIFE:
			case WP_M1911A1_PISTOL:
				 {
					 x = 295;
					 y = 453;
					 w = 50;
					 h = 25;
					 break;
				 }
			case WP_USSOCOM_PISTOL:
				 {
					 x = 299;
					 y = 457;
					 w = 42;
					 h = 21;
					 break;
				 }
			default:
				return;
		}
	}

	trap_R_SetColor( colorShadow );
	CG_DrawPic( x+2, y+2, w, h, icon );

	trap_R_SetColor( colorSelected );
	CG_DrawPic( x, y, w, h, icon );

	trap_R_SetColor( NULL );

	value = cg.predictedPlayerState.ammo[weaponData[cg.predictedPlayerState.weapon].attack[ATTACK_NORMAL].ammoIndex];

	// If the ammo value is crazy then dont display it
	if ( value <= -1 )
	{
		return;
	}

	ammo = va ( "%02d", value );

	ammo_y = trap_R_GetTextHeight ( ammo, cgs.media.hudFont, 0.35f, 0 ) - 1;
	CG_DrawText ( x + w + 3, 470, cgs.media.hudFont, 0.35f, colorShadow, ammo, 0, 0  );
	CG_DrawText ( x + w + 1, 468, cgs.media.hudFont, 0.35f, colorSelected, ammo, 0, 0  );

	value = cg.predictedPlayerState.clip[ATTACK_NORMAL][cg.predictedPlayerState.weapon];

	if ( value <= -1 )
	{
		return;
	}

	ammo = va ( "%02d", value );

	ammo_x = trap_R_GetTextWidth ( ammo, cgs.media.hudFont, 0.7f, 0 ) + 1;

	CG_DrawText ( x - ammo_x + 2, 460, cgs.media.hudFont, 0.7f, colorShadow, ammo, 0, 0  );
	CG_DrawText ( x - ammo_x, 458, cgs.media.hudFont, 0.7f, colorSelected, ammo, 0, 0  );

	if(cg.predictedPlayerState.weapon == WP_M4_ASSAULT_RIFLE )
	{
		value  = cg.predictedPlayerState.ammo[weaponData[cg.predictedPlayerState.weapon].attack[ATTACK_ALTERNATE].ammoIndex];
		value += cg.predictedPlayerState.clip[ATTACK_ALTERNATE][cg.predictedPlayerState.weapon];
		if(value)
		{
			ammo = va ( "%02d", value );

			trap_R_SetColor( colorShadow );
			CG_DrawPic ( x + w +39, 457, 25, 25, cgs.media.rpm40mmShader );

			trap_R_SetColor( colorSelected );
			CG_DrawPic ( x + w +37, 455, 25, 25, cgs.media.rpm40mmShader );

			trap_R_SetColor(NULL);
			CG_DrawText ( x + w + 42, 466, cgs.media.hudFont, 0.5f, colorShadow, ammo, 0, 0  );
			CG_DrawText ( x + w + 40, 464, cgs.media.hudFont, 0.5f, colorSelected, ammo, 0, 0  );
		}
	}

	switch (cg.predictedPlayerState.weapon)
	{
		case WP_M4_ASSAULT_RIFLE:
		case WP_AK74_ASSAULT_RIFLE:
		case WP_MICRO_UZI_SUBMACHINEGUN:
		case WP_USAS_12_SHOTGUN:
			{
				curMode = cg.predictedPlayerState.firemode[cg.predictedPlayerState.weapon];
				break;
			}
		default:
			return;
	}
	switch ( curMode )
	{
		case WP_FIREMODE_NONE:
			mode = "";
			break;
		case WP_FIREMODE_AUTO:
			mode = "A";
			break;
		case WP_FIREMODE_BURST:
			mode = "B";
			break;
		case WP_FIREMODE_SINGLE:
			mode = "S";
			break;
		default:
			// This shouldn't happen.
			assert(0);
			mode = "";
	}

	CG_DrawText (x + w + 7, 470 - ammo_y, cgs.media.hudFont, .35, colorShadow, mode, 0, 0 );
	CG_DrawText (x + w + 5, 468 - ammo_y, cgs.media.hudFont, .35, colorSelected, mode, 0, 0 );
}

/*
=============
RPM_DrawAwards
=============
*/
void RPM_DrawAwards (qboolean clear)
{
	static int			awardNum = 0, lastAwardTime = 0, awardGiven = 0;
	vec4_t				fade = {1.0, 1.0, 1.0, 0.0};
	float				x = 360, y = -20, w = 200, h = 40;
	float				picX = 360, picY = -19, picW = 20, picH = 40;
	float				tempX, addFade;
	static float		intensity[MAX_AWARDS] = {0};
	int					i;
	static int			fadeTime = 0;

	if(clear)
	{
		awardNum = 0;
		lastAwardTime = 0;
		awardGiven = 0;
		memset(intensity, 0, sizeof(intensity));
		fadeTime = 0;
		return;
	}

	if(!intensity[awardNum])
	{
		//start off with a fade amount of 0.01
		fadeTime = cg.time - 9;
	}

	if(awardNum < MAX_AWARDS)
	{
		addFade = (float)(cg.time - fadeTime) / (float)900;
		fadeTime = cg.time;

		if(intensity[awardNum] < 1.0)
		{
			intensity[awardNum] += addFade;
			//intensity[awardNum] += 0.01;
		}

		if(intensity[awardNum] > 0.4 && awardNum < MAX_AWARDS - 1)
		{
			intensity[awardNum + 1] += addFade;
			//intensity[awardNum + 1] += 0.01;
		}

		if(intensity[awardNum] >= 1.0)
		{
			intensity[awardNum] = 1.0;
			awardNum++;
		}
	}

//	trap_R_SetColor( NULL );

	for(i = 0; i < MAX_AWARDS; i++)
	{
	/*	if( i > awardNum + 1)
		{
			break;
		}
*/
		y += 45;
		picY += 45;
		fade[3] = intensity[i];

		trap_R_SetColor ( fade );
		CG_DrawPic( x, y, w, h, cgs.media.blackShader);
		CG_DrawPic( picX, picY, picW, picH, award[i].medal );

		//simplicity
		tempX = x + ( w / 2 ) - (trap_R_GetTextWidth ( award[i].text, cgs.media.hudFont, 0.40f, 0 ) / 2);

		CG_DrawText(tempX, y + 2, cgs.media.hudFont, 0.40f, fade, award[i].text, 0, 0);
	}

	trap_R_SetColor( NULL );

	y = -20;

	if(intensity[MAX_AWARDS - 1] >= 1.0)
	{
		for(i = 0; i < awardGiven || awardGiven == 0; i++)
		{
			y += 45;

			if(awardGiven < MAX_AWARDS && lastAwardTime < cg.time)
			{
				trap_S_StartLocalSound ( cgs.media.rifleShot, CHAN_AUTO );
				lastAwardTime = cg.time + 700;
				awardGiven++;
			}

			if(atof(award[i].scoreValue))
			{
				tempX = x + ( w / 2 ) - (trap_R_GetTextWidth ( va("%s  %s", cgs.clientinfo[award[i].clientNum].cleanName /*cgs.clientinfo[award[i].clientNum].name*/, award[i].scoreValue), cgs.media.hudFont, 0.35f, 0 ) / 2);

				CG_DrawText(tempX, y + 24, cgs.media.hudFont, 0.35f, fade, va("%s  %s", cgs.clientinfo[award[i].clientNum].cleanName /*cgs.clientinfo[award[i].clientNum].name*/, award[i].scoreValue), 0, 0);
			}
			else
			{
				tempX = x + ( w / 2 ) - (trap_R_GetTextWidth ( "None", cgs.media.hudFont, 0.35f, 0 ) / 2);

				CG_DrawText(tempX, y + 24, cgs.media.hudFont, 0.35f, fade, "None", 0, 0);
			}
		}
	}

	RPM_DrawStats();
}

/*
==================
RPM_DrawStats
==================
*/
void RPM_DrawStats (void)
{
	int					i = 0, overallScore;
	float				x = 50, y = 20;
	score_t*			scores;
	char				*name, *statText = 0, statValue[32];


	name = cgs.clientinfo[cg.clientNum].cleanName /*cgs.clientinfo[cg.clientNum].name*/;
	scores = &cg.scores[cg.scoreNum];


	CG_DrawText(x, y, cgs.media.hudFont, 0.50f, colorWhite, va("%s^7's Stats", name) , 0, 0);
	y += 13;
	CG_DrawText(x, y, cgs.media.hudFont, 0.50f, colorRed, "------------------------------------------------------------------------------" , 0, 0);

	do
	{
		i++;
		switch (i)
		{
			case 1:
				statText = "Time Played:";
				Com_sprintf(statValue, 32, "%d Mins : %d Secs", cg.stats.mins, cg.stats.secs );
				break;
			case 2:
				statText = "Score:";
				Com_sprintf(statValue, 32, "%d", scores->score );
				break;
			case 3:
				statText = "Kills:";
				Com_sprintf(statValue, 32, "%d", scores->kills );
				break;
			case 4:
				statText = "Deaths:";
				Com_sprintf(statValue, 32, "%d", scores->deaths );
				break;
			case 5:
				statText = "Ratio:";
				Com_sprintf(statValue, 32, "%.2f:1", cg.stats.ratio );
				break;
			case 6:
				statText = "Shots Fired:";
				Com_sprintf(statValue, 32, "%d", cg.stats.shotCount );
				break;
			case 7:
				statText = "Shots Hit:";
				Com_sprintf(statValue, 32, "%d", cg.stats.hitCount );
				break;
			case 8:
				statText = "Headshots:";
				Com_sprintf(statValue, 32, "%d", scores->headshots );
				break;
			case 9:
				statText = "Accuracy:";
				Com_sprintf(statValue, 32, "%.2f%%", scores->accuracy );
				break;
			case 10:
				statText = "Damage Done:";
				Com_sprintf(statValue, 32, "%i", cg.stats.damageDone );
				break;
			case 11:
				statText = "Damage Taken:";
				Com_sprintf(statValue, 32, "%i", cg.stats.damageTaken );
				break;
			case 12:
				statText = "Explosive Kills:";
				Com_sprintf(statValue, 32, "%i", cg.stats.explosiveKills );
				break;
			case 13:
				statText = "Knife Kills:";
				Com_sprintf(statValue, 32, "%i", cg.stats.knifeKills );
				i = 0;
				break;
			default:
				i = 0;
				break;
		}

		y += 20;
		x = 150 - trap_R_GetTextWidth ( statText, cgs.media.hudFont, 0.45f, 0 );
		CG_DrawText(x, y, cgs.media.hudFont, 0.45f, colorWhite, va("%s  %s", statText, statValue) , 0, 0);

	}while (i);

	overallScore = scores->score + (int)(100 * (scores->accuracy + cg.stats.ratio))  + (cg.stats.damageDone - cg.stats.damageTaken);

	x = 50;
	y += 20;

	CG_DrawText(x, y, cgs.media.hudFont, 0.50f, colorRed, "------------------------------------------------------------------------------" , 0, 0);

	y += 13;

	CG_DrawText(x, y, cgs.media.hudFont, 0.50f, colorWhite, va("Overall Score:  %d", overallScore) , 0, 0);
}

/*
=================
RPM_DrawClientScore
=================
*/
void RPM_DrawClientScore( float x, float y, float w, score_t* score, float* color )
{
	clientInfo_t*	ci;
	vec4_t			dataColor;
	vec4_t			nameColor;
	const char*		s;
	char			loc[16];
	float			f;

	nameColor[3] = dataColor[3] = 1.0f;

	// Validate the score
	if ( score->client < 0 || score->client >= cgs.maxclients )
	{
		Com_Printf( "Bad score->client: %i\n", score->client );
		return;
	}

	// Convienience
	ci = &cgs.clientinfo[score->client];

	CG_DrawPic ( x - 5, y, w, sb_lineHeight, cgs.media.scoreboardLine );

	// highlight your position
	if ( score->client == cg.snap->ps.clientNum )
	{
		vec4_t hcolor;

		hcolor[0] = 1.0f;
		hcolor[1] = 1.0f;
		hcolor[2] = 1.0f;
		hcolor[3] = .10f;

		CG_FillRect( x - 5, y, w, sb_lineHeight, hcolor );

		VectorSet ( nameColor, 1.0f, 1.0f, 1.0f );
		VectorSet ( dataColor, 0.5f, 0.5f, 0.5f );
	}
	else if ( (cg.snap->ps.pm_type == PM_DEAD) && score->client == cg.snap->ps.persistant[PERS_ATTACKER] )
	{
		vec4_t hcolor;

		hcolor[0] = 1.0f;
		hcolor[1] = 1.0f;
		hcolor[2] = 1.0f;
		hcolor[3] = .10f;

		CG_FillRect( x - 5, y, w, sb_lineHeight, hcolor );

		VectorCopy ( color, nameColor );
		VectorSet ( dataColor, 0.5f, 0.5f, 0.5f );
	}
	else
	{
		VectorCopy ( color, nameColor );
		VectorSet ( dataColor, 0.3f, 0.3f, 0.3f );

		if ( ci->ghost )
		{
			VectorScale ( nameColor, 0.6f, nameColor );
		}
	}

	CG_DrawText( x, y + 2, cgs.media.hudFont, sb_nameFontScale, nameColor, ci->name, 24, DT_OUTLINE );

	s = va("%i", score->score );
	f = trap_R_GetTextWidth ( s, cgs.media.hudFont, sb_nameFontScale + 0.10f, 0 );
	CG_DrawText( x + w - 10 - f, y, cgs.media.hudFont, sb_nameFontScale + 0.10f, nameColor, va("%i", score->score), 0, DT_OUTLINE );

	// Draw skull if dead and not in intermission
/*	if ( cg.snap->ps.pm_type == PM_INTERMISSION  )
	{
		if ( cg.snap->ps.stats[ STAT_CLIENTS_READY ] & ( 1 << score->client ) )
		{
			vec3_t deadColor;
			deadColor[0] = 0.60f;
			deadColor[1] = 0.60f;
			deadColor[2] = 0.60f;
			deadColor[3] = 1.0f;
			CG_DrawText( x + w - 60, y + 3, cgs.media.hudFont, sb_readyFontScale, deadColor, "READY", 0, DT_OUTLINE );
		}
	}
	else*/ if ( ci->ghost )
	{
		CG_DrawPic ( x + w - 60, y + 1, sb_numberFontScale * 0.8f, sb_numberFontScale * 0.8f, cgs.media.deadShader );
	}
	// Draw any gametype items the guy is carrying
	else
	{
		float xx = x + w - 60;
		int   i;
		for ( i = 0; i < MAX_GAMETYPE_ITEMS; i ++ )
		{
			centity_t* cent;

			cent = CG_GetEntity ( score->client);

			// No have item, no draw it
			if ( !(ci->gametypeitems & (1<<i) ) )
			{
				continue;
			}

			if ( !cg_items[MODELINDEX_GAMETYPE_ITEM+i].icon )
			{
				continue;
			}

			CG_DrawPic ( xx, y + 1, sb_numberFontScale * 0.8f, sb_numberFontScale * 0.8f, cg_items[MODELINDEX_GAMETYPE_ITEM+i].icon );

			xx += sb_numberFontScale;
		}
	}
	//GodOrDevil - Added more status to the scoreboard - 1.12.2005
	/*
	if(ci->admin == 1)
	{
	CG_DrawText( x + w - 110, y + 3, cgs.media.hudFont, sb_readyFontScale - 0.02f, colorMagenta, "^7*^-Suspended^7*", 0, DT_OUTLINE );
	}
	else if(ci->admin == 2)
	{
		CG_DrawText( x + w - 110, y + 3, cgs.media.hudFont, sb_readyFontScale - 0.02f, colorMagenta, "^7*^yB-Admin^7*", 0, DT_OUTLINE );
	}
	else if(ci->admin == 3)
	{
		CG_DrawText( x + w - 110, y + 3, cgs.media.hudFont, sb_readyFontScale - 0.02f, colorMagenta, "^7*^6Admin^7*", 0, DT_OUTLINE );
	}
	else if(ci->admin == 4)
	{
		CG_DrawText( x + w - 110, y + 3, cgs.media.hudFont, sb_readyFontScale - 0.02f, colorMagenta, "^7*^SS-Admin^7*", 0, DT_OUTLINE );
	}
	else if(ci->referee)
	{
		CG_DrawText( x + w - 105, y + 3, cgs.media.hudFont, sb_readyFontScale - 0.02f, colorYellow, "Referee", 0, DT_OUTLINE );
	}
	//CJJ - 1.04.2005 - Showing Mute on the Scoreboard
	if(ci->mute)
	{
		CG_DrawText( x + w - 105, y + 11, cgs.media.hudFont, sb_readyFontScale - 0.02f, colorRed, "^1MUTED", 0, DT_OUTLINE );
	}
	//CJJ - #END

	if(ci->clanMember)
	{	
	CG_DrawText( x + w - 105, y + 19, cgs.media.hudFont, sb_statFontSize, colorGreen, "^4Clan Member", 0, DT_OUTLINE );
	}
	*/
	//End

	s = va("%i/%i", score->kills, score->deaths);
	f = trap_R_GetTextWidth ( s, cgs.media.hudFont, sb_readyFontScale, 0 );
	CG_DrawText( x + w - 10 - f, y + sb_numberFontScale, cgs.media.hudFont, sb_readyFontScale, dataColor, s, 0, 0 );

	//GodOrDevil - Added more Status to the scoreboard - 1.15.2005
	if (cgs.rpmServer > 0.6 )
	{
		if ( ci->clanMember )
		{
			CG_DrawText( x + 180, y + 3, cgs.media.hudFont, sb_statFontSize, dataColor, "^,[^7C^,]", 0, 0 );
		}
		if ( ci->admin == 2)
		{
			CG_DrawText( x + 192, y + 3, cgs.media.hudFont, sb_statFontSize, dataColor, "^,[^iB^,]", 0, 0 );
		}
		if ( ci->admin == 3 )
		{
			CG_DrawText( x + 192, y + 3, cgs.media.hudFont, sb_statFontSize, dataColor, "^,[^6A^,]", 0, 0 );
		}
		if ( ci->admin == 4)
		{
			CG_DrawText( x + 192, y + 3, cgs.media.hudFont, sb_statFontSize, dataColor, "^,[^SS^,]", 0, 0 );
		}
		if ( ci->referee == 1)
		{
			CG_DrawText( x + 192, y + 3, cgs.media.hudFont, sb_statFontSize, dataColor, "^,[^3R^,]", 0, 0 );
		}
		if ( ci->mute )
		{
			CG_DrawText( x + 170, y + 3, cgs.media.hudFont,sb_statFontSize, colorRed, "^_M", 0, 0 );
		}
	}
	else if (cgs.rpmServer > 0.4)
	{
		if(ci->admin == 1)
		{
			CG_DrawText( x + 180, y + 3, cgs.media.hudFont, sb_statFontSize, dataColor, "^,[^6A^,]", 0, DT_OUTLINE );
		}
		else if(ci->referee)
		{
			CG_DrawText( x + 180, y + 3, cgs.media.hudFont, sb_statFontSize, dataColor, "^,[^3R^,]", 0, DT_OUTLINE );
		}
	}
	//End

	if ( score->teamkillDamage )
	{
		CG_DrawText( x + 140, y+5, cgs.media.hudFont, sb_statFontSize/1.5f, dataColor, va("tk: %i%%", score->teamkillDamage), 0, 0 );
	}

	y += sb_statFontScale;
	/*	GodOrDevil - Made a little room
	CG_DrawText( x, y, cgs.media.hudFont, sb_statFontSize, dataColor, va("ping: %i", score->ping), 0, 0 );
	CG_DrawText( x + 50, y, cgs.media.hudFont, sb_statFontSize, dataColor, va("accy: %.2f%%", score->accuracy), 0, 0 );
	CG_DrawText( x + 110, y, cgs.media.hudFont, sb_statFontSize, dataColor, va("hs: %i", score->headshots), 0, 0 );
	*/
	CG_DrawText( x, y, cgs.media.hudFont, sb_statFontSize, dataColor, va("ping: %i", score->ping), 0, 0 );
	CG_DrawText( x + 40, y, cgs.media.hudFont, sb_statFontSize, dataColor, va("accy: %.1f%%", score->accuracy), 0, 0 );
		
	// draw locations if theres not a lot of players
	if(cgs.rpmServer >= 1.0 && sb_maxClients != 16 && ((score->team == cgs.clientinfo[cg.snap->ps.clientNum].team && cgs.clientinfo[cg.clientNum].team != TEAM_FREE) || cg.demoPlayback || cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR))
	{			
		s = CG_ConfigString(CS_LOCATIONS + ci->location);
		if (!s || !*s) 
		{
			s = "unknown";

		}
		Q_strncpyz( loc, s, 16 );
		CG_DrawText( x + 105, y, cgs.media.hudFont, sb_statFontSize, dataColor, loc, 0, 0 );
	}

	y += trap_R_GetTextHeight ( "9", cgs.media.hudFont, sb_statFontSize, 0 );
		
	CG_DrawText( x, y, cgs.media.hudFont, sb_statFontSize, dataColor, va("id: %i", score->client), 0, 0 );
	CG_DrawText( x + 40, y, cgs.media.hudFont, sb_statFontSize, dataColor, va("time: %i", score->time), 0, 0 );
	CG_DrawText( x + 105, y, cgs.media.hudFont, sb_statFontSize, dataColor, va("hs: %i", score->headshots), 0, 0 );
	
	if(cgs.allowThirdPerson != 0 && cgs.rpmServer >= 1.0)
		CG_DrawText( x + 145, y, cgs.media.hudFont, sb_statFontSize, dataColor, va("%s", ci->view?"3rd":"1st"), 0, 0 );

	if((score->team == cgs.clientinfo[cg.snap->ps.clientNum].team || cg.demoPlayback || cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR) && ci->weapon > WP_NONE && ci->weapon < WP_NUM_WEAPONS && cg_weapons[ci->weapon].weaponIcon)
	{	
		trap_R_SetColor( colorDkGrey );
		if(sb_maxClients == 16)
			CG_DrawPic ( x + 180, y-4, 18, 9, cg_weapons[ci->weapon].weaponIcon );
		else
			CG_DrawPic ( x + 180, y-3, 22, 11, cg_weapons[ci->weapon].weaponIcon );
		trap_R_SetColor( NULL );
	}
}

void NV_crossHairHealth (float x, float y, float w, float h, vec4_t color)
{
	vec4_t	bgColor = {0,0, 0, 0.85f};

	w/=2;
	CG_FillRect ( x-.5f, y, (100 / 2)+1, h, bgColor );

	if(w >= 0.5){
		if(w <= 15) 
			CG_FillRect ( x+1, y+1, w-2, h-2, colorYellow );
		else
			CG_FillRect ( x+1, y+1, w-2, h-2, color );
	}
}
qboolean CG_IsDucked(int legsanim)
{
	switch(legsanim)
	{
	case LEGS_IDLE_CROUCH:
	case LEGS_WALK_CROUCH:
	case LEGS_WALK_CROUCH_BACK:
	case LEGS_LEAN_CROUCH_LEFT:
	case LEGS_LEAN_CROUCH_RIGHT:
	case LEGS_LEANLEFT_CROUCH_WALKLEFT:
	case LEGS_LEANLEFT_CROUCH_WALKRIGHT:
	case LEGS_LEANRIGHT_CROUCH_WALKLEFT:
	case LEGS_LEANRIGHT_CROUCH_WALKRIGHT:
		return qtrue;

	default:
		break;
	}
	
	return qfalse;
}
qboolean NV_CalcSimpleMuzzlePoint( int entityNum, vec3_t muzzlePoint)
{
	centity_t *cent;
	vec3_t forward;
	vec3_t right, up;
	int roll;

	if (entityNum == cg.snap->ps.clientNum)
	{
		float leanOffset;

		VectorCopy( cg.predictedPlayerState.origin, muzzlePoint);
		muzzlePoint[2] += cg.predictedPlayerState.viewheight;

		if (cg.predictedPlayerState.pm_flags & PMF_LEANING)
		{
			leanOffset = (float)(cg.predictedPlayerState.leanTime - LEAN_TIME) / LEAN_TIME * LEAN_OFFSET;
			AngleVectors(cg.predictedPlayerState.viewangles, NULL, right, up);
			VectorMA(muzzlePoint, leanOffset, right, muzzlePoint);
			VectorMA(muzzlePoint, -fabs(leanOffset / 4), up, muzzlePoint);
		}

		return qtrue;
	}

	cent = &cg_entities[entityNum];
	if (!cent->currentValid)
	{
		return qfalse;
	}

	//VectorCopy( cent->currentState.pos.trBase, muzzlePoint);
	VectorCopy( cent->lerpOrigin, muzzlePoint);

	//AngleVectors(cent->currentState.apos.trBase, forward, NULL, NULL);
	AngleVectors(cent->lerpOrigin, forward, NULL, NULL);
	if (CG_IsDucked(cent->currentState.legsAnim & ~ANIM_TOGGLEBIT))
	{
		muzzlePoint[2] += CROUCH_VIEWHEIGHT;
	}
	else
	{
		muzzlePoint[2] += DEFAULT_VIEWHEIGHT;
	}

	// Handle leaning
	roll = ((int)cent->currentState.origin2[0] >> 12) & 0xfff;
	if (roll > 180)
	{
		roll -= 360;
	}

	if (roll != 0)
	{
		//AngleVectors(cent->currentState.apos.trBase, NULL, right, up);
		AngleVectors(cent->lerpOrigin, NULL, right, up);
		VectorMA(muzzlePoint, roll, right, muzzlePoint);
		VectorMA(muzzlePoint, -fabs(roll / 4), up, muzzlePoint);
	}
	
	return qtrue;
}

void NV_AddDrawName(int client, vec3_t vec, qboolean force)
{
	cg.dnTime[client] = cg.time;
	VectorCopy(vec, cg.dnVecs[client]);
}
/*
void NV_AddDrawNames(void)
{
	int			i, j, len, height;
	float		x, y, scale, baseScale;
	float		teamColor[4], color[4];
	char		health[1], armor[1];
	clientInfo_t *ci;
	char		s1[80] = "\0";
	const char	*s;
	centity_t*	cent;
	int			outline, tCount, maxDistance;

	if(cg_drawFriend.integer == 0 || cg_drawFriend.integer == 2)
		return;
	if(cgs.gametypeData->teams == qfalse)
		return;

	if(cg.demoPlayback && cg.demoCam.camFree)
		return;

	baseScale = cg_drawFriendScale.value;

	if(baseScale < .15)
		baseScale = .15;

	tCount = CG_TeamCount(cgs.clientinfo[cg.clientNum].team);
	if(tCount > 22)
	{
		outline = 0;
		maxDistance = 3200;
	}
	else
	{		
		outline = 2;
		maxDistance = 5200;
	}

	for(i = 0; i < cgs.maxclients; i++)
	{
		if (cg.clientNum == i)
		{	// its us
			continue;
		}		
		
		if (cg.time - cg.dnTime[i] > 100)
			continue; // keep info on screen for up to 1.1 secs before clearing

		ci = &cgs.clientinfo[i];
		cent = CG_GetEntity (i);

		// Determine the color for this team
		switch (ci->team)
		{
		case TEAM_RED:
			VectorCopy4(g_color_table[ColorIndex(COLOR_RED)], teamColor);
			break;

		case TEAM_BLUE:
			VectorCopy4(g_color_table[ColorIndex(COLOR_BLUE)], teamColor);
			break;

		default:
			VectorCopy4(colorWhite, teamColor);
			break;
		}
		
		VectorCopy4(teamColor, color);
		scale = baseScale;

		color[3] = (cg_friendlyNameAlpha.value);
		
		if (CG_WorldCoordToScreenCoordFloat(cg.dnVecs[i], &x, &y))
		{	
			height = trap_R_GetTextHeight(ci->name, cgs.media.hudFont, scale, 0);
			
			//y -= 14;
			y -= height;

			if(y < 0 || y >= SCREEN_HEIGHT)
				continue;
			if(x < 0 || x > SCREEN_WIDTH)
				continue;

			if ( ci->team != cgs.clientinfo[cg.snap->ps.clientNum].team )
			{		
				//char nameColor[80] = "\0";
				Com_sprintf(s1, 80, "%s ^3[e]", ci->name);
				//s = nameColor;
			}
			else if(cg_friendlyNameColor.string[0])
			{
				if(cg_friendlyNameColor.string[0] == '0') // team color
				{
					//s = ci->name;
					Com_sprintf(s1, 64, "%s", ci->name);
				}
				else
				{
					//char nameColor[68] = "\0";
					Com_sprintf(s1, 65, "^%c%s", cg_friendlyNameColor.string[0], ci->cleanName);
					//s = nameColor;
				}
			}
			else
			{
				//s = ci->name;
				Com_sprintf(s1, 64, "%s", ci->name);
			}

			scale = baseScale;
			
			height = trap_R_GetTextHeight("W", cgs.media.hudFont, scale, 0);
			len = trap_R_GetTextWidth(s1, cgs.media.hudFont, scale, 32);

			if (cg_drawFriend.integer < 7)
			{
				CG_DrawText (x - (len/2), y, cgs.media.hudFont, scale, color, s1, 32, outline);							
			}
		
			if(ci->health < 50)
				health[0] = 'C';
			else
				health[0] = '1';
			if(ci->armor < 50)
				armor[0] = '7';
			else
				armor[0] = 'I';

			if (cg_drawFriend.integer == 3)
			{
				s = va("  ^%c%3d\n  ^%c%3d",  health[0], ci->health, armor[0], ci->armor);
				CG_DrawText (x + len/2, y-((height*.5)/2), cgs.media.hudFont, scale*.75, color, s, 0, outline);
			} 
			else if (cg_drawFriend.integer == 4 )
			{
				y += height + 1;
				s = va(S_COLOR_WHITE"^2[ ^%c%d ^2| ^%c%d^2 ]", health[0], ci->health, armor[0], ci->armor);
				len = trap_R_GetTextWidth(s, cgs.media.hudFont, scale*.7, 0);
				CG_DrawText (x - (len/2), y, cgs.media.hudFont, scale*.7, color, s, 0, outline);
			} 	
			else if (cg_drawFriend.integer == 5 )
			{		
				y += height + 2;
				if ((cent->pe.weapon > WP_NONE) && (cent->pe.weapon < WP_NUM_WEAPONS))
				{
					gitem_t *item;
					item = BG_FindWeaponItem(cent->pe.weapon);
					s = va(S_COLOR_WHITE"^2[ ^%c%d ^2| ^%c%d^2 ] ^7%s", health[0], ci->health, armor[0], ci->armor, bg_weaponNames[cent->pe.weapon]);
				}
				else 
				{
					s = va(S_COLOR_WHITE"^2[ ^%c%d ^2| ^%c%d^2 ]", health[0], ci->health, armor[0], ci->armor);
				}
				len = trap_R_GetTextWidth(s, cgs.media.hudFont, scale*.7, 0);
				CG_DrawText (x - (len/2), y, cgs.media.hudFont, scale*.7, color, s, 0, outline);

			}
			else if (cg_drawFriend.integer >= 6 )
			{	
				y += height + (5* scale);
				
				if( ci->armor)
					NV_crossHairHealth (x-25, y+3, ci->armor, 3, colorLtGrey);
				if(ci->team == TEAM_BLUE)
				{
					teamColor[0] -= .25f;
					teamColor[1] -= .2f;
					NV_crossHairHealth (x-25, y,   ci->health, 4, teamColor);
				}
				else
				{
					NV_crossHairHealth (x-25, y, ci->health, 4, colorRed);
				}
			}
			//if(nvc_drawFloatingItems.integer)
			{
				cent = &cg_entities[i]; 
				len = 0;
				height = ((scale*100));
		
				y -= (height * 1.5);
				x -= (float)((height) / 2);

				for (j = 0; j < MAX_GAMETYPE_ITEMS; j++)
				{
					if (!(cent->currentState.gametypeitems & (1<<j)))
						continue;

					if (cg_items[MODELINDEX_GAMETYPE_ITEM + j].icon)
					{
						CG_DrawPic(x, y, height, height, cg_items[MODELINDEX_GAMETYPE_ITEM + j].icon);
						break;
					}
				}
			}
		}
	}
}
*/


void NV_AddDrawNames(void)
{
	int			i, j, len, height;
	float		x, y, scale, baseScale;
	float		teamColor[4], color[4];
	char		health[1], armor[1];
	clientInfo_t *ci;
	char		s1[80] = "\0";
	const char	*s;
	centity_t*	cent;
	int			outline, tCount, maxDistance;

	if(cg_drawFriend.integer == 0 || cg_drawFriend.integer == 8)
		return;
	if(cgs.gametypeData->teams == qfalse)
		return;

	if(cg.demoPlayback && cg.demoCam.camFree)
		return;

	baseScale = cg_drawFriendScale.value;

	if(baseScale < .15)
		baseScale = .15;

	if(baseScale > 2.5)
		baseScale = 2.5;

	tCount = CG_TeamCount(cgs.clientinfo[cg.clientNum].team);
	if(tCount > 22)
	{
		outline = 0;
		maxDistance = 3200;
	}
	else
	{		
		outline = 2;
		maxDistance = 5200;
	}

	for(i = 0; i < cgs.maxclients; i++)
	{
		if (cg.clientNum == i)
		{	// its us
			continue;
		}		
		
		if (cg.time - cg.dnTime[i] > 100)
			continue; // keep info on screen for up to 1.1 secs before clearing

		ci = &cgs.clientinfo[i];
		cent = CG_GetEntity (i);

		// Determine the color for this team
		switch (ci->team)
		{
		case TEAM_RED:
			VectorCopy4(g_color_table[ColorIndex(COLOR_RED)], teamColor);
			break;

		case TEAM_BLUE:
			VectorCopy4(g_color_table[ColorIndex(COLOR_BLUE)], teamColor);
			break;

		default:
			VectorCopy4(colorWhite, teamColor);
			break;
		}
		
		VectorCopy4(teamColor, color);
		scale = baseScale;

		color[3] = (cg_friendlyNameAlpha.value);
		
		if (CG_WorldCoordToScreenCoordFloat(cg.dnVecs[i], &x, &y))
		{	
			height = trap_R_GetTextHeight(ci->name, cgs.media.hudFont, scale, 0);
			
			//y -= 14;
			y -= height;

			if(y < 0 || y >= SCREEN_HEIGHT)
				continue;
			if(x < 0 || x > SCREEN_WIDTH)
				continue;

			if ( ci->team != cgs.clientinfo[cg.snap->ps.clientNum].team )
			{		
				//char nameColor[80] = "\0";
				Com_sprintf(s1, 80, "%s ^3[e]", ci->name);
				//s = nameColor;
			}
			else if(cg_friendlyNameColor.string[0])
			{
				if(cg_friendlyNameColor.string[0] == '0') // team color
				{
					//s = ci->name;
					Com_sprintf(s1, 64, "%s", ci->name);
				}
				else
				{
					//char nameColor[68] = "\0";
					Com_sprintf(s1, 65, "^%c%s", cg_friendlyNameColor.string[0], ci->cleanName);
					//s = nameColor;
				}
			}
			else
			{
				//s = ci->name;
				Com_sprintf(s1, 64, "%s", ci->name);
			}

			scale = baseScale;
			
			height = trap_R_GetTextHeight("W", cgs.media.hudFont, scale, 0);
			len = trap_R_GetTextWidth(s1, cgs.media.hudFont, scale, 32);

			if (cg_drawFriend.integer < 7)
			{
				CG_DrawText (x - (len/2), y, cgs.media.hudFont, scale, color, s1, 32, outline);							
			}
		
			if(ci->health < 50)
				health[0] = 'C';
			else
				health[0] = '1';
			if(ci->armor < 50)
				armor[0] = '7';
			else
				armor[0] = 'I';

			if (cg_drawFriend.integer == 3)
			{
				s = va("  ^%c%3d\n  ^%c%3d",  health[0], ci->health, armor[0], ci->armor);
				CG_DrawText (x + len/2, y-((height*.5)/2), cgs.media.hudFont, scale*.75, color, s, 0, outline);
			} 
			else if (cg_drawFriend.integer == 4 )
			{
				y += height + 1;
				s = va(S_COLOR_WHITE"^2[ ^%c%d ^2| ^%c%d^2 ]", health[0], ci->health, armor[0], ci->armor);
				len = trap_R_GetTextWidth(s, cgs.media.hudFont, scale*.7, 0);
				CG_DrawText (x - (len/2), y, cgs.media.hudFont, scale*.7, color, s, 0, outline);
			} 	
			else if (cg_drawFriend.integer == 5 )
			{		
				y += height + 2;
				if ((cent->pe.weapon > WP_NONE) && (cent->pe.weapon < WP_NUM_WEAPONS))
				{
					gitem_t *item;
					item = BG_FindWeaponItem(cent->pe.weapon);
					s = va(S_COLOR_WHITE"^2[ ^%c%d ^2| ^%c%d^2 ] ^7%s", health[0], ci->health, armor[0], ci->armor, bg_weaponNames[cent->pe.weapon]);
				}
				else 
				{
					s = va(S_COLOR_WHITE"^2[ ^%c%d ^2| ^%c%d^2 ]", health[0], ci->health, armor[0], ci->armor);
				}
				len = trap_R_GetTextWidth(s, cgs.media.hudFont, scale*.7, 0);
				CG_DrawText (x - (len/2), y, cgs.media.hudFont, scale*.7, color, s, 0, outline);

			}
			else if (cg_drawFriend.integer >= 6 )
			{	
				//y += height + (5* scale);
				y -= height + (5* scale);
				
				//if( ci->armor)
				//	NV_crossHairHealth (x-25, y+3, ci->armor, 3, colorLtGrey);
				if(ci->team == TEAM_BLUE)
				{
					teamColor[0] -= .25f;
					teamColor[1] -= .2f;
					NV_crossHairHealth (x-25, y,   ci->health, 5, teamColor);
				}
				else
				{
					NV_crossHairHealth (x-25, y, ci->health, 5, colorRed);
				}
			}
			//if(nvc_drawFloatingItems.integer)
			{
				cent = &cg_entities[i]; 
				len = 0;
				height = ((scale*100));
		
				y -= (height * 1.5);
				x -= (float)((height) / 2);

				for (j = 0; j < MAX_GAMETYPE_ITEMS; j++)
				{
					if (!(cent->currentState.gametypeitems & (1<<j)))
						continue;

					if (cg_items[MODELINDEX_GAMETYPE_ITEM + j].icon)
					{
						CG_DrawPic(x, y, height, height, cg_items[MODELINDEX_GAMETYPE_ITEM + j].icon);
						break;
					}
				}
			}
		}
	}
}



void CG_RailTrail2( vec3_t start, vec3_t end, int time, unsigned int color, int radius) {
	localEntity_t	*le;
	refEntity_t		*re;

	le = CG_AllocLocalEntity();
	re = &le->refEntity;

	le->leType = LE_FADE_RGB;
	le->startTime = cg.time;
	le->endTime = cg.time + time;
	le->lifeRate = 1;

	re->shaderTime = 1;
	///re->shaderTime = cg.time / time;
	re->reType = RT_LINE;

	re->radius = radius * 0.3f;
	re->customShader = cgs.media.whiteShader; 
	//re->customShader = cgs.media.whiteShader; 
	//re->customShader = trap_R_RegisterShaderNoMip("textures/colombia/canvas_doublesided");
	//re->customShader = trap_R_RegisterShaderNoMip("textures/tools/_mirror");
	//if(!re->customShader)
	//	re->customShader = cgs.media.whiteShader; 


	VectorCopy( start, re->origin );
	VectorCopy( end, re->oldorigin );
	
	//color = 0x08;
	if (color==0){ //blue
		///re->shaderRGBA[0] = re->shaderRGBA[1] = re->shaderRGBA[2] = re->shaderRGBA[3] = 0x00;
		le->color[0] = 0;
		le->color[1] = 0;
		le->color[2] = 1;
	}
	else if (color==1) // red
	{
		le->color[0] = 1;
		le->color[1] = 0;
		le->color[2] = 0;
	}
	else if (color==2) // yaller
	{
		le->color[0] = 1;
		le->color[1] = 1;
		le->color[2] = 0;
	}
	else if (color==3) // white
	{
		le->color[0] = 1;
		le->color[1] = 1;
		le->color[2] = 1;
	}
	else if (color==4) // turquoise
	{
		le->color[0] = 0;
		le->color[1] = 1;
		le->color[2] = 1;
	}	
	else if (color==5) // purple
	{
		le->color[0] = 1;
		le->color[1] = 0;
		le->color[2] = 1;
	}	
	else // Green
	{
		//re->shaderRGBA[0] = color & 0xff;
		//color >>= 8;
		//re->shaderRGBA[1] = color & 0xff;
		//color >>= 8;
		//re->shaderRGBA[2] = color & 0xff;
		//re->shaderRGBA[3] = 0xff;

		le->color[0] = 0;
		le->color[1] = 1;
		le->color[2] = 0;
	}
	le->color[3] = 1.0f;

	AxisClear( re->axis );
}

void ///CG_RailTrail( clientInfo_t *ci, vec3_t start, vec3_t end)
CG_RailTrail( vec3_t start, vec3_t end, int time, unsigned int color, int radius) 
{	
	vec3_t	diff, v1, v2, v3, v4, v5, v6;

	VectorSubtract(start, end, diff);

	VectorCopy(start, v1);
	VectorCopy(start, v2);
	VectorCopy(start, v3);
	v1[0] -= diff[0];
	v2[1] -= diff[1];
	v3[2] -= diff[2];

	CG_RailTrail2( start, v1, time, color, radius);
	CG_RailTrail2( start, v2, time, color, radius);
	CG_RailTrail2( start, v3, time, color, radius);

	//CG_TestLine( start, v1, time, color, radius); 
	//CG_TestLine( start, v2, time, color, radius);
	//CG_TestLine( start, v3, time, color, radius);

	VectorCopy(end, v4);
	VectorCopy(end, v5);
	VectorCopy(end, v6);
	v4[0] += diff[0];
	v5[1] += diff[1];
	v6[2] += diff[2];

	CG_RailTrail2( end, v4, time, color, radius);
	CG_RailTrail2( end, v5, time, color, radius);
	CG_RailTrail2( end, v6, time, color, radius);
	CG_RailTrail2( v2, v6, time, color, radius);
	CG_RailTrail2( v6, v1, time, color, radius);
	CG_RailTrail2( v1, v5, time, color, radius);
	CG_RailTrail2( v2, v4, time, color, radius);
	CG_RailTrail2( v4, v3, time, color, radius);
	CG_RailTrail2( v3, v5, time, color, radius);


	//CG_TestLine( end, v4, time, color, radius); 
	//CG_TestLine( end, v5, time, color, radius);
	//CG_TestLine( end, v6, time, color, radius);
	//CG_TestLine( v2, v6, time, color, radius); 
	//CG_TestLine( v6, v1, time, color, radius);
	//CG_TestLine( v1, v5, time, color, radius);
	//CG_TestLine( v2, v4, time, color, radius); 
	//CG_TestLine( v4, v3, time, color, radius);
	//CG_TestLine( v3, v5, time, color, radius);
}
