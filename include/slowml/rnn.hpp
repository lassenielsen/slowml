#pragma once
#include <slowml/shallowmodel.hpp>
#include <slowml/wrapperdata.hpp>
#include <dpl/parsetree.hpp>
#include <tuple>

class RecurrentNetwork : public Model<std::vector<double> > // {{{
{ public:
    typedef std::pair<std::vector<size_t>,ShallowModel<double>*> Node;

    RecurrentNetwork(size_t input_size, size_t memory_size);
    virtual ~RecurrentNetwork();

    std::string Type() const { return "RecurrentNetwork Model"; }

    size_t AddNode(size_t layer, const std::vector<size_t> &inputs);
    Node &GetNode(size_t layer, size_t node) { return myNodes.at(layer).at(node); }
    const Node &GetNode(size_t layer, size_t node) const { return myNodes.at(layer).at(node); }

    std::vector<double> Eval(const vector<vector<double>> &instances, size_t pos) const;
    std::vector<double> Eval(const std::vector<double> &instance) const;
    double LogDistance(const std::vector<double> &guess, const std::vector<double> &truth);
    double Cost(const std::vector<std::vector<double>> &instances, const std::vector<std::vector<double>> &truths, double lambda=1.0);
    std::vector<double> Delta(const vector<vector<double>> &instances, const vector<vector<double>> &truths, double lambda=1.0);

    size_t InputSize() const { return myInputSize; }
    size_t LayerSize(size_t layer) const { return myNodes.at(layer).size(); }
    size_t Layers() const { return myNodes.size(); }
    size_t CountParameters() const // {{{
    { return myPmap.size();
    } // }}}
    double GetParameter(size_t i) const // {{{
    { return myNodes[std::get<0>(myPmap[i])][std::get<1>(myPmap[i])].second->GetParameter(std::get<2>(myPmap[i]));
    } // }}}
    void SetParameter(size_t i, double val) // {{{
    { myNodes[std::get<0>(myPmap[i])][std::get<1>(myPmap[i])].second->SetParameter(std::get<2>(myPmap[i]),val);
    } // }}}

    void SaveParameters(std::ostream &dest, bool saveSize=true) const;

    static double ReadDouble(istream &src);
    static int ReadInt(istream &src);
    static std::string ReadString(istream &src, size_t size);
    void LoadParameters(std::istream &src, bool loadSize=true);

    static RecurrentNetwork *Parse(const std::string &network);
    static RecurrentNetwork *ParseNetwork(const dpl::parsetree &tree);
    static void ParseLayers(RecurrentNetwork &dest, const dpl::parsetree &tree);
    static void ParseNodes(RecurrentNetwork &dest, size_t idx, const dpl::parsetree &tree);
    static void ParseNode(RecurrentNetwork &dest, size_t idx, const dpl::parsetree &tree);
    static void ParseInputs(size_t inputs, std::vector<size_t> &sources, std::vector<double> &weights, const dpl::parsetree &tree);

    const std::vector<std::vector<Node> > &Nodes() const { return myNodes; }
    std::vector<std::vector<Node> > &Nodes() { return myNodes; }
    size_t MemorySize() const { return myMemorySize; }

  private:
    void InitPmap() // {{{
    { myPmap.clear();
      for (size_t l=0; l<myNodes.size(); ++l)
      { for (size_t n=0; n<myNodes[l].size(); ++n)
        { for (size_t p=0; p<myNodes[l][n].second->CountParameters(); ++p)
            myPmap.push_back(tuple<size_t,size_t,size_t>(l,n,p));
        }
      }
    } // }}}

    std::vector<std::vector<Node> > myNodes;
    std::vector<std::tuple<size_t,size_t,size_t> > myPmap;
    //std::vector<std::pair<std::string,std::pair<size_t,size_t> > > myOutputs;
    size_t myInputSize;
    size_t myMemorySize;
}; // }}}
