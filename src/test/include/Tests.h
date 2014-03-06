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

#ifndef DEF_TESTS
#define DEF_TESTS
#include "BeliefFunctions.h"
#include "BeliefsFromSensors.h"
#include "BeliefsFromBeliefs.h"
#include "BeliefsFromRandomness.h"
#include "BeliefCombinations.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @file Tests.h
 * @author Bastien Pietropaoli (bastien.pietropaoli@inria.fr)
 * @brief TESTS: Run runTests() to test that everything works fine on your configuration.
 */
 
/**
 * A typical fusion problem.
 * @param bs The associated belief structure to test
 * @param sensorTypes The strings of sensor types
 * @param sensorMeasures The measures
 * @param nbSensors The number of sensors
 * @param write Write in result file or not
 */
void Tests_typicalProblem(BFS_BeliefStructure bs, char** sensorTypes, double* sensorMeasures, int nbSensors, int write);

/**
 * Tests the generation of random mass functions.
 * @param nbIterations The number of mass functions to generate.
 */
void Tests_beliefsFromRandomness(int nbIterations);

/**
 * Tests the generation of mass functions from sensor measures.
 */
void Tests_beliefsFromSensors();

/**
 * Tests the options of the module "BeliefsFromSensors".
 */
void Tests_beliefsFromSensorsOptions();

/**
 * Generates some tests for the temporization based on fusion.
 */
void Tests_tempo_fusion();

/**
 * Generates some tests for the temporization based on specificity.
 */
void Tests_tempo_specificity();

/**
 * Tests the generation of mass functions from other mass functions.
 */
void Tests_beliefsFromBeliefs();

/**
 * A huge function to run tests and a bench on a typical case.
 * @param nbIterations The number of iterations used to do the bench
 * @param write Does it have to write every result of the bench in result files
 * @return Returns 0 if everything is fine
 */
int Tests_runTests(int nbIterations, int write);

#endif

