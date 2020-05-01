#include "streetData.h"

//Constants used to load data structures; and data types
#define HIGHWAY "highway"

//Values for key-value pair to identify major/minor roads
vector<string> PrimaryRoadTags = {"motorway", "trunk", "primary", "motorway_link",
    "trunk_link", "primary_link"};
vector<string> SecondaryRoadTags = {"secondary", "tertiary", "secondary_link",
    "tertiary_link"};
vector<string> TertiaryRoadTags = {"unclassified", "residential", "service",
    "living_street", "pedestrian", "raceway", "road"};


typedef pair<string, string> TagPair;

streetData::streetData() {

    unsigned numWays = getNumberOfWays();

    //For each way get its tag count, tag, and for each tag check if it 
    //has key "highway" then store its corresponding OSMID and value.
    for (unsigned i = 0; i < numWays; i++) {

        const OSMWay* way_i = getWayByIndex(i);

        unsigned numTags = getTagCount(way_i);

        for (unsigned j = 0; j < numTags; j++) {

            TagPair tag = getTagPair(way_i, j);

            if (tag.first == HIGHWAY) {
                streetOSMs[tag.second].push_back(way_i->id());
            }

        }
    }

    //Load primary OSMS
    for (unsigned k = 0; k < PrimaryRoadTags.size(); k++) {
        vector<OSMID> osms = streetOSMs[PrimaryRoadTags[k]];
        primary_OSMS.insert(primary_OSMS.end(), osms.begin(), osms.end());
    }

    //Load secondary OSMS
    for (unsigned m = 0; m < SecondaryRoadTags.size(); m++) {
        vector<OSMID> osms = streetOSMs[SecondaryRoadTags[m]];
        secondary_OSMS.insert(secondary_OSMS.end(), osms.begin(), osms.end());
    }

    //Load tertiary OSMS
    for (unsigned n = 0; n < TertiaryRoadTags.size(); n++) {
        vector<OSMID> osms = streetOSMs[TertiaryRoadTags[n]];
        tertiary_OSMS.insert(tertiary_OSMS.end(), osms.begin(), osms.end());
    }

}

void streetData::loadStreetVectors(multimap<OSMID, id_pts >  & segment_ID_to_pts) {
    /*Using the map of identifier-OSMID; Load the t_point value of the 
     corresponding street segment with the matching OSMID into the relevant 
     vector.*/

    for (unsigned i = 0; i < primary_OSMS.size(); i++) {
        auto range = segment_ID_to_pts.equal_range(primary_OSMS[i]);

        for (auto iter = range.first; iter != range.second; iter++) {
            primaryRoads.push_back((iter->second).second);
            
            //We want to get the stree segment id, and insert the begin and end pt of the street segment into a vector
            primaryName_Parameters.push_back(segIDToEndPts((iter->second).first, (iter->second).second));
        }
    }

    for (unsigned j = 0; j < secondary_OSMS.size(); j++) {
        auto range = segment_ID_to_pts.equal_range(secondary_OSMS[j]);

        for (auto iter = range.first; iter != range.second; iter++) {
            secondaryRoads.push_back((iter->second).second);
            
             //We want to get the street segment id, and insert the begin and end pt of the street into a vector
            secondaryName_Parameters.push_back(segIDToEndPts((iter->second).first, {(iter->second).second.front(), (iter->second).second.back()}));
        }
    }

    for (unsigned k = 0; k < tertiary_OSMS.size(); k++) {
        auto range = segment_ID_to_pts.equal_range(tertiary_OSMS[k]);
        
        for (auto iter = range.first; iter != range.second; iter++) {
            tertiaryRoads.push_back((iter->second).second);
            
             //We want to get the street id, and insert the begin and end pt of the street into a vector
            tertiaryName_Parameters.push_back(segIDToEndPts((iter->second).first, {(iter->second).second.front(), (iter->second).second.back()}));
        }
    }
}

vector<vector<t_point>> streetData::getPrimaryRoads(void) {
    return primaryRoads;
}

vector<vector<t_point>> streetData::getSecondaryRoads(void) {
    return secondaryRoads;
}

vector<vector<t_point>> streetData::getTertiaryRoads(void) {
    return tertiaryRoads;
}

vector<segIDToEndPts> streetData::get_primaryRoadParam(void) {
    return primaryName_Parameters;
    
}
vector<segIDToEndPts> streetData::get_secondaryRoadParam(void){
    return secondaryName_Parameters;
    
}
vector<segIDToEndPts> streetData::get_tertiaryRoadParam(void){
    return tertiaryName_Parameters;
    
}
