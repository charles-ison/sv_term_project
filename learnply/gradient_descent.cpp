#include "gradient_descent.h"

#include "polyline.h"
#include "icVector.H"
#include <vector>
#include <string>
#include <iostream>

extern Polyhedron* poly;
extern char* csv_filepath;
extern double HEIGHT_MULTIPLIER;

bool insideQuad(const Quad* q, const icVector3& p) {
	double v0x = q->verts[2]->x;
	double v0y = q->verts[2]->y;
	double v2x = q->verts[0]->x;
	double v2y = q->verts[0]->y;
	if (p.x <= v0x && p.x >= v2x && p.y <= v0y && p.y >= v2y)
		return true;
	else
		return false;
}


Quad* findQuad(const icVector3& v) {
	for (int i = 0; i < poly->nquads; i++) {
		Quad* qtemp = poly->qlist[i];
		if (insideQuad(qtemp, v))
			return qtemp;
	}

	return nullptr;
}

double interpolate_height_of_point_in_quad(Quad* q, icVector3& point) {
	// Bilinear Interpolation / Weighted Mean
	// https://en.wikipedia.org/wiki/Bilinear_interpolation#Weighted_mean

	Vertex* v1 = q->verts[0]; // bottom-left
	Vertex* v2 = q->verts[1]; // bottom-right
	Vertex* v3 = q->verts[2]; // top-right
	Vertex* v4 = q->verts[3]; // top-left
	
	double x1 = v1->x;
	double y1 = v1->y;
	double x2 = v3->x;
	double y2 = v3->y;

	double w_11 = (x2 - point.x) * (y2 - point.y) / ((x2 - x1) * (y2 - y1));
	double w_12 = (x2 - point.x) * (point.y - y1) / ((x2 - x1) * (y2 - y1));
	double w_21 = (point.x - x1) * (y2 - point.y) / ((x2 - x1) * (y2 - y1));
	double w_22 = (point.x - x1) * (point.y - y1) / ((x2 - x1) * (y2 - y1));

	return w_11 * v1->scalar + w_12 * v2->scalar + w_21 * v3->scalar + w_22 * v4->scalar;
}

std::vector<icVector3> load_grad_descent_points_from_csv() {
	std::ifstream csv_filestream;
	csv_filestream.open(csv_filepath);
	if (!csv_filestream) {
		printf("Cannot find file %s\n", csv_filepath);
		exit(1);
	}
	std::string x_string;
	std::string y_string;

	std::vector<icVector3> grad_descent_points;
	double x;
	double y;
	while (getline(csv_filestream, x_string, ',')) {
		x = stod(x_string);

		getline(csv_filestream, y_string);
		y = stod(y_string);

		icVector3 point(x, y, 0.0);
		Quad* q = findQuad(point);
		point.z = interpolate_height_of_point_in_quad(q, point) * HEIGHT_MULTIPLIER + 0.01;
		grad_descent_points.push_back(point);
	}

	return grad_descent_points;
}

void add_grad_descent_points_to_polylines(std::vector<icVector3>& grad_descent_points, std::vector<POLYLINE>* polylines) {
	
	bool use_solid_color = false;

	// Define two rgb colors to use for the gradient.
	icVector3 color_1(1.0, 1.0, 0.0);
	icVector3 color_2(0.0, 1.0, 1.0);

	icVector3 start_point = grad_descent_points.front();
	icVector3 end_point = grad_descent_points.back();

	// Create a gradient of color for the descent line from the top to the bottom of the line.
	double min_height = end_point.z;
	double max_height = start_point.z;

	for (int i = 1; i < grad_descent_points.size(); i++) {
		icVector3 point_a = grad_descent_points[i - 1];
		icVector3 point_b = grad_descent_points[i];

		POLYLINE line;
		line.m_vertices.push_back(point_a);
		line.m_vertices.push_back(point_b);

		double l = (point_a.z - min_height) / (max_height - min_height);
		double r = (max_height - point_b.z) / (max_height - min_height);
		icVector3 color = color_1 * l + color_2 * r;

		if (use_solid_color) {
			line.m_rgb = icVector3(0, 1, 0);
		}
		else {
			line.m_rgb = icVector3(color.x, color.y, color.z);
		}
		line.m_weight = 4;

		polylines->push_back(line);
	}
}