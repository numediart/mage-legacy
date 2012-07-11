#pragma once

class obOlaBuffer {
    
  public:
    
    obOlaBuffer( int bufferLen );
    void ola( float *frame, int frameLen, int atLoc );
    void pull( float *buffer, int bufferLen );
    
  protected:
    
    float *rawData;
    int length, pos;
    
    int m, k, remain;
    int start, end;
};
