#include <math.h>
#include <cmath>
#include <utility>
#include <memory>
#include <iterator>
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include "m1.h"
#include "m1FuncLib.h"
#include "StreetsDatabaseAPI.h"
#include "graphics.h"
#include <graphics_types.h>
#include "conversionFunctions.h"
#include "ShoelaceAlgorithm.h"
#include "OSMDatabaseAPI.h"
#include "dataTypes.h"
#include <future>
#include <thread>

using namespace std;
/**Define the external global variables defined in m1FuncLib*/
bgi::rtree<value, bgi::quadratic<16> > interRTree;
bgi::rtree<value, bgi::quadratic<16> > POI_RTree;
double latAverage;
double xMax, yMax, xMin, yMin;
float maxSpeed = 0;
unsigned totalNumIntersections;
unsigned totalNumStreets;
unsigned totalNumStreetSegs;
unsigned totalNumPOI;
unsigned totalNumFeatures;
unordered_map <string, vector<unsigned>> streetToId;
vector<string> streetIDToName;
vector<vector<unsigned>>intersectionIdToStreetSeg;
unordered_map<unsigned, vector<unsigned>>streetIdToStreetSegId;
vector<unsigned> streetSegIDToStreetID;
vector<vector<LatLon>> streetSegmentPts;
vector<LatLon> intersectionPos;
multimap<OSMID, id_pts > segmentOSMID_to_PT;
vector<double> streetSegLength;
vector<double> streetSeg_travelTime;
vector<vector<unsigned> > streetID_to_interIDs;
streetData *streetVectors;
vector<vector<unsigned>> streetSegIDWithIntersectionID;
vector<Vertex> adjacencyList;
vector<vector<toInter_segID> > interID_toInter_and_SegIDS;
vector<unsigned> deliverySegPath;
string cityName;
unordered_map<string, vector<unsigned> > preDeliveryPaths;
vector<unsigned> orderPickUpDropOff; 
/*Declare Colours********************************************/
t_color lake = t_color(163, 205, 255);
t_color greenspace = t_color(201, 231, 164);
t_color shoreline = t_color(239, 236, 229);
t_color river = t_color(162, 205, 255);
t_color stream = t_color(163, 204, 253);
t_color golfcourse = t_color(203, 230, 159);
t_color building = t_color(221, 220, 216);
t_color island = t_color(235, 235, 235);
t_color park = t_color(205, 231, 170);
t_color beach = t_color(250, 241, 199);
t_color background = t_color(240, 237, 229);
t_color unknown = t_color(1, 1, 1);

unordered_map <unsigned, t_color> featureColorLibrary{
    {0, unknown},
    {1, park},
    {2, beach},
    {3, lake},
    {4, river},
    {5, island},
    {6, shoreline},
    {7, building},
    {8, greenspace},
    {9, golfcourse},
    {10, stream}
};
/******************************************************************/
vector<featureInfo> features_open;
vector<featureInfo> features_closed;
vector<interestInfo> interests;

/************************************************************/


void loadInterPos_and_MinMax() {
      //Initial values for min/max lat/lons
        double max_Lat = getIntersectionPosition(0).lat();
        double min_Lat = max_Lat;

        double max_Lon = getIntersectionPosition(0).lon();
        double min_Lon = max_Lon;
        //*********************************

        //Get position of each intersection and sum the latitudes
        for (unsigned i = 0; i < totalNumIntersections; i++) { //Can thread this 1
            LatLon position = getIntersectionPosition(i);

            max_Lat = max(max_Lat, position.lat());
            min_Lat = min(min_Lat, position.lat());

            max_Lon = max(max_Lon, position.lon());
            min_Lon = min(min_Lon, position.lon());

            intersectionPos.push_back(position);
        }

        latAverage = cos((0.5 * (max_Lat + min_Lat)) * DEG_TO_RAD);

        //Max x-y coordinates*******************
        xMin = min_Lon * DEG_TO_RAD * latAverage;
        yMin = min_Lat * DEG_TO_RAD;

        xMax = max_Lon * DEG_TO_RAD * latAverage;
        yMax = max_Lat * DEG_TO_RAD;
}

