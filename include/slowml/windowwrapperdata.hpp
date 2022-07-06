#pragma once
#include <slowml/data.hpp>
#include <vector>

template<class T> class WindowWrapperData : public Data<T> // {{{
{ public:
    WindowWrapperData(size_t dwidth, size_t dheight, size_t rowsize, size_t woffset_x, size_t woffset_y, size_t wwidth, size_t wheight, size_t swidth, size_t sheight, Data<T> *data=NULL);
    WindowWrapperData(size_t dwidth, size_t dheight, size_t rowsize, size_t woffset_x, size_t woffset_y, size_t wwidth, size_t wheight, size_t swidth, size_t sheight, std::vector<T> *data=NULL);
    virtual ~WindowWrapperData();

    T GetValue(size_t instance, size_t i) const;
    void SetValue(size_t instance, size_t i, const T &val);
    size_t ImgWidth() const { return myImgWidth; }
    size_t ImgHeight() const { return myImgHeight; }
    size_t OffsetX() const { return myOffsetX; }
    size_t OffsetY() const { return myOffsetY; }
    void SetOffset(size_t x, size_t y) { myOffsetX=x; myOffsetY=y; if (!Check) throw string("Error in WindowWrapperData::SetOffset"); }
    size_t WindowWidth() const { return myWindowWidth; }
    size_t WindowHeight() const { return myWindowHeight; }
    size_t ScaledWidth() const { return myWindowWidth; }
    size_t ScaledHeight() const { return myWindowHeight; }
    void SetScaled(size_t w, size_t h) { myScaledWidth=w; myScaledHeight=h; if (!Check) throw string("Error in WindowWrapperData::SetOffset"); }
    bool Check() const { return myOffsetX+myScaledWidth>=myImgWidth &&
                                myOffsetY+myScaledHeight>=myImgHeight; }

  private:
    Data<T> *myInstancesPtr;
    std::vector<T> *myDataPtr;
    size_t myDataWidth;

    size_t myImgWidth;
    size_t myImgHeight;
    size_t myOffsetX;
    size_t myOffsetY;
    size_t myWindowWidth;
    size_t myWindowHeight;
    size_t myScaledWidth;
    size_t myScaledHeight;
}; // }}}

template <class T> WindowWrapperData<T>::WindowWrapperData(size_t dwidth, size_t dheight, size_t rowsize, size_t woffset_x, size_t woffset_y, size_t wwidth, size_t wheight, size_t swidth, size_t sheight, Data<T> *data) // {{{
: Data<T>(wwidth*wheight,dheight)
, myInstancesPtr(data)
, myDataPtr(NULL)
, myDataWidth(dwidth)
, myImgWidth(rowsize)
, myImgHeight(dwidth/rowsize)
, myOffsetX(woffset_x)
, myOffsetY(woffset_y)
, myWindowWidth(wwidth)
, myWindowHeight(wheight)
, myScaledWidth(swidth)
, myScaledHeight(sheight)
{ if (myDataWidth>0 && Data<T>::Height()>0 && myDataWidth*Data<T>::Height()!=myDataPtr->size())
    throw string("Vector does not have the correct dimensions");
  if (myOffsetX+myScaledWidth>=myImgWidth) // x out of bounds
    throw string("Window exceeds image width");
  if (myOffsetY+myScaledHeight>=myImgHeight) // y out of bounds
    throw string("Window exceeds image height");
} // }}}
template <class T> WindowWrapperData<T>::WindowWrapperData(size_t dwidth, size_t dheight, size_t rowsize, size_t woffset_x, size_t woffset_y, size_t wwidth, size_t wheight, size_t swidth, size_t sheight, std::vector<T> *data) // {{{
: Data<T>(wwidth*wheight,dheight)
, myInstancesPtr(NULL)
, myDataPtr(data)
, myDataWidth(dwidth)
, myImgWidth(rowsize)
, myImgHeight(dwidth/rowsize)
, myOffsetX(woffset_x)
, myOffsetY(woffset_y)
, myWindowWidth(wwidth)
, myWindowHeight(wheight)
, myScaledWidth(swidth)
, myScaledHeight(sheight)
{ if (myDataWidth>0 && Data<T>::Height()>0 && myDataWidth*Data<T>::Height()!=myDataPtr->size())
    throw string("Vector does not have the correct dimensions");
  if (myOffsetX+myScaledWidth>=myImgWidth) // x out of bounds
    throw string("Window exceeds image width");
  if (myOffsetY+myScaledHeight>=myImgHeight) // y out of bounds
    throw string("Window exceeds image height");
} // }}}

template <class T> WindowWrapperData<T>::~WindowWrapperData() // {{{
{ // Do not delete data
} // }}}

template <class T> T WindowWrapperData<T>::GetValue(size_t instance, size_t i) const // {{{
{ if (i>Data<T>::Width()) // Index out of bounds
    throw string("WindowWrapperData::GetValue index out of bounds");
  
  size_t pos_x=i%myWindowWidth;
  size_t pos_y=i/myWindowWidth;

  // Calculate max and min x and y for scaled window
  size_t min_sx=pos_x*myWindowWidth/myScaledWidth;
  size_t max_sx=max((pos_x+1)*myWindowWidth/myScaledWidth-1,min_sx);
  size_t min_sy=pos_y*myWindowHeight/myScaledHeight;
  size_t max_sy=max((pos_y+1)*myWindowHeight/myScaledHeight-1,min_sy);

  T res=0.0;

  for (size_t x=min_sx; x<=max_sx; ++x)
    for(size_t y=min_sy; y<=max_sy; ++y)
    { size_t idx=myOffsetX+myImgWidth*myOffsetY+x+myImgWidth*y;
      if (myDataPtr!=NULL)
        res+=myDataPtr->at(instance*myDataWidth+idx);
      else
        res+=myInstancesPtr->GetValue(instance,idx);
    }
  return res;
} // }}}

template<class T> void WindowWrapperData<T>::SetValue(size_t instance, size_t i, const T &val) // {{{
{ throw string("WindowWrapperData::SetValue not implemented");
} // }}}
