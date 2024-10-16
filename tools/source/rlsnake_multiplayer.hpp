#include <slowml/reinforcementlearning.hpp>
#include <algorithm>
#include <vector>
#include <sstream>

#define fow 3

class RLSnake : public RLGame // {{{
{ private:
    struct snake
    { vector<pair<size_t,size_t> > myTail;
      pair<size_t,size_t> myHead;
      double myPoints;
      char myDir;
      bool myDead;
      size_t myBonus;
      size_t myLength;
      size_t myFow;
    };
  public:
    RLSnake(size_t w, size_t h, size_t players) // {{{
    : myWidth(w)
    , myHeight(h)
    { for (size_t p=0; p<players; ++p)
        myPlayers.push_back(snake());
      Init();
    } // }}}
    virtual ~RLSnake() // {{{
    {
    } // }}}

    RLGame *Copy() const { return new RLSnake(*this); }
    void Init() // {{{
    { for (size_t player=0; player<myPlayers.size(); ++player)
      { myPlayers[player].myTail.clear();
      }
      for (size_t player=0; player<myPlayers.size(); ++player)
      { for (size_t t=0; t==0 || (t<100 && isDangerous(myPlayers[player].myHead.first,myPlayers[player].myHead.second)); ++t)
        { myPlayers[player].myHead.first=2+rand()%(myWidth-4);
          myPlayers[player].myHead.second=2+rand()%(myHeight-4);
        }
        myPlayers[player].myPoints=0.0d;
        myPlayers[player].myDir=rand()%4;
        myPlayers[player].myDead=false;
        myPlayers[player].myBonus=myWidth*myHeight;
        myPlayers[player].myLength=30;
        myPlayers[player].myFow=fow;
      }
      for (size_t i=0; i==0 || (i<100 && ((myHeadX==myFruitX && myHeadY==myFruitY) || Dangerous(myFruitX,myFruitY))); ++i)
      { myFruitX=1+rand()%(myWidth-2);
        myFruitY=1+rand()%(myHeight-2);
      }
      myAlive=myPlayers.size();
      myTurn=rand()%myPlayers.size();
    } // }}}
    vector<double> State() // {{{
    { vector<double> result;
      result.push_back(1.0d);
      // Make State
      result.push_back(myFruitX<myPlayers[myTurn].myHead.first?1.0d:0.0d); // Fruit left
      result.push_back(myFruitX>myPlayers[myTurn].myHead.first?1.0d:0.0d); // Fruit right
      result.push_back(myFruitY<myPlayers[myTurn].myHead.second?1.0d:0.0d); // Fruit up
      result.push_back(myFruitY>myPlayers[myTurn].myHead.second?1.0d:0.0d); // Fruit down
      for (int x=myPlayers[myTurn].myHead.first-myPlayers[myTurn].myFow; x<=myPlayers[myTurn].myHead.first+myPlayers[myTurn].myFow; ++x)
        for (int y=myPlayers[myTurn].myHead.second-myPlayers[myTurn].myFow; y<=myPlayers[myTurn].myHead.second+myPlayers[myTurn].myFow; ++y)
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
    { if (myAlive==0)
        return;
      for (size_t i=0; i==0 || myPlayers[myTurn].myDead; ++i)
        myTurn=(myTurn+1)%myPlayers.size();

      //cout << "Stepping from " << myX << "," << myY << "->";
      // Determine movce
      double move_confidence=-1.0d;
      // Check if move up
      if (inputs[0]>=move_confidence)
      { myPlayers[myTurn].myDir=0;
        move_confidence=inputs[0];
      }
      // Check if move down
      if (inputs[1]>=move_confidence)
      { myPlayers[myTurn].myDir=1;
        move_confidence=inputs[1];
      }
      // Check if move left
      if (inputs[2]>=move_confidence)
      { myPlayers[myTurn].myDir=2;
        move_confidence=inputs[2];
      }
      // Check if move right
      if (inputs[3]>=move_confidence)
      { myPlayers[myTurn].myDir=3;
        move_confidence=inputs[3];
      }

      // Move tail
      while (myPlayers[myTurn].myTail.size()>=myPlayers[myTurn].myLength)
        myPlayers[myTurn].myTail.pop_back();

      myPlayers[myTurn].myTail.insert(myPlayers[myTurn].myTail.begin(),myPlayers[myTurn].myHead);

      // Perform move
      if (myPlayers[myTurn].myDir==0)
        --myPlayers[myTurn].myHead.second;
      else if (myPlayers[myTurn].myDir==1)
        ++myPlayers[myTurn].myHead.second;
      else if (myPlayers[myTurn].myDir==2)
        --myPlayers[myTurn].myHead.first;
      else if (myPlayers[myTurn].myDir==3)
        ++myPlayers[myTurn].myHead.first;
      else
      { std::stringstream ss;
        ss << GameString() << endl
           << "RLSnake::Step: Bad direction: " << myDir;
        throw ss.str();
      }

      if (Dangerous(myPlayers[myTurn].myHead.first,myPlayers[myTurn].myHead.second))
      { myPlayers[myTurn].myDead=true;
        return;
      }

      if (myPlayers[myTurn].myHead.first==myFruitX && myPlayers[myTurn].myHead.second==myFruitY)
      { myPoints+=1.0d;
        myPoints+=((double)myBonus)/(myWidth*myHeight*10);
        myBonus=myWidth*myHeight;
        ++myLength;
        for (size_t i=0; i==0 || (i<100 && (Dangerous(myFruitX,myFruitY))); ++i)
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
    size_t Players() const { return myPlayers; }
    size_t Turn() const { return myTurn; }
    bool Done() const { return myDead; }
    bool Dangerous(int x, int y) const // {{{
    { if (x<0 || y<0 || x>=myWidth || y>=myHeight)
        return true;
      for (size_t player=0; player<myPlayers.size(); ++player]
        if (find(myPlayers[player].myTail.begin(),myPlayers[player].myTail.end(),pair<size_t,size_t>(x,y))!=myPlayers[player].myTail.end());
          return true;
      return false;
          //|| (x>=10 && y>=3 && x<12 && y<myHeight-3)
          //|| (x>=myWidth-12 && y>=3 && x<myWidth-10 && y<myHeight-3)
    } // }}}
    bool Visible(int x, int y) const // {{{
    { for (size_t player=0; player<myPlayers.size(); ++player)
        if (abs(x-myPlayers[myTurn].myHead.first)<=fow && abs(y-myPlayers[myTurn].myHeadY)<=myPlayers[myTurn].myFow)
          return true;
      return false;
    } // }}}
    vector<double> Score() const // {{{
    { vector<double> result;
      for (size_t player=0; player<myPlayers.size(); ++player)
        result.push_back(myPlayers[player].myPoints-(myPlayers[myTurn].myDead?10.0:0.0));
      return result;
    } // }}}
  
    std::string GameString() const // {{{
    { std::stringstream result;
      result << std::string(myWidth+2,'_') << endl;
      for (size_t y=0;y<myHeight; ++y)
      { result << "|";
        for (size_t x=0;x<myWidth; ++x)
        { bool head=false;
          for (size_t player=0; player<myPlayers.size(); ++player)
            if (x==myPlayers[player].myHead.first && y==myPlayers[player].myHead.second)
              head=true;
          if (head)
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
        result << '|' << endl;
      }
      result << std::string(myWidth+2,'-');
      return result.str();
    } // }}}

  private:
    size_t myWidth;
    size_t myHeight;
    size_t myFruitX;
    size_t myFruitY;
    size_t myTurn;
    vector<snake> myPlayers;
    int myAlive;
}; // }}}