void loadPOI_DATA() {
        /*****************Set up POI r-tree****************************************/
        //Also store the POI name to pos POI_Name_to_Pos

        //Using packing algorithm 
        vector<value> POI_cartesPt_ID;

        //Pair intersection position and id then insert into rTree
        for (unsigned i = 0; i < totalNumPOI; i++) { //Can thread this 2

            interestInfo interest_i;

            interest_i.interestName = getPointOfInterestName(i);
            interest_i.interestType = getPointOfInterestType(i);
            interest_i.interestPosition = convertLatLonToCartesian(getPointOfInterestPosition(i), latAverage);
            interest_i.interestPositionLatLon = getPointOfInterestPosition(i);

            interests.push_back(interest_i);

            t_point coord = convertLatLonToCartesian(getPointOfInterestPosition(i), latAverage);
            POI_cartesPt_ID.push_back(value(point(coord.x, coord.y), i));
        }

        //Uses packing algorithm to load the POI r-tree
        POI_RTree.insert(POI_cartesPt_ID.begin(), POI_cartesPt_ID.end());
        /**************************************************************************/
    
}

void loadFeature_DATA() {
            /*Load data into vector for each feature*********************************/

        for (unsigned i = 0; i < totalNumFeatures; i++) {
            featureInfo feature_i;
            vector<LatLon> feature_i_points;

            feature_i.featureName = getFeatureName(i);
            feature_i.featureType = getFeatureType(i);
            auto iter = featureColorLibrary.find(feature_i.featureType);
            feature_i.featureColor = iter->second;
            feature_i.pointCount = getFeaturePointCount(i);

            for (unsigned j = 0; j < feature_i.pointCount; j++) {
                feature_i_points.push_back(getFeaturePoint(i, j));
            }

            //If there are more than 1 point check if feature is open/closed
            if (feature_i_points.size() > 1) {

                LatLon beginPt = feature_i_points.front();
                LatLon endPt = feature_i_points.back();

                if ((beginPt.lat() == endPt.lat()) && (beginPt.lon() == endPt.lon()))
                    feature_i.open = false;

                else {
                    feature_i.open = true;
                }
            }

            for (unsigned k = 0; k < feature_i_points.size(); k++) {
                LatLon position = feature_i_points[k];
                feature_i.featurePoints.push_back(convertLatLonToCartesian(position, latAverage));
            }

            

            if (feature_i.open) {
                features_open.push_back(feature_i);
            } else {
                feature_i.featureArea = computeArea(feature_i.featurePoints);
                features_closed.push_back(feature_i);
            }
        }

        //Sort features based on area
        sort(features_closed.begin(), features_closed.end(), compareArea);
    
}


void loadInterRtree() {
     /*****************Set up Intersection r-tree*******************************/
      
        //*******************************************
        //Vector of values for intersections
        vector<value> intersections_cartesian;

        //Pair intersection position and id then insert into rTree
        for (unsigned i = 0; i < totalNumIntersections; i++) {
            t_point coord = convertLatLonToCartesian(intersectionPos[i], latAverage);
            intersections_cartesian.push_back(value(point(coord.x, coord.y), i));
        }

        //Uses packing-algorithm to load r-tree
        interRTree.insert(intersections_cartesian.begin(), intersections_cartesian.end());
        /**************************************************************************/
    
}

void loadGraph_data() {
       //Create data structures related to intersections
        for (unsigned i = 0; i < totalNumIntersections; i++) { //Can thread this 3
            vector <unsigned> streetSeg; //vector for streetsegments
            vector<toInter_segID> inter_i;

            unsigned numOfSegs = getIntersectionStreetSegmentCount(i);

            for (unsigned j = 0; j < numOfSegs; j++) {
                StreetSegmentIndex segID = getIntersectionStreetSegment(i, j);
                StreetSegmentInfo segObj = getStreetSegmentInfo(segID);

                if (i != segObj.to)
                    inter_i.push_back(toInter_segID(segObj.to, segID));

                else if (!segObj.oneWay)
                    inter_i.push_back(toInter_segID(segObj.from, segID));

                streetSeg.push_back(segID);
            }

            intersectionIdToStreetSeg.push_back(streetSeg); //punch into intersectionIdToStreetSeg
            interID_toInter_and_SegIDS.push_back(inter_i);
        }
    
}

