#include "HXc_EmailAnalyser.h"
#include <iostream>
#include <time.h>
#include <random>


using namespace Upp;


void HXc_EmailAnalyser::AnalyserMail(Upp::String email){
	Cout() << "Début de l'Analyse du mail suivante :\n" << email <<"\n-----------------------\n";
//	Cout() << email <<"\n";
	auto resultat0 = Split(email,"|");
		String leVraiEmail ="";
	String niveau ="";
	String sinistre = "";
	String typeApprentissage;
	String object;
	bool apprentissage;
	
	leVraiEmail = resultat0[1];
	Cout() << " _____________________\n";
	//
	niveau = FindDataOnEmailObject("NIVEAU" , resultat0[0]);
	sinistre = FindDataOnEmailObject("SINISTRE" , resultat0[0]);
	typeApprentissage = FindDataOnEmailObject("TYPEEMAIL" , resultat0[0]);
	apprentissage = atoi( FindDataOnEmailObject("APPRENTISSAGE" , resultat0[0]).ToStd().c_str());
	object = FindDataOnEmailObject("OBJECT" , resultat0[0]);
	RegisterMailDb(sinistre,niveau,object);
	if(apprentissage){
		if(StillInApprentissage()){
			Apprend(leVraiEmail,typeApprentissage);
		}
		else
		{
			auto myEmail = Split(email," ");	
			findNeuronnesById(0)->resolve(myEmail);
		}
	}
	else{
		auto myEmail = Split(email," ");
		findNeuronnesById(0)->resolve(myEmail);
	}
	
	Cout() << "Analyse terminée ! " << "\n";
	if(!apprentissage){
		Cout() << "Resultat : " << findNeuronnesById(-1)->getResult();	
	}
}

Upp::String HXc_EmailAnalyser::FindDataOnEmailObject(Upp::String idOfData,Upp::String whereIsTheData){
	int iterator1 = whereIsTheData.Find(idOfData  +":",0);
	if(iterator1 > -1){
		int iterator2 = iterator1;
		if(idOfData.IsEqual("OBJECT"))
			iterator2 = whereIsTheData.GetCount();
		else
			iterator2 = whereIsTheData.Find(" ",iterator1);
		return whereIsTheData.Mid(iterator1  + idOfData.GetCount() +1, iterator2 - (iterator1  + idOfData.GetCount() +1));
	}
	return "";
}


