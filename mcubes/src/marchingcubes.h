
#ifndef _MARCHING_CUBES_H
#define _MARCHING_CUBES_H

#include <stddef.h>
#include <array>
#include <vector>
#include <unordered_map>
#include "Vector3.h"
#include "Triangle.h"

namespace mc
{

namespace private_
{
    std::vector<Triangle*>* marchCellTetrahedra(Vector3* v0, Vector3* v1, Vector3* v2, Vector3* v3,
                                                Vector3* v4, Vector3* v5, Vector3* v6, Vector3* v7, double isovalue);
}

template<typename vector3, typename formula>
void marching_cubes(const vector3& lower, const vector3& upper,
    int numx, int numy, int numz, formula f, double isovalue,
    std::vector<double>& vertices, std::vector<typename vector3::size_type>& polygons)
{
    using coord_type = typename vector3::value_type;
    using size_type = typename vector3::size_type;
    using namespace private_;

    // Some initial checks
    if(numx < 2 || numy < 2 || numz < 2)
        return;

    if(!std::equal(std::begin(lower), std::end(lower), std::begin(upper),
                   [](double a, double b)->bool {return a <= b;}))
        return;

    // numx, numy and numz are the numbers of evaluations in each direction
    --numx; --numy; --numz;

    coord_type dx = (upper[0] - lower[0]) / static_cast<coord_type>(numx);
    coord_type dy = (upper[1] - lower[1]) / static_cast<coord_type>(numy);
    coord_type dz = (upper[2] - lower[2]) / static_cast<coord_type>(numz);

    // Generate the 3D data structure where the triangles of each cell will be stored
	// NOTE: This is done for finding the shells
	auto triangles = new std::vector<Triangle*>***[numx];
	for (int i = 0; i < numx; i++)
	{
		triangles[i] = new std::vector<Triangle*>**[numy];
		for (int j = 0; j < numy; j++)
		{
			triangles[i][j] = new std::vector<Triangle*>*[numz];
		}
	}

    for(int i=0; i<numx; ++i)
    {
        coord_type x = lower[0] + dx*i;
        coord_type x_dx = lower[0] + dx*(i+1);

        for(int j=0; j<numy; ++j)
        {
            coord_type y = lower[1] + dy*j;
            coord_type y_dy = lower[1] + dy*(j+1);

            double v[8];
            v[4] = f(x, y, lower[2]); v[5] = f(x_dx, y, lower[2]);
            v[6] = f(x_dx, y_dy, lower[2]); v[7] = f(x, y_dy, lower[2]);

            for(int k=0; k<numz; ++k)
            {
                coord_type z = lower[2] + dz*k;
                coord_type z_dz = lower[2] + dz*(k+1);

                // 0-8: (---)(+--)(++-)(-+-)(--+)(+-+)(+++)(-++)
                v[0] = v[4]; v[1] = v[5];
                v[2] = v[6]; v[3] = v[7];
                v[4] = f(x, y, z_dz); v[5] = f(x_dx, y, z_dz);
                v[6] = f(x_dx, y_dy, z_dz); v[7] = f(x, y_dy, z_dz);

                // 0-8: (---)(+--)(+-+)(--+)(-+-)(++-)(+++)(-++)
                // swap y z
                auto v0 = new Vector3(x, z, y);
                auto v1 = new Vector3(x_dx, z, y);
                auto v2 = new Vector3(x_dx, z, y_dy);
                auto v3 = new Vector3(x, z, y_dy);
                auto v4 = new Vector3(x, z_dz, y);
                auto v5 = new Vector3(x_dx, z_dz, y);
                auto v6 = new Vector3(x_dx, z_dz, y_dy);
                auto v7 = new Vector3(x, z_dz, y_dy);

                // Isovalue of each point
                v0->info = v[0]; v1->info = v[1];
                v2->info = v[2]; v3->info = v[3];
                v4->info = v[4]; v5->info = v[5];
                v6->info = v[6]; v7->info = v[7];

                // March the cell's tetrahedra and store the triangles in the respective cell
				triangles[i][j][k] = marchCellTetrahedra(v0, v1, v2, v3, v4, v5, v6, v7, isovalue);
            }
        }
    }

    // Hash map to record the index
    std::unordered_map<Vector3, int, Vector3Hash> vertex_map;
    int current_id = -1;

    // Lambda to add a new vertex to both the list and hash map
    auto add_vertex = [&](Vector3* v) -> int {
        if (vertex_map.find({v->x, v->y, v->z}) == vertex_map.end()) {
            vertices.push_back(v->x);
            vertices.push_back(v->z); // swap y z back
            vertices.push_back(v->y);
            vertex_map[{v->x, v->y, v->z}] = ++current_id;
            return current_id;
        } else {
            return vertex_map[{v->x, v->y, v->z}];
        }
    };

    for (int i = 0; i < numx; i++) {
		for (int j = 0; j < numy; j++) {
		    for (int k = 0; k < numz; k++) {
                for (auto& tri : *triangles[i][j][k]) {
                    polygons.push_back(add_vertex(tri->v0));
                    polygons.push_back(add_vertex(tri->v2));
                    polygons.push_back(add_vertex(tri->v1));
                }
		    }
		}
	}
}

}

#endif // _MARCHING_CUBES_H
