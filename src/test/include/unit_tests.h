/*
 * unit_tests.h
 *
 *  Created on: Mar 4, 2014
 *      Author: arichez
 */

#ifndef UNIT_TESTS_H_
#define UNIT_TESTS_H_

#include <math.h>

#define assert_flt_equals(expected, actual, precision) \
		ck_assert_msg(fabs((float)expected - (float)actual) <= precision, \
		"error - expected: %f; actual: %f", expected, actual)
/*
 * Data for the test. We use the structure defined in
 * data/beliefFromSensors/unittest
 */

#if GCC_VERSION_AT_LEAST(2,95)
#define ATTRIBUTE_UNUSED __attribute__ ((unused))
#else
#define ATTRIBUTE_UNUSED
#endif /* GCC 2.95 */

#define SENSOR_NB 2
#define ATOM_NB 3
#define BELIEF_DEFINITION_PATH "./data/beliefsFromSensors/"


#define SET_ELEMENT_DECLARE(name, val1, val2, val3) \
	static char name ##  _VAL[] = {val1, val2, val3}; \
	static Sets_Element name ATTRIBUTE_UNUSED = {name ##  _VAL, val1 + val2 + val3}

SET_ELEMENT_DECLARE(A, 1, 0, 0);
SET_ELEMENT_DECLARE(B, 0, 1, 0);
SET_ELEMENT_DECLARE(C, 0, 0, 1);
SET_ELEMENT_DECLARE(BuC, 0, 1, 1);
SET_ELEMENT_DECLARE(AuC, 1, 0, 1);
SET_ELEMENT_DECLARE(AuB, 1, 1, 0);
SET_ELEMENT_DECLARE(AuBuC, 1, 1, 1);
SET_ELEMENT_DECLARE(VOID, 0, 0, 0);

#endif /* UNIT_TESTS_H_ */
