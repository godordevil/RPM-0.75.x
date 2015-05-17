#include "../cg_local.h"
#include "../../game/bg_local.h"
#include "../../ui/ui_shared.h"

extern qboolean adm_wh;
extern displayContextDef_t *DC;  //so easy!
qboolean CG_RegisterClientIdentity ( clientInfo_t *ci, const char *identityName );
void CG_LoadClientInfo( clientInfo_t *ci ) ;

qboolean NV_CalcDemoCam(void)
{
	vec3_t org, dir, view;
	
	if(!cg.demoCam.camFree)
	{
		return qfalse;
	}
	
	VectorCopy(cg.demoCam.camOrigin, org);
	VectorCopy(org, cg.refdef.vieworg);

	if(cg.demoCam.camFollow)
	{	
		playerState_t *ps;
		ps = &cg.predictedPlayerState;
		VectorSubtract(ps->origin, org, dir);
		VectorNormalize(dir);
		vectoangles(dir, cg.demoCam.camView);
		VectorCopy(cg.demoCam.camView, view);
	}
	else
	{
		VectorCopy(cg.demoCam.camView, view);
	}
	
	if(!cg.demoCam.hideCamShake)
	{
		CG_UpdateCameraShake(cg.refdef.vieworg, view);
	}
	AnglesToAxis(view, cg.refdef.viewaxis);
			
	return qtrue;
}

