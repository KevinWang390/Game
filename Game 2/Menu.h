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
	int type;
	bool selected;
	float x, y;
	std::string name;

	friend class Menu;
	friend class Page;
	friend class Link;
public:
	PageElement() {
		type = -1;
		selected = false;
		x = y = 0.0f;
		name = "";
	};
	PageElement(int t, float xpos, float ypos, std::string n) {
		type = t;
		selected = false;
		x = xpos; y = ypos;
		name = n;
	};
	int what_type() { return type; }
	virtual void* click(WPARAM wParam) = 0;
	virtual void draw() = 0; // different based off value of selected
};

class Link : public PageElement {
	std::string target;
public:
	Link() {
		target = "";
	}
	Link(std::string t, float xpos, float ypos, std::string n) : PageElement(LINK, xpos, ypos, n) {
		target = t;
	}
	void* click(WPARAM wParam) override { return (void*)&target; }
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
	Page(std::string n, int id, std::vector<PageElement*>& e, std::vector<PageElement*>& d, Overlay* o) {
		name = n;
		ID = id;
		element_idx = 0;
		elements = e;
		display = d;
		overlay = o;
		if (elements.size()) currentElement = elements[0];
		else currentElement = nullptr;
	}
	void drawPage() {
		Graphics::rtarget->FillRoundedRectangle(
			D2D1::RoundedRect(D2D1::RectF(15.0f, 15.0f, 785.0f, 585.0f), 10.0f, 10.0f),
			Graphics::getSolidColorBrush("teal"));
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
		if (InputController::get_mode() != VK_TAB) return;
		Graphics::rtarget->GetTransform(&transform);
		Graphics::rtarget->SetTransform(D2D1::IdentityMatrix());
		if (currentPage) currentPage->drawPage();
		Graphics::rtarget->SetTransform(transform);
	}
	void input_start(WPARAM wParam) {
		if (InputController::get_mode() != VK_TAB) return;
		//int type = currentPage->currentElement->what_type(); // TODO: this still doesn't work
		switch (wParam) {
		case VK_TAB:
			backtrace.clear();
			enter_page(entryPage);
			break;
		case VK_Q:
			back_out();
			break;
		case VK_E:
			//if (type == LINK) enter_page((Page*)(currentPage->currentElement->click(-1)));
			break;
		case VK_W:
		case VK_S:
			currentPage->move_idx(wParam);
		default:
			break;
		}
	}
	void input_end(WPARAM wParam) { return; }
};

Menu* get_field_menu() { // TODO: hardcode the menu creator, this will be called in run() in scripts.h
	Menu* m = new Menu();
	std::vector<PageElement*> p_elist;
	std::vector<PageElement*> p_dlist;
	p_elist.push_back(new Link("none", 30.0f, 30.0f, "dummy"));
	p_elist.push_back(new Link("none", 30.0f, 100.0f, "dummy"));
	p_elist.push_back(new Link("none", 30.0f, 170.0f, "dummy"));
	Page* p = new Page("entry", 0, p_elist, p_dlist, nullptr);
	m->add_page(p, true);
	return m;
}

