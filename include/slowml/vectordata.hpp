#pragma once
#include <slowml/data.hpp>
#include <sstream>
#include <iostream>
#include <vector>

template<class T>class VectorData : public Data<T> // {{{
{ public:
    VectorData(const std::vector<T> &values, size_t width, size_t height) // {{{
    : Data<T>(width,height)
    , myValues(values)
    { if (myValues.size()!=Data<T>::Width()*Data<T>::Height())
        throw std::string("VectorData: Constructed with wrong dimensions");
    } // }}}
    virtual ~VectorData() {}

    T GetValue(size_t instance, size_t i) const
    { return myValues[Data<T>::Width()*instance+i];
     }
    void SetValue(size_t instance, size_t i, const T &val) { myValues[Data<T>::Width()*instance+i]=val; }

    const std::vector<T> &GetValues() const { return myValues; }

    void AddRow(const std::vector<T> &inst) // {{{
    { if (inst.size()!=Data<T>::Width())
      { std::stringstream ss;
        ss << "Instance::AddRow: Instance has size " << inst.size() << " expected size " << Data<T>::Width();
        throw ss.str();
      }
      myValues.insert(myValues.end(),inst.begin(),inst.end());
      Data<T>::SetHeight(Data<T>::Height()+1);
    } // }}}

    void LoadRow(std::istream &features) // {{{
    { size_t fcount;
      features.read((char*)&fcount,sizeof(size_t));
      if (Data<T>::Width()==0)
        Data<T>::SetWidth(fcount);

      if (fcount!=Data<T>::Width())
      { std::stringstream ss;
        ss << "Instance::LoadRow: Instance has size " << fcount << " expected size " << Data<T>::Width();
        throw ss.str();
      }
      std::vector<T> row;

      for (size_t i=0; i<fcount; ++i)
      { T val;
        features.read((char*)&val,sizeof(T));
        row.push_back(val);
      }

      AddRow(row);
    } // }}}
    void SaveRow(size_t instance, std::ostream &features) // {{{
    { size_t fcount=Data<T>::Width();
      features.write((char*)&fcount,sizeof(size_t));

      for (size_t i=0; i<fcount; ++i)
      { T val=GetValue(instance,i);
        features.write((char*)&val,sizeof(T));
      }
    } // }}}


  private:
    std::vector<T> myValues;
    //matrix<double> myValues;
}; // }}}

