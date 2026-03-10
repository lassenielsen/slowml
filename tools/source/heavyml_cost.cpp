#include <slowml/network.hpp>
#include <slowml/vec.hpp>
#include <iostream>
#include <algorithm>

using namespace std;

int main(int argc, char **argv)
{ try
  { if (argc==2 && string("--help")==argv[1] || argc==1)
      throw string("Syntax is heavyml_cost [--gd_lambda|-gdl <double=1>] [--model|-m <modelpath>] [--network|-n <network=\"#features->[3*[3*[all]],[#labels*[all]]]\">] [--filename_truths|-fnt <int=0>] [--vectormap|-vm <map=\"X->X\">] <datapath>");

    string datapath="./data";
    string network="#features->[3*[3*[all]],[#labels*[all]]]";
    //string vectormap="X->X";
    size_t filename_truths=0;
    double gd_lambda=1;
    Network *model=NULL;

    for (size_t arg=1; arg+1<argc; ++arg)
    { if (string("--gd_lambda")==argv[arg] || string("-gdl")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--gd_lambda must be succeeded by a value");
        stringstream ss;
        ss << argv[arg];
        ss >> gd_lambda;
        if (gd_lambda<=0)
          throw string("--gd_lambda must be succeeded by positive number");
      }
      else if ((string("--model")==argv[arg] || string("-m")==argv[arg]) && model==NULL)
      { ++arg;
        if (arg+1>=argc)
          throw string("--model must be succeeded by a value");
        ifstream fin(argv[arg]);
        model=new Network(1);
        model->LoadParameters(fin);
        fin.close();
        cout << "Loaded model" << endl;
      }
      else if (string("--network")==argv[arg] || string("-n")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--network must be succeeded by another arg");
       network=argv[arg];
      }
      //else if (string("--vectormap")==argv[arg] || string("-vm")==argv[arg])
      //{ ++arg;
      //  if (arg+1>=argc)
      //    throw string("--vectormap must be succeeded by another arg");
      //  vectormap=argv[arg];
      //}
      else if (string("--filename_truths")==argv[arg] || string("-fnt")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--filename_results must be succeeded by an integer");
        stringstream ss;
        ss << argv[arg];
        ss >> filename_truths;
      }
      else
        throw string("Unknown argument") + argv[arg];
    }

    datapath=argv[argc-1];

    // Parse map
    cout << "Parsing vector mapping" << flush;
    string vecmaparg;
    //VectorMap *vecmap=VectorMap::Parse(vectormap,vecmaparg);
    cout << " done." << endl;

    // Find labels
    vector<string> labels=GetLabelsDirs(datapath);

    // Read data
    cout << "Loading data" << endl;
    vector<vector<double> > data;
    // VectorData<double> data(vector<double>(),0,0);
    vector<vector<double>> truths;
    for (size_t label=0; label<labels.size(); ++label)
    { cout << "Label: " << labels[label] << flush;
      size_t count=0;
      vector<string> vfiles=GetVectorFiles(datapath+"/"+labels[label]);
      for (auto vfile=vfiles.begin(); vfile!=vfiles.end(); ++vfile)
      { ifstream fin(datapath+"/"+labels[label]+"/"+*vfile);
        vector<double> row=readvec(fin);
        data.push_back(row);
        //data.LoadRow(fin);
        fin.close();
        // Add filename truths
        vector<double> truth=IdVec(labels.size(),label);
        size_t endpos=vfile->rfind('.');
        if (endpos==string::npos && filename_truths>0)
        { cerr << "Ill formatted filename " << *vfile << endl;
          for (size_t t=0; t<filename_truths; ++t)
            truth.push_back(0.0);
        }
        else
        { for (size_t t=0; t<filename_truths; ++t)
          { size_t startpos=vfile->rfind('_',endpos);
            if (startpos==string::npos)
            { cerr << "Ill formatted filename truth " << *vfile << endl;
              truth.push_back(0.0);
            }
            else
            { string tstr=vfile->substr(startpos+1,endpos);
              endpos=startpos-1;
              stringstream ss;
              ss << tstr;
              double tval;
              ss >> tval;
              truth.push_back(tval);
            }
          }
        }
        truths.push_back(truth);
        ++count;
      }
      cout << ". Loaded " << count << " instances." << endl;
    }
    cout << "Loading data done." << endl;
    //VectorMapData mapdata(&data,vecmap,vecmaparg);

    // Parse network
    if (model==NULL)
    { stringstream c_features;
      c_features << (data.size()>0?data[0].size():0);
      stringstream c_labels;
      c_labels << labels.size();
      string network_str=string_replace(string_replace(network,"#features",c_features.str()),"#labels",c_labels.str());
      model=Network::Parse(network_str);
      network_str="";
    }

    double cost=model->Cost(data,truths,gd_lambda);
    cout << "Cost: " << cost << endl;
    delete model;
  }
  catch (const std::string &error)
  { cerr << "Error: " << error << endl;
  }
}

