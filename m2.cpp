/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m2FuncLib.h"
#include "m3FuncLib.h"



using namespace std;
double turn_penalty = 0;
//vector<unsigned> shortest_path;

/*******************************User Interaction States************************/

/*Buttons clicked*/

bool clicked_on_help_button = false; // this also serves as a flag for quitting the menu later on

/*Mouse click states*/

bool has_clicked = false; // check if the user has clicked or not


// Specific mouse clicks

bool search_query = false;

bool source_query = false;

bool dest_query = false;

bool clicked_get_directions = false; //check if user is searching for path

bool quit_directions = false; //check 

LatLon source_coordinate, dest_coordinate;

int path_option = UNDEFINED_OPTION;

bool clicked_source_point = false;

bool clicked_dest_point = false;

int count_get_direction_click = 0;

int num_of_clicks_directions = 0;

//bool routeDisplayScale = 1;

bool display_direction = 0;

//Keyboard input

std::string user_input = ""; //empty string
std::string source_path = "";
std::string dest_path = "";

/******************************************************************************/

/**************************Zoom-Level Function*********************************/

//#define DEBUG_ZOOM_LEVEL

/*Takes the ratio of the current area of the user view over the
     area of the entire map. Multiplies by 100 to normalize value
     This returned result is an indication of zoom level*/

double zoom_level(void) {

    return (get_visible_world().area() / AREA_MAP * 100);
}
/******************************************************************************/

/********************Street-Related Graphics Functions*************************/

void draw_street_name(void) {
    //Draws street names for the given map

    //Get a vector of the street segment ID's and corresponding street segment points
    vector<segIDToEndPts> primaryParameters = streetVectors->get_primaryRoadParam();
    vector<segIDToEndPts> secondaryParameters = streetVectors->get_secondaryRoadParam();
    vector<segIDToEndPts> tertiaryParameters = streetVectors->get_tertiaryRoadParam();

    /*Primary Streets - Go through each street segment and plot the street name
     at the midpoint of the street segment*/
    for (unsigned i = 0; i < primaryParameters.size(); i++) {

        segIDToEndPts streetElement = primaryParameters[i]; //Vector of pts

        //Get the street name
        string streetName = streetIDToName[streetSegIDToStreetID[streetElement.first]];

        int numPts = streetElement.second.size(); //Ge the number of points for the street segment

        //Check if there are at least 2 points
        if (numPts >= MIN_NUM_PTS) {

            if (streetName != "<unknown>") { //Don't print unknown streets

                int midIndex = (streetElement.second.size() - 1) / 2;

                //Determine the midpoint and get the next point
                t_point midPoint = streetElement.second[midIndex];
                t_point nextPoint = streetElement.second[midIndex + 1];

                int angle = atan((nextPoint.y - midPoint.y) / (nextPoint.x - midPoint.x)) * 1 / (DEG_TO_RAD);

                settextrotation(angle);
                setfontsize(8);
                setcolor(BLACK);

                t_bound_box nameBox = t_bound_box(midPoint, nextPoint);

                //Check if street segment curves
                if (numPts > MIN_NUM_PTS) {
                    drawtext(nameBox.get_center(), streetName, nameBox.get_width() * BOX_WIDTH_MULTIPLIER, nameBox.get_height());
                }
                else {
                    drawtext(nameBox.get_center(), streetName, nameBox.get_width(), nameBox.get_height());
                }

            }
        }
    }


    if (zoom_level() < MED_THRESHOLD) {
        // if (numPts >= MIN_NUM_PTS) {
        for (unsigned j = 0; j < secondaryParameters.size(); j++) {

            segIDToEndPts streetElement = secondaryParameters[j];
            //Get the street name
            string streetName = streetIDToName[streetSegIDToStreetID[streetElement.first]];

            int numPts = streetElement.second.size();

            if (numPts >= MIN_NUM_PTS) {
                if (streetName != "<unknown>") {

                    int midIndex = (streetElement.second.size() - 1) / 2;
                    t_point midPoint = streetElement.second[midIndex];
                    t_point nextPoint = streetElement.second[midIndex + 1];


                    int angle = atan((nextPoint.y - midPoint.y) / (nextPoint.x - midPoint.x)) * 1 / (DEG_TO_RAD);

                    settextrotation(angle);
                    setfontsize(8);
                    setcolor(BLACK);

                    t_bound_box nameBox = t_bound_box(midPoint, nextPoint);

                    //Check if street segment curves if so we only want to print the street name once with no bounded box
                    if (numPts > MIN_NUM_PTS) {
                        drawtext(nameBox.get_center(), streetName, nameBox.get_width() * BOX_WIDTH_MULTIPLIER, nameBox.get_height()); //abs(endPt.x - midPoint.x), abs(endPt.y - midPoint.y)); //FLT_MAX, FLT_MAX);
                    } else {
                        drawtext(nameBox.get_center(), streetName, nameBox.get_width(), nameBox.get_height());
                    }
                }
            }
        }

    }


    if (zoom_level() < HIGH_THRESHOLD) {

        for (unsigned k = 0; k < tertiaryParameters.size(); k++) {

            segIDToEndPts streetElement = tertiaryParameters[k];
            //Get the street name
            string streetName = streetIDToName[streetSegIDToStreetID[streetElement.first]];
            int numPts = streetElement.second.size();

            if (numPts >= MIN_NUM_PTS) {

                if (streetName != "<unknown>") {

                    int midIndex = (numPts - 1) / 2;
                    t_point midPoint = streetElement.second[midIndex];
                    t_point nextPoint = streetElement.second[midIndex + 1];

                    int angle = atan((nextPoint.y - midPoint.y) / (nextPoint.x - midPoint.x)) * 1 / (DEG_TO_RAD);

                    settextrotation(angle);
                    setfontsize(8);
                    setcolor(BLACK);

                    t_bound_box nameBox = t_bound_box(midPoint, nextPoint);

                    //Check if street segment curves if so we only want to print the street name once with no bounded box
                    if (numPts > 2) {
                        drawtext(nameBox.get_center(), streetName, nameBox.get_width() * BOX_WIDTH_MULTIPLIER, nameBox.get_height());
                    } else {
                        drawtext(nameBox.get_center(), streetName, nameBox.get_width(), nameBox.get_height());

                    }
                }
            }
        }
    }
}

void drawStreetSegments(void) {
    //Draws the street segments for the given map

    //Vectors holding the cartesian coordinates for each street
    vector<vector < t_point>> primaryStreets = streetVectors->getPrimaryRoads();
    vector<vector < t_point>> secondaryStreets = streetVectors->getSecondaryRoads();
    vector<vector < t_point>> tertiaryStreets = streetVectors->getTertiaryRoads();

    double widthMultiplier = abs((zoom_level() / 100) - 1); //Normalized value; When zoom is 1 we set to 0.

    //Draw primary roads
    for (unsigned i = 0; i < primaryStreets.size(); i++) {
        for (unsigned j = 0; j < primaryStreets[i].size() - 1; j++) {

            //Draw road
            setcolor(primaryRoad);
            setlinewidth(SIZE4 + MAX_WIDTH_PRIMARY * widthMultiplier);
            setlinestyle(SOLID, ROUND);
            drawline(primaryStreets[i][j], primaryStreets[i][j + 1]);


        }
    }

    //Draw secondary roads
    if (zoom_level() < MED_THRESHOLD) {
        for (unsigned i = 0; i < secondaryStreets.size(); i++) {
            for (unsigned j = 0; j < secondaryStreets[i].size() - 1; j++) {
                setcolor(secondaryRoad);
                setlinewidth(SIZE3 + MAX_WIDTH_SECONDARY * widthMultiplier);
                setlinestyle(SOLID, ROUND);
                drawline(secondaryStreets[i][j], secondaryStreets[i][j + 1]);
            }
        }
    }

    //Draw tertiary roads
    if (zoom_level() < HIGH_THRESHOLD) {
        for (unsigned i = 0; i < tertiaryStreets.size(); i++) {
            for (unsigned j = 0; j < tertiaryStreets[i].size() - 1; j++) {
                setcolor(tertiaryRoad);
                setlinewidth(SIZE2 + MAX_WIDTH_TERTIARY * widthMultiplier);
                setlinestyle(SOLID, ROUND);
                drawline(tertiaryStreets[i][j], tertiaryStreets[i][j + 1]);
            }
        }
    }
}

