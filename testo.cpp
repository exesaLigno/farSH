#define _XOPEN_SOURCE
#include "symbol.cpp"

#include "interval.hpp"

class Test
{};

#define Decode(what) what == Interval<wchar_t>::Relation::Equal ? "Equal" : \
                     what == Interval<wchar_t>::Relation::Inside ? "Inside" : \
                     what == Interval<wchar_t>::Relation::Contains ? "Contains" : \
                     what == Interval<wchar_t>::Relation::OverlapsBefore ? "OverlapsBefore" : \
                     what == Interval<wchar_t>::Relation::OverlapsAfter ? "OverlapsAfter" : \
                     what == Interval<wchar_t>::Relation::Before ? "Before" : \
                     what == Interval<wchar_t>::Relation::After ? "After" : \
                     "Ambigous"

int main()
{
    static const Interval<wchar_t> ints[] = {
        { 56 },
        { 134, 287 }
    };

    printf("%d %d\n", ints[0].Start(), ints[0].End());
    printf("%d %d\n", ints[1].Start(), ints[1].End());

    printf("%s\n", Decode(ints[1].WhereIs({445, 447})));

    Byte buf[] = {'\e', '[', '1', ';', '3', '1', 'm'};

    auto s = UnicodeSymbol(buf, sizeof(buf) / sizeof(Byte));

    s.DebugWriteTo();
    printf("\nU+%04X : w = %d\n", s.AsWchar(), s.DisplayWidth());
}
