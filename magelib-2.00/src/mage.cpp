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
 /* 		Maria Astrinaki, Alexis Moinet, Geoffrey Wilfart, Nicolas d'Alessandro, Thierry Dutoit	*/
 /* 																								*/
 /* ----------------------------------------------------------------------------------------------- */

#include "mage.h"

// constructor
MAGE::Mage::Mage( void )
{
	this->init();
}

MAGE::Mage::Mage( std::string EngineName, std::string confFilename )
{		
	this->init();
	this->addEngine( EngineName, confFilename );
}

MAGE::Mage::Mage( std::string EngineName, int argc, char ** argv )
{	
	this->init();
	this->addEngine( EngineName, argc, argv );
}

// destructor
MAGE::Mage::~Mage( void )
{
	this->flagReady = false;

	// --- Queues ---
	delete this->labelQueue;
	delete this->modelQueue;
	delete this->frameQueue;
		
	// --- SPTK Vocoder ---
	delete this->vocoder;

	//free memory for all Engine allocated by addEngine
	map < std::string, std::pair < double * , Engine * > >::const_iterator it;

	for( it = this->engine.begin(); it != this->engine.end(); it++ )
	{
		delete[] ( * it ).second.first;
		delete ( * it ).second.second;
	}
}

// getters
double MAGE::Mage::getPitch ( void )
{
	return( this->vocoder->getPitch() );
}

double MAGE::Mage::getAlpha ( void )
{
	return( this->vocoder->getAlpha() );
}

double MAGE::Mage::getGamma ( void )
{
	return( this->vocoder->getGamma() );
}

double MAGE::Mage::getPOrder ( void )
{
	return( this->vocoder->getPadeOrder() );
}

double MAGE::Mage::getVolume( void )
{
	return( this->vocoder->getVolume() );
}

double MAGE::Mage::getDuration( void )
{
	return( this->model->getDuration() );
}

// setters
void MAGE::Mage::setPitch ( double pitch, int action )
{	
	this->vocoder->setPitch( pitch, action );
	return;
}

void MAGE::Mage::setSpeed ( double speed, int action )
{
	switch( action )
	{
		case MAGE::overwrite:
			this->hopLen = speed;
			break;
			
		case MAGE::shift:
			this->hopLen = ( this->hopLen ) + ( speed ); 
			break;
			
		case MAGE::scale:
			this->hopLen = ( this->hopLen ) * ( speed ); 
			break;
			
		case MAGE::synthetic:
		case MAGE::noaction:
		default:
			this->hopLen = defaultFrameRate;
	}
	
	if( hopLen < 1 )
		hopLen = 1;

	if( hopLen > defaultFrameRate * 20 )
		hopLen = defaultFrameRate * 20;
	
	return;
}

void MAGE::Mage::setAlpha ( double alpha )
{
	this->vocoder->setAlpha( alpha );
	return;
}

void MAGE::Mage::setGamma ( double gamma )
{
	this->vocoder->setGamma( gamma );
	return;
}

void MAGE::Mage::setPOrder ( double pOrder )
{
	this->vocoder->setPadeOrder( pOrder );
	return;
}

void MAGE::Mage::setVolume( double volume )
{
	this->vocoder->setVolume( volume );
	return;
}

void MAGE::Mage::setDuration( double * updateFunction, int action )
{
	this->action = action;
	
	for( int i = 0; i < nOfStates; i++ )
		this->updateFunction[i] = updateFunction[i];
	
	return;
}

void MAGE::Mage::setDefaultEngine( std::string adefaultEngine )
{
	map < std::string, std::pair < double * , Engine * > >::const_iterator it;

	it = this->engine.find( adefaultEngine );
	
	if( it != this->engine.end() )
		this->defaultEngine = adefaultEngine;
	
	return;
}

// methods
void MAGE::Mage::parseConfigFile( std::string confFilename )
{
	int k = 0;
	string line, s;
	ifstream confFile( confFilename.c_str() );
	
	if( !confFile.is_open() )
	{
		printf( "could not open file %s\n",confFilename.c_str() );
		return;
	}
	
	// configuration arguments
	this->argv	= ( char ** ) calloc( maxNumOfArguments,sizeof( char * ) );
	
	while( getline( confFile, line ) )
	{
		istringstream iss( line );
		while( getline( iss, s, ' ' ) )
		{
			if( s.c_str()[0] != '\0')
			{
				this->argv[k] = new char[maxStrLen];  // ATTENTION :: FREE!!! DISALLOCATE!!!
				strcpy(this->argv[k], s.c_str() ); 
				k++;
			}
		}
	}
	confFile.close();

	this->argc = k;

	return;
}

