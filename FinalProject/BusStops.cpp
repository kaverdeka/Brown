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
}

BusStop::BusStop(const BusStop& busStop) {
    _name = busStop._name;
    _latitude = busStop._latitude;
    _longitude = busStop._longitude;
    _routes = busStop._routes;
}

BusStop::BusStop(const std::string& name) :
    _name(name)
{
}

void BusStop::set(double lat, double lon) {
    _latitude = lat;
    _longitude = lon;
}

const std::string& BusStop::name() const {return _name;}
double BusStop::latitude() const {return _latitude;}
double BusStop::longitude() const {return _longitude;}

void BusStop::addRoute(const Route& route) {
    _routes.insert(route.number());
}

void BusStop::print(std::ostream& os) {
    if(_routes.empty()) {
        os << "no buses";
        return;
    }
    os << "buses";
    for(auto route : _routes) {
        os << " " << route;
    }
}

