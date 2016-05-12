/// \file outputFactory.h
/// Defines the gemc output Factory .\n
/// It contains the factory method
/// that returns a pointer to goutputs.\n
/// The virtual method processOutput writes events to
/// the correct stream.\n
/// \author \n Maurizio Ungaro
/// \author mail: ungaro@jlab.org\n\n\n
#ifndef OUTPUT_FACTORY_H
#define OUTPUT_FACTORY_H 1

// gemc headers
#include "gbank.h"
#include "options.h"
#include "MPrimaryGeneratorAction.h"

// EVIO headers
#include "evioUtil.hxx"
#include "evioFileChannel.hxx"
using namespace evio;

// geant4 headers
#include "G4ThreeVector.hh"


/// \class hitOutput
/// <b> hitOutput</b>\n\n
/// Contains dynamic output informations
/// - geant4 information, summed over the hit: vector<double> raws
/// - digitized information, from g4 summed: vector<int> dgtz
// This information is relevant to ONE hit only
class hitOutput
{
	private:
		
		// geant4 integrated (over the hit) information.
		// DISABLED by default
		map<string, double> raws;
		
		// digitized information coming from raws
		// ENABLED by default
		// key is variable name
		map<string, double> dgtz;
		
		// geant4 step by step information.
		// DISABLED by default
		map< string, vector <double> > allRaws;
		
		// treated Volt signal as a function of time
		// DISABLED by default
		map< double, double >  signalVT;
	
		// quantized signal as a function of time bunch
		// DISABLED by default
		map< int, int >  quantumS;
	
	
		// multi - digitized information from step by step
		// DISABLED by default
		map< string, vector <int> >  multiDgt;
		
	public:
		
		void setRaws     (map<string, double> r)            {raws = r;}
		void setDgtz     (map<string, double> d)            {dgtz = d;}
		void setAllRaws  (map< string, vector <double> > r) {allRaws  = r;}
		void setSignal   (map< double, double > s)          {signalVT = s;}
		void setQuantumS (map< int, int > s)                {quantumS = s;}
		void setMultiDgt (map< string, vector <int> > d)    {multiDgt = d;}
	
		void setOneRaw    (string s, double d)              {raws[s] = d;}
		void setOneRaw    (string s, int i)                 {raws[s] = (double) i;}

		void setOneDgt    (string s, double d)              {dgtz[s] = d;}
		void setOneDgt    (string s, int i)                 {dgtz[s] = (double) i;}

	
		// may want to insert verbosity here?
		map<string, double>            getRaws()     {return raws;}
		map<string, double>            getDgtz()     {return dgtz;}
		map< string, vector <double> > getAllRaws()  {return allRaws;}
		map< double, double >          getSignalVT() {return signalVT;}
		map< int, int >         		 getQuantumS() {return quantumS;}
		map< string, vector <int> >    getMultiDgt() {return multiDgt;}
	
		double getIntRawVar(string s)
		{
			if(raws.find(s) != raws.end()) return raws[s];
			return -99;
		}
		double getIntDgtVar(string s)
		{
			if(dgtz.find(s) != dgtz.end()) return dgtz[s];
			return -99;
		}
};



/// \class summaryForParticle
/// <b> summaryForParticle </b>\n\n
/// Contains a summary information for each detector, caused
/// by a primary particle and all its descendant
/// - stat (how many hits generated by this track)
/// - total energy
/// - fastest time
/// - nphe
/// - detector name
class summaryForParticle
{
	public:
		summaryForParticle(string detector)
		{
			dname = detector;
			stat  = 0;
			etot  = 0;
			t     = -1;  // initializing to negative for the first assignment
			nphe  = 0;
		}
		~summaryForParticle(){;}
	
		string dname;
		int    stat;
		double etot;
		double t;
		int    nphe;
	
};



/// \class generatedParticle
/// <b> generatedParticle </b>\n\n
/// Contains particle informations.
/// The Primary Particles will be written to the output.\n
/// Secondary Particles will be written to the output if option is specified.\n
class generatedParticle
{
	public:
		generatedParticle()
		{
			pSum.clear();
		}
		~generatedParticle(){;}
		
		G4ThreeVector vertex;
		G4ThreeVector momentum;
		int PID;
		double time;
		int multiplicity;

		// adding summary information for each detector.
		vector<summaryForParticle> pSum;
	
		int    getVariableFromStringI(string);
		double getVariableFromStringD(string);
};



/// \class outputContainer
/// <b> outputContainer </b>\n\n
/// Contains all possible outputs.
class outputContainer
{
	public:
		outputContainer(goptions);
		~outputContainer();
		
		goptions gemcOpt;
		string outType;
		string outFile;
		
		ofstream        *txtoutput;
		evioFileChannel *pchan;
};

/// \class outputFactory
/// <b> outputFactory </b>\n\n
/// outputFactory is registered in a map<string, outputFactory>\n
/// The virtual method processOutput is called at the end of each event.

class outputFactory
{
	public:
		
		// record the simulation conditions on the file
		virtual void recordSimConditions(outputContainer*, map<string, string>) = 0;
		
		// write event header
		virtual void writeHeader(outputContainer*, map<string, double>, gBank)  = 0;
		
		// write generated particles
		virtual void writeGenerated(outputContainer*, vector<generatedParticle>, map<string, gBank> *banksMap) = 0;
		
		// write geant4 true integrated info
		virtual void writeG4RawIntegrated(outputContainer*, vector<hitOutput>, string, map<string, gBank>*) = 0;

		// write geant4 true info for every step
		virtual void writeG4RawAll(outputContainer*, vector<hitOutput>, string, map<string, gBank>*) = 0;
		
		// write geant4 raw integrated info
		virtual void writeG4DgtIntegrated(outputContainer*, vector<hitOutput>, string, map<string, gBank>*) = 0;
	
		// write event and close stream if necessary
		virtual void writeEvent(outputContainer*) = 0;
		
		string outputType;
		
		virtual ~outputFactory(){;}
};

// Define outputFactory as a pointer to a function that returns a pointer to a outputFactory
typedef outputFactory *(*outputFactoryInMap)();

// Instantiates the outputFactory
outputFactory *getOutputFactory(map<string, outputFactoryInMap>*, string);

map<string, outputFactoryInMap> registerOutputFactories();


#endif
