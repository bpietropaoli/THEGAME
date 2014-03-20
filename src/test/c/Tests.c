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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "Tests.h"
#include "config.h"
#include "BeliefsFromSensors.h"
#include "BeliefDecisions.h"

/**
 * @file Tests.c
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief TESTS: Run runTests() to test that everything works fine on your configuration.
 */


void Tests_typicalProblem(BFS_BeliefStructure bs, char** sensorTypes, double* sensorMeasures, int nbSensors, int write){
    BF_BeliefFunction bf2;
    BF_BeliefFunction *evidences;
    int i = 0, j = 0;
    char *str = NULL, *str2 = NULL;
    float* conflict = NULL;
    FILE* f = NULL;
	
	if(write){
		f = fopen("data/results/bench-data.txt", "a");
		if(f == NULL){
            printf("Cannot open the file data/results/bench-data.txt...\n");
			exit(EXIT_FAILURE);
		}

		/*Get the evidences: */
		evidences = BFS_getEvidence(bs, (const char* const * const)sensorTypes, sensorMeasures, nbSensors);
		fprintf(f, "\n\n****\n* Evidences\n****\n");
		bf2 = BF_fullDempsterCombination(evidences, nbSensors);
		str = BF_beliefFunctionToString(bf2, bs.refList);
		fprintf(f, "Combination results:\n%s\n", str);
		free(str);
		BF_freeBeliefFunction(&bf2);
		/*Double the number of combinations: */
		bf2 = BF_fullDempsterCombination(evidences, nbSensors);
		BF_freeBeliefFunction(&bf2);
		/*Computations on evidences: */
		for(j = 0; j<nbSensors; j++){
		    str = BF_beliefFunctionToString(evidences[j], bs.refList);
		    fprintf(f, "Belief function:\n%s\n", str);
		    free(str);
		    fprintf(f, "Function specific:\n\n");
		    fprintf(f, "Conf(m, E) = %f\n", BF_globalDistance(evidences[j], evidences, nbSensors));
		    fprintf(f, "Specificity : %f\n", BF_specificity(evidences[j]));
		    fprintf(f, "Non Specificity : %f\n", BF_nonSpecificity(evidences[j]));
		    fprintf(f, "Discrepancy : %f\n", BF_discrepancy(evidences[j]));

		    bf2 = BF_conditioning(evidences[j], bs.powerset.elements[bs.powerset.card/2], bs.powerset);
		    str = BF_beliefFunctionToString(bf2, bs.refList);
		    str2 = Sets_elementToString(bs.powerset.elements[bs.powerset.card/2], bs.refList);
		    fprintf(f, "Conditioning:\nElement:%s\nResult:\n%s\n", str2, str);
		    free(str);
		    free(str2);
		    BF_freeBeliefFunction(&bf2);

		    conflict = BF_autoConflict(evidences[j], 5);
		    for(i = 0; i<5; i++){
		        fprintf(f, "Degree %d : autoConflict(m) = %f\n", (i+1), conflict[i]);
		    }
		    free(conflict);

		    fprintf(f, "\nFunction AND element specific:\n");
		    for(i = 0; i<evidences[j].nbFocals; i++){
		        str = Sets_elementToString(evidences[j].focals[i].element, bs.refList);
		        fprintf(f, "\nbel(%s) = %f\n", str, BF_bel(evidences[j], evidences[j].focals[i].element));
		        fprintf(f, "betP(%s) = %f\n", str, BF_betP(evidences[j], evidences[j].focals[i].element));
		        fprintf(f, "pl(%s) = %f\n", str, BF_pl(evidences[j], evidences[j].focals[i].element));
		        fprintf(f, "q(%s) = %f\n", str, BF_q(evidences[j], evidences[j].focals[i].element));
		        free(str);
		        str = NULL;
		    }
		    fprintf(f, "-----------------\n");
		}

		/*Deallocation: */
		for(i = 0; i<nbSensors; i++){
		    BF_freeBeliefFunction(&(evidences[i]));
		}
		free(evidences);
		fclose(f);
	}
    else{
		/*Get the evidences: */
		evidences = BFS_getEvidence(bs, (const char* const * const)sensorTypes, sensorMeasures, nbSensors);
		bf2 = BF_fullDempsterCombination(evidences, nbSensors);
		BF_freeBeliefFunction(&bf2);
		/*Double the number of combinations: */
		bf2 = BF_fullDempsterCombination(evidences, nbSensors);
		BF_freeBeliefFunction(&bf2);
		/*Computations on evidences: */
		for(j = 0; j<nbSensors; j++){
		    BF_globalDistance(evidences[j], evidences, nbSensors);
		    BF_specificity(evidences[j]);
		    BF_nonSpecificity(evidences[j]);
		    BF_discrepancy(evidences[j]);

		    bf2 = BF_conditioning(evidences[j], bs.powerset.elements[bs.powerset.card/2], bs.powerset);
		    BF_freeBeliefFunction(&bf2);

		    conflict = BF_autoConflict(evidences[j], 5);
		    free(conflict);

		    for(i = 0; i<evidences[j].nbFocals; i++){
		        BF_bel(evidences[j], evidences[j].focals[i].element);
		        BF_betP(evidences[j], evidences[j].focals[i].element);
		        BF_pl(evidences[j], evidences[j].focals[i].element);
		        BF_q(evidences[j], evidences[j].focals[i].element);
		    }
		}

		/*Deallocation: */
		for(i = 0; i<nbSensors; i++){
		    BF_freeBeliefFunction(&(evidences[i]));
		}
		free(evidences);
    }
}








void Tests_beliefsFromRandomness(int nbIterations){
	char str[512];
	int i = 0, j = 0, k = 0;
	BF_BeliefFunction bf;
	int* density = NULL;
	FILE* f = NULL;
	int sum = 0;
	
	density = malloc(sizeof(int) * 101);
	
	for(j = 2; j < 11; j++){
		for(i = 0; i < 101; i++){
			density[i] = 0;
		}
		
		for(i = 0; i < nbIterations; i++){
			bf = BFR_getCrappyRandomBelief(j);
			for(k = 0; k < bf.nbFocals; k++){
				density[(int)(bf.focals[k].beliefValue * 100)]++;
			}
			BF_freeBeliefFunction(&bf);
		}
		
		for(i = 0; i < 100; i++){
			sum += density[i];
		}
		
		sprintf(str, "./data/results/Random-%d", j);
		f = fopen(str, "w");
		for(i = 0; i < 101; i++){
			fprintf(f, "%f;%f\n", i*0.01, (float)density[i]/sum);
		}
		fclose(f);
		
		printf("Number of possible worlds: %d -> Done!\n", j);
	}
	
	free(density);
}








