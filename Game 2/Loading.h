#pragma once

#include <thread>

#include "Graphics.h"

bool loading = false;

void draw_loading_screen_init() {
	while (loading) {
		Graphics::rtarget->BeginDraw();
		Graphics::rtarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
		Graphics::rtarget->DrawTextW(
			L"LOADING",
			7,
			Graphics::getTextFormat("macro"),
			D2D1::RectF(200.0f, 200.0f, 800.0f, 600.0f),
			Graphics::getSolidColorBrush("white")
			);
		Graphics::rtarget->EndDraw();
	}
}

void start_load() { 
	loading = true;
}

void end_load() {
	Sleep(2000);
	loading = false;
}

