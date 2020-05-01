/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   dataTypes.h
 * Author: tatlahar
 *
 * Created on March 18, 2017, 7:09 PM
 */

#ifndef DATATYPES_H
#define DATATYPES_H

#include <vector>
#include <string>
#include "StreetsDatabaseAPI.h"


using namespace std;



//Data types
typedef pair<unsigned, vector<t_point>> id_pts;
typedef pair<unsigned, vector<t_point>> segIDToEndPts;
typedef pair<unsigned, unsigned> toInter_segID; //intersectionID, street segment ID
typedef pair<string, int> nameToDist;



//Struct definitions
struct featureInfo {
    string featureName;
    FeatureType featureType;
    t_color featureColor;
    unsigned pointCount;
    vector<t_point> featurePoints;
    double featureArea;
    bool open;
};

struct interestInfo {
    string interestName;
    string interestType;
    t_point interestPosition;
    LatLon interestPositionLatLon;
};




#endif /* DATATYPES_H */

