#pragma once
#include <string>
#include <istream>
#include <ostream>

class Label // {{{
{ public:
    Label(const std::string &name, const std::string &desc, size_t id, double value=0.0)
    : myId(id)
    , myValue(value)
    , myName(name)
    , myDescription(desc)
    {}
    virtual ~Label() {}

    size_t Id() const { return myId; }
    double Value() const { return myValue; }
    const std::string &Name() const { return myName; }
    const std::string &Description() const { return myDescription; }
    
    void Save(std::ostream &dest) const // {{{
    { size_t count=Id();
      dest.write((const char*)&count,sizeof(size_t));
      dest.write((const char*)&myValue,sizeof(double));
      count=Name().size();
      dest.write((const char*)&count,sizeof(size_t));
      dest.write(Name().c_str(),count);
      count=Description().size();
      dest.write((const char*)&count,sizeof(size_t));
      dest.write(Description().c_str(),count);
    } // }}}
    
    void Load(std::istream &src) // {{{
    { size_t count;
      src.read((char*)&count,sizeof(size_t));
      myId=count;
      src.read((char*)&myValue,sizeof(double));
      // Name
      src.read((char*)&count,sizeof(size_t));
      { char data[count+1];
        src.read(&data[0],count);
        data[count]=0;
        myName=data;
      }
      // Description
      src.read((char*)&count,sizeof(size_t));
      { char data[count+1];
        src.read(&data[0],count);
        data[count]=0;
        myDescription=data;
      }
    } // }}}

  private:
    size_t myId;
    double myValue;
    std::string myName;
    std::string myDescription;
}; // }}}
