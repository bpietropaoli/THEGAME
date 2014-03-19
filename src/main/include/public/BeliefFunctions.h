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

 
#ifndef DEF_BELIEFFUNCTIONS
#define DEF_BELIEFFUNCTIONS


#include "Sets.h"

/**
 * This module does not enable the building of belief functions but only to manipulate them!
 * Thus, this module offers many functions to characterize and discount belief functions.
 *
 * If you have no idea to what corresponds exactly a function, you should refer to the given references.
 * @file BeliefFunctions.h
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief CORE: Gives structures and main
 *        functions to manipulate belief functions.
 */

/**
 * @def BF_PRECISION
 * The precision for the masses of belief functions.
 */
#define BF_PRECISION 0.000002


/*
  +------------+
  | STRUCTURES |
  +------------+
*/


/* !!! The actual belief !!! */


/**
 * A couple (element, belief) to associate a belief to an element.
 * @param elements The focal elements
 * @param beliefValues The belief on each element (in the same order)
 * @struct BF_FocalElement
 */
struct BF_FocalElement{
    Sets_Element element;
    float beliefValue;
};
typedef struct BF_FocalElement BF_FocalElement;


/**
 * The real belief function. There are several ways to build
 * belief functions (for instance using the BeliefsFromSensors
 * module). Operations defined in this module are for mass functions
 * but this structure can be used to store credibility or plausibility
 * as well.
 * @param focals The focal elements of the mass function
 * @param nbFocals The number of focals
 * @param elementSize The number of possible worlds in the frame of discernment.
 * @struct BF_BeliefFunction
 */
struct BF_BeliefFunction{
    BF_FocalElement *focals;
    int nbFocals;
    int elementSize;
};
typedef struct BF_BeliefFunction BF_BeliefFunction;




/*
  +-----------+
  | FUNCTIONS |
  +-----------+
*/


/**
 * @name Utility functions
 * @{
 */

/**
 * Copies a BF_BeliefFunction.
 * @param m The BF_BeliefFunction to copy
 * @return A new BF_BeliefFunction. Must be freed after use.
 */
BF_BeliefFunction BF_copyBeliefFunction(const BF_BeliefFunction m);

/**
 * Creates and return a vacuous BF_BeliefFunction
 * @param elementSize The number of bits used to represend elements
 * @return A vacuous BF_BeliefFunction. Must be freed after use.
 */
BF_BeliefFunction BF_getVacuousBeliefFunction(const int elementSize);

/**
 * Cleans the BF_BeliefFunction given from all the non-focal elements.
 * If there are zeros, the list of BF_FocalElement is freed and replaced
 * by a clean one. Thus, it modifies the given BF_BeliefFunction.
 * @param bf A pointer to a BF_BeliefFunction
 */
void BF_cleanBeliefFunction(BF_BeliefFunction* bf);

/**
 * Normalizes the given BF_BeliefFunction. It modifies it.
 * @param bf The BF_BeliefFunction to normalize.
 */
void BF_normalize(BF_BeliefFunction* bf);

/** @} */


/* !!! Operation on beliefs !!! */



/**
 * @name Operations on belief functions
 * @{
 */

/**
 * Get the new resulting BF_BeliefFunction knowing that an certain element
 * is true. The rule used is defined in P. Smets 1999 (The transferable
 * belief model for belief representation).
 * @param m The BF_BeliefFunction to work on
 * @param e The element which is true
 * @param powerset The set of values the BF_BeliefFunction is applied on
 * @return A conditioned BF_BeliefFunction knowing that e is true
 */
BF_BeliefFunction BF_conditioning(const BF_BeliefFunction m, const Sets_Element e, const Sets_Set powerset);

/**
 * Weakens a belief function given a coefficient alpha in [0,1]. All
 * believes on focal elements will be multiplied by a factor of (1 - alpha).
 * The lost belief is transfered to the void focal element.
 * @param m The BF_BeliefFunction to work on
 * @param alpha The weakening coefficient
 * @return The weakened BF_BeliefFunction resulting from m and alpha.
 */
BF_BeliefFunction BF_weakening(const BF_BeliefFunction m, const float alpha);

/**
 * Discounts a belief function given a coefficient alpha in [0,1]. All
 * believes on focal elements will be multiplied by a factor of (1 - alpha).
 * The lost belief is transfered to the complete frame of discernment.
 * @param m The BF_BeliefFunction to work on
 * @param alpha The weakening coefficient
 * @return The weakened BF_BeliefFunction resulting from m and alpha.
 */
