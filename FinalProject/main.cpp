#include <string>
#include <iostream>
#include <unordered_map>
#include <list>
#include <algorithm>
#include <numeric>

#include "test_runner.h"

#include "BusStops.h"
#include "Routes.h"
#include "details.h"
#include "json.h"
#include "graph.h"
#include "router.h"

#include "GraphRouter.h"

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

    void initJson(const Json::Node& node) {
        auto requests = node.AsArray();
        for (auto &request: requests) {
            auto query = request.AsMap();
            string type = query.at("type").AsString();
            if (type == "Stop") {
                BusStop busStop(request);
                if (_busStops.count(busStop.name()) > 0) {
                    _busStops[busStop.name()]->set(busStop.latitude(), busStop.longitude(), busStop.distances());
                } else {
                    auto stop = make_shared<BusStop>(busStop);
                    _busStops[busStop.name()] = move(stop);
                }

            } else if (type == "Bus") {
                auto route = make_shared<Route>(request, _busStops);
                _routes[route->number()] = move(route);
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
                    } else {
                        os << "not found";
                    }
                    break;
                }
                case Stop: {
                    string name;
                    getline(ss, name, ':');
                    os << "Stop " << name << ": ";
                    if (_busStops.count(name) > 0) {
                        _busStops.at(name)->print(os);

                    } else {
                        os << "not found";
                    }
                    break;
                }
            }
        }
    }

    void runJson(const Json::Node& node, ostream& os) {
        auto requests = node.AsArray();
        bool isFirst = true;
        os << "[";
        for (auto& request: requests) {
            if(!isFirst)
                os << ", \n";
            os << "{";
            isFirst = false;

            auto query = request.AsMap();
            string type = query.at("type").AsString();
            auto id = query.at("id").AsInt();
            if (type == "Stop") {
                auto name = query.at("name").AsString();
                if (_busStops.count(name) > 0) {
                    _busStops.at(name)->print(os, true, id);

                } else {
                    os << "\"request_id\": " << id <<
                    ",\"error_message\": " << "\"not found\"";
                }
            } else if (type == "Bus") {
                auto name = query.at("name").AsString();
                if (_routes.count(name) > 0) {
                    _routes.at(name)->print(os, true, id);

                } else {
                    os << "\"request_id\": " << id <<
                       ",\"error_message\": " << "\"not found\"";
                }
            } else if (type == "Route") {
                // TODO
                auto from = query.at("from").AsString();
                auto to = query.at("to").AsString();

                if(!_router) {
                    graphInit();
                }

                _router->buildRoute(from, to);
                os << "\"request_id\": " << id << "";

                // TODO
            }
            os << "}";
        }
        os << "]";
    }

    void preInit(const Json::Node& node) {
        auto settings = node.AsMap();
        _waitingTime = settings.at("bus_wait_time").AsInt();
        _busVelocity = settings.at("bus_velocity").AsInt() * 1000. / 60.;
    }

    void graphInit() {
        size_t vertexCount = std::accumulate(
                _busStops.begin(), _busStops.end(), _busStops.size(), [](size_t ret, std::pair<string, std::shared_ptr<BusStop>> stop)
        {
            return ret += stop.second->routesCount();
        });

        _router = make_unique<GraphRouter>();
        _router->setVertexCount(vertexCount);

        for(auto& [name, route] : _routes) {
            route->createEdges(_busVelocity, *_router);
        }
        for(auto& [name, busStop] : _busStops) {
            busStop->createEdges(_waitingTime, *_router);
        }
    }

    struct Vertex {
        std::string stopName;
        std::string routeName;
    };

private:
    BusStops _busStops;
    Routes _routes;

    double _waitingTime;        // min
    double _busVelocity;        // m / min

    unique_ptr<GraphRouter> _router = nullptr;
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

