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

#define SENSOR_NB 2

#define SET_ELEMENT_DECLARE(name, val1, val2, val3) \
	static char name ##  _VAL[] = {val1, val2, val3}; \
	static Sets_Element name = {name ##  _VAL, val1 + val2 + val3}

SET_ELEMENT_DECLARE(A, 1,0,0);
SET_ELEMENT_DECLARE(B, 0,1,0);
SET_ELEMENT_DECLARE(C, 0,0,1);
SET_ELEMENT_DECLARE(VOID, 0,0,0);


BFS_BeliefStructure beliefStructure;
BF_BeliefFunction *evidences;
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
 * Manipulation Tests
 * ==================
 */

/*
 * ## getMax
 */

START_TEST(getMaxMassReturnsTheRightValues) {
	/* The max for the first evidence function should be 0.75 for {A}*/
	BF_FocalElement element = BF_getMax(BF_M, evidences[0], 0,
			Sets_generatePowerSet(evidences[0].elementSize));
	float value = BF_M(evidences[0], element.element);
	ck_assert(Sets_equals(element.element, A, 3));
	assert_flt_equals(0.75f, value, BF_PRECISION);
	BF_freeBeliefPoint(&element);
}
END_TEST

START_TEST(getMaxBetPReturnsTheRightValues) {
	/* The max for the first evidence function should be 1.0 for {AuB} */
	BF_FocalElement focalPoint = BF_getMax(BF_betP, evidences[0], 0,
			Sets_generatePowerSet(evidences[0].elementSize));
	float value = BF_betP(evidences[0], focalPoint.element);
	assert_flt_equals(1.0f, value, BF_PRECISION);
	ck_assert_msg(Sets_equals(focalPoint.element, Sets_union(A,B, 3),3),
			"BetP did not return AKA u BEA");
}
END_TEST

START_TEST(getMaxBetWithCardLimitReturnsTheRightValues) {
	/* The max with 1 as card for the first evidence function should be 0.825 for {A} */
	BF_FocalElement focalPoint = BF_getMax(BF_betP, evidences[0], 1,
			Sets_generatePowerSet(evidences[0].elementSize));
	float value = BF_betP(evidences[0], focalPoint.element);
	assert_flt_equals(0.825f, value, BF_PRECISION);
	ck_assert_msg(Sets_equals(focalPoint.element, A,3),
			"BetP did not return AKA u BEA");
}
END_TEST

/*
 * ## getMin
 */

START_TEST(getMinMassReturnsTheRightValues) {
	/* The minimum focal element (i.e. > 0) for the first evidence function should be B with 0.1*/
	BF_FocalElement element = BF_getMin(BF_M, evidences[0], 0,
			Sets_generatePowerSet(evidences[0].elementSize));
	float value = BF_M(evidences[0], element.element);
	ck_assert(Sets_equals(element.element, B, 3));
	assert_flt_equals(0.1f, value, BF_PRECISION);
	BF_freeBeliefPoint(&element);
}
END_TEST

START_TEST(getMinBetPReturnsTheRightValues) {
	/* The minimum focal element (i.e. > 0) for the first evidence function should be B with 0.175*/
	BF_FocalElement element = BF_getMin(BF_M, evidences[0], 0,
			Sets_generatePowerSet(evidences[0].elementSize));
	float value = BF_betP(evidences[0], element.element);
	ck_assert(Sets_equals(element.element, B, 3));
	assert_flt_equals(0.175f, value, BF_PRECISION);
	BF_freeBeliefPoint(&element);
}
END_TEST

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


TCase* createManipulationTestCase() {
TCase* testCaseManipulation = tcase_create("Manipulation");
tcase_add_checked_fixture(testCaseManipulation, setup, teardown);
tcase_add_test(testCaseManipulation, getMaxMassReturnsTheRightValues);
tcase_add_test(testCaseManipulation, getMaxBetPReturnsTheRightValues);
tcase_add_test(testCaseManipulation, getMaxBetWithCardLimitReturnsTheRightValues);
tcase_add_test(testCaseManipulation, getMinMassReturnsTheRightValues);
tcase_add_test(testCaseManipulation, getMinBetPReturnsTheRightValues);
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
