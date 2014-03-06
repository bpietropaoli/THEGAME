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


#include "BeliefFunctions.h"

/**
 * This module does not enable the building of belief functions but only to manipulate them!
 * Thus, this module offers many functions to characterize, combine and discount belief functions.
 * Some decision support functions have also been implemented in order to ease the decision making.@n@n
 * Different combination rules are implemented but one is free to implement some more.@n@n
 * If you have no idea to what corresponds exactly a function, you should refer to the given references.
 * @file BeliefFunctions.c
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief CORE: Gives structures and main
 *        functions to manipulate belief functions.
 */


/**
 * @name Utility functions
 * @{
 */

BF_BeliefFunction BF_copyBeliefFunction(const BF_BeliefFunction m){
    BF_BeliefFunction copy;
    int i = 0;

    /*Memory alocation:*/
    copy.nbFocals = m.nbFocals;
    copy.focals = malloc(sizeof(BF_FocalElement ) * copy.nbFocals);
    #ifdef DEBUG
    if(copy.focals == NULL){
       	printf("debug: malloc failed in BF_copyBeliefFunction() for \"copy.focals\".\n");
    }
    #endif
    copy.elementSize = m.elementSize;
    /*Copy: */
    for(i = 0; i < copy.nbFocals; i++){
        copy.focals[i].element = Sets_copyElement(m.focals[i].element, m.elementSize);
        copy.focals[i].beliefValue = m.focals[i].beliefValue;
    }

    return copy;
}



BF_BeliefFunction BF_getVacuousBeliefFunction(const int elementSize){
	BF_BeliefFunction vacuous = {NULL, 0, 0};
	Sets_Element empty;
	
	vacuous.nbFocals = 1;
	vacuous.focals = malloc(sizeof(BF_FocalElement ));
	#ifdef DEBUG
    if(vacuous.focals == NULL){
       	printf("debug: malloc failed in BF_getVacuousBeliefFunction() for \"vacuous.focals\".\n");
    }
    #endif
    vacuous.elementSize = elementSize;
	
	empty = Sets_getEmptyElement(elementSize);
	vacuous.focals[0].element = Sets_getOpposite(empty, elementSize);
	vacuous.focals[0].beliefValue = 1;
	
	Sets_freeElement(&empty);
	
	return vacuous;
}



void BF_cleanBeliefFunction(BF_BeliefFunction* bf){
	int i = 0, index = 0;
	int nbZeros = 0;
	BF_FocalElement* focals;
	
	/*Count the number of zeros:*/
	for(i = 0; i < bf->nbFocals; i++){
		if(bf->focals[i].beliefValue < BF_PRECISION){
			nbZeros++;
		}
	}
	
	/*Create a new list of point:*/
	focals = malloc(sizeof(BF_FocalElement) * (bf->nbFocals - nbZeros));
	/*Recopy:*/
	for(i = 0; i < bf->nbFocals; i++){
		if(bf->focals[i].beliefValue >= BF_PRECISION){
			focals[index] = bf->focals[i];
			index++;
		}
		else{
			BF_freeBeliefPoint(&(bf->focals[i]));
		}
	}
	/*Free:*/
	free(bf->focals);
	/*Replace:*/
	bf->focals = focals;
	bf->nbFocals -= nbZeros;
	
	BF_normalize(bf);
}



void BF_normalize(BF_BeliefFunction* bf){
	float sum = 0;
	int i = 0;
	
	for(i = 0; i<bf->nbFocals; i++){
        sum += bf->focals[i].beliefValue;
    }

    if(sum != 1){
        for(i = 0; i < bf->nbFocals; i++){
        	bf->focals[i].beliefValue /= sum;
        }
    }
}



/** @} */