void JsonTest() {
    {
        stringstream ss("{      \"type\": \"Stop\",\n"
                        "      \"road_distances\": {\n"
                        "        \"Biryulyovo Zapadnoye\": 1200\n"
                        "      },\n"
                        "      \"longitude\": 37,\n"
                        "      \"name\": \"Prazhskaya\",\n"
                        "      \"latitude\": 55}");

        auto doc = Json::Load(ss);
        auto dict = doc.GetRoot().AsMap();
        ASSERT_EQUAL(dict.at("type").AsString(), "Stop");
        ASSERT_EQUAL(dict.at("name").AsString(), "Prazhskaya");
        ASSERT_EQUAL(dict.at("latitude").AsInt(), 55);
        ASSERT_EQUAL(dict.at("road_distances").AsMap().at("Biryulyovo Zapadnoye").AsInt(), 1200);
    }

    {
        stringstream ss("{\"isOk\": true, \"notOk\": false}");
        auto doc = Json::Load(ss);
        ASSERT(doc.GetRoot().AsMap().at("isOk").AsBool());
        ASSERT(!doc.GetRoot().AsMap().at("notOk").AsBool());
    }

    {
        stringstream ss("{\"isOk\": 5, \"notOk\": -6.1}");
        auto doc = Json::Load(ss);
        ASSERT_EQUAL(doc.GetRoot().AsMap().at("isOk").AsDouble(), 5);
        ASSERT_EQUAL(doc.GetRoot().AsMap().at("notOk").AsDouble(), -6.1);
    }

    {
        stringstream ss("{\n"
                        "  \"base_requests\": [\n"
                        "    {\n"
                        "      \"type\": \"Stop\",\n"
                        "      \"road_distances\": {\n"
                        "        \"Marushkino\": 3900\n"
                        "      },\n"
                        "      \"longitude\": 37.20829,\n"
                        "      \"name\": \"Tolstopaltsevo\",\n"
                        "      \"latitude\": 55.611087\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Stop\",\n"
                        "      \"road_distances\": {\n"
                        "        \"Rasskazovka\": 9900\n"
                        "      },\n"
                        "      \"longitude\": 37.209755,\n"
                        "      \"name\": \"Marushkino\",\n"
                        "      \"latitude\": 55.595884\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Bus\",\n"
                        "      \"name\": \"256\",\n"
                        "      \"stops\": [\n"
                        "        \"Biryulyovo Zapadnoye\",\n"
                        "        \"Biryusinka\",\n"
                        "        \"Universam\",\n"
                        "        \"Biryulyovo Tovarnaya\",\n"
                        "        \"Biryulyovo Passazhirskaya\",\n"
                        "        \"Biryulyovo Zapadnoye\"\n"
                        "      ],\n"
                        "      \"is_roundtrip\": true\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Bus\",\n"
                        "      \"name\": \"750\",\n"
                        "      \"stops\": [\n"
                        "        \"Tolstopaltsevo\",\n"
                        "        \"Marushkino\",\n"
                        "        \"Rasskazovka\"\n"
                        "      ],\n"
                        "      \"is_roundtrip\": false\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Stop\",\n"
                        "      \"road_distances\": {},\n"
                        "      \"longitude\": 37.333324,\n"
                        "      \"name\": \"Rasskazovka\",\n"
                        "      \"latitude\": 55.632761\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Stop\",\n"
                        "      \"road_distances\": {\n"
                        "        \"Rossoshanskaya ulitsa\": 7500,\n"
                        "        \"Biryusinka\": 1800,\n"
                        "        \"Universam\": 2400\n"
                        "      },\n"
                        "      \"longitude\": 37.6517,\n"
                        "      \"name\": \"Biryulyovo Zapadnoye\",\n"
                        "      \"latitude\": 55.574371\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Stop\",\n"
                        "      \"road_distances\": {\n"
                        "        \"Universam\": 750\n"
                        "      },\n"
                        "      \"longitude\": 37.64839,\n"
                        "      \"name\": \"Biryusinka\",\n"
                        "      \"latitude\": 55.581065\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Stop\",\n"
                        "      \"road_distances\": {\n"
                        "        \"Rossoshanskaya ulitsa\": 5600,\n"
                        "        \"Biryulyovo Tovarnaya\": 900\n"
                        "      },\n"
                        "      \"longitude\": 37.645687,\n"
                        "      \"name\": \"Universam\",\n"
                        "      \"latitude\": 55.587655\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Stop\",\n"
                        "      \"road_distances\": {\n"
                        "        \"Biryulyovo Passazhirskaya\": 1300\n"
                        "      },\n"
                        "      \"longitude\": 37.653656,\n"
                        "      \"name\": \"Biryulyovo Tovarnaya\",\n"
                        "      \"latitude\": 55.592028\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Stop\",\n"
                        "      \"road_distances\": {\n"
                        "        \"Biryulyovo Zapadnoye\": 1200\n"
                        "      },\n"
                        "      \"longitude\": 37.659164,\n"
                        "      \"name\": \"Biryulyovo Passazhirskaya\",\n"
                        "      \"latitude\": 55.580999\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Bus\",\n"
                        "      \"name\": \"828\",\n"
                        "      \"stops\": [\n"
                        "        \"Biryulyovo Zapadnoye\",\n"
                        "        \"Universam\",\n"
                        "        \"Rossoshanskaya ulitsa\",\n"
                        "        \"Biryulyovo Zapadnoye\"\n"
                        "      ],\n"
                        "      \"is_roundtrip\": true\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Stop\",\n"
                        "      \"road_distances\": {},\n"
                        "      \"longitude\": 37.605757,\n"
                        "      \"name\": \"Rossoshanskaya ulitsa\",\n"
                        "      \"latitude\": 55.595579\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Stop\",\n"
                        "      \"road_distances\": {},\n"
                        "      \"longitude\": 37.603831,\n"
                        "      \"name\": \"Prazhskaya\",\n"
                        "      \"latitude\": 55.611678\n"
                        "    }\n"
                        "  ],\n"
                        "  \"stat_requests\": [\n"
                        "    {\n"
                        "      \"type\": \"Bus\",\n"
                        "      \"name\": \"256\",\n"
                        "      \"id\": 1965312327\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Bus\",\n"
                        "      \"name\": \"750\",\n"
                        "      \"id\": 519139350\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Bus\",\n"
                        "      \"name\": \"751\",\n"
                        "      \"id\": 194217464\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Stop\",\n"
                        "      \"name\": \"Samara\",\n"
                        "      \"id\": 746888088\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Stop\",\n"
                        "      \"name\": \"Prazhskaya\",\n"
                        "      \"id\": 65100610\n"
                        "    },\n"
                        "    {\n"
                        "      \"type\": \"Stop\",\n"
                        "      \"name\": \"Biryulyovo Zapadnoye\",\n"
                        "      \"id\": 1042838872\n"
                        "    }\n"
                        "  ]\n"
                        "}");

        using BusStops = unordered_map<string, std::shared_ptr<BusStop>>;
        using Routes = unordered_map<string, std::shared_ptr<Route>>;
        BusStops _busStops;
        Routes _routes;

        auto doc = Json::Load(ss);
        auto root = doc.GetRoot().AsMap();
        auto requests = root["base_requests"].AsArray();
        for (auto &request: requests) {
            auto query = request.AsMap();
            string type = query.at("type").AsString();
            if (type == "Stop") {
                string name = query.at("name").AsString();
                double lon = query.at("longitude").AsDouble();
                double lat = query.at("latitude").AsDouble();
                auto stops = query.at("road_distances").AsMap();
                BusStop::Distances distances;
                for (const auto&[name, length]: stops) {
                    distances[name] = length.AsInt();
                }
                BusStop busStop(name);
                busStop.set(lat, lon, distances);

                if (_busStops.count(name) > 0) {
                    _busStops[name]->set(busStop.latitude(), busStop.longitude(), busStop.distances());
                } else {
                    auto stop = make_shared<BusStop>(busStop);
                    _busStops[name] = move(stop);
                }

            } else if (type == "Bus") {
                string name = query.at("name").AsString();
                bool isRound = query.at("is_roundtrip").AsBool();

                auto route = make_shared<Route>(name, static_cast<Route::Type>(!isRound));
                auto stops = query.at("stops").AsArray();
                for (const auto &stop: stops) {
                    auto stopName = stop.AsString();

                    if (_busStops.count(stopName) > 0) {
                        auto busStop = _busStops[stopName];
                        busStop->addRoute(*route);
                        route->addStop(_busStops[stopName]);
                    } else {
                        auto busStop = std::make_shared<BusStop>(stopName);
                        route->addStop(busStop);
                        busStop->addRoute(*route);
                        _busStops[stopName] = move(busStop);
                    }
                }
                _routes[name] = move(route);
            }
        }

        auto stats = root["stat_requests"].AsArray();
        for(const auto& stat : stats) {
            auto query = stat.AsMap();
            string type = query.at("type").AsString();
            string name = query.at("name").AsString();
            int id = query.at("id").AsInt();

        }
    }

}

void RouterTest() {
    GraphRouter router;
    router.setVertexCount(3);
    router.addEdge("a", "a", "b", "b", 1);
    router.addEdge("b", "b", "a", "a", 5);
    router.addEdge("a", "", "a", "a", 2);


    router.buildRoute("a", "bb");
    router.buildRoute("bb", "aa");
}

int main() {
//    TestRunner tr;
//    RUN_TEST(tr, BusStopTest);
//    RUN_TEST(tr, ReadTokenTest);
//    RUN_TEST(tr, RouteTest);
//    RUN_TEST(tr, DistanceTest);
//    PrintRouteTest();
//   RUN_TEST(tr, DbTest);
//    RUN_TEST(tr, JsonTest);
 //   RUN_TEST(tr, RouterTest);

    ios_base::sync_with_stdio(false);
    cin.tie(NULL);


      DB db;
//    db.init(cin);
//    db.run(cin, cout);

    auto doc = Json::Load(cin);
    auto root = doc.GetRoot().AsMap();
    db.preInit(root["routing_settings"]);
    db.initJson(root["base_requests"]);
    db.runJson(root["stat_requests"], cout);

    return 0;
};