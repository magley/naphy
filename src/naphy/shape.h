#pragma once
#include "utility/mathutil.h"
#include <vector>


enum ShapeType {
	SHAPE_CIRCLE,
	SHAPE_POLYGON
};


/**
 * @brief Shape object, defining basic physical properties of a body.
 */
struct Shape {
    // See ShapeType enum for valid values.
	unsigned type;
    // Radius of the circle. Used only for circles.
	double radius;
    // Vector of all vertices. Used only for polygons.
	std::vector<Vec2> vert;
    // Vector of all normals. Used only for polygons.
	std::vector<Vec2> norm;


    /**
     * @brief By default, the shape is a circle of radius 10.
     * 
     */
	Shape();
    /**
     * @brief Construct a new circle.
     * 
     * @param circle_radius Radius of the shape.
     */
	Shape(double circle_radius);
    /**
     * @brief Construct a new regular poygon.
     * 
     * @param regular_poly_vertex_count Number of vertices the reguar polygon will have.
     * @param regular_poly_radius Distance between shape center and any of the vertices.
     */
	Shape(unsigned regular_poly_vertex_count, double regular_poly_radius);
    /**
     * @brief Construct a new polygon.
     * 
     * @param polygon_vertices Vector of vertices in model-space.
     */
    Shape(std::vector<Vec2> polygon_vertices);


    /**
     * @brief Gather some info about the shape.
     * Note that any of the parameters can be NULL, 
     * in which case that parameter won't be passed as a result.
     * 
     * @param[out] out_area Out pointer to the shape's area.
     * @param[out] out_area_moment Out pointer to the shape's area moment.
     * @param[out] out_centroid Out pointer to the shape's centroid vector. 
     */
	void query(double* out_area, double* out_area_moment, Vec2* out_centroid) const;
    /**
     * @brief Polygons only, calculates `norm` from `vert`.
     * 
     */
	void compute_normals();
    /**
     * @brief Get the orientation of the polygon (clockwise or counter clockwise).
     * Normally, this function is called at most once during a shape's lifetime, in
     * order to assign the proper direction to the polygon's normals.
     * 
     * @return int +1 if CW or -1 if CCW.
     */
	int get_orientation() const;
    /**
     * @brief Calculates the support point of the shape in model-space.
     * For polygons it will be one of the defined vertices.
     * 
     * @param dir Direction.
     * @return Vec2 Support point in that direction.
     */
	Vec2 support(Vec2 dir) const;
};