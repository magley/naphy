#include "collision.h"
#include "physbody.h"
#include "arbiter.h"

// Specialized collision functions, _cc means 'circle-circle', _cp mean 'circle-polygon' etc.

static int collision_cc(const PhysBody* const A, const PhysBody* const B, Arbiter* const R);
static int collision_cp(const PhysBody* const A, const PhysBody* const B, Arbiter* const R);
static int collision_pc(const PhysBody* const A, const PhysBody* const B, Arbiter* const R);
static int collision_pp(const PhysBody* const A, const PhysBody* const B, Arbiter* const R);

// Helper functions for the polygon-polygon algorithm.

// Get axis of minimal penetration between bodies. It's always one of A's face normals.
// Instead of projecting both shapes, we use support functions which is faster.
// @param A,B are the physics bodies. Order matters.
// @param out_dist is the penetration in the resulting axis.
// @param out_index is the index of the face normal in body A.
static void best_axis(const PhysBody* A, const PhysBody* B, double* out_dist, unsigned* out_index);
// 
static void get_inc_and_ref_face(const PhysBody* ref, const PhysBody* inc, unsigned ref_index, Vec2 ref_vert[2], Vec2 inc_vert[2]);
//
static std::vector<Vec2> clip(Vec2 normal, double c, const Vec2* face);


int collision(Arbiter* const R) {
	if (R->A->shape.type == SHAPE_CIRCLE && R->B->shape.type == SHAPE_CIRCLE)
		return collision_cc(R->A, R->B, R);
	if (R->A->shape.type == SHAPE_CIRCLE && R->B->shape.type == SHAPE_POLYGON)
		return collision_cp(R->A, R->B, R);
	if (R->A->shape.type == SHAPE_POLYGON && R->B->shape.type == SHAPE_CIRCLE)
		return collision_pc(R->A, R->B, R);	
	if (R->A->shape.type == SHAPE_POLYGON && R->B->shape.type == SHAPE_POLYGON)
		return collision_pp(R->A, R->B, R);
	return 0;
}


//=================================================================================================


static int collision_cc(const PhysBody* const A, const PhysBody* const B, Arbiter* const R) {
	/* 
		Circle-circle collision algorithm.
				d(A, B) <= rA + rB

		The collision normal is always a vector pointing from one 
		circle to the other, because that's the direction at which
		they'll move away from each other the quickest.
		There will be at most 1 contact point, collinear to their
		centers i.e. position.
	*/

	const Shape* shpA = &A->shape;
	const Shape* shpB = &B->shape;

	const Vec2 posA = A->pos;
	const Vec2 posB = B->pos;

	const Vec2 dpos = posB - posA;
	const double dist_sqr = (dpos.x * dpos.x) + (dpos.y * dpos.y);
	const double radii_sum = shpA->radius + shpB->radius;

	if (dist_sqr > radii_sum * radii_sum) {
		return 0;
	}

	const double dist = std::sqrt(dist_sqr);

	if (dist == 0.0f) {
		R->depth = shpA->radius;
		R->normal = Vec2(1, 0);
		R->contact.push_back(posA);
	} else {
		R->depth = radii_sum - dist;
		R->normal = dpos / dist;
		R->contact.push_back(posA + R->normal * shpA->radius);
	}

	return 1;
}


