// Copyright (C) 2001-2002 Raven Software.
//
// gt_local.h -- local definitions for gametype module

#include "../../game/q_shared.h"
#include "../gt_public.h"
#include "../gt_syscalls.h"

typedef struct gametypeLocals_s
{
	int		time;

	int		caseTakenSound;
	int		caseCaptureSound;
	int		caseReturnSound;

	//#dem
	int			bombBeepTime;

	int			bombPlantTime;
	vec3_t		bombPlantOrigin;
	char		bombPlantTarget[MAX_QPATH];
	
	qboolean	firstFrame;

	int			bombExplodeEffect;
	int			bombBeepSound;
	int			bombTakenSound;
	int			bombExplodedSound;
	int			bombPlantedSound;
	int			bombDropped;
	int			roundExpired;

	//int			iconBombPlanted[7];

	int			bombGiveClient;
	int			bombPlantClient;

	qboolean	roundOver;
	//End
	//#sab
	int			bombPlantedTeam;
	int			bombPlantCount[2];
	//end

	qboolean	paused;

} gametypeLocals_t;

extern	gametypeLocals_t	gametype;

