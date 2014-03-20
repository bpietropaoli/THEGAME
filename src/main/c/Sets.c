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



#include "Sets.h"


/**
 * @file Sets.c
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief CORE: Implements the basics
 * required to work on sets and elements
 * in the theory of belief functions.
 */

/**
 * @name Reference lists
 * @{
 */

Sets_ReferenceList Sets_loadRefList(const char* fileName){
    Sets_ReferenceList loadedList = {NULL, 0};
    int nbLines = 0, i = 0, card = 0, current = 0;
    int* charPerLine = NULL;
    char** lines = NULL;

    /*Get the lines: */
    nbLines = ReadFile_countLines(fileName);
    if(nbLines == 0){
    	return loadedList;
    }
    charPerLine = ReadFile_charsPerLine(fileName,nbLines);
    lines = ReadFile_readLines(fileName, nbLines, charPerLine);

	/*Get the number of real lines: */
	card = nbLines;
	for(i = 0; i < nbLines; i++){
		if(charPerLine[i] == 0){
			card -= 1;
		}
	}

    /*Memory allocation: */
    loadedList.card = card;
    loadedList.values = malloc(sizeof(char*) * loadedList.card);
    DEBUG_CHECK_MALLOC(loadedList.values);

    for(i = 0; i < nbLines; i++){
    	if(charPerLine[i] != 0){
		    loadedList.values[current] = malloc(sizeof(char) * charPerLine[i] + 1);
		    DEBUG_CHECK_MALLOC(loadedList.values[current]);

		    strcpy(loadedList.values[current], lines[i]);
		    strcat(loadedList.values[current], "\0");
		    current++;
		}
    }

    /*Free: */
    free(charPerLine);
    for(i = 0; i<nbLines; i++){
        free(lines[i]);
    }
    free(lines);

    return loadedList;
}

/** @} */

/*
 +----------------+
 | Set operations |
 +----------------+
*/

/**
 * @name Set and element creation
 * @{
 */

Sets_Set Sets_createSetFromRefList(const Sets_ReferenceList rl){
    return Sets_createSet(rl.card);
}

Sets_Set Sets_createSet(const int nbAtoms){
	Sets_Set createdSet = {NULL, 0};
    int i = 0, j = 0;
    
    createdSet.card = nbAtoms;
    createdSet.elements = malloc(sizeof(Sets_Element) * createdSet.card);
    DEBUG_CHECK_MALLOC(createdSet.elements);
    
    /*Create the elements: */
    for(i = 0; i < createdSet.card; i++){
        createdSet.elements[i].card = 1;
        createdSet.elements[i].values = malloc(sizeof(char) * createdSet.card);
        DEBUG_CHECK_MALLOC(createdSet.elements[i].values);

        for(j = 0; j < createdSet.card; j++){
            if(i == j) { createdSet.elements[i].values[j] = 1;}
            else { createdSet.elements[i].values[j] = 0;}
        }
    }

    return createdSet;
}

Sets_Set Sets_createPowerSet(const Sets_Set set){
    Sets_Set powerset = {NULL, 0};
    int i = 0, j = 0, sum = 0;

    /*Powerset allocation: */
    powerset.card = pow(2, set.card);
    powerset.elements = malloc(sizeof(Sets_Element) * powerset.card);
    DEBUG_CHECK_MALLOC(powerset.elements);

    /* ***********************
       Building the elements:
       ***********************
       Empty set:*/
    powerset.elements[0].values = malloc(sizeof(char) * set.card);
    DEBUG_CHECK_MALLOC(powerset.elements[0].values);

    for(j = 0; j<set.card; j++){
        powerset.elements[0].values[j] = 0;
    }
    powerset.elements[0].card = 0;
    /*Other elements: */
    for(i = 1; i < powerset.card; i++){
        /*Reset the sum: */
        sum = 0;
        /*Allocate memory: */
        powerset.elements[i].values = malloc(sizeof(char) * set.card);
        DEBUG_CHECK_MALLOC(powerset.elements[i].values);

        /*Compute the binary value: */
        for(j = 0; j < set.card; j++){
                powerset.elements[i].values[j] = ((int)(i/pow(2,j)))%2;
                sum += powerset.elements[i].values[j];
        }
        powerset.elements[i].card = sum;
    }

    return powerset;
}

