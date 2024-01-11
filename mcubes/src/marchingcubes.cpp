
#include "marchingcubes.h"

namespace mc
{

namespace private_
{

/*
	Returns the parameter t such that it gives the linear interpolation value f between f1 and f2
*/
double inverseLinearInterpolation(double f, double f1, double f2)
{
	return (f - f1) / (f2 - f1);
}

/*
	Builds the triangle corresponding to the indicated vertices and given isovalue
	@param V0 Vertex that has a different value from the other three vertices. If this is positive
	the other vertices must be negative, and viceversa
	@param V1 The next vertex in counter clockwise order
	@param V2 The next next vertex in counter clockwise order
	@param V3 The vertex that is not on the plane where V0, V1 and V2 are
	@param isovalue The isovalue to be considered for inverse interpolation
	@return The pointer to the generated triangle
*/
Triangle* buildTriangle(Vector3* V0, Vector3* V1, Vector3* V2, Vector3* V3, double isovalue)
{
	// Get the t parameters for the intersected values in the edges
	double t01 = inverseLinearInterpolation(isovalue, V0->info, V1->info);
	double t02 = inverseLinearInterpolation(isovalue, V0->info, V2->info);
	double t03 = inverseLinearInterpolation(isovalue, V0->info, V3->info);

	// Interpolate values and get the triangle vertices
	Vector3* T0 = V0->interpolate(t01, V1);
	Vector3* T1 = V0->interpolate(t03, V3);
	Vector3* T2 = V0->interpolate(t02, V2);

	// Generate the triangle
	Triangle* T = new Triangle(T0, T1, T2);
    if ((V0->info > isovalue) ^ T->isCCW(V0)) {
        T->v1 = T2;
        T->v2 = T1;
    }

	// If it happens to be a degenerate triangle (all the vertices are in the same point) then
	// return NULL, Otherwise return the triangle
	return (T->isPoint()) ? NULL : T;
}

/*
	Builds the two triangles corresponding to the indicated vertices and given isovalue
	@param V0 One of the vertices that has a different value from the other two vertices. If this is positive
	the other vertices must be negative, and viceversa
	@param V1 The other vertex with the same sign value as V0
	@param V2 The next next vertex in counter clockwise order
	@param V3 The vertex that is not on the plane where V0, V1 and V2 are
	@param isovalue The isovalue to be considered for inverse interpolation
	@return The pointer to a vector with the generated triangles
*/
std::vector<Triangle*>* buildTriangles(Vector3* V0, Vector3* V1, Vector3* V2, Vector3* V3, double isovalue)
{
	// Get the t parameters for the intersected values in the edges
	double t02 = inverseLinearInterpolation(isovalue, V0->info, V2->info);
	double t03 = inverseLinearInterpolation(isovalue, V0->info, V3->info);
	double t12 = inverseLinearInterpolation(isovalue, V1->info, V2->info);
	double t13 = inverseLinearInterpolation(isovalue, V1->info, V3->info);

	Vector3* T0 = V0->interpolate(t02, V2);
	Vector3* T1 = V1->interpolate(t12, V2);
	Vector3* T2 = V1->interpolate(t13, V3);
	Vector3* T3 = V0->interpolate(t03, V3);

	// Generate the triangles and store them in a vector
	Triangle* triangle1 = new Triangle(T0, T1, T2);
    if ((V0->info > isovalue) ^ triangle1->isCCW(V0)) {
        triangle1->v1 = T2;
        triangle1->v2 = T1;
    }

	Triangle* triangle2 = new Triangle(T2, T3, T0);
    if ((V0->info > isovalue) ^ triangle2->isCCW(V0)) {
        triangle2->v1 = T0;
        triangle2->v2 = T3;
    }

	std::vector<Triangle*>* triangles = new std::vector<Triangle*>();

	// If the triangles are not degenerate (their vertices are the same point) then add them to the triangles vector
	if (!triangle1->isPoint())
	{
		triangles->push_back(triangle1);
	}

	if (!triangle2->isPoint())
	{
		triangles->push_back(triangle2);
	}

	// Returns the triangles
	return triangles;
}

/*
	March the tetrahedron given by its vertices and returns the generated triangles
	@param V0
	@param V1
	@param V2
	@param V3
	@param isovalue
	@return A pointer to a vector with the generated triangles
*/
std::vector<Triangle*>* marchTetrahedra(Vector3* V0, Vector3* V1, Vector3* V2, Vector3* V3, double isovalue)
{
	// Initialize the vector where the triangles will be stored
	std::vector<Triangle*>* triangles = new std::vector<Triangle*>();

	// Define the signs of each vertex of the tetrahedron
	int s0 = (V0->info > isovalue) ? 1 : 0;
	int s1 = (V1->info > isovalue) ? 1 : 0;
	int s2 = (V2->info > isovalue) ? 1 : 0;
	int s3 = (V3->info > isovalue) ? 1 : 0;

	// Process each case
	if (s0 == 0 && s1 == 0 && s2 == 0 && s3 == 0)
	{
		// CASE 0: All negative => No triangles
	}
	else if (s0 == 1 && s1 == 0 && s2 == 0 && s3 == 0)
	{
		// Case 1: 1000
		// One positive vertex, all others negative => One triangle
		// Vertices are in edges 0-1, 0-2 and 0-3

		// Generate the triangle and add it only if it is not NULL
		// NOTE: A triangle is NULL if it is a degenerated triangle
		Triangle* triangle = buildTriangle(V0, V1, V2, V3, isovalue);
		if (triangle != NULL)
		{
			triangles->push_back(triangle);
		}
	}
	else if (s0 == 0 && s1 == 1 && s2 == 0 && s3 == 0)
	{
		// Case 2: 0100
		// One positive vertex, all others negative => One triangle
		// Vertices are in edges 1-2, 1-3 and 1-0

		// Generate the triangle and add it only if it is not NULL
		// NOTE: A triangle is NULL if it is a degenerated triangle
		Triangle* triangle = buildTriangle(V1, V2, V0, V3, isovalue);
		if (triangle != NULL)
		{
			triangles->push_back(triangle);
		}
	}
	else if (s0 == 1 && s1 == 1 && s2 == 0 && s3 == 0)
	{
		std::vector<Triangle*>* t = buildTriangles(V0, V1, V2, V3, isovalue);
		for (auto it = t->begin(); it != t->end(); ++it)
		{
			triangles->push_back(*it);
		}
	}
	else if (s0 == 0 && s1 == 0 && s2 == 1 && s3 == 0)
	{
		// Case 4: 0010
		// One positive vertex, all others negative => One triangle
		// Vertices are in edges 2-0, 2-1 and 2-3

		// Generate the triangle and add it only if it is not NULL
		// NOTE: A triangle is NULL if it is a degenerated triangle
		Triangle* triangle = buildTriangle(V2, V0, V1, V3, isovalue);
		if (triangle != NULL)
		{
			triangles->push_back(triangle);
		}
	}
	else if (s0 == 1 && s1 == 0 && s2 == 1 && s3 == 0)
	{
		std::vector<Triangle*>* t = buildTriangles(V0, V2, V1, V3, isovalue);
		for (auto it = t->begin(); it != t->end(); ++it)
		{
			triangles->push_back(*it);
		}
	}
	else if (s0 == 0 && s1 == 1 && s2 == 1 && s3 == 0)
	{
		std::vector<Triangle*>* t = buildTriangles(V1, V2, V0, V3, isovalue);
		for (auto it = t->begin(); it != t->end(); ++it)
		{
			triangles->push_back(*it);
		}

	}
	else if (s0 == 1 && s1 == 1 && s2 == 1 && s3 == 0)
	{
		// Case 7: 1110
		// One negative vertex, all others positive => One triangle
		// Vertices are in edges 3-2, 3-0 and 3-1

		// Generate the triangle and add it only if it is not NULL
		// NOTE: A triangle is NULL if it is a degenerated triangle
		Triangle* triangle = buildTriangle(V3, V0, V2, V1, isovalue);
		if (triangle != NULL)
		{
			triangles->push_back(triangle);
		}
	}
	else if (s0 == 0 && s1 == 0 && s2 == 0 && s3 == 1)
	{
		// Case 8: 0001
		// One positive vertex, all others negative => One triangle
		// Vertices are in edges 3-2, 3-0 and 3-1

		// Generate the triangle and add it only if it is not NULL
		// NOTE: A triangle is NULL if it is a degenerated triangle
		Triangle* triangle = buildTriangle(V3, V2, V1, V0, isovalue);
		if (triangle != NULL)
		{
			triangles->push_back(triangle);
		}
	}
	else if (s0 == 1 && s1 == 0 && s2 == 0 && s3 == 1)
	{
		std::vector<Triangle*>* t = buildTriangles(V0, V3, V1, V2, isovalue);
		for (auto it = t->begin(); it != t->end(); ++it)
		{
			triangles->push_back(*it);
		}
	}
	else if (s0 == 0 && s1 == 1 && s2 == 0 && s3 == 1)
	{
		std::vector<Triangle*>* t = buildTriangles(V1, V3, V0, V2, isovalue);
		for (auto it = t->begin(); it != t->end(); ++it)
		{
			triangles->push_back(*it);
		}
	}
	else if (s0 == 1 && s1 == 1 && s2 == 0 && s3 == 1)
	{
		// Case 11: 1101
		// One negative vertex, all others positive => One triangle
		// Vertices are in edges 2-0, 2-3 and 2-1

		// Generate the triangle and add it only if it is not NULL
		// NOTE: A triangle is NULL if it is a degenerated triangle
		Triangle* triangle = buildTriangle(V2, V0, V1, V3, isovalue);
		if (triangle != NULL)
		{
			triangles->push_back(triangle);
		}
	}
	else if (s0 == 0 && s1 == 0 && s2 == 1 && s3 == 1)
	{
		std::vector<Triangle*>* t = buildTriangles(V2, V3, V0, V1, isovalue);
		for (auto it = t->begin(); it != t->end(); ++it)
		{
			triangles->push_back(*it);
		}
	}
	else if (s0 == 1 && s1 == 0 && s2 == 1 && s3 == 1)
	{
		// Case 13: 1011
		// One negative vertex, all others positive => One triangle
		// Vertices are in edges 1-2, 1-3 and 1-0

		// Generate the triangle and add it only if it is not NULL
		// NOTE: A triangle is NULL if it is a degenerated triangle
		Triangle* triangle = buildTriangle(V1, V2, V0, V3, isovalue);
		if (triangle != NULL)
		{
			triangles->push_back(triangle);
		}

	}
	else if (s0 == 0 && s1 == 1 && s2 == 1 && s3 == 1)
	{
		// Case 14: 0111
		// One negative vertex, all others positive => One triangle
		// Vertices are in edges 0-1, 0-3 and 0-2

		// Generate the triangle and add it only if it is not NULL
		// NOTE: A triangle is NULL if it is a degenerated triangle
		Triangle* triangle = buildTriangle(V0, V1, V2, V3, isovalue);
		if (triangle != NULL)
		{
			triangles->push_back(triangle);
		}
	}
	else if (s0 == 1 && s1 == 1 && s2 == 1 && s3 == 1)
	{
		// CASE 15: All positive => No triangles
	}
	else
	{
		// Something weird is occuring here!
	}

	// Return the generated triangles
	return triangles;
}

/*
	Run the marching tetrahedra using the information of the cell vertices and the isovalue
	@param v0
	@param v1
	@param v2
	@param v3
	@param v4
	@param v5
	@param v6
	@param isovalue
	@return The pointer to the vector containing the triangles from the cell
*/
std::vector<Triangle*>* marchCellTetrahedra(Vector3* v0, Vector3* v1, Vector3* v2, Vector3* v3,
                                            Vector3* v4, Vector3* v5, Vector3* v6, Vector3* v7, double isovalue)
{
    // Initialize the vector where the triangles will be stored
    std::vector<Triangle*>* triangles = new std::vector<Triangle*>();

    // Run the march algorithm on each tetrahedra and keep the generated triangles
    std::vector<Triangle*>* tetrahedra1Triangles = marchTetrahedra(v0, v1, v3, v5, isovalue);
    std::vector<Triangle*>* tetrahedra2Triangles = marchTetrahedra(v1, v2, v3, v5, isovalue);
    std::vector<Triangle*>* tetrahedra3Triangles = marchTetrahedra(v0, v3, v4, v5, isovalue);
    std::vector<Triangle*>* tetrahedra4Triangles = marchTetrahedra(v2, v3, v5, v6, isovalue);
    std::vector<Triangle*>* tetrahedra5Triangles = marchTetrahedra(v3, v4, v5, v7, isovalue);
    std::vector<Triangle*>* tetrahedra6Triangles = marchTetrahedra(v3, v5, v6, v7, isovalue);

    // Merge the tetrahedra vectors into the general one
    for (auto it = tetrahedra1Triangles->begin(); it != tetrahedra1Triangles->end(); ++it)
    {
        triangles->push_back(*it);
    }

    for (auto it = tetrahedra2Triangles->begin(); it != tetrahedra2Triangles->end(); ++it)
    {
        triangles->push_back(*it);
    }

    for (auto it = tetrahedra3Triangles->begin(); it != tetrahedra3Triangles->end(); ++it)
    {
        triangles->push_back(*it);
    }

    for (auto it = tetrahedra4Triangles->begin(); it != tetrahedra4Triangles->end(); ++it)
    {
        triangles->push_back(*it);
    }

    for (auto it = tetrahedra5Triangles->begin(); it != tetrahedra5Triangles->end(); ++it)
    {
        triangles->push_back(*it);
    }

    for (auto it = tetrahedra6Triangles->begin(); it != tetrahedra6Triangles->end(); ++it)
    {
        triangles->push_back(*it);
    }

    // Return the triangles
    return triangles;
}

}

}