BF_BeliefFunction BF_discounting(const BF_BeliefFunction m, const float alpha);

/**
 * Get a vector (represented as a BF_BeliefFunction but is NOT an actual one)
 * of the difference of two BeliefFunctions.
 * @param m1 The first BF_BeliefFunction to work on
 * @param m2 The second BF_BeliefFunction to work on
 * @param powerset The set of all the elements the BeliefFunctions are applied on
 * @return A fake BF_BeliefFunction representing in fact the difference between m1 and m2.
 * Must be freed after use.
 */
BF_BeliefFunction BF_difference(const BF_BeliefFunction m1, const BF_BeliefFunction m2);

/** @} */

/**
 * @name Function-and-element-dependant operations
 * @{
 */

/**
 * Get the belief on a element.
 * @param m The BF_BeliefFunction to work on
 * @param e The element whose belief we want on
 * @return m(e), the belief on the element e from the belief function m.
 */
float BF_m(const BF_BeliefFunction m, const Sets_Element e);

/**
 * Get the belief (or credibility) of an element given a BF_BeliefFunction. The operation used
 * is defined in P. Smets 1999 (The transferable belief model for
 * belief representation). It reprensents a pessimistic probability of
 * occurrence of an element.
 * @param m The BF_BeliefFunction to work on
 * @param e The element to work on
 * @return The belief (or credibility) value associated to the element given the BF_BeliefFunction
 */
float BF_bel(const BF_BeliefFunction m, const Sets_Element e);

/**
 * Get the plausibility of an element given a BF_BeliefFunction. The operation used
 * is defined in P. Smets 1999 (The transferable belief model for
 * belief representation). It represents an optimistic probability of
 * occurrence of an element.
 * @param m The BF_BeliefFunction to work on
 * @param e The Element to work on
 * @return The plausibility value associated to the element given the BF_BeliefFunction
 */
float BF_pl(const BF_BeliefFunction m, const Sets_Element e);

/**
 * Get the commonality of an element given a BF_BeliefFunction. The operation used
 * is defined in P. Smets 1998 (The transferable belief model for
 * quantified belief representation). The commonality values are the
 * eigenvalues of the Dempsterian specialization matrix.
 * @param m The BF_BeliefFunction to work on
 * @param e The Element to work on
 * @return The commonality value associated to the element given the BF_BeliefFunction
 */
float BF_q(const BF_BeliefFunction m, const Sets_Element e);

/**
 * Get the pignistic (the bet) probability of an element given a BF_BeliefFunction.
 * The rule used is defined in P. Smets 1999 (The transferable belief model for
 * belief representation). It represents neither a pessimistic nor an optimistic
 * probability of occurrence of an element. Actually, bel(m,e) < betP(m,e) < pl(m,e).
 * @param m The BF_BeliefFunction to work on
 * @param e The Element to work on
 * @return The pignistic probability of an element given the BF_BeliefFunction
 */
float BF_betP(const BF_BeliefFunction m, const Sets_Element e);

/** @} */

/**
 * @name Function-dependant operations
 * @{
 */



/**
 * Get the specificity of a BF_BeliefFunction. The rule used is defined in A. Martin
 * 2009 (Modelisation et gestion du conflit dans la theorie des fonctions de croyance
 * (French)). The rule enable to characterize the partial ignorance of the system given
 * by a BF_BeliefFunction.
 * @param m The BF_BeliefFunction to work on
 * @return The specificity value of the BF_BeliefFunction
 */
float BF_specificity(const BF_BeliefFunction m);

/**
 * Get the non-specificity of a BF_BeliefFunction. The rule used is defined in Dubois & Prade
 * 1985 (A note on measures of specificity for fuzzy sets).
 * @param m The BF_BeliefFunction to work on
 * @return The non-specificity value of the BF_BeliefFunction
 */
float BF_nonSpecificity(const BF_BeliefFunction m);

/**
 * Get the discrepancy of a BF_BeliefFunction. This is maximal for the uniform pignistic
 * distribution over the set a possible values. The rule used is defined in
 * P. Vannoorenberghe 2001 (State of the art on belief functions applied to data
 * processing (French)).
 * @param m The BF_BeliefFunction to work on
 * @return The discrepancy value of the BF_BeliefFunction
 */
float BF_discrepancy(const BF_BeliefFunction m);