void displayDirections(const vector<unsigned>& path) {

    unsigned size = path.size();
    if (size == 1) {
        //if only one street segment id
        unsigned straightDistance = 0;

        cout << "-----------------------------------------------------------" << endl;
        cout << "                  Direction Instruction                    " << endl;
        cout << "-----------------------------------------------------------" << endl;
        //cout << "Estimated Time of Arrival: " << compute_path_travel_time(path, turn_penalty) << " minutes" << endl;
        cout << "-----------------------------------------------------------" << endl;

        straightDistance += find_street_segment_length(path[0]);
        cout << " " << std::endl;
        cout << "Keep driving straight for " << straightDistance << " meters, then you'll arrive at the destination" << endl;
        cout << "-----------------------------------------------------------" << endl;
        cout << "-----------------------------------------------------------" << endl;

    } else if (size > 1) {
        //for multiple street segment ids
        unsigned maxIndex = size - 1;
        unsigned straightDistance = 0;

        unsigned startingIntersection;
        unsigned endingIntersection;
        //determine the start intersection
        if (getStreetSegmentInfo(path[0]).from == getStreetSegmentInfo(path[1]).from || getStreetSegmentInfo(path[0]).from == getStreetSegmentInfo(path[1]).to)
            startingIntersection = getStreetSegmentInfo(path[0]).to;
        else
            startingIntersection = getStreetSegmentInfo(path[0]).from;
        //determine the end intersection
        if (getStreetSegmentInfo(path[size - 1]).to == getStreetSegmentInfo(path[size - 2]).from || getStreetSegmentInfo(path[size - 1]).to == getStreetSegmentInfo(path[size - 2]).to)
            endingIntersection = getStreetSegmentInfo(path[size - 1]).from;
        else
            endingIntersection = getStreetSegmentInfo(path[size - 1]).to;

        cout << "-----------------------------------------------------------" << endl;
        cout << "                        Direction                          " << endl;
        cout << "-----------------------------------------------------------" << endl;
        cout << "Starting Point: " << getIntersectionName(startingIntersection) << endl;
        cout << "Destination: " << getIntersectionName(endingIntersection) << endl;
        //cout << "Estimated Time of Arrival: " << compute_path_travel_time(path,turn_penalty) << " minutes" << endl;
        cout << "-----------------------------------------------------------" << endl;

        for (unsigned i = 0; i < size; i++) {
            if (i != maxIndex) {
                if (getStreetSegmentInfo(path[i]).streetID == getStreetSegmentInfo(path[i + 1]).streetID) {
                    straightDistance += find_street_segment_length(path[i]);

                } else {

                    vector<t_point> from;
                    vector<t_point> to;

                    t_point position1 = convertLatLonToCartesian(getIntersectionPosition(getStreetSegmentInfo(path[i]).to), latAverage);

                    t_point position2 = convertLatLonToCartesian(getIntersectionPosition(getStreetSegmentInfo(path[i]).from), latAverage);

                    t_point position3 = convertLatLonToCartesian(getIntersectionPosition(getStreetSegmentInfo(path[i + 1]).to), latAverage);

                    t_point position4 = convertLatLonToCartesian(getIntersectionPosition(getStreetSegmentInfo(path[i + 1]).from), latAverage);

                    t_point push1; //difference between from position
                    push1.x = position1.x - position2.x;

                    push1.y = position1.y - position2.y;

                    t_point push2; //difference between to position
                    push2.x = position3.x - position4.x;

                    push2.y = position3.y - position4.y;

                    from.push_back(push1);
                    to.push_back(push2);

                    double slope1 = from[0].y / from[0].x;
                    double slope2 = to[0].y / to[0].x;

                    double comparison = fabs(atan((slope2 - slope1) / (1 + slope1 * slope2))); //calculate the angle of two position in radian

                    if (comparison <= 0.0872665) {//5 degree in radian    
                        straightDistance += find_street_segment_length(path[i]);
                        cout << "Keep driving straight for " << straightDistance << " meters, " << "drive into " << getStreetName(getStreetSegmentInfo(path[i + 1]).streetID) << "." << endl;
                        cout << "-----------------------------------------------------------" << endl;
                        straightDistance = 0;
                    } else {
                        double check = turningDetermination(from, to);
                        straightDistance += find_street_segment_length(path[i]);

                        if (check > 0) {//if crossproduct is positive, turn left
                            cout << "Keep driving straight for " << straightDistance << " meters, " << "turn left into " << getStreetName(getStreetSegmentInfo(path[i + 1]).streetID) << "." << endl;
                            cout << "-----------------------------------------------------------" << endl;
                        } else {//if crossproduct is negative, turn right
                            cout << "Keep driving straight for " << straightDistance << " meters, " << "turn right into " << getStreetName(getStreetSegmentInfo(path[i + 1]).streetID) << "." << endl;
                            cout << "-----------------------------------------------------------" << endl;
                        }
                        straightDistance = 0;
                    }
                    //clear the vector
                    from.clear();
                    to.clear();

                }
            } else {
                //for the last index in vector
                straightDistance += find_street_segment_length(path[i]);
                cout << "Keep driving straight for " << straightDistance << " meters, then you'll arrive at the destination" << endl;
                cout << "-----------------------------------------------------------" << endl;
                cout << "-----------------------------------------------------------" << endl;



            }
        }
    } else { //if the vector is zero 
        cout << "-----------------------------------------------------------" << endl;
        cout << "   No valid route. Or you've arrived at the destination.   " << endl;
        cout << "-----------------------------------------------------------" << endl;


    }
}

int angle_cal(t_point first, t_point second) {

    int angle = atan2((second.y - first.y), (second.x - first.x)) * 1 / (DEG_TO_RAD);

    return angle;
}

void draw_one_way(void) {
    for (unsigned i = 0; i < totalNumStreetSegs; i++) {
        StreetSegmentInfo ssi;
        ssi = getStreetSegmentInfo(i);
        int angle;
        t_point p1, p2, c1, c2, center;
        setcolor(128, 128, 128); //GREY
        if (zoom_level() <= ARROW_THRESHOLD) {
            if (ssi.oneWay) {
                string arrow = "\u2192";
                setfontsize(8);
                if (ssi.curvePointCount) {

                    //first point 
                    p1 = convertLatLonToCartesian(getIntersectionPosition(ssi.from), latAverage);
                    //first curve point 
                    c1 = convertLatLonToCartesian(getStreetSegmentCurvePoint(i, 0), latAverage);
                    //last curve point 
                    c2 = convertLatLonToCartesian(getStreetSegmentCurvePoint(i, ssi.curvePointCount - 1), latAverage);
                    //last point
                    p2 = convertLatLonToCartesian(getIntersectionPosition(ssi.to), latAverage);
                    angle = angle_cal(p1, c1);

                    center.x = (p1.x + c1.x) / 2;
                    center.y = (p1.y + c1.y) / 2;
                    settextrotation(angle);
                    drawtext(center.x, center.y, arrow, 600, 600);
                    angle = angle_cal(c2, p2);

                    center.x = (p2.x + c2.x) / 2;
                    center.y = (p2.y + c2.y) / 2;

                    settextrotation(angle);
                    drawtext(center.x, center.y, arrow, 600, 600);

                } else {
                    p1 = convertLatLonToCartesian(getIntersectionPosition(ssi.from), latAverage);
                    p2 = convertLatLonToCartesian(getIntersectionPosition(ssi.to), latAverage);
                    angle = angle_cal(p1, p2);
                    settextrotation(angle);

                    center.x = (p1.x + p2.x) / 2;
                    center.y = (p1.y + p2.y) / 2;

                    drawtext(center.x, center.y, arrow, 600, 600);

                }
            }
        }
    }
}
/******************************************************************************/

