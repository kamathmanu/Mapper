/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Graph.h
 * Author: tatlahar
 *
 * Created on March 19, 2017, 1:45 AM
 * Code adopted from http://www.redblobgames.com/pathfinding/a-star/implementation.html#python-breadth-first
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <list>
#include <cstddef>

using namespace std;

#define INFINITE INT_MAX
#define UNDEFINED_EDGE -1
#define SOURCE_NO_EDGE -2

struct Edge {
    unsigned segID; //Segment id
    unsigned nextVertex; //Destination vertex after traversing street segment
    
    Edge(unsigned VERT, unsigned SEG) {
        segID = SEG;
        nextVertex =  VERT;
    }
};

struct Vertex {
    int reachingEdgeID = UNDEFINED_EDGE;
    float cost = INFINITE;
    bool visited = false; 
    unsigned vertexID; //ID of the vertex
    vector<Edge> edges; //Vector of edges
    Vertex(unsigned ID_): vertexID(ID_) {}
};

typedef pair<unsigned, float> VectorCost;

struct compareCosts {
    //Greater than comparison used to achieve min priority queue
    bool operator() (const VectorCost& elem1, const VectorCost& elem2) const {
        return (elem1.second > elem2.second);
    }
};



#endif /* GRAPH_H */

