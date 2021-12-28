#include "physbody.h"
#include "rend/rend.h"


PhysBody::PhysBody() {
    shape = Shape();
    dynamic_state = PHYSBODY_STATE_STATIC;
    pos = vel = force = Vec2(0, 0);
    ang = angvel = torque = 0.0;
    m = m_inv = I = I_inv = 0;
	material = PhysMaterial();

    set_angle(ang);
    calc_mass(0.1);
}


PhysBody::PhysBody(Vec2 position, Shape shape) {
	this->shape = shape;
	dynamic_state = PHYSBODY_STATE_AWAKE;
	pos = position;
	vel = force = Vec2(0, 0);
    ang = angvel = torque = 0.0;
	m = m_inv = I = I_inv = 0;
	material = PhysMaterial();

	set_angle(ang);
	calc_mass(0.1);
}


void PhysBody::calc_mass(double density) {
	double area, area_moment;
	shape.query(&area, &area_moment, NULL);

	if (shape.type == SHAPE_POLYGON && shape.vert.size() < 3) {
		m = 1 * density;
		I = 1 * density;
		m_inv = 1 * density;;
		I_inv = 1 * density;
	} else {
		m = density * area;
		I = density * area_moment * 2;
		m_inv = m ? (1.0 / m) : 0.0;
		I_inv = I ? (1.0 / I) : 0.0;
	}

	if (m_inv == 0 && I_inv == 0)
		dynamic_state = PHYSBODY_STATE_STATIC;
	else if (dynamic_state == PHYSBODY_STATE_STATIC)
		dynamic_state = PHYSBODY_STATE_AWAKE;
}


void PhysBody::set_angle(double radians) {
	ang = radians;
	rot = Mat2x2(radians);
}


std::vector<Vec2> PhysBody::get_bbox() const {
	Vec2 my_ul, my_dr;

	if (shape.type == SHAPE_CIRCLE) {
		my_ul = -Vec2(shape.radius, shape.radius);
		my_dr =  Vec2(shape.radius, shape.radius);
	} else if (shape.type == SHAPE_POLYGON) {
		my_ul = shape.vert[0];
		my_dr = shape.vert[0];

		for (unsigned i = 0; i < shape.vert.size(); i++) {
			const Vec2 v = rot * shape.vert[i]; // We have to rotate it here.

			my_ul.x = std::min(my_ul.x, v.x);
			my_ul.y = std::min(my_ul.y, v.y);
			my_dr.x = std::max(my_dr.x, v.x);
			my_dr.y = std::max(my_dr.y, v.y);
		}
	}

	my_ul += pos;
	my_dr += pos;

	std::vector<Vec2> v;
	v.push_back(my_ul);
	v.push_back(my_dr);
	return v;
}


int PhysBody::bbox_query(Vec2 ul, Vec2 dr) const {
	const std::vector<Vec2> v = get_bbox();
	const Vec2 my_ul = v[0];
	const Vec2 my_dr = v[1];

	return (my_ul.x <= dr.x) && (ul.x <= my_dr.x) && (my_ul.y <= dr.y) && (ul.y <= my_dr.y);
}

int PhysBody::bbox_collision(const PhysBody* other) const {
	const std::vector<Vec2> v = get_bbox();
	const Vec2 my_ul = v[0];
	const Vec2 my_dr = v[1];
	return other->bbox_query(my_ul, my_dr);
}


void PhysBody::draw(SDL_Renderer* rend) {
	Vec2 Q; // One point on the shape for a line that shows body orientation.

	if (shape.type == SHAPE_CIRCLE) {
		draw_circle(rend, pos.x, pos.y, shape.radius);
		Q = rot * (Vec2(shape.radius, 0));
	} if (shape.type == SHAPE_POLYGON) {
		draw_poly(rend, shape.vert, pos, ang);
		Q = rot * (shape.vert[0]);
	}

	Q += pos;
	SDL_RenderDrawLineF(rend, pos.x, pos.y, Q.x, Q.y);
}