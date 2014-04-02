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


#include <check.h>


#include "XmlBeliefFromSensors.h"
#include "BeliefsFromSensors.h"

#define  checkOptionFlags(structure, sensorName, opts) \
	ck_assert_msg(getSensorBelief(structure, sensorName)->optionFlags == opts, \
			"wrong option flags for %s, expected : %s (%d), was %d", sensorName, #opts, opts, \
			getSensorBelief(structure, sensorName)->optionFlags);


static BFS_SensorBeliefs *getSensorBelief(BFS_BeliefStructure from,char *name) {
	int i;
	for(i = 0; i < from.nbSensors; i++) {
		if(0 == strcmp(from.beliefs[i].sensorType, name)) {
			return &(from.beliefs[i]);
		}
	}
	return NULL;
}

BFS_BeliefStructure beliefStructure1;

static void setup() {
	beliefStructure1 = BFS_loadBeliefStructureFromXml("data/belief-model-1.xml");
}

static void teardown() {
	//BFS_freeBeliefStructure(&beliefStructure);
}

START_TEST(FrameNameIsOk1) {
	ck_assert_str_eq("unittest", beliefStructure1.frameName);
}
END_TEST

START_TEST(beliefStructureSensorNbIsOk) {
	ck_assert_int_eq(4, beliefStructure1.nbSensors);
}
END_TEST

START_TEST(beliefStructureValuesNbIsOk) {
	ck_assert_int_eq(3, beliefStructure1.refList.card);
}
END_TEST

START_TEST(beliefStructureNamesAreOk) {
	/* The reference list size should be 3 with "A", "B" and "C" as values.
	 */
	Sets_ReferenceList refList = beliefStructure1.refList;
	ck_assert_str_eq("A", refList.values[0]);
	ck_assert_str_eq("B", refList.values[1]);
	ck_assert_str_eq("C", refList.values[2]);
}
END_TEST

START_TEST(sensorOptionIsOk1) {
	checkOptionFlags(beliefStructure1, "S1", OP_NONE);

}
END_TEST

START_TEST(sensorOptionIsOk2) {
	checkOptionFlags(beliefStructure1, "S3", OP_TEMPO_SPECIFICITY);

}
END_TEST

START_TEST(sensorOptionIsOk3) {
	checkOptionFlags(beliefStructure1, "S4", OP_TEMPO_FUSION);

}
END_TEST

START_TEST(sensorNbFocalsIsOk) {
	ck_assert_int_eq(4 ,getSensorBelief(beliefStructure1, "S1")->nbFocal);
}
END_TEST

static TCase* createTestcase() {
	TCase *testCase = tcase_create("Loading");
	tcase_add_checked_fixture(testCase, setup, teardown);
	tcase_add_test(testCase, FrameNameIsOk1);
	tcase_add_test(testCase, beliefStructureSensorNbIsOk);
	tcase_add_test(testCase, beliefStructureValuesNbIsOk);
	tcase_add_test(testCase, beliefStructureNamesAreOk);
	tcase_add_test(testCase, sensorOptionIsOk1);
	tcase_add_test(testCase, sensorOptionIsOk2);
	tcase_add_test(testCase, sensorOptionIsOk3);
	tcase_add_test(testCase, sensorNbFocalsIsOk);
	return testCase;
}

Suite *createSuite(void) {
	Suite *suite = suite_create("xmlBeliefLoading");
	suite_add_tcase(suite, createTestcase());
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
