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
                          double time, int spanCount) {

    std::string fullNameFrom = busStopNameFrom;
    std::string fullNameTo = busStopNameTo;

    if(_idxes.count(fullNameFrom) < 1) {
        _idxes[fullNameFrom] = _currentIdx++;
    }
    if(_idxes.count(fullNameTo) < 1) {
        _idxes[fullNameTo] = _currentIdx++;
    }

    std::string type;
    if (spanCount == 0) {
        type = "Wait";
    } else {
        type = "Bus";
    }

    size_t id = _graph->AddEdge({_idxes[fullNameFrom], _idxes[fullNameTo], time});
    _types[id] = {busStopNameFrom, busStopNameTo, type, time, routeNameFrom, spanCount};

//    std::cout << busStopNameFrom << " -> " << busStopNameTo << "; type = "
//        << type << "; route = " << routeNameFrom << "; span_count = " << spanCount
//        << "; time = " << time << "\n";
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

        std::string number;
        for(size_t i = 0; i < edgeInfo.edge_count; ++i) {

            auto edgeId = _router->GetRouteEdge(edgeInfo.id, i);
            const auto& info = _types[edgeId];

            if(!isFirst)
                std::cout << ", ";

            if (info.type == "Wait") {
                std::cout << "{ \"time\": " << info.weight << ", " << "\"type\": " << "\""<< info.type << "\"" << ", "
                    << "\"stop_name\": " << "\"" << info.toName << "\"}";
            } else {
                std::cout << "{\"time\": " << info.weight << ", " << "\"type\": " << "\"" << info.type << "\"" << ", "
                              << "\"span_count\": " << info.spanCount << ", \"bus\": " << "\"" << info.number << "\"}";
            }

            isFirst = false;
        }

        std::cout << "], ";
    } else {
        std::cout << "\"error_message\": \"not found\", ";
    }

}
