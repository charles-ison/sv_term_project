#pragma once

#include <vector>
#include "polyline.h"
#include <fstream>

std::vector<icVector3> load_grad_descent_points_from_csv();
void add_grad_descent_points_to_polylines(std::vector<icVector3>& grad_descent_points, std::vector<POLYLINE>* polylines);
