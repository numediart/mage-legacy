#pragma once

#include <cstdlib>
#include <cstring>

class obOlaBuffer {
    
  public:
    
    obOlaBuffer( int bufferLen );
    void ola( float *frame, int frameLen, int atLoc );
    void pop( float *buffer, int bufferLen );
    
  protected:
    
    float *rawData;
    int length, pos;
    
    int m, k, remain;
    int start, end;
};
