#pragma once

#include <type_traits>
#include <utility>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/hana.hpp>


namespace hana = boost::hana;

namespace util
{
  namespace detail
  {
    template <class Container>
    using ValueType = std::conditional_t<
        std::is_const_v<Container>
      , typename Container::value_type const
      , typename Container::value_type>;


    template <class... Containers>
    using ZipValueType = hana::tuple<ValueType<Containers>...>;


    template <class Ref, class = hana::when<true>>
    struct AddConst
    {
      using type = std::add_lvalue_reference_t<std::remove_reference_t<Ref> const>;
    };


    template <class NotRef>
    struct AddConst<NotRef, hana::when<!std::is_reference_v<NotRef>>>
    {
      using type = NotRef const;
    };


    template <class T>
    using AddConst_t = typename AddConst<T>::type;


    template <class Container>
    using ReferenceType = std::conditional_t<
        std::is_const_v<Container>
      , AddConst_t<typename Container::reference>
      , typename Container::reference>;


    template <class... Containers>
    using ZipReferenceType = hana::tuple<ReferenceType<Containers>...>;


    template <class... Containers>
    class ZipIterator
      : public boost::iterator_facade <
                        ZipIterator<Containers...>
                      , ZipValueType<Containers...>
                      , boost::single_pass_traversal_tag
                      , ZipReferenceType<Containers...>
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
        // NOTICE. I cannot use hana::transform here as it uses
        // make<tuple_tag> internally which finally uses the following code:
        //          static constexpr
        //            tuple<typename detail::decay<Xs>::type...> apply(Xs&& ...xs)
        //           {
        //            return{ static_cast<Xs&&>(xs)... };
        //           }
        // Because of 'decay', reference is removed which breaks our logic.

        return hana::fuse([](auto... iterators) {
          return ZipReferenceType<Containers...>(*iterators...); })(current_);
      }


      Iterators current_;
    };
  }

  template <class... Containers>
  auto zip(Containers & ... containers)
  {
    // I do not use boost's make_zip_iterator, because it relies on boost tuple which is obsolette

    auto begin = detail::ZipIterator<Containers...>(hana::make_tuple(std::begin(containers)...));
    auto end = detail::ZipIterator<Containers...>(hana::make_tuple(std::end(containers)...));
    return boost::make_iterator_range(std::move(begin), std::move(end));
  }
}