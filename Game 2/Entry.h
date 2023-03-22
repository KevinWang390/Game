#pragma once

#include "Graphics.h"
#include "Bounds.h"
#include "Player.h"

class Entry {
public:
	Entry(int t, float x, float y, float left, float top, float right, float bottom, int a) {
		automatic = bool(a);
		target_bounds = t;
		playerx = x;
		playery = y;
		area = nullptr;
		Graphics::factory->CreateRectangleGeometry(D2D1::RectF(left, top, right, bottom), &area);
	}
	ID2D1RectangleGeometry* get_geom() {
		return area;
	}
private:
	ID2D1RectangleGeometry* area;
	bool automatic;
	int target_bounds;
	float playerx;
	float playery;

	friend class Bounds;
	friend class Player;
};