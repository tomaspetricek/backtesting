//
// Created by Tomáš Petříček on 20.02.2023.
//

#ifndef BACKTESTING_CONCEPTS_HPP
#define BACKTESTING_CONCEPTS_HPP

#include <type_traits>
#include <concepts>

namespace trading {
    template<class ConcreteTerminationCriteria, class Optimizer>
    concept TerminationCriteria = std::invocable<ConcreteTerminationCriteria, const Optimizer&> &&
            std::same_as<bool, std::invoke_result_t<ConcreteTerminationCriteria, const Optimizer&>>;

    template<class ConcreteFitnessFunction, class Config>
    concept FitnessFunction = std::invocable<ConcreteFitnessFunction, const Config&> &&
            std::same_as<double, std::invoke_result_t<ConcreteFitnessFunction, const Config&>>;
}

#endif //BACKTESTING_CONCEPTS_HPP
