/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m4Funclib.h"


vector<unsigned> pickUpLocs;
vector<unsigned> dropOffLocs;
multimap<unsigned, unsigned> pickUp_to_deliveryIndex;

std::vector<unsigned> traveling_courier(const std::vector<DeliveryInfo>& deliveries,
        const std::vector<unsigned>& depots,
        const float turn_penalty) {

    //Utilizes the Greedy Algorithm to determine a sub-optimal delivery route
    unsigned numOfDeliveries = deliveries.size();

    pickUpLocs.clear();
    dropOffLocs.clear();
    pickUp_to_deliveryIndex.clear();


    vector<unsigned> valid_locations; //Holds valid pick-up locations and drop-off locations as items picked up
    vector<unsigned> orderPickUpDropOffsSoFar;


    vector<unsigned> path_so_far;
    vector<unsigned> best_seg_path;
    vector<unsigned> locations;

    //Load in the pick up and drop off locations
    for (unsigned i = 0; i < numOfDeliveries; i++) {
        pickUpLocs.push_back(deliveries[i].pickUp);
        dropOffLocs.push_back(deliveries[i].dropOff);
        pickUp_to_deliveryIndex.insert(make_pair(deliveries[i].pickUp, deliveries[i].dropOff));

        locations.push_back(deliveries[i].pickUp);
        locations.push_back(deliveries[i].dropOff);
    }
    
    for(unsigned v = 0; v < depots.size(); v++) {
        locations.push_back(depots[v]);
    }

    //Pre-compute delivery paths
    set<unsigned> makeUnique(locations.begin(), locations.end());
    locations.assign(makeUnique.begin(), makeUnique.end());
    //preComputeDeliveryPaths(locations, turn_penalty);

    float minCostSoFar = INT_MAX;

    for (unsigned i = 0; i < depots.size(); i++) {
        //Clear the previous path
        deliverySegPath.clear();
        orderPickUpDropOffsSoFar.clear();

        if ((depots[i] == isolated) && (cityName == MAPISO)) continue; //Hard-coded instruction to deal with isolated problem in toronto

        //Remove duplicate pick up locations then assign to valid_locations
        set<unsigned> uniqueSet(pickUpLocs.begin(), pickUpLocs.end());
        valid_locations.assign(uniqueSet.begin(), uniqueSet.end());

        //Get the path from the depot to nearest pick up
        path_so_far = dijkstra_M4(depots[i], valid_locations, turn_penalty);
        unsigned position = path_so_far.back(); //Get location of nearest pick up

        orderPickUpDropOffsSoFar.push_back(depots[i]);
        orderPickUpDropOffsSoFar.push_back(position);

        //Determine the index of the package that was picked up
        auto range_1 = pickUp_to_deliveryIndex.equal_range(position);

        for (auto iter = range_1.first; iter != range_1.second; iter++) {
            valid_locations.push_back(iter->second);
        }

        //Remove this pick-up location and push in the corresponding drop off location
        valid_locations.erase(remove(valid_locations.begin(), valid_locations.end(), position), valid_locations.end());

        //While there are pick-ups and drop-offs keep delivering 
        while (!valid_locations.empty()) {

            //Get the path from position to next nearest pick up or drop off location
            vector<unsigned> sub_path = dijkstra_M4(position, valid_locations, turn_penalty);

            //Append the sub path to the path so far
            path_so_far.insert(path_so_far.end(), sub_path.begin(), sub_path.end());

            position = sub_path.back(); //Get the nearest pick up or drop off location
            orderPickUpDropOffsSoFar.push_back(position);

            //Now we need to check if this is a pick-up or drop-off location
            //Check if this location is in the pick up locations vector and get its index
            auto range = pickUp_to_deliveryIndex.equal_range(position);
            auto index_1 = pickUp_to_deliveryIndex.find(position);

            if (index_1 != pickUp_to_deliveryIndex.end()) {
                //Position is a pick-up location. So remove this pick up location from valid locations and add package drop off
                //to this vector.

                for (auto iter = range.first; iter != range.second; iter++) {
                    valid_locations.push_back(iter->second);
                }

                valid_locations.erase(remove(valid_locations.begin(), valid_locations.end(), position), valid_locations.end());
            } else {
                //Else the position is a drop off location. Remove it.
                valid_locations.erase(remove(valid_locations.begin(), valid_locations.end(), position), valid_locations.end()); //eliminate if it is a drop off
            }
        }

        //find the last delivery position to depot
        unsigned last_position = path_so_far.back();
        vector<unsigned> last_depot = dijkstra_M4(last_position, depots, turn_penalty);
        orderPickUpDropOffsSoFar.push_back(last_depot.back());
        path_so_far.insert(path_so_far.end(), last_depot.begin(), last_depot.end());

        float newCost = compute_path_travel_time(deliverySegPath, turn_penalty);

        if (newCost < minCostSoFar) {
            minCostSoFar = newCost;
            best_seg_path = deliverySegPath;
            orderPickUpDropOff = orderPickUpDropOffsSoFar;
        }
        
        
        
    }

    //    vector<unsigned> segPath_from_prePath;
    //    //Code to check if pre-compute paths is accurate
    //    for(unsigned m = 0; m < best_inter_path.size()-1; m++) {
    //        string key = to_string(best_inter_path[m]) + to_string(best_inter_path[m+1]);
    //        cout << key << flush << endl;
    //        auto path_inter1_to_inter2 = preDeliveryPaths.find(to_string(best_inter_path[m]) + to_string(best_inter_path[m+1]));
    //        segPath_from_prePath.insert(segPath_from_prePath.end(), path_inter1_to_inter2->second.begin(), path_inter1_to_inter2->second.end());
    //    }
    //    
    //    return segPath_from_prePath;
    


    return best_seg_path;
}

