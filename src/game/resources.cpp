#include "resources.h"

Sprite spr[10];
void sprites_init(Image* img_atlas) {
	// Down
	spr[SPR_DRIFTER_DOWN_STAND] 	= Sprite(img_atlas, 0 * 16, 0, 	16, 32, 1);
	spr[SPR_DRIFTER_DOWN_WALK] 		= Sprite(img_atlas, 1 * 16, 0, 	16, 32, 8);
	spr[SPR_DRIFTER_DOWN_DRIFT] 	= Sprite(img_atlas, 9 * 16, 0, 	16, 32, 8);
	// Right
	spr[SPR_DRIFTER_RIGHT_STAND] 	= Sprite(img_atlas, 0 * 16, 32, 16, 32, 1);
	spr[SPR_DRIFTER_RIGHT_WALK] 	= Sprite(img_atlas, 1 * 16, 32, 16, 32, 8);
	spr[SPR_DRIFTER_RIGHT_DRIFT] 	= Sprite(img_atlas, 9 * 16, 32, 32, 32, 8);
	// Up
	spr[SPR_DRIFTER_UP_STAND] 		= Sprite(img_atlas, 0 * 16, 64, 16, 32, 1);
	spr[SPR_DRIFTER_UP_WALK] 		= Sprite(img_atlas, 1 * 16, 64, 16, 32, 8);
	spr[SPR_DRIFTER_UP_DRIFT] 		= Sprite(img_atlas, 9 * 16, 64, 16, 32, 8);

	spr[SPR_BG_FLOOR]				= Sprite(img_atlas, 0, 96, 334, 270, 	1);
}