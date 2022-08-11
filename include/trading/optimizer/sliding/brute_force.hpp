//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_SLIDING_BRUTE_FORCE_HPP
#define EMASTRATEGY_SLIDING_BRUTE_FORCE_HPP

#include <tuple>
#include <array>

#include <trading/range.hpp>
#include <trading/tuple.hpp>
#include <trading/function.hpp>
#include <trading/optimizer/brute_force.hpp>
#include <trading/optimizer/sliding/search_space.hpp>

namespace trading::optimizer::sliding {
    template<typename Type, std::size_t size>
    class brute_force : public optimizer::brute_force<search_space<Type>, size,
            function_of<void, Type, size>, tuple_of<Type, size>> {
    public:
        explicit brute_force(const search_space<Type>& space, function_of<void, Type, size>&& func)
                :optimizer::brute_force<search_space<Type>, size, function_of<void, Type, size>, tuple_of<Type, size>>
                         (space, std::move(func)) { }
    };
}

#endif //EMASTRATEGY_SLIDING_BRUTE_FORCE_HPP
