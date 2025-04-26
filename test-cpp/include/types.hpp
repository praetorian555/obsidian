#pragma once

#include <cstdint>

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