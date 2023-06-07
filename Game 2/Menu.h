#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Graphics.h"
#include "Inputable.h"
#include "Inventory.h"

#define LINK 1

// TODO: UI bitmaps will be global values, classes will draw them, will need function to load from file
ID2D1Bitmap* page_base;

class Page;
class Overlay;
class Menu;

class PageElement {
	bool selected;
	float x, y;
	std::string name;

	friend class Menu;
	friend class Page;
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
	virtual void* click() = 0;
	virtual void draw() = 0; // different based off value of selected
};

class Link : public PageElement {
	Page* target;
public:
	Link() {
		target = nullptr;
	}
	Link(Page* p, float xpos, float ypos, std::string n) : PageElement(xpos, ypos, n) {
		target = p;
	}
	int what_type() override { return LINK; }
	void* click() override { return (void*)target; }
	void draw() override {
		Graphics::rtarget->FillRoundedRectangle(D2D1::RoundedRect(
			D2D1::RectF(x, y, x + 100.0f, y + 50.0f), 10.0f, 10.0f),
			Graphics::getSolidColorBrush("yellow"));
		if (selected) Graphics::rtarget->DrawRoundedRectangle(D2D1::RoundedRect(
			D2D1::RectF(x, y, x + 100.0f, y + 50.0f), 10.0f, 10.0f),
			Graphics::getSolidColorBrush("blue"), 
			2.0f);
	} // TODO: draw code goes here
};

class Button : public PageElement {}; // will likely need to invoke some other file for game-affecting functions

class Tab : public PageElement {}; // TODO: decide if I need this at all

// table for the various inventories
class Table : public PageElement {}; // TODO: maybe this should be a Page?

// Textbox
std::unordered_map<std::string, std::string> master_text; // stores text fields, will need to be pickled and unpickled

class Textbox : public PageElement { // TODO: implement this
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
	Image() {
		filename = "";
		bitmap = nullptr;
	};
	void draw() {};
}; // does nothing

class Overlay : public PageElement {}; // TODO: know how to do this

class Page {
	// parent class
	std::string name;
	int ID; // dunno what i'm gonna use this for
	int element_idx;
	PageElement* currentElement; // current page element selected
	std::vector<PageElement*> elements; // elements that should be selected, element 0 will be the entry element
	std::vector<PageElement*> display; // elements that should not be selected, e.g. images, text
	Overlay* overlay; // the current overlay (will be nullptr if no overlay is active)

	friend class PageElement;
	friend class Menu;
public:
	Page() {
		name = "";
		ID = -1;
		element_idx = 0;
		currentElement = nullptr;
		elements = display = {};
		overlay = nullptr;
	}
	Page(std::string n, int id, Overlay* o) {
		name = n;
		ID = id;
		element_idx = 0;
		elements = display = {};
		overlay = o;
		if (elements.size()) currentElement = elements[0];
		else currentElement = nullptr;
	}
	void add_element(PageElement* e) {
		elements.push_back(e);
	}
	void add_display(PageElement* d) {
		display.push_back(d);
	}
	void drawPage() {
		Graphics::rtarget->DrawBitmap(
			page_base,
			D2D1::RectF(0.0f, 0.0f, 800.0f, 600.0f),
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
			);
		for (auto e : elements) e->draw();
		for (auto d : display) d->draw();
	}
	void move_idx(WPARAM wParam) {
		if (wParam == VK_W) {
			if (element_idx == 0) element_idx = elements.size() - 1;
			else element_idx--;
		}
		else if (wParam == VK_S) {
			element_idx++;
			if (element_idx == elements.size()) element_idx = 0;
		}
		currentElement->selected = false;
		currentElement = elements[element_idx];
		currentElement->selected = true;
	}
};

class Menu : public Inputable {
	WPARAM mode;
	D2D1_MATRIX_3X2_F transform;
	std::unordered_map<std::string, Page*> page_table; // TODO: maybe don't need this
	Page* currentPage; // current page to be displayed
	Page* entryPage; // initial page upon menu activation
	std::vector<Page*> backtrace; // stack structure for backing out of pages
public:
	Menu() {
		mode = 0;
		transform = D2D1::IdentityMatrix();
		entryPage = currentPage = nullptr;
	}
	Menu(WPARAM m) {
		mode = m;
		transform = D2D1::IdentityMatrix();
		page_table = {};
		entryPage = currentPage = nullptr;
		backtrace = {};
	}
	void add_page(Page* p, bool entry) {
		if (entry) entryPage = p;
		page_table[p->name] = p;
	}
	void enter_page(Page* p) { // goes to the page, saving it in the backtrace stack
		currentPage = p;
		currentPage->element_idx = 0;
		if (currentPage->elements.size()) {
			currentPage->currentElement = currentPage->elements[0];
			currentPage->currentElement->selected = true;
		}
		backtrace.push_back(currentPage);
	}
	void back_out() { // pops current page off the stack and goes to next; if none, then exits
		backtrace.pop_back();
		if (backtrace.empty()) InputController::change_mode(VK_TAB);
		else {
			currentPage = backtrace.back();
			currentPage->currentElement->selected = true;
		}
	}
	void draw() {
		if (InputController::get_mode() != mode) return;
		Graphics::rtarget->GetTransform(&transform);
		Graphics::rtarget->SetTransform(D2D1::IdentityMatrix());
		if (currentPage) currentPage->drawPage();
		Graphics::rtarget->SetTransform(transform);
	}
	void input_start(WPARAM wParam) {
		if (InputController::get_mode() != mode) return;
		int type;
		switch (wParam) {
		case VK_TAB:
			backtrace.clear();
			enter_page(entryPage);
			break;
		case VK_Q:
			back_out();
			break;
		case VK_E:
			if (currentPage->elements.empty()) break;
			type = currentPage->currentElement->what_type();
			if (type == LINK) enter_page((Page*)(currentPage->currentElement->click()));
			break;
		case VK_W:
		case VK_S:
			currentPage->move_idx(wParam);
			break;
		default:
			break;
		}
	}
	void input_end(WPARAM wParam) { return; }
};

void load_menu_bitmaps() {
	page_base = Graphics::bitmapFromFilename(L"ui_page_base.png");
}

Menu* get_field_menu() { // TODO: hardcode the menu creator, this will be called in run() in scripts.h
	Menu* m = new Menu(VK_TAB);
	Page* p = new Page("entry", 0, nullptr);
	Page* p2 = new Page("unnecessary", 1, nullptr);
	p->add_element(new Link(p2, 30.0f, 30.0f, "dummy"));
	p->add_element(new Link(nullptr, 30.0f, 100.0f, "dummy"));
	p->add_element(new Link(nullptr, 30.0f, 170.0f, "dummy"));
	m->add_page(p, true);
	return m;
}

