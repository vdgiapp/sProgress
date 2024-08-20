
/*
	sProgress by Giapp
	(based on progress2)
	Version: v1.0

	progress2: https://github.com/Southclaws/progress2
	textdraw-streamer: https://github.com/nexquery/samp-textdraw-streamer
	YSI-Includes: https://github.com/pawn-lang/YSI-Includes

	FUNCTIONS:
	+ bool:IsValidPlayerSProgress(playerid, barid)
	+ bool:IsPlayerSProgressVisible(playerid, barid)
	+ CreatePlayerSProgress(playerid, Float:x, Float:y, Float:w, Float:h, Float:max = 100.0, type = SP_BAR_TYPE_RIGHT)
	+ DestroyPlayerSProgress(playerid, barid)
	+ DestroyAllPlayerSProgress(playerid)
	+ (Show/Hide)PlayerSProgress(playerid, barid)
	+ (Get/Set)PlayerSProgressBackColor
	+ (Get/Set)PlayerSProgressFillColor
	+ (Get/Set)PlayerSProgressMainColor
	+ (Get/Set)PlayerSProgressSubColor
	+ (Get/Set)PlayerSProgressPos
	+ (Get/Set)PlayerSProgressOverride
	+ (Get/Set)PlayerSProgressType
	+ (Float:Get/Set)PlayerSProgressWidth
	+ (Float:Get/Set)PlayerSProgressHeight
	+ (Float:Get/Set)PlayerSProgressBorderSize
	+ (Float:Get/Set)PlayerSProgressMinValue
	+ (Float:Get/Set)PlayerSProgressMaxValue
	+ (Float:Get/Set)PlayerSProgressValue
	+ (Float:Get/Set)PlayerSProgressSubMinValue
	+ (Float:Get/Set)PlayerSProgressSubMaxValue
*/

#if defined _inc_s_progress
	#undef _inc_s_progress
#endif

#if defined _s_progress_included
	#endinput
#endif
#define _s_progress_included

#tryinclude <a_samp>
#tryinclude <open.mp>

#if !defined _INC_open_mp
	#if !defined PlayerTextDrawColour
		#define PlayerTextDrawColour PlayerTextDrawBoxColor
	#endif
#endif

#include <YSI_Data\y_iterate>
#include <YSI_Coding\y_hooks>

#if !defined MAX_SPROGRESS_BARS
	#define MAX_SPROGRESS_BARS (MAX_PLAYER_TEXT_DRAWS / 4) // 64
#endif

#define INVALID_SP_BAR_VALUE		(Float:0xFFFFFFFF)
#define INVALID_SP_BAR_ID			(-1)

// Type defines (direction)
#define	SP_BAR_TYPE_RIGHT (0) // left to right
#define SP_BAR_TYPE_LEFT (1) // right to left
#define SP_BAR_TYPE_UP (2) // bottom to top
#define SP_BAR_TYPE_DOWN (3) // top to bottom
#define SP_BAR_TYPE_MID_V (4) // middle vertical
#define SP_BAR_TYPE_MID_H (5) // middle horizontal

enum static E_SPROGRESS_DATA {
	bool:is_created,
	bool:sp_show,
	Float:sp_posX,
	Float:sp_posY,
	Float:sp_width,
	Float:sp_height,
	Float:sp_borderSize,
	sp_colorFill,
	sp_colorMain,
	sp_colorBack,
	sp_colorSub,
	bool:sp_override,
	Float:sp_minValue,
	Float:sp_maxValue,
	Float:sp_value,
	Float:sp_subMin,
	Float:sp_subMax,
	sp_type
};

enum static E_SPROGRESS_TD {
	PlayerText:sp_back,
	PlayerText:sp_fill,
	PlayerText:sp_sub,
	PlayerText:sp_main
};

static PlayerText:sp_TextDraw[MAX_PLAYERS][MAX_SPROGRESS_BARS][E_SPROGRESS_TD];
static bool:is_sprogress_initialised = false;
static Iterator:sp_Index[MAX_PLAYERS]<MAX_SPROGRESS_BARS>;
static sp_Data[MAX_PLAYERS][MAX_SPROGRESS_BARS][E_SPROGRESS_DATA];

