//
// Created by Renatus Madrigal on 5/23/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_TYPETRAITS_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_TYPETRAITS_H

// This contains some enhanced type traits for the Phosphorus library.
// Thus, we use the naming style of STL instead of Google style.

namespace phosphorus {

template <typename T>
struct function_traits : function_traits<decltype(&T::operator())> {};

// TODO: Implement a general case for function traits

template <typename ClassType, typename ReturnType>
struct function_traits<ReturnType (ClassType::*)()> {
  using result_type = ReturnType;
};

template <typename ClassType, typename ReturnType, typename Arg1>
struct function_traits<ReturnType (ClassType::*)(const Arg1 &)> {
  using first_argument_type = Arg1;
  using result_type = ReturnType;
};

template <typename ClassType, typename ReturnType, typename Arg1, typename Arg2>
struct function_traits<ReturnType (ClassType::*)(const Arg1 &, const Arg2 &)
                           const> {
  using first_argument_type = Arg1;
  using second_argument_type = Arg2;
  using result_type = ReturnType;
};

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_TYPETRAITS_H
