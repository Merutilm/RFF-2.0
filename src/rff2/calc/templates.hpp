//
// Created by Merutilm on 8/25/26.
//
#pragma once
#include <concepts>
template<typename T>
   concept Number = std::totally_ordered<T> && std::is_trivially_copyable_v<T> && requires(T a, T b) {
    { a + b } -> std::same_as<T>;
    { a - b } -> std::same_as<T>;
    { a * b } -> std::same_as<T>;
    { a / b } -> std::same_as<T>;
   };