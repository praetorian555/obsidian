#pragma once

#include <cstdint>
#include <string>

#include "obs/obs.hpp"

#if !defined(DONT_CRASH)
#error This should not happen!
#endif

/// A global enum.
OBS_ENUM()
enum class GlobalColor
{
    Red,
    Green,
    Blue,
};

OBS_CLASS()
struct GlobalPoint
{
    OBS_PROP()
    float x = 0.0f;

    OBS_PROP()
    float y = 0.0f;
};

OBS_ENUM()
enum class EmptyEnum : int32_t
{
};

OBS_CLASS()
struct EmptyStruct
{
};

namespace FirstNamespace
{

OBS_ENUM()
enum DayOfWeek
{
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday,
};

/// This is a vegetable enum.
OBS_ENUM("flags")
enum class Vegetable : int8_t
{
    /** This is carrot. */
    Carrot = -10,
    Potato,
    /// This is cucumber.
    Cucumber,
};

namespace SecondNamespace
{
/**
 * This is a fruit enum.
 */
OBS_ENUM()
enum class Fruit
{
    Apple = 5,
    /** This is orange. */
    Orange,

    Banana
};
using namespace std;
/**
 * This is a test struct.
 */
OBS_CLASS("serializable=1")
struct DataStruct
{
    OBS_ENUM()
    enum class DataType : int16_t
    {
        A,
        B,
        C
    };

    /// The first value.
    OBS_PROP("min=0", "max=100")
    int32_t a = 1;

    /** The second value. */
    OBS_PROP()
    float b = 5.0f;

    OBS_PROP()
    const char * c = "this is test";

    OBS_PROP()
    DataType d = DataType::A;

    OBS_PROP()
    string e = "this is test";
};

/// A player entity.
OBS_CLASS("entity")
class Player
{
public:
    /// Player name.
    OBS_PROP()
    string name = "unnamed";

    OBS_PROP()
    int32_t health = 100;

    OBS_PROP()
    float speed = 5.0f;
};

} // SecondNamespace
} // FirstNamespace