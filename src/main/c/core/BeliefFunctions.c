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


#include "BeliefFunctions.h"

/**
 * This module does not enable the building of belief functions but only to manipulate them!
 * Thus, this module offers many functions to characterize, combine and discount belief functions.
 *
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
    DEBUG_CHECK_MALLOC(copy.focals);

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
	
	vacuous.nbFocals = 1;
	vacuous.focals = malloc(sizeof(BF_FocalElement ));
	DEBUG_CHECK_MALLOC(vacuous.focals);

    vacuous.elementSize = elementSize;
	
	vacuous.focals[0].element = Sets_getCompleteElement(elementSize);
	vacuous.focals[0].beliefValue = 1;
	
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
	DEBUG_CHECK_MALLOC(focals);
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
        if(m.focals[i].element.card == 0){
            containVoid = 1;
        }
    }

    /*Memory allocation:*/
    if(containVoid){
        conditioned.nbFocals = m.nbFocals;
        conditioned.focals = malloc(sizeof(BF_FocalElement )*m.nbFocals);
        DEBUG_CHECK_MALLOC(conditioned.focals);

		conditioned.elementSize = m.elementSize;
    }
    else {
        conditioned.nbFocals = m.nbFocals + 1;
        conditioned.focals = malloc(sizeof(BF_FocalElement )*(m.nbFocals+1));
        DEBUG_CHECK_MALLOC(conditioned.focals);

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
                    conditioned.focals[i].beliefValue += BF_m(m, disj);
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
        if(m.focals[i].element.card == 0){
            containVoid = 1;
            voidIndex = i;
        }
    }
    /*Weaken:*/
    if(containVoid){
        /*Weaken the believes on elements:*/
        weakened.nbFocals = m.nbFocals;
        weakened.focals = malloc(sizeof(BF_FocalElement) * m.nbFocals);
        DEBUG_CHECK_MALLOC(weakened.focals);

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
        DEBUG_CHECK_MALLOC(weakened.focals);

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
    Sets_Element complete;
    
    if(alpha >= 1){
    	realAlpha = 1;
    }
    else if(alpha <= 0){
    	realAlpha = 0;
    }
    else {
    	realAlpha = alpha;
    }

    /*Get the complete set: */
    complete = Sets_getCompleteElement(m.elementSize);
    /*Check if the function contain the complete set element: */
    for(i = 0; i<m.nbFocals; i++){
        if(m.focals[i].element.card == m.elementSize){
            containComplete = 1;
            completeIndex = i;
        }
    }
    /*Discount: */
    if(containComplete){
        /*Discount the believes on elements: */
        discounted.nbFocals = m.nbFocals;
        discounted.focals = malloc(sizeof(BF_FocalElement )*m.nbFocals);
        DEBUG_CHECK_MALLOC(discounted.focals);

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
        DEBUG_CHECK_MALLOC(discounted.focals);

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
    DEBUG_CHECK_MALLOC(values.elements);

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
    DEBUG_CHECK_MALLOC(diff.focals);

	diff.elementSize = m1.elementSize;
    /*Get the differences: */
    for(i = 0; i<values.card; i++){
        diff.focals[i].element = Sets_copyElement(values.elements[i], m1.elementSize);
        diff.focals[i].beliefValue = BF_m(m1, values.elements[i]) - BF_m(m2, values.elements[i]);
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

float BF_m(const BF_BeliefFunction m, const Sets_Element e){
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
    
    /*Compute: */
    for(i = 0; i<m.nbFocals; i++){
        if(Sets_isSubset(m.focals[i].element, e, m.elementSize) && m.focals[i].element.card > 0){
            cred += m.focals[i].beliefValue;
        }
    }

    return cred;
}



float BF_pl(const BF_BeliefFunction m, const Sets_Element e){
    float plaus = 0;
    int i = 0;
    Sets_Element conj;

    /*Compute: */
    for(i = 0; i<m.nbFocals; i++){
        conj = Sets_conjunction(m.focals[i].element, e, m.elementSize);
        if(conj.card > 0){
            plaus += m.focals[i].beliefValue;
        }
        Sets_freeElement(&conj);
    }
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
    
    /*Compute: */
    for(i = 0; i<m.nbFocals; i++){
        if(m.focals[i].element.card > 0){
            conj = Sets_conjunction(e, m.focals[i].element, m.elementSize);
            proba += m.focals[i].beliefValue * conj.card / m.focals[i].element.card;
            Sets_freeElement(&conj);
        }
    }

    return proba;
}


/** @} */



/**
 * @name Function-dependant operations
 * @{
 */




float BF_specificity(const BF_BeliefFunction m){
    float spec = 0;
    int i = 0;
    
    /*Computation: */
    for(i = 0; i<m.nbFocals; i++){
        if(m.focals[i].element.card > 0){
            spec += m.focals[i].beliefValue / m.focals[i].element.card;
        }
    }

    return spec;
}



float BF_nonSpecificity(const BF_BeliefFunction m){
    float nonSpe = 0;
    int i = 0;
   	
    /*Computation: */
    for(i = 0; i<m.nbFocals; i++){
        if(m.focals[i].element.card > 0){
            nonSpe += m.focals[i].beliefValue * log(m.focals[i].element.card) / log(2);
        }
    }
    
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
    /*Sets_Element emptySet, conj, disj;*/
    Sets_Element conj, disj;
	
	#ifdef CHECK_COMPATIBILITY
    if(m1.elementSize != m2.elementSize){
    	printf("debug: in BF_distance(), the two mass functions aren't defined on the same frame...\n");
    }
    #endif
	
    /*Get void: */
    /*emptySet = Sets_getEmptyElement(m1.elementSize); */

    /*Get differences between the two functions: */
    diff = BF_difference(m1, m2);

    /*Compute the matrix: */
    matrix = malloc(sizeof(float*) * diff.nbFocals);
    DEBUG_CHECK_MALLOC(matrix);

    for(i = 0; i<diff.nbFocals; i++){
        matrix[i] = malloc(sizeof(float) * diff.nbFocals);
        DEBUG_CHECK_MALLOC(matrix);

        for(j = 0; j<diff.nbFocals; j++){
            /*if(!Sets_equals(diff.focals[i].element, emptySet, m1.elementSize) || !Sets_equals(diff.focals[j].element, emptySet, m1.elementSize)){ */
            if(diff.focals[i].element.card > 0 || diff.focals[j].element.card > 0){
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
    DEBUG_CHECK_MALLOC(temp);

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
    /*Sets_freeElement(&emptySet); */
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
    DEBUG_CHECK_MALLOC_OR_RETURN(str, NULL);

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
    DEBUG_CHECK_MALLOC_OR_RETURN(str, NULL);

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

