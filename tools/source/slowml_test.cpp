#include <slowml/onevsall.hpp>
#include <slowml/vectordata.hpp>
#include <slowml/vectormapdata.hpp>
#include <fstream>
#include <dirent.h>
#include <stdio.h>
#include <algorithm>

using namespace std;

vector<string> GetLabelsDirs(const string &path) // {{{
{ vector<string> result;
  DIR *dir = opendir(path.c_str());
  struct dirent *entry = readdir(dir);
  while (entry != NULL)
  {
    if (entry->d_type == DT_DIR && string(entry->d_name).substr(0,6)=="label_")
      result.push_back(entry->d_name);
    entry = readdir(dir);
  }
  closedir(dir);

  // Sort result
  std::sort(result.begin(),result.end());
  return result;
} // }}}
vector<string> GetVectorFiles(const string &path) // {{{
{ vector<string> result;
  DIR *dir = opendir(path.c_str());
  struct dirent *entry = readdir(dir);
  while (entry != NULL)
  {
    string name(entry->d_name);
    if (name.size()>4 && entry->d_type == DT_REG && name.substr(name.size()-4,4)==".vec")
      result.push_back(entry->d_name);
    entry = readdir(dir);
  }
  closedir(dir);
  return result;
} // }}}

int main(int argc, char **argv)
{ try
  { if (argc==2 && string("--help")==argv[1] || argc==1)
      throw string("Syntax is usemodel [--model|-m <modelpath>] [--vectormap|-vm <map=\"X->X\">] <datapath>");

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
    vector<Label> tmp_labels;
    tmp_labels.push_back(Label("Dummy","Dummy Label",1,1.0));
    OneVsAll model(tmp_labels);
    ifstream fin(modelfile);
    model.Load(fin);
    fin.close();
    cout << "Loaded model" << endl;
    
    size_t successes=0;
    size_t errors=0;
    // Find labels
    vector<string> labels=GetLabelsDirs(datapath);
    
    // Read data
    for (size_t label=0; label<labels.size(); ++label)
    { cout << "Label: " << labels[label] << endl;
      vector<string> vfiles=GetVectorFiles(datapath+"/"+labels[label]);
      for (auto vfile=vfiles.begin(); vfile!=vfiles.end(); ++vfile)
      { ifstream fin(datapath+"/"+labels[label]+"/"+*vfile);
        VectorData<double> data(vector<double>(),0,0);
        data.LoadRow(fin);
        fin.close();
        VectorMapData mapdata(&data,vecmap,vecmaparg);
        // Eval
        Label result=model.Eval(mapdata,0);
        if (result.Name()!=labels[label].substr(6))
        { cout << "Error on file: " << datapath << "/" <<labels[label] << "/" << *vfile << " (returned " << result.Name() << "!=" << labels[label].substr(6) << ")" << endl;
          ++errors;
        }
        else
        { cout << "Success on file: " << datapath << "/" <<labels[label] << "/" << *vfile << endl;
          ++successes;
        }
      }
    }
    delete vecmap;
    cout << "Total " << successes << " succeeded (" << (100.0 * successes) / (successes+errors) << "%)" << endl
         << "Total " << errors << " failed (" << (100.0 * errors) / (successes+errors) << "%)" << endl;
  }
  catch (const std::string &error)
  { cerr << "Error: " << error << endl;
  }
}