void HXc_EmailAnalyser::Apprend(Upp::String email,Upp::String type){
	auto resultat = Split(email," ");
	bool upload = false;
	SYSTEMTIME timestamp;
	GetLocalTime(&timestamp);
	int test1 =  timestamp.wMilliseconds;
	double test = timestamp.wMilliseconds;
	unMot buffer;
	SQL = sqlite3;
	Sql sql;
	for(int i = 0; i < resultat.GetCount(); i++){
		upload = false;
		buffer = pBdd->GetAWord(resultat[i]);
		if(buffer.orthographe.GetCount() != 0){
			if(buffer.type.Compare("ADJ")==0){
				if(buffer.orthographe.GetCount() >2) upload = true;
				NbAdj++;
			}else if(buffer.type.Compare("VER")==0){
				if(buffer.orthographe.GetCount() >2) upload = true;
				NbVerbe++;
			}else if(buffer.type.Compare("NOM")==0){
				if(buffer.orthographe.GetCount() >2) upload = true;
				NbNom++;
			}else if(buffer.type.Compare("ADV")==0){
				if(buffer.orthographe.GetCount() >2) upload = true;
				NbAdverbe++;
			}else if(buffer.type.Compare("AUX")==0){
				if(buffer.orthographe.GetCount() >2) upload = true;
				NbAux++;
			}
		}			
		if(upload){

		//	Cout() << "Last MAJ : "  << String(std::to_string(test)) << "\n";
			if(type.IsEqual("AR")){
				sql.Execute("Select POUVOIR from AR WHERE MOT ='" << buffer.orthographe <<"';");
				if(sql.Fetch()){	
					sql.Execute("update AR set LASTMAJ=" + String(std::to_string(test)) + ", NBAPPARITION = NBAPPARITION +1, POUVOIR=POUVOIR+0.02  WHERE MOT ='" << buffer.orthographe <<"';");
				}else{
					sql.Execute("Insert into AR(MOT,MTYPE,POUVOIR,LASTMAJ,NBAPPARITION)VALUES('"<< buffer.orthographe <<"','"<< buffer.type <<"',1.0," + String(std::to_string(test)) +",1);");
				}
			}else if(type.IsEqual("RT")){
				sql.Execute("Select POUVOIR from RT WHERE MOT ='" << buffer.orthographe <<"';");
				if(sql.Fetch()){	
					sql.Execute("update RT set LASTMAJ = " + String(std::to_string(test)) + ", NBAPPARITION = NBAPPARITION +1, POUVOIR = POUVOIR + 0.02  WHERE MOT ='" << buffer.orthographe <<"';");
				}else{
					sql.Execute("Insert into RT(MOT,MTYPE,POUVOIR,LASTMAJ,NBAPPARITION)VALUES('"<< buffer.orthographe <<"','"<< buffer.type <<"',1.0," + String(std::to_string(test)) + ",1);");
				}			
			}else if(type.IsEqual("AN")){
				sql.Execute("Select POUVOIR from AN WHERE MOT ='" << buffer.orthographe <<"';");
				if(sql.Fetch()){	
					sql.Execute("update AN set LASTMAJ = " +  String(std::to_string(test)) + ", NBAPPARITION = NBAPPARITION +1, POUVOIR = POUVOIR + 0.02  WHERE MOT ='"  << buffer.orthographe <<"';");
				}else{
					sql.Execute("Insert into AN(MOT,MTYPE,POUVOIR,LASTMAJ,NBAPPARITION)VALUES('"<< buffer.orthographe <<"','"<< buffer.type << "',1.0," +  String(std::to_string(test)) + ",1);");
				}				
			}
		}
	}
	if(type.IsEqual("AR")){
		sql.Execute("update AR set POUVOIR = POUVOIR - 0.01 where LASTMAJ !=" + String(std::to_string(test)) + ";");
	}else if(type.IsEqual("RT")){
		sql.Execute("update RT set POUVOIR = POUVOIR - 0.01 where LASTMAJ !=" +  String(std::to_string(test)) + ";");
	}else if(type.IsEqual("AN")){
		sql.Execute("update AN set POUVOIR = POUVOIR - 0.01 where LASTMAJ !=" +  String(std::to_string(test)) + ";");
	}
}

void HXc_EmailAnalyser::dumpResultat(){
	Cout() << "Il y à dans ce Texte : \n"<< NbVerbe << " verbes, " << NbAdverbe << " adverbes,\n" << NbNom <<" noms et "<< NbAdj << " adjectifs\n";
}

HXc_LanguageDataBase&  HXc_EmailAnalyser::getWordDataBase(){
	return *pBdd;	
}

void HXc_EmailAnalyser::loadBdd(){
	String path = GetExeFilePath();
	path.Replace(GetExeTitle() +".exe","HXc_EmailAnalyser.db");
	String path2 = GetExeFilePath();
	path2.Replace(GetExeTitle() +".exe","HXc_LanguageDataBase.db");
	if(FileExists(path) || FileExists(path2)){
		if (FileExists(path2)) path = path2;
		sqlite3.LogErrors(true);
		if(!sqlite3.Open(path)) {
			LOG("Bdd impossible à ouvrir !");
			exit(0);
		}
		SQL = sqlite3;
		Sql sql;
		sql.Execute("Select ACTIVE from ISLEARNING;");
		if(sql.Fetch()){
			if (sql[0] !=0)
				this->Apprentissage = true;
			else
				this->Apprentissage = false;
		}
		sql.Execute("Select COUNT(ID) from NEURONNES;");
		if(sql.Fetch()){
			if (static_cast <int>( sql[0]) !=0)
				this->isNeuronnesRdy = true;
			else
				this->isNeuronnesRdy = false;
		}
		sql.Execute("Select COUNT(NEURONNES1_ID) from LIENS;");
		if(sql.Fetch()){
			if (static_cast <int>( sql[0]) !=0){
				GetAllNeuronnes();
				this->isNeuronnesLier = true;
			}
			else
				this->isNeuronnesLier = false;
		}
	}
	else{
		initSqlite3();
	}
}

void HXc_EmailAnalyser::NettoyerBDD(){
	SQL = sqlite3;
	Sql sql;
	Vector<String> monVector ={"AR","AN","RT"}; //Ici les noms de tables sont en dur, c'est pas Jojo
	for(String &e : monVector){
		for(String &q :monVector){
			if(&e != &q){
				sql.Execute("DELETE FROM "+ e +" WHERE exists(SELECT MOT FROM "+ q +" WHERE "+ e +".MOT= "+ q +".MOT);");
			}
		}
		sql.Execute("DELETE FROM "+ e +" WHERE POUVOIR < 0.70;");
	}
}

