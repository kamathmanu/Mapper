///*
// * To change this license header, choose License Headers in Project Properties.
// * To change this template file, choose Tools | Templates
// * and open the template in the editor.
// */
//

#include "m3FuncLib.h"

double compute_path_travel_time(const std::vector<unsigned>& path,
        const double turn_penalty) {

    double path_travel_time = 0.0;
    unsigned size = path.size();

    if (size != 0) { //Check if path has any elements

        for (unsigned i = 0; i < size - 1; i++) {

            unsigned index = path[i];
            unsigned nextIndex = path[i + 1];
            path_travel_time += streetSeg_travelTime[index];

            // If the street ID of the following index != the street ID of the current index,
            // considered a turn and add the turn penalty

            if (streetSegIDToStreetID[index] != streetSegIDToStreetID[nextIndex])
                path_travel_time += turn_penalty;
        }

        // Add the time taken to travel the last street segment of the path.
        path_travel_time += streetSeg_travelTime[path[size - 1]];

    }
    
    return path_travel_time;
}

std::vector<unsigned> find_path_between_intersections(const unsigned intersect_id_start,
        const unsigned intersect_id_end,
        const double turn_penalty) {
    
    return aStar(intersect_id_start, intersect_id_end, turn_penalty);
}

std::vector<unsigned> find_path_to_point_of_interest(const unsigned intersect_id_start,
        const std::string point_of_interest_name,
        const double turn_penalty) {
    //Given a point of interest will return the path of minimum travel time
    //from the given start ID to the nearest intersection ID to the named POI


    return dijkstra_POI(intersect_id_start, point_of_interest_name, turn_penalty);
}
