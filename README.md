# sProgress
Created by Giapp, based on progress2.inc by Southclaws with new features.

- Support [open.mp](https://github.com/openmultiplayer/open.mp/releases)
- Support [textdraw-streamer](https://github.com/nexquery/samp-textdraw-streamer)

## Installation
Include in your code and begin using the library:

```pawn
#include <sProgress>
```

## Usage
### Values
- `MAX_SPROGRESS_BARS`:
  - Defaults to the player textdraw limit divided by 4 (64).
- `INVALID_SP_BAR_VALUE`:
  - Invalid return value for interface functions.
- `INVALID_SP_BAR_ID`:
  - Invalid bar ID value.

### Types
- `SP_BAR_TYPE_RIGHT`: Bar direction left-to-right.
- `SP_BAR_TYPE_LEFT`: Bar direction right-to-left.
- `SP_BAR_TYPE_UP`: Bar direction bottom to top.
- `SP_BAR_TYPE_DOWN`: Bar direction top to bottom.
- `SP_BAR_TYPE_MID_V`: Bar direction middle-to-edge vertical
- `SP_BAR_TYPE_MID_H`: Bar direction middle-to-edge horizontal

### Function
- `bool:IsValidPlayerSProgress(playerid, barid)`:
- - Returns true if the input bar ID is valid and exists.
- bool:IsPlayerSProgressVisible(playerid, barid)
- CreatePlayerSProgress(playerid, Float:x, Float:y, Float:w, Float:h, Float:max = 100.0, type = SP_BAR_TYPE_RIGHT)
- DestroyPlayerSProgress(playerid, barid)
- DestroyAllPlayerSProgress(playerid)
- (Show/Hide)PlayerSProgress(playerid, barid)
- (Get/Set)PlayerSProgressBackColor
- (Get/Set)PlayerSProgressFillColor
- (Get/Set)PlayerSProgressMainColor
- (Get/Set)PlayerSProgressSubColor
- (Get/Set)PlayerSProgressPos
- (Get/Set)PlayerSProgressOverride
- (Get/Set)PlayerSProgressType
- (Float:Get/Set)PlayerSProgressWidth
- (Float:Get/Set)PlayerSProgressHeight
- (Float:Get/Set)PlayerSProgressBorderSize
- (Float:Get/Set)PlayerSProgressMinValue
- (Float:Get/Set)PlayerSProgressMaxValue
- (Float:Get/Set)PlayerSProgressValue
- (Float:Get/Set)PlayerSProgressSubMinValue
- (Float:Get/Set)PlayerSProgressSubMaxValue
