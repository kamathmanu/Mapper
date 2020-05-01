/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   conversionFunctions.h
 * Author: tatlahar
 *
 * Created on February 24, 2017, 1:04 AM
 */

#pragma once 
#include "LatLon.h"
#include <graphics_types.h>

#define DEG_TO_RAD  0.017453292519943295769236907684886
#define RAD_TO_DEG 1/DEG_TO_RAD

t_point convertLatLonToCartesian (LatLon pointToBeConverted, double latAverage);
LatLon convertCartesianToLatLon (t_point pointToBeConverted, double latAverage);



