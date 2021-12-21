#include <string>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <list>
#include <deque>
#include <cmath>
#include <algorithm>

#include "test_runner.h"

using namespace std;

//************************
pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter = " ") {
    const size_t pos = s.find(delimiter);
    if (pos == s.npos) {
        return {s, nullopt};
    } else {
        return {s.substr(0, pos), s.substr(pos + delimiter.length())};
    }
}

pair<string_view, string_view> SplitTwo(string_view s, string_view delimiter = " ") {
    const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
    return {lhs, rhs_opt.value_or("")};
}

string_view ReadToken(string_view& s, string_view delimiter = " ") {
    const auto [lhs, rhs] = SplitTwo(s, delimiter);
    s = rhs;
    return lhs;
}

//********************************
//a = sin²(Δφ/2) + cos φ1 ⋅ cos φ2 ⋅ sin²(Δλ/2)
//c = 2 ⋅ atan2( √a, √(1−a) )
//d = R ⋅ c

static constexpr double pi = 3.1415926535;
static constexpr double r = 6371;

double deg2rad(double deg) {
    return deg * (pi/180.);
}

double calculateDistance(double lat1, double lon1, double lat2, double lon2) {

    double dLat = deg2rad(lat2-lat1);
    double dLon = deg2rad(lon2-lon1);
    double a =
            sin(dLat/2) * sin(dLat/2) +
            cos(deg2rad(lat1)) * cos(deg2rad(lat2)) *
            sin(dLon/2) * sin(dLon/2);

    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    double d = r * c * 1000; // Distance in km
    return d;
}
//********************************
class BusStop {
public:
    BusStop(const string& name, double latitude, double longitude) :
        _name(name),
        _latitude(latitude),
        _longitude(longitude)
        {}
    friend istream& operator >>(istream& is, BusStop& busStop);

    BusStop() = default;

    const string& name() const {return _name;}
    double latitude() const {return _latitude;}
    double longitude() const {return _longitude;}

private:
    string _name;
    double _latitude;
    double _longitude;
};

istream& operator >>(istream& is, BusStop& busStop) {
    getline(is, busStop._name, ':');
    is.ignore(1); // ' '
    is >> busStop._latitude;
    is.ignore(2);
    is >> busStop._longitude;
    return is;
};

class Route {
public:
    enum Type {
        Circle,
        Linear
    };

    string number() const { return _number;}

    int stopsCount() const {
        if(_type == Circle) {
            return size();
        } else {
            return size() * 2 - 1;
        }
    }

    int uniqueStopsCount() const {
        return _uniqueBusStops.size();
    }

    Type type() const {return _type;}
    size_t size() const {return _busStops.size();}

    void addStop(string_view busStop) {
        _busStops.push_back(string(busStop));
        _uniqueBusStops.insert(_busStops.back());
    }

    void print(unordered_map<string, BusStop>& busStops, ostream& os) {
        if (_distance == 0.0) {
            for (size_t i = 0; i < _busStops.size() - 1; ++i) {
                auto &busStop1 = busStops.at(_busStops[i]);
                auto &busStop2 = busStops.at(_busStops[i + 1]);
                _distance += calculateDistance(busStop1.latitude(), busStop1.longitude(), busStop2.latitude(),
                                              busStop2.longitude());
            }

            if (_type == Linear)
                _distance *= 2;
        }

        os << stopsCount() << " stops on route, "
        << uniqueStopsCount() << " unique stops, "
        << _distance << " route length";
    }

    friend istream& operator >>(istream& is, Route& route);

private:
    Type _type = Linear;
    string _number;
    vector<string> _busStops;
    unordered_set<string> _uniqueBusStops;

    double _distance = 0.;
};

istream& operator >>(istream& is, Route& route) {
    getline(is, route._number, ':');
    is.ignore(1); // ' '
    string line;
    getline(is, line);
    string delim;
    if (line.find('>') != string::npos) { // circle
        route._type = route.Circle;
        delim = " > ";
    } else { // linear
        route._type = route.Linear;
        delim = " - ";
    }
    string_view name;
    string_view l = line;
    while(name = ReadToken(l, delim), name != "") {
        route.addStop(name);
    }

    return is;
};

enum QueryType {
    Stop,
    Bus
};

istream& operator>> (istream& is, QueryType& type) {
    string word;
    //is >> word;
    getline(is, word, ' ');
    if(word == "Stop") {
        type = QueryType::Stop;
    } else if (word == "Bus") {
        type = QueryType::Bus;
    }
    return is;
}

class DB {
public:
    void init(istream& is) {
        size_t queryCount;
        is >> queryCount;

        string str;
        getline(is, str);

        for(size_t i = 0; i < queryCount; ++i) {
            string line;
            getline(is, line);
            stringstream ss(line);
            QueryType type;
            ss >> type;
            switch (type) {
                case Stop: {
                    BusStop busStop;
                    ss >> busStop;
                    _busStops[busStop.name()] = move(busStop);
                    break;
                }
                case Bus: {
                    Route route;
                    ss >> route;
                    _routes[route.number()] = move(route);
                    break;
                }
                default:
                    throw std::invalid_argument("");
            }
        }
    }

