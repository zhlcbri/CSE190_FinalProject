#pragma once
#ifndef VECTOR3D_H_I0UEF3KR
#define VECTOR3D_H_I0UEF3KR

#include <vector>

struct Vector3d {
	unsigned char x, y, z;
	double send(int a, int b, int c) {
		Vector3d res;
		res.x = a;
		res.y = b;
		res.z = c;
		return a+b+c;
	}
};


#endif