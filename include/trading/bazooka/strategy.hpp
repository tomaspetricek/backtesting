//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_BAZOOKA_STRATEGY_HPP
#define EMASTRATEGY_BAZOOKA_STRATEGY_HPP

#include <array>
#include <memory>

#include <trading/indicator/moving_average.hpp>

namespace trading::bazooka {
    template<typename MovingAverage, std::size_t size>
    class strategy {
        std::array<MovingAverage, size> levels_;
    };
}

#endif //EMASTRATEGY_BAZOOKA_STRATEGY_HPP
