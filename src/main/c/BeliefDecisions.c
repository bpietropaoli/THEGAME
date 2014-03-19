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
 


#include "BeliefDecisions.h"


/**
 * This module eases the decision making process by providing decision support functions.
 *
 * @file BeliefDecisions.c
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief CORE: Implements decision support functions.
 */
 

/*
  +-------------------+
  | PRIVATE FUNCTIONS |
  +-------------------+
*/
  


static unsigned int listAppend(BF_FocalElementList *list, const BF_FocalElement element,
		const unsigned int realSize, const int elementSize) {
	BF_FocalElement *newArray;
	unsigned int newSize = realSize;
	if(realSize == list->size) {
		/* increase allocated memory size */
		newSize = (list->size + 1)  * 1.25;
		newArray = realloc(list->elements, sizeof(BF_FocalElement) * newSize);
		DEBUG_CHECK_MALLOC_OR_RETURN(newArray, realSize);

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




/*
  +-----------+
  | FUNCTIONS |
  +-----------+
*/




/**
 * @name Decision support functions
 * @{
 */

BF_FocalElement BF_getMax(BF_criterionFunction criterion, const BF_BeliefFunction beliefFunction,
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

BF_FocalElement BF_getMin(BF_criterionFunction criterion, const BF_BeliefFunction beliefFunction,
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



BF_FocalElementList BF_getMaxList(BF_criterionFunction criterion, const BF_BeliefFunction beliefFunction,
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

BF_FocalElementList BF_getMinList(BF_criterionFunction criterion, const BF_BeliefFunction beliefFunction,
		const int maxCard, const Sets_Set powerset) {
	BF_FocalElementList  list = newList();
	unsigned int listSize = 0;

    BF_FocalElement  min = {{NULL,0}, 2};
	int i = 0;
	float value = 0;


	for(i = 0; i < powerset.card; i++){
		if((powerset.elements[i].card <= maxCard ||
				maxCard == 0)                         &&
		   powerset.elements[i].card > 0 ){
			value = criterion(beliefFunction, powerset.elements[i]);

			if(value < min.beliefValue && value > 0){
				emptyList(&list);
				min.element = powerset.elements[i];
				min.beliefValue = value;
				listSize = listAppend(&list, min, listSize, beliefFunction.elementSize);
			}
			else if(value == min.beliefValue) {
				min.element = powerset.elements[i];
				listSize = listAppend(&list, min, listSize, beliefFunction.elementSize);
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


void BF_freeFocalElementList(BF_FocalElementList *list) {
	unsigned int i;
	for (i = 0; i < list->size; ++i) {
		Sets_freeElement(&(list->elements[i].element));
	}
	free(list->elements);
	list->size = 0;
}


/** @} */




/**
 * @name Old decision support functions (Deprecated)
 * @{
 */


BF_FocalElement BF_getMaxMass(const BF_BeliefFunction m, const int card){
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
    DEBUG_CHECK_MALLOC(list);

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
    DEBUG_CHECK_MALLOC(list);

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
    DEBUG_CHECK_MALLOC(list);

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
    DEBUG_CHECK_MALLOC(list);

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
    DEBUG_CHECK_MALLOC(list);

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
    DEBUG_CHECK_MALLOC(list);

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
    DEBUG_CHECK_MALLOC(list);

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
    DEBUG_CHECK_MALLOC(list);

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

