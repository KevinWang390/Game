#pragma once
#include <functional>

#include "Graphics.h"
#include "InputController.h"

bool active = false;
bool first_half = true;
float counter = 0.0f;
const float MAX_COUNTER = 15.0f;
std::function<void()> func;
ID2D1SolidColorBrush* trans_brush;

void init_trans() {
	Graphics::rtarget->CreateSolidColorBrush(D2D1::ColorF(0.0, 0.0, 0.0, 1.0), &trans_brush);
}

void start_trans(const std::function<void()>& f) {
	if (active) return;
	InputController::change_mode(0x101);
	func = f;
	active = true;
}

void end_trans() {
	InputController::change_mode(0x101);
	active = false;
	first_half = true;
	counter = 0;
}

void run_trans() {
	if (!active) return;
	trans_brush->SetColor(D2D1::ColorF(0.0, 0.0, 0.0, counter / 7.0f));
	Graphics::rtarget->FillRectangle(D2D1::RectF(-1000, -1000, 2000, 2000), trans_brush);
	if (first_half) counter++;
	else counter--;
	if (counter == MAX_COUNTER) {
		first_half = false;
		func();
	}
	if (counter == 0) end_trans();
}