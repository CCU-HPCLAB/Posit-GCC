// Copyright (C) 2018-2019 Free Software Foundation, Inc.
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

// { dg-options "-std=gnu++17" }
// { dg-do compile { target c++17 } }

#include <unordered_map>
#include <memory_resource>

struct X { };
struct Y { };
struct Hash { std::size_t operator()(X) const { return 0; } };
struct Eq { bool operator()(X, X) const { return true; } };

static_assert(std::is_same_v<std::pmr::unordered_map<X, Y>,
    std::unordered_map<X, Y, std::hash<X>, std::equal_to<X>,
      std::pmr::polymorphic_allocator<std::pair<const X, Y>>>>);
static_assert(std::is_same_v<std::pmr::unordered_map<X, Y, Hash>,
    std::unordered_map<X, Y, Hash, std::equal_to<X>,
      std::pmr::polymorphic_allocator<std::pair<const X, Y>>>>);
static_assert(std::is_same_v<std::pmr::unordered_map<X, Y, Hash, Eq>,
    std::unordered_map<X, Y, Hash, Eq,
      std::pmr::polymorphic_allocator<std::pair<const X, Y>>>>);
