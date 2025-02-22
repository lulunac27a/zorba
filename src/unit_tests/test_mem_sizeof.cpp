/*
 * Copyright 2006-2016 zorba.io
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "stdafx.h"
#include <iostream>
#include <string>

#include <zorba/internal/cxx_util.h>

#include "util/mem_sizeof.h"

using namespace std;
using namespace zorba;

///////////////////////////////////////////////////////////////////////////////

static int failures;

static bool assert_true( char const *expr, int line, bool result ) {
  if ( !result ) {
    cout << "FAILED, line " << line << ": " << expr << endl;
    ++failures;
  }
  return result;
}

#define ASSERT_TRUE( EXPR ) assert_true( #EXPR, __LINE__, !!(EXPR) )

///////////////////////////////////////////////////////////////////////////////

static void test_int() {
  int i;
  ASSERT_TRUE( ztd::mem_sizeof( i ) == sizeof( i ) );
}

///////////////////////////////////////////////////////////////////////////////

static void test_map_string_int() {
  typedef map<string,int> map_type;
  map_type m;
  string const key( "a" );
  m[ key ] = 1;

  size_t const expected_size = sizeof( m )
    + sizeof( map_type::value_type ) + key.capacity() + 1
    + sizeof( void* ) * 2;

  ASSERT_TRUE( ztd::mem_sizeof( m ) == expected_size );
}

///////////////////////////////////////////////////////////////////////////////

struct point {
  int x, y, z;
};

static void test_pod() {
  point p;
  ASSERT_TRUE( ztd::mem_sizeof( p ) == sizeof( p ) );
}

///////////////////////////////////////////////////////////////////////////////

static void test_pointer() {
  int *p = nullptr;
  ASSERT_TRUE( ztd::mem_sizeof( p ) == sizeof( p ) );
  p = new int;
  ASSERT_TRUE( ztd::mem_sizeof( p ) == sizeof( p ) + sizeof( int ) );
  delete p;
}

///////////////////////////////////////////////////////////////////////////////

template<class StringType>
static void test_string_empty() {
  StringType const s;
  ASSERT_TRUE( ztd::mem_sizeof( s ) == sizeof( s ) + 1 );
}

template<>
void test_string_empty<zstring>() {
  zstring const s;
  ASSERT_TRUE(
    ztd::mem_sizeof( s ) == sizeof( s ) + 1 + sizeof( zstring::rep_type )
  );
}

template<class StringType>
static void test_string_not_empty() {
  StringType const s( "hello" );
  ASSERT_TRUE( ztd::mem_sizeof( s ) == sizeof( s ) + s.capacity() + 1 );
}

template<>
void test_string_not_empty<zstring>() {
  zstring const s( "hello" );
  ASSERT_TRUE(
    ztd::mem_sizeof( s ) ==
      sizeof( s ) + sizeof( zstring::rep_type ) + s.capacity() + 1
  );
}

///////////////////////////////////////////////////////////////////////////////

static void test_vector_int() {
  typedef vector<int> vector_type;
  vector_type v;
  v.push_back( 1 );

  size_t const expected_size = sizeof( v )
    + v.size() * sizeof( vector_type::value_type )
    + (v.capacity() - v.size()) * sizeof( vector_type::value_type );

  ASSERT_TRUE( ztd::mem_sizeof( v ) == expected_size );
}

///////////////////////////////////////////////////////////////////////////////

struct my_base {
  virtual size_t alloc_size() const {
    return ztd::alloc_sizeof( b1_ ) + ztd::alloc_sizeof( b2_ );
  }
  virtual size_t dynamic_size() const {
    return sizeof( *this );
  }

  string b1_, b2_;
};

struct my_derived : my_base {
  my_derived( string const &s ) : d1_( s ) { }

  size_t alloc_size() const {
    return my_base::alloc_size() + ztd::alloc_sizeof( d1_ );
  }
  virtual size_t dynamic_size() const {
    return sizeof( *this );
  }

  string d1_;
};

static void test_base_empty() {
  my_base b;
  ASSERT_TRUE(
    ztd::mem_sizeof( b ) == sizeof( b )
      + b.b1_.capacity() + 1
      + b.b2_.capacity() + 1
  );
}

static void test_derived_not_empty() {
  string const s( "hello" );
  my_derived d( s );
  ASSERT_TRUE(
    ztd::mem_sizeof( d ) == sizeof( d )
      + d.b1_.capacity() + 1
      + d.b2_.capacity() + 1
      + d.d1_.capacity() + 1
  );
}

///////////////////////////////////////////////////////////////////////////////

namespace zorba {
namespace UnitTests {

int test_mem_sizeof( int, char*[] ) {
  test_int();
  test_pointer();
  test_pod();

  test_string_empty<string>();
  test_string_not_empty<string>();
  test_string_empty<zstring>();
  test_string_not_empty<zstring>();

  test_map_string_int();
  test_vector_int();

  test_base_empty();
  test_derived_not_empty();

  cout << failures << " test(s) failed\n";
  return failures ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace UnitTests
} // namespace zorba
/* vim:set et sw=2 ts=2: */