/**
 * @name Combination rules
 * @{
 */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !!! Operation on beliefs !!!
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!  */



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
    #ifdef DEBUG
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
    voidMass = BF_M(combined, emptySet);
    /*Normalize with the void mass:*/
    if(voidMass < 1 - BF_PRECISION){
        for(i = 0; i<combined.nbFocals; i++){
            if(!Sets_equals(combined.focals[i].element, emptySet, m1.elementSize)){
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
    			#ifdef DEBUG
				if(combined.focals == NULL){
					printf("debug: malloc failed in BF_SmetsCombination() for \"combined.focals\".\n");
				}
				#endif
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
    Sets_Element complete, emptySet;
    int i = 0, addComplete = 1, completeIndex = -1, voidIndex = -1;

	#ifdef CHECK_COMPATIBILITY
    if(m1.elementSize != m2.elementSize){
    	printf("debug: in BF_YagerCombination(), the two mass functions aren't defined on the same frame...\n");
    }
    #endif

    /*Get void and the complete set:*/
    emptySet = Sets_getEmptyElement(m1.elementSize);
    complete = Sets_getOpposite(emptySet, m1.elementSize);
    /*Get the Smets combination:*/
    smets = BF_SmetsCombination(m1, m2);
    #ifdef CHECK_SUM
    if(BF_checkSum(smets)){
        printf("debug: in BF_YagerCombination(), bad sum from SmetsCombination() !\n");
    }
    #endif
    /*Check if the complete set is a focal element:*/
    for(i = 0; i<smets.nbFocals; i++){
        if(Sets_equals(smets.focals[i].element, complete, m1.elementSize)){
            addComplete = 0;
            completeIndex = i;
        }
        if(Sets_equals(smets.focals[i].element, emptySet, m1.elementSize)){
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
    #ifdef DEBUG
    if(combined.focals == NULL){
    	printf("debug: malloc failed in BF_YagerCombination() for \"combined.focals\".\n");
    }
    #endif
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
    Sets_freeElement(&emptySet);
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
    Sets_Element newFocal, emptySet;
    int i = 0, j = 0, k = 0, index = -1;
	
	#ifdef CHECK_COMPATIBILITY
    if(m1.elementSize != m2.elementSize){
    	printf("debug: in BF_DuboisPradeCombination(), the two mass functions aren't defined on the same frame...\n");
    }
    #endif
	
    /*Get void:*/
    emptySet = Sets_getEmptyElement(m1.elementSize);
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
    		if(Sets_equals(newFocal, emptySet, combined.elementSize)){
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
    			#ifdef DEBUG
				if(combined.focals == NULL){
					printf("debug: malloc failed in BF_DuboisPradeCombination() for \"combined.focals\".\n");
				}
				#endif
    			combined.focals[combined.nbFocals - 1].element = Sets_copyElement(newFocal, combined.elementSize);
    			combined.focals[combined.nbFocals - 1].beliefValue = m1.focals[i].beliefValue * m2.focals[j].beliefValue;
    		}
    		Sets_freeElement(&newFocal);
    	}
    }
    /*Deallocate:*/
    Sets_freeElement(&emptySet);
    
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
				#ifdef DEBUG
				if(combined.focals == NULL){
					printf("debug: malloc failed in BF_averageCombination() for \"combined.focals\".\n");
				}
				#endif
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
    		#ifdef DEBUG
			if(combined.focals == NULL){
				printf("debug: malloc failed in BF_averageCombination() for \"combined.focals\".\n");
			}
			#endif
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
    		#ifdef DEBUG
			if(combined.focals == NULL){
				printf("debug: malloc failed in BF_averageCombination() for \"combined.focals\".\n");
			}
			#endif
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
    #ifdef DEBUG
    if(focals.elements == NULL){
    	printf("debug: malloc failed in BF_fullChenCombination() for \"focals.elements\".\n");
    }
    #endif
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
    #ifdef DEBUG
    if(combined.focals == NULL){
    	printf("debug: malloc failed in BF_fullChenCombination() for \"combined.focals\".\n");
    }
    #endif
    combined.elementSize = size;
    /*Get the credibility for each body of evidence:*/
    supports = malloc(sizeof(float)*nbM);
    #ifdef DEBUG
    if(supports == NULL){
    	printf("debug: malloc failed in BF_fullChenCombination() for \"supports\".\n");
    }
    #endif
    cred = malloc(sizeof(float)*nbM);
    #ifdef DEBUG
    if(cred == NULL){
    	printf("debug: malloc failed in BF_fullChenCombination() for \"cred\".\n");
    }
    #endif
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
            combined.focals[i].beliefValue += cred[j] * BF_M(m[j], focals.elements[i]);
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
            #ifdef DEBUG
			if(m == NULL){
				printf("debug: malloc failed in BF_combination() for \"m\".\n");
			}
			#endif
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
 * @name Operations on belief functions
 * @{
 */

BF_BeliefFunction BF_conditioning(const BF_BeliefFunction m, const Sets_Element e, const Sets_Set powerset){
    BF_BeliefFunction conditioned = {NULL, 0, 0};
    Sets_Element disj = {NULL, 0};
    Sets_Element opposite = {NULL, 0};
    Sets_Element emptySet;
    int i = 0, j = 0, containVoid = 0;

    /*Get the opposite:*/
    opposite = Sets_getOpposite(e, m.elementSize);

    /*Check if the belief function contain the void element:*/
    emptySet = Sets_getEmptyElement(m.elementSize);
    for(i = 0; i<m.nbFocals; i++){
        if(Sets_equals(m.focals[i].element, emptySet, m.elementSize)){
            containVoid = 1;
        }
    }

    /*Memory allocation:*/
    if(containVoid){
        conditioned.nbFocals = m.nbFocals;
        conditioned.focals = malloc(sizeof(BF_FocalElement )*m.nbFocals);
        #ifdef DEBUG
		if(conditioned.focals == NULL){
			printf("debug: malloc failed in BF_conditioning() for \"conditioned.focals\".\n");
		}
		#endif
		conditioned.elementSize = m.elementSize;
    }
    else {
        conditioned.nbFocals = m.nbFocals + 1;
        conditioned.focals = malloc(sizeof(BF_FocalElement )*(m.nbFocals+1));
        #ifdef DEBUG
		if(conditioned.focals == NULL){
			printf("debug: malloc failed in BF_conditioning() for \"conditioned.focals\".\n");
		}
		#endif
		conditioned.elementSize = m.elementSize;
    }
    /*Check for the new focal elements:*/
    for(i = 0; i<conditioned.nbFocals; i++){
        /*Initialize the value:*/
        if(containVoid){
            conditioned.focals[i].beliefValue = 0;
            conditioned.focals[i].element = Sets_copyElement(m.focals[i].element, m.elementSize);
        }
        else {
            if(i==0){ /*Add the void element:*/
                conditioned.focals[0].beliefValue = 0;
                conditioned.focals[0].element = Sets_copyElement(emptySet, m.elementSize);
            }
            else { /*Consider the element normaly...*/
                conditioned.focals[i].beliefValue = 0;
                conditioned.focals[i].element = Sets_copyElement(m.focals[i-1].element, m.elementSize);
            }
        }
        /*New belief computation:*/
        if(Sets_isSubset(conditioned.focals[i].element, e, m.elementSize)){
            for(j = 0; j<powerset.card; j++){
                if(Sets_isSubset(powerset.elements[j], opposite, m.elementSize)){
                    disj = Sets_disjunction(conditioned.focals[i].element, powerset.elements[j], m.elementSize);
                    conditioned.focals[i].beliefValue += BF_M(m, disj);
                    Sets_freeElement(&disj);
                }
            }
        }
    }
    /*Set to 0 everything that is not in e:*/
    for(i = 0; i<conditioned.nbFocals; i++){
        if(!Sets_isSubset(conditioned.focals[i].element, e, m.elementSize)){
            conditioned.focals[i].beliefValue = 0;
        }
    }

    /*Deallocate:*/
    Sets_freeElement(&opposite);
    Sets_freeElement(&emptySet);
	
	#ifdef CHECK_SUM
    if(BF_checkSum(conditioned)){
        printf("debug: in BF_conditioning(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
    }
    #endif
    #ifdef CHECK_VALUES 
    if(BF_checkValues(conditioned)){
    	printf("debug: in BF_conditioning(), at least one value is not valid!\n");
    }
    #endif

    return conditioned;
}



BF_BeliefFunction BF_weakening(const BF_BeliefFunction m, const float alpha){
    BF_BeliefFunction weakened = {NULL, 0, 0};
    int containVoid = 0, voidIndex = 0;
    int i = 0;
    float sum = 0, realAlpha = 0;
    Sets_Element emptySet;
    
    if(alpha >= 1){
    	realAlpha = 1;
    }
    else {
    	realAlpha = alpha;
    }

    /*Get void:*/
    emptySet = Sets_getEmptyElement(m.elementSize);
    /*Check if the function contain the void element:*/
    for(i = 0; i<m.nbFocals; i++){
        if(Sets_equals(m.focals[i].element, emptySet, m.elementSize)){
            containVoid = 1;
            voidIndex = i;
        }
    }
    /*Weaken:*/
    if(containVoid){
        /*Weaken the believes on elements:*/
        weakened.nbFocals = m.nbFocals;
        weakened.focals = malloc(sizeof(BF_FocalElement) * m.nbFocals);
        #ifdef DEBUG
		if(weakened.focals == NULL){
			printf("debug: malloc failed in BF_weakening() for \"weakened.focals\".\n");
		}
		#endif
		weakened.elementSize = m.elementSize;
        for(i = 0; i<m.nbFocals; i++){
            weakened.focals[i].element = Sets_copyElement(m.focals[i].element, m.elementSize);
            if(i != voidIndex){
                weakened.focals[i].beliefValue = m.focals[i].beliefValue * (1 - realAlpha);
                sum += weakened.focals[i].beliefValue;
            }
        }
        /*Transfer the lost belief on void:*/
        weakened.focals[voidIndex].beliefValue = 1 - sum;
    }
    else {
        weakened.nbFocals = m.nbFocals + 1; /* + void */
        weakened.focals = malloc(sizeof(BF_FocalElement) * (m.nbFocals + 1));
        #ifdef DEBUG
		if(weakened.focals == NULL){
			printf("debug: malloc failed in BF_weakening() for \"weakened.focals\".\n");
		}
		#endif
		weakened.elementSize = m.elementSize;
        /*Put the elements: */
        for(i = 0; i<m.nbFocals; i++){
            weakened.focals[i].element = Sets_copyElement(m.focals[i].element, m.elementSize);
            weakened.focals[i].beliefValue = m.focals[i].beliefValue * (1 - realAlpha);
        }
        /*Transfer the lost belief on void: */
        weakened.focals[m.nbFocals].element = Sets_copyElement(emptySet, m.elementSize);
        weakened.focals[m.nbFocals].beliefValue = realAlpha;
    }

    /*Deallocate: */
    Sets_freeElement(&emptySet);

    #ifdef CHECK_SUM
    if(BF_checkSum(weakened)){
        printf("debug: in BF_weakening(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
    }
    #endif
    #ifdef CHECK_VALUES 
    if(BF_checkValues(weakened)){
    	printf("debug: in BF_weakening(), at least one value is not valid!\n");
    }
    #endif

    return weakened;
}



BF_BeliefFunction BF_discounting(const BF_BeliefFunction m, const float alpha){
    BF_BeliefFunction discounted;
    int containComplete = 0, completeIndex = 0;
    int i = 0;
    float sum = 0, realAlpha = 0;
    Sets_Element emptySet, complete;
    
    if(alpha >= 1){
    	realAlpha = 1;
    }
    else if(alpha <= 0){
    	realAlpha = 0;
    }
    else {
    	realAlpha = alpha;
    }

    /*Get void: */
    emptySet = Sets_getEmptyElement(m.elementSize);
    /*Get the complete set: */
    complete = Sets_getOpposite(emptySet, m.elementSize);
    /*Check if the function contain the complete set element: */
    for(i = 0; i<m.nbFocals; i++){
        if(Sets_equals(m.focals[i].element, complete, m.elementSize)){
            containComplete = 1;
            completeIndex = i;
        }
    }
    /*Discount: */
    if(containComplete){
        /*Discount the believes on elements: */
        discounted.nbFocals = m.nbFocals;
        discounted.focals = malloc(sizeof(BF_FocalElement )*m.nbFocals);
        #ifdef DEBUG
		if(discounted.focals == NULL){
			printf("debug: malloc failed in BF_discounting() for \"discounted.focals\".\n");
		}
		#endif
		discounted.elementSize = m.elementSize;
        for(i = 0; i<m.nbFocals; i++){
            discounted.focals[i].element = Sets_copyElement(m.focals[i].element, m.elementSize);
            if(i != completeIndex){
                discounted.focals[i].beliefValue = m.focals[i].beliefValue * (1 - realAlpha);
                sum += discounted.focals[i].beliefValue;
            }
        }
        /*Transfer the lost belief on complete: */
        discounted.focals[completeIndex].beliefValue = 1 - sum;
    }
    else {
        discounted.nbFocals = m.nbFocals + 1; /* + complete */
        discounted.focals = malloc(sizeof(BF_FocalElement )*(m.nbFocals + 1));
        #ifdef DEBUG
		if(discounted.focals == NULL){
			printf("debug: malloc failed in BF_discounting() for \"discounted.focals\".\n");
		}
		#endif
		discounted.elementSize = m.elementSize;
        /*Put the elements: */
        for(i = 0; i<m.nbFocals; i++){
            discounted.focals[i].element = Sets_copyElement(m.focals[i].element, m.elementSize);
            discounted.focals[i].beliefValue = m.focals[i].beliefValue * (1 - realAlpha);
        }
        /*Transfer the lost belief on complete: */
        discounted.focals[m.nbFocals].element = Sets_copyElement(complete, m.elementSize);
        discounted.focals[m.nbFocals].beliefValue = realAlpha;
    }

    /*Deallocate: */
    Sets_freeElement(&emptySet);
    Sets_freeElement(&complete);

    #ifdef CHECK_SUM
    if(BF_checkSum(discounted)){
        printf("debug: in BF_discounting(), the sum is not equal to 1.\ndebug: There may be a problem in the model.\n");
        printf("debug: alpha = %f\n", alpha);
    }
    #endif
    #ifdef CHECK_VALUES 
    if(BF_checkValues(discounted)){
    	printf("debug: in BF_discounting(), at least one value is not valid!\n");
    	printf("debug: alpha = %f\n", alpha);
    }
    #endif

    return discounted;
}



BF_BeliefFunction BF_difference(const BF_BeliefFunction m1, const BF_BeliefFunction m2){
    BF_BeliefFunction diff = {NULL, 0, 0};
    int i = 0;
    Sets_Set values = {NULL, 0};

	#ifdef CHECK_COMPATIBILITY
    if(m1.elementSize != m2.elementSize){
    	printf("debug: in BF_difference(), the two mass functions aren't defined on the same frame...\n");
    }
    #endif

    /*Get the set of values to considerate: */
    values.elements = malloc(sizeof(Sets_Element) * (m1.nbFocals + m2.nbFocals));
    #ifdef DEBUG
	if(values.elements == NULL){
		printf("debug: malloc failed in BF_difference() for \"values.elements\".\n");
	}
	#endif
    for(i = 0; i<m1.nbFocals; i++){
        values.elements[i] = Sets_copyElement(m1.focals[i].element, m1.elementSize);
        values.card++;
    }
    for(i = 0; i<m2.nbFocals; i++){
        if(!Sets_isMember(m2.focals[i].element, values, m1.elementSize)){
            values.elements[values.card] = Sets_copyElement(m2.focals[i].element, m1.elementSize);
            values.card++;
        }
    }
    /*Allocation: */
    diff.nbFocals = values.card;
    diff.focals = malloc(sizeof(BF_FocalElement) * values.card);
    #ifdef DEBUG
    if(diff.focals == NULL){
		printf("debug: malloc failed in BF_difference() for \"diff.focals\".\n");
	}
	#endif
	diff.elementSize = m1.elementSize;
    /*Get the differences: */
    for(i = 0; i<values.card; i++){
        diff.focals[i].element = Sets_copyElement(values.elements[i], m1.elementSize);
        diff.focals[i].beliefValue = BF_M(m1, values.elements[i]) - BF_M(m2, values.elements[i]);
    }
    /*Deallocation: */
    Sets_freeSet(&values);

    return diff;
}



/** @} */



/**
 * @name Function-and-element-dependant operations
 * @{
 */

float BF_M(const BF_BeliefFunction m, const Sets_Element e){
    int i = 0;

    for(i = 0; i<m.nbFocals; i++){
        if(Sets_equals(e, m.focals[i].element, m.elementSize)){
            return m.focals[i].beliefValue;
        }
    }
    return 0;
}



float BF_bel(const BF_BeliefFunction m, const Sets_Element e){
    float cred = 0;
    int i = 0;
    Sets_Element emptySet;

    /*Get void: */
    emptySet = Sets_getEmptyElement(m.elementSize);
    /*Compute: */
    for(i = 0; i<m.nbFocals; i++){
        if(Sets_isSubset(m.focals[i].element, e, m.elementSize) && !Sets_equals(m.focals[i].element, emptySet, m.elementSize)){
            cred += m.focals[i].beliefValue;
        }
    }

    /*Deallocate: */
    Sets_freeElement(&emptySet);

    return cred;
}



float BF_pl(const BF_BeliefFunction m, const Sets_Element e){
    float plaus = 0;
    int i = 0;
    Sets_Element emptySet;
    Sets_Element conj;

    /*Get void: */
    emptySet = Sets_getEmptyElement(m.elementSize);
    /*Compute: */
    for(i = 0; i<m.nbFocals; i++){
        conj = Sets_conjunction(m.focals[i].element, e, m.elementSize);
        if(!Sets_equals(conj, emptySet, m.elementSize)){
            plaus += m.focals[i].beliefValue;
        }
        Sets_freeElement(&conj);
    }
    /*Deallocate: */
    Sets_freeElement(&emptySet);

    return plaus;
}



float BF_q(const BF_BeliefFunction m, const Sets_Element e){
    float common = 0;
    int i = 0;

    for(i = 0; i<m.nbFocals; i++){
        if(Sets_isSubset(e, m.focals[i].element, m.elementSize)){
            common += m.focals[i].beliefValue;
        }
    }

    return common;
}



float BF_betP(const BF_BeliefFunction m, const Sets_Element e){
    float proba = 0;
    int i = 0;
    Sets_Element conj;
    Sets_Element emptySet;

    /*Get void: */
    emptySet = Sets_getEmptyElement(m.elementSize);
    /*Compute: */
    for(i = 0; i<m.nbFocals; i++){
        if(!Sets_equals(m.focals[i].element, emptySet, m.elementSize)){
            conj = Sets_conjunction(e, m.focals[i].element, m.elementSize);
            proba += m.focals[i].beliefValue * conj.card / m.focals[i].element.card;
            Sets_freeElement(&conj);
        }
    }
    /*Deallocate: */
    Sets_freeElement(&emptySet);

    return proba;
}


/** @} */



/**
 * @name Function-dependant operations
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
    #ifdef DEBUG
    if(voidMasses == NULL){
		printf("debug: malloc failed in BF_autoConflict() for \"voidMasses\".\n");
	}
	#endif
    /*Initialization: */
    temp = BF_SmetsCombination(m, m);
    for(i = 0; i<maxDegree; i++){
        voidMasses[i] = BF_M(temp, emptySet);
        temp2 = temp;
        temp = BF_SmetsCombination(temp, m);
        BF_freeBeliefFunction(&temp2);
    }

    /*Deallocation: */
    BF_freeBeliefFunction(&temp);
    Sets_freeElement(&emptySet);

    return voidMasses;
}



float BF_specificity(const BF_BeliefFunction m){
    float spec = 0;
    int i = 0;
    Sets_Element emptySet;

    /*Get void: */
    emptySet = Sets_getEmptyElement(m.elementSize);
    /*Computation: */
    for(i = 0; i<m.nbFocals; i++){
        if(!Sets_equals(m.focals[i].element, emptySet, m.elementSize)){
            spec += m.focals[i].beliefValue / m.focals[i].element.card;
        }
    }
    /*Deallocation: */
    Sets_freeElement(&emptySet);

    return spec;
}



float BF_nonSpecificity(const BF_BeliefFunction m){
    float nonSpe = 0;
    int i = 0;
    Sets_Element emptySet;

    /*Get void: */
    emptySet = Sets_getEmptyElement(m.elementSize);
    /*Computation: */
    for(i = 0; i<m.nbFocals; i++){
        if(!Sets_equals(m.focals[i].element, emptySet, m.elementSize)){
            nonSpe += m.focals[i].beliefValue * log(m.focals[i].element.card) / log(2);
        }
    }
    /*Deallocation: */
    Sets_freeElement(&emptySet);
    return nonSpe;
}



float BF_discrepancy(const BF_BeliefFunction m){
    float disc = 0;
    int i = 0;

    for(i = 0; i<m.nbFocals; i++){
        disc -= m.focals[i].beliefValue * log(BF_betP(m, m.focals[i].element)) / log(2);
    }

    return disc;
}



float BF_distance(const BF_BeliefFunction m1, const BF_BeliefFunction m2){
    float dist = 0;
    float *temp = NULL;
    float **matrix = NULL;
    int i = 0, j = 0;
    BF_BeliefFunction diff;
    Sets_Element emptySet, conj, disj;
	
	#ifdef CHECK_COMPATIBILITY
    if(m1.elementSize != m2.elementSize){
    	printf("debug: in BF_distance(), the two mass functions aren't defined on the same frame...\n");
    }
    #endif
	
    /*Get void: */
    emptySet = Sets_getEmptyElement(m1.elementSize);

    /*Get differences between the two functions: */
    diff = BF_difference(m1, m2);

    /*Compute the matrix: */
    matrix = malloc(sizeof(float*) * diff.nbFocals);
    #ifdef DEBUG
    if(matrix == NULL){
		printf("debug: malloc failed in BF_distance() for \"matrix\".\n");
	}
	#endif
    for(i = 0; i<diff.nbFocals; i++){
        matrix[i] = malloc(sizeof(float) * diff.nbFocals);
        #ifdef DEBUG
		if(matrix == NULL){
			printf("debug: malloc failed in BF_distance() for \"matrix\".\n");
		}
		#endif
        for(j = 0; j<diff.nbFocals; j++){
            if(!Sets_equals(diff.focals[i].element, emptySet, m1.elementSize) || !Sets_equals(diff.focals[j].element, emptySet, m1.elementSize)){
                disj = Sets_disjunction(diff.focals[i].element, diff.focals[j].element, m1.elementSize);
                conj = Sets_conjunction(diff.focals[i].element, diff.focals[j].element, m1.elementSize);
                matrix[i][j] = (float)conj.card / (float)disj.card;
                Sets_freeElement(&disj);
                Sets_freeElement(&conj);
            }
            else {
                matrix[i][j] = 1;
            }
        }
    }

    /*Compute the distance: */
    temp = malloc(sizeof(float) * diff.nbFocals);
    #ifdef DEBUG
    if(temp == NULL){
		printf("debug: malloc failed in BF_distance() for \"temp\".\n");
	}
	#endif
    for(i = 0; i<diff.nbFocals; i++){
        temp[i] = 0;
        for(j = 0; j<diff.nbFocals; j++){
            temp[i] += diff.focals[j].beliefValue * matrix[i][j];
        }
    }
    for(i = 0; i<diff.nbFocals; i++){
        dist += temp[i] * diff.focals[i].beliefValue;
    }
    dist = sqrt(0.5 * dist);

    /*Deallocate: */
    free(temp);
    for(i = 0; i<diff.nbFocals; i++){
        free(matrix[i]);
    }
    free(matrix);
    Sets_freeElement(&emptySet);
    BF_freeBeliefFunction(&diff);

    return dist;
}



float BF_globalDistance(const BF_BeliefFunction m, const BF_BeliefFunction* s, const int nbBF){
    float conflict = 0;
    int i = 0; 
    
    #ifdef CHECK_COMPATIBILITY
    int size = m.elementSize;
    for(i = 0; i < nbBF; i++){
    	if(s[i].elementSize != size){
    		printf("debug: in BF_globalDistance(), at least one mass function is not compatible with others...\n");
    	}
    }
    #endif
	
    for(i = 0; i<nbBF; i++){
        conflict += BF_distance(m, s[i]);
    }
    conflict /= (nbBF - 1);

    return conflict;
}



float BF_similarity(const BF_BeliefFunction m1, const BF_BeliefFunction m2){
    #ifdef CHECK_COMPATIBILITY
    if(m1.elementSize != m2.elementSize){
    	printf("debug: in BF_distance(), the two mass functions aren't defined on the same frame...\n");
    }
    #endif
    
    return (0.5 * (cos(3.14159 * BF_distance(m1,m2) + 1)));
}



float BF_support(const BF_BeliefFunction ref, const BF_BeliefFunction* m, const int nbM){
    float sup = 0;
    int i = 0; 
    
    #ifdef CHECK_COMPATIBILITY
    int size = ref.elementSize;
    for(i = 0; i < nbM; i++){
    	if(m[i].elementSize != size){
    		printf("debug: in BF_support(), at least one mass function is not compatible with others...\n");
    	}
    }
    #endif

    for(i = 0; i<nbM; i++){
        sup += BF_similarity(ref, m[i]);
    }

    return (sup-1);
}



int BF_checkSum(const BF_BeliefFunction m){
    float sum = 0;
    int i = 0;

    for(i = 0; i<m.nbFocals; i++){
        sum += m.focals[i].beliefValue;
    }

    if(!(1 - BF_PRECISION < sum && sum < 1 + BF_PRECISION)){
        printf("debug: Sum problem: %f\n", sum);
    }

    /*Take into account the precision of the computations... */
    return !(1 - BF_PRECISION < sum && sum < 1 + BF_PRECISION); /*(sum != 1);*/
}



int BF_checkValues(const BF_BeliefFunction m){
	int okay = 0;
	int i = 0;
	int problemIndex = 0;
	
	for(i = 0; i < m.nbFocals; i++){
		okay = okay || (m.focals[i].beliefValue < 0 || m.focals[i].beliefValue > 1 + BF_PRECISION);
		if(m.focals[i].beliefValue < 0 || m.focals[i].beliefValue > 1 + BF_PRECISION){
			problemIndex = i;
		}
	}
	
	if(okay){
		printf("debug: Value problem!\n");
		printf("m(%s) = %f\n", m.focals[problemIndex].element.values, m.focals[problemIndex].beliefValue);
	}
	
	return okay;
}



/** @} */



/**
 * @name Decision support functions
 * @{
 */

BF_FocalElement BF_getMax(BF_criterionFun criterion, const BF_BeliefFunction beliefFunction,
		const int maxCard, const Sets_Set powerset) {
    BF_FocalElement  max = {{NULL,0}, 0};
    int i = 0, maxIndex = -1;
    float value = 0;


    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= maxCard ||
        		maxCard == 0)                         &&
           powerset.elements[i].card > 0 ){
            value = criterion(beliefFunction, powerset.elements[i]);
            if(value > max.beliefValue){
                maxIndex = i;
                max.beliefValue = value;
            }
        }
    }
    if(maxIndex != -1){
        max.element = Sets_copyElement(powerset.elements[maxIndex], beliefFunction.elementSize);
    }

    return max;
}

BF_FocalElement BF_getMin(BF_criterionFun criterion, const BF_BeliefFunction beliefFunction,
		const int maxCard, const Sets_Set powerset) {
    BF_FocalElement  min = {{NULL,0}, 1};
    int i = 0, minIndex = -1;
    float value = 0;

    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= maxCard ||
        		maxCard == 0)                         &&
           powerset.elements[i].card > 0){
            value = criterion(beliefFunction, powerset.elements[i]);
            if(value <= min.beliefValue &&
               value != 0){
                minIndex = i;
                min.beliefValue = value;
            }
        }
    }
    if(minIndex != -1){
        min.element = Sets_copyElement(powerset.elements[minIndex], beliefFunction.elementSize);
    }

    return min;
}

