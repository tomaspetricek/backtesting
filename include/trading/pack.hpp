//
// Created by Tomáš Petříček on 04.07.2022.
//

#ifndef EMASTRATEGY_PACK_HPP
#define EMASTRATEGY_PACK_HPP

// src: https://stackoverflow.com/questions/20162903/template-parameter-packs-access-nth-type-and-nth-element
template<int index, class... Types>
decltype(auto) get(Types&& ... pack)
{
    return std::get<index>(std::forward_as_tuple(pack...));
}

// src: https://stackoverflow.com/questions/20162903/template-parameter-packs-access-nth-type-and-nth-element
template<int idx, typename... Ts> using get_type = typename std::tuple_element<idx, std::tuple<Ts...>>::type;

template<typename... Type>
auto sum(Type... vals)
{
    return (vals + ...);
}

template <class Real, class... Ts>
Real mean(Ts... vals)
{
    static_assert(std::is_floating_point<Real>::value);
    return static_cast<Real>(sum(vals...))/sizeof...(vals);
}

#endif //EMASTRATEGY_PACK_HPP
