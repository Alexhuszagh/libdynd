//
// Copyright (C) 2011-15 DyND Developers
// BSD 2-Clause License, see LICENSE.txt
//

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cmath>

#include "inc_gtest.hpp"

#include <dynd/array.hpp>
#include <dynd/types/fixed_string_type.hpp>
#include <dynd/types/date_type.hpp>
#include <dynd/func/callable.hpp>
#include <dynd/func/elwise.hpp>
#include <dynd/func/take.hpp>
#include <dynd/func/compose.hpp>
#include <dynd/func/copy.hpp>
#include <dynd/func/callable_registry.hpp>
#include <dynd/func/apply.hpp>
#include <dynd/convert.hpp>

using namespace std;
using namespace dynd;

TEST(Compose, Simple)
{
  nd::callable composed = nd::functional::compose(nd::copy, func::get_regfunction("sin"), ndt::type::make<double>());
  nd::array a = nd::empty<double>();
  composed("0.0", kwds("dst", a));
  EXPECT_EQ(0., a.as<double>());
  composed("1.5", kwds("dst", a));
  EXPECT_DOUBLE_EQ(sin(1.5), a.as<double>());
  composed(3.1, kwds("dst", a));
  EXPECT_DOUBLE_EQ(sin(3.1), a.as<double>());
}

/*
TEST(Convert, Unary)
{
  nd::callable f = nd::functional::apply([](double x) { return x; });
  nd::callable g = nd::functional::convert(ndt::type("(float32) -> float64"), f);
}
*/
