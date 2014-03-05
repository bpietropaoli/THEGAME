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

#define SET_ELEMENT_DECLARE(name, val1, val2, val3) \
	static char name ##  _VAL[] = {val1, val2, val3}; \
	static Sets_Element name = {name ##  _VAL, val1 + val2 + val3}

SET_ELEMENT_DECLARE(AKA, 1,0,0);
SET_ELEMENT_DECLARE(BEA, 0,1,0);
SET_ELEMENT_DECLARE(ELF, 0,0,1);
SET_ELEMENT_DECLARE(VOID, 0,0,0);


BFS_BeliefStructure beliefStructure;
BF_BeliefFunction *evidences;
BF_BeliefFunction DempsterfusedBelief, SmetsFusedBelief;


static void setup() {
    char const *sensorTypes[] = {"S1", "S2", "S3", "S4", "S5"};
    double sensorMeasures[] = {300.0, 400.0, 345.76, 315.58, 297.42};
	beliefStructure = BFS_loadBeliefStructure("test");

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
 * Manipulation Tests
 * ==================
 */
START_TEST(getMaxMassReturnsTheRightValues) {
	/* The max for the first evidence function should be 0.75 for Aka*/
	BF_FocalElement element = BF_getMax(BF_M, evidences[0], 0,
			Sets_generatePowerSet(evidences[0].elementSize));
	float value = BF_M(evidences[0], element.element);
	ck_assert(Sets_equals(element.element, AKA, 3));
	assert_flt_equals(0.75f, value, BF_PRECISION);
	BF_freeBeliefPoint(&element);
}
END_TEST

START_TEST(getMaxBetPReturnsTheRightValues) {
	/* The max for the first evidence function should be 1.0 for {Aka u Bea u Elf} */
	BF_FocalElement focalPoint = BF_getMax(BF_betP, evidences[0], 0,
			Sets_generatePowerSet(evidences[0].elementSize));
	float value = BF_betP(evidences[0], focalPoint.element);
	assert_flt_equals(1.0f, value, BF_PRECISION);
	ck_assert_msg(Sets_equals(focalPoint.element, Sets_union(AKA,BEA, 3),3),
			"BetP did not return AKA u BEA");
}
END_TEST

/*
 * Fusion tests
 * ============
 */
START_TEST(SmetsCombinationValuesAreOk) {
	/*
	 * expected values :
	 * m(Aka) = 0.45
	 * m(Bea) = 0.025
	 * m(void) = 0.525
	 * m(E) = 0
	 */
	assert_flt_equals(0.45f, BF_M(SmetsFusedBelief, AKA), BF_PRECISION);
	assert_flt_equals(0.025f, BF_M(SmetsFusedBelief, BEA), BF_PRECISION);
	assert_flt_equals(0.0f, BF_M(SmetsFusedBelief, ELF), BF_PRECISION);
	assert_flt_equals(0.525f, BF_M(SmetsFusedBelief, VOID), BF_PRECISION);
}
END_TEST

START_TEST(DempsterCombinationValuesAreOk) {
	/*
	 * expected values :
	 * m(Aka) = 0.45 / 0.475
	 * m(Bea) = 0.025 / 0.475
	 * m(AuB) = 0
	 * m(E) = 0
	 */
	assert_flt_equals(0.45 / 0.475, BF_M(DempsterfusedBelief, AKA), BF_PRECISION);
	assert_flt_equals(0.025 / 0.475, BF_M(DempsterfusedBelief, BEA), BF_PRECISION);
	assert_flt_equals(0.0f, BF_M(DempsterfusedBelief, VOID), BF_PRECISION);
}
END_TEST


TCase* createManipulationTestCase() {
TCase* testCaseManipulation = tcase_create("Manipulation");
tcase_add_checked_fixture(testCaseManipulation, setup, teardown);
tcase_add_test(testCaseManipulation, getMaxMassReturnsTheRightValues);
tcase_add_test(testCaseManipulation, getMaxBetPReturnsTheRightValues);
return testCaseManipulation;
}

TCase* createFusionTestCase() {
TCase* testCaseFusion = tcase_create("Fusion");
tcase_add_checked_fixture(testCaseFusion, setup, teardown);
tcase_add_test(testCaseFusion, SmetsCombinationValuesAreOk);
tcase_add_test(testCaseFusion, DempsterCombinationValuesAreOk);
return testCaseFusion;
}



Suite *createSuite(void) {
	Suite *suite = suite_create("BeliefFunctions");
	suite_add_tcase(suite, createManipulationTestCase());
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
