//
// Created by ka on 21.12.2021.
//

#include "Routes.h"

#include <memory>
#include "BusStops.h"

Route::Route(std::istream& is, BusStops& busStops) {
    getline(is, _number, ':');
    is.ignore(1); // ' '
    std::string line;
    getline(is, line);
    std::string delim;
    if (line.find('>') != std::string::npos) { // circle
        _type = Circle;
        delim = " > ";
    } else { // linear
        _type = Linear;
        delim = " - ";
    }
    std::string_view name;
    std::string_view l = line;

    while(name = details::ReadToken(l, delim), name != "") {
        std::string stopName(name);
        if(busStops.count(stopName) > 0) {
            auto busStop = busStops[stopName];
            busStop->addRoute(*this);
            addStop(busStops[stopName]);
        } else {
            auto busStop = std::make_shared<BusStop>(stopName);
            addStop(busStop);
            busStop->addRoute(*this);
            busStops[stopName] = move(busStop);
        }
    }
}

Route::Route(const Json::Node& node, BusStops& busStops) {
    auto query = node.AsMap();
    _number = query.at("name").AsString();
    bool isRound = query.at("is_roundtrip").AsBool();
    _type = static_cast<Route::Type>(!isRound);
    auto stops = query.at("stops").AsArray();
    for (const auto &stop: stops) {
        std::string stopName = stop.AsString();

        if (busStops.count(stopName) > 0) {
            auto busStop = busStops[stopName];
            busStop->addRoute(*this);
            addStop(busStops[stopName]);
        } else {
            auto busStop = std::make_shared<BusStop>(stopName);
            addStop(busStop);
            busStop->addRoute(*this);
            busStops[stopName] = move(busStop);
        }
    }
}

const std::string& Route::number() const { return _number;}

int Route::stopsCount() const {
    if(_type == Circle) {
        return size();
    } else {
        return size() * 2 - 1;
    }
}

int Route::uniqueStopsCount() const {
    return _uniqueBusStops.size();
}

Route::Type Route::type() const {return _type;}
size_t Route::size() const {return _busStops.size();}

void Route::addStop(std::shared_ptr<BusStop> busStop) {
    _busStops.push_back(busStop);
    _uniqueBusStops.insert(_busStops.back());
}

double calculateShortDistance(const BusStop& busStop1, const BusStop& busStop2) {

    return details::calculateDistance(busStop1.latitude(), busStop1.longitude(),
                                                    busStop2.latitude(), busStop2.longitude());
}

double calculateRealDistance(const BusStop& busStop1, const BusStop& busStop2) {

    if(busStop1.distances().count(busStop2.name()) > 0)
        return busStop1.distances().at(busStop2.name());

    if(busStop2.distances().count(busStop1.name()) > 0)
        return busStop2.distances().at(busStop1.name());

    return 0.0;
}

void Route::print(std::ostream& os, bool isJson, int id) {
    if (_shortestDistance == 0.0) {
        for (size_t i = 0; i < _busStops.size() - 1; ++i) {
            const auto& busStop1 = *_busStops[i];
            const auto& busStop2 = *_busStops[i + 1];

            _shortestDistance += calculateShortDistance(busStop1, busStop2);
            _realDistance += calculateRealDistance(busStop1, busStop2);

            if(_type == Linear) {
                _realDistance += calculateRealDistance(busStop2, busStop1);
            }
        }

        if (_type == Linear) {
            _shortestDistance *= 2;
        }
    }

    if(!isJson) {
        os << stopsCount() << " stops on route, "
           << uniqueStopsCount() << " unique stops, "
           << _realDistance << " route length, "
           << _realDistance / _shortestDistance << " curvature";
    } else {

        os << "\"route_length\": " << _realDistance << ", "
            <<"\"request_id\": " << id << ", "
            << "\"curvature\": " << _realDistance / _shortestDistance << ", "
            << "\"stop_count\": " << stopsCount()<< ", "
            << "\"unique_stop_count\": " << uniqueStopsCount();
    }

}

void Route::createEdges(double velocity, GraphRouter& router) {

    for (size_t i = 0; i < _busStops.size() - 1; ++i) {
        const auto& busStop1 = *_busStops[i];
        const auto& busStop2 = *_busStops[i + 1];

        if(_type == Circle && i == _busStops.size() - 2) {
            router.addEdge(busStop1.name(), _number, busStop2.name(), _number,
                           calculateRealDistance(busStop1, busStop2) / velocity, "End");
            return;
        }

        router.addEdge(busStop1.name(), _number, busStop2.name(), _number,
                       calculateRealDistance(busStop1, busStop2) / velocity);

        if(_type == Linear) {
            router.addEdge(busStop2.name(), _number, busStop1.name(), _number,
                           calculateRealDistance(busStop2, busStop1) / velocity);
        }
    }
}