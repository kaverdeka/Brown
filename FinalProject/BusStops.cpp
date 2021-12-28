//
// Created by ka on 21.12.2021.
//

#include "BusStops.h"

//
// Created by ka on 21.12.2021.
//

#include <iostream>
#include <string>
#include <set>
#include <memory>
#include <algorithm>

#include "details.h"
#include "Routes.h"


BusStop::BusStop(std::istream& is) {
    getline(is, _name, ':');
    is.ignore(1); // ' '
    is >> _latitude;
    is.ignore(2);
    is >> _longitude;
    is.ignore(2);

    std::string line;
    //is >> line;
    getline(is, line);
    std::string_view str = line;

    while(true) {
        //auto res = details::ReadToken(str, ", ");
        auto distance = details::ReadToken(str, " to ");
        if (distance.empty())
            return;

        auto name = details::ReadToken(str, ", ");
        int dist = std::stoi(std::string(distance));
        _distances[std::string(name)] = dist;
    }
}

BusStop::BusStop(const Json::Node& node) {
    auto query = node.AsMap();

    _name = query.at("name").AsString();
    _longitude = query.at("longitude").AsDouble();
    _latitude= query.at("latitude").AsDouble();
    auto stops = query.at("road_distances").AsMap();
    for (const auto&[name, length]: stops) {
        _distances[name] = length.AsInt();
    }
}

BusStop::BusStop(const BusStop& busStop) {
    _name = busStop._name;
    _latitude = busStop._latitude;
    _longitude = busStop._longitude;
    _routes = busStop._routes;
    _distances = busStop._distances;
}

BusStop::BusStop(const std::string& name) :
    _name(name)
{
}

void BusStop::set(double lat, double lon, const Distances& distances) {
    _latitude = lat;
    _longitude = lon;
    _distances = distances;
}

const std::string& BusStop::name() const {return _name;}
double BusStop::latitude() const {return _latitude;}
double BusStop::longitude() const {return _longitude;}

void BusStop::addRoute(const Route& route) {
    _routes.insert(route.number());
}

void BusStop::print(std::ostream& os, bool isJson, int id) {
    if (!isJson) {
        if (_routes.empty()) {
            os << "no buses";
            return;
        }
        os << "buses";
        for (auto route: _routes) {
            os << " " << route;
        }
    } else {
        os << "\"buses\": [";

        bool isFirst = true;
        for (auto route: _routes) {
            if(!isFirst)
                os << ", ";
            os << "\"" << route << "\"";
            isFirst = false;
        }

        os << "], " << "\"request_id\": " << id;
    }
}

void BusStop::createEdges(double waitingTime, GraphRouter& router) {

//    std::string type = "Wait";
//    router.addEdge(_name + "_wait", "", _name, "", waitingTime, type);

//    std::vector<std::string> routes(_routes.begin(), _routes.end());
//    if (routes.empty())
//        return;
//
//    std::string type = "Wait";
//    for(size_t i = 0; i < routes.size()-1; ++i) {
//        router.addEdge(_name, std::string(""), _name, routes[i], waitingTime, type);
//        router.addEdge(_name, routes[i], _name, std::string(""), 0, type);
//
//        for(size_t j = i + 1; j < routes.size(); ++j) {
//            router.addEdge(_name, routes[i], _name, routes[j], waitingTime, type);
//            router.addEdge(_name, routes[j], _name, routes[i], waitingTime, type);
//        }
//    }
//    // for last
//    router.addEdge(_name, std::string(""), _name, routes[routes.size()-1], waitingTime, type);
//    router.addEdge(_name, routes[routes.size()-1], _name, std::string(""), 0., type);
}
