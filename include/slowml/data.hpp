#pragma once
#include <string>

template<class T> class Data // {{{
{ public:
    Data(size_t width, size_t height) // {{{
    : myHeight(height)
    , myWidth(width)
    {} // }}}
    virtual ~Data() {}

    virtual T GetValue(size_t instance, size_t i) const=0;
    virtual void SetValue(size_t instance, size_t i, const T &val)=0;

    void SetHeight(size_t height) // {{{
    { myHeight=height;
    } // }}}
    size_t Height() const { return myHeight; }
    void SetWidth(size_t width) // {{{
    { //if (Height()>0)
      //  throw std::string("VectorData::SetWidth: Height is not zero");
      myWidth=width;
    } // }}}
    size_t Width() const { return myWidth; }

  private:
    size_t myWidth;
    size_t myHeight;
}; // }}}
