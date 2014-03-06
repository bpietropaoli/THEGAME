/*
 * checkBeliefCombination.c
 *
 *  Created on: Mar 6, 2014
 *      Author: arichez
 */

#include <check.h>


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
	beliefStructure = BFS_loadBeliefStructure("unittest");

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
	assert_flt_equals(0.45f, BF_M(SmetsFusedBelief, A), BF_PRECISION);
	assert_flt_equals(0.025f, BF_M(SmetsFusedBelief, B), BF_PRECISION);
	assert_flt_equals(0.0f, BF_M(SmetsFusedBelief, C), BF_PRECISION);
	assert_flt_equals(0.525f, BF_M(SmetsFusedBelief, VOID), BF_PRECISION);
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
	assert_flt_equals(0.45 / 0.475, BF_M(DempsterfusedBelief, A), BF_PRECISION);
	assert_flt_equals(0.025 / 0.475, BF_M(DempsterfusedBelief, B), BF_PRECISION);
	assert_flt_equals(0.0f, BF_M(DempsterfusedBelief, VOID), BF_PRECISION);
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
