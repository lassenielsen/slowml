#include <slowml/label.hpp>
#include <sstream>
#include <iostream>
using namespace std;

int main()
{ try
  { bool result=true;
    Label tlabel("test","test label",2,3.0);
    if (tlabel.Name()!="test")
    { result=false;
      cerr << "Failed name consistency test" << endl;
    }
    if (tlabel.Description()!="test label")
    { result=false;
      cerr << "Failed description consistency test" << endl;
    }
    if (tlabel.Id()!=2)
    { result=false;
      cerr << "Failed id consistency test" << endl;
    }
    if (tlabel.Value()!=3.0)
    { result=false;
      cerr << "Failed id consistency test" << endl;
    }
    stringstream store;
    tlabel.Save(store);

    // cout << "Stored label: " << store.str() << endl;
    Label tlabel2("","",0);
    tlabel2.Load(store);
    if (tlabel2.Name()!="test")
    { result=false;
      cerr << "Failed name save-load test (" << tlabel2.Name() << "!=test)" << endl;
    }
    if (tlabel2.Description()!="test label")
    { result=false;
      cerr << "Failed description save-load test" << endl;
    }
    if (tlabel2.Id()!=2)
    { result=false;
      cerr << "Failed id save-load test" << endl;
    }
    if (tlabel2.Value()!=3.0)
    { result=false;
      cerr << "Failed id save-load test" << endl;
    }
    string str;
    store >> str;
    if (str.size()!=0)
    { result=false;
      cerr << "Failed extra data stored: " << str << endl;
    }

    if (!result)
    { cout << "There were errors" << endl;
      return 1;
    }
    else
    { cout << "All tests passed" << endl;
      return 0;
    }
  }
  catch (const std::string &error)
  { cerr << "Error: " << error << endl;
  }

}