static int collision_cp(const PhysBody* const A, const PhysBody* const B, Arbiter* const R) {
	/*
		Circle-polygon collision algorithm.
		Two steps:
			(1)  Find the "best" edge on the polygon
			(2)  Circle collision with "best" edge

		To solve (1), we need an edge whose normal forms the 
		greatest dot product against a difference vector from 
		the circle to a point on one side of the edge. This is
		very similar to finding the closest edge of a polygon to 
		a point. 
		
		There are 2 cases where a circle is not intersecting an 
		edge of the polygon:
		(1.1)  The dot product is negative. This means that the 
		edge is "across" the polygon. In other words, that edge 
		is too far away compared to some other edge to even be 
		taken into consideration.
		(1.2)  The dot product is positive and greater than the
		circle's radius. This edge is a good candidate but the 
		circle is too far away. If this is the case, we can be 
		sure that the shapes are not intersecting and terminate.
		
		Our "best" edge is the one the circle is most far away 
		from while still intersecting, assuming the circle could 
		physically cut that edge in the first place.
		
		For (2) we do an algorithm for collision between a circle 
		and a line-segment. It boils down to checking for the 3
		Voronoi regions of the line segment.
		https://stackoverflow.com/a/1079478
	*/

	// (1)

	const Shape* shpA = &A->shape;
	const Shape* shpB = &B->shape;

	const Vec2 center = B->rot.transpose() * (A->pos - B->pos);

	double p = -FLT_MAX;
	unsigned face = 0;
	for (unsigned i = 0; i < shpB->vert.size(); i++) {
		const double proj = dot(shpB->norm[i], center - shpB->vert[i]);

		if (proj > shpA->radius)
			return 0;

		if (proj > p) {
			p = proj;
			face = i;
		}
	}

	if (p < EPSILON) {
		R->depth = shpA->radius;
		R->normal = -(B->rot * shpB->norm[face]);
		R->contact.push_back((R->normal * shpA->radius) + A->pos);
		return 1;
	}

	// (2)

	Vec2 v1 = shpB->vert[face];
	Vec2 v2 = shpB->vert[(face + 1) % shpB->vert.size()];

	const double proj1 = dot(center - v1, v2 - v1);
	const double proj2 = dot(center - v2, v1 - v2);
	R->depth = shpA->radius - p;

	if (proj1 <= 0 || proj2 <= 0) {
		const Vec2 vertex = proj1 <= 0 ? v1 : v2;
		if (dist_sqr(center, vertex) > shpA->radius * shpA->radius)
			return 0;
		const Vec2 normal = B->rot * (vertex - center).normalized();
		R->normal = normal;
		R->contact.push_back(B->rot * vertex + B->pos);
	} else {
		Vec2 normal = shpB->norm[face];
		if (dot(center - v1, normal) > shpA->radius)
			return 0;
		normal = B->rot * normal;
		R->normal = -normal;
		R->contact.push_back(R->normal * shpA->radius + A->pos);
	}

	return 1;
}


static int collision_pc(const PhysBody* const A, const PhysBody* const B, Arbiter* const R) {
	int k = collision_cp(B, A, R);
	R->normal *= -1;
	return k;
}


static int collision_pp(const PhysBody* const A, const PhysBody* const B, Arbiter* const R) {
	/* 
		Polygon-polygon collision algorithm.
		This is a modified SAT algorithm that utilizes support functions to reduce the number of 
		checks. It is based on Dirk Gregorius' GDC 2013 talk. Instead of O(n+m), it's O((n+m)/2).
		Then we find reference and incident edges, and clip the incident face against the edge face. 
	*/

	double distA, distB;
	unsigned faceA, faceB;

	best_axis(A, B, &distA, &faceA);
	best_axis(B, A, &distB, &faceB);

	if (distA >= 0.0 || distB >= 0.0)
		return 0;

	/* 
		The polygons are colliding. Which polygon should be reference and
		which should be incident? The one with greater dist (smaller projected 
		penetration)  should be the reference. If they're the same, either is OK.
	*/

	const PhysBody* ref = A;
	const PhysBody* inc = B;
	unsigned ref_face = faceA;
	unsigned inc_face = faceB;
	int dir = 1;

	if (distB > distA) {
		ref = B;
		inc = A;
		ref_face = faceB;
		inc_face = faceA;
		dir = -1;
	}


	// We now need the vertices that make up the significant reference and incident faces


	Vec2 ref_vert[2];
	Vec2 inc_vert[2];
	get_inc_and_ref_face(ref, inc, ref_face, ref_vert, inc_vert);

	const Vec2 ref_vec_normalized = (ref_vert[1] - ref_vert[0]).normalized();
	const Vec2 ref_vec_normal(ref_vec_normalized.y, -ref_vec_normalized.x);

	R->normal = ref_vec_normal * dir;
	R->depth = 0;

	/* 
		Equation of a line :: Ax + By = C
		The vector (A, B) is the normal of the line
		C is the negative distance from the origin.
		(A, B) dot (x, y) = C
		(x, y) can be either point of a face whose
		normal is (A, B). (A, B) is ref_vec_normal.
	*/
	const double C = dot(ref_vec_normal, ref_vert[0]);
	const double neg = -dot(ref_vec_normalized, ref_vert[0]);
	const double pos = dot(ref_vec_normalized, ref_vert[1]);
	std::vector<Vec2> inc_clipped;

	inc_clipped = clip(-ref_vec_normalized, neg, inc_vert);
	inc_vert[0] = inc_clipped[0];
	inc_vert[1] = inc_clipped[1];
	inc_clipped = clip(ref_vec_normalized, pos, inc_vert);
	inc_vert[0] = inc_clipped[0];
	inc_vert[1] = inc_clipped[1];

	for (unsigned i = 0; i < 2; i++) {
		double sep = dot(ref_vec_normal, inc_vert[i]) - C;
		if (sep <= 0.0) {
			R->contact.push_back(inc_vert[i]);
			R->depth += -sep;
		}
	}

	if (R->contact.size() > 0)
		R->depth /= R->contact.size();
	return 1;
}


