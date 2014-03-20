/*
 * checkBeliefFromSensors.c
 *
 *  Created on: Mar 3, 2014
 *      Author: arichez
 */

#include <stdlib.h>
#include <check.h>

#include "BeliefsFromSensors.h"
#include "unit_tests.h"

#define STRUCTURE_NAME "unittest"

BFS_BeliefStructure beliefStructurePresence;

static void setup(void) {
	beliefStructurePresence = BFS_loadBeliefStructure(BELIEF_DEFINITION_PATH, STRUCTURE_NAME);
}

static void teardown(void) {
	BFS_freeBeliefStructure(&beliefStructurePresence);
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

static BFS_PartOfBelief *getPartOfBelief(BFS_SensorBeliefs *sensorBelief, Sets_Element element) {
	int i;
	for (i = 0; i < sensorBelief->nbFocal; ++i) {
		if(Sets_equals(sensorBelief->beliefOnElements[i].focalElement,element, ATOM_NB)) {
			return &(sensorBelief->beliefOnElements[i]);
		}
	}
	return NULL;
}


#define  checkOptionFlags(structure, sensorName, opts) \
	ck_assert_msg(getSensorBelief(structure, sensorName)->optionFlags == opts, \
			"wrong option flags for %s", sensorName);




START_TEST(beliefStructureNameIsOk) {
	ck_assert_str_eq(STRUCTURE_NAME, beliefStructurePresence.frameName);
}
END_TEST

START_TEST(beliefStructureSensorNbIsOk) {
	ck_assert_int_eq(3, beliefStructurePresence.nbSensors);
}
END_TEST

START_TEST(beliefStructureValuesAreOk) {
	/* The reference list size should be 2 with "yes" and "no" as values.
	 */
	Sets_ReferenceList refList = beliefStructurePresence.refList;
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
	Sets_Set powerSet = beliefStructurePresence.powerset;
	ck_assert_int_eq(8, powerSet.card);
	ck_assert_int_eq(0, powerSet.elements[0].card);
	ck_assert_int_eq(1, powerSet.elements[1].card);
	ck_assert_int_eq(1, powerSet.elements[2].card);
	ck_assert_int_eq(2, powerSet.elements[3].card);
}
END_TEST

START_TEST(powersetValuesAreOk) {
	/* Values are boolean tables [yes, no]. */
	Sets_Set powerSet = beliefStructurePresence.powerset;

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
	checkOptionFlags(beliefStructurePresence, "S1", OP_NONE);
	checkOptionFlags(beliefStructurePresence, "S3", OP_TEMPO_FUSION);

}
END_TEST

START_TEST(sensorNbFocalsIsOk) {
	ck_assert_int_eq(4 ,getSensorBelief(beliefStructurePresence, "S1")->nbFocal);
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
	BFS_SensorBeliefs *belief = getSensorBelief(beliefStructurePresence, "S1");
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
	BFS_SensorBeliefs *belief = getSensorBelief(beliefStructurePresence, "S1");
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


BFS_SensorBeliefs *soundBelief;
BF_FocalElement focalElementMinus20, focalElement5, focalElement60, focalElement63;
BF_BeliefFunction beliefFunction5;

static void setupProjection(void) {
	beliefStructurePresence = BFS_loadBeliefStructure(BELIEF_DEFINITION_PATH, "presence");
	soundBelief = getSensorBelief(beliefStructurePresence, "sound");
	focalElementMinus20 = BFS_getBeliefValue(soundBelief->beliefOnElements[0], -20.0,
	    		beliefStructurePresence.refList.card);
	focalElement5 = BFS_getBeliefValue(soundBelief->beliefOnElements[0], 5.0,
	    		beliefStructurePresence.refList.card);
	focalElement60 = BFS_getBeliefValue(soundBelief->beliefOnElements[0], 60.0,
	    		beliefStructurePresence.refList.card);
	focalElement63 = BFS_getBeliefValue(soundBelief->beliefOnElements[0], 63.0,
    		beliefStructurePresence.refList.card);
	beliefFunction5 = BFS_getProjection(*soundBelief, 5.0,
    		beliefStructurePresence.refList.card);
}

static void teardownProjection(void) {
	BFS_freeBeliefStructure(&beliefStructurePresence);
    BF_freeBeliefPoint(&focalElementMinus20);
    BF_freeBeliefPoint(&focalElement5);
    BF_freeBeliefPoint(&focalElement60);
    BF_freeBeliefPoint(&focalElement63);
    BF_freeBeliefFunction(&beliefFunction5);

}

START_TEST(beliefValueForSound) {

    assert_flt_equals(0.0, focalElementMinus20.beliefValue, 0);
    assert_flt_equals(0.1, focalElement5.beliefValue, 0);
    assert_flt_equals(1.0, focalElement60.beliefValue, 0);
    assert_flt_equals(1.0, focalElement63.beliefValue, 0);
}
END_TEST

START_TEST(ProjectionForSoundFocalNb) {
    ck_assert_int_eq(2, beliefFunction5.nbFocals);
}
END_TEST


START_TEST(ProjectionForSoundFocalValues) {
	assert_flt_equals(0.1, beliefFunction5.focals[0].beliefValue, 0);
	assert_flt_equals(0.9, beliefFunction5.focals[1].beliefValue, 0);
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
	tcase_add_test(testCaseProjections, beliefValueForSound);
	tcase_add_test(testCaseProjections, ProjectionForSoundFocalNb);
	tcase_add_test(testCaseProjections, ProjectionForSoundFocalValues);

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
