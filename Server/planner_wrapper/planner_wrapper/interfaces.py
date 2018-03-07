import typing
import json
from abc import ABCMeta, abstractmethod

from planner_wrapper import program_invoker
from planner_wrapper.sokoban_solution_converter import to_serializable


class IPlanner(metaclass=ABCMeta):

    @property
    @abstractmethod
    def name(self):
        """

        :return: the unique name fo the planner
        """
        raise NotImplementedError()

    @abstractmethod
    def call_string(self, domain_filename: str, problem_filename: str) -> typing.List[str]:
        """
        Execute the planner
        :param domain_filename: the domain file to execute
        :param problem_filename: the problem file to execute
        :return:
        """
        raise NotImplementedError()

    @abstractmethod
    def check_planner_solution(self, call_result: program_invoker.CallProgram) -> bool:
        """

        :param call_result: the return value of the function call_string
        :return: true if the plaanner has return a solution, fale otherwise
        """
        raise NotImplementedError()

    def invoke(self, domain_filename: str, problem_filename: str, working_directory: str=".") -> bool:
        """
        Run the planner. This call is a blocking one!
        :param domain_filename: the pddl domain filename of the problem to solve
        :param problem_filename: the pddl problem filename of the problem to solve
        :param working_directory: where we need to call the planner
        :return: true if a solution has been found, false otherwise
        """
        ret_val = program_invoker.call_program(
            program=self.call_string(domain_filename=domain_filename, problem_filename=problem_filename),
            working_directory=working_directory
        )
        return self.check_planner_solution(ret_val)

    @abstractmethod
    def convert_plan_to_json_structure(self, plan_filename: str) -> typing.Dict:
        raise NotImplementedError()

    def convert_plan_to_json(self, plan_filename: str) -> str:
        j = self.convert_plan_to_json_structure(plan_filename)
        return json.dumps(j, default=to_serializable)