#pragma once
#include <string>
#include <fstream>

#include "Graphics.h"
#include "Inputable.h"
#include "InputController.h"

class Dialogue : public Inputable {
public:
	Dialogue(std::string f, float left, float top, float right, float bottom) {
		filename = f;
		area = nullptr;
		Graphics::factory->CreateRectangleGeometry(D2D1::RectF(left, top, right, bottom), &area);
	}
	void draw() {
		Graphics::rtarget->FillGeometry(area, Graphics::getSolidColorBrush("pink"));
	}
	void input_start(WPARAM wParam) {
		if (InputController::get_mode() != DIALOGUE) return;
		switch (wParam) {
		case VK_E:
			Dialogue::forward();
			break;
		case VK_Q:
			Dialogue::end();
			break;
		default:
			break;
		}
	}
	void input_end(WPARAM wParam) {
		if (wParam == VK_E || wParam == VK_Q) lock = false;
	}
	ID2D1RectangleGeometry* get_geom() {
		return area;
	}
	static void start(Dialogue* d) {
		file.open(d->filename);
		forward();
		InputController::change_mode(DIALOGUE);

	}
	static void end() {
		InputController::change_mode(DIALOGUE);
		file.close();
		lock = true;
	}
	static void forward() {
		if (!file.good()) {
			end();
			return;
		};
		std::string t;
		std::getline(file, t);
		text = std::wstring(t.begin(), t.end());
	}
	static void draw_s() {
		if (InputController::get_mode() != DIALOGUE) return;
		Graphics::rtarget->GetTransform(&transform);
		Graphics::rtarget->SetTransform(D2D1::IdentityMatrix());
		Graphics::rtarget->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(100, 400, 700, 550), 5, 5),
			Graphics::getSolidColorBrush("teal"));
		Graphics::rtarget->DrawTextW(text.c_str(), (UINT32)text.size(), Graphics::getTextFormat("font"),
			D2D1::RectF(120, 420, 680, 530), Graphics::getSolidColorBrush("black"));
		Graphics::rtarget->SetTransform(transform);
	}
private:
	std::string filename;
	ID2D1RectangleGeometry* area;
	static bool lock;
	static std::ifstream file;
	static std::wstring text;
	static D2D1_MATRIX_3X2_F transform;

	friend class Player;
};