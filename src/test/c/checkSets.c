/*
 * checkSets.c
 *
 *  Created on: Mar 5, 2014
 *      Author: arichez
 */

#include <stdlib.h>
#include <check.h>

#include "Sets.h"
#include "unit_tests.h"



START_TEST(testUnion1) {
	/*
	 * union of A and B should give AuB
	 */
	Sets_Element setUnion = Sets_union(A, B, ATOM_NB);
	ck_assert_msg(Sets_equals(setUnion, AuB, ATOM_NB), "Sets_union(A,B) did not equal AuB");
	Sets_freeElement(&setUnion);
}
END_TEST

START_TEST(testUnion2) {
	/*
	 * union of AuB and C should give AuBuC
	 */
	Sets_Element setUnion = Sets_union(AuB, C, ATOM_NB);
	ck_assert_msg(Sets_equals(setUnion, AuBuC, ATOM_NB), "Sets_union(AuB,C) did not equal AuBuC");
	Sets_freeElement(&setUnion);
}
END_TEST

Suite *createSuite(void) {
	Suite *suite = suite_create("BeliefFunctions");
	TCase* testCaseManipulation = tcase_create("Manipulation");
	tcase_add_test(testCaseManipulation, testUnion1);
	tcase_add_test(testCaseManipulation, testUnion2);
	;
	suite_add_tcase(suite, testCaseManipulation);
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
