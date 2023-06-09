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
std::unordered_map<unsigned int, std::wstring> item_descriptions = {

}