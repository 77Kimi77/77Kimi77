#pragma once
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include "geo.h"
namespace transport {
    struct Stop {
        std::string name_;
        geo::Coordinates coordinates_;
        std::set<std::string> buses_by_stop_;
    };

    struct Bus {
        std::string name_;
        std::vector<const Stop*> stops_;
        bool is_circle_;
    };

    struct BusStat {
        size_t stops_count_;
        size_t unique_stops_count_;
        double route_length_;
        double curvature_;
    };
}
