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

bool getBit(unsigned char byte, size_t position) // {{{
{ if (position>7)
    throw string("getBit: Position out of bounds");
  return (byte >> position) & 0x1;
} // }}}

int main(int argc, char **argv)
{ if (argc!=3)
  { cout << "Usage: " << argv[0] << " <input path> <output path>" << endl;
    return 1;
  }
  string input_path=argv[1];
  string output_path=argv[2];

  ifstream fin(input_path);
  if (!fin)
  { cerr << "Unable to open input file" << endl;
    return 2;
  }
  ofstream fout(output_path);
  if (!fout)
  { cerr << "Unable to open output file" << endl;
    fin.close();
    return 3;
  }

  // Output first feature
  size_t dim=100*8+1;
  double f0=1.0;
  fout.write((const char*)&dim,sizeof(size_t));
  fout.write((const char*)&f0,sizeof(double));
  
  for (size_t i=0; i<100; ++i)
  { unsigned char val;
    if (fin.eof())
      val=0;
    else
      val=fin.get();
    for (size_t pos=0; pos<8; ++pos)
    { double f=(double)getBit(val,pos);
      fout.write((const char*)&f,sizeof(double));
    }
  }
  fin.close();
  fout.close();
  return 0;
}
