#include <slowml/vectordata.hpp>
#include <slowml/vectormapdata.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

int main(int argc, char **argv)
{ try
  { if (argc<2)
      throw string("Syntax is slowml_printvec [--cols <int=0>] [--startindex <int=0>] [--vectormap|-vm <map=\"X->X\">] <filename>");

    size_t cols=0;
    size_t startindex=0;
    string vectormap="X -> X";
    for (size_t arg=1; arg+1<argc; ++arg)
    { if (string("--cols")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--cols must be succeeded by another arg");
        stringstream ss;
        ss << argv[arg];
        ss >> cols;
        if (cols<=0)
          throw string("--cols must be succeeded by a positive integer");
      }
      else if (string("--startindex")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--cols must be succeeded by another arg");
        stringstream ss;
        ss << argv[arg];
        ss >> startindex;
      }
      else if (string("--vectormap")==argv[arg] || string("-vm")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--vectormap must be succeeded by another arg");
        vectormap=argv[arg];
      }
      else if (string("--vectormapfile")==argv[arg] || string("-vmf")==argv[arg])
      { ++arg;
        if (arg+1>=argc)
          throw string("--vectormap must be succeeded by another arg");
        vectormap=argv[arg];
      }
      else
        throw string("Unknown argument") + argv[arg];
    }

    ifstream fin(argv[argc-1]);
    VectorData<double> data(vector<double>{},0,0);
    data.LoadRow(fin);
    string vecmaparg;
    VectorMap *vecmap=VectorMap::Parse(vectormap,vecmaparg);
    VectorMapData mapdata(&data,vecmap,vecmaparg);
    for (size_t i=0; i<mapdata.Width(); ++i)
    { cout << (int)(mapdata.GetValue(0,i)*9.99);
      if (cols>0 && (i+startindex)%cols==0)
        cout << endl;
    }
    delete vecmap;
  }
  catch (const std::string &error)
  { cerr << "Error: " << error << endl;
  }
}

