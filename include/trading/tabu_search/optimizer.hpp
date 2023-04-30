//
// Created by Tomáš Petříček on 20.02.2023.
//

#ifndef BACKTESTING_TABU_SEARCH_OPTIMIZER_HPP
#define BACKTESTING_TABU_SEARCH_OPTIMIZER_HPP

#include <unordered_map>
#include <trading/state.hpp>
#include <trading/interface.hpp>

namespace trading::tabu_search {
    template<class State, class Move, ITabuList<Move> TabuList>
    class optimizer {
        TabuList tabu_list_;
        State best_state_, curr_state_;
        Move curr_move_, candidate_move_;
        std::size_t it_{0};
        using state_t = State;
        using config_t = typename state_t::config_type;
        using move_t = Move;

    public:
        explicit optimizer(TabuList tabu_list)
                :tabu_list_(tabu_list) { }

        void operator()(const config_t& init,
                IResult<state_t> auto& result,
                IConstraints<state_t> auto&& constraints,
                IObjectiveFunction<state_t> auto&& objective,
                INeighbor<config_t, move_t> auto&& neighbor,
                INeighborhoodSizer<optimizer> auto&& neighborhood,
                ITerminationCriteria<optimizer> auto&& terminate,
                IAspirationCriteria<state_t, optimizer> auto&& aspire,
                IObserver<optimizer> auto& ... observers)
        {
            best_state_ = curr_state_ = objective(init);
            state_t candidate, origin;

            (observers.started(*this), ...);
            for (it_ = 0; !terminate(*this); it_++) {
                // explore neighborhood
                origin = curr_state_;
                std::tie(curr_state_.config, curr_move_) = neighbor(origin.config);
                curr_state_ = objective(curr_state_.config);

                for (std::size_t i{0}; i<neighborhood(*this)-1; i++) {
                    std::tie(candidate.config, candidate_move_) = neighbor(origin.config);
                    candidate = objective(candidate.config);

                    if ((!tabu_list_.contains(candidate_move_) && result.compare(candidate, curr_state_))
                        || aspire(candidate, *this)) {
                        curr_state_ = candidate;
                        curr_move_ = candidate_move_;
                    }
                }

                // try update best state
                if (result.compare(curr_state_, best_state_)) {
                    best_state_ = curr_state_;
                    if (constraints(best_state_))
                        result.update(best_state_);
                }

                tabu_list_.forget();
                tabu_list_.remember(curr_move_);
                (observers.iteration_passed(*this), ...);
            }
            (observers.finished(*this), ...);
        }

        const state_t& best_state() const
        {
            return best_state_;
        }

        const state_t& current_state() const
        {
            return curr_state_;
        }

        const TabuList& tabu_list() const
        {
            return tabu_list_;
        }

        std::size_t it() const
        {
            return it_;
        }
    };
}

#endif //BACKTESTING_TABU_SEARCH_OPTIMIZER_HPP