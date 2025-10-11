#pragma once

#include <cassert>
#include <cstdint>
#include <concepts>

template<std::totally_ordered T>
class Interval
{
private:
    T start = 0;
    T end = 0;

    bool inline IsStrange() const { return start > end; }

public:
    Interval(T _single_point) : start(_single_point), end(_single_point) { }
    Interval(T _start, T _end) : start(_start), end(_end) { assert(_start <= _end); }

    T inline Start() const noexcept { return start; }
    T inline End() const noexcept { return end; }
    bool inline Contains(const T _single_point) const noexcept { return _single_point >= start and _single_point <= end; }

    enum class Relation : uint8_t
    {
        Equal,
        Inside,
        Contains,
        OverlapsBefore,
        OverlapsAfter,
        Before,
        After,
        Ambigous
    };

    Relation inline WhereIs(const T _single_point) const
    {
        return IsStrange() ? Relation::Ambigous :
               _single_point < start ? Relation::Before :
               _single_point > end   ? Relation::After :
               Relation::Inside;
    }

    Relation WhereIs(const Interval<T>& other) const
    {
        if (IsStrange())
            return Relation::Ambigous;
            
        if (start == other.start and end == other.end)
            return Relation::Equal;
        
        Relation start_rel = WhereIs(other.start);
        Relation end_rel = WhereIs(other.end);

        if (start_rel == Relation::Inside and end_rel == Relation::Inside)
            return Relation::Inside;

        else if (start_rel == Relation::Before and end_rel == Relation::After)
            return Relation::Contains;

        else if (start_rel == Relation::Before and end_rel == Relation::Inside)
            return Relation::OverlapsBefore;
        
        else if (start_rel == Relation::Inside and end_rel == Relation::After)
            return Relation::OverlapsAfter;

        else if (start_rel == Relation::Before and end_rel == Relation::Before)
            return Relation::Before;

        else if (start_rel == Relation::After and end_rel == Relation::After)
            return Relation::After;

        return Relation::Ambigous;
    }
};
