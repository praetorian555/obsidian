#pragma once

#define OBS_ENUM(...)
#define OBS_CLASS(...)
#define OBS_PROP(...)

/// This  is an enum about fruit.
OBS_ENUM("serialize", "test=1")
enum class Fruit
{
    Apple,
    /// This is a banana.
    Banana,
    Orange,
};

OBS_ENUM() enum Color : unsigned short
{
    Red = 10,
    Blue,
    Green
};
