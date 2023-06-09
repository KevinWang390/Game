#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Graphics.h"
#include "Inputable.h"
#include "Inventory.h"

#define LINK 1
#define TABLE 2

// TODO: UI bitmaps will be global values, classes will draw them, will need function to load from file
ID2D1Bitmap* ui_page_base;
ID2D1Bitmap* ui_button;
ID2D1Bitmap* ui_table;
ID2D1Bitmap* ui_table_select;
ID2D1Bitmap* ui_item_restor;

class Page;
class Overlay;
class Menu;
class Table;

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
		D2D1_RECT_F source;
		if (selected) source = D2D1::RectF(0.0f, 30.0f, 110.0f, 60.0f);
		else source = D2D1::RectF(0.0f, 0.0f, 110.0f, 30.0f);
		Graphics::rtarget->DrawBitmap(
			ui_button,
			D2D1::RectF(x, y, x + 220.0f, y + 60.0f),
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
			source
		);
		Graphics::rtarget->DrawTextW(std::wstring(name.begin(), name.end()).c_str(),
			name.size(),
			Graphics::getTextFormat("font"),
			D2D1::RectF(x+20.0f, y+18.0f, x+200.0f, y+60.0f),
			Graphics::getSolidColorBrush("ui_dark"));
	} // TODO: draw code goes here
};

class Button : public PageElement {}; // will likely need to invoke some other file for game-affecting functions

class Tab : public PageElement {}; // TODO: decide if I need this at all

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

// IMPORTANT: Inventory::unpickle() must be run before any of these are created, else will crash
/* 
* Note: Table is a PageElement, but it behaves more like a Page:
* -- it receives and processes menu movement inputs
* -- -- Page will check what_type(), then route input to the Table if appropriate
* -- it takes up the whole page
* -- it manages its own sub-units and is meant to be the only PageElement on the page
* 
* it is possible to add more PageElements to the page, if it becomes necessary
*/
class Table : public PageElement {
	// stuff for linking to inventory
	unsigned int item_type;
	std::vector<std::pair<unsigned int, int>> *items;
	ID2D1Bitmap* item_src_bitmap;

	// stuff for display
	int idx, idx_x, idx_y;
	D2D1_RECT_F select_src;