void MAGE::Mage::init( void )
{	
	// --- Queues ---	
	this->labelQueue = new MAGE::LabelQueue( maxLabelQueueLen );
	this->modelQueue = new MAGE::ModelQueue( maxModelQueueLen );
	this->frameQueue = new MAGE::FrameQueue( maxFrameQueueLen );
	
	// --- SPTK Vocoder ---
	this->vocoder = new MAGE::Vocoder();
	
	// --- Label ---
	this->labelQueue->get( this->label );
	
	this->flag = true;
	this->flagReady = false;
	this->labelSpeed = 1;
	this->sampleCount = 0;
	
	this->action = noaction;
	this->defaultEngine = "";
	this->hopLen = defaultFrameRate;
	this->interpolationFlag = false;
 
	return;
}

void MAGE::Mage::run( void )
{
	if( this->popLabel() )
	{
		this->prepareModel();
		this->computeDuration();
		this->computeParameters ();
		this->optimizeParameters();
	}		
	return;
}

void MAGE::Mage::pushLabel( Label label )
{
	if( !this->labelQueue->isFull() )
		this->labelQueue->push( label );
	else 
		printf( "label queue is full !\n%s", label.getQuery().c_str() );
	
	return;
}

bool MAGE::Mage::popLabel( void )
{
	if( !this->labelQueue->isEmpty() )
	{
		this->labelQueue->pop( this->label );
		this->label.setSpeed ( this->labelSpeed );
		return( true );
	}
	else
		usleep( 100 );
	
	return( false );
}

void MAGE::Mage::prepareModel( void )
{
	this->model = this->modelQueue->next();
	this->model->checkInterpolationWeights( this->engine[this->defaultEngine].second );
	return;
}

void MAGE::Mage::checkInterpolationWeights( bool forced )
{
	this->model->checkInterpolationWeights( this->engine[this->defaultEngine].second, forced );
	return;
}

void MAGE::Mage::computeDuration( void )
{
	map < std::string, std::pair < double * , Engine * > >::const_iterator it;

	this->model->initDuration();
	
	if( !this->interpolationFlag )		
		this->model->computeDuration( this->engine[this->defaultEngine].second, &(this->label), NULL );
	else
		for( it = this->engine.begin(); it != this->engine.end(); it++ )			
			this->model->computeDuration( ( * it ).second.second, &(this->label), ( * it ).second.first );
	
	this->model->updateDuration( this->updateFunction, this->action ); 
	this->action = noaction;
	
	return;
}

void MAGE::Mage::computeParameters( void )
{
	Engine * currentEngine;
	double * interpolationFunction;
	map < std::string, std::pair < double * , Engine * > >::const_iterator it;

	this->model->initParameters();

	if( !this->interpolationFlag )
		this->model->computeParameters( this->engine[this->defaultEngine].second, &(this->label), NULL );
	else 
		for( it = this->engine.begin(); it != this->engine.end(); it++ )
			this->model->computeParameters( ( * it ).second.second, &(this->label), ( * it ).second.first );
		
	this->model->computeGlobalVariances( this->engine[this->defaultEngine].second, &(this->label) );
	this->modelQueue->push( );

	return;
}

void MAGE::Mage::optimizeParameters( void )
{
	if( this->modelQueue->getNumOfItems() > nOfLookup + nOfBackup )
	{
		this->flag = false;
		this->modelQueue->optimizeParameters( this->engine[this->defaultEngine].second, nOfBackup, nOfLookup );
		this->modelQueue->generate( this->frameQueue, nOfBackup );				
		this->modelQueue->pop();
	} 
	else if( this->modelQueue->getNumOfItems() > nOfLookup && this->flag )
	{
		this->modelQueue->optimizeParameters( this->engine[this->defaultEngine].second, 
											  this->modelQueue->getNumOfItems() - nOfLookup - 1, nOfLookup );
		this->modelQueue->generate( this->frameQueue, this->modelQueue->getNumOfItems() - nOfLookup - 1 );	
	}	
	return; 
}

