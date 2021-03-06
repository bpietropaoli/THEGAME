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

/*
 * checkBeliefCombination.c
 *
 *  Created on: Mar 6, 2014
 *      Author: arichez
 */

#include <check.h>
#include <stdlib.h>

#include "BeliefCombinations.h"
#include "BeliefsFromSensors.h"

#include "unit_tests.h"
#define SENSOR_NB 2

BFS_BeliefStructure beliefStructure;
BF_BeliefFunction *evidences = NULL;
BF_BeliefFunction DempsterfusedBelief, SmetsFusedBelief;


static void setup() {
    char const *sensorTypes[] = {"S1", "S2"};
    double sensorMeasures[] = {300.0, 400.0};
	beliefStructure = BFS_loadBeliefStructure(BELIEF_DEFINITION_PATH, "unittest");

	evidences = BFS_getEvidence(beliefStructure, sensorTypes, sensorMeasures, SENSOR_NB);
	SmetsFusedBelief = BF_SmetsCombination(evidences[0], evidences[1]);
	DempsterfusedBelief = BF_DempsterCombination(evidences[0], evidences[1]);
}

static void teardown() {
	int i;
	BFS_freeBeliefStructure(&beliefStructure);
	BF_freeBeliefFunction(&SmetsFusedBelief);
	BF_freeBeliefFunction(&DempsterfusedBelief);
	for(i = 0; i < SENSOR_NB; i++){
		BF_freeBeliefFunction(&(evidences[i]));
	}
	free(evidences);
}


/*
 * Fusion tests
 * ============
 */

/* ##Smets */
START_TEST(SmetsCombinationValuesAreOk) {
	/*
	 * expected values :
	 * m(A) = 0.45
	 * m(B) = 0.025
	 * m(void) = 0.525
	 * m(C) = 0
	 */
	assert_flt_equals(0.45f, BF_m(SmetsFusedBelief, A), BF_PRECISION);
	assert_flt_equals(0.025f, BF_m(SmetsFusedBelief, B), BF_PRECISION);
	assert_flt_equals(0.0f, BF_m(SmetsFusedBelief, C), BF_PRECISION);
	assert_flt_equals(0.525f, BF_m(SmetsFusedBelief, VOID), BF_PRECISION);
}
END_TEST

/* ##Dempster */
START_TEST(DempsterCombinationValuesAreOk) {
	/*
	 * expected values :
	 * m(A) = 0.45 / 0.475
	 * m(B) = 0.025 / 0.475
	 * m(AuB) = 0
	 * m(C) = 0
	 */
	assert_flt_equals(0.45 / 0.475, BF_m(DempsterfusedBelief, A), BF_PRECISION);
	assert_flt_equals(0.025 / 0.475, BF_m(DempsterfusedBelief, B), BF_PRECISION);
	assert_flt_equals(0.0f, BF_m(DempsterfusedBelief, VOID), BF_PRECISION);
}
END_TEST


TCase* createFusionTestCase() {
TCase* testCaseFusion = tcase_create("Fusion");
tcase_add_checked_fixture(testCaseFusion, setup, teardown);
tcase_add_test(testCaseFusion, SmetsCombinationValuesAreOk);
tcase_add_test(testCaseFusion, DempsterCombinationValuesAreOk);
return testCaseFusion;
}

Suite *createSuite(void) {
	Suite *suite = suite_create("BeliefFunctions");
	suite_add_tcase(suite, createFusionTestCase());

	return suite;
}


int main() {
	int numberFailed = 0;
	Suite *suite = createSuite();
	SRunner *suiteRunner= srunner_create(suite);
	srunner_run_all(suiteRunner, CK_NORMAL);
	numberFailed = srunner_ntests_failed (suiteRunner);
	srunner_free(suiteRunner);
	return (numberFailed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
