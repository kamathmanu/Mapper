/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */



#include "levenshtein_distance.h"

int levenshtein_distance(const std::string &s1, const std::string &s2) {
    //Computes the levenshtein_distance between two strings
	int s1len = s1.size();
	int s2len = s2.size();
	
	int column_start = 1;
	
	auto column = new decltype(s1len)[s1len + 1];
	std::iota(column + column_start, column + s1len + 1, column_start);
	
	for (auto x = column_start; x <= s2len; x++) {
		column[0] = x;
		auto last_diagonal = x - column_start;
		for (auto y = column_start; y <= s1len; y++) {
			auto old_diagonal = column[y];
			auto possibilities = {
				column[y] + 1,
				column[y - 1] + 1,
				last_diagonal + (tolower(s1[y - 1]) == tolower(s2[x - 1])? 0 : 1)
			};
			column[y] = std::min(possibilities);
			last_diagonal = old_diagonal;
		}
	}
	auto result = column[s1len];
	delete[] column;
	return result;
}

