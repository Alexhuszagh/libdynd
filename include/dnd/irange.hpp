//
// Copyright (C) 2011-12, Dynamic NDArray Developers
// BSD 2-Clause License, see LICENSE.txt
//

#ifndef _DND__IRANGE_HPP_
#define _DND__IRANGE_HPP_

#include <limits>

#include <dnd/config.hpp>

namespace dynd {

/**
 * This class represents a range of indices, or a single index.
 *
 * The single index is indicated by a step of 0, in which case the
 * value of m_start is taken as that index. This form removes the
 * dimension it is indexing from the array.
 *
 * The range of indices is the set {m_start, m_start + m_step, ...
 * m_start + k * m_step} as large as possible where each element
 * is less than m_finish when m_step is positive, or greater than
 * m_finish when m_step is negative.
 *
 * The ways to construct this class do no error checking, in order
 * to fit the requirements of C++ DND_CONSTEXPR.
 *
 * Typical ways to construct ranges are as follows:
 *
 * irange()                 // The full index range [begin, end)
 * 3 <= irange()            // The index range [3, end)
 * 2 <= irange() < 10       // The index range [2, 10)
 * 1 < irange() < 5         // The index range [2, 5)
 * 3 <= irange() / 2 < 10   // The indices {3, 5, 7, 9}
 *
 * TODO: Should negative indices be supported with the meaning as in Python?
 */
class irange {
    intptr_t m_start, m_finish, m_step;
public:
    /*DND_CONSTEXPR*/ irange()
        : m_start(std::numeric_limits<intptr_t>::min()),
          m_finish(std::numeric_limits<intptr_t>::max()), m_step(1) {
    }

    DND_CONSTEXPR irange(intptr_t idx)
        : m_start(idx), m_finish(idx), m_step(0) {
    }

    DND_CONSTEXPR irange(intptr_t start, intptr_t finish, intptr_t step = 1)
        : m_start(start), m_finish(finish), m_step(step) {
    }

    DND_CONSTEXPR const intptr_t& start() const {
        return m_start;
    }

    DND_CONSTEXPR const intptr_t& finish() const {
        return m_finish;
    }

    DND_CONSTEXPR const intptr_t& step() const {
        return m_step;
    }

    void set_start(intptr_t value) {
        m_start = value;
    }

    void set_finish(intptr_t value) {
        m_finish = value;
    }

    void set_step(intptr_t value) {
        m_step = value;
    }

    /**
     * The notation "irange() / step" is a way to specify
     * the step of the range. Think of it as the word "by".
     */
    DND_CONSTEXPR irange operator/(intptr_t step) {
        return irange(m_start, m_finish, step);
    }

    /**
     * The notation "irange() < finish" is a way to specify
     * the end of a range with a positive step.
     */
    DND_CONSTEXPR irange operator<(intptr_t finish) {
        return irange(m_start, finish, m_step);
    }

    /**
     * The notation "irange() <= last" is a way to specify
     * the end of a range with a positive step.
     */
    DND_CONSTEXPR irange operator<=(intptr_t last) {
        return irange(m_start, (last != -1) ? (last+1) : std::numeric_limits<intptr_t>::max(), m_step);
    }

    /**
     * The notation "irange() > finish" is a way to specify
     * the end of a range with a negative step.
     */
    DND_CONSTEXPR irange operator>(intptr_t finish) {
        return irange(m_start, finish, m_step);
    }

    /**
     * The notation "irange() >= last" is a way to specify
     * the end of a range with a negative step.
     */
    DND_CONSTEXPR irange operator>=(intptr_t last) {
        return irange(m_start, (last != 0) ? (last-1) : std::numeric_limits<intptr_t>::max(), m_step);
    }

    friend DND_CONSTEXPR irange operator<(intptr_t start_minus_one, const irange& i);
    friend DND_CONSTEXPR irange operator<=(intptr_t start, const irange& i);
    friend DND_CONSTEXPR irange operator>(intptr_t start_plus_one, const irange& i);
    friend DND_CONSTEXPR irange operator>=(intptr_t start, const irange& i);
};

/**
 * The notation "lower < irange()" is a way to specify
 * the beginning of a range with a positive step.
 */
inline DND_CONSTEXPR irange operator<(intptr_t start_minus_one, const irange& i) {
    return irange(start_minus_one + 1, i.m_finish, i.m_step);
}

/**
 * The notation "start <= irange()" is a way to specify
 * the beginning of a range with a positive step.
 */
inline DND_CONSTEXPR irange operator<=(intptr_t start, const irange& i) {
    return irange(start, i.m_finish, i.m_step);
}

/**
 * The notation "upper > irange()" is a way to specify
 * the beginning of a range with a negative step.
 */
inline DND_CONSTEXPR irange operator>(intptr_t start_plus_one, const irange& i) {
    return irange(start_plus_one - 1, i.m_finish, i.m_step);
}

/**
 * The notation "start >= irange()" is a way to specify
 * the beginning of a range with a negative step.
 */
inline DND_CONSTEXPR irange operator>=(intptr_t start, const irange& i) {
    return irange(start, i.m_finish, i.m_step);
}

} // namespace dynd

#endif // _DND__IRANGE_HPP_
