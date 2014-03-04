/*
 * unit_tests.h
 *
 *  Created on: Mar 4, 2014
 *      Author: arichez
 */

#ifndef UNIT_TESTS_H_
#define UNIT_TESTS_H_
#define assert_flt_equals(expected, actual, precision) \
		ck_assert_msg(fabs((float)expected - (float)actual) <= precision, \
		"error - expected: %f; actual: %f", expected, actual)


#endif /* UNIT_TESTS_H_ */
