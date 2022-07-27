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

namespace trading {
    std::vector<candle> read_candles(const std::filesystem::path& path, char delim,
            const std::chrono::seconds& period)
    {
        if (!std::filesystem::exists(path))
            throw std::invalid_argument("File does not exist");

        if (path.extension()!=".csv")
            throw std::invalid_argument("Invalid file extension");

        std::ifstream file(path.string());

        if (!file.is_open())
            throw std::runtime_error("Cannot open "+path.string());

        std::vector<candle> candles;
        std::string line, data;
        std::time_t curr_opened, bef_opened = 0;
        double open, high, low, close;
        boost::posix_time::ptime opened;

        // read header
        std::getline(file, line);

        // read lines
        while (std::getline(file, line)) {
            // clean
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
            std::stringstream ss(line);
            ss.exceptions(std::ios::failbit);

            // parse line
            try {
                std::getline(ss, data, delim);
                curr_opened = std::stol(data);

                std::getline(ss, data, delim);
                open = std::stod(data);

                std::getline(ss, data, delim);
                high = std::stod(data);

                std::getline(ss, data, delim);
                low = std::stod(data);

                std::getline(ss, data, delim);
                close = std::stod(data);
            }
            catch (...) {
                std::throw_with_nested(bad_formatting("Cannot parse line"));
            }

            if (curr_opened<0)
                throw std::invalid_argument("Candle time has to greater or equal to 0");

            // check duration between
            if (bef_opened!=0)
                if (period.count()!=(curr_opened-bef_opened))
                    throw std::invalid_argument("Incorrect duration between candles");

            bef_opened = curr_opened;

            // create ptime
            try {
                opened = boost::posix_time::from_time_t(curr_opened);
            }
            catch (...) {
                std::throw_with_nested(std::invalid_argument("Cannot create posix time"));
            }

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
}

#endif //EMASTRATEGY_READ_HPP
