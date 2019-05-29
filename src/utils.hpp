#pragma once

#include <iterator>
#include <algorithm>

// generic get median helper function
template <typename T>
typename T::value_type getMedian(const T& container)
{
    typename T::value_type median;
    auto temp = container;
    std::sort(std::begin(temp), std::end(temp));
    if (temp.size() % 2 == 0) {
        median = (temp[temp.size() / 2] + temp[temp.size() / 2 + 1]) / 2;
    } else {
        median = temp[temp.size() / 2];
    }
    return median;
}

// generic helper to return random element from container
// container type T must implement at() method and have T::value_type
template <typename T>
typename T::value_type random_element(const T& container)
{
    return container.at(std::rand() % container.size());
}

// return signed one (+1 or -1) integer helper functions

template <typename T>
inline constexpr T signum(T x) {
    return x/abs(x);
}