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

	menu = get_field_menu();

	bounds = new Bounds(0, "bounds.txt");
	bounds1 = new Bounds(1, "bounds1.txt");
	bounds2 = new Bounds(2, "bounds2.txt");
	Bounds::go_to(0);

	dialogue_master = new Dialogue("i hate this", 0.0, 69.69f, 420.0, 0.0);

	std::vector<Inputable*> v = { player, menu, dialogue_master };
	InputController::init(v);

	init_trans();

	Graphics::setSolidColorBrush(1.0, 0.0, 0.0, 1.0, "brush");
	Graphics::setSolidColorBrush(1.0, 1.0, 1.0, 1.0, "white");
	Graphics::setSolidColorBrush(0.0, 0.0, 0.0, 1.0, "black");
	Graphics::setSolidColorBrush(0.0, 0.0, 1.0, 1.0, "blue");
	Graphics::setSolidColorBrush(0.0, 1.0, 0.0, 1.0, "green");
	Graphics::setSolidColorBrush(1.0, 0.0, 1.0, 1.0, "pink");
	Graphics::setSolidColorBrush(1.0, 1.0, 0.0, 1.0, "yellow");
	Graphics::setSolidColorBrush(0.0, 1.0, 1.0, 1.0, "teal");


	Graphics::setTextFormat(L"Consolas", 20, "font");

	// testing this out
	Inventory::unpickle();
	// Inventory::pickle();
	Inventory::refresh_all();

	// menu bitmaps
	load_menu_bitmaps();
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