/**
 * Get the distance between two BeliefFunctions. The rule used is defined in
 * A. Martin 2009 (Modelisation et gestion du conflit dans la theorie des fonctions
 * de croyance (French)). The distance can be a good characteristic of the conflict
 * between two BeliefFunctions.
 * @param m1 The first BF_BeliefFunction to work on
 * @param m2 The second BF_BeliefFunction to work on
 * @return The distance between the two BeliefFunctions
 */
float BF_distance(const BF_BeliefFunction m1, const BF_BeliefFunction m2);

/**
 * Get the global distance between a BF_BeliefFunction and a set of BeliefFunctions.
 * The rule used is defined in A. Martin 2009 (Modelisation et gestion du conflit
 * dans la theorie des fonctions de croyance (French)). The @link distance() @endlink is what is
 * is used to characterize the conflict between a BF_BeliefFunction and the given set.
 * @param m The BF_BeliefFunction to work on
 * @param s The set of BeliefFunctions to work on (m should be included in)
 * @param nbBF The number of BeliefFunctions contained in the given set
 * @return The global distance between the BF_BeliefFunction m and a BF_BeliefFunction set s.
 */
float BF_globalDistance(const BF_BeliefFunction m, const BF_BeliefFunction* s, const int nbBF);

/**
 * Get the similarity between two BeliefFunctions. It can be representative of the
 * agreement between two bodies of evidence. This rule is defined in L.-Z. Chen 2005
 * (A new fusion approach based on distance of evidence).
 * @param m1 The first BF_BeliefFunction to work on
 * @param m2 The second BF_BeliefFunction to work on
 * @return The similarity between the two BeliefFunctions, 0 meaning no similarity, 1 meaning identical.
 */
float BF_similarity(const BF_BeliefFunction m1, const BF_BeliefFunction m2);

/**
 * Get the support degree given to a BF_BeliefFunction by a set of BeliefFunctions.
 * The BF_BeliefFunction used as a reference should be included in the set.
 * This value is the sum of the similarity of ref with the functions of the given set.
 * @param ref The BF_BeliefFunction used as reference
 * @param m The set of BeliefFunctions that may support ref or not
 * @param nbM The number of BeliefFunctions in the set
 * @return The support degree of ref among the set m.
 */
float BF_support(const BF_BeliefFunction ref, const BF_BeliefFunction* m, const int nbM);

/**
 * Checks the sum of all beliefs on elements.
 * @param m The BF_BeliefFunction to work on
 * @return 0 if the sum is okay ( == 1), 1 if not
 */
int BF_checkSum(const BF_BeliefFunction m);

/**
 * Checks the values of the BF_BeliefFunction.
 * @param m TheBeliefFunction to work on
 * @return 0 if the values are okay, 1 if not
 */
int BF_checkValues(const BF_BeliefFunction m);

/** @} */


/* !!! Deallocate memory given to believes !!! */

/**
 * @name Memory deallocation
 * @{
 */

/**
 * Frees the memory used for the BF_BeliefFunction.
 * @param bs A pointer to the BF_BeliefFunction to free
 */
void BF_freeBeliefFunction(BF_BeliefFunction* bf);

/**
 * Frees the memory used for the BF_FocalElement.
 * @param bp A pointer to the BF_FocalElement to free
 */
void BF_freeBeliefPoint(BF_FocalElement *bp);

/**
 * Frees the memory used by a list (array contained by the list and elements
 * contained  by the array).
 * @param list List to free.
 */
void BF_freeFocalElementList(BF_FocalElementList *list);

/** @} */

/* !!! Conversion into strings !!! */

/**
 * @name Conversion into string
 * @{
 */

/**
 * Converts a BF_BeliefFunction into a string ready to print.
 * @param bf The BF_BeliefFunction to convert
 * @param rl The ReferenceList containing the real values of the context attribute
 * @return A string ready to print representing the BF_BeliefFunction.
 *         Must be freed after use.
 */
char* BF_beliefFunctionToString(const BF_BeliefFunction bf, const Sets_ReferenceList rl);

/**
 * Converts a BF_BeliefFunction into a string ready to print where elements are given
 * in their binary form.
 * @param bf The BF_BeliefFunction to convert
 * @return A string ready to print representing the BF_BeliefFunction.
 *         Must be freed after use.
 */
char* BF_beliefFunctionToBitString(const BF_BeliefFunction bf);

/** @} */

#endif


