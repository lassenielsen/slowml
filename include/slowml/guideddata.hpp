#pragma once
#include <slowml/data.hpp>

template<class T, class R>class GuidedData : public Data<T> // {{{
{ public:
    GuidedData(size_t width, size_t height) // {{{
    : Data<T>(width,height)
    { } // }}}
    virtual ~GuidedData() {}
    virtual const R GetResult(size_t instance) const=0;
}; // }}}
