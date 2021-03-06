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
 * checkBeliefFromSensors.c
 *
 *  Created on: Mar 3, 2014
 *      Author: arichez
 */

#include <stdlib.h>
#include <check.h>
#include <unistd.h>

#include "BeliefsFromSensors.h"
#include "unit_tests.h"


BFS_BeliefStructure beliefStructure;

static void setup(void) {
	beliefStructure = BFS_loadBeliefStructure(BELIEF_DEFINITION_PATH, STRUCTURE_NAME);
}

static void teardown(void) {
	BFS_freeBeliefStructure(&beliefStructure);
}



/*
 * Tests on belief structure
 * =========================
 *
 * The belief structure is the structure in the folder unittest.
 */

static BFS_SensorBeliefs *getSensorBelief(BFS_BeliefStructure from,char *name) {
	int i;
	for(i = 0; i < from.nbSensors; i++) {
		if(0 == strcmp(from.beliefs[i].sensorType, name)) {
			return &(from.beliefs[i]);
		}
	}
	return NULL;
}

/*
 * get a part of belief for a given element.
 */
static BFS_PartOfBelief *getPartOfBelief(BFS_SensorBeliefs *sensorBelief, Sets_Element element) {
	int i;
	for (i = 0; i < sensorBelief->nbFocal; ++i) {
		if(Sets_equals(sensorBelief->beliefOnElements[i].focalElement,element, ATOM_NB)) {
			return &(sensorBelief->beliefOnElements[i]);
		}
	}
	return NULL;
}

/*
 * return the value for a given element in the belief function.
 */
static float valueFor(BF_BeliefFunction function, Sets_Element element) {
	int i = 0;
	for(i = 0; i < function.nbFocals; i++) {
		if(Sets_equals(element, function.focals[i].element, function.elementSize)) {
			return function.focals[i].beliefValue;
		}
	}
	return -1.0;
}

#define  checkOptionFlags(structure, sensorName, opts) \
	ck_assert_msg(getSensorBelief(structure, sensorName)->optionFlags == opts, \
			"wrong option flags for %s", sensorName);




START_TEST(beliefStructureNameIsOk) {
	ck_assert_str_eq(STRUCTURE_NAME, beliefStructure.frameName);
}
END_TEST

