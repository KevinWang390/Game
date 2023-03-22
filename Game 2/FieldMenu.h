#pragma once

#include "Menu.h"

class FieldMenu : public Menu {
	FieldMenu() {
		parent = nullptr;
	}
	void load() {
		return;
	}
	void draw() {
		return;
	}
	void back() {
		return;
	}
	void input_start(int i) {
		switch (i) {
		case -1:
			break;
		default:
			break;
		}
	}
	void input_end(int i) {};
};