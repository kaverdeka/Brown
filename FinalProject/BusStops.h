//
// Created by ka on 21.12.2021.
//

#ifndef FINALPROJECT_STOPBUSES_H
#define FINALPROJECT_STOPBUSES_H
#pragma once

#include <iostream>
#include <string>
#include <set>
#include <memory>

#include "details.h"

class Route;

class BusStop {
public:
    BusStop(std::istream& is);
    BusStop(const BusStop& busStop);
    BusStop(const std::string& name);

    void set(double lat, double lon);

    const std::string& name() const;
    double latitude() const;
    double longitude() const;

    void addRoute(const Route& route);
    void print(std::ostream& os);

private:
    std::string _name;
    double _latitude = 0.;
    double _longitude = 0.;

    std::set<std::string> _routes;
};

#endif //FINALPROJECT_STOPBUSES_H
