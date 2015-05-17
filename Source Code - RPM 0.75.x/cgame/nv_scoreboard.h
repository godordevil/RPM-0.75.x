//
#include "cg_local.h"
//
#define MAX_SB_ITEMS 30
typedef struct {
  float x;    // horiz position
  float y;    // vert position
  float w;    // width
  float h;    // height;
} rectDef_t;
typedef rectDef_t Rectangle;

typedef struct 
{
	Rectangle	rect;                 // client coord rectangle
	Rectangle	rectClient;           // screen coord rectangle
	const char	*name;               //
	//const char	*group;              // if it belongs to a group
	//const char	*cinematicName;		  // cinematic name
	//int			cinematic;								  // cinematic handle
	int			style;                      //
	int			border;                     //
	int			ownerDraw;									// ownerDraw style
	int			ownerWidth;
	int			ownerDrawFlags;		  // show flags for ownerdraw items
	const char* ownerDrawParam;
	float		borderSize;           // 
	int			flags;                // visible, focus, mouseover, cursor
	Rectangle	rectEffects;          // for various effects
	Rectangle	rectEffects2;         // for various effects
	int			offsetTime;           // time based value for various effects
	int			nextTime;             // time next effect should cycle
	vec4_t		foreColor;            // text color
	vec4_t		backColor;            // border color
	vec4_t		borderColor;          // border color
	vec4_t		outlineColor;         // border color
	qhandle_t	background;           // background asset  

} windowDef_t;
typedef windowDef_t Window;

#define ITEM_ALIGN_LEFT		0		// left alignment
#define ITEM_ALIGN_CENTER	1		// center alignment
#define ITEM_ALIGN_RIGHT	2		// right alignment

#define CVAR_ENABLE		0x00000001
#define CVAR_DISABLE	0x00000002
#define CVAR_SHOW			0x00000004
#define CVAR_HIDE			0x00000008

typedef struct itemDef_s {
	Window		window;						// common positional, border, style, layout info
	Rectangle	textRect;					// rectangle the text ( if any ) consumes     
	int			type;						// text, button, radiobutton, checkbox, textfield, listbox, combo
	int			alignment;					// left center right
	int			textalignment;				// ( optional ) alignment for text within rect based on text width
	float		textalignx;					// ( optional ) text alignment x coord
	float		textaligny;					// ( optional ) text alignment x coord
	float		textscale;					// scale percentage from 72pts
	int			textStyle;					// ( optional ) style, normal and shadowed are it for now
	qhandle_t	textFont;					// Font to render text with
	const char	*text;						// display text
	const char	*text2;						// display text, 2nd line
	float		text2alignx;				// ( optional ) text2 alignment x coord
	float		text2aligny;				// ( optional ) text2 alignment y coord
	void		*parent;					// menu owner
	qhandle_t	asset;						// handle to asset
	const char	*mouseEnterText;			// mouse enter script
	const char	*mouseExitText;				// mouse exit script
	const char	*mouseEnter;				// mouse enter script
	const char	*mouseExit;					// mouse exit script 
	const char	*action;					// select script
	const char	*onFocus;					// select script
	const char	*leaveFocus;				// select script
	const char	*cvar;						// associated cvar 
	const char	*cvarTest;					// associated cvar for enable actions
	const char	*enableCvar;				// enable, disable, show, or hide based on value, this can contain a list
	int			cvarFlags;					//	what type of action to take on cvarenables
	sfxHandle_t focusSound;
	int			numColors;					// number of color ranges
	colorRangeDef_t colorRanges[MAX_COLOR_RANGES];
	float		special;					// used for feeder id's etc.. diff per type
	int			cursorPos;					// cursor position in characters
	void		*typeData;					// type specific data ptr's	
	const char	*descText;					//	Description text
	int			appearanceSlot;				// order of appearance
	const char  *tooltip;					// tooltip that pops up for this item
	char		hotKey;						// hotkey that activates button
} itemDef_t;

typedef struct {
	Window window;
	const char  *font;						// font
	qboolean fullScreen;					// covers entire screen 
	int itemCount;							// number of items;
	int fontIndex;							// 
	int cursorItem;							// which item as the cursor
	int fadeCycle;							//
	float fadeClamp;						//
	float fadeAmount;						//
	const char *onOpen;						// run when the menu is first opened
	const char *onClose;					// run when the menu is closed
	const char *onESC;						// run when the menu is closed
	const char *soundName;					// background loop sound for menu

	vec4_t focusColor;						// focus color for items
	vec4_t disableColor;					// focus color for items
	itemDef_t *items[MAX_MENUITEMS];		// items this menu contains   
	int			descX;						// X position of description
	int			descY;						// X position of description
	vec4_t		descColor;					// description text color for items
	int			descAlignment;				// Description of alignment
	float		appearanceTime;				//	when next item should appear
	int			appearanceCnt;				//	current item displayed
	float		appearanceIncrement;		//

	vec4_t		tooltipForeColor;			// foreground color for tooltips
	vec4_t		tooltipBackColor;			// background color for tooltips
	float		tooltipScale;				// scale of text for tooltips
	int			tooltipDelay;
	qhandle_t	tooltipFont;				// font to render tooltip text with

} menuDef_t;
