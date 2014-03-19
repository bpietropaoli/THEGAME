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


#include "BeliefsFromSensors.h"


/**
 * @section BFS_intro Introduction
 * This module enables the building of belief functions from raw sensor measures.
 * The method used is the one used in Ricquebourg et at. 2007 and Pietropaoli et al. 2011/2012.
 * For more details, please check those references. Anyway, I will try here to introduce the way it works.
 * The mass functions are built from raw sensor measures using predefined set of mass functions (cf figure).
 * 
 * @image html ../images/FunctionSet.jpg "Example of set of mass functions for a Phidget motion sensor connected to an USB Interface Kit and applied to the detection of presence."
 * @image latex ../images/FunctionSet.jpg "Example of set of mass functions for a Phidget motion sensor connected to an USB Interface Kit and applied to the detection of presence." width=8cm
 *
 * In the given example, whenever a raw sensor measure is received, the projection on this set gives 
 * the resulting mass function. For instance, with the given figure, if the motion
 * sensor returns a value of 450, then the resulting mass function would have two focal
 * sets: m({Yes}) = 0.7 and m({Yes u No}) = 0.3.
 *
 * The following options are developped until now:
 * @li Variation : considers the variation of the raw measure instead of the measure itself.
 * It takes as a parameter the number of previous measures to compare to.
 * @li Tempo-Specificity : includes temporization in the building of mass functions using a discrimination based on specificity. Please 
 * read Pietropaoli et al. 2012 for explanations. It takes the number of seconds before complete forgetness as parameter.
 * @li Tempo-Fusion : includes temporization in the building of mass functions using a fusion based on Dubois & Prade's combination rule.
 * It takes the number of seconds before complete forgetness as parameter.
 *
 * @section BFS_howto How to use this module
 * So, to build your sets of mass functions, you should do the following :
 * @li For each thing you want to compute, create a directory with the name you want in data/beliefsFromSensors/ (or the directory defined by BFS_PATH).
 * For example, if you want to compute a thing called "Presence" to determine if there is someone in a room or not, just call the directory "Presence".
 * @li In this directory, create a file called "values" (or with the name defined by BFS_VALUES_NAME) with on each line, the name of
 * one possible world (and no more line than necessary !)
 * Example:
 * @code
 * Yes
 * No
 * @endcode
 * This example creates two possible worlds : Yes and No (useful for a binary case...).
 * @li In this directory, for each sensor, build a directory with the name of that sensor.
 * @li In each sensor subdirectory, create, if necessary, a file called "options" (no extension...) with on each
 * line, the name of one of the options you want and the corresponding parameter. Example:
 * @code
 * 2 options                 //Number of options
 * Tempo-Specificity 3       //Type of tempo + nb of seconds
 * Variation 2               //Variation + nb of measures to consider
 * @endcode
 * This options file applies a temporization of 3 seconds and compares the given measure with the 2 previous one...
 * The case does not matter to write the name of the options.
 * @li In each sensor subdirectory, for each subset of possible worlds, create a file with on first line, the number
 * of considered world in the subset, then come the names of those worlds, one per line. Then, put the number of specific
 * points used to build the set of mass functions. Then, indicate the couples "sensorMeasure beliefValue" for the set of
 * mass functions, one per line. Example:
 * @code
 * 2 elements   //Number of worlds in the subset
 * Aka          //World 1
 * Elf          //World 2
 * 5 points     //Number of considered measure
 * 100 0        //For a measure of 100, get a null mass for this subset of worlds
 * 200 0.40     //etc...
 * 300 0.15
 * 400 0.40
 * 500 0
 * @endcode
 * This file corresponds to the set of mass function only for the subset {Aka u Elf}. Those files can have any name, that
 * is not important at all... However, the given sensor measures should be in an increasing order or it could produce weird
 * behaviors! Also, for any measure, the sum of masses over files should always be 1. Uncomment the constant CHECK_MODELS 
 * in config.h to make sure your models are okay.
 *
 * It is not actually necessary to write a file for each subset of possible worlds. A file per possible focal element is enough.
 *
 * WARNING: Check that your files are in the correct format for you OS (specifically for the endline character...), thus,
 * do not forget to format them correctly (or you could see ^M characters appearing in your possible worlds for example...).
 * To convert in a correct format, on Linux, it is possible to use the package 'tofrodos' with the two commands 'fromdos' and 'todos'.
 *
 * If you're not sure you understood this quick explanation, please check the example files (for instance those used
 * to run tests...).
 * 
 * Once everything has been created correctly, it is possible to load a belief structure using 
 * BFS_loadBeliefStructure(nameOfTheThingYouWantToCompute), the files will all be loaded automatically.
 * The functions BFS_getEvidences() et BFS_getProjection() are then used to build the mass functions from
 * given raw measures.
 *
 * @file BeliefsFromSensors.c
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief APPLICATION: Gives structures and main
 *        functions to create belief functions
 *        from sensor measures.
 */


