import typing

from planner_wrapper import utils
from planner_wrapper.interfaces import IPlanner


class FakePlanner(IPlanner):

    @property
    def name(self):
        return "fake"

    def call_string(self, domain_filename: str, problem_filename: str) -> typing.List[str]:
        self.domain_filename = domain_filename
        return ""

    def check_planner_solution(self, call_result: utils.CallProgram) -> bool:
        return True

    @property
    def output_filename(self):
        tmp = self.domain_filename.lower()
        if "pull" in tmp:
            return "../Problems/Sokoban/example.of.pull.plan"
        elif "push" in tmp:
            return "../Problems/Sokoban/example.of.push.plan"
        else:
            raise ValueError("'{}' is invalid!".format(self.domain_filename))
