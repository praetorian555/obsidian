#pragma once

namespace FirstNamespace
{
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
