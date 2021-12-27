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


/**
 * @brief Fundamental structure in naphy.
 */
struct PhysBody {
    /**
     * @brief Position.
     */
    Vec2 pos;
    /**
     * @brief Velocity.
     */
    Vec2 vel;
    /**
     * @brief Force.
     */
    Vec2 force;
    /**
     * @brief Angular velocity.
     */
    double angvel;
    /**
     * @brief Torque (angular force).
     */
    double torque;
    /**
     * @brief Angle (in radians). Don't change directly, use set_angle()
     */
    double ang;
    /**
     * @brief Rotation matrix. It should *always* match ang.
     */
    Mat2x2 rot;
    /**
     * @brief See the PhysBodyDynamicState enum for allowed values.
     */
    unsigned dynamic_state;
    /**
     * @brief Moment of intertia. We also store the inverse value because of how commonly it's used.
     * Moment of inertia of 0 will also have inverse moment of inertia be equal 0.
     */
    double I, I_inv;
    /**
     * @brief Mass. We also store the inverse value because of how commonly it's used.
     * Mass of 0 will also have inverse mass be equal 0.
     */
	double m, m_inv;
    /**
     * @brief Shape of this body. Determines the body's mass and moment of inertia.
     */
    Shape shape;

    PhysMaterial material;


    /**
     * @brief Construct a new static PhysBody object with a default Shape.
     */
    PhysBody();
    /**
     * @brief Construct a new Phys Body object at given position with specified shape.
     * 
     * @param position Position in the game world.
     * @param shape Shape of the object. Determines the mass.
     */
    PhysBody(Vec2 position, Shape shape);
    /**
     * @brief Sets the angle and rotation vector. Use this instead of directly modifying `ang`.
     * 
     * @param radians New angle in radians.
     */
	void set_angle(double radians);
    /**
     * @brief Recalculates the mass and moment of inertia for this object, based on density and shape.
     * If the mass/moment of inertia is 0, the body is automatically tagged as static (see dynamic_state).
     * 
     * @param density Density of the object.
     * Fine-tuning may be required if you plan on switching rendering backends because of scaling.
     * If unsure, use 0.1.
     */
	void calc_mass(double density);
    /**
     * @brief Draws the body.
     * 
     * @param rend Handle to the SDL renderer which the object will be drawn on.
     */
    void draw(SDL_Renderer* rend);
    /**
     * @brief Get the minimal axis-aligned bounding box of this body (in world space).
     * This is used for broad- and middle-phase collision detection.
     * 
     * @return 2 element vector of Vec2's denoting the top-left and bottom-right bounds of the bbox.
     */
	std::vector<Vec2> get_bbox() const;
    /**
     * @brief Check whether this body's bounding box intersects the bounding box specified by the parameters.
     * Used in broad-phase collision detection.
     * 
     * @param ul Top left point of the bounding box we check against.
     * @param dr Bottom right point of the bounding box we check against.
     * @return int 1 if there is intersection, 0 otherwise.
     */
	int bbox_query(Vec2 ul, Vec2 dr) const;
    /**
     * @brief Check for collision between the two bodies' bounding boxes.
     * This is used for middle-phase collision detection.
     * 
     * @param other Other body we check against.
     * @return int 1 if there is bounding box intersection (might be a false-positive), 0 otherwise (true-negative).
     */
	int bbox_collision(const PhysBody* other) const;
};