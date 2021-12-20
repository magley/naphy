#include "shape.h"


Shape::Shape() {
    type = SHAPE_CIRCLE;
    radius = 10;
}


Shape::Shape(double circle_radius) {
    type = SHAPE_CIRCLE;
    radius = circle_radius;
}


Shape::Shape(unsigned regular_poly_vertex_count, double regular_poly_radius) {
    type = SHAPE_POLYGON;
    for (unsigned i = 0; i < regular_poly_vertex_count; i++) {
        const double angle = i * (2.0 * PI) / regular_poly_vertex_count;
        const double c = cos(angle) * regular_poly_radius;
        const double s = sin(angle) * regular_poly_radius;
        vert.push_back(Vec2(c, s));
    }
    compute_normals();
}


Shape::Shape(std::vector<Vec2> polygon_vertices) {
    type = SHAPE_POLYGON;
    vert = polygon_vertices;
    compute_normals();
}


void Shape::query(double* out_area, double* out_area_moment, Vec2* out_centroid) const {
    //
	// https://en.wikipedia.org/wiki/Second_moment_of_area
	// https://en.wikipedia.org/wiki/List_of_second_moments_of_area
	//
	// Second moment of area is related to bending surfaces
	// and not generally used to calculate moment of impulse
	// but for very thin objects (2D shapes can be assumed to
	// be infinitely thin in the z-axis):  I = mm * rho.
	// That is, moment of inertia = moment of mass * density.


	double area = 0;
	double area_moment = 0;
	Vec2 centroid = Vec2(0, 0);

	if (type == SHAPE_CIRCLE) {
		area = radius * radius * PI;
		area_moment = radius * radius * radius * radius * PI;
		centroid = Vec2(0, 0);
	} else if (type == SHAPE_POLYGON) {
		for (unsigned i = 0; i < vert.size(); i++) {
			const Vec2 v1 = vert[i];
			const Vec2 v2 = vert[(i + 1) % vert.size()];

			const double crossed = cross(v1, v2);
			const double Ix = (v1.x * v1.x) + (v1.x * v2.x) + (v2.x * v2.x);
			const double Iy = (v1.y * v1.y) + (v1.y * v2.y) + (v2.y * v2.y);

			area += 0.5 * crossed;
			area_moment += (0.25 / 3.0) * crossed * (Ix + Iy);
			centroid += (0.5 / 3.0) * (v1 + v2);
		}
		centroid *= 1 / area;
	}

	if (out_area != NULL) *out_area = area;
	if (out_area_moment != NULL) *out_area_moment = area_moment;
	if (out_centroid != NULL) *out_centroid = centroid;
}


int Shape::get_orientation() const {
    //
	// http://www.faqs.org/faqs/graphics/algorithms-faq/ section 2.07.
	// 
	// This isn't the fastest way, but it doesn't matter since this procedure
	// should be called once for a shape's lifetime. Otherwise, we could cache
	// it in a struct member.
 

	double sum = 0.0;
	for (int i = 0; i < vert.size(); i++) {
		const Vec2 v1 = vert[i];
		const Vec2 v2 = vert[(i + 1) % vert.size()];
		sum += cross(v1, v2);
	}
	return sum > 0.0 ? 1 : -1;
}


void Shape::compute_normals() {
    //
	// Polygon normals are always pointing outwards, but
	// if the vertices are given in a counter-clockwise
	// order, the sign needs to be flipped.


	const int sgn = get_orientation();
	norm = std::vector<Vec2>();

	for (unsigned i = 0; i < vert.size(); i++) {
		const Vec2 A = vert[i];
		const Vec2 B = vert[(i + 1) % vert.size()];
		const Vec2 edge = B - A;

		Vec2 normal = Vec2(edge.y, -edge.x) * sgn;
		norm.push_back(normal.normalized());
	}
}


Vec2 Shape::support(Vec2 direction) const {
    //
	// For circles it'll be:
	//       dir.normalized() * radius
	// 
	// For polygons, we search for a point that produces
	// the maximum dot product with the direction vector.
	// Note that only a point from the list of vertices
	// can be a support point (which is ok for our needs).


	if (type == SHAPE_CIRCLE) {
		Vec2 v = direction.normalized();
		v *= radius;
		return v;
	} else if (type == SHAPE_POLYGON) {
		double proj_best = -DBL_MAX;
		Vec2 vertex;

		for (unsigned i = 0; i < vert.size(); i++) {
			const Vec2* v = &vert[i];
			const double proj = dot(*v, direction);

			if (proj > proj_best) {
				proj_best = proj;
				vertex = *v;
			}
		}
		return vertex;
	} else {
		return Vec2(0, 0);
	}
}