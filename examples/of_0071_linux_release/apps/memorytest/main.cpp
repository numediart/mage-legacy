/**
 *   @file    main.cpp
 *   @author  A. Moinet
 *   @brief   This application runs all the step of speech
 *            synthesis linearly. If run in valgring, it 
 *            shows the memory leaks (if any). Cannot use
 *            testApp because OF and audio are full of crap
 */


#include <cstdlib>
#include <string>
#include <cstring>
#include <fstream>
#include <queue>

#include "mage.h"

// --- QUEUE THINGS ---
const unsigned int nOfLookup = 1; // # of looked-up labels
const int labelQueueLen = 256; // max amount of labels that can wait
const int modelQueueLen = nOfLookup + 2; // max stored past models for generation
const int frameQueueLen = 200; // longest label 1 sec = 200 frames of 5 smsec

using namespace MAGE;

std::queue<std::string> parsefile(std::string filename) 
{
    std::queue<std::string> labellist;
    std::string line;
    std::ifstream myfile(filename.c_str());
    
    if (!myfile.is_open()) {
        printf("could not open file %s",filename.c_str());
        return labellist;
    }
    
    for (int k=0;getline(myfile, line);k++) {
        labellist.push(line);
    }
    
    myfile.close();
    
    return labellist;
}

int main(int argc, char **argv) {
    LabelQueue *labelQueue;
    ModelQueue *modelQueue;
    FrameQueue *frameQueue;
    Engine *engine;

    Label label;
    Model model;

    labelQueue = new MAGE::LabelQueue(labelQueueLen);
    modelQueue = new MAGE::ModelQueue(modelQueueLen);
    frameQueue = new MAGE::FrameQueue(frameQueueLen);

    // --- HTS Engine ---
    engine = new MAGE::Engine();
    engine->load(argc, argv);

    string s(argv[argc - 1]);
    std::queue<std::string> t = parsefile(s);

    while (!t.empty()) {
        label.setQuery(t.front());
        labelQueue->push(label);
        t.pop();
    }
    
    while (!labelQueue->isEmpty()) {
        labelQueue->pop(label);
        
        model.computeDuration( engine, &label );
        model.computeParameters( engine, &label );
        model.computeGlobalVariances( engine, &label );
    }
    
    delete engine;
    delete frameQueue;
    delete modelQueue;
    delete labelQueue;

    return 0;
}
