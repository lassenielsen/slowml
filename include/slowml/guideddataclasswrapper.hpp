#pragma once
#include <slowml/guideddata.hpp>

class GuidedDataClassWrapper : public GuidedData<double,double> // {{{
{ public:
    GuidedDataClassWrapper(const GuidedData<double,size_t> *data, size_t c) // {{{
    : GuidedData<double,double>(data->Width(),data->Height())
    , myDataRef(*data)
    , myClass(c)
    , myTrueResult(1.0)
    , myFalseResult(0.0)
    { 
    } // }}}
    virtual ~GuidedDataClassWrapper() { }
    double GetValue(size_t instance, size_t i) const { return myDataRef.GetValue(instance,i); }
    void SetValue(size_t instance, size_t i, const double &val) { throw std::string("GuideVectorDataClassWrapper::SetValue invoked"); }
    virtual const double &GetResult(size_t instance) const { return myDataRef.GetResult(instance)==myClass?myTrueResult:myFalseResult; }

 private:
   const GuidedData<double,size_t> &myDataRef;
   size_t myClass;
   double myTrueResult, myFalseResult;
}; // }}}
