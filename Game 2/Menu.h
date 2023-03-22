#pragma once

#include <unordered_map>

#include "Graphics.h"
#include "Inputable.h"

class Menu : public Inputable {
public:
	Menu() {
		menu_map = {
			{1,{1,2,3,4}},
			{2,{1}},
			{3,{1}},
			{4,{1}}
		};
		current = 1;
		things = &(menu_map[current]);
		pointer = 1;
		active = false;
		transform = D2D1::IdentityMatrix();
	}
	void load() {};
	void draw() {
		if (InputController::get_mode() != VK_TAB) return;
		Graphics::rtarget->GetTransform(&transform);
		Graphics::rtarget->SetTransform(D2D1::IdentityMatrix());
		switch (current) {
		case 1:
			draw1();
			break;
		case 2:
			draw2();
			break;
		case 3:
			draw3();
			break;
		default:
			back();
			break;
		}
		Graphics::rtarget->SetTransform(transform);
	};
	void draw1() {
		Graphics::rtarget->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(10, 10, 790, 590), 5, 5),
			Graphics::getSolidColorBrush("teal"));
		Graphics::rtarget->DrawTextW(L"Menu", 4, Graphics::getTextFormat("font"),
			D2D1::RectF(100, 50, 300, 150), Graphics::getSolidColorBrush("black"));
		Graphics::rtarget->DrawTextW(L"Inventory", 9, Graphics::getTextFormat("font"),
			D2D1::RectF(350, 150, 450, 200), Graphics::getSolidColorBrush("black"));
		Graphics::rtarget->DrawTextW(L"Controls",8, Graphics::getTextFormat("font"),
			D2D1::RectF(350, 250, 450, 300), Graphics::getSolidColorBrush("black"));
		Graphics::rtarget->DrawTextW(L"Game", 4, Graphics::getTextFormat("font"),
			D2D1::RectF(350, 350, 450, 400), Graphics::getSolidColorBrush("black"));
		float top, bottom;
		switch(pointer) {
		case 1:
			top = 145; bottom = 185;
			break;
		case 2:
			top = 245; bottom = 285;
			break;
		case 3:
			top = 345; bottom = 385;
			break;
		default:
			top = 0; bottom = 0;
			break;
		}
		Graphics::rtarget->DrawRectangle(D2D1::RectF(345, top, 455, bottom), Graphics::getSolidColorBrush("black"), 2.0f);
	}
	void draw3() {
		Graphics::rtarget->Clear(D2D1::ColorF(1.0, 0.0, 1.0));
		Graphics::rtarget->DrawTextW(L"cONtRolS", 8, Graphics::getTextFormat("font"), 
			D2D1::RectF(600,200,800,400), Graphics::getSolidColorBrush("black"));
		Graphics::rtarget->DrawTextW(L"W|A|S|D", 7, Graphics::getTextFormat("font"),
			D2D1::RectF(200, 350, 400, 400), Graphics::getSolidColorBrush("black"));
		Graphics::rtarget->DrawTextW(L"E", 1, Graphics::getTextFormat("font"),
			D2D1::RectF(200, 400, 400, 450), Graphics::getSolidColorBrush("black"));
		Graphics::rtarget->DrawTextW(L"Q", 1, Graphics::getTextFormat("font"),
			D2D1::RectF(200, 450, 400, 500), Graphics::getSolidColorBrush("black"));
		Graphics::rtarget->DrawTextW(L"TAB", 3, Graphics::getTextFormat("font"),
			D2D1::RectF(200, 500, 400, 550), Graphics::getSolidColorBrush("black"));
		Graphics::rtarget->DrawTextW(L"move around", 11, Graphics::getTextFormat("font"),
			D2D1::RectF(400, 350, 800, 400), Graphics::getSolidColorBrush("black"));
		Graphics::rtarget->DrawTextW(L"do the thing", 12, Graphics::getTextFormat("font"),
			D2D1::RectF(400, 400, 800, 450), Graphics::getSolidColorBrush("black"));
		Graphics::rtarget->DrawTextW(L"(un)do the thing", 16, Graphics::getTextFormat("font"),
			D2D1::RectF(400, 450, 800, 500), Graphics::getSolidColorBrush("black"));
		Graphics::rtarget->DrawTextW(L"menu", 4, Graphics::getTextFormat("font"),
			D2D1::RectF(400, 500, 800, 550), Graphics::getSolidColorBrush("black"));
	}
	void draw2() {
		Graphics::rtarget->Clear(D2D1::ColorF(1.0, 1.0, 0.0));
	}
	void forward() {
		if ((*things).size() < 2) return;
		current = (*things)[pointer];
		things = &(menu_map[current]);
		pointer = 1;
	}
	void back() {
		current = (*things)[0];
		things = &(menu_map[current]);
		pointer = 1;
	}
	void input_start(WPARAM wParam) {
		if (InputController::get_mode() != VK_TAB) return;
		switch (wParam) {
		case VK_W:
			pointer--;
			if (pointer == 0) pointer = (int)(*things).size() - 1;
			break;
		case VK_S:
			pointer++;
			if (pointer == (*things).size()) pointer = 1;
			break;
		case VK_E:
			forward();
			break;
		case VK_Q:
			back();
			break;
		default:
			break;
		}
	}
	void input_end(WPARAM wParam) { return; }
private:
	std::unordered_map<int,std::vector<int>> menu_map;
	int current;
	int pointer;
	std::vector<int> *things;
	bool active;
	D2D1_MATRIX_3X2_F transform;
};