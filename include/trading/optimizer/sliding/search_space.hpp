//
// Created by Tomáš Petříček on 11.08.2022.
//

#ifndef EMASTRATEGY_SLIDING_SEARCH_SPACE_HPP
#define EMASTRATEGY_SLIDING_SEARCH_SPACE_HPP

namespace trading::optimizer::sliding {
    template<class Type>
    class search_space {
        range<Type> range_;
        Type shift_;

    public:
        template<int curr_idx>
        range<Type> next(Type curr)
        {
            Type end = *range_.end()+((curr_idx+1)*shift_);
            return range<Type>{curr+shift_, end, range_.step()};
        }

        range<Type> begin()
        {
            return range_;
        }

        explicit search_space(const range<Type>& range, Type shift)
                :range_(range), shift_(shift) { }
    };
}
#endif //EMASTRATEGY_SLIDING_SEARCH_SPACE_HPP
