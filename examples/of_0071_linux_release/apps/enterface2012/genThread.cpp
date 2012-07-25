#include "genThread.h"

genThread::genThread( LabelQueue *lab, ModelQueue *mq, FrameQueue *frm, Engine *eng, Model *mod ) {

    this->labelQueue = lab;
    this->modelQueue = mq;
    this->frameQueue = frm;
    this->engine = eng;
    this->model = mod;
    
    this->model->checkInterpolationWeights( engine );
}

void genThread::threadedFunction( void ) {
    //unsigned int k, s;


    while ( isThreadRunning() ) {
        
        if( !labelQueue->isEmpty() ) {
        
            labelQueue->pop( label );

            model->computeDuration( engine, &label );
            //TODO model->updateDuration() // to put a speed profile on state duration (put it inside compute duration ?)
            model->computeParameters( engine, &label );
            model->computeGlobalVariances( engine, &label );
            
            //model->optimizeParameters( engine );

            modelQueue->push( model, 1 );
            
            if( modelQueue->getNumOfItems() > nOfLookup ) {
                modelQueue->optimizeParameters(engine, nOfLookup+1);
                modelQueue->generate( engine, frameQueue, nOfLookup+1 );                
                modelQueue->pop();
            }
                
        } else {
        
            usleep(100);
        }
    }
}
