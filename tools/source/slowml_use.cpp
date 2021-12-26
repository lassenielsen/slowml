#include <slowml/onevsall.hpp>
#include <slowml/vectordata.hpp>
#include <slowml/vectormapdata.hpp>
#include <fstream>
using namespace std;

int main(int argc, char **argv)
{ try
  { if (argc==2 && string("--help")==argv[1] || argc==1)
      throw string("Syntax is usemodel [--model|-m <modelpath=./modle.pars>] [--vectormap|-vm <map=\"X->X\">] <datapath=./data>");

    string datapath="./data";
    string modelfile="./model.pars";
    string vectormap="X->X";

    for (size_t arg=1; arg+1<argc; ++arg)
    { if (string("--model")==argv[arg] || string("-m")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--model must be succeeded by a value");
        modelfile=argv[arg];
      }
      else if (string("--vectormap")==argv[arg] || string("-vm")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--vectormap must be succeeded by another arg");
        vectormap=argv[arg];
      }
      else
        throw string("Unknown argument") + argv[arg];
    }

    datapath=argv[argc-1];

    // Parse map
    cout << "Parsing vector mapping" << flush;
    string vecmaparg;
    VectorMap *vecmap=VectorMap::Parse(vectormap,vecmaparg);
    cout << " done." << endl;

    // Load model
    cout << "Loading model" << flush;
    vector<Label> labels;
    labels.push_back(Label("Dummy","Dummy Label",1,1.0));
    OneVsAll model(labels);
    ifstream fin(modelfile);
    model.Load(fin);
    fin.close();
    cout << " done." << endl;

    // Load data
    cout << "Loading data" << flush;
    VectorData<double> data(vector<double>(),0,0);
    ifstream fdata(datapath);
    data.LoadRow(fdata);
    fdata.close();
    cout << " done." << endl;
    VectorMapData mapdata(&data,vecmap,vecmaparg);

    cout << "Evaluating model on data" << flush;
    Label result=model.Eval(mapdata,0);
    cout << " done." << endl;
    delete vecmap;
    cout << "Result: " << result.Name() << endl;
  }
  catch (const std::string &error)
  { cerr << "Error: " << error << endl;
  }
}

