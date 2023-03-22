#pragma once

class Position {
public:
	Position() {
		x = 400;
		y = 300;
	}
	float get_x() {
		return x;
	}
	float get_y() {
		return y;
	}
	void set_x(float nx) {
		x = nx;
	}
	void set_y(float ny) {
		y = ny;
	}
	void change_x(float delta) {
		x += delta;
	}
	void change_y(float delta) {
		y += delta;
	}
private:
	float x, y;
};