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


#ifndef DEF_BELIEFSFROMBELIEFS
#define DEF_BELIEFSFROMBELIEFS

#include "BeliefFunctions.h"
#include "ReadDirectory.h"
#include "ReadFile.h"



/**
 * @section BFB_intro Introduction
 * This module enables the building of belief functions from another belief function based on another frame of discernment.
 *
 * @image html ../images/Xzibit.jpg "What Xzibit thinks of this module."
 * @image latex ../images/Xzibit.jpg "What Xzibit thinks of this module." width=8cm
 *
 * The concept of this transformation is based on the paper "Propagating Belief Functions in Qualitative Markov Trees" by G. Shafer et al. (1987).
 * Using relations between frames or simply by dividing huge frames of discernment into subframes, it is possible to work on smaller frames and
 * to transfer the belief functions from one frame to another. This is called coarsening (big -> smaller) or vacuous extension (small -> bigger).
 *
 * In this module, we extend the possibilities by offering a way to manage fuzzy subsets. By this, we mean that the mass on one set could be
 * transferred to multiple sets because. It could be convenient when working with fuzzy concepts in natural language or simply because links
 * between possible world is not a perfect correlation.
 *
 * @section BFS_HowTo How to use this module
 * To build the models, you should do the following :
 * @li For each thing you want to compute, create a directory with the name you want in /data/beliefsFromBeliefs (or the directory defined by BFB_PATH).
 * For example, if you want to compute a thing called "Presence" to determine if there is someone in a room or not, just call the directory "Presence".
 * @li In this directory, create a file called "values" (or with the name defined by BFB_VALUES_NAME) with on each line, the name of
 * one possible world (and no more line than necessary !). Those worlds are the ones corresponding to the thing you want to compute.
 * Example:
 * @code
 * Aka
 * Bea
 * Elf
 * @endcode
 * This example creates three possible worlds : Aka, Bea and Elf.
 * @li In this directory, for each subframe on which beliefs will be based, build a directory with the name of that subframe.
 * @li In each of this directory, create a file also called "values" (or with the name defined by BFB_VALUES_NAME) with on each line, the name of
 * one possible world (and no more line than necessary !). Those worlds are the ones of the subframe.
 * @li In each subframe directory, for each subset of possible worlds, create a file (Those files can have any name, that
 * is not important at all...) with on first line, the number of considered worlds of the subframe, then the number of transformations and for each transformation
 * the number of considered worlds, the worlds' names and then the transformation factor.
 * Considere an example where the frame Posture = {Seated, Standing, LyingDown} has to be transformed into the frame Sleeping = {Yes, No}. The file corresponding
 * to the transformation of the element {Seated u Standing} could be written like this :
 * @code
 * 2 elements           //The number of worlds in the element of the frame we're coming from
 * Seated               //World 1
 * Standing             //World 2
 * 2 conversions        //Number of resulting elements
 * 1 element            //+-+
 * no                   //  |-> First element, {no} with a factor of 0.3
 * 0.3                  //+-+
 * 2 elements           //+-+
 * yes                  //  |-> Second element, {yes u no} with a factor of 0.7
 * no                   //  |
 * 0.7                  //+-+
 * @endcode
 * With this file, any belief m({Seated u Standing}) whould result in m({no}) = 0.3 * m({Seated u Standing}) and m({yes u no}) = 0.7 * m({Seated u Standing}).
 * If there's not a file for each possible subset of possible worlds, it is possible to get weird behaviors and sum errors.
 * Also, the sum of masses of any transformations should equal 1 or it won't work properly! Uncomment the constant CHECK_MODELS
 * in config.h to make sure your models are okay.
 *
 * WARNING: Check that your files are in the correct format for you OS (specifically for the endline character...), thus,
 * do not forget to format them correctly (or you could see ^M characters appearing in your possible worlds for example...).
 * To convert in a correct format, on Linux, it is possible to use the package 'tofrodos' with the two commands 'fromdos' and 'todos'.
 *
 * If you're not sure you understood this quick explanation, please check the example files (for instance those used
 * to run tests...).
 * 
 * Once everything has been created correctly, it is possible to load a belief structure using 
 * BFB_loadBeliefStructure(nameOfTheThingYouWantToCompute), the files will all be loaded automatically.
 * The functions BFB_believeFromBeliefs() et BFB_believeFromBelief() are then used to build the mass functions from
 * given belief functions.
 *
 * @file BeliefsFromBeliefs.h
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief APPLICATION: Gives structures and main
 *        functions to create belief functions
 *        from belief functions.
 */


