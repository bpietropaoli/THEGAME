/*
 * checkFusion.c
 *
 *  Created on: Mar 3, 2014
 *      Author: arichez
 */


#include <check.h>
#include <stdlib.h>

#include "BeliefFunctions.h"
#include "BeliefsFromSensors.h"
#include "unit_tests.h"


BFS_BeliefStructure beliefStructure;
BF_BeliefFunction *evidences = NULL;
Sets_Set powerset;


static void setup() {
    char const *sensorTypes[] = {"S1", "S2"};
    double sensorMeasures[] = {300.0, 400.0};
	beliefStructure = BFS_loadBeliefStructure("unittest");

	evidences = BFS_getEvidence(beliefStructure, sensorTypes, sensorMeasures, SENSOR_NB);
}

static void teardown() {
	int i;
	BFS_freeBeliefStructure(&beliefStructure);
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
			beliefStructure.powerset);
	float value = BF_M(evidences[0], element.element);
	ck_assert(Sets_equals(element.element, A, ATOM_NB));
	assert_flt_equals(0.75f, value, BF_PRECISION);
	BF_freeBeliefPoint(&element);
}
END_TEST

START_TEST(getMaxBetPReturnsTheRightValues) {
	/* The max for the first evidence function should be 1.0 for {AuB} */
	BF_FocalElement focalPoint = BF_getMax(BF_betP, evidences[0], 0,
			beliefStructure.powerset);
	float value = BF_betP(evidences[0], focalPoint.element);
	assert_flt_equals(1.0f, value, BF_PRECISION);
	ck_assert_msg(Sets_equals(focalPoint.element, Sets_union(A,B, ATOM_NB),ATOM_NB),
			"BetP did not return AKA u BEA");
}
END_TEST

START_TEST(getMaxBetWithCardLimitReturnsTheRightValues) {
	/* The max with 1 as card for the first evidence function should be 0.825 for {A} */
	BF_FocalElement focalPoint = BF_getMax(BF_betP, evidences[0], 1,
			beliefStructure.powerset);
	float value = BF_betP(evidences[0], focalPoint.element);
	assert_flt_equals(0.825f, value, BF_PRECISION);
	ck_assert_msg(Sets_equals(focalPoint.element, A, ATOM_NB),
			"BetP did not return AKA u BEA");
}
END_TEST

/*
 * ## getMin
 */

START_TEST(getMinMassReturnsTheRightValues) {
	/* The minimum focal element (i.e. > 0) for the first evidence function should be B with 0.1*/
	BF_FocalElement element = BF_getMin(BF_M, evidences[0], 0,
			beliefStructure.powerset);
	float value = BF_M(evidences[0], element.element);
	ck_assert(Sets_equals(element.element, B, ATOM_NB));
	assert_flt_equals(0.1f, value, BF_PRECISION);
	BF_freeBeliefPoint(&element);
}
END_TEST

START_TEST(getMinBetPWithCardLimitReturnsTheRightValues) {
	/* The minimum focal element (i.e. > 0) for the first evidence function should be B with 0.175.
	 * We apply a limit of 1 for the card */
	BF_FocalElement element = BF_getMin(BF_betP, evidences[0], 1,
			beliefStructure.powerset);
	float value = BF_betP(evidences[0], element.element);
	ck_assert(Sets_equals(element.element, B, ATOM_NB));
	assert_flt_equals(0.175f, value, BF_PRECISION);
	BF_freeBeliefPoint(&element);
}
END_TEST

/*
 * ## getListMax
 */

START_TEST(getListMaxMassReturnsTheRightNumberOfValues) {
	/* There should be two max for the second belief function */
	BF_FocalElementList list = BF_getMaxList(BF_M, evidences[1], 0,
			beliefStructure.powerset);
	ck_assert_int_eq(2, list.size);
	BF_freeFocalElementList(&list);
}
END_TEST

START_TEST(getListMaxMassReturnsTheRightFocals) {
	BF_FocalElementList list = BF_getMaxList(BF_M, evidences[1], 0,
				beliefStructure.powerset);
	int i;
	for (i = 0; i < list.size; ++i) {
		ck_assert_msg(Sets_equals( C,list.elements[0].element, ATOM_NB) ||
				Sets_equals( AuC, list.elements[0].element, ATOM_NB),
				"One of the max is not C or AuC.");
	}
	BF_freeFocalElementList(&list);
}
END_TEST

/*
 * ## getListMin
 */

START_TEST(getListMinMassReturnsTheRightNumberOfValues) {
	/* There should be two min for the second belief function */
	BF_FocalElementList list = BF_getMinList(BF_M, evidences[1], 0,
					beliefStructure.powerset);
	ck_assert_int_eq(2, list.size);
	BF_freeFocalElementList(&list);
}
END_TEST

START_TEST(getListMinMassReturnsTheRightFocals) {
	/* There should be two min for the second belief function */
	BF_FocalElementList list = BF_getMinList(BF_M, evidences[1], 0,
					beliefStructure.powerset);
	int i;
	for(i = 0; i < list.size;++i) {
		ck_assert_msg(Sets_equals( A,list.elements[0].element, ATOM_NB) ||
						Sets_equals( B, list.elements[0].element, ATOM_NB),
						"One of the max is not A or B.");
	}
	BF_freeFocalElementList(&list);
}
END_TEST

TCase* createManipulationTestCase() {
TCase* testCaseManipulation = tcase_create("Manipulation");
tcase_add_checked_fixture(testCaseManipulation, setup, teardown);
tcase_add_test(testCaseManipulation, getMaxMassReturnsTheRightValues);
tcase_add_test(testCaseManipulation, getMaxBetPReturnsTheRightValues);
tcase_add_test(testCaseManipulation, getMaxBetWithCardLimitReturnsTheRightValues);
tcase_add_test(testCaseManipulation, getMinMassReturnsTheRightValues);
tcase_add_test(testCaseManipulation, getMinBetPWithCardLimitReturnsTheRightValues);
tcase_add_test(testCaseManipulation, getListMaxMassReturnsTheRightNumberOfValues);
tcase_add_test(testCaseManipulation, getListMaxMassReturnsTheRightFocals);
tcase_add_test(testCaseManipulation, getListMinMassReturnsTheRightNumberOfValues);
tcase_add_test(testCaseManipulation, getListMinMassReturnsTheRightFocals);
return testCaseManipulation;
}




Suite *createSuite(void) {
	Suite *suite = suite_create("BeliefFunctions");
	suite_add_tcase(suite, createManipulationTestCase());

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