void loadStreet_and_segData() {
    for (unsigned c = 0; c < totalNumStreets; c++) {

            //Street name to id and vice versa
            string streetName_c = getStreetName(c);
            streetToId[streetName_c].push_back(c); // Push into vector
            streetIDToName.push_back(streetName_c);

            //Remove duplicates
            sort(streetIdToStreetSegId[c].begin(), streetIdToStreetSegId[c].end());
            streetIdToStreetSegId[c].erase(unique(streetIdToStreetSegId[c].begin(),
                    streetIdToStreetSegId[c].end()), streetIdToStreetSegId[c].end());

            std::vector<unsigned> intersectionsOnStreet;

            //Get intersections on street including duplicates
            for (unsigned i = 0; i < streetIdToStreetSegId[c].size(); i++) {
                StreetSegmentInfo streetSegment = getStreetSegmentInfo(streetIdToStreetSegId[c][i]);
                intersectionsOnStreet.push_back(streetSegment.from);
                intersectionsOnStreet.push_back(streetSegment.to);
            }

            sort(intersectionsOnStreet.begin(), intersectionsOnStreet.end());
            intersectionsOnStreet.erase(unique(intersectionsOnStreet.begin(),
                    intersectionsOnStreet.end()), intersectionsOnStreet.end());

            streetID_to_interIDs.push_back(intersectionsOnStreet);

        }
}

void loadStreetData() {
        //Load road vectors
        //Load streetVectors
        streetVectors = new streetData();
        streetVectors->loadStreetVectors(segmentOSMID_to_PT);
}

void loadStreet_data() {
     vector<vector < t_point>> streetSegmentPts_cart;

        for (unsigned i = 0; i < totalNumStreetSegs; i++) {
            StreetSegmentInfo streetSeg = getStreetSegmentInfo(i);
            maxSpeed = max(maxSpeed, streetSeg.speedLimit);
            streetSegIDToStreetID.push_back(streetSeg.streetID);
            streetIdToStreetSegId[streetSeg.streetID].push_back(i);

            /*************Street Segment pts, length, travel***********************/
            vector<LatLon> points;
            vector<t_point> cartestianPts;

            points.push_back(getIntersectionPosition(streetSeg.from));
            cartestianPts.push_back(convertLatLonToCartesian(points.front(), latAverage));

            for (unsigned j = 0; j < streetSeg.curvePointCount; j++) {

                points.push_back(getStreetSegmentCurvePoint(i, j));
                cartestianPts.push_back(convertLatLonToCartesian(points[j + 1], latAverage));
            }

            points.push_back(getIntersectionPosition(streetSeg.to));
            cartestianPts.push_back(convertLatLonToCartesian(points.back(), latAverage));


            streetSegmentPts.push_back(points);
            streetSegmentPts_cart.push_back(cartestianPts);

            vector<LatLon> segPoints;
            segPoints.insert(segPoints.end(), streetSegmentPts[i].begin(), streetSegmentPts[i].end());

            //Unordered map
            StreetSegmentInfo seg_info = getStreetSegmentInfo(i);
            segmentOSMID_to_PT.insert(make_pair(seg_info.wayOSMID, make_pair(i, streetSegmentPts_cart[i])));


            /*************Street Segment IDs containing intersection from and to ***********************/
            double totalLength = 0;

            for (unsigned m = 0; m < segPoints.size() - 1; m++) {
                totalLength += find_distance_between_two_points(segPoints[m], segPoints[m + 1]);
            }

            streetSegLength.push_back(totalLength);
            streetSeg_travelTime.push_back(totalLength / getStreetSegmentInfo(i).speedLimit * KMH_TO_MS);
        }

       
    
}


bool load_map(std::string map_name) {
    //Load map and map related data structures
    cityName = map_name;

    //Load street database and osm database 
    bool success = loadStreetsDatabaseBIN(map_name);
    //We need to replace "streets" which is 7 chars in map_name with "osm" 
    map_name.replace(map_name.find("streets"), 7, "osm");
    success = loadOSMDatabaseBIN(map_name);

    if (success) { //Create data structures if map successfully loaded.

        //Get max values for parameters
        totalNumIntersections = getNumberOfIntersections();
        totalNumStreets = getNumberOfStreets();
        totalNumStreetSegs = getNumberOfStreetSegments();
        totalNumPOI = getNumberOfPointsOfInterest();
        totalNumFeatures = getNumberOfFeatures();
        
        thread t1(loadInterPos_and_MinMax);
        thread t2(loadPOI_DATA);
        thread t3(loadFeature_DATA);
        t1.join();
        t2.join();
        t3.join();
        
        thread t4(loadInterRtree);
        thread t5(loadGraph_data);
        thread t6(loadStreet_data);
        t4.join();
        t5.join();
        t6.join();
        
        thread t7(loadStreet_and_segData);
        thread t8(loadStreetData);
        t7.join();
        t8.join();

        //Create adjacency list
        for (unsigned i = 0; i < totalNumIntersections; i++) {
            Vertex vert(i); //Create vertex with intersection ID initialized 
            vector<Edge> edges_;
            for (unsigned j = 0; j < interID_toInter_and_SegIDS[i].size(); j++) {
                edges_.push_back(Edge(interID_toInter_and_SegIDS[i][j].first, interID_toInter_and_SegIDS[i][j].second));
            }

            vert.edges = edges_;
            adjacencyList.push_back(vert);
        }

    } else {
        return false;
    }

    return success;
}

