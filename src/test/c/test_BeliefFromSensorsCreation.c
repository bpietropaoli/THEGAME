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


#include <stdlib.h>
#include <check.h>
#include <unistd.h>

#include "BeliefsFromSensors.h"
#include "unit_tests.h"


#define  checkOptionFlags(structure, sensorName, opts) \
	ck_assert_msg(getSensorBelief(structure, sensorName)->optionFlags == opts, \
			"wrong option flags for %s", sensorName);

BFS_BeliefStructure beliefStructure;

static void setup() {
	const char *values[] = {"A", "B", "C"};
	beliefStructure = BFS_createBeliefStructure(STRUCTURE_NAME, values, 3);
}

static void teardown() {
	BFS_freeBeliefStructure(&beliefStructure);
}

START_TEST(beliefStructureNameIsOk) {
	ck_assert_str_eq(STRUCTURE_NAME, beliefStructure.frameName);
}
END_TEST

START_TEST(beliefRefListSizeIsOk) {
	ck_assert_int_eq(3, beliefStructure.refList.card);
}
END_TEST

START_TEST(beliefStructureValuesAreOk) {
	/* The reference list size should be 2 with "yes" and "no" as values.
	 */
	Sets_ReferenceList refList = beliefStructure.refList;
	ck_assert_int_eq(3, refList.card);
	ck_assert_str_eq("A", refList.values[0]);
	ck_assert_str_eq("B", refList.values[1]);
	ck_assert_str_eq("C", refList.values[2]);
}
END_TEST

START_TEST(powersetCardsAreOk) {
	/*
	 * powerset are stored in the order : {{void}, {yes}, {no}, {yes u no}}
	 */
	Sets_Set powerSet = beliefStructure.powerset;
	ck_assert_int_eq(8, powerSet.card);
	ck_assert_int_eq(0, powerSet.elements[0].card);
	ck_assert_int_eq(1, powerSet.elements[1].card);
	ck_assert_int_eq(1, powerSet.elements[2].card);
	ck_assert_int_eq(2, powerSet.elements[3].card);
}
END_TEST

START_TEST(sensorNbis0BeforeAddingOne) {
	ck_assert_int_eq(0, beliefStructure.nbSensors);
}
END_TEST


static void addSensors(BFS_BeliefStructure *beliefStructure) {
	BFS_SensorBeliefs sensorBeliefs = BFS_createSensorBeliefs("S1");
	BFS_putSensorBelief(beliefStructure, sensorBeliefs);
	sensorBeliefs = BFS_createSensorBeliefs("S2");
	BFS_putSensorBelief(beliefStructure, sensorBeliefs);
}

START_TEST(sensorNbIsOkAfterAddingSensors) {
	addSensors(&beliefStructure);
	ck_assert_int_eq(2, beliefStructure.nbSensors);
}
END_TEST

START_TEST(sensorNamesAreOkAfterAddingSensors) {
	addSensors(&beliefStructure);
	ck_assert_str_eq("S1", beliefStructure.beliefs[0].sensorType);
	ck_assert_str_eq("S2", beliefStructure.beliefs[1].sensorType);
}
END_TEST

/*
 * =============== sensor beliefs creation ===============
 */

BFS_SensorBeliefs sensorBeliefs;

static void createSensorBelief() {
	sensorBeliefs = BFS_createSensorBeliefs("S1");
}

static void cleanSensorBelief() {
	BFS_freeSensorBeliefs(&sensorBeliefs);
}

START_TEST(sensorNameIsOk) {
	ck_assert_str_eq("S1", sensorBeliefs.sensorType);
}
END_TEST

START_TEST(sensorOptionsAreOk) {
	ck_assert_int_eq(OP_NONE, sensorBeliefs.optionFlags);
	BFS_addOption(&sensorBeliefs, OP_TEMPO_SPECIFICITY, 3.0);
	ck_assert_int_eq(OP_TEMPO_SPECIFICITY, sensorBeliefs.optionFlags);
}
END_TEST

START_TEST(addingOptionIncrementOptionNb) {
	BFS_addOption(&sensorBeliefs, OP_VARIATION, 3.0);
	ck_assert_int_eq(1, sensorBeliefs.nbOptions);
	BFS_addOption(&sensorBeliefs, OP_TEMPO_FUSION, 3.0);
	ck_assert_int_eq(2, sensorBeliefs.nbOptions);
}
END_TEST


START_TEST(addingOPNoneDoesNotIncrementOptionNb) {
	BFS_addOption(&sensorBeliefs, OP_NONE, 3.0);
	ck_assert_int_eq(0, sensorBeliefs.nbOptions);
}
END_TEST

