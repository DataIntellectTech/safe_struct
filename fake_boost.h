//x---------------x
//| fake_boost.h |
//x---------------x

#ifndef FAKE_BOOST_H_
#define FAKE_BOOST_H_

namespace fake_boost
{

template <typename T, int ARR_LEN=5>
struct flat_set
{
    T m_arr_[ARR_LEN];
    int m_size_{};
    int Size() const { return m_size_; }
    constexpr bool operator<=>(const flat_set&) const = default;
};

}

#endif
