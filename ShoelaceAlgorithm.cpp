/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "ShoelaceAlgorithm.h"


double computeArea(vector<t_point> polyPoints) {
    double upGroup = 0;
    double downGroup = 0;
    
    for (unsigned i = 0; i < polyPoints.size() - 1; i++) {
        //Multiply set of points in a  "shoelace "fashion
        upGroup += polyPoints[i].x * polyPoints[i+1].y;
        downGroup += polyPoints[i].y * polyPoints[i+1].x; 
    }
    
    return (0.5 * abs(upGroup + downGroup)); 
}

bool compareArea(const featureInfo &feature1, const featureInfo &feature2) {
    //Compare areas; Used to sort vector of features from largest area to small
    return (abs(feature1.featureArea) < abs(feature2.featureArea));
}

