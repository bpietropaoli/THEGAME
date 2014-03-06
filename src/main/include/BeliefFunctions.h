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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "config.h"
#include "ReadDirectory.h"
#include "ReadFile.h"
#include "Sets.h"

/**
 * This module does not enable the building of belief functions but only to manipulate them!
 * Thus, this module offers many functions to characterize, combine and discount belief functions.
 * Some decision support functions have also been implemented in order to ease the decision making.@n@n
 * Different combination rules are implemented but one is free to implement some more.@n@n
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

/**
 * Define the prototype for criterionFunction such as plausibility or pignistic
 * probability.
 * @param beliefFunction the belief function on which the criterion will be applied
 * @param element The element for which we want to get the value of the criterion
 * @return The value for the given element according to the criterion.
 * @see BF_pl(), BF_M(), BF_q(), BF_betP(), Bf_q()
 */
typedef  float (*BF_criterionFun)(const BF_BeliefFunction beliefFunction, const Sets_Element element) ;

/*
  +--------------+
  | ENUMERATIONS |
  +--------------+
*/

/**
 * @enum BF_CombinationRule
 * The different types of combination rules that can be used
 * in the generic combination function.
 */
enum BF_CombinationRule
{
    DEMPSTER,
    SMETS,
    YAGER,
    DUBOISPRADE,
    MURPHY,
    CHEN,
    AVERAGE
};
typedef enum BF_CombinationRule BF_CombinationRule;


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
 * @name Combination rules
 * @{
 */

/**
 * Combines a list of belief functions into one. The combination rule used
 * is the classical normalized Dempster rule of combination.
 * @param m A list of BeliefFunctions
 * @param nbM The number of functions in the list
 * @return The resulting BF_BeliefFunction corresponding to the accumulation of evidences
 */
BF_BeliefFunction BF_fullDempsterCombination(const BF_BeliefFunction* m, const int nbM);

/**
 * Combines two BeliefFunctions into one. The combination rule used
 * is the classical normalized Dempster rule of combination.
 * @param m1 The first BF_BeliefFunction to combine
 * @param m2 The second BF_BeliefFunction to combine
 * @return The resulting BF_BeliefFunction corresponding to the accumulation of evidences
 */
BF_BeliefFunction BF_DempsterCombination(const BF_BeliefFunction m1, const BF_BeliefFunction m2);

/**
 * Combines a list of belief functions into one. The combination rule used
 * is defined in P. Smets 1999 (The transferable belief model for
 * belief representation). This is the same rule than the Dempster's one but
 * without any normalization. Thus, the void element may have a non-null mass.
 * @param m A list of BeliefFunctions
 * @param nbM The number of functions in the list
 * @return The resulting BF_BeliefFunction corresponding to the accumulation of evidences
 */
BF_BeliefFunction BF_fullSmetsCombination(const BF_BeliefFunction* m, const int nbM);

/**
 * Combines two BeliefFunctions into one. The combination rule used
 * is defined in P. Smets 1999 (The transferable belief model for
 * belief representation). This is the same rule than the Dempster's one but
 * without any normalization. Thus, the void element may have a non-null mass.
 * @param m1 The first BF_BeliefFunction to combine
 * @param m2 The second BF_BeliefFunction to combine
 * @return The resulting BF_BeliefFunction corresponding to the accumulation of evidences
 */
BF_BeliefFunction BF_SmetsCombination(const BF_BeliefFunction m1, const BF_BeliefFunction m2);

/**
 * Combines a list of belief functions into one. The combination rule used
 * is defined in R.R. Yager 1987 (On the Dempster-Shager framework and new
 * combination rules). The close-world assumption is done and thus, the mass
 * given to the void element is transfered to the whole world. @n
 * CAUTION: This operation is not associative, thus, changing the belief functions
 * order may change the final result!
 * @param m A list of BeliefFunctions
 * @param nbM The number of functions in the list
 * @return The resulting BF_BeliefFunction corresponding to the accumulation of evidences
 */
BF_BeliefFunction BF_fullYagerCombination(const BF_BeliefFunction* m, const int nbM);

/**
 * Combines two BeliefFunctions into one. The combination rule used
 * is defined in R.R. Yager 1987 (On the Dempster-Shager framework and new
 * combination rules). The close-world assumption is done and thus, the mass
 * given to the void element is transfered to the whole world.
 * @param m1 The first BF_BeliefFunction to combine
 * @param m2 The second BF_BeliefFunction to combine
 * @return The resulting BF_BeliefFunction corresponding to the accumulation of evidences
 */
BF_BeliefFunction BF_YagerCombination(const BF_BeliefFunction m1, const BF_BeliefFunction m2);

/**
 * Combines a list of belief functions into one. The combination rule used
 * is defined in D. Dubois & H. Prade 1998 (Representation and combination of uncertainty
 * with belief functions and possibility measures). In this rule, when two belief functions
 * conflict, the mass is transfered to the union of the conflicting focal elements.
 * WARNING: This implementation is not the proper way of doing the fusion of more than 2 mass functions...
 * (The order in which the mass functions are given does matter!)
 * @param m A list of BeliefFunctions
 * @param nbM The number of functions in the list
 * @return The resulting BF_BeliefFunction corresponding to the accumulation of evidences
 */
BF_BeliefFunction BF_fullDuboisPradeCombination(const BF_BeliefFunction* m, const int nbM);

/**
 * Combines two BeliefFunctions into one. The combination rule used
 * is defined in D. Dubois & H. Prade 1998 (Representation and combination of uncertainty
 * with belief functions and possibility measures). In this rule, when two belief functions
 * conflict, the mass is transfered to the union of the conflicting focal elements.
 * @param m1 The first BF_BeliefFunction to combine
 * @param m2 The second BF_BeliefFunction to combine
 * @return The resulting BF_BeliefFunction corresponding to the accumulation of evidences
 */
BF_BeliefFunction BF_DuboisPradeCombination(const BF_BeliefFunction m1, const BF_BeliefFunction m2);

/**
 * Combines a list of belief functions into one. The combination rule used
 * is defined in C. K. Murphy 1999 (Combining belief functions when evidence conflicts).
 * Here, a simple average of all the belief functions is done.
 * @param m A list of BeliefFunctions
 * @param nbM The number of functions in the list
 * @return The resulting BF_BeliefFunction corresponding to the accumulation of evidences
 */
BF_BeliefFunction BF_fullAverageCombination(const BF_BeliefFunction* m, const int nbM);

/**
 * Combines two BeliefFunctions into one. The combination rule used
 * is defined in C. K. Murphy 1999 (Combining belief functions when evidence conflicts).
 * This rule consists in an average of the belief functions instead of the classical
 * combination.
 * @param m1 The first BF_BeliefFunction to combine
 * @param m2 The second BF_BeliefFunction to combine
 * @return The resulting BF_BeliefFunction corresponding to the accumulation of evidences
 */
BF_BeliefFunction BF_averageCombination(const BF_BeliefFunction m1, const BF_BeliefFunction m2);

/**
 * Combines a list of belief functions into one. The combination rule used
 * is defined in C. K. Murphy 1999 (Combining belief functions when evidence conflicts).
 * This combination is based on the average Murphy's combination. Then, nbM - 1 Dempster's
 * combinations of the average are performed to create a convergence.
 * @param m A list of BeliefFunctions
 * @param nbM The number of functions in the list
 * @return The resulting BF_BeliefFunction corresponding to the accumulation of evidences
 */
BF_BeliefFunction BF_fullMurphyCombination(const BF_BeliefFunction* m, const int nbM);

/**
 * Combines two BeliefFunctions into one. The combination rule used
 * is defined in C. K. Murphy 1999 (Combining belief functions when evidence conflicts).
 * This rule consists in an average of the belief functions followed by a classical
 * Dempster combination.
 * @param m1 The first BF_BeliefFunction to combine
 * @param m2 The second BF_BeliefFunction to combine
 * @return The resulting BF_BeliefFunction corresponding to the accumulation of evidences
 */
BF_BeliefFunction BF_MurphyCombination(const BF_BeliefFunction m1, const BF_BeliefFunction m2);

/**
 * Combines a list of belief functions into one. The combination rule used
 * is defined in  L-Z. Chen 2005 (A new fusion approach based on distance of evidences).
 * This rule corresponds to a weighted average taking into account the credibility of
 * each belief function among a set of belief functions. The Chen combination is not
 * very useful with only two belief functions. This is why there is no binary Chen
 * combination implementation.
 * @param m A list of BeliefFunctions
 * @param nbM The number of functions in the list
 * @return The resulting BF_BeliefFunction corresponding to the accumulation of evidences
 */
BF_BeliefFunction BF_fullChenCombination(const BF_BeliefFunction* m, const int nbM);

/**
 * Combines a list of belief functions into one. The combination rule used
 * depends on the given type of combination. You can get a list of those types in the
 * CombinationRule enumeration.
 * @param m A list of BeliefFunctions
 * @param nbM The number of functions in the list
 * @param bs The belief structure the functions are based on
 * @param type The type of combination rule to use
 * @return The resulting BF_BeliefFunction corresponding to the accumulation of evidences
 */
BF_BeliefFunction BF_fullCombination(const BF_BeliefFunction* m, const int nbM, const BF_CombinationRule type);

/**
 * Combines two BeliefFunctions into one. The combination rule used
 * depends on the given type of combination. You can get a list of those types in the
 * CombinationRule enumeration.
 * @param m1 The first BF_BeliefFunction to combine
 * @param m2 The second BF_BeliefFunction to combine
 * @param bs The belief structure the functions are based on
 * @param type The type of combination rule to use
 * @return The resulting BF_BeliefFunction corresponding to the accumulation of evidences
 */
BF_BeliefFunction BF_combination(const BF_BeliefFunction m1, const BF_BeliefFunction m2, const BF_CombinationRule type);

/** @} */

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
float BF_M(const BF_BeliefFunction m, const Sets_Element e);

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
 * Get the self-conflict generated by a BF_BeliefFunction to a certain degree.
 * The rule is defined in A. Martin 2009 (Modelisation et gestion du conflit
 * dans la theorie des fonctions de croyance (French)). The combination rule
 * used is the one defined above.
 * @param m The BF_BeliefFunction to work on
 * @param maxDegree The maximum degree of self-conflict required (the maximum
 *        of self-combination that will be done)
 * @return The list of values of the belief on the void set from 1st degree to
 *         maxDegree-th degree
 */
float* BF_autoConflict(const BF_BeliefFunction m, const int maxDegree);

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

/**
 * @name Decision support functions
 * @{
 */

/**
 * Returns the BF_FocalElement corresponding to the maximum of a BF_BeliefFunction
 * according to a criterion. The criterion is given as a pointer to a function.
 * This can be a pointer to BF_M() or BF_Pl() for instance.
 * The cardinality should be at least of 1 as the empty set is not considered.
 * @param criterion criterion used to get the max.
 * @param beliefFunction function from which we extract the max
 * @param maxCard The maximum authorized cardinality of the max Element (0 = no card limit)
 * @return The BF_FocalElement (Element + mass) corresponding to the maximum. Must be freed after use.
 */
BF_FocalElement BF_getMax(BF_criterionFun criterion, const BF_BeliefFunction beliefFunction,
		const int maxCard, const Sets_Set powerset);

/**
 * Returns the minimum focal element (i.e with a value > 0) corresponding to
 * the minimum of a BF_BeliefFunction according to a criterion. As for
 * BF_getMax() The criterion is given as a pointer to a function. This can be
 * a pointer to BF_M() or BF_Pl() for instance. The cardinality should be at
 * least of 1 as the empty set is not considered.
 * @param criterion criterion used to get the max.
 * @param beliefFunction function from which we extract the minimum
 * @param maxCard The maximum authorized cardinality of the max Element (0 = no card limit)
 * @return The BF_FocalElement (Element + mass) corresponding to the maximum. Must be freed after use.
 * @see BF_getMax()
 */
BF_FocalElement BF_getMin(BF_criterionFun criterion, const BF_BeliefFunction beliefFunction,
		const int maxCard, const Sets_Set powerset);

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


