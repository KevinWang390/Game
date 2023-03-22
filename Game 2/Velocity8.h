#pragma once
#include <cmath>
#include <vector>

class Velocity8 {
public:
	Velocity8(float s) {
		components = { 0,0 };
		speed = s;
		norm = 1.0;
	}
	float get_x_speed() {
		return speed * (float)components[0] / norm;
	}
	float get_y_speed() {
		return speed * (float)components[1] / norm;
	}
	void inc_x() { components[0] += 1; };
	void dec_x() { components[0] -= 1; };
	void inc_y() { components[1] += 1; };
	void dec_y() { components[1] -= 1; };
	void normalize() {
		if (components[0] && components[1]) norm = (float)sqrt(2);
		else norm = 1.0;
	}
	void set_speed(float new_speed) {
		speed = new_speed;
	}
	void change_speed(float delta) {
		speed += delta;
	}
private:
	std::vector<int> components;
	float norm;
	float speed;
};