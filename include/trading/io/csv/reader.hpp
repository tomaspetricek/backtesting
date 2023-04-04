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
#include <exception>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "trading/candle.hpp"
#include "trading/exception.hpp"
#include "trading/currency.hpp"
#include <trading/types.hpp>
#include "trading/tuple.hpp"
#include <trading/io/parser.hpp>
#include <trading/io/csv/base.hpp>
#include <utility>
#include <typeinfo>

namespace trading::io::csv {
    template<std::size_t n_cols>
    class reader final : public base<n_cols, std::ifstream> {
        std::string line_;
        static_assert(n_cols>0);
        using base_type = base<n_cols, std::ifstream>;

        template<class Value>
        void read_value(std::stringstream& line, Value& val)
        {
            std::string data;
            try {
                std::getline(line, data, this->delim_);
            }
            catch (...) {
                std::throw_with_nested(
                        std::runtime_error{
                                "Unable to separate value from: "+line.str()+" using delimiter: "+this->delim_});
            }
            try {
                val = parser::parse<Value>(data);
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error{
                        "Unable to parse value of type: "+std::string{typeid(Value).name()}+" from: "+data});
            }
        }

    public:
        explicit reader(const std::filesystem::path& path, char delim = base_type::default_delim)
                :base_type{delim}
        {
            if (!std::filesystem::exists(path))
                throw std::invalid_argument("File does not exist");

            this->file_ = typename base_type::file_stream_type{path.string()};

            if (!this->file_.is_open())
                throw std::runtime_error("Cannot open "+path.string());
        }

        bool read_header(std::array<std::string, n_cols>& header)
        {
            if (!std::getline(this->file_, line_)) return false;

            // parse line
            std::stringstream ss(line_);
            ss.exceptions(std::ios::failbit);

            for (std::size_t i{0}; i<header.size(); i++)
                read_value(ss, header[i]);

            return true;
        }

        template<class ...Types>
        bool read_row(Types& ...inputs)
        {
            static_assert(sizeof...(Types)==n_cols);
            if (!std::getline(this->file_, line_)) return false;

            // clean
            line_.erase(std::remove(line_.begin(), line_.end(), '\r'), line_.end());

            // convert to string stream
            std::stringstream ss(line_);
            ss.exceptions(std::ios::failbit);

            auto parse_line = [&](auto& in) {
                read_value(ss, in);
            };

            (parse_line(inputs), ...);
            return true;
        }
    };
}

#endif //EMASTRATEGY_READ_HPP
