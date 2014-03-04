/*
 * checkFusion.c
 *
 *  Created on: Mar 3, 2014
 *      Author: arichez
 */


#include <check.h>

#include "BeliefFunctions.h"
#include "BeliefsFromSensors.h"
#include "unit_tests.h"

#define SENSOR_NB 5
#define AKA 1
#define BEA 2
#define ELF 32

BFS_BeliefStructure beliefStructure;
BF_BeliefFunction *evidences;
BF_BeliefFunction fusedBelief;

static void setup() {
    char const *sensorTypes[] = {"S1", "S2", "S3", "S4", "S5"};
    double sensorMeasures[] = {300.0, 400.0, 345.76, 315.58, 297.42};
	beliefStructure = BFS_loadBeliefStructure("test");

	evidences = BFS_getEvidence(beliefStructure, sensorTypes, sensorMeasures, SENSOR_NB);
	fusedBelief = BF_DempsterCombination(evidences[1], evidences[2]);
}

static void teardown() {
	int i;
	BFS_freeBeliefStructure(&beliefStructure);
	for(i = 0; i < SENSOR_NB; i++){
		BF_freeBeliefFunction(&(evidences[i]));
	}
	free(evidences);
}


START_TEST(DempsterCombinationValuesAreOk) {
	/*
	 * m(Aka) = 0.39 / 0.515
	 * m(Bea) = 0.065 / 0.515
	 * m(AuB) = 0.06 / 0.515
	 * m(E) = 0
	 */
	ck_abort_msg("not implemented");
}
END_TEST


Suite *createSuite(void) {
	Suite *suite = suite_create("BeliefFunctions");
	TCase* testCaseFusion = tcase_create("Fusion");

	tcase_add_checked_fixture(testCaseFusion, setup, teardown);
	tcase_add_test(testCaseFusion, DempsterCombinationValuesAreOk);

	suite_add_tcase(suite, testCaseFusion);

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
