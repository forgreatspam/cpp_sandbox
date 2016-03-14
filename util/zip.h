#pragma once

#include <type_traits>
#include <utility>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator.hpp>
#include <boost/hana.hpp>


namespace hana = boost::hana;

namespace util
{
  namespace detail
  {
    template <class... Containers>
    class ZipIterator
      : public boost::iterator_facade<
                  ZipIterator<Containers...>
                , hana::tuple<typename Containers::value_type...>
                , boost::single_pass_traversal_tag
                , hana::tuple<typename Containers::value_type...>
              >
    {
    public:
      using Iterators = hana::tuple<decltype(std::begin(std::declval<Containers &>()))...>;

      ZipIterator(Iterators && iterators)
        : current_(std::move(iterators))
      {}

    private:
      friend class boost::iterator_core_access;

      void increment()
      {
        hana::for_each(current_, [](auto & iterator) { ++iterator; });
      }

      bool equal(ZipIterator const & other) const
      {
        return this->current_ == other.current_;
      }

      auto dereference() const
      {
        return hana::transform(current_, [](auto & iterator) { return *iterator; });

      }

      Iterators current_;
    };
  }

  template <class... Containers>
  auto zip(Containers & ... containers)
  {
    auto begin = detail::ZipIterator<Containers...>(hana::make_tuple(std::begin(containers)...));
    auto end = detail::ZipIterator<Containers...>(hana::make_tuple(std::end(containers)...));
    return boost::make_iterator_range(std::move(begin), std::move(end));
  }
}