	friend class Menu;
public:
	Table() {
		item_type = 0;
		items = {};
		item_src_bitmap = nullptr;
		idx = idx_x = idx_y = 0;
		select_src = D2D1::RectF(40.0f, 136.0f, 108.0f, 204.0f);
	};
	Table(std::string n, unsigned int it, ID2D1Bitmap* isrc) : PageElement(0.0f, 0.0f, n) {
		item_type = it;
		items = &(Inventory::inventories[it]->display_list);
		item_src_bitmap = isrc;
		idx = idx_x = idx_y = 0;
		select_src = D2D1::RectF(40.0f, 136.0f, 108.0f, 204.0f);
	}
	int what_type() override { return TABLE; }
	void* click() override { return nullptr; } // TODO
	void draw() override {
		Graphics::rtarget->DrawBitmap(
			ui_table,
			D2D1::RectF(0.0f, 0.0f, 800.0f, 600.0f),
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
		Graphics::rtarget->DrawBitmap(
			ui_table_select,
			select_src,
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
		int i = 0;
		for (auto item : *items) {
			// determine bitmap source from low-word of item code
			int src_idx = (item.first & 0xff) - 1;
			float src_left = (src_idx % 5) * 32.0f;
			float src_top = (src_idx / 5) * 32.0f;
			D2D1_RECT_F src = D2D1::RectF(src_left, src_top, src_left + 32.0f, src_top + 32.0f);
			// determine draw destination from number
			float dest_left = 40.0f + (i % 5) * 70.0f;
			float dest_top = 136.0f + (i / 5) * 70.0f;
			i++;
			D2D1_RECT_F dest = D2D1::RectF(dest_left, dest_top, dest_left + 64.0f, dest_top + 64.0f);
			Graphics::rtarget->DrawBitmap(
				item_src_bitmap,
				dest,
				1.0f,
				D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
				src
			);
		}
		if (idx < items->size()) {
			// TODO: draw the item details
		}
	}
	void move(WPARAM wParam) { // moving around the table
		switch (wParam) {
		case VK_W:
			idx_y--; break;
		case VK_A:
			idx_x--; break;
		case VK_S:
			idx_y++; break;
		case VK_D:
			idx_x++; break;
		default:
			break;
		}

		if (idx_x == 5) idx_x = 0;
		else if (idx_x == -1) idx_x = 4;
		if (idx_y == 6) idx_y = 0;
		else if (idx_y == -1) idx_y = 5;

		idx = idx_x + 5 * idx_y; // will be used for accessing item list

		float src_left = 40.0f + 70.0f * idx_x;
		float src_top = 136.0f + 70.0f * idx_y;
		select_src = D2D1::RectF(src_left, src_top, src_left + 68.0f, src_top + 68.0f);
	}
};

class Page {
	// parent class
	std::string name;
	int ID; // dunno what i'm gonna use this for
	int element_idx;
	PageElement* currentElement; // current page element selected
	std::vector<PageElement*> elements; // elements that should be selected, element 0 will be the entry element
	std::vector<PageElement*> display; // elements that should not be selected, e.g. images, text
	Overlay* overlay; // the current overlay (will be nullptr if no overlay is active)

	friend class Table;
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
			ui_page_base,
			D2D1::RectF(0.0f, 0.0f, 800.0f, 600.0f),
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
		Graphics::rtarget->DrawTextW(std::wstring(name.begin(), name.end()).c_str(),
			name.size(),
			Graphics::getTextFormat("header"),
			D2D1::RectF(50.0f, 50.0f, 600.0f, 100.0f),
			Graphics::getSolidColorBrush("ui_light"));
		for (auto e : elements) e->draw();
		for (auto d : display) d->draw();
	}
	void move_idx(WPARAM wParam) {
		if (currentElement->what_type() == TABLE) {
			Table* t = (Table*)currentElement;
			t->move(wParam);
			return;
		}
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
			if (type == LINK) {
				Page* p = (Page*)(currentPage->currentElement->click());
				enter_page(p);
			}
			break;
		case VK_W:
		case VK_A:
		case VK_S:
		case VK_D:
			currentPage->move_idx(wParam);
			break;
		default:
			break;
		}
	}
	void input_end(WPARAM wParam) { return; }
};

void load_menu_bitmaps() {
	ui_page_base = Graphics::bitmapFromFilename(L"ui_page_base.png");
	ui_button = Graphics::bitmapFromFilename(L"ui_button.png");
	ui_table = Graphics::bitmapFromFilename(L"ui_table.png");
	ui_table_select = Graphics::bitmapFromFilename(L"ui_table_select.png");
	ui_item_restor = Graphics::bitmapFromFilename(L"ui_item_restor.png");
}

Menu* get_field_menu() { // TODO: hardcode the menu creator, this will be called in run() in scripts.h
	Menu* m = new Menu(VK_TAB);

	// DECLARATION
	// declare base page
	Page* base = new Page("Menu", 0, nullptr);

	// declare inventory pages
	Page* inv = new Page("Inventory", 1, nullptr);
	Page* consum = new Page("Consumables", 2, nullptr);
	Page* equip = new Page("Equipment", 3, nullptr);
	Page* key = new Page("Key Items", 4, nullptr);

	// declare status page
	Page* stat = new Page("Party Status", 5, nullptr);

	// ASSEMBLY
	// assemble base
	base->add_element(new Link(inv, 50.0f, 140.0f, "Inventory"));
	base->add_element(new Link(stat, 50.0f, 220.0f, "Status"));
	base->add_element(new Link(nullptr, 50.0f, 300.0f, "Game"));

	// assemble inv
	inv->add_element(new Link(consum, 50.0f, 140.0f, "Consumables"));
	inv->add_element(new Link(equip, 50.0f, 220.0f, "Equipment"));
	inv->add_element(new Link(key, 50.0f, 300.0f, "Key Items"));
	consum->add_element(new Table("Consumables", 0b00010001, ui_item_restor));

	// set menu entry
	m->add_page(base, true);
	return m;
}

