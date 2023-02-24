//
// Created by Tomáš Petříček on 24.02.2023.
//

#ifndef BACKTESTING_SIMULATED_ANNEALING_PROGRESS_REPORTER_HPP
#define BACKTESTING_SIMULATED_ANNEALING_PROGRESS_REPORTER_HPP

#include <trading/simulated_anneling/optimizer.hpp>

namespace trading::simulated_annealing {

    template<class Logger>
    class progress_reporter {
        std::shared_ptr<Logger> logger_;

    public:
        explicit progress_reporter(std::shared_ptr<Logger> logger)
                :logger_{std::move(logger)} { }

        template<class Optimizer, class State>
        void begin(const Optimizer&, const State&) { }

        template<class Optimizer>
        void better_accepted(const Optimizer&) { }

        template<class Optimizer>
        void worse_accepted(const Optimizer&, double) { }

        template<class Optimizer, class State>
        void cooled(const Optimizer& optimizer, const State& curr)
        {
            *logger_ << "curr: temp: " << optimizer.current_temperature() <<
                     ", net profit: " << curr.value << std::endl;
        }

        template<class Optimizer>
        void end(const Optimizer&) { }
    };
}

#endif //BACKTESTING_SIMULATED_ANNEALING_PROGRESS_REPORTER_HPP
