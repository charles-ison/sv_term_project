#include "polyline.h"
#include "GL/glew.h"
#include <iostream>
#define EPSILON 1.0e-5
#define CRIT_EPSILON 0.000001 // sets epsilon for finding non-min-max critical points

bool POLYLINE::isNeighbor(const POLYLINE& line) {
	// If the start or end of a polyline is close enough (EPSILON) to the start or end of another polyline, they are neighbors.

	if ((m_vertices.front() - line.m_vertices.front()).length() < EPSILON ||
		(m_vertices.front() - line.m_vertices.back()).length() < EPSILON ||
		(m_vertices.back() - line.m_vertices.front()).length() < EPSILON ||
		(m_vertices.back() - line.m_vertices.back()).length() < EPSILON) {
		return true;
	}
	else
		return false;


	return true;
}

void POLYLINE::merge(const POLYLINE& line) {
	if ((m_vertices.front() - line.m_vertices.front()).length() < EPSILON) {
		POLYLINE line_ = line;
		line_.m_vertices.pop_front();
		for (auto i = line_.m_vertices.begin(); i != line_.m_vertices.end(); i++) {
			m_vertices.push_front(*i);
		}
	}
	else if ((m_vertices.front() - line.m_vertices.back()).length() < EPSILON) {
		POLYLINE reverseLine = line;
		reverseLine.m_vertices.pop_back();
		reverseLine.m_vertices.reverse();
		for (auto i = reverseLine.m_vertices.begin(); i != reverseLine.m_vertices.end(); i++) {
			m_vertices.push_front(*i);
		}
	}
	else if ((m_vertices.back() - line.m_vertices.front()).length() < EPSILON) {
		POLYLINE line_ = line;
		line_.m_vertices.pop_front();
		for (auto i = line_.m_vertices.begin(); i != line_.m_vertices.end(); i++) {
			m_vertices.push_back(*i);
		}
	}
	else if ((m_vertices.back() - line.m_vertices.back()).length() < EPSILON) {
		POLYLINE reverseLine = line;
		reverseLine.m_vertices.pop_back();
		reverseLine.m_vertices.reverse();
		for (auto i = reverseLine.m_vertices.begin(); i != reverseLine.m_vertices.end(); i++) {
			m_vertices.push_back(*i);
		}
	}

}

void display_polyline(std::vector<POLYLINE>& polylines) {
	glDisable(GL_LIGHTING);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (auto& polyline : polylines) {
		glLineWidth(polyline.m_weight);
		glColor3f(polyline.m_rgb.entry[0], polyline.m_rgb.entry[1], polyline.m_rgb.entry[2]);
		glBegin(GL_LINE_STRIP);

		for (auto it = polyline.m_vertices.begin(); it != polyline.m_vertices.end(); it++) {
			glVertex3d(it->entry[0], it->entry[1], it->entry[2]);
		}


		glEnd();
	}


	glDisable(GL_BLEND);
	glLineWidth(1);
}

Vertex linearInterpolateByScalar(const Vertex& v0, const Vertex& v1, const double& iso_val) {
	// Decides where the boundary point should be on the edge between two vertices
	double f0_f1 = v0.scalar - v1.scalar;
	Vertex r(0.0, 0.0, 0.0);
	if (std::abs(f0_f1) < 1.0e-5) {
		r.x = (v0.x + v1.x) / 2;
		r.y = (v0.y + v1.y) / 2;
		r.z = (v0.z + v1.z) / 2;
	}
	else {
		double t = std::abs((v0.scalar - iso_val) / ((v0.scalar - iso_val) - (v1.scalar - iso_val)));
		r.x = v0.x + t * (v1.x - v0.x);
		r.y = v0.y + t * (v1.y - v0.y);
		r.z = v0.z + t * (v1.z - v0.z);
	}
	return r;
}