//=================================================================================================


static void get_inc_and_ref_face(const PhysBody* ref, const PhysBody* inc, unsigned ref_face, Vec2 ref_vert[2], Vec2 inc_vert[2]) {
	// We know what the reference face is (getting it here is mostly for readabililty).
	// The incident face is the one that's most parallel to the reference face's normal.

	Vec2 n = ref->shape.norm[ref_face];
	n = inc->rot.transpose() * (ref->rot * n);

	unsigned inc_face = 0;
	double d = FLT_MAX;
	for (unsigned i = 0; i < inc->shape.norm.size(); i++) {
		const double dd = dot(n, inc->shape.norm[i]);
		if (dd < d) {
			d = dd;
			inc_face = i;
		}
	}

	ref_vert[0] = ref->shape.vert[ref_face];
	ref_vert[1] = ref->shape.vert[(ref_face + 1) % ref->shape.vert.size()];

	inc_vert[0] = inc->shape.vert[inc_face];
	inc_vert[1] = inc->shape.vert[(inc_face + 1) % inc->shape.vert.size()];

	// We also need everything in world space.

	ref_vert[0] = (ref->rot * ref_vert[0]) + ref->pos;
	ref_vert[1] = (ref->rot * ref_vert[1]) + ref->pos;

	inc_vert[0] = (inc->rot * inc_vert[0]) + inc->pos;
	inc_vert[1] = (inc->rot * inc_vert[1]) + inc->pos;
}


static void best_axis(const PhysBody* A, const PhysBody* B, double* out_dist, unsigned* out_index) {
	double dist_best = -FLT_MAX;
	unsigned index = 0;

	for (unsigned i = 0; i < A->shape.norm.size(); i++) {
		// Everything is calculated in B's model-space, hence the rotations and translations.

		const Vec2 n = B->rot.transpose() * (A->rot * A->shape.norm[i]);
		const Vec2 sup = B->shape.support(-n);

		Vec2 v = A->shape.vert[i];
		v = B->rot.transpose() * (((A->rot * v) + A->pos) - B->pos);

		const double dist = dot(n, sup - v);
		if (dist > dist_best) {
			dist_best = dist;
			index = i;
		}
	}

	*out_dist = dist_best;
	*out_index = index;
}


static std::vector<Vec2> clip(Vec2 normal, double c, const Vec2* face) {
	// Sutherland-Hodgman clopping algorithm (we only clip 'face' against 'normal').

	std::vector<Vec2> out;

	for (unsigned i = 0; i < 2; i++) {
		Vec2 p0 = face[i];
		Vec2 p1 = face[(i + 1) % 2];

		const double side_0 = dot(normal, p0) - c;
		const double side_1 = dot(normal, p1) - c;

		const Vec2 intersecting = lerp(p0, p1, side_0 / (side_0 - side_1));

		if (side_0 <= 0)
			out.push_back(p0);

		if (side_0 * side_1 < 0)
			out.push_back(intersecting);
	}

	return out;
}