Upp::String HXc_EmailAnalyser::RandomType(){
	SQL =sqlite3;
	Sql sql;	
	sql.Execute("SELECT DISTINCT MTYPE from MOTS where NOT instr(MTYPE,':') >0;");
	Vector<String> vctString;
	while (sql.Fetch()){
		if(sql[0].ToString().GetCount())
			vctString.Add(sql[0].ToString());
	}
	std::uniform_int_distribution<int> distribution(0,vctString.GetCount()-1);
	return vctString[ distribution(generator)]; // generates number in the range 0.. GEt count
}

void HXc_EmailAnalyser::DumpANeuronnes(int id){
	findNeuronnesById(id)->Dump();
}


void HXc_EmailAnalyser::ResetNeuronnes(){
	SQL = sqlite3;
	Sql sql;
	sql.Execute("DELETE from NEURONNES;");
	sql.Execute("DELETE from LIENS;");
	tabNeuronnes.Clear();
	this->isNeuronnesRdy = false;
	this->isNeuronnesLier = false;
}
void HXc_EmailAnalyser::GenererPopulationDeNeuronne(int nbNeuronnes){
	SQL = sqlite3;
	Sql sql;
	sql.Execute("SELECT COUNT(ID) FROM NEURONNES");
	int i= 0;
	if(sql.Fetch()) i = std::atoi(sql[0].ToString().ToStd().c_str()) +1;
	for(int e= i; e < nbNeuronnes+i; e++){
		sql.Execute("INSERT INTO NEURONNES(ID,SPECIALITER_TYPE,POID,ACTIVE)VALUES("+ String(std::to_string(e)) +",'" + RandomType() + "',1.0,1);");
	}
	this->isNeuronnesRdy = true;
}
void HXc_EmailAnalyser::LiasonMulticouche(int nbCouche){
	if(this->isNeuronnesRdy && !this->isNeuronnesLier){
		Vector<int> neuronnesID;
		SQL = sqlite3;
		Sql sql;
		sql.Execute("DELETE FROM NEURONNES WHERE ID < 1");
		sql.Execute("SELECT ID FROM NEURONNES");
		while(sql.Fetch()){
			neuronnesID.Add(static_cast<int>(sql[0]));
		}
		int nbNeuronnes = neuronnesID.GetCount();
		int nbNeuronnesParCouche = nbNeuronnes/(nbCouche==0? 1:nbCouche);
		Vector<Vector<int>> couches;
		for(int n1 = 0; n1 < nbCouche; n1++){
			Vector<int> buffer;
			for(int n2 = 0; n2 < nbNeuronnesParCouche; n2++){
				std::uniform_int_distribution<int> distribution(0,neuronnesID.GetCount()-1);
				int RNG1 =  distribution(generator);
				buffer.Add(neuronnesID[RNG1]);
			}
			couches.Add(buffer);
		}
		
		sql.Execute("INSERT INTO NEURONNES(ID,SPECIALITER_TYPE,POID,ACTIVE)VALUES(0,'" + RandomType() + "',1.0,1);");
		sql.Execute("INSERT INTO NEURONNES(ID,SPECIALITER_TYPE,POID,ACTIVE)VALUES(-1,'" + RandomType() + "',1.0,1);");
		
		Vector<Vector<int>> liens;

		int RNG2;
		for(int n3 = 0; n3 < nbNeuronnesParCouche; n3++){
			Vector<int> buffer2;
			for(Vector<int> &uneCouche : couches){
				if(uneCouche.GetCount()-1 >0){
					std::uniform_int_distribution<int> distribution(0,uneCouche.GetCount()-1);
					RNG2 = 	 distribution(generator);
					buffer2.Add( uneCouche[RNG2]);
					uneCouche.Remove(RNG2);
				}
				else{
					buffer2.Add( uneCouche[uneCouche.GetCount()-1]);
					uneCouche.Remove(uneCouche.GetCount()-1);	
				}
			}
			liens.Add(buffer2);
		}
		
		for(Vector<int> &lesLiens : liens){
			lesLiens.Insert(0,0);
			lesLiens.Add(-1);
		}
		
		for(Vector<int> &lesLiens : liens){
			for(int n3 = 0; n3 < lesLiens.GetCount(); n3++){
				if(( n3 +1) < lesLiens.GetCount()){
					sql.Execute("INSERT INTO LIENS(NEURONNES1_ID,NEURONNES2_ID)VALUES(" + String(std::to_string(lesLiens[n3])) +","+ String(std::to_string(lesLiens[n3+1])) +" );");	
				}
			}
		}
		this->GetAllNeuronnes();
		this->isNeuronnesLier = true;	
	}
	else
		Cout() << "Aucunes neuronnes à lier !\n";
}

