#pragma once
#include <vector>
#include <array>
#include <numeric>
#include <iterator>
#include <type_traits>

#include <boost/mpl/list.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/hana.hpp>

#include "zip.h"
#include "range.h"
#include "unused.h"


// TODO: use namespaces

using Container = std::vector<int>;
size_t const DIM = 2;
size_t const SIZE = 5;


struct ZipTest
  : public ::testing::Test
{
  ZipTest()
  {
    Container::value_type ii = 0;
    for (auto & container : containers)
      for (auto _ UNUSED : util::range(SIZE))
        container.push_back(ii++);
  }

  std::array<Container, DIM> containers;
};


TEST_F(ZipTest, Mutable)
{
  using namespace boost::hana::literals;

  static_assert(DIM >= 2, "DIM should be at least two");
  
  for (auto it : util::zip(containers[0], containers[1]))
  {
    it[0_c] += it[1_c];
  }

  Container correct;
  for (auto ii : util::range(SIZE))
    correct.push_back(ii * 2 + SIZE);

  EXPECT_THAT(containers[0], ::testing::ContainerEq(correct));
}


template <class Ref>
constexpr bool IsReferenceToConst(Ref & ref)
{
  return std::is_const_v<std::remove_reference_t<Ref>>;
}


TEST_F(ZipTest, NonMutable)
{
  using namespace boost::hana::literals;

  static_assert(DIM >= 2, "DIM should be at least two");

  Container const & constContainer = containers[0];
  auto it = *std::begin(util::zip(constContainer, containers[1]));

  EXPECT_TRUE(IsReferenceToConst(it[0_c]));
  EXPECT_FALSE(IsReferenceToConst(it[1_c]));
}
