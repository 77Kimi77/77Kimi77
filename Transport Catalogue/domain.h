#pragma once
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include "geo.h"
namespace transport {
    /*Фаил содержит различные структуры, которые используются
    для описания данных которые хранятся в хэш-таблицах в классе транспортного каталога
    
    */
    struct Stop {
        std::string name_; //название остановки
        geo::Coordinates coordinates_; //координаты(долгота, широта) остановки
        std::set<std::string> buses_by_stop_; //контейнер для зранения всех автобусов проходящих чернз определенную остановку
    };

    struct Bus {
        std::string name_; //номер* маршрута
        std::vector<const Stop*> stops_; //вектор структур остановок для данного маршрута
        bool is_circle_; //индикатор не-/кругового маршрута
    };

    struct BusStat {
    /* Структура, которая описывает характеристики определенного маршрута,
    согласно указанным остановкам;
    */
        size_t stops_count_; //кол-во остановок
        size_t unique_stops_count_; // кол-во уникальных остановок
        double route_length_; //длина маршрута 
        double curvature_; //фактическая длина маршрута по кривой
    };
}
