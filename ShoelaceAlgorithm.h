/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ShoelaceAlgorithm.h
 * Author: tatlahar
 *
 * Created on February 24, 2017, 2:41 AM
 */

#pragma once 
#include <graphics_types.h>
#include <vector>
#include <cmath>
#include "dataTypes.h"

using namespace std;

//Computes the area of a simple polygon using shoelace algorithm
//https://en.wikipedia.org/wiki/Shoelace_formula

double computeArea(vector<t_point> polyPoints);

bool compareArea(const featureInfo &feature1, const featureInfo &feature2);



