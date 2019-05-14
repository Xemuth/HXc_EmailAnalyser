#ifndef _HXc_EmailAnalyser_HXc_EmailAnalyser_h_
#define _HXc_EmailAnalyser_HXc_EmailAnalyser_h_

#include <HXc_LanguageDataBase/HXc_LanguageDataBase.h>
#undef MODEL 
#define MODEL <HXc_EmailAnalyser/HXc_EmailAnalyser.sch>
#define SCHEMADIALECT <plugin/sqlite3/Sqlite3Schema.h>
#include "HXc_SchemaDefinition.h"

#include <random>
#include <Core/Core.h>
class Neuronnes;

class HXc_EmailAnalyser{
	private:
		int NbVerbe = 0;
		int NbNom = 0;
		int NbAdverbe = 0;
		int NbAux = 0;
		int NbAdj = 0;
		
		bool Apprentissage =false;
		bool isNeuronnesRdy = false;
		bool isNeuronnesLier = false;
	
		HXc_LanguageDataBase *pBdd = NULL;
		Sqlite3Session sqlite3;
		void initSqlite3();
		void loadBdd();
		Upp::String RandomType();
		
		std::default_random_engine generator;
		
		Vector<Neuronnes> tabNeuronnes;
		void GetAllNeuronnes();
		Neuronnes* findNeuronnesById(int id);
		
	public:
		HXc_EmailAnalyser();
		HXc_EmailAnalyser(HXc_LanguageDataBase &bddToUse);
		bool StillInApprentissage();
		
		HXc_LanguageDataBase& getWordDataBase();
		
		void ResetNeuronnes();
		void GenererPopulationDeNeuronne(int nbNeuronnes);
		void LiasonMulticouche(int nbCouche);
		void EvolutionDuReseau();
		
		//Method used with server receveir
		void stopApprentissage();	
		void startApprentissage();
		void NettoyerBDD();
		void GenererNeuronnes(int nbNeuronnes); 
		//**********************************
		
		void Apprend(Upp::String email,Upp::String type);
		
		Upp::String Resolve();
		void Affiner(Upp::String type);
		
		void AnalyserMail(Upp::String email);
		void dumpResultat();
		void DumpANeuronnes(int id);
		Upp::VectorMap<Upp::String,float> GetWordScore(Upp::String myWord);
		
		std::string currentDateTime();
		void RegisterMailDb(String sinistre,String niveau,String object);
		Upp::String FindDataOnEmailObject(Upp::String idOfData,Upp::String whereIsTheData);
};

class Neuronnes : public Upp::Moveable<Neuronnes> {
	private:
		int ID;
		Upp::String Type_spe;
		float poid;
		
		HXc_EmailAnalyser* an;
		
		Vector<Neuronnes*> nextNeuronnes;
		VectorMap<Upp::String,float> resultats;
		
	public:
		Neuronnes(Neuronnes &nextNeuronne,int id,Upp::String spe,float poid,HXc_EmailAnalyser* emailanalyser);
		Neuronnes(int id,Upp::String spe,float poid,HXc_EmailAnalyser* emailanalyser);
		void calculate(VectorMap<Upp::String,float> resultats,Vector<String> &email);	
		Upp::String getResult();
		
		int getId();
		
		void defineNext(Neuronnes &nextNeuronne);
		
		void Dump();
		
		void resolve( Vector<String> &email);
		
		void Valoriser();
		void Devaloriser();	
};
#endif
