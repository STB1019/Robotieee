import typing
from abc import ABCMeta, abstractmethod

import re

from planner_wrapper import utils
from planner_wrapper.domains.sokoban import sokoban_world


class IPlannerAction:

    def __init__(self, name):
        self.name = name


class ISokobanWorldToPddlProblemConverter(metaclass=ABCMeta):
    """
    Represents a class allowing you to generate a PDDL problem instance for a sokoban world
    """

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


class IPlanFilenameToPlanConverter(metaclass=ABCMeta):

    @abstractmethod
    def convert_plan_filename_into_plan(self, plan_filename: str) -> typing.List[IPlannerAction]:
        raise NotImplementedError()

    def fetch_action_information(cls, string: str) -> typing.Tuple[str, typing.List[str]]:
        """
        Process the action move in order to fetch action information from the output plan
        :param string: the string present inside the output plan the planner has created
        :return: a tuple. The first item is the name of the action while the second element is the list of string parameters of the action (if any)
        """
        m = re.search("^\s*\d+:\s*\((?P<actionname>[^\s]+)\s*(?P<actionparameters>[^\)]+)\)\s*(\[\d+\])?$", string)
        if m is None:
            raise ValueError(f"'{string}' can't be correctly parsed")

        action_name = m.group('actionname')
        action_parameters = m.group('actionparameters')

        action_parameters = re.sub('\s+', ' ', action_parameters)
        action_parameters = action_parameters.split(' ')
        return action_name, action_parameters


class IPlanToJsonConverter(metaclass=ABCMeta):

    @abstractmethod
    def convert_plan(self, actions: typing.List[IPlannerAction]) -> typing.Dict[typing.AnyStr, typing.Any]:
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

    @property
    @abstractmethod
    def output_filename(self):
        """
        :return: the filename of the solution computed by the planner
        """
        raise NotImplementedError()


class IJsonToWorld(metaclass=ABCMeta):

    @abstractmethod
    def convert_json_to_sokoban_world(self, j: typing.Dict[str, typing.Any]) -> sokoban_world.SokobanWorld:
        pass


class IPlannerFactory(metaclass=ABCMeta):

    @property
    @abstractmethod
    def domain_filename(self) -> str:
        """
        :return: the filename of the domain the planner will use to solve the PDDL problem
        """
        raise NotImplementedError()

    @abstractmethod
    def json_to_world(self) -> IJsonToWorld:
        """

        :return: a structure allows to convert the json received into a world representation
        """
        pass

    @abstractmethod
    def sokoban_world_to_pddl_problem(self) -> ISokobanWorldToPddlProblemConverter:
        """

        :return: a structure converting a wordl representation into a valid PDDL problem file
        """
        pass

    @property
    @abstractmethod
    def planner(self) -> IPlanner:
        """

        :return: the planner to use to solve tbe problem
        """
        pass

    @abstractmethod
    def plan_filename_to_plan(self) -> IPlanFilenameToPlanConverter:
        """

        :return: a structure allowing you to conveert the plan output by a planner into an in-memory  representation of the plan
        """
        pass

    @abstractmethod
    def plan_to_json(self) -> IPlanToJsonConverter:
        """

        :return: a structure allowing yout to convert the in-memory representation of the plan into a json
        """
        pass