Sets_Set Sets_generatePowerSet(const int elementSize){
    Sets_Set set, powerset;
    int i = 0, j = 0;

    /*Generate a simple set: */
    set.elements = malloc(sizeof(Sets_Element) * elementSize);
    set.card = elementSize;
    for(i = 0; i < elementSize; i++){
        set.elements[i].card = 1;
        set.elements[i].values = malloc(sizeof(char) * elementSize);
        DEBUG_CHECK_MALLOC(set.elements[i].values);

        for(j = 0; j < elementSize; j++){
            if(i == j){
                set.elements[i].values[j] = 1;
            }
            else {
                set.elements[i].values[j] = 0;
            }
        }
    }
    
    powerset = Sets_createPowerSet(set);
    Sets_freeSet(&set);

    return powerset;
}

Sets_Element Sets_createElementFromStrings(const char* const * const values, const int nbValues, const Sets_ReferenceList rl){
    Sets_Element newElem = {NULL, 0};
    int i = 0, j = 0;
    #ifdef CHECK_MODELS
    int valid = 0;
    #endif
	
	#ifdef CHECK_MODELS
	for(i = 0; i < nbValues; i++){
		valid = 0;
		for(j = 0; j < rl.card; j++){
			if(!strcmp(values[i], rl.values[j])){
				valid = 1;
				break;
			}
		}
		if(!valid){
		   	printf("debug: CHECK MODELS FAIL!\n");
		   	printf("debug: In function Sets_createElementFromStrings(), \"%s\" is invalid...\n", values[i]);
		   	printf("debug: It does not correspond to any value in the given ReferenceList.\n");
		   	printf("debug: Given reference list:\n");
		   	for(j = 0; j < rl.card; j++){
		   		printf("debug: %s\n", rl.values[j]);
		   	}
		}
	}
    #endif
	
    /*Memory allocation: */
    newElem.values = malloc(sizeof(char) * rl.card);
    DEBUG_CHECK_MALLOC(newElem.values);

    /*Fill the element: */
    for(i = 0; i < rl.card; i++){
        /*Initialize: */
        newElem.values[i] = 0;
        /*If the value is referenced: */
        for(j = 0; j < nbValues; j++){
            if(!strcmp(rl.values[i], values[j])) {
                newElem.values[i] = 1;
                newElem.card++;
                break;
            }
        }
    }

    return newElem;
}

Sets_Element Sets_createElementFromBits(const char* values, const int size){
    Sets_Element newElem = {NULL, 0};
    int i = 0;

    newElem.values = malloc(sizeof(char) * size);
    DEBUG_CHECK_MALLOC(newElem.values);

    for(i = 0; i < size; i++){
        newElem.values[i] = values[i];
        newElem.card += values[i];
    }

    return newElem;
}

Sets_Element Sets_copyElement(const Sets_Element e, const int size){
    Sets_Element copy = {NULL, 0};
    int i = 0;

    /*Copy the element: */
    copy.card = e.card;
    copy.values = malloc(sizeof(char) * size);
    DEBUG_CHECK_MALLOC(copy.values);

    for(i = 0; i < size; i++){
        copy.values[i] = e.values[i];
    }

    return copy;
}

Sets_Element Sets_getEmptyElement(const int size){
    Sets_Element emptySet = {NULL, 0};
    int i = 0;

    /*Allocate memory: */
    emptySet.values = malloc(sizeof(char) * size);
    DEBUG_CHECK_MALLOC(emptySet.values);

    /*Put zeros: */
    for(i = 0; i < size; i++){
        emptySet.values[i] = 0;
    }

    return emptySet;
}

Sets_Element Sets_getOpposite(const Sets_Element e, const int size){
    Sets_Element opposite = {NULL, 0};
    int i = 0;

    /*Create opposite: */
    opposite.card = size - e.card;
    opposite.values = malloc(sizeof(char) * size);
    DEBUG_CHECK_MALLOC(opposite.values);

    for(i = 0; i < size; i++){
        opposite.values[i] = !e.values[i];
    }

    return opposite;
}

