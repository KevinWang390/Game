#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>

#include "Graphics.h"
#include "Inputable.h"

#define LINK 1

class Page;
class Overlay;

class PageElement {
	bool selected;
	float x, y;
	std::string name;

	friend class Link;
public:
	PageElement() {
		selected = false;
		x = y = 0.0f;
		name = "none";
	};
	PageElement(float xpos, float ypos, std::string n) {
		selected = false;
		x = xpos; y = ypos;
		name = n;
	};
	virtual int what_type() = 0;
	virtual void move() = 0;
	virtual void* click(WPARAM wParam) = 0;
	virtual void draw() = 0; // different based off value of selected
};

class Link : public PageElement {
	std::string target;
	bool selected;
public:
	int what_type() { return LINK; }
	void move() {} // TODO:
	void* click(WPARAM wParam) {
		if (wParam == VK_RETURN) return (void*)&target;
	}
	void draw() override {} // TODO: draw code goes here
};

class Tab : public PageElement {}; // TODO: decide if I need this at all

class Table : public PageElement {};

// Textbox
std::unordered_map<std::string, std::string> master_text; // stores text fields, will need to be pickled and unpickled

class Textbox : public PageElement {
	std::string label;
	std::string value;
	static std::vector<Textbox*> textboxes;
public:
	Textbox() {};
	void update() {};
	static void update_all() {};
	void draw() {};
};

std::vector<Textbox*> Textbox::textboxes = {};

// Image : does not need master unordered_map

class Image : public PageElement {
	std::string filename;
	ID2D1Bitmap* bitmap;
public:
	Image() {};
	void draw() {};
}; // does nothing

class Overlay : public PageElement {}; // TODO: know how to do this

class Page {
	// parent class
	std::string name;
	int ID;
	PageElement* entryElement; // initial selection upon entry (not meant to be changed on runtime)
	PageElement* currentElement; // current page element selected
	std::vector<PageElement*> elements; // draw the page elements
	Overlay* overlay; // the current overlay (will be nullptr if no overlay is active)

	friend class PageElement;
public:
	Page() {}
	void enterPage() {} // enters page, reinitializes currentElement
	void drawPage() {
		Graphics::rtarget->FillRoundedRectangle(
			D2D1::RoundedRect(D2D1::RectF(15.0f, 15.0f, 785.0f, 585.0f), 10.0f, 10.0f),
			Graphics::getSolidColorBrush("teal"));
		for (auto e : elements) {
			e->draw();
		}
	} // member function for draw
};

class Menu : public Inputable {
	D2D1_MATRIX_3X2_F transform;
	std::unordered_map<std::string, Page*> page_table;
	Page* currentPage; // current page to be displayed
	Page* entryPage; // initial page upon menu activation
	std::vector<Page*> backtrace; // stack structure for backing out of pages
public:
	Menu() {
		transform = D2D1::IdentityMatrix();
		page_table = {};
		entryPage = currentPage = nullptr;
		backtrace = {};
	}
	void draw() {
		if (InputController::get_mode() != VK_TAB) return;
		Graphics::rtarget->GetTransform(&transform);
		Graphics::rtarget->SetTransform(D2D1::IdentityMatrix());
		// actual drawing goes here
		Graphics::rtarget->SetTransform(transform);
	};
	void input_start(WPARAM wParam) {
		if (InputController::get_mode() != VK_TAB) return;
	}
	void input_end(WPARAM wParam) { return; }
};

// no need to "compile" menu from text file; it's small enough to do it manually in global function:

Menu* get_field_menu() { // TODO: hardcode the menu creator
	Menu* m = new Menu();
	return m;
}