START_TEST(addingNewFocalPointIncrementFocalNb) {
	BFS_addPointTosensorBelief(&sensorBeliefs, A, ATOM_NB, 100, 0.0);
	ck_assert_int_eq(1, sensorBeliefs.nbFocal);
}
END_TEST

static void addPoints(BFS_SensorBeliefs* sensorBeliefs) {
	BFS_addPointTosensorBelief(sensorBeliefs, A, ATOM_NB, 100, 0.0);
	BFS_addPointTosensorBelief(sensorBeliefs, A, ATOM_NB, 200, 0.25);
}

static void addPointsInWrongOrder(BFS_SensorBeliefs* sensorBeliefs) {
	BFS_addPointTosensorBelief(sensorBeliefs, A, ATOM_NB, 200, 0.25);
	BFS_addPointTosensorBelief(sensorBeliefs, A, ATOM_NB, 100, 0.0);
}

START_TEST(addingExistingFocalPointDoesNotIncrementFocalNb) {
	addPoints(&sensorBeliefs);
	ck_assert_int_eq(1, sensorBeliefs.nbFocal);
}
END_TEST

START_TEST(sensorBeliefContainsA) {
	addPoints(&sensorBeliefs);
	ck_assert(Sets_equals(sensorBeliefs.beliefOnElements[0].focalElement, A, ATOM_NB));
}
END_TEST

START_TEST(sensorBeliefContainsTheRightValueNb) {
	addPoints(&sensorBeliefs);
	ck_assert_int_eq(2, sensorBeliefs.beliefOnElements[0].nbPts);
}
END_TEST

START_TEST(sensorBeliefContainsTheRightValues) {
	addPoints(&sensorBeliefs);
	assert_flt_equals(100, sensorBeliefs.beliefOnElements[0].points[0].sensorValue, 0);
	assert_flt_equals(200, sensorBeliefs.beliefOnElements[0].points[1].sensorValue, 0);
}
END_TEST


START_TEST(sensorBeliefContainsTheRightMasses) {
	addPoints(&sensorBeliefs);
	assert_flt_equals(0, sensorBeliefs.beliefOnElements[0].points[0].belief, 0);
	assert_flt_equals(0.25, sensorBeliefs.beliefOnElements[0].points[1].belief, 0);
}
END_TEST

START_TEST(sensorBeliefContainsValuesInTheRightOrder) {
	addPointsInWrongOrder(&sensorBeliefs);
	assert_flt_equals(100, sensorBeliefs.beliefOnElements[0].points[0].sensorValue, 0);
	assert_flt_equals(200, sensorBeliefs.beliefOnElements[0].points[1].sensorValue, 0);
}
END_TEST

static TCase* createSensorBeliefTestCase() {
	TCase* testCase = tcase_create("SensorBeliefCreation");
	tcase_add_checked_fixture(testCase, createSensorBelief, cleanSensorBelief);
	tcase_add_test(testCase, sensorNameIsOk);
	tcase_add_test(testCase, sensorOptionsAreOk);
	tcase_add_test(testCase, addingOptionIncrementOptionNb);
	tcase_add_test(testCase, addingOPNoneDoesNotIncrementOptionNb);
	tcase_add_test(testCase, addingNewFocalPointIncrementFocalNb);
	tcase_add_test(testCase, addingExistingFocalPointDoesNotIncrementFocalNb);
	tcase_add_test(testCase, sensorBeliefContainsA);
	tcase_add_test(testCase, sensorBeliefContainsTheRightValueNb);
	tcase_add_test(testCase, sensorBeliefContainsTheRightValues);
	tcase_add_test(testCase, sensorBeliefContainsTheRightMasses);
	tcase_add_test(testCase, sensorBeliefContainsValuesInTheRightOrder);

	return testCase;
}

static TCase* createBeliefStructureTestCase() {
	TCase* testCase = tcase_create("BeliefStructureCreation");
	tcase_add_checked_fixture(testCase, setup, teardown);
	tcase_add_test(testCase, beliefStructureNameIsOk);
	tcase_add_test(testCase, beliefRefListSizeIsOk);
	tcase_add_test(testCase, beliefStructureValuesAreOk);
	tcase_add_test(testCase, powersetCardsAreOk);
	tcase_add_test(testCase, sensorNbis0BeforeAddingOne);
	tcase_add_test(testCase, sensorNbIsOkAfterAddingSensors);
	tcase_add_test(testCase, sensorNamesAreOkAfterAddingSensors);

	return testCase;

}

Suite *createSuite(void) {
	Suite *suite = suite_create("beliefFromSensors");
	suite_add_tcase(suite, createBeliefStructureTestCase());
	suite_add_tcase(suite, createSensorBeliefTestCase());
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