void preComputeDeliveryPaths(vector<unsigned> &locations, double turn_penalty) {
    //Clear prior values
    preDeliveryPaths.clear();
#pragma omp parallel for
    for (unsigned k = 0; k < locations.size(); k++) {
        dijkstra_prePaths(locations[k], locations, adjacencyList, turn_penalty);
    }
}

vector<unsigned> two_opt(const float turn_penalty) {

    //Remove start and end depot
    vector<unsigned> delivery_path;
    delivery_path.insert(delivery_path.end(), orderPickUpDropOff.begin() + 1, orderPickUpDropOff.end() - 1);
    
    double bestTravelTime = compute_path_travel_time(delivery_path, turn_penalty);
    
    for (unsigned i = 0; i < delivery_path.size() - 2; i++) {
        for (unsigned j = i+2; j < delivery_path.size(); j++) {

            //Do the swap and check if it is valid
            vector<unsigned> tempIntVec = delivery_path;
            vector<unsigned> tempStreetSeg;

            unsigned tempVal = tempIntVec[i];
            tempIntVec[i] = tempIntVec[j];
            tempIntVec[j] = tempVal;

            //Check if this swapped path is legal
            if (checkIfPathLegal(tempIntVec)) {
            
                for(unsigned g = 0; g < tempIntVec.size()-1; g++) {
                    string key = to_string(tempIntVec[g]) + to_string(tempIntVec[g + 1]);
                    vector<unsigned> pathSub = preDeliveryPaths.find(key)->second;
                    tempStreetSeg.insert(tempStreetSeg.end(), pathSub.begin(), pathSub.end());
                }
                
                
                double newCost = compute_path_travel_time(tempStreetSeg, turn_penalty);
                
                if (newCost < bestTravelTime) {
                    //if(courier_path_is_legal(deliveries, depots, ))
                    bestTravelTime = newCost;
                    delivery_path = tempIntVec;
                }
            }
        }

    }

    vector<unsigned> streetSegPath;
    delivery_path.insert(delivery_path.begin(), orderPickUpDropOff[0]);
    delivery_path.insert(delivery_path.end(), orderPickUpDropOff.back());
    //Trace the best path
    for (unsigned n = 0; n < delivery_path.size() - 1; n++) {
        vector<unsigned> subPath = preDeliveryPaths.find(to_string(delivery_path[n]) + to_string(delivery_path[n + 1]))->second;
        streetSegPath.insert(streetSegPath.end(), subPath.begin(), subPath.end());
    }
    
    return streetSegPath;


    //for(unsigned i=0; i < )

    //    //make a permutation for the path
    //    //deliverySegPath
    //    //vec.erase(std::remove(vec.begin(), vec.end(), number_in), vec.end());

    //    //the beginning and the end of this vector are depots and depots order can not be changed. So delete them from the vector for calculating permutation

    //    sort(delivery_path.begin(), delivery_path.end());
    //    unsigned size = delivery_path.size();
    //    unordered_map<int,vector<unsigned>>permutation_path;//index is integer and each index is a vector of possible path
    //    
    //    //boost::math::factorial(2);
    //    unsigned factorial_size = boost::math::factorial(size);
    //    do {
    //        for(unsigend i=0; i< factorial_size; i++ ){
    //            permutation_path[i].push_back();
    //        }
    //    } while(next_permutation(s.begin(), s.end()));
    //   
    //   //bool courier_path_is_legal(const std::vector<DeliveryInfo>& deliveries,
    //                          //const std::vector<unsigned>& depots,
    //                          //const std::vector<unsigned>& path);
    //   //check if valid
    //   bool if_valid = courier_path_is_legal(deliveries,depots,v);

}

bool checkIfPathLegal(vector<unsigned> path) {

    for (unsigned i = 0; i < pickUpLocs.size(); i++) {
        int  pickUp = find(path.begin(), path.end(), pickUpLocs[i]) - path.begin();

        auto rangeDropOffs = pickUp_to_deliveryIndex.equal_range(pickUpLocs[i]);

        for (auto iter = rangeDropOffs.first; iter != rangeDropOffs.second; iter++) {
            
            int dropOffVal = iter->second;
            
            int pos = find(path.begin(), path.end(), dropOffVal) - path.begin();
            
            if (pickUp > pos) return false;
        }
    }
    return true;
}

//  //Determine the index of the package that was picked up
//        auto range_1 = pickUp_to_deliveryIndex.equal_range(position);
//
//        for (auto iter = range_1.first; iter != range_1.second; iter++) {
//            valid_locations.push_back(deliveries[iter->second].dropOff);
//        }