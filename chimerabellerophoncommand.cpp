/*
 *  chimerabellerophoncommand.cpp
 *  Mothur
 *
 *  Created by westcott on 4/1/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "chimerabellerophoncommand.h"
#include "bellerophon.h"

//***************************************************************************************************************

ChimeraBellerophonCommand::ChimeraBellerophonCommand(string option)  {
	try {
		abort = false;
		
		//allow user to run help
		if(option == "help") { help(); abort = true; }
		
		else {
			//valid paramters for this command
			string Array[] =  {"fasta","filter","correction","processors","window","increment","outputdir","inputdir"};
			vector<string> myArray (Array, Array+(sizeof(Array)/sizeof(string)));
			
			OptionParser parser(option);
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			map<string,string>::iterator it;
			
			//check to make sure all parameters are valid for command
			for (it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			
			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);		
			if (inputDir == "not found"){	inputDir = "";		}
			
			fastafile = validParameter.validFile(parameters, "fasta", false);
			if (fastafile == "not found") { fastafile = ""; m->mothurOut("fasta is a required parameter for the chimera.bellerophon command."); m->mothurOutEndLine(); abort = true;  }
			else { 
				splitAtDash(fastafile, fastaFileNames);
				
				//go through files and make sure they are good, if not, then disregard them
				for (int i = 0; i < fastaFileNames.size(); i++) {
					if (inputDir != "") {
						string path = hasPath(fastaFileNames[i]);
						//if the user has not given a path then, add inputdir. else leave path alone.
						if (path == "") {	fastaFileNames[i] = inputDir + fastaFileNames[i];		}
					}
	
					int ableToOpen;
					ifstream in;
					
					#ifdef USE_MPI	
						int pid;
						MPI_Comm_size(MPI_COMM_WORLD, &processors); //set processors to the number of mpi processes running
						MPI_Comm_rank(MPI_COMM_WORLD, &pid); //find out who we are
				
						if (pid == 0) {
					#endif

					ableToOpen = openInputFile(fastaFileNames[i], in);
					in.close();
					
					#ifdef USE_MPI	
							for (int j = 1; j < processors; j++) {
								MPI_Send(&ableToOpen, 1, MPI_INT, j, 2001, MPI_COMM_WORLD); 
							}
						}else{
							MPI_Status status;
							MPI_Recv(&ableToOpen, 1, MPI_INT, 0, 2001, MPI_COMM_WORLD, &status);
						}
						
					#endif

					if (ableToOpen == 1) { 
						m->mothurOut(fastaFileNames[i] + " will be disregarded."); m->mothurOutEndLine(); 
						//erase from file list
						fastaFileNames.erase(fastaFileNames.begin()+i);
						i--;
					}
					
				}
				
				//make sure there is at least one valid file left
				if (fastaFileNames.size() == 0) { m->mothurOut("no valid files."); m->mothurOutEndLine(); abort = true; }
			}
			
			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found"){	
				outputDir = "";	
				outputDir += hasPath(fastafile); //if user entered a file with a path then preserve it	
			}

			string temp;
			temp = validParameter.validFile(parameters, "filter", false);			if (temp == "not found") { temp = "F"; }
			filter = isTrue(temp);
			
			temp = validParameter.validFile(parameters, "correction", false);		if (temp == "not found") { temp = "T"; }
			correction = isTrue(temp);
			
			temp = validParameter.validFile(parameters, "processors", false);		if (temp == "not found") { temp = "1"; }
			convert(temp, processors);
			
			temp = validParameter.validFile(parameters, "window", false);			if (temp == "not found") { temp = "0"; }
			convert(temp, window);
			
			temp = validParameter.validFile(parameters, "increment", false);		if (temp == "not found") { temp = "25"; }
			convert(temp, increment);
		}
	}
	catch(exception& e) {
		m->errorOut(e, "ChimeraBellerophonCommand", "ChimeraBellerophonCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

void ChimeraBellerophonCommand::help(){
	try {
		m->mothurOut("The chimera.bellerophon command reads a fastafile and creates list of potentially chimeric sequences.\n");
		m->mothurOut("The chimera.bellerophon command parameters are fasta, filter, correction, processors, window, increment. The fasta parameter is required.\n");
		m->mothurOut("The fasta parameter is required.  You may enter multiple fasta files by separating their names with dashes. ie. fasta=abrecovery.fasta-amzon.fasta \n");
		m->mothurOut("The filter parameter allows you to specify if you would like to apply a vertical and 50% soft filter, default=false. \n");
		m->mothurOut("The correction parameter allows you to put more emphasis on the distance between highly similar sequences and less emphasis on the differences between remote homologs, default=true.\n");
		m->mothurOut("The processors parameter allows you to specify how many processors you would like to use.  The default is 1. \n");
		#ifdef USE_MPI
		m->mothurOut("When using MPI, the processors parameter is set to the number of MPI processes running. \n");
		#endif
		m->mothurOut("The window parameter allows you to specify the window size for searching for chimeras, default is 1/4 sequence length. \n");
		m->mothurOut("The increment parameter allows you to specify how far you move each window while finding chimeric sequences, default is 25.\n");
		m->mothurOut("chimera.bellerophon(fasta=yourFastaFile, filter=yourFilter, correction=yourCorrection, processors=yourProcessors) \n");
		m->mothurOut("Example: chimera.bellerophon(fasta=AD.align, filter=True, correction=true, window=200) \n");
		m->mothurOut("Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFastaFile).\n\n");	
	}
	catch(exception& e) {
		m->errorOut(e, "ChimeraBellerophonCommand", "help");
		exit(1);
	}
}

//***************************************************************************************************************

ChimeraBellerophonCommand::~ChimeraBellerophonCommand(){	/*	do nothing	*/	}

