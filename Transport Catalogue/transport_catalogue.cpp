#include "transport_catalogue.h"

namespace transport {

    void Catalogue::AddStop(std::string_view stop_name, const geo::Coordinates coordinates_) {
        all_stops_.push_back({ std::string(stop_name), coordinates_, {} });
        stopname_to_stop_[all_stops_.back().name_] = &all_stops_.back();
    }

    void Catalogue::AddRoute(std::string_view bus_number, const std::vector<const Stop*> stops_, bool is_circle_) {
        all_buses_.push_back({ std::string(bus_number), stops_, is_circle_ });
        busname_to_bus_[all_buses_.back().name_] = &all_buses_.back();
        for (const auto& route_stop : stops_) {
            for (auto& stop_ : all_stops_) {
                if (stop_.name_ == route_stop->name_) stop_.buses_by_stop_.insert(std::string(bus_number));
            }
        }
    }

    const Bus* Catalogue::FindRoute(std::string_view bus_number) const {
        return busname_to_bus_.count(bus_number) ? busname_to_bus_.at(bus_number) : nullptr;
    }

    const Stop* Catalogue::FindStop(std::string_view stop_name) const {
        return stopname_to_stop_.count(stop_name) ? stopname_to_stop_.at(stop_name) : nullptr;
    }

    size_t Catalogue::UniqueStopsCount(std::string_view bus_number) const {
        std::unordered_set<std::string_view> unique_stops;
        for (const auto& stop : busname_to_bus_.at(bus_number)->stops_) {
            unique_stops.insert(stop->name_);
        }
        return unique_stops.size();
    }

    void Catalogue::SetDistance(const Stop* from, const Stop* to, const int distance) {
        stop_distances_[{from, to}] = distance;
    }

    int Catalogue::GetDistance(const Stop* from, const Stop* to) const {
        if (stop_distances_.count({ from, to })) return stop_distances_.at({ from, to });
        else if (stop_distances_.count({ to, from })) return stop_distances_.at({ to, from });
        else return 0;
    }

    const std::map<std::string_view, const Bus*> Catalogue::GetSortedAllBuses() const {
        std::map<std::string_view, const Bus*> result;
        for (const auto& bus : busname_to_bus_) {
            result.emplace(bus);
        }
        return result;
    }

    const std::map<std::string_view, const Stop*> Catalogue::GetSortedAllStops() const
    {
        std::map<std::string_view, const Stop*> result;
        for (const auto& stop : stopname_to_stop_) {
            result.emplace(stop);
        }
        return result;
    }

    std::optional<BusStat> Catalogue::GetBusStat(const std::string_view bus_number) const
    {
        BusStat bus_stat{};
        const Bus* bus = FindRoute(bus_number);

        if (!bus) throw std::invalid_argument("bus not found");
        if (bus->is_circle_) bus_stat.stops_count_ = bus->stops_.size();
        else bus_stat.stops_count_ = bus->stops_.size() * 2 - 1;

        int route_length_ = 0;
        double geographic_length = 0.0;

        for (size_t i = 0; i < bus->stops_.size() - 1; ++i) {
            auto from = bus->stops_[i];
            auto to = bus->stops_[i + 1];
            if (bus->is_circle_) {
                route_length_ += GetDistance(from, to);
                geographic_length += geo::ComputeDistance(from->coordinates_,
                    to->coordinates_);
            }
            else {
                route_length_ += GetDistance(from, to) + GetDistance(to, from);
                geographic_length += geo::ComputeDistance(from->coordinates_,
                    to->coordinates_) * 2;
            }
        }

        bus_stat.unique_stops_count_ = UniqueStopsCount(bus_number);
        bus_stat.route_length_ = route_length_;
        bus_stat.curvature_ = route_length_ / geographic_length;
        return bus_stat;
    }
}