void HXc_EmailAnalyser::GetAllNeuronnes(){
	SQL = sqlite3;
	Sql sql;
	sql.Execute("Select ID,POID,SPECIALITER_TYPE from Neuronnes");
	while(sql.Fetch()){
		tabNeuronnes.Add(Neuronnes(static_cast<int>(sql[0]),sql[2].ToString(), std::stof(sql[1].ToString().ToStd()),this ));
	}
	sql.Execute("Select * from LIENS");
	while(sql.Fetch()){
		if(findNeuronnesById(static_cast<int>( sql[0])) !=NULL ) findNeuronnesById(static_cast<int>( sql[0]))->defineNext(*findNeuronnesById(static_cast<int>( sql[1])));
	}
}

Neuronnes* HXc_EmailAnalyser::findNeuronnesById(int id){
	for(Neuronnes &e : tabNeuronnes){
		if(e.getId() == id){
			return &e;	
		}
	}
	return NULL;
}

void HXc_EmailAnalyser::EvolutionDuReseau(){

	
}


Upp::VectorMap<Upp::String,float> HXc_EmailAnalyser::GetWordScore(Upp::String myWord){
	SQL = sqlite3;
	Sql sql;
	Upp::String type ="";
	float pouvoir = 0.0;
	Vector<String> monVector ={"AR","AN","RT"}; //Ici les noms de tables sont en dur, c'est pas Jojo
	for(String &e : monVector){
		sql.Execute("Select POUVOIR from " + e + " where ORTHOGRAPHE = '" +  myWord + "' COLLATE NOCASE");
		if(sql.Fetch()){
			type = e;	
			pouvoir = std::stof(sql[2].ToString().ToStd());
		}
	}
	VectorMap<Upp::String,float> returnValue;
	returnValue.Add(type,pouvoir);
	return returnValue;
	
}

void HXc_EmailAnalyser::GenererNeuronnes(int nbNeuronnes){
	this->GenererPopulationDeNeuronne(nbNeuronnes);
}

void HXc_EmailAnalyser::initSqlite3(){
		String path = GetExeFilePath();
		path.Replace(GetExeTitle() +".exe","HXc_LanguageDataBase.db");
		sqlite3.LogErrors(true);
		if(!sqlite3.Open(path)) {
			LOG("Bdd impossible à ouvrir !");
			exit(0);
		}
		
		SQL = sqlite3;
		SqlSchema sch1(SQLITE3);
		All_Tables2(sch1);

		SqlPerformScript(sch1.Upgrade());
		SqlPerformScript(sch1.Attributes());
		SqlPerformScript(sch1.ConfigDrop());
		SqlPerformScript(sch1.Config());
		
		sch1.SaveNormal();
		
		SQL = sqlite3;
		Sql sql;
		sql.Execute("INSERT INTO ISLEARNING(ACTIVE)VALUES(0);");
		Apprentissage = true;
		isNeuronnesRdy = false;
		isNeuronnesLier = false;
}

void HXc_EmailAnalyser::RegisterMailDb(String sinistre,String niveau,String object){
	SQL = sqlite3;
	Sql sql;
	String theRequete;
	//	Sql insert("insert into MOTS(ORTHOGRAPHE,LEMME,MTYPE,ANOMBRE,AGENRE) values(?,?,?,?,?)",sqlite3);
	theRequete << "insert into EMAIL(SINISTRE,NIVEAU,OBJECT,DATER) values('"<< sinistre <<"','" << niveau << "','" << object << "','" << String(currentDateTime()) <<"');";
	sql.Execute(theRequete);	
}

std::string HXc_EmailAnalyser::currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

HXc_EmailAnalyser::HXc_EmailAnalyser(){
	HXc_LanguageDataBase bdd;	
	pBdd = &bdd;
	loadBdd();
}

