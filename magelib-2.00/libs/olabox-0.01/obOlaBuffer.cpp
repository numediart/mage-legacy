#include "obOlaBuffer.h"

obOlaBuffer::obOlaBuffer( int bufferLen ) {

    length = bufferLen; // init the OLA buffer to 0
    rawData = (float *) malloc( length*sizeof(float) );
    memset( rawData, 0, length*sizeof(float) );
    pos = 0; // 1st position is buffer head
}


void obOlaBuffer::ola( float *frame, int frameLen, int atLoc ) {
    
    start = (pos+atLoc)%length;
    end = (pos+atLoc+(frameLen-1))%length;

    for( k=0; k<frameLen; k++ ) {
    
        m = (pos+atLoc+k)%length; // we cycle through OLA buffer indices
        rawData[m] += frame[k]; // and add the current frame at location
    }
}


void obOlaBuffer::pop( float *buffer, int bufferLen ) {

    if( pos+bufferLen < length ) {
    
        // transfer samples to out in one block
        memcpy( buffer, &rawData[pos], bufferLen*sizeof(float) );
        memset( &rawData[pos], 0, bufferLen*sizeof(float) );
        
    } else {
    
        // remaining bit
        remain = length-pos;
        
        // transfer/clean from pos to end
        memcpy( buffer, &rawData[pos], remain*sizeof(float) );
        memset( &rawData[pos], 0, remain*sizeof(float) );
        
        // transfer/clean from beginning to remaining bit
        memcpy( &buffer[remain], rawData, (bufferLen-remain)*sizeof(float) );
        memset( rawData, 0, (bufferLen-remain)*sizeof(float) );
    }
    
    // finally move pos to next
    pos = (pos+bufferLen)%length;
}