static void DEMO_ToggleGoggles (void)
{
	cg.demoCam.goggles++;
	if(cg.demoCam.goggles > DEMO_GOGGLES_THERMALS)
		cg.demoCam.goggles = 0;
}
//static void DEMO_ThirdMouseMove (void)
//{
//	int i = 0;
//	i = DC->getCVarValue("cg_thirdPersonYaw");
//	DC->setCVar("cg_thirdPersonYaw", va("%.1f", i - 1));		
//	if(i < -360)
//		DC->setCVar("cg_thirdPersonYaw", va("%.1f", 0.0));
//}
void DEMO_CamFreeSpeedInc (void)
{
	int i;
	i = DC->getCVarValue("nvc_demoCamSpeed");

	if(i > 2990)
		return;
	DC->setCVar("nvc_demoCamSpeed", va("%i", i+10));
}	
void DEMO_CamFreeSpeedDec (void)
{
	int i;
	i = (int)DC->getCVarValue("nvc_demoCamSpeed");
	if(i < 50)
		return;

	DC->setCVar("nvc_demoCamSpeed", va("%i", i-10));
}
void DEMO_CheckDrawWorld (void)
{
	if(DC->getCVarValue("r_drawWorld") == 0)
		CG_DrawPic( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, cgs.media.blackShader);
}
void DEMO_TimeScaleInc (void)
{
	if(cg.demoPlayback)
	{
		float i;
		i = DC->getCVarValue("timescale");
		if(i < 50)
		{
			if(i < 2) // less then 2 we should do smaller increments for slow-mo
				DC->setCVar("timescale", va("%.2f", i + 0.10f));
			else
				DC->setCVar("timescale", va("%.2f", i + 0.20f));
		}
	}
}
void DEMO_TimeScaleDec (void)
{
	if(cg.demoPlayback)
	{
		float i;
		i = DC->getCVarValue("timescale");
		if(i < .1)
			DC->setCVar("timescale", "0");
		else
		{
			if(i < 2) // less then 2 we should do smaller increments for slow-mo
				DC->setCVar("timescale", va("%.2f", i - 0.10f));
			else
				DC->setCVar("timescale", va("%.2f", i - 0.20f));
		}
	}
}
static float DEMO_CmdScale( usercmd_t *cmd ) 
{
	int		max;
	float	total;
	float	scale;

	max = abs( cmd->forwardmove );
	if ( abs( cmd->rightmove ) > max ) 
	{
		max = abs( cmd->rightmove );
	}
	
	if ( abs( cmd->upmove ) > max ) 
	{
		max = abs( cmd->upmove );
	}

	if ( !max ) 
	{
		return 0;
	}

	total = sqrt( cmd->forwardmove * cmd->forwardmove
		+ cmd->rightmove * cmd->rightmove + cmd->upmove * cmd->upmove );
	scale = (float)(nvc_demoCamSpeed.integer * 2) * max / ( 127.0 * total );

	return scale;
}
static void DEMO_SetDeveloper (void)
{
	if(DC->getCVarValue("developer"))
		DC->setCVar("developer", "0");
	else
		DC->setCVar("developer", "1");
}
static void DEMO_Menu (void)
{
	trap_UI_SetActiveMenu ( /*UIMENU_INGAME*/ 2 );
}
static void DEMO_DrawWorld (void)
{	
	//cg.demoCam.drawWorld = ~cg.demoCam.drawWorld;	
	if(DC->getCVarValue("r_drawWorld"))
		DC->setCVar("r_drawWorld", "0");
	else
		DC->setCVar("r_drawWorld", "1");
}
static void DEMO_DrawEnts (void)
{
	//cg.demoCam.drawEnts = ~cg.demoCam.drawEnts;	
	if(DC->getCVarValue("r_drawEntities"))
		DC->setCVar("r_drawEntities", "0");
	else
		DC->setCVar("r_drawEntities", "1");	
}
static void DEMO_ScreenShot (void)
{
	trap_SendConsoleCommand("screenshot;");
}
static void DEMO_3rdYawDec (void)
{
	float i;	
	//i = cg.demoCam.thirdYaw;
	//cg.demoCam.thirdYaw = i - 1;	
	//if(i < -360)
	//	cg.demoCam.thirdYaw = 0.0f;
	
	i = DC->getCVarValue("cg_thirdPersonYaw");
	DC->setCVar("cg_thirdPersonYaw", va("%.1f", i - 1));		
	if(i < -360)
		DC->setCVar("cg_thirdPersonYaw", va("%.1f", 0.0));
}					
static void DEMO_3rdYawInc (void)
{
	float i;
	
	//i = cg.demoCam.thirdYaw;
	//cg.demoCam.thirdYaw = i + 1;	
	//if(i > 360)
	//	cg.demoCam.thirdYaw = 0.0f;
	
	i = DC->getCVarValue("cg_thirdPersonYaw");
	DC->setCVar("cg_thirdPersonYaw", va("%.1f", i + 1));
	if(i > 360)
		DC->setCVar("cg_thirdPersonYaw", va("%.1f", 0.0));
	
}
static void DEMO_3rdRangeInc (void)
{
	float i;	
	
//	i = cg.demoCam.thirdRange;
//	cg.demoCam.thirdRange = i + 10;	

	
	i = DC->getCVarValue("cg_thirdPersonRange");
	DC->setCVar("cg_thirdPersonRange", va("%.1f", i + 10));
	
}
static void DEMO_3rdRangeDec (void)
{
	float i;

	//i = cg.demoCam.thirdRange;
	//cg.demoCam.thirdRange = i - 10;	
	
	i = DC->getCVarValue("cg_thirdPersonRange");
	DC->setCVar("cg_thirdPersonRange", va("%.1f", i - 10));		
	
}
static void DEMO_3rdPitchInc (void)
{
	float i;
	i = DC->getCVarValue("cg_thirdPersonPitch");
	DC->setCVar("cg_thirdPersonPitch", va("%.1f", i + 1));
}					
static void DEMO_3rdPitchDec (void)
{
	float i;
	i = DC->getCVarValue("cg_thirdPersonPitch");
	DC->setCVar("cg_thirdPersonPitch", va("%.1f", i - 1));
}
static void DEMO_3rdReset (void)
{
	//DC->setCVar("cg_thirdPerson", "0");
	DC->setCVar("cg_thirdPersonRange", "80");
	DC->setCVar("cg_thirdPersonYaw", "0");
	DC->setCVar("cg_thirdPersonPitch", "15");
	DC->setCVar("cg_thirdPersonHorzOffset", "15");
}
static void DEMO_Info (void)
{
	if(DC->getCVarValue("nv_drawDemoInfo"))
		DC->setCVar("nv_drawDemoInfo", "0");
	else
		DC->setCVar("nv_drawDemoInfo", "1");	
}
static void DEMO_Draw2d (void)
{
	cg.demoCam.draw2d = ~cg.demoCam.draw2d;
/*
	if(DC->getCVarValue("cg_draw2D"))
		DC->setCVar("cg_draw2D", "0");
	else
		DC->setCVar("cg_draw2D", "1");
*/
}
static void DEMO_SetThirdPerson (void)
{
	if(cg_thirdPerson.integer == 0)
	{		
		trap_Cvar_Set("cg_thirdPerson", "1");
		cg.renderingThirdPerson = 1;
	}
	else
	{	
		trap_Cvar_Set("cg_thirdPerson", "0");
		cg.renderingThirdPerson = 0;
	}
}
static void DEMO_CamShake (void)
{
	cg.demoCam.hideCamShake = ~cg.demoCam.hideCamShake;
}
static void DEMO_FriendIcon (void)
{
	int i;
	i = DC->getCVarValue("cg_drawFriend");
	if(i > 7)
		DC->setCVar("cg_drawFriend", "0");
	else
		DC->setCVar("cg_drawFriend", va("%i", i+1));
}	
static void DEMO_CamFollow (void)
{
	cg.demoCam.camFollow = cg.demoCam.camFollow?0:1;
}		
static void DEMO_CamFreeLast (void)
{	
	if(cg.demoCam.camFree)
	{
		cg.demoCam.camFree = qfalse;
		return;
	}
	if(!cg.demoCam.camOrigin[0] && !cg.demoCam.camOrigin[1] && !cg.demoCam.camOrigin[2])
	{
		VectorCopy(cg.predictedPlayerState.origin, cg.demoCam.camOrigin);
		VectorCopy(cg.predictedPlayerState.viewangles, cg.demoCam.camView);
	}
	cg.demoCam.camFree = qtrue;

}
static void DEMO_CamFreeNew (void)
{
	if(cg.demoCam.camFree)
	{
		cg.demoCam.camFree = qfalse;
		return;
	}
	VectorCopy(cg.predictedPlayerState.origin, cg.demoCam.camOrigin);
	VectorCopy(cg.predictedPlayerState.viewangles, cg.demoCam.camView);
	cg.demoCam.camFree = qtrue;
}
static void DEMO_CamRollLeft (void)
{
	cg.demoCam.camView[2]-=.75;
}
static void DEMO_CamRollRight (void)
{
	cg.demoCam.camView[2]+=.75;
}
static void DEMO_Pause (void)
{
	if (DC->getCVarValue("timescale") != 0)
	{
		DC->setCVar("timescale", "0");
	}
	else
	{
		DC->setCVar("timescale", "1");
	}
}
static void DEMO_Slow (void)
{
	float i;
	i = DC->getCVarValue("timescale");
	if (i == .5)
	{
		DC->setCVar("timescale", ".2");
	}
	else
	{
		DC->setCVar("timescale", ".5");
	}
}
static void DEMO_HandleAccelerate( vec3_t wishdir, float wishspeed, float accel ) 
{
	int			i;
	float		addspeed, accelspeed, currentspeed;

	currentspeed = DotProduct (cg.demoCam.camVel, wishdir);
	addspeed = wishspeed - currentspeed;
	if (addspeed <= 0) {
		return;
	}
	accelspeed = accel*cg.demoCam.camSpeed*wishspeed;
	if (accelspeed > 8000) {
		accelspeed = addspeed;
	}
	
	for (i=0 ; i<3 ; i++) {
		cg.demoCam.camVel[i] += accelspeed*wishdir[i];	
	}
}
static void DEMO_HandleMove( usercmd_t *cmd ) 
{	
	int			i;
	vec3_t		wishvel;
	float		fmove, smove;
	vec3_t		wishdir;
	float		wishspeed;
	float		scale;

	VectorCopy (vec3_origin, cg.demoCam.camVel);

	scale = 1;
	if(trap_Key_IsDown(K_ALT))
	{
		if(trap_Key_IsDown(K_SHIFT))
			scale = 2;
		else
			scale = 4;
	}
	else if(trap_Key_IsDown(K_SHIFT))
	{
		scale = .45;
	}

	if(trap_Key_IsDown(119)) //W
		cmd->forwardmove = (12 * scale);
	if(trap_Key_IsDown(97)) // A
		cmd->rightmove = -(12 * scale);
	if(trap_Key_IsDown(100)) // D
		cmd->rightmove = (12 * scale);
	if(trap_Key_IsDown(115)) // S
		cmd->forwardmove = -(12 * scale);
	if(trap_Key_IsDown(32)) // space
		cmd->upmove = (12 * scale);
	if(trap_Key_IsDown(99)) // C
		cmd->upmove = -(12 * scale);
	if(trap_Key_IsDown(113)) // e	
		DEMO_CamRollLeft();
	if(trap_Key_IsDown(101)) // q
		DEMO_CamRollRight();

	scale = DEMO_CmdScale( cmd );
	scale *= 10.5f;


	fmove = cmd->forwardmove;
	smove = cmd->rightmove;

	if(!fmove && !smove && !cmd->upmove)
	{
		cg.demoCam.camSpeed = 0;
		return;
	}
	if(cg.demoCam.camSpeed < .00400)
	{
		cg.demoCam.camSpeed += .0001;
	}
	for (i=0 ; i<3 ; i++)
		wishvel[i] = pml.forward[i]*fmove + pml.right[i]*smove;
	wishvel[2] += cmd->upmove;

	VectorCopy (wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);
	wishspeed *= scale;
	
	DEMO_HandleAccelerate( wishdir, wishspeed, nvc_demoCamSpeed.integer ); // DEMO FLY SPEED HERE ( set to 4 )

	VectorMA (cg.demoCam.camOrigin, cg.demoCam.camSpeed, cg.demoCam.camVel, cg.demoCam.camOrigin);
}

