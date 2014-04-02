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
 * checkSets.c
 *
 *  Created on: Mar 5, 2014
 *      Author: arichez
 */

#include <stdlib.h>
#include <check.h>

#include "Sets.h"
#include "unit_tests.h"

START_TEST(testCreationFromArray){
	const char *worlds[] = {"A", "B", "C"};
	Sets_ReferenceList refList = Sets_createRefListFromArray(worlds, 3);
	ck_assert_int_eq(3, refList.card);
	ck_assert_str_eq("A", refList.values[0]);
	ck_assert_str_eq("B", refList.values[1]);
	ck_assert_str_eq("C", refList.values[2]);
	Sets_freeReferenceList(&refList);
}
END_TEST

START_TEST(testDisjunction1) {
	/*
	 * union of A and B should give AuB
	 */
	Sets_Element setUnion = Sets_disjunction(A, B, ATOM_NB);
	ck_assert_msg(Sets_equals(setUnion, AuB, ATOM_NB), "Sets_union(A,B) did not equal AuB");
	Sets_freeElement(&setUnion);
}
END_TEST

START_TEST(testDisjunction2) {
	/*
	 * union of AuB and C should give AuBuC
	 */
	Sets_Element setUnion = Sets_disjunction(AuB, C, ATOM_NB);
	ck_assert_msg(Sets_equals(setUnion, AuBuC, ATOM_NB), "Sets_union(AuB,C) did not equal AuBuC");
	Sets_freeElement(&setUnion);
}
END_TEST

Suite *createSuite(void) {
	Suite *suite = suite_create("Sets");

	TCase *testCaseCreation = tcase_create("Creation");
	tcase_add_test(testCaseCreation, testCreationFromArray);

	TCase* testCaseManipulation = tcase_create("Manipulation");
	tcase_add_test(testCaseManipulation, testDisjunction1);
	tcase_add_test(testCaseManipulation, testDisjunction2);


	suite_add_tcase(suite, testCaseCreation);
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