/**********************Feature & POI Graphics Functions************************/

void draw_feature(void) {
    //Draws closed features then open feature

    unsigned NumClosedFeat = features_closed.size();
    unsigned NumOpenFeat = features_open.size();



    //Draw closed features
    for (unsigned i = 0; i < NumClosedFeat; i++) {
        featureInfo feature_toDraw = features_closed[i];

        unsigned numPoints = feature_toDraw.featurePoints.size();
        t_point *polyPoints = new t_point[numPoints];

        for (unsigned k = 0; k < numPoints; k++) {
            polyPoints[k] = feature_toDraw.featurePoints[k];
        }

        setcolor(feature_toDraw.featureColor);
        fillpoly(polyPoints, numPoints);
        delete [] polyPoints;
    }

    //Draw open features
    for (unsigned j = 0; j < NumOpenFeat; j++) {
        featureInfo feature_toDraw = features_open[j];
        unsigned numPointsDraw = feature_toDraw.featurePoints.size() - 1;

        for (unsigned m = 0; m < numPointsDraw; m++) {
            t_point position1 = feature_toDraw.featurePoints[m];
            t_point position2 = feature_toDraw.featurePoints[m + 1];
            setlinewidth(SIZE1);
            setlinestyle(SOLID, ROUND);
            setcolor(feature_toDraw.featureColor);
            drawline(position1, position2);
        }
    }



}

void draw_interest(void) {

    for (unsigned i = 0; i < totalNumPOI; i++) {

        interestInfo interest_to_draw = interests[i];
        LatLon position = getPointOfInterestPosition(i);
        t_point coord = convertLatLonToCartesian(position, latAverage);
        //get position for POI

        if (zoom_level() > 0.02 && zoom_level() <= 0.1) {
            // show only ten icons, not all poi types are classified
            if (interest_to_draw.interestType == "fuel") {
                t_point coordinate = interest_to_draw.interestPosition;
                Surface image = load_png_from_file("libstreetmap/resources/fuel.png");
                draw_surface(image, coordinate.x, coordinate.y);
                //draw gas station icon

            } else if (interest_to_draw.interestType == "atm" || interest_to_draw.interestType == "bank" || interest_to_draw.interestType == "bureau_de_change") {
                t_point coordinate = interest_to_draw.interestPosition;
                Surface image = load_png_from_file("libstreetmap/resources/atm.png");
                draw_surface(image, coordinate.x, coordinate.y);
                //draw bank icon

            } else if (interest_to_draw.interestType == "toilets") {
                t_point coordinate = interest_to_draw.interestPosition;
                Surface image = load_png_from_file("libstreetmap/resources/toilets.png");
                draw_surface(image, coordinate.x, coordinate.y);
                //draw toilets icon

            } else if (interest_to_draw.interestType == "hospital" || interest_to_draw.interestType == "clinic" || interest_to_draw.interestType == "doctor") {
                t_point coordinate = interest_to_draw.interestPosition;
                Surface image = load_png_from_file("libstreetmap/resources/hospital.png");
                draw_surface(image, coordinate.x, coordinate.y);
                //draw hospital icon

            } else if (interest_to_draw.interestType == "telephone") {
                t_point coordinate = interest_to_draw.interestPosition;
                Surface image = load_png_from_file("libstreetmap/resources/telephone.png");
                draw_surface(image, coordinate.x, coordinate.y);
                //draw telephone icon

            } else if (interest_to_draw.interestType == "theatre" || interest_to_draw.interestType == "cinema") {
                t_point coordinate = interest_to_draw.interestPosition;
                Surface image = load_png_from_file("libstreetmap/resources/theatre.png");
                draw_surface(image, coordinate.x, coordinate.y);
                //draw theatre icon

            } else if (interest_to_draw.interestType == "bar" || interest_to_draw.interestType == "nightclub") {
                t_point coordinate = interest_to_draw.interestPosition;
                Surface image = load_png_from_file("libstreetmap/resources/bar.png");
                draw_surface(image, coordinate.x, coordinate.y);
                //draw bar icon

            } else if (interest_to_draw.interestType == "coffee") {
                t_point coordinate = interest_to_draw.interestPosition;
                Surface image = load_png_from_file("libstreetmap/resources/coffee.png");
                draw_surface(image, coordinate.x, coordinate.y);
                //draw coffee cup icon

            } else if (interest_to_draw.interestType == "food_court" || interest_to_draw.interestType == "restaurant") {
                t_point coordinate = interest_to_draw.interestPosition;
                Surface image = load_png_from_file("libstreetmap/resources/food_court.png");
                draw_surface(image, coordinate.x, coordinate.y);
                //draw food icon

            } else if (interest_to_draw.interestType == "library" || interest_to_draw.interestType == "university") {
                t_point coordinate = interest_to_draw.interestPosition;
                Surface image = load_png_from_file("libstreetmap/resources/library.png");
                draw_surface(image, coordinate.x, coordinate.y);
                //draw book icon
            }

        } else if (zoom_level() <= 0.02) {
            //All poi points only show in lowest zoom level
            string output_text = interest_to_draw.interestName;
            setcolor(180, 222, 255);
            fillarc(coord.x, coord.y, 0.000001, 0, 360);

            setcolor(BLACK);
            setfontsize(10);
            settextrotation(0);
            //output the names of POI
            drawtext(coord.x, coord.y, output_text, 600, 600);

        }
    }
}

