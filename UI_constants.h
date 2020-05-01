/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UI_coords_definitions.h
 * Author: kamathma
 *
 * Created on March 15, 2017, 2:35 PM
 */

#ifndef UI_CONSTANTS_H
#define UI_CONSTANTS_H

//These are the constant definitions for the bounds for icons used in the UI input

/******************************SEARCH BAR***************************/

//ENTIRE BAR - Dimensions: Pixels

#define SEARCH_AREA_WIDTH_PIXELS 490
#define SEARCH_AREA_HEIGHT_PIXELS 47

// QUERY REGION - Dimensions: Pixels

#define QUERY_AREA_WIDTH_PIXELS 391
#define QUERY_AREA_HEIGHT_PIXELS 45

// SEARCH ICON - Dimensions: Pixels

#define SEARCH_ICON_WIDTH_PIXELS 49
#define SEARCH_ICON_HEIGHT_PIXELS 46

// DIRECTIONS ICON - Dimensions: Pixels

#define DIRECTIONS_ICON_WIDTH_PIXELS 53
#define DIRECTIONS_ICON_HEIGHT_PIXELS 46

//Mouse Click Return Values

enum{ OUTSIDE_SEARCH_BAR_BOUNDS = 0,
 SEARCH_QUERY = 1,
 SEARCH_HIT = 2,
 GET_DIRECTIONS = 3};

/****************************GET DIRECTIONS DIALOG BOX*************************/

//ENTIRE BOX - Dimensions: Pixels

#define DIRECTIONS_MENU_WIDTH_PIXELS 492
#define DIRECTIONS_MENU_HEIGHT_PIXELS 181

//Int-Int Button - Dimensions: Pixels

#define INT2INT_BUTTON_WIDTH_PIXELS 213 
#define INT2INT_BUTTON_HEIGHT_PIXELS 60

//Int-POI Button - Dimensions: Pixels

#define INT2POI_BUTTON_WIDTH_PIXELS 211 
#define INT2POI_BUTTON_HEIGHT_PIXELS 60

//clear space - Pixels:
 
#define CLEAR_BLUE_SPACE_HEIGHT 27 

//Reverse Icon - Dimensions: Pixels

#define REVERSE_PATHS_ICON_WIDTH_PIXELS 59
#define REVERSE_PATHS_ICON_HEIGHT_PIXELS 94

//Find Path Icon - Dimensions: Pixels

#define FIND_PATH_ICON_WIDTH_PIXELS 41
#define FIND_PATH_ICON_HEIGHT_PIXELS 47

//Quit Button - Dimensions: pixels

#define QUIT_DIRECTIONS_WIDTH_PIXELS 68
#define QUIT_DIRECTIONS_HEIGHT_PIXELS 60

//SOURCE QUERY REGION - Dimensions: Pixels

#define SOURCE_QUERY_AREA_WIDTH_PIXELS 328
#define SOURCE_QUERY_AREA_HEIGHT_PIXELS 47

//DESTINATION QUERY REGION - Dimensions: Pixels

#define DEST_QUERY_AREA_WIDTH_PIXELS 369
#define DEST_QUERY_AREA_HEIGHT_PIXELS 47
 
 //Mouse Click Return Values

enum{ OUTSIDE_DIRECTION_BOX_BOUNDS = 0,
 INT2INT = 1,
 INT2POI = 2,
 SOURCE_QUERY = 3,
 DEST_QUERY = 4,
 FIND_PATH = 5,
 FLIP_PATH = 6,
 QUIT_DIRECTIONS = 7};

/******************************HELP MENU***************************/

//ENTIRE Help Menu - Dimensions: Pixels

#define HELP_MENU_WIDTH_PIXELS 360
#define HELP_MENU_HEIGHT_PIXELS 514
 
#define QUIT_HELP_WIDTH_PIXELS 43
#define QUIT_HELP_HEIGHT_PIXELS 56 

/******************************************************************/
 
/******************************PATH FINDING OPTIONS***************************/

#define ERR_CODE -100
#define INVALID_INPUT -100
#define UNDEFINED_OPTION 0 
#define PATH_FROM_INTERSECTION_TO_INTERSECTION 1
#define PATH_FROM_INTERSECTION_TO_POI 2
 
/******************************************************************/

#endif /* UI_CONSTANTS_H */

