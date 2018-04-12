/*
 * test_string.cpp
 *
 *  Created on: Apr 12, 2018
 *      Author: koldar
 */

#include "catch.hpp"
#include "string.hpp"

using namespace robo_utils;

#include <string.h>

SCENARIO("string", "") {

	GIVEN("create string") {
		string<15> s = string<15>{"hello"};

		WHEN("checking static string") {

			REQUIRE(s.getSize() == 5);
			REQUIRE(s.getCapacity() == 15);
			REQUIRE(!s.isEmpty());
		}

		WHEN("creating empty string") {
			string<15> s2 = string<15>{};

			REQUIRE(s2.getSize() == 0);
			REQUIRE(s2.getCapacity() == 15);
			REQUIRE(s2.isEmpty());
		}

		WHEN("appending data") {
			bool val = s.append(" world");

			REQUIRE(val);

			THEN("") {
				printf("1) %s\n", s.getBuffer());
				REQUIRE(strcmp(s.getBuffer(), "hello world") == 0);
				REQUIRE(s.getSize() == 11);
				REQUIRE(s.getCapacity() == 15);
			}

		}

		WHEN("fill whole data") {
			bool val = s.append("1234567890");

			REQUIRE(val);

			THEN("") {
				printf("2) %s\n", s.getBuffer());
				REQUIRE(strcmp(s.getBuffer(), "hello1234567890") == 0);
				REQUIRE(s.getSize() == 15);
				REQUIRE(s.getCapacity() == 15);
			}
		}

		WHEN("overflowing buffer") {
			bool val = s.append("12345678901");

			REQUIRE(!val);

			THEN("") {
				REQUIRE(strcmp(s.getBuffer(), "hello") == 0);
				REQUIRE(s.getSize() == 5);
				REQUIRE(s.getCapacity() == 15);
			}
		}
	}

}
