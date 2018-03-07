import typing
import json
from abc import ABCMeta, abstractmethod

from planner_wrapper import utils, sokoban_world


class IPddlSokobanConverter(metaclass=ABCMeta):

    @abstractmethod
    def generate_problem(self, problem_filename: str, domain_name: str, problem_name: str,
                         world: sokoban_world.SokobanWorld) -> str:
        """
        Build a new problem file representing the state in then sokoban world given
        :param problem_filename: the name of the problem file to generate
        :param domain_name: the name of the domain file to use
        :param problem_name: the name of the problem. An unique string representing the given problem
        :param world: the world we must inspect to generate the actual pddl problem file
        :return: the absolute path of problem_filename
        """
        raise NotImplementedError()


class IPlanJsonConverter(metaclass=ABCMeta):

    @abstractmethod
    def convert_plan(self, plan_filename: str) -> typing.Dict[typing.AnyStr, typing.Any]:
        raise NotImplementedError()


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
    def check_planner_solution(self, call_result: utils.CallProgram) -> bool:
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
        ret_val = utils.call_program(
            program=self.call_string(domain_filename=domain_filename, problem_filename=problem_filename),
            working_directory=working_directory
        )
        return self.check_planner_solution(ret_val)