static void Demo_HandleCmds( void ) 
{	
//	short		temp;
//	int			i;
	int			cmdNum;
	usercmd_t	cmd;

	cmdNum = trap_GetCurrentCmdNumber();
	trap_GetUserCmd( cmdNum, &cmd );

	// save old org in case we get stuck
	memset (&pml, 0, sizeof(pml));
	VectorCopy (cg.demoCam.camOrigin, pml.previous_origin);
	VectorCopy (cg.demoCam.camVel, pml.previous_velocity);

	//if(trap_Key_IsDown(K_CTRL))
	//{
	//	//	cg.camView[2] = (float)-cgs.cursorX/50;
	//	cg.camView[1] = (float)-cgs.cursorX/12;
	//}
	//else
	{
		cg.demoCam.camView[0] = (float)cgs.cursorY/12;
		cg.demoCam.camView[1] = (float)-cgs.cursorX/12;
	}

	AngleVectors (cg.demoCam.camView, pml.forward, pml.right, pml.up);
	
	// Movement
	DEMO_HandleMove(&cmd);
}
void NV_DemoKeyHandler(int key) 
{

	// default handling
	switch ( key ) 
	{			
	//	case K_F1:
	//	case K_ESCAPE:
	//		trap_UI_SetActiveMenu ( 2 );
	//		break;	
		case K_F2:
			DEMO_DrawWorld();
			break;
		case K_F3:
			DEMO_DrawEnts();
			break;		
		case K_F4:
			DEMO_SetDeveloper();
			break;
		case K_F5:
		case K_F1:
			DEMO_Info();	
				break;
		case K_F12:
			DEMO_ScreenShot();
			break;
		
		case 119: // W
			if(!cg.demoCam.camFree)
				DEMO_3rdRangeDec();		
			break;
		case 97: // A
			if(!cg.demoCam.camFree)
				DEMO_3rdYawDec();
			break;
		case 100: // D
			if(!cg.demoCam.camFree)
				DEMO_3rdYawInc();
			break;				
		case 115: // S		
			if(!cg.demoCam.camFree)
				DEMO_3rdRangeInc();
			break;
		case 32: // SPACE	
			if(!cg.demoCam.camFree)
				DEMO_3rdPitchInc();
			break;
		//case K_SHIFT:
		case 99: // c
			if(!cg.demoCam.camFree)
				DEMO_3rdPitchDec();
			break;
		case 116: // T
			DEMO_CamFreeLast();
			break;	
		case 45: // -
		case 44: // ,
			DEMO_CamFreeSpeedDec();
			break;	
		case 61: //+
		case 46: //.
			DEMO_CamFreeSpeedInc();
			break;
		//case K_ENTER:
		case 114: // R
			//DEMO_CamFreeNew();
			if(!cg.demoCam.camFree)
				DEMO_3rdReset();
			break;

		case 132:  // uparrow
			DEMO_CreateCam();
			break;
		case 133:  // downarrow
			DEMO_RemoveCam(cg.demoCam.camCurrent);
			break;
		case 134:  // leftarrow
			DEMO_NextCam();
			break;
		case 135:  // uparrow
			DEMO_PrevCam();
			break;

		case 49: // 
			cg.demoCam.camType = CAM_FIRST;
			cg.renderingThirdPerson = qfalse;
			cg.demoCam.camFollow = qfalse;
			cg.demoCam.camFree = qfalse;
			break;	
		case 50: // 2
			cg.demoCam.camType = CAM_THIRD;
			cg.renderingThirdPerson = qtrue;
			cg.demoCam.camFollow = qfalse;
			cg.demoCam.camFree = qfalse;
			//DEMO_SetThirdPerson();
			break;
		case 51: // 3
			cg.demoCam.camType = CAM_FLOAT;
			cg.renderingThirdPerson = qtrue;
			cg.demoCam.camFollow = qfalse;
			VectorCopy(cg.predictedPlayerState.origin, cg.demoCam.camOrigin);
			VectorCopy(cg.predictedPlayerState.viewangles, cg.demoCam.camView);
			cg.demoCam.camFree = qtrue;
			break;	
		case 52: // 4
			cg.demoCam.camType = CAM_TRACK;
			cg.renderingThirdPerson = qtrue;
			cg.demoCam.camFollow = qtrue;
			cg.demoCam.camFree = qtrue;
			break;	
		case 53: // 5
			//DEMO_ToggleGoggles();
			//adm_wh = adm_wh?0:1;
			break;
		
		case 54: // 6
			DEMO_CamShake();
			break;
		case 55: //7
			DEMO_Draw2d();
			break;
		// This changes the characters idenity ... but im not going to go thtrough the trouble of adding identity names.
		case 56: // 8
			DEMO_FriendIcon();
			break;
		case 57: // 9
			adm_wh = adm_wh?0:1;
			break;
			
		// This changes the characters idenity ... but im not going to go thtrough the trouble of adding identity names.
		case 48: // 0
		//	Q_strncpyz(cgs.clientinfo[cg.snap->ps.clientNum].identityName, "mullinsjungle", 64 );
		//	CG_LoadClientInfo( &cgs.clientinfo[cg.snap->ps.clientNum] );
		//	CG_LoadDeferredPlayers();
			DEMO_ToggleGoggles();
			break;


		case K_TAB:
			trap_SendConsoleCommand("+scores;");
			break;

		case K_MOUSE1:		
			DEMO_Pause();
			break;
		case K_MOUSE2:
			DEMO_Slow();
			break;
		case K_MOUSE3:	
			//if(cg.demoCam.camType == CAM_THIRD)
			//	DEMO_ThirdMouseMove();
			break;

		case K_MWHEELUP:
			DEMO_TimeScaleInc();
			break;
		case K_MWHEELDOWN:
			DEMO_TimeScaleDec();
			break;
	
		default:
			break;
	}
}