void close_map() {
    //Clean-up your map related data structures here
    delete streetVectors;
    streetToId.clear();
    streetIDToName.clear();
    intersectionIdToStreetSeg.clear();
    streetIdToStreetSegId.clear();
    streetSegIDToStreetID.clear();
    streetSegmentPts.clear();
    intersectionPos.clear();
    segmentOSMID_to_PT.clear();
    streetSegLength.clear();
    streetSeg_travelTime.clear();
    streetID_to_interIDs.clear();
    streetSegIDWithIntersectionID.clear();
    features_open.clear();
    features_closed.clear();
    interests.clear();
    adjacencyList.clear();
    interID_toInter_and_SegIDS.clear();
 
    closeStreetDatabase();
    closeOSMDatabase();
}

//Returns street id(s) for the given street name

std::vector<unsigned> find_street_ids_from_name(std::string street_name) {
    return streetToId[street_name]; // return the vector
}

std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id) {
    //return vectors of street segments
    return intersectionIdToStreetSeg [intersection_id];

}

std::vector<std::string> find_intersection_street_names(unsigned intersection_id) {
    /*Return a vector of the street names associated with the given intersection*/

    std::vector<std::string> street_names;

    std::vector<unsigned> street_seg_IDs;

    //get the vector of street segment IDs
    street_seg_IDs = find_intersection_street_segments(intersection_id);

    for (unsigned i = 0; i < street_seg_IDs.size(); i++) {

        street_names.push_back(getStreetName((getStreetSegmentInfo(street_seg_IDs[i])).streetID));
    }
    return (street_names);
}

bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2) {
    //Returns true if you can get from intersection1 to intersection2 using a single street segment (hint: check for 1-way streets too)
    //corner case: an intersection is considered to be connected to itself

    //If intersections are identical then return true.
    if (intersection_id1 == intersection_id2) return true;

    vector<unsigned> commonStreetSegs;

    //Get streetSeg ids for both intersections
    vector<unsigned> streetSegs1 = find_intersection_street_segments(intersection_id1);
    vector<unsigned> streetSegs2 = find_intersection_street_segments(intersection_id2);

    //Get common street segment ids from both
    std::set_intersection(streetSegs1.begin(), streetSegs1.end(),
            streetSegs2.begin(), streetSegs2.end(), std::back_inserter(commonStreetSegs));

    for (unsigned i = 0; i < commonStreetSegs.size(); i++) {
        StreetSegmentInfo streetSeg = getStreetSegmentInfo(commonStreetSegs[i]);

        if ((intersection_id1 == streetSeg.to) && (streetSeg.from == intersection_id2) && (streetSeg.oneWay == true))
            return false;

        else
            return true;
    }

    return false;
}

std::vector<unsigned> find_street_street_segments(unsigned street_id) {

    return streetIdToStreetSegId[street_id];
}

double find_distance_between_two_points(LatLon point1, LatLon point2) {
    //Converts (latitude, longitude) -> (x, y) then finds distance between pts
    //via Pythagoras' theorem on an equirectangular projection.

    double latAvg = cos(((point1.lat() + point2.lat())*0.5) * DEG_TO_RAD);

    double x1 = point1.lon();
    double y1 = point1.lat();

    double x2 = point2.lon();
    double y2 = point2.lat();

    double sumSquares = DEG_TO_RAD * DEG_TO_RAD * ((y2 - y1)*(y2 - y1) + (x2 - x1)*(x2 - x1) * latAvg * latAvg);

    return EARTH_RADIUS_IN_METERS * sqrt(sumSquares);
}

double find_street_segment_length(unsigned street_segment_id) {
    return streetSegLength[street_segment_id];
}

double find_street_segment_travel_time(unsigned street_segment_id) {
    //Computes the time in seconds needed to traverse street segment 
    //time = distance/speed_limit
    return streetSeg_travelTime[street_segment_id];
}

double find_street_length(unsigned street_id) {

    double streetLength = 0.0;

    vector <unsigned> street_seg_id = streetIdToStreetSegId[street_id];
    //extract street segment ids from vector of vectors streetIdToStreetSegId
    for (auto i = street_seg_id.begin(); i < street_seg_id.end(); i++) {
        //calculate the street length

        streetLength += find_street_segment_length(*i);
    }

    return streetLength;
}

