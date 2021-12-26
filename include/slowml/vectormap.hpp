#pragma once

#include <slowml/vectordata.hpp>
#include <slowml/vectormap_bnf.hpp>
#include <dpl/slrparser.hpp>

class ValueMap // {{{
{ public:
    ValueMap() // {{{
    {
    }  // }}}
    virtual ~ValueMap() {}
    virtual double Eval(double arg)=0;

    static ValueMap *Parse(const dpl::parsetree &tree, const std::string &var="");
}; // }}}
class ValueMapAdd : public ValueMap // {{{
{ public:
    ValueMapAdd(ValueMap *lhs, ValueMap *rhs) // {{{
    : myLhs(lhs)
    , myRhs(rhs)
    {
    }  // }}}
    virtual ~ValueMapAdd() // {{{
    { delete myLhs;
      delete myRhs;
    } // }}}
    double Eval(double arg) // {{{
    { double lhs=myLhs->Eval(arg);
      double rhs=myRhs->Eval(arg);
      return lhs+rhs;
    } // }}}
  private:
    ValueMap *myLhs;
    ValueMap *myRhs;
}; // }}}
class ValueMapSub : public ValueMap // {{{
{ public:
    ValueMapSub(ValueMap *lhs, ValueMap *rhs) // {{{
    : myLhs(lhs)
    , myRhs(rhs)
    {
    }  // }}}
    virtual ~ValueMapSub() // {{{
    { delete myLhs;
      delete myRhs;
    } // }}}
    double Eval(double arg) // {{{
    { double lhs=myLhs->Eval(arg);
      double rhs=myRhs->Eval(arg);
      return lhs-rhs;
    } // }}}
  private:
    ValueMap *myLhs;
    ValueMap *myRhs;
}; // }}}
class ValueMapMul : public ValueMap // {{{
{ public:
    ValueMapMul(ValueMap *lhs, ValueMap *rhs) // {{{
    : myLhs(lhs)
    , myRhs(rhs)
    {
    }  // }}}
    virtual ~ValueMapMul() // {{{
    { delete myLhs;
      delete myRhs;
    } // }}}
    double Eval(double arg) // {{{
    { double lhs=myLhs->Eval(arg);
      double rhs=myRhs->Eval(arg);
      return lhs*rhs;
    } // }}}
  private:
    ValueMap *myLhs;
    ValueMap *myRhs;
}; // }}}
class ValueMapDiv : public ValueMap // {{{
{ public:
    ValueMapDiv(ValueMap *lhs, ValueMap *rhs) // {{{
    : myLhs(lhs)
    , myRhs(rhs)
    {
    }  // }}}
    virtual ~ValueMapDiv() // {{{
    { delete myLhs;
      delete myRhs;
    } // }}}
    double Eval(double arg) // {{{
    { double lhs=myLhs->Eval(arg);
      double rhs=myRhs->Eval(arg);
      return lhs/rhs;
    } // }}}
  private:
    ValueMap *myLhs;
    ValueMap *myRhs;
}; // }}}
class ValueMapVar : public ValueMap // {{{
{ public:
    ValueMapVar(const std::string &name) // {{{
    : myName(name)
    {
    }  // }}}
    virtual ~ValueMapVar() // {{{
    {
    } // }}}
    double Eval(double arg) // {{{
    { return arg;
    } // }}}
  private:
    std::string myName;
}; // }}}

class VectorMap // {{{
{ public:
    VectorMap() // {{{
    {
    }  // }}}
    virtual ~VectorMap() {}
    virtual double Eval(const std::map<std::string,const VectorData<double>* > &env, size_t instance, size_t index) const=0;
    virtual size_t Width(std::map<std::string,size_t> &env) const=0;