static unsigned int listAppend(BF_FocalElementList *list, const BF_FocalElement element,
		const unsigned int realSize, const int elementSize) {
	BF_FocalElement *newArray;
	unsigned int newSize = realSize;
	if(realSize == list->size) {
		/* increase allocated memory size */
		newSize = (list->size + 1)  * 1.25;
		newArray = realloc(list->elements, sizeof(BF_FocalElement) * newSize);
		#ifdef DEBUG
		if(NULL == newArray) {
			fprintf(stderr, "debug: realloc failed in listAppend() line %d.\n", __LINE__);
			return realSize;
		}
		#endif /* DEBUG */
		list->elements = newArray;
	}
	list->elements[list->size].beliefValue = element.beliefValue;
	list->elements[list->size].element = Sets_copyElement(element.element, elementSize);
	list->size++;
	return newSize;
}

static BF_FocalElementList newList() {
	BF_FocalElementList newList = {NULL, 0};
	return newList;
}

static void emptyList(BF_FocalElementList *list) {
	unsigned int i;
	for (i = 0; i < list->size; ++i) {
		Sets_freeElement(&(list->elements[i].element));
	}
	list->size = 0;
}

void BF_freeFocalElementList(BF_FocalElementList *list) {
	unsigned int i;
	for (i = 0; i < list->size; ++i) {
		Sets_freeElement(&(list->elements[i].element));
	}
	free(list->elements);
	list->size = 0;
}

