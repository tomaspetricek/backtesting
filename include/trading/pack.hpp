//
// Created by Tomáš Petříček on 04.07.2022.
//

#ifndef EMASTRATEGY_PACK_HPP
#define EMASTRATEGY_PACK_HPP

// https://stackoverflow.com/questions/20162903/template-parameter-packs-access-nth-type-and-nth-element
template<int index, class... Types>
decltype(auto) get(Types&& ... pack)
{
    return std::get<index>(std::forward_as_tuple(pack...));
}

// https://stackoverflow.com/questions/20162903/template-parameter-packs-access-nth-type-and-nth-element
template<int idx, typename... Ts> using get_type = typename std::tuple_element<idx, std::tuple<Ts...>>::type;

template<typename... Type>
auto sum(Type... vals)
{
    return (vals + ...);
}

template<typename... Type>
double mean(Type... vals)
{
    return static_cast<double>(sum(vals...))/sizeof...(vals);
}

#endif //EMASTRATEGY_PACK_HPP
