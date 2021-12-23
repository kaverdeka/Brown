#include <string>
#include <iostream>
#include <unordered_map>
#include <list>
#include <algorithm>

#include "test_runner.h"

#include "BusStops.h"
#include "Routes.h"
#include "details.h"

using namespace std;
using  namespace details;
//************************






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
    using BusStops = unordered_map<string, std::shared_ptr<BusStop>>;
    using Routes =  unordered_map<string, std::shared_ptr<Route>>;

    void init(istream& is) {
        size_t queryCount;
        is >> queryCount;

        string str;
        getline(is, str);

        for(size_t i = 0; i < queryCount; ++i) {
            QueryType type;
            is >> type;

            string line;
            getline(is, line);
            stringstream ss(line);

            switch (type) {
                case Stop: {
                    BusStop stop(ss);
                    if(_busStops.count(stop.name()) > 0) {
                        _busStops[stop.name()]->set(stop.latitude(), stop.longitude(), stop.distances());
                    } else {
                        auto busStop = make_shared<BusStop>(stop);
                        _busStops[busStop->name()] = move(busStop);
                    }
                    break;
                }
                case Bus: {
                    auto route = make_shared<Route>(ss, _busStops);
                    _routes[route->number()] = move(route);
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
                        _routes.at(number)->print(os);
                        os << "\n";

                    } else {
                        os << "not found\n";
                    }
                    break;
                }
                case Stop: {
                    string name;
                    getline(ss, name, ':');
                    os << "Stop " << name << ": ";
                    if (_busStops.count(name) > 0) {
                        _busStops.at(name)->print(os);
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
    BusStops _busStops;
    Routes _routes;
};



void BusStopTest() {
    string line = "Biryulyovo Zapadnoye: 12.12, 13.13, 3900m to Biryulyovo Tovarnaya, 1000m to Biryulyovo Passazhirskaya";
    //              Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam
    stringstream ss(line);
    BusStop busStop(ss);
    ASSERT_EQUAL(busStop.name(), "Biryulyovo Zapadnoye");
    ASSERT_EQUAL(busStop.latitude(), 12.12);
    ASSERT_EQUAL(busStop.longitude(), 13.13);
    BusStop::Distances dist;
    dist["Biryulyovo Tovarnaya"] = 3900;
    dist["Biryulyovo Passazhirskaya"] = 1000;
    ASSERT_EQUAL(busStop.distances(), dist);
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
    using BusStops = unordered_map<string, std::shared_ptr<BusStop>>;
    BusStops stops;
    {
        string line = "750: Tolstopaltsevo - Marushkino - Rasskazovka";
        stringstream ss(line);
        Route route(ss, stops);
        ASSERT_EQUAL(route.type(), Route::Type::Linear);
        ASSERT_EQUAL(route.number(), "750");
        ASSERT_EQUAL(route.stopsCount(), 5);
        ASSERT_EQUAL(route.uniqueStopsCount(), 3);
    }

    {
        string line = "751: Tolstopaltsevo > Marushkino > Rasskazovka > Tolstopaltsevo";
        stringstream ss(line);
        Route route(ss, stops);
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

//void PrintRouteTest(){
//    Route route;
//    route.addStop("Biryulyovo Zapadnoye");
//    route.addStop("Biryulyovo Zapadnoye2");
//    std::unordered_map<string, BusStop> stops;
//    stops["Biryulyovo Zapadnoye"] = BusStop("Biryulyovo Zapadnoye", 1, 1);
//    stops["Biryulyovo Zapadnoye2"] = BusStop("Biryulyovo Zapadnoye2", 2, 2);
//    route.print(stops, cout);
//}
//
void DbTest() {
    DB db;
    stringstream ss("13\n"
            "Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino\n"
                    "Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka\n"
                    "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
                    "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n"
                    "Stop Rasskazovka: 55.632761, 37.333324\n"
                    "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam\n"
                    "Stop Biryusinka: 55.581065, 37.64839, 750m to Universam\n"
                    "Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya\n"
                    "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya\n"
                    "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye\n"
                    "Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye\n"
                    "Stop Rossoshanskaya ulitsa: 55.595579, 37.605757\n"
                    "Stop Prazhskaya: 55.611678, 37.603831");

    db.init(ss);

    stringstream is("6\n"
                    "Bus 256\n"
                    "Bus 750\n"
                    "Bus 751\n"
                    "Stop Samara\n"
                    "Stop Prazhskaya\n"
                    "Stop Biryulyovo Zapadnoye");
    stringstream os;
    db.run(is, os);

    cout << os.str();
}

int main() {
//    TestRunner tr;
//    RUN_TEST(tr, BusStopTest);
//    RUN_TEST(tr, ReadTokenTest);
//    RUN_TEST(tr, RouteTest);
//    RUN_TEST(tr, DistanceTest);
////    PrintRouteTest();
 //   RUN_TEST(tr, DbTest);

    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    DB db;
    db.init(cin);
    db.run(cin, cout);

    return 0;
};