BF_FocalElementList BF_getMaxList(BF_criterionFun criterion, const BF_BeliefFunction beliefFunction,
		const int maxCard, const Sets_Set powerset) {
	BF_FocalElementList  list = newList();
	unsigned int listSize = 0;

    BF_FocalElement  max = {{NULL,0}, 0};
	int i = 0;
	float value = 0;


	for(i = 0; i < powerset.card; i++){
		if((powerset.elements[i].card <= maxCard ||
				maxCard == 0)                         &&
		   powerset.elements[i].card > 0 ){
			value = criterion(beliefFunction, powerset.elements[i]);

			if(value > max.beliefValue){
				emptyList(&list);
				max.element = powerset.elements[i];
				max.beliefValue = value;
				listSize = listAppend(&list, max, listSize, beliefFunction.elementSize);
			}
			else if(value == max.beliefValue && value > 0) {
				max.element = powerset.elements[i];
				listSize = listAppend(&list, max, listSize, beliefFunction.elementSize);
			}
		}
	}

	return list;
}


BF_FocalElement  BF_getMaxMass(const BF_BeliefFunction m, const int card){
    BF_FocalElement  max = {{NULL,0}, 0};
    int i = 0, maxIndex = -1;

    for(i = 0; i < m.nbFocals; i++){
        if(m.focals[i].beliefValue > max.beliefValue &&
           (m.focals[i].element.card <= card         ||
           card == 0)                                &&
           m.focals[i].element.card > 0){
            maxIndex = i;
            max.beliefValue = m.focals[i].beliefValue;
        }
    }
    if(maxIndex != -1){
        max.element = Sets_copyElement(m.focals[maxIndex].element, m.elementSize);
    }

    return max;
}



