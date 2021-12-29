//
// Created by ka on 27.12.2021.
//

#ifndef FINALPROJECT_GRAPHROUTER_H
#define FINALPROJECT_GRAPHROUTER_H

#include "graph.h"
#include "router.h"

#include <memory>

using namespace Graph;

class GraphRouter {

public:
    GraphRouter();
    void setVertexCount(size_t);
    void addEdge( const std::string& busStopNameFrom, const std::string& routeNameFrom,
                  const std::string& busStopNameTo, const std::string& routeNameTo,
                  double time, const std::string& type = "Bus");

    void buildRoute(const std::string&, const std::string& );

    struct Info {
        std::string fromName;
        std::string toName;
        std::string type;
        double weight;
        std::string number;
    };

private:
    std::unique_ptr<Graph::DirectedWeightedGraph<double>> _graph = nullptr;
    std::unique_ptr<Graph::Router<double>> _router = nullptr;

    size_t _currentIdx = 0;
    std::unordered_map<std::string, size_t> _idxes;
    std::unordered_map<EdgeId, Info> _types;
};


#endif //FINALPROJECT_GRAPHROUTER_H
