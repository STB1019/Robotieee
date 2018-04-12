/*
 * test_matrix.cpp
 *
 *  Created on: Feb 16, 2018
 *      Author: koldar
 */


#include "catch.hpp"
#include "matrix.hpp"

using namespace robo_utils;

SCENARIO("matrixes", "") {

	matrix<int> m = matrix<int>{5, 3, 1};

	GIVEN("creation of matrix") {

		REQUIRE(m.rows() == 5);
		REQUIRE(m.columns() == 3);
		for (int y=0; y<5; y++) {
			for (int x=0; x<3; x++) {
				REQUIRE(m(y,x) == 1);
			}
		}

		WHEN("setting a matrix value") {
			REQUIRE(m(1,2) == 1);
			m(1,2) = 2;
			REQUIRE(m(1,2) == 2);
		}

	}

	GIVEN("matrix and points") {

		REQUIRE(m({1,2}) == 1);
		m({1,2}) = 2;
		REQUIRE(m({1,2}) == 2);
	}
}

