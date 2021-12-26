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

int main(int argc, char **argv)
{ string images_path="";
  string labels_path="";

  if (argc!=3)
  { cout << "Usage: " << argv[0] << " <images path> <labels path>" << endl;
    return 1;
  }

  images_path=argv[1];
  labels_path=argv[2];

  ifstream img(images_path);
  ifstream lbl(labels_path);
  size_t magic=read32bituint(img);
  cout << "Images magic number is: " << magic << endl;
  magic=read32bituint(lbl);
  cout << "Labels magic number is: " << magic << endl;
  size_t count=read32bituint(img);
  cout << "Number of images is: " << count << endl;
  count=read32bituint(lbl);
  cout << "Number of labels is: " << count << endl;
  //cin.get();
  size_t rows=read32bituint(img);
  size_t cols=read32bituint(img);
  cout << "Images have dimnsion: " << rows << "x" << cols << endl;
  unsigned char val=0;
  size_t dim=rows*cols+1;
  double f0=1.0;
  for (size_t i=0; i<count; ++i)
  { lbl.read((char*)&val,1);
    cout << "Image " << i << " of the number " << (int)val << endl;
    stringstream fname;
    fname << "label_" << (int)val << "/";
    string folder=string("mkdir -p ") + fname.str();
    system(folder.c_str());
    fname << i << ".vec";
    ofstream fout(fname.str());
    fout.write((const char*)&dim,sizeof(size_t));
    fout.write((const char*)&f0,sizeof(double));
    for (size_t r=0; r<rows; ++r)
    { for (size_t c=0; c<cols; ++c)
      { img.read((char*)&val,1);
        double feature=((double)val)/256.0;
        fout.write((const char*)&feature,sizeof(double));
        cout << val/26;
      }
      cout << endl;
    }
    fout.close();
  }
  img.close();
  lbl.close();
  return 0;
}