BF_FocalElement  BF_getMinMass(const BF_BeliefFunction m, const int card){
    BF_FocalElement  min = {{NULL,0}, 1};
    int i = 0, minIndex = -1;

    for(i = 0; i < m.nbFocals; i++){
        if(m.focals[i].beliefValue <= min.beliefValue &&
           m.focals[i].beliefValue != 0               &&
           (m.focals[i].element.card <= card          ||
           card == 0)                                 &&
           m.focals[i].element.card > 0){
            minIndex = i;
            min.beliefValue = m.focals[i].beliefValue;
        }
    }
    if(minIndex != -1){
        min.element = Sets_copyElement(m.focals[minIndex].element, m.elementSize);
    }

    return min;
}



BF_FocalElement  BF_getMaxBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    BF_FocalElement  max = {{NULL,0}, 0};
    int i = 0, maxIndex = -1;
    float value = 0;


    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
           powerset.elements[i].card > 0 ){
            value = BF_bel(m, powerset.elements[i]);
            if(value > max.beliefValue){
                maxIndex = i;
                max.beliefValue = value;
            }
        }
    }
    if(maxIndex != -1){
        max.element = Sets_copyElement(powerset.elements[maxIndex], m.elementSize);
    }

    return max;
}



BF_FocalElement  BF_getMinBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    BF_FocalElement  min = {{NULL,0}, 1};
    int i = 0, minIndex = -1;
    float value = 0;

    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
           powerset.elements[i].card > 0){
            value = BF_bel(m, powerset.elements[i]);
            if(value <= min.beliefValue &&
               value != 0){
                minIndex = i;
                min.beliefValue = value;
            }
        }
    }
    if(minIndex != -1){
        min.element = Sets_copyElement(powerset.elements[minIndex], m.elementSize);
    }

    return min;
}