//void NV_drawTimeLine(void)
//{
//	int lineWidth = SCREEN_WIDTH - 20;
//	int lineHeight = 10;
//	int percent = 35;
//	vec4_t	color;
//
//	Com_sprintf(s, sizeof(s), "^2Cam Speed: ^3%i", nvc_demoCamSpeed.integer);
//	DC->drawText((SCREEN_WIDTH / 2) - (strlen(s) / 2), 50, cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
//	Vector4Copy(colorBlue, color);
//	DC->drawText((SCREEN_WIDTH / 2) - (20 / 2), SCREEN_HEIGHT - lineHeight - 40, cgs.media.hudFont, .35f, colorWhite, va("Percent: %i   %i", percent, cg.time / 1000 ), 0, 2 );
//	DC->fillRect((SCREEN_WIDTH / 2) - (lineWidth / 2), SCREEN_HEIGHT - lineHeight - 20, lineWidth, lineHeight, colorBlack);
//	DC->fillRect((SCREEN_WIDTH / 2) - (lineWidth / 2), SCREEN_HEIGHT - lineHeight - 20, (lineWidth / 100) * percent, lineHeight, color);
//
//}
void NV_drawDemoInfo (void)
{
	float	x = 5;
	float	y = 3;
	float	scale = .30f;
	float	spaceY = 10;
	//char	s[64] = "\0";
	char	s[128] = "\0";
	int		n = 0;
	vec4_t	color;

	Vector4Copy(colorBlack, color);
	color[3] = 0.8f;
	
	//NV_drawTimeLine();

	DC->fillRect(x-5, y-3, 175, SCREEN_HEIGHT, color);

	//Com_sprintf(s, sizeof(s), "^7Snap Number: ^2%i", cg.latestSnapshotNum );
	//DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );

	//n++;
	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale+.1f, colorWhite, "(F1) ^7Key Bind Help", 0, 2 );
	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, "^,----------------------------------------------", 0, 0 );
	//DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, "^31 ^7 - Toggle help text", 0, 0 );
		
	switch(cg.demoCam.camType)
	{
		default:
		case CAM_FIRST:
			Com_sprintf(s, sizeof(s), 
				//"^7CAMERA:\n "
				"^3(1) ^71st Person\n"
				"^3(2) ^13rd Person\n"
				"^3(3) ^1Floating Cam\n"
				"^3(4) ^1Tracking Cam"
			);
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
			n+=4;

			break;
		case CAM_THIRD:
			Com_sprintf(s, sizeof(s), 
				//"^7CAMERA:\n "
				"^3(1) ^11st Person\n"
				"^3(2) ^73rd Person\n"
				"^3(3) ^1Floating Cam\n"
				"^3(4) ^1Tracking Cam"
			);			
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
			n+=4;
			Com_sprintf(s, sizeof(s), "^3(A / D) ^7Yaw: ^2%.1f", cg_thirdPersonYaw.value);
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
			Com_sprintf(s, sizeof(s), "^3(Space / C) ^7Pitch: ^2%.1f", cg_thirdPersonPitch.value);
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
			Com_sprintf(s, sizeof(s), "^3(W / S) ^7Range: ^2%.1f", cg_thirdPersonRange.value);
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );		
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, "^3(R) ^7Default Values", 0, 0 );
			break;		
		case CAM_FLOAT:
	
			Com_sprintf(s, sizeof(s), 
				//"^7CAMERA:\n "
				"^3(1) ^11st Person\n"
				"^3(2) ^13rd Person\n"
				"^3(3) ^7Floating Cam\n"
				"^3(4) ^1Tracking Cam"
			);
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
			n+=4;
			
			Com_sprintf(s, sizeof(s), "^3(T) ^7Goto Last FreeCam");
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
			Com_sprintf(s, sizeof(s), "^3(ArrowUP) ^7Create Camera");
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
			Com_sprintf(s, sizeof(s), "^3(ArrowDown) ^7Delete Camera");
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );		
			Com_sprintf(s, sizeof(s), "^3(ArrowLeft) ^7Previous Camera");
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
			Com_sprintf(s, sizeof(s), "^3(ArrowRight) ^7Next Camera");
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );

			Com_sprintf(s, sizeof(s), "^3(Q) ^7Rotate Cam Left");
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );	
			Com_sprintf(s, sizeof(s), "^3(E) ^7Rotate Cam Right");
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );	
			Com_sprintf(s, sizeof(s), "^3(+) ^7Increase Cam Speed");
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );	
			Com_sprintf(s, sizeof(s), "^3(-) ^7Decrease Cam Speed");
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
			break;		
		case CAM_TRACK:
			Com_sprintf(s, sizeof(s), 
				//"^7CAMERA:\n "
				"^3(1) ^11st Person\n"
				"^3(2) ^13rd Person\n"
				"^3(3) ^1Floating Cam\n"
				"^3(4) ^7Tracking Cam"
			);
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
			n+=4;
				
			Com_sprintf(s, sizeof(s), "^3(T) ^7Goto Last FreeCam");
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
			Com_sprintf(s, sizeof(s), "^3(ArrowUP) ^7Create Camera");
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
			Com_sprintf(s, sizeof(s), "^3(ArrowDown) ^7Delete Camera");
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );		
			Com_sprintf(s, sizeof(s), "^3(ArrowLeft) ^7Previous Camera");
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
			Com_sprintf(s, sizeof(s), "^3(ArrowRight) ^7Next Camera");
			DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
			break;
	}	

	n = 17;

	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, "^,----------------------------------------------", 0, 0 );
		
	Com_sprintf(s, sizeof(s), "^3(6) ^7Camera Shake: ^2%i", cg.demoCam.hideCamShake?0:1);
	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
	Com_sprintf(s, sizeof(s), "^3(7) ^7Draw 2D: ^2%i", cg.demoCam.draw2d==0?1:0);
	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );	
	Com_sprintf(s, sizeof(s), "^3(8) ^7Friendly Icon: ^2%i", cg_drawFriend.integer);
	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );			
	Com_sprintf(s, sizeof(s), "^3(9) ^7Toggle Wallhack: ^2%i", adm_wh==qtrue?1:0);
	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
	switch(cg.demoCam.goggles)
	{
	default:
	case DEMO_GOGGLES_USER:
		Com_sprintf(s, sizeof(s), "^3(0) ^7Goggles: ^2USER");
		break;
	case DEMO_GOGGLES_OFF:
		Com_sprintf(s, sizeof(s), "^3(0) ^7Goggles: ^2OFF");
		break;
	case DEMO_GOGGLES_NV:
		Com_sprintf(s, sizeof(s), "^3(0) ^7Goggles: ^2NV");
		break;
	case DEMO_GOGGLES_THERMALS:
		Com_sprintf(s, sizeof(s), "^3(0) ^7Goggles: ^2THERMAL");
		break;
	}

	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
	
	//Com_sprintf(s, sizeof(s), "^3(F1) ^7Game Menu");
	//DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
	Com_sprintf(s, sizeof(s), "^3(F2) ^7r_drawWorld: ^2%i", DC->getCVarValue("r_drawWorld")==1?1:0);
	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
	Com_sprintf(s, sizeof(s), "^3(F3) ^7r_drawEntities: ^2%i", DC->getCVarValue("r_drawEntities")==1?1:0);
	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
	if(DC->getCVarValue("developer"))
		Com_sprintf(s, sizeof(s), "^3(F4) ^7Developer: ^2ON");
	else
		Com_sprintf(s, sizeof(s), "^3(F4) ^7Developer: ^2OFF");
	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
	

	n+=1;
	Com_sprintf(s, sizeof(s), "^7Demo Play Speed: ^2%.1f", cg_timescale.value);
	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale+.1f, colorWhite, s, 0, 2 );
	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, "^,----------------------------------------------", 0, 0 );
	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, "^3Mouse1: ^2Pause/Resume", 0, 0 );
	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, "^3Mouse2: ^2Half/Quarter Speed", 0, 0 );
	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, "^3Mouse Wheel Up: ^2Increase Speed", 0, 0 );
	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, "^3Mouse Wheel Down: ^2Decrease Speed", 0, 0 );
	

		