START_TEST(beliefStructureSensorNbIsOk) {
	ck_assert_int_eq(4, beliefStructure.nbSensors);
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

START_TEST(powersetValuesAreOk) {
	/* Values are boolean tables [yes, no]. */
	Sets_Set powerSet = beliefStructure.powerset;

	/* for void, the table should be [0,0] */
	ck_assert_int_eq(0, powerSet.elements[0].values[0]);
	ck_assert_int_eq(0, powerSet.elements[0].values[1]);
	/* for yes, the table should be [1,0] */
	ck_assert_int_eq(1, powerSet.elements[1].values[0]);
	ck_assert_int_eq(0, powerSet.elements[1].values[1]);
	/* for no, the table should be [0,1] */
	ck_assert_int_eq(0, powerSet.elements[2].values[0]);
	ck_assert_int_eq(1, powerSet.elements[2].values[1]);
	/* for yes u no, the table should be [1,1] */
	ck_assert_int_eq(1, powerSet.elements[3].values[0]);
	ck_assert_int_eq(1, powerSet.elements[3].values[1]);
}
END_TEST

START_TEST(sensorOptionIsOk) {
	/* In the test configuration, the sound sensor has no options */
	checkOptionFlags(beliefStructure, "S1", OP_NONE);
	checkOptionFlags(beliefStructure, "S3", OP_TEMPO_SPECIFICITY);

}
END_TEST

START_TEST(sensorNbFocalsIsOk) {
	ck_assert_int_eq(4 ,getSensorBelief(beliefStructure, "S1")->nbFocal);
}
END_TEST

START_TEST(sensorValuesAreOk1) {
	/*
	 * Focal: {A}
	 * Points:
	 * - (100, 0)
	 * - (200, 0.25)
	 * - (300, 0.75)
	 * - (400, 0.25)
	 * - (500, 0)
	 */
	BFS_SensorBeliefs *belief = getSensorBelief(beliefStructure, "S1");
	BFS_PartOfBelief *focalBelief = getPartOfBelief(belief, A);

	ck_assert_int_eq(5, focalBelief->nbPts);

	assert_flt_equals(100.0f, focalBelief->points[0].sensorValue, 0.0);
	assert_flt_equals(0, focalBelief->points[0].belief, 0);

	assert_flt_equals(200.0f, focalBelief->points[1].sensorValue, 0);
	assert_flt_equals(0.25f, focalBelief->points[1].belief, 0);

	assert_flt_equals(300.0f, focalBelief->points[2].sensorValue, 0);
	assert_flt_equals(0.75f, focalBelief->points[2].belief, 0);

	assert_flt_equals(400.0f, focalBelief->points[3].sensorValue, 0);
	assert_flt_equals(0.25f, focalBelief->points[3].belief, 0);

	assert_flt_equals(500.0f, focalBelief->points[4].sensorValue, 0);
	assert_flt_equals(0.0f, focalBelief->points[4].belief, 0);

}
END_TEST

START_TEST(sensorValuesAreOk2) {
	/*
	 * Focal: {A,B}
	 * Points:
	 *  - (100.0, 0.0)
	 *  - (200.0, 0.25)
	 *  - (300.0, 0.75)
	 *  - (400.0, 0.25)
	 *  - (500.0, 0.0)
	 */
	BFS_SensorBeliefs *belief = getSensorBelief(beliefStructure, "S1");
	BFS_PartOfBelief *focalBelief = getPartOfBelief(belief, AuB);

	ck_assert_int_eq(4, focalBelief->nbPts);

	assert_flt_equals(100.0f, focalBelief->points[0].sensorValue, 0);
	assert_flt_equals(0.0f, focalBelief->points[0].belief, 0);

	assert_flt_equals(200.0f, focalBelief->points[1].sensorValue, 0);
	assert_flt_equals(0.15f, focalBelief->points[1].belief, 0);

	assert_flt_equals(400.0f, focalBelief->points[2].sensorValue, 0);
	assert_flt_equals(0.15f, focalBelief->points[2].belief, 0);

	assert_flt_equals(500.0f, focalBelief->points[3].sensorValue, 0);
	assert_flt_equals(0.0f, focalBelief->points[3].belief, 0);

}
END_TEST


/*
 * Test for beliefValue
 * ===================
 */


BFS_SensorBeliefs *belief;
BF_FocalElement focalElementMinus20, focalElement150, focalElement300, focalElement550;
BF_BeliefFunction beliefFunction150;
Sets_Element yesElement;

static void setupProjection(void) {
	beliefStructure = BFS_loadBeliefStructure(BELIEF_DEFINITION_PATH, STRUCTURE_NAME);
	belief = getSensorBelief(beliefStructure, "S1");

	BFS_PartOfBelief *beliefForA = getPartOfBelief(belief, A);

	focalElementMinus20 = BFS_getBeliefValue(*beliefForA, -20.0,
	    		beliefStructure.refList.card);
	focalElement150 = BFS_getBeliefValue(*beliefForA, 150.0,
	    		beliefStructure.refList.card);
	focalElement300 = BFS_getBeliefValue(*beliefForA, 300.0,
	    		beliefStructure.refList.card);
	focalElement550 = BFS_getBeliefValue(*beliefForA, 550.0,
    		beliefStructure.refList.card);
	beliefFunction150 = BFS_getProjection(*belief, 150.0,
    		beliefStructure.refList.card);
	BF_cleanBeliefFunction(&beliefFunction150);
}

static void teardownProjection(void) {
	BFS_freeBeliefStructure(&beliefStructure);
    BF_freeBeliefPoint(&focalElementMinus20);
    BF_freeBeliefPoint(&focalElement150);
    BF_freeBeliefPoint(&focalElement300);
    BF_freeBeliefPoint(&focalElement550);
    BF_freeBeliefFunction(&beliefFunction150);

}

START_TEST(beliefValueForA) {

    assert_flt_equals(0.0, focalElementMinus20.beliefValue, 0);
    assert_flt_equals(0.125, focalElement150.beliefValue, 0);
    assert_flt_equals(0.75, focalElement300.beliefValue, 0);
    assert_flt_equals(0.0, focalElement550.beliefValue, 0);
}
END_TEST

START_TEST(ProjectionFocalNb) {
    ck_assert_int_eq(4, beliefFunction150.nbFocals);
}
END_TEST


START_TEST(ProjectionFocalValues) {
	assert_flt_equals(0.125, valueFor(beliefFunction150, A), 0);
	assert_flt_equals(0.625, valueFor(beliefFunction150, B), 0);
	assert_flt_equals(0.175, valueFor(beliefFunction150, C), 0);
	assert_flt_equals(0.075, valueFor(beliefFunction150, AuB), 0);
}
END_TEST

START_TEST(testTempoSpecificity) {
	BFS_SensorBeliefs *beliefS3 = getSensorBelief(beliefStructure,"S3");
	BF_BeliefFunction function = BFS_getProjection(*beliefS3, 100, ATOM_NB);
	BF_freeBeliefFunction(&function);
	function = BFS_getProjectionElapsedTime(*beliefS3, NO_MEASURE, ATOM_NB, 0.5);
	assert_flt_equals(0.5, valueFor(function, A), BF_PRECISION);

}
END_TEST

START_TEST(testTempoFusion) {
	BFS_SensorBeliefs *beliefS4 = getSensorBelief(beliefStructure,"S4");
	BF_BeliefFunction function = BFS_getProjection(*beliefS4, 100, ATOM_NB);
	BF_freeBeliefFunction(&function);
	function = BFS_getProjectionElapsedTime(*beliefS4, 100, ATOM_NB, 0.5);
	assert_flt_equals(0.625, valueFor(function, A), BF_PRECISION);

}
END_TEST

static TCase* createParsingTestcase() {
	TCase* testCaseParsing = tcase_create("Parsing");
	tcase_add_checked_fixture(testCaseParsing, setup, teardown);
	tcase_add_test(testCaseParsing, beliefStructureNameIsOk);
	tcase_add_test(testCaseParsing, beliefStructureSensorNbIsOk);
	tcase_add_test(testCaseParsing, beliefStructureValuesAreOk);
	tcase_add_test(testCaseParsing, powersetCardsAreOk);
	tcase_add_test(testCaseParsing, powersetValuesAreOk);
	tcase_add_test(testCaseParsing, sensorOptionIsOk);
	tcase_add_test(testCaseParsing, sensorNbFocalsIsOk);
	tcase_add_test(testCaseParsing, sensorValuesAreOk1);
	tcase_add_test(testCaseParsing, sensorValuesAreOk2);
	return testCaseParsing;
}

static TCase* createBeliefProjectionsTestCase() {
	TCase* testCaseProjections = tcase_create("Projections");
	tcase_add_checked_fixture(testCaseProjections, setupProjection, teardownProjection);
	tcase_add_test(testCaseProjections, beliefValueForA);
	tcase_add_test(testCaseProjections, ProjectionFocalNb);
	tcase_add_test(testCaseProjections, ProjectionFocalValues);
	tcase_add_test(testCaseProjections, testTempoSpecificity);
	tcase_add_test(testCaseProjections, testTempoFusion);

	return testCaseProjections;

}

Suite *createSuite(void) {
	Suite *suite = suite_create("beliefFromSensors");
	suite_add_tcase(suite, createParsingTestcase());
	suite_add_tcase(suite, createBeliefProjectionsTestCase());
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
