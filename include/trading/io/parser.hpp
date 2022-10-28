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

#include "trading/candle.hpp"
#include "trading/exception.hpp"
#include "trading/index_t.hpp"
#include "trading/tuple.hpp"

namespace trading::io {
    template<class ...Types>
    class parser final {
        static constexpr int n = sizeof...(Types);
        std::tuple<Types...> data_;

        template<class T>
        requires std::same_as<T, long>
        long parse(const std::string& data)
        {
            return std::stol(data);
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

        template<std::size_t I = 0>
        inline typename std::enable_if<I==sizeof...(Types), void>::type
        parse_line(std::string (& line)[n]) { }

        template<std::size_t I = 0>
        inline typename std::enable_if<I<sizeof...(Types), void>::type
        parse_line(std::string (& line)[n])
        {
            try {
                std::get<I>(data_) = parse<get_type<I, Types...>>(line[I]);
            }
            catch (...) {
                std::throw_with_nested(bad_formatting("Cannot parse line"));
            }

            // keep parsing
            parse_line<I+1>(line);
        }

    public:
        std::tuple<Types...> operator()(std::string (& line)[n])
        {
            parse_line(line);
            return data_;
        }
    };
}


#endif //BACKTESTING_PARSER_HPP