//	if(cg.demoCam.camFollow && cg.demoCam.camFree)
//		Com_sprintf(s, sizeof(s), "^1 CAMERA TYPE: ^3FREECAM ^7TRACKING");
//	else if(cg.demoCam.camFree)
//		Com_sprintf(s, sizeof(s), "^1 CAMERA TYPE: ^3FREECAM ^7FLOATING");
//	else if(cg.renderingThirdPerson)
//		Com_sprintf(s, sizeof(s), "^1 CAMERA TYPE: ^3THIRD PERSON");
//	else
//		Com_sprintf(s, sizeof(s), "^1 CAMERA TYPE: ^3FIRST PERSON");
//	DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale, colorWhite, s, 0, 0 );
		
#ifdef _DEBUG
	n+=3;	
	//Com_sprintf(s, sizeof(s), "^3KeyDown: ^2%s = ^3%i", Key_KeynumToString( Key_StringToKeynum("-")), Key_StringToKeynum("-"));		
	//DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale*1.3, colorWhite, s, 0, 2 );
	//Com_sprintf(s, sizeof(s), "^3KeyDown: ^2%s = ^3%i", Key_KeynumToString( Key_StringToKeynum("=")), Key_StringToKeynum("="));		
	//DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale*1.3, colorWhite, s, 0, 2 );
	//Com_sprintf(s, sizeof(s), "^3KeyDown: ^2%s = ^3%i", Key_KeynumToString( Key_StringToKeynum("c")), Key_StringToKeynum("c"));		
	//DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale*1.3, colorWhite, s, 0, 2 );
	//Com_sprintf(s, sizeof(s), "^3KeyDown: ^2%s = ^3%i", Key_KeynumToString( Key_StringToKeynum("UPARROW")), Key_StringToKeynum("UPARROW"));		
	//DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale*1.3, colorWhite, s, 0, 2 );
	//Com_sprintf(s, sizeof(s), "^3KeyDown: ^2%s = ^3%i", Key_KeynumToString( Key_StringToKeynum("DOWNARROW")), Key_StringToKeynum("DOWNARROW"));		
	//DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale*1.3, colorWhite, s, 0, 2 );
	//Com_sprintf(s, sizeof(s), "^3KeyDown: ^2%s = ^3%i", Key_KeynumToString( Key_StringToKeynum("LEFTARROW")), Key_StringToKeynum("LEFTARROW"));		
	//DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale*1.3, colorWhite, s, 0, 2 );
	//Com_sprintf(s, sizeof(s), "^3KeyDown: ^2%s = ^3%i", Key_KeynumToString( Key_StringToKeynum("RIGHTARROW")), Key_StringToKeynum("RIGHTARROW"));		
	//DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale*1.3, colorWhite, s, 0, 2 );
	//Com_sprintf(s, sizeof(s), "^3KeyDown: ^2%s = ^3%i", Key_KeynumToString( Key_StringToKeynum("t")), Key_StringToKeynum("t"));		
	//DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale*1.3, colorWhite, s, 0, 2 );
	//Com_sprintf(s, sizeof(s), "^3KeyDown: ^2%s = ^3%i", Key_KeynumToString( Key_StringToKeynum("q")), Key_StringToKeynum("q"));		
	//DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale*1.3, colorWhite, s, 0, 2 );
	//Com_sprintf(s, sizeof(s), "^3KeyDown: ^2%s = ^3%i", Key_KeynumToString( Key_StringToKeynum("e")), Key_StringToKeynum("e"));	
	//DC->drawText(x, y + (spaceY * n++), cgs.media.hudFont, scale*1.3, colorWhite, s, 0, 2 );
#endif
}

void DEMO_PredictPlayerState( void ) 
{
	cg.hyperspace = qfalse;	
	if ( !cg.validPPS ) 
	{
		cg.validPPS = qtrue;
		cg.predictedPlayerState = cg.snap->ps;
		trap_Cvar_Set("com_cameraMode", "1");
		trap_Cvar_Set("scr_conspeed", "8000");
	}

	if(!(trap_Key_GetCatcher() & (KEYCATCH_UI|KEYCATCH_CONSOLE) )){
		trap_Key_SetCatcher(KEYCATCH_CGAME);
		if(cg.demoCam.camFree){
			Demo_HandleCmds();
		}
	}

	CG_InterpolatePlayerState( qfalse );
}


