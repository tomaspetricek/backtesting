//
// Created by Tomáš Petříček on 16.02.2023.
//

#ifndef BACKTESTING_SETTING_DOCUMENT_HPP
#define BACKTESTING_SETTING_DOCUMENT_HPP

#include <nlohmann/json.hpp>

namespace trading {
    class settings_document {
        nlohmann::json settings_;

    public:
        void candles_section(nlohmann::json&& doc)
        {
            settings_.emplace(doc);
        }

        void resampling_section(nlohmann::json&& doc)
        {
            settings_.emplace(doc);
        }

        nlohmann::json get()
        {
            return settings_;
        }
    };
}

#endif //BACKTESTING_SETTING_DOCUMENT_HPP
