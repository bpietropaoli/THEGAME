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

/**
 * This module contains the elements related to belief function combination.
 *
 * Different combination rules are implemented but one is free to implement some more.
 *
 * If you have no idea to what corresponds exactly a function, you should refer to the given references.
 * @file BeliefCombinations.c
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief CORE: Gives functions to combine belief functions.
 */


#include "BeliefCombinations.h"



/**
 * @name Combination rules
 * @{
 */


BF_BeliefFunction BF_fullDempsterCombination(const BF_BeliefFunction* m, const int nbM){
    BF_BeliefFunction combined = {NULL, 0, 0};
    BF_BeliefFunction temp = {NULL, 0, 0};
    int i = 0;

    #ifdef CHECK_COMPATIBILITY
    int size = m[0].elementSize;
    for(i = 0; i < nbM; i++){
    	if(m[i].elementSize != size){
    		printf("debug: in BF_fullDempsterCombination(), at least one mass function is not compatible with others...\n");
    	}
    }
    #endif

    /*Initialization: */
    combined = BF_DempsterCombination(m[0], m[1]);
    for(i = 2; i < nbM; i++){
        temp = BF_DempsterCombination(combined, m[i]);
        BF_freeBeliefFunction(&combined);
        combined = temp;
    }

    #ifdef CHECK_SUM
    if(BF_checkSum(combined)){
        printf("debug: in BF_fullDempsterCombination(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
    }
    #endif
    #ifdef CHECK_VALUES
    /*Check values: */
    if(BF_checkValues(combined)){
    	printf("debug: in BF_fullDempsterCombination(), at least one value is not valid!\n");
    }
    #endif

    return combined;
}



BF_BeliefFunction BF_DempsterCombination(const BF_BeliefFunction m1, const BF_BeliefFunction m2){
    BF_BeliefFunction combined;
    Sets_Element emptySet;
    int i = 0, voidIndex = -1;
    float voidMass = 0;
    #if defined(CHECK_VALUES) || defined(CHECK_SUM)
    char *str;
    #endif

    #ifdef CHECK_COMPATIBILITY
    if(m1.elementSize != m2.elementSize){
    	printf("debug: in BF_DempsterCombination(), the two mass functions aren't defined on the same frame...\n");
    }
    #endif

    /*Get the Smets combination:*/
    combined = BF_SmetsCombination(m1, m2);
    #ifdef CHECK_SUM
    if(BF_checkSum(combined)){
        printf("debug: in BF_DempsterCombination(), bad sum from SmetsCombination() !\n");
    }
    #endif
    emptySet = Sets_getEmptyElement(m1.elementSize);
    voidMass = BF_m(combined, emptySet);
    /*Normalize with the void mass:*/
    if(voidMass < 1 - BF_PRECISION){
        for(i = 0; i<combined.nbFocals; i++){
            if(combined.focals[i].element.card > 0){
                combined.focals[i].beliefValue *= 1.0 / (1.0 - voidMass);
            }
            else {
                voidIndex = i;
            }
        }
        /*Set the void mass to 0:*/
        if(voidIndex != -1){
        	combined.focals[voidIndex].beliefValue = 0;
        }
    }
    #ifdef CHECK_VALUES
    else{
    	printf("debug: in BF_DempsterCombination(), major conflict, m(void) = 1!\n");
    	str = BF_beliefFunctionToBitString(m1);
    	printf("debug: m1\n%s\n", str);
    	free(str);
    	str = BF_beliefFunctionToBitString(m2);
    	printf("debug: m2\n%s\n", str);
    	free(str);
    }
    #endif
    /*Deallocation:*/
    Sets_freeElement(&emptySet);

    #ifdef CHECK_SUM
    if(BF_checkSum(combined)){
        printf("debug: in BF_DempsterCombination(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
        str = BF_beliefFunctionToBitString(m1);
    	printf("debug: m1\n%s\n", str);
    	free(str);
    	str = BF_beliefFunctionToBitString(m2);
    	printf("debug: m2\n%s\n", str);
    	free(str);
    	str = BF_beliefFunctionToBitString(combined);
    	printf("debug: combined\n%s\n", str);
    	free(str);
    }
    #endif
    #ifdef CHECK_VALUES
    if(BF_checkValues(combined)){
    	printf("debug: in BF_DempsterCombination(), at least one value is not valid!\n");
    }
    #endif

    return combined;
}



BF_BeliefFunction BF_fullSmetsCombination(const BF_BeliefFunction* m, const int nbM){
    BF_BeliefFunction combined = {NULL, 0, 0};
    BF_BeliefFunction temp = {NULL, 0, 0};
    int i = 0;

    #ifdef CHECK_COMPATIBILITY
    int size = m[0].elementSize;
    for(i = 0; i < nbM; i++){
    	if(m[i].elementSize != size){
    		printf("debug: in BF_fullSmetsCombination(), at least one mass function is not compatible with others...\n");
    	}
    }
    #endif

    /*Initialization:*/
    combined = BF_SmetsCombination(m[0], m[1]);
    for(i = 2; i<nbM; i++){
        temp = BF_SmetsCombination(combined, m[i]);
        BF_freeBeliefFunction(&combined);
        combined = temp;
    }

    #ifdef CHECK_SUM
    if(BF_checkSum(combined)){
        printf("debug: in BF_fullSmetsCombination(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
    }
    #endif
    #ifdef CHECK_VALUES
    if(BF_checkValues(combined)){
    	printf("debug: in BF_fullSmetsCombination(), at least one value is not valid!\n");
    }
    #endif

    return combined;
}



BF_BeliefFunction BF_SmetsCombination(const BF_BeliefFunction m1, const BF_BeliefFunction m2){
    BF_BeliefFunction combined = {NULL, 0, 0};
    Sets_Element conj = {NULL, 0};
    int i = 0, j = 0, k = 0, index = -1;

	#ifdef CHECK_COMPATIBILITY
    if(m1.elementSize != m2.elementSize){
    	printf("debug: in BF_SmetsCombination(), the two mass functions aren't defined on the same frame...\n");
    }
    #endif

    /*Memory allocation:*/
    combined.nbFocals = 0;
    combined.focals = NULL;
    combined.elementSize = m1.elementSize;
    /* For all focal elements of both mass functions : */
    for(i = 0; i < m1.nbFocals; i++){
    	for(j = 0; j < m2.nbFocals; j++){
    		/* Conjunction */
    		conj = Sets_conjunction(m1.focals[i].element, m2.focals[j].element, combined.elementSize);
    		index = -1;
    		/* Check if already in the focals */
    		for(k = 0; k < combined.nbFocals; k++){
    			if(Sets_equals(combined.focals[k].element, conj, combined.elementSize)){
    				index = k;
    				combined.focals[k].beliefValue += m1.focals[i].beliefValue * m2.focals[j].beliefValue;
    				break;
    			}
    		}
    		/* If not in, add it ! */
    		if(index == -1){
    			combined.nbFocals++;
    			combined.focals = realloc(combined.focals, sizeof(BF_FocalElement) * combined.nbFocals);
    			DEBUG_CHECK_MALLOC(combined.focals);

    			combined.focals[combined.nbFocals - 1].element = Sets_copyElement(conj, combined.elementSize);
    			combined.focals[combined.nbFocals - 1].beliefValue = m1.focals[i].beliefValue * m2.focals[j].beliefValue;
    		}
    		Sets_freeElement(&conj);
    	}
    }

    #ifdef CHECK_SUM
    if(BF_checkSum(combined)){
        printf("debug: in BF_SmetsCombination(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
        printf("debug: First belief function:\n");
        for(i = 0; i < m1.nbFocals; i++){
        	printf("debug: ");
        	for(j = 0; j < m1.elementSize; j++){
        		printf("%d", m1.focals[i].element.values[j]);
        	}
        	printf(" : %f\n", m1.focals[i].beliefValue);
        }
        printf("debug: Second belief function:\n");
        for(i = 0; i < m2.nbFocals; i++){
        	printf("debug: ");
        	for(j = 0; j < m2.elementSize; j++){
        		printf("%d", m2.focals[i].element.values[j]);
        	}
        	printf(" : %f\n", m2.focals[i].beliefValue);
        }
        printf("debug: Resulting belief function:\n");
        for(i = 0; i < combined.nbFocals; i++){
        	printf("debug: ");
        	for(j = 0; j < combined.elementSize; j++){
        		printf("%d", combined.focals[i].element.values[j]);
        	}
        	printf(" : %f\n", combined.focals[i].beliefValue);
        }
    }
    #endif

    #ifdef CHECK_VALUES
    if(BF_checkValues(combined)){
    	printf("debug: in BF_SmetsCombination(), at least one value is not valid!\n");
    	printf("debug: First belief function:\n");
        for(i = 0; i < m1.nbFocals; i++){
        	printf("debug: ");
        	for(j = 0; j < m1.elementSize; j++){
        		printf("%d", m1.focals[i].element.values[j]);
        	}
        	printf(" : %f\n", m1.focals[i].beliefValue);
        }
        printf("debug: Second belief function:\n");
        for(i = 0; i < m2.nbFocals; i++){
        	printf("debug: ");
        	for(j = 0; j < m2.elementSize; j++){
        		printf("%d", m2.focals[i].element.values[j]);
        	}
        	printf(" : %f\n", m2.focals[i].beliefValue);
        }
        printf("debug: Resulting belief function:\n");
        for(i = 0; i < combined.nbFocals; i++){
        	printf("debug: ");
        	for(j = 0; j < combined.elementSize; j++){
        		printf("%d", combined.focals[i].element.values[j]);
        	}
        	printf(" : %f\n", combined.focals[i].beliefValue);
        }
    }
    #endif

    return combined;
}



BF_BeliefFunction BF_fullYagerCombination(const BF_BeliefFunction* m, const int nbM){
    BF_BeliefFunction combined = {NULL, 0, 0};
    BF_BeliefFunction temp = {NULL, 0, 0};
    int i = 0;

    #ifdef CHECK_COMPATIBILITY
    int size = m[0].elementSize;
    for(i = 0; i < nbM; i++){
    	if(m[i].elementSize != size){
    		printf("debug: in BF_fullYagerCombination(), at least one mass function is not compatible with others...\n");
    	}
    }
    #endif

    /*Initialization:*/
    combined = BF_YagerCombination(m[0], m[1]);
    for(i = 2; i<nbM; i++){
        temp = BF_YagerCombination(combined, m[i]);
        BF_freeBeliefFunction(&combined);
        combined = temp;
    }

    #ifdef CHECK_SUM
    if(BF_checkSum(combined)){
        printf("debug: in BF_fullYagerCombination(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
    }
    #endif
    #ifdef CHECK_VALUES
    if(BF_checkValues(combined)){
    	printf("debug: in BF_fullYagerCombination(), at least one value is not valid!\n");
    }
    #endif

    return combined;
}



BF_BeliefFunction BF_YagerCombination(const BF_BeliefFunction m1, const BF_BeliefFunction m2){
    BF_BeliefFunction combined = {NULL, 0, 0}, smets = {NULL, 0, 0};
    Sets_Element complete;
    int i = 0, addComplete = 1, completeIndex = -1, voidIndex = -1;

	#ifdef CHECK_COMPATIBILITY
    if(m1.elementSize != m2.elementSize){
    	printf("debug: in BF_YagerCombination(), the two mass functions aren't defined on the same frame...\n");
    }
    #endif

    /*Get the complete set:*/
    complete = Sets_getCompleteElement(m1.elementSize);
    /*Get the Smets combination:*/
    smets = BF_SmetsCombination(m1, m2);
    #ifdef CHECK_SUM
    if(BF_checkSum(smets)){
        printf("debug: in BF_YagerCombination(), bad sum from SmetsCombination() !\n");
    }
    #endif
    /*Check if the complete set is a focal element:*/
    for(i = 0; i<smets.nbFocals; i++){
        if(smets.focals[i].element.card == m1.elementSize){
            addComplete = 0;
            completeIndex = i;
        }
        if(smets.focals[i].element.card == 0){
            voidIndex = i;
        }
    }
    /*Copy the function:*/
    if(addComplete){
        combined.nbFocals = smets.nbFocals + 1;
        completeIndex = smets.nbFocals;
    }
    else {
        combined.nbFocals = smets.nbFocals;
    }
    combined.focals = malloc(sizeof(BF_FocalElement) * combined.nbFocals);
    DEBUG_CHECK_MALLOC(combined.focals);

    combined.elementSize = smets.elementSize;
    for(i = 0; i<smets.nbFocals; i++){
        combined.focals[i].element = Sets_copyElement(smets.focals[i].element, combined.elementSize);
        combined.focals[i].beliefValue = smets.focals[i].beliefValue;
    }
    if(addComplete){
        combined.focals[completeIndex].element = Sets_copyElement(complete, combined.elementSize);
        combined.focals[completeIndex].beliefValue = 0;
    }
    if(voidIndex != -1){
    	/*Transfer the void mass to the complete set:*/
		combined.focals[completeIndex].beliefValue += combined.focals[voidIndex].beliefValue;
		/*Set the void mass to 0:*/
		combined.focals[voidIndex].beliefValue = 0;
	}
    /*Deallocation:*/
    Sets_freeElement(&complete);
    BF_freeBeliefFunction(&smets);

    #ifdef CHECK_SUM
    if(BF_checkSum(combined)){
        printf("debug: in BF_YagerCombination(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
    }
    #endif
    #ifdef CHECK_VALUES
    if(BF_checkValues(combined)){
    	printf("debug: in BF_YagerCombination(), at least one value is not valid!\n");
    }
    #endif

    return combined;
}



BF_BeliefFunction BF_fullDuboisPradeCombination(const BF_BeliefFunction* m, const int nbM){
    BF_BeliefFunction combined = {NULL, 0, 0};
    BF_BeliefFunction temp = {NULL, 0, 0};
    int i = 0;

    #ifdef CHECK_COMPATIBILITY
    int size = m[0].elementSize;
    for(i = 0; i < nbM; i++){
    	if(m[i].elementSize != size){
    		printf("debug: in BF_fullDuboisPradeCombination(), at least one mass function is not compatible with others...\n");
    	}
    }
    #endif

    /*Initialization:*/
    combined = BF_DuboisPradeCombination(m[0], m[1]);
    for(i = 2; i<nbM; i++){
        temp = BF_DuboisPradeCombination(combined, m[i]);
        BF_freeBeliefFunction(&combined);
        combined = temp;
    }

    #ifdef CHECK_SUM
    if(BF_checkSum(combined)){
        printf("debug: in BF_fullDuboisPradeCombination(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
    }
    #endif
    #ifdef CHECK_VALUES
    if(BF_checkValues(combined)){
    	printf("debug: in BF_fullDuboisPradeCombination(), at least one value is not valid!\n");
    }
    #endif

    return combined;
}



BF_BeliefFunction BF_DuboisPradeCombination(const BF_BeliefFunction m1, const BF_BeliefFunction m2){
    BF_BeliefFunction combined;
    Sets_Element newFocal;
    int i = 0, j = 0, k = 0, index = -1;

	#ifdef CHECK_COMPATIBILITY
    if(m1.elementSize != m2.elementSize){
    	printf("debug: in BF_DuboisPradeCombination(), the two mass functions aren't defined on the same frame...\n");
    }
    #endif

    /*Initialize the belief function:*/
    combined.nbFocals = 0;
    combined.focals = NULL;
    combined.elementSize = m1.elementSize;
    /* For all focal elements of both mass functions : */
    for(i = 0; i < m1.nbFocals; i++){
    	for(j = 0; j < m2.nbFocals; j++){
    		/* Conjunction */
    		newFocal = Sets_conjunction(m1.focals[i].element, m2.focals[j].element, combined.elementSize);
    		/* If empty intersection, then disjunction */
    		if(newFocal.card == 0){
    			Sets_freeElement(&newFocal);
    			newFocal = Sets_disjunction(m1.focals[i].element, m2.focals[j].element, combined.elementSize);
    		}
    		index = -1;
    		/* Check if already in the focals */
    		for(k = 0; k < combined.nbFocals; k++){
    			if(Sets_equals(combined.focals[k].element, newFocal, combined.elementSize)){
    				index = k;
    				combined.focals[k].beliefValue += m1.focals[i].beliefValue * m2.focals[j].beliefValue;
    				break;
    			}
    		}
    		/* If not in, add it ! */
    		if(index == -1){
    			combined.nbFocals++;
    			combined.focals = realloc(combined.focals, sizeof(BF_FocalElement) * combined.nbFocals);
    			DEBUG_CHECK_MALLOC(combined.focals);

    			combined.focals[combined.nbFocals - 1].element = Sets_copyElement(newFocal, combined.elementSize);
    			combined.focals[combined.nbFocals - 1].beliefValue = m1.focals[i].beliefValue * m2.focals[j].beliefValue;
    		}
    		Sets_freeElement(&newFocal);
    	}
    }

    #ifdef CHECK_SUM
    if(BF_checkSum(combined)){
        printf("debug: in BF_DuboisPradeCombination(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
    }
    #endif
    #ifdef CHECK_VALUES
    if(BF_checkValues(combined)){
    	printf("debug: in BF_DuboisPradeCombination(), at least one value is not valid!\n");
    }
    #endif

    return combined;
}



BF_BeliefFunction BF_fullAverageCombination(const BF_BeliefFunction* m, const int nbM){
    BF_BeliefFunction combined;
    int i = 0, j = 0, k = 0, index = -1;
    int size = m[0].elementSize;

    #ifdef CHECK_COMPATIBILITY
    for(i = 0; i < nbM; i++){
    	if(m[i].elementSize != size){
    		printf("debug: in BF_fullAverageCombination(), at least one mass function is not compatible with others...\n");
    	}
    }
    #endif

    /*Initialize the belief function:*/
    combined.nbFocals = 0;
    combined.focals = NULL;
    combined.elementSize = size;
    /* For all mass functions : */
    for(k = 0; k < nbM; k++){
		for(i = 0; i < m[k].nbFocals; i++){
			index = -1;
			/* Check if already in the focals */
			for(j = 0; j < combined.nbFocals; j++){
				if(Sets_equals(combined.focals[j].element, m[k].focals[i].element, combined.elementSize)){
					index = j;
					combined.focals[j].beliefValue += m[k].focals[i].beliefValue;
					break;
				}
			}
			/* If not in, add it ! */
			if(index == -1){
				combined.nbFocals++;
				combined.focals = realloc(combined.focals, sizeof(BF_FocalElement) * combined.nbFocals);
				DEBUG_CHECK_MALLOC(combined.focals);

				combined.focals[combined.nbFocals - 1].element = Sets_copyElement(m[k].focals[i].element, combined.elementSize);
				combined.focals[combined.nbFocals - 1].beliefValue = m[k].focals[i].beliefValue;
			}
		}
	}
	for(i = 0; i<combined.nbFocals; i++){
        combined.focals[i].beliefValue /= nbM;
    }

    #ifdef CHECK_SUM
    if(BF_checkSum(combined)){
        printf("debug: in BF_fullAverageCombination(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
    }
    #endif
    #ifdef CHECK_VALUES
    if(BF_checkValues(combined)){
    	printf("debug: in BF_fullAverageCombination(), at least one value is not valid!\n");
    }
    #endif

    return combined;
}



BF_BeliefFunction BF_averageCombination(const BF_BeliefFunction m1, const BF_BeliefFunction m2){
    BF_BeliefFunction combined;
    int i = 0, j = 0, index = -1;

	#ifdef CHECK_COMPATIBILITY
    if(m1.elementSize != m2.elementSize){
    	printf("debug: in BF_averageCombination(), the two mass functions aren't defined on the same frame...\n");
    }
    #endif

    /*Initialize the belief function:*/
    combined.nbFocals = 0;
    combined.focals = NULL;
    combined.elementSize = m1.elementSize;
    /*Do the average:*/
    for(i = 0; i < m1.nbFocals; i++){
    	index = -1;
    	/* Check if already in the focals */
    	for(j = 0; j < combined.nbFocals; j++){
    		if(Sets_equals(combined.focals[j].element, m1.focals[i].element, combined.elementSize)){
    			index = j;
    			combined.focals[j].beliefValue += m1.focals[i].beliefValue;
    			break;
    		}
    	}
    	/* If not in, add it ! */
    	if(index == -1){
    		combined.nbFocals++;
    		combined.focals = realloc(combined.focals, sizeof(BF_FocalElement) * combined.nbFocals);
    		DEBUG_CHECK_MALLOC(combined.focals);

    		combined.focals[combined.nbFocals - 1].element = Sets_copyElement(m1.focals[i].element, combined.elementSize);
    		combined.focals[combined.nbFocals - 1].beliefValue = m1.focals[i].beliefValue;
    	}
    }
    for(i = 0; i < m2.nbFocals; i++){
    	index = -1;
    	/* Check if already in the focals */
    	for(j = 0; j < combined.nbFocals; j++){
    		if(Sets_equals(combined.focals[j].element, m2.focals[i].element, combined.elementSize)){
    			index = j;
    			combined.focals[j].beliefValue += m2.focals[i].beliefValue;
    			break;
    		}
    	}
    	/* If not in, add it ! */
    	if(index == -1){
    		combined.nbFocals++;
    		combined.focals = realloc(combined.focals, sizeof(BF_FocalElement) * combined.nbFocals);
    		DEBUG_CHECK_MALLOC(combined.focals);

    		combined.focals[combined.nbFocals - 1].element = Sets_copyElement(m2.focals[i].element, combined.elementSize);
    		combined.focals[combined.nbFocals - 1].beliefValue = m2.focals[i].beliefValue;
    	}
    }
    for(i = 0; i<combined.nbFocals; i++){
        combined.focals[i].beliefValue /= 2;
    }


    #ifdef CHECK_SUM
    if(BF_checkSum(combined)){
        printf("debug: in BF_AverageCombination(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
    }
    #endif
    #ifdef CHECK_VALUES
    if(BF_checkValues(combined)){
    	printf("debug: in BF_AverageCombination(), at least one value is not valid!\n");
    }
    #endif

    return combined;
}



BF_BeliefFunction BF_fullMurphyCombination(const BF_BeliefFunction* m, const int nbM){
    BF_BeliefFunction combined, temp, temp2;
    int i = 0;

    #ifdef CHECK_COMPATIBILITY
    int size = m[0].elementSize;
    for(i = 0; i < nbM; i++){
    	if(m[i].elementSize != size){
    		printf("debug: in BF_fullMurphyCombination(), at least one mass function is not compatible with others...\n");
    	}
    }
    #endif

    /*Get the average:*/
    combined = BF_fullAverageCombination(m, nbM);
    temp = BF_DempsterCombination(combined, combined);
    /*Do the nbM-1 Dempster combinations:*/
    for(i = 1; i<nbM-1; i++){
        temp2 = BF_DempsterCombination(temp, combined);
        BF_freeBeliefFunction(&temp);
        temp = temp2;
    }
    BF_freeBeliefFunction(&combined);
    combined = temp;

    #ifdef CHECK_SUM
    if(BF_checkSum(combined)){
        printf("debug: in BF_fullMurphyCombination(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
    }
    #endif
    #ifdef CHECK_VALUES
    if(BF_checkValues(combined)){
    	printf("debug: in BF_fullMurphyCombination(), at least one value is not valid!\n");
    }
    #endif

    return combined;
}



BF_BeliefFunction BF_MurphyCombination(const BF_BeliefFunction m1, const BF_BeliefFunction m2){
    BF_BeliefFunction combined, temp;

	#ifdef CHECK_COMPATIBILITY
    if(m1.elementSize != m2.elementSize){
    	printf("debug: in BF_MurphyCombination(), the two mass functions aren't defined on the same frame...\n");
    }
    #endif

    /*Get the average:*/
    temp = BF_averageCombination(m1, m2);
    /*Combine once using Dempster's rule of combination:*/
    combined = BF_DempsterCombination(temp, temp);
    /*Deallocation:*/
    BF_freeBeliefFunction(&temp);

    #ifdef CHECK_SUM
    if(BF_checkSum(combined)){
        printf("debug: in BF_MurphyCombination(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
    }
    #endif
    #ifdef CHECK_VALUES
    if(BF_checkValues(combined)){
    	printf("debug: in BF_MurphyCombination(), at least one value is not valid!\n");
    }
    #endif

    return combined;
}



BF_BeliefFunction BF_fullChenCombination(const BF_BeliefFunction* m, const int nbM){
    BF_BeliefFunction combined, temp, temp2;
    Sets_Set focals = {NULL, 0};
    int i = 0, j = 0, nbMaxFocals = 0;
    float *supports = NULL, *cred = NULL;
    float supportSum = 0;
    int size = m[0].elementSize;

    #ifdef CHECK_COMPATIBILITY
    for(i = 0; i < nbM; i++){
    	if(m[i].elementSize != size){
    		printf("debug: in BF_fullChenCombination(), at least one mass function is not compatible with others...\n");
    	}
    }
    #endif

    /*Count the max number of focals:*/
    for(i = 0; i<nbM; i++){
        nbMaxFocals += m[i].nbFocals;
    }
    /*Initialize set:*/
    focals.elements = malloc(sizeof(Sets_Element) * nbMaxFocals);
    DEBUG_CHECK_MALLOC(focals.elements);

    /*Get focals:*/
    for(i = 0; i<nbM; i++){
        for(j = 0; j<m[i].nbFocals; j++){
            if(!Sets_isMember(m[i].focals[j].element, focals, size)){
                focals.elements[focals.card] = Sets_copyElement(m[i].focals[j].element, size);
                focals.card++;
            }
        }
    }
    /*Initialize the belief function:*/
    combined.nbFocals = focals.card;
    combined.focals = malloc(sizeof(BF_FocalElement) * combined.nbFocals);
    DEBUG_CHECK_MALLOC(combined.focals);

    combined.elementSize = size;
    /*Get the credibility for each body of evidence:*/
    supports = malloc(sizeof(float)*nbM);
    DEBUG_CHECK_MALLOC(supports);

    cred = malloc(sizeof(float)*nbM);
    DEBUG_CHECK_MALLOC(cred);


    for(i = 0; i<nbM; i++){
        supports[i] = BF_support(m[i], m, nbM);
        supportSum += supports[i];
    }
    for(i = 0; i<nbM; i++){
        cred[i] = supports[i] / supportSum;
    }
    /*Compute the weighted average:*/
    for(i = 0; i<combined.nbFocals; i++){
        combined.focals[i].element = Sets_copyElement(focals.elements[i], size);
        combined.focals[i].beliefValue = 0;
        for(j = 0; j<nbM; j++){
            combined.focals[i].beliefValue += cred[j] * BF_m(m[j], focals.elements[i]);
        }
    }
    /*nbM-1 Dempster combinations: */
    temp = BF_DempsterCombination(combined, combined);
    for(i = 1; i<nbM-1; i++){
        temp2 = BF_DempsterCombination(temp, combined);
        BF_freeBeliefFunction(&temp);
        temp = temp2;
    }
    BF_freeBeliefFunction(&combined);
    combined = temp;
    /*Deallocation:*/
    free(supports);
    free(cred);
    Sets_freeSet(&focals);

    #ifdef CHECK_SUM
    if(BF_checkSum(combined)){
        printf("debug: in BF_ChenCombination(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
    }
    #endif
    #ifdef CHECK_SUM
    if(BF_checkValues(combined)){
    	printf("debug: in BF_ChenCombination(), at least one value is not valid!\n");
    }
    #endif

    return combined;
}



BF_BeliefFunction BF_fullCombination(const BF_BeliefFunction* m, const int nbM, const BF_CombinationRule type){
    BF_BeliefFunction fail = {NULL, 0, 0};
    switch(type){
        case DEMPSTER :    return BF_fullDempsterCombination(m, nbM);    break;
        case SMETS :       return BF_fullSmetsCombination(m, nbM);       break;
        case YAGER :       return BF_fullYagerCombination(m, nbM);       break;
        case DUBOISPRADE : return BF_fullDuboisPradeCombination(m, nbM); break;
        case AVERAGE :     return BF_fullAverageCombination(m, nbM);     break;
        case MURPHY :      return BF_fullMurphyCombination(m, nbM);      break;
        case CHEN :        return BF_fullChenCombination(m, nbM);        break;
        default :
            printf("debug: The type of combination rule required in BF_fullCombination() is unknown.\n");
            return fail;
            break;
    }
}



BF_BeliefFunction BF_combination(const BF_BeliefFunction m1, const BF_BeliefFunction m2, const BF_CombinationRule type){
    BF_BeliefFunction result;
    BF_BeliefFunction* m = NULL;
    BF_BeliefFunction fail = {NULL, 0, 0};

    switch(type){
        case DEMPSTER :    return BF_DempsterCombination(m1, m2);    break;
        case SMETS :       return BF_SmetsCombination(m1, m2);       break;
        case YAGER :       return BF_YagerCombination(m1, m2);       break;
        case DUBOISPRADE : return BF_DuboisPradeCombination(m1, m2); break;
        case AVERAGE :     return BF_averageCombination(m1, m2);     break;
        case MURPHY :      return BF_MurphyCombination(m1, m2);      break;
        case CHEN :
            m = malloc(sizeof(BF_BeliefFunction)*2);
            DEBUG_CHECK_MALLOC(m);
            m[0] = m1;
            m[1] = m2;
            result = BF_fullChenCombination(m, 2);
            free(m);
            return result;
            break;
        default :
            printf("debug: The type of combination rule required in BF_combination() is unknown.\n");
            return fail;
            break;
    }
}

/** @} */



/**
 * @name Operations using combinations
 * @{
 */


float* BF_autoConflict(const BF_BeliefFunction m, const int maxDegree){
    float* voidMasses = NULL;
    int i = 0;
    Sets_Element emptySet;
    BF_BeliefFunction temp, temp2;

    /*Get void: */
    emptySet = Sets_getEmptyElement(m.elementSize);

    /*Allocation: */
    voidMasses = malloc(sizeof(float) * maxDegree);
    DEBUG_CHECK_MALLOC(voidMasses);

    /*Initialization: */
    temp = BF_SmetsCombination(m, m);
    for(i = 0; i<maxDegree; i++){
        voidMasses[i] = BF_m(temp, emptySet);
        temp2 = temp;
        temp = BF_SmetsCombination(temp, m);
        BF_freeBeliefFunction(&temp2);
    }

    /*Deallocation: */
    BF_freeBeliefFunction(&temp);
    Sets_freeElement(&emptySet);

    return voidMasses;
}



/** @} */




