#pragma once
#include <slowml/guideddata.hpp>
#include <slowml/vectordata.hpp>
#include <vector>
#include <iostream>

template<class T, class R>class GuidedVectorData : public GuidedData<T,R> // {{{
{ public:
    GuidedVectorData(Data<T> &data, const std::vector<R> &results) // {{{
    : GuidedData<T,R>(data.Width(),data.Height())
    , myData(data)
    , myResults(results,1,data.Height())
    {
    } // }}}
    virtual ~GuidedVectorData() {}

    T GetValue(size_t instance, size_t i) const { return myData.GetValue(instance,i); }
    void SetValue(size_t instance, size_t i, const T &val) { myData.SetValue(instance,i,val); }

    const Data<T> &GetData() const { return myData; }
    virtual const R GetResult(size_t instance) const
    { return myResults.GetValue(instance,0);
    }

    //void AddRow(const std::vector<T> &inst, const R result) // {{{
    //{ myData.AddRow(inst);
    //  myResults.AddRow(std::vector<R>{ result });
    //  GuidedData<T,R>::SetHeight(Data<T>::Height()+1);
    //} // }}}

  private:
    Data<T> &myData;
    VectorData<R> myResults;
}; // }}}
