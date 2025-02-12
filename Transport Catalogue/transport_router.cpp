#include "transport_router.h"

namespace transport {

    constexpr double KM = 1000.0;
    constexpr double MIN = 60.0; 

    void TransRouter::BuildGraph(const Catalogue& catalogue)
    {
        const auto& all_stops = catalogue.GetSortedAllStops();
        graph::DirectedWeightedGraph<double> stops_graph(all_stops.size() * 2);
        AddStopsToGraph(catalogue, stops_graph);
        AddBusesToGraph(catalogue, stops_graph);

        graph_ = std::move(stops_graph);
        router_ = std::make_unique<graph::Router<double>>(graph_);
    }

    

    void TransRouter::AddStopsToGraph(const Catalogue& catalogue, graph::DirectedWeightedGraph<double>& stops_graph) {
        std::map<std::string, graph::VertexId> stop_ids;
        graph::VertexId vertex_id = 0;
        const auto& all_stops = catalogue.GetSortedAllStops();

        for (const auto& [stop_name, stop_info] : all_stops) {
            stop_ids[stop_info->name_] = vertex_id;
            stops_graph.AddEdge({
                stop_info->name_,
                0,
                vertex_id,
                ++vertex_id,
                static_cast<double>(bus_wait_time_)
                });
            ++vertex_id;
        }
        stop_ids_ = std::move(stop_ids);
    }

    void TransRouter::AddBusesToGraph(const Catalogue& catalogue, graph::DirectedWeightedGraph<double>& stops_graph)
    {
        const auto& all_buses = catalogue.GetSortedAllBuses();
        for (auto item = all_buses.begin(); item != all_buses.end(); ++item) 
        {
            const auto& bus_info = item->second;
            const auto& stops = bus_info->stops_;
            size_t stops_count = stops.size();

            for (size_t i = 0; i < stops_count; ++i) {
                for (size_t j = i + 1; j < stops_count; ++j) {
                    const Stop* stop_from = stops[i];
                    const Stop* stop_to = stops[j];
                    int dist_sum = 0;
                    int dist_sum_inverse = 0;
                    for (size_t k = i + 1; k <= j; ++k) {
                        dist_sum += catalogue.GetDistance(stops[k - 1], stops[k]);
                        dist_sum_inverse += catalogue.GetDistance(stops[k], stops[k - 1]);
                    }
                    stops_graph.AddEdge({ bus_info->name_,
                        j - i,
                        stop_ids_.at(stop_from->name_) + 1,
                        stop_ids_.at(stop_to->name_),
                        static_cast<double>(dist_sum) / (bus_velocity_ * (KM / MIN)) });

                    if (!bus_info->is_circle_) {
                        stops_graph.AddEdge({ bus_info->name_,
                            j - i,
                            stop_ids_.at(stop_to->name_) + 1,
                            stop_ids_.at(stop_from->name_),
                            static_cast<double>(dist_sum_inverse) / (bus_velocity_ * (KM / MIN)) });
                    }
                }
            }
        }
    }

    const std::optional<graph::Router<double>::RouteInfo> TransRouter::FindRoute(const std::string_view stop_from, const std::string_view stop_to) const {
        const auto stop_from_id = stop_ids_.at(std::string(stop_from));
        const auto stop_to_id = stop_ids_.at(std::string(stop_to));
        auto route = router_->BuildRoute(stop_from_id, stop_to_id);
        return route;
    }

    const graph::DirectedWeightedGraph<double>& TransRouter::GetGraph() const {
        return graph_;
    }
}
