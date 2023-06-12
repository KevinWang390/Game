#pragma once

#include <bitset>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

#define INC 1
#define DEC 2
#define SET 3

class Inventory { // there is no master inventory for now
	std::vector<std::pair<unsigned int, int>> display_list; // has only the nonzero items with specific type_header, must be loaded from master
	std::unordered_map<unsigned int, int> items;
	static std::unordered_map<unsigned int, Inventory*> inventories; // maps header to appropriate inventory

	friend class Table;
public:
	static void unpickle() { // read items from file
		std::ifstream f;
		std::string s;
		unsigned int item_code = 0;
		unsigned int item_type = 0;
		unsigned int amount = 0;
		f.open("inventory.txt");
		while (f.good()) {
			f >> s;
			item_code = std::stoi(s, nullptr, 2);
			item_type = item_code >> 8;
			if (inventories.find(item_type) == inventories.end()) inventories.emplace(item_type, new Inventory());
			f >> s;
			amount = std::stoi(s);
			inventories[item_type]->items.emplace(item_code, amount);
		}
		f.close();
	}
	static void pickle() { // write items into file
		std::ofstream f;
		f.open("inventory.txt", 'w');
		for (auto& i : inventories) {
			Inventory* inv = i.second;
			for (auto& item : inv->items) {
				f << std::bitset<16>(item.first);
				f << " ";
				f << item.second;
				f << "\n";
			}
		}
		f.close();
	}
	static void refresh_all() {
		for (auto& i : inventories) {
			Inventory* inv = i.second;
			inv->display_list.clear();
			for (auto& item : inv->items) {
				if (item.second) inv->display_list.push_back(item);
			}
		}
	}
	static void update(unsigned int item, int mode, int val) {}; // TODO: update status of one item, looping through each Inventory
};

// item descriptions
std::unordered_map<unsigned int, std::wstring> item_names = {
	{0b0001000100000001, L"Potion of Flesh 1"},
	{0b0001000100000010, L"Potion of Flesh 2"},
	{0b0001000100000011, L"Potion of Flesh 3"},
	{0b0001000100000100, L"Potion of Flesh 4"},
	{0b0001000100000101, L"Potion of Spirit 1"},
	{0b0001000100000110, L"Potion of Spirit 2"},
	{0b0001000100000111, L"Potion of Spirit 3"},
	{0b0001000100001000, L"Potion of Spirit 4"},
	{0b0001000100001001, L"Potion of Life 1"},
	{0b0001000100001010, L"Potion of Life 2"},
	{0b0001000100001011, L"Potion of Life 3"},
	{0b0001000100001100, L"Korish Pellet"},
	{0b0001000100001101, L"Undecided"},
	{0b0001000100001110, L"Medicine"},
	{0b0001000100001111, L"Holy Water"}
};

// item descriptions
std::unordered_map<unsigned int, std::wstring> item_descriptions = {
	{0b0001000100000001, L"Restores health by 20%\nThe Potion of Flesh is thick and pungent, like salted milk with a hint of raspberry, but with no acidity. Strange."},
	{0b0001000100000010, L"Restores health by 35%\nPotions like this are in short supply and heavy demand, as they are effective for treating both wounds and various illness."},
	{0b0001000100000011, L"Restores health by 50%\nBears an unintelligible certificate of approval. Unfortunately, the origin and nature of these potions are unknown. All efforts to reproduce them have been unfruitful."},
	{0b0001000100000100, L"Restores health by 75%\nA much coveted and fought-over rarity. How many have died in search of a second chance at life?"},
	{0b0001000100000101, L"Restores mana by 20%\nGlass bottles are greatly inconvenient for the traveler. Yet, these potions degrade so quickly upon contact with air that it is impossible to transfer them from their original wax-sealed vessel."},
	{0b0001000100000110, L"Restores mana by 30%\nOne certainly grows fond of the taste, in time. Retired warriors have been known to seek out these precious bottles, long after their need for it has passed."},
	{0b0001000100000111, L"Restores mana by 50%\nIt is becoming harder and harder to find these. One day, there will be no more."},
	{0b0001000100001000, L"Restores mana by 75%\nIndeed, the spirit is the source of power. In this world, the size of your spirit determines your lot in life. Those born with pale spirits will have little use for such a thing, other than to sell."},
	{0b0001000100001001, L"Revives by 20%\nTo defeat death is to step into the domain of the divine. As such, the Potion of Life is not merely expensive; it is holy. The problem seems to be getting a dead person to drink it."},
	{0b0001000100001010, L"Revives by 30%\nThe potion performs the miracle of restarting biological processes, yet its power to restore tissue is relatively unremarkable. After a about day, the dead become dead forever."},
	{0b0001000100001011, L"Revives by 50%\nThis could buy you a lavish estate, complete with servants, livestock, and a yearly salary for you and your progeny. It might not be able to buy you a cozy cottage at the riverside, with an apple orchard to enjoy with your wife."},
	{0b0001000100001100, L"Korish Pellet: TO BE WRITTEN"},
	{0b0001000100001101, L"Korish Elixir: TO BE WRITTEN"},
	{0b0001000100001110, L"Cures poison.\nA small packet of various herbs and roots, used commonly by the denizens of Buttercup Valley. Chew on it counteract common poisons, irritants, fever, anxiety, getting-broken-up-with, etc."},
	{0b0001000100001111, L"Cures certain status defects.\nWater from Harsen's Well, delivered in a ceramic vial and covered with a piece of dark cloth. Just as there is life beyond bread, there are terrors beyond flame and iron."}
};

// TODO
// consumable attributes