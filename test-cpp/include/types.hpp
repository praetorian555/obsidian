#pragma once

#include <cstdint>

#if !defined(DONT_CRASH)
#error This should not happen!
#endif

namespace FirstNamespace
{

// This is vegetable enum.
enum class [[obs::refl]] Vegetable : int8_t
{
    Carrot = -10,
    Potato,
    Cucumber,
};

namespace SecondNamespace
{
/**
 * This is a fruit enum.
 */
enum class [[obs::refl]] Fruit
{
    Apple = 5,
    Orange,
    Banana
};
} // SecondNamespace
} // FirstNamespace