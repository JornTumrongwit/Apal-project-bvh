#include "matvec.h"
#include <iostream>
#include <cmath>

myvec3::myvec3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

myvec4::myvec4(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

mymat4::mymat4(float xx, float xy, float xz, float xw,
	float yx, float yy, float yz, float yw,
	float zx, float zy, float zz, float zw,
	float wx, float wy, float wz, float ww) {
	this->content[0][0] = xx;
	this->content[0][1] = xy;
	this->content[0][2] = xz;
	this->content[0][3] = xw;
	this->content[1][0] = yx;
	this->content[1][1] = yy;
	this->content[1][2] = yz;
	this->content[1][3] = yw;
	this->content[2][0] = zx;
	this->content[2][1] = zy;
	this->content[2][2] = zz;
	this->content[2][3] = zw;
	this->content[3][0] = wx;
	this->content[3][1] = wy;
	this->content[3][2] = wz;
	this->content[3][3] = ww;
}

mymat4::mymat4(float input) {
	this->content[0][0] = input;
	this->content[0][1] = 0.0f;
	this->content[0][2] = 0.0f;
	this->content[0][3] = 0.0f;
	this->content[1][0] = 0.0f;
	this->content[1][1] = input;
	this->content[1][2] = 0.0f;
	this->content[1][3] = 0.0f;
	this->content[2][0] = 0.0f;
	this->content[2][1] = 0.0f;
	this->content[2][2] = input;
	this->content[2][3] = 0.0f;
	this->content[3][0] = 0.0f;
	this->content[3][1] = 0.0f;
	this->content[3][2] = 0.0f;
	this->content[3][3] = input;
}


mymat3::mymat3(float xx, float xy, float xz,
	float yx, float yy, float yz,
	float zx, float zy, float zz) {
	this->content[0][0] = xx;
	this->content[0][1] = xy;
	this->content[0][2] = xz;
	this->content[1][0] = yx;
	this->content[1][1] = yy;
	this->content[1][2] = yz;
	this->content[2][0] = zx;
	this->content[2][1] = zy;
	this->content[2][2] = zz;;
}

mymat3::mymat3(float input) {
	this->content[0][0] = input;
	this->content[0][1] = 0.0f;
	this->content[0][2] = 0.0f;
	this->content[1][0] = 0.0f;
	this->content[1][1] = input;
	this->content[1][2] = 0.0f;
	this->content[2][0] = 0.0f;
	this->content[2][1] = 0.0f;
	this->content[2][2] = input;
}

mymat3::mymat3(myvec3 vector0, myvec3 vector1, myvec3 vector2) {
	this->content[0][0] = vector0.x;
	this->content[0][1] = vector0.y;
	this->content[0][2] = vector0.z;
	this->content[1][0] = vector1.x;
	this->content[1][1] = vector1.y;
	this->content[1][2] = vector1.z;
	this->content[2][0] = vector2.x;
	this->content[2][1] = vector2.y;
	this->content[2][2] = vector2.z;
}


myvec3 operator*(mymat4 matrix, myvec3 vector) {
	return myvec3(dot(myvec3(matrix.content[0][0], matrix.content[0][1], matrix.content[0][2]), vector),
		dot(myvec3(matrix.content[1][0], matrix.content[1][1], matrix.content[1][2]), vector),
		dot(myvec3(matrix.content[2][0], matrix.content[2][1], matrix.content[2][2]), vector));
}

mymat4 operator*(mymat4 mat1, mymat4 mat2) {
	myvec4 row0 = myvec4(mat1.content[0][0], mat1.content[0][1], mat1.content[0][2], mat1.content[0][3]);
	myvec4 row1 = myvec4(mat1.content[1][0], mat1.content[1][1], mat1.content[1][2], mat1.content[1][3]);
	myvec4 row2 = myvec4(mat1.content[2][0], mat1.content[2][1], mat1.content[2][2], mat1.content[2][3]);
	myvec4 row3 = myvec4(mat1.content[3][0], mat1.content[3][1], mat1.content[3][2], mat1.content[3][3]);
	myvec4 col0 = myvec4(mat2.content[0][0], mat2.content[1][0], mat2.content[2][0], mat2.content[3][0]);
	myvec4 col1 = myvec4(mat2.content[0][1], mat2.content[1][1], mat2.content[2][1], mat2.content[3][1]);
	myvec4 col2 = myvec4(mat2.content[0][2], mat2.content[1][2], mat2.content[2][2], mat2.content[3][2]);
	myvec4 col3 = myvec4(mat2.content[0][3], mat2.content[1][3], mat2.content[2][3], mat2.content[3][3]);
	return mymat4(
		dot(row0, col0), dot(row0, col1), dot(row0, col2), dot(row0, col3),
		dot(row1, col0), dot(row1, col1), dot(row1, col2), dot(row1, col3),
		dot(row2, col0), dot(row2, col1), dot(row2, col2), dot(row2, col3),
		dot(row3, col0), dot(row3, col1), dot(row3, col2), dot(row3, col3)
	);
}

mymat3 operator*(mymat3 mat1, mymat3 mat2) {
	myvec3 row0 = myvec3(mat1.content[0][0], mat1.content[0][1], mat1.content[0][2]);
	myvec3 row1 = myvec3(mat1.content[1][0], mat1.content[1][1], mat1.content[1][2]);
	myvec3 row2 = myvec3(mat1.content[2][0], mat1.content[2][1], mat1.content[2][2]);
	myvec3 col0 = myvec3(mat2.content[0][0], mat2.content[1][0], mat2.content[2][0]);
	myvec3 col1 = myvec3(mat2.content[0][1], mat2.content[1][1], mat2.content[2][1]);
	myvec3 col2 = myvec3(mat2.content[0][2], mat2.content[1][2], mat2.content[2][2]);
	return mymat3(
		dot(row0, col0), dot(row0, col1), dot(row0, col2),
		dot(row1, col0), dot(row1, col1), dot(row1, col2),
		dot(row2, col0), dot(row2, col1), dot(row2, col2)
	);
}

mymat4 operator*(mymat3 mat1, mymat4 mat2) {
	myvec4 row0 = myvec4(mat1.content[0][0], mat1.content[0][1], mat1.content[0][2], 0);
	myvec4 row1 = myvec4(mat1.content[1][0], mat1.content[1][1], mat1.content[1][2], 0);
	myvec4 row2 = myvec4(mat1.content[2][0], mat1.content[2][1], mat1.content[2][2], 0);
	myvec4 row3 = myvec4(0, 0, 0, 1);
	myvec4 col0 = myvec4(mat2.content[0][0], mat2.content[1][0], mat2.content[2][0], mat2.content[3][0]);
	myvec4 col1 = myvec4(mat2.content[0][1], mat2.content[1][1], mat2.content[2][1], mat2.content[3][1]);
	myvec4 col2 = myvec4(mat2.content[0][2], mat2.content[1][2], mat2.content[2][2], mat2.content[3][2]);
	myvec4 col3 = myvec4(mat2.content[0][3], mat2.content[1][3], mat2.content[2][3], mat2.content[3][3]);
	return mymat4(
		dot(row0, col0), dot(row0, col1), dot(row0, col2), dot(row0, col3),
		dot(row1, col0), dot(row1, col1), dot(row1, col2), dot(row1, col3),
		dot(row2, col0), dot(row2, col1), dot(row2, col2), dot(row2, col3),
		dot(row3, col0), dot(row3, col1), dot(row3, col2), dot(row3, col3)
	);
}

mymat4 operator*(mymat4 mat1, mymat3 mat2) {
	myvec4 row0 = myvec4(mat1.content[0][0], mat1.content[0][1], mat1.content[0][2], mat1.content[0][3]);
	myvec4 row1 = myvec4(mat1.content[1][0], mat1.content[1][1], mat1.content[1][2], mat1.content[1][3]);
	myvec4 row2 = myvec4(mat1.content[2][0], mat1.content[2][1], mat1.content[2][2], mat1.content[2][3]);
	myvec4 row3 = myvec4(mat1.content[3][0], mat1.content[3][1], mat1.content[3][2], mat1.content[3][3]);
	myvec4 col0 = myvec4(mat2.content[0][0], mat2.content[1][0], mat2.content[2][0], 0);
	myvec4 col1 = myvec4(mat2.content[0][1], mat2.content[1][1], mat2.content[2][1], 0);
	myvec4 col2 = myvec4(mat2.content[0][2], mat2.content[1][2], mat2.content[2][2], 0);
	myvec4 col3 = myvec4(0, 0, 0, 1);
	return mymat4(
		dot(row0, col0), dot(row0, col1), dot(row0, col2), dot(row0, col3),
		dot(row1, col0), dot(row1, col1), dot(row1, col2), dot(row1, col3),
		dot(row2, col0), dot(row2, col1), dot(row2, col2), dot(row2, col3),
		dot(row3, col0), dot(row3, col1), dot(row3, col2), dot(row3, col3)
	);
}

myvec4 operator*(mymat4 matrix, myvec4 vector) {
	return myvec4(dot(myvec4(matrix.content[0][0], matrix.content[0][1], matrix.content[0][2], matrix.content[0][3]), vector),
		dot(myvec4(matrix.content[1][0], matrix.content[1][1], matrix.content[1][2], matrix.content[1][3]), vector),
		dot(myvec4(matrix.content[2][0], matrix.content[2][1], matrix.content[2][2], matrix.content[2][3]), vector),
		dot(myvec4(matrix.content[3][0], matrix.content[3][1], matrix.content[3][2], matrix.content[3][3]), vector));
}

mymat3 operator*(mymat3 matrix, float input) {
	return mymat3(
		matrix.content[0][0] * input, matrix.content[0][1] * input, matrix.content[0][2] * input,
		matrix.content[1][0] * input, matrix.content[1][1] * input, matrix.content[1][2] * input,
		matrix.content[2][0] * input, matrix.content[2][1] * input, matrix.content[2][2] * input
	);
}
mymat4 operator*(mymat4 matrix, float input) {
	return mymat4(
		matrix.content[0][0] * input, matrix.content[0][1] * input, matrix.content[0][2] * input, matrix.content[0][3] * input,
		matrix.content[1][0] * input, matrix.content[1][1] * input, matrix.content[1][2] * input, matrix.content[1][3] * input,
		matrix.content[2][0] * input, matrix.content[2][1] * input, matrix.content[2][2] * input, matrix.content[2][3] * input,
		matrix.content[3][0] * input, matrix.content[3][1] * input, matrix.content[3][2] * input, matrix.content[3][3] * input
	);
}
myvec3 operator*(myvec3 vector, float input) {
	return myvec3(vector.x * input, vector.y * input, vector.z * input);
}
myvec4 operator*(myvec4 vector, float input){
	return myvec4(vector.x * input, vector.y * input, vector.z * input, vector.w * input);
}
myvec3 operator*(myvec3 vector, myvec3 input) {
	return myvec3(vector.x * input.x, vector.y * input.y, vector.z * input.z);
}
myvec4 operator*(myvec4 vector, myvec4 input) {
	return myvec4(vector.x * input.x, vector.y * input.y, vector.z * input.z, vector.w * input.w);
}


myvec4 operator+(myvec4 vec1, myvec4 vec2) {
	return myvec4(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z, vec1.w+ vec2.w);
}
myvec4 operator+(myvec4 vec1, myvec3 vec2){
	return myvec4(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z, vec1.w);
}
myvec4 operator+(myvec3 vec1, myvec4 vec2){
	return myvec4(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z, vec2.w);
}
myvec3 operator+(myvec3 vec1, myvec3 vec2) {
	return myvec3(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
}
mymat4 operator+(mymat4 mat1, mymat4 mat2) {
	return mymat4(
		mat1.content[0][0] + mat2.content[0][0], mat1.content[0][1] + mat2.content[0][1], mat1.content[0][2] + mat2.content[0][2], mat1.content[0][3] + mat2.content[0][3],
		mat1.content[1][0] + mat2.content[1][0], mat1.content[1][1] + mat2.content[1][1], mat1.content[1][2] + mat2.content[1][2], mat1.content[1][3] + mat2.content[1][3],
		mat1.content[2][0] + mat2.content[2][0], mat1.content[2][1] + mat2.content[2][1], mat1.content[2][2] + mat2.content[2][2], mat1.content[2][3] + mat2.content[2][3],
		mat1.content[3][0] + mat2.content[3][0], mat1.content[3][1] + mat2.content[3][1], mat1.content[3][2] + mat2.content[3][2], mat1.content[3][3] + mat2.content[3][3]
	);
}
mymat4 operator+(mymat4 mat1, mymat3 mat2) {
	return mymat4(
		mat1.content[0][0] + mat2.content[0][0], mat1.content[0][1] + mat2.content[0][1], mat1.content[0][2] + mat2.content[0][2], mat1.content[0][3],
		mat1.content[1][0] + mat2.content[1][0], mat1.content[1][1] + mat2.content[1][1], mat1.content[1][2] + mat2.content[1][2], mat1.content[1][3],
		mat1.content[2][0] + mat2.content[2][0], mat1.content[2][1] + mat2.content[2][1], mat1.content[2][2] + mat2.content[2][2], mat1.content[2][3],
		mat1.content[3][0], mat1.content[3][1], mat1.content[3][2], mat1.content[3][3]
	);
}
mymat4 operator+(mymat3 mat1, mymat4 mat2) {
	return mymat4(
		mat1.content[0][0] + mat2.content[0][0], mat1.content[0][1] + mat2.content[0][1], mat1.content[0][2] + mat2.content[0][2], mat2.content[0][3],
		mat1.content[1][0] + mat2.content[1][0], mat1.content[1][1] + mat2.content[1][1], mat1.content[1][2] + mat2.content[1][2], mat2.content[1][3],
		mat1.content[2][0] + mat2.content[2][0], mat1.content[2][1] + mat2.content[2][1], mat1.content[2][2] + mat2.content[2][2], mat2.content[2][3],
		mat2.content[3][0], mat2.content[3][1], mat2.content[3][2], mat2.content[3][3]
	);
}
mymat3 operator+(mymat3 mat1, mymat3 mat2) {
	return mymat3(
		mat1.content[0][0] + mat2.content[0][0], mat1.content[0][1] + mat2.content[0][1], mat1.content[0][2] + mat2.content[0][2],
		mat1.content[1][0] + mat2.content[1][0], mat1.content[1][1] + mat2.content[1][1], mat1.content[1][2] + mat2.content[1][2],
		mat1.content[2][0] + mat2.content[2][0], mat1.content[2][1] + mat2.content[2][1], mat1.content[2][2] + mat2.content[2][2]
	);
}

myvec4 operator-(myvec4 vec1, myvec4 vec2) {
	return myvec4(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z, vec1.w - vec2.w);
}
myvec4 operator-(myvec4 vec1, myvec3 vec2){
	return myvec4(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z, vec1.w);
}
myvec4 operator-(myvec3 vec1, myvec4 vec2){
	return myvec4(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z, vec2.w);
}
myvec3 operator-(myvec3 vec1, myvec3 vec2) {
	return myvec3(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z);
}
mymat4 operator-(mymat4 mat1, mymat4 mat2) {
	return mymat4(
		mat1.content[0][0] - mat2.content[0][0], mat1.content[0][1] - mat2.content[0][1], mat1.content[0][2] - mat2.content[0][2], mat1.content[0][3] - mat2.content[0][3],
		mat1.content[1][0] - mat2.content[1][0], mat1.content[1][1] - mat2.content[1][1], mat1.content[1][2] - mat2.content[1][2], mat1.content[1][3] - mat2.content[1][3],
		mat1.content[2][0] - mat2.content[2][0], mat1.content[2][1] - mat2.content[2][1], mat1.content[2][2] - mat2.content[2][2], mat1.content[2][3] - mat2.content[2][3],
		mat1.content[3][0] - mat2.content[3][0], mat1.content[3][1] - mat2.content[3][1], mat1.content[3][2] - mat2.content[3][2], mat1.content[3][3] - mat2.content[3][3]
	);
}
mymat4 operator-(mymat4 mat1, mymat3 mat2) {
	return mymat4(
		mat1.content[0][0] - mat2.content[0][0], mat1.content[0][1] - mat2.content[0][1], mat1.content[0][2] - mat2.content[0][2], mat1.content[0][3],
		mat1.content[1][0] - mat2.content[1][0], mat1.content[1][1] - mat2.content[1][1], mat1.content[1][2] - mat2.content[1][2], mat1.content[1][3],
		mat1.content[2][0] - mat2.content[2][0], mat1.content[2][1] - mat2.content[2][1], mat1.content[2][2] - mat2.content[2][2], mat1.content[2][3],
		mat1.content[3][0], mat1.content[3][1], mat1.content[3][2], mat1.content[3][3]
	);
}
mymat4 operator-(mymat3 mat1, mymat4 mat2) {
	return mymat4(
		mat1.content[0][0] - mat2.content[0][0], mat1.content[0][1] - mat2.content[0][1], mat1.content[0][2] - mat2.content[0][2], mat2.content[0][3],
		mat1.content[1][0] - mat2.content[1][0], mat1.content[1][1] - mat2.content[1][1], mat1.content[1][2] - mat2.content[1][2], mat2.content[1][3],
		mat1.content[2][0] - mat2.content[2][0], mat1.content[2][1] - mat2.content[2][1], mat1.content[2][2] - mat2.content[2][2], mat2.content[2][3],
		mat2.content[3][0], mat2.content[3][1], mat2.content[3][2], mat2.content[3][3]
	);
}
mymat3 operator-(mymat3 mat1, mymat3 mat2) {
	return mymat3(
		mat1.content[0][0] - mat2.content[0][0], mat1.content[0][1] - mat2.content[0][1], mat1.content[0][2] - mat2.content[0][2],
		mat1.content[1][0] - mat2.content[1][0], mat1.content[1][1] - mat2.content[1][1], mat1.content[1][2] - mat2.content[1][2],
		mat1.content[2][0] - mat2.content[2][0], mat1.content[2][1] - mat2.content[2][1], mat1.content[2][2] - mat2.content[2][2]
	);
}
myvec3 operator/(myvec3 vec1, myvec3 vec2){
	return myvec3(vec1.x / vec2.x, vec1.y / vec2.y, vec1.z / vec2.z);
}
myvec4 operator/(myvec4 vec1, myvec4 vec2){
	return myvec4(vec1.x / vec2.x, vec1.y / vec2.y, vec1.z / vec2.z, vec1.w / vec2.w);
}

float dot(myvec4 vec1, myvec4 vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z + vec1.w * vec2.w;
}
float dot(myvec4 vec1, myvec3 vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}
float dot(myvec3 vec1, myvec4 vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}
float dot(myvec3 vec1, myvec3 vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

myvec3 cross(myvec3 vec1, myvec3 vec2) {
	return myvec3(vec1.y * vec2.z - vec1.z * vec2.y, vec1.z * vec2.x - vec1.x * vec2.z, vec1.x * vec2.y - vec1.y * vec2.x);
}

mymat4 transpose(mymat4 matrix) {
	return mymat4(
		matrix.content[0][0], matrix.content[1][0], matrix.content[2][0], matrix.content[3][0],
		matrix.content[0][1], matrix.content[1][1], matrix.content[2][1], matrix.content[3][1],
		matrix.content[0][2], matrix.content[1][2], matrix.content[2][2], matrix.content[3][2],
		matrix.content[0][3], matrix.content[1][3], matrix.content[2][3], matrix.content[3][3]
	);
}
mymat3 transpose(mymat3 matrix) {
	return mymat3(
		matrix.content[0][0], matrix.content[1][0], matrix.content[2][0],
		matrix.content[0][1], matrix.content[1][1], matrix.content[2][1],
		matrix.content[0][2], matrix.content[1][2], matrix.content[2][2]
	);
}

//Thanks Quake3, time to copy
float Q_rsqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long*)&y;                       // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);               // what the fuck? (This comment is in the original code. Yes, I'll keep it in)
	y = *(float*)&i;
	y = y * (threehalfs - (x2 * y * y));   // 1st iteration
	// y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

myvec4 normalize(myvec4 vector) {
	float q_distance = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w);
	return vector * q_distance;
}

myvec3 normalize(myvec3 vector) {
	float q_distance = 1/sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	return vector * q_distance;
}

myvec3 reflect(myvec3 vector, myvec3 normal) {
	return vector - normal * (2.0 * dot(normal, vector));
}
float distance(myvec3 a, myvec3 b) {
	float sideA = b.x - a.x;
	float sideB = b.y - a.y;
	float sideC = b.z - a.z;
	return sqrt(sideA * sideA + sideB * sideB + sideC * sideC);
}

void printvec3(myvec3 inp){
  std::cout<<"("<<inp.x<<", "<<inp.y<<", "<<inp.z<<")";
}
void printvec4(myvec4 inp){
  std::cout<<"("<<inp.x<<", "<<inp.y<<", "<<inp.z<<", "<<inp.w<<")";
}

myvec3 myvec3::copy(){
	return myvec3(this->x, this->y, this->z);
}
