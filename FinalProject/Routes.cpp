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
        std::string busNumber(name);
        if(busStops.count(busNumber) > 0) {
            auto busStop = busStops[busNumber];
            busStop->addRoute(*this);
            addStop(busStops[busNumber]);
        } else {
            auto busStop = std::make_shared<BusStop>(busNumber);
            addStop(busStop);
            busStop->addRoute(*this);
            busStops[busNumber] = move(busStop);
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

void Route::print(std::ostream& os) {
    if (_distance == 0.0) {
        for (size_t i = 0; i < _busStops.size() - 1; ++i) {
            auto busStop1 = _busStops[i];
            auto busStop2 = _busStops[i + 1];
            _distance += details::calculateDistance(busStop1->latitude(), busStop1->longitude(),
                                                    busStop2->latitude(), busStop2->longitude());
        }

        if (_type == Linear)
            _distance *= 2;
    }

    os << stopsCount() << " stops on route, "
       << uniqueStopsCount() << " unique stops, "
       << _distance << " route length";
}