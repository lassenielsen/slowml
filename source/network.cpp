#include <slowml/network.hpp>
#include <slowml/network_bnf.hpp>
#include <slowml/logisticregressionmodel.hpp>
#include <dpl/slrparser.hpp>
#include <cstdlib>
#include <cmath>

using namespace std;

Network::Network(size_t input_size) // {{{
: myInputSize(input_size)
{
} // }}}

Network::~Network() // {{{
{ for (auto level=myNodes.begin(); level!=myNodes.end(); ++level)
  { for (auto node=level->begin(); node!=level->end(); ++node)
    { delete node->second;
    }
  }

  myNodes.clear();
} // }}}

size_t Network::AddNode(size_t layer, const vector<size_t> &inputs) // {{{
{ cout << "Network::AddNode(" << layer << "," << inputs.size() << ")" << endl;
  while (myNodes.size()<=layer)
    myNodes.push_back(vector<Node>());
  
  Node node(WrapperData<double>(0,0,inputs,NULL),new LogisticRegressionModel());
  if (inputs[0]!=0)
    cout << "Network::AddNode: input 0 is not used.";
  for (size_t i=1; i<inputs.size(); ++i)
    node.second->AddParameter((500.0-rand()%1000)/100.0);
  myNodes[layer].push_back(node);
  return myNodes[layer].size()-1;
} // }}}

vector<double> Network::Eval(const Data<double> &instances, size_t pos) // {{{
{ if (instances.Width()!=myInputSize)
    throw string("Network::Eval input width does not match");
  
  vector<double> state;
  for (size_t i=0; i<myInputSize; ++i)
    state.push_back(instances.GetValue(pos,i));

  //cout << "Network::Eval evaluating " << myNodes.size() << "layers" << endl;
  for (auto layer=myNodes.begin(); layer!=myNodes.end(); ++layer)
  { // Debug state
    //cout << "Network::Eval state: ";
    //for (auto it=state.begin(); it!=state.end(); ++it)
    //{ if (it!=state.begin())
    //    cout << ", ";
    //  cout << *it;
    //}
    //cout << endl;

    vector<double> newstate;
    if (layer+1!=myNodes.end())
      newstate.push_back(1.0);
    for (auto node=layer->begin(); node!=layer->end(); ++node)
    { node->first.SetData(&state,state.size(),1);
      double result=node->second->Eval(node->first,0);
      node->first.SetData(NULL,0,0);
      newstate.push_back(result);
    }
    state=newstate;
  }
  // Debug state
  //cout << "Network::Eval final state: ";
  //for (auto it=state.begin(); it!=state.end(); ++it)
  //{ if (it!=state.begin())
  //    cout << ", ";
  //  cout << *it;
  //}
  //cout << endl;
  return state;
} // }}}
vector<double> Network::Eval(const vector<double> &instance) // {{{
{ if (instance.size()!=myInputSize)
    throw string("Network::Eval input width does not match");
  
  vector<double> state(instance.begin(),instance.end());

  //cout << "Network::Eval evaluating " << myNodes.size() << "layers" << endl;
  for (auto layer=myNodes.begin(); layer!=myNodes.end(); ++layer)
  { // Debug state
    //cout << "Network::Eval state: ";
    //for (auto it=state.begin(); it!=state.end(); ++it)
    //{ if (it!=state.begin())
    //    cout << ", ";
    //  cout << *it;
    //}
    //cout << endl;

    vector<double> newstate;
    if (layer+1!=myNodes.end())
      newstate.push_back(1.0);
    for (auto node=layer->begin(); node!=layer->end(); ++node)
    { node->first.SetData(&state,state.size(),1);
      double result=node->second->Eval(node->first,0);
      node->first.SetData(NULL,0,0);
      newstate.push_back(result);
    }
    state=newstate;
  }
  // Debug state
  //cout << "Network::Eval final state: ";
  //for (auto it=state.begin(); it!=state.end(); ++it)
  //{ if (it!=state.begin())
  //    cout << ", ";
  //  cout << *it;
  //}
  //cout << endl;
  return state;
} // }}}

double Network::LogDistance(const vector<double> &guess, const vector<double> &truth) // {{{
{ if (guess.size()!=truth.size())
    throw string("Network::LogDistance input width does not match");
  double dist=0.0;
  for (size_t i=0; i<guess.size(); ++i)
  { dist+= -truth[i]*log(guess[i])-(1-truth[i])*log(1-guess[i]);
  }
  return dist;
} // }}}

