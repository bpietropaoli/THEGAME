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


#ifndef DEF_SETS
#define DEF_SETS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "config.h"
#include "ReadFile.h"

/**
 * @file Sets.h
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief CORE: Implements the basics
 * required to work on sets and elements
 * in the theory of belief functions.
 */

/*
  +------------+
  | STRUCTURES |
  +------------+
*/

/**
 * The reference list to get the real values
 * of the atoms in a set or in an element.
 * @param values The real values (strings) of the atoms
 * @param card The cardinal of the list
 * @struct Sets_ReferenceList
 */
struct Sets_ReferenceList {
    char** values;
    int card;
};
typedef struct Sets_ReferenceList Sets_ReferenceList;


/**
 * A structure to define elements of sets or
 * subsets for powersets.
 * Hence, an element may have a cardinality != 0.
 * @param values The list of atoms
 * @param card The cardinality of the element
 * @struct Sets_Element
 */
struct Sets_Element {
    char* values;
    int card;
};
typedef struct Sets_Element Sets_Element;


/**
 * A structure to define sets in general.
 * May be used to define powersets.
 * @param elements The list of elements in the set
 * @param card  The cardinality of the set
  * @struct Sets_Set
 */
struct Sets_Set {
    Sets_Element* elements;
    int card;
};
typedef struct Sets_Set Sets_Set;


/*
  +-----------+
  | FUNCTIONS |
  +-----------+
*/


/**
 * @name Reference lists
 * @{
 */

/**
 * Loads a reference list of possible values for
 * a context attribute from a file. There should be
 * one atom per line in the file and no empty line!
 * 'void' is reserved, use it may cause unpredictable behaviour.
 * @param fileName The name of the file to read
 * @return The loaded ReferenceList or a list with a card = 0 if fail.
 *         Must be freed after use.
 */
Sets_ReferenceList Sets_loadRefList(const char* filename);

/** @} */


/* !!! Creation of sets and elements !!! */


/**
 * @name Set and element creation
 * @{
 */

/**
 * Creates the set of atoms given a ReferenceList.
 * @param rl The ReferenceList of atomic values
 * @return The set associated to the given ReferenceList.
 *         Must be freed after use.
 */
Sets_Set Sets_createSetFromRefList(const Sets_ReferenceList rl);

/**
 * Creates the set of atoms given the total number of required atoms.
 * @param nbAtoms The wanted number of atoms
 * @return The set of atoms. Must be freed after use.
 */
Sets_Set Sets_createSet(const int nbAtoms);

/**
 * Creates a powerset using a Set.
 * @param set The generator set, assuming that it contains only atoms
 * @return A new set representing the powerset. Must be freed
 *         after use.
 */
Sets_Set Sets_createPowerSet(const Sets_Set set);

/**
 * Creates a powerset knowing the number of bits to represent the elements.
 * @param elementSize The number of bits used to represent elements
 * @return A new Set representing the powerset. Must be freed after use.
 */
Sets_Set Sets_generatePowerSet(const int elementSize);

/**
 * Creates an element from a list of string values.
 * @param values The strings corresponding to the values
 * @param nbValues The number of values to store
 * @param rl The ReferenceList containing the real values of the context attribute
 * @return A new element corresponding to the given values. Must be
 *         freed after use.
 */
Sets_Element Sets_createElementFromStrings(const char* const * const values, const int nbValues, const Sets_ReferenceList rl);

/**
 * Creates an element from a list of bits.
 * @param values The list of bits corresponding of the binary representation of the Element
 * @param size The number of bits used
 * @return A new element corresponding to the given values. Must be freed after use.
 */
Sets_Element Sets_createElementFromBits(const char* values, const int size);

/**
 * Create a copy of an element.
 * @param e The element to duplicate
 * @param size The size of the element to duplicate
 * @return A copy of the given element. Must be freed after use.
 */
Sets_Element Sets_copyElement(const Sets_Element e, const int size);

/**
 * Creates the empty set as an Element.
 * @param size The size of the empty set to create
 * @return An empty Element. Must be freed after use.
 */
Sets_Element Sets_getEmptyElement(const int size);

/**
 * Creates the opposite of the element e.
 * @param e The element whose opposite is required
 * @param size The size of the elements
 * @return A new element which is the opposite of e. Must
 * be freed after use.
 */
Sets_Element Sets_getOpposite(const Sets_Element e, const int size);

