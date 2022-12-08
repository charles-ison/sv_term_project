#include "gradient_descent.h"

#include "polyline.h"
#include "icVector.H"
#include <vector>
#include <string>
#include <iostream>

std::vector<icVector3> load_grad_descent_points_from_csv() {
	const char* csv_filepath = "../../jupyter_notebooks/runs/12_epochs_from_-10_to_12_with_2ss-19_0_12_7_2022/gradient_descent_results_PCA.csv";
	std::ifstream csv_filestream;
	csv_filestream.open(csv_filepath);
	if (!csv_filestream) {
		printf("Cannot find file %s\n", csv_filepath);
		exit(1);
	}
	std::string x_string;
	std::string y_string;
	std::string z_string;

	std::vector<icVector3> grad_descent_points;
	double x;
	double y;
	double z;
	while (getline(csv_filestream, x_string, ',')) {
		x = stod(x_string);

		getline(csv_filestream, y_string, ',');
		y = stod(y_string);

		getline(csv_filestream, z_string);
		z = stod(z_string);

		icVector3 vec(x, y, z);
		grad_descent_points.push_back(vec);
	}

	// For debugging:
	/*for (auto& vec : grad_descent_points) {
		std::cout << "x: " << vec.x << " ";
		std::cout << "y: " << vec.y << " ";
		std::cout << "z: " << vec.z << "\n";
	}*/

	return grad_descent_points;
}

void add_grad_descent_points_to_polylines(std::vector<icVector3>& grad_descent_points, std::vector<POLYLINE>* polylines) {
	for (int i = 1; i < grad_descent_points.size(); i++) {
		icVector3 point_a = grad_descent_points[i - 1];
		icVector3 point_b = grad_descent_points[i];

		POLYLINE line;
		line.m_vertices.push_back(point_a);
		line.m_vertices.push_back(point_b);
		line.m_rgb = icVector3(0.0, 1.0, 0.0);
		line.m_weight = 2;
		polylines->push_back(line);
	}
}