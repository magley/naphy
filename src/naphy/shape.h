#pragma once
#include "utility/mathutil.h"
#include <vector>


enum ShapeType {
	SHAPE_CIRCLE,
	SHAPE_POLYGON
};


// Shape object, defining geometric properties of a body.
struct Shape {
    // Instead of using nasty OOP, we made everything into one struct.
    // The shape's type is determined by this value.
	ShapeType type;
    // Radius of the circle. Used only for circles.
	double radius;
    // Vector of all vertices. Used only for polygons.
	std::vector<Vec2> vert;
    // Vector of all normals. Used only for polygons.
	std::vector<Vec2> norm;


    // By default, the shape is a circle of radius 10.
	Shape();
    // Construct a new circle.
    // @param circle_radius Radius of the shape.
	Shape(double circle_radius);
    // Construct a new rectangle. The rectangle is centered. 
    // @param rect_size size as a Vec2.
	Shape(Vec2 rect_size);
    // Construct a new regular poygon.
    // @param regular_poly_vertex_count Number of vertices the reguar polygon will have.
    // @param regular_poly_radius Distance between shape center and any of the vertices.
	Shape(unsigned regular_poly_vertex_count, double regular_poly_radius);
    // Construct a new polygon. 
    // @param polygon_vertices Vector of vertices in model-space.
    Shape(std::vector<Vec2> polygon_vertices);


    // Gather some info about the shape.
    // Note that any of the parameters can be NULL, 
    // in which case that parameter won't be passed as a result.
    // @param[out] out_area Out pointer to the shape's area.
    // @param[out] out_area_moment Out pointer to the shape's area moment.
    // @param[out] out_centroid Out pointer to the shape's centroid vector. 
	void query(double* out_area, double* out_area_moment, Vec2* out_centroid) const;
    // Polygons only, calculates `norm` from `vert`.
	void compute_normals();
    // @brief Get the orientation of the polygon (clockwise or counter clockwise).
    // Normally, this function is called at most once during a shape's lifetime, in
    // order to assign the proper direction to the polygon's normals.
    // @return +1 if CW, -1 if CCW.
	int get_orientation() const;
    // Calculates the support point of the shape in model-space.
    // For polygons it will be one of the defined vertices.
    // @param dir Direction.
    // @return Support point in that direction.
	Vec2 support(Vec2 dir) const;
};