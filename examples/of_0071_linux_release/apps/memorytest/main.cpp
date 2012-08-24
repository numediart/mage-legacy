/**
 *   @file    main.cpp
 *   @author  A. Moinet
 *   @brief   This application runs all the step of speech
 *            synthesis linearly. If run in valgrind, it
 *            shows the memory leaks (if any). Cannot use
 *            testApp because OF and audio are full of crap
 *
 *   @command run script mem-test.sh (adapt paths as needed)
 */


#include <cstdlib>
#include <string>
#include <cstring>
#include <fstream>
#include <queue>

#include "mage.h"

using namespace MAGE;
// --- QUEUE THINGS ---
const int labelQueueLen = 256; // max amount of labels that can wait
const int modelQueueLen = nOfLookup + nOfBackup + 2; // max stored past models for generation
const int frameQueueLen = 200; // longest label 1 sec = 200 frames of 5 smsec

void optimizeParameters(MAGE::Engine *engine, MAGE::Model *model1, MAGE::Model *model2);

using namespace MAGE;

std::queue<std::string> parsefile(std::string filename) {
	std::queue<std::string> labellist;
	std::string line;
	std::ifstream myfile(filename.c_str());

	if (!myfile.is_open()) {
		printf("could not open file %s", filename.c_str());
		return labellist;
	}

	for (int k = 0; getline(myfile, line); k++) {
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
	Vocoder *vocoder;

	Label *label;//, label2;
	ModelMemory *memory = new MAGE::ModelMemory::ModelMemory();
	Model *model;
	//Model model2;

	labelQueue = new MAGE::LabelQueue(labelQueueLen);
	modelQueue = new MAGE::ModelQueue(modelQueueLen, memory);
	frameQueue = new MAGE::FrameQueue(frameQueueLen);

	// --- HTS Engine ---
	engine = new MAGE::Engine();
	engine->load(argc, argv);

	vocoder = new MAGE::Vocoder();

	string s(argv[argc - 1]);
	std::queue<std::string> t = parsefile(s);

	while (!t.empty()) {
		//NB labelQueue is big enough, otherwise, check isFull()
		label = labelQueue->next();
		label->setQuery(t.front());
		labelQueue->push();
		t.pop();
	}

	//    int sampleCount = 0;
	//    int hopLen = 240;
	int bufSize = 128;
	MAGE::Frame frame;
	float *outbuffer;
	outbuffer = new float[bufSize];

	//    labelQueue->pop(label);
	//    labelQueue->pop(label2);
	//
	//    model.computeDuration( engine, &label );
	//    model.computeParameters( engine, &label );
	//    model.computeGlobalVariances( engine, &label );
	//
	//    model2.computeDuration( engine, &label2 );
	//    model2.computeParameters( engine, &label2 );
	//    model2.computeGlobalVariances( engine, &label2 );
	//
	//    modelQueue->push(&model,1);
	//    modelQueue->push(&model2,1);
	//
	//    modelQueue->optimizeParameters(engine,MAGE::nOfLookup+1);
	//
	//    printf("done !\n");

	bool flag = true;

	while (!labelQueue->isEmpty()) {
		label = labelQueue->get();
		printf("pop label %s\n", label->getQuery().c_str());

		model = modelQueue->next();
		
		model->computeDuration(engine, label);
		model->computeParameters(engine, label);
		model->computeGlobalVariances(engine, label);
		
		modelQueue->push();
		labelQueue->pop();
		printf("push model\n");

		if (modelQueue->getNumOfItems() > nOfLookup + nOfBackup) {
			flag = false;
			modelQueue->optimizeParameters(engine, nOfBackup, nOfLookup);
			modelQueue->generate(frameQueue, nOfBackup);
			modelQueue->pop();
		} else if (modelQueue->getNumOfItems() > nOfLookup && flag) {
			modelQueue->optimizeParameters(engine, modelQueue->getNumOfItems() - nOfLookup - 1, nOfLookup);
			modelQueue->generate(frameQueue, modelQueue->getNumOfItems() - nOfLookup - 1);
		}

		while (!frameQueue->isEmpty()) {
			frameQueue->pop(&frame);
			vocoder->push(frame);
			for (int k = 0; k < bufSize; k++) {
				if (vocoder->ready())
					outbuffer[k] = 0.5 * vocoder->pop() / 32768;
			}
		}

		//        for ( int k=0; k<bufSize; k++ ) {
		//            if( sampleCount >= hopLen-1 ) { // if we hit the hop length            
		//                if( !frameQueue->isEmpty() ) {               
		//                    frameQueue->pop( &frame, 1 ); // we pop a speech parameter frame
		//                    vocoder->push(frame);
		//                }
		//                //olaBuffer->ola( sampleFrame, frameLen, k ); // OLA the frame
		//                sampleCount = 0; // and reset the sample count for next time
		//            } else {
		//                sampleCount++; // otherwise increment sample count
		//            }
		//            if (vocoder->ready())
		//                outbuffer[k] = vocoder->pop()/1000;
		//        }
	}

	delete engine;
	delete frameQueue;
	delete modelQueue;
	delete labelQueue;
	delete memory;
	delete vocoder;
	delete[] outbuffer;

	return 0;
}

void optimizeParameters(MAGE::Engine *engine, MAGE::Model *model1, MAGE::Model *model2) {
	static HTS_ModelSet ms = engine->getModelSet();
	static HTS_Global global = engine->getGlobal();
	static HTS_PStream pss = engine->getPStream();

	ModelMemory *mem = new MAGE::ModelMemory::ModelMemory();

	int i, j, k, l, m;
	int state, frame = 0, msd_frame = 0, static_length;
	int total_duration = 0;

	bool not_bound;

	total_duration = model1->getDuration() + model2->getDuration();

	//MODEL 1
	for (state = 0, frame = 0; state < ms.nstate; state++) // for every state
	{
		for (j = 0; j < model1->getState(state).duration; j++, frame++) // for every frame
		{
			if (model1->getState(state).lf0[0].msdFlag > global.msd_threshold[1]) // if voiced
				mem->voiced_unvoiced[frame] = true;
			else
				mem->voiced_unvoiced[frame] = false;
		}
	}

	//MODEL 2
	for (state = 0; state < ms.nstate; state++) // for every state
	{
		for (j = 0; j < model2->getState(state).duration; j++, frame++) // for every frame
		{
			if (model2->getState(state).lf0[0].msdFlag > global.msd_threshold[1]) // if voiced
				mem->voiced_unvoiced[frame] = true;
			else
				mem->voiced_unvoiced[frame] = false;
		}
	}

	for (i = 0; i < ms.nstream; i++) // for every stream : mgc, lf0, lpf
	{
		if (ms.stream[i].msd_flag) // for MSD
		{
			//MODEL 1
			for (state = 0, frame = 0, msd_frame = 0; state < ms.nstate; state++) // for every state
			{
				for (j = 0; j < model1->getState(state).duration; j++) // for every frame
				{
					if (mem->voiced_unvoiced[frame]) {
						// check current frame is MSD boundary or not 
						for (k = 0; k < ms.stream[i].window.size; k++) {
							not_bound = true;
							for (l = ms.stream[i].window.l_width[k]; l <= ms.stream[i].window.r_width[k]; l++) {
								if (frame + l < 0 || total_duration <= frame + l || !mem->voiced_unvoiced[frame + l]) // voiced unvoided
								{
									not_bound = false;
									break;
								}
							}

							static_length = (ms.stream[i].vector_length / ms.stream[i].window.size);

							for (l = 0; l < static_length; l++) {
								// ATTENTION !!! This is not the good way
								m = static_length * k + l;

								if (i == 1) // lf0
								{
									mem->mean[i][msd_frame][m] = model1->getState(state).lf0[m].mean;

									if (not_bound || k == 0)
										mem->ivar[i][msd_frame][m] = HTS_finv(model1->getState(state).lf0[m].vari);

									else
										mem->ivar[i][msd_frame][m] = 0.0;

									// ATTENTION !!! Loop assignment
									mem->gv_mean[i][m] = model1->getState(0).gv_lf0[m].mean;
									mem->gv_vari[i][m] = model1->getState(0).gv_lf0[m].vari;
								}
							}
						}
						mem->gv_switch[i][msd_frame] = model1->getState(state).lf0_gv_switch;
						msd_frame++;
					}
					frame++;
				}
			}

			//MODEL 2
			for (state = 0; state < ms.nstate; state++) // for every state
			{
				for (j = 0; j < model2->getState(state).duration; j++) // for every frame
				{
					if (mem->voiced_unvoiced[frame]) {
						// check current frame is MSD boundary or not 
						for (k = 0; k < ms.stream[i].window.size; k++) {
							not_bound = true;
							for (l = ms.stream[i].window.l_width[k]; l <= ms.stream[i].window.r_width[k]; l++) {
								if (frame + l < 0 || total_duration <= frame + l || !mem->voiced_unvoiced[frame + l]) // voiced unvoided
								{
									not_bound = false;
									break;
								}
							}

							static_length = (ms.stream[i].vector_length / ms.stream[i].window.size);

							for (l = 0; l < static_length; l++) {
								// ATTENTION !!! This is not the good way
								m = static_length * k + l;

								if (i == 1) // lf0
								{
									mem->mean[i][msd_frame][m] = model2->getState(state).lf0[m].mean;

									if (not_bound || k == 0)
										mem->ivar[i][msd_frame][m] = HTS_finv(model2->getState(state).lf0[m].vari);

									else
										mem->ivar[i][msd_frame][m] = 0.0;

									// ATTENTION !!! Loop assignment
									mem->gv_mean[i][m] = model2->getState(0).gv_lf0[m].mean;
									mem->gv_vari[i][m] = model2->getState(0).gv_lf0[m].vari;
								}
							}
						}
						mem->gv_switch[i][msd_frame] = model2->getState(state).lf0_gv_switch;
						msd_frame++;
					}
					frame++;
				}
			}
		}
		else // for non MSD 
		{
			//MODEL 1
			for (state = 0, frame = 0; state < ms.nstate; state++) // for every state
			{
				for (j = 0; j < model1->getState(state).duration; j++) // for every frame
				{
					for (k = 0; k < ms.stream[i].window.size; k++) {
						not_bound = true;
						for (l = ms.stream[i].window.l_width[k]; l <= ms.stream[i].window.r_width[k]; l++) {
							if (frame + l < 0 || total_duration <= frame + l) {
								not_bound = false;
								break;
							}
						}

						static_length = (ms.stream[i].vector_length / ms.stream[i].window.size);

						for (l = 0; l < static_length; l++) {
							// ATTENTION !!! This is not the good way
							m = static_length * k + l;

							if (i == 0) // mgcs
							{
								mem->mean[i][frame][m] = model1->getState(state).mgc[m].mean;

								if (not_bound || k == 0)
									mem->ivar[i][frame][m] = HTS_finv(model1->getState(state).mgc[m].vari);
								else
									mem->ivar[i][frame][m] = 0.0;

								// ATTENTION !!! Loop assignment
								mem->gv_mean[i][m] = model1->getState(0).gv_mgc[m].mean;
								mem->gv_vari[i][m] = model1->getState(0).gv_mgc[m].vari;
								mem->gv_switch[i][m] = model1->getState(0).mgc_gv_switch;
							}

							if (i == 2) // lpf
							{
								mem->mean[i][frame][m] = model1->getState(state).lpf[m].mean;

								if (not_bound || k == 0)
									mem->ivar[i][frame][m] = HTS_finv(model1->getState(state).lpf[m].vari);
								else
									mem->ivar[i][frame][m] = 0.0;

								// ATTENTION !!! Loop assignment
								mem->gv_mean[i][m] = model1->getState(0).gv_lpf[m].mean;
								mem->gv_vari[i][m] = model1->getState(0).gv_lpf[m].vari;
								mem->gv_switch[i][m] = model1->getState(0).lpf_gv_switch;
							}
						}
					}
					frame++;
				}
			}

			//MODEL 2
			for (state = 0; state < ms.nstate; state++) // for every state
			{
				for (j = 0; j < model2->getState(state).duration; j++) // for every frame
				{
					for (k = 0; k < ms.stream[i].window.size; k++) {
						not_bound = true;
						for (l = ms.stream[i].window.l_width[k]; l <= ms.stream[i].window.r_width[k]; l++) {
							if (frame + l < 0 || total_duration <= frame + l) {
								not_bound = false;
								break;
							}
						}

						static_length = (ms.stream[i].vector_length / ms.stream[i].window.size);

						for (l = 0; l < static_length; l++) {
							// ATTENTION !!! This is not the good way
							m = static_length * k + l;

							if (i == 0) // mgcs
							{
								mem->mean[i][frame][m] = model2->getState(state).mgc[m].mean;

								if (not_bound || k == 0)
									mem->ivar[i][frame][m] = HTS_finv(model1->getState(state).mgc[m].vari);
								else
									mem->ivar[i][frame][m] = 0.0;

								// ATTENTION !!! Loop assignment
								mem->gv_mean[i][m] = model2->getState(0).gv_mgc[m].mean;
								mem->gv_vari[i][m] = model2->getState(0).gv_mgc[m].vari;
								mem->gv_switch[i][m] = model2->getState(0).mgc_gv_switch;
							}

							if (i == 2) // lpf
							{
								mem->mean[i][frame][m] = model2->getState(state).lpf[m].mean;

								if (not_bound || k == 0)
									mem->ivar[i][frame][m] = HTS_finv(model1->getState(state).lpf[m].vari);
								else
									mem->ivar[i][frame][m] = 0.0;

								// ATTENTION !!! Loop assignment
								mem->gv_mean[i][m] = model2->getState(0).gv_lpf[m].mean;
								mem->gv_vari[i][m] = model2->getState(0).gv_lpf[m].vari;
								mem->gv_switch[i][m] = model2->getState(0).lpf_gv_switch;
							}
						}
					}
					frame++;
				}
			}
		}

		// just vector assigments in order to create a HTS_PStream object and use the default HTS_PStream_mlpg function
		pss.vector_length = ms.stream[i].vector_length; // vector length (include static and dynamic features) 
		pss.width = ms.stream[i].window.max_width * 2 + 1; // width of dynamic window 
		pss.win_size = ms.stream[i].window.size; // # of windows (static + deltas) 
		pss.win_l_width = ms.stream[i].window.l_width; // left width of windows 
		pss.win_r_width = ms.stream[i].window.r_width; // right width of windows 
		pss.win_coefficient = ms.stream[i].window.coefficient; // window coefficients 
		pss.gv_mean = mem->gv_mean[i]; // mean vector of GV 
		pss.gv_vari = mem->gv_vari[i]; // variance vector of GV 
		pss.gv_switch = mem->gv_switch[i]; // GV flag sequence 
		pss.static_length = ms.stream[i].vector_length / ms.stream[i].window.size; // static features length 


		if (ms.stream[i].msd_flag) // for MSD :: lf0
			pss.length = msd_frame; // stream length :: total number of frames
		else // for non MSD :: mgcs & lpf
			pss.length = frame; // stream length :: total number of frames

		if (HTS_ModelSet_use_gv(&ms, i)) // if GV is used 
			pss.gv_length = pss.length; //frame length for GV calculation
		else
			pss.gv_length = 0; //frame length for GV calculation

		// matrices for parameter generation 		
		pss.sm.mean = mem->mean[i]; // mean vector sequence 
		pss.sm.ivar = mem->ivar[i]; // inverse diag variance sequence
		pss.sm.g = mem->g[i]; // vector used in the forward substitution 
		pss.sm.wum = mem->wum[i]; // W' U^-1 mu 
		pss.sm.wuw = mem->wuw[i]; // W' U^-1 W  

		// output parameter vector in a pre-allocated memory
		pss.par = mem->par[i];

		HTS_PStream_mlpg(&pss); // parameter generation 

		FILE *file;

		file = fopen("2labs-1-2.mgc", "ab");


		if (i == 0) {
			for (int z = 0; z < pss.length; z++) {
				for (int zz = 0; zz < 35; zz++) {
					//printf("[%d,%d] %f\n",z,zz,mem->par[i][z][zz]);     
				}
				fwrite(mem->par[i][z], sizeof (double), 35, file);
			}
		}

		fclose(file);
	}

	return;
}
