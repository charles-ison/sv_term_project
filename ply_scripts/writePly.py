from plyfile import PlyData, PlyElement
import numpy

# Read in a .ply file:
plydata = PlyData.read('data/scalar_data/r2.ply')

# ---

# Create a .ply file:
# data types: ['int8', 'i1', 'char', 'uint8', 'u1', 'uchar', 'b1', 'int16', 'i2', 'short', 'uint16', 'u2', 'ushort', 'int32', 'i4', 'int', 'uint32', 'u4', 'uint', 'float32', 'f4', 'float', 'float64', 'f8', 'double']
vertex = numpy.array([(0, 0, 0, 0, 0, 0, 0),
                      (0, 1, 1, 0, 0, 0, 0),
                      (1, 0, 1, 0, 0, 0, 0),
                      (1, 1, 0, 0, 0, 0, 0)],
                     dtype=[('x', 'float64'), ('y', 'float64'), ('z', 'float64'),
                            ('vx', 'float64'), ('vy', 'float64'), ('vz', 'float64'),
                            ('s', 'float64')])

face_data = numpy.array([([0, 1, 2, 0]),
                         ([0, 2, 3, 0]),
                         ([0, 1, 3, 0]),
                         ([1, 2, 3, 0]),
                         ([1, 2, 3, 0])],
                        dtype=[('vertex_indices', 'int32', (4,))])

# PlyElement.describe requires a one-dimensional structured array, so we have to use two lines of code.
ply_faces = numpy.empty(len(face_data),
                        dtype=[('vertex_indices', 'int32', (4,))])
ply_faces['vertex_indices'] = face_data

element_1 = PlyElement.describe(vertex, 'vertex')
element_2 = PlyElement.describe(ply_faces, 'face')

# Create a PlyData with the elements and write it to a file.
PlyData([element_1, element_2], text=True).write('demo.ply')

print("done")
