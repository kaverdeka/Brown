//
// Created by ka on 21.12.2021.
//

#ifndef FINALPROJECT_ROUTES_H
#define FINALPROJECT_ROUTES_H
#pragma once

#include <string>
#include <string_view>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "details.h"
#include <memory>

class BusStop;

class Route {
public:
    enum Type {
        Circle,
        Linear
    };
    using BusStops = std::unordered_map<std::string, std::shared_ptr<BusStop>>;

    Route(std::istream& is, BusStops& busStops);

    const std::string& number() const;

    int stopsCount() const;

    int uniqueStopsCount() const;

    Type type() const;

    size_t size() const;

    void addStop(std::shared_ptr<BusStop>);

    void print(std::ostream& os);

private:
    Type _type = Linear;
    std::string _number;
    double _distance = 0.;

    std::vector<std::shared_ptr<BusStop>> _busStops;
    std::unordered_set<std::shared_ptr<BusStop>> _uniqueBusStops;
};

#endif //FINALPROJECT_ROUTES_H