void MAGE::Mage::resetVocoder( void )
{
	this->vocoder->reset();
	return;
}

void MAGE::Mage::reset( void )
{	
	map < std::string, std::pair < double * , Engine * > >::iterator it; // iterator for the map of engines

	this->flag = true;
	this->labelSpeed = 1;
	this->sampleCount = 0;
	this->action = noaction;
	this->hopLen = defaultFrameRate;
	this->interpolationFlag = false;
	
	this->resetVocoder();
	
	for( it = this->engine.begin(); it != this->engine.end(); it++ )
		for( int i = 0; i < nOfStreams + 1; i++ )
			if( this->interpolationWeights[i] )
				( * it ).second.first[i] = 1;	
	return;
}

void MAGE::Mage::updateSamples( void )
{
	if( this->sampleCount >= this->hopLen-1 ) // if we hit the hop length
	{	
		if( !this->frameQueue->isEmpty() )
		{			
			this->vocoder->push( this->frameQueue->get() );
			this->frameQueue->pop();
		}
		this->sampleCount = 0; // and reset the sample count for next time
	} 
	else 
		this->sampleCount++; // otherwise increment sample count
	
 	return;
}

void MAGE::Mage::addEngine( std::string EngineName )
{//this function is not thread safe (exactly like the MemQueue) it should never be called simultaneously in different threads
	map < std::string, std::pair < double * , Engine * > >::iterator it;
	std::pair < double * , Engine * > tmpEngine;

	// check that the Engine doesn't exist already
	it = this->engine.find( EngineName );

	// then we need to decide what to do:
	//  - nothing, directly return
	//  - overwrite the existing Engine with a new one --> DONE
	// we could try to modify "load()" so that it can be called a second time
	// on existing Engine instead of doing a 'delete' and then a 'new'
	// (or write a "reload()" function ?)
	if( it != this->engine.end() )
	{
		// get a copy of the std::pair containing adresses of pointers
		tmpEngine = ( * it ).second;
		// this should be fast, but if computeDuration/Parameters/checkInterp...
		// use this->engine at the same time we might have a problem, if it happens,
		// a lock will be necessary around this erase (! we're out of audio thread)
		this->engine.erase( it );
		//free existing engine
		delete[] tmpEngine.first;	// free interpolation weight vector (double *)
		delete tmpEngine.second;	// free engine (call to ~Engine)
	}

	tmpEngine.first = new double[nOfStreams + 1];
	
	for( int i = 0; i < nOfStreams + 1; i++ )
		tmpEngine.first[i] = defaultInterpolationWeight;

	// we load outside of this->engine so it shouldn't crash other threads accessing it.
	tmpEngine.second = new MAGE::Engine();
	tmpEngine.second->load( this->argc, this->argv);
	
	// this should be fast, but if computeDuration/Parameters/checkInterp...
	// use this->engine at the same time we might have a problem, if it happens,
	// a lock will be necessary around this insert (! we're out of audio thread)
	this->engine[EngineName] = tmpEngine;
	
	if( this->defaultEngine.empty() )
	{
		this->defaultEngine = EngineName;
		this->flagReady = true;
		printf("default Engine is %s\n",this->defaultEngine.c_str());
	}

 	return;
}

void MAGE::Mage::addEngine( std::string EngineName, int argc, char ** argv )
{
	this->argc = argc;
	this->argv = argv;

	this->addEngine( EngineName );
	
	return;
}

void MAGE::Mage::addEngine( std::string EngineName, std::string confFilename )
{
	this->parseConfigFile( confFilename );

	this->addEngine( EngineName );

 	return;
}