void lookUpTable(
	std::vector<Vertex>& r,
	const Vertex& v0,
	const Vertex& v1,
	const Vertex& v2,
	const Vertex& v3,
	const double& iso_val) {

	r.reserve(2);
	int id = 0;
	if (v0.scalar <= iso_val + EPSILON) {
		id += 1;
	}
	if (v1.scalar <= iso_val + EPSILON) {
		id += 2;
	}
	if (v2.scalar <= iso_val + EPSILON) {
		id += 4;
	}
	if (v3.scalar <= iso_val + EPSILON) {
		id += 8;
	}

	double center = 0;
	switch (id)
	{
	case 0:
		break;
	case 1:
		// v0 - v1 // v0 - v3
		r.push_back(linearInterpolateByScalar(v0, v1, iso_val));
		r.push_back(linearInterpolateByScalar(v0, v3, iso_val));
		break;
	case 2:
		// v0 - v1 // v1 - v2
		r.push_back(linearInterpolateByScalar(v0, v1, iso_val));
		r.push_back(linearInterpolateByScalar(v1, v2, iso_val));
		break;
	case 3:
		// v0 - v1 // v0 - v3
		r.push_back(linearInterpolateByScalar(v1, v2, iso_val));
		r.push_back(linearInterpolateByScalar(v0, v3, iso_val));
		break;
	case 4:
		// v1 - v2 // v1 - v2
		// v2 - v3 // v0 - v3
		r.push_back(linearInterpolateByScalar(v1, v2, iso_val));
		r.push_back(linearInterpolateByScalar(v2, v3, iso_val));
		break;
	case 5:
		// v0 - v1 // v1 - v2
		// v2 - v3 // v0 - v3
		center = v0.scalar + v1.scalar + v2.scalar + v3.scalar;
		center /= 4;
		if (center <= iso_val) {
			r.push_back(linearInterpolateByScalar(v0, v1, iso_val));
			r.push_back(linearInterpolateByScalar(v1, v2, iso_val));
			r.push_back(linearInterpolateByScalar(v2, v3, iso_val));
			r.push_back(linearInterpolateByScalar(v0, v3, iso_val));
		}
		else {
			r.push_back(linearInterpolateByScalar(v0, v1, iso_val));
			r.push_back(linearInterpolateByScalar(v0, v3, iso_val));
			r.push_back(linearInterpolateByScalar(v1, v2, iso_val));
			r.push_back(linearInterpolateByScalar(v2, v3, iso_val));
		}
		break;
	case 6:
		// v0 - v1 // v2 - v3
		r.push_back(linearInterpolateByScalar(v0, v1, iso_val));
		r.push_back(linearInterpolateByScalar(v2, v3, iso_val));
		break;
	case 7:
		// v2 - v3 // v0 - v3
		r.push_back(linearInterpolateByScalar(v2, v3, iso_val));
		r.push_back(linearInterpolateByScalar(v0, v3, iso_val));
		break;
	case 8:
		// v0 - v1 // v0 - v3
		r.push_back(linearInterpolateByScalar(v2, v3, iso_val));
		r.push_back(linearInterpolateByScalar(v0, v3, iso_val));
		break;
	case 9:
		// v0 - v1 // v2 - v3
		r.push_back(linearInterpolateByScalar(v0, v1, iso_val));
		r.push_back(linearInterpolateByScalar(v2, v3, iso_val));
		break;
	case 10:
		// v0 - v1 // v0 - v3
		// v1 - v2 // v2 - v3

		center = v0.scalar + v1.scalar + v2.scalar + v3.scalar;
		center /= 4;
		if (center <= iso_val) {
			r.push_back(linearInterpolateByScalar(v0, v1, iso_val));
			r.push_back(linearInterpolateByScalar(v0, v3, iso_val));
			r.push_back(linearInterpolateByScalar(v1, v2, iso_val));
			r.push_back(linearInterpolateByScalar(v2, v3, iso_val));
		}
		else {
			r.push_back(linearInterpolateByScalar(v0, v1, iso_val));
			r.push_back(linearInterpolateByScalar(v1, v2, iso_val));
			r.push_back(linearInterpolateByScalar(v2, v3, iso_val));
			r.push_back(linearInterpolateByScalar(v0, v3, iso_val));
		}
		break;

	case 11:
		// v1 - v2 // v2 - v3
		r.push_back(linearInterpolateByScalar(v1, v2, iso_val));
		r.push_back(linearInterpolateByScalar(v2, v3, iso_val));
		break;
	case 12:
		// v1 - v2 // v0 - v3
		r.push_back(linearInterpolateByScalar(v1, v2, iso_val));
		r.push_back(linearInterpolateByScalar(v0, v3, iso_val));
		break;
	case 13:
		// v0 - v1 // v1 - v2
		r.push_back(linearInterpolateByScalar(v0, v1, iso_val));
		r.push_back(linearInterpolateByScalar(v1, v2, iso_val));
		break;
	case 14:
		// v0 - v1 // v0 - v3
		r.push_back(linearInterpolateByScalar(v0, v1, iso_val));
		r.push_back(linearInterpolateByScalar(v0, v3, iso_val));
		break;
	case 15:
		break;
	}


}

