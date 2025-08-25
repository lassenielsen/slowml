#include <slowml/reinforcementlearning.hpp>
#include <algorithm>
#include <vector>
#include <sstream>
#include <cmath>
double sqr(double a) { return a*a; }
class RLSnake : public RLGame // {{{
{ public:
    struct snake
    { vector<pair<int,int> > myTail;
      pair<int,int> myHead;
      double myPoints;
      char myDir;
      bool myDead;
      size_t myBonus;
      size_t myLength;
      int myFov;
    };

    RLSnake(size_t w, size_t h, const vector<snake> &players) // {{{
    : myWidth(w)
    , myHeight(h)
    , myPlayers(players)
    { Init();
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
      { for (size_t t=0; t==0 || (t<100 && Dangerous(myPlayers[player].myHead.first,myPlayers[player].myHead.second)); ++t)
        { myPlayers[player].myHead.first=2+rand()%(myWidth-4);
          myPlayers[player].myHead.second=2+rand()%(myHeight-4);
        }
        myPlayers[player].myPoints=0.0d;
        myPlayers[player].myDir=rand()%4;
        myPlayers[player].myDead=false;
        myPlayers[player].myBonus=myWidth*myHeight;
        myPlayers[player].myLength=30;
        //myPlayers[player].myFov=fov;
      }
      for (size_t i=0; i==0 || (i<100 && Dangerous(myFruitX,myFruitY)); ++i)
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
      result.push_back(myFruitX<myPlayers[myTurn].myHead.first?1.0d/(myPlayers[myTurn].myHead.first-myFruitX):0.0d); // Fruit left
      result.push_back(myFruitX>myPlayers[myTurn].myHead.first?1.0d/(myFruitX-myPlayers[myTurn].myHead.first):0.0d); // Fruit right
      result.push_back(myFruitY<myPlayers[myTurn].myHead.second?1.0d/(myPlayers[myTurn].myHead.second-myFruitY):0.0d); // Fruit up
      result.push_back(myFruitY>myPlayers[myTurn].myHead.second?1.0d/(myFruitY-myPlayers[myTurn].myHead.second):0.0d); // Fruit down
      if (myPlayers[myTurn].myFov>0)
      { for (int x=myPlayers[myTurn].myHead.first-myPlayers[myTurn].myFov; x<=myPlayers[myTurn].myHead.first+myPlayers[myTurn].myFov; ++x)
          for (int y=myPlayers[myTurn].myHead.second-myPlayers[myTurn].myFov; y<=myPlayers[myTurn].myHead.second+myPlayers[myTurn].myFov; ++y)
            if (x!=myPlayers[myTurn].myHead.first || y!=myPlayers[myTurn].myHead.second)
              result.push_back(Dangerous(x,y)?1.0d:0.0d); // Tile is dangerous
      }
      else
      { for (int ang=0; ang<-myPlayers[myTurn].myFov; ++ang)
        { float dx,dy;
          switch (ang)
          { case 0:
              dx=0.0;
              dy=-1.0;
              break;
            case 1:
              dx=1.0;
              dy=0.0;
              break;
            case 2:
              dx=0.0;
              dy=1.0;
              break;
            case 3:
              dx=-1.0;
              dy=0.0;
              break;
            case 4:
              dx=1.0;
              dy=-1.0;
              break;
            case 5:
              dx=1.0;
              dy=1.0;
              break;
            case 6:
              dx=-1.0;
              dy=1.0;
              break;
            case 7:
              dx=-1.0;
              dy=-1.0;
              break;
            default:
              dx=0.5;
              dy=-1.0;
              break;
          }
          double feature=1.0;
          for (size_t d=0; d<100; ++d)
          { if (Dangerous(myPlayers[myTurn].myHead.first+(int)(dx*d),
                          myPlayers[myTurn].myHead.second+(int)(dy*d)))
              break;
            feature=feature/2.0;
          }
          result.push_back(feature);
        }
      }
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
           << "RLSnake::Step: Bad direction: " << myPlayers[myTurn].myDir;
        throw ss.str();
      }

      if (Dangerous(myPlayers[myTurn].myHead.first,myPlayers[myTurn].myHead.second))
      { myPlayers[myTurn].myDead=true;
        myAlive-=1;
        // Next turn
        if (myAlive>0)
        { for (size_t i=0; i==0 || myPlayers[myTurn].myDead; ++i)
            myTurn=(myTurn+1)%myPlayers.size();
        }
        return;
      }