HXc_EmailAnalyser::HXc_EmailAnalyser( HXc_LanguageDataBase &bddToUse){
	pBdd = &bddToUse;
	this->loadBdd();
}

void HXc_EmailAnalyser::stopApprentissage(){
	SQL = sqlite3;
	Sql sql;
	sql.Execute("UPDATE ISLEARNING set ACTIVE =1;");
	this->Apprentissage = false;
}
void HXc_EmailAnalyser::startApprentissage(){
	SQL = sqlite3;
	Sql sql;
	sql.Execute("UPDATE ISLEARNING set ACTIVE =0;");
	this->Apprentissage = true;
}

bool HXc_EmailAnalyser::StillInApprentissage(){
	return this->Apprentissage;	
}


int main(int argc, const char *argv[]){
	HXc_LanguageDataBase bdd;
	TcpSocket socket1;
	Cout() << "Démarage du serveur d'écoute sur le port 2706"<<"\n";
	socket1.Listen(2706);
	Cout() << "Serveur démarré.";
	for(;;){
		Cout() << " En attente de data..."<<"\n";
		TcpSocket data;
		if(data.Accept(socket1)){
			String w;
			Cout() << "Reception de data..."<<"\n";
			while(data.Term() != -1){
			    w = data.GetLine(); 
			}
			String mail = w;
			LPINT result;
		//	IsTextUnicode(&mail,mail.GetCount(),result);
			mail = ToCharset(CHARSET_CP850,mail,CHARSET_ISO8859_16); 
			mail =  ToCharset(CHARSET_ISO8859_16,mail,CHARSET_DEFAULT);
			mail =  ToCharset(CHARSET_DEFAULT,mail,CHARSET_CP850);
			
		//	Cout() << mail <<"\n";

			HXc_EmailAnalyser analyser(bdd);
			//Cout() << "Request: " << mail << " from: " << data.GetPeerAddr() << '\n';
			data.Put("\n");
			
			if(mail.Find("STOPLEARNINGNOW") > -1 ){
				analyser.stopApprentissage();
				Cout() << "Compris, j'arrete d'apprendre"<<"\n";
			}
			else if(mail.Find("STARTLEARNINGNOW") > -1 ){
				analyser.startApprentissage();
				Cout() << "Compris,j'apprend désormais"<<"\n";
			}
			else if(mail.Find("WASHITNOW") > -1){
				analyser.NettoyerBDD();
				Cout() << "Compris, Je nettoie la BDD"<<"\n";
			}
			else if(mail.Find("GENERERNEURONNES") > -1){
				int nbNeuronnes = std::stoi( mail.Mid((mail.Find("NEURONNES") +10), mail.GetCount() - (mail.Find("NEURONNES") +10)).ToStd());

				analyser.GenererNeuronnes(nbNeuronnes);
				Cout() << nbNeuronnes << " Neuronnes générées !"<<"\n";
			}
			else if(mail.Find("RESETNEURONNES") > -1){
				analyser.ResetNeuronnes();
				Cout() << "Neuronnes Renitialisées !" <<"\n";	
			}
			else if(mail.Find("LIERNEURONNES") > -1){
				int nbCouches = std::stoi( mail.Mid((mail.Find("NEURONNES") +10), mail.GetCount() - (mail.Find("NEURONNES") +10)).ToStd());
				analyser.LiasonMulticouche(nbCouches);
				Cout() <<"Réseau de neuronnes " << nbCouches << " couches créer !\n";
			}
			else if(mail.Find("DUMPNEURONNES") > -1){
				int id = std::stoi( mail.Mid((mail.Find("NEURONNES") +10), mail.GetCount() - (mail.Find("NEURONNES") +10)).ToStd());
				analyser.DumpANeuronnes(id);
			}
			else{
				analyser.AnalyserMail(mail);
				analyser.dumpResultat();
			}
			
			//analyser.insertTestDb();
			
			Cout() << "-----------------"<<"\n";
		}
	}
	
	/*
	String pathParameter("");
	if(argc > 1) {
	 	pathParameter = argv[1];
	}
	HXc_LanguageDataBase bdd(pathParameter);
	std::string theWord;
	for(;;){
		Cout() << "Entrer le mot à rechercher (les accents compte !) :\n";
		std::cin >> theWord;
		theWord = ToCharset(CHARSET_DEFAULT,theWord,CHARSET_CP850); //CETTE MERDE DENCODAGE MA PRIT 3 JOURS 
																	//la console française 
																	//windows 7 est de base à
																	//l'encodage ISO_8859_CP850
																	//ce qui représente
																	//l'encodage Latin 1.
																	//C++ récupère la string et
																	//la caste en UTF-8 (de ce
																	//que j'en ai compris) ce
																	//qui faussais les données
																	//envoyées Il fallait donci
																	//après récupération dans
																	//la variable convertire la
																	//data de la variable de
																	//l'encodage par défaut
																	//(Utf 8) vers l'encodage
																	//Windows soit
																	//ISO_8859_CP850
		bdd.GetAWord(Upp::String(theWord)).Dump();
	}
	*/
	
}





