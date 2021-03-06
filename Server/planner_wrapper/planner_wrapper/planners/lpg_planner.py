import typing
import os

from planner_wrapper import utils
from planner_wrapper.interfaces import IPlanner


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

    def check_planner_solution(self, call_result: utils.CallProgram) -> bool:
        if call_result.stdout.find("Solution number") > 0:
            return True
        else:
            return False
