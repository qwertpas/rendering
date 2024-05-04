from stl import mesh
from mpl_toolkits import mplot3d
import numpy as np
import colorsys
import random

def generate_random_color(hue_min=0, hue_max=1, saturation_min=0.5, saturation_max=1, value_min=0.5, value_max=1):
    hue = random.uniform(hue_min, hue_max)
    saturation = random.uniform(saturation_min, saturation_max)
    value = random.uniform(value_min, value_max)
    r, g, b = colorsys.hsv_to_rgb(hue, saturation, value)
    return int(r * 255), int(g * 255), int(b * 255)



# Load the STL file
teapot = mesh.Mesh.from_file('./3dmodels/illteapot.stl')


# print(teapot.vectors)
faces = teapot.vectors

vertices = []
triangles = []

unique_vertices = np.array(list(set(tuple(vertex) for face in teapot.vectors for vertex in face)))
print(unique_vertices)

for i in range(len(faces)): #for each face
    tri_data = [0,0,0,0,0,0] #vertex indices, rgb
    for j in range(len(faces[i])): #for each vertex in face
        vertex = faces[i][j]

        index = 0
        for unique_vertex in unique_vertices:
            if np.array_equal(vertex, unique_vertex):
                index = np.where((unique_vertices == unique_vertex).all(axis=1))[0][0]
                break

        tri_data[j] = index

    tri_data[3:6] = generate_random_color()
    tri_data[3:6] = 235, 241, 244
    
    triangles.append(tri_data)
triangles = np.array(triangles)
print(triangles, len(triangles))

print("float vertices_unrot[] = {")
for vertex in unique_vertices:
    print(f"    {vertex[0]}, {vertex[1]}, {vertex[2]},")
print("};")


print("uint8_t faces[] = {")
for triangle in triangles:
    print(f"    {triangle[0]}, {triangle[1]}, {triangle[2]}, {triangle[3]}, {triangle[4]}, {triangle[5]},")
print("};")
