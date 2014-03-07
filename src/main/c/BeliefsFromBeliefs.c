/*
 * Copyright 2011-2013, EDF. This software was developed with the collaboration of INRIA (Bastien Pietropaoli)
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
 
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "BeliefsFromBeliefs.h"
#include "ReadDirectory.h"
#include "ReadFile.h"
#include "config.h"

/**
 * @section BFB_intro Introduction
 * This module enables the building of belief functions from another belief function based on another frame of discernment.
 *
 * @image html images/Xzibit.jpg "What Xzibit thinks of this module."
 * @image latex images/Xzibit.jpg "What Xzibit thinks of this module." width=8cm
 *
 * The concept of this transformation is based on the paper "Propagating Belief Functions in Qualitative Markov Trees" by G. Shafer et al. (1987).
 * Using relations between frames or simply by dividing huge frames of discernment into subframes, it is possible to work on smaller frames and
 * to transfer the belief functions from one frame to another. This is called coarsening (big -> smaller) or vacuous extension (small -> bigger).
 *
 * In this module, we extend the possibilities by offering a way to manage fuzzy subsets. By this, we mean that the mass on one set could be
 * transferred to multiple sets because. It could be convenient when working with fuzzy concepts in natural language or simply because links
 * between possible world is not a perfect correlation.
 *
 * @section BFS_HowTo How to use this module
 * To build the models, you should do the following :
 * @li For each thing you want to compute, create a directory with the name you want in /data/beliefsFromBeliefs (or the directory defined by BFB_PATH).
 * For example, if you want to compute a thing called "Presence" to determine if there is someone in a room or not, just call the directory "Presence".
 * @li In this directory, create a file called "values" (or with the name defined by BFB_VALUES_NAME) with on each line, the name of
 * one possible world (and no more line than necessary !). Those worlds are the ones corresponding to the thing you want to compute.
 * Example:
 * @code
 * Aka
 * Bea
 * Elf
 * @endcode
 * This example creates three possible worlds : Aka, Bea and Elf.
 * @li In this directory, for each subframe on which beliefs will be based, build a directory with the name of that subframe.
 * @li In each of this directory, create a file also called "values" (or with the name defined by BFB_VALUES_NAME) with on each line, the name of
 * one possible world (and no more line than necessary !). Those worlds are the ones of the subframe.
 * @li In each subframe directory, for each subset of possible worlds, create a file (Those files can have any name, that
 * is not important at all...) with on first line, the number of considered worlds of the subframe, then the number of transformations and for each transformation
 * the number of considered worlds, the worlds' names and then the transformation factor.
 * Considere an example where the frame Posture = {Seated, Standing, LyingDown} has to be transformed into the frame Sleeping = {Yes, No}. The file corresponding
 * to the transformation of the element {Seated u Standing} could be written like this :
 * @code
 * 2 elements           //The number of worlds in the element of the frame we're coming from
 * Seated               //World 1
 * Standing             //World 2
 * 2 conversions        //Number of resulting elements
 * 1 element            //+-+
 * no                   //  |-> First element, {no} with a factor of 0.3
 * 0.3                  //+-+
 * 2 elements           //+-+
 * yes                  //  |-> Second element, {yes u no} with a factor of 0.7
 * no                   //  |
 * 0.7                  //+-+
 * @endcode
 * With this file, any belief m({Seated u Standing}) whould result in m({no}) = 0.3 * m({Seated u Standing}) and m({yes u no}) = 0.7 * m({Seated u Standing}).
 * If there's not a file for each possible subset of possible worlds, it is possible to get weird behaviors and sum errors.
 * Also, the sum of masses of any transformations should equal 1 or it won't work properly! Uncomment the constant CHECK_MODELS
 * in config.h to make sure your models are okay.
 *
 * WARNING: Check that your files are in the correct format for you OS (specifically for the endline character...), thus,
 * do not forget to format them correctly (or you could see ^M characters appearing in your possible worlds for example...).
 * To convert in a correct format, on Linux, it is possible to use the package 'tofrodos' with the two commands 'fromdos' and 'todos'.
 *
 * If you're not sure you understood this quick explanation, please check the example files (for instance those used
 * to run tests...).
 * 
 * Once everything has been created correctly, it is possible to load a belief structure using 
 * BFB_loadBeliefStructure(nameOfTheThingYouWantToCompute), the files will all be loaded automatically.
 * The functions BFB_believeFromBeliefs() et BFB_believeFromBelief() are then used to build the mass functions from
 * given belief functions.
 *
 * @file BeliefsFromBeliefs.c
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief APPLICATION: Gives structures and main
 *        functions to create belief functions
 *        from belief functions.
 */


