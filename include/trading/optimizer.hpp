//
// Created by Tomáš Petříček on 31.03.2023.
//

#ifndef BACKTESTING_OPTIMIZER_HPP
#define BACKTESTING_OPTIMIZER_HPP

#include <trading/concepts.hpp>

namespace trading {
    template<class ConcreteConstraints, class State>
    concept Constraints = std::invocable<ConcreteConstraints, const State&> &&
            std::same_as<bool, std::invoke_result_t<ConcreteConstraints, const State&>>;
}

#endif //BACKTESTING_OPTIMIZER_HPP
