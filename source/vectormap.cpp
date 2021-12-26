#include <slowml/vectormap.hpp>

dpl::SlrParser VectorMap::ourParser("vmap");
int _init_VectorMap_ourParser=VectorMap::_InitParser();

ValueMap *ValueMap::Parse(const dpl::parsetree &tree, const std::string &var) // {{{
{ if (tree.TypeCase()=="emap.map") // EID ::var -> eexp2 ::def
  { return Parse(*tree.Child("def"),tree.Child("var")->Token().Content());
  }
  else if (tree.TypeCase()=="eexp2.add") // eexp2 ::lhs + eexp1 ::rhs
  { return new ValueMapAdd(Parse(*tree.Child("lhs"),var),Parse(*tree.Child("rhs"),var));
  }
  else if (tree.TypeCase()=="eexp2.sub") // eexp2 ::lhs - eexp1 ::rhs
  { return new ValueMapSub(Parse(*tree.Child("lhs"),var),Parse(*tree.Child("rhs"),var));
  }
  else if (tree.TypeCase()=="eexp1.mul") // eexp1 ::lhs * eexp0 ::rhs
  { return new ValueMapMul(Parse(*tree.Child("lhs"),var),Parse(*tree.Child("rhs"),var));
  }
  else if (tree.TypeCase()=="eexp1.div") // eexp1 ::lhs / eexp0 ::rhs
  { return new ValueMapMul(Parse(*tree.Child("lhs"),var),Parse(*tree.Child("rhs"),var));
  }
  else if (tree.TypeCase()=="eexp0.var") // EID ::id
  { return new ValueMapVar(tree.Child("id")->Token().Content());
  }
  else if (tree.Case()=="lvl") // eexp* ::sub
  { return Parse(*tree.Child("sub"));
  }
  else
    throw std::string("Unknown emap constructor: ") + tree.TypeCase();
} // }}}

void VectorMap::ParseSlice(const dpl::parsetree &tree, size_t &from, size_t &to, size_t &step) // {{{
{ if (tree.TypeCase()=="slice.sub") // INT ::from COLON INT ::to
  { from=std::stoi(tree.Child("from")->Token().Content());
    to=std::stoi(tree.Child("to")->Token().Content());
    step=1;
  }
  else if (tree.TypeCase()=="slice.head") // COLON INT ::to
  { from=0;
    to=std::stoi(tree.Child("to")->Token().Content());
    step=1;
  }
  else if (tree.TypeCase()=="slice.tail") // INT ::from COLON
  { from=std::stoi(tree.Child("from")->Token().Content());
    to=0;
    step=1;
  }
  else if (tree.TypeCase()=="slice.fsub") // INT ::from COLON INT ::to COLON INT ::step
  { from=std::stoi(tree.Child("from")->Token().Content());
    to=std::stoi(tree.Child("to")->Token().Content());
    step=std::stoi(tree.Child("step")->Token().Content());
  }
  else if (tree.TypeCase()=="slice.fhead") // COLON INT ::to COLON INT ::step
  { from=0;
    to=std::stoi(tree.Child("to")->Token().Content());
    step=std::stoi(tree.Child("step")->Token().Content());
  }
  else if (tree.TypeCase()=="slice.ftail") // INT ::from COLON COLON INT ::step
  { from=std::stoi(tree.Child("from")->Token().Content());
    to=0;
    step=std::stoi(tree.Child("step")->Token().Content());
  }
  else
    throw std::string("Unknown slice constructor: ") + tree.TypeCase();
  if (step==0)
    throw std::string("Error in slice: step is 0!");
} // }}}

VectorMap *VectorMap::Parse(const dpl::parsetree &tree, std::string &arg) // {{{
{ if (tree.TypeCase()=="vmap.map") // VID ::var -> vexp ::def
  { arg=tree.Child("var")->Token().Content();
    return Parse(*tree.Child("def"),arg);
  }
  else if (tree.TypeCase()=="vexp.sing") // vexp3 ::sub
  { return Parse(*tree.Child("sub"),arg);
  }
  else if (tree.TypeCase()=="vexp.cons") // vexp3 ::head , vexp ::tail
  { return new VectorMapAppend(Parse(*tree.Child("head"),arg),Parse(*tree.Child("tail"),arg));
  }
  else if (tree.TypeCase()=="vexp3.mul") // vexp3 ::lhs * vexp2 ::rhs
  { return new VectorMapMult(Parse(*tree.Child("lhs"),arg),Parse(*tree.Child("rhs"),arg));
  }
  else if (tree.TypeCase()=="vexp2.map") // { emap ::map } LPAR vexp ::arg RPAR
  { return new VectorMapApply(ValueMap::Parse(*tree.Child("map")),Parse(*tree.Child("arg"),arg));
  }
  else if (tree.TypeCase()=="vexp1.idx") // vexp1 ::vec LBRAC slice ::slice RBRAC
  { size_t from, to, step;
    ParseSlice(*tree.Child("slice"),from,to,step);
    return new VectorMapSlice(Parse(*tree.Child("vec"),arg),from,to,step);
  }
  else if (tree.TypeCase()=="vexp0.var") // VID ::id
  { return new VectorMapVar(tree.Child("id")->Token().Content());
  }
  else if (tree.TypeCase()=="vexp0.par") // LPAR vexp ::sub RPAR
  { return Parse(*tree.Child("sub"),arg);
  }
  else if (tree.Case()=="lvl") // vexp* ::sub
  { return Parse(*tree.Child("sub"),arg);
  }

  throw std::string("Unknown vmap constructor: ") + tree.TypeCase();
} // }}}