/// <summary>
/// Takes an empty list of edges, a polyhedron, and an iso value, 
/// and finds a contour for the polhedron at iso_val. Runs marching square by performing linear interpolation on every quad.
/// Returns the contour by adding a list of edges to `edges`.
/// </summary>
/// <param name="edges"></param>
/// <param name="poly"></param>
/// <param name="iso_val"></param>
void marchingSquare(std::list<POLYLINE>& edges, const Polyhedron& poly, const double& iso_val) {

	// Connect polylines
	for (int i = 0; i < poly.nquads; i++) {
		std::vector<Vertex> r;
		lookUpTable(r,
			*poly.qlist[i]->verts[0],
			*poly.qlist[i]->verts[1],
			*poly.qlist[i]->verts[2],
			*poly.qlist[i]->verts[3],
			iso_val);

		if (r.size() > 0) {
			for (int r_i = 0; r_i < r.size() / 2; r_i++) {
				POLYLINE line;
				auto v0 = icVector3(
					r[r_i * 2].x,
					r[r_i * 2].y,
					r[r_i * 2].z
				);
				auto v1 = icVector3(
					r[r_i * 2 + 1].x,
					r[r_i * 2 + 1].y,
					r[r_i * 2 + 1].z
				);

				line.m_vertices.push_back(v0);
				line.m_vertices.push_back(v1);
				edges.push_back(line);
			}
		}

	}
}

void makePolylineFromEdges(std::vector<POLYLINE>& polylines, const std::list<POLYLINE>& edges) {
	// 1. Given a set of polylines, take one and compare it to the rest of the polylines to see if they are connected.
	// 2. If they are connected, merge into a new one. Then repeat again to see if you need to merge new components!
	polylines.reserve(edges.size());
	std::list<POLYLINE> edges_temp(edges);
	// Note: you can use recrusion instead of while loop here.
	while (edges_temp.size() > 0) {
		polylines.push_back(edges_temp.front());
		edges_temp.erase(edges_temp.begin());
		int init_size = 0;
		while (init_size != edges_temp.size()) {
			init_size = edges_temp.size();
			for (auto i = edges_temp.begin(); i != edges_temp.end();) {
				if (polylines.back().isNeighbor(*i)) {
					polylines.back().merge(*i);
					i = edges_temp.erase(i);
				}
				else {
					i++;
				}
			}
		}
	}
}

bool checkIfMin(Vertex* v1) {
	/*
	Grid of quads around v1 represented by *:
	.---.---.
	|   |   |
	.---*---.
	|   |   |
	.---.---.
	*/
	// Don't classify vertices that are on the edge of the scalar field.
	if (v1->nquads != 4) {
		return false;
	}

	int c = 0;
	for (int i = 0; i < v1->nquads; i++) {
		Quad* q = v1->quads[i];
		for (int j = 0; j < 4; j++) {
			Vertex* temp = q->verts[j];
			if (temp != v1) {
				// Check if greater than rest
				if (v1->scalar >= temp->scalar) {
					return false;
				}
			}
		}
	}
	std::cout << "Found min! x: " << v1->x << " y: " << v1->y << " s: " << v1->scalar << std::endl;
	return true;
}

bool checkIfMax(Vertex* v1) {
	/*
	Grid of quads around v1 represented by *:
	.---.---.
	|   |   |
	.---*---.
	|   |   |
	.---.---.
	*/
	// Don't classify vertices that are on the edge of the scalar field.
	if (v1->nquads != 4) {
		return false;
	}

	int c = 0;
	for (int i = 0; i < v1->nquads; i++) {
		Quad* q = v1->quads[i];
		for (int j = 0; j < 4; j++) {
			Vertex* temp = q->verts[j];
			if (temp != v1) {
				// Check if greater than rest
				if (v1->scalar <= temp->scalar) {
					return false;
				}
			}
		}
	}
	std::cout << "Found max! x: " << v1->x << " y: " << v1->y << " s: " << v1->scalar << std::endl;
	return true;
}

