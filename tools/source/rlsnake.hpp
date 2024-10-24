#include <slowml/reinforcementlearning.hpp>
#include <algorithm>
#include <sstream>

#define fow 2

class RLSnake : public RLGame // {{{
{ public:
    RLSnake(size_t w, size_t h) // {{{
    : myWidth(w)
    , myHeight(h)
    , myPoints(0.0d)
    { Init();
    } // }}}
    virtual ~RLSnake() { }

    RLGame *Copy() const { return new RLSnake(*this); }
    void Init() // {{{
    { myLength=30;
      myTail.clear();
      for (size_t i=0; i==0 || (i<100 && Dangerous(myHeadX,myHeadY)); ++i)
      { myHeadX=3+rand()%(myWidth-6);
        myHeadY=3+rand()%(myHeight-6);
      }
      for (size_t i=0; i==0 || (i<100 && ((myHeadX==myFruitX && myHeadY==myFruitY) || Dangerous(myFruitX,myFruitY))); ++i)
      { myFruitX=1+rand()%(myWidth-2);
        myFruitY=1+rand()%(myHeight-2);
      }
      myPoints=0.0d;
      myDir=rand()%4;
      myDead=false;
      myBonus=myWidth*myHeight;
    } // }}}
    vector<double> State() // {{{
    { vector<double> result;
      result.push_back(1.0d);
      // Make State
      result.push_back(myFruitX<myHeadX?1.0d:0.0d); // Fruit left
      result.push_back(myFruitX>myHeadX?1.0d:0.0d); // Fruit right
      result.push_back(myFruitY<myHeadY?1.0d:0.0d); // Fruit up
      result.push_back(myFruitY>myHeadY?1.0d:0.0d); // Fruit down
      for (int x=myHeadX-fow; x<=myHeadX+fow; ++x)
        for (int y=myHeadY-fow; y<=myHeadY+fow; ++y)
          if (x!=myHeadX || y!=myHeadY)
            result.push_back(Dangerous(x,y)?1.0d:0.0d); // Tile is dangerous
      //Done
      return result;
    } // }}}
    vector<vector<double> > Choices() // {{{
    { vector<vector<double> > choices;
      // Add Choices
      // Add up
      { vector<double> move;
        move.push_back(1.0d);
        move.push_back(0.0d);
        move.push_back(0.0d);
        move.push_back(0.0d);
        choices.push_back(move);
      }
      // Add Down
      { vector<double> move;
        move.push_back(0.0d);
        move.push_back(1.0d);
        move.push_back(0.0d);
        move.push_back(0.0d);
        choices.push_back(move);
      }
      // Add Left
      { vector<double> move;
        move.push_back(0.0d);
        move.push_back(0.0d);
        move.push_back(1.0d);
        move.push_back(0.0d);
        choices.push_back(move);
      }
      // Add Right
      { vector<double> move;
        move.push_back(0.0d);
        move.push_back(0.0d);
        move.push_back(0.0d);
        move.push_back(1.0d);
        choices.push_back(move);
      }
      return choices;
    } // }}}
    void Step(const std::vector<double> &inputs) // {{{
    { if (myDead)
        return;
      //cout << "Stepping from " << myX << "," << myY << "->";
      // Determine movce
      double move_confidence=-1.0d;
      // Check if move up
      if (inputs[0]>=move_confidence)
      { myDir=0;
        move_confidence=inputs[0];
      }
      // Check if move down
      if (inputs[1]>=move_confidence)
      { myDir=1;
        move_confidence=inputs[1];
      }
      // Check if move left
      if (inputs[2]>=move_confidence)
      { myDir=2;
        move_confidence=inputs[2];
      }
      // Check if move right
      if (inputs[3]>=move_confidence)
      { myDir=3;
        move_confidence=inputs[3];
      }

      // Move tail
      while (myTail.size()>=myLength)
        myTail.pop_back();

      myTail.insert(myTail.begin(),pair<size_t,size_t>(myHeadX,myHeadY));

      // Perform move
      if (myDir==0)
        --myHeadY;
      else if (myDir==1)
        ++myHeadY;
      else if (myDir==2)
        --myHeadX;
      else if (myDir==3)
        ++myHeadX;
      else
      { std::stringstream ss;
        ss << GameString() << endl
           << "RLSnake::Step: Bad direction: " << myDir;
        throw ss.str();
      }

      if (Dangerous(myHeadX,myHeadY))
      { myDead=true;
        return;
      }

      if (myHeadX==myFruitX && myHeadY==myFruitY)
      { myPoints+=1.0d;
        myPoints+=((double)myBonus)/(myWidth*myHeight*10);
        myBonus=myWidth*myHeight;
        ++myLength;
        for (size_t i=0; i==0 || (i<100 && ((myHeadX==myFruitX && myHeadY==myFruitY) || Dangerous(myFruitX,myFruitY))); ++i)
        { myFruitX=1+rand()%(myWidth-2);
          myFruitY=1+rand()%(myHeight-2);
        }
      }
      //Add points for staying alive
      //myPoints+=((double)myLength)/100000.0d;
      //myPoints+=((double)(myWidth-abs((long)(myHeadX-myFruitX))))/1000000.0d;
      //myPoints+=((double)(myHeight-abs((long)(myHeadY-myFruitY))))/1000000.0d;
      if (myBonus>0)
        --myBonus;
    } // }}}
    size_t Players() const { return 1; }
    size_t Turn() const { return 0; }
    bool Done() const { return myDead; }
    bool Dangerous(int x, int y) const // {{{
    { return (x<0 || y<0 || x>=myWidth || y>=myHeight
          //|| (x>=10 && y>=3 && x<12 && y<myHeight-3)
          //|| (x>=myWidth-12 && y>=3 && x<myWidth-10 && y<myHeight-3)
          || find(myTail.begin(),myTail.end(),pair<size_t,size_t>(x,y))!=myTail.end());
    } // }}}
    bool Visible(int x, int y) const // {{{
    { return abs(x-myHeadX)<=fow && abs(y-myHeadY)<=fow;
    } // }}}
    vector<double> Score() const // {{{
    { vector<double> result;
      result.push_back(myPoints-(myDead?10.0:0.0));
      return result;
    } // }}}
  
    std::string GameString() const // {{{
    { std::stringstream result;
      result << std::string(myWidth+2,'_') << "\n\r";
      for (size_t y=0;y<myHeight; ++y)
      { result << "|";
        for (size_t x=0;x<myWidth; ++x)
        { if (x==myHeadX && y==myHeadY)
            result << "O";
          else if (x==myFruitX && y==myFruitY)
            result << "$";
          else if (Dangerous(x,y))
            result << "#";
          else if (Visible(x,y))
            result << ".";
          else
            result << " ";
        }
        result << "|\n\r";
      }
      result << std::string(myWidth+2,'-');
      return result.str();
    } // }}}

  private:
    size_t myWidth;
    size_t myHeight;
    size_t myLength;
    size_t myBonus;
    vector<pair<size_t,size_t> > myTail;
    int myHeadX;
    int myHeadY;
    size_t myFruitX;
    size_t myFruitY;
    double myPoints;
    bool myDead;
    char myDir;
}; // }}}
