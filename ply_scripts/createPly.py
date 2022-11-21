from plyfile import PlyData, PlyElement
import numpy

output_filename = 'new_data.ply'


def f(x, y):
    """Helper function to determine the scalar value for a vertex."""

    # TODO: call a function to get an approximate point from the loss function.

    # x**2 - y**2 makes a pringle/saddle shape.
    return x**2 - y**2


def generate_vertex_data():
    x_min, x_max = -10, 10
    y_min, y_max = x_min, x_max

    # Create a list of vertices.
    data = []
    for y in range(y_max, y_min - 1, -1):
        for x in range(x_max, x_min - 1, -1):
            z, vx, vy, vz = 0, 0.0, 0.0, 0.0
            s = f(x, y)  # Generate our scalar value using f(x, y).
            data.append((x, y, z, vx, vy, vz, s))

    # numpy data types: ['int8', 'i1', 'char', 'uint8', 'u1', 'uchar', 'b1', 'int16', 'i2', 'short', 'uint16', 'u2', 'ushort', 'int32', 'i4', 'int', 'uint32', 'u4', 'uint', 'float32', 'f4', 'float', 'float64', 'f8', 'double']
    vertex = numpy.array(data,
                         dtype=[('x', 'float64'), ('y', 'float64'), ('z', 'float64'),
                                ('vx', 'float64'), ('vy', 'float64'), ('vz', 'float64'),
                                ('s', 'float64')])

    return PlyElement.describe(vertex, 'vertex')


def generate_face_data():
    num_faces = 400
    row_size = 20

    # For every face, insert it into `data` as a tuple, which we will turn into a PlyElement.
    data = []
    x = 0
    while x < num_faces + row_size:
        if x % row_size - (x // row_size - 1) == 0:
            x += 1
            continue

        next_row_offset = row_size + 2 + x
        data.append(([x, x + 1, next_row_offset, next_row_offset - 1]))
        x += 1

    face_data = numpy.array(data,
                            dtype=[('vertex_indices', 'int32', (4,))])

    # PlyElement.describe requires a one-dimensional structured array, so we have to do this in two lines.
    ply_faces = numpy.empty(len(face_data),
                            dtype=[('vertex_indices', 'int32', (4,))])
    ply_faces['vertex_indices'] = face_data

    element_2 = PlyElement.describe(ply_faces, 'face')
    return element_2


# Generate the face and vertex elements.
vertex_element = generate_vertex_data()
face_element = generate_face_data()
# Create a PlyData object and write it to a file.
new_ply_data = PlyData([vertex_element, face_element], text=True)
new_ply_data.write(output_filename)

print(f"Saved to {output_filename}")
