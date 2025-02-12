#include "json_reader.h"
#include "json_builder.h"

const json::Node& JsonReader::GetBaseRequests() const {
    const auto pointer = input_.GetRoot().AsDict().find("base_requests");
    if (pointer == input_.GetRoot().AsDict().end()) {
        return dummy_;
    }
    else {
        return pointer->second;
    }
}

const json::Node& JsonReader::GetStatRequests() const {
    const auto pointer = input_.GetRoot().AsDict().find("stat_requests");
    if (pointer == input_.GetRoot().AsDict().end()) {
        return dummy_;
    }
    else {
        return pointer->second;
    }
}

const json::Node& JsonReader::GetRenderSettings() const {
    const auto pointer = input_.GetRoot().AsDict().find("render_settings");
    if (pointer == input_.GetRoot().AsDict().end()) {
        return dummy_;
    }
    else {
        return pointer->second;
    }
}

const json::Node& JsonReader::GetRoutingSettings() const
{
    const auto pointer = input_.GetRoot().AsDict().find("routing_settings");
    if (pointer == input_.GetRoot().AsDict().end()) {
        return dummy_;
    }
    else {
        return pointer->second;
    }
}

void JsonReader::ProcessRequests(const json::Node& stat_requests, RequestHandler& rh) const {
    json::Array result;
    for (const auto& request : stat_requests.AsArray()) {
        const auto& request_map = request.AsDict();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") {
            result.push_back(PrintStop(request_map, rh).AsDict());
        }
        if (type == "Bus") {
            result.push_back(PrintRoute(request_map, rh).AsDict());
        }
        if (type == "Map") {
            result.push_back(PrintMap(request_map, rh).AsDict());
        }
        if (type == "Route") {
            result.push_back(PrintRouter(request_map, rh).AsDict());
        }
    }
    json::Print(json::Document{ result }, std::cout);
}

void JsonReader::FillCatalogue(transport::Catalogue& catalogue) {
    const json::Array& arr = GetBaseRequests().AsArray();
    for (const auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsDict();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            auto [stop_name, coordinates_, stop_distances] = FillStop(request_stops_map);
            catalogue.AddStop(stop_name, coordinates_);
        }
    }
    FillStopDistances(catalogue);
    for (const auto& request_bus : arr) {
        const auto& request_bus_map = request_bus.AsDict();
        const auto& type = request_bus_map.at("type").AsString();
        if (type == "Bus") {
            auto [bus_number, stops_, circular_route] = FillRoute(request_bus_map, catalogue);
            catalogue.AddRoute(bus_number, stops_, circular_route);
        }
    }
}

std::tuple<std::string_view, geo::Coordinates, std::map<std::string_view, int>> JsonReader::FillStop(const json::Dict& request_map) const {
    std::string_view stop_name = request_map.at("name").AsString();
    geo::Coordinates coordinates_ = { request_map.at("latitude").AsDouble(), request_map.at("longitude").AsDouble() };
    std::map<std::string_view, int> stop_distances;
    auto& distances = request_map.at("road_distances").AsDict();
    for (const auto& [stop_name, dist] : distances) {
        stop_distances.emplace(stop_name, dist.AsInt());
    }
    return std::make_tuple(stop_name, coordinates_, stop_distances);
}

void JsonReader::FillStopDistances(transport::Catalogue& catalogue) const {
    const json::Array& arr = GetBaseRequests().AsArray();
    for (const auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsDict();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            auto [stop_name, coordinates_, stop_distances] = FillStop(request_stops_map);
            for (const auto& [to_name, dist] : stop_distances) {
                auto from = catalogue.FindStop(stop_name);
                auto to = catalogue.FindStop(to_name);
                catalogue.SetDistance(from, to, dist);
            }
        }
    }
}

std::tuple<std::string_view, std::vector<const transport::Stop*>, bool> JsonReader::FillRoute(const json::Dict& request_map, transport::Catalogue& catalogue) const {
    std::string_view bus_number = request_map.at("name").AsString();
    std::vector<const transport::Stop*> stops_;
    for (const auto& stop : request_map.at("stops").AsArray()) {
        stops_.push_back(catalogue.FindStop(stop.AsString()));
    }
    bool circular_route = request_map.at("is_roundtrip").AsBool();
    return std::make_tuple(bus_number, stops_, circular_route);
}

