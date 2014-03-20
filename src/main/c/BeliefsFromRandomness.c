/*
 * Copyright 2011-2014, EDF. This software was developed with the collaboration of INRIA (Bastien Pietropaoli)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */




#include "BeliefsFromRandomness.h"

/**
 * This module provides with methods to randomly generate mass functions. It is roughly based on common
 * algorithms to do so. An interesting paper on the subject is "Random Generation of Mass Functions:
 * A Short Howto" by Thomas Burger and Sébastien Destercke (2012).
 *
 * @file BeliefsFromRandomness.c
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief APPLICATION: Gives structures and main
 *        functions to create random belief functions.
 */
 
/**
 * @name Utility functions
 * @{
 */

void BFR_generateRandomSeed(){
	struct tm* tm;
	time_t t;
	
	t = time(NULL);
	tm = gmtime(&t);
	
	srand(tm->tm_sec + tm->tm_min*60 + tm->tm_hour*3600 + tm->tm_mday*86400);
}

/**@}*/

/**
 * @name Random generators
 * @{
 */
 
BF_BeliefFunction BFR_getCrappyRandomBelief(const int elementSize){
	BF_BeliefFunction bf;
	int valid = 0;
	int i = 0, j = 0;
	int *elementNumbers = NULL;
	
	bf.elementSize = elementSize;
	bf.nbFocals = rand() % (int)(pow(2, elementSize));
	bf.focals = malloc(sizeof(BF_FocalElement) * bf.nbFocals);
	elementNumbers = malloc(sizeof(int) * bf.nbFocals);
	for(i = 0; i < bf.nbFocals; i++){
		valid = 0;
		while(!valid){
			elementNumbers[i] = rand() % (int)(pow(2, elementSize));
			valid = 1;
			for(j = 0; j < i; j++){
				if(elementNumbers[i] == elementNumbers[j]){
					valid = 0;
				}
			}
		}
		bf.focals[i].element = Sets_elementFromNumber(elementNumbers[i], elementSize);
		bf.focals[i].beliefValue = (float)rand() / RAND_MAX;
	}
	
	free(elementNumbers);
	BF_normalize(&bf);
	
	return bf;
}


BF_BeliefFunction BFR_getCrappyRandomBeliefWithFixedNbFocals(const int elementSize, const int nbFocals){
	BF_BeliefFunction bf = {NULL, 0, 0};
	int valid = 0;
	int i = 0, j = 0;
	int *elementNumbers = NULL;
	
	if(nbFocals <= pow(2, elementSize)){
		bf.elementSize = elementSize;
		bf.nbFocals = nbFocals;
		bf.focals = malloc(sizeof(BF_FocalElement) * nbFocals);
		elementNumbers = malloc(sizeof(int) * nbFocals);
		for(i = 0; i < bf.nbFocals; i++){
			valid = 0;
			while(!valid){
				elementNumbers[i] = rand() % (int)(pow(2, elementSize));
				valid = 1;
				for(j = 0; j < i; j++){
					if(elementNumbers[i] == elementNumbers[j]){
						valid = 0;
					}
				}
			}
			bf.focals[i].element = Sets_elementFromNumber(elementNumbers[i], elementSize);
			bf.focals[i].beliefValue = (float)rand() / RAND_MAX;
		}
		free(elementNumbers);
		BF_normalize(&bf);
	}
	
	return bf;
}


/**@}*/



