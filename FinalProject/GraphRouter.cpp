//
// Created by ka on 27.12.2021.
//

#include "GraphRouter.h"
#include <memory>
#include <iostream>


//size_t getIndex(const std::string& stopName, const std::string& routeName);

GraphRouter::GraphRouter() {}

void GraphRouter::setVertexCount(size_t vertexCount) {
    _graph = std::make_unique<Graph::DirectedWeightedGraph<double>>(vertexCount);
}

void GraphRouter::addEdge(const std::string& busStopNameFrom, const std::string& routeNameFrom,
                          const std::string& busStopNameTo, const std::string& routeNameTo,
                          double time, const std::string& type) {

    std::string fullNameFrom = busStopNameFrom + routeNameFrom;
    std::string fullNameTo = busStopNameTo + routeNameTo;
    if(_idxes.count(fullNameFrom) < 1) {
        _idxes[fullNameFrom] = _currentIdx++;
    }
    if(_idxes.count(fullNameTo) < 1) {
        _idxes[fullNameTo] = _currentIdx++;
    }
    size_t id = _graph->AddEdge({_idxes[fullNameFrom], _idxes[fullNameTo], time});
    _types[id] = {busStopNameFrom, busStopNameTo, type, time, routeNameFrom};
}

void GraphRouter::buildRoute(const std::string& busStopFrom, const std::string& busStopTo) {

    if(!_router) {
        _router = std::make_unique<Graph::Router<double>>(*_graph);
    }

    auto edges = _router->BuildRoute(_idxes[busStopFrom], _idxes[busStopTo]);
    if(edges) {
        auto edgeInfo = edges.value();
        std::cout << "\"total_time\": " << edgeInfo.weight << ", " <<
        "\"items\": [";
        if(edgeInfo.edge_count == 0) {
            std::cout << "], ";
            return;
        }

        bool isFirst = true;

        int spanCount = 0;
        double time = 0.;
        std::string number;
        for(size_t i = 0; i < edgeInfo.edge_count - 1; ++i) {

            auto edgeId = _router->GetRouteEdge(edgeInfo.id, i);
            const auto& info = _types[edgeId];

            if (info.type == "Wait") {
                if(!isFirst)
                    std::cout << ", ";

                if(spanCount > 0) {
                    std::cout << "{\"time\": " << time << ", " << "\"type\": " << "\"" << "Bus" << "\"" << ", "
                        << "\"span_count\": " << spanCount << ", \"bus\": " << "\"" << number << "\"}, ";

                    spanCount = 0;
                    time = 0;
                }
                std::cout << "{ \"time\": " << info.weight << ", " << "\"type\": " << "\""<< info.type << "\"" << ", "
                    << "\"stop_name\": " << "\"" << info.toName << "\"}";

                isFirst = false;
            } else {
                ++spanCount;
                time += info.weight;
                number = info.number;
            }

            if ( i == edgeInfo.edge_count - 2) {
                if(spanCount > 0) {
                    std::cout << ", {\"time\": " << time << ", " << "\"type\": " << "\"" << "Bus" << "\"" << ", "
                              << "\"span_count\": " << spanCount << ", \"bus\": " << "\"" << number << "\"}";

                    spanCount = 0;
                    time = 0;
                }
            }
        }

        std::cout << "], ";
    } else {
        std::cout << "\"error_message\": \"not found\", ";
    }

}
