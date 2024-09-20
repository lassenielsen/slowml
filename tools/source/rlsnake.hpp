#include <slowml/reinforcementlearning.hpp>
#include <algorithm>
#include <sstream>

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
    { myLength=3;
      myTail.clear();
      myHeadX=3+rand()%(myWidth-6);
      myHeadY=3+rand()%(myHeight-6);
      myFruitX=1+rand()%(myWidth-2);
      myFruitY=1+rand()%(myHeight-2);
      myPoints=0.0d;
      myDir=rand()%4;
      myDead=false;
    } // }}}
    vector<double> State() // {{{
    { vector<double> result;
      result.push_back(1.0d);
      // Make State
      result.push_back(myFruitX<myHeadX?1.0d:0.0d); // Fruit left
      result.push_back(myFruitX>myHeadX?1.0d:0.0d); // Fruit right
      result.push_back(myFruitY<myHeadY?1.0d:0.0d); // Fruit up
      result.push_back(myFruitY>myHeadY?1.0d:0.0d); // Fruit down
      for (int x=myHeadX-3; x<=myHeadX+3; ++x)
        for (int y=myHeadY-3; y<=myHeadY+3; ++y)
          if (x!=myHeadX || y!=myHeadY)
            result.push_back(Dangerous(x,y)?1.0d:0.0d); // Tile is dangerous
      for (int x=myHeadX-3; x<=myHeadX+3; ++x)
        for (int y=myHeadY-3; y<=myHeadY+3; ++y)
          if (x!=myHeadX || y!=myHeadY)
            result.push_back((x==myFruitX && y==myFruitY)?1.0d:0.0d); // Fruit position
      //result.push_back((double)(myDir&0x01?1.0d:0.0d));
      //result.push_back((double)(myDir&0x02?1.0d:0.0d));
      //for (int y=0; y<myHeight; ++y)
      //{ for (int x=0; x<myWidth; ++x)
      //  { if (x==myHeadX && y==myHeadY) // Head
      //    { result.push_back(1.0d);
      //      result.push_back(0.0d);
      //      result.push_back(0.0d);
      //    }
      //    else if (find(myTail.begin(),myTail.end(),pair<size_t,size_t>(x,y))!=myTail.end()) // Tail
      //    { result.push_back(0.0d);
      //      result.push_back(1.0d);
      //      result.push_back(0.0d);
      //    }
      //    else if (x==myFruitX && y==myFruitY) // Fruit
      //    { result.push_back(0.0d);
      //      result.push_back(0.0d);
      //      result.push_back(1.0d);
      //    }
      //    else // Empty
      //    { result.push_back(0.0d);
      //      result.push_back(0.0d);
      //      result.push_back(0.0d);
      //    }
      //  }
      //}
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
        ++myLength;
        while ((myHeadX==myFruitX && myHeadY==myFruitY) ||
               (Dangerous(myFruitX,myFruitY)))
        { myFruitX=1+rand()%(myWidth-2);
          myFruitY=1+rand()%(myHeight-2);
        }
      }
      myPoints+=((double)myLength)/1000.0d;
    } // }}}
    size_t Players() const { return 1; }
    size_t Turn() const { return 0; }
    bool Done() const { return myDead; }
    bool Dangerous(int x, int y) const // {{{
    { return (x<0 || y<0 || x>=myWidth || y>=myHeight
          || (x>=10 && y>=3 && x<12 && y<myHeight-3)
          || (x>=myWidth-12 && y>=3 && x<myWidth-10 && y<myHeight-3)
          || find(myTail.begin(),myTail.end(),pair<size_t,size_t>(x,y))!=myTail.end());
    } // }}}
    vector<double> Score() const // {{{
    { vector<double> result;
      result.push_back(myPoints);
      return result;
    } // }}}
  
    std::string GameString() const // {{{
    { std::stringstream result;
      result << std::string(myWidth+2,'_') << endl;
      for (size_t y=0;y<myHeight; ++y)
      { result << "|";
        for (size_t x=0;x<myWidth; ++x)
        { if (x==myHeadX && y==myHeadY)
            result << "O";
          else if (x==myFruitX && y==myFruitY)
            result << "$";
          else if (Dangerous(x,y))
            result << "o";
          else
            result << " ";
        }
        result << '|' << endl;
      }
      result << std::string(myWidth+2,'-');
      return result.str();
    } // }}}

  private:
    size_t myWidth;
    size_t myHeight;
    size_t myLength;
    vector<pair<size_t,size_t> > myTail;
    int myHeadX;
    int myHeadY;
    size_t myFruitX;
    size_t myFruitY;
    double myPoints;
    bool myDead;
    char myDir;
}; // }}}