/**
 * @name Loading a model
 * @{
 */

BFB_BeliefStructure BFB_loadBeliefStructure(const char* frameName){
	BFB_BeliefStructure bs = {NULL, {NULL, 0}, NULL, 0};
	char path[MAX_SIZE_PATH];
    int* charsPerDir = NULL;
    char** directories = NULL;
    int i = 0;
    
    /*Test if the model exists:  */
    strcpy(path, BFB_PATH);       /* The directory where to find the models  */
    strcat(path, frameName);      /* The name of the frame  */
    if(ReadDir_isDirectory(path)){
    	/*Copy the frame name: */
        bs.frameName = malloc(sizeof(char)*(strlen(frameName) + 1));
        #ifdef DEBUG
        if(bs.frameName == NULL){
        	printf("debug: malloc failed in BFB_loadBeliefStructure() for \"bs.frameName\".\n");
        }
        #endif
        strcpy(bs.frameName, frameName);
        strcat(bs.frameName, "\0");
        /*Creation of the path to load the set: */
        strcat(path, "/");         
        strcat(path,BFB_VALUES_NAME);
        /*Load the reference list: */
        bs.refList = Sets_loadRefList(path);
        /*Get the number of subframes: */
        strcpy(path, BFB_PATH);    
        strcat(path, frameName);      
        bs.nbBeliefs = ReadDir_countDirectories(path);
        /*Get the directories: */
        charsPerDir = ReadDir_charsPerDirectory(path, bs.nbBeliefs);
        directories = ReadDir_getDirectories(path, bs.nbBeliefs, charsPerDir);
        /*Load the beliefs from beliefs: */
        bs.beliefs = malloc(sizeof(BFB_BeliefFromBelief)*bs.nbBeliefs);
        #ifdef DEBUG
        if(bs.beliefs == NULL){
        	printf("debug: malloc failed in BFB_loadBeliefStructure() for \"bs.beliefs\".\n");
        }
        #endif
        for(i = 0; i < bs.nbBeliefs; i++){
            strcpy(path, BFB_PATH);         /* The directory where to find the CAs */
            strcat(path, frameName);        /* The name of the CA */
            strcat(path, "/");              /* Enter the directory... */
            strcat(path, directories[i]);   /* Name of the directory */
            bs.beliefs[i] = BFB_loadBeliefFromBelief(directories[i], path, bs.refList);
        }
        /*Deallocate: */
        free(charsPerDir);
        for(i = 0; i<bs.nbBeliefs; i++){
            free(directories[i]);
        }
        free(directories);
    }
    #ifdef DEBUG
    else {
        printf("debug: Error opening the directory %s.\n", path);
    }
    #endif
    
    return bs;
}

