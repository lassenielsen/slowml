#include <slowml/rnn.hpp>
#include <slowml/network_bnf.hpp>
#include <slowml/logisticregressionmodel.hpp>
#include <dpl/slrparser.hpp>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <thread>

using namespace std;

RecurrentNetwork::RecurrentNetwork(size_t input_size, size_t memory_size) // {{{
: myInputSize(input_size)
, myMemorySize(memory_size)
{
} // }}}

RecurrentNetwork::~RecurrentNetwork() // {{{
{ for (auto level=myNodes.begin(); level!=myNodes.end(); ++level)
  { for (auto node=level->begin(); node!=level->end(); ++node)
    { delete node->second;
    }
  }

  myNodes.clear();
} // }}}

size_t RecurrentNetwork::AddNode(size_t layer, const vector<size_t> &inputs) // {{{
{ //cout << "RecurrentNetwork::AddNode(" << layer << "," << inputs.size() << ")" << endl;
  while (myNodes.size()<=layer)
    myNodes.push_back(vector<Node>());
  
  Node node(inputs,new LogisticRegressionModel());
  if (inputs[0]!=0)
    cout << "Warning: RecurrentNetwork::AddNode: input 0 is not used.";
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

vector<double> RecurrentNetwork::Eval(const vector<vector<double>> &instances, size_t pos) const // {{{
{ if (pos>=instances.size())
    throw string("RecurrentNetwork::Eval pos exceedes number of instances");
  
  const vector<double> &instance=instances[pos];
  return Eval(instance);
} // }}}
vector<double> RecurrentNetwork::Eval(const vector<double> &instance) const // {{{
{ if (instance.size()%myInputSize!=0)
    throw string("RecurrentNetwork::Eval input width does not match");

  int recurrences=instance.size()/myInputSize;
  
  vector<double> output;
  vector<double> memory;
  for (size_t i=0; i<myMemorySize; ++i)
    memory.push_back(0.0); // Initial memory

  //cout << "RecurrentNetwork::Eval evaluating " << myNodes.size() << "layers" << endl;
  for (size_t r=0; r<recurrences; ++r)
  { // Initial state from input and memory
    vector<double> state;
    for (size_t i=0; i<myInputSize; ++i)
      state.push_back(instance[myInputSize*r+i]);
    for (size_t i=0; i<myMemorySize; ++i)
      state.push_back(memory[i]);
    // Evaluate network
    for (auto layer=myNodes.begin(); layer!=myNodes.end(); ++layer)
    { // Debug state
      //cout << "RecurrentNetwork::Eval state: ";
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
    // Collect output and memory
    for (size_t i=0; i<state.size()-myMemorySize; ++i)
      output.push_back(state[i]);
    for (size_t i=0; i<myMemorySize; ++i)
      memory[i]=state[state.size()-myMemorySize+i];
  }
  // Debug state
  //cout << "RecurrentNetwork::Eval final state: ";
  //for (auto it=state.begin(); it!=state.end(); ++it)
  //{ if (it!=state.begin())
  //    cout << ", ";
  //  cout << *it;
  //}
  //cout << endl;
  return output;
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

double RecurrentNetwork::LogDistance(const vector<double> &guess, const vector<double> &truth) // {{{
{ if (guess.size()!=truth.size())
    throw string("RecurrentNetwork::LogDistance input width does not match");
  double dist=0.0;
  for (size_t i=0; i<guess.size(); ++i)
  { double distPart=-truth[i]*log(max(0.0001,guess[i]))-(1.0-truth[i])*log(1.0-min(0.9999,guess[i]));
    if (std::isnan(distPart) || std::isinf(distPart))
      return 1.0/0.0;
    //cout << "RecurrentNetwork::LogDistance: diff of " << guess[i] << " and " << truth[i] << " is " << distPart << endl;
    dist+= distPart;
  }
  return dist;
} // }}}

struct SumCostArg // {{{
{ SumCostArg(RecurrentNetwork *net,
             const vector<vector<double>> &instances,
             const vector<vector<double>> &truths,
             size_t instance_min,
             size_t instance_max)
  : net(net)
  , instances(instances)
  , truths(truths)
  , instance_min(instance_min)
  , instance_max(instance_max)
  , result(0.0)
  {
  }

  RecurrentNetwork *net;
  const vector<vector<double>> instances;
  const vector<vector<double>> truths;
  size_t instance_min;
  size_t instance_max;
  double result;
}; // }}}
void SumCosts(SumCostArg *arg) // {{{
{ //cout << "SumCost from " << flush << arg->instance_min << " to " << arg->instance_max << endl;
  arg->result=0;
  for (size_t i=arg->instance_min; i<arg->instance_max; ++i)
    arg->result += arg->net->LogDistance(arg->net->Eval(arg->instances,i),arg->truths[i]);
} // }}}
double RecurrentNetwork::Cost(const vector<vector<double>> &instances, const vector<vector<double>> &truths, double lambda) // {{{
{ double cost=0;
  size_t cores=std::thread::hardware_concurrency();
  //cout << "RecurrentNetwork::Cost: Debug - Number of cores: " << cores << endl;
  vector<SumCostArg*> args;
  vector<thread> threads;
  // Start threads
  for (size_t core=0; core<cores; ++core)
  { SumCostArg *arg=new SumCostArg(this,instances,truths,instances.size()*core/cores,instances.size()*(core+1)/cores);
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

  cost = cost/(double)instances.size();

  // Add Regularization
  for (size_t layer=0; layer<myNodes.size(); ++layer)
    for (size_t node=0; node<myNodes[layer].size(); ++node)
      for (size_t param=1; param<myNodes[layer][node].second->CountParameters(); ++param)
        cost +=lambda*pow(myNodes[layer][node].second->GetParameter(param),2.0)/(2.0*instances.size());
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
{ SumDeltaArg(RecurrentNetwork *net,
             const vector<vector<double>> &instances,
             const vector<vector<double>> &truths,
             size_t instance_min,
             size_t instance_max)
  : net(net)
  , instances(instances)
  , truths(truths)
  , instance_min(instance_min)
  , instance_max(instance_max)
  {
  }

  RecurrentNetwork *net;
  const vector<vector<double>> &instances;
  const vector<vector<double>> &truths;
  size_t instance_min;
  size_t instance_max;
  vector<double> result;
}; // }}}
void SumDelta(SumDeltaArg *arg) // {{{
{ // Layout:
  // netDepth=arg->net->Nodes().size() which is the number of layers in the network
  // signals[layer][slice] -> arg->net->Nodes()[layer-%(netDepth+1)][node].second -> signals[layer+1][node+1]
  // where slice is defined by arg->net->Nodes()[layer][node].first
  // delta[layer][node] corresponds directly to signals[layer][node]

  //cout << "SumDelta from " << flush << arg->instance_min << " to " << arg->instance_max << endl;
  arg->result=vector<double>(arg->net->CountParameters(),0.0);

  for (size_t pos=arg->instance_min; pos<arg->instance_max; ++pos)
  { // Look up data
    const vector<double> &instance=arg->instances[pos];
    const vector<double> &truth=arg->truths[pos];
    // Validate sizes
    if (instance.size()%arg->net->InputSize()!=0)
      throw string("RecurrentNetwork::SumDelta input width does not match");

    int recurrences=instance.size()/arg->net->InputSize();
    size_t outputSize=arg->net->Nodes().back().size()-arg->net->MemorySize();
    size_t netDepth=arg->net->Nodes().size();

    if (truth.size()!=outputSize*recurrences)
      throw string("RecurrentNetwork::SumDelta truth width does not match");
    
    // Calculate signals
    vector< vector<double> > signals;
    vector<double> memory;
    for (size_t i=0; i<arg->net->MemorySize(); ++i)
      memory.push_back(0.0); // Initial memory

    for (size_t r=0; r<recurrences; ++r)
    { // Add layer with state from input and memory
      signals.push_back(vector<double>(arg->net->InputSize()+arg->net->MemorySize()));
      for (size_t i=0; i<arg->net->InputSize(); ++i)
        signals.back()[i]=arg->instances[pos][arg->net->InputSize()*r+i];
      for (size_t i=0; i<arg->net->MemorySize(); ++i)
        signals.back()[arg->net->InputSize()+i]=memory[i]; // Set memory

      // Evaluate network
      for (auto layer=arg->net->Nodes().begin(); layer!=arg->net->Nodes().end(); ++layer)
      { vector<double> &state=signals.back();
    
        vector<double> newstate;
        if (layer+1!=arg->net->Nodes().end())
          newstate.push_back(1.0);
        for (auto node=layer->begin(); node!=layer->end(); ++node)
        { WrapperData<double> data(state.size(),1,node->first,&state);
          double result=node->second->Eval(data,0);
          //node->first.SetData(NULL,0,0);
          newstate.push_back(result);
        }
        signals.push_back(newstate);
      }
      // Collect output and memory
      for (size_t i=0; i<arg->net->MemorySize(); ++i)
        memory[i]=signals.back()[signals.back().size()-arg->net->MemorySize()+i];
    }


    // Calculate delta - myNodes[l][n] has error delta[l][n+1]
    vector< vector<double> > delta;
    while (delta.size()<signals.size())
      delta.push_back(vector<double>(signals[delta.size()].size(),0.0));
    
    // Backpropagate delta
    for (int layer=delta.size()-1; layer>=0; --layer)
    { int netRecurrence = layer/(netDepth+1);
      int netLayer=layer%(netDepth+1);
      if (layer%(netDepth+1)==netDepth)
      { // Compute delta for result layer
        for (size_t i=0; i<outputSize; ++i)
        { delta[layer][i]=signals[layer][i]-arg->truths[pos][outputSize*netRecurrence+i];
        }
        // Transfer delta for memory
        for (size_t i=0; i<arg->net->MemorySize(); ++i)
        { delta[layer][outputSize+i]=layer+1<delta.size()?delta[layer+1][i+arg->net->InputSize()]:0.0;
        }
      }
      else
      { //Computer delata[layer] from delta[layer+1], signals[layer] and parameters
        // delta[l]=Theta[l]'*delta[l+1] . signals(l) . (1-signals(l))
        for (size_t node=0; node<arg->net->Nodes()[layer%(netDepth+1)].size(); ++node)
        { RecurrentNetwork::Node &n(arg->net->Nodes()[layer][node]);
          WrapperData<double> delta_data(delta[layer].size(),1,n.first,&delta[layer]);
          for (size_t param=0; param<arg->net->Nodes()[layer][node].second->CountParameters(); ++param)
            delta_data.SetValue(0,param,delta_data.GetValue(0,param)+n.second->GetParameter(param)*delta[layer+1][node+1]);
        }
        // Multiply by signals(l) * (1-signals(l))
        for (size_t node=0; node<delta[layer].size(); ++node)
          delta[layer][node]=delta[layer][node]*sigmoid_deriv(signals[layer+1][node]);
        }
    }

    // Update flat_delta from delta
    for (size_t recurrence=0; recurrence<recurrences; ++recurrence)
    { size_t dest=0;
      for (size_t layer=0; layer<netDepth; ++layer)
      { for (size_t node=0; node<arg->net->Nodes()[layer].size(); ++node)
        { RecurrentNetwork::Node &n(arg->net->Nodes()[layer][node]);
          WrapperData<double> signals_data(signals[layer+recurrence*(netDepth+1)].size(),1,n.first,&signals[layer+recurrence*(netDepth+1)]);
          vector<double> d(n.second->CountParameters(),0.0);
          // Calculate delta increment to d
          n.second->AddDelta(signals_data,0,d,delta[layer+recurrence*(netDepth+1)][node+1]);
          // Update delta from d
          for (size_t i=0; i<d.size(); ++i)
            arg->result[dest++]+=d[i];
        }
      }
    }
  }

  return;
} // }}}
std::vector<double> RecurrentNetwork::Delta(const vector<vector<double>> &instances, const vector<vector<double>> &truths, double lambda) // {{{
{ vector<double> flat_delta(CountParameters(),0.0);

  size_t cores=std::thread::hardware_concurrency();
  //cout << "RecurrentNetwork::Delta: Debug - Number of cores: " << cores << endl;
  vector<SumDeltaArg*> args;
  vector<thread> threads;
  // Start threads
  for (size_t core=0; core<cores; ++core)
  { SumDeltaArg *arg=new SumDeltaArg(this,instances,truths,instances.size()*core/cores,instances.size()*(core+1)/cores);
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
  { flat_delta[i]=flat_delta[i]/instances.size();
  }

  // Add regularization
  size_t dest=0;
  for (size_t layer=0; layer<myNodes.size(); ++layer)
  { for (size_t node=0; node<myNodes[layer].size(); ++node)
    { Node &n(myNodes[layer][node]);
      // Update flat_delta regularization 
      for (size_t i=0; i<n.second->CountParameters(); ++i)
      { if (i>0)
          flat_delta[dest]+=lambda*n.second->GetParameter(i)/instances.size();
        ++dest;
      }
    }
  }

  return flat_delta;
} // }}}
void RecurrentNetwork::SaveParameters(ostream &dest, bool saveSize) const // {{{
{ dest.precision(std::numeric_limits<double>::max_digits10 - 1);
  dest << myInputSize << ":" << myMemorySize << "->[";
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
double RecurrentNetwork::ReadDouble(istream &src) // {{{
{ double result;
  src >> result; // FIXME: This probably does not work!
  return result;
} // }}}
int RecurrentNetwork::ReadInt(istream &src) // {{{
{ int result;
  src >> result; // FIXME: This probably does not work!
  return result;
} // }}}
string RecurrentNetwork::ReadString(istream &src, size_t size) // {{{
{ char *buffer=new char(size+1);
  src.read(buffer,size);
  buffer[size]='\0';
  string result(buffer);
  delete [] buffer;
  return result;
} // }}}
void RecurrentNetwork::LoadParameters(istream &src, bool loadSize) // {{{
{ // Clear nodes
  for (auto level=myNodes.begin(); level!=myNodes.end(); ++level)
  { for (auto node=level->begin(); node!=level->end(); ++node)
    { delete node->second;
    }
  }
  myNodes.clear();

  myInputSize=ReadInt(src);
  if (ReadString(src,1)!=":")
    throw string("RecurrentNetwork::LoadParameters exprected '->['");
  myMemorySize=ReadInt(src);
  if (ReadString(src,3)!="->[")
    throw string("RecurrentNetwork::LoadParameters exprected '->['");
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
              throw string("RecurrentNetwork::LoadParameters expected ':'");
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
              throw string("RecurrentNetwork::LoadParameters unknown parameter continuation: ")+string(1,src.get());
          } // }}}
        }
        else if (src.peek()==']') // exit layer
        { src.get(); // read ']'
          break;
        }
        else
          throw string("RecurrentNetwork::LoadParameters unknown node continuation: ")+string(1,src.get());
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
      throw string("RecurrentNetwork::LoadParameters unknown layer continuation: ")+string(1,src.get());
  }
  cerr << "RecurrentNetwork::LoadParameters::Unexpected end of model, partial model loaded" << endl;
} // }}}

RecurrentNetwork *RecurrentNetwork::Parse(const std::string &networkstr, size_t memory_size) // {{{
{ dpl::SlrParser parser("network");
  parser.Load(std::string((char*)bnf_network_bnf,bnf_network_bnf_len));
  dpl::parsetree *tree=parser.Parse(networkstr);
  RecurrentNetwork *result=ParseRecurrentNetwork(*tree, memory_size);
  delete tree;
  return result;
} // }}}
RecurrentNetwork *RecurrentNetwork::ParseRecurrentNetwork(const dpl::parsetree &tree, size_t memory_size) // {{{
{ if (tree.TypeCase()=="network.net")
  { RecurrentNetwork *result=new RecurrentNetwork(std::stoi(tree.Child("features")->Token().Content()), memory_size);
    ParseLayers(*result,*tree.Child("layers"));
    return result;
  }
  else
    throw string("RecurrentNetwork::ParseRecurrentNetwork: Unknown case ")+tree.TypeCase();
} // }}}
void RecurrentNetwork::ParseLayers(RecurrentNetwork &dest, const dpl::parsetree &tree) // {{{
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
    throw string("RecurrentNetwork::ParseLayers: Unknown case ")+tree.TypeCase();
} // }}}
void RecurrentNetwork::ParseNodes(RecurrentNetwork &dest, size_t layer, const dpl::parsetree &tree) // {{{
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
    throw string("RecurrentNetwork::ParseNodes: Unknown case ")+tree.TypeCase();
} // }}}
void RecurrentNetwork::ParseNode(RecurrentNetwork &dest, size_t layer, const dpl::parsetree &tree) // {{{
{ if (tree.TypeCase()=="node.unam")
  { vector<size_t> node_sources;
    vector<double> node_weights;
    size_t node_inputs=layer>0?dest.LayerSize(layer-1)+1:(dest.InputSize()+dest.MemorySize());
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
    size_t node_inputs=layer>0?dest.LayerSize(layer-1)+1:(dest.InputSize()+dest.MemorySize());
    ParseInputs(node_inputs,node_sources,node_weights,*tree.Child("inputs"));
    size_t node_id=dest.AddNode(layer,node_sources);
    for (size_t i=0; i<node_weights.size(); ++i)
      dest.GetNode(layer,node_id).second->SetParameter(i,node_weights[i]);
    return;
  }
  else
    throw string("RecurrentNetwork::ParseNode: Unknown case ")+tree.TypeCase();
} // }}}
void RecurrentNetwork::ParseInputs(size_t inputs, vector<size_t> &sources, vector<double> &weights, const dpl::parsetree &tree) // {{{
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
    throw string("RecurrentNetwork::ParseInputs: Unknown case ")+tree.TypeCase();
} // }}}
