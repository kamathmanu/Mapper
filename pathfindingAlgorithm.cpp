/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "pathfindingAlgorithm.h"

std::vector<unsigned> aStar(const unsigned source, const unsigned target, const double turn_penalty) {
    //Utilizes the A* algorithm to determine the most optimal path from source to target.

    // auto const start = chrono::high_resolution_clock::now();

    //Reinitialize graph
    for (unsigned i = 0; i < adjacencyList.size(); i++) {
        adjacencyList[i].cost = INFINITE;
        adjacencyList[i].reachingEdgeID = UNDEFINED_EDGE;
        adjacencyList[i].visited = false;
    }
    
    vector<unsigned> path;
    vector<Fibonacci_Queue::handle_type>handles(adjacencyList.size());
    vector<bool> has_handle(adjacencyList.size(), false);
    //Get the LatLon position of target; Used to compute Heuristic
    LatLon finalPos = intersectionPos[target];

    //Create a heap to store our intersections
    Fibonacci_Queue activeVertices;

    //Set distance from source to be 0; Push source into heap; Set its reaching edge id to specify it is non-existent
    adjacencyList[source].cost = 0;
    adjacencyList[source].reachingEdgeID = SOURCE_NO_EDGE;
    handles[source] = (activeVertices.push(VectorCost(source, 0)));
    has_handle[source] = true;
    
    unsigned count = 0;

    //While the heap is not empty keep searching for a path
    while (!activeVertices.empty()) {

        //Get intersection ID with minimum distance from source
        unsigned vertID = activeVertices.top().first;
        
        cout << count << endl;
        
        count++; //Count number of nodes visited
        
        if (vertID == target) break; //We have found the path

        activeVertices.pop(); //Remove most top element

        if (adjacencyList[vertID].visited) continue; //Don't need to visit its neighbours


        adjacencyList[vertID].visited = true; //mark as visited


        //Visit each neighbour and calculate its cost
        for (unsigned i = 0; i < adjacencyList[vertID].edges.size(); i++) {
            unsigned nextVertID = adjacencyList[vertID].edges[i].nextVertex;


            //Cost is defined as cost to vertex + cost from vertex to neighbour vertex
            float newCost = streetSeg_travelTime[adjacencyList[vertID].edges[i].segID] + adjacencyList[vertID].cost;

            //Determine if we need to add turn penalty: occurs if we change streets
            if (adjacencyList[vertID].reachingEdgeID != SOURCE_NO_EDGE) {
                if (streetSegIDToStreetID[adjacencyList[vertID].reachingEdgeID] != streetSegIDToStreetID[adjacencyList[vertID].edges[i].segID])
                    newCost += turn_penalty;
            }

            //Now check if we should update the neighbours cost and add it to the heap
            if (adjacencyList[nextVertID].cost > newCost) {
                //Compute heurisitic 
                float hCost = find_distance_between_two_points(intersectionPos[nextVertID], finalPos) / maxSpeed * KMH_TO_MS;

                adjacencyList[nextVertID].cost = newCost;
                adjacencyList[nextVertID].reachingEdgeID = adjacencyList[vertID].edges[i].segID;

                if (!has_handle[nextVertID]) {
                    has_handle[nextVertID] = true;
                    handles[nextVertID] = activeVertices.push(VectorCost(nextVertID, adjacencyList[nextVertID].cost + hCost));
                } else {
                    activeVertices.erase(handles[nextVertID]);
                    handles[nextVertID] = activeVertices.push(VectorCost(nextVertID, adjacencyList[nextVertID].cost + hCost));
                }
            }
        }
    }

    unsigned current = target;
    while (adjacencyList[current].reachingEdgeID != SOURCE_NO_EDGE && adjacencyList[current].reachingEdgeID != UNDEFINED_EDGE) {
        path.push_back(adjacencyList[current].reachingEdgeID);

        StreetSegmentInfo streetSeg = getStreetSegmentInfo(adjacencyList[current].reachingEdgeID);

        current = current == streetSeg.from ? streetSeg.to : streetSeg.from;
    }

    reverse(path.begin(), path.end());


    return path;

}

