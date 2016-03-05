#pragma once

#include <stdexcept>
#include <type_traits>

#include <boost/iterator/zip_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/iterator.hpp>


namespace util
{
  namespace detail
  {
    // TODO: boost hana's tuple provides similar functionality
    // TODO: use integral literals instead of size_t
    template <class Tuple, size_t Max, size_t N = 0>
    struct TupleElement
    {
      static constexpr auto & Get(Tuple & tuple, size_t pos)
      {
        return pos ? TupleElement<Tuple, Max, N + 1>::Get(tuple, pos - 1)
          : tuple.template get<N>();
      }
    };


    template <class Tuple, size_t Max>
    struct TupleElement<Tuple, Max, Max>
    {
      static constexpr auto & Get(Tuple & tuple, size_t)
      {
        throw std::out_of_range("You should not have called operator[] in runtime");
        return tuple.template get<0>();
      }
    };


    template <class Tuple>
    constexpr decltype(auto) GetTupleElement(Tuple & tuple, size_t pos)
    {
      return TupleElement<Tuple, boost::tuples::length<Tuple>::value>::Get(tuple, pos);
    }


    // Tuple elements must contain similar types
    template <class Tuple>
    struct TupleHelper : public Tuple
    {
      TupleHelper(Tuple && tuple) : Tuple(std::move(tuple))
      {}

      constexpr auto & operator[](size_t pos) const
      {
        // Warning. Using this operator with non-constexpr arguments is very ineffective
        return GetTupleElement(*this, pos);
      }
    };


    // I could not return auto here because of gcc which fails to deduce type of &MakeTupleHelper<Tuple>
    // in template function instantiation
    template <class Tuple>
    TupleHelper<Tuple> MakeTupleHelper(Tuple && tuple)
    {
      return TupleHelper<Tuple>(std::move(tuple));
    }

    template <class Iterator>
    auto MakeHelperIterator(Iterator iter)
    {
      using Tuple = typename Iterator::value_type;
      return boost::make_transform_iterator(iter, &MakeTupleHelper<Tuple>);
    }
  }


  template <class... T>
  auto zip(T&... containers)
  {
    auto begin = boost::iterators::make_zip_iterator(boost::make_tuple(std::begin(containers)...));
    auto end = boost::iterators::make_zip_iterator(boost::make_tuple(std::end(containers)...));

    return boost::make_iterator_range(detail::MakeHelperIterator(begin), detail::MakeHelperIterator(end));
  }
}