/**
 * @def BFB_PATH
 * The path where belief models associated to belief should
 * be placed.
 */
#define BFB_PATH "./data/beliefsFromBeliefs/"

/**
 * @def BFB_VALUES_NAME
 * The name of the file containing the possible values of
 * a frame of discernment.
 */
#define BFB_VALUES_NAME "values"


/*
  +------------+
  | STRUCTURES |
  +------------+
*/

/**
 * The vector representing the conversion of belief on a particular element
 * into a belief on another element defined in another frame of discernement.
 * @param from The element we want to convert
 * @param to The list of element in which "from" will be converted
 * @param factors The list of factors used to convert "from" into "to"
 * @struct BFB_BeliefVector
 */
struct BFB_BeliefVector{
	Sets_Element from;
	Sets_Element* to;
	float* factors;
	int nbTos;
};
typedef struct BFB_BeliefVector BFB_BeliefVector;


/**
 * The matrix to convert belief on a frame of discernment in a belief on another
 * frame of discernment.
 * @param frameName The name of the frame of discernment from which we start
 * @param refList The reference list for the explicitation of the states names in the frame
 * @param vectors The list of vectors corresponding to the matrix
 * @struct BFB_BeliefFromBelief
 */
struct BFB_BeliefFromBelief{
	char* frameName;
	Sets_ReferenceList refList;
	BFB_BeliefVector* vectors;
	int nbVectors;
};
typedef struct BFB_BeliefFromBelief BFB_BeliefFromBelief;


/**
 * The complete belief structure with all the beliefs
 * of all the other belief on which it is possible to build metabelief.
 * @param frameName The name of the context attribute we want to compute
 * @param refList The Sets_ReferenceList corresponding to the real values of the context attribute
 * @param beliefs The model of belief to get the context attribute value
 * @param nbSensors The number of sensors in the structure
 * @struct BFB_BeliefStructure
 */
struct BFB_BeliefStructure{
    char* frameName;
    Sets_ReferenceList refList;
    BFB_BeliefFromBelief* beliefs;
    int nbBeliefs;
};
typedef struct BFB_BeliefStructure BFB_BeliefStructure;


/*
  +-----------+
  | FUNCTIONS |
  +-----------+
*/

/**
 * @name Loading a model
 * @{
 */

/**
 * Loads a belief structure from a directory. The directory must have
 * the frame of discernment name and should be in the directory defined
 * by BFB_PATH (if unchanged "./data/beliefsFromBeliefs/"). It may contain :
 * @li @c values A file with the different possible values for the
 *        frame of discernment (the name of this file is @link BFS_VALUES_NAME @endlink (if unchanged "values"). 
 *        See the function @link loadSet() @endlink for more information about the format of the file.
 * @li @c subframes A bunch of subdirectories, each one with the name
 *        of a specific subframe of discernment
 * @li @c transformations Each subframe directory may contain a collection
 *        of files, each one corresponding to the beliefs transformation.
 *        The filenames aren't important at all. See the documentation of this module itself for
 *        more information about the format of the file.
 * @param frameName The name of the frame of discernment (Name of the directory to look for.)
 * @return The complete BFB_BeliefStructure containing all the beliefs that may be used
 *         to estimate the real world state. The BFB_BeliefStructure is empty if an error occurs.
 *         (The frameName attribute should be NULL in case of error.) Must be freed after use.
 */
BFB_BeliefStructure BFB_loadBeliefStructure(const char* frameName);

/**
 * Loads the belief from belief from a directory. A bunch of files corresponding to the BFB_BeliefFromBelief
 * may be in the path (see @link loadBeliefVector() @endlink ).
 * @param frameOfOrigin The name of the subframe on which the transformation is based (name of the subdirectory)
 * @param path The path of the directory containing the different belief vectors
 * @param rl The Sets_ReferenceList containing the real values of the frame of discernment we want to compute
 * @return The BFB_BeliefFromBelief with the different BFB_BeliefVector. Must be freed after use.
 */
BFB_BeliefFromBelief BFB_loadBeliefFromBelief(const char* frameOfOrigin, const char* path, const Sets_ReferenceList rl);