BF_FocalElement  BF_getMaxPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    BF_FocalElement  max = {{NULL,0}, 0};
    int i = 0, maxIndex = -1;
    float value = 0;

    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
           powerset.elements[i].card > 0){
            value = BF_pl(m, powerset.elements[i]);
            if(value > max.beliefValue){
                maxIndex = i;
                max.beliefValue = value;
            }
        }
    }
    if(maxIndex != -1){
        max.element = Sets_copyElement(powerset.elements[maxIndex], m.elementSize);
    }

    return max;
}



BF_FocalElement  BF_getMinPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    BF_FocalElement  min = {{NULL,0}, 1};
    int i = 0, minIndex = -1;
    float value = 0;

    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
           powerset.elements[i].card > 0){
            value = BF_pl(m, powerset.elements[i]);
            if(value <= min.beliefValue &&
               value != 0){
                minIndex = i;
                min.beliefValue = value;
            }
        }
    }
    if(minIndex != -1){
        min.element = Sets_copyElement(powerset.elements[minIndex], m.elementSize);
    }

    return min;
}



BF_FocalElement  BF_getMaxBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    BF_FocalElement  max = {{NULL,0}, 0};
    int i = 0, maxIndex = -1;
    float value = 0;

    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
           powerset.elements[i].card > 0){
            value = BF_betP(m, powerset.elements[i]);
            if(value > max.beliefValue){
                maxIndex = i;
                max.beliefValue = value;
            }
        }
    }
    if(maxIndex != -1){
        max.element = Sets_copyElement(powerset.elements[maxIndex], m.elementSize);
    }

    return max;
}



BF_FocalElement  BF_getMinBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    BF_FocalElement  min = {{NULL,0}, 1};
    int i = 0, minIndex = -1;
    float value = 0;

    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
           powerset.elements[i].card > 0){
            value = BF_betP(m, powerset.elements[i]);
            if(value <= min.beliefValue &&
               value != 0){
                minIndex = i;
                min.beliefValue = value;
            }
        }
    }
    if(minIndex != -1){
        min.element = Sets_copyElement(powerset.elements[minIndex], m.elementSize);
    }

    return min;
}



int BF_getNbMaxMass(const BF_BeliefFunction m, const int card){
    int nbMax = 0;
    BF_FocalElement  max;

    max = BF_getMaxMass(m, card);
    nbMax = BF_getQuickNbMaxMass(m, card, max.beliefValue);
    BF_freeBeliefPoint(&max);

    return nbMax;
}



int BF_getNbMinMass(const BF_BeliefFunction m, const int card){
    int nbMin = 0;
    BF_FocalElement  min;

    min = BF_getMinMass(m, card);
    nbMin = BF_getQuickNbMinMass(m, card, min.beliefValue);

    BF_freeBeliefPoint(&min);

    return nbMin;
}



int BF_getNbMaxBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    int nbMax = 0;
    BF_FocalElement  max;

    max = BF_getMaxBel(m, card, powerset);
    nbMax = BF_getQuickNbMaxBel(m, card, powerset, max.beliefValue);

    BF_freeBeliefPoint(&max);

    return nbMax;
}



int BF_getNbMinBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    int nbMin = 0;
    BF_FocalElement  min;

    min = BF_getMinBel(m, card, powerset);
    nbMin = BF_getQuickNbMinBel(m, card, powerset, min.beliefValue);

    BF_freeBeliefPoint(&min);

    return nbMin;
}



int BF_getNbMaxPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    int nbMax = 0;
    BF_FocalElement  max;

    max = BF_getMaxPl(m, card, powerset);
    nbMax = BF_getQuickNbMaxPl(m, card, powerset, max.beliefValue);

    BF_freeBeliefPoint(&max);

    return nbMax;
}



int BF_getNbMinPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    int nbMin = 0;
    BF_FocalElement  min;

    min = BF_getMinPl(m, card, powerset);
    nbMin = BF_getQuickNbMinPl(m, card, powerset, min.beliefValue);

    BF_freeBeliefPoint(&min);

    return nbMin;
}



int BF_getNbMaxBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    int nbMax = 0;
    BF_FocalElement  max;

    max = BF_getMaxBetP(m, card, powerset);
    nbMax = BF_getQuickNbMaxBetP(m, card, powerset, max.beliefValue);

    BF_freeBeliefPoint(&max);

    return nbMax;
}



int BF_getNbMinBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    int nbMin = 0;
    BF_FocalElement  min;

    min = BF_getMinBetP(m, card, powerset);
    nbMin = BF_getQuickNbMinBetP(m, card, powerset, min.beliefValue);

    BF_freeBeliefPoint(&min);

    return nbMin;
}



int BF_getQuickNbMaxMass(const BF_BeliefFunction m, const int card, float maxValue){
    int nbMax = 0;
    int i = 0;

    for(i = 0; i < m.nbFocals; i++){
        if(m.focals[i].beliefValue == maxValue &&
           (m.focals[i].element.card <= card   ||
            card == 0)                         &&
           m.focals[i].element.card > 0){
            nbMax++;
        }
    }

    return nbMax;
}



int BF_getQuickNbMinMass(const BF_BeliefFunction m, const int card, float minValue){
    int nbMin = 0;
    int i = 0;

    for(i = 0; i < m.nbFocals; i++){
        if(m.focals[i].beliefValue == minValue &&
           (m.focals[i].element.card <= card   ||
           card == 0)                          &&
           m.focals[i].element.card > 0){
            nbMin++;
        }
    }

    return nbMin;
}



int BF_getQuickNbMaxBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset, float maxValue){
    int nbMax = 0;
    int i = 0;

    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
           powerset.elements[i].card > 0){
            if(BF_bel(m, powerset.elements[i]) == maxValue){
                nbMax++;
            }
        }
    }

    return nbMax;
}



int BF_getQuickNbMinBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset, float minValue){
    int nbMin = 0;
    int i = 0;

    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
           powerset.elements[i].card > 0){
            if(BF_bel(m, powerset.elements[i]) == minValue){
                nbMin++;
            }
        }
    }

    return nbMin;
}



int BF_getQuickNbMaxPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset, float maxValue){
    int nbMax = 0;
    int i = 0;

    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
           powerset.elements[i].card > 0){
            if(BF_pl(m, powerset.elements[i]) == maxValue){
                nbMax++;
            }
        }
    }

    return nbMax;
}



int BF_getQuickNbMinPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset, float minValue){
    int nbMin = 0;
    int i = 0;

    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
           powerset.elements[i].card > 0){
            if(BF_pl(m, powerset.elements[i]) == minValue){
                nbMin++;
            }
        }
    }

    return nbMin;
}



int BF_getQuickNbMaxBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset, float maxValue){
    int nbMax = 0;
    int i = 0;

    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
           powerset.elements[i].card > 0){
            if(BF_betP(m, powerset.elements[i]) == maxValue){
                nbMax++;
            }
        }
    }

    return nbMax;
}



int BF_getQuickNbMinBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset, float minValue){
    int nbMin = 0;
    int i = 0;

    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
           powerset.elements[i].card > 0){
            if(BF_betP(m, powerset.elements[i]) == minValue){
                nbMin++;
            }
        }
    }

    return nbMin;
}



BF_FocalElement * BF_getListMaxMass(const BF_BeliefFunction m, const int card){
    BF_FocalElement  *list = NULL;
    BF_FocalElement  max;

    max = BF_getMaxMass(m, card);
    if(max.element.values != NULL){
        list = BF_getQuickListMaxMass(m, card, max.beliefValue);
        BF_freeBeliefPoint(&max);
    }

    return list;
}