    void run (istream& is, ostream& os) {
        size_t queryCount;
        is >> queryCount;

        string str;
        getline(is, str);

        for(size_t i = 0; i < queryCount; ++i) {
            string line;
            getline(is, line);
            stringstream ss(line);
            QueryType type;
            ss >> type;
            switch (type) {
                case Bus: {
                    string number;
                    getline(ss, number, ':');
                    os << "Bus " << number << ": ";
                    if (_routes.count(number) > 0) {
                        _routes.at(number).print(_busStops, os);
                        os << "\n";

                    } else {
                        os << "not found\n";
                    }
                    break;
                }
            }
        }
    }
private:
    unordered_map<string, BusStop> _busStops;
    unordered_map<string, Route> _routes;
};



void BusStopTest() {
    string line = "Biryulyovo Zapadnoye: 12.12, 13.13";
    stringstream ss(line);
    BusStop busStop;
    ss >> busStop;
    ASSERT_EQUAL(busStop.name(), "Biryulyovo Zapadnoye");
    ASSERT_EQUAL(busStop.latitude(), 12.12);
    ASSERT_EQUAL(busStop.longitude(), 13.13);
}

void ReadTokenTest() {
    string_view line = "Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye";
    ASSERT_EQUAL(ReadToken(line, " > "), "Biryulyovo Zapadnoye");
    ASSERT_EQUAL(ReadToken(line, " > "), "Biryusinka");
    ASSERT_EQUAL(ReadToken(line, " > "), "Universam");
    ASSERT_EQUAL(ReadToken(line, " > "), "Biryulyovo Tovarnaya");
    ASSERT_EQUAL(ReadToken(line, " > "), "Biryulyovo Passazhirskaya");
    ASSERT_EQUAL(ReadToken(line, " > "), "Biryulyovo Zapadnoye");

    line = "Biryulyovo Zapadnoye - Biryusinka - Universam";
    ASSERT_EQUAL(ReadToken(line, " > "), "Biryulyovo Zapadnoye - Biryusinka - Universam");
}

void RouteTest() {
    {
        Route route;
        string line = "750: Tolstopaltsevo - Marushkino - Rasskazovka";
        stringstream ss(line);
        ss >> route;
        ASSERT_EQUAL(route.type(), Route::Type::Linear);
        ASSERT_EQUAL(route.number(), "750");
        ASSERT_EQUAL(route.stopsCount(), 5);
        ASSERT_EQUAL(route.uniqueStopsCount(), 3);
    }

    {
        Route route;
        string line = "751: Tolstopaltsevo > Marushkino > Rasskazovka > Tolstopaltsevo";
        stringstream ss(line);
        ss >> route;
        ASSERT_EQUAL(route.type(), Route::Type::Circle);
        ASSERT_EQUAL(route.number(), "751");
        ASSERT_EQUAL(route.stopsCount(), 4);
        ASSERT_EQUAL(route.uniqueStopsCount(), 3);
    }
}

void DistanceTest() {
    double res1 = calculateDistance(55.611087, 37.20829, 55.595884, 37.209755);
    double res2 = calculateDistance(55.595884, 37.209755, 55.632761, 37.333324);
    ASSERT_EQUAL((res1 + res2)*2, 20939.5);
}

void PrintRouteTest(){
    Route route;
    route.addStop("Biryulyovo Zapadnoye");
    route.addStop("Biryulyovo Zapadnoye2");
    std::unordered_map<string, BusStop> stops;
    stops["Biryulyovo Zapadnoye"] = BusStop("Biryulyovo Zapadnoye", 1, 1);
    stops["Biryulyovo Zapadnoye2"] = BusStop("Biryulyovo Zapadnoye2", 2, 2);
    route.print(stops, cout);
}

void DbTest() {
    DB db;
    stringstream ss("10\n"
            "Stop Tolstopaltsevo: 55.611087, 37.20829\n"
            "Stop Marushkino: 55.595884, 37.209755\n"
            "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
            "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n"
            "Stop Rasskazovka: 55.632761, 37.333324\n"
            "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517\n"
            "Stop Biryusinka: 55.581065, 37.64839\n"
            "Stop Universam: 55.587655, 37.645687\n"
            "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656\n"
            "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164\n");

    db.init(ss);

    stringstream is("3\n"
                    "Bus 256\n"
                    "Bus 750\n"
                    "Bus 751");
    stringstream os;
    db.run(is, os);

    cout << os.str();
}

int main() {
//        TestRunner tr;
//    RUN_TEST(tr, BusStopTest);
//    RUN_TEST(tr, ReadTokenTest);
//    RUN_TEST(tr, RouteTest);
//    RUN_TEST(tr, DistanceTest);
//    PrintRouteTest();
//    RUN_TEST(tr, DbTest);

    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    DB db;
    db.init(cin);
    db.run(cin, cout);

    return 0;
};