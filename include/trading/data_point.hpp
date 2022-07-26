//
// Created by Tomáš Petříček on 26.07.2022.
//

#ifndef EMASTRATEGY_DATA_POINT_HPP
#define EMASTRATEGY_DATA_POINT_HPP

namespace trading {
    template<typename Type>
    struct data_point {
        std::time_t happened_;
        Type value_;

    public:
        data_point(time_t happened, Type value)
                :happened_(happened), value_(value) { }

        const price& value() const
        {
            return value_;
        }

        time_t happened() const
        {
            return happened_;
        }
    };
}

#endif //EMASTRATEGY_DATA_POINT_HPP