static stock SProgressTD_ResetPlayerItem(playerid, barid) {
	sp_TextDraw[playerid][barid][sp_back] = INVALID_PLAYER_TEXT_DRAW;
	sp_TextDraw[playerid][barid][sp_fill] = INVALID_PLAYER_TEXT_DRAW;
	sp_TextDraw[playerid][barid][sp_sub] = INVALID_PLAYER_TEXT_DRAW;
	sp_TextDraw[playerid][barid][sp_main] = INVALID_PLAYER_TEXT_DRAW;
	sp_Data[playerid][barid][is_created] = false;
	return 1;
}

static stock SProgressTD_ResetPlayer(playerid) {
	for(new barid = 0; barid < MAX_SPROGRESS_BARS; ++barid) {
		SProgressTD_ResetPlayerItem(playerid, barid);
	}
	return 1;
}

static stock SProgressTD_ResetAll() {
	for(new playerid = 0; playerid < MAX_PLAYERS; ++playerid) {
		SProgressTD_ResetPlayer(playerid);
	}
	is_sprogress_initialised = true;
	return 1;
}

stock bool:IsValidPlayerSProgress(playerid, barid) {
	return Iter_Contains(sp_Index[playerid], barid);
}

stock bool:IsPlayerSProgressVisible(playerid, barid) {
	return (IsValidPlayerSProgress(playerid, barid) && sp_Data[playerid][barid][sp_show]);
}

stock ShowPlayerSProgress(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	sp_Data[playerid][barid][sp_show] = true;
	PlayerTextDrawShow(playerid, sp_TextDraw[playerid][barid][sp_back]);
	PlayerTextDrawShow(playerid, sp_TextDraw[playerid][barid][sp_fill]);
	if(!sp_Data[playerid][barid][sp_override]) {
		PlayerTextDrawShow(playerid, sp_TextDraw[playerid][barid][sp_sub]);
		PlayerTextDrawShow(playerid, sp_TextDraw[playerid][barid][sp_main]);
	}
	else {
		PlayerTextDrawShow(playerid, sp_TextDraw[playerid][barid][sp_main]);
		PlayerTextDrawShow(playerid, sp_TextDraw[playerid][barid][sp_sub]);
	}
	return 1;
}

stock HidePlayerSProgress(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	sp_Data[playerid][barid][sp_show] = false;
	PlayerTextDrawHide(playerid, sp_TextDraw[playerid][barid][sp_back]);
	PlayerTextDrawHide(playerid, sp_TextDraw[playerid][barid][sp_fill]);
	PlayerTextDrawHide(playerid, sp_TextDraw[playerid][barid][sp_sub]);
	PlayerTextDrawHide(playerid, sp_TextDraw[playerid][barid][sp_main]);
	return 1;
}