void draw_path_endPoints(const vector<unsigned>& path) {
    string arrow_r = "\u2192";

    //rightwards
    setlinewidth(3);

    setcolor(0, 179, 253);

    if (path.size() == 1) {

        StreetSegmentInfo info;
        LatLon segFrom, segTo;
        t_point from, to;

        for (unsigned i = 0; i < 1; i++) {
            info = getStreetSegmentInfo(path[i]);
            if (info.curvePointCount == 0) {

                segFrom = getIntersectionPosition(info.from);
                from = convertLatLonToCartesian(segFrom, latAverage);
                segTo = getIntersectionPosition(info.to);
                to = convertLatLonToCartesian(segTo, latAverage);

                drawline(from.x, from.y, to.x, to.y);

            } else if (info.curvePointCount == 1) {

                segFrom = getIntersectionPosition(info.from);
                from = convertLatLonToCartesian(segFrom, latAverage);
                segTo = getIntersectionPosition(info.to);
                to = convertLatLonToCartesian(segTo, latAverage);
                t_point curve = convertLatLonToCartesian(getStreetSegmentCurvePoint(path[i], 0), latAverage); //curve point position in t_point

                setcolor(0, 179, 253);
                drawline(from.x, from.y, curve.x, curve.y);
                drawline(curve.x, curve.y, to.x, to.y);

            } else {

                segFrom = getIntersectionPosition(info.from);
                from = convertLatLonToCartesian(segFrom, latAverage);
                segTo = getIntersectionPosition(info.to);
                to = convertLatLonToCartesian(segTo, latAverage);
                t_point curve = convertLatLonToCartesian(getStreetSegmentCurvePoint(path[i], 0), latAverage); //curve point position in t_point
                setcolor(0, 179, 253);
                drawline(from.x, from.y, curve.x, curve.y);

                for (unsigned m = 0; m < (info.curvePointCount - 1); m++) {
                    t_point curve_position_1, curve_position_2;
                    curve_position_1 = convertLatLonToCartesian(getStreetSegmentCurvePoint(path[i], m), latAverage);
                    curve_position_2 = convertLatLonToCartesian(getStreetSegmentCurvePoint(path[i], m + 1), latAverage);
                    setcolor(0, 179, 253);
                    drawline(curve_position_1.x, curve_position_1.y, curve_position_2.x, curve_position_2.y);

                }

                //draw the line between last curve point and the ending point
                t_point last_curve_point = convertLatLonToCartesian(getStreetSegmentCurvePoint(path[i], info.curvePointCount - 1), latAverage);
                setcolor(0, 179, 253);
                drawline(last_curve_point.x, last_curve_point.y, to.x, to.y);

            }
        }

        t_point startpin, endpin;

        info = getStreetSegmentInfo(path[0]);

        startpin = convertLatLonToCartesian(getIntersectionPosition(info.from), latAverage);

        endpin = convertLatLonToCartesian(getIntersectionPosition(info.to), latAverage);

        unsigned start_pin_screen_x = xworld_to_scrn_fl(startpin.x);
        unsigned start_pin_scrren_y = yworld_to_scrn_fl(startpin.y);

        Surface start_point = "libstreetmap/resources/start_point.png";
        set_coordinate_system(GL_SCREEN);


        draw_surface(start_point, start_pin_screen_x - 10, start_pin_scrren_y - 10);

        set_coordinate_system(GL_WORLD);


        unsigned end_pin_screen_x = xworld_to_scrn_fl(endpin.x);
        unsigned end_pin_scrren_y = yworld_to_scrn_fl(endpin.y);

        Surface end_point = "libstreetmap/resources/end_point.png";
        set_coordinate_system(GL_SCREEN);

        setfontsize(10);
        settextrotation(0);
        string start_name = getIntersectionName(info.from);
        string end_name = getIntersectionName(info.to);
        string start_s = "Start: " + start_name;

        string end_s = "Destination: " + end_name;
        draw_surface(end_point, end_pin_screen_x, end_pin_scrren_y - 12);
        setcolor(BLACK);
        drawtext(start_pin_screen_x - 60, start_pin_scrren_y + 30, start_s, 600, 600);

        drawtext(end_pin_screen_x, end_pin_scrren_y - 60, end_s, 600, 600);
        set_coordinate_system(GL_WORLD);


        LatLon startPoint = getIntersectionPosition(getStreetSegmentInfo(path[0]).from);
        LatLon endPoint = getIntersectionPosition(getStreetSegmentInfo(path[0]).to);

        t_point start_point_1, end_point_1;
        start_point_1 = convertLatLonToCartesian(startPoint, latAverage);
        end_point_1 = convertLatLonToCartesian(endPoint, latAverage);



    } else if (path.size() > 1) {

        unsigned maxsize = path.size();
        StreetSegmentInfo info;
        LatLon segFrom, segTo;
        t_point from, to;

        unsigned startingIntersection;
        unsigned endingIntersection;

        if (getStreetSegmentInfo(path[0]).from == getStreetSegmentInfo(path[1]).from || getStreetSegmentInfo(path[0]).from == getStreetSegmentInfo(path[1]).to)
            startingIntersection = getStreetSegmentInfo(path[0]).to;
        else
            startingIntersection = getStreetSegmentInfo(path[0]).from;

        if (getStreetSegmentInfo(path[maxsize - 1]).to == getStreetSegmentInfo(path[maxsize - 2]).from || getStreetSegmentInfo(path[maxsize - 1]).to == getStreetSegmentInfo(path[maxsize - 2]).to)
            endingIntersection = getStreetSegmentInfo(path[maxsize - 1]).from;
        else
            endingIntersection = getStreetSegmentInfo(path[maxsize - 1]).to;

        for (unsigned j = 0; j < maxsize; j++) {
            info = getStreetSegmentInfo(path[j]);
            setlinewidth(5);


            if (info.curvePointCount == 0) {

                segFrom = getIntersectionPosition(info.from);
                from = convertLatLonToCartesian(segFrom, latAverage);
                segTo = getIntersectionPosition(info.to);
                to = convertLatLonToCartesian(segTo, latAverage);
                setcolor(0, 179, 253);
                drawline(from.x, from.y, to.x, to.y);

            } else if (info.curvePointCount == 1) {

                segFrom = getIntersectionPosition(info.from);
                from = convertLatLonToCartesian(segFrom, latAverage);
                segTo = getIntersectionPosition(info.to);
                to = convertLatLonToCartesian(segTo, latAverage);
                t_point curve = convertLatLonToCartesian(getStreetSegmentCurvePoint(path[j], 0), latAverage); //curve point position in t_point
                setcolor(0, 179, 253);
                drawline(from.x, from.y, curve.x, curve.y);
                drawline(curve.x, curve.y, to.x, to.y);

                if (to.y > from.y && to.x > from.x) {

                    //draw arrow for direction 
                    t_point center;
                    center.x = (from.x + to.x) / 2;
                    center.y = (from.y + to.y) / 2;
                    setfontsize(25);
                    setcolor(6, 82, 114);

                    settextrotation(angle_cal(from, to));
                    if (zoom_level() < 0.1) {
                        drawtext(center.x, center.y, arrow_r, 600, 600);
                    }
                }



            } else {

                segFrom = getIntersectionPosition(info.from);
                from = convertLatLonToCartesian(segFrom, latAverage);
                segTo = getIntersectionPosition(info.to);
                to = convertLatLonToCartesian(segTo, latAverage);
                t_point curve = convertLatLonToCartesian(getStreetSegmentCurvePoint(path[j], 0), latAverage); //curve point position in t_point
                setcolor(0, 179, 253);
                drawline(from.x, from.y, curve.x, curve.y);

                for (unsigned m = 0; m < (info.curvePointCount - 1); m++) {
                    t_point curve_position_1, curve_position_2;
                    curve_position_1 = convertLatLonToCartesian(getStreetSegmentCurvePoint(path[j], m), latAverage);
                    curve_position_2 = convertLatLonToCartesian(getStreetSegmentCurvePoint(path[j], m + 1), latAverage);
                    setcolor(0, 179, 253);
                    drawline(curve_position_1.x, curve_position_1.y, curve_position_2.x, curve_position_2.y);



                }

                //draw the line between last curve point and the ending point

                t_point last_curve_point = convertLatLonToCartesian(getStreetSegmentCurvePoint(path[j], info.curvePointCount - 1), latAverage);
                setcolor(0, 179, 253);
                drawline(last_curve_point.x, last_curve_point.y, to.x, to.y);

            }
        }

        t_point startpin, endpin;
        startpin = convertLatLonToCartesian(getIntersectionPosition(startingIntersection), latAverage);
        endpin = convertLatLonToCartesian(getIntersectionPosition(endingIntersection), latAverage);

        unsigned start_pin_screen_x = xworld_to_scrn_fl(startpin.x);
        unsigned start_pin_scrren_y = yworld_to_scrn_fl(startpin.y);

        Surface start_point = "libstreetmap/resources/start_point.png";
        set_coordinate_system(GL_SCREEN);


        draw_surface(start_point, start_pin_screen_x - 10, start_pin_scrren_y - 10);

        set_coordinate_system(GL_WORLD);


        unsigned end_pin_screen_x = xworld_to_scrn_fl(endpin.x);
        unsigned end_pin_scrren_y = yworld_to_scrn_fl(endpin.y);

        Surface end_point = "libstreetmap/resources/end_point.png";
        set_coordinate_system(GL_SCREEN);

        setfontsize(10);
        settextrotation(0);
        string start_name = getIntersectionName(startingIntersection);
        string end_name = getIntersectionName(endingIntersection);
        string start_s = "Start: " + start_name;

        string end_s = "Destination: " + end_name;
        draw_surface(end_point, end_pin_screen_x, end_pin_scrren_y - 12);
        setcolor(BLACK);
        drawtext(start_pin_screen_x - 10, start_pin_scrren_y + 30, start_s, 600, 600);

        drawtext(end_pin_screen_x, end_pin_scrren_y - 20, end_s, 600, 600);

        set_coordinate_system(GL_WORLD);


    }
}

double turningDetermination(vector<t_point> vector1, vector<t_point> vector2) {
    //use cross product to determine turning direction
    double crossProduct;

    crossProduct = vector1[0].x * vector2[0].y - vector1[0].y * vector2[0].x;

    return crossProduct;

}
/******************************************************************************/

/***********************Search Bar Searching Functions*************************/

bool fuzzyCompare(nameToDist &element1, nameToDist &element2) {
    //Sort in ascending order
    return (element1.second < element2.second);
}