BF_FocalElement * BF_getListMinMass(const BF_BeliefFunction m, const int card){
    BF_FocalElement  *list = NULL;
    BF_FocalElement  min;

    min = BF_getMinMass(m, card);
    if(min.element.values != NULL){
        list = BF_getQuickListMinMass(m, card, min.beliefValue);
        BF_freeBeliefPoint(&min);
    }

    return list;
}



BF_FocalElement * BF_getListMaxBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    BF_FocalElement  *list = NULL;
    BF_FocalElement  max;

    max = BF_getMaxBel(m, card, powerset);
    if(max.element.values != NULL){
        list = BF_getQuickListMaxBel(m, card, powerset, max.beliefValue);
        BF_freeBeliefPoint(&max);
    }

    return list;
}



BF_FocalElement * BF_getListMinBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    BF_FocalElement  *list = NULL;
    BF_FocalElement  min;

    min = BF_getMinBel(m, card, powerset);
    if(min.element.values != NULL){
        list = BF_getQuickListMinBel(m, card, powerset, min.beliefValue);
        BF_freeBeliefPoint(&min);
    }

    return list;
}



BF_FocalElement * BF_getListMaxPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    BF_FocalElement  *list = NULL;
    BF_FocalElement  max;

    max = BF_getMaxPl(m, card, powerset);
    if(max.element.values != NULL){
        list = BF_getQuickListMaxPl(m, card, powerset, max.beliefValue);
        BF_freeBeliefPoint(&max);
    }

    return list;
}



BF_FocalElement * BF_getListMinPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    BF_FocalElement  *list = NULL;
    BF_FocalElement  min;

    min = BF_getMinPl(m, card, powerset);
    if(min.element.values != NULL){
        list = BF_getQuickListMinPl(m, card, powerset, min.beliefValue);
        BF_freeBeliefPoint(&min);
    }

    return list;
}



BF_FocalElement * BF_getListMaxBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    BF_FocalElement  *list = NULL;
    BF_FocalElement  max;

    max = BF_getMaxBetP(m, card, powerset);
    if(max.element.values != NULL){
        list = BF_getQuickListMaxBetP(m, card, powerset, max.beliefValue);
        BF_freeBeliefPoint(&max);
    }

    return list;
}



BF_FocalElement * BF_getListMinBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset){
    BF_FocalElement  *list = NULL;
    BF_FocalElement  min;

    min = BF_getMinBetP(m, card, powerset);
    if(min.element.values != NULL){
        list = BF_getQuickListMinBetP(m, card, powerset, min.beliefValue);
        BF_freeBeliefPoint(&min);
    }

    return list;
}



BF_FocalElement * BF_getQuickListMaxMass(const BF_BeliefFunction m, const int card, const float maxValue){
    int nbMax = 0;
    BF_FocalElement * list = NULL;

    nbMax = BF_getQuickNbMaxMass(m, card, maxValue);
    if(nbMax){
        list = BF_getQuickerListMaxMass(m, card, maxValue, nbMax);
    }
    return list;
}



BF_FocalElement * BF_getQuickListMinMass(const BF_BeliefFunction m, const int card, const float minValue){
    int nbMin = 0;
    BF_FocalElement * list = NULL;

    nbMin = BF_getQuickNbMinMass(m, card, minValue);
    if(nbMin){
        list = BF_getQuickerListMinMass(m, card, minValue, nbMin);
    }
    return list;
}



BF_FocalElement * BF_getQuickListMaxBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float maxValue){
    int nbMax = 0;
    BF_FocalElement * list = NULL;

    nbMax = BF_getQuickNbMaxBel(m, card, powerset, maxValue);
    if(nbMax){
        list = BF_getQuickerListMaxBel(m, card, powerset, maxValue, nbMax);
    }
    return list;
}



BF_FocalElement * BF_getQuickListMinBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float minValue){
    int nbMin = 0;
    BF_FocalElement * list = NULL;

    nbMin = BF_getQuickNbMinBel(m, card, powerset, minValue);
    if(nbMin){
        list = BF_getQuickerListMinBel(m, card, powerset, minValue, nbMin);
    }
    return list;
}



BF_FocalElement * BF_getQuickListMaxPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float maxValue){
    int nbMax = 0;
    BF_FocalElement * list = NULL;

    nbMax = BF_getQuickNbMaxPl(m, card, powerset, maxValue);
    if(nbMax){
        list = BF_getQuickerListMaxPl(m, card, powerset, maxValue, nbMax);
    }
    return list;
}



BF_FocalElement * BF_getQuickListMinPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float minValue){
    int nbMin = 0;
    BF_FocalElement * list = NULL;

    nbMin = BF_getQuickNbMinPl(m, card, powerset, minValue);
    if(nbMin){
        list = BF_getQuickerListMinPl(m, card, powerset, minValue, nbMin);
    }
    return list;
}



BF_FocalElement * BF_getQuickListMaxBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float maxValue){
    int nbMax = 0;
    BF_FocalElement * list = NULL;

    nbMax = BF_getQuickNbMaxBetP(m, card, powerset, maxValue);
    if(nbMax){
        list = BF_getQuickerListMaxBetP(m, card, powerset, maxValue, nbMax);
    }
    return list;
}



BF_FocalElement * BF_getQuickListMinBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float minValue){
    int nbMin = 0;
    BF_FocalElement * list = NULL;

    nbMin = BF_getQuickNbMinBetP(m, card, powerset, minValue);
    if(nbMin){
        list = BF_getQuickerListMinBetP(m, card, powerset, minValue, nbMin);
    }
    return list;
}



BF_FocalElement * BF_getQuickerListMaxMass(const BF_BeliefFunction m, const int card, const float maxValue, const int nbMax){
    BF_FocalElement  *list = NULL;
    int i = 0;
    int index = 0;

    list = malloc(sizeof(BF_FocalElement ) * nbMax);
    #ifdef DEBUG
    if(list == NULL){
		printf("debug: malloc failed in BF_getQuickerListMaxMass() for \"list\".\n");
	}
	#endif
    for(i = 0; i < m.nbFocals; i++){
        if(m.focals[i].beliefValue == maxValue &&
           (m.focals[i].element.card <= card   ||
           card == 0)                          &&
           m.focals[i].element.card > 0){
            list[index].element = Sets_copyElement(m.focals[i].element, m.elementSize);
            list[index].beliefValue = maxValue;
            index++;
        }
    }

    return list;
}



BF_FocalElement * BF_getQuickerListMinMass(const BF_BeliefFunction m, const int card, const float minValue, const int nbMin){
    BF_FocalElement  *list = NULL;
    int i = 0;
    int index = 0;

    list = malloc(sizeof(BF_FocalElement) * nbMin);
    #ifdef DEBUG
    if(list == NULL){
		printf("debug: malloc failed in BF_getQuickerListMinMass() for \"list\".\n");
	}
	#endif
    for(i = 0; i < m.nbFocals; i++){
        if(m.focals[i].beliefValue == minValue &&
           (m.focals[i].element.card <= card   ||
           card == 0)                          &&
           m.focals[i].element.card > 0){
            list[index].element = Sets_copyElement(m.focals[i].element, m.elementSize);
            list[index].beliefValue = minValue;
            index++;
        }
    }

    return list;
}



BF_FocalElement * BF_getQuickerListMaxBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float maxValue, const int nbMax){
    BF_FocalElement  *list = NULL;
    int i = 0;
    int index = 0;

    list = malloc(sizeof(BF_FocalElement ) * nbMax);
    #ifdef DEBUG
    if(list == NULL){
		printf("debug: malloc failed in BF_getQuickerListMaxBel() for \"list\".\n");
	}
	#endif
    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
            powerset.elements[i].card > 0){
            if(BF_bel(m, powerset.elements[i]) == maxValue){
                list[index].element = Sets_copyElement(powerset.elements[i], m.elementSize);
                list[index].beliefValue = maxValue;
                index++;
            }
        }
    }

    return list;
}