/**
 * Load a belief vector corresponding to a column of the transformation matrix.
 * Example :
 * @code
 * 2 elements           //The number of worlds in the element of the frame we're coming from
 * Seated               //World 1
 * Standing             //World 2
 * 2 conversions        //Number of resulting elements
 * 1 element            //+-+
 * no                   //  |-> First element, {no} with a factor of 0.3
 * 0.3                  //+-+
 * 2 elements           //+-+
 * yes                  //  |-> Second element, {yes u no} with a factor of 0.7
 * no                   //  |
 * 0.7                  //+-+
 * @endcode
 * @param fileName The name of the file to load
 * @param rlFrom The Sets_ReferenceList containing the real values of the frame of discernment we're coming from
 * @param rlTo The Sets_ReferenceList containing the real values of the frame of discernment we're going to
 * @return A BFB_BeliefVector to be freed after use.
 */
BFB_BeliefVector BFB_loadBeliefVector(const char* fileName, const Sets_ReferenceList rlFrom, const Sets_ReferenceList rlTo);

/** @} */


/**
 * @name Building belief functions
 * @{
 */

/**
 * Get the BF_BeliefFunctions resulting of the transformations given a model
 * (BFB_BeliefStructure) and a set of BF_BeliefFunctions associated to their frame names.
 * If no corresponding model is found in the BFB_BeliefStructure, then a vacuous mass function
 * is returned.
 * @param bs The BFB_BeliefStructure in which the model is stored
 * @param frameNames A list of strings corresponding to the names of the frames of discernment
 * @param from A list a BF_BeliefFunctions to transform
 * @param nbBF The number of given belief functions
 * @return A list of BF_BeliefFunctions corresponding to the transformation of the given ones.
 */
BF_BeliefFunction* BFB_believeFromBeliefs(const BFB_BeliefStructure bs, const char* const * const frameNames, const BF_BeliefFunction* from, const int nbBF);

/**
 * Get the BF_BeliefFunction resulting of the transformation given a specific model.
 * @param bfb The BFB_BeliefFromBelief model to use for the transformation
 * @param from The BF_BeliefFunction to transform
 * @param elementSize The number of digits to use in the resulting belief function.
 */
BF_BeliefFunction BFB_believeFromBelief(const BFB_BeliefFromBelief bfb, const BF_BeliefFunction from, const int elementSize);

/** @} */


/**
 * @name Free memory
 * @{
 */

/**
 * Frees the memory used for the BFB_BeliefStructure.
 * @param bs A pointer to the BFB_BeliefStructure to free
 */
void BFB_freeBeliefStructure(BFB_BeliefStructure* bs);

/**
 * Frees the memory used for the BFB_BeliefFromBelief.
 * @param bfb A pointer to the BFS_BeliefFromBelief to free
 */
void BFB_freeBeliefFromBelief(BFB_BeliefFromBelief* bfb);

/**
 * Frees the memory used for the BFB_BeliefVector.
 * @param bv A pointer to the BFS_BeliefVector to free
 */
void BFB_freeBeliefVector(BFB_BeliefVector* bv);

/** @} */


/**
 * @name Conversion into strings
 * @}
 */

/**
 * Converts a BFB_BeliefStructure into a string ready to print.
 * @param bs The BFB_BeliefStructure to convert
 * @return A string ready to print representing the BFB_BeliefStructure.
 *         Must be freed after use.
 */
char* BFB_beliefStructureToString(const BFB_BeliefStructure bs);

/**
 * Converts a BFB_BeliefFromBelief into a string ready to print.
 * @param bfb The BFB_BeliefFromBelief to convert
 * @param to The Sets_ReferenceList to use to get the real values of resulting elements
 * @return A string ready to print representing the BFB_BeliefFromBelief.
 *         Must be freed after use.
 */
char* BFB_beliefFromBeliefToString(const BFB_BeliefFromBelief bfb, const Sets_ReferenceList to);

/**
 * Converts a BFB_BeliefVector into a string ready to print.
 * @param bv The BFB_BeliefVector to convert
 * @param to The Sets_ReferenceList to use to get the real values of resulting elements
 * @param from The Sets_ReferenceList to use to the real values of elements to transform
 * @return A string ready to print representing the BFB_BeliefVector.
 *         Must be freed after use.
 */
char* BFB_beliefVectorToString(const BFB_BeliefVector bv, const Sets_ReferenceList to, const Sets_ReferenceList from);


/** @} */

#endif


