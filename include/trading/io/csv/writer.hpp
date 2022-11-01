//
// Created by Tomáš Petříček on 20.08.2022.
//

#ifndef BACKTESTING_CSV_WRITER_HPP
#define BACKTESTING_CSV_WRITER_HPP

#include <filesystem>
#include <fstream>
#include <utility>
#include "trading/table.hpp"

namespace trading::io::csv {
    class writer final {
        std::ofstream file_;
        const std::filesystem::path path_;
        const char delim_;
        std::ios_base::openmode mode_{std::ios_base::out};

    public:
        explicit writer(const std::filesystem::path& path, char delim = ',')
                :path_(path), delim_(delim)
        {
            file_ = std::ofstream{path.string(), mode_};

            if (!file_.is_open())
                throw std::runtime_error("Cannot open "+path.string());
        }

        template<std::size_t size>
        void write_line(const std::array<std::string, size>& data)
        {
            char sep;
            for (int i{0}; i<size; i++) {
                sep = (i<size-1) ? delim_ : '\n';
                file_ << data[i] << sep;
            }
        }
    };
}

#endif //BACKTESTING_CSV_WRITER_HPP