    static void ParseSlice(const dpl::parsetree &tree, size_t &from, size_t &to, size_t &step);
    static VectorMap *Parse(const dpl::parsetree &tree, std::string &arg);
    static VectorMap *Parse(const std::string &code, std::string &arg) // {{{
    { dpl::parsetree *tree=ourParser.Parse(code);
      VectorMap *result=Parse(*tree,arg);
      delete tree;
      return result;
    } // }}}
    static int _InitParser() // {{{
    { ourParser.Load(std::string((char*)bnf_vectormap_bnf,bnf_vectormap_bnf_len));
      return 1;
    } // }}}
  private:
    static dpl::SlrParser ourParser;
}; // }}}
class VectorMapAppend : public VectorMap // {{{
{ public:
    VectorMapAppend(VectorMap *lhs, VectorMap *rhs) // {{{
    : myLhs(lhs)
    , myRhs(rhs)
    {} // }}}
    ~VectorMapAppend() // {{{
    { delete myLhs;
      delete myRhs;
    } // }}}
    double Eval(const std::map<std::string,const VectorData<double>* > &env, size_t instance, size_t index) const // {{{
    { std::map<std::string,size_t> widths;
      for (auto it=env.begin(); it!=env.end(); ++it)
        widths[it->first]=it->second->Width();
      size_t lhsWidth=myLhs->Width(widths);
      if (index<lhsWidth)
        return myLhs->Eval(env,instance,index);
      else
        return myRhs->Eval(env,instance,index-lhsWidth);
    } // }}}
    size_t Width(std::map<std::string,size_t> &env) const // {{{
    { return myLhs->Width(env)+myRhs->Width(env);
    } // }}}
  private:
    VectorMap *myLhs;
    VectorMap *myRhs;
}; // }}}
class VectorMapMult : public VectorMap // {{{
{ public:
    VectorMapMult(VectorMap *lhs, VectorMap *rhs) // {{{
    : myLhs(lhs)
    , myRhs(rhs)
    {} // }}}
    ~VectorMapMult() // {{{
    { delete myLhs;
      delete myRhs;
    } // }}}
    double Eval(const std::map<std::string,const VectorData<double>* > &env, size_t instance, size_t index) const // {{{
    { std::map<std::string,size_t> widths;
      for (auto it=env.begin(); it!=env.end(); ++it)
        widths[it->first]=it->second->Width();
      size_t rhsWidth=myRhs->Width(widths);
      size_t lhsIndex=index/rhsWidth;
      size_t rhsIndex=index%rhsWidth;
      return myLhs->Eval(env,instance,lhsIndex)*myRhs->Eval(env,instance,rhsIndex);
    } // }}}
    size_t Width(std::map<std::string,size_t> &env) const // {{{
    { return myLhs->Width(env)*myRhs->Width(env);
    } // }}}
  private:
    VectorMap *myLhs;
    VectorMap *myRhs;
}; // }}}
class VectorMapApply : public VectorMap // {{{
{ public:
    VectorMapApply(ValueMap *map, VectorMap *arg) // {{{
    : myMap(map)
    , myArg(arg)
    {} // }}}
    ~VectorMapApply() // {{{
    { delete myMap;
      delete myArg;
    } // }}}
    double Eval(const std::map<std::string,const VectorData<double>* > &env, size_t instance, size_t index) const // {{{
    { double arg=myArg->Eval(env,instance,index);
      return myMap->Eval(arg);
    } // }}}
    size_t Width(std::map<std::string,size_t> &env) const // {{{
    { return myArg->Width(env);
    } // }}}
  private:
    ValueMap *myMap;
    VectorMap *myArg;
}; // }}}
class VectorMapSlice : public VectorMap // {{{
{ public:
    VectorMapSlice(VectorMap *arg, size_t from, size_t to, size_t step) // {{{
    : myArg(arg)
    , myFrom(from)
    , myTo(to)
    , myStep(step)
    {} // }}}
    ~VectorMapSlice() // {{{
    { delete myArg;
    } // }}}
    double Eval(const std::map<std::string,const VectorData<double>* > &env, size_t instance, size_t index) const // {{{
    { return myArg->Eval(env,instance,myFrom+myStep*index);
    } // }}}
    size_t Width(std::map<std::string,size_t> &env) const // {{{
    { size_t argWidth=myArg->Width(env);
      size_t to=myTo==0?argWidth:myTo;
      if (myFrom>to)
        return 0;
      return (to-myFrom)/myStep;
    } // }}}
  private:
    VectorMap *myArg;
    size_t myFrom, myTo, myStep;
}; // }}}
class VectorMapVar : public VectorMap // {{{
{ public:
    VectorMapVar(const std::string &name) // {{{
    : myName(name)
    {} // }}}
    ~VectorMapVar() // {{{
    {
    } // }}}
    double Eval(const std::map<std::string,const VectorData<double>* > &env, size_t instance, size_t index) const // {{{
    { auto var=env.find(myName);
      if (var==env.end())
        throw std::string("VectorMapVar: Unbound name ") + myName;
      return var->second->GetValue(instance,index);
    } // }}}
    size_t Width(std::map<std::string,size_t> &env) const // {{{
    { return env[myName];
    } // }}}
  private:
    std::string myName;
}; // }}}
