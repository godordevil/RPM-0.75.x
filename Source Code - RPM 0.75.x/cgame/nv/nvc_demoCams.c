#include "../cg_local.h"
#include "../../game/bg_local.h"
#include "../../ui/ui_shared.h"

extern displayContextDef_t *DC;

freeCams_t	cams[MAX_CAMS];

void DEMO_CreateCam (void)
{
	if(cg.demoCam.camCount > MAX_CAMS - 1)
	{	
		Com_Printf ( "^1Error: ^3Too many cameras! ^7(%i max)!\n", MAX_CAMS);
		// $todo : too many cams
		return;
	}
	cg.demoCam.camCount++;
	VectorCopy(cg.demoCam.camOrigin, cams[cg.demoCam.camCount].origin);
	VectorCopy(cg.demoCam.camView,  cams[cg.demoCam.camCount].view);
	cams[cg.demoCam.camCount].enabled = qtrue;
	cams[cg.demoCam.camCount].startTime = cg.time;
	cams[cg.demoCam.camCount].speed = 300;
	//cams[cg.demoCam.camCount].follow = cg.demoCam.camFollow;
	Com_Printf ( "^2CAM ^3#%i ^GCREATED\n", cg.demoCam.camCount);
}

void DEMO_RemoveCam (int camNum)
{
	memset(&cams[camNum], 0, sizeof(freeCams_t));
	Com_Printf ( "^2CAM ^3#%i ^_DELETED\n", camNum);
}

void DEMO_NextCam (void)
{
	int i;
	if(cg.demoCam.camCurrent > MAX_CAMS - 1)
		return;

	for (i = cg.demoCam.camCurrent + 1; i < MAX_CAMS; i++)
	{
		if(!cams[i].enabled)
			continue;
		VectorCopy(cams[i].origin, cg.demoCam.camOrigin);
		VectorCopy(cams[i].view, cg.demoCam.camView);
		cg.demoCam.camCurrent = i;
		//cg.demoCam.camFollow = cams[i].follow;
		if(cg.demoCam.camFollow)
		{
			cg.demoCam.camType = 4;
			cg.renderingThirdPerson = qtrue;
			cg.demoCam.camFollow = qtrue;
			cg.demoCam.camFree = qtrue;
		}
		Com_Printf ( "^2CAM ^3#%i\n", cg.demoCam.camCurrent);
		return;
	}
}

void DEMO_PrevCam (void)
{
	int i;
	if(cg.demoCam.camCurrent < 1)
		return;
	for (i = cg.demoCam.camCurrent - 1; i > 0; i--)
	{
		if(!cams[i].enabled)
			continue;
		VectorCopy(cams[i].origin, cg.demoCam.camOrigin);
		VectorCopy(cams[i].view, cg.demoCam.camView);
		cg.demoCam.camCurrent = i;
		//cg.demoCam.camFollow = cams[i].follow;
		if(cg.demoCam.camFollow)
		{
			cg.demoCam.camType = 4;
			cg.renderingThirdPerson = qtrue;
			cg.demoCam.camFollow = qtrue;
			cg.demoCam.camFree = qtrue;
		}

		Com_Printf ( "^2CAM ^3#%i\n", cg.demoCam.camCurrent);
		return;
	}
}
