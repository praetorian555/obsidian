#pragma once

#include <cstdint>

#if !defined(DONT_CRASH)
#error This should not happen!
#endif

namespace FirstNamespace
{

enum class [[obs::refl]] Vegetable : int8_t
{
    Carrot = -10,
    Potato,
    Cucumber,
};

namespace SecondNamespace
{
enum class [[obs::refl]] Fruit
{
    Apple = 5,
    Orange,
    Banana
};
} // SecondNamespace
} // FirstNamespace