/* !!!!!!!!!!!!!!!!!!!!!!!!!!!
   !!! Creation of beliefs !!!
   !!!!!!!!!!!!!!!!!!!!!!!!!!! */

/**
 * @name Loading a model
 * @{
 */

BFS_BeliefStructure BFS_loadBeliefStructure(const char* frameName){
    BFS_BeliefStructure bs = {NULL, {NULL,0}, {NULL,0}, {NULL,0}, NULL, 0};
    char path[MAX_SIZE_PATH];
    int* charsPerDir = NULL;
    char** directories = NULL;
    int i = 0;

    /*Test if the CA exists: */
    strcpy(path, BFS_PATH);       /* The directory where to find the CAs */
    strcat(path, frameName);      /* The name of the CA */
    if(ReadDir_isDirectory(path)){
        /*Copy the context attribute name: */
        bs.frameName = malloc(sizeof(char)*(strlen(frameName) + 1));
        DEBUG_CHECK_MALLOC(bs.frameName);

        strcpy(bs.frameName, frameName);
        strcat(bs.frameName, "\0");
        /*Creation of the path to load the set: */
        strcat(path, "/");         /* Enter the directory... */
        strcat(path,BFS_VALUES_NAME);  /* The name of the file containing the values */
        /*Load the reference list: */
        bs.refList = Sets_loadRefList(path);
        /*Create the set: */
        bs.possibleValues = Sets_createSetFromRefList(bs.refList);
        /*Create the powerset: */
        bs.powerset = Sets_createPowerSet(bs.possibleValues);
        /*Get the number of sensors: */
        strcpy(path, BFS_PATH);     /* The directory where to find the CAs */
        strcat(path, frameName);      /* The name of the CA */
        bs.nbSensors = ReadDir_countDirectories(path);
        /*Get the directories:*/
        charsPerDir = ReadDir_charsPerDirectory(path, bs.nbSensors);
        directories = ReadDir_getDirectories(path, bs.nbSensors, charsPerDir);
        /*Load the sensors' beliefs: */
        bs.beliefs = malloc(sizeof(BFS_SensorBeliefs)*bs.nbSensors);
        DEBUG_CHECK_MALLOC(bs.beliefs);

        for(i = 0; i<bs.nbSensors; i++){
            strcpy(path, BFS_PATH);       /* The directory where to find the CAs */
            strcat(path, frameName);      /* The name of the CA */
            strcat(path, "/");            /* Enter the directory... */
            strcat(path, directories[i]);         /* Name of the directory */
            bs.beliefs[i] = BFS_loadSensorBeliefs(directories[i], path, bs.refList);
        }
        /*Deallocate: */
        free(charsPerDir);
        for(i = 0; i<bs.nbSensors; i++){
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

BFS_SensorBeliefs BFS_loadSensorBeliefs(const char* sensorType, const char* path, const Sets_ReferenceList rl){
    BFS_SensorBeliefs sb = {NULL, NULL, 0, NULL, 0, OP_NONE};
    int i = 0, j = 0, k = 0, nbLines = 0, beliefIndex = 0, nbFiles = 0, opIndex = 0;
    int* charsPerFile = NULL, *charPerLine = NULL;
    char filepath[MAX_SIZE_PATH], *temp, *temp2;
    char** filenames = NULL, **lines = NULL;
    int applyOptions = 0;
    #ifdef CHECK_MODELS
    BF_BeliefFunction projection;
    double fakeMeasure = 0;
    #endif

    if(ReadDir_isDirectory(path)){
    	temp = malloc(sizeof(char) * MAX_STR_LEN);
    	temp2 = malloc(sizeof(char) * MAX_STR_LEN);
        /*Sensor type: */
        sb.sensorType = malloc(sizeof(char)*(strlen(sensorType)+1));
        DEBUG_CHECK_MALLOC(sb.sensorType);

        strcpy(sb.sensorType, sensorType);
        strcat(sb.sensorType, "\0");
        /*Get the files: */
        nbFiles = ReadDir_countFiles(path);
        charsPerFile = ReadDir_charsPerFilename(path, nbFiles);
        filenames = ReadDir_getFilenames(path, nbFiles, charsPerFile);
        /*Look for options: */
        for(i = 0; i<nbFiles; i++){
            if(!strcmp(filenames[i], "options")){
                applyOptions = 1;
            }
        }
        if(!applyOptions){
            sb.nbOptions = 0;
            sb.options = NULL;
            sb.nbFocal = nbFiles;
        }
        else {
            sb.nbFocal = nbFiles - 1;
        }
        sb.optionFlags = OP_NONE;
        /*Load the files: */
        sb.beliefOnElements = malloc(sizeof(BFS_PartOfBelief)*sb.nbFocal);
        DEBUG_CHECK_MALLOC(sb.beliefOnElements);

        for(i = 0; i<nbFiles; i++){
            strcpy(filepath, path);
            strcat(filepath, "/");
            strcat(filepath, filenames[i]);
            if(strcmp(filenames[i], "options")){
                sb.beliefOnElements[beliefIndex] = BFS_loadPartOfBelief(filepath, rl);
                beliefIndex++;
            }
            /*Options loading: */
            else{
                nbLines = ReadFile_countLines(filepath);
                charPerLine = ReadFile_charsPerLine(filepath, nbLines);
                lines = ReadFile_readLines(filepath, nbLines, charPerLine);
                /*Upper the lines: */
                for(j = 0; j < nbLines; j++){
                	for(k = 0; k < charPerLine[j]; k++){
                		lines[j][k] = toupper(lines[j][k]);
                	}
                }
                /*Memory allocation : */
                sb.nbOptions = atoi(lines[0]);
                sb.options = malloc(sizeof(BFS_Option)*sb.nbOptions);
                DEBUG_CHECK_MALLOC(sb.options);

                for(j = 0; j < sb.nbOptions; j++){
                    sscanf(lines[1+j], "%s %s", temp, temp2);
                    sb.options[j].type = OP_NONE;
                    sb.options[j].parameter = atof(temp2);
                    /* ------------------------------
                       Option specific parameters:
                       ------------------------------
                       Tempo-specificity:
                       Storage: time of the previous measure + a pointer to the previous BF_BeliefFunction*/
                    if(!strcmp(temp, "TEMPO-SPECIFICITY")){
                        sb.options[j].util = malloc(sizeof(BFS_UtilData) * 2);
                        DEBUG_CHECK_MALLOC(sb.options[j].util);

                        clock_gettime(CLOCK_ID, &(sb.options[j].util[0].time));
                        sb.options[j].util[1].bf.nbFocals = 0;
                        sb.options[j].util[1].bf.focals = NULL;
                        sb.options[j].util[1].bf.elementSize = 0;
                        sb.options[j].type = OP_TEMPO_SPECIFICITY;
                        sb.optionFlags = sb.optionFlags | OP_TEMPO_SPECIFICITY;
                    }
                    /*Variation: */
                    /*Storage: the "parameter" previous measures */
                    else if(!strcmp(temp, "VARIATION")){
                        sb.options[j].parameter = (int)(sb.options[j].parameter);
                        if(sb.options[j].parameter == 0){
                            sb.options[j].parameter = 1;
                        }
                        sb.options[j].util = malloc(sizeof(BFS_UtilData) * sb.options[j].parameter);
                        DEBUG_CHECK_MALLOC(sb.options[j].util);

                        for(opIndex = 0; opIndex < sb.options[j].parameter; opIndex++){
                            sb.options[j].util[opIndex].measure = 0;
                        }
                        sb.options[j].type = OP_VARIATION;
                        sb.optionFlags = sb.optionFlags | OP_VARIATION;
                    }
                    /*Tempo-fusion: */
                    else if(!strcmp(temp, "TEMPO-FUSION")){
                    	sb.options[j].util = malloc(sizeof(BFS_UtilData) * 2);
                        DEBUG_CHECK_MALLOC(sb.options[j].util);

                        clock_gettime(CLOCK_ID, &(sb.options[j].util[0].time));
                        sb.options[j].util[1].bf.nbFocals = 0;
                        sb.options[j].util[1].bf.focals = NULL;
                        sb.options[j].util[1].bf.elementSize = 0;
                        sb.options[j].type = OP_TEMPO_FUSION;
                        sb.optionFlags = sb.optionFlags | OP_TEMPO_FUSION;
                    }
                    /*WTF: */
                    else{
                        sb.options[j].util = NULL;
                        #ifdef DEBUG
                        printf("debug: the option %s is not implemented!\n", temp);
                        #endif
                    }
                }
                free(charPerLine);
                for(j = 0; j < nbLines; j++){
                	free(lines[j]);
                }
                free(lines);
            }
        }
        
        #ifdef CHECK_MODELS
        if((sb.optionFlags & OP_TEMPO_SPECIFICITY) && (sb.optionFlags & OP_TEMPO_FUSION)){
        	printf("debug: CHECK MODELS FAIL!\n");
        	printf("debug: Two types of temporization are applied at the same time... It should not!\n");
        	printf("debug: The behavior may be very strange!\n");
        } 
        #endif
        
        /*Deallocate: */
        free(charsPerFile);
        for(i = 0; i<nbFiles; i++){
            free(filenames[i]);
        }
        free(filenames);
        free(temp);
        free(temp2);
    }
    #ifdef DEBUG
    else{
        printf("%s is not a directory.\n", path);
    }
    #endif
    
    #ifdef CHECK_MODELS
    for(i = 0; i < sb.nbFocal; i++){
    	for(j = 0; j < sb.beliefOnElements[i].nbPts; j++){
    		projection.nbFocals = sb.nbFocal;
    		projection.focals = malloc(sizeof(BF_FocalElement) * projection.nbFocals);
    		if(projection.focals == NULL){
    			printf("debug: malloc failed in BFS_loadSensorBeliefs() for \"projection.focals\".\n");
    		}
    		projection.elementSize = rl.card;
  			
  			fakeMeasure = sb.beliefOnElements[i].points[j].sensorValue;
   
			for(k = 0; k < projection.nbFocals; k++){
				projection.focals[k] = BFS_getBeliefValue(sb.beliefOnElements[k], fakeMeasure, rl.card);
			}
    		
    		if(BF_checkSum(projection)){
    			printf("debug: MODEL CHECKING = FAIL!\n");
    			printf("debug: One thing doesn't equal 1 where it should...\n");
    			printf("debug: A wrong model has been given in %s\n", path);
    			printf("debug: The error seems to appear for the sensor value %f\n", fakeMeasure);
    		}
    		
    		BF_freeBeliefFunction(&projection);
    	}
    }
    #endif

    return sb;
}

BFS_PartOfBelief BFS_loadPartOfBelief(const char* fileName, const Sets_ReferenceList rl){
    BFS_PartOfBelief pob = {{NULL, 0}, NULL, 0};
    int i = 0, nbElements = 0, nbLines = 0;
    int* charsPerLin = 0;
    char** elements = NULL;
    char** lines = NULL;

    if(ReadDir_isFile(fileName)){
        /*Read the file: */
        nbLines = ReadFile_countLines(fileName);
        charsPerLin = ReadFile_charsPerLine(fileName, nbLines);
        lines = ReadFile_readLines(fileName, nbLines, charsPerLin);
        /*Nb of elements: */
        nbElements = atoi(lines[0]);
        /*Nb of pts: */
        pob.nbPts = atoi(lines[1 + nbElements]);
        /*Create element: */
        elements = malloc(sizeof(char*)*nbElements);
        DEBUG_CHECK_MALLOC(elements);

        for(i = 0; i<nbElements; i++){
            elements[i] = lines[i + 1];
        }
        pob.focalElement = Sets_createElementFromStrings((const char* const * const)elements, nbElements, rl);
        /*Create pts: */
        pob.points = malloc(sizeof(BFS_Point)*pob.nbPts);
        DEBUG_CHECK_MALLOC(pob.points);

        for(i = 0; i<pob.nbPts; i++){
            sscanf(lines[2+nbElements+i], "%f %f", &(pob.points[i].sensorValue),&(pob.points[i].belief));
        }
        /*Deallocate: */
        free(charsPerLin);
        for(i = 0; i<nbLines; i++){
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
    for(i = 0; i < pob.nbPts - 1; i++){
    	if(!(pob.points[i].sensorValue < pob.points[i+1].sensorValue)){
    		printf("debug: MODEL CHECKING = FAIL!\n");
    		printf("debug: The values should be given in the right order in the beliefs from sensors models...\n");
    		printf("debug: THe problem is in the file %s\n", fileName);
    	}
    }
    #endif

    return pob;
}

/** @} */

/**
 * @name Creation of belief functions
 * @{
 */

BF_BeliefFunction* BFS_getEvidence(const BFS_BeliefStructure bs, const char* const * const sensorTypes, const double* sensorMeasures, const int nbMeasures){
    BF_BeliefFunction* evidences = NULL;
    int i = 0, j = 0, in = 0;

    /*Memory allocation: */
    evidences = malloc(sizeof(BF_BeliefFunction) * nbMeasures);
    DEBUG_CHECK_MALLOC(evidences);

    /*Get the functions: */
    for(i = 0; i<nbMeasures; i++){
    	in = 0;
        for(j = 0; j<bs.nbSensors; j++){
            if(strcmp(bs.beliefs[j].sensorType, sensorTypes[i]) == 0){
                evidences[i] = BFS_getProjection(bs.beliefs[j], sensorMeasures[i], bs.refList.card);
                in = 1;
            }
        }
        if(!in){
        	evidences[i] = BF_getVacuousBeliefFunction(bs.refList.card);
        }
    }

    return evidences;
}

BF_BeliefFunction BFS_getProjection(const BFS_SensorBeliefs sb, const double sensorMeasure, const int elementSize){
    BF_BeliefFunction projection = {NULL, 0, 0};
    BF_BeliefFunction temp = {NULL, 0, 0};
    BF_BeliefFunction noMeasure = {NULL, 0, 0};
    double modifiedMeasure = 0;
    int parameterIndex = 0;
    int i = 0;
    #ifdef CHECK_SUM
    int l = 0;
    #endif
	
	if(sensorMeasure != NO_MEASURE){
		/*Memory allocation: */
		projection.nbFocals = sb.nbFocal;
		projection.focals = malloc(sizeof(BF_FocalElement) * sb.nbFocal);
		DEBUG_CHECK_MALLOC(projection.focals);

		projection.elementSize = elementSize;
		/*
		 * Apply variation option if required :
		 */
		if(sb.optionFlags & OP_VARIATION){
			/*Find the option index: */
			for(i = 0; i < sb.nbOptions; i++){
		        if(sb.options[i].type & OP_VARIATION){
		            parameterIndex = i;
		        }
		    }
		    /*Modify measure = average variation from previous measures: */
		    for(i = 0; i < sb.options[parameterIndex].parameter; i++){
		        modifiedMeasure += sensorMeasure - sb.options[parameterIndex].util[i].measure;
		    }
		    modifiedMeasure /= sb.options[parameterIndex].parameter;
		    /*Save measure: */
		    for(i = 1; i < sb.options[parameterIndex].parameter; i++){
		        sb.options[parameterIndex].util[i].measure = sb.options[parameterIndex].util[i-1].measure;
		    }
		    sb.options[parameterIndex].util[0].measure = sensorMeasure;
		}
		else{
			modifiedMeasure = sensorMeasure;
		}
		
		/*
		 * Get the projection :
		 */
		for(i = 0; i < projection.nbFocals; i++){
		    projection.focals[i] = BFS_getBeliefValue(sb.beliefOnElements[i], modifiedMeasure, elementSize);
		}
	}
	else {
		projection = BF_getVacuousBeliefFunction(elementSize);
	}
    
    /*
     * Apply the temporization if required :
     */
    if(sb.optionFlags & OP_TEMPO_SPECIFICITY){
        /*Get the option index: */
        for(i = 0; i < sb.nbOptions; i++){
            if(sb.options[i].type & OP_TEMPO_SPECIFICITY){
                parameterIndex = i;
            }
        }
        
        /*If not first measure: */
        if(sb.options[parameterIndex].util[1].bf.focals != NULL){
            temp = BFS_temporization_specificity(sb.options[parameterIndex].util[1].bf, projection, 
                                     sb.options[parameterIndex].parameter, sb.options[parameterIndex].util[0].time, 
                                     &(sb.options[parameterIndex]));
            BF_freeBeliefFunction(&projection);
            projection = temp;
        /*First measure: */
        }else{
            sb.options[parameterIndex].util[1].bf = BF_copyBeliefFunction(projection);
            clock_gettime(CLOCK_ID, &(sb.options[parameterIndex].util[0].time));
        }
    }
    else if(sb.optionFlags & OP_TEMPO_FUSION){
    	/*Get the option index: */
        for(i = 0; i < sb.nbOptions; i++){
            if(sb.options[i].type & OP_TEMPO_FUSION){
                parameterIndex = i;
            }
        }
        
        /*If not first measure: */
        if(sb.options[parameterIndex].util[1].bf.focals != NULL){
        	if(sensorMeasure != NO_MEASURE){
		        temp = BFS_temporization_fusion(sb.options[parameterIndex].util[1].bf, projection, 
		                                 sb.options[parameterIndex].parameter, sb.options[parameterIndex].util[0].time, 
		                                 &(sb.options[parameterIndex]));
            } else {
            	temp = BFS_temporization_fusion(sb.options[parameterIndex].util[1].bf, noMeasure, 
		                                 sb.options[parameterIndex].parameter, sb.options[parameterIndex].util[0].time, 
		                                 &(sb.options[parameterIndex]));
            }
            BF_freeBeliefFunction(&projection);
            projection = temp;
        /*First measure: */
        }else{
            sb.options[parameterIndex].util[1].bf = BF_copyBeliefFunction(projection);
            clock_gettime(CLOCK_ID, &(sb.options[parameterIndex].util[0].time));
        }
    }

    #ifdef CHECK_SUM
    if(BF_checkSum(projection)){
    	printf("debug: Sensor type = %s\n", sb.sensorType);
    	printf("debug: Sensor measure = %f\n", sensorMeasure);
        printf("debug: in BFS_getProjection(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
        printf("debug: Resulting belief function:\n");
        for(i = 0; i < projection.nbFocals; i++){
        	printf("debug: ");
        	for(l = 0; l < elementSize; l++){
        		printf("%d", projection.focals[i].element.values[l]);
        	}
        	printf(" : %f\n", projection.focals[i].beliefValue);
        }
    }
    #endif
    #ifdef CHECK_VALUES
    if(BF_checkValues(projection)){
    	printf("debug: in BFS_getProjection(), at least one value is not valid!\n");
    }
    #endif

    return projection;
}

BF_FocalElement BFS_getBeliefValue(const BFS_PartOfBelief pob, const double sensorMeasure, const int elementSize){
    BF_FocalElement point = {{NULL, 0}, 0};
    int i = 0;
    float approximation = 0;

    /*Save the focal element: */
    point.element = Sets_copyElement(pob.focalElement, elementSize);
    /*If the first point is bigger: */
    if(sensorMeasure <= pob.points[0].sensorValue){
        point.beliefValue = pob.points[0].belief;
    }
    /*If the last point is smaller: */
    else if (sensorMeasure >= pob.points[pob.nbPts - 1].sensorValue){
        point.beliefValue = pob.points[pob.nbPts - 1].belief;
    }
    else {
        /*Linear approximation: */
        for(i = 0; i < pob.nbPts - 1; i++){
            if(pob.points[i].sensorValue <= sensorMeasure && sensorMeasure <= pob.points[i+1].sensorValue){
                approximation = ((pob.points[i+1].belief - pob.points[i].belief) *
                                 (sensorMeasure - pob.points[i].sensorValue) /
                                 (pob.points[i+1].sensorValue - pob.points[i].sensorValue));
                point.beliefValue = pob.points[i].belief + approximation;
                break;
            }
        }
    }

    return point;
}

/** @} */

/**
 * @name Temporizations
 * @{
 */


BF_BeliefFunction BFS_temporization_specificity(const BF_BeliefFunction oldOne, const BF_BeliefFunction newOne, const float timeFactor, const struct timespec oldTime, BFS_Option* op){
    float alpha = 0, timeDiff = 0;
    BF_BeliefFunction temp = {NULL, 0, 0};
    BF_BeliefFunction result = {NULL, 0, 0};
    struct timespec newTime;
    #ifdef DEBUG
    int error = 0;
    
    /*Get the new time: */
    error = clock_gettime(CLOCK_ID, &newTime);
    if(error){
    	switch(errno){
    		case EFAULT: 
    			printf("debug: In BFS_temporization_specificity(), the clock had a problem...\n");
    			printf("debug: \"tp points outside the accessible address space\"\n");
    			break;
    		case EINVAL: 
    			printf("debug: In BFS_temporization_specificity(), the clock had a problem...\n");
    			printf("debug: \"The clk_id specified is not supported on this system.\"\n");
    		break;
    	}
    }
    #endif
    
    /*Compute the alpha factor:     */   
    timeDiff = (newTime.tv_sec + (float)newTime.tv_nsec/1000000000) - (oldTime.tv_sec + (float)oldTime.tv_nsec/1000000000);
    alpha = timeDiff / timeFactor;
    /*Discount: */
    temp = BF_discounting(oldOne, alpha);
    /*Compare specificity: */
    if(BF_specificity(newOne) > BF_specificity(temp)){
        BF_freeBeliefFunction(&(op->util[1].bf));
        op->util[1].bf = BF_copyBeliefFunction(newOne);
        op->util[0].time = newTime;
        result = BF_copyBeliefFunction(newOne);
    }
    else {
        result = BF_copyBeliefFunction(temp);
    }
    BF_freeBeliefFunction(&temp);
    
    return result;
}

BF_BeliefFunction BFS_temporization_fusion(const BF_BeliefFunction oldOne, const BF_BeliefFunction newOne, const float timeFactor, const struct timespec oldTime, BFS_Option* op){
	float alpha = 0, timeDiff = 0;
    BF_BeliefFunction temp = {NULL, 0, 0};
    BF_BeliefFunction result = {NULL, 0, 0};
    struct timespec newTime;
    #ifdef DEBUG
    int error = 0;
    
    /*Get the new time: */
    error = clock_gettime(CLOCK_ID, &newTime);
    if(error){
    	switch(errno){
    		case EFAULT: 
    			printf("debug: In BFS_temporization_specificity(), the clock had a problem...\n");
    			printf("debug: \"tp points outside the accessible address space\"\n");
    			break;
    		case EINVAL: 
    			printf("debug: In BFS_temporization_specificity(), the clock had a problem...\n");
    			printf("debug: \"The clock_id specified is not supported on this system.\"\n");
    		break;
    	}
    }
    #endif
    
    /*Compute the alpha factor:   */     
    timeDiff = (newTime.tv_sec + (float)newTime.tv_nsec/1000000000) - (oldTime.tv_sec + (float)oldTime.tv_nsec/1000000000);
    alpha = timeDiff / timeFactor;
    /*If the new one corresponds to a loss of evidence:*/
    if(newOne.focals == NULL){
    	return BF_discounting(oldOne, alpha);
    }
    /*Discount: */
    temp = BF_discounting(oldOne, alpha);
    /*Fuse the discounted old one with the new one: */
    result = BF_DuboisPradeCombination(temp, newOne);
    /*Clean as combination may create multiple elements...*/
    /*BF_cleanBeliefFunction(&result);*/
    /*Save: */
    BF_freeBeliefFunction(&(op->util[1].bf));
    op->util[1].bf = BF_copyBeliefFunction(result);
    op->util[0].time = newTime;
    BF_freeBeliefFunction(&temp);
    
    return result;
}


/** @} */

/**
 * @name Memory deallocation
 * @{
 */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !!! Deallocate memory given to beliefs !!!
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

void BFS_freeBeliefStructure(BFS_BeliefStructure* bs){
    int i = 0;

    free(bs->frameName);

    Sets_freeReferenceList(&(bs->refList));
    Sets_freeSet(&(bs->possibleValues));
    Sets_freeSet(&(bs->powerset));
    for(i = 0; i<bs->nbSensors; i++){
        BFS_freeSensorBeliefs(&(bs->beliefs)[i]);
    }
    free(bs->beliefs);
}

void BFS_freeOption(BFS_Option* o){
    if((o->type & OP_TEMPO_SPECIFICITY) || (o->type & OP_TEMPO_FUSION)){
    	BF_freeBeliefFunction(&(o->util[1].bf));
    }
    free(o->util);
}

void BFS_freeSensorBeliefs(BFS_SensorBeliefs* sb){
    int i = 0;

    free(sb->sensorType);
    for(i = 0; i<sb->nbFocal; i++){
        BFS_freePartOfBelief(&(sb->beliefOnElements[i]));
    }
    free(sb->beliefOnElements);
    for(i = 0; i<sb->nbOptions; i++){
        BFS_freeOption(&(sb->options[i]));
    }
    free(sb->options);
}

void BFS_freePartOfBelief(BFS_PartOfBelief* pob){
    free(pob->points);
    Sets_freeElement(&(pob->focalElement));
}

/** @} */

/**
 * @name Conversion to string
 * @{
 */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !!! Conversion into strings !!!
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */


char* BFS_partOfBeliefToString(const BFS_PartOfBelief pob, const Sets_ReferenceList rl){
    char temp[MAX_STR_LEN];
    char *elem = NULL, *str = NULL;
    int i = 0, totChar = 0;

    /*Get the element string: */
    elem = Sets_elementToString(pob.focalElement, rl);
    /*Counts number of chars required: */
    totChar += 16 + strlen(elem);
    for(i = 0; i<pob.nbPts; i++){
        sprintf(temp, " - (%f, %f)\n", pob.points[i].sensorValue, pob.points[i].belief);
        totChar += strlen(temp);
    }

    /*Fill the string: */
    str = malloc(sizeof(char)*(totChar+1));
    DEBUG_CHECK_MALLOC_OR_RETURN(str, NULL);

    sprintf(str, "Focal: %s\nPoints:\n", elem);
    for(i = 0; i<pob.nbPts; i++){
        sprintf(temp, " - (%f, %f)\n", pob.points[i].sensorValue, pob.points[i].belief);
        strcat(str, temp);
    }
    /*Add the end of str char: */
    strcat(str, "\0");

    /*Deallocate: */
    free(elem);

    return str;
}

char* BFS_optionToString(const BFS_Option o){
    char* str = NULL;
    char temp[MAX_STR_LEN];

    if(o.type & OP_VARIATION){
    	sprintf(temp, "Variation (%f)", o.parameter);
    }
    else if(o.type & OP_TEMPO_SPECIFICITY){
    	sprintf(temp, "Tempo-specificity (%f)", o.parameter);
    }
    else if(o.type & OP_TEMPO_FUSION){
    	sprintf(temp, "Tempo-fusion (%f)", o.parameter);
    }
    str = malloc(sizeof(char)*strlen(temp)+1);
    DEBUG_CHECK_MALLOC_OR_RETURN(str, NULL);

    strcpy(str, temp);
    strcat(str, "\0");

    return str;
}

char* BFS_sensorBeliefsToString(const BFS_SensorBeliefs sb, const Sets_ReferenceList rl){
    char* str = NULL;
    char **pobStr = NULL, **opStr = NULL;
    int i = 0, totChar = 0;
    char* separator = "---------------------\n";

    /*Get the strings of parts of belief: */
    pobStr = malloc(sizeof(char*)*sb.nbFocal);
    DEBUG_CHECK_MALLOC_OR_RETURN(pobStr, NULL);

    for(i = 0; i<sb.nbFocal; i++){
        pobStr[i] = BFS_partOfBeliefToString(sb.beliefOnElements[i], rl);
        if(pobStr[i] != NULL){
            totChar += strlen(pobStr[i]);
        }
    }
    /*Get the strings of options: */
    if(sb.nbOptions){
        opStr = malloc(sizeof(char*)*sb.nbOptions);
        DEBUG_CHECK_MALLOC_OR_RETURN(opStr, NULL);

        for(i = 0; i<sb.nbOptions; i++){
            opStr[i] = BFS_optionToString(sb.options[i]);
            if(opStr[i] != NULL){
                totChar += strlen(opStr[i]);
            }
        }
    }
    else{
    	totChar += 5;
    }
    /*Count total char: */
    totChar += 25 + strlen(sb.sensorType) + sb.nbFocal - 1 + 3*strlen(separator);

    /*Fill the string: */
    str = malloc(sizeof(char)*(totChar+1));
    DEBUG_CHECK_MALLOC_OR_RETURN(str, NULL);

    /*Header: */
    sprintf(str, "%sSensor type: %s\n%sOptions:\n", separator, sb.sensorType, separator);
    /*Options: */
    for(i = 0; i<sb.nbOptions; i++){
        strcat(str, opStr[i]);
        strcat(str, "\n");
    }
    if(!sb.nbOptions){
    	strcat(str, "none\n");
    }
    strcat(str, separator);
    /*Parts of belief: */
    for(i = 0; i<sb.nbFocal; i++){
        strcat(str, pobStr[i]);
        if(i != sb.nbFocal - 1){
            strcat(str, "\n");
        }
    }
    /*Add the end of str char: */
    strcat(str, "\0");

    /*Deallocate: */
    for(i = 0; i<sb.nbFocal; i++){
        free(pobStr[i]);
    }
    free(pobStr);
    for(i = 0; i<sb.nbOptions; i++){
        free(opStr[i]);
    }
    free(opStr);

    return str;
}

char* BFS_beliefStructureToString(const BFS_BeliefStructure bs){
    char* str = NULL, *set = NULL, *powerset = NULL;
    char** sbStr = NULL;
    int i = 0, totChar = 0;

    /*Get the strings of sensor beliefs: */
    sbStr = malloc(sizeof(char*)*bs.nbSensors);
    DEBUG_CHECK_MALLOC_OR_RETURN(sbStr, NULL);

    for(i = 0; i<bs.nbSensors; i++){
        sbStr[i] = BFS_sensorBeliefsToString(bs.beliefs[i], bs.refList);
        if(sbStr[i] != NULL){
            totChar += strlen(sbStr[i]);
        }
    }
    /*Get the other strings: */
    set = Sets_setToString(bs.possibleValues, bs.refList);
    powerset = Sets_setToString(bs.powerset, bs.refList);
    /*Count the nb of chars: */
    totChar += strlen(set) + strlen(powerset) + strlen(bs.frameName) + bs.nbSensors - 1 + 50;

    /*Fill the string: */
    str = malloc(sizeof(char)*(totChar+1));
    DEBUG_CHECK_MALLOC_OR_RETURN(str, NULL);

    sprintf(str, "Context attribute:\n%s\nPossible values:\n%s\nPowerset:\n%s\n\n", bs.frameName, set, powerset);
    for(i = 0; i<bs.nbSensors; i++){
        strcat(str, sbStr[i]);
        if(i != bs.nbSensors - 1){
            strcat(str, "\n");
        }
    }
    /*Add the end of str char: */
    strcat(str, "\0");

    /*Deallocate: */
    for(i = 0; i<bs.nbSensors; i++){
        free(sbStr[i]);
    }
    free(sbStr);
    free(set);
    free(powerset);

    return str;
}

/** @} */

