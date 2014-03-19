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


#ifndef DEF_BELIEFDECISIONS
#define DEF_BELIEFDECISIONS

#include "BeliefFunctions.c"

/**
 * This module eases the decision making process by providing decision support functions.
 *
 * @file BeliefDecisions.h
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief CORE: Implements decision support functions.
 */


/**
 * Define the prototype for criterionFunction such as plausibility or pignistic
 * probability.
 * @param beliefFunction the belief function on which the criterion will be applied
 * @param element The element for which we want to get the value of the criterion
 * @return The value for the given element according to the criterion.
 * @see BF_pl(), BF_m(), BF_bel(), BF_betP(), Bf_q()
 */
typedef  float (*BF_criterionFunction)(const BF_BeliefFunction beliefFunction, const Sets_Element element) ;


/*
  +------------+
  | STRUCTURES |
  +------------+
*/


/**
 * A simple structure to store a list of Focal elements. It is used by
 * BF_getMinList() and BF_getMaxList().
 * @param elements array of elements
 * @param size size of the array
 */
struct BF_FocalElementList{
	BF_FocalElement *elements;
	unsigned int size;
};
typedef struct BF_FocalElementList BF_FocalElementList;


/*
  +-----------+
  | FUNCTIONS |
  +-----------+
*/



/**
 * @name Generic decision support functions
 * @{
 */
 

/**
 * Returns the BF_FocalElement corresponding to the maximum of a BF_BeliefFunction
 * according to a criterion. The criterion is given as a pointer to a function.
 * Usual criteria are the mass (BF_m()), the credibility/belief (BF_bel()),
 * the plausibility (BF_pl()), and the pignistic transformation (BF_betP()).
 * It is also possible to use the commonality (BF_q()) or any criterion one may
 * want to add/implement.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @note 1) BF_GetMax() return only one maximum value for convenience. If there may
 * be several focal element which have the same value, BF_getMaxList() can return
 * every maximum values. @n@n
 * 2)If you use the mass as a criterion, you may want to use the old decision support
 * functions as it may be a lot more efficient (mass does not require to work on
 * the powerset!). 
 * @param criterion The criterion used to get the max
 * @param beliefFunction The belief function function from which we extract the max
 * @param maxCard The maximum authorized cardinality of the max Element (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The BF_FocalElement (Element + mass) corresponding to the maximum. Must be freed after use.
 */
BF_FocalElement BF_getMax(BF_criterionFunction criterion, const BF_BeliefFunction beliefFunction,
		const int maxCard, const Sets_Set powerset);

/**
 * Returns the BF_FocalElement corresponding to the minimum of a BF_BeliefFunction
 * according to a criterion. The criterion is given as a pointer to a function.
 * Usual criteria are the mass (BF_m()), the credibility/belief (BF_bel()),
 * the plausibility (BF_pl()), and the pignistic transformation (BF_betP()).
 * It is also possible to use the commonality (BF_q()) or any criterion one may
 * want to add/implement.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @note 1) BF_GetMax() return only one maximum value for convenience. If there may
 * be several focal element which have the same value, BF_getMaxList() can return
 * every maximum values. @n@n
 * 2) If you use the mass as a criterion, you may want to use the old decision support
 * functions as it may be a lot more efficient (mass does not require to work on
 * the powerset!). 
 * @param criterion The criterion used to get the min
 * @param beliefFunction The belief function function from which we extract the min
 * @param maxCard The maximum authorized cardinality of the min Element (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The BF_FocalElement (Element + mass) corresponding to the minimum. Must be freed after use.
 */
BF_FocalElement BF_getMin(BF_criterionFunction criterion, const BF_BeliefFunction beliefFunction,
		const int maxCard, const Sets_Set powerset);

/**
 * Gets the list of focals corresponding to the maximum for a given criterion
 * for a BF_BeliefFunction. The cardinality should be at least of 1 as the empty
 * set is not considered. This function is useful if you think there may be several
 * set elements with the same value for the belief function. The difference
 * with BF_getMax() is that if there are several focal elements with the same
 * value which can be the maximum, they will be all returned.
 * @param criterion The criterion used to find the max
 * @param beliefFunction The belief function from which we extract the maxima
 * @param maxCard The maximum authorized cardinality of the max Element (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The BF_FocalElement (Element + mass) list corresponding to the maximum.
 * Must be freed with BF_freeFocalElementList().
 */
BF_FocalElementList BF_getMaxList(BF_criterionFunction criterion, const BF_BeliefFunction beliefFunction,
		const int maxCard, const Sets_Set powerset);