stock CreatePlayerSProgress(playerid, Float:x, Float:y, Float:w, Float:h, Float:max = 100.0, type = SP_BAR_TYPE_RIGHT) {
	if(!IsPlayerConnected(playerid)) {
		printf("[SProgress] ERROR: Attempt to create SProgress for invalid player");
		return INVALID_SP_BAR_ID;
	}
	if(!is_sprogress_initialised) SProgressTD_ResetAll();
	new barid = Iter_Free(sp_Index[playerid]);
	if(barid == ITER_NONE) {
		printf("[SProgress] ERROR: Limit reached.");
		return INVALID_SP_BAR_ID;
	}
	sp_Data[playerid][barid][is_created] = true;
	sp_Data[playerid][barid][sp_show] = false;
	sp_Data[playerid][barid][sp_posX] = x;
	sp_Data[playerid][barid][sp_posY] = y;
	sp_Data[playerid][barid][sp_width] = w;
	sp_Data[playerid][barid][sp_height] = h;
	sp_Data[playerid][barid][sp_borderSize] = 2.0;
	sp_Data[playerid][barid][sp_colorBack] = 0x000000FF;
	sp_Data[playerid][barid][sp_colorFill] = 0x9E9E9EFF;
	sp_Data[playerid][barid][sp_colorMain] = 0xFFFFFFFF;
	sp_Data[playerid][barid][sp_colorSub] = 0x00000000;
	sp_Data[playerid][barid][sp_override] = false;
	sp_Data[playerid][barid][sp_minValue] = 0.0;
	sp_Data[playerid][barid][sp_maxValue] = max;
	sp_Data[playerid][barid][sp_value] = 50.0;
	sp_Data[playerid][barid][sp_subMin] = 0.0;
	sp_Data[playerid][barid][sp_subMax] = 0.0;
	sp_Data[playerid][barid][sp_type] = type;

	sp_TextDraw[playerid][barid][sp_back] = CreatePlayerTextDraw(playerid, x-sp_Data[playerid][barid][sp_borderSize], y-sp_Data[playerid][barid][sp_borderSize], "LD_SPAC:white");
	PlayerTextDrawTextSize(playerid, sp_TextDraw[playerid][barid][sp_back], sp_Data[playerid][barid][sp_width]*2, sp_Data[playerid][barid][sp_height]*2);
	PlayerTextDrawAlignment(playerid, sp_TextDraw[playerid][barid][sp_back], TEXT_DRAW_ALIGN_LEFT);
	PlayerTextDrawColour(playerid, sp_TextDraw[playerid][barid][sp_back], sp_Data[playerid][barid][sp_colorBack]);
	PlayerTextDrawSetShadow(playerid, sp_TextDraw[playerid][barid][sp_back], 0);
	PlayerTextDrawSetOutline(playerid, sp_TextDraw[playerid][barid][sp_back], 0);
	PlayerTextDrawBackgroundColour(playerid, sp_TextDraw[playerid][barid][sp_back], 255);
	PlayerTextDrawFont(playerid, sp_TextDraw[playerid][barid][sp_back], TEXT_DRAW_FONT_SPRITE_DRAW);
	PlayerTextDrawSetProportional(playerid, sp_TextDraw[playerid][barid][sp_back], true);

	sp_TextDraw[playerid][barid][sp_fill] = CreatePlayerTextDraw(playerid, x, y, "LD_SPAC:white");
	PlayerTextDrawTextSize(playerid, sp_TextDraw[playerid][barid][sp_fill], sp_Data[playerid][barid][sp_width], sp_Data[playerid][barid][sp_height]);
	PlayerTextDrawAlignment(playerid, sp_TextDraw[playerid][barid][sp_fill], TEXT_DRAW_ALIGN_LEFT);
	PlayerTextDrawColour(playerid, sp_TextDraw[playerid][barid][sp_fill], sp_Data[playerid][barid][sp_colorFill]);
	PlayerTextDrawSetShadow(playerid, sp_TextDraw[playerid][barid][sp_fill], 0);
	PlayerTextDrawSetOutline(playerid, sp_TextDraw[playerid][barid][sp_fill], 0);
	PlayerTextDrawBackgroundColour(playerid, sp_TextDraw[playerid][barid][sp_fill], 255);
	PlayerTextDrawFont(playerid, sp_TextDraw[playerid][barid][sp_fill], TEXT_DRAW_FONT_SPRITE_DRAW);
	PlayerTextDrawSetProportional(playerid, sp_TextDraw[playerid][barid][sp_fill], true);

	sp_TextDraw[playerid][barid][sp_sub] = CreatePlayerTextDraw(playerid, x, y, "LD_SPAC:white");
	PlayerTextDrawTextSize(playerid, sp_TextDraw[playerid][barid][sp_sub], 0, 0);
	PlayerTextDrawAlignment(playerid, sp_TextDraw[playerid][barid][sp_sub], TEXT_DRAW_ALIGN_LEFT);
	PlayerTextDrawColour(playerid, sp_TextDraw[playerid][barid][sp_sub], sp_Data[playerid][barid][sp_colorSub]);
	PlayerTextDrawSetShadow(playerid, sp_TextDraw[playerid][barid][sp_sub], 0);
	PlayerTextDrawSetOutline(playerid, sp_TextDraw[playerid][barid][sp_sub], 0);
	PlayerTextDrawBackgroundColour(playerid, sp_TextDraw[playerid][barid][sp_sub], 255);
	PlayerTextDrawFont(playerid, sp_TextDraw[playerid][barid][sp_sub], TEXT_DRAW_FONT_SPRITE_DRAW);
	PlayerTextDrawSetProportional(playerid, sp_TextDraw[playerid][barid][sp_sub], true);

	sp_TextDraw[playerid][barid][sp_main] = CreatePlayerTextDraw(playerid, x, y, "LD_SPAC:white");
	PlayerTextDrawTextSize(playerid, sp_TextDraw[playerid][barid][sp_main], sp_Data[playerid][barid][sp_width], sp_Data[playerid][barid][sp_height]);
	PlayerTextDrawAlignment(playerid, sp_TextDraw[playerid][barid][sp_main], TEXT_DRAW_ALIGN_LEFT);
	PlayerTextDrawColour(playerid, sp_TextDraw[playerid][barid][sp_main], sp_Data[playerid][barid][sp_colorMain]);
	PlayerTextDrawSetShadow(playerid, sp_TextDraw[playerid][barid][sp_main], 0);
	PlayerTextDrawSetOutline(playerid, sp_TextDraw[playerid][barid][sp_main], 0);
	PlayerTextDrawBackgroundColour(playerid, sp_TextDraw[playerid][barid][sp_main], 255);
	PlayerTextDrawFont(playerid, sp_TextDraw[playerid][barid][sp_main], TEXT_DRAW_FONT_SPRITE_DRAW);
	PlayerTextDrawSetProportional(playerid, sp_TextDraw[playerid][barid][sp_main], true);

	Iter_Add(sp_Index[playerid], barid);
	SProgress_RenderBar(playerid, barid);
	return barid;
}