Sets_Element Sets_elementFromNumber(const int number, const int nbDigits){
	Sets_Element e = {NULL, 0};
	int nb = number;
	int i = 0;
	
	e.values = malloc(sizeof(char) * nbDigits);
	DEBUG_CHECK_MALLOC(e.values);

	for(i = 0; i < nbDigits; i++){
		e.values[i] = nb % 2 ;
		nb /= 2;
		if(e.values[i]){
			e.card++;
		}
	}
	
	return e;
}

int Sets_numberFromElement(const Sets_Element e, const int nbDigits){
	int nb = 0, i = 0;
	
	for(i = 0; i < nbDigits; i++){
		nb += e.values[i] * pow(2, i);
	}
	
	return nb;
}


/** @} */

/**
 * @name Operations on elements
 * @{
 */

/*
 +--------------------+
 | Element operations |
 +--------------------+
*/

Sets_Element Sets_conjunction(const Sets_Element e1, const Sets_Element e2, const int size){
    Sets_Element conj = {NULL, 0};
    int i = 0, sum = 0;

    /*Memory allocation: */
    conj.values = malloc(sizeof(char) * size);
    DEBUG_CHECK_MALLOC(conj.values);

    /*Compare both elements: */
    for(i = 0; i < size; i++){
        conj.values[i] = e1.values[i] && e2.values[i];
        sum += conj.values[i];
    }
    /*Set cardinal: */
    conj.card = sum;

    return conj;
}

Sets_Element Sets_disjunction(const Sets_Element e1, const Sets_Element e2, const int size){
    Sets_Element disj = {NULL, 0};
    int i = 0, sum = 0;

    /*Memory allocation: */
    disj.values = malloc(sizeof(char) * size);
    DEBUG_CHECK_MALLOC(disj.values);

    /*Compare both elements: */
    for(i = 0; i < size; i++){
        disj.values[i] = e1.values[i] || e2.values[i];
        sum += disj.values[i];
    }
    /*Set cardinal: */
    disj.card = sum;

    return disj;
}

Sets_Element Sets_union(const Sets_Element e1, const Sets_Element e2, const int size) {
	Sets_Element unionResult = {NULL, 0};
	int i;
	unionResult.values = malloc(sizeof(char) * size);
	DEBUG_CHECK_MALLOC(unionResult.values);

	for (i = 0; i < size; ++i) {
		unionResult.values[i] = e1.values[i] || e2.values[i];
		unionResult.card += unionResult.values[i];
	}

	return unionResult;
}

int Sets_equals(const Sets_Element e1, const Sets_Element e2, const int size){
    int equality = 1, i = 0;

    if(e1.card == e2.card){
        for(i = 0; i < size; i++){
            equality = equality && (e1.values[i] == e2.values[i]);
        }
    }
    else {
        equality = 0;
    }

    return equality;
}

int Sets_isMember(const Sets_Element e, const Sets_Set s, const int size){
    int member = 0, i = 0;

    for(i = 0; i<s.card; i++){
        member = member || Sets_equals(e, s.elements[i], size);
    }

    return member;
}

int Sets_isSubset(const Sets_Element e1, const Sets_Element e2, const int size){
    int valuesInCommon = 0, i = 0;

    for(i = 0; i < size; i++){
        valuesInCommon += e1.values[i] && e2.values[i];
    }

    return valuesInCommon == e1.card;
}

/** @} */

/**
 * @name Memory deallocation
 * @{
 */

/*
 +--------------+
 | Deallocation |
 +--------------+
*/

void Sets_freeReferenceList(Sets_ReferenceList* rl){
    int i = 0;

    for(i = 0; i < rl->card; i++){
        free(rl->values[i]);
    }
    free(rl->values);
}

void Sets_freeElement(Sets_Element* e){
    free(e->values);
}

void Sets_freeSet(Sets_Set* s){
    int i = 0;

    for(i = 0; i<s->card; i++){
        /*Deallocation of the values: */
        Sets_freeElement(&(s->elements[i]));
    }
    /*Deallocation of the elements table: */
    free(s->elements);
}