/**
 * Gets the list of focals (i.e. with a value > 0) corresponding to the minimum
 * for a given criterion for a BF_BeliefFunction. The cardinality should be at
 * least of 1 as the empty set is not considered. This function is useful if you think
 * there may be several set elements with the same value for the belief function.
 * The difference with BF_getMin() is that if there are several focal elements
 * with the minimum value, they will be all returned.
 * @param criterion The criterion used to find the minimum
 * @param beliefFunction The belief function from which we extract the minima
 * @param maxCard The maximum authorized cardinality of the max Element (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The BF_FocalElement (Element + mass) list corresponding to the maximum.
 * Must be freed with BF_freeFocalElementList().
 */
BF_FocalElementList BF_getMinList(BF_criterionFunction criterion, const BF_BeliefFunction beliefFunction,
		const int maxCard, const Sets_Set powerset);


/** @} */




/**
 * @name Memory deallocation
 * @{
 */

/**
 * Deallocate memory of a BF_FocalElementList.
 * @param list A pointer to the list to free.
 */
void BF_freeFocalElementList(BF_FocalElementList *list);

/** @} */




/**
 * @name Old decision support functions (Deprecated)
 * @{
 */


/**
 * Returns the BF_FocalElement corresponding to the maximum of mass of a BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @return The BF_FocalElement (Element + mass) corresponding to the maximum of mass. Must be freed after use.
 * Returns a BF_FocalElement with a void Element (NULL, 0) if no element fitting the cardinality constraint is found.
 */
BF_FocalElement BF_getMaxMass(const BF_BeliefFunction m, const int card);


/**
 * Returns the BF_FocalElement corresponding to the non-nul minimum of mass of a BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @return The BF_FocalElement (Element + mass) corresponding to the minimum of mass. Must be freed after use.
 * Returns a BF_FocalElement with a void Element (NULL, 0) if no element fitting the cardinality constraint is found.
 */
BF_FocalElement BF_getMinMass(const BF_BeliefFunction m, const int card);

/**
 * Returns the BF_FocalElement corresponding to the maximum of belief (see bel()) of a BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The BF_FocalElement (Element + belief) corresponding to the maximum of belief. Must be freed after use.
 * Returns a BF_FocalElement with a void Element (NULL, 0) if no element fitting the cardinality constraint is found.
 */
BF_FocalElement BF_getMaxBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Returns the BF_FocalElement corresponding to the non-nul minimum of belief (see bel()) of a BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The BF_FocalElement (Element + belief) corresponding to the minimum of belief. Must be freed after use.
 * Returns a BF_FocalElement with a void Element (NULL, 0) if no element fitting the cardinality constraint is found.
 */
BF_FocalElement BF_getMinBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Returns the BF_FocalElement corresponding to the maximum of plausibility (see pl()) of a BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The BF_FocalElement (Element + plausibility) corresponding to the maximum of plausibility. Must be freed after use.
 * Returns a BF_FocalElement with a void Element (NULL, 0) if no element fitting the cardinality constraint is found.
 */
BF_FocalElement BF_getMaxPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Returns the BF_FocalElement corresponding to the non-nul minimum of plausibility (see pl()) of a BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The BF_FocalElement (Element + plausibility) corresponding to the minimum of plausibility. Must be freed after use.
 * Returns a BF_FocalElement with a void Element (NULL, 0) if no element fitting the cardinality constraint is found.
 */
BF_FocalElement BF_getMinPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Returns the BF_FocalElement corresponding to the maximum of pignistic transformation (seep betP()) of a BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The BF_FocalElement (Element + pignistic) corresponding to the maximum of pignistic transformation. Must be freed after use.
 * Returns a BF_FocalElement with a void Element (NULL, 0) if no element fitting the cardinality constraint is found.
 */
BF_FocalElement BF_getMaxBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Returns the BF_FocalElement corresponding to the non-null minimum of pignistic transformation (see betP()) of a BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The BF_FocalElement (Element + pignistic) corresponding to the minimum of pignistic transformation. Must be freed after use.
 * Returns a BF_FocalElement with a void Element (NULL, 0) if no element fitting the cardinality constraint is found.
 */
BF_FocalElement BF_getMinBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Returns the number of focals actually corresponding to the maximum of mass of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @return The number of focals corresponding to the maximum of mass. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getNbMaxMass(const BF_BeliefFunction m, const int card);