string fuzzySearch(string &src) {
    //Performs a fuzzy search using Levenshtein algorithm
    vector<nameToDist> bestMatches;

    for (unsigned i = 0; i < streetIDToName.size(); i++) {

        int distance = levenshtein_distance(src, streetIDToName[i]);
        bestMatches.push_back(nameToDist(streetIDToName[i], distance));
    }

    sort(bestMatches.begin(), bestMatches.end(), fuzzyCompare);


    unsigned index = 0; //Default value
    //Find first instance of matching first character
    for (unsigned j = 0; j < bestMatches.size(); j++) {

        if (tolower(src[0]) == tolower(bestMatches[j].first[0])) {
            index = j;
            break;
        }
    }

    return bestMatches[index].first; //0th element has minimum distance and is best match
}

void execute_search() {

    string exactStreet1;
    string exactStreet2;

    //Extract first and second relative street names
    //Trim is used to remove leading and trailing white spaces
    size_t posOfAnd = user_input.find("&");
    string street1 = user_input.substr(0, posOfAnd - 1);
    string street2 = user_input.substr(posOfAnd + 1);
    boost::trim(street1);
    boost::trim(street2);

    //Now we need to get the exact street names 
    exactStreet1 = fuzzySearch(street1);
    exactStreet2 = fuzzySearch(street2);

    cout << "Searching for intersections coincident on " << exactStreet1 << " & " << exactStreet2 << endl;

    //Now we get the intersection IDS coincident with those two streets
    vector<unsigned> inter_ids = find_intersection_ids_from_street_names(exactStreet1, exactStreet2);

    if (inter_ids.size() != 0) {
        //Print to terminal the name of each coincident intersections
        cout << endl << "There are " << inter_ids.size() << " intersections are coincident on " << exactStreet1 <<
                " & " << exactStreet2 << ":" << endl;

        for (unsigned i = 0; i < inter_ids.size(); i++) {

            LatLon position = getIntersectionPosition(inter_ids[i]);
            cout << "At position: (" << position.lat() << "," << position.lon() << ")" << endl;

            //Draw circles at coincident intersectionss
            t_point coordinate = convertLatLonToCartesian(position, latAverage);
            setcolor(28, 20, 13);
            fillarc(coordinate.x, coordinate.y, 0.000001, 0, 360);

        }

        copy_off_screen_buffer_to_screen(); //display intersections

    } else {
        cout << "No intersections exist" << flush << endl;
    }

}

/******************************************************************************/

/******************User-input Interface Conditional Functions******************/

//The function below determines where the user has clicked on the search bar to 
//search for an intersection or a point of interest. Refer to the constants defined
//in UI_constants.h for details, although the names should be self explanatory. 
//This allows one to know whether keystroke inputs should be allowed or not, 
//whether placeholder text should be drawn or not, and whether intersections 
//should be highlighted upon mouse clicks or not.

//#define DEBUG_CLICKS_ON_UI_BOUNDED_AREAS

int check_search_area_clicked(t_point mouse_click_coordinate) {

    //we will be using screen coordinates for the comparison so converted the mouse_click 
    //point to screen coords from world coordinates.

    t_point clicked_point_screen_coord = world_to_scrn(mouse_click_coordinate);

    //define the respective bounded areas. Note this is in screen coordinates
    //so the conventional "bottom" and "top" of box are reversed.

    //the query region within which the user enters his text

    t_bound_box query_region = t_bound_box(get_visible_screen().left(), get_visible_screen().top(), get_visible_screen().left() + QUERY_AREA_WIDTH_PIXELS, get_visible_screen().top() + QUERY_AREA_HEIGHT_PIXELS);

    //the search icon region

    t_bound_box search_icon_box = t_bound_box(get_visible_screen().left() + QUERY_AREA_WIDTH_PIXELS, get_visible_screen().top(), get_visible_screen().left() + QUERY_AREA_WIDTH_PIXELS + SEARCH_ICON_WIDTH_PIXELS, get_visible_screen().top() + SEARCH_ICON_HEIGHT_PIXELS);

    //the get directions icon region

    t_bound_box directions_icon_box = t_bound_box(get_visible_screen().left() + QUERY_AREA_WIDTH_PIXELS + SEARCH_ICON_WIDTH_PIXELS, get_visible_screen().top(), get_visible_screen().left() + QUERY_AREA_WIDTH_PIXELS + SEARCH_ICON_WIDTH_PIXELS + DIRECTIONS_ICON_WIDTH_PIXELS, get_visible_screen().top() + DIRECTIONS_ICON_HEIGHT_PIXELS);

    if ((query_region).intersects(clicked_point_screen_coord)) {

#ifdef DEBUG_CLICKS_ON_UI_BOUNDED_AREAS
        cout << "Query" << endl;
#endif

        return (SEARCH_QUERY);
    }

    if ((search_icon_box).intersects(clicked_point_screen_coord)) {

#ifdef DEBUG_CLICKS_ON_UI_BOUNDED_AREAS
        cout << "Search" << endl;
#endif

        return (SEARCH_HIT);
    }


    if ((directions_icon_box).intersects(clicked_point_screen_coord)) {

#ifdef DEBUG_CLICKS_ON_UI_BOUNDED_AREAS
        cout << "Path" << endl;
#endif

        return (GET_DIRECTIONS);
    }

#ifdef DEBUG_CLICKS_ON_UI_BOUNDED_AREAS
    cout << "No" << endl;
#endif

    return (OUTSIDE_SEARCH_BAR_BOUNDS);

}

//similar function used for the path finding direction menu dialog box.
//Refer to the documentation in the function above for info.