      if (myPlayers[myTurn].myHead.first==myFruitX && myPlayers[myTurn].myHead.second==myFruitY)
      { myPlayers[myTurn].myPoints+=1.0d;
        myPlayers[myTurn].myPoints+=((double)myPlayers[myTurn].myBonus)/(myWidth*myHeight*10);
        myPlayers[myTurn].myBonus=myWidth*myHeight;
        ++myPlayers[myTurn].myLength;
        for (size_t i=0; i==0 || (i<100 && (Dangerous(myFruitX,myFruitY))); ++i)
        { myFruitX=1+rand()%(myWidth-2);
          myFruitY=1+rand()%(myHeight-2);
        }
      }
      //Add points for staying alive
      //myPoints+=((double)myLength)/100000.0d;
      //myPoints+=((double)(myWidth-abs((long)(myHeadX-myFruitX))))/1000000.0d;
      //myPoints+=((double)(myHeight-abs((long)(myHeadY-myFruitY))))/1000000.0d;
      if (myPlayers[myTurn].myBonus>0)
        --myPlayers[myTurn].myBonus;
      // Next turn
      for (size_t i=0; i==0 || myPlayers[myTurn].myDead; ++i)
        myTurn=(myTurn+1)%myPlayers.size();
    } // }}}
    size_t Players() const { return myPlayers.size(); }
    size_t Turn() const { return myTurn; }
    bool Done() const { return myAlive==0; }
    bool Dangerous(int x, int y) const // {{{
    { if (x<0 || y<0 || x>=myWidth || y>=myHeight)
        return true;
      for (size_t player=0; player<myPlayers.size(); ++player)
      { if (myPlayers[player].myDead)
          continue;
        const vector<pair<int,int>> &tail(myPlayers[player].myTail);
        if (find(tail.begin(),tail.end(),pair<int,int>(x,y))!=tail.end())
          return true;
      }
      return false;
          //|| (x>=10 && y>=3 && x<12 && y<myHeight-3)
          //|| (x>=myWidth-12 && y>=3 && x<myWidth-10 && y<myHeight-3)
    } // }}}
    bool Visible(int x, int y) const // {{{
    { for (size_t player=0; player<myPlayers.size(); ++player)
      { if (myPlayers[player].myDead)
          continue;
        if (myPlayers[player].myFov>0)
        { if (abs((long)(x-myPlayers[player].myHead.first))<=myPlayers[player].myFov && abs((long)(y-myPlayers[player].myHead.second))<=myPlayers[player].myFov)
            return true;
        }
        else
        { if (myPlayers[player].myHead.first-x==0 || myPlayers[player].myHead.second-y==0 ||
              ((myPlayers[player].myHead.first-x)==(myPlayers[player].myHead.second-y)) ||
              ((myPlayers[player].myHead.first-x)==(y-myPlayers[player].myHead.second)) )
          { int x1=x;
            int y1=y;
            int dx1=0;
            int dy1=0;
            if (myPlayers[player].myHead.first<x1)
              dx1=-1;
            else if (myPlayers[player].myHead.first>x1)
              dx1=1;
            if (myPlayers[player].myHead.second<y1)
              dy1=-1;
            else if (myPlayers[player].myHead.second>y1)
              dy1=1;
            bool unbroken=true;
            while (unbroken && x1!=myPlayers[player].myHead.first || y1!=myPlayers[player].myHead.second)
            { if (Dangerous(x1,y1))
                unbroken=false;
              x1+=dx1;
              y1+=dy1;
            }
            if (unbroken)
             return true;
          }
          else
            continue;
        }
      }
      return false;
    } // }}}
    vector<double> Score() const // {{{
    { vector<double> result;
      for (size_t player=0; player<myPlayers.size(); ++player)
        result.push_back(myPlayers[player].myPoints
                        +(0.1/sqrt(sqr(myPlayers[myTurn].myHead.first-myFruitX)+sqr(myPlayers[myTurn].myHead.second-myFruitY)+1))/(myWidth+myHeight));
        // Added element to score to prefer proximity to fruit
      return result;
    } // }}}
  
    std::string GameString() const // {{{
    { std::stringstream result;
      result << "Live Players: " << myAlive << endl;
      vector<double> scores=Score();
      for (size_t player=0; player<myPlayers.size(); ++player)
      { result << "Snake " << player << " score: " << scores[player] << endl; //" - " << myPlayers[player].myDead?string("dead"):string("alive") <<  endl;
      }
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

    size_t LiveSnakes() const { return myAlive; }
  private:
    size_t myWidth;
    size_t myHeight;
    size_t myFruitX;
    size_t myFruitY;
    size_t myTurn;
    vector<snake> myPlayers;
    int myAlive;
}; // }}}
