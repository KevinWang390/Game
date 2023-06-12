#pragma once
#include <vector>
#include <unordered_set>
#include <Windows.h>

#include "Inputable.h"

#define VK_W 0x57
#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44
#define VK_E 0x45
#define VK_Q 0x51

#define TRANSITION 0x101
#define DIALOGUE 0x102
#define MAIN_MENU 0x103

class InputController {
public: 
	InputController() {}
	static void init(std::vector<Inputable*> &v) {
		mode_keys = {VK_TAB, VK_ESCAPE, TRANSITION, DIALOGUE, MAIN_MENU};
		members = v;
	}
	static void receive_keyup(WPARAM wParam) {
		for (auto m : members) m->input_end(wParam);
	}
	static void receive_keydown(WPARAM wParam) {
		change_mode(wParam);
		for (auto m : members) m->input_start(wParam);
	}
	static int get_mode() { return mode; }
	static void change_mode(WPARAM wParam) {
		if (mode_keys.find(wParam) == mode_keys.end()) return;
		if (mode == 0) mode = (int)wParam;
		else if (mode == wParam) mode = 0;
	}
private:
	static std::vector<Inputable*> members;
	static std::unordered_set<WPARAM> mode_keys;
	static int mode;
};