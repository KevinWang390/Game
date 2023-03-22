#pragma once
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>

#include "Graphics.h"
#include "Entry.h"
#include "Dialogue.h"

//TODO: this

class Bounds { //TODO: make this a proper level
public:
	Bounds(int i, std::string filename) {
		id = i;
		bounds_map[i] = this;
		Graphics::factory->CreatePathGeometry(&border);
		sink = nullptr;
		border->Open(&sink);
		load(filename);
	}
	static void draw() {
		Graphics::rtarget->FillGeometry(current->border, Graphics::getSolidColorBrush("blue"));
		std::vector<Entry*> *v = &(current->entries);
		for (Entry* e : *v) {
			if (e->automatic) Graphics::rtarget->FillGeometry(e->get_geom(), Graphics::getSolidColorBrush("green"));
			else Graphics::rtarget->FillGeometry(e->get_geom(), Graphics::getSolidColorBrush("yellow"));
		}
		std::vector<Dialogue*>* v2 = &(current->dialogues);
		for (Dialogue* d : *v2) {
			d->draw();
		}
	}
	static void go_to(int i, bool entry = false) { //TODO: give go_to a default player position
		if (bounds_map.find(i) == bounds_map.end()) return;
		else current = bounds_map[i];
		xl = current->x_cam_l;
		xu = current->x_cam_u;
		yl = current->y_cam_l;
		yu = current->y_cam_u;
	}
	static ID2D1PathGeometry* get_geom() {
		return current->border;
	}
	//TODO: make this work
	static Bounds* current;
	static float xl, xu, yl, yu;
private:
	void load(std::string filename) {
		std::ifstream f;
		f.open(filename);
		std::string s = "";

		// load camera bounds
		f >> s;
		x_cam_l = std::stof(s);
		f >> s;
		x_cam_u = std::stof(s);
		f >> s;
		y_cam_l = std::stof(s);
		f >> s;
		y_cam_u = std::stof(s);

		// load entry points
		f >> s;
		int num = std::stoi(s);
		for (num; num > 0; num--) {
			f >> s;
			int target = std::stoi(s);
			f >> s;
			float playerx = std::stof(s);
			f >> s;
			float playery = std::stof(s);
			f >> s;
			float left = std::stof(s);
			f >> s;
			float top = std::stof(s);
			f >> s;
			float right = std::stof(s);
			f >> s;
			float bottom = std::stof(s);
			f >> s;
			int automatic = std::stoi(s);
			entries.push_back(new Entry(target, playerx, playery, left, top, right, bottom, automatic));
		}

		// load dialogues
		f >> s;
		num = std::stoi(s);
		for (num; num > 0; num--) {
			f >> s;
			std::string filename = s;
			f >> s;
			float left = std::stof(s);
			f >> s;
			float top = std::stof(s);
			f >> s;
			float right = std::stof(s);
			f >> s;
			float bottom = std::stof(s);
			dialogues.push_back(new Dialogue(filename, left, top, right, bottom));
		}

		// load border
		f >> s;
		float x = std::stof(s);
		f >> s;
		float y = std::stof(s);

		sink->SetFillMode(D2D1_FILL_MODE_ALTERNATE);
		sink->BeginFigure(D2D1::Point2F(x, y), D2D1_FIGURE_BEGIN_FILLED);

		while (f) {
			f >> s;
			x = std::stof(s);
			f >> s;
			y = std::stof(s);
			sink->AddLine(D2D1::Point2F(x, y));
		}
		sink->EndFigure(D2D1_FIGURE_END_CLOSED);
		sink->Close();
		f.close();
	}
	int id;
	ID2D1PathGeometry* border;
	ID2D1GeometrySink* sink;
	float x_cam_l, x_cam_u, y_cam_l, y_cam_u;
	static Player* player;
	static std::unordered_map<int, Bounds*> bounds_map;
	std::vector<Entry*> entries;
	std::vector<Dialogue*> dialogues;

	friend class Player;
};