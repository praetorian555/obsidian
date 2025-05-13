#pragma once

#include <cstdint>

#if !defined(DONT_CRASH)
#error This should not happen!
#endif

namespace FirstNamespace
{

// This is a vegetable enum.
enum class [[obs::refl]] Vegetable : int8_t
{
    /** This is carrot. */
    Carrot = -10,
    Potato,
    // This is cucumber.
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
    /** This is orange. */
    Orange,

    Banana
};

/**
 * This is a test struct.
 */
struct [[obs::refl]] DataStruct
{
    enum class [[obs::refl]] DataType : int16_t
    {
        A,
        B,
        C
    };

    [[obs::refl]] int32_t a = 1;
    [[obs::refl]] float b = 5.0f;
    [[obs::refl]] const char * c = "this is test";
};

} // SecondNamespace
} // FirstNamespace