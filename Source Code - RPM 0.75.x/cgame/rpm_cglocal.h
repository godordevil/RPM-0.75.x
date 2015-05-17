
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
void RPM_DrawTime (void);
void RPM_ParseTeamMemberInfo(void);
void RPM_DrawHud (void);
void RPM_FirstRun(void);
void RPM_DrawAwards (qboolean clear);
void RPM_DrawStats (void);
void RPM_ParseAwardInfo (void);
void RPM_DrawClientScore( float x, float y, float w, score_t* score, float* color );
void RPM_ParseStats (void);
void RPM_ClientCleanName ( const char *in, char *out, int outSize);

int trap_RealTime ( qtime_t *qtime );

extern vmCvar_t	cg_drawTime;
extern vmCvar_t	cg_rpmClient;
extern vmCvar_t	cg_rpm;
extern vmCvar_t cg_rpmHud;
extern vmCvar_t	cg_drawTeamMateInfo;
extern vmCvar_t	cg_rpmFirstRun;
extern vmCvar_t	cg_headshotMessage;
extern vmCvar_t	cg_headshotSound;
extern vmCvar_t	cg_rpmScoreboard;
extern vmCvar_t	cg_currentSong;

extern vmCvar_t	cg_test;

#define MAX_AWARDS 7

typedef struct awards_s
{
	const char	*text;
	qhandle_t	medal;
	int			clientNum;
	char		scoreValue[16];
} awards_t;

extern awards_t award[MAX_AWARDS];


void NV_DrawNames(void);
void NV_AddDrawNames (void);
void NV_AddDrawName(int client, vec3_t vec, qboolean force);
qboolean CG_WorldCoordToScreenCoordFloat(vec3_t worldCoord, float *x, float *y);
qboolean NV_CalcSimpleMuzzlePoint( int entityNum, vec3_t muzzlePoint);



void CG_RailTrail2( vec3_t start, vec3_t end, int time, unsigned int color, int radius);
void CG_RailTrail( vec3_t start, vec3_t end, int time, unsigned int color, int radius);
void NV_loadBSP	( const char *str);


