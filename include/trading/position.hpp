//
// Created by Tomáš Petříček on 05.07.2022.
//

#ifndef EMASTRATEGY_POSITION_HPP
#define EMASTRATEGY_POSITION_HPP

#include <ostream>

#include <trading/currency.hpp>
#include <trading/price.hpp>

namespace trading {
    class position {
        std::size_t size_;
        price price_;
        std::time_t created_;

    public:
        position(size_t size, const price& price, time_t created)
                :size_(size), price_(price), created_(created) { }

        size_t size() const
        {
            return size_;
        }
        const price& price() const
        {
            return price_;
        }
        time_t created() const
        {
            return created_;
        }
    };
}

#endif //EMASTRATEGY_POSITION_HPP
