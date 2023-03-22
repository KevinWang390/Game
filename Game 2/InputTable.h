#pragma once
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>
#include <Windows.h>

#include "scripts.h"

class InputTable {
public:
	InputTable() {} //save the below stuff for InputController
	//void receive_keydown(WPARAM wParam) {}
	//void receive_keyup(WPARAM wParam) {}
	//void bind(std::function<void()> f, WPARAM wParam) {}
	std::unordered_map<WPARAM, void(void*)> input_id;
	//std::vector<int> id_flags;
	//std::vector<bool> id_holding;
};