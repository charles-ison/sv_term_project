#include "color_and_height_transform.h"
#include "polyhedron.h"
#include "GL/freeglut.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include "polyline.h"
#include <cmath>

extern Polyhedron* poly;
extern std::vector<POLYLINE> polylines;
extern double HEIGHT_MULTIPLIER;
extern double CAP_HEIGHT;
extern bool APPLY_NORM;
extern bool APPLY_LOG;

void findMinMax(double& min, double& max) {
	// Set min and max scalar for the poly's verticies

	for (int i = 0; i < poly->nverts; i++) {
		auto& vertex = poly->vlist[i];

		if (vertex->scalar < min) {
			min = vertex->scalar;
		}

		if (vertex->scalar > max) {
			max = vertex->scalar;
		}
	}
}


void transform_and_show_greyscale() {
	std::cout << "Displaying scalar values as greyscale map." << std::endl;

	double min = INFINITY;
	double max = -min;

	findMinMax(min, max);

	std::cout << "Min: " << min << std::endl;
	std::cout << "Max: " << max << std::endl;

	// Update the rgb value to gray for each vertex
	for (int i = 0; i < poly->nverts; i++) {
		auto& vertex = poly->vlist[i];
		double s_v = vertex->scalar;
		double gray_ = (s_v - min) / (max - min);
		vertex->R = vertex->G = vertex->B = gray_;
	}

	// Now we need to display vertex
	glutPostRedisplay();

}

void transform_and_show_bicolor() {
	std::cout << "Displaying scalar values as bi-color map." << std::endl;

	double min = INFINITY;
	double max = -min;
	findMinMax(min, max);

	// Update the rgb value for each vertex
	for (int i = 0; i < poly->nverts; i++) {
		auto& vertex = poly->vlist[i];
		double s_v = vertex->scalar;

		// Define two rgb colors
		icVector3 c1(1.0, 0.0, 0.0);
		icVector3 c2(0.0, 0.0, 1.0);

		double l = (s_v - min) / (max - min);
		double r = (max - s_v) / (max - min);

		icVector3 c = c1 * l + c2 * r;

		vertex->R = c.x;
		vertex->G = c.y;
		vertex->B = c.z;
	}
	glutPostRedisplay();
}


void HSVtoRGB(icVector3& rgb, const icVector3& hsv) {
	double H = hsv.x;
	double s = hsv.y;
	double v = hsv.z;

	double C = s * v;

	double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	double m = v - C;

	double r, g, b;

	if (H >= 0 && H < 60) {
		r = C, g = X, b = 0;
	}
	else if (H >= 60 && H < 120) {
		r = X, g = C, b = 0;
	}
	else if (H >= 120 && H < 180) {
		r = 0, g = C, b = X;
	}
	else if (H >= 180 && H < 240) {
		r = 0, g = X, b = C;
	}
	else if (H >= 240 && H < 300) {
		r = X, g = 0, b = C;
	}
	else {
		r = C, g = 0, b = X;
	}

	rgb.x = (r + m);
	rgb.y = (g + m);
	rgb.z = (b + m);
}

void RGBtoHSV(icVector3& hsv, const icVector3& rgb) {
	double r = rgb.x;
	double g = rgb.y;
	double b = rgb.z;

	// h, s, v = hue, saturation, value
	double cmax = std::max(r, std::max(g, b));
	double cmin = std::min(r, std::min(g, b));
	double diff = cmax - cmin;

	double& h = hsv.x;
	double& s = hsv.y;
	double& v = hsv.z;

	if (cmax == cmin)
		h = 0;

	// if cmax equal r then compute h
	else if (cmax == r)
		h = fmod(60 * ((g - b) / diff) + 360, 360);

	// if cmax equal g then compute h
	else if (cmax == g)
		h = fmod(60 * ((b - r) / diff) + 120, 360);

	// if cmax equal b then compute h
	else if (cmax == b)
		h = fmod(60 * ((r - g) / diff) + 240, 360);

	// if cmax equal zero
	if (cmax == 0)
		s = 0;
	else
		s = (diff / cmax);

	// compute v
	v = cmax;
}


// NOTE: I am in CS 453 (400-level), so this is not graded, I just wanted to do it for practice.
void transform_and_show_rainbow_colors() {
	std::cout << "Displaying scalar values as rainbow map." << std::endl;

	double min = INFINITY;
	double max = -min;
	findMinMax(min, max);

	// Update the rgb value for each vertex
	for (int i = 0; i < poly->nverts; i++) {
		auto& vertex = poly->vlist[i];
		double s_v = vertex->scalar;

		// Define two rgb colors
		icVector3 c1(1.0, 0.0, 0.0);
		icVector3 c2(0.0, 0.0, 1.0);

		icVector3 HSVc1, HSVc2;

		RGBtoHSV(HSVc1, c1);
		RGBtoHSV(HSVc2, c2);


		double l = (s_v - min) / (max - min);
		double r = (max - s_v) / (max - min);

		icVector3 HSVc = HSVc1 * l + HSVc2 * r;
		icVector3 RGBc;

		HSVtoRGB(RGBc, HSVc);

		vertex->R = RGBc.x;
		vertex->G = RGBc.y;
		vertex->B = RGBc.z;
	}
	glutPostRedisplay();
}


void transform_and_show_height() {
	
	std::cout << "Displaying scalar values as height. Height multiplier = " << HEIGHT_MULTIPLIER << std::endl;

	for (int i = 0; i < poly->nverts; i++) {
		auto& vertex = poly->vlist[i];
		vertex->scalar = vertex->scalar;

		if (vertex->scalar > CAP_HEIGHT)
			vertex->scalar = CAP_HEIGHT;
	}

	double min = INFINITY;
	double max = -min;
	findMinMax(min, max);

	// Scale the height value for each vertex
	for (int i = 0; i < poly->nverts; i++) {
		auto& vertex = poly->vlist[i];
		vertex->z = vertex->scalar;

		vertex->z = APPLY_LOG ? log(vertex->z) : vertex->z;
		vertex->z = APPLY_NORM ? (vertex->z - min) / (max - min) : vertex->z;

		vertex->z *= HEIGHT_MULTIPLIER;
	}

	glutPostRedisplay();
}