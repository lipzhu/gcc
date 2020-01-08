// Copyright (C) 2019-2020 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

// { dg-options "-std=gnu++2a" }
// { dg-do compile { target c++2a } }

#include <ranges>
#include <span>
#include <string>
#include <experimental/string_view>

template<typename T>
constexpr bool
rvalue_is_safe_range()
{
  using std::ranges::safe_range;

  // An lvalue range always models safe_range
  static_assert( safe_range<T&> );
  static_assert( safe_range<const T&> );

  // Result should not depend on addition of const or rvalue-reference.
  static_assert( safe_range<T&&> == safe_range<T> );
  static_assert( safe_range<const T> == safe_range<T> );
  static_assert( safe_range<const T&&> == safe_range<T> );

  return std::ranges::safe_range<T>;
}

static_assert( rvalue_is_safe_range<std::ranges::subrange<int*, int*>>() );
static_assert( rvalue_is_safe_range<std::ranges::empty_view<int>>() );
static_assert( rvalue_is_safe_range<std::ranges::iota_view<int>>() );
static_assert( rvalue_is_safe_range<std::ranges::iota_view<int, int>>() );

static_assert( rvalue_is_safe_range<std::span<int>>() );
static_assert( rvalue_is_safe_range<std::span<int, 99>>() );

static_assert( ! rvalue_is_safe_range<std::string>() );
static_assert( ! rvalue_is_safe_range<std::wstring>() );

static_assert( rvalue_is_safe_range<std::string_view>() );
static_assert( rvalue_is_safe_range<std::wstring_view>() );

static_assert( rvalue_is_safe_range<std::experimental::string_view>() );
static_assert( rvalue_is_safe_range<std::experimental::wstring_view>() );
