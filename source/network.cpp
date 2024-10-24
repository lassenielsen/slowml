#include <slowml/network.hpp>
#include <slowml/network_bnf.hpp>
#include <slowml/logisticregressionmodel.hpp>
#include <dpl/slrparser.hpp>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <thread>

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
{ //cout << "Network::AddNode(" << layer << "," << inputs.size() << ")" << endl;
  while (myNodes.size()<=layer)
    myNodes.push_back(vector<Node>());
  
  Node node(inputs,new LogisticRegressionModel());
  if (inputs[0]!=0)
    cout << "Warning: Network::AddNode: input 0 is not used.";
  for (size_t i=1; i<inputs.size(); ++i)
    node.second->AddParameter((500.0-rand()%1000)/100.0);
  myNodes[layer].push_back(node);

  // Add entries to myPmap
  vector<tuple<size_t,size_t,size_t> > elts;
  for (size_t par=0; par<inputs.size(); ++par)
  { tuple<size_t,size_t,size_t> elt(layer,myNodes[layer].size()-1,par);
    elts.push_back(elt);
  }
  tuple<size_t,size_t,size_t> nxt(layer+1,0,0);
  vector<tuple<size_t,size_t,size_t> >::iterator pos=std::find(myPmap.begin(),myPmap.end(),nxt);
  myPmap.insert(pos,elts.begin(),elts.end());
  return myNodes[layer].size()-1;
} // }}}