BF_FocalElement * BF_getQuickerListMinBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float minValue, const int nbMin){
    BF_FocalElement  *list = NULL;
    int i = 0;
    int index = 0;

    list = malloc(sizeof(BF_FocalElement ) * nbMin);
    #ifdef DEBUG
    if(list == NULL){
		printf("debug: malloc failed in BF_getQuickerListMinBel() for \"list\".\n");
	}
	#endif
    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
            powerset.elements[i].card > 0){
            if(BF_bel(m, powerset.elements[i]) == minValue){
                list[index].element = Sets_copyElement(powerset.elements[i], m.elementSize);
                list[index].beliefValue = minValue;
                index++;
            }
        }
    }

    return list;
}



BF_FocalElement * BF_getQuickerListMaxPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float maxValue, const int nbMax){
    BF_FocalElement  *list = NULL;
    int i = 0;
    int index = 0;

    list = malloc(sizeof(BF_FocalElement ) * nbMax);
    #ifdef DEBUG
    if(list == NULL){
		printf("debug: malloc failed in BF_getQuickerListMaxPl() for \"list\".\n");
	}
	#endif
    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
            powerset.elements[i].card > 0){
            if(BF_pl(m, powerset.elements[i]) == maxValue){
                list[index].element = Sets_copyElement(powerset.elements[i], m.elementSize);
                list[index].beliefValue = maxValue;
                index++;
            }
        }
    }

    return list;
}



BF_FocalElement * BF_getQuickerListMinPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float minValue, const int nbMin){
    BF_FocalElement  *list = NULL;
    int i = 0;
    int index = 0;

    list = malloc(sizeof(BF_FocalElement ) * nbMin);
    #ifdef DEBUG
    if(list == NULL){
		printf("debug: malloc failed in BF_getQuickerListMinPl() for \"list\".\n");
	}
	#endif
    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
            powerset.elements[i].card > 0){
            if(BF_pl(m, powerset.elements[i]) == minValue){
                list[index].element = Sets_copyElement(powerset.elements[i], m.elementSize);
                list[index].beliefValue = minValue;
                index++;
            }
        }
    }

    return list;
}



BF_FocalElement * BF_getQuickerListMaxBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float maxValue, const int nbMax){
    BF_FocalElement  *list = NULL;
    int i = 0;
    int index = 0;

    list = malloc(sizeof(BF_FocalElement) * nbMax);
    #ifdef DEBUG
    if(list == NULL){
		printf("debug: malloc failed in BF_getQuickerListMaxBetP() for \"list\".\n");
	}
	#endif
    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
            powerset.elements[i].card > 0){
            if(BF_betP(m, powerset.elements[i]) == maxValue){
                list[index].element = Sets_copyElement(powerset.elements[i], m.elementSize);
                list[index].beliefValue = maxValue;
                index++;
            }
        }
    }

    return list;
}



BF_FocalElement * BF_getQuickerListMinBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float minValue, const int nbMin){
    BF_FocalElement  *list = NULL;
    int i = 0;
    int index = 0;

    list = malloc(sizeof(BF_FocalElement) * nbMin);
    #ifdef DEBUG
    if(list == NULL){
		printf("debug: malloc failed in BF_getQuickerListMinBetP() for \"list\".\n");
	}
	#endif
    for(i = 0; i < powerset.card; i++){
        if((powerset.elements[i].card <= card ||
           card == 0)                         &&
            powerset.elements[i].card > 0){
            if(BF_betP(m, powerset.elements[i]) == minValue){
                list[index].element = Sets_copyElement(powerset.elements[i], m.elementSize);
                list[index].beliefValue = minValue;
                index++;
            }
        }
    }

    return list;
}



/** @} */




/**
 * @name Memory deallocation
 * @{
 */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !!! Deallocate memory given to beliefs !!!
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */


void BF_freeBeliefFunction(BF_BeliefFunction* bf){
    int i = 0;

    for(i = 0; i<bf->nbFocals; i++){
        BF_freeBeliefPoint(&(bf->focals[i]));
    }

    free(bf->focals);
}



void BF_freeBeliefPoint(BF_FocalElement  *bp){
    Sets_freeElement(&(bp->element));
}


/** @} */



/**
 * @name Conversion to string
 * @{
 */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !!! Conversion into strings !!!
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */


char* BF_beliefFunctionToString(const BF_BeliefFunction bf, const Sets_ReferenceList rl){
    char *str = NULL, *elem = NULL;
    char temp[MAX_STR_LEN];
    int i = 0, totChar = 0;

    /*Count the chars for the focals: */
    for(i = 0; i<bf.nbFocals; i++){
        elem = Sets_elementToString(bf.focals[i].element, rl);
        sprintf(temp, "m(%s) = %f\n", elem, bf.focals[i].beliefValue);
        totChar += strlen(temp);
        free(elem);
        elem = NULL;
    }
    /*Memory allocation: */
    str = malloc(sizeof(char)*(totChar+1));
    #ifdef DEBUG
    if(str == NULL){
        printf("debug: malloc failed in beliefFunctionToString() for \"str\".\n");
        return NULL;
    }
    #endif
    /*Fill the string: */
    elem = Sets_elementToString(bf.focals[0].element, rl);
    sprintf(str, "m(%s) = %f\n", elem, bf.focals[0].beliefValue);
    free(elem);
    elem = NULL;
    for(i = 1; i<bf.nbFocals; i++){
        elem = Sets_elementToString(bf.focals[i].element, rl);
        sprintf(temp, "m(%s) = %f\n", elem, bf.focals[i].beliefValue);
        strcat(str, temp);
        free(elem);
        elem = NULL;
    }
    /*Add the end of str char: */
    strcat(str, "\0");

    return str;
}



char* BF_beliefFunctionToBitString(const BF_BeliefFunction bf){
	char *str = NULL, *elem = NULL;
    char temp[MAX_STR_LEN];
    int i = 0, totChar = 0;

    /*Count the chars for the focals: */
    for(i = 0; i<bf.nbFocals; i++){
        elem = Sets_elementToBitString(bf.focals[i].element, bf.elementSize);
        sprintf(temp, "m(%s) = %f\n", elem, bf.focals[i].beliefValue);
        totChar += strlen(temp);
        free(elem);
        elem = NULL;
    }
    /*Memory allocation: */
    str = malloc(sizeof(char)*(totChar+1));
    #ifdef DEBUG
    if(str == NULL){
        printf("debug: malloc failed in beliefFunctionToBitString() for \"str\".\n");
        return NULL;
    }
    #endif
    /*Fill the string: */
    elem = Sets_elementToBitString(bf.focals[0].element, bf.elementSize);
    sprintf(str, "m(%s) = %f\n", elem, bf.focals[0].beliefValue);
    free(elem);
    elem = NULL;
    for(i = 1; i < bf.nbFocals; i++){
        elem = Sets_elementToBitString(bf.focals[i].element, bf.elementSize);
        sprintf(temp, "m(%s) = %f\n", elem, bf.focals[i].beliefValue);
        strcat(str, temp);
        free(elem);
        elem = NULL;
    }
    /*Add the end of str char: */
    strcat(str, "\0");

    return str;
}

/** @} */

