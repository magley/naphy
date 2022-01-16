#include "rend/sprite.h"

extern Sprite spr[10];
enum SpriteNames {
	SPR_DRIFTER_DOWN_STAND,
	SPR_DRIFTER_DOWN_WALK,
	SPR_DRIFTER_DOWN_DRIFT,

	SPR_DRIFTER_RIGHT_STAND,
	SPR_DRIFTER_RIGHT_WALK,
	SPR_DRIFTER_RIGHT_DRIFT,

	SPR_DRIFTER_UP_STAND,
	SPR_DRIFTER_UP_WALK,
	SPR_DRIFTER_UP_DRIFT,

	SPR_BG_FLOOR,
};

// Call this once before the game starts.
void sprites_init(Image* img_atlas);