double calc_x0(Vertex& v1, Vertex& v2, Vertex& v3, Vertex& v4) {
	// v1 is (x2, y2) top-right
	// v2 is (x1, y2) top-left
	// v3 is (x1, y1) bottom-left
	// v4 is (x2, y1) bottom-right

	double y1 = v3.y;
	double y2 = v1.y;

	double f_x2_y2 = v1.scalar;
	double f_x1_y2 = v2.scalar;
	double f_x1_y1 = v3.scalar;
	double f_x2_y1 = v4.scalar;

	double numerator = y2 * f_x1_y1 - y2 * f_x2_y1 - y1 * f_x1_y2 + y1 * f_x2_y2;
	double denominator = f_x1_y1 - f_x2_y1 - f_x1_y2 + f_x2_y2;

	if (denominator == 0) {
		return 0;
	}

	return numerator / denominator;
}

double calc_y0(Vertex& v1, Vertex& v2, Vertex& v3, Vertex& v4) {
	// v1 is (x2, y2) top-right
	// v2 is (x1, y2) top-left
	// v3 is (x1, y1) bottom-left
	// v4 is (x2, y1) bottom-right

	double x1 = v3.x;
	double x2 = v1.x;

	double f_x2_y2 = v1.scalar;
	double f_x1_y2 = v2.scalar;
	double f_x1_y1 = v3.scalar;
	double f_x2_y1 = v4.scalar;

	double numerator = x2 * f_x1_y1 - x1 * f_x2_y1 - x2 * f_x1_y2 + x1 * f_x2_y2;
	double denominator = f_x1_y1 - f_x2_y1 - f_x1_y2 + f_x2_y2;

	if (denominator == 0) {
		return 0;
	}

	return numerator / denominator;
}


void findCriticalPoints(std::vector<CRITICAL_POINT>& critical_points, const Polyhedron& poly) {

	// For each quad:
	for (int i = 0; i < poly.nquads; i++) {

		Vertex v1 = *poly.qlist[i]->verts[0];
		Vertex v2 = *poly.qlist[i]->verts[1];
		Vertex v3 = *poly.qlist[i]->verts[2];
		Vertex v4 = *poly.qlist[i]->verts[3];

		double y0 = calc_x0(v1, v2, v3, v4);
		double x0 = calc_y0(v1, v2, v3, v4);

		// Check out if it is valid (inside the quad)
		if (x0 >= v3.x + CRIT_EPSILON && x0 <= v1.x - CRIT_EPSILON) {
			if (y0 >= v3.y + CRIT_EPSILON && y0 <= v1.y - CRIT_EPSILON) {

				// Then draw a dot at that point.
				double average_height = (v1.z + v2.z + v3.z + v4.z) / 4.0;
				auto new_point = icVector3(x0, y0, average_height);
				auto rgb = icVector3(0, 0, 1);
				auto pt = CRITICAL_POINT(new_point, rgb);
				critical_points.push_back(pt);
				std::cout << "Found critical point at " << new_point.x << " " << new_point.y << " " << v1.scalar << std::endl;

			}
		}
	}

	// For each vertex:
	for (int i = 0; i < poly.nverts; i++) {
		// Check for local min.
		Vertex* vptr = poly.vlist[i];
		bool isMin = checkIfMin(vptr);
		if (isMin) {
			auto new_point = icVector3(vptr->x, vptr->y, vptr->z);
			auto rgb = icVector3(1, 0, 0);
			auto pt = CRITICAL_POINT(new_point, rgb);
			critical_points.push_back(pt);
		}

		// Check for local max.
		bool isMax = checkIfMax(vptr);
		if (isMax) {
			auto new_point = icVector3(vptr->x, vptr->y, vptr->z);
			auto rgb = icVector3(0, 1, 0);
			auto pt = CRITICAL_POINT(new_point, rgb);
			critical_points.push_back(pt);
		}
	}
}