/**
 * Returns the number of focals actually corresponding to the non-null minimum of mass of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @return The number of focals corresponding to the minimum of mass. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getNbMinMass(const BF_BeliefFunction m, const int card);

/**
 * Returns the number of focals actually corresponding to the maximum of belief (see bel()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The number of focals corresponding to the maximum of belief. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getNbMaxBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Returns the number of focals actually corresponding to the non-null minimum of belief (see bel()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The number of focals corresponding to the minimum of belief. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getNbMinBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Returns the number of focals actually corresponding to the maximum of plausibility (see pl()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The number of focals corresponding to the maximum of plausibility. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getNbMaxPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Returns the number of focals actually corresponding to the non-null minimum of plausibility (see pl()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The number of focals corresponding to the minimum of plausibility. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getNbMinPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Returns the number of focals actually corresponding to the maximum of pignistic transformation (see betP()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The number of focals corresponding to the maximum of pignistic transformation. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getNbMaxBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Returns the number of focals actually corresponding to the non-null minimum of pignistic transformation (see betP()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return The number of focals corresponding to the minimum of pignistic transformation. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getNbMinBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Returns the number of focals actually corresponding to the maximum of mass of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param maxValue The known max value of the BF_BeliefFunction
 * @return The number of focals corresponding to the maximum of mass. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getQuickNbMaxMass(const BF_BeliefFunction m, const int card, float maxValue);

/**
 * Returns the number of focals actually corresponding to the non-null minimum of mass of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param minValue The known min value of the BF_BeliefFunction
 * @return The number of focals corresponding to the minimum of mass. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getQuickNbMinMass(const BF_BeliefFunction m, const int card, float minValue);

/**
 * Returns the number of focals actually corresponding to the maximum of belief (see bel()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param maxValue The known max value of the BF_BeliefFunction
 * @return The number of focals corresponding to the maximum of belief. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getQuickNbMaxBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset, float maxValue);

/**
 * Returns the number of focals actually corresponding to the non-null minimum of belief (see bel()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param minValue The known min value of the BF_BeliefFunction
 * @return The number of focals corresponding to the minimum of belief. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getQuickNbMinBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset, float minValue);

/**
 * Returns the number of focals actually corresponding to the maximum of plausibility (see pl()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param maxValue The known max value of the BF_BeliefFunction
 * @return The number of focals corresponding to the maximum of plausibility. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getQuickNbMaxPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset, float maxValue);

/**
 * Returns the number of focals actually corresponding to the non-null minimum of plausibility (see pl()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param minValue The known max value of the BF_BeliefFunction
 * @return The number of focals corresponding to the minimum of plausibility. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getQuickNbMinPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset, float minValue);

/**
 * Returns the number of focals actually corresponding to the maximum of pignistic transformation (see betP()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param maxValue The known max value of the BF_BeliefFunction
 * @return The number of focals corresponding to the maximum of pignistic transformation. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getQuickNbMaxBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset, float maxValue);

/**
 * Returns the number of focals actually corresponding to the non-null minimum of pignistic transformation (see betP()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param minValue The known max value of the BF_BeliefFunction
 * @return The number of focals corresponding to the minimum of pignistic transformation. Returns 0 if no element
 * fitting the cardinality constraint is found.
 */
int BF_getQuickNbMinBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset, float minValue);

