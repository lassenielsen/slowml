#include <vector>
#include <dirent.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <algorithm>

vector<double> readvec(std::istream &features) // {{{
{ size_t fcount;
  features.read((char*)&fcount,sizeof(size_t));
  vector<double> result(fcount);

  for (size_t i=0; i<fcount; ++i)
  { features.read((char*)&(result[i]),sizeof(double));
  }

  return result;
} // }}}
void writevec(std::vector<double> &data, std::ostream &features) // {{{
{ size_t fcount=data.size();
  features.write((char*)&fcount,sizeof(size_t));

  for (size_t i=0; i<fcount; ++i)
  { features.write((char*)&(data[i]),sizeof(double));
  }
} // }}}

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
std::string string_replace(const std::string &src,const std::string &search, const std::string &replace) // {{{
{ stringstream result;
  size_t index=0;
  while (index!=std::string::npos)
  { size_t next=src.find(search, index);
    if (next==std::string::npos)
    { result << src.substr(index);
      index=next;
    }
    else
    { result << src.substr(index,next-index) << replace;
      index=next+search.size();
    }
  }
  return result.str();
} // }}}
vector<double> IdVec(size_t length, size_t pos) // {{{
{ vector<double> result;
  for (size_t i=0; i<length; ++i)
  { if (i==pos)
      result.push_back(1.0);
    else
      result.push_back(0.0);
  }

  return result;
} // }}}

