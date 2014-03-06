/*
 * BeliefCombinations.h
 *
 *  Created on: Mar 6, 2014
 *      Author: arichez
 */

#include "BeliefFunctions.h"

/**
 * This module contains the elements related to belief function combination.
 *
 * Different combination rules are implemented but one is free to implement some more.
 *
 * If you have no idea to what corresponds exactly a function, you should refer to the given references.
 * @file BeliefCombinations.h
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief CORE: Gives function to combine belief functions.
 */

/**
 * @name Combination rules
 * @{
 */

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
