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
#include "json.h"
#include "graph.h"
#include "GraphRouter.h"

class BusStop;

class Route {
public:
    enum Type {
        Circle,
        Linear
    };
    using BusStops = std::unordered_map<std::string, std::shared_ptr<BusStop>>;
    using Edges = std::vector<Graph::Edge<double>>;

    Route(std::istream& is, BusStops& busStops);

    Route(const Json::Node& node, BusStops& busStops);

    Route(const std::string& name, Type type) : _number(name), _type(type) {}

    const std::string& number() const;

    int stopsCount() const;

    int uniqueStopsCount() const;

    Type type() const;

    size_t size() const;

    void addStop(std::shared_ptr<BusStop>);

    void print(std::ostream& os, bool isJson = false, int id = 0);

    void createEdges(double velocity, GraphRouter& router);

private:
    Type _type = Linear;
    std::string _number;
    double _shortestDistance = 0.;
    double _realDistance = 0.;

    std::vector<std::shared_ptr<BusStop>> _busStops;
    std::unordered_set<std::shared_ptr<BusStop>> _uniqueBusStops;
};

#endif //FINALPROJECT_ROUTES_H