/**
 * Get the list of focals corresponding to the maximum of mass of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @return A list of BF_FocalElement corresponding to the maximum of mass. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getListMaxMass(const BF_BeliefFunction m, const int card);

/**
 * Get the list of focals corresponding to the non-null minimum of mass of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @return A list of BF_FocalElement corresponding to the minimum of mass. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getListMinMass(const BF_BeliefFunction m, const int card);

/**
 * Get the list of focals corresponding to the maximum of belief (see bel())of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return A list of BF_FocalElement corresponding to the maximum of belief. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getListMaxBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Get the list of focals corresponding to the non-null minimum of belief (see bel()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return A list of BF_FocalElement corresponding to the minimum of belief. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getListMinBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Get the list of focals corresponding to the maximum of plausibility (see pl()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return A list of BF_FocalElement corresponding to the maximum of plausibility. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getListMaxPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Get the list of focals corresponding to the non-null minimum of plausibility (see pl()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return A list of BF_FocalElement corresponding to the minimum of plausibility. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getListMinPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Get the list of focals corresponding to the maximum of pignistic transformation (see betP()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return A list of BF_FocalElement corresponding to the maximum of pignistic transformation. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getListMaxBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Get the list of focals corresponding to the non-null minimum of pignistic transformation (see betP()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @return A list of BF_FocalElement corresponding to the minimum of pignistic transformation. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getListMinBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset);

/**
 * Get the list of focals corresponding to the maximum of mass of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param maxValue The known max value of the BF_BeliefFunction
 * @return A list of BF_FocalElement corresponding to the maximum of mass. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickListMaxMass(const BF_BeliefFunction m, const int card, const float maxValue);

/**
 * Get the list of focals corresponding to the non-null minimum of mass of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param minValue The known min value of the BF_BeliefFunction
 * @return A list of BF_FocalElement corresponding to the minimum of mass. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickListMinMass(const BF_BeliefFunction m, const int card, const float minValue);

/**
 * Get the list of focals corresponding to the maximum of belief (see bel())of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param maxValue The known max value of the BF_BeliefFunction
 * @return A list of BF_FocalElement corresponding to the maximum of belief. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickListMaxBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float maxValue);

/**
 * Get the list of focals corresponding to the non-null minimum of belief (see bel()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param minValue The known min value of the BF_BeliefFunction
 * @return A list of BF_FocalElement corresponding to the minimum of belief. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickListMinBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float minValue);

/**
 * Get the list of focals corresponding to the maximum of plausibility (see pl()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param maxValue The known max value of the BF_BeliefFunction
 * @return A list of BF_FocalElement corresponding to the maximum of plausibility. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickListMaxPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float maxValue);

/**
 * Get the list of focals corresponding to the non-null minimum of plausibility (see pl()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param minValue The known min value of the BF_BeliefFunction
 * @return A list of BF_FocalElement corresponding to the minimum of plausibility. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickListMinPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float minValue);

/**
 * Get the list of focals corresponding to the maximum of pignistic transformation (see betP()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param maxValue The known max value of the BF_BeliefFunction
 * @return A list of BF_FocalElement corresponding to the maximum of pignistic transformation. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickListMaxBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float maxValue);

/**
 * Get the list of focals corresponding to the non-null minimum of pignistic transformation (see betP()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param minValue The known min value of the BF_BeliefFunction
 * @return A list of BF_FocalElement corresponding to the minimum of pignistic transformation. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickListMinBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float minValue);

/**
 * Get the list of focals corresponding to the maximum of mass of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param maxValue The known max value of the BF_BeliefFunction
 * @param nbMax The number of elements corresponding to the given max
 * @return A list of BF_FocalElement corresponding to the maximum of mass. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickerListMaxMass(const BF_BeliefFunction m, const int card, const float maxValue, const int nbMax);

/**
 * Get the list of focals corresponding to the non-null minimum of mass of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param minValue The known min value of the BF_BeliefFunction
 * @param nbMin The number of elements corresponding to the given min
 * @return A list of BF_FocalElement corresponding to the minimum of mass. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickerListMinMass(const BF_BeliefFunction m, const int card, const float minValue, const int nbMin);

/**
 * Get the list of focals corresponding to the maximum of belief (see bel())of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param maxValue The known max value of the BF_BeliefFunction
 * @param nbMax The number of elements corresponding to the given max
 * @return A list of BF_FocalElement corresponding to the maximum of belief. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickerListMaxBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float maxValue, const int nbMax);

/**
 * Get the list of focals corresponding to the non-null minimum of belief (see bel()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param minValue The known min value of the BF_BeliefFunction
 * @param nbMin The number of elements corresponding to the given min
 * @return A list of BF_FocalElement corresponding to the minimum of belief. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickerListMinBel(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float minValue, const int nbMin);

/**
 * Get the list of focals corresponding to the maximum of plausibility (see pl()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param maxValue The known max value of the BF_BeliefFunction
 * @param nbMax The number of elements corresponding to the given max
 * @return A list of BF_FocalElement corresponding to the maximum of plausibility. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickerListMaxPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float maxValue, const int nbMax);

/**
 * Get the list of focals corresponding to the non-null minimum of plausibility (see pl()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param minValue The known min value of the BF_BeliefFunction
 * @param nbMin The number of elements corresponding to the given min
 * @return A list of BF_FocalElement corresponding to the minimum of plausibility. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickerListMinPl(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float minValue, const int nbMin);

/**
 * Get the list of focals corresponding to the maximum of pignistic transformation (see betP()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the max (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param maxValue The known max value of the BF_BeliefFunction
 * @param nbMax The number of elements corresponding to the given max
 * @return A list of BF_FocalElement corresponding to the maximum of pignistic transformation. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickerListMaxBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float maxValue, const int nbMax);

/**
 * Get the list of focals corresponding to the non-null minimum of pignistic transformation (see betP()) of the given BF_BeliefFunction.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param m The BF_BeliefFunction to work with
 * @param card The maximum authorized cardinality of the Element corresponding to the min (0 = no card limit)
 * @param powerset The powerset generated from the size of the elements
 * @param minValue The known min value of the BF_BeliefFunction
 * @param nbMin The number of elements corresponding to the given min
 * @return A list of BF_FocalElement corresponding to the minimum of pignistic transformation. Must be freed after use.
 * Returns null if no Element fitting the cardinality constraint is found.
 */
BF_FocalElement* BF_getQuickerListMinBetP(const BF_BeliefFunction m, const int card, const Sets_Set powerset, const float minValue, const int nbMin);

/** @} */



#endif