static stock _ptextdraw_destroy_array(playerid, const PlayerText:ptd_array[E_SPROGRESS_TD]) {
	for(new E_SPROGRESS_TD:ptd_slot = E_SPROGRESS_TD:0; ptd_slot < E_SPROGRESS_TD:4; ++ptd_slot) {
		PlayerTextDrawDestroy(playerid, ptd_array[ptd_slot]);
	}
}

stock DestroyPlayerSProgress(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	_ptextdraw_destroy_array(playerid, sp_TextDraw[playerid][barid]);
	SProgressTD_ResetPlayerItem(playerid, barid);
	Iter_Remove(sp_Index[playerid], barid);
	return 1;
}

stock DestroyAllPlayerSProgress(playerid) {
	for(new i = 0; i < MAX_SPROGRESS_BARS; i++) {
		DestroyPlayerSProgress(playerid, i);
	}
	return 1;
}

// Pos
stock GetPlayerSProgressPos(playerid, barid, &Float:x, &Float:y) {
	if(!IsValidPlayerSProgress(playerid, barid)) return INVALID_SP_BAR_VALUE;
	x = sp_Data[playerid][barid][sp_posX];
	y = sp_Data[playerid][barid][sp_posY];
	return 1;
}

stock SetPlayerSProgressPos(playerid, barid, Float:x, Float:y) {
	if(!IsValidPlayerSProgress(playerid, barid)) return INVALID_SP_BAR_VALUE;
	sp_Data[playerid][barid][sp_posX] = x;
	sp_Data[playerid][barid][sp_posY] = y;
	SProgress_RenderBar(playerid, barid);
	return 1;
}

// Width
stock Float:GetPlayerSProgressWidth(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return INVALID_SP_BAR_VALUE;
	return sp_Data[playerid][barid][sp_width];
}

stock SetPlayerSProgressWidth(playerid, barid, Float:w) {
	if(w < 0) {
		printf("[SProgress] ERROR: SetPlayerSProgressWidth must be greater or equal 0");
		return 0;
	}
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	sp_Data[playerid][barid][sp_width] = w;
	SProgress_RenderBar(playerid, barid);
	return 1;
}

// Height
stock Float:GetPlayerSProgressHeight(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return INVALID_SP_BAR_VALUE;
	return sp_Data[playerid][barid][sp_height];
}

stock SetPlayerSProgressHeight(playerid, barid, Float:h) {
	if(h < 0) {
		printf("[SProgress] ERROR: SetPlayerSProgressHeight must be greater or equal 0");
		return 0;
	}
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	sp_Data[playerid][barid][sp_height] = h;
	SProgress_RenderBar(playerid, barid);
	return 1;
}

// Border
stock Float:GetPlayerSProgressBorderSize(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return INVALID_SP_BAR_VALUE;
	return sp_Data[playerid][barid][sp_borderSize];
}

stock SetPlayerSProgressBorderSize(playerid, barid, Float:b) {
	if(b < 0) {
		printf("[SProgress] ERROR: SetPlayerSProgressBorderSize must be greater or equal 0");
		return 0;
	}
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	sp_Data[playerid][barid][sp_borderSize] = b;
	SProgress_RenderBar(playerid, barid);
	return 1;
}

// Override (sub bar override main bar)
stock bool:GetPlayerSProgressOverride(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return false;
	return sp_Data[playerid][barid][sp_override];
}

stock SetPlayerSProgressOverride(playerid, barid, bool:override) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	sp_Data[playerid][barid][sp_override] = override;
	SProgress_RenderBar(playerid, barid);
	return 1;
}

