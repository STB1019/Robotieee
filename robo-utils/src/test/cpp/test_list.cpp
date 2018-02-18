/*
 * list.cpp
 *
 *  Created on: Feb 13, 2018
 *      Author: koldar
 */

#include "catch.hpp"
#include "list.hpp"

using namespace robo_utils;

#include <iostream>

SCENARIO("lists", "") {

	list<int>* l = new list<int>{0, false};

	REQUIRE(l->is_empty());
	REQUIRE(l->get_size() == 0);

	GIVEN("adding on list") {

		WHEN("adding on head") {
			l->add_to_head(3);

			THEN("list is incremented") {
				REQUIRE(!l->is_empty());
				REQUIRE(l->get_size() == 1);
				REQUIRE(l->get(0) == 3);
			}
		}

		WHEN("adding on tail") {
			l->add_to_tail(3);

			THEN("list is incremented") {
				REQUIRE(!l->is_empty());
				REQUIRE(l->get_size() == 1);
				REQUIRE(l->get(0) == 3);
			}
		}

		WHEN("adding 2 items") {
			l->add_to_tail(2);
			l->add_to_head(1);

			THEN("list is incremented") {
				REQUIRE(!l->is_empty());
				REQUIRE(l->get_size() == 2);
				REQUIRE(l->get_head() == 1);
				REQUIRE(l->get_tail() == 2);
				REQUIRE(l->get(0) == 1);
				REQUIRE(l->get(1) == 2);
			}

			THEN("testing const iteration") {
				int sum = 0;
				for(auto it=l->cbegin(); it != l->cend(); ++it) {
					sum += *it;
				}
				REQUIRE(sum == 3);
			}
		}

		WHEN("adding several elements on tail") {
			l->add_to_tail(1);
			l->add_to_tail(2);
			l->add_to_tail(3);
			l->add_to_tail(4);
			l->add_to_tail(5);

			THEN("everything is fine") {
				REQUIRE(l->get_size() == 5);

				REQUIRE(l->get_head() == 1);
				REQUIRE(l->get_tail() == 5);

				REQUIRE(l->get(0) == 1);
				REQUIRE(l->get(1) == 2);
				REQUIRE(l->get(2) == 3);
				REQUIRE(l->get(3) == 4);
				REQUIRE(l->get(4) == 5);
			}
		}

	}

	GIVEN("removing from list of 5 elements") {

		l->add_to_tail(1);
		l->add_to_tail(2);
		l->add_to_tail(3);
		l->add_to_tail(4);
		l->add_to_tail(5);

		WHEN("removing head") {

			for (auto it=l->begin(); it!=l->end(); ++it) {
				if (*it == 1) {
					l->remove_element(it);
					break;
				}
			}

			THEN("eveyrthing is fine") {
				REQUIRE(l->get_size() == 4);
				REQUIRE(l->get_head() == 2);
				REQUIRE(l->get_tail() == 5);

				int sum = 0;
				for(auto it=l->cbegin(); it != l->cend(); ++it) {
					sum += *it;
				}
				REQUIRE(sum == 14);
			}
		}

		WHEN("removing tail") {

			for (auto it=l->begin(); it!=l->end(); ++it) {
				if (*it == 5) {
					l->remove_element(it);
					break;
				}
			}

			THEN("eveyrthing is fine") {
				REQUIRE(l->get_size() == 4);
				REQUIRE(l->get_head() == 1);
				REQUIRE(l->get_tail() == 4);

				int sum = 0;
				for(auto it=l->cbegin(); it != l->cend(); ++it) {
					sum += *it;
				}
				REQUIRE(sum == 10);
			}
		}

		WHEN("removing middle") {

			for (auto it=l->begin(); it!=l->end(); ++it) {
				if (*it == 2) {
					l->remove_element(it);
					break;
				}
			}

			THEN("eveyrthing is fine") {
				REQUIRE(l->get_size() == 4);
				REQUIRE(l->get_head() == 1);
				REQUIRE(l->get_tail() == 5);

				int sum = 0;
				for(auto it=l->cbegin(); it != l->cend(); ++it) {
					sum += *it;
				}
				REQUIRE(sum == 13);
			}
		}

		WHEN("accessing list via the bracket operators") {
			//define reference in order to avoid calling the destructor when this scope terminates
			list<int>& stack_l = *l;
			REQUIRE(stack_l[0] == 1);
			REQUIRE(stack_l[1] == 2);
			REQUIRE(stack_l[2] == 3);
			REQUIRE(stack_l[3] == 4);
			REQUIRE(stack_l[4] == 5);
		}

		WHEN("setting list via bracket operators") {
			list<int>& stack_l = *l;

			REQUIRE(stack_l[1] == 2);
			stack_l[1] = 3;
			REQUIRE(stack_l[1] == 3);
		}

	}

	GIVEN("removing elements from an single element list") {
		l->add_to_head(5);

		WHEN("removing nothing") {

			for (auto it=l->begin(); it!=l->end(); ++it) {
				if (*it == 2) {
					l->remove_element(it);
					break;
				}
			}

			THEN("nothing happens") {
				int sum = 0;
				for(auto it=l->cbegin(); it != l->cend(); ++it) {
					sum += *it;
				}
				REQUIRE(sum == 5);
			}
		}

		WHEN("removing an element") {

			for (auto it=l->begin(); it!=l->end(); ++it) {
				if (*it == 5) {
					l->remove_element(it);
					break;
				}
			}

			THEN("list is empty") {
				int sum = 0;
				for(auto it=l->cbegin(); it != l->cend(); ++it) {
					sum += *it;
				}
				REQUIRE(sum == 0);
				REQUIRE(l->is_empty());
				REQUIRE(l->get_head() == 0); //default value
				REQUIRE(l->get_tail() == 0); //default value
			}
		}


	}


	delete l;
}