double Network::Cost(const GuidedData<double,vector<double>> &instances, double lambda) // {{{
{ double cost=0;
  for (size_t i=0; i<instances.Height(); ++i)
    cost += LogDistance(Eval(instances,i),instances.GetResult(i));
  cost = cost/(double)instances.Height();

  // Add Regularization
  for (size_t layer=0; layer<myNodes.size(); ++layer)
    for (size_t node=0; node<myNodes[layer].size(); ++node)
      for (size_t param=1; param<myNodes[layer][node].second->CountParameters(); ++param)
        cost +=lambda*pow(myNodes[layer][node].second->GetParameter(param),2.0)/(2.0*instances.Height());
  return cost;
} // }}}
inline double sigmoid_deriv(double val) // {{{
{ return val*(1.0-val);
} // }}}
std::vector<double> Network::Delta(const GuidedData<double,vector<double> > &instances, double lambda) // {{{
{ vector<double> flat_delta(CountParameters(),0.0);
  for (size_t pos=0; pos<instances.Height(); ++pos)
  { 
    //// Debug: parameters {{{
    //cout << "Network::Delta parameters" << endl;
    //for (size_t layer=0; layer<myNodes.size(); ++layer)
    //{ cout << "layer " << layer << ": ";
    //  for (size_t node=0; node<myNodes[layer].size(); ++node)
    //  { if (node>0)
    //      cout << "; ";
    //    for (size_t i=0; i<myNodes[layer][node].second->CountParameters(); ++i)
    //    { if (i>0)
    //        cout << ",";
    //      cout << myNodes[layer][node].second->GetParameter(i);
    //    }
    //  }
    //  cout << endl;
    //} // }}}
    vector< vector<double> > signals;
    // Calculate signals - myNodes[l][n] outputs signals[l+1][n+1]
    signals.push_back(vector<double>(instances.Width()));
    for (size_t i=0; i<instances.Width(); ++i)
      signals[0][i]=instances.GetValue(pos,i);
    for (size_t layer=0; layer<myNodes.size(); ++layer)
    { signals.push_back(vector<double>());
      signals[layer+1].push_back(1.0);
      for (size_t node=0; node<myNodes[layer].size(); ++node)
      { Node &n(myNodes[layer][node]);
        n.first.SetData(&(signals[layer]),signals[layer].size(),1);
        signals[layer+1].push_back(n.second->Eval(n.first,0));
      }
    }

    //// Debug: signals {{{
    //cout << "Network::Delta signals" << endl;
    //for (size_t layer=0; layer<signals.size(); ++layer)
    //{ cout << "layer " << layer << ": ";
    //  for (size_t node=0; node<signals[layer].size(); ++node)
    //  { if (node>0)
    //      cout << ", ";
    //    cout << signals[layer][node];
    //  }
    //  cout << endl;
    //} // }}}

    // Calculate delta - myNodes[l][n] has error delta[l][n+1]
    vector< vector<double> > delta;
    while (delta.size()<myNodes.size())
      delta.push_back(vector<double>(myNodes[delta.size()].size()+1,0.0));
    
    // Compute delta for the last layer
    for (size_t i=0; i+1<delta[delta.size()-1].size(); ++i)
    { //cout << "Result: " << instances.GetResult(pos)[i] << endl;
      delta[delta.size()-1][i+1]=signals[signals.size()-1][i+1]-instances.GetResult(pos)[i];
    }
    // Backpropagate delta
    for (int layer=delta.size()-2; layer>=0; --layer)
    { //Computer delata[layer] from delta[layer+1], signals[layer] and parameters
      // delta[l]=Theta[l]'*delta[l+1] . signals(l) . (1-signals(l))
      for (size_t node=0; node<myNodes[layer+1].size(); ++node)
      { Node &n(myNodes[layer+1][node]);
        n.first.SetData(&(delta[layer]),delta[layer].size(),1);
        for (size_t param=0; param<myNodes[layer+1][node].second->CountParameters(); ++param)
          n.first.SetValue(0,param,n.first.GetValue(0,param)+n.second->GetParameter(param)*delta[layer+1][node+1]);
        // Reset data reference
        n.first.SetData(NULL,0,0);
      }
      // Multiply by signals(l) * (1-signals(l))
      for (size_t node=0; node<delta[layer].size(); ++node)
        delta[layer][node]=delta[layer][node]*sigmoid_deriv(signals[layer+1][node]);
    }

    //// Debug delta {{{
    //cout << "Network::Delta delta" << endl;
    //for (size_t layer=0; layer<delta.size(); ++layer)
    //{ cout << "layer " << layer << ": ";
    //  for (size_t node=0; node<delta[layer].size(); ++node)
    //  { if (node>0)
    //      cout << ", ";
    //    cout << delta[layer][node];
    //  }
    //  cout << endl;
    //} // }}}

    // Update flat_delta from delta
    size_t dest=0;
    for (size_t layer=0; layer<myNodes.size(); ++layer)
    { for (size_t node=0; node<myNodes[layer].size(); ++node)
      { Node &n(myNodes[layer][node]);
        n.first.SetData(&(signals[layer]),signals[layer].size(),1);
        vector<double> d(n.second->CountParameters(),0.0);
        // Calculate delta increment to d
        n.second->AddDelta(n.first,0,d,delta[layer][node+1]);
        // Update delta from d
        for (size_t i=0; i<d.size(); ++i)
          flat_delta[dest++]+=d[i];
        // Reset data reference
        n.first.SetData(NULL,0,0);
      }
    }
  }

  // flat_delta = flat_delta/m
  for (size_t i=0; i<flat_delta.size(); ++i)
  { flat_delta[i]=flat_delta[i]/instances.Height();
  }

  // Add regularization
  size_t dest=0;
  for (size_t layer=0; layer<myNodes.size(); ++layer)
  { for (size_t node=0; node<myNodes[layer].size(); ++node)
    { Node &n(myNodes[layer][node]);
      // Update flat_delta regularization 
      for (size_t i=0; i<n.second->CountParameters(); ++i)
      { if (i>0)
          flat_delta[dest]+=lambda*n.second->GetParameter(i)/instances.Height();
        ++dest;
      }
    }
  }

  return flat_delta;
} // }}}

