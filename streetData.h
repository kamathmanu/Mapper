/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   streetData.h
 * Author: tatlahar
 *
 * Created on February 24, 2017, 7:00 PM
 */

#pragma once 
#include <vector>
#include <unordered_map>
#include <map>
#include "StreetsDatabaseAPI.h"
#include "graphics_types.h"
#include "OSMDatabaseAPI.h"
#include "dataTypes.h"
#include <iostream>

using namespace std;

/*This class is used to store the OSMIDs of various road types.
 These IDs will be used later to classify a street segment as either belonging
 to a major road or minor*/




class streetData {
private:
    //Stores the OSM ID for each road
    unordered_map <string, vector<OSMID> > streetOSMs;
    
    //Vectors that hold OSMids corresponding to major/minor roads
    vector<OSMID> primary_OSMS;
    vector<OSMID> secondary_OSMS;
    vector<OSMID> tertiary_OSMS;
    
    //Vectors containing the coordinates for each primary, secondary, tertiary street
    vector<vector<t_point>> primaryRoads;
    vector<vector<t_point>> secondaryRoads;
    vector<vector<t_point>> tertiaryRoads;
    
    //Vectors containing the street id and beginning and end point of each primary, secondary, and tertiary street
    vector<segIDToEndPts> primaryName_Parameters;
    vector<segIDToEndPts> secondaryName_Parameters; //
    vector<segIDToEndPts> tertiaryName_Parameters;
    
    
public:
    streetData();
    void loadStreetVectors(multimap<OSMID, id_pts > &segment_ID_to_pts);
    vector<vector<t_point>>  getPrimaryRoads(void);
    vector<vector<t_point>>  getSecondaryRoads(void);
    vector<vector<t_point>>  getTertiaryRoads(void);
    
    vector<segIDToEndPts> get_primaryRoadParam(void);
    vector<segIDToEndPts> get_secondaryRoadParam(void);
    vector<segIDToEndPts> get_tertiaryRoadParam(void);
};