/** @} */

/**
 * @name Conversion to string
 * @{
 */

/*
 +-------------------+
 | String conversion |
 +-------------------+
*/

char* Sets_elementToString(const Sets_Element e, const Sets_ReferenceList rl){
    char* str = NULL;
    int i = 0, totChar = 0;
    int sum = 0, firstElement = -1;

    /*Empty set: */
    if(e.card == 0){
        str = malloc(sizeof(char) * 7);
        DEBUG_CHECK_MALLOC(str);

        strcpy(str, "{void}\0");
    }
    /*Others: */
    else{
        /*Size of the values: */
        for(i = 0; i<rl.card; i++){
            if(e.values[i]){
                totChar += strlen(rl.values[i]);
                if(firstElement == -1) { firstElement = i; }
                sum++;
            }
        }
        /*Add the braquets and the union symbols: */
        totChar += 4 + 3*(sum - 1);
        /*Allocate memory: */
        str = malloc(sizeof(char)*totChar);
        DEBUG_CHECK_MALLOC(str);
        /*Create the string: */
        sprintf(str, "{%s", rl.values[firstElement]);
        for(i = firstElement + 1; i<rl.card; i++){
            if(e.values[i]) {
                strcat(str, " u ");
                strcat(str, (rl.values)[i]);
            }
        }
        strcat(str,"}\0");
    }

    return str;
}

char* Sets_elementToBitString(const Sets_Element e, int size){
	char* str = NULL;
	int i = 0;
	
	str = malloc(sizeof(char) * (size + 1));
	DEBUG_CHECK_MALLOC(str);

	for(i = 0; i < size; i++){
		str[i] = e.values[i] ? '1' :'0';
	}
	str[i] = '\0';
	
	return str;
}

char* Sets_setToString(const Sets_Set s, const Sets_ReferenceList rl){
    char* str = NULL;
    char** elements = NULL;
    int i = 0, totChar = 0;

    /*Allocate memory for elements strings: */
    elements = malloc(sizeof(char*)*s.card);
    DEBUG_CHECK_MALLOC_OR_RETURN(elements, NULL);

    /*Count the number of chars: */
    for(i = 0; i<s.card; i++) {
        elements[i] = Sets_elementToString(s.elements[i], rl);
        totChar += strlen(elements[i]);
    }
    totChar += 3 + 2*(s.card - 1);

    /*Allocate memory for the string: */
    str = malloc(sizeof(char)*(totChar+1));
    DEBUG_CHECK_MALLOC_OR_RETURN(str, NULL);

    /*Fill the string: */
    sprintf(str, "{%s", elements[0]);
    for(i = 1; i<s.card; i++){
        strcat(str, ", ");
        strcat(str, elements[i]);
    }
    strcat(str,"}\0");

    /*Deallocate useless memory: */
    for(i = 0; i<s.card; i++){
        free(elements[i]);
    }
    free(elements);

    return str;
}

char* Sets_setToBitString(const Sets_Set s, int size){
	char* str = NULL;
    char** elements = NULL;
    int i = 0, totChar = 0;

    /*Allocate memory for elements strings: */
    elements = malloc(sizeof(char*)*s.card);
    DEBUG_CHECK_MALLOC_OR_RETURN(elements, NULL);

    /*Count the number of chars: */
    for(i = 0; i<s.card; i++) {
        elements[i] = Sets_elementToBitString(s.elements[i], size);
        totChar += strlen(elements[i]);
    }
    totChar += 3 + 2*(s.card - 1);

    /*Allocate memory for the string: */
    str = malloc(sizeof(char)*(totChar+1));
    DEBUG_CHECK_MALLOC_OR_RETURN(str, NULL);

    /*Fill the string: */
    sprintf(str, "{%s", elements[0]);
    for(i = 1; i<s.card; i++){
        strcat(str, ", ");
        strcat(str, elements[i]);
    }
    strcat(str,"}\0");

    /*Deallocate useless memory: */
    for(i = 0; i<s.card; i++){
        free(elements[i]);
    }
    free(elements);

    return str;
}

/** @} */


