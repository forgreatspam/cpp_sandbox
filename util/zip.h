#pragma once

#include <exception>
#include <tuple>
#include <boost/iterator/zip_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/iterator.hpp>


namespace util
{
  namespace detail
  {
    // Tuple must contain similartypes
    template <class Tuple>
    struct TupleHelper : Tuple
    {
      TupleHelper(Tuple && tuple) : Tuple(std::move(tuple))
      {}

      constexpr auto & operator[](size_t pos) const
      {
        // Warning. Using this operator with non-constexpr arguments is very ineffective
        return GetTupleElement_(pos);
      }

    private:
      template <size_t N = 0>
      constexpr auto & GetTupleElement_(size_t pos) const
      {
        return pos ? GetTupleElement_<N + 1>(pos - 1) : get<N>();
      }

      template <>
      constexpr auto & GetTupleElement_<boost::tuples::length<Tuple>::value>(size_t) const
      {
        throw std::out_of_range("Too big tuple");
        return get<0>();
      }
    };


    template <class Tuple>
    auto MakeTupleHelper(Tuple && tuple)
    {
      return TupleHelper<Tuple>(std::move(tuple));
    }

    template <class Iterator>
    auto MakeHelperIterator(Iterator iter)
    {
      return boost::make_transform_iterator(iter, &MakeTupleHelper<Iterator::value_type>);
    }
  }


  template <class... T>
  auto zip(T&... containers)
  {
    using Tuple = boost::tuple<typename T::value_type...>;
    auto begin = boost::iterators::make_zip_iterator(boost::make_tuple(std::begin(containers)...));
    auto end = boost::iterators::make_zip_iterator(boost::make_tuple(std::end(containers)...));

    return boost::make_iterator_range(detail::MakeHelperIterator(begin), detail::MakeHelperIterator(end));
  }
}