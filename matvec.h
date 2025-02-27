#pragma once
#include <stdexcept>
#include <iostream>
struct myvec3 {
	myvec3(float x, float y, float z);
	myvec3() = default;
	float x;
	float y;
	float z;
	float& operator[](int ItemKey) {
		if (ItemKey == 0) return this->x;
		if (ItemKey == 1) return this->y;
		if (ItemKey == 2) return this->z;
		else throw std::out_of_range("Accessing myvec3 with value > 2 or < 0");
	}
};

struct myvec4 {
	myvec4(float x, float y, float z, float w);
	myvec4() = default;
	float x;
	float y;
	float z;
	float w;
	float operator[](int ItemKey) {
		if (ItemKey == 0) return this->x;
		if (ItemKey == 1) return this->y;
		if (ItemKey == 2) return this->z;
		if (ItemKey == 3) return this->w;
		else{
			std::clog<<"Tried accessing with "<<ItemKey<<"\n";
			throw std::out_of_range("Accessing myvec4 with value > 3 or < 0");
		}
	}
};

struct mymat4 {
	mymat4() = default;
	mymat4(float xx, float xy, float xz, float xw,
		float yx, float yy, float yz, float yw,
		float zx, float zy, float zz, float zw,
		float wx, float wy, float wz, float ww);
	mymat4(float input);
	float content[4][4];
};

struct mymat3 {
	mymat3() = default;
	mymat3(float xx, float xy, float xz,
		float yx, float yy, float yz,
		float zx, float zy, float zz);
	mymat3(float input);
	mymat3(myvec3 vector0, myvec3 vector1, myvec3 vector2);
	float content[3][3];
};

mymat4 operator*(mymat4 mat1, mymat4 mat2);
mymat4 operator*(mymat3 mat1, mymat4 mat2);
mymat4 operator*(mymat4 mat1, mymat3 amt2);
myvec4 operator*(mymat4 matrix, myvec4 vector);
myvec3 operator*(mymat4 matrix, myvec3 vector);
mymat3 operator*(mymat3 matrix, float input);
mymat4 operator*(mymat4 matrix, float input);
myvec3 operator*(myvec3 vector, float input);
myvec4 operator*(myvec4 vector, float input);
myvec3 operator*(myvec3 vector, myvec3 input);
myvec4 operator*(myvec4 vector, myvec4 input);

myvec4 operator+(myvec4 vec1, myvec4 vec2);
myvec4 operator+(myvec4 vec1, myvec3 vec2);
myvec4 operator+(myvec3 vec1, myvec4 vec2);
myvec3 operator+(myvec3 vec1, myvec3 vec2);
mymat4 operator+(mymat4 mat1, mymat4 mat2);
mymat4 operator+(mymat4 mat1, mymat3 mat2);
mymat4 operator+(mymat3 mat1, mymat4 mat2);
mymat3 operator+(mymat3 mat1, mymat3 mat2);

myvec4 operator-(myvec4 vec1, myvec4 vec2);
myvec4 operator-(myvec4 vec1, myvec3 vec2);
myvec4 operator-(myvec3 vec1, myvec4 vec2);
myvec3 operator-(myvec3 vec1, myvec3 vec2);
mymat4 operator-(mymat4 mat1, mymat4 mat2);
mymat4 operator-(mymat4 mat1, mymat3 mat2);
mymat4 operator-(mymat3 mat1, mymat4 mat2);
mymat3 operator-(mymat3 mat1, mymat3 mat2);

float dot(myvec4 vec1, myvec4 vec2);
float dot(myvec4 vec1, myvec3 vec2);
float dot(myvec3 vec1, myvec4 vec2);
float dot(myvec3 vec1, myvec3 vec2);

myvec3 cross(myvec3 vec1, myvec3 vec2);

mymat4 transpose(mymat4 matrix);
mymat3 transpose(mymat3 matrix);

myvec4 normalize(myvec4 vector);
myvec3 normalize(myvec3 vector);
myvec3 reflect(myvec3 vector, myvec3 normal);

float distance(myvec3 a, myvec3 b);

void printvec3(myvec3 inp);
void printvec4(myvec4 inp);