/**
 * Builds an element from the integer corresponding to its binary form.
 * @param number The number to convert into an Element in a binary form.
 * @param nbDigits The number of digits to use for the binary form.
 * @result The corresponding Element. Must be freed after use.
 */
Sets_Element Sets_elementFromNumber(const int number, const int nbDigits);

/**
 * Gives the number corresponding to the binary form of an Element.
 * @param e The Sets_Element to convert.
 * @param nbDigits The number of digits used for the binary form.
 * @result The number corresponding to the binary form of the given Element.
 */
int Sets_numberFromElement(const Sets_Element e, const int nbDigits);

/** @} */


/* !!! Operations on sets and elements !!! */


/**
 * @name Operations on elements
 * @{
 */

/**
 * Conjunction operation for elements (remember, it can
 * be subsets...).
 * @param e1 The first element of the operation
 * @param e2 The second element of the operation
 * @param size The size of the elements
 * @return A new element equal to the conjunction of e1 and e2.
 * Must be freed after use.
 */
Sets_Element Sets_conjunction(const Sets_Element e1, const Sets_Element e2, const int size);

/**
 * Disjunction operation for elements (remember, it can
 * be subsets...).
 * @param e1 The first element of the operation
 * @param e2 The second element of the operation
 * @param size The size of the elements
 * @return A new element equal to the disjunction of e1 and e2.
 * Must be freed after use.
 */
Sets_Element Sets_disjunction(const Sets_Element e1, const Sets_Element e2, const int size);

/**
 * Create the union of two elements.
 * @param e1 The first element of the operation
 * @param e2 The second element of the operation
 * @param size The size of the elements
 * @return A new element equal to the disjunction of e1 and e2.
 * Must be freed after use.
 */
Sets_Element Sets_union(const Sets_Element e1, const Sets_Element e2, const int size);

/**
 * Compares two elements.
 * @param e1 The first element
 * @param e2 The second element to compare to
 * @param size The size of the elements
 * @return 1 if e1=e2, 0 if not.
 */
int Sets_equals(const Sets_Element e1, const Sets_Element e2, const int size);

/**
 * Tests if the element is a member of the set.
 * @param e The element to test
 * @param s The set to use for the test
 * @param size The size of the elements
 * @return 1 if e is in s, 0 if not.
 */
int Sets_isMember(const Sets_Element e, const Sets_Set s, const int size);

/**
 * Tests if the element is a subset of another one.
 * @param e1 The element that may be included
 * @param e2 The 'set' in which e1 may be included
 * @param size The size of the elements
 * @return 1 if e1 is a subset of e2, 0 if not.
 */
int Sets_isSubset(const Sets_Element e1, const Sets_Element e2, const int size);

/** @} */


/* !!! Deallocation of the memory !!! */


/**
 * @name Memory deallocation
 * @{
 */

/**
 * Frees the memory used for a ReferenceList.
 * @param rl A pointer to the ReferenceList to deallocate in memory
 */
void Sets_freeReferenceList(Sets_ReferenceList* rl);

/**
 * Frees the memory used for the values storage in an element.
 * @param e A pointer to the element to deallocate in memory
 */
void Sets_freeElement(Sets_Element* e);

/**
 * Frees the memory used by all the elements in the set.
 * @param s A pointer to the set to deallocate in memory
 */
void Sets_freeSet(Sets_Set* s);

/** @} */


/* !!! Conversion into strings !!! */


/**
 * @name Conversion to string
 * @{
 */

/**
 * Creates a natural representation of an element.
 * @param e The element to convert
 * @param rl The ReferenceList containing the real value of atoms
 * @return A string ready to print. Must be freed
 * after use.
 */
char* Sets_elementToString(const Sets_Element e, const Sets_ReferenceList rl);

/**
 * Creates a binary representation of an element.
 * @param e The element to convert
 * @param size The size of the elements
 * @return A string ready to print. Must be freed
 * after use.
 */
char* Sets_elementToBitString(const Sets_Element e, int size);

/**
 * Creates a natural representation of a set.
 * @param s The set to convert
 * @param rl The ReferenceList containing the real value of atoms
 * @return A string ready to print. Must be freed
 * after use.
 */
char* Sets_setToString(const Sets_Set s, const Sets_ReferenceList rl);

/**
 * Creates a binary representation of a set of elements.
 * @param s The set to convert
 * @param size The size of the elements
 * @return A string ready to print. Must be freed after use.
 */
char* Sets_setToBitString(const Sets_Set s, int size);

/** @} */

#endif


