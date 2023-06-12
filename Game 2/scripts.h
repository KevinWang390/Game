#pragma once
#include "Position.h"
#include "Velocity8.h"
#include "InputController.h"
#include "Player.h"
#include "Menu.h"
#include "Bounds.h"
#include "Entry.h"
#include "Transition.h"
#include "Dialogue.h"
#include "Inventory.h"
#include "Loading.h"

Player* Player::instance = nullptr;

int InputController::mode = 0;
std::vector<Inputable*> InputController::members;
std::unordered_set<WPARAM> InputController::mode_keys;

Bounds* Bounds::current = nullptr;
Player* Bounds::player = nullptr;
std::unordered_map<int, Bounds*> Bounds::bounds_map;
float Bounds::xl, Bounds::xu, Bounds::yl, Bounds::yu;

bool Dialogue::lock = false;
std::ifstream Dialogue::file;
std::wstring Dialogue::text;
D2D1_MATRIX_3X2_F Dialogue::transform;

Player* player;
Menu* menu;
InputController* ic;
Bounds* bounds;
Bounds* bounds1;
Bounds* bounds2;

Dialogue* dialogue_master;

std::unordered_map<unsigned int, Inventory*> Inventory::inventories;

void setup() {
	player = new Player();

	// load inventory system
	Inventory::unpickle();
	// Inventory::pickle();
	Inventory::refresh_all();


	// menu bitmaps
	load_menu_bitmaps();

	// build menus
	menu = get_field_menu();

	// build levels
	bounds = new Bounds(0, "bounds.txt");
	bounds1 = new Bounds(1, "bounds1.txt");
	bounds2 = new Bounds(2, "bounds2.txt");
	Bounds::go_to(0);

	// bruh
	dialogue_master = new Dialogue("i hate this", 0.0, 69.69f, 420.0, 0.0);

	// attach stuff to InpuController
	std::vector<Inputable*> v = { player, menu, dialogue_master };
	InputController::init(v);

	// initialize transition animation
	init_trans();

	// create animation resources
	Graphics::setSolidColorBrush(1.0, 0.0, 0.0, 1.0, "brush");
	Graphics::setSolidColorBrush(0.0, 0.0, 0.0, 1.0, "black");
	Graphics::setSolidColorBrush(0.0, 0.0, 1.0, 1.0, "blue");
	Graphics::setSolidColorBrush(0.0, 1.0, 0.0, 1.0, "green");
	Graphics::setSolidColorBrush(1.0, 0.0, 1.0, 1.0, "pink");
	Graphics::setSolidColorBrush(1.0, 1.0, 0.0, 1.0, "yellow");
	Graphics::setSolidColorBrush(0.0, 1.0, 1.0, 1.0, "teal");

	Graphics::setSolidColorBrush(0.796f, 0.859f, 0.988f, 1.0f, "ui_light");
	Graphics::setSolidColorBrush(0.247f, 0.247f, 0.455f, 1.0f, "ui_dark");

	Graphics::setTextFormat(L"Game-2.2", 27, "font");
	Graphics::setTextFormat(L"Game-2.2", 45, "header");
	Graphics::setTextFormat(L"Game-2.2", 18, "small");

}

void run() {
	int mode = InputController::get_mode();
	Bounds::draw();
	player->move();
	player->move_cam();
	player->draw();
	Dialogue::draw_s();
	menu->draw();
	run_trans();
}