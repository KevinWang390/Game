#pragma once

class Inputable {
public:
	virtual void input_start(WPARAM i) = 0;
	virtual void input_end(WPARAM i) = 0;
private:
	bool active;

	friend class Player;
	friend class Menu;
};