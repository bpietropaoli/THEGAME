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
 

#ifndef DEF_BELIEFSFROMRANDOMNESS
#define DEF_BELIEFSFROMRANDOMNESS
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include "config.h"
#include "Sets.h"
#include "BeliefFunctions.h"

/**
 * This module provides with methods to randomly generate mass functions. It is roughly based on common
 * algorithms to do so. An interesting paper on the subject is "Random Generation of Mass Functions:
 * A Short Howto" by Thomas Burger and Sébastien Destercke (2012).
 *
 * @file BeliefsFromRandomness.h
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief APPLICATION: Gives structures and main
 *        functions to create random belief functions.
 */

/**
 * @name Utility functions
 * @{
 */

/**
 * Generates a seed based on the date and the hour.
 * This function should be called each time a new seed is required.
 */
void BFR_generateRandomSeed();

/**@}*/

/**
 * @name Random generators
 * @{
 */

/**
 * Generates a crappy random belief function with a random number of focals elements,
 * random focal elements and random mass values. It is crap as the mass values and the
 * size of focal elements are not well distributed.
 * @param elementSize The number of possible states/worlds.
 * @return A random belief function. 
 */
BF_BeliefFunction BFR_getCrappyRandomBelief(const int elementSize);

/**
 * Generates a crappy random belief function with a fixed number of focal elements but those
 * elements and their mass values are random. A constraint nbFocals <= 2^elementSize has to
 * be respected. Returns a null BF_BeliefFunction if not.
 * @param elementSize The number of possible states/worlds.
 * @param nbFocals The number of focal elements wanted.
 * @param A random belief function.
 */
BF_BeliefFunction BFR_getCrappyRandomBeliefWithFixedNbFocals(const int elementSize, const int nbFocals);

/**@}*/


#endif



