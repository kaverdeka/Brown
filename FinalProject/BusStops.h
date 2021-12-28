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
#include <unordered_map>

#include "details.h"
#include "json.h"
#include "graph.h"
#include "GraphRouter.h"

class Route;

class BusStop {
public:
    using Distances = std::unordered_map<std::string, double>;
    using Edges = std::vector<Graph::Edge<double>>;

    BusStop(std::istream& is);
    BusStop(const Json::Node& node);
    BusStop(const BusStop& busStop);
    BusStop(const std::string& name);

    void set(double lat, double lon, const Distances& distances);

    const std::string& name() const;
    double latitude() const;
    double longitude() const;

    void addRoute(const Route& route);
    void print(std::ostream& os, bool isJson = false, int id = 0);

    const Distances& distances() const { return _distances; }
    //void setDistance(const std::string& name, double distance);
    const size_t routesCount() const { return _routes.size(); }

    void createEdges(double waitingTime, GraphRouter& router);

private:
    std::string _name;
    double _latitude = 0.;
    double _longitude = 0.;

    std::set<std::string> _routes;
    Distances _distances;
};

#endif //FINALPROJECT_STOPBUSES_H
