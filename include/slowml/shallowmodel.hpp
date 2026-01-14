#pragma once

#include<slowml/model.hpp>

template<class Result> class ShallowModel : public Model<Result> // {{{
{ public:
    ShallowModel();
    virtual ~ShallowModel();

    virtual void Clear() { myParameters.clear(); }
    size_t CountParameters() const { return myParameters.size(); }
    void AddParameter(double parameter=0.0) { myParameters.push_back(parameter); }
    void SetParameter(size_t i, double val) { myParameters[i]=val; }
    double GetParameter(size_t i) const { return myParameters[i]; }
    const std::vector<double> &Parameters() const { return myParameters; }
    std::vector<double> &Parameters() { return myParameters; }

    virtual void AddDelta(const vector<vector<double> > &inputs, size_t pos, std::vector<double> &deltasum, const Result &diff)=0;

    virtual void SaveParameters(std::ostream &dest, bool saveSize=true) const // {{{
    { size_t count=CountParameters();
      if (saveSize)
        dest.write((const char*)&count,sizeof(size_t));
      for (size_t i=0; i<count; ++i)
      { double val=GetParameter(i);
        dest.write((const char*)&val,sizeof(double));
      }
    } // }}}
    virtual void LoadParameters(std::istream &src, bool loadSize=true) // {{{
    { size_t count;
      if (loadSize)
        src.read((char*)&count,sizeof(size_t));
      else
        count=CountParameters();

      Clear();
      for (size_t i=0; i<count; ++i)
      { double val;
        src.read((char*)&val,sizeof(double));
        AddParameter(val);
      }
    } // }}}

  private:
    std::vector<double> myParameters;
}; // }}}

template<class Result> ShallowModel<Result>::ShallowModel() // {{{
{ AddParameter();
} // }}}
template<class Result> ShallowModel<Result>::~ShallowModel() // {{{
{
} // }}}