renderer::MapRenderer JsonReader::FillRenderSettings(const json::Dict& request_map) const {
    renderer::RenderSettings render_settings;
    render_settings.width = request_map.at("width").AsDouble();
    render_settings.height = request_map.at("height").AsDouble();
    render_settings.padding = request_map.at("padding").AsDouble();
    render_settings.stop_radius = request_map.at("stop_radius").AsDouble();
    render_settings.line_width = request_map.at("line_width").AsDouble();
    render_settings.bus_label_font_size = request_map.at("bus_label_font_size").AsInt();
    const json::Array& bus_label_offset = request_map.at("bus_label_offset").AsArray();
    render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
    render_settings.stop_label_font_size = request_map.at("stop_label_font_size").AsInt();
    const json::Array& stop_label_offset = request_map.at("stop_label_offset").AsArray();
    render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };

    if (request_map.at("underlayer_color").IsString()) render_settings.underlayer_color = request_map.at("underlayer_color").AsString();
    else if (request_map.at("underlayer_color").IsArray()) {
        const json::Array& underlayer_color = request_map.at("underlayer_color").AsArray();
        if (underlayer_color.size() == 3) {
            render_settings.underlayer_color = svg::Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
        }
        else if (underlayer_color.size() == 4) {
            render_settings.underlayer_color = svg::Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
        }
        else throw std::logic_error("wrong underlayer colortype");
    }
    else throw std::logic_error("wrong underlayer color");
    render_settings.underlayer_width = request_map.at("underlayer_width").AsDouble();

    const json::Array& color_palette = request_map.at("color_palette").AsArray();
    for (const auto& color_element : color_palette) {
        if (color_element.IsString()) render_settings.color_palette.push_back(color_element.AsString());
        else if (color_element.IsArray()) {
            const json::Array& color_type = color_element.AsArray();
            if (color_type.size() == 3) {
                render_settings.color_palette.push_back(svg::Rgb(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt()));
            }
            else if (color_type.size() == 4) {
                render_settings.color_palette.push_back(svg::Rgba(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt(), color_type[3].AsDouble()));
            }
            else throw std::logic_error("wrong color_palette type");
        }
        else throw std::logic_error("wrong color_palette");
    }
    return render_settings;
}

transport::TransRouter JsonReader::FillTransRouterSettings(const json::Node& request_map) const
{
    transport::TransRouter routing_settings;
    return transport::TransRouter{ request_map.AsDict()
    .at("bus_wait_time").AsInt(), request_map.AsDict().at("bus_velocity").AsDouble()};
}


const json::Node JsonReader::PrintRoute(const json::Dict& request_map, RequestHandler& rh) const {
    json::Dict result;
    const std::string& route_number = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();
    if (!rh.IsBusNumber(route_number)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        result["curvature"] = rh.GetBusInfo(route_number)->curvature_;
        result["route_length"] = rh.GetBusInfo(route_number)->route_length_;
        result["stop_count"] = static_cast<int>(rh.GetBusInfo(route_number)->stops_count_);
        result["unique_stop_count"] = static_cast<int>(rh.GetBusInfo(route_number)->unique_stops_count_);
    }
    return json::Node{ result };
}

const json::Node JsonReader::PrintStop(const json::Dict& request_map, RequestHandler& rh) const {
    json::Dict result;
    const std::string& stop_name = request_map.at("name").AsString();
    result["request_id"] = request_map.at("id").AsInt();
    if (!rh.IsStopName(stop_name)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        json::Array buses;
        for (const auto& bus : rh.GetBusesByStop(stop_name)) {
            buses.push_back(bus);
        }
        result["buses"] = buses;
    }
    return json::Node{ result };
}

const json::Node JsonReader::PrintMap(const json::Dict& request_map, RequestHandler& rh) const {
    json::Dict result;
    result["request_id"] = request_map.at("id").AsInt();
    std::ostringstream strm;
    svg::Document map = rh.RenderMap();
    map.Render(strm);
    result["map"] = strm.str();
    return json::Node{ result };
}

const json::Node JsonReader::PrintRouter(const json::Dict& request_map, RequestHandler& rh) const
{
    json::Node result;
    const int id = request_map.at("id").AsInt();
    const std::string_view stop_from = request_map.at("from").AsString();
    const std::string_view stop_to = request_map.at("to").AsString();
    const auto& routing = rh.GetOptimalRoute(stop_from, stop_to);
    if (!routing) {
        result = json::Builder{}.StartDict().Key("request_id")
            .Value(id).Key("error_message")
            .Value("not found").EndDict().Build();
    }
    else {
        json::Array items;
        double total_time = 0.0;
        items.reserve(routing.value().edges.size());
        for (const auto& edge_id : routing.value().edges){
            const graph::Edge<double> edge = rh.GetRouterGraph().GetEdge(edge_id);
            if (edge.quality == 0) {
                items.emplace_back(json::Node(
                    json::Builder{}
                    .StartDict().Key("bus").Value(edge.name)
                    .Key("stop_name").Value(edge.weight)
                    .Key("type").Value("Wait").EndDict().Build()));
                total_time += edge.weight;
            }
            else {
                items.emplace_back(json::Node(json::Builder{}
                    .StartDict()
                    .Key("bus").Value(edge.name)
                    .Key("span_count").Value(static_cast<int>(edge.quality))
                    .Key("time").Value(edge.weight)
                    .Key("type").Value("Bus")
                    .EndDict()
                    .Build()));

                total_time += edge.weight;
            }
        }
        result = json::Builder{}
            .StartDict().Key("request_id").Value(id)
            .Key("total_time").Value(total_time)
            .Key("items").Value(items).EndDict().Build();
    }
    return result;
}