int check_directions_menu_clicked(t_point mouse_click_coordinate) {

    t_point clicked_point_screen_coord = world_to_scrn(mouse_click_coordinate);

    //the button for choosing a path from intersection to intersection

    t_bound_box search_Intersection_to_Intersection = t_bound_box(get_visible_screen().left(), get_visible_screen().top(), get_visible_screen().left() + INT2INT_BUTTON_WIDTH_PIXELS, get_visible_screen().top() + INT2INT_BUTTON_HEIGHT_PIXELS);

    //the button for choosing a path from intersection to POI

    t_bound_box search_Intersection_to_POI = t_bound_box(get_visible_screen().left() + INT2INT_BUTTON_WIDTH_PIXELS, get_visible_screen().top(), get_visible_screen().left() + INT2INT_BUTTON_WIDTH_PIXELS + INT2POI_BUTTON_WIDTH_PIXELS, get_visible_screen().top() + INT2POI_BUTTON_HEIGHT_PIXELS);

    //the button to quit the dialog menu

    t_bound_box quit_directions_box = t_bound_box(get_visible_screen().left() + INT2INT_BUTTON_WIDTH_PIXELS + INT2POI_BUTTON_WIDTH_PIXELS, get_visible_screen().top(), get_visible_screen().left() + DIRECTIONS_MENU_WIDTH_PIXELS, get_visible_screen().top() + QUIT_DIRECTIONS_HEIGHT_PIXELS);

    //button to flip paths

    t_bound_box reverse_paths = t_bound_box(get_visible_screen().left(), get_visible_screen().top() + DIRECTIONS_MENU_HEIGHT_PIXELS - REVERSE_PATHS_ICON_HEIGHT_PIXELS, get_visible_screen().left() + REVERSE_PATHS_ICON_WIDTH_PIXELS, get_visible_screen().top() + DIRECTIONS_MENU_HEIGHT_PIXELS);

    //query for source path

    t_bound_box source_query_box = t_bound_box(get_visible_screen().left() + REVERSE_PATHS_ICON_WIDTH_PIXELS, get_visible_screen().top() + DIRECTIONS_MENU_HEIGHT_PIXELS - REVERSE_PATHS_ICON_HEIGHT_PIXELS, get_visible_screen().left() + REVERSE_PATHS_ICON_WIDTH_PIXELS + SOURCE_QUERY_AREA_WIDTH_PIXELS, get_visible_screen().top() + DIRECTIONS_MENU_HEIGHT_PIXELS - DEST_QUERY_AREA_HEIGHT_PIXELS);

    //query for destination path

    t_bound_box dest_query_box = t_bound_box(get_visible_screen().left() + REVERSE_PATHS_ICON_WIDTH_PIXELS, get_visible_screen().top() + DIRECTIONS_MENU_HEIGHT_PIXELS - DEST_QUERY_AREA_HEIGHT_PIXELS, get_visible_screen().left() + REVERSE_PATHS_ICON_WIDTH_PIXELS + DEST_QUERY_AREA_WIDTH_PIXELS, get_visible_screen().top() + DIRECTIONS_MENU_HEIGHT_PIXELS);

    //search for directions icon

    t_bound_box search_directions = t_bound_box(get_visible_screen().left() + REVERSE_PATHS_ICON_WIDTH_PIXELS + SOURCE_QUERY_AREA_WIDTH_PIXELS, get_visible_screen().top() + DIRECTIONS_MENU_HEIGHT_PIXELS - REVERSE_PATHS_ICON_HEIGHT_PIXELS, get_visible_screen().left() + REVERSE_PATHS_ICON_WIDTH_PIXELS + SOURCE_QUERY_AREA_WIDTH_PIXELS + FIND_PATH_ICON_WIDTH_PIXELS, get_visible_screen().top() + DIRECTIONS_MENU_HEIGHT_PIXELS - DEST_QUERY_AREA_HEIGHT_PIXELS);

    if ((search_Intersection_to_Intersection).intersects(clicked_point_screen_coord)) {

#ifdef DEBUG_CLICKS_ON_UI_BOUNDED_AREAS
        cout << "INT2INT" << endl;
#endif

        return (INT2INT);
    }

    if ((search_Intersection_to_POI).intersects(clicked_point_screen_coord)) {

#ifdef DEBUG_CLICKS_ON_UI_BOUNDED_AREAS
        cout << "INT2POI" << endl;
#endif

        return (SEARCH_HIT);
    }

    if ((source_query_box).intersects(clicked_point_screen_coord)) {

#ifdef DEBUG_CLICKS_ON_UI_BOUNDED_AREAS
        cout << "SOURCE" << endl;
#endif

        return (SOURCE_QUERY);
    }

    if ((dest_query_box).intersects(clicked_point_screen_coord)) {

#ifdef DEBUG_CLICKS_ON_UI_BOUNDED_AREAS
        cout << "DEST" << endl;
#endif

        return (DEST_QUERY);
    }

    if ((search_directions).intersects(clicked_point_screen_coord)) {

#ifdef DEBUG_CLICKS_ON_UI_BOUNDED_AREAS
        cout << "FIND PATH" << endl;
#endif

        return (FIND_PATH);
    }

    if ((reverse_paths).intersects(clicked_point_screen_coord)) {

#ifdef DEBUG_CLICKS_ON_UI_BOUNDED_AREAS
        cout << "REVERSE PATHS" << endl;
#endif

        return (FLIP_PATH);
    }

    if ((quit_directions_box).intersects(clicked_point_screen_coord)) {

#ifdef DEBUG_CLICKS_ON_UI_BOUNDED_AREAS
        cout << "Quitting" << endl;
        //        clicked_get_directions = false;
#endif
        return (QUIT_DIRECTIONS);
    }

#ifdef DEBUG_CLICKS_ON_UI_BOUNDED_AREAS
    cout << "Not in direction bar" << endl;
#endif

    return (OUTSIDE_DIRECTION_BOX_BOUNDS);

}

bool clicked_on_quit_help(t_point mouse_click_coordinate) {

    t_point clicked_point_screen_coord = world_to_scrn(mouse_click_coordinate);

    t_bound_box quit_help = t_bound_box(get_visible_screen().right() - QUIT_HELP_WIDTH_PIXELS, get_visible_screen().bottom() - HELP_MENU_HEIGHT_PIXELS, get_visible_screen().right(), get_visible_screen().bottom() - HELP_MENU_HEIGHT_PIXELS + QUIT_HELP_HEIGHT_PIXELS);

    if ((quit_help).intersects(clicked_point_screen_coord)) {

        clicked_on_help_button = false;
        draw_screen();
    }

    return (clicked_on_help_button);
}

/******************************************************************************/

/***********************User Input Parsing Functions************************/

//These functions are used to parse the inputted data into the needed form to perform
//the user's desired computation. In the case of finding the shortest path, this involves
//converting the inputted strings or mouse coordinates into intersection IDs. For inputs
//defining POIs these are just left as strings so there is no need to parse this data.

//This function converts the intersection string name to 

#define DEBUG_INPUT_PARSING




//Helper function to parse in the input source/dest point for the path to be computed.
//The function assumes input has been entered in one way or the other, i.e. keyboard
//input or mouse click.

unsigned parse_input_intersection_point(string typed_input, LatLon clicked_coordinate) {

    unsigned parsed_ID;

    //check if the user has typed in a query

    if (typed_input.size() != 0)
        parsed_ID = convert_input_Intersection_Name_to_ID(typed_input);

        //otherwise he has clicked at a point

    else
        parsed_ID = find_closest_point_of_interest(clicked_coordinate);

    return (parsed_ID);

}

string parse_input_POI_point(string typed_input, LatLon clicked_coordinate) {

    string POI_name;

    //check if the user has typed in a query

    if (typed_input.size() != 0)
        POI_name = typed_input;

        //otherwise he has clicked at a point

    else {
        unsigned POI_ID = find_closest_point_of_interest(clicked_coordinate);
        POI_name = getPointOfInterestName(POI_ID);
    }

    return (POI_name);
}
/******************************************************************************/

/***********************Highlight Intersections/POIs***************************/

#define BASE_RADIUS 0.00001
#define MAX_RADIUS 0.00003
#define MULTIPLIER_RADIUS 0.00002

void highlight_closest_intersection_on_mouse_click(LatLon & mousePosition) {

    has_clicked = true;

    //Find nearest intersection id and x-y coordinate
    unsigned intersectionID = find_closest_intersection(mousePosition);
    t_point nearestInterCoord = convertLatLonToCartesian(getIntersectionPosition(intersectionID), latAverage);
    string inter_name = getIntersectionName(intersectionID);


    //    if (zoom_level)
    setcolor(237, 24, 24);

    if (!clicked_get_directions) {
        fillarc(nearestInterCoord.x, nearestInterCoord.y, MAX_RADIUS - (1 - zoom_level() / 100) * MULTIPLIER_RADIUS, 0, 360);
        if (zoom_level() <= 0.65) {//show intersection names in certain zoom level
            setfontsize(10);
            settextrotation(0);
            setcolor(BLACK);
            drawtext(nearestInterCoord.x, nearestInterCoord.y,
                    inter_name, FLT_MAX, FLT_MAX);
        }

        copy_off_screen_buffer_to_screen();
    } else {

        if (num_of_clicks_directions == 1 && source_path.size() == 0) {

            Surface start_point = "libstreetmap/resources/start_icon.png";

            t_point clicked_screen_coords = world_to_scrn(nearestInterCoord);

            set_coordinate_system(GL_SCREEN);

            draw_surface(start_point, clicked_screen_coords.x - 10, clicked_screen_coords.y - 10);

            copy_off_screen_buffer_to_screen();

            set_coordinate_system(GL_WORLD);
        } else if (num_of_clicks_directions == 2 || dest_path.size() == 0) {

            Surface end_point = "libstreetmap/resources/end_icon.png";

            t_point clicked_screen_coords = world_to_scrn(nearestInterCoord);

            set_coordinate_system(GL_SCREEN);

            draw_surface(end_point, clicked_screen_coords.x, clicked_screen_coords.y - 12);

            copy_off_screen_buffer_to_screen();

            set_coordinate_system(GL_WORLD);
        }

    }




}

