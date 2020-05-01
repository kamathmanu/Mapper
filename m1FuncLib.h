/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m1FuncLib.h
 * Author: tatlahar
 *
 * Created on March 18, 2017, 6:55 PM
 */

#ifndef M1FUNCLIB_H
#define M1FUNCLIB_H
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <set>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/heap/fibonacci_heap.hpp>
#include <graphics_types.h>
#include <chrono>
#include "LatLon.h"
#include "dataTypes.h"
#include "streetData.h"
#include "Graph.h"
#include "pathfindingAlgorithm.h"

#define KMH_TO_MS 3.6
#define RAD_TO_DEG 1/DEG_TO_RAD
#define MAX_CLOSEST_CANDIDATES 25

/***********************************r-tree related namespace*******************/
using namespace std;
using Fibonacci_Queue = boost::heap::fibonacci_heap<VectorCost, boost::heap::compare<compareCosts>>;
using priorityQueue = std::priority_queue<VectorCost, vector<VectorCost>, compareCosts>;
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
/******************************************************************************/

/******************************r-tree set up*********************************/
//Points are doubles in 2D and represent geographical location in degrees
typedef bg::model::point<double, 2, bg::cs::cartesian > point; //latLon points in cartesian
typedef std::pair<point, unsigned> value; //Holds an intersection point in cartesian and its index
extern bgi::rtree<value, bgi::quadratic<16> > interRTree;
extern bgi::rtree<value, bgi::quadratic<16> > POI_RTree;
/*****************************************************************************/

/*Provide Average values and min/max such as latAverage, averageSpeed, etc*/
extern double latAverage;
extern float maxSpeed;
extern double xMax, yMax, xMin, yMin;
/****************************************************************************/

/*Provide total number of entities*******************************************/
extern unsigned totalNumIntersections;
extern unsigned totalNumStreets;
extern unsigned totalNumStreetSegs;
extern unsigned totalNumPOI;
extern unsigned totalNumFeatures;
/******************************************************************************/

/*****************************DATA STRUCTURES*********************************/

//Given street name get back a vector of street IDS
extern unordered_map <string, vector<unsigned>> streetToId;

//Given a street ID get back its street name as a string
extern vector<string> streetIDToName;

//Given a intersection ID get back a vector of the streetSegment ID's it connects
extern vector<vector<unsigned>>intersectionIdToStreetSeg;

//Given a street ID get back a vector of the streetSegment IDs on the street
extern unordered_map<unsigned, vector<unsigned>>streetIdToStreetSegId;

//Given a street segment ID get its corresponding street ID
extern vector<unsigned> streetSegIDToStreetID;

//Given a street segment ID get back a vector of its ordered LatLon points
extern vector<vector<LatLon>> streetSegmentPts;

//Given an intersection ID get back its LatLon position
extern vector<LatLon> intersectionPos;

//Multimap where the segment OSMID corresponds to a vector of its points
extern multimap<OSMID, id_pts > segmentOSMID_to_PT;

//Given a street segment ID get back its length in metres
extern vector<double> streetSegLength;

//Given a street segment ID get back its travel time in seconds
extern vector<double> streetSeg_travelTime;

//Given a street ID get back a vector of all the intersection IDs on that street
extern vector<vector<unsigned> > streetID_to_interIDs;

//Object that holds vectors for each category of roads
extern streetData *streetVectors;

//Given a street segment ID get back the associated intersection ID
extern vector<vector<unsigned>> streetSegIDWithIntersectionID;


//Colours used to construct the map
//Colours for each feature
//RGB colours obtained from: [place source]
extern t_color lake;
extern t_color greenspace;
extern t_color shoreline;
extern t_color river;
extern t_color stream;
extern t_color golfcourse;
extern t_color building;
extern t_color island;
extern t_color park;
extern t_color beach;
extern t_color background;
extern t_color unknown;

//Given a colour ID return the color as a t_color
extern unordered_map <unsigned, t_color> featureColorLibrary;

//Given a feature name return: open or closed feature
extern vector<featureInfo> features_open;
extern vector<featureInfo> features_closed;

//Given a interest name returns the interest
extern vector<interestInfo> interests;

//Each element is a vertex that has branching edges
extern vector<Vertex> adjacencyList;

//Given an intersection ID get back a vector of the intersection ids you can traverse to legally
//And the street segment id you would take
extern vector<vector<toInter_segID> > interID_toInter_and_SegIDS; 

extern vector<unsigned> deliverySegPath; 

void loadInterPos_and_MinMax();
void loadPOI_DATA();
void loadFeature_DATA();
void loadInterRtree();
void loadGraph_data();
void loadStreet_data();
void loadStreet_and_segData();
void loadStreetData();


extern string cityName;

//String ID to a pre computed delivery path
extern unordered_map<string, vector<unsigned> > preDeliveryPaths;

//Holds order start depot, pickups/dropoffs, end depot
extern vector<unsigned> orderPickUpDropOff; 


#endif /* M1FUNCLIB_H */
