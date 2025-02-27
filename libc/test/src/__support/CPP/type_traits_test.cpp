//===-- Unittests for type_traits -----------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/__support/CPP/type_traits.h"
#include "test/UnitTest/Test.h"

// TODO: Split this file if it becomes too big.

namespace __llvm_libc::cpp {

class Class {};
union Union {};
struct Struct {};
enum Enum {};
enum class EnumClass {};

using UnqualObjectTypes = testing::TypeList<int, float, Class, Union, Struct>;

TYPED_TEST(LlvmLibcTypeTraitsTest, add_lvalue_reference, UnqualObjectTypes) {
  // non-ref cv, adds ref
  EXPECT_TRUE((is_same_v<add_lvalue_reference_t<T>, T &>));
  EXPECT_TRUE((is_same_v<add_lvalue_reference_t<const T>, const T &>));
  EXPECT_TRUE((is_same_v<add_lvalue_reference_t<volatile T>, volatile T &>));
  EXPECT_TRUE((
      is_same_v<add_lvalue_reference_t<const volatile T>, const volatile T &>));

  // pointer cv, adds ref
  EXPECT_TRUE((is_same_v<add_lvalue_reference_t<T *>, T *&>));
  EXPECT_TRUE((is_same_v<add_lvalue_reference_t<const T *>, const T *&>));
  EXPECT_TRUE((is_same_v<add_lvalue_reference_t<volatile T *>, volatile T *&>));
  EXPECT_TRUE((is_same_v<add_lvalue_reference_t<const volatile T *>,
                         const volatile T *&>));

  // ref cv, returns same type
  EXPECT_TRUE((is_same_v<add_lvalue_reference_t<T &>, T &>));
  EXPECT_TRUE((is_same_v<add_lvalue_reference_t<const T &>, const T &>));
  EXPECT_TRUE((is_same_v<add_lvalue_reference_t<volatile T &>, volatile T &>));
  EXPECT_TRUE((is_same_v<add_lvalue_reference_t<const volatile T &>,
                         const volatile T &>));
}

TEST(LlvmLibcTypeTraitsTest, add_lvalue_reference_void) {
  // void cannot be referenced
  EXPECT_TRUE((is_same_v<add_lvalue_reference_t<void>, void>));
  EXPECT_TRUE((is_same_v<add_lvalue_reference_t<const void>, const void>));
  EXPECT_TRUE(
      (is_same_v<add_lvalue_reference_t<volatile void>, volatile void>));
  EXPECT_TRUE((is_same_v<add_lvalue_reference_t<const volatile void>,
                         const volatile void>));
}

TYPED_TEST(LlvmLibcTypeTraitsTest, add_pointer, UnqualObjectTypes) {
  // object types -> pointer type
  EXPECT_TRUE((is_same_v<add_pointer_t<T>, T *>));
  EXPECT_TRUE((is_same_v<add_pointer_t<const T>, const T *>));
  EXPECT_TRUE((is_same_v<add_pointer_t<volatile T>, volatile T *>));
  EXPECT_TRUE((is_same_v<add_pointer_t<const volatile T>, const volatile T *>));

  // pointer types -> pointer type
  EXPECT_TRUE((is_same_v<add_pointer_t<T *>, T **>));
  EXPECT_TRUE((is_same_v<add_pointer_t<const T *>, const T **>));
  EXPECT_TRUE((is_same_v<add_pointer_t<volatile T *>, volatile T **>));
  EXPECT_TRUE(
      (is_same_v<add_pointer_t<const volatile T *>, const volatile T **>));

  // reference type -> pointer type
  EXPECT_TRUE((is_same_v<add_pointer_t<T &>, T *>));
  EXPECT_TRUE((is_same_v<add_pointer_t<const T &>, const T *>));
  EXPECT_TRUE((is_same_v<add_pointer_t<volatile T &>, volatile T *>));
  EXPECT_TRUE(
      (is_same_v<add_pointer_t<const volatile T &>, const volatile T *>));
}

TEST(LlvmLibcTypeTraitsTest, add_pointer_void) {
  // void -> pointer type
  EXPECT_TRUE((is_same_v<add_pointer_t<void>, void *>));
  EXPECT_TRUE((is_same_v<add_pointer_t<const void>, const void *>));
  EXPECT_TRUE((is_same_v<add_pointer_t<volatile void>, volatile void *>));
  EXPECT_TRUE(
      (is_same_v<add_pointer_t<const volatile void>, const volatile void *>));
}

TYPED_TEST(LlvmLibcTypeTraitsTest, add_rvalue_reference, UnqualObjectTypes) {

  // non-ref cv, adds ref
  EXPECT_TRUE((is_same_v<add_rvalue_reference_t<T>, T &&>));
  EXPECT_TRUE((is_same_v<add_rvalue_reference_t<const T>, const T &&>));
  EXPECT_TRUE((is_same_v<add_rvalue_reference_t<volatile T>, volatile T &&>));
  EXPECT_TRUE((is_same_v<add_rvalue_reference_t<const volatile T>,
                         const volatile T &&>));

  // ref cv, returns same type
  EXPECT_TRUE((is_same_v<add_rvalue_reference_t<T &>, T &>));
  EXPECT_TRUE((is_same_v<add_rvalue_reference_t<const T &>, const T &>));
  EXPECT_TRUE((is_same_v<add_rvalue_reference_t<volatile T &>, volatile T &>));
  EXPECT_TRUE((is_same_v<add_rvalue_reference_t<const volatile T &>,
                         const volatile T &>));
}

TEST(LlvmLibcTypeTraitsTest, add_rvalue_reference_void) {
  // void cannot be referenced
  EXPECT_TRUE((is_same_v<add_rvalue_reference_t<void>, void>));
  EXPECT_TRUE((is_same_v<add_rvalue_reference_t<const void>, const void>));
  EXPECT_TRUE(
      (is_same_v<add_rvalue_reference_t<volatile void>, volatile void>));
  EXPECT_TRUE((is_same_v<add_rvalue_reference_t<const volatile void>,
                         const volatile void>));
}

TEST(LlvmLibcTypeTraitsTest, bool_constant) {
  EXPECT_TRUE((bool_constant<true>::value));
  EXPECT_FALSE((bool_constant<false>::value));
}

TEST(LlvmLibcTypeTraitsTest, conditional_t) {
  EXPECT_TRUE((is_same_v<conditional_t<true, int, float>, int>));
  EXPECT_TRUE((is_same_v<conditional_t<false, int, float>, float>));
}

TEST(LlvmLibcTypeTraitsTest, decay) {
  EXPECT_TRUE((is_same_v<decay_t<int>, int>));

  // array decay
  EXPECT_TRUE((is_same_v<decay_t<int[2]>, int *>));
  EXPECT_TRUE((is_same_v<decay_t<int[2]>, int *>));
  EXPECT_TRUE((is_same_v<decay_t<int[2][4]>, int(*)[4]>));

  // cv ref decay
  EXPECT_TRUE((is_same_v<decay_t<int &>, int>));
  EXPECT_TRUE((is_same_v<decay_t<const int &>, int>));
  EXPECT_TRUE((is_same_v<decay_t<volatile int &>, int>));
  EXPECT_TRUE((is_same_v<decay_t<const volatile int &>, int>));
}

// TODO enable_if

TEST(LlvmLibcTypeTraitsTest, false_type) { EXPECT_FALSE((false_type::value)); }

TEST(LlvmLibcTypeTraitsTest, integral_constant) {
  EXPECT_EQ((integral_constant<int, 4>::value), 4);
}

using IntegralAndFloatingTypes =
    testing::TypeList<bool, char, short, int, long, long long, unsigned char,
                      unsigned short, unsigned int, unsigned long,
                      unsigned long long, float, double, long double>;

TYPED_TEST(LlvmLibcTypeTraitsTest, is_arithmetic, IntegralAndFloatingTypes) {
  EXPECT_TRUE((is_arithmetic_v<T>));
  EXPECT_TRUE((is_arithmetic_v<const T>));
  EXPECT_TRUE((is_arithmetic_v<volatile T>));
  EXPECT_TRUE((is_arithmetic_v<const volatile T>));

  EXPECT_FALSE((is_arithmetic_v<T *>));
  EXPECT_FALSE((is_arithmetic_v<T &>));
}

TEST(LlvmLibcTypeTraitsTest, is_arithmetic_non_integral) {
  EXPECT_FALSE((is_arithmetic_v<Union>));
  EXPECT_FALSE((is_arithmetic_v<Class>));
  EXPECT_FALSE((is_arithmetic_v<Struct>));
  EXPECT_FALSE((is_arithmetic_v<Enum>));
}

TEST(LlvmLibcTypeTraitsTest, is_array) {
  EXPECT_FALSE((is_array_v<int>));
  EXPECT_FALSE((is_array_v<float>));
  EXPECT_FALSE((is_array_v<Struct>));
  EXPECT_FALSE((is_array_v<int *>));

  EXPECT_TRUE((is_array_v<Class[]>));
  EXPECT_TRUE((is_array_v<Union[4]>));
}

TEST(LlvmLibcTypeTraitsTest, is_base_of) {
  struct A {};
  EXPECT_TRUE((is_base_of_v<A, A>));

  // Test public, protected and private inheritance.
  struct B : public A {};
  EXPECT_TRUE((is_base_of_v<A, B>));
  EXPECT_FALSE((is_base_of_v<B, A>));

  struct C : protected A {};
  EXPECT_TRUE((is_base_of_v<A, C>));
  EXPECT_FALSE((is_base_of_v<C, A>));

  struct D : private A {};
  EXPECT_TRUE((is_base_of_v<A, D>));
  EXPECT_FALSE((is_base_of_v<D, A>));

  // Test inheritance chain.
  struct E : private B {};
  EXPECT_TRUE((is_base_of_v<A, E>));
}

TEST(LlvmLibcTypeTraitsTest, is_class) {
  EXPECT_TRUE((is_class_v<Struct>));
  EXPECT_TRUE((is_class_v<Class>));

  // Pointer or ref do not qualify.
  EXPECT_FALSE((is_class_v<Class *>));
  EXPECT_FALSE((is_class_v<Class &>));

  // Neither other types.
  EXPECT_FALSE((is_class_v<Union>));
  EXPECT_FALSE((is_class_v<int>));
  EXPECT_FALSE((is_class_v<EnumClass>));
}

TYPED_TEST(LlvmLibcTypeTraitsTest, is_const, UnqualObjectTypes) {
  EXPECT_FALSE((is_const_v<T>));
  EXPECT_TRUE((is_const_v<const T>));

  using Aliased = const T;
  EXPECT_TRUE((is_const_v<Aliased>));
}

// TODO is_convertible

TYPED_TEST(LlvmLibcTypeTraitsTest, is_destructible, UnqualObjectTypes) {
  EXPECT_TRUE((is_destructible_v<T>));
}
TEST(LlvmLibcTypeTraitsTest, is_destructible_no_destructor) {
  struct S {
    ~S() = delete;
  };
  EXPECT_FALSE((is_destructible_v<S>));
}

TYPED_TEST(LlvmLibcTypeTraitsTest, is_enum, UnqualObjectTypes) {
  EXPECT_FALSE((is_enum_v<T>));
}
TEST(LlvmLibcTypeTraitsTest, is_enum_enum) {
  EXPECT_TRUE((is_enum_v<Enum>));
  EXPECT_TRUE((is_enum_v<EnumClass>));
}

// TODO is_floating_point

// TODO is_function

// TODO is_integral

// TODO is_lvalue_reference

// TODO is_member_pointer

// TODO is_null_pointer

TEST(LlvmLibcTypeTraitsTest, is_object) {
  EXPECT_TRUE((is_object_v<int>));      // scalar
  EXPECT_TRUE((is_object_v<Struct[]>)); // array
  EXPECT_TRUE((is_object_v<Union>));    // union
  EXPECT_TRUE((is_object_v<Class>));    // class

  // pointers are still objects
  EXPECT_TRUE((is_object_v<int *>));       // scalar
  EXPECT_TRUE((is_object_v<Struct(*)[]>)); // array
  EXPECT_TRUE((is_object_v<Union *>));     // union
  EXPECT_TRUE((is_object_v<Class *>));     // class

  // reference are not objects
  EXPECT_FALSE((is_object_v<int &>));       // scalar
  EXPECT_FALSE((is_object_v<Struct(&)[]>)); // array
  EXPECT_FALSE((is_object_v<Union &>));     // union
  EXPECT_FALSE((is_object_v<Class &>));     // class

  // not an object
  EXPECT_FALSE((is_object_v<void>));
}

// TODO is_pointer

// TODO is_reference

// TODO is_rvalue_reference

// TODO is_same

// TODO is_scalar

// TODO is_signed

// TODO is_trivially_constructible

// TODO is_trivially_copyable

// TODO is_trivially_destructible

// TODO is_union

// TODO is_unsigned

// TODO is_void

// TODO make_signed

// TODO make_unsigned

// TODO remove_all_extents

// TODO remove_cv

// TODO remove_cvref

// TODO remove_extent

// TODO remove_reference

TEST(LlvmLibcTypeTraitsTest, true_type) { EXPECT_TRUE((true_type::value)); }

// TODO type_identity

// TODO void_t

} // namespace __llvm_libc::cpp
