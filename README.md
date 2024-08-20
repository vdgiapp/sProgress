# sProgress
Created by Giapp, based on [progress2](https://github.com/Southclaws/progress2) by Southclaws with new features.

- Require [YSI-Includes](https://github.com/pawn-lang/YSI-Includes)
- Support [open.mp](https://github.com/openmultiplayer/open.mp/releases)
- Support [textdraw-streamer](https://github.com/nexquery/samp-textdraw-streamer)

## Installation
Include in your code and begin using the library:

```pawn
#include <sProgress>
```

## Explaination
### Bars and override mode
![](https://github.com/vdgiapp/sProgress/blob/main/_explaination.gif)

### Types
![](https://github.com/vdgiapp/sProgress/blob/main/_types_1.gif)
![](https://github.com/vdgiapp/sProgress/blob/main/_types_2.gif)

## Usage
### Values
- `MAX_SPROGRESS_BARS`: Defaults to the player textdraw limit divided by 4 (64).
- `INVALID_SP_BAR_VALUE`: Invalid return value for interface functions.
- `INVALID_SP_BAR_ID`: Invalid bar ID value.

### Types
- `SP_BAR_TYPE_RIGHT`: Bar direction left-to-right.
- `SP_BAR_TYPE_LEFT`: Bar direction right-to-left.
- `SP_BAR_TYPE_UP`: Bar direction bottom to top.
- `SP_BAR_TYPE_DOWN`: Bar direction top to bottom.
- `SP_BAR_TYPE_MID_V`: Bar direction middle-to-edge vertical (sub bar will not work with this type).
- `SP_BAR_TYPE_MID_H`: Bar direction middle-to-edge horizontal (sub bar will not work with this type).
  
### Functions
#### Main
- `CreatePlayerSProgress(playerid, Float:x, Float:y, Float:w, Float:h, Float:max = 100.0, type = SP_BAR_TYPE_RIGHT)`: Create a progress bar for a player, return the bar ID if its created.
- `DestroyPlayerSProgress(playerid, barid)`: Destroy a player's progress bar.
- `DestroyAllPlayerSProgress(playerid)`: Destroy all progress bars for a player.
- `ShowPlayerSProgress(playerid, barid)`: Shows a player's progress bar to them.
- `HidePlayerSProgress(playerid, barid)`: Hides a player's progress bar from them.
- `bool:IsValidPlayerSProgress(playerid, barid)`: Returns true if the input bar ID is valid and exists.
- `bool:IsPlayerSProgressVisible(playerid, barid)`: Return true if the input bar ID is visible to player.

#### Colour
- `GetPlayerSProgressBackColour(playerid, barid)`: Returns the background colour of a progress bar.
- `SetPlayerSProgressBackColour(playerid, barid, colour)`: Sets the background colour of a progress bar.
- `GetPlayerSProgressFillColour(playerid, barid)`: Returns the fill colour of a progress bar.
- `SetPlayerSProgressFillColour(playerid, barid, colour)`: Sets the fill colour of a progress bar.
- `GetPlayerSProgressMainColour(playerid, barid)`: Returns the main colour of a progress bar.
- `SetPlayerSProgressMainColour(playerid, barid, colour)`: Sets the main colour of a progress bar.
- `GetPlayerSProgressSubColour(playerid, barid)`: Returns the sub colour of a progress bar.
- `SetPlayerSProgressSubColour(playerid, barid, colour)`: Sets the sub colour of a progress bar.

#### Position
- `GetPlayerSProgressPos(playerid, barid, &Float:x, &Float:y)`: Returns the on-screen position of the specified progress bar.
- `SetPlayerSProgressPos(playerid, barid, Float:x, Float:y)`: Updates the position for a progress bar.

#### Width/height
- `Float:GetPlayerSProgressWidth(playerid, barid)`: Returns the width of a progress bar.
- `SetPlayerSProgressWidth(playerid, barid, Float:w)`: Updates the width of a progress bar.
- `Float:GetPlayerSProgressHeight(playerid, barid)`: Returns the height of a progress bar.
- `SetPlayerSProgressHeight(playerid, barid, Float:w)`: Updates the height of a progress bar.

#### Border size
- `Float:GetPlayerSProgressBorderSize(playerid, barid)`: Return border size of a progress bar.
- `SetPlayerSProgressBorderSize(playerid, barid, Float:b)`: Updates border size of a progress bar.

#### Override
- `bool:GetPlayerSProgressOverride(playerid, barid)`: Return true if the input bar ID uses override mode.
- `SetPlayerSProgressOverride(playerid, barid, bool:override)`: Set override mode of a progress bar.

#### Type
- `GetPlayerSProgressType(playerid, barid)`: Return the type of a progress bar.
- `SetPlayerSProgressType(playerid, barid, type)`: Updates the type of a progress bar.

#### Main value
- `Float:GetPlayerSProgressMinValue(playerid, barid)`: Return the min value of a progress bar.
- `Float:GetPlayerSProgressMaxValue(playerid, barid)`: Return the max value of a progress bar.
- `Float:GetPlayerSProgressValue(playerid, barid)`: Return the value of a progress bar.
- `SetPlayerSProgressMinValue(playerid, barid, Float:val)`: Set the min value of a progress bar.
- `SetPlayerSProgressMaxValue(playerid, barid, Float:val)`: Set the max value of a progress bar.
- `SetPlayerSProgressValue(playerid, barid, Float:val)`: Set the value of a progress bar.

#### Sub value
- `Float:GetPlayerSProgressSubMinValue(playerid, barid)`: Return the sub min value of a progress bar.
- `Float:GetPlayerSProgressSubMaxValue(playerid, barid)`: Return the sub max value of a progress bar.
- `SetPlayerSProgressSubMinValue(playerid, barid, Float:val)`: Set the sub min value of a progress bar.
- `SetPlayerSProgressSubMaxValue(playerid, barid, Float:val)`: Set the sub max value of a progress bar.

### Hooked
- `OnScriptInit`: When y_iterate is used, initialises iterators.
- `OnPlayerDisconnect`: To automatically destroy bars when a player disconnects.
- `OnScriptExit`
