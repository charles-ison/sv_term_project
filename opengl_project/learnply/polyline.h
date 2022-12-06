#pragma once

#include "icVector.H"
#include "polyhedron.h"
#include <utility>
#include <list>
#include <vector>


class POLYLINE {
public:
	std::list<icVector3> m_vertices;
	icVector3 m_rgb = icVector3(1.0, 0.0, 0.0);
	double m_weight = 1.0;
	bool isNeighbor(const POLYLINE& line);
	void merge(const POLYLINE& line);
};

class CRITICAL_POINT {
public:
	icVector3 m_coord;
	icVector3 m_rgb;

	CRITICAL_POINT(icVector3 coord, icVector3 rgb) {
		m_coord = coord;
		m_rgb = rgb;
	}
};

void display_polyline(std::vector<POLYLINE>& polylines);
void marchingSquare(std::list<POLYLINE>& edges,
	const Polyhedron& poly,
	const double& iso_val);
void makePolylineFromEdges(
	std::vector<POLYLINE>& polylines,
	const std::list<POLYLINE>& edges);
void findCriticalPoints(std::vector<CRITICAL_POINT>& points, const Polyhedron& poly);


class LineSegment
{
public:

	// fields
	icVector3 start, end;
	double len;

	// constructors

	LineSegment(icVector3 start_in, icVector3 end_in)
	{
		start = start_in;
		end = end_in;
		len = length(end - start);
	}

	LineSegment(double sx, double sy, double sz, double ex, double ey, double ez)
	{
		start = icVector3(sx, sy, sz);
		end = icVector3(ex, ey, ez);
		len = length(end - start);
	}

	// methods

	icVector3 midpoint()
	{
		icVector3 diff = end - start;
		return start + (0.5 * diff);
	}
};

// PolyLine is a list of connected line segments
typedef std::vector<LineSegment> PolyLine;