// Color
stock GetPlayerSProgressMainColor(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	return sp_Data[playerid][barid][sp_colorMain];
}

stock GetPlayerSProgressFillColor(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	return sp_Data[playerid][barid][sp_colorFill];
}

stock GetPlayerSProgressBackColor(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	return sp_Data[playerid][barid][sp_colorBack];
}

stock GetPlayerSProgressSubColor(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	return sp_Data[playerid][barid][sp_colorSub];
}

stock SetPlayerSProgressMainColor(playerid, barid, colour) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	sp_Data[playerid][barid][sp_colorMain] = colour;
	SProgress_RenderBar(playerid, barid);
	return 1;
}

stock SetPlayerSProgressFillColor(playerid, barid, colour) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	sp_Data[playerid][barid][sp_colorFill] = colour;
	SProgress_RenderBar(playerid, barid);
	return 1;
}

stock SetPlayerSProgressBackColor(playerid, barid, colour) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	sp_Data[playerid][barid][sp_colorBack] = colour;
	SProgress_RenderBar(playerid, barid);
	return 1;
}

stock SetPlayerSProgressSubColor(playerid, barid, colour) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	sp_Data[playerid][barid][sp_colorSub] = colour;
	SProgress_RenderBar(playerid, barid);
	return 1;
}

// Value
stock GetPlayerSProgressMinValue(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	return sp_Data[playerid][barid][sp_minValue];
}

stock GetPlayerSProgressMaxValue(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	return sp_Data[playerid][barid][sp_maxValue];
}

stock GetPlayerSProgressValue(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	return sp_Data[playerid][barid][sp_value];
}

stock GetPlayerSProgressSubMinValue(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	return sp_Data[playerid][barid][sp_subMin];
}

stock GetPlayerSProgressSubMaxValue(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	return sp_Data[playerid][barid][sp_subMax];
}

stock SetPlayerSProgressMinValue(playerid, barid, Float:val) {
	if(val < 0) {
		printf("[SProgress] ERROR: SetPlayerSProgressMinValue must be greater or equal 0");
		return 0;
	}
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	sp_Data[playerid][barid][sp_minValue] = val;
	sp_Data[playerid][barid][sp_value] =
		(sp_Data[playerid][barid][sp_value] <= val) ? val : sp_Data[playerid][barid][sp_value];
	SProgress_RenderBar(playerid, barid);
	return 1;
}

stock SetPlayerSProgressMaxValue(playerid, barid, Float:val) {
	if(val < 0) {
		printf("[SProgress] ERROR: SetPlayerSProgressMaxValue must be greater or equal 0");
		return 0;
	}
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	sp_Data[playerid][barid][sp_maxValue] = val;
	sp_Data[playerid][barid][sp_value] =
		(sp_Data[playerid][barid][sp_value] >= val) ? val : sp_Data[playerid][barid][sp_value];
	SProgress_RenderBar(playerid, barid);
	return 1;
}

stock SetPlayerSProgressValue(playerid, barid, Float:val) {
	if(val < 0) {
		printf("[SProgress] ERROR: SetPlayerSProgressValue must be greater or equal 0");
		return 0;
	}
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;

	new Float:min_value = sp_Data[playerid][barid][sp_minValue],
		Float:max_value = sp_Data[playerid][barid][sp_maxValue];
	
	if(val <= min_value) val = min_value;
	else if(val >= max_value) val = max_value;
	sp_Data[playerid][barid][sp_value] = val;

	SProgress_RenderBar(playerid, barid);
	return 1;
}

stock SetPlayerSProgressSubMinValue(playerid, barid, Float:val) {
	if(val < 0) {
		printf("[SProgress] ERROR: SetPlayerSProgressSubMinValue must be greater or equal 0");
		return 0;
	}
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	sp_Data[playerid][barid][sp_subMin] = val;
	SProgress_RenderBar(playerid, barid);
	return 1;
}

stock SetPlayerSProgressSubMaxValue(playerid, barid, Float:val) {
	if(val < 0) {
		printf("[SProgress] ERROR: SetPlayerSProgressSubMaxValue must be greater or equal 0");
		return 0;
	}
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	sp_Data[playerid][barid][sp_subMax] = val;
	SProgress_RenderBar(playerid, barid);
	return 1;
}