std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id) {
    //Returns all intersections reachable by traveling down exactly one street
    //segment from given node.

    std::vector<unsigned> adjacentIntersections;

    //Get street segment ID's of segments connected to intersection
    std::vector<unsigned> streetSegIDs = find_intersection_street_segments(intersection_id);
    std::vector<unsigned>::iterator iter = streetSegIDs.begin();

    while (iter != streetSegIDs.end()) {
        StreetSegmentInfo streetSegment = getStreetSegmentInfo(*iter);

        // if you can go down the street segment from your intersection ID - this means it's either a 2 way
        // or a favorable one way

        if ((intersection_id == streetSegment.from) && (intersection_id != streetSegment.to)) {
            if (std::find(adjacentIntersections.begin(), adjacentIntersections.end(), streetSegment.to) == adjacentIntersections.end()) //avoid duplicates??
                adjacentIntersections.push_back(streetSegment.to);
        } else {
            if (!streetSegment.oneWay) {

                if (std::find(adjacentIntersections.begin(), adjacentIntersections.end(), streetSegment.from) == adjacentIntersections.end())
                    adjacentIntersections.push_back(streetSegment.from);
            }
        }

        iter++;
    }

    return adjacentIntersections;
}

std::vector<unsigned> find_all_street_intersections(unsigned street_id) {
    return streetID_to_interIDs[street_id];
}

std::vector < unsigned > find_intersection_ids_from_street_names(std::string
street_name1, std::string street_name2) {

    //streetID_to_interIDs
    vector<unsigned> streetID1 = streetToId[street_name1];
    vector<unsigned> streetID2 = streetToId[street_name2];

    vector<unsigned> intersectionIDs1;
    vector<unsigned> intersectionIDs2;

    vector<unsigned> streetIntersections;

    for (unsigned i = 0; i < streetID1.size(); i++) {
        vector<unsigned> temp_vec = streetID_to_interIDs[streetID1[i]];
        intersectionIDs1.insert(intersectionIDs1.end(), temp_vec.begin(), temp_vec.end());
    }

    for (unsigned j = 0; j < streetID2.size(); j++) {
        vector<unsigned> temp_vec = streetID_to_interIDs[streetID2[j]];
        intersectionIDs2.insert(intersectionIDs2.end(), temp_vec.begin(), temp_vec.end());
    }

    sort(intersectionIDs1.begin(), intersectionIDs1.end());
    sort(intersectionIDs2.begin(), intersectionIDs2.end());

    std::set_intersection(intersectionIDs1.begin(), intersectionIDs1.end(),
            intersectionIDs2.begin(), intersectionIDs2.end(),
            std::back_inserter(streetIntersections));

    streetIntersections.erase(unique(streetIntersections.begin(),
            streetIntersections.end()), streetIntersections.end());


    return streetIntersections;
}

unsigned find_closest_intersection(LatLon my_position) {
    std::vector<value> nearestIntersection;

    //Find 1 nearest point
    interRTree.query(bgi::nearest(point(my_position.lon() * DEG_TO_RAD * latAverage,
            my_position.lat() * DEG_TO_RAD), MAX_CLOSEST_CANDIDATES), std::back_inserter(nearestIntersection));


    unsigned closest = 0;
    double minDis = EARTH_RADIUS_IN_METERS;
    double temp = 0;

    for (unsigned j = 0; j < nearestIntersection.size(); j++) {
        LatLon pos = getIntersectionPosition(nearestIntersection[j].second);
        temp = find_distance_between_two_points(my_position, pos);

        if (temp < minDis) {
            minDis = temp;
            closest = nearestIntersection[j].second;
        }
    }

    return closest;
}

unsigned find_closest_point_of_interest(LatLon my_position) {
    std::vector<value> nearestPOI;

    //Find 1 nearest point
    POI_RTree.query(bgi::nearest(point(my_position.lon() * DEG_TO_RAD * latAverage,
            my_position.lat() * DEG_TO_RAD), MAX_CLOSEST_CANDIDATES), std::back_inserter(nearestPOI));

    unsigned closest = 0;
    double minDis = EARTH_RADIUS_IN_METERS;
    double temp = 0;

    for (unsigned j = 0; j < nearestPOI.size(); j++) {
        LatLon pos = getPointOfInterestPosition(nearestPOI[j].second);
        temp = find_distance_between_two_points(my_position, pos);

        if (temp < minDis) {
            minDis = temp;
            closest = nearestPOI[j].second;
        }
    }

    return closest;
}
