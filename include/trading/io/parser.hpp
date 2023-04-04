//
// Created by Tomáš Petříček on 09.10.2022.
//

#ifndef BACKTESTING_PARSER_HPP
#define BACKTESTING_PARSER_HPP

#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <trading/candle.hpp>
#include <trading/exception.hpp>
#include <trading/types.hpp>
#include <trading/tuple.hpp>

namespace trading::io {
    struct parser final {
        template<class T>
        requires std::same_as<T, int>
        inline static long parse(const std::string& data)
        {
            return std::stoi(data);
        }

        template<class T>
        requires std::same_as<T, long>
        inline static long parse(const std::string& data)
        {
            return std::stol(data);
        }

        template<class T>
        requires std::same_as<T, std::size_t>
        inline static std::size_t parse(const std::string& data)
        {
            return std::stoul(data);
        }

        template<class T>
        requires std::same_as<T, double>
        inline static double parse(const std::string& data)
        {
            return std::stod(data);
        }

        template<class T>
        requires std::same_as<T, float>
        inline static float parse(const std::string& data)
        {
            return std::stof(data);
        }

        template<class T>
        requires std::same_as<T, std::string>
        inline static std::string parse(const std::string& data)
        {
            return data;
        }
    };
}


#endif //BACKTESTING_PARSER_HPP
