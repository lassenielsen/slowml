#pragma once
#include <string>
#include <ostream>

class Feature // {{{
{ public:
    Feature(const std::string &name, const std::string &desc, const std::string &unit)
    : myName(name)
    , myDescription(desc)
    , myUnit(unit)
    , myMin(-1.0)
    , myMax(1.0)
    , mySum(0.0)
    , myCount(0)
    {}
    virtual ~Feature() {}

    const std::string &Name() const { return myName; }
    const std::string &Description() const { return myDescription; }
    const std::string &Unit() const { return myUnit; }
    double MinValue() const { return myMin; }
    void SetMin(double val)  { myMin=val; }
    double MaxValue() const { return myMax; }
    void SetMax(double val)  { myMax=val; }
    double SumValue() const { return mySum; }
    void SetSum(double val)  { mySum=val; }
    size_t CountValue() const { return myCount; }
    void SetCount(size_t val)  { myCount=val; }

    double MeanValue() const // {{{
    { if (myCount==0)
        return 0.0;
      else
        return mySum/myCount;
    } // }}}
    double Factor() const // {{{
    { if (myMin>=myMax)
        return 1.0;
      else
        return (myMax-myMin)/2.0;
    } // }}}
    double ScaleValue(double val) const // {{{
    { return (val-MeanValue())/(Factor());
    } // }}}
    double UnscaleValue(double val) const // {{{
    { return (val*Factor())+MeanValue();
    } // }}}

    void Save(std::ostream &dest) const // {{{
    { dest << "Feature: " << Name() << std::endl
           << "Description: " << Description() << std::endl
           << "Unit: " << Unit() << std::endl
           << "Min value: " << MinValue() << std::endl
           << "Max value: " << MaxValue() << std::endl
           << "Sum: " << SumValue() << std::endl
           << "Count: " << CountValue() << std::endl;
    }

  private:
    std::string myName;
    std::string myDescription;
    std::string myUnit;
    double myMin;
    double myMax;
    double mySum;
    size_t myCount;
}; // }}}
