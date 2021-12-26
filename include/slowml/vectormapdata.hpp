#pragma once
#include <slowml/data.hpp>
#include <slowml/vectormap.hpp>

class VectorMapData : public Data<double> // {{{
{ public:
    typedef const VectorData<double> internaldata;
    VectorMapData(internaldata *data, VectorMap const *map, const std::string &arg) // {{{
    : Data<double>(0,0)
    , myMap(map)
    , myArg(arg)
    { std::map<std::string,size_t> envsizes;
      envsizes[arg]=data->Width();
      Data<double>::SetWidth(map->Width(envsizes));
      Data<double>::SetHeight(data->Height());
      myEnv[myArg]=data;
    } // }}}
    virtual ~VectorMapData() {}

    double GetValue(size_t instance, size_t i) const
    { return myMap->Eval(myEnv,instance,i);
    }
    void SetValue(size_t instance, size_t i, const double &val)
    { throw std::string("SetValue called on VectorMapData");
    }

  private:
    VectorMap const *myMap;
    std::map<std::string,internaldata *> myEnv;
    std::string myArg;
}; // }}}

