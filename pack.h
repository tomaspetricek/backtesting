//
// Created by Tomáš Petříček on 04.07.2022.
//

#ifndef EMASTRATEGY_PACK_H
#define EMASTRATEGY_PACK_H

// https://stackoverflow.com/questions/20162903/template-parameter-packs-access-nth-type-and-nth-element
template<int Index, class... Types>
decltype(auto) get(Types&& ... pack)
{
    return std::get<Index>(std::forward_as_tuple(pack...));
}

template<typename... Type>
auto sum(Type... vals)
{
    return (vals + ...);
}

template<typename... Type>
auto mean(Type... vals)
{
    return sum(vals...)/sizeof...(vals);
}

#endif //EMASTRATEGY_PACK_H
