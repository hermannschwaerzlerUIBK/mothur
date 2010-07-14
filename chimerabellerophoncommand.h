#ifndef CHIMERABELLEROPHONCOMMAND_H
#define CHIMERABELLEROPHONCOMMAND_H

/*
 *  chimerabellerophoncommand.h
 *  Mothur
 *
 *  Created by westcott on 4/1/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "mothur.h"
#include "command.hpp"
#include "chimera.h"


/***********************************************************/

class ChimeraBellerophonCommand : public Command {
public:
	ChimeraBellerophonCommand(string);
	~ChimeraBellerophonCommand();
	int execute();
	void help();
		
private:

	bool abort, filter, correction;
	string fastafile, outputDir;
	int processors, window, increment, numSeqs;
	Chimera* chimera;
	vector<string> outputNames;
	vector<string> fastaFileNames;
};

/***********************************************************/

#endif