// Type (direction)
stock GetPlayerSProgressType(playerid, barid) {
	if(!IsValidPlayerSProgress(playerid, barid)) return SP_BAR_TYPE_RIGHT;
	return sp_Data[playerid][barid][sp_type];
}

stock SetPlayerSProgressType(playerid, barid, type) {
	if(type < SP_BAR_TYPE_RIGHT || type > SP_BAR_TYPE_MID_H) {
		printf("[SProgress] ERROR: Invalid SProgress type! Using SP_BAR_TYPE_RIGHT...");
		return SP_BAR_TYPE_RIGHT;
	}
	if(!IsValidPlayerSProgress(playerid, barid)) return 0;
	sp_Data[playerid][barid][sp_type] = type;
	SProgress_RenderBar(playerid, barid);
	return 1;
}

// use this instead of PlayerTextDrawSetPos to compatible with textdraw-streamer
static stock PlayerText:SProgress_Recreate(playerid, &PlayerText:textid, Float:x, Float:y, Float:w, Float:h) {
	PlayerTextDrawDestroy(playerid, textid);
	textid = CreatePlayerTextDraw(playerid, x, y, "LD_SPAC:white");
	PlayerTextDrawTextSize(playerid, textid, w, h);
	PlayerTextDrawAlignment(playerid, textid, TEXT_DRAW_ALIGN_LEFT);
	PlayerTextDrawSetShadow(playerid, textid, 0);
	PlayerTextDrawSetOutline(playerid, textid, 0);
	PlayerTextDrawFont(playerid, textid, TEXT_DRAW_FONT_SPRITE_DRAW);
	PlayerTextDrawSetProportional(playerid, textid, true);
	return textid;
}

