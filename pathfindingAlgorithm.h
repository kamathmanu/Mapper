/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   pathfindingAlgorithm.h
 * Author: tatlahar
 *
 * Created on March 29, 2017, 8:58 PM
 * 
 * 
 * We use a version of the two-opt algorithm available on 
 * http://www.technical-recipes.com/2012/applying-c-implementations-of-2-opt-to-travelling-salesman-problems/
 */

#ifndef PATHFINDINGALGORITHM_H
#define PATHFINDINGALGORITHM_H
#include "m1FuncLib.h"
#include "m1.h"
#include <omp.h>

using namespace std; 

std::vector<unsigned> aStar(const unsigned source, const unsigned target, const double turn_penalty);
std::vector<unsigned> dijkstra_POI(const unsigned source, const std::string point_of_interest_name, const double turn_penalty);
std::vector<unsigned> dijkstra_M4(const unsigned source, const vector<unsigned>& target, const double turn_penalty);
void dijkstra_prePaths(const unsigned source, const vector<unsigned> locations, vector<Vertex> adjList, const double turn_penalty);
std::vector<unsigned> traceBackPath(unsigned targetID, unsigned sourceID);
std::vector<unsigned> traceBackPathPrePath(unsigned targetID, vector<Vertex> &adjList);
#endif /* PATHFINDINGALGORITHM_H */

