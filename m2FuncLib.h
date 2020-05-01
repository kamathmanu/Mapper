/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m2FuncLib.h
 * Author: tatlahar
 *
 * Created on February 24, 2017, 1:00 AM
 */

#ifndef M2FUNCLIB_H
#define M2FUNCLIB_H

#include "easygl_constants.h"
#include "m1.h"
#include "m1FuncLib.h"
#include "m2.h"
#include "graphics.h"
#include "OSMDatabaseAPI.h"
#include "dataTypes.h"
#include "UI_constants.h"
#include <X11/keysym.h>
#include <cstdlib>
#include <readline/readline.h>
#include "conversionFunctions.h"
#include <readline/history.h>
#include "levenshtein_distance.h"
#include <boost/heap/fibonacci_heap.hpp>



//#define ZOOM_STREETS_VIEW 1.2608e-05

// Initial area of the map upon drawing with no zoom

#define MED_THRESHOLD 14.00
#define HIGH_THRESHOLD 1.00
#define ARROW_THRESHOLD 0.03
#define STREET_ZOOM 0.01
#define SIZE4 4
#define SIZE3 3
#define SIZE2 2
#define SIZE1 1
#define MIN_NUM_PTS 2

#define MAX_WIDTH_PRIMARY 6
#define MAX_WIDTH_SECONDARY 3.3
#define MAX_WIDTH_TERTIARY 2
#define BOX_WIDTH_MULTIPLIER 1.5

#define BACKSPACE_ASCII '\b'
#define NULL_TERM_ASCII '\0'

double AREA_MAP;

//Road colours
t_color primaryRoad(254, 216, 157);
t_color secondaryRoad(192, 192, 192);//(0xFF,0xFF,0xFF);
t_color tertiaryRoad(211,211,211);


/*Library of functions for drawing a map*/
void drawStreetSegments(void); 
void draw_feature(void);
void find_element(void (*drawscreen_ptr)(void));
void draw_screen(void); 
void act_on_button_press(float xPos, float yPos, t_event_buttonPressed event);
void act_on_key_press (char key_pressed, int keysym);
void highlight_closest_intersection_on_mouse_click (LatLon &mousePosition);
void highlight_closest_interest_on_mouse_click (LatLon &mousePosition);
void draw_search_bar (void);
void draw_interest(void);
void draw_street_name(void);
double zoom_level(void);
void draw_path_endPoints(const vector<unsigned>& path);
void displayDirections(const vector<unsigned>& path);
void draw_one_way(void);
double turningDetermination(vector<t_point> vector1, vector<t_point> vector2) ;
int angle_cal(t_point first, t_point second);
void execute_search();
int check_search_area_clicked (t_point mouse_click_coordinate);
int check_directions_menu_clicked (t_point mouse_click_coordinate);
bool clicked_on_quit_help (t_point mouse_click_coordinate);
unsigned convert_input_Intersection_Name_to_ID (string intersection_name);
unsigned parse_input_intersection_point(string typed_input, LatLon clicked_coordinate);
string parse_input_POI_point(string typed_input, LatLon clicked_coordinate);
void draw_path_finding_box (void);
void display_help_menu(void);
void help_button_callback(void (*drawscreen_ptr)(void));
void reset_query (void);
string fuzzySearch(string &src);
bool fuzzyCompare(nameToDist &element1, nameToDist &element2);

#endif /* M2FUNCLIB_H */

