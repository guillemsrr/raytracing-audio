#include "Interval.h"
#include <cfloat>

Interval::Interval(): min(FLT_MAX), max(-FLT_MAX)
{
}

Interval::Interval(double min, double max) : min(min), max(max)
{
}

const Interval Interval::empty = Interval(+FLT_MAX, -FLT_MAX);
const Interval Interval::universe = Interval(-FLT_MAX, +FLT_MAX);