#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;
size_t read32bituint(ifstream &fin) // {{{
{ unsigned char val=0;
  size_t result=0;
  fin.read((char*)&val,1);
  result=result*256+val;
  fin.read((char*)&val,1);
  result=result*256+val;
  fin.read((char*)&val,1);
  result=result*256+val;
  fin.read((char*)&val,1);
  result=result*256+val;
  return result;
} // }}}

void WriteCharVector(ofstream &fout, unsigned char c) // {{{
{ const double d1=1.0;
  const double d0=0.0;
  for (size_t s=0; s<c; ++s)
    fout.write((const char*)&d0,sizeof(double));
  fout.write((const char*)&d1,sizeof(double));
  for (size_t s=c+1; s<256; ++s)
    fout.write((const char*)&d0,sizeof(double));
} // }}}

int main(int argc, char **argv)
{ if (argc!=3)
  { cout << "Usage: " << argv[0] << " <data file> <dest folder>" << endl;
    return 1;
  }

  string data_path=argv[1];

  string dest_path=argv[2];

  ifstream data(data_path);
  size_t id=0;
  while (!data.eof())
  { string line;
    std::getline(data,line);
    int pos=line.find(':');
    if (pos>0)
    { // Create input and output vector
      ++id;
      stringstream fname;
      fname<<dest_path;
      fname<<"/data_"<<id;
      ofstream ifout(fname.str()+".ivec");
      ofstream ofout(fname.str()+".ovec");
      string text=line.substr(pos+2);
      size_t dim=(text.size()+1)*257;
      ifout.write((const char*)&dim,sizeof(size_t));
      ofout.write((const char*)&dim,sizeof(size_t));
      const double d1=1.0;
      // Add zero to inputs
      ifout.write((const char*)&d1,sizeof(double));
      WriteCharVector(ifout,0);
      for (size_t p=0; p<text.size(); ++p)
      { ifout.write((const char*)&d1,sizeof(double));
        WriteCharVector(ifout,text[p]);
        ofout.write((const char*)&d1,sizeof(double));
        WriteCharVector(ofout,text[p]);
      }
      ofout.write((const char*)&d1,sizeof(double));
      WriteCharVector(ofout,0);
      ifout.close();
      ofout.close();
    }
  }
  data.close();
  return 0;
}