void Tests_beliefsFromSensors(){
	BFS_SensorBeliefs sb;
    BFS_PartOfBelief pob;
    BFS_BeliefStructure bs;
    BF_FocalElement bp;
    BF_BeliefFunction bf;
    BF_BeliefFunction *evidences;
    int i = 0;
    char *str = NULL, *str2 = NULL;
    FILE* f = NULL;
    double* sensorMeasures = NULL;
    char** sensorTypes = NULL;
    
	f = fopen("./data/results/beliefsFromSensors.txt", "w");
	if(f == NULL){
        printf("Cannot open the file data/results/beliefsFromSensors.txt...\n");
        exit(EXIT_FAILURE);
    }
	
	fprintf(f,"+-----------------------------------------------+\n");
	fprintf(f,"|+---------------------------------------------+|\n");
	fprintf(f,"||                                             ||\n");
	fprintf(f,"||  TEST OF THE MODULE : BeliefsFromSensors !  ||\n");
	fprintf(f,"||                                             ||\n");
	fprintf(f,"|+---------------------------------------------+|\n");
	fprintf(f,"+-----------------------------------------------+\n");
	fprintf(f,"\n\n");

    fprintf(f,"*******************************\n");
    fprintf(f,"Test of belief structure loading:\n");
    fprintf(f,"*******************************\n");
    /*Loading: */
    bs = BFS_loadBeliefStructure(BFS_PATH, "presence");

    /*Printing: */
    str = BFS_beliefStructureToString(bs);
    fprintf(f,"%s\n", str);
    free(str);
    str = NULL;

    /*Freeing: */
    BFS_freeBeliefStructure(&bs);
    fprintf(f,"\n\n");

    fprintf(f,"*******************************\n");
    fprintf(f,"Test of getBeliefValue:\n");
    fprintf(f,"*******************************\n");
    /*Loading a belief structure: */
    bs = BFS_loadBeliefStructure(BFS_PATH, "presence");
    /*Print a part of belief: */
    pob = bs.beliefs[0].beliefOnElements[0];
    str = BFS_partOfBeliefToString(pob, bs.refList);
    fprintf(f,"%s\n", str);
    free(str);
    str = NULL;
    /*Test the projection: */
    bp = BFS_getBeliefValue(pob, 30, bs.refList.card);
    str = Sets_elementToString(bp.element, bs.refList);
    fprintf(f,"Sensor value: 30\n");
    fprintf(f,"Element: %s\nBelief value: %f\n\n", str, bp.beliefValue);
    free(str);
    BF_freeBeliefPoint(&bp);
    str = NULL;
    /*Test the projection: */
    bp = BFS_getBeliefValue(pob, 186, bs.refList.card);
    str = Sets_elementToString(bp.element, bs.refList);
    fprintf(f,"Sensor value: 186\n");
    fprintf(f,"Element: %s\nBelief value: %f\n\n", str, bp.beliefValue);
    free(str);
    BF_freeBeliefPoint(&bp);
    str = NULL;
    /*Test the projection: */
    bp = BFS_getBeliefValue(pob, 225, bs.refList.card);
    str = Sets_elementToString(bp.element, bs.refList);
    fprintf(f,"Sensor value: 225\n");
    fprintf(f,"Element: %s\nBelief value: %f\n\n", str, bp.beliefValue);
    free(str);
    BF_freeBeliefPoint(&bp);
    str = NULL;
    /*Test the projection: */
    bp = BFS_getBeliefValue(pob, 240, bs.refList.card);
    str = Sets_elementToString(bp.element, bs.refList);
    fprintf(f,"Sensor value: 240\n");
    fprintf(f,"Element: %s\nBelief value: %f\n\n", str, bp.beliefValue);
    free(str);
    BF_freeBeliefPoint(&bp);
    str = NULL;
    /*Test the projection: */
    bp = BFS_getBeliefValue(pob, 275, bs.refList.card);
    str = Sets_elementToString(bp.element, bs.refList);
    fprintf(f,"Sensor value: 275\n");
    fprintf(f,"Element: %s\nBelief value: %f\n\n", str, bp.beliefValue);
    free(str);
    BF_freeBeliefPoint(&bp);
    str = NULL;
    /*Test the projection: */
    bp = BFS_getBeliefValue(pob, 300, bs.refList.card);
    str = Sets_elementToString(bp.element, bs.refList);
    fprintf(f,"Sensor value: 300\n");
    fprintf(f,"Element: %s\nBelief value: %f\n\n", str, bp.beliefValue);
    free(str);
    BF_freeBeliefPoint(&bp);
    str = NULL;
    /*Test the projection: */
    bp = BFS_getBeliefValue(pob, 350, bs.refList.card);
    str = Sets_elementToString(bp.element, bs.refList);
    fprintf(f,"Sensor value: 350\n");
    fprintf(f,"Element: %s\nBelief value: %f\n\n", str, bp.beliefValue);
    free(str);
    BF_freeBeliefPoint(&bp);
    str = NULL;

    fprintf(f, "\n\n");


    fprintf(f,"*******************************\n");
    fprintf(f,"Test of getProjection():\n");
    fprintf(f,"*******************************\n");
    /*Print a BFS_SensorBeliefs: */
    sb = bs.beliefs[bs.nbSensors - 1];
    str = BFS_sensorBeliefsToString(sb, bs.refList);
    fprintf(f, "%s\n", str);
    free(str);
    str = NULL;
    /*Test the complete projection: */
    bf = BFS_getProjection(sb, 30, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Sensor value: 30\n");
    fprintf(f, "Belief function:\n%s\n", str);
    BF_freeBeliefFunction(&bf);
    free(str);
    str = NULL;
    /*Test the complete projection: */
    bf = BFS_getProjection(sb, 134.89, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Sensor value: 134.89\n");
    fprintf(f, "Belief function:\n%s\n", str);
    BF_freeBeliefFunction(&bf);
    free(str);
    str = NULL;
    /*Test the complete projection: */
    bf = BFS_getProjection(sb, 172.6459, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Sensor value: 172.6459\n");
    fprintf(f, "Belief function:\n%s\n", str);
    BF_freeBeliefFunction(&bf);
    free(str);
    str = NULL;
    /*Test the complete projection: */
    bf = BFS_getProjection(sb, 184.65648, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Sensor value: nbIterations\n");
    fprintf(f, "Belief function:\n%s\n", str);
    BF_freeBeliefFunction(&bf);
    free(str);
    str = NULL;
    /*Test the complete projection: */
    bf = BFS_getProjection(sb, 225, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Sensor value: 225\n");
    fprintf(f, "Belief function:\n%s\n", str);
    BF_freeBeliefFunction(&bf);
    free(str);
    str = NULL;
    /*Test the complete projection: */
    bf = BFS_getProjection(sb, 240, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Sensor value: 240\n");
    fprintf(f, "Belief function:\n%s\n", str);
    BF_freeBeliefFunction(&bf);
    free(str);
    str = NULL;
    /*Test the complete projection: */
    bf = BFS_getProjection(sb, 275, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Sensor value: 275\n");
    fprintf(f, "Belief function:\n%s\n", str);
    BF_freeBeliefFunction(&bf);
    free(str);
    str = NULL;
    /*Test the complete projection: */
    bf = BFS_getProjection(sb, 282.34, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Sensor value: 282.34\n");
    fprintf(f, "Belief function:\n%s\n", str);
    BF_freeBeliefFunction(&bf);
    free(str);
    str = NULL;
    /*Test the complete projection: */
    bf = BFS_getProjection(sb, 300, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Sensor value: 300\n");
    fprintf(f, "Belief function:\n%s\n", str);
    BF_freeBeliefFunction(&bf);
    free(str);
    str = NULL;
    /*Test the complete projection: */
    bf = BFS_getProjection(sb, 350, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Sensor value: 350\n");
    fprintf(f, "Belief function:\n%s\n", str);
    BF_freeBeliefFunction(&bf);
    free(str);
    str = NULL;

    fprintf(f, "\n\n");


    fprintf(f,"*******************************\n");
    fprintf(f,"Test of getEvidence():\n");
    fprintf(f,"*******************************\n");
    /*Allocation: */
    sensorTypes = malloc(sizeof(char*)*3);
    sensorTypes[0] = "CO2";
    sensorTypes[1] = "sound";
    sensorTypes[2] = "motion";
    sensorMeasures = malloc(sizeof(double)*3);
    sensorMeasures[0] = 212.76;
    sensorMeasures[1] = 234.76;
    sensorMeasures[2] = 145.76;
    evidences = BFS_getEvidence(bs, (const char* const * const)sensorTypes, sensorMeasures, 3);
    /*Printing: */
    for(i = 0; i<3; i++){
        str = BF_beliefFunctionToString(evidences[i], bs.refList);
        str2 = BFS_sensorBeliefsToString(bs.beliefs[i], bs.refList);
        fprintf(f, "Belief model:\n%s\n***Projection***\nMeasures: \n%s:%f\n%s:%f\n%s:%f\n%s\n\n\n", str2, sensorTypes[0], sensorMeasures[0], sensorTypes[1], sensorMeasures[1], sensorTypes[2], sensorMeasures[2], str);
        free(str);
        free(str2);
        str = NULL;
        str2 = NULL;
    }

    fprintf(f, "\n\n");
    /*Deallocation: */
    free(sensorTypes);
    free(sensorMeasures);
    for(i = 0; i<3; i++){
        BF_freeBeliefFunction(&evidences[i]);
    }
    free(evidences);

    /*Freeing: */
    BFS_freeBeliefStructure(&bs);
    
    fclose(f);
}








void Tests_beliefsFromSensorsOptions(){
	BFS_SensorBeliefs sb;
    BFS_BeliefStructure bs;
    BF_BeliefFunction bf;
    int i = 0, j = 0;
    char *str = NULL;
    FILE* f = NULL;
	
	
	f = fopen("./data/results/beliefsFromSensors-options.txt", "w");
	if(f == NULL){
        printf("Cannot open the file data/results/beliefsFromSensors-options.txt...\n");
        exit(EXIT_FAILURE);
    }

    fprintf(f, "\n\n*******************************\n");
    fprintf(f, "*******************************\n");
    fprintf(f, "**      Test of options:     **\n");
    fprintf(f, "*******************************\n");
    fprintf(f, "*******************************\n\n");
    bs = BFS_loadBeliefStructure(BFS_PATH, "optionTest");
    str = BFS_beliefStructureToString(bs);
    fprintf(f, "%s\n", str);
    free(str);
    
    printf("Tests of variation option...");

    fprintf(f, "\n*******************************\n");
    fprintf(f, "Test of variation option:\n");
    fprintf(f, "*******************************\n");
    for(i = 0; i < bs.nbSensors; i++){
        if(!strcmp(bs.beliefs[i].sensorType, "variation")){
            j = i;
        }
    }
    sb = bs.beliefs[j];

    bf = BFS_getProjection(sb, 30, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 30\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    bf = BFS_getProjection(sb, 140, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 140\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    bf = BFS_getProjection(sb, 328, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 328\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    bf = BFS_getProjection(sb, 0, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 0\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    printf(" done.\n");
    
    
    printf("Tests of tempo(-specificity) option...");

    fprintf(f, "\n*******************************\n");
    fprintf(f, "Test of tempo(-specificity) option:\n");
    fprintf(f, "*******************************\n");
    for(i = 0; i < bs.nbSensors; i++){
        if(!strcmp(bs.beliefs[i].sensorType, "tempo")){
            j = i;
        }
    }
    sb = bs.beliefs[j];

    bf = BFS_getProjection(sb, 30, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 30\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    usleep(500000);

    bf = BFS_getProjection(sb, 140, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 140\nWaited for: 0.5s\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    usleep(800000);

    bf = BFS_getProjection(sb, 328, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 328\nWaited for: 0.8s\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    usleep(15000000);

    bf = BFS_getProjection(sb, 0, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 0\nWaited for: 1.5s\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    printf(" done.\n");
    

    printf("Tests of tempo(-fusion) option...");

    fprintf(f, "\n*******************************\n");
    fprintf(f, "Test of tempo(-fusion) option:\n");
    fprintf(f, "*******************************\n");
    for(i = 0; i < bs.nbSensors; i++){
        if(!strcmp(bs.beliefs[i].sensorType, "tempoFusion")){
            j = i;
        }
    }
    sb = bs.beliefs[j];

    bf = BFS_getProjection(sb, 30, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 30\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    usleep(500000);

    bf = BFS_getProjection(sb, 140, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 140\nWaited for: 0.5s\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    usleep(800000);

    bf = BFS_getProjection(sb, 328, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 328\nWaited for: 0.8s\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    usleep(15000000);

    bf = BFS_getProjection(sb, 0, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 0\nWaited for: 1.5s\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    printf(" done.\n");
    
    
    printf("Tests of tempo(-specificity)+variation option...");

    fprintf(f, "\n*******************************\n");
    fprintf(f, "Test of variation-tempo(-specificity) option:\n");
    fprintf(f, "*******************************\n");
    for(i = 0; i < bs.nbSensors; i++){
        if(!strcmp(bs.beliefs[i].sensorType, "tempoVariation")){
            j = i;
        }
    }
    sb = bs.beliefs[j];

    bf = BFS_getProjection(sb, 30, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 30\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    usleep(500000);

    bf = BFS_getProjection(sb, 140, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 140\nWaited for: 0.5s\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    usleep(800000);

    bf = BFS_getProjection(sb, 328, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 328\nWaited for: 0.8s\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    usleep(15000000);

    bf = BFS_getProjection(sb, 0, bs.refList.card);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Measure: 0\nWaited for: 1.5s\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    printf(" done.\n");

    BFS_freeBeliefStructure(&bs); 
	fclose(f);
}






void Tests_tempo_specificity(){
    BFS_BeliefStructure bs;
    BFS_BeliefStructure bs_temoin;
    BF_BeliefFunction bf;
    BF_BeliefFunction bf_temoin;
    int i = 0, j = 0;
    char *str = NULL, path[MAX_STR_LEN];
    FILE* f = NULL, *f3 = NULL, *f2 = NULL;
	
	f = fopen("./data/results/beliefsFromSensors-tempo-spec.txt", "w");

	if(f == NULL){
        printf("Cannot open the file data/results/beliefsFromSensors-tempo-spec.txt...\n");
        exit(EXIT_FAILURE);
    }
	fprintf(f, "\n*******************************\n");
    fprintf(f, "Test of tempo(-specificity) option:\n");
    fprintf(f, "*******************************\n");
    fprintf(f, "To see the results, check the directory ./data/results/tempo-spec/\n");
    fprintf(f, "You could also run the python script \"printResults.py\" to see the results of temporization\n more explicitly!\n");
	fclose(f);
	
	sprintf(path, "./data/results/tempo-spec/Temporization-Specificity - General behavior");
	f3 = fopen(path, "w");
	sprintf(path, "./data/results/tempo-spec-temoin/Temporization-Specificity - General behavior");
	f2 = fopen(path, "w");
	bs = BFS_loadBeliefStructure(BFS_PATH, "tempoSpec");
	bs_temoin = BFS_loadBeliefStructure(BFS_PATH, "tempoSpecTemoin");
	
	/*******************
	 * Behavior resume *
	 *******************/
	
    /*With tempo: */
	bf = BFS_getProjection(bs.beliefs[0], 0, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 0, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 0, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 0, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf_temoin);
	
	/*With tempo: */
	bf = BFS_getProjection(bs.beliefs[0], 8*10, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 1, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 8*10, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf_temoin);
			
	/* ////////////////////////////
	   // Accumulation over time //
	   //////////////////////////// */
	for(i = 0; i < 10; i++){
		usleep(250000/pow(2,2));
		bf = BFS_getProjection(bs.beliefs[0], 8*10, 2);
		for(j = 0; j < bf.nbFocals; j++){
			str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+2, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 8*10, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+2, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
		
	/* /////////////////////////////////
	   // Transition to another state //
	   ///////////////////////////////// */
	for(i = 0; i < 10; i++){
		usleep(250000/pow(2,2));
	
		bf = BFS_getProjection(bs.beliefs[0], 200 + 8*10, 2);
		for(j = 0; j < bf.nbFocals; j++){
			str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+12, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + 8*10, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+12, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
			
	/* //////////////////////////
	   // FLuctuation response //
	   ////////////////////////// */
	/*Convergence: */
	for(i = 0; i < 4; i++){
		usleep(250000/pow(2,2));
		bf = BFS_getProjection(bs.beliefs[0], 200 + 8*10, 2);
		for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+22, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + 8*10, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+22, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
	/*Fluctuation: */
	usleep(250000/pow(2,2));
	bf = BFS_getProjection(bs.beliefs[0], 4*10, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 26, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 4*10, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 26, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf_temoin);
	/*Return of the normal state: */
	for(i = 0; i < 5; i++){
		usleep(250000/pow(2,2));
		bf = BFS_getProjection(bs.beliefs[0], 200 + 8*10, 2);
		for(j = 0; j < bf.nbFocals; j++){
			str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+27, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + 8*10, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+27, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
	/* Dunno */
	for(i = 0; i < 10; i++){
		usleep(250000/pow(2,2));
		bf = BFS_getProjection(bs.beliefs[0], 0, 2);
		for(j = 0; j < bf.nbFocals; j++){
			str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+32, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 0, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+32, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
			
	fclose(f3);
	fclose(f2);
	
	
	
	/****************************
	 * Behavior - State changes *
	 ****************************/
	
	sprintf(path, "./data/results/tempo-spec/Temporization-Specificity - State changes");
	f3 = fopen(path, "w");
	sprintf(path, "./data/results/tempo-spec-temoin/Temporization-Specificity - State changes");
	f2 = fopen(path, "w");
	
    /*With tempo: */
	bf = BFS_getProjection(bs.beliefs[0], 0, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 0, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 0, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 0, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf_temoin);
	

	/*With tempo: */
	bf = BFS_getProjection(bs.beliefs[0], 8*10, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 1, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 8*10, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf_temoin);
			
	/* ////////////////////////////
	   // Accumulation over time //
	   //////////////////////////// */
	for(i = 0; i < 10; i++){
		usleep(250000/pow(2,2));
		bf = BFS_getProjection(bs.beliefs[0], 8*10, 2);
		for(j = 0; j < bf.nbFocals; j++){
			str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+2, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 8*10, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+2, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
	/* /////////////////////////////////
	   // Transition to another state //
	   ///////////////////////////////// */
	for(i = 0; i < 10; i++){
		usleep(250000/pow(2,2));
	
		bf = BFS_getProjection(bs.beliefs[0], 200 + 8*10, 2);
		for(j = 0; j < bf.nbFocals; j++){
			str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+12, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + 8*10, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+12, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
	/* /////////////////////////////////
	   // Transition to another state //
	   ///////////////////////////////// */
	for(i = 0; i < 15; i++){
		usleep(250000/pow(2,2));
	
		bf = BFS_getProjection(bs.beliefs[0], 3*10, 2);
		for(j = 0; j < bf.nbFocals; j++){
			str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+22, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 3*10, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+22, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
	fclose(f3);
	fclose(f2);
	
	
	
	
	/**************************
	 * Behavior - Fluctuation *
	 **************************/
	
	sprintf(path, "./data/results/tempo-spec/Temporization-Specificity - Fluctuation and noise");
	f3 = fopen(path, "w");
	sprintf(path, "./data/results/tempo-spec-temoin/Temporization-Specificity - Fluctuation and noise");
	f2 = fopen(path, "w");
	
	
    /*With tempo: */
	bf = BFS_getProjection(bs.beliefs[0], 0, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 0, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 0, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 0, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf_temoin);
	

	/*With tempo: */
	bf = BFS_getProjection(bs.beliefs[0], 7*10, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 1, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 7*10, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf_temoin);
			
	/* ////////////////////////////
	   // Accumulation over time //
	   //////////////////////////// */
	for(i = 0; i < 10; i++){
		usleep(250000/pow(2,2));
		bf = BFS_getProjection(bs.beliefs[0], 7*10, 2);
		for(j = 0; j < bf.nbFocals; j++){
			str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+2, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 7*10, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+2, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
			
	/* //////////////////////////
	   // FLuctuation response //
	   ////////////////////////// */
	/*Fluctuation: */
	usleep(250000/pow(2,2));
	bf = BFS_getProjection(bs.beliefs[0], 4*10, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 12, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 4*10, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 12, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	/*Fluctuation: */
	usleep(250000/pow(2,2));
	bf = BFS_getProjection(bs.beliefs[0], 3*10, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 13, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 3*10, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 13, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf_temoin);
	/*Return of the normal state: */
	for(i = 0; i < 8; i++){
		usleep(250000/pow(2,2));
		bf = BFS_getProjection(bs.beliefs[0], 7*10, 2);
		for(j = 0; j < bf.nbFocals; j++){
			str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+14, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0],  7*10, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+14, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
	/*Noisy measure: */
	usleep(250000/pow(2,2));
	bf = BFS_getProjection(bs.beliefs[0], 200 + 4*10, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 22, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + 4*10, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 22, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf_temoin);
	/*Noisy measure bis: */
	usleep(250000/pow(2,2));
	bf = BFS_getProjection(bs.beliefs[0], 200 + 3*10, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 23, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + 3*10, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 23, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf_temoin);
	/*Return of the normal state: */
	for(i = 0; i < 8; i++){
		usleep(250000/pow(2,2));
		bf = BFS_getProjection(bs.beliefs[0], 7*10, 2);
		for(j = 0; j < bf.nbFocals; j++){
			str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+24, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0],  7*10, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+24, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
	/*PIKE! */
	usleep(250000/pow(2,2));
	bf = BFS_getProjection(bs.beliefs[0], 200 + 7*10, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 32, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + 7*10, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 32, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	/*Return of the normal state: */
	for(i = 0; i < 9; i++){
		usleep(250000/pow(2,2));
		bf = BFS_getProjection(bs.beliefs[0], 7*10, 2);
		for(j = 0; j < bf.nbFocals; j++){
			str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+33, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0],  7*10, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+33, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
	/*HUGE PIKE! */
	usleep(250000/pow(2,2));
	bf = BFS_getProjection(bs.beliefs[0], 200 + 10*10, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 42, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + 10*10, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 42, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	/*Return of the normal state: */
	for(i = 0; i < 7; i++){
		usleep(250000/pow(2,2));
		bf = BFS_getProjection(bs.beliefs[0], 7*10, 2);
		for(j = 0; j < bf.nbFocals; j++){
			str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+43, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0],  7*10, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+43, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
	
	fclose(f3);
	fclose(f2);
	
	
	
	
	
	/**************************
	 * Behavior - Fluctuation *
	 **************************/
	
	sprintf(path, "./data/results/tempo-spec/Temporization-Specificity - Loss of data");
	f3 = fopen(path, "w");
	sprintf(path, "./data/results/tempo-spec-temoin/Temporization-Specificity - Loss of data");
	f2 = fopen(path, "w");
	
	
    /*With tempo: */
	bf = BFS_getProjection(bs.beliefs[0], 0, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 0, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 0, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 0, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf_temoin);
	
	/*With tempo: */
	bf = BFS_getProjection(bs.beliefs[0], 8*10, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 1, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 8*10, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf_temoin);
			
	/* ////////////////////////////
	   // Accumulation over time //
	   //////////////////////////// */
	for(i = 0; i < 10; i++){
		usleep(250000/pow(2,2));
		bf = BFS_getProjection(bs.beliefs[0], 8*10, 2);
		for(j = 0; j < bf.nbFocals; j++){
			str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+2, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 8*10, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+2, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
	
	/*Small loss */
	usleep(250000/pow(2,2));
	bf = BFS_getProjection(bs.beliefs[0], 0*10, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 12, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 0*10, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 12, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	usleep(250000/pow(2,2));
	bf = BFS_getProjection(bs.beliefs[0], 0*10, 2);
	for(j = 0; j < bf.nbFocals; j++){
		str = Sets_elementToString(bf.focals[j].element, bs.refList);
		fprintf(f3,  "%d;%s;%f\n", 13, str, BF_m(bf, bf.focals[j].element));
		free(str);
	}
	BF_freeBeliefFunction(&bf);
	/*Without tempo: */
	bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 0*10, 2);
	for(j = 0; j < bf_temoin.nbFocals; j++){
		str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
		fprintf(f2,  "%d;%s;%f\n", 13, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
		free(str);
	}
	
	/* Back to normal: */
	for(i = 0; i < 8; i++){
		usleep(250000/pow(2,2));
		bf = BFS_getProjection(bs.beliefs[0], 8*10, 2);
		for(j = 0; j < bf.nbFocals; j++){
			str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+14, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 8*10, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+14, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
	
	/* Dunno forever */
	for(i = 0; i < 10; i++){
		usleep(250000/pow(2,2));
		bf = BFS_getProjection(bs.beliefs[0], 0, 2);
		for(j = 0; j < bf.nbFocals; j++){
			str = Sets_elementToString(bf.focals[j].element, bs.refList);
			fprintf(f3,  "%d;%s;%f\n", i+22, str, BF_m(bf, bf.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf);
		/*Without tempo: */
		bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 0, 2);
		for(j = 0; j < bf_temoin.nbFocals; j++){
			str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
			fprintf(f2,  "%d;%s;%f\n", i+22, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
			free(str);
		}
		BF_freeBeliefFunction(&bf_temoin);
	}
	fclose(f3);
	fclose(f2);
	
	BFS_freeBeliefStructure(&bs);
	BFS_freeBeliefStructure(&bs_temoin);
}







void Tests_tempo_fusion(){
    BFS_BeliefStructure bs;
    BFS_BeliefStructure bs_temoin;
    BF_BeliefFunction bf;
    BF_BeliefFunction bf_temoin;
    int i = 0, j = 0, k = 0, l = 0, m = 0;
    char *str = NULL, path[MAX_STR_LEN];
    FILE* f = NULL, *f3 = NULL, *f2 = NULL;
	
	f = fopen("./data/results/beliefsFromSensors-tempo-fusion.txt", "w");
	if(f == NULL){
        printf("Cannot open the file data/results/beliefsFromSensors-tempo-fusion.txt...\n");
        exit(EXIT_FAILURE);
    }
	fprintf(f, "\n*******************************\n");
    fprintf(f, "Test of tempo(-fusion) option:\n");
    fprintf(f, "*******************************\n");
    fprintf(f, "To see the results, check the directory ./data/results/tempo-fusion/\n");
    fprintf(f, "You could also run the python script \"printResults.py\" to see the results of temporization\n more explicitly!\n");
	fclose(f);
	
	for(k = 1; k < 10; k++){
		for(l = 1; l < 5; l++){
			sprintf(path, "./data/results/tempo-fusion/R%d-B%d", (int)pow(2, l), k);
			f3 = fopen(path, "w");
			sprintf(path, "./data/results/tempo-fusion-temoin/R%d-B%d", (int)pow(2, l), k);
			f2 = fopen(path, "w");
			bs = BFS_loadBeliefStructure(BFS_PATH, "tempoFusion");
			bs_temoin = BFS_loadBeliefStructure(BFS_PATH, "tempoFusionTemoin");
			
			/*With tempo: */
			bf = BFS_getProjection(bs.beliefs[0], 0, 2);
			for(j = 0; j < bf.nbFocals; j++){
				str = Sets_elementToString(bf.focals[j].element, bs.refList);
				fprintf(f3,  "%d;%s;%f\n", 0, str, BF_m(bf, bf.focals[j].element));
				free(str);
			}
			BF_freeBeliefFunction(&bf);
			/*Without tempo: */
			bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 0, 2);
			for(j = 0; j < bf_temoin.nbFocals; j++){
				str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
				fprintf(f2,  "%d;%s;%f\n", 0, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
				free(str);
			}
			BF_freeBeliefFunction(&bf_temoin);
			
			/*With tempo: */
			bf = BFS_getProjection(bs.beliefs[0], k*10, 2);
			for(j = 0; j < bf.nbFocals; j++){
				str = Sets_elementToString(bf.focals[j].element, bs.refList);
				fprintf(f3,  "%d;%s;%f\n", 1, str, BF_m(bf, bf.focals[j].element));
				free(str);
			}
			BF_freeBeliefFunction(&bf);
			/*Without tempo: */
			bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], k*10, 2);
			for(j = 0; j < bf_temoin.nbFocals; j++){
				str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
				fprintf(f2,  "%d;%s;%f\n", 1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
				free(str);
			}
			BF_freeBeliefFunction(&bf_temoin);
			
			/* ////////////////////////////
			   // Accumulation over time //
			   //////////////////////////// */
			for(i = 0; i < 10; i++){
				usleep(250000/pow(2,l));
	
				bf = BFS_getProjection(bs.beliefs[0], k*10, 2);
				for(j = 0; j < bf.nbFocals; j++){
					str = Sets_elementToString(bf.focals[j].element, bs.refList);
					fprintf(f3,  "%d;%s;%f\n", i+2, str, BF_m(bf, bf.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf);
				/*Without tempo: */
				bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], k*10, 2);
				for(j = 0; j < bf_temoin.nbFocals; j++){
					str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
					fprintf(f2,  "%d;%s;%f\n", i+2, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf_temoin);
			}
			
			/* /////////////////////////////////
			   // Transition to another state //
			   ///////////////////////////////// */
			for(i = 0; i < 10; i++){
				usleep(250000/pow(2,l));
	
				bf = BFS_getProjection(bs.beliefs[0], 200 + k*10, 2);
				for(j = 0; j < bf.nbFocals; j++){
					str = Sets_elementToString(bf.focals[j].element, bs.refList);
					fprintf(f3,  "%d;%s;%f\n", i+12, str, BF_m(bf, bf.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf);
				/*Without tempo: */
				bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + k*10, 2);
				for(j = 0; j < bf_temoin.nbFocals; j++){
					str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
					fprintf(f2,  "%d;%s;%f\n", i+12, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf_temoin);
			}
			
			/* //////////////////////////
			   // FLuctuation response //
			   ////////////////////////// */
			/*Convergence: */
			for(i = 0; i < 4; i++){
				usleep(250000/pow(2,l));
	
				bf = BFS_getProjection(bs.beliefs[0], 200 + k*10, 2);
				for(j = 0; j < bf.nbFocals; j++){
					str = Sets_elementToString(bf.focals[j].element, bs.refList);
					fprintf(f3,  "%d;%s;%f\n", i+22, str, BF_m(bf, bf.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf);
				/*Without tempo: */
				bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + k*10, 2);
				for(j = 0; j < bf_temoin.nbFocals; j++){
					str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
					fprintf(f2,  "%d;%s;%f\n", i+22, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf_temoin);
			}
			/*Fluctuation: */
			bf = BFS_getProjection(bs.beliefs[0], k*10, 2);
			for(j = 0; j < bf.nbFocals; j++){
				str = Sets_elementToString(bf.focals[j].element, bs.refList);
				fprintf(f3,  "%d;%s;%f\n", 26, str, BF_m(bf, bf.focals[j].element));
				free(str);
			}
			BF_freeBeliefFunction(&bf);
			/*Without tempo: */
			bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], k*10, 2);
			for(j = 0; j < bf_temoin.nbFocals; j++){
				str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
				fprintf(f2,  "%d;%s;%f\n", 26, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
				free(str);
			}
			BF_freeBeliefFunction(&bf_temoin);
			/*Return of the normal state: */
			for(i = 0; i < 5; i++){
				usleep(250000/pow(2,l));
	
				bf = BFS_getProjection(bs.beliefs[0], 200 + k*10, 2);
				for(j = 0; j < bf.nbFocals; j++){
					str = Sets_elementToString(bf.focals[j].element, bs.refList);
					fprintf(f3,  "%d;%s;%f\n", i+27, str, BF_m(bf, bf.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf);
				/*Without tempo: */
				bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + k*10, 2);
				for(j = 0; j < bf_temoin.nbFocals; j++){
					str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
					fprintf(f2,  "%d;%s;%f\n", i+27, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf_temoin);
			}
			
			/* ////////////////////////////////
			   // High Perturbation response //
			   //////////////////////////////// */
			/*Convergence: */
			for(i = 0; i < 4; i++){
				usleep(250000/pow(2,l));
	
				bf = BFS_getProjection(bs.beliefs[0], 200 + k*10, 2);
				for(j = 0; j < bf.nbFocals; j++){
					str = Sets_elementToString(bf.focals[j].element, bs.refList);
					fprintf(f3,  "%d;%s;%f\n", i+32, str, BF_m(bf, bf.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf);
				/*Without tempo: */
				bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + k*10, 2);
				for(j = 0; j < bf_temoin.nbFocals; j++){
					str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
					fprintf(f2,  "%d;%s;%f\n", i+32, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf_temoin);
			}
			/*Fluctuation: */

			bf = BFS_getProjection(bs.beliefs[0], 90, 2);
			for(j = 0; j < bf.nbFocals; j++){
				str = Sets_elementToString(bf.focals[j].element, bs.refList);
				fprintf(f3,  "%d;%s;%f\n", 36, str, BF_m(bf, bf.focals[j].element));
				free(str);
			}
			BF_freeBeliefFunction(&bf);
			/*Without tempo: */
			bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 90, 2);
			for(j = 0; j < bf_temoin.nbFocals; j++){
				str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
				fprintf(f2,  "%d;%s;%f\n", 36, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
				free(str);
			}
			BF_freeBeliefFunction(&bf_temoin);
			/*Return of the normal state: */
			for(i = 0; i < 10; i++){
				usleep(250000/pow(2,l));
	
				bf = BFS_getProjection(bs.beliefs[0], 200 + k*10, 2);
				for(j = 0; j < bf.nbFocals; j++){
					str = Sets_elementToString(bf.focals[j].element, bs.refList);
					fprintf(f3,  "%d;%s;%f\n", i+37, str, BF_m(bf, bf.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf);
				/*Without tempo: */
				bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + k*10, 2);
				for(j = 0; j < bf_temoin.nbFocals; j++){
					str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
					fprintf(f2,  "%d;%s;%f\n", i+37, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf_temoin);
			}
			
			fclose(f3);
			fclose(f2);
			BFS_freeBeliefStructure(&bs);
			BFS_freeBeliefStructure(&bs_temoin);
		}
	}
	
	
	
	
	/* Tests for paper on tempo: */
	
	/*
	 * Test of convergence:
	 */
	for(k = 1; k < 10; k++){
		for(l = 1; l < 5; l++){
			bs = BFS_loadBeliefStructure(BFS_PATH, "tempoFusion");
			bs_temoin = BFS_loadBeliefStructure(BFS_PATH, "tempoFusionTemoin");
			/* Log files: */
			sprintf(path, "./data/results/tempo-fusion/Convergence - R%d-B0.%d", (int)pow(2, l), k);
			f3 = fopen(path, "w");
			sprintf(path, "./data/results/tempo-fusion-temoin/Convergence - R%d-B0.%d", (int)pow(2, l), k);
			f2 = fopen(path, "w");
			/* Believing... */
			for(i = 0; i < 30; i++){
				usleep(250000/pow(2,l));
				bf = BFS_getProjection(bs.beliefs[0], k*10, 2);
				for(j = 0; j < bf.nbFocals; j++){
					str = Sets_elementToString(bf.focals[j].element, bs.refList);
					fprintf(f3,  "%d;%s;%f\n", i+1, str, BF_m(bf, bf.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf);
				/*Without tempo: */
				bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], k*10, 2);
				for(j = 0; j < bf_temoin.nbFocals; j++){
					str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
					fprintf(f2,  "%d;%s;%f\n", i+1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf_temoin);
			}
			/* Closing log files */
			fclose(f3);
			fclose(f2);
			BFS_freeBeliefStructure(&bs);
			BFS_freeBeliefStructure(&bs_temoin);
		}
	}
	
	/*
	 * Test of convergence then loss of evidence:
	 */
	for(k = 1; k < 10; k++){
		for(l = 1; l < 5; l++){
			bs = BFS_loadBeliefStructure(BFS_PATH, "tempoFusion");
			bs_temoin = BFS_loadBeliefStructure(BFS_PATH, "tempoFusionTemoin");
			/* Log files: */
			sprintf(path, "./data/results/tempo-fusion/Convergence then loss of evidence - R%d-B0.%d", (int)pow(2, l), k);
			f3 = fopen(path, "w");
			sprintf(path, "./data/results/tempo-fusion-temoin/Convergence then loss of evidence - R%d-B0.%d", (int)pow(2, l), k);
			f2 = fopen(path, "w");
			/* Believing... */
			for(i = 0; i < 15; i++){
				usleep(250000/pow(2,l));
				bf = BFS_getProjection(bs.beliefs[0], k*10, 2);
				for(j = 0; j < bf.nbFocals; j++){
					str = Sets_elementToString(bf.focals[j].element, bs.refList);
					fprintf(f3,  "%d;%s;%f\n", i+1, str, BF_m(bf, bf.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf);
				/*Without tempo: */
				bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], k*10, 2);
				for(j = 0; j < bf_temoin.nbFocals; j++){
					str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
					fprintf(f2,  "%d;%s;%f\n", i+1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf_temoin);
			}
	
			for(i = 15; i < 30; i++){
				usleep(250000/pow(2,l));
				bf = BFS_getProjection(bs.beliefs[0], NO_MEASURE, 2);
				for(j = 0; j < bf.nbFocals; j++){
					str = Sets_elementToString(bf.focals[j].element, bs.refList);
					fprintf(f3,  "%d;%s;%f\n", i+1, str, BF_m(bf, bf.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf);
				/*Without tempo: */
				/*bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], WEIRD_NUMBER, 2);
				for(j = 0; j < bf_temoin.nbFocals; j++){
					str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
					fprintf(f2,  "%d;%s;%f\n", i, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf_temoin);*/
			}
			/* Closing log files */
			fclose(f3);
			fclose(f2);
			BFS_freeBeliefStructure(&bs);
			BFS_freeBeliefStructure(&bs_temoin);
		}
	}
	
	
	/*
	 * Test of convergence then no real evidence:
	 */
	for(k = 1; k < 10; k++){
		for(l = 1; l < 5; l++){
			bs = BFS_loadBeliefStructure(BFS_PATH, "tempoFusion");
			bs_temoin = BFS_loadBeliefStructure(BFS_PATH, "tempoFusionTemoin");
			/* Log files: */
			sprintf(path, "./data/results/tempo-fusion/Convergence then no real evidence - R%d-B0.%d", (int)pow(2, l), k);
			f3 = fopen(path, "w");
			sprintf(path, "./data/results/tempo-fusion-temoin/Convergence then no real evidence - R%d-B0.%d", (int)pow(2, l), k);
			f2 = fopen(path, "w");
			/* Believing... */
			for(i = 0; i < 15; i++){
				usleep(250000/pow(2,l));
				bf = BFS_getProjection(bs.beliefs[0], k*10, 2);
				for(j = 0; j < bf.nbFocals; j++){
					str = Sets_elementToString(bf.focals[j].element, bs.refList);
					fprintf(f3,  "%d;%s;%f\n", i+1, str, BF_m(bf, bf.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf);
				/*Without tempo: */
				bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], k*10, 2);
				for(j = 0; j < bf_temoin.nbFocals; j++){
					str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
					fprintf(f2,  "%d;%s;%f\n", i+1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf_temoin);
			}
	
			for(i = 15; i < 30; i++){
				usleep(250000/pow(2,l));
				bf = BFS_getProjection(bs.beliefs[0], 200, 2);
				for(j = 0; j < bf.nbFocals; j++){
					str = Sets_elementToString(bf.focals[j].element, bs.refList);
					fprintf(f3,  "%d;%s;%f\n", i+1, str, BF_m(bf, bf.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf);
				/*Without tempo: */
				bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200, 2);
				for(j = 0; j < bf_temoin.nbFocals; j++){
					str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
					fprintf(f2,  "%d;%s;%f\n", i+1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf_temoin);
			}
			/* Closing log files */
			fclose(f3);
			fclose(f2);
			BFS_freeBeliefStructure(&bs);
			BFS_freeBeliefStructure(&bs_temoin);
		}
	}
	
	
	/*
	 * Test of the effect of noise:
	 */
	for(k = 1; k < 10; k++){
		for(l = 1; l < 5; l++){
			bs = BFS_loadBeliefStructure(BFS_PATH, "tempoFusion");
			bs_temoin = BFS_loadBeliefStructure(BFS_PATH, "tempoFusionTemoin");
			/* Log files: */
			sprintf(path, "./data/results/tempo-fusion/Effect of noise - R%d-B0.%d", (int)pow(2, l), k);
			f3 = fopen(path, "w");
			sprintf(path, "./data/results/tempo-fusion-temoin/Effect of noise - R%d-B0.%d", (int)pow(2, l), k);
			f2 = fopen(path, "w");
			/* Believing... */
			for(i = 0; i < 15; i++){
				usleep(250000/pow(2,l));
				bf = BFS_getProjection(bs.beliefs[0], k*10, 2);
				for(j = 0; j < bf.nbFocals; j++){
					str = Sets_elementToString(bf.focals[j].element, bs.refList);
					fprintf(f3,  "%d;%s;%f\n", i+1, str, BF_m(bf, bf.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf);
				/*Without tempo: */
				bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], k*10, 2);
				for(j = 0; j < bf_temoin.nbFocals; j++){
					str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
					fprintf(f2,  "%d;%s;%f\n", i+1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf_temoin);
			}
	
			/* Noise: */
			i = 15;
			usleep(250000/pow(2,l));
			bf = BFS_getProjection(bs.beliefs[0], 200 + k*10, 2);
			for(j = 0; j < bf.nbFocals; j++){
				str = Sets_elementToString(bf.focals[j].element, bs.refList);
				fprintf(f3,  "%d;%s;%f\n", i+1, str, BF_m(bf, bf.focals[j].element));
				free(str);
			}
			BF_freeBeliefFunction(&bf);
			/*Without tempo: */
			bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + k*10, 2);
			for(j = 0; j < bf_temoin.nbFocals; j++){
				str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
				fprintf(f2,  "%d;%s;%f\n", i+1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
				free(str);
			}
			BF_freeBeliefFunction(&bf_temoin);
	
			for(i = 16; i < 30; i++){
				usleep(250000/pow(2,l));
				bf = BFS_getProjection(bs.beliefs[0], k*10, 2);
				for(j = 0; j < bf.nbFocals; j++){
					str = Sets_elementToString(bf.focals[j].element, bs.refList);
					fprintf(f3,  "%d;%s;%f\n", i+1, str, BF_m(bf, bf.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf);
				/*Without tempo: */
				bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], k*10, 2);
				for(j = 0; j < bf_temoin.nbFocals; j++){
					str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
					fprintf(f2,  "%d;%s;%f\n", i+1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf_temoin);
			}
			/* Closing log files */
			fclose(f3);
			fclose(f2);
			BFS_freeBeliefStructure(&bs);
			BFS_freeBeliefStructure(&bs_temoin);
		}
	}
	
	
	
	/*
	 * Test of transition between states:
	 */
	for(k = 1; k < 10; k++){
		for(l = 1; l < 5; l++){
			bs = BFS_loadBeliefStructure(BFS_PATH, "tempoFusion");
			bs_temoin = BFS_loadBeliefStructure(BFS_PATH, "tempoFusionTemoin");
			/* Log files: */
			sprintf(path, "./data/results/tempo-fusion/Real state changes - R%d-B0.%d", (int)pow(2, l), k);
			f3 = fopen(path, "w");
			sprintf(path, "./data/results/tempo-fusion-temoin/Real state changes - R%d-B0.%d", (int)pow(2, l), k);







			f2 = fopen(path, "w");
			/* Believing... */
			for(i = 0; i < 15; i++){
				usleep(250000/pow(2,l));
				bf = BFS_getProjection(bs.beliefs[0], k*10, 2);
				for(j = 0; j < bf.nbFocals; j++){
					str = Sets_elementToString(bf.focals[j].element, bs.refList);
					fprintf(f3,  "%d;%s;%f\n", i+1, str, BF_m(bf, bf.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf);
				/*Without tempo: */
				bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], k*10, 2);
				for(j = 0; j < bf_temoin.nbFocals; j++){
					str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
					fprintf(f2,  "%d;%s;%f\n", i+1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf_temoin);
			}
	
			for(i = 15; i < 30; i++){
				usleep(250000/pow(2,l));
				bf = BFS_getProjection(bs.beliefs[0], 200 + k*10, 2);
				for(j = 0; j < bf.nbFocals; j++){
					str = Sets_elementToString(bf.focals[j].element, bs.refList);
					fprintf(f3,  "%d;%s;%f\n", i+1, str, BF_m(bf, bf.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf);
				/*Without tempo: */
				bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + k*10, 2);
				for(j = 0; j < bf_temoin.nbFocals; j++){
					str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
					fprintf(f2,  "%d;%s;%f\n", i+1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
					free(str);
				}
				BF_freeBeliefFunction(&bf_temoin);
			}
			/* Closing log files */
			fclose(f3);
			fclose(f2);
			BFS_freeBeliefStructure(&bs);
			BFS_freeBeliefStructure(&bs_temoin);
		}
	}
	
	
	/*
	 * Test of transition between states:
	 */
	for(k = 5; k < 10; k++){
		for(l = 1; l < 5; l++){
			for(m = 1; m < 5; m++){
				bs = BFS_loadBeliefStructure(BFS_PATH, "tempoFusion");
				bs_temoin = BFS_loadBeliefStructure(BFS_PATH, "tempoFusionTemoin");
				/* Log files: */
				sprintf(path, "./data/results/tempo-fusion/Real state changes (slow) - R%d-B0.%d-B0.%d", (int)pow(2, l), k, m);
				f3 = fopen(path, "w");
				sprintf(path, "./data/results/tempo-fusion-temoin/Real state changes (slow) - R%d-B0.%d-B0.%d", (int)pow(2, l), k, m);
				f2 = fopen(path, "w");



				/* Believing... */
				for(i = 0; i < 15; i++){
					usleep(250000/pow(2,l));
					bf = BFS_getProjection(bs.beliefs[0], k*10, 2);
					for(j = 0; j < bf.nbFocals; j++){
						str = Sets_elementToString(bf.focals[j].element, bs.refList);
						fprintf(f3,  "%d;%s;%f\n", i+1, str, BF_m(bf, bf.focals[j].element));
						free(str);
					}
					BF_freeBeliefFunction(&bf);
					/*Without tempo: */
					bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], k*10, 2);
					for(j = 0; j < bf_temoin.nbFocals; j++){
						str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
						fprintf(f2,  "%d;%s;%f\n", i+1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
						free(str);
					}
					BF_freeBeliefFunction(&bf_temoin);
				}
	
				for(i = 15; i < 30; i++){
					usleep(250000/pow(2,l));
					bf = BFS_getProjection(bs.beliefs[0], 200 + m*10, 2);
					for(j = 0; j < bf.nbFocals; j++){
						str = Sets_elementToString(bf.focals[j].element, bs.refList);
						fprintf(f3,  "%d;%s;%f\n", i+1, str, BF_m(bf, bf.focals[j].element));
						free(str);
					}
					BF_freeBeliefFunction(&bf);
					/*Without tempo: */
					bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + m*10, 2);
					for(j = 0; j < bf_temoin.nbFocals; j++){
						str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
						fprintf(f2,  "%d;%s;%f\n", i+1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
						free(str);
					}
					BF_freeBeliefFunction(&bf_temoin);
				}
				/* Closing log files */
				fclose(f3);
				fclose(f2);
				BFS_freeBeliefStructure(&bs);
				BFS_freeBeliefStructure(&bs_temoin);
			}
		}
	}
	
	
	
	/*
	 * Test of transition between states:
	 */
	for(k = 1; k < 5; k++){
		for(l = 1; l < 5; l++){
			for(m = 5; m < 10; m++){
				bs = BFS_loadBeliefStructure(BFS_PATH, "tempoFusion");
				bs_temoin = BFS_loadBeliefStructure(BFS_PATH, "tempoFusionTemoin");
				/* Log files: */
				sprintf(path, "./data/results/tempo-fusion/Real state changes (fast) - R%d-B0.%d-B0.%d", (int)pow(2, l), k, m);
				f3 = fopen(path, "w");
				sprintf(path, "./data/results/tempo-fusion-temoin/Real state changes (fast) - R%d-B0.%d-B0.%d", (int)pow(2, l), k, m);
				f2 = fopen(path, "w");
				/* Believing... */
				for(i = 0; i < 15; i++){
					usleep(250000/pow(2,l));
					bf = BFS_getProjection(bs.beliefs[0], k*10, 2);
					for(j = 0; j < bf.nbFocals; j++){
						str = Sets_elementToString(bf.focals[j].element, bs.refList);
						fprintf(f3,  "%d;%s;%f\n", i+1, str, BF_m(bf, bf.focals[j].element));
						free(str);
					}
					BF_freeBeliefFunction(&bf);
					/*Without tempo: */
					bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], k*10, 2);
					for(j = 0; j < bf_temoin.nbFocals; j++){
						str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
						fprintf(f2,  "%d;%s;%f\n", i+1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
						free(str);
					}
					BF_freeBeliefFunction(&bf_temoin);
				}
	
				for(i = 15; i < 30; i++){
					usleep(250000/pow(2,l));
					bf = BFS_getProjection(bs.beliefs[0], 200 + m*10, 2);
					for(j = 0; j < bf.nbFocals; j++){
						str = Sets_elementToString(bf.focals[j].element, bs.refList);
						fprintf(f3,  "%d;%s;%f\n", i+1, str, BF_m(bf, bf.focals[j].element));
						free(str);
					}
					BF_freeBeliefFunction(&bf);
					/*Without tempo: */
					bf_temoin = BFS_getProjection(bs_temoin.beliefs[0], 200 + m*10, 2);
					for(j = 0; j < bf_temoin.nbFocals; j++){
						str = Sets_elementToString(bf_temoin.focals[j].element, bs_temoin.refList);
						fprintf(f2,  "%d;%s;%f\n", i+1, str, BF_m(bf_temoin, bf_temoin.focals[j].element));
						free(str);
					}
					BF_freeBeliefFunction(&bf_temoin);
				}
				/* Closing log files */
				fclose(f3);
				fclose(f2);
				BFS_freeBeliefStructure(&bs);
				BFS_freeBeliefStructure(&bs_temoin);
			}
		}
	}
}








void Tests_beliefsFromBeliefs(){
    BF_BeliefFunction bf, bf2;
    BFB_BeliefStructure bs_bfb;
    int i = 0;
    char *str = NULL;
    FILE* f = NULL;
	BFS_BeliefStructure bs_test;
	BF_BeliefFunction *bfs, *bfs2;
	char** frameNames = NULL;
	
	f = fopen("data/results/beliefsFromBeliefs.txt", "w");
    if(f == NULL){

        printf("Cannot open the file data/results/beliefsFromBeliefs.txt...\n");
        exit(EXIT_FAILURE);
    }
	
	fprintf(f,"\n\n");
	fprintf(f,"+-----------------------------------------------+\n");
	fprintf(f,"|+---------------------------------------------+|\n");
	fprintf(f,"||                                             ||\n");
	fprintf(f,"||  TEST OF THE MODULE : BeliefsFromBeliefs !  ||\n");
	fprintf(f,"||                                             ||\n");
	fprintf(f,"|+---------------------------------------------+|\n");
	fprintf(f,"+-----------------------------------------------+\n");
	fprintf(f,"\n\n");
	
	fprintf(f,"*******************************\n");
    fprintf(f,"Test of belief structure loading:\n");
    fprintf(f,"*******************************\n");
	/*Loading: */
	bs_bfb = BFB_loadBeliefStructure("test9");
	/*Printing: */
	str = BFB_beliefStructureToString(bs_bfb);
	fprintf(f,"%s", str);
	free(str);
	/*Freeing: */
	BFB_freeBeliefStructure(&bs_bfb);
	
	/*Loading: */
	bs_bfb = BFB_loadBeliefStructure("Sleeping");
	/*Printing: */
	str = BFB_beliefStructureToString(bs_bfb);
	fprintf(f,"%s", str);
	free(str);
	/*Freeing: */
	BFB_freeBeliefStructure(&bs_bfb);

	
	fprintf(f,"\n\n*******************************\n");
    fprintf(f,"Test of frame transformation:\n");
    fprintf(f,"*******************************\n");
	
	bs_bfb = BFB_loadBeliefStructure("test9");
	bs_test = BFS_loadBeliefStructure(BFS_PATH, "test");
	
	str = BFB_beliefStructureToString(bs_bfb);
	fprintf(f, "%s\n", str);
	free(str);
	

	/*Projection: */
	bf = BFS_getProjection(bs_test.beliefs[0], 100, bs_test.refList.card);
	str = BF_beliefFunctionToString(bf, bs_test.refList);
	fprintf(f, "Measure : 100\n-------------\n%s\n", str);
	free(str);
	bf2 = BFB_believeFromBelief(bs_bfb.beliefs[0], bf, bs_bfb.refList.card);
	str = BF_beliefFunctionToString(bf2, bs_bfb.refList);
	fprintf(f, "Conversion :\n%s\n", str);
	free(str);
	BF_freeBeliefFunction(&bf);
	BF_freeBeliefFunction(&bf2);
	
	bf = BFS_getProjection(bs_test.beliefs[0], 200, bs_test.refList.card);
	str = BF_beliefFunctionToString(bf, bs_test.refList);
	fprintf(f, "Measure : 200\n-------------\n%s\n", str);
	free(str);
	bf2 = BFB_believeFromBelief(bs_bfb.beliefs[0], bf, bs_bfb.refList.card);
	str = BF_beliefFunctionToString(bf2, bs_bfb.refList);
	fprintf(f, "Conversion :\n%s\n", str);
	free(str);
	BF_freeBeliefFunction(&bf);
	BF_freeBeliefFunction(&bf2);
	
	bf = BFS_getProjection(bs_test.beliefs[0], 300, bs_test.refList.card);
	str = BF_beliefFunctionToString(bf, bs_test.refList);
	fprintf(f, "Measure : 300\n-------------\n%s\n", str);
	free(str);
	bf2 = BFB_believeFromBelief(bs_bfb.beliefs[0], bf, bs_bfb.refList.card);
	str = BF_beliefFunctionToString(bf2, bs_bfb.refList);
	fprintf(f, "Conversion :\n%s\n", str);
	free(str);
	BF_freeBeliefFunction(&bf);
	BF_freeBeliefFunction(&bf2);
	
	bf = BFS_getProjection(bs_test.beliefs[0], 350, bs_test.refList.card);
	str = BF_beliefFunctionToString(bf, bs_test.refList);
	fprintf(f, "Measure : 350\n-------------\n%s\n", str);
	free(str);
	bf2 = BFB_believeFromBelief(bs_bfb.beliefs[0], bf, bs_bfb.refList.card);
	str = BF_beliefFunctionToString(bf2, bs_bfb.refList);
	fprintf(f, "Conversion :\n%s\n", str);
	free(str);
	BF_freeBeliefFunction(&bf);
	BF_freeBeliefFunction(&bf2);
	
	bf = BFS_getProjection(bs_test.beliefs[0], 412, bs_test.refList.card);
	str = BF_beliefFunctionToString(bf, bs_test.refList);
	fprintf(f, "Measure : 412\n-------------\n%s\n", str);
	free(str);
	bf2 = BFB_believeFromBelief(bs_bfb.beliefs[0], bf, bs_bfb.refList.card);
	str = BF_beliefFunctionToString(bf2, bs_bfb.refList);
	fprintf(f, "Conversion :\n%s\n", str);
	free(str);
	
	frameNames = malloc(sizeof(char*) * 4);
	frameNames[0] = "test3";
	frameNames[1] = "peuh";

	frameNames[2] = "the game";
	frameNames[3] = "I lost";
	
	bfs = malloc(sizeof(BF_BeliefFunction) * 4);
	bfs[0] = bf;
	bfs[1] = bf;
	bfs[2] = bf;
	bfs[3] = bf;
	
	bfs2 = BFB_believeFromBeliefs(bs_bfb, (const char* const * const)frameNames, bfs, 4);
	
	for(i = 0; i < 4; i++){
		fprintf(f, "Frame name : %s\n", frameNames[i]);
		str = BF_beliefFunctionToString(bfs2[i], bs_bfb.refList);
		fprintf(f, "Transformation :\n%s\n", str);
		free(str);
	}
	
	for(i = 0; i < 4; i++){
		BF_freeBeliefFunction(&(bfs2[i]));	
	}
	free(frameNames);
	free(bfs);
	free(bfs2);
	BF_freeBeliefFunction(&bf);
	BF_freeBeliefFunction(&bf2);
	BFB_freeBeliefStructure(&bs_bfb);
	BFS_freeBeliefStructure(&bs_test);
	fclose(f);
}







void Test_beliefFunctions(){
	BFS_BeliefStructure bs;
    BF_FocalElement bp;
    BF_FocalElement *extrema = NULL;
    BF_BeliefFunction bf, bf2;
    BF_BeliefFunction *evidences;
    Sets_Set powerset;
    int i = 0, j = 0;
    char *str = NULL;
    FILE* f = NULL;
    double* sensorMeasures = NULL;
    char** sensorTypes = NULL;
    float* conflict = NULL;
    int nbMax = 0, nbMin = 0;
    
    f = fopen("data/results/beliefFunctions.txt", "w");
    if(f == NULL){
        printf("Cannot open the file data/results/beliefFunctions.txt...\n");
        exit(EXIT_FAILURE);
    }
    
	fprintf(f,"\n\n");
	fprintf(f,"+-----------------------------------------------+\n");
	fprintf(f,"|+---------------------------------------------+|\n");
	fprintf(f,"||                                             ||\n");
	fprintf(f,"||   TEST OF THE MODULE : BeliefsFunctions !   ||\n");
	fprintf(f,"||                                             ||\n");
	fprintf(f,"|+---------------------------------------------+|\n");
	fprintf(f,"+-----------------------------------------------+\n");
	fprintf(f,"\n\n");
	
	fprintf(f,"*******************************\n");
    fprintf(f,"Test of cleanBeliefFunction():\n");
    fprintf(f,"*******************************\n");
	bs = BFS_loadBeliefStructure(BFS_PATH, "test9");
	
	bf = BFS_getProjection(bs.beliefs[0], 1000, bs.refList.card);
	str = BF_beliefFunctionToString(bf, bs.refList);
	fprintf(f,"Before :\n%s\n", str);
	free(str);
	
	BF_cleanBeliefFunction(&bf);
	str = BF_beliefFunctionToString(bf, bs.refList);
	fprintf(f,"After :\n%s\n", str);
	free(str);
	
	BF_freeBeliefFunction(&bf);
	BFS_freeBeliefStructure(&bs);
	
	/* !!!
       Loading a new belief structure:
       !!! */
    fprintf(f,"*******************************\n");
    fprintf(f,"Test BeliefStructure loading:\n");
    fprintf(f,"*******************************\n");
    bs = BFS_loadBeliefStructure(BFS_PATH, "test");
    /*Printing: */
    str = BFS_beliefStructureToString(bs);
    fprintf(f,"%s\n", str);
    free(str);
    str = NULL;
    /*Freeing: */
    fprintf(f,"\n\n");
    /*Allocation: */
    sensorTypes = malloc(sizeof(char*)*5);
    sensorTypes[0] = "S1";
    sensorTypes[1] = "S2";
    sensorTypes[2] = "S3";
    sensorTypes[3] = "S4";
    sensorTypes[4] = "S5";
    
    sensorMeasures = malloc(sizeof(double)*5);
    sensorMeasures[0] = 301.11;
    sensorMeasures[1] = 334.34;
    sensorMeasures[2] = 345.76;
    sensorMeasures[3] = 315.58;
    sensorMeasures[4] = 297.42;
    
    evidences = BFS_getEvidence(bs, (const char* const * const)sensorTypes, sensorMeasures, 5);

    free(sensorTypes);
    free(sensorMeasures);
	
    fprintf(f,"*******************************\n");
    fprintf(f,"Test of DempsterCombination():\n");
    fprintf(f,"*******************************\n");
    /*DempsterCombination: */
    bf = BF_DempsterCombination(evidences[0], evidences[1]);
    /*Printing: */
    str = BF_beliefFunctionToString(evidences[0], bs.refList);
    fprintf(f, "Belief function 1:\n%s\n", str);
    free(str);
    str = NULL;
    str = BF_beliefFunctionToString(evidences[1], bs.refList);
    fprintf(f, "Belief function 2:\n%s\n", str);
    free(str);
    str = NULL;
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f,"Dempster Combination:\n%s\n", str);
    free(str);
    str = NULL;
    /*Freeing: */
    BF_freeBeliefFunction(&bf);

    fprintf(f, "\n\n");

    fprintf(f,"*******************************\n");
    fprintf(f,"Test of fullDempsterCombination():\n");
    fprintf(f,"*******************************\n");
    /*fullDempsterCombination: */
    bf = BF_fullDempsterCombination(evidences, 5);
    /*Printing: */
    for(i = 0; i<bs.nbSensors; i++){
        str = BF_beliefFunctionToString(evidences[i], bs.refList);
        fprintf(f, "Belief function %d:\n%s\n", i, str);
        free(str);
        str = NULL;
    }
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f,"Full Dempster Combination:\n%s\n", str);
    free(str);
    str = NULL;
    /*Freeing: */
    BF_freeBeliefFunction(&bf);
    fprintf(f, "\n\n");


    fprintf(f,"*******************************\n");
    fprintf(f,"Test of SmetsCombination():\n");
    fprintf(f,"*******************************\n");
    /*SmetsCombination: */
    bf = BF_SmetsCombination(evidences[0], evidences[1]);
    /*Printing: */
    str = BF_beliefFunctionToString(evidences[0], bs.refList);
    fprintf(f, "Belief function 1:\n%s\n", str);
    free(str);
    str = NULL;
    str = BF_beliefFunctionToString(evidences[1], bs.refList);
    fprintf(f, "Belief function 2:\n%s\n", str);
    free(str);
    str = NULL;
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f,"Smets Combination:\n%s\n", str);
    free(str);
    str = NULL;
    /*Freeing: */
    BF_freeBeliefFunction(&bf);

    fprintf(f, "\n\n");

    fprintf(f,"*******************************\n");
    fprintf(f,"Test of fullSmetsCombination():\n");
    fprintf(f,"*******************************\n");
    /*fullSmetsCombination: */
    bf = BF_fullSmetsCombination(evidences, 5);
    /*Printing: */
    for(i = 0; i<bs.nbSensors; i++){
        str = BF_beliefFunctionToString(evidences[i], bs.refList);
        fprintf(f, "Belief function %d:\n%s\n", i, str);
        free(str);
        str = NULL;
    }
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f,"Full Smets Combination:\n%s\n", str);
    free(str);
    str = NULL;
    /*Freeing: */
    BF_freeBeliefFunction(&bf);
    fprintf(f, "\n\n");

    fprintf(f,"*******************************\n");
    fprintf(f,"Test of YagerCombination():\n");
    fprintf(f,"*******************************\n");
    /*YagerCombination: */
    bf = BF_YagerCombination(evidences[0], evidences[1]);
    /*Printing: */
    str = BF_beliefFunctionToString(evidences[0], bs.refList);
    fprintf(f, "Belief function 1:\n%s\n", str);
    free(str);
    str = NULL;
    str = BF_beliefFunctionToString(evidences[1], bs.refList);
    fprintf(f, "Belief function 2:\n%s\n", str);
    free(str);
    str = NULL;
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f,"Yager Combination:\n%s\n", str);
    free(str);
    str = NULL;
    /*Freeing: */
    BF_freeBeliefFunction(&bf);

    fprintf(f, "\n\n");

    fprintf(f,"*******************************\n");
    fprintf(f,"Test of fullYagerCombination():\n");
    fprintf(f,"*******************************\n");
    /*fullYagerCombination: */
    bf = BF_fullYagerCombination(evidences, 5);
    /*Printing: */
    for(i = 0; i<bs.nbSensors; i++){
        str = BF_beliefFunctionToString(evidences[i], bs.refList);
        fprintf(f, "Belief function %d:\n%s\n", i, str);
        free(str);
        str = NULL;
    }
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f,"Full Yager Combination:\n%s\n", str);
    free(str);
    str = NULL;
    /*Freeing: */
    BF_freeBeliefFunction(&bf);
    fprintf(f, "\n\n");

    fprintf(f,"*******************************\n");
    fprintf(f,"Test of DuboisPradeCombination():\n");
    fprintf(f,"*******************************\n");
    /*DuboisPradeCombination: */
    bf = BF_DuboisPradeCombination(evidences[0], evidences[1]);
    /*Printing: */
    str = BF_beliefFunctionToString(evidences[0], bs.refList);
    fprintf(f, "Belief function 1:\n%s\n", str);
    free(str);
    str = NULL;
    str = BF_beliefFunctionToString(evidences[1], bs.refList);
    fprintf(f, "Belief function 2:\n%s\n", str);
    free(str);
    str = NULL;
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f,"Dubois & Prade Combination:\n%s\n", str);
    free(str);
    str = NULL;
    /*Freeing: */
    BF_freeBeliefFunction(&bf);

    fprintf(f, "\n\n");

    fprintf(f,"*******************************\n");
    fprintf(f,"Test of fullDuboisPradeCombination():\n");
    fprintf(f,"*******************************\n");
    /*fullDuboisPradeCombination: */
    bf = BF_fullDuboisPradeCombination(evidences, 5);
    /*Printing: */
    for(i = 0; i<bs.nbSensors; i++){
        str = BF_beliefFunctionToString(evidences[i], bs.refList);
        fprintf(f, "Belief function %d:\n%s\n", i, str);
        free(str);
        str = NULL;
    }
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f,"Full Dubois & Prade Combination:\n%s\n", str);
    free(str);
    str = NULL;
    /*Freeing: */
    BF_freeBeliefFunction(&bf);
    fprintf(f, "\n\n");


    fprintf(f,"*******************************\n");
    fprintf(f,"Test of averageCombination():\n");
    fprintf(f,"*******************************\n");
    /*averageCombination: */
    bf = BF_averageCombination(evidences[0], evidences[1]);
    /*Printing: */
    str = BF_beliefFunctionToString(evidences[0], bs.refList);
    fprintf(f, "Belief function 1:\n%s\n", str);
    free(str);
    str = NULL;
    str = BF_beliefFunctionToString(evidences[1], bs.refList);
    fprintf(f, "Belief function 2:\n%s\n", str);
    free(str);
    str = NULL;
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f,"Average Combination:\n%s\n", str);
    free(str);
    str = NULL;
    /*Freeing: */
    BF_freeBeliefFunction(&bf);

    fprintf(f, "\n\n");

    fprintf(f,"*******************************\n");
    fprintf(f,"Test of fullAverageombination():\n");
    fprintf(f,"*******************************\n");
    /*fullAverageCombination: */
    bf = BF_fullAverageCombination(evidences, 5);
    /*Printing: */
    for(i = 0; i<bs.nbSensors; i++){
        str = BF_beliefFunctionToString(evidences[i], bs.refList);
        fprintf(f, "Belief function %d:\n%s\n", i, str);
        free(str);
        str = NULL;
    }
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f,"Full average Combination:\n%s\n", str);
    free(str);
    str = NULL;
    /*Freeing: */
    BF_freeBeliefFunction(&bf);
    fprintf(f, "\n\n");


    fprintf(f,"*******************************\n");
    fprintf(f,"Test of MurphyCombination():\n");
    fprintf(f,"*******************************\n");
    /*MurphyCombination: */
    bf = BF_MurphyCombination(evidences[0], evidences[1]);
    /*Printing: */
    str = BF_beliefFunctionToString(evidences[0], bs.refList);
    fprintf(f, "Belief function 1:\n%s\n", str);
    free(str);
    str = NULL;
    str = BF_beliefFunctionToString(evidences[1], bs.refList);
    fprintf(f, "Belief function 2:\n%s\n", str);
    free(str);
    str = NULL;
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f,"Murphy Combination:\n%s\n", str);
    free(str);
    str = NULL;
    /*Freeing: */
    BF_freeBeliefFunction(&bf);

    fprintf(f, "\n\n");


    fprintf(f,"*******************************\n");
    fprintf(f,"Test of fullMurphyCombination():\n");
    fprintf(f,"*******************************\n");
    /*fullMurphyCombination: */
    bf = BF_fullMurphyCombination(evidences, 5);
    /*Printing: */
    for(i = 0; i<bs.nbSensors; i++){
        str = BF_beliefFunctionToString(evidences[i], bs.refList);
        fprintf(f, "Belief function %d:\n%s\n", i, str);
        free(str);
        str = NULL;
    }
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f,"Full Murphy Combination:\n%s\n", str);
    free(str);
    str = NULL;
    /*Freeing: */
    BF_freeBeliefFunction(&bf);
    fprintf(f, "\n\n");


    fprintf(f,"*******************************\n");
    fprintf(f,"Test of fullChenCombination():\n");
    fprintf(f,"*******************************\n");
    /*fullChenCombination: */
    bf = BF_fullChenCombination(evidences, 5);
    /*Printing: */
    for(i = 0; i<bs.nbSensors; i++){
        str = BF_beliefFunctionToString(evidences[i], bs.refList);
        fprintf(f, "Belief function %d:\n%s\n", i, str);
        free(str);
        str = NULL;
    }
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f,"Full Chen Combination:\n%s\n", str);
    free(str);
    str = NULL;
    /*Freeing: */
    /*BF_freeBeliefFunction(&bf); */
    fprintf(f, "\n\n");


    fprintf(f,"*******************************\n");
    fprintf(f,"Test of weakening():\n");
    fprintf(f,"*******************************\n");
    /*Weaken: */
    bf2 = BF_weakening(evidences[0], 0.1);
    /*printing: */
    str = BF_beliefFunctionToString(evidences[0], bs.refList);
    fprintf(f, "Function to weaken:\n%s\n", str);
    free(str);
    str = BF_beliefFunctionToString(bf2, bs.refList);
    fprintf(f, "Resulting from weakening:\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf2);
    /*Weaken again: */
    bf2 = BF_weakening(bf, 0.1);
    /*printing: */
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Function to weaken:\n%s\n", str);
    free(str);
    str = BF_beliefFunctionToString(bf2, bs.refList);
    fprintf(f, "Resulting from weakening:\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf2);
    /*Freeing: */
    /*BF_freeBeliefFunction(&bf); */

    fprintf(f, "\n\n");


    fprintf(f,"*******************************\n");
    fprintf(f,"Test of discounting():\n");

    fprintf(f,"*******************************\n");
    /*Discount: */
    bf2 = BF_discounting(evidences[0], 0.1);
    /*printing: */
    str = BF_beliefFunctionToString(evidences[0], bs.refList);
    fprintf(f, "Function to discount:\n%s\n", str);
    free(str);
    str = BF_beliefFunctionToString(bf2, bs.refList);
    fprintf(f, "Resulting from discount\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf2);
    /*Discount again: */
    bf2 = BF_discounting(bf, 0.1);
    /*printing: */
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Function to discount:\n%s\n", str);
    free(str);
    str = BF_beliefFunctionToString(bf2, bs.refList);
    fprintf(f, "Resulting from discount:\n%s\n", str);
    free(str);
    /*Freeing: */
    BF_freeBeliefFunction(&bf);
    /*And again ! */
    bf = BF_discounting(bf2, 0.1);
    /*printing: */
    str = BF_beliefFunctionToString(bf2, bs.refList);
    fprintf(f, "Function to discount:\n%s\n", str);
    free(str);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Resulting from discount:\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf2);
    /*Freeing: */
    BF_freeBeliefFunction(&bf);

    fprintf(f, "\n\n");


    fprintf(f,"*******************************\n");
    fprintf(f,"Test of conditioning():\n");
    fprintf(f,"*******************************\n");
    str = BF_beliefFunctionToString(evidences[1], bs.refList);
    fprintf(f, "Belief function:\n%s\n", str);
    free(str);
    str = Sets_elementToString(evidences[1].focals[1].element, bs.refList);
    fprintf(f, "Conditioning by %s\n", str);
    free(str);
    /*Conditioning: */
    bf = BF_conditioning(evidences[1], evidences[1].focals[1].element, bs.powerset);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Result:\n%s\n", str);
    free(str);
    str = NULL;
    /*Freeing: */
    BF_freeBeliefFunction(&bf);


    bf = BF_fullSmetsCombination(evidences, 5);
    bf2 = BF_conditioning(bf, evidences[1].focals[1].element, bs.powerset);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Belief function:\n%s\n", str);
    free(str);
    str = Sets_elementToString(evidences[1].focals[1].element, bs.refList);
    fprintf(f, "Conditioning by %s\n", str);

    free(str);
    str = BF_beliefFunctionToString(bf2, bs.refList);
    fprintf(f, "Result:\n%s\n", str);
    free(str);
    str = NULL;
    BF_freeBeliefFunction(&bf);
    BF_freeBeliefFunction(&bf2);

    fprintf(f, "\n\n");


    fprintf(f,"*******************************\n");
    fprintf(f,"Test of bel(), pl(), q() and betP():\n");
    fprintf(f,"*******************************\n");
    bf = BF_fullSmetsCombination(evidences, 5);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Belief function:\n%s\n", str);
    free(str);
    for(i = 0; i<bf.nbFocals; i++){
        str = Sets_elementToString(bf.focals[i].element, bs.refList);
        fprintf(f, "bel(%s) = %f\n", str, BF_bel(bf, bf.focals[i].element));
        fprintf(f, "betP(%s) = %f\n", str, BF_betP(bf, bf.focals[i].element));
        fprintf(f, "pl(%s) = %f\n", str, BF_pl(bf, bf.focals[i].element));
        fprintf(f, "q(%s) = %f\n", str, BF_q(bf, bf.focals[i].element));
        free(str);
        str = NULL;
        fprintf(f, "\n");
    }
    fprintf(f, "-----------------\n");
    /*Freeing: */
    BF_freeBeliefFunction(&bf);

    for(j = 0; j<5; j++){
        str = BF_beliefFunctionToString(evidences[j], bs.refList);
        fprintf(f, "Belief function:\n%s\n", str);
        free(str);

        powerset = Sets_generatePowerSet(bs.refList.card);

        fprintf(f, "*** Mass maxima: ***\n");
        bp = BF_getMaxMass(evidences[j], 0);
        nbMax = BF_getNbMaxMass(evidences[j], 0);
        extrema = BF_getListMaxMass(evidences[j], 0);
        fprintf(f, "Nb max for card = no limit (0): %d\n", nbMax);
        for(i = 0; i < nbMax; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMax; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMaxMass(evidences[j], 1);
        nbMax = BF_getNbMaxMass(evidences[j], 1);
        extrema = BF_getListMaxMass(evidences[j], 1);
        fprintf(f, "Nb max for card = 1: %d\n", nbMax);
        for(i = 0; i < nbMax; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMax; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMaxMass(evidences[j], 2);
        nbMax = BF_getNbMaxMass(evidences[j], 2);
        extrema = BF_getListMaxMass(evidences[j], 2);
        fprintf(f, "Nb max for card = 2: %d\n", nbMax);
        for(i = 0; i < nbMax; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMax; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        fprintf(f, "\n*** Belief maxima: ***\n");
        bp = BF_getMaxBel(evidences[j], 0, powerset);
        nbMax = BF_getNbMaxBel(evidences[j], 0, powerset);
        extrema = BF_getListMaxBel(evidences[j], 0, powerset);
        fprintf(f, "Nb max for card = no limit (0): %d\n", nbMax);
        for(i = 0; i < nbMax; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMax; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMaxBel(evidences[j], 1, powerset);
        nbMax = BF_getNbMaxBel(evidences[j], 1, powerset);
        extrema = BF_getListMaxBel(evidences[j], 1, powerset);
        fprintf(f, "Nb max for card = 1: %d\n", nbMax);
        for(i = 0; i < nbMax; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMax; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMaxBel(evidences[j], 2, powerset);
        nbMax = BF_getNbMaxBel(evidences[j], 2, powerset);
        extrema = BF_getListMaxBel(evidences[j], 2, powerset);
        fprintf(f, "Nb max for card = 2: %d\n", nbMax);
        for(i = 0; i < nbMax; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMax; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        fprintf(f, "\n*** BetP maxima: ***\n");
        bp = BF_getMaxBetP(evidences[j], 0, powerset);
        nbMax = BF_getNbMaxBetP(evidences[j], 0, powerset);
        extrema = BF_getListMaxBetP(evidences[j], 0, powerset);
        fprintf(f, "Nb max for card = no limit (0): %d\n", nbMax);
        for(i = 0; i < nbMax; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMax; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMaxBetP(evidences[j], 1, powerset);
        nbMax = BF_getNbMaxBetP(evidences[j], 1, powerset);
        extrema = BF_getListMaxBetP(evidences[j], 1, powerset);
        fprintf(f, "Nb max for card = 1: %d\n", nbMax);
        for(i = 0; i < nbMax; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMax; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMaxBetP(evidences[j], 2, powerset);
        nbMax = BF_getNbMaxBetP(evidences[j], 2, powerset);
        extrema = BF_getListMaxBetP(evidences[j], 2, powerset);
        fprintf(f, "Nb max for card = 2: %d\n", nbMax);
        for(i = 0; i < nbMax; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMax; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        fprintf(f, "\n*** Plausibility maxima: ***\n");
        bp = BF_getMaxPl(evidences[j], 0, powerset);
        nbMax = BF_getNbMaxPl(evidences[j], 0, powerset);
        extrema = BF_getListMaxPl(evidences[j], 0, powerset);
        fprintf(f, "Nb max for card = no limit (0): %d\n", nbMax);
        for(i = 0; i < nbMax; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMax; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMaxPl(evidences[j], 1, powerset);
        nbMax = BF_getNbMaxPl(evidences[j], 1, powerset);
        extrema = BF_getListMaxPl(evidences[j], 1, powerset);
        fprintf(f, "Nb max for card = 1: %d\n", nbMax);
        for(i = 0; i < nbMax; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMax; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMaxPl(evidences[j], 2, powerset);
        nbMax = BF_getNbMaxPl(evidences[j], 2, powerset);
        extrema = BF_getListMaxPl(evidences[j], 2, powerset);
        fprintf(f, "Nb max for card = 2: %d\n", nbMax);
        for(i = 0; i < nbMax; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);

        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMax; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        fprintf(f, "\nn*** Mass minima: ***\n");
        bp = BF_getMinMass(evidences[j], 0);
        nbMin = BF_getNbMinMass(evidences[j], 0);
        extrema = BF_getListMinMass(evidences[j], 0);
        fprintf(f, "Nb min for card = no limit (0): %d\n", nbMin);
        for(i = 0; i < nbMin; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMin; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMinMass(evidences[j], 1);
        nbMin = BF_getNbMinMass(evidences[j], 1);
        extrema = BF_getListMinMass(evidences[j], 1);
        fprintf(f, "Nb min for card = 1: %d\n", nbMin);
        for(i = 0; i < nbMin; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMin; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMinMass(evidences[j], 2);
        nbMin = BF_getNbMinMass(evidences[j], 2);
        extrema = BF_getListMinMass(evidences[j], 2);
        fprintf(f, "Nb min for card = 2: %d\n", nbMin);
        for(i = 0; i < nbMin; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMin; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        fprintf(f, "\n*** Belief minima: ***\n");
        bp = BF_getMinBel(evidences[j], 0, powerset);
        nbMin = BF_getNbMinBel(evidences[j], 0, powerset);
        extrema = BF_getListMinBel(evidences[j], 0, powerset);
        fprintf(f, "Nb min for card = no limit (0): %d\n", nbMin);
        for(i = 0; i < nbMin; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMin; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMinBel(evidences[j], 1, powerset);
        nbMin = BF_getNbMinBel(evidences[j], 1, powerset);
        extrema = BF_getListMinBel(evidences[j], 1, powerset);
        fprintf(f, "Nb min for card = 1: %d\n", nbMin);
        for(i = 0; i < nbMin; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMin; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMinBel(evidences[j], 2, powerset);
        nbMin = BF_getNbMinBel(evidences[j], 2, powerset);
        extrema = BF_getListMinBel(evidences[j], 2, powerset);
        fprintf(f, "Nb min for card = 2: %d\n", nbMin);
        for(i = 0; i < nbMin; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMin; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        fprintf(f, "\n*** BetP minima: ***\n");
        bp = BF_getMinBetP(evidences[j], 0, powerset);
        nbMin = BF_getNbMinBetP(evidences[j], 0, powerset);
        extrema = BF_getListMinBetP(evidences[j], 0, powerset);
        fprintf(f, "Nb min for card = no limit (0): %d\n", nbMin);
        for(i = 0; i < nbMin; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMin; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMinBetP(evidences[j], 1, powerset);
        nbMin = BF_getNbMinBetP(evidences[j], 1, powerset);
        extrema = BF_getListMinBetP(evidences[j], 1, powerset);
        fprintf(f, "Nb min for card = 1: %d\n", nbMin);
        for(i = 0; i < nbMin; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMin; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMinBetP(evidences[j], 2, powerset);
        nbMin = BF_getNbMinBetP(evidences[j], 2, powerset);
        extrema = BF_getListMinBetP(evidences[j], 2, powerset);
        fprintf(f, "Nb min for card = 2: %d\n", nbMin);
        for(i = 0; i < nbMin; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMin; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        fprintf(f, "\n*** Plausibility minima: ***\n");
        bp = BF_getMinPl(evidences[j], 0, powerset);
        nbMin = BF_getNbMinPl(evidences[j], 0, powerset);
        extrema = BF_getListMinPl(evidences[j], 0, powerset);
        fprintf(f, "Nb min for card = 0 (no limit): %d\n", nbMin);
        for(i = 0; i < nbMin; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMin; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMinPl(evidences[j], 1, powerset);
        nbMin = BF_getNbMinPl(evidences[j], 1, powerset);
        extrema = BF_getListMinPl(evidences[j], 1, powerset);
        fprintf(f, "Nb min for card = 1: %d\n", nbMin);
        for(i = 0; i < nbMin; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMin; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        bp = BF_getMinPl(evidences[j], 2, powerset);
        nbMin = BF_getNbMinPl(evidences[j], 2, powerset);
        extrema = BF_getListMinPl(evidences[j], 2, powerset);
        fprintf(f, "Nb min for card = 2: %d\n", nbMin);
        for(i = 0; i < nbMin; i++){
            str = Sets_elementToString(extrema[i].element, bs.refList);
            fprintf(f, "%s: %f\n", str, extrema[i].beliefValue);
            free(str);
        }
        BF_freeBeliefPoint(&bp);
        for(i = 0; i < nbMin; i++){
            BF_freeBeliefPoint(&(extrema[i]));
        }
        free(extrema);

        Sets_freeSet(&powerset);

        fprintf(f, "\n");

        for(i = 0; i<evidences[j].nbFocals; i++){
            str = Sets_elementToString(evidences[j].focals[i].element, bs.refList);
            fprintf(f, "bel(%s) = %f\n", str, BF_bel(evidences[j], evidences[j].focals[i].element));
            fprintf(f, "betP(%s) = %f\n", str, BF_betP(evidences[j], evidences[j].focals[i].element));
            fprintf(f, "pl(%s) = %f\n", str, BF_pl(evidences[j], evidences[j].focals[i].element));
            fprintf(f, "q(%s) = %f\n", str, BF_q(evidences[j], evidences[j].focals[i].element));
            free(str);
            str = NULL;
            fprintf(f, "\n");
        }
        fprintf(f, "-----------------\n");
    }

    fprintf(f, "\n\n");

    fprintf(f,"*******************************\n");
    fprintf(f,"Test of selfConlict():\n");
    fprintf(f,"*******************************\n");

    for(j = 0; j<5; j++){
        str = BF_beliefFunctionToString(evidences[j], bs.refList);
        fprintf(f, "Belief function:\n%s\n", str);
        free(str);
        conflict = BF_autoConflict(evidences[j], 5);
        for(i = 0; i<5; i++){
            fprintf(f, "Degree %d : autoConflict(m) = %f\n", (i+1), conflict[i]);
        }
        fprintf(f, "-----------------\n");
        free(conflict);
    }

    fprintf(f, "\n\n");

    fprintf(f,"*******************************\n");
    fprintf(f,"Test of specificity(), nonSpecificity() and discrepancy():\n");
    fprintf(f,"*******************************\n");

    for(j = 0; j<5; j++){
        str = BF_beliefFunctionToString(evidences[j], bs.refList);
        fprintf(f, "Belief function:\n%s\n", str);
        free(str);
        fprintf(f, "Specificity : %f\n", BF_specificity(evidences[j]));
        fprintf(f, "Non Specificity : %f\n", BF_nonSpecificity(evidences[j]));
        fprintf(f, "Discrepancy : %f\n", BF_discrepancy(evidences[j]));
        fprintf(f, "-----------------\n");
    }


    fprintf(f, "\n\n");

    fprintf(f,"*******************************\n");
    fprintf(f,"Test of difference():\n");
    fprintf(f,"*******************************\n");

    str = BF_beliefFunctionToString(evidences[1], bs.refList);
    fprintf(f, "Belief function 1:\n%s\n", str);
    free(str);
    str = BF_beliefFunctionToString(evidences[2], bs.refList);
    fprintf(f, "Belief function 2:\n%s\n", str);
    free(str);
    bf = BF_difference(evidences[1], evidences[2]);
    str = BF_beliefFunctionToString(bf, bs.refList);
    fprintf(f, "Resulting difference:\n%s\n", str);
    free(str);
    BF_freeBeliefFunction(&bf);

    fprintf(f, "\n\n");

    fprintf(f,"*******************************\n");
    fprintf(f,"Test of distance() and similarity():\n");
    fprintf(f,"*******************************\n");
    str = BF_beliefFunctionToString(evidences[1], bs.refList);
    fprintf(f, "Belief function 1:\n%s\n", str);
    free(str);
    str = BF_beliefFunctionToString(evidences[2], bs.refList);
    fprintf(f, "Belief function 2:\n%s\n", str);
    free(str);
    fprintf(f, "Resulting distance: %f\n", BF_distance(evidences[1], evidences[2]));
    fprintf(f, "Resulting similarity: %f\n", BF_similarity(evidences[1], evidences[2]));

    fprintf(f, "\n\n");

    fprintf(f,"*******************************\n");
    fprintf(f,"Test of globalDistance():\n");
    fprintf(f,"*******************************\n");
    for(i = 0; i<5; i++){
        str = BF_beliefFunctionToString(evidences[i], bs.refList);
        fprintf(f, "Belief function:\n%s\n", str);
        free(str);
        fprintf(f, "Resulting conflict: %f\n\n-----------------\n", BF_globalDistance(evidences[i], evidences, 5));
    }

    fprintf(f, "\n\n");

    fprintf(f,"*******************************\n");
    fprintf(f,"Test of support():\n");
    fprintf(f,"*******************************\n");
    for(i = 0; i<5; i++){
        str = BF_beliefFunctionToString(evidences[i], bs.refList);
        fprintf(f, "Belief function:\n%s\n", str);
        free(str);
    }
    fprintf(f, "Resulting supports:\n - %f\n - %f\n - %f\n - %f\n - %f\n",
            BF_support(evidences[0], evidences, 5),
            BF_support(evidences[1], evidences, 5),
            BF_support(evidences[2], evidences, 5),
            BF_support(evidences[3], evidences, 5),
            BF_support(evidences[4], evidences, 5));

    fprintf(f, "\n\n");


    /*Free belief structure: */
    BFS_freeBeliefStructure(&bs);
    for(i = 0; i<5; i++){
        BF_freeBeliefFunction(&(evidences[i]));
    }
    free(evidences);

    fclose(f);
}








int Tests_runTests(int nbIterations, int write){
    BFS_BeliefStructure bs, bs2, bs3, bs4;
    int i = 0, j = 0, k = 0, l = 0;
    FILE *f = NULL, *f2 = NULL, *f3 = NULL, *f4 = NULL;
    double* sensorMeasures = NULL;
    char** sensorTypes = NULL;

    clock_t start, end, st, en;
    float execTime = 0;
    float execTimeDempster = 0;
    float execTimeDuboisPrade = 0;
    float execTimeAverage = 0;
    float execTimeMurphy = 0;
    
    int elementSizes[4] = {8, 32, 128, 512};
    int nbFocals[7] = {2, 4, 8, 16, 32, 64, 128};
    int nbIt = 0;
    BF_BeliefFunction m1, m2, m;


    start = clock();

	printf("Tests of beliefs from randomness... (takes several minutes)\n");
	BFR_generateRandomSeed();
	Tests_beliefsFromRandomness(10 * nbIterations);
	printf("...Done!\n\n");
	
	printf("Tests of beliefs from sensors...\n");
	Tests_beliefsFromSensors();
	printf("...Done!\n\n");
	
	printf("Tests of options of beliefs from sensors... (takes ~1min)\n");
	Tests_beliefsFromSensorsOptions();
	printf("...Done!\n\n");
	
	printf("Tests of temporization with fusion... (takes several minutes)\n");
	Tests_tempo_fusion();
	printf("...Done!\n\n");
	
	printf("Tests of temporization with specificity... (takes several minutes)\n");
	Tests_tempo_specificity();
	printf("...Done!\n\n");
    
    printf("Tests of beliefs from beliefs...\n");
    Tests_beliefsFromBeliefs();
    printf("...Done!\n\n");
    
    /********************************************
     *                                          *
     *           COMBINATIONS BENCH             *
     *                                          *
     ********************************************/
    
    f = fopen("data/results/benchDempster.txt", "w");
    if(f == NULL){
        printf("Cannot open the file data/results/benchDempster.txt...\n");
        exit(EXIT_FAILURE);
    }
    
    f2 = fopen("data/results/benchDuboisPrade.txt", "w");
    if(f2 == NULL){
        printf("Cannot open the file data/results/benchDuboisPrade.txt...\n");
        exit(EXIT_FAILURE);
    }
    
    f3 = fopen("data/results/benchAverage.txt", "w");
    if(f3 == NULL){
        printf("Cannot open the file data/results/benchAverage.txt...\n");
        exit(EXIT_FAILURE);
    }
    
    f4 = fopen("data/results/benchMurphy.txt", "w");
    if(f4 == NULL){
        printf("Cannot open the file data/results/benchMurphy.txt...\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Combination's rule bench...\n");
    
    for(i = 0; i < 4; i++){
    	for(j = 0; j < 7; j++){
    		execTimeDempster = 0;
    		execTimeDuboisPrade = 0;
    		execTimeAverage = 0;
    		execTimeMurphy = 0;
    		if(j == 0 || j == 1){
    			nbIt = nbIterations * 10;
    		}
    		else if(j > 3){
    			nbIt = nbIterations/100;
    		}
    		else {
    			nbIt = nbIterations;
    		}
    		for(k = 0; k < nbIt; k++){
    			m1 = BFR_getCrappyRandomBeliefWithFixedNbFocals(elementSizes[i], nbFocals[j]);
    			m2 = BFR_getCrappyRandomBeliefWithFixedNbFocals(elementSizes[i], nbFocals[j]);

    			st = clock();
    			for(l = 0; l < 10; l++){
					m = BF_combination(m1, m2, DEMPSTER);
					BF_freeBeliefFunction(&m);
				}
    			en = clock();
    			execTimeDempster += ((float)(en - st))/CLOCKS_PER_SEC;
    			
    			
    			st = clock();
    			for(l = 0; l < 10; l++){
					m = BF_combination(m1, m2, DUBOISPRADE);
					BF_freeBeliefFunction(&m);
				}
    			en = clock();
    			execTimeDuboisPrade += ((float)(en - st))/CLOCKS_PER_SEC;
    	
    			
    			st = clock();
    			for(l = 0; l < 10; l++){
    				m = BF_combination(m1, m2, AVERAGE);
    				BF_freeBeliefFunction(&m);
    			}
    			en = clock();
    			execTimeAverage += ((float)(en - st))/CLOCKS_PER_SEC;
    			
    			
    			st = clock();
    			for(l = 0; l < 10; l++){
					m = BF_combination(m1, m2, MURPHY);
					BF_freeBeliefFunction(&m);
				}
    			en = clock();
    			execTimeMurphy += ((float)(en - st))/CLOCKS_PER_SEC;
    			
    			BF_freeBeliefFunction(&m1);
    			BF_freeBeliefFunction(&m2);
    			
    		}
    		fprintf(f, "Dempster, nbIterations = %8d, nbFocals = %4d, elementSize = %4d, totalTime = %10f, averageTime = %10f\n", (nbIt*10), nbFocals[j], elementSizes[i], execTimeDempster, execTimeDempster/(nbIt*10));
    		fprintf(f2, "DuboisPrade, nbIterations = %8d, nbFocals = %4d, elementSize = %4d, totalTime = %10f, averageTime = %10f\n", (nbIt*10), nbFocals[j], elementSizes[i], execTimeDuboisPrade, execTimeDuboisPrade/(nbIt*10));
    		fprintf(f3, "Average, nbIterations = %8d, nbFocals = %4d, elementSize = %4d, totalTime = %10f, averageTime = %10f\n", (nbIt*10), nbFocals[j], elementSizes[i], execTimeAverage, execTimeAverage/(nbIt*10));
    		fprintf(f4, "Murphy, nbIterations = %8d, nbFocals = %4d, elementSize = %4d, totalTime = %10f, averageTime = %10f\n", (nbIt*10), nbFocals[j], elementSizes[i], execTimeMurphy, execTimeMurphy/(nbIt*10));
    	}
    }
    
    printf("...Done!\n\n");
    
    fclose(f);
    fclose(f2);
    fclose(f3);
    fclose(f4);
   
    

	/********************************************
	 *                                          *
	 *              TYPICAL CASE !              *
	 *                                          *
	 ********************************************/

    /********************************
      Test of time execution for a typical case:
    ***********************************/
    printf("\n\n****\nBench\n****\n");
    
    f = fopen("data/results/bench-data.txt", "w");
    if(f == NULL){
        printf("Cannot open the file data/results/bench-data.txt...\n");
        exit(EXIT_FAILURE);
    }
    fclose(f);
    
    /*Bench file: */
    f = fopen("data/results/bench.txt", "w");
    if(f == NULL){
        printf("Cannot open the file data/results/bench.txt...\n");
        exit(EXIT_FAILURE);
    }
    
    /*fprintf(f,"*******************************\n");
    fprintf(f,"Test of time execution for a \ntypical case using 20 sensors:\n");
    fprintf(f,"*******************************\n");*/
    
    /*Load belief structure: */
    bs = BFS_loadBeliefStructure(BFS_PATH, "test");
    /*str = BFS_beliefStructureToString(bs);
    fprintf(f, "%s\n", str);
    free(str);*/

    bs2 = BFS_loadBeliefStructure(BFS_PATH, "test5");
    /*str = BFS_beliefStructureToString(bs2);
    fprintf(f, "%s\n", str);
    free(str);*/

    bs3 = BFS_loadBeliefStructure(BFS_PATH, "test7");
    /*str = BFS_beliefStructureToString(bs3);
    fprintf(f, "%s\n", str);
    free(str);*/

    bs4 = BFS_loadBeliefStructure(BFS_PATH, "test9");
    /*str = BFS_beliefStructureToString(bs4);
    fprintf(f, "%s\n", str);
    free(str);*/

    

    /*Create the sensors measures: */
    sensorTypes = malloc(sizeof(char*) * 20);
    sensorMeasures = malloc(sizeof(double) * 20);
    sensorTypes[0] = "S1";
    sensorTypes[1] = "S1";
    sensorTypes[2] = "S1";
    sensorTypes[3] = "S1";
    sensorTypes[4] = "S2";
    sensorTypes[5] = "S2";
    sensorTypes[6] = "S2";
    sensorTypes[7] = "S2";
    sensorTypes[8] = "S3";
    sensorTypes[9] = "S3";
    sensorTypes[10] = "S3";
    sensorTypes[11] = "S3";
    sensorTypes[12] = "S4";
    sensorTypes[13] = "S4";
    sensorTypes[14] = "S4";
    sensorTypes[15] = "S4";
    sensorTypes[16] = "S5";
    sensorTypes[17] = "S5";
    sensorTypes[18] = "S5";
    sensorTypes[19] = "S5";

    sensorMeasures[0] = 301.11;
    sensorMeasures[1] = 314.34;
    sensorMeasures[2] = 320.76;
    sensorMeasures[3] = 315.58;
    sensorMeasures[4] = 297.42;
    sensorMeasures[5] = 301.11;
    sensorMeasures[6] = 314.34;
    sensorMeasures[7] = 320.76;

    sensorMeasures[8] = 315.58;
    sensorMeasures[9] = 297.42;

    sensorMeasures[10] = 301.11;
    sensorMeasures[11] = 314.34;
    sensorMeasures[12] = 320.76;
    sensorMeasures[13] = 315.58;
    sensorMeasures[14] = 297.42;
    sensorMeasures[15] = 301.11;
    sensorMeasures[16] = 314.34;
    sensorMeasures[17] = 320.76;
    sensorMeasures[18] = 315.58;
    sensorMeasures[19] = 297.42;

    st = clock();
    for(i = 0; i<nbIterations; i++){
        Tests_typicalProblem(bs,  sensorTypes, sensorMeasures, 20, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time (20 sensors - 3 atoms): %fs\n", execTime/nbIterations);
    fprintf(f, "Execution time (20 sensors - 3 atoms): %fs\n", execTime/nbIterations);

    st = clock();
    for(i = 0; i<nbIterations; i++){
        Tests_typicalProblem(bs2,  sensorTypes, sensorMeasures, 20, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time (20 sensors - 5 atoms): %fs\n", execTime/nbIterations);
    fprintf(f, "Execution time (20 sensors - 5 atoms): %fs\n", execTime/nbIterations);

    st = clock();
    for(i = 0; i<nbIterations; i++){
        Tests_typicalProblem(bs3,  sensorTypes, sensorMeasures, 20, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time (20 sensors - 7 atoms): %fs\n", execTime/nbIterations);
    fprintf(f, "Execution time (20 sensors - 7 atoms): %fs\n", execTime/nbIterations);

    st = clock();
    for(i = 0; i<nbIterations; i++){
        Tests_typicalProblem(bs4,  sensorTypes, sensorMeasures, 20, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time (20 sensors - 9 atoms): %fs\n", execTime/nbIterations);
    fprintf(f, "Execution time (20 sensors - 9 atoms): %fs\n", execTime/nbIterations);



    /*Resimulate measures : */
    free(sensorMeasures);
    free(sensorTypes);
    /*Create the sensors measures: */
    sensorTypes = malloc(sizeof(char*) * 15);
    sensorMeasures = malloc(sizeof(double) * 15);
    sensorTypes[0] = "S1";
    sensorTypes[1] = "S1";
    sensorTypes[2] = "S1";
    sensorTypes[3] = "S2";
    sensorTypes[4] = "S2";
    sensorTypes[5] = "S2";
    sensorTypes[6] = "S3";
    sensorTypes[7] = "S3";
    sensorTypes[8] = "S3";
    sensorTypes[9] = "S4";
    sensorTypes[10] = "S4";
    sensorTypes[11] = "S4";
    sensorTypes[12] = "S5";
    sensorTypes[13] = "S5";

    sensorTypes[14] = "S5";

    sensorMeasures[0] = 301.11;
    sensorMeasures[1] = 314.34;
    sensorMeasures[2] = 320.76;
    sensorMeasures[3] = 315.58;
    sensorMeasures[4] = 297.42;
    sensorMeasures[5] = 301.11;
    sensorMeasures[6] = 314.34;
    sensorMeasures[7] = 320.76;
    sensorMeasures[8] = 315.58;
    sensorMeasures[9] = 297.42;
    sensorMeasures[10] = 301.11;
    sensorMeasures[11] = 314.34;
    sensorMeasures[12] = 320.76;
    sensorMeasures[13] = 315.58;
    sensorMeasures[14] = 297.42;

    st = clock();
    for(i = 0; i<nbIterations; i++){
        Tests_typicalProblem(bs,  sensorTypes, sensorMeasures, 15, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time (15 sensors - 3 atoms): %fs\n", execTime/nbIterations);
    fprintf(f, "Execution time (15 sensors - 3 atoms): %fs\n", execTime/nbIterations);

    st = clock();
    for(i = 0; i<nbIterations; i++){
        Tests_typicalProblem(bs2,  sensorTypes, sensorMeasures, 15, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time (15 sensors - 5 atoms): %fs\n", execTime/nbIterations);
    fprintf(f, "Execution time (15 sensors - 5 atoms): %fs\n", execTime/nbIterations);

    st = clock();
    for(i = 0; i<nbIterations; i++){
        Tests_typicalProblem(bs3,  sensorTypes, sensorMeasures, 15, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time (15 sensors - 7 atoms): %fs\n", execTime/nbIterations);
    fprintf(f, "Execution time (15 sensors - 7 atoms): %fs\n", execTime/nbIterations);

    st = clock();
    for(i = 0; i<nbIterations; i++){
        Tests_typicalProblem(bs4,  sensorTypes, sensorMeasures, 15, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time (15 sensors - 9 atoms): %fs\n", execTime/nbIterations);
    fprintf(f, "Execution time (15 sensors - 9 atoms): %fs\n", execTime/nbIterations);


    /*Resimulate measures : */
    free(sensorMeasures);
    free(sensorTypes);
    /*Create the sensors measures: */
    sensorTypes = malloc(sizeof(char*) * 10);
    sensorMeasures = malloc(sizeof(double) * 10);
    sensorTypes[0] = "S1";
    sensorTypes[1] = "S1";
    sensorTypes[2] = "S2";
    sensorTypes[3] = "S2";
    sensorTypes[4] = "S3";
    sensorTypes[5] = "S3";
    sensorTypes[6] = "S4";
    sensorTypes[7] = "S4";
    sensorTypes[8] = "S5";
    sensorTypes[9] = "S5";

    sensorMeasures[0] = 301.11;
    sensorMeasures[1] = 314.34;
    sensorMeasures[2] = 320.76;
    sensorMeasures[3] = 315.58;
    sensorMeasures[4] = 297.42;
    sensorMeasures[5] = 301.11;
    sensorMeasures[6] = 314.34;
    sensorMeasures[7] = 320.76;
    sensorMeasures[8] = 315.58;
    sensorMeasures[9] = 297.42;

    st = clock();
    for(i = 0; i<nbIterations; i++){
        Tests_typicalProblem(bs,  sensorTypes, sensorMeasures, 10, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time (10 sensors - 3 atoms): %fs\n", execTime/nbIterations);
    fprintf(f, "Execution time (10 sensors - 3 atoms): %fs\n", execTime/nbIterations);

    st = clock();
    for(i = 0; i<nbIterations; i++){
        Tests_typicalProblem(bs2,  sensorTypes, sensorMeasures, 10, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time (10 sensors - 5 atoms): %fs\n", execTime/nbIterations);
    fprintf(f, "Execution time (10 sensors - 5 atoms): %fs\n", execTime/nbIterations);

    st = clock();
    for(i = 0; i<nbIterations; i++){
        Tests_typicalProblem(bs3,  sensorTypes, sensorMeasures, 10, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time (10 sensors - 7 atoms): %fs\n", execTime/nbIterations);
    fprintf(f, "Execution time (10 sensors - 7 atoms): %fs\n", execTime/nbIterations);

    st = clock();
    for(i = 0; i<nbIterations; i++){
        Tests_typicalProblem(bs4,  sensorTypes, sensorMeasures, 10, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time (10 sensors - 9 atoms): %fs\n", execTime/nbIterations);
    fprintf(f, "Execution time (10 sensors - 9 atoms): %fs\n", execTime/nbIterations);



    /*Resimulate measures : */
    free(sensorMeasures);
    free(sensorTypes);
    /*Create the sensors measures: */
    sensorTypes = malloc(sizeof(char*) * 5);
    sensorMeasures = malloc(sizeof(double) * 5);
    sensorTypes[0] = "S1";
    sensorTypes[1] = "S2";
    sensorTypes[2] = "S3";
    sensorTypes[3] = "S4";
    sensorTypes[4] = "S5";

    sensorMeasures[0] = 301.11;
    sensorMeasures[1] = 314.34;
    sensorMeasures[2] = 320.76;
    sensorMeasures[3] = 315.58;
    sensorMeasures[4] = 297.42;

    st = clock();
    for(i = 0; i<nbIterations; i++){
        Tests_typicalProblem(bs,  sensorTypes, sensorMeasures, 5, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time ( 5 sensors - 3 atoms): %fs\n", execTime/nbIterations);
    fprintf(f, "Execution time ( 5 sensors - 3 atoms): %fs\n", execTime/nbIterations);

    st = clock();
    for(i = 0; i<30; i++){
        Tests_typicalProblem(bs2,  sensorTypes, sensorMeasures, 5, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time ( 5 sensors - 5 atoms): %fs\n", execTime/30);
    fprintf(f, "Execution time ( 5 sensors - 5 atoms): %fs\n", execTime/30);

    st = clock();
    for(i = 0; i<nbIterations; i++){
        Tests_typicalProblem(bs3,  sensorTypes, sensorMeasures, 5, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time ( 5 sensors - 7 atoms): %fs\n", execTime/nbIterations);
    fprintf(f, "Execution time ( 5 sensors - 7 atoms): %fs\n", execTime/nbIterations);

    st = clock();
    for(i = 0; i<nbIterations; i++){
        Tests_typicalProblem(bs4,  sensorTypes, sensorMeasures, 5, write);
    }
    en = clock();
    execTime = ((float)(en - st))/CLOCKS_PER_SEC;
    printf("Execution time ( 5 sensors - 9 atoms): %fs\n", execTime/nbIterations);
    fprintf(f, "Execution time ( 5 sensors - 9 atoms): %fs\n", execTime/nbIterations);


    /*Deallocate: */
    BFS_freeBeliefStructure(&bs);
    BFS_freeBeliefStructure(&bs2);
    BFS_freeBeliefStructure(&bs3);
    BFS_freeBeliefStructure(&bs4);
    free(sensorMeasures);
    free(sensorTypes);

    printf("****\nBench\n****\n\n");
    fclose(f);

    /*Total execution time: */
    end = clock();
    execTime = ((float)(end - start))/CLOCKS_PER_SEC;
    printf("Total execution time: %fs (Yeah, the sleep doesn't count!)\n", execTime);

    return 0;
}

