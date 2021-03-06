/*
 * zip_iterator.hpp
 *
 *  Created on: Mar 22, 2015
 *      Author: belyaev
 */

#ifndef ZIP_ITERATOR_HPP_
#define ZIP_ITERATOR_HPP_

#include <iostream>

#include "iterator_adapter.hpp"
#include "copy_assignable.hpp"

namespace essentials {
namespace iterations {

template<class ItLhv, class ItRhv, class Zipper>
struct zipping_iterator_simple: copy_assignable_function<Zipper> {

    ItLhv lhv;
    ItRhv rhv;

    using copy_assignable_function<Zipper>::call;

    zipping_iterator_simple(ItLhv lhv, ItRhv rhv, Zipper zipper):
        copy_assignable_function<Zipper>(zipper),
        lhv(lhv), rhv(rhv) {}

    auto value() const -> decltype(call(*lhv, *rhv)) {
        return call(*lhv, *rhv);
    }

    void next() { ++lhv; ++rhv; }
    void next(size_t diff) { lhv += diff; rhv += diff; }
    void prev() { --lhv; --rhv; }
    void prev(size_t diff) { lhv -= diff; rhv -= diff; }
    bool equals(const zipping_iterator_simple& other) const {
        return lhv == other.lhv || rhv == other.rhv; // this is done on purpose
    }
    size_t distance(const zipping_iterator_simple& other) const {
        return std::min((size_t)std::distance(other.lhv, lhv), (size_t)std::distance(other.rhv, rhv));
    }
    int compare(const zipping_iterator_simple& other) const {
        return (lhv < other.lhv || rhv < other.rhv) ? (-1) :
               (lhv > other.lhv || rhv > other.rhv) ?   1 :
                                                        0;
    }

};


template<class LIt, class RIt, class Zipper>
auto zip_iterator(LIt lit, RIt rit, Zipper zipper) {
    return adapt_simple_iterator(
        zipping_iterator_simple<LIt, RIt, Zipper>{lit, rit, zipper},
        common_iterator_category_for<LIt, RIt>{}
    );
}

template<class LIt, class RIt, class Zipper>
using zipped_iterator = iterator_adapter<zipping_iterator_simple<LIt, RIt, Zipper>, common_iterator_category_for<LIt, RIt>>;

namespace zip_iterator_impl {

template<class T> struct remove_rvalue_reference{ using type = T; };
template<class T> struct remove_rvalue_reference<T&&> { using type = T; };
template<class T> using remove_rvalue_reference_t = typename remove_rvalue_reference<T>::type;

}

template<class LIt, class RIt>
auto zip_iterator(LIt lit, RIt rit) {
    return zip_iterator(lit, rit, [&](auto&& lhv, auto&& rhv){ 
        using LType = zip_iterator_impl::remove_rvalue_reference_t<decltype(lhv)>;
        using RType = zip_iterator_impl::remove_rvalue_reference_t<decltype(rhv)>;
        return std::pair<LType, RType>(std::forward<LType>(lhv), std::forward<RType>(rhv));
    });
}


} /* namespace iterations */
} /* namespace essentials */

#endif /* ZIP_ITERATOR_HPP_ */
