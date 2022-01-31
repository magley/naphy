#pragma once


#include "utility/mathutil.h"
#include "shape.h"
#include "physmaterial.h"


struct SDL_Renderer;


enum PhysBodyDynamicState {
	PHYSBODY_STATE_STATIC,      // A state in which the body does not move.
	PHYSBODY_STATE_AWAKE,       // Normal state for a dynamic body.
	PHYSBODY_STATE_SLEEPING,    // Sleeping state, entered when a body does not move for a period of time.
};

enum PhysBodyAggregateState {
	PHYSBODY_SOLID,				// Can push other solids and be pushed by other solids.
	PHYSBODY_PASSIVE,			// Cannot push or be pushed when colliding with other bodies.
};


// Physics body.
struct PhysBody {
	// Position
	Vec2 pos;
	// Velocity
	Vec2 vel;
	Vec2 vel_prev;
	// Force
	Vec2 force;
	// Angular velocity.
	double angvel;
	double angvel_prev;
	// Torque.
	double torque;
	// Angle (in radians). 
	// DON'T CHANGE DIRECTLY! Use set_angle().
	double ang;
	// Rotation matrix, always matching the value of ang.
	// DON'T CHANGE DIRECTLY! Use set_angle().
	Mat2x2 rot;
	// See PhysBodyDynamicState for more info.
	unsigned dynamic_state;
	// See PhysBodyAggregateState for more info.
	unsigned aggregate_state;
	// Bodies can be grouped into layers that can be used for specialized collision detection.
	// Default is 0.
	unsigned layer;
	// Moment of intertia. We also store the inverse value because of how commonly it's used.
	// If moment of inertia of 0, then inverse moment of inertia is 0 (body cannot rotate).
	double I, I_inv;
	// Mass. We also store the inverse value because of how commonly it's used.
	// If mass of 0, then inverse mass is 0 (body cannot move).
	double m, m_inv;
	// Shape of this body. Determines the body's mass and moment of inertia.
	Shape shape;
	// Material of this body. Determiens friction, bounciness, ...
	PhysMaterial material;
	// Pointers to non-solid physbodies this body collided with.
	// Resets every frame.
	std::vector<PhysBody*> cld;


	// Construct a new static PhysBody object with a default Shape.
	PhysBody();
	// Construct a new Phys Body object at given position with specified shape.
	// @param position Position in the game world.
	// @param shape Shape of the object. Determines the mass.
	PhysBody(Vec2 position, Shape shape);
	// @brief Sets the angle and rotation vector. Use this instead of directly modifying `ang`.
	// @param radians New angle in radians.
	void set_angle(double radians);
	//@brief Recalculates the mass and moment of inertia for this object, based on density and shape.
	// @param density Density of the object. If 0, the body is set to be static (see dynamic_state).
	// Fine-tuning may be required if you plan on switching rendering backends because of scaling.
	// If unsure, use 0.1.
	void calc_mass(double density);
	// Draws the body.
	// @param rend Handle to the SDL renderer which the object will be drawn on.
	void draw(SDL_Renderer* rend);
	// @brief Get the minimal axis-aligned bounding box of this body (in world space).
	// This is used for broad- and middle-phase collision detection.
	// @return 2 element vector of Vec2's denoting the top-left and bottom-right bounds of the bbox.
	std::vector<Vec2> get_bbox() const;
	//Check whether this body's bounding box intersects the bounding box specified by the parameters.
	// Used in broad-phase collision detection.
	// @param ul Top left point of the bounding box we check against.
	// @param dr Bottom right point of the bounding box we check against.
	// @return 1 if there is intersection, 0 otherwise.
	int bbox_query(Vec2 ul, Vec2 dr) const;
	// Check for collision between the two bodies' bounding boxes.
	// This is used for middle-phase collision detection. 
	// @param other Other body we check against.
	// @return 1 if intersection exists (may be a false-positive), 0 otherwise (true-negative).
	int bbox_collision(const PhysBody* other) const;
};