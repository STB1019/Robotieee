import unittest
import os
from planner_wrapper import sokoban_world, planner
from planner_wrapper import sokoban_problem_generator
from planner_wrapper import program_invoker


class TestSokobanWorld(unittest.TestCase):

    def test_parse(self):
        world = sokoban_world.SokobanWorld.parse("""
        {
            "version": "1.0",
            "world": {
                "rows": 2,
                "columns": 2,
                "cells": [
                    { "y": 0, "x": 0, "entities": ""},
                    { "y": 0, "x": 1, "entities": ""},
                    { "y": 1, "x": 0, "entities": ""},
                    { "y": 1, "x": 1, "entities": ""}
                ]
            }
        }
        """)

        self.assertTrue(world.rows == 2)
        self.assertTrue(world.cols == 2)
        self.assertTrue(len(world[0, 0]) == 0)
        self.assertTrue(len(world[0, 1]) == 0)
        self.assertTrue(len(world[1, 0]) == 0)
        self.assertTrue(len(world[1, 1]) == 0)
        self.assertTrue(len(world.blocks) == 0)
        self.assertTrue(len(world.goals) == 0)

    def test_parse2(self):
        world = sokoban_world.SokobanWorld.parse("""
        {
            "version": "1.0",
            "world": {
                "rows": 2,
                "columns": 2,
                "cells": [
                    { "y": 0, "x": 0, "entities": "RD"},
                    { "y": 0, "x": 1, "entities": "B"},
                    { "y": 1, "x": 0, "entities": "U"},
                    { "y": 1, "x": 1, "entities": "G"}
                ]
            }
        }
        """)

        self.assertTrue(world.rows == 2)
        self.assertTrue(world.cols == 2)
        self.assertTrue(len(world[0, 0]) == 2)
        self.assertTrue(len(world[0, 1]) == 1)
        self.assertTrue(len(world[1, 0]) == 1)
        self.assertTrue(len(world[1, 1]) == 1)
        self.assertTrue(len(world.blocks) == 1)
        self.assertTrue(len(world.goals) == 1)

        self.assertTrue(sokoban_world.BaseCellContent.ROBOT in world[0, 0])
        self.assertTrue(sokoban_world.BaseCellContent.DOCKING_STATION in world[0, 0])
        self.assertTrue(sokoban_world.BaseCellContent.BLOCK in world[0, 1])
        self.assertTrue(sokoban_world.BaseCellContent.UNTRAVERSABLE in world[1, 0])
        self.assertTrue(sokoban_world.BaseCellContent.GOAL in world[1, 1])

    def test_pddl_converter(self):
        world = sokoban_world.SokobanWorld.parse("""
                {
                    "version": "1.0",
                    "world": {
                        "rows": 2,
                        "columns": 2,
                        "cells": [
                            { "y": 0, "x": 0, "entities": "RD"},
                            { "y": 0, "x": 1, "entities": "B"},
                            { "y": 1, "x": 0, "entities": "U"},
                            { "y": 1, "x": 1, "entities": "G"}
                        ]
                    }
                }
                """)
        sokoban_converter = sokoban_problem_generator.PddlSokobanConverter()
        sokoban_converter.convert_to_pddl(
            problem_filename="tmp",
            domain_name="sokoban",
            problem_name="simple",
            world=world
        )

        self.assertTrue(os.path.exists("tmp"))
        with open("tmp", "r") as f:
            lines = f.readlines()

    def test_planner_invoker(self):
        lpg = planner.LPGPlanner(lpg_location="/home/koldar/Documents/LPGFrancesco/LPG/lpg")

        lpg.use_best_first = True
        lpg.use_lpg = False
        lpg.solutions_to_find = 1
        lpg.output_filename = os.path.join(".", "computed.plan")

        ret = lpg.invoke(
            domain_filename="../Problems/Sokoban/domain.pddl",
            problem_filename="../Problems/Sokoban/instance-1",
            working_directory="."
        )

        self.assertTrue(ret)
        self.assertTrue(os.path.exists("computed.plan"))

        j = lpg.convert_plan_to_json(lpg.output_filename)
        print(j)
        self.assertTrue(j == "ciao")


if __name__ == '__main__':
    unittest.main()
