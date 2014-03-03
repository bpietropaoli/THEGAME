/*
 * checkBeliefFromSensors.c
 *
 *  Created on: Mar 3, 2014
 *      Author: arichez
 */

#include <stdlib.h>
#include <check.h>

#include "BeliefsFromSensors.h"


BFS_BeliefStructure beliefStructurePresence;

static void setup(void) {
	beliefStructurePresence = BFS_loadBeliefStructure("presence");
}

static void teardown(void) {
	BFS_freeBeliefStructure(&beliefStructurePresence);
}

/*
 * Tests on belief structure
 * =========================
 *
 * The belief structure is about presence and includes 3 sensors (CO2, motion,
 * sound). Its value can be yes or no.
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


#define  checkOptionFlags(structure, sensorName, opts) \
	ck_assert_msg(getSensorBelief(structure, sensorName)->optionFlags == opts, \
			"wrong option flags for %s", sensorName);

#define assert_flt_equals(expected, actual, precision) \
		ck_assert_msg(fabs((float)expected - (float)actual) <= precision, \
		"error - expected: %f; actual: %f", expected, actual)


START_TEST(beliefStructureNameIsOk) {
	ck_assert_str_eq("presence", beliefStructurePresence.frameName);
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
	ck_assert_int_eq(2, refList.card);
	ck_assert_str_eq("yes", refList.values[0]);
	ck_assert_str_eq("no", refList.values[1]);
}
END_TEST

START_TEST(powersetCardsAreOk) {
	/*
	 * powerset are stored in the order : {{void}, {yes}, {no}, {yes u no}}
	 */
	Sets_Set powerSet = beliefStructurePresence.powerset;
	ck_assert_int_eq(4, powerSet.card);
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

START_TEST(soundSensorOptionIsOk) {
	/* In the test configuration, the sound sensor has no options */
	checkOptionFlags(beliefStructurePresence, "sound", OP_NONE);
}
END_TEST

START_TEST(soundSensorNbFocalsIsOk) {
	ck_assert_int_eq(2 ,getSensorBelief(beliefStructurePresence, "sound")->nbFocal);
}
END_TEST

START_TEST(soundSensorYesValuesAreOk) {
	/*
	 * Focal: {yes}
	 * Points:
	 *  - (0.000000, 0.000000)
	 *  - (10.000000, 0.200000)
	 *  - (30.000000, 0.800000)
	 *  - (60.000000, 1.000000)
	 */
	BFS_SensorBeliefs *belief = getSensorBelief(beliefStructurePresence, "sound");

	ck_assert_int_eq(4, belief->beliefOnElements[0].nbPts);

	assert_flt_equals(0, belief->beliefOnElements[0].points[0].sensorValue, 0.0);
	assert_flt_equals(0, belief->beliefOnElements[0].points[0].belief, 0);

	assert_flt_equals(10.0f, belief->beliefOnElements[0].points[1].sensorValue, 0);
	assert_flt_equals(0.2f, belief->beliefOnElements[0].points[1].belief, 0);

	assert_flt_equals(30.0f, belief->beliefOnElements[0].points[2].sensorValue, 0);
	assert_flt_equals(0.8f, belief->beliefOnElements[0].points[2].belief, 0);

	assert_flt_equals(60.0f, belief->beliefOnElements[0].points[3].sensorValue, 0);
	assert_flt_equals(1.0f, belief->beliefOnElements[0].points[3].belief, 0);

}
END_TEST

START_TEST(soundSensorYesUNoValuesAreOk) {
	/*
	 * Focal: {yes u no}
	 * Points:
	 *  - (0.000000, 1.000000)
	 *  - (10.000000, 0.800000)
	 *  - (30.000000, 0.200000)
	 *  - (60.000000, 0.000000)
	 */
	BFS_SensorBeliefs *belief = getSensorBelief(beliefStructurePresence, "sound");

	ck_assert_int_eq(4, belief->beliefOnElements[1].nbPts);

	assert_flt_equals(0, belief->beliefOnElements[1].points[0].sensorValue, 0);
	assert_flt_equals(1.0f, belief->beliefOnElements[1].points[0].belief, 0);

	assert_flt_equals(10.0f, belief->beliefOnElements[1].points[1].sensorValue, 0);
	assert_flt_equals(0.8f, belief->beliefOnElements[1].points[1].belief, 0);

	assert_flt_equals(30.0f, belief->beliefOnElements[1].points[2].sensorValue, 0);
	assert_flt_equals(0.2f, belief->beliefOnElements[1].points[2].belief, 0);

	assert_flt_equals(60.0f, belief->beliefOnElements[1].points[3].sensorValue, 0);
	assert_flt_equals(0.0f, belief->beliefOnElements[1].points[3].belief, 0);

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
	beliefStructurePresence = BFS_loadBeliefStructure("presence");
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
	tcase_add_test(testCaseParsing, soundSensorOptionIsOk);
	tcase_add_test(testCaseParsing, soundSensorNbFocalsIsOk);
	tcase_add_test(testCaseParsing, soundSensorYesValuesAreOk);
	tcase_add_test(testCaseParsing, soundSensorYesUNoValuesAreOk);
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
