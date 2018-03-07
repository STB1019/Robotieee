import enum
import json
from abc import ABCMeta, abstractmethod
import typing
import os
import re
from functools import singledispatch

from planner_wrapper import program_invoker
from planner_wrapper.interfaces import IPlanner
from planner_wrapper.sokoban_actions import ISokobanAction


class LPGPlanner(IPlanner):

    def __init__(self, lpg_location=None):
        self._lpg_location = lpg_location
        self._solutions_to_find = 1
        self._use_lpg = True
        self._use_best_first = True
        self._restarts = 9
        self._repeats = 5
        self._output_filename = "computed_plan.plan"

    @property
    def output_filename(self):
        return self._output_filename

    @output_filename.setter
    def output_filename(self, value):
        self._output_filename = os.path.abspath(value)

    @property
    def solutions_to_find(self):
        return self._solutions_to_find

    @solutions_to_find.setter
    def solutions_to_find(self, value):
        self._solutions_to_find = value

    @property
    def use_lpg(self):
        return self._use_lpg

    @use_lpg.setter
    def use_lpg(self, value):
        self._use_lpg = value

    @property
    def use_best_first(self):
        return self._use_best_first

    @use_best_first.setter
    def use_best_first(self, value):
        self._use_best_first = value

    @property
    def restarts(self):
        return self._restarts

    @restarts.setter
    def restarts(self, value):
        self._restarts = value

    @property
    def repeats(self):
        return self._repeats

    @repeats.setter
    def repeats(self, value):
        self._repeats = value

    def name(self):
        return "LPG"

    def call_string(self, domain_filename: str, problem_filename: str) -> typing.List[str]:
        if self._lpg_location is None:
            planner_executable = "lpg"
        else:
            planner_executable = os.path.abspath(self._lpg_location)

        domain_filename = os.path.abspath(domain_filename)
        problem_filename = os.path.abspath(problem_filename)

        #we can't put the double quotes on -f or -o parameters
        ret_val = [
            planner_executable,
            "-n", '{}'.format(str(self.solutions_to_find)),
            "-f", '{}'.format(problem_filename),
            "-o", '{}'.format(domain_filename),
            "-out", '{}'.format(self.output_filename)
        ]

        if self.use_lpg and self.use_best_first:
            return ret_val
        elif self.use_lpg and not self.use_best_first:
            ret_val.append("-nobestfirst")
        elif not self.use_lpg and self.use_best_first:
            ret_val.append("-onlybestfirst")
        else:
            raise ValueError("both best first and lpg have been disabled!")

        return ret_val

    def check_planner_solution(self, call_result: program_invoker.CallProgram) -> bool:
        if call_result.stdout.find("Solution number") > 0:
            return True
        else:
            return False

    def convert_plan_to_json_structure(self, plan_filename: str) -> typing.Dict:
        plan_filename = os.path.abspath(plan_filename)
        with open(plan_filename, "r") as f:
            plan = f.readlines()

        #trim lines
        plan = map(lambda x: x.strip(' '), plan)
        plan = map(lambda x: x.strip('\t'), plan)
        plan = map(lambda x: x.strip('\n'), plan)
        #filter out empty lines
        plan = filter(lambda x: len(x) > 0, plan)
        #filter out lines starting with ";" a comment
        plan = filter(lambda x: x[0] != ';', plan)
        #the lines starting with a number represents a step of the solution
        plan = filter(lambda x: re.match('^[0-9]+', x), plan)
        #with the lines in plan we now build the actual plan
        actions = list(map(lambda x: ISokobanAction.parse(x), plan))
        #now we dump actions within a json

        ret_val = {}
        ret_val['version'] = "1.0"
        ret_val['plan'] = []

        for i, action in enumerate(actions):
            ret_val['plan'].append(action.to_json())

        return ret_val
