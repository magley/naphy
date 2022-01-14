#include "entity.h"

#include "gamescene.h"
#include "utility/input.h"
#include "rend/image.h"

Entity::Entity() {  
}

Entity::Entity(unsigned flag) {
    this->flag = flag; 
}

void Entity::update(const Input* input) {
    if (flag_c(C_DRIFTER)) {
        cdrifter.move_and_drift(&cphys, input);
        cdrifter.fall_in_pits(&cphys);
        cdrifter.update_sprite(&cphys, &csprite, input);
    }
}

void Entity::draw(GameScene* scene, Image* img_drifter) {
    if (flag_c(C_DRIFTER)) {
        cdrifter.draw(&cphys, &csprite, img_drifter, scene);
    }
}