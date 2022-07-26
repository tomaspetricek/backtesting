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

        static std::size_t validate_size(std::size_t size)
        {
            if (size<=0)
                throw std::invalid_argument("Position size has to be greater than 0");

            return size;
        }

    public:
        position(std::size_t size, const price& price, time_t created)
                :size_(validate_size(size)), price_(price), created_(created) { }

        // avoid implicit conversion
        template<typename Type>
        position(Type size, const price& price, time_t created) = delete;

        const price& price() const
        {
            return price_;
        }
        time_t created() const
        {
            return created_;
        }

        std::size_t size() const
        {
            return size_;
        }
    };
}

#endif //EMASTRATEGY_POSITION_HPP
