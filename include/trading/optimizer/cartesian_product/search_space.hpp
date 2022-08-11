//
// Created by Tomáš Petříček on 11.08.2022.
//

#ifndef EMASTRATEGY_CARTESIAN_PRODUCT_SEARCH_SPACE_HPP
#define EMASTRATEGY_CARTESIAN_PRODUCT_SEARCH_SPACE_HPP

#include <trading/range.hpp>

namespace trading::optimizer::cartesian_product {
    template<class ...Types>
    class search_space {
        std::tuple<range<Types>...> ranges_;

    public:
        template<int curr_idx>
        range<get_type<curr_idx+1, Types...>> next(get_type<curr_idx, Types...> curr)
        {
            return std::get<curr_idx+1>(ranges_);
        }

        range<get_type<0, Types...>> begin()
        {
            return std::get<0>(ranges_);
        }

        explicit search_space(range<Types>... ranges)
                :ranges_(std::forward_as_tuple(ranges...)) { }
    };
}

#endif //EMASTRATEGY_CARTESIAN_PRODUCT_SEARCH_SPACE_HPP