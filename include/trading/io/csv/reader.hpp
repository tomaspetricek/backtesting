//
// Created by Tomáš Petříček on 27.06.2022.
//

#ifndef EMASTRATEGY_READ_HPP
#define EMASTRATEGY_READ_HPP

#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "trading/candle.hpp"
#include "trading/exception.hpp"
#include "trading/currency.hpp"
#include "trading/index_t.hpp"
#include "trading/tuple.hpp"

namespace trading::io::csv {
    template<class Input, class ...ColumnTypes>
    class reader final {
        std::ifstream file_;
        std::filesystem::path path_;
        char delim_;
        std::function<Input(ColumnTypes...)> deserializer_;
        std::tuple<ColumnTypes...> args_;

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
        inline typename std::enable_if<I==sizeof...(ColumnTypes), void>::type
        parse_line(std::stringstream&) { }

        template<std::size_t I = 0>
        inline typename std::enable_if<I<sizeof...(ColumnTypes), void>::type
        parse_line(std::stringstream& line)
        {
            try {
                std::string data;
                std::getline(line, data, delim_);
                std::get<I>(args_) = parse<get_type<I, ColumnTypes...>>(data);
            }
            catch (...) {
                std::throw_with_nested(bad_formatting("Cannot parse line"));
            }

            // keep parsing
            parse_line<I+1>(line);
        }

    public:
        explicit reader(const std::filesystem::path& path, char delim, const std::function<Input(ColumnTypes...)>& deserializer)
                :path_(path), delim_(delim), deserializer_(deserializer)
        {
            if (!std::filesystem::exists(path_))
                throw std::invalid_argument("File does not exist");
        }

        std::vector<Input> operator()()
        {
            file_ = std::ifstream{path_.string()};

            if (!file_.is_open())
                throw std::runtime_error("Cannot open "+path_.string());

            std::vector<Input> inputs;
            std::string line;

            // read header
            std::getline(file_, line);

            // read lines
            while (std::getline(file_, line)) {
                // clean
                line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

                // parse line
                std::stringstream ss(line);
                ss.exceptions(std::ios::failbit);
                parse_line(ss);

                // add input
                inputs.emplace_back(call(deserializer_, args_));
            }

            return inputs;
        }
    };
}

#endif //EMASTRATEGY_READ_HPP