BFB_BeliefFromBelief BFB_loadBeliefFromBelief(const char* frameOfOrigin, const char* path, const Sets_ReferenceList rl){
	BFB_BeliefFromBelief bfb;
	int i = 0, nbFiles = 0, beliefIndex = 0;
    int* charsPerFile = NULL;
    char filepath[MAX_SIZE_PATH], temp[MAX_STR_LEN];
    char** filenames = NULL;
	
	if(ReadDir_isDirectory(path)){
		/*Copy the frame name: */
        bfb.frameName = malloc(sizeof(char)*(strlen(frameOfOrigin) + 1));
        #ifdef DEBUG
        if(bfb.frameName == NULL){
        	printf("debug: malloc failed in BFB_loadBeliefFromBelief() for \"bfb.frameName\".\n");
        }
        #endif
        strcpy(bfb.frameName, frameOfOrigin);
        strcat(bfb.frameName, "\0");
        /*Load the reference list : */
        strcpy(temp, path);
        strcat(temp, "/");
        strcat(temp, BFB_VALUES_NAME);
        bfb.refList = Sets_loadRefList(temp);
        /*Get the files: */
        nbFiles = ReadDir_countFiles(path);
        charsPerFile = ReadDir_charsPerFilename(path, nbFiles);
        filenames = ReadDir_getFilenames(path, nbFiles, charsPerFile);
        /*Load the vectors: */
        bfb.nbVectors = nbFiles - 1;
        #ifdef CHECK_MODELS
        if(bfb.nbVectors != pow(2, bfb.refList.card) - 1){
        	printf("debug: MODEL CHECKING = FAIL! You didn't write a file for each possible element in %s.\n", path);
        }
        #endif
        bfb.vectors = malloc(sizeof(BFB_BeliefVector) * bfb.nbVectors);
        #ifdef DEBUG
        if(bfb.vectors == NULL){
        	printf("debug: malloc failed in BFB_loadBeliefFromBelief() for \"bfb.vectors\".\n");
        }
        #endif
        for(i = 0; i < nbFiles; i++){
            strcpy(filepath, path);
            strcat(filepath, "/");
            strcat(filepath, filenames[i]);
            if(strcmp(filenames[i], BFB_VALUES_NAME)){
                bfb.vectors[beliefIndex] = BFB_loadBeliefVector(filepath, bfb.refList, rl);
                beliefIndex++;
            }
        }
        /*Deallocate: */
        free(charsPerFile);
        for(i = 0; i<nbFiles; i++){
            free(filenames[i]);
        }
        free(filenames);
	}
	#ifdef DEBUG
    else{
        printf("%s is not a directory.\n", path);
    }
    #endif
    
    return bfb;
}

BFB_BeliefVector BFB_loadBeliefVector(const char* fileName, const Sets_ReferenceList rlFrom, const Sets_ReferenceList rlTo){
	BFB_BeliefVector bv;
	int i = 0, j = 0, nbElements = 0, nbLines = 0, lineCounter = 0;
    int* charsPerLin = 0;
    char** elements = NULL;
    char** lines = NULL;
    #ifdef CHECK_MODELS
    float sum = 0;
    #endif
    
    if(ReadDir_isFile(fileName)){
        /*Read the file: */
        nbLines = ReadFile_countLines(fileName);
        charsPerLin = ReadFile_charsPerLine(fileName, nbLines);
        lines = ReadFile_readLines(fileName, nbLines, charsPerLin);
        /*Element From : */
        nbElements = atoi(lines[0]);
        lineCounter++;
        elements = malloc(sizeof(char*)*nbElements);
        #ifdef DEBUG
		if(elements == NULL){
			printf("debug: malloc failed in BFB_loadBeliefVector() for \"elements\".\n");
		}
		#endif
        for(i = 0; i < nbElements; i++){
            elements[i] = lines[lineCounter];
            lineCounter++;
        }
        bv.from = Sets_createElementFromStrings((const char* const * const)elements, nbElements, rlFrom);
        /*Get the number of conversions: */
        bv.nbTos = atoi(lines[lineCounter]);
        lineCounter++;
        bv.to = malloc(sizeof(Sets_Element) * bv.nbTos);
        #ifdef DEBUG
		if(bv.to == NULL){
			printf("debug: malloc failed in BFB_loadBeliefVector() for \"bv.to\".\n");
		}
		#endif
		bv.factors = malloc(sizeof(float) * bv.nbTos);
		#ifdef DEBUG
		if(bv.factors == NULL){
			printf("debug: malloc failed in BFB_loadBeliefVector() for \"bv.factors\".\n");
		}
		#endif
        /*Elements To : */
        for(i = 0; i < bv.nbTos; i++){
        	free(elements);
        	nbElements = atoi(lines[lineCounter]);
        	lineCounter++;
        	elements = malloc(sizeof(char*)*nbElements);
		    #ifdef DEBUG
			if(elements == NULL){
				printf("debug: malloc failed in BFB_loadBeliefVector() for \"elements\".\n");
			}
			#endif
        	for(j = 0; j < nbElements; j++){
		        elements[j] = lines[lineCounter];
		        lineCounter++;
		    }
		    bv.to[i] = Sets_createElementFromStrings((const char* const * const)elements, nbElements, rlTo);
		    bv.factors[i] = atof(lines[lineCounter]);
		    lineCounter++;
        }
        /*Deallocate: */
        free(charsPerLin);
        for(i = 0; i < nbLines; i++){
            free(lines[i]);
        }
        free(lines);
        free(elements);
    }
    #ifdef DEBUG
    else{
        printf("%s is not a file.\n", fileName);
    }
    #endif
    
    #ifdef CHECK_MODELS
    for(i = 0; i < bv.nbTos; i++){
    	sum += bv.factors[i];
    }
    if(sum != 1){
    	printf("debug: CHECK MODELS FAIL!\n");
    	printf("debug: In the file %s, the sum of factors should be 1.\n", fileName);
    }
    #endif
    
	return bv;
}

