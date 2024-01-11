#pragma once

#include <string>

#define DBL_EPS 1e-4
#define DBL_APPROX(a, b) (std::abs((a) - (b)) < DBL_EPS)

class Vector3
{
public:

	double info;

	// The element of the vector
	double x;
	double y;
	double z;

	/*
	Constructor of the class
	*/
    Vector3() : x(0.0), y(0.0), z(0.0) { info = 0.0; }
	Vector3(double x, double y, double z) : x(x), y(y), z(z) { info = 0.0; }

	/*
	Returns the magnitude of the vector
	*/
	double magnitude() {
        // Calculate the magnitude of the vector and return it
        return sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
    }

	/*
	Normalizes the vector
	*/
	Vector3* normalize() {
        // Get the magnitude of the vector
        double mag = this->magnitude();

        // If it has a magnitude then normalize it
        if (mag > 0)
        {
            return new Vector3(this->x / mag, this->y / mag, this->z / mag);
        }

        // Since the vector has no magnitude then return a zero vector
        return new Vector3(0, 0, 0);
    }

	/*
	Multiply the vector by the given scalar
	*/
	Vector3* multiply(double s) {
        // Multiply each component by the scalar
        return new Vector3(this->x * s, this->y * s, this->z * s);
    }

	/*
	Clones the vector
	*/
	Vector3* clone() {
        return new Vector3(this->x, this->y, this->z);
    }

	/*
	Adds the value of the given vector
	*/
	Vector3* add(Vector3* B) {
        return new Vector3(this->x + B->x, this->y + B->y, this->z + B->z);
    }

	/*
	Subtracts the values of the given vector
	*/
	Vector3* sub(Vector3* B) {
        return new Vector3(this->x - B->x, this->y - B->y, this->z - B->z);
    }

	/*
	Calculates the dot product with vector B
	*/
	double dot(Vector3* B) {
        return (this->x * B->x) + (this->y * B->y) + (this->z * B->z);
    }

	/*
	Calculates the cross product with vector B
	*/
	Vector3* cross(Vector3* B) {
        // Calculate the cross product values
        double i = (this->y * B->z) - (this->z * B->y);
        double j = (this->z * B->x) - (this->x * B->z);
        double k = (this->x * B->y) - (this->y * B->x);

        // Return the new vector
        return new Vector3(i, j, k);
    }

	/*
	Calculates the square euclidean distance to another vector
	*/
	double squareDistance(Vector3* B) {
        return ((B->x - this->x) * (B->x - this->x)) +
               ((B->y - this->y) * (B->y - this->y)) +
               ((B->z - this->z) * (B->z - this->z));
    }

	/*
	Calculates the euclidean distance to another vector
	*/
	double distance(Vector3* B) {
        return sqrt(squareDistance(B));
    }

	/*
	Returns the intyerpolated point using parameter t and point B
	*/
	Vector3* interpolate(double t, Vector3* B) {
        return this->multiply(1.0 - t)->add(B->multiply(t));
    }

	/*
	*/
	bool equal(Vector3* B) const {
        return (this->x == B->x && this->y == B->y && this->z == B->z);
    }

	bool operator==(const Vector3& b) const {
        return DBL_APPROX(this->x, b.x) && DBL_APPROX(this->y, b.y) && DBL_APPROX(this->z, b.z);
    }

};

class Vector3Hash
{
public:
    size_t operator()(const Vector3& v) const
    {
        int x_int = (int)(v.x / DBL_EPS);
        int y_int = (int)(v.y / DBL_EPS);
        int z_int = (int)(v.z / DBL_EPS);
        return std::hash<int>()(x_int) * 73856093 ^
               std::hash<int>()(y_int) * 19349663 ^
               std::hash<int>()(z_int) * 83492791;
    }
};