std::vector<unsigned> dijkstra_POI(const unsigned source,
        const std::string point_of_interest_name, const double turn_penalty) {
    //Uses Dijkstra algorithm to find nearest intersection to POI

    //Reinitialize graph
    for (unsigned i = 0; i < adjacencyList.size(); i++) {
        adjacencyList[i].cost = INFINITE;
        adjacencyList[i].reachingEdgeID = UNDEFINED_EDGE;
        adjacencyList[i].visited = false;
    }

    vector<unsigned> path;
    vector<Fibonacci_Queue::handle_type>handles(adjacencyList.size());
    vector<bool> has_handle(adjacencyList.size(), false);

    //Get final pos of the intersection
    unsigned endID = source;

    //Create a heap to store our intersections
    Fibonacci_Queue activeVertices;

    //Set distance from source to be 0; Push source into heap; Set its reaching edge id to specify it is non-existent
    adjacencyList[source].cost = 0;
    adjacencyList[source].reachingEdgeID = SOURCE_NO_EDGE;
    handles[source] = (activeVertices.push(VectorCost(source, 0)));
    has_handle[source] = true;

    //First get all the possible POI points associated with the name
    vector<unsigned> nearestIntersections;

    for (unsigned j = 0; j < interests.size(); j++) {
        if (interests[j].interestName == point_of_interest_name)
            nearestIntersections.push_back(find_closest_intersection(interests[j].interestPositionLatLon));
    }

    //While the heap is not empty keep searching for a path
    while (!activeVertices.empty()) {

        //Get intersection ID with minimum distance from source
        unsigned vertID = activeVertices.top().first;

        if (find(nearestIntersections.begin(), nearestIntersections.end(), vertID) != nearestIntersections.end()) {
            endID = vertID;
            break; //We have found the path
        }

        activeVertices.pop(); //Remove most top element

        if (adjacencyList[vertID].visited) continue; //Don't need to visit its neighbours


        adjacencyList[vertID].visited = true; //mark as visited

        //Visit each neighbour and calculate its cost
        for (unsigned i = 0; i < adjacencyList[vertID].edges.size(); i++) {
            unsigned nextVertID = adjacencyList[vertID].edges[i].nextVertex;


            //Cost is defined as cost to vertex + cost from vertex to neighbour vertex
            float newCost = streetSeg_travelTime[adjacencyList[vertID].edges[i].segID] + adjacencyList[vertID].cost;

            //Determine if we need to add turn penalty: occurs if we change streets
            if (adjacencyList[vertID].reachingEdgeID != SOURCE_NO_EDGE) {
                if (streetSegIDToStreetID[adjacencyList[vertID].reachingEdgeID] != streetSegIDToStreetID[adjacencyList[vertID].edges[i].segID])
                    newCost += turn_penalty;
            }

            //Now check if we should update the neighbours cost and add it to the heap
            if (adjacencyList[nextVertID].cost > newCost) {
                //Compute heurisitic 

                adjacencyList[nextVertID].cost = newCost;
                adjacencyList[nextVertID].reachingEdgeID = adjacencyList[vertID].edges[i].segID;

                if (!has_handle[nextVertID]) {
                    has_handle[nextVertID] = true;
                    handles[nextVertID] = activeVertices.push(VectorCost(nextVertID, adjacencyList[nextVertID].cost));
                } else {
                    activeVertices.erase(handles[nextVertID]);
                    handles[nextVertID] = activeVertices.push(VectorCost(nextVertID, adjacencyList[nextVertID].cost));
                }
            }
        }
    }

    unsigned current = endID;
    while (adjacencyList[current].reachingEdgeID != SOURCE_NO_EDGE && adjacencyList[current].reachingEdgeID != UNDEFINED_EDGE) {
        path.push_back(adjacencyList[current].reachingEdgeID);

        StreetSegmentInfo streetSeg = getStreetSegmentInfo(adjacencyList[current].reachingEdgeID);

        current = current == streetSeg.from ? streetSeg.to : streetSeg.from;
    }

    reverse(path.begin(), path.end());

    return path;
}

