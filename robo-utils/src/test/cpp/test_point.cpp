/*
 * test_point.cpp
 *
 *  Created on: Feb 18, 2018
 *      Author: koldar
 */

#include "catch.hpp"
#include "point.hpp"

using namespace robo_utils;

SCENARIO("test point") {

	GIVEN("a point p ") {
		point p{5,3};

		WHEN("another point q") {
			point q{2,4};

			THEN("p+q is correct") {
				REQUIRE(p + q == point{7,7});
			}

			THEN("p-q is correct") {
				REQUIRE(p - q == point{3,-1});
			}

			THEN("p+=q is correct") {
				p += q;
				REQUIRE(p == point{7,7});
			}

			THEN("p-=q is correct") {
				p -= q;
				REQUIRE(p == point{3,-1});
			}
		}

		WHEN("another value") {
			int v = 2;

			THEN("operations are correct") {
				REQUIRE(p + v == point{7,5});
				REQUIRE(p - v == point{3,1});
				p += v;
				REQUIRE(p == point{7,5});
				p -= v;
				REQUIRE(p == point{5,3});
			}

		}

		WHEN("checking negative") {

			THEN("everything is correct") {
				REQUIRE(+p == point{5,3});
				REQUIRE(-p == point{-5, -3});
				REQUIRE(&(+p) != &p );
				REQUIRE(&(-p) != &p );
			}
		}



	}
}


