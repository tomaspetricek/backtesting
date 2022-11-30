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
    class parser final {
    public:
        template<class T>
        requires std::same_as<T, long>
        long parse(const std::string& data)
        {
            return std::stol(data);
        }

        template<class T>
        requires std::same_as<T, std::size_t>
        std::size_t parse(const std::string& data)
        {
            return std::stoul(data);
        }

        template<class T>
        requires std::same_as<T, double>
        double parse(const std::string& data)
        {
            return std::stod(data);
        }

        template<class T>
        requires std::same_as<T, std::string>
        std::string parse(const std::string& data)
        {
            return data;
        }
    };
}


#endif //BACKTESTING_PARSER_HPP
