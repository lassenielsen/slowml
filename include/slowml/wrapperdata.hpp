#pragma once
#include <slowml/data.hpp>
#include <vector>

template<class T> class WrapperData : public Data<T> // {{{
{ public:
    WrapperData(size_t dwidth, size_t dheight, std::vector<size_t> indexes, std::vector<T> *data=NULL);
    virtual ~WrapperData();

    T GetValue(size_t instance, size_t i) const;
    void SetValue(size_t instance, size_t i, const T &val);

    void SetData(std::vector<T> *data, size_t width, size_t height);

  private:
    std::vector<T> *myDataPtr;
    std::vector<size_t> myMap;
    size_t myDataWidth;
}; // }}}

template <class T> WrapperData<T>::WrapperData(size_t dwidth, size_t dheight, std::vector<size_t> indexes, std::vector<T> *data) // {{{
: Data<T>(indexes.size(),dheight)
, myDataPtr(NULL)
, myDataWidth(0)
, myMap(indexes)
{ Data<T>::SetWidth(myMap.size());
  // Set pointer to data
  SetData(data,dwidth,dheight);
} // }}}

template <class T> WrapperData<T>::~WrapperData() // {{{
{ // Do not delete myDataPtr
}

template <class T> T WrapperData<T>::GetValue(size_t instance, size_t i) const // {{{
{ size_t idx=myMap.at(i);
  return myDataPtr->at(instance*myDataWidth+idx);
} // }}}

template<class T> void WrapperData<T>::SetValue(size_t instance, size_t i, const T &val) // {{{
{ size_t idx=myMap.at(i);
  (*myDataPtr)[instance*myDataWidth+idx]=val;
} // }}}

template<class T> void WrapperData<T>::SetData(vector<T> *data, size_t width, size_t height) // {{{
{ myDataPtr=data;
  myDataWidth=width;
  Data<T>::SetHeight(height);

  if (myDataWidth>0 && Data<T>::Height()>0 && myDataWidth*Data<T>::Height()!=myDataPtr->size())
    throw string("Vector does not have the correct dimensions");
} // }}}
