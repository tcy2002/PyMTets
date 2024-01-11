#pragma once

#include "Vector3.h"

class Triangle 
{
public:

    // The vertices of the triangle
    Vector3* v0;
    Vector3* v1;
    Vector3* v2;

    /*
        Constructor of the class
    */
    Triangle(Vector3* v0, Vector3* v1, Vector3* v2) : v0(v0), v1(v1), v2(v2) {}

    /*
        Indicates whether the triangle has a vertex with the same (x, y, z) coordinate values
        @param V A vertex
        @return true if the triangle has a vertex with the same (x, y, z) values, otherwise false
    */
    bool hasVertex(Vector3* V) {
	    return (v0->equal(V) || v1->equal(V) || v2->equal(V));
    }

    /*
        Indicates whether the triangle is adjacent (shares an edge) with triangle T
        @param T A triangle
        @return true if both triangles share an edge, otherwise false
    */
    bool isAdjacent(Triangle* T) {
        // Two triangles are adjacent if they share one common edge, which means they share two vertices
        if (hasVertex(T->v0) && hasVertex(T->v1))
        {
            return true;
        }
        else if (hasVertex(T->v1) && hasVertex(T->v2))
        {
            return true;
        }
        else if (hasVertex(T->v2) && hasVertex(T->v0))
        {
            return true;
        }

        // Since they don't share two vertices then return false
        return false;
    }

    /*
        Indicates whether the triangle's vertices are the same point
        @return true if the vertices are the same, otherwise false
    */
    bool isPoint() {
	    return (v0->equal(v1) && v1->equal(v2));
    }

    /*
        Returns the normal vector of the triangle using the vertices
        @return a Vector3 pointer to the object representing the normal
    */
    Vector3* normal() {
	    return v1->sub(v0)->cross(v2->sub(v0));
    }

    /*
        Indicates whether the triangle has counterclockwise direction with respect of a given vector
        @param V A vector
        @return true if the triangle vertices are in counterclockwise direction with respect of the
        given vector
    */
    bool isCCW(Vector3* V) {
        // The vertices of the triangle are in CCW if the dot product between the normal of
        // the triangle and the given vector is positive, which means the angle is positive
        // and less than 90 degrees
        return ((v0->sub(V))->dot(normal()) > 0);
    }
};