void highlight_closest_interest_on_mouse_click(LatLon & mousePosition) {

    //Find closest point of interest
    unsigned poiID = find_closest_point_of_interest(mousePosition);

    interestInfo interest_to_draw = interests[poiID];

    t_point poiCoords = interest_to_draw.interestPosition;
    //use a black pin to show the point
    //Surface pin1 = "libstreetmap/resources/pin1.png";
    setcolor(RED);
    fillarc(poiCoords.x, poiCoords.y, 0.000001, 0, 360);
    string poi_name = interest_to_draw.interestName;

    if (zoom_level() <= 0.65) {//show poi names in certain zoom level
        setcolor(BLACK);
        setfontsize(10);
        settextrotation(0);
        //output the name of POI in black
        drawtext(poiCoords.x, poiCoords.y, poi_name, 600, 600);
    }

}
/******************************************************************************/

/**********************User-input Interface Drawing Functions******************/

//Function to draw a search bar. The search bar is taken from the Google Maps 
//search bar, customized accordingly, and converted to .png format. The search 
//bar coordinate is set to screen coordinates to ensure it is located constant 
//relative to the window.

void draw_search_bar(void) {

    Surface search_bar_png = "libstreetmap/resources/search_bar_reworked.png";

    // Establish a screen coordinate system to ensure a constant offset that
    // allows the search bar to stay in the same location relative to the window
    set_coordinate_system(GL_SCREEN);

    //draw the search bar icon
    draw_surface(search_bar_png, get_visible_screen().left(), get_visible_screen().top());

    if (search_query) {

        setcolor(BLACK);

        settextrotation(0);

        t_bound_box query_region = t_bound_box(get_visible_screen().left(), get_visible_screen().top(), get_visible_screen().left() + QUERY_AREA_WIDTH_PIXELS, get_visible_screen().top() + QUERY_AREA_HEIGHT_PIXELS);
        drawtext_in(query_region, user_input);

        flushinput();
    }

    // Reset coordinate systems
    set_coordinate_system(GL_WORLD);
}


//Function to draw the path finding menu. The path finding menu is inspired
//by Google's Maps "Get Directions" dialog box, but customized to search for
//specific paths for intersection - intersection and intersection - POI.

void draw_path_finding_box(void) {

    Surface path_finder = "libstreetmap/resources/path_finding_menu.png";

    // Establish a screen coordinate system to ensure a constant offset that
    // allows the search bar to stay in the same location relative to the window
    set_coordinate_system(GL_SCREEN);

    //draw the path finding menu

    draw_surface(path_finder, get_visible_screen().left(), get_visible_screen().top());

    setcolor(BLACK);

    settextrotation(0);
}

unsigned convert_input_Intersection_Name_to_ID(string intersection_name) {

    //    //Extract first and second relative street names
    //    //Trim is used to remove leading and trailing white spaces
    //    size_t posOfAnd = intersection_name.find("&");
    //    string street1 = intersection_name.substr(0, posOfAnd - 1);
    //    string street2 = intersection_name.substr(posOfAnd + 1);
    //    boost::trim(street1);
    //    boost::trim(street2);
    //    
    //    string exactStreet1, exactStreet2;
    //    
    //    vector<string> exactNames_st1;
    //    vector<string> exactNames_st2;
    //    
    //     exactNames_st1 = streetNameFinder->autocomplete(street1);
    //     exactNames_st2  = streetNameFinder->autocomplete(street2);
    //    
    //    //Check if relative street name actually exists
    //    if ((exactNames_st1.size() != 0) && (exactNames_st2.size() != 0)) {
    //        //We expect the first element is the closest to desired name
    //        exactStreet1 = exactNames_st1[0];
    //        exactStreet2 = exactNames_st2[0];
    //    
    //    vector<unsigned> intersection_ids = find_intersection_ids_from_street_names(street1, street2);
    //    
    //    //For simplicity sake, if multiple intersections exists for the same name, we will
    //    //just take the first intersection ID. I will later try to improve this to allow the user to
    //    //choose which one is the one he desires. 
    //    
    //    if (intersection_ids.size() >= 1)
    //        return (intersection_ids[0]);
    //    
    //    //in the case of an empty vector, return an error value
    //    
    //    if (intersection_ids.size() == 0)
    //        return (INVALID_INPUT);
    //    }
    //else return(INAVLID_INPUT);
    return (INVALID_INPUT);


    //    drawtext_in(source_query_dialog_box, source_path);
    //
    //    drawtext_in(dest_query_dialog_box, dest_path);
    //
    //    flushinput();
    //
    //    // Reset coordinate systems
    //    set_coordinate_system(GL_WORLD);

}

//Function to display the help menu. It gets called as soon as the help button 
//is clicked. The user can quit the menu by clicking the 'x' sign. 

void display_help_menu(void) {

    Surface help_menu = "libstreetmap/resources/help_prompt.png";

    // Establish a screen coordinate system to ensure a constant offset that
    // allows the search bar to stay in the same location relative to the window

    set_coordinate_system(GL_SCREEN);

    //draw the search bar icon

    draw_surface(help_menu, get_visible_screen().get_center().x + HELP_MENU_WIDTH_PIXELS, get_visible_screen().get_center().y);

    flushinput();

    copy_off_screen_buffer_to_screen();

    // Reset coordinate systems
    set_coordinate_system(GL_WORLD);
}

void help_button_callback(void (*drawscreen_ptr)(void)) {

    clicked_on_help_button = true;

    display_help_menu();

    drawscreen_ptr();
}

/******************************************************************************/

/************************User Input Enabler Functions**************************/

//Function to draw user inputted text on the search bar

//#define DEBUG_KEYPRESSES

void act_on_key_press(char key_pressed, int keysym) {

    //    #ifdef DEBUG_KEYPRESSES
    //        cout << key_pressed <<endl;
    //        #endif

    if (key_pressed != BACKSPACE_ASCII && key_pressed != NULL_TERM_ASCII) {
        user_input += key_pressed;
#ifdef DEBUG_KEYPRESSES
        cout << user_input;
#endif
    }

    if (key_pressed == BACKSPACE_ASCII && key_pressed != NULL_TERM_ASCII) {
        if (user_input.length() != 0)
            user_input.pop_back();
    }

    //Now based on the state of the query flags, concatenate the current user_input to
    //the respective string

    if (source_query) {

        source_path = user_input;
    }

    if (dest_query) {

        dest_path = user_input;
    }

    draw_screen();
}

// Function to allow the user to interact with mouse clicks. It records the coordinates
// where the click occurred and conducts actions accordingly. Some of the functionality
// is as follows: 
// a) If the user clicks on a part of the map, the closest intersection to the point
//    is highlighted
// b) If the user clicks on various parts of the search bar, searches are enabled
//    or dialog boxes get drawn accordingly
// c) If the user wishes to quit either the directions box or the help box, checks
//    for this

//pin icons get online from: https://www.iconfinder.com/search/?q=pin

//#define DEBUG_MOUSE_CLICKS

//Helper functions to reset queries

void reset_query(void) {

    user_input = ""; //Reset the input string for a new search
    set_keypress_input(true); // allow user to enter text
}

