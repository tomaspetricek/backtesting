//
// Created by Tomáš Petříček on 30.07.2022.
//

#ifndef EMASTRATEGY_CARTESIAN_PRODUCT_BRUTE_FORCE_HPP
#define EMASTRATEGY_CARTESIAN_PRODUCT_BRUTE_FORCE_HPP

#include <tuple>
#include <array>

#include <trading/range.hpp>
#include <trading/tuple.hpp>
#include <trading/function.hpp>
#include <trading/optimizer/brute_force.hpp>
#include <trading/pack.hpp>
#include <trading/range.hpp>
#include <trading/optimizer/cartesian_product/search_space.hpp>

namespace trading::optimizer::cartesian_product {
    template<class ...Types>
    class brute_force : public optimizer::brute_force<search_space<Types...>, sizeof...(Types),
            std::function<void(Types...)>, std::tuple<Types...>> {

    public:
        explicit brute_force(const search_space<Types...>& space, std::function<void(Types...)>&& func)
                :optimizer::brute_force<search_space<Types...>, sizeof...(Types),
                std::function<void(Types...)>, std::tuple<Types...>>(space, std::move(func)) { }
    };
}

#endif //EMASTRATEGY_CARTESIAN_PRODUCT_BRUTE_FORCE_HPP