Neuronnes::Neuronnes(Neuronnes &nextNeuronne,int id,Upp::String spe,float poid,HXc_EmailAnalyser* emailanalyser){
	this->ID =id;
	this->nextNeuronnes.Add(&nextNeuronne);
	this->poid = poid;
	this->Type_spe = spe;
	
	resultats.Add("AR",0.0);
	resultats.Add("RT",0.0);
	resultats.Add("AN",0.0);
}

Neuronnes::Neuronnes(int id,Upp::String spe,float poid,HXc_EmailAnalyser* emailanalyser){
	this->ID =id;
	this->poid = poid;
	this->Type_spe = spe;
	
	resultats.Add("AR",0.0);
	resultats.Add("RT",0.0);
	resultats.Add("AN",0.0);
}

void Neuronnes::defineNext(Neuronnes &nextNeuronne){
	this->nextNeuronnes.Add( &nextNeuronne);
}

void Neuronnes::resolve( Vector<String> &email){
	if(ID==0){
		this->calculate(resultats,email);	
	}
	else
		Cout() <<"Cet neuronne n'est pas la neuronne 0 !"<< "\n";
}

int Neuronnes::getId(){
	return ID;
}

void Neuronnes::Dump(){
	Cout() << "Id de neuronnes : " << ID <<"\nLier avec la neuronnes : ";
	for(Neuronnes* e : nextNeuronnes){
		Cout() << e->getId() << "\n";
	}
	Cout() <<"\nSpecialisation : " << Type_spe <<"\n de poid : " << poid <<"\n";
	Cout() <<"Actuellement sont Resultat est le suivant : " << getResult();
}

void Neuronnes::calculate(VectorMap<Upp::String,float> result,Vector<String> &email){
	if (ID == 0){
		for(Neuronnes* next : nextNeuronnes){
			next->calculate(resultats.,email);
		}
	}else if(ID == -1){
		for(int i = 0; i< resultats.GetCount();i++){
			for(int e = 0; e < result.GetCount(); e++){
				if(result.GetKey(e).IsEqual(resultats.GetKey(i))){
					resultats.Get(resultats.GetKey(i)) += result.Get(result.GetKey(e));
					break;
				}
			}
		}
	}else{
		resultats = result;
		unMot buffer;
		SQL = sqlite3;
		Sql sql;	
		for(int i = 0; i < email.GetCount(); i++){
			buffer = an->getWordDataBase().GetAWord(email[i]);
			if(buffer.orthographe.GetCount() != 0){
				auto buff =	an->GetWordScore(buffer.orthographe);
				for(int i = 0; i< resultats.GetCount();i++){
					if(resultats.GetKey(i).IsEqual(buff.GetKey(0))){
						if( buffer.type.IsEqual(this->Type_spe))
							resultats.Get(resultats.GetKey(i)) += (buff.Get(buff.GetKey(0)) + (buff.Get(buff.GetKey(0))*0.5)) ;
						else
							resultats.Get(resultats.GetKey(i)) += buff.Get(buff.GetKey(0));
						
						break;
					}
				}
			}			
		}
		for(int i = 0; i< resultats.GetCount();i++){
			resultats.Get(resultats.GetKey(i)) = resultats.Get(resultats.GetKey(i))*this->poid;
		}
		for(Neuronnes* next : nextNeuronnes){
			next->calculate(resultats,email);
		}
	}
}

Upp::String Neuronnes::getResult(){
	String type = "";
	float score = 0.0;
	for(int i = 0; i< resultats.GetCount();i++){
		if (resultats.Get(resultats.GetKey(i)) > score){
			score = resultats.Get(resultats.GetKey(i));
			type = resultats.GetKey(i);
		}
	}
	return type;
}

void Neuronnes::Valoriser(){
	return;
}

void Neuronnes::Devaloriser(){
	return;
}
