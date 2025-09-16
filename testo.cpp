#define _XOPEN_SOURCE
#include "symbol.cpp"

#include "interval.hpp"

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

    Byte buf[] = {0xf0, 0x9f, 0x92, 0xa9};

    auto s = UnicodeSymbol(buf, sizeof(buf) / sizeof(Byte));

    printf("U+%04X : w = %d\n", s.AsWchar(), s.GlyphWidth());
}