std::vector<unsigned> dijkstra_M4(const unsigned source, const vector<unsigned>& target, const double turn_penalty) {
    //Finds the path to the nearest pick up or drop off location

    //Reinitialize graph
    for (unsigned i = 0; i < adjacencyList.size(); i++) {
        adjacencyList[i].cost = INFINITE;
        adjacencyList[i].reachingEdgeID = UNDEFINED_EDGE;
        adjacencyList[i].visited = false;
    }

    vector<Fibonacci_Queue::handle_type>handles(adjacencyList.size());
    vector<bool> has_handle(adjacencyList.size(), false);

    //Get final pos of the intersection
    unsigned endID = source;

    //Create a heap to store our intersections
    Fibonacci_Queue activeVertices;

    //Set distance from source to be 0; Push source into heap; Set its reaching edge id to specify it is non-existent
    adjacencyList[source].cost = 0;
    adjacencyList[source].reachingEdgeID = SOURCE_NO_EDGE;
    handles[source] = (activeVertices.push(VectorCost(source, 0)));
    has_handle[source] = true;

    //While the heap is not empty keep searching for a path
    while (!activeVertices.empty()) {

        //Get intersection ID with minimum distance from source
        unsigned vertID = activeVertices.top().first;

        if (find(target.begin(), target.end(), vertID) != target.end()) {
            endID = vertID;
            break; //We have found the path
        }

        activeVertices.pop(); //Remove most top element

        if (adjacencyList[vertID].visited) continue; //Don't need to visit its neighbours

        adjacencyList[vertID].visited = true; //mark as visited

        //Visit each neighbour and calculate its cost
        for (unsigned i = 0; i < adjacencyList[vertID].edges.size(); i++) {
            unsigned nextVertID = adjacencyList[vertID].edges[i].nextVertex;


            //Cost is defined as cost to vertex + cost from vertex to neighbour vertex
            float newCost = streetSeg_travelTime[adjacencyList[vertID].edges[i].segID] + adjacencyList[vertID].cost;

            //Determine if we need to add turn penalty: occurs if we change streets
            if (adjacencyList[vertID].reachingEdgeID != SOURCE_NO_EDGE) {
                if (streetSegIDToStreetID[adjacencyList[vertID].reachingEdgeID] != streetSegIDToStreetID[adjacencyList[vertID].edges[i].segID])
                    newCost += turn_penalty;
            }

            //Now check if we should update the neighbours cost and add it to the heap
            if (adjacencyList[nextVertID].cost > newCost) {
                //Compute heurisitic 

                adjacencyList[nextVertID].cost = newCost;
                adjacencyList[nextVertID].reachingEdgeID = adjacencyList[vertID].edges[i].segID;

                if (!has_handle[nextVertID]) {
                    has_handle[nextVertID] = true;
                    handles[nextVertID] = activeVertices.push(VectorCost(nextVertID, adjacencyList[nextVertID].cost));
                } else {
                    activeVertices.erase(handles[nextVertID]);
                    handles[nextVertID] = activeVertices.push(VectorCost(nextVertID, adjacencyList[nextVertID].cost));
                }
            }
        }
    }

    return traceBackPath(endID, source);
}

std::vector<unsigned> traceBackPath(unsigned targetID, unsigned sourceID) {
    //Given a set of intersection ID's provides a set of street segment ID's that form the path

    vector<unsigned> path;
    vector<unsigned> pathSegs;

    unsigned current = targetID;
    while (adjacencyList[current].reachingEdgeID != SOURCE_NO_EDGE && adjacencyList[current].reachingEdgeID != UNDEFINED_EDGE) {
        pathSegs.push_back(adjacencyList[current].reachingEdgeID);
        path.push_back(current);

        StreetSegmentInfo streetSeg = getStreetSegmentInfo(adjacencyList[current].reachingEdgeID);

        current = current == streetSeg.from ? streetSeg.to : streetSeg.from;
    }

    reverse(pathSegs.begin(), pathSegs.end());
    deliverySegPath.insert(deliverySegPath.end(), pathSegs.begin(), pathSegs.end());

    path.push_back(sourceID);
    reverse(path.begin(), path.end());

    return path;
}

