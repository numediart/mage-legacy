 /* ----------------------------------------------------------------------------------------------- */
 /* 																								*/
 /* 	This file is part of MAGE / pHTS( the performative HMM-based speech synthesis system )		*/
 /* 																								*/
 /* 	MAGE / pHTS is free software: you can redistribute it and/or modify it under the terms		*/
 /* 	of the GNU General Public License as published by the Free Software Foundation, either		*/
 /* 	version 3 of the license, or any later version.												*/
 /* 																								*/
 /* 	MAGE / pHTS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;	*/	
 /* 	without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	*/
 /* 	See the GNU General Public License for more details.										*/
 /* 																								*/	
 /* 	You should have received a copy of the GNU General Public License along with MAGE / pHTS.	*/ 
 /* 	If not, see http://www.gnu.org/licenses/													*/
 /* 																								*/
 /* 																								*/	
 /* 	Copyright 2011 University of Mons :															*/
 /* 																								*/	
 /* 			Numediart Institute for New Media Art( www.numediart.org )							*/
 /* 			Acapela Group ( www.acapela-group.com )												*/
 /* 																								*/
 /* 																								*/
 /* 	 Developed by :																				*/
 /* 																								*/
 /* 		Maria Astrinaki, Geoffrey Wilfart, Alexis Moinet, Nicolas d'Alessandro, Thierry Dutoit	*/
 /* 																								*/
 /* ----------------------------------------------------------------------------------------------- */

#pragma once

#include <fstream>

#include "Constants.h"
#include "MathFunctions.h"

#include "Frame.h"
#include "Label.h"
#include "Vocoder.h"

#include "State.h"
#include "Model.h"
#include "Distribution.h"
#include "MSDistribution.h"

#include "MemQueue.h"
#include "LabelQueue.h"
#include "ModelQueue.h"
#include "FrameQueue.h"

namespace MAGE 
{
	class Mage 
	{
		public:
		
			// constructor
			Mage( void );
			Mage( int argc, char ** argv );
			Mage( std::string confFilename );
			
			//destructor
			~Mage( void );
		
			// getters
			inline Frame getFrame ( void ){ return( this->frame ); };
			inline Label getLabel ( void ){ return( this->label ); };
			inline Model * getModel( void ){ return( this->model ); };
		
			inline Engine  * getEngine ( void ){ return( this->engine );  };		
			inline Vocoder * getVocoder( void ){ return( this->vocoder ); };

			inline LabelQueue * getLabelQueue( void ){ return( this->labelQueue ); };
			inline ModelQueue * getModelQueue( void ){ return( this->modelQueue ); };
			inline FrameQueue * getFrameQueue( void ){ return( this->frameQueue ); };

			inline double getSpeed ( void ){ return( this->hopLen ); };
			inline double getLabelSpeed ( void ){ return( this->labelSpeed ); };
			
			double getPitch ( void );
			double getAlpha ( void );
			double getGamma ( void );
			double getPOrder( void );
			double getVolume( void );
		
			double getDuration( void );

			//setters
			inline void setFrame( Frame   aframe ){ this->frame = aframe; };
			inline void setLabel( Label   alabel ){ this->label = alabel; };
			inline void setModel( Model * amodel ){ this->model = amodel; };
		
			inline void setEngine ( Engine  * aengine  ){ this->engine  = aengine;  };		
			inline void setVocoder( Vocoder * avocoder ){ this->vocoder = avocoder; };

			inline void setLabelQueue( LabelQueue * alabelQueue ){ this->labelQueue = alabelQueue; };
			inline void setModelQueue( ModelQueue * amodelQueue ){ this->modelQueue = amodelQueue; };
			inline void setFrameQueue( FrameQueue * aframeQueue ){ this->frameQueue = aframeQueue; };

			inline void setLabelSpeed ( double aLabelSpeed ){ this->labelSpeed = aLabelSpeed; }; // less reactive

			void setAlpha ( double alpha  );
			void setGamma ( double gamma  );
			void setPOrder( double glitch );
			void setVolume( double volume );
			void setPitch ( double pitch,  int action );
			void setSpeed ( double aspeed, int action );

			void setDuration( int * updateFunction, int action );

			// methods
			void run( void );	
			bool popLabel ( void );
			void pushLabel( Label label );

			void resetVocoder ( void );
			void updateSamples( void );		

			void prepareModel( void );
			void checkInterpolationWeights( bool forced=false );
			
			void computeDuration( void );
			void updateDuration ( void );
			void computeParameters ( void );
			void optimizeParameters( void );

			double popSamples ( void );
		
		protected:		
			// --- Queues ---	
			LabelQueue * labelQueue;
			ModelQueue * modelQueue;
			FrameQueue * frameQueue;
		
			// --- HTS Engine ---
			Engine * engine;
		
			// --- Model ---
			Model * model;
		
			// --- SPTK vocoder ---
			Vocoder * vocoder;
		
			// --- Frame ---
			Frame frame;
		
			// --- Label ---
			Label label;
		
			int hopLen;
			int sampleCount;
			double labelSpeed;	// we need this because the speed changes on the lable 
								// level and we need to have memory of this
		
		private:
		
			bool flag;
			int argc;
			char ** argv;
			
			int action;
			int * updateFunction;
		
			// methods
			void init( int argc, char ** argv );
			void parseConfigFile( std::string filename );
	};
} // namespace
