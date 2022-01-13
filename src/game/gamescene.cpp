#include "gamescene.h"

GameScene::GameScene(PhysScene* physscene, GUI* gui) {
    this->physscene = physscene;
    this->gui = gui;
    this->gui->scene = this;
    this->background = NULL;
}

GameScene::~GameScene() {
    delete physscene;
    delete gui;
}