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
    if (flag_c(C_JUMPNRUN)) {
        cjumpnrun.jump_n_run(&cphys, input);
    }
    if (flag_c(C_SPRITE)) {
        csprite.update_animation();
    }
}

void Entity::draw(GameScene* scene, Image* img_atlas) {
    if (flag_c(C_DRIFTER)) {
        cdrifter.draw(&cphys, &csprite, img_atlas, scene);
    } else if (flag_c(C_SPRITE)) {
        scene->draw_sprite(csprite.make_ctx(Vec2(0, 0)));
    }
}