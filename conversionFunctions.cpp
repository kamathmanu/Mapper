/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "conversionFunctions.h"


/***CONVERSION FUNCTIONS: used to convert LatLon to Cartesian and vice-versa***/

t_point convertLatLonToCartesian (LatLon pointToBeConverted, double latAverage){
    //using the formula (x,y) = (lon*cos(average lat.), lat) 
    t_point convertedPoint;
    convertedPoint.x = (pointToBeConverted.lon()*DEG_TO_RAD)*latAverage; 
    convertedPoint.y = pointToBeConverted.lat()*DEG_TO_RAD;
    return (convertedPoint);
}

LatLon convertCartesianToLatLon (t_point pointToBeConverted, double latAverage){
    //using the inverse of the formula used in convertLatLonToCartesian
    
    LatLon convertedPoint(pointToBeConverted.y * RAD_TO_DEG, pointToBeConverted.x 
                * RAD_TO_DEG/latAverage);
    
    return (convertedPoint);
}
/******************************************************************************/