vector<double> Network::Eval(const Data<double> &instances, size_t pos) const // {{{
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
    { WrapperData<double> data(state.size(),1,node->first,&state); //.SetData(&state,state.size(),1); //!!! CONCURRENCY
      double result=node->second->Eval(data,0);
      //node->first.SetData(NULL,0,0);
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
vector<double> Network::Eval(const vector<double> &instance) const // {{{
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
    { WrapperData<double> data(state.size(),1,node->first,&state); //.SetData(&state,state.size(),1); //!!! CONCURRENCY
      double result=node->second->Eval(data,0);
      //node->first.SetData(NULL,0,0);
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

double max(double lhs, double rhs) // {{{
{ if (lhs<rhs)
    return rhs;
  return lhs;
} // }}}
double min(double lhs, double rhs) // {{{
{ if (lhs<rhs)
    return lhs;
  return rhs;
} // }}}

double Network::LogDistance(const vector<double> &guess, const vector<double> &truth) // {{{
{ if (guess.size()!=truth.size())
    throw string("Network::LogDistance input width does not match");
  double dist=0.0;
  for (size_t i=0; i<guess.size(); ++i)
  { double distPart=-truth[i]*log(max(0.0001,guess[i]))-(1.0-truth[i])*log(1.0-min(0.9999,guess[i]));
    if (std::isnan(distPart) || std::isinf(distPart))
      return 1.0/0.0;
    //cout << "Network::LogDistance: diff of " << guess[i] << " and " << truth[i] << " is " << distPart << endl;
    dist+= distPart;
  }
  return dist;
} // }}}

struct SumCostArg // {{{
{ SumCostArg(Network *net,
             const GuidedData<double,vector<double>> &instances,
             size_t instance_min,
             size_t instance_max)
  : net(net)
  , instances(instances)
  , instance_min(instance_min)
  , instance_max(instance_max)
  , result(0.0)
  {
  }

  Network *net;
  const GuidedData<double,vector<double>> &instances;
  size_t instance_min;
  size_t instance_max;
  double result;
}; // }}}
void SumCosts(SumCostArg *arg) // {{{
{ //cout << "SumCost from " << flush << arg->instance_min << " to " << arg->instance_max << endl;
  arg->result=0;
  for (size_t i=arg->instance_min; i<arg->instance_max; ++i)
    arg->result += arg->net->LogDistance(arg->net->Eval(arg->instances,i),arg->instances.GetResult(i));
} // }}}
double Network::Cost(const GuidedData<double,vector<double>> &instances, double lambda) // {{{
{ double cost=0;
  size_t cores=std::thread::hardware_concurrency();
  //cout << "Network::Cost: Debug - Number of cores: " << cores << endl;
  vector<SumCostArg*> args;
  vector<thread> threads;
  // Start threads
  for (size_t core=0; core<cores; ++core)
  { SumCostArg *arg=new SumCostArg(this,instances,instances.Height()*core/cores,instances.Height()*(core+1)/cores);
    args.push_back(arg);
    threads.push_back(thread(SumCosts,arg));
  }
  // Collect results
  for (size_t core=0; core<cores; ++core)
  { threads[core].join();
    cost+=args[core]->result;
    delete args[core];
  }

  // Single threaded summation
  //for (size_t i=0; i<instances.Height(); ++i)
  //{ vector<double> r1=Eval(instances,i);
  //  cost += LogDistance(Eval(instances,i),instances.GetResult(i));
  //}

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
inline double norm_sigmoid_deriv(double val) // {{{
{ double s=sigmoid_deriv(val);
  if (s<0.0)
    return -1.0;
  else if (s>0.0)
    return 1.0;
  else
    return 0.0;
} // }}}

struct SumDeltaArg // {{{
{ SumDeltaArg(Network *net,
             const GuidedData<double,vector<double>> &instances,
             size_t instance_min,
             size_t instance_max)
  : net(net)
  , instances(instances)
  , instance_min(instance_min)
  , instance_max(instance_max)
  {
  }

  Network *net;
  const GuidedData<double,vector<double>> &instances;
  size_t instance_min;
  size_t instance_max;
  vector<double> result;
}; // }}}
void SumDelta(SumDeltaArg *arg) // {{{
{ //cout << "SumDelta from " << flush << arg->instance_min << " to " << arg->instance_max << endl;
  arg->result=vector<double>(arg->net->CountParameters(),0.0);
  for (size_t pos=arg->instance_min; pos<arg->instance_max; ++pos)
  { vector< vector<double> > signals;
    // Calculate signals - myNodes[l][n] outputs signals[l+1][n+1]
    signals.push_back(vector<double>(arg->instances.Width()));
    for (size_t i=0; i<arg->instances.Width(); ++i)
      signals[0][i]=arg->instances.GetValue(pos,i);
    for (size_t layer=0; layer<arg->net->Nodes().size(); ++layer)
    { signals.push_back(vector<double>());
      signals[layer+1].push_back(1.0);
      for (size_t node=0; node<arg->net->Nodes()[layer].size(); ++node)
      { Network::Node &n(arg->net->Nodes()[layer][node]);
        WrapperData<double> signals_data(signals[layer].size(),1,n.first,&signals[layer]); //.SetData(&(signals[layer]),signals[layer].size(),1);
        signals[layer+1].push_back(n.second->Eval(signals_data,0));
      }
    }

    // Calculate delta - myNodes[l][n] has error delta[l][n+1]
    vector< vector<double> > delta;
    while (delta.size()<arg->net->Nodes().size())
      delta.push_back(vector<double>(arg->net->Nodes()[delta.size()].size()+1,0.0));
    
    // Compute delta for the last layer
    for (size_t i=0; i+1<delta[delta.size()-1].size(); ++i)
    { //cout << "Result: " << instances.GetResult(pos)[i] << endl;
      delta[delta.size()-1][i+1]=signals[signals.size()-1][i+1]-arg->instances.GetResult(pos)[i];
    }
    // Backpropagate delta
    for (int layer=delta.size()-2; layer>=0; --layer)
    { //Computer delata[layer] from delta[layer+1], signals[layer] and parameters
      // delta[l]=Theta[l]'*delta[l+1] . signals(l) . (1-signals(l))
      for (size_t node=0; node<arg->net->Nodes()[layer+1].size(); ++node)
      { Network::Node &n(arg->net->Nodes()[layer+1][node]);
        WrapperData<double> delta_data(delta[layer].size(),1,n.first,&delta[layer]); //.SetData(&(delta[layer]),delta[layer].size(),1);
        for (size_t param=0; param<arg->net->Nodes()[layer+1][node].second->CountParameters(); ++param)
          delta_data.SetValue(0,param,delta_data.GetValue(0,param)+n.second->GetParameter(param)*delta[layer+1][node+1]);
      }
      // Multiply by signals(l) * (1-signals(l))
      for (size_t node=0; node<delta[layer].size(); ++node)
        delta[layer][node]=delta[layer][node]*sigmoid_deriv(signals[layer+1][node]);
    }

    // Update flat_delta from delta
    size_t dest=0;
    for (size_t layer=0; layer<arg->net->Nodes().size(); ++layer)
    { for (size_t node=0; node<arg->net->Nodes()[layer].size(); ++node)
      { Network::Node &n(arg->net->Nodes()[layer][node]);
        WrapperData<double> signals_data(signals[layer].size(),1,n.first,&signals[layer]); //.SetData(&(signals[layer]),signals[layer].size(),1);
        vector<double> d(n.second->CountParameters(),0.0);
        // Calculate delta increment to d
        n.second->AddDelta(signals_data,0,d,delta[layer][node+1]);
        // Update delta from d
        for (size_t i=0; i<d.size(); ++i)
          arg->result[dest++]+=d[i];
      }
    }
  }
  
  return;
} // }}}
std::vector<double> Network::Delta(const GuidedData<double,vector<double> > &instances, double lambda) // {{{
{ vector<double> flat_delta(CountParameters(),0.0);

  size_t cores=std::thread::hardware_concurrency();
  //cout << "Network::Delta: Debug - Number of cores: " << cores << endl;
  vector<SumDeltaArg*> args;
  vector<thread> threads;
  // Start threads
  for (size_t core=0; core<cores; ++core)
  { SumDeltaArg *arg=new SumDeltaArg(this,instances,instances.Height()*core/cores,instances.Height()*(core+1)/cores);
    args.push_back(arg);
    threads.push_back(thread(SumDelta,arg));
  }
  // Collect results
  for (size_t core=0; core<cores; ++core)
  { threads[core].join();
    for (size_t i=0; i<CountParameters(); ++i)
      flat_delta[i]+=args[core]->result[i];
    delete args[core];
  }

  // Finalize collected results

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

void Network::SaveParameters(ostream &dest, bool saveSize) const // {{{
{ dest.precision(std::numeric_limits<double>::max_digits10 - 1);
  dest << myInputSize << "->[";
  for (size_t layer=0; layer<myNodes.size(); ++layer)
  { if (layer>0)
      dest << ",";
    dest << "[";
    for (size_t node=0; node<myNodes[layer].size(); ++node)
    { if (node>0)
        dest << ",";
      dest << "[";
      for (size_t par=0; par<myNodes[layer][node].second->CountParameters(); ++par)
      { if (par>0)
          dest << ",";
          dest << myNodes[layer][node].first[par] << ":" << std::fixed << myNodes[layer][node].second->GetParameter(par);
      }
      dest << "]";
    }
    dest << "]" << endl;
  }
  dest << "]";
  return;
} // }}}
double Network::ReadDouble(istream &src) // {{{
{ double result;
  src >> result; // FIXME: This probably does not work!
  return result;
} // }}}
int Network::ReadInt(istream &src) // {{{
{ int result;
  src >> result; // FIXME: This probably does not work!
  return result;
} // }}}
string Network::ReadString(istream &src, size_t size) // {{{
{ char *buffer=new char(size+1);
  src.read(buffer,size);
  buffer[size]='\0';
  string result(buffer);
  delete [] buffer;
  return result;
} // }}}
void Network::LoadParameters(istream &src, bool loadSize) // {{{
{ // Clear nodes
  for (auto level=myNodes.begin(); level!=myNodes.end(); ++level)
  { for (auto node=level->begin(); node!=level->end(); ++node)
    { delete node->second;
    }
  }
  myNodes.clear();

  myInputSize=ReadInt(src);
  if (ReadString(src,3)!="->[")
    throw string("Network::LoadParameters exprected '->['");
  while (!src.eof())
  { // Parse Layers
    if (src.peek()=='[')
    { src.get(); // Read '['
      myNodes.push_back(vector<Node>()); // Ensure layer is accessible
      while (!src.eof()) // Parse Nodes {{{
      { if (src.peek()=='[')
        { src.get(); // Read '['
          myNodes.back().push_back(Node(vector<size_t>(),new LogisticRegressionModel())); // Ensure node is accessible
          myNodes.back().back().second->Parameters().clear();
          while (!src.eof()) // Parse Parameters {{{
          { // Read parameter
            size_t idx=ReadInt(src);
            if (src.get()!=':')
              throw string("Network::LoadParameters expected ':'");
            double weight=ReadDouble(src);
            myNodes.back().back().first.push_back(idx);
            myNodes.back().back().second->AddParameter(weight);

            if (src.peek()==']') // exit node
            { src.get(); // read ']'
              if (src.peek()==',')
              src.get(); // read ','
              break;
            }
            else if (src.peek()==',')
            { src.get();
            }
            else
              throw string("Network::LoadParameters unknown parameter continuation: ")+string(1,src.get());
          } // }}}
        }
        else if (src.peek()==']') // exit layer
        { src.get(); // read ']'
          break;
        }
        else
          throw string("Network::LoadParameters unknown node continuation: ")+string(1,src.get());
      } // }}}
    }
    else if (src.peek()==']') // exit network
    { src.get(); // read ']'
      // Initialize myPmap
      InitPmap();
      return;
    }
    else if (src.peek()==' ' || src.peek()=='\n' || src.peek()=='\r' || src.peek()==',')
    { src.get(); // read whitespace
    }
    else
      throw string("Network::LoadParameters unknown layer continuation: ")+string(1,src.get());
  }
  cerr << "Network::LoadParameters::Unexpected end of model, partial model loaded" << endl;
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
