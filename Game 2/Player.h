#pragma once
#include <functional>

#include "Velocity8.h"
#include "Position.h"
#include "Graphics.h"
#include "Inputable.h"
#include "Bounds.h"
#include "Transition.h"

class Player : public Inputable {
public:
	Player() {
		size = 20.0;
		interact = false;
		vel = new Velocity8(3);
		pos = new Position();
		xbuff = 400.0f;
		ybuff = 300.0f;
		src = nullptr;
		hitbox = nullptr;
		Graphics::factory->CreateRectangleGeometry(D2D1::RectF(0, 0, size, size), &src);
		instance = this;
	}
	void input_start(WPARAM id) {
		switch (id) {
		case(VK_W):
			vel->dec_y(); break;
		case(VK_A):
			vel->dec_x(); break;
		case(VK_S):
			vel->inc_y(); break;
		case(VK_D):
			vel->inc_x(); break;
		case(VK_E):
			interact = true;
		default:
			break;
		}
	}
	void input_end(WPARAM id) {
		switch (id) {
		case(VK_W):
			vel->inc_y(); break;
		case(VK_A):
			vel->inc_x(); break;
		case(VK_S):
			vel->dec_y(); break;
		case(VK_D):
			vel->dec_x(); break;
		case(VK_E):
			interact = false;
		default:
			break;
		}
	}
	void draw() {
		Graphics::factory->CreateTransformedGeometry(src, D2D1::Matrix3x2F::Translation(pos->get_x(), pos->get_y()), &hitbox);
		if (!hitbox) return;
		Graphics::rtarget->FillGeometry(hitbox,
			Graphics::getSolidColorBrush("brush"));
	}
	void enter(Entry* e) {
		interact = false;
		Bounds::go_to(e->target_bounds);
		float xpos = e->playerx;
		pos->set_x(xpos);
		if (xpos <= Bounds::xl) {
			Graphics::rtarget->SetTransform(D2D1::Matrix3x2F::Translation(
				400 - Bounds::xl, ybuff));
			xbuff = 400 - Bounds::xl;
		}
		else if (xpos >= Bounds::xu) {
			Graphics::rtarget->SetTransform(D2D1::Matrix3x2F::Translation(
				400 - Bounds::xu, ybuff));
			xbuff = 400 - Bounds::xu;
		}
		float ypos = e->playery;
		pos->set_y(ypos);
		if (ypos <= Bounds::yl) {
			Graphics::rtarget->SetTransform(D2D1::Matrix3x2F::Translation(
				xbuff, 300 - Bounds::yl));
			ybuff = Bounds::yl;
		}
		else if (ypos >= Bounds::yu) {
			Graphics::rtarget->SetTransform(D2D1::Matrix3x2F::Translation(
				xbuff, 300 - Bounds::yu));
			ybuff = Bounds::yu;
		}
	}
	void move() {
		if (InputController::get_mode()) return;
		if (!hitbox) return;
		
		// move, checking for wall collision
		vel->normalize();
		float xs = vel->get_x_speed();
		float ys = vel->get_y_speed();
		D2D1_GEOMETRY_RELATION result = D2D1_GEOMETRY_RELATION_UNKNOWN;
		hitbox->CompareWithGeometry(Bounds::get_geom(), D2D1::Matrix3x2F::Translation(-xs, 0), &result);
		if (result != D2D1_GEOMETRY_RELATION_OVERLAP) pos->change_x(xs);
		hitbox->CompareWithGeometry(Bounds::get_geom(), D2D1::Matrix3x2F::Translation(0, -ys), &result);
		if (result != D2D1_GEOMETRY_RELATION_OVERLAP) pos->change_y(ys);

		// check for border entry
		std::vector<Entry*>* v = &Bounds::current->entries;
		for (Entry* e : *v) {
			if (!e->automatic && !interact) continue;
			hitbox->CompareWithGeometry(e->get_geom(), D2D1::Matrix3x2F::Identity(), &result);
			if (result != D2D1_GEOMETRY_RELATION_DISJOINT) {
				std::function<void()> f = [this, e]() {this->enter(e); };
				start_trans(f);
			}
		}

		// check for dialogue
		std::vector<Dialogue*>* v2 = &Bounds::current->dialogues;
		if (interact) for (Dialogue* d : *v2) {
			if (Dialogue::lock) break;
			hitbox->CompareWithGeometry(d->get_geom(), D2D1::IdentityMatrix(), &result);
			if (result != D2D1_GEOMETRY_RELATION_DISJOINT) Dialogue::start(d);
		}
	}
	void move_cam() {
		// adjust camera as needed
		float xpos = pos->get_x();
		float ypos = pos->get_y();
		if (xpos > Bounds::xl && xpos < Bounds::xu) {
			Graphics::rtarget->SetTransform(D2D1::Matrix3x2F::Translation(
				400 - xpos, ybuff));
			xbuff = 400 - xpos;
		}
		if (ypos > Bounds::yl && ypos < Bounds::yu) {
			Graphics::rtarget->SetTransform(D2D1::Matrix3x2F::Translation(
				xbuff, 300 - ypos));
			ybuff = 300 - ypos;
		}
	}
private:
	static Player* instance;
	bool interact;
	float size;
	float xbuff;
	float ybuff;
	Velocity8* vel;
	Position* pos;
	ID2D1RectangleGeometry* src;
	ID2D1TransformedGeometry* hitbox;

};