//
// Created by Tomáš Petříček on 27.06.2022.
//

#ifndef EMASTRATEGY_READ_H
#define EMASTRATEGY_READ_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>

#include "candle.h"
#include "exceptions.h"

namespace trading {
    std::vector<candle> read_candles(const std::filesystem::path& path, char delim,
            const std::chrono::seconds& period)
    {
        std::ifstream file(path.string());

        if (!file.is_open())
            throw std::runtime_error("Cannot open "+path.string());

        std::vector<candle> candles;
        std::string line, data;
        long curr_created, bef_created = 0;
        double open, high, low, close;

        // read header
        std::getline(file, line);

        while (std::getline(file, line)) {
            // clean
            line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
            std::stringstream ss(line);
            ss.exceptions(std::ios::failbit);

            try {
                std::getline(ss, data, delim);
                curr_created = std::stol(data);

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

            if (bef_created!=0)
                if (period.count()!=(curr_created-bef_created))
                    throw std::invalid_argument("Incorrect duration between candles");

            bef_created = curr_created;

            try {
                candles.emplace_back(curr_created, open, high, low, close);
            }
            catch (...) {
                std::throw_with_nested(std::runtime_error("Cannot create candle"));
            }
        }

        return candles;
    }
}

#endif //EMASTRATEGY_READ_H
