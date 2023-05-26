#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Graphics.h"
#include "Inputable.h"

#define LINK 1

class Page;
class Overlay;
class Menu;

class PageElement {
	bool selected;
	float x, y;
	std::string name;

	friend class Link;
public:
	PageElement() {
		selected = false;
		x = y = 0.0f;
		name = "";
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
public:
	Link() {
		target = "";
	}
	Link(std::string t, float xpos, float ypos, std::string n) : PageElement(xpos, ypos, n) {
		target = t;
	}
	int what_type() override { return LINK; }
	void move() override {} // TODO:
	void* click(WPARAM wParam) override {
		if (wParam == VK_RETURN) return (void*)&target;
		return nullptr;
	}
	void draw() override {
		Graphics::rtarget->FillRoundedRectangle(D2D1::RoundedRect(
			D2D1::RectF(x, y, x + 100.0f, y + 50.0f), 10.0f, 10.0f),
			Graphics::getSolidColorBrush("yellow"));
	} // TODO: draw code goes here
};

class Button : public PageElement {}; // will likely need to invoke some other file for game-affecting functions

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
	int ID; // dunno what i'm gonna use this for
	PageElement* currentElement; // current page element selected
	std::vector<PageElement*> elements; // draw the page elements, element 0 will be the entry element
	Overlay* overlay; // the current overlay (will be nullptr if no overlay is active)

	friend class PageElement;
	friend class Menu;
public:
	Page() {
		name = "";
		ID = -1;
		currentElement = nullptr;
		elements = {};
		overlay = nullptr;
	}
	Page(std::string n, int id, std::vector<PageElement*>& e, Overlay* o) {
		name = n;
		ID = id;
		elements = e;
		overlay = o;
		if (elements.size()) currentElement = elements[0];
		else currentElement = nullptr;
	}
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
	void add_page(Page* p, bool entry) {
		if (entry) entryPage = p;
		page_table[p->name] = p;
	}
	void draw() {
		if (InputController::get_mode() != VK_TAB) return;
		Graphics::rtarget->GetTransform(&transform);
		Graphics::rtarget->SetTransform(D2D1::IdentityMatrix());
		if (currentPage) currentPage->drawPage();
		Graphics::rtarget->SetTransform(transform);
	};
	void input_start(WPARAM wParam) {
		if (InputController::get_mode() != VK_TAB) return;
		switch (wParam) {
		case VK_TAB:
			currentPage = entryPage; // TODO: sort out the page's entry element
			break;
		default:
			break;
		}
	}
	void input_end(WPARAM wParam) { return; }
};

// no need to "compile" menu from text file; it's small enough to do it manually in global function:

Menu* get_field_menu() { // TODO: hardcode the menu creator
	Menu* m = new Menu();
	std::vector<PageElement*> p_elist;
	p_elist.push_back(new Link("none", 30.0f, 30.0f, "dummy"));
	Page* p = new Page("entry", 0, p_elist, nullptr);
	m->add_page(p, true);
	return m;
}