void dijkstra_prePaths(const unsigned source, const vector<unsigned> locations, 
        vector<Vertex> adjList, const double turn_penalty) {
    //Takes a set of points and finds the paths between all of them
    //Stores the results into a unordered map

    unsigned limit = locations.size();
    unsigned counter = 0;
    
    //Reinitialize graph
    for (unsigned j = 0; j < adjList.size(); j++) {
        adjList[j].cost = INFINITE;
        adjList[j].reachingEdgeID = UNDEFINED_EDGE;
        adjList[j].visited = false;
    }

    vector<Fibonacci_Queue::handle_type>handles(adjList.size());
    vector<bool> has_handle(adjList.size(), false);

    //Create a heap to store our intersections
    Fibonacci_Queue activeVertices;

    //Set distance from source to be 0; Push source into heap; Set its reaching edge id to specify it is non-existent
    adjList[source].cost = 0;
    adjList[source].reachingEdgeID = SOURCE_NO_EDGE;
    handles[source] = (activeVertices.push(VectorCost(source, 0)));
    has_handle[source] = true;

    //While the heap is not empty keep searching for a path
    while (!activeVertices.empty()) {

        //Get intersection ID with minimum distance from source
        unsigned vertID = activeVertices.top().first;

        if (find(locations.begin(), locations.end(), vertID) != locations.end()) {

            if (vertID != source) {
                string key = to_string(source) + to_string(vertID);
                preDeliveryPaths.insert(make_pair(key, traceBackPathPrePath(vertID, adjList)));
                counter++;
            }
        }
        
                if (counter >= limit)
            return;

        activeVertices.pop(); //Remove most top element

        if (adjList[vertID].visited) continue; //Don't need to visit its neighbours

        adjList[vertID].visited = true; //mark as visited

        //Visit each neighbour and calculate its cost
        for (unsigned i = 0; i < adjList[vertID].edges.size(); i++) {
            unsigned nextVertID = adjList[vertID].edges[i].nextVertex;

            //Cost is defined as cost to vertex + cost from vertex to neighbour vertex
            float newCost = streetSeg_travelTime[adjList[vertID].edges[i].segID] + adjList[vertID].cost;

            //Determine if we need to add turn penalty: occurs if we change streets
            if (adjList[vertID].reachingEdgeID != SOURCE_NO_EDGE) {
                if (streetSegIDToStreetID[adjList[vertID].reachingEdgeID] != streetSegIDToStreetID[adjList[vertID].edges[i].segID])
                    newCost += turn_penalty;
            }

            //Now check if we should update the neighbours cost and add it to the heap
            if (adjList[nextVertID].cost > newCost) {
                //Compute heurisitic 

                adjList[nextVertID].cost = newCost;
                adjList[nextVertID].reachingEdgeID = adjList[vertID].edges[i].segID;

                if (!has_handle[nextVertID]) {
                    has_handle[nextVertID] = true;
                    handles[nextVertID] = activeVertices.push(VectorCost(nextVertID, adjList[nextVertID].cost));
                } else {
                    activeVertices.erase(handles[nextVertID]);
                    handles[nextVertID] = activeVertices.push(VectorCost(nextVertID, adjList[nextVertID].cost));
                }
            }
        }
    }
}

std::vector<unsigned> traceBackPathPrePath(unsigned targetID, vector<Vertex> &adjList) {
    //Given a set of intersection ID's provides a set of street segment ID's that form the path
    vector<unsigned> pathSegs;

    unsigned current = targetID;
    while (adjList[current].reachingEdgeID != SOURCE_NO_EDGE && adjList[current].reachingEdgeID != UNDEFINED_EDGE) {
        pathSegs.push_back(adjList[current].reachingEdgeID);

        StreetSegmentInfo streetSeg = getStreetSegmentInfo(adjList[current].reachingEdgeID);

        current = current == streetSeg.from ? streetSeg.to : streetSeg.from;
    }

    reverse(pathSegs.begin(), pathSegs.end());

    return pathSegs;
}
