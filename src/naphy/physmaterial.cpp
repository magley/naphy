#include "physmaterial.h"


PhysMaterial::PhysMaterial() {
	sfric = 0.4;
	kfric = 0.3;
	e = 0.3;
	col = {255, 255, 255, 255};
}

PhysMaterial::PhysMaterial(double static_fric, double kinetic_friction, double resitution) {
	sfric = static_fric;
	kfric = kinetic_friction;
	e = resitution;
	col = {255, 255, 255, 255};
}

//==================================================================================================
// Presets

void PhysMaterial::set_default() {
	sfric = 0.4;
	kfric = 0.3;
	e = 0.3;
	col = {255, 255, 255, 255};
}
void PhysMaterial::set_ice() {
	sfric = 0.3;
	kfric = 0.01;
	e = 0.4;
	col = {164, 196, 255, 255};
}
void PhysMaterial::set_metal() {
	sfric = 0.6;
	kfric = 0.3;
	e = 0.05;
	col = {96, 96, 96, 255};
}
void PhysMaterial::set_jelly() {
	sfric = 0.4;
	kfric = 0.3;
	e = 0.8;
	col = {128, 192, 64, 255};
}