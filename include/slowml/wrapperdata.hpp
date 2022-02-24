#pragma once
#include <slowml/data.hpp>
#include <vector>

template<class T> class WrapperData : public Data<T> // {{{
{ public:
    WrapperData(size_t dwidth, size_t dheight, std::vector<size_t> indexes, std::vector<T> *data=NULL);
    WrapperData(size_t dwidth, size_t dheight, std::vector<size_t> indexes, Data<T> *data=NULL);
    virtual ~WrapperData();

    T GetValue(size_t instance, size_t i) const;
    void SetValue(size_t instance, size_t i, const T &val);

    const std::vector<size_t> &GetMap() const { return myMap; }
  private:
    Data<T> *myInstancesPtr;
    std::vector<T> *myDataPtr;
    std::vector<size_t> myMap;
    size_t myDataWidth;
}; // }}}

template <class T> WrapperData<T>::WrapperData(size_t dwidth, size_t dheight, std::vector<size_t> indexes, Data<T> *data) // {{{
: Data<T>(indexes.size(),dheight)
, myInstancesPtr(data)
, myDataPtr(NULL)
, myDataWidth(dwidth)
, myMap(indexes)
{ if (myDataWidth>0 && Data<T>::Height()>0 && myDataWidth*Data<T>::Height()!=myDataPtr->size())
    throw string("Vector does not have the correct dimensions");
} // }}}
template <class T> WrapperData<T>::WrapperData(size_t dwidth, size_t dheight, std::vector<size_t> indexes, std::vector<T> *data) // {{{
: Data<T>(indexes.size(),dheight)
, myInstancesPtr(NULL)
, myDataPtr(data)
, myDataWidth(dwidth)
, myMap(indexes)
{ if (myDataWidth>0 && Data<T>::Height()>0 && myDataWidth*Data<T>::Height()!=myDataPtr->size())
    throw string("Vector does not have the correct dimensions");
} // }}}

template <class T> WrapperData<T>::~WrapperData() // {{{
{ // Do not delete data
} // }}}

template <class T> T WrapperData<T>::GetValue(size_t instance, size_t i) const // {{{
{ size_t idx=myMap.at(i);
  if (myDataPtr!=NULL)
    return myDataPtr->at(instance*myDataWidth+idx);
  else
    return myInstancesPtr->GetValue(instance,idx);
} // }}}

template<class T> void WrapperData<T>::SetValue(size_t instance, size_t i, const T &val) // {{{
{ size_t idx=myMap.at(i);
  if (myDataPtr!=NULL)
    (*myDataPtr)[instance*myDataWidth+idx]=val;
  else
    myInstancesPtr->SetValue(instance,idx,val);
} // }}}
