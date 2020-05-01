/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   levenshtein_distance.h
 * Author: tatlahar
 *
 * Created on March 22, 2017, 2:19 PM
 * 
 * An implementation of the levenshtein distance algorithm 
 * found on https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C.2B.2B
 */

#ifndef LEVENSHTEIN_DISTANCE_H
#define LEVENSHTEIN_DISTANCE_H

#include <numeric>
#include <algorithm>
#include <string>
//Need to fix so it ignores caps
int levenshtein_distance(const std::string &s1, const std::string &s2);



#endif /* LEVENSHTEIN_DISTANCE_H */