static stock SProgress_RenderBar(playerid, barid) {
	
	new bar_data[E_SPROGRESS_DATA];
	bar_data = sp_Data[playerid][barid];
	
	SProgress_Recreate(playerid, sp_TextDraw[playerid][barid][sp_fill],
		bar_data[sp_posX],
		bar_data[sp_posY],
		bar_data[sp_width],
		bar_data[sp_height]
	);
	
	SProgress_Recreate(playerid, sp_TextDraw[playerid][barid][sp_back],
		bar_data[sp_posX] - bar_data[sp_borderSize],
		bar_data[sp_posY] - bar_data[sp_borderSize],
		bar_data[sp_width] + (bar_data[sp_borderSize] * 2),
		bar_data[sp_height] + (bar_data[sp_borderSize] * 2)
	);
	
	switch(bar_data[sp_type]) {
		case SP_BAR_TYPE_RIGHT: {
			SProgress_Recreate(playerid, sp_TextDraw[playerid][barid][sp_main],
				bar_data[sp_posX],
				bar_data[sp_posY],
				floatabs((bar_data[sp_width] / bar_data[sp_maxValue]) * bar_data[sp_value]),
				bar_data[sp_height]
			);
			
			SProgress_Recreate(playerid, sp_TextDraw[playerid][barid][sp_sub], 
				bar_data[sp_posX] + floatabs((bar_data[sp_width] / bar_data[sp_maxValue]) * bar_data[sp_subMin]),
				bar_data[sp_posY],
				floatabs((bar_data[sp_width] / bar_data[sp_maxValue]) * (bar_data[sp_subMax] - bar_data[sp_subMin])),
				bar_data[sp_height]
			);
		}
		case SP_BAR_TYPE_LEFT: {
			SProgress_Recreate(playerid, sp_TextDraw[playerid][barid][sp_main],
				bar_data[sp_posX] + bar_data[sp_width] - floatabs((bar_data[sp_width] / bar_data[sp_maxValue]) * bar_data[sp_value]),
				bar_data[sp_posY],
				floatabs((bar_data[sp_width] / bar_data[sp_maxValue]) * bar_data[sp_value]),
				bar_data[sp_height]
			);
			
			SProgress_Recreate(playerid, sp_TextDraw[playerid][barid][sp_sub], 
				bar_data[sp_posX] + bar_data[sp_width] - floatabs((bar_data[sp_width] / bar_data[sp_maxValue]) * bar_data[sp_subMax]),
				bar_data[sp_posY],
				floatabs((bar_data[sp_width] / bar_data[sp_maxValue]) * (bar_data[sp_subMax] - bar_data[sp_subMin])),
				bar_data[sp_height]
			);
		}
		case SP_BAR_TYPE_UP: {
			SProgress_Recreate(playerid, sp_TextDraw[playerid][barid][sp_main],
				bar_data[sp_posX],
				bar_data[sp_posY] + bar_data[sp_height] - floatabs((bar_data[sp_height] / bar_data[sp_maxValue]) * bar_data[sp_value]),
				bar_data[sp_width],
				floatabs((bar_data[sp_height] / bar_data[sp_maxValue]) * bar_data[sp_value])
			);
			
			SProgress_Recreate(playerid, sp_TextDraw[playerid][barid][sp_sub], 
				bar_data[sp_posX],
				bar_data[sp_posY] + bar_data[sp_height] - floatabs((bar_data[sp_height] / bar_data[sp_maxValue]) * bar_data[sp_subMax]),
				bar_data[sp_width],
				floatabs((bar_data[sp_height] / bar_data[sp_maxValue]) * (bar_data[sp_subMax] - bar_data[sp_subMin]))
			);
		}
		case SP_BAR_TYPE_DOWN: {
			SProgress_Recreate(playerid, sp_TextDraw[playerid][barid][sp_main],
				bar_data[sp_posX],
				bar_data[sp_posY],
				bar_data[sp_width],
				floatabs((bar_data[sp_height] / bar_data[sp_maxValue]) * bar_data[sp_value])
			);
			
			SProgress_Recreate(playerid, sp_TextDraw[playerid][barid][sp_sub], 
				bar_data[sp_posX],
				bar_data[sp_posY] + floatabs((bar_data[sp_height] / bar_data[sp_maxValue]) * bar_data[sp_subMin]),
				bar_data[sp_width],
				floatabs((bar_data[sp_height] / bar_data[sp_maxValue]) * (bar_data[sp_subMax] - bar_data[sp_subMin]))
			);
		}
		case SP_BAR_TYPE_MID_H: {
			// textdraw-streamer got error with this so i disable it with PlayerTextDrawSetString
			PlayerTextDrawSetString(playerid, sp_TextDraw[playerid][barid][sp_sub], "");
			SProgress_Recreate(playerid, sp_TextDraw[playerid][barid][sp_main],
				bar_data[sp_posX] + floatabs((bar_data[sp_width] / bar_data[sp_maxValue]) * (bar_data[sp_maxValue] - bar_data[sp_value])) / 2,
				bar_data[sp_posY],
				floatabs((bar_data[sp_width] / bar_data[sp_maxValue]) * bar_data[sp_value]),
				bar_data[sp_height]
			);
		}
		case SP_BAR_TYPE_MID_V: {
			// textdraw-streamer got error with this so i disable it with PlayerTextDrawSetString
			PlayerTextDrawSetString(playerid, sp_TextDraw[playerid][barid][sp_sub], "");
			SProgress_Recreate(playerid, sp_TextDraw[playerid][barid][sp_main],
				bar_data[sp_posX],
				bar_data[sp_posY] + floatabs((bar_data[sp_height] / bar_data[sp_maxValue]) * (bar_data[sp_maxValue] - bar_data[sp_value])) / 2,
				bar_data[sp_width],
				floatabs((bar_data[sp_height] / bar_data[sp_maxValue]) * bar_data[sp_value])
			);
		}
	}
	PlayerTextDrawColour(playerid, sp_TextDraw[playerid][barid][sp_fill], bar_data[sp_colorFill]);
	PlayerTextDrawColour(playerid, sp_TextDraw[playerid][barid][sp_back], bar_data[sp_colorBack]);
	PlayerTextDrawColour(playerid, sp_TextDraw[playerid][barid][sp_sub], bar_data[sp_colorSub]);
	PlayerTextDrawColour(playerid, sp_TextDraw[playerid][barid][sp_main], bar_data[sp_colorMain]);

	// redraw
	if(IsPlayerSProgressVisible(playerid, barid)) {
		HidePlayerSProgress(playerid, barid);
		ShowPlayerSProgress(playerid, barid);
	}
	return 1;
}

hook OnScriptInit() {
	SProgressTD_ResetAll();
	Iter_Init(sp_Index);
	return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
	Iter_Clear(sp_Index[playerid]);
	return 1;
}

hook OnScriptExit() {
	for(new i = 0; i < MAX_PLAYERS; i++) {
		if(IsPlayerConnected(i)) {
			DestroyAllPlayerSProgress(i);
		}
	}
	return 1;
}