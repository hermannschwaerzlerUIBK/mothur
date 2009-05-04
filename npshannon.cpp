/*
 *  npshannon.cpp
 *  Dotur
 *
 *  Created by John Westcott on 1/7/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "npshannon.h"

/***********************************************************************/


EstOutput NPShannon::getValues(SAbundVector* rank){
	try {
		data.resize(1,0);
		float npShannon = 0.0000;
	
		double maxRank = (double)rank->getMaxRank();
		int sampled = rank->getNumSeqs();
	
		double Chat = 1.0000 - (double)rank->get(1)/(double)sampled;
	
		if(Chat>0)	{	
			for(int i=1;i<=maxRank;i++){
				double pi = ((double) i)/((double)sampled);
				double ChatPi = Chat*pi;
				if(ChatPi>0){
					npShannon += rank->get(i) * ChatPi*log(ChatPi)/(1-pow(1-ChatPi,(double)sampled));
					//cout << ChatPi << '\t' << rank->get(i) * ChatPi*log(ChatPi)/(1-pow(1-ChatPi,(double)sampled)) << endl;
				}
			}
			npShannon = -npShannon;
		}
		else{
			npShannon = 0.000;
		}
	
		data[0] = npShannon;
		
		if (isnan(data[0]) || isinf(data[0])) { data[0] = 0; }
		
		return data;
	}
	catch(exception& e) {
		cout << "Standard Error: " << e.what() << " has occurred in the NPShannon class Function getValues. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}
	catch(...) {
		cout << "An unknown error has occurred in the NPShannon class function getValues. Please contact Pat Schloss at pschloss@microbio.umass.edu." << "\n";
		exit(1);
	}	
}

/***********************************************************************/