//***************************************************************************************************************

int ChimeraBellerophonCommand::execute(){
	try{
		
		if (abort == true) { return 0; }
		
		for (int i = 0; i < fastaFileNames.size(); i++) {
			
			m->mothurOut("Checking sequences from " + fastaFileNames[i] + " ..." ); m->mothurOutEndLine();
			
			int start = time(NULL);	
			
			chimera = new Bellerophon(fastaFileNames[i], filter, correction, window, increment, processors, outputDir);	
					
			string outputFileName = outputDir + getRootName(getSimpleName(fastaFileNames[i])) +  "bellerophon.chimeras";
			string accnosFileName = outputDir + getRootName(getSimpleName(fastaFileNames[i])) + "bellerophon.accnos";
			
			chimera->getChimeras();
			
			if (m->control_pressed) { delete chimera; for (int i = 0; i < outputNames.size(); i++) {	remove(outputNames[i].c_str());	} return 0;	}
			
		#ifdef USE_MPI
			MPI_File outMPI;
			MPI_File outMPIAccnos;
			
			int outMode=MPI_MODE_CREATE|MPI_MODE_WRONLY; 
			
			char outFilename[1024];
			strcpy(outFilename, accnosFileName.c_str());

			char FileName[1024];
			strcpy(FileName, outputFileName.c_str());

			MPI_File_open(MPI_COMM_WORLD, FileName, outMode, MPI_INFO_NULL, &outMPI);  //comm, filename, mode, info, filepointer
			MPI_File_open(MPI_COMM_WORLD, outFilename, outMode, MPI_INFO_NULL, &outMPIAccnos);
	
			numSeqs = chimera->print(outMPI, outMPIAccnos);
			
			MPI_File_close(&outMPI);
			MPI_File_close(&outMPIAccnos);

		#else
		
			ofstream out;
			openOutputFile(outputFileName, out);
			
			ofstream out2;
			openOutputFile(accnosFileName, out2);
			
			numSeqs = chimera->print(out, out2);
			out.close();
			out2.close(); 
			
		#endif
			
			if (m->control_pressed) { remove(accnosFileName.c_str()); remove(outputFileName.c_str()); for (int i = 0; i < outputNames.size(); i++) {	remove(outputNames[i].c_str());	} delete chimera;	return 0;	}
			
			m->mothurOutEndLine(); m->mothurOut("It took " + toString(time(NULL) - start) + " secs to check " + toString(numSeqs) + " sequences.");	m->mothurOutEndLine(); m->mothurOutEndLine();
			
			outputNames.push_back(outputFileName);
			outputNames.push_back(accnosFileName);
			
			delete chimera;
		}
		
		m->mothurOutEndLine();
		m->mothurOut("Output File Names: "); m->mothurOutEndLine();
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}
		m->mothurOutEndLine();
		
		return 0;
				
	}
	catch(exception& e) {
		m->errorOut(e, "ChimeraBellerophonCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************
