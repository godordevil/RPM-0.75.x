
#ifdef _DEMO

#define	MAX_CAMS		32

#define CAM_FIRST 1
#define CAM_THIRD 2
#define CAM_FLOAT 3
#define CAM_TRACK 4

#define DEMO_GOGGLES_USER 0
#define DEMO_GOGGLES_OFF 1
#define DEMO_GOGGLES_NV 2
#define DEMO_GOGGLES_THERMALS 3

typedef struct {
	vec3_t		origin;
	vec3_t		view;		// if > 1, it is autorepeating
	int			startTime;
	float		speed;
	int			nextCam;
	qboolean	follow;
	qboolean	enabled;
} freeCams_t;

extern	freeCams_t	cams[MAX_CAMS];

void CG_InterpolatePlayerState( qboolean grabAngles );
int Key_StringToKeynum( char *str );
char *Key_KeynumToString( int keynum );

qboolean NV_CalcDemoCam(void);
void DEMO_CheckDrawWorld (void);
void NV_drawDemoInfo(void);
void DEMO_PredictPlayerState( void );

void DEMO_CreateCam (void);
void DEMO_RemoveCam (int camNum);
void DEMO_PrevCam (void);
void DEMO_NextCam (void);
void DEMO_TimeScaleInc (void);
void DEMO_TimeScaleDec (void);

void NV_DemoKeyHandler(int key);

#endif