/** @} */


/**
 * @name Building belief functions
 * @{
 */

BF_BeliefFunction* BFB_believeFromBeliefs(const BFB_BeliefStructure bs, const char* const * const frameNames, const BF_BeliefFunction* from, const int nbBF){
	BF_BeliefFunction* bf = NULL;
    int i = 0, j = 0, in = 0;

    /*Memory allocation: */
    bf = malloc(sizeof(BF_BeliefFunction) * nbBF);
    #ifdef DEBUG
	if(bf == NULL){
		printf("debug: malloc failed in BFB_believeFromBeliefs() for \"bf\".\n");
	}
	#endif

    /*Get the functions: */
    for(i = 0; i<nbBF; i++){
    	in = 0;
        for(j = 0; j<bs.nbBeliefs; j++){
            if(strcmp(bs.beliefs[j].frameName, frameNames[i]) == 0){
                bf[i] = BFB_believeFromBelief(bs.beliefs[j], from[i], bs.refList.card);
                in = 1;
                break;
            }
        }
        if(!in){
        	bf[i] = BF_getVacuousBeliefFunction(bs.refList.card);
        }
    }
    
	return bf;
}

BF_BeliefFunction BFB_believeFromBelief(const BFB_BeliefFromBelief bfb, const BF_BeliefFunction from, const int elementSize){
	BF_BeliefFunction bf;
	int i = 0, j = 0, k = 0, l = 0;
	int nbFocals = 0;
	char in = 0;
	Sets_Element emptyset;
	float emptyMass = 0;
	
	/*Init: */
	bf.nbFocals = 0;
	bf.focals = NULL;
	bf.elementSize = elementSize;
	
	/*Process the empty set: */
	emptyset = Sets_getEmptyElement(from.elementSize);
	emptyMass = BF_M(from, emptyset);
	if(emptyMass > 0){
		bf.focals = malloc(sizeof(BF_FocalElement));
		#ifdef DEBUG
		if(bf.focals == NULL){
			printf("debug: realloc failed in BFB_believeFromBelief() for \"bf.focals\".\n");
		}
		#endif
		bf.nbFocals++;
		bf.focals[0].element = Sets_getEmptyElement(elementSize);
		bf.focals[0].beliefValue = emptyMass;
	}
	
	/*Transform: */
	for(i = 0; i < from.nbFocals; i++){
		for(j = 0; j < bfb.nbVectors; j++){
			if(Sets_equals(from.focals[i].element, bfb.vectors[j].from, from.elementSize) &&
			   !Sets_equals(from.focals[i].element, emptyset, from.elementSize)){
				nbFocals = bf.nbFocals;
				for(k = 0; k < bfb.vectors[j].nbTos; k++){
					in = 0;
					for(l = 0; l < nbFocals; l++){
						if(Sets_equals(bf.focals[l].element, bfb.vectors[j].to[k], elementSize)){
							in = 1;
							bf.focals[l].beliefValue += from.focals[i].beliefValue * bfb.vectors[j].factors[k];
							break;
						}	
					}
					if(!in){
						bf.focals = realloc(bf.focals, sizeof(BF_FocalElement) * (bf.nbFocals + 1));
						#ifdef DEBUG
						if(bf.focals == NULL){
							printf("debug: realloc failed in BFB_believeFromBelief() for \"bf.focals\".\n");
						}
						#endif
						bf.nbFocals++;
						bf.focals[bf.nbFocals - 1].element = Sets_copyElement(bfb.vectors[j].to[k], elementSize);
						bf.focals[bf.nbFocals - 1].beliefValue = from.focals[i].beliefValue * bfb.vectors[j].factors[k];
					} 
				}
				break;
			}
		}
	}
	Sets_freeElement(&emptyset);
	
	#ifdef CHECK_VALUES
    if(BF_checkValues(bf)){
    	printf("debug: in BFB_believeFromBelief(), at least one value is not valid!\n");
    }
	#endif
	
	#ifdef CHECK_SUM
	if(BF_checkSum(bf)){
    	printf("debug: Frame name = %s\n", bfb.frameName);
        printf("debug: in BFB_beliefFromBelief(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
        printf("debug: Resulting belief function:\n");
        for(i = 0; i < bf.nbFocals; i++){
        	printf("debug: ");
        	for(j = 0; j < elementSize; j++){
        		printf("%d", bf.focals[i].element.values[j]);
        	}
        	printf(" : %f\n", bf.focals[i].beliefValue);
        }
    }
	#endif
	
	return bf;
}

/** @} */


/**
 * @name Free memory
 * @{
 */

void BFB_freeBeliefStructure(BFB_BeliefStructure* bs){
	int i = 0;
	
	free(bs->frameName);
	Sets_freeReferenceList(&(bs->refList));
	for(i = 0; i < bs->nbBeliefs; i++){
		BFB_freeBeliefFromBelief(&(bs->beliefs[i]));
	}
	free(bs->beliefs);
}

void BFB_freeBeliefFromBelief(BFB_BeliefFromBelief* bfb){
	int i = 0;
	
	free(bfb->frameName);
	Sets_freeReferenceList(&(bfb->refList));
	for(i = 0; i < bfb->nbVectors; i++){
		BFB_freeBeliefVector(&(bfb->vectors[i]));
	}
	free(bfb->vectors);
}

void BFB_freeBeliefVector(BFB_BeliefVector* bv){
	int i = 0;
	
	free(bv->factors);
	Sets_freeElement(&(bv->from));
	for(i = 0; i < bv->nbTos; i++){
		Sets_freeElement(&(bv->to[i]));
	}
	free(bv->to);
}

/** @} */


/**
 * @name Conversion into strings
 * @}
 */

char* BFB_beliefStructureToString(const BFB_BeliefStructure bs){
	char *str = NULL;
	char** beliefs = NULL;
	int i = 0, totChar = 0;
	int len = 0;
	
	beliefs = malloc(sizeof(char*) * bs.nbBeliefs);
	#ifdef DEBUG
	if(beliefs == NULL){
		printf("debug: malloc failed in BFB_beliefStructureToString() for \"beliefs\".\n");
	}
	#endif
	for(i = 0; i < bs.nbBeliefs; i++){
		beliefs[i] = BFB_beliefFromBeliefToString(bs.beliefs[i], bs.refList);
		totChar += strlen(beliefs[i]);
	}
	
	/*Count total char: */
	totChar += (strlen(bs.frameName) + 5) * 3 + 1;
	
	/*Print: */
	str = malloc(sizeof(char) * (totChar+1));
	#ifdef DEBUG
	if(str == NULL){
		printf("debug: malloc failed in BFB_beliefStructureToString() for \"str\".\n");
	}
	#endif
	strcpy(str, "*");
	len = strlen(bs.frameName);
	for(i = 0; i < len + 3; i++){
		strcat(str, "*");
	}
	strcat(str, "\n* ");
	strcat(str, bs.frameName);
	strcat(str, " *\n");
	for(i = 0; i < len + 4; i++){
		strcat(str, "*");
	}
	strcat(str, "\n");
	for(i = 0; i < bs.nbBeliefs; i++){
		strcat(str, beliefs[i]);
	}
	
	for(i = 0; i < bs.nbBeliefs; i++){
		free(beliefs[i]);
	}
	free(beliefs);
	
	return str;
}

char* BFB_beliefFromBeliefToString(const BFB_BeliefFromBelief bfb, const Sets_ReferenceList to){
	char *str = NULL, *str2 = NULL, *str3 = NULL;
	char** vectors = NULL;
	Sets_Set valuesTo, valuesFrom;
	int i = 0, totChar = 0;
	int len = 0;
	
	/*Vectors' strings: */
	vectors = malloc(sizeof(char*) * bfb.nbVectors);
	#ifdef DEBUG
	if(vectors == NULL){
		printf("debug: malloc failed in BFB_beliefFromBeliefToString() for \"vectors\".\n");
	}
	#endif
	for(i = 0; i < bfb.nbVectors; i++){
		vectors[i] = BFB_beliefVectorToString(bfb.vectors[i], to, bfb.refList);
		totChar += strlen(vectors[i]);
	}
	
	/*Values string: */
	valuesFrom = Sets_createSetFromRefList(bfb.refList);
	valuesTo = Sets_createSetFromRefList(to);
	
	str2 = Sets_setToString(valuesFrom, bfb.refList);
	str3 = Sets_setToString(valuesTo, to);
	
	/*Count number of chars: */
	totChar += (strlen(bfb.frameName) + 12) * 2 + strlen(str2) + strlen(str3) + 17;/* + bfb.nbVectors; */
	
	/*Print: */
	str = malloc(sizeof(char) * (totChar+1));
	#ifdef DEBUG
	if(str == NULL){
		printf("debug: malloc failed in BFB_beliefFromBeliefToString() for \"str\".\n");
	}
	#endif
	sprintf(str, "Subframe %s :\n", bfb.frameName);
	len = strlen(bfb.frameName);
	for(i = 0; i < len + 11; i++){
		strcat(str, "-");
	}
	strcat(str, "\n");
	strcat(str, "From : ");
	strcat(str, str2);
	strcat(str, "\nTo   : ");
	strcat(str, str3);
	strcat(str, "\n\n");
	for(i = 0; i < bfb.nbVectors; i++){
		strcat(str, vectors[i]);
		/*strcat(str, "\n"); */
	}
	strcat(str, "\0");
	
	/*Free: */
	free(str2);
	free(str3);
	Sets_freeSet(&valuesFrom);
	Sets_freeSet(&valuesTo);
	for(i = 0; i < bfb.nbVectors; i++){
		free(vectors[i]);
	}
	free(vectors);
	
	return str;
}

char* BFB_beliefVectorToString(const BFB_BeliefVector bv, const Sets_ReferenceList to, const Sets_ReferenceList from){
	char* str = NULL;
	char temp[MAX_STR_LEN], temp2[MAX_STR_LEN];
	char* elem = NULL;
	int i = 0;
	
	elem = Sets_elementToString(bv.from, from);
	sprintf(temp2, "From %s to :\n", elem);
	free(elem);
	
	for(i = 0; i < bv.nbTos; i++){
		elem = Sets_elementToString(bv.to[i], to);
		sprintf(temp, " --> %s : %f\n", elem, bv.factors[i]);
		strcat(temp2, temp);
		free(elem);
	}
	
	str = malloc(sizeof(char) * (strlen(temp2) + 1));
	#ifdef DEBUG
	if(str == NULL){
		printf("debug: malloc failed in BFB_beliefVectorToString() for \"str\".\n");
	}
	#endif
	strcpy(str, temp2);
	strcat(str, "\0");
	
	return str;
}


/** @} */


