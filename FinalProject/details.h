//
// Created by ka on 21.12.2021.
//

#ifndef FINALPROJECT_DETAILS_H
#define FINALPROJECT_DETAILS_H

#include <cmath>
#include <string_view>
#include <optional>
#include <utility>

namespace details {


//********************************
//a = sin²(Δφ/2) + cos φ1 ⋅ cos φ2 ⋅ sin²(Δλ/2)
//c = 2 ⋅ atan2( √a, √(1−a) )
//d = R ⋅ c

    double deg2rad(double deg);

    double calculateDistance(double lat1, double lon1, double lat2, double lon2);

    std::pair<std::string_view, std::optional<std::string_view>> SplitTwoStrict(std::string_view s, std::string_view delimiter = " ");

    std::pair<std::string_view, std::string_view> SplitTwo(std::string_view s, std::string_view delimiter = " ");

    std::string_view ReadToken(std::string_view& s, std::string_view delimiter = " ");

//********************************
};


#endif //FINALPROJECT_DETAILS_H