void act_on_button_press(float xPos, float yPos, t_event_buttonPressed event) {

    //    if (has_clicked) //Check if the user has clicked before 
    //        draw_screen(); //Clear screen and redraw if so

    t_point coordinate(xPos, yPos); //Create t_point for mouse position

    LatLon mousePosition = convertCartesianToLatLon(coordinate, latAverage);

#ifdef DEBUG_MOUSE_CLICKS

    cout << "x position: " << coordinate.x << endl;
    cout << "y position: " << coordinate.y << endl;

#endif

    if (event.button == 1) { //left click

        //check search functionality. So first need to determine which part of the
        //search bar the user has clicked on and then act accordingly. 
        t_bound_box source_query_dialog_box = t_bound_box(get_visible_screen().left() + REVERSE_PATHS_ICON_WIDTH_PIXELS, get_visible_screen().top() + DIRECTIONS_MENU_HEIGHT_PIXELS - REVERSE_PATHS_ICON_HEIGHT_PIXELS, get_visible_screen().left() + REVERSE_PATHS_ICON_WIDTH_PIXELS + SOURCE_QUERY_AREA_WIDTH_PIXELS, get_visible_screen().top() + DIRECTIONS_MENU_HEIGHT_PIXELS - DEST_QUERY_AREA_HEIGHT_PIXELS);
        // this means the user should be able to type in input to search

        search_query = true;

        reset_query();

    }

    if (check_search_area_clicked(coordinate) == SEARCH_HIT) {

        // this means user has clicked search icon. Need to execute search

        if (user_input.size() != 0) {

            string search_query_input = user_input;

            unsigned intID = convert_input_Intersection_Name_to_ID(search_query_input);

            if (path_option == INVALID_INPUT) {

                update_message("Please enter a valid intersection");

#ifdef DEBUG_INPUT_PARSING

                cout << intID << endl;

                cout << getIntersectionName(intID) << endl;

#endif

            } else {

                execute_search();

                set_keypress_input(false);
            }

        } else
            update_message("Please enter valid input");
    }

    if (check_search_area_clicked(coordinate) == GET_DIRECTIONS) {

        // this means user wishes to search for a path. Draw the directions
        // menu, update the state variable so that one doesn't check for search
        // bar related clicks until the user quits the directions box

        clicked_get_directions = true;
        draw_screen(); //redraw screen

        set_keypress_input(false);
    }

    if (check_search_area_clicked(coordinate) == OUTSIDE_SEARCH_BAR_BOUNDS) {

        if (clicked_on_quit_help(coordinate)) {
            clicked_on_help_button = false;
            draw_screen();
        } else highlight_closest_intersection_on_mouse_click(mousePosition); //highlight intersection
    }


    //check directions box functionality. So first need to determine which part of the
    //dialog menu the user has clicked on and then act accordingly. 

    if (clicked_get_directions) {

        //set flags, and option codes for getting directions

        count_get_direction_click += 1;

        search_query = false;

        //store mouse clicks as means of getting input for source and destination points

        if (check_directions_menu_clicked(coordinate) == INT2INT) {
            // the user can search for a path from an intersection to another
            // intersection.

            update_message("Enter your input to find a path between two intersections");

            path_option = PATH_FROM_INTERSECTION_TO_INTERSECTION;
        }

        if (check_directions_menu_clicked(coordinate) == INT2POI) {

            // the user can search for a path from an intersection to a
            // POI.

            update_message("Enter input to find a path between an intersection and a POI.");

            path_option = PATH_FROM_INTERSECTION_TO_POI;
        }

        if (check_directions_menu_clicked(coordinate) == SOURCE_QUERY) {

            // allow user to type source path.

            source_query = true;
            dest_query = false;

            reset_query();
        }

        if (check_directions_menu_clicked(coordinate) == DEST_QUERY) {

            // allow user to type destination path.

            source_query = false;
            dest_query = true;

            reset_query();
        }

        if (check_directions_menu_clicked(coordinate) == FIND_PATH) {

            // allow user to search for path. This is based on the user's choice
            //of path - either the path from an intersection to another, or to a
            //POI.


            if (path_option == UNDEFINED_OPTION) {

                update_message("Please choose one of the two valid options for path-finding.");
            }

            if (path_option == PATH_FROM_INTERSECTION_TO_INTERSECTION) {

                string source_input, destination_input;

                source_input = source_path;

                destination_input = dest_path;

                unsigned source_intersection_ID, dest_intersection_ID;

                bool source_input_entered = (source_input.size() != 0) || clicked_source_point;

                bool dest_input_entered = (destination_input.size() != 0) || clicked_dest_point;

                if (source_input_entered)
                    source_intersection_ID = parse_input_intersection_point(source_input, source_coordinate);
                else
                    path_option = INVALID_INPUT;

                if (dest_input_entered)
                    dest_intersection_ID = parse_input_intersection_point(destination_input, dest_coordinate);
                else
                    path_option = INVALID_INPUT;

                if (path_option != INVALID_INPUT) {
#ifdef DEBUG_INPUT_PARSING

                    cout << source_intersection_ID << endl;

                    cout << getIntersectionName(source_intersection_ID) << endl;
#endif 

                    t_bound_box dest_query_dialog_box = t_bound_box(get_visible_screen().left() + REVERSE_PATHS_ICON_WIDTH_PIXELS, get_visible_screen().top() + DIRECTIONS_MENU_HEIGHT_PIXELS - DEST_QUERY_AREA_HEIGHT_PIXELS, get_visible_screen().left() + REVERSE_PATHS_ICON_WIDTH_PIXELS + DEST_QUERY_AREA_WIDTH_PIXELS, get_visible_screen().top() + DIRECTIONS_MENU_HEIGHT_PIXELS);
                } else
                    update_message("Please enter a valid path");

                set_keypress_input(false);

            }

            if (path_option == PATH_FROM_INTERSECTION_TO_POI) {

                string source_input, dest_input, destination_POI;

                unsigned source_intersection_ID;

                source_input = source_path;

                dest_input = dest_path;

                source_intersection_ID = parse_input_intersection_point(source_input, source_coordinate);

                if (path_option != INVALID_INPUT) {

                    std::vector<unsigned> shortest_path = find_path_to_point_of_interest(source_intersection_ID, dest_input, turn_penalty);
                    draw_path_endPoints(shortest_path);
                    displayDirections(shortest_path);
                } else
                    update_message("Enter a valid path. Check your POI or intersection is valid");

                set_keypress_input(false);
            }

            //Reset flags to default state for future user inputs

            num_of_clicks_directions = 0;
            clicked_source_point = false;
            clicked_dest_point = false;
        }

        if (check_directions_menu_clicked(coordinate) == FLIP_PATH) {

            // flip the paths. This is an extra feature to worry about way later.

            //                flip_path_inputs();
            //                set_keypress_input(false);
        }

        if (check_directions_menu_clicked(coordinate) == QUIT_DIRECTIONS && count_get_direction_click >= 2) {

            // Quit the help dialog menu. 

            //first disable keyboard input for safety
            set_keypress_input(false);
            clicked_get_directions = false;
            count_get_direction_click = 0;
            draw_screen();
        }

        if (check_directions_menu_clicked(coordinate) == OUTSIDE_DIRECTION_BOX_BOUNDS) {

            num_of_clicks_directions += 1;

            if (source_path.size() == 0 && num_of_clicks_directions == 1) {
                source_coordinate = mousePosition; //store starting point
                clicked_source_point = true;
            }
            if (dest_path.size() == 0 || num_of_clicks_directions == 2) {
                dest_coordinate = mousePosition; //store ending point
                clicked_dest_point = true;
            }

            if (!clicked_on_quit_help(coordinate))
                highlight_closest_intersection_on_mouse_click(mousePosition); //highlight intersection

            else {
                clicked_on_help_button = false;
                draw_screen();
            }
        }
    }
}

/******************************************************************************/

/***********************Event_Loop() Callback Functions************************/

// The callback function where the graphics are drawn. This function is called 
// in event loop while drawing the map.

void draw_screen(void) {

    clearscreen(); //Clear the screen before redrawing
    set_drawing_buffer(OFF_SCREEN); // double buffer
    draw_feature();
    drawStreetSegments();
    draw_street_name();
    draw_interest();
    draw_one_way();
#ifdef DEBUG_ZOOM_LEVEL
    cout << zoom_level() << endl;
#endif

    if (clicked_get_directions == false)
        draw_search_bar();

    if (clicked_get_directions) {
        draw_path_finding_box();
        flushinput();
    }

    if (clicked_on_help_button)
        display_help_menu();

    copy_off_screen_buffer_to_screen();

}

void draw_map(void) {

    init_graphics("Map", background);

    set_visible_world(xMin, yMin, xMax, yMax);

    AREA_MAP = get_visible_world().area(); //Get initial area with no zoom

    create_button("Proceed", "Help", help_button_callback);

    draw_screen();
    event_loop(act_on_button_press, nullptr, act_on_key_press, draw_screen);

    clicked_get_directions = false;
    clicked_on_help_button = false;

    close_graphics();

}

//    if (!clicked_get_directions){

// if (check_search_area_clicked(coordinate) == SEARCH_QUERY){