size_t Network::CountParameters() const // {{{
{ size_t result=0;
  for (size_t layer=0; layer<myNodes.size(); ++layer)
    for (size_t node=0; node<myNodes[layer].size(); ++node)
      result+=myNodes[layer][node].second->CountParameters();

  return result;
} // }}}
double Network::GetParameter(size_t i) const // {{{
{ for (size_t layer=0; layer<myNodes.size(); ++layer)
    for (size_t node=0; node<myNodes[layer].size(); ++node)
    { if (i<=myNodes[layer][node].second->CountParameters())
        return myNodes[layer][node].second->GetParameter(i);
      else
        i-=myNodes[layer][node].second->CountParameters();
    }
  throw string("Network::GetParameter: Index out of bounds");
} // }}}
void Network::SetParameter(size_t i, double val) // {{{
{ for (size_t layer=0; layer<myNodes.size(); ++layer)
    for (size_t node=0; node<myNodes[layer].size(); ++node)
    { if (i<=myNodes[layer][node].second->CountParameters())
        return myNodes[layer][node].second->SetParameter(i,val);
      else
        i-=myNodes[layer][node].second->CountParameters();
    }
  throw string("Network::SetParameter: Index out of bounds");
} // }}}
void Network::SaveParameters(ostream &dest, bool saveSize) const // {{{
{ dest << myInputSize << "->[";
  for (size_t layer=0; layer<myNodes.size(); ++layer)
  { if (layer>0)
      dest << ",";
    dest << "[";
    for (size_t node=0; node<myNodes[layer].size(); ++node)
    { if (node>0)
        dest << ",";
      dest << "[";
      for (size_t par=0; par<myNodes[layer][node].first.Width(); ++par)
      { if (par>0)
          dest << ",";
          dest << myNodes[layer][node].first.GetMap()[par] << ":" << myNodes[layer][node].second->GetParameter(par);
      }
      dest << "]";
    }
    dest << "]" << endl;
  }
  dest << "]";
  return;
} // }}}
void Network::LoadParameters(istream &src, bool loadSize) // {{{
{ throw string("Network::LoadParameters Not Implemented");
} // }}}
Network *Network::Parse(const std::string &networkstr) // {{{
{ dpl::SlrParser parser("network");
  parser.Load(std::string((char*)bnf_network_bnf,bnf_network_bnf_len));
  dpl::parsetree *tree=parser.Parse(networkstr);
  Network *result=ParseNetwork(*tree);
  delete tree;
  return result;
} // }}}
Network *Network::ParseNetwork(const dpl::parsetree &tree) // {{{
{ if (tree.TypeCase()=="network.net")
  { Network *result=new Network(std::stoi(tree.Child("features")->Token().Content()));
    ParseLayers(*result,*tree.Child("layers"));
    return result;
  }
  else
    throw string("Network::ParseNetwork: Unknown case ")+tree.TypeCase();
} // }}}
void Network::ParseLayers(Network &dest, const dpl::parsetree &tree) // {{{
{ if (tree.TypeCase()=="layers.lvl")
  { return ParseLayers(dest,*tree.Child("layers"));
  }
  else if (tree.TypeCase()=="layers.cons")
  { ParseLayers(dest,*tree.Child("first"));
    return ParseLayers(dest,*tree.Child("second"));
  }
  else if (tree.TypeCase()=="layers2.end")
  { return ParseNodes(dest,dest.Layers(),*tree.Child("layer"));
  }
  else if (tree.TypeCase()=="layers2.mul")
  { int its=std::stoi(tree.Child("iterations")->Token().Content());
    for (int i=0; i<its; ++i)
      ParseLayers(dest,*tree.Child("layers"));
    return;
  }
  else if (tree.TypeCase()=="layers2.par")
  { return ParseLayers(dest,*tree.Child("layers"));
  }
  else
    throw string("Network::ParseLayers: Unknown case ")+tree.TypeCase();
} // }}}
void Network::ParseNodes(Network &dest, size_t layer, const dpl::parsetree &tree) // {{{
{ if (tree.TypeCase()=="layer.nodes")
  { return ParseNodes(dest,layer,*tree.Child("nodes"));
  }
  else if (tree.TypeCase()=="nodes.lvl")
  { return ParseNodes(dest,layer,*tree.Child("nodes"));
  }
  else if (tree.TypeCase()=="nodes.cons")
  { ParseNodes(dest,layer,*tree.Child("first"));
    return ParseNodes(dest,layer,*tree.Child("second"));
  }
  else if (tree.TypeCase()=="nodes2.end")
  { return ParseNode(dest,layer,*tree.Child("node"));
  }
  else if (tree.TypeCase()=="nodes2.mul")
  { int its=std::stoi(tree.Child("iterations")->Token().Content());
    for (int i=0; i<its; ++i)
      ParseNodes(dest,layer,*tree.Child("nodes"));
    return;
  }
  else if (tree.TypeCase()=="nodes2.par")
  { return ParseNodes(dest,layer,*tree.Child("nodes"));
  }
  else
    throw string("Network::ParseNodes: Unknown case ")+tree.TypeCase();
} // }}}
void Network::ParseNode(Network &dest, size_t layer, const dpl::parsetree &tree) // {{{
{ if (tree.TypeCase()=="node.unam")
  { vector<size_t> node_sources;
    vector<double> node_weights;
    size_t node_inputs=layer>0?dest.LayerSize(layer-1)+1:dest.InputSize();
    ParseInputs(node_inputs,node_sources,node_weights,*tree.Child("inputs"));
    size_t node_id=dest.AddNode(layer,node_sources);
    for (size_t i=0; i<node_weights.size(); ++i)
      dest.GetNode(layer,node_id).second->SetParameter(i,node_weights[i]);
    return;
  }
  else if (tree.TypeCase()=="node.nam")
  { // Ignore names for now
    vector<size_t> node_sources;
    vector<double> node_weights;
    size_t node_inputs=layer>0?dest.LayerSize(layer-1)+1:dest.InputSize();
    ParseInputs(node_inputs,node_sources,node_weights,*tree.Child("inputs"));
    size_t node_id=dest.AddNode(layer,node_sources);
    for (size_t i=0; i<node_weights.size(); ++i)
      dest.GetNode(layer,node_id).second->SetParameter(i,node_weights[i]);
    return;
  }
  else
    throw string("Network::ParseNode: Unknown case ")+tree.TypeCase();
} // }}}
void Network::ParseInputs(size_t inputs, vector<size_t> &sources, vector<double> &weights, const dpl::parsetree &tree) // {{{
{ if (tree.TypeCase()=="inputs.all")
  { for (size_t i=0; i<inputs; ++i)
    { sources.push_back(i);
      weights.push_back((50000.0-rand()%100000)/10000.0); // Use random weights
    }
    return;
  }
  else if (tree.TypeCase()=="inputs.lvl")
  { return ParseInputs(inputs,sources,weights,*tree.Child("inputs"));
  }
  else if (tree.TypeCase()=="inputs2.end")
  { return ParseInputs(inputs,sources,weights,*tree.Child("input"));
  }
  else if (tree.TypeCase()=="inputs2.cons")
  { ParseInputs(inputs,sources,weights,*tree.Child("input"));
    return ParseInputs(inputs,sources,weights,*tree.Child("inputs"));
  }
  else if (tree.TypeCase()=="input.idx")
  { sources.push_back(std::stoi(tree.Child("idx")->Token().Content()));
    weights.push_back((50000.0-rand()%100000)/10000.0); // Use random weights
    return;
  }
  else if (tree.TypeCase()=="input.wgt")
  { sources.push_back(std::stoi(tree.Child("idx")->Token().Content()));
    weights.push_back(std::stod(tree.Child("weight")->Token().Content())); // Use random weights
    return;
  }
  else
    throw string("Network::ParseInputs: Unknown case ")+tree.TypeCase();
} // }}}
