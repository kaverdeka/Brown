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
                os << ",";
            os << "\n\"" << route << "\"";
            isFirst = false;
        }

        os << "],\n" << "\"request_id\": " << id;
    }
}

//void BusStop::setDistance(const std::string& name, double distance) {
//    if(_distances.count(name) < 1) {
//        _distances[name] = distance;
//    }
//}

