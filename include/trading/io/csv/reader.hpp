//
// Created by Tomáš Petříček on 27.06.2022.
//

#ifndef EMASTRATEGY_READ_HPP
#define EMASTRATEGY_READ_HPP

#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <type_traits>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "trading/candle.hpp"
#include "trading/exception.hpp"
#include "trading/currency.hpp"
#include <trading/types.hpp>
#include "trading/tuple.hpp"
#include <trading/io/parser.hpp>
#include <utility>

namespace trading::io::csv {
    class reader final {
        std::ifstream file_;
        std::filesystem::path path_;
        char delim_;
        std::string line_;
//        static constexpr std::size_t n_cols_ = sizeof...(Types);

    public:
//        using row_type = std::tuple<Types...>;

        explicit reader(std::filesystem::path  path, char delim)
                :path_(std::move(path)), delim_(delim)
        {
            if (!std::filesystem::exists(path_))
                throw std::invalid_argument("File does not exist");

            file_ = std::ifstream{path_.string()};

            if (!file_.is_open())
                throw std::runtime_error("Cannot open "+path_.string());
        }

        template<class ...Types>
        bool read_row(Types& ...inputs)
        {
            // read lines
            if(!std::getline(file_, line_))
                return false;

            // clean
            line_.erase(std::remove(line_.begin(), line_.end(), '\r'), line_.end());

            // parse line
            std::stringstream ss(line_);
            ss.exceptions(std::ios::failbit);
            std::string data;

            auto parse_line = [&](auto& in){
                std::getline(ss, data, delim_);
                in = parser::parse<typename std::remove_reference<decltype(in)>::type>(data);
            };

            (parse_line(inputs),...);
            return true;
        }
    };
}

#endif //EMASTRATEGY_READ_HPP
