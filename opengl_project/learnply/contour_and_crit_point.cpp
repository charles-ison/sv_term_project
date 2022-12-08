#pragma once

#include "contour_and_crit_point.h"
#include "color_and_height_transform.h"
#include "polyhedron.h"
#include "polyline.h"
#include "GL/freeglut.h"
#include <iostream>
#include <algorithm>

extern int NUM_CONTOURS;
extern Polyhedron* poly;
extern std::vector<POLYLINE> polylines;
extern std::vector<CRITICAL_POINT> critical_points;

void generate_and_show_contours() {
	double min_iso_val = INFINITY;
	double max_iso_val = -min_iso_val;
	findMinMax(min_iso_val, max_iso_val);

	double step_size = 0; // default case, step size does not matter if there is only one contour.
	if (NUM_CONTOURS > 1) {
		step_size = (max_iso_val - min_iso_val) / (NUM_CONTOURS - 1);
	}

	auto& contour_color = icVector3(1.0, 0.2, 0.0);

	std::cout << "Drawing " << NUM_CONTOURS << " contours from " << min_iso_val << " to " << max_iso_val << std::endl;

	double curr_iso_val = min_iso_val;
	for (int i = 1; i <= NUM_CONTOURS; i++) {
		std::list<POLYLINE> edgei;

		// `curr_iso_val` is the iso-value at which to draw the contour
		marchingSquare(edgei, *poly, curr_iso_val);

		std::vector<POLYLINE> polylinei;
		makePolylineFromEdges(polylinei, edgei);
		for (auto& polyline_ : polylinei) {
			polyline_.m_rgb = contour_color; // We color based on the index of the # line it is.
			polyline_.m_weight = 2;
			polylines.push_back(polyline_);
		}

		curr_iso_val += step_size;
	}

	glutPostRedisplay();
}

void generate_and_show_colored_contours() {
	double min_iso_val = INFINITY;
	double max_iso_val = -min_iso_val;
	findMinMax(min_iso_val, max_iso_val);

	//min_iso_val = -2000;
	//max_iso_val = 3000;

	double step_size = 0; // default case, step size does not matter if there is only one contour.
	if (NUM_CONTOURS > 1) {
		step_size = (max_iso_val - min_iso_val) / (NUM_CONTOURS - 1);
	}

	// Define two rgb colors for the gradient of contours.
	icVector3 start_color(1.0, 0.0, 0.0);
	icVector3 end_color(0.0, 0.0, 1.0);

	// Loop for each contour, each with a new curr_iso_val.
	double curr_iso_val = min_iso_val;
	for (int i = 1; i <= NUM_CONTOURS; i++) {

		// Determine contour_color.
		double start_color_bias = (curr_iso_val - min_iso_val) / (max_iso_val - min_iso_val);
		double end_color_bias = (max_iso_val - curr_iso_val) / (max_iso_val - min_iso_val);
		icVector3 contour_color = start_color * start_color_bias + end_color * end_color_bias;

		// `curr_iso_val` is the iso-value at which to draw the contour.
		// Returns the contour as a list of edges `edgei`.
		std::list<POLYLINE> edgei;
		marchingSquare(edgei, *poly, curr_iso_val);

		// Turns the list of edges into a polyline to add to `polylines`.
		std::vector<POLYLINE> polylinei;
		makePolylineFromEdges(polylinei, edgei);
		for (auto& polyline_ : polylinei) {
			polyline_.m_rgb = icVector3(contour_color.x, contour_color.y, contour_color.z); // We color based on the index of the # line it is.
			polyline_.m_weight = 2;
			polylines.push_back(polyline_);
		}

		curr_iso_val += step_size;
	}

	// Display the new polylines.
	glutPostRedisplay();
}

void generate_and_show_critical_points() {
	findCriticalPoints(critical_points, *poly);
	glutPostRedisplay();
}