void MAGE::Mage::removeEngine( std::string EngineName )
{//this function is not thread safe (exactly like the MemQueue) it should never be called simultaneously in different threads
	map < std::string, std::pair < double * , Engine * > >::iterator it;
	std::pair < double * , Engine * > tmpEngine;

	it = this->engine.find( EngineName );

	if( it != this->engine.end() )
	{
		tmpEngine = ( * it ).second;
		// this should be fast, but if computeDuration/Parameters/checkInterp...
		// use this->engine at the same time we might have a problem, if it happens,
		// a lock will be necessary around this erase (! we're out of audio thread)
		this->engine.erase( it );		//remove from std::map (fast ?)
		
		//this shouldn't impact on other threads
		delete[] tmpEngine.first;	//free double*
		delete tmpEngine.second;	//free memory by calling ~Engine
		
		// TODO :: add checks for this->engine.empty() in other part of code ?
		if( this->engine.empty() )
		{
			printf("ATTENTION: Mage::removeEngine(): no Engine remaining, defaultEngine is now undefined (was %s)\n",EngineName.c_str());
			this->defaultEngine = "";
			this->flagReady = false;
		}
		else
		{
			if( !this->defaultEngine.compare(EngineName) )
			{	//we removed the default Engine, better switch to another one
				it = this->engine.begin();
				this->defaultEngine = ( * it ).first;
				this->flagReady = true;
			}
		}
	}

	return;
}

double MAGE::Mage::popSamples ( void )
{
	double sample; 
	
	if( this->vocoder->ready() )
	{
		sample = 0.5 * this->vocoder->pop() / 32768;
		
		if( sample > 1.0 )
			return( 1.0 );
	
		if( sample < -1.0 )
			return( -1.0 );
		
		return( sample );			
	} 
	return( 0 );
}

void MAGE::Mage::setInterpolationFunctions( std::map < std::string, double * > interpolationFunctionsSet )
{
	string EngineName;
	double * itInterpolationFunction;
	
	map < std::string, double * >::iterator it; // iterator for the map of interpolationFunctions
	map < std::string, std::pair < double * , Engine * > >::iterator itEngine; // iterator for the map of engines
	
	for( it = interpolationFunctionsSet.begin(); it != interpolationFunctionsSet.end(); it++ ) // for all the interpolation functions
	{
		EngineName = ( * it ).first;
		itInterpolationFunction = ( * it ).second;
				
		itEngine = this->engine.find( EngineName );
		
		for( int i = 0; i < nOfStreams + 1; i++ )
			( * itEngine ).second.first[i] = itInterpolationFunction[i];// itInterpolationFunction;
	}
	
	this->checkInterpolationFunctions();
		
	return;
}

void MAGE::Mage::checkInterpolationFunctions( void )
{
	int i;
	map < std::string, std::pair < double * , Engine * > >::iterator it; // iterator for the map of engines

	for( i = 0; i < nOfStreams + 1; i++ )
		this->interpolationWeights[i] = 0;
	
	for( it = this->engine.begin(); it != this->engine.end(); it++ )
		for( i = 0; i < nOfStreams + 1; i++ )
			this->interpolationWeights[i] += abs( ( * it ).second.first[i] );
	
	for( it = this->engine.begin(); it != this->engine.end(); it++ )
		for( i = 0; i < nOfStreams + 1; i++ )
			if( this->interpolationWeights[i] )
				( * it ).second.first[i] /= this->interpolationWeights[i];
	
	return;
}

void MAGE::Mage::printInterpolationWeights( void )
{
	map < std::string, std::pair < double * , Engine * > >::iterator itEngine; // iterator for the map of engines

	for( itEngine = this->engine.begin(); itEngine != this->engine.end(); itEngine++ )
		for( int i = 0; i < 4; i++ )
			printf("weights %s %f\n", ( * itEngine ).first.c_str(), ( * itEngine ).second.first[i]);
}

bool MAGE::Mage::ready( void )
{
	return this->flagReady;
}

void MAGE::Mage::checkReady( void )
{
	// CAUTION if this function becomes too heavy, it will cause problems if it is
	// called in an infinite loop such as the one in genThread.cpp (cf. OFx app)
	// TODO since most of it won't change, it should be changed for a cached version ?
	// IOW I'm not happy with this code, it's suboptimal as hell

	// I am sure we can fix it in a way that you would be proud of it :: when do we have the next coding meeting?
	// Oh I am leeting messages in the code!! And I am sure you will read it geek!!!
	
	this->flagReady = false;
	
	if( this->engine.empty() )
		return;
	
	if( this->defaultEngine.empty() )
		return;	

	//add any other meaningful condition here

	this->flagReady = true;
	
	return;
}

std::string MAGE::Mage::timestamp( void )
{
	string d( __DATE__ );
	string t( __TIME__ );
	
	return( d + " at " + t );
}
