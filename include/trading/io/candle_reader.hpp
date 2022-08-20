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

#include <trading/candle.hpp>
#include <trading/exception.hpp>
#include <trading/currency.hpp>

namespace trading::io {
    class candle_reader final {
        std::ifstream file_;
        std::filesystem::path path_;
        char delim_;
        std::chrono::seconds period_;

    public:
        explicit candle_reader(const std::filesystem::path& path, char delim, const std::chrono::seconds& period)
                :path_(path), delim_(delim), period_(period)
        {
            if (!std::filesystem::exists(path_))
                throw std::invalid_argument("File does not exist");
        }

        std::vector<candle> operator()()
        {
            file_ = std::ifstream{path_.string()};

            if (!file_.is_open())
                throw std::runtime_error("Cannot open "+path_.string());

            std::vector<candle> candles;
            std::string line, data;
            std::time_t curr_opened{0}, bef_opened{0};
            double open, high, low, close;
            boost::posix_time::ptime opened;

            // read header
            std::getline(file_, line);

            // read lines
            while (std::getline(file_, line)) {
                // clean
                line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
                std::stringstream ss(line);
                ss.exceptions(std::ios::failbit);

                // parse line
                try {
                    std::getline(ss, data, delim_);
                    curr_opened = std::stol(data);

                    std::getline(ss, data, delim_);
                    open = std::stod(data);

                    std::getline(ss, data, delim_);
                    high = std::stod(data);

                    std::getline(ss, data, delim_);
                    low = std::stod(data);

                    std::getline(ss, data, delim_);
                    close = std::stod(data);
                }
                catch (...) {
                    std::throw_with_nested(bad_formatting("Cannot parse line"));
                }

                if (curr_opened<0)
                    throw std::invalid_argument("Candle time has to greater or equal to 0");

                if (open<0 || high<0 || low<0 || close<0)
                    throw std::invalid_argument("Price has to be greater or equal to 0");

//            // check duration between
//            if (bef_opened!=0)
//                if (period.count()!=(curr_opened-bef_opened))
//                    throw std::invalid_argument("Incorrect duration between candles");

                bef_opened = curr_opened;

                // convert to ptime
                opened = boost::posix_time::from_time_t(curr_opened);

                // add candle
                try {
                    candles.emplace_back(opened, open, high, low, close);
                }
                catch (...) {
                    std::throw_with_nested(std::runtime_error("Cannot create candle"));
                }
            }

            return candles;
        }
    };
}

#endif //EMASTRATEGY_READ_HPP
