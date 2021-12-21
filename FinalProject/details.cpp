//
// Created by ka on 21.12.2021.
//

#include "details.h"

namespace details {
    static constexpr double pi = 3.1415926535;
    static constexpr double r = 6371;
    using namespace std;

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

    pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter) {
        const size_t pos = s.find(delimiter);
        if (pos == s.npos) {
            return {s, nullopt};
        } else {
            return {s.substr(0, pos), s.substr(pos + delimiter.length())};
        }
    }

    pair<string_view, string_view> SplitTwo(string_view s, string_view delimiter) {
        const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
        return {lhs, rhs_opt.value_or("")};
    }

    string_view ReadToken(string_view& s, string_view delimiter) {
        const auto [lhs, rhs] = SplitTwo(s, delimiter);
        s = rhs;
        return lhs;
    }

}