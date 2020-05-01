/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m4Funlib.h
 * Author: tatlahar
 *
 * Created on March 31, 2017, 11:08 PM
 */

#ifndef M4FUNLIB_H
#define M4FUNLIB_H

#define MAPISO "/cad2/ece297s/public/maps/toronto_canada.streets.bin"
#define isolated 75020

#include "m1FuncLib.h"
#include "m3.h"
#include "m4.h"

using namespace std;


extern vector<unsigned> pickUpLocs;
extern vector<unsigned> dropOffLocs;
extern multimap<unsigned, unsigned> pickUp_to_deliveryIndex;

void preComputeDeliveryPaths(vector<unsigned> &locations, double turn_penalty);

vector<unsigned> two_opt(const float turn_penalty);

bool checkIfPathLegal(vector<unsigned> path);
#endif /* M4FUNLIB_H */

