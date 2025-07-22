// Copyright (c) Guillem Serra. All Rights Reserved.

#pragma once

class Interval
{
public:
    double min, max;

    Interval();

    Interval(double min, double max);

    double size() const
    {
        return max - min;
    }

    bool contains(double x) const
    {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const
    {
        return min < x && x < max;
    }

    static const Interval empty, universe;
};