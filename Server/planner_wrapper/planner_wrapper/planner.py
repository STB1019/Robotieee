import enum
from abc import ABCMeta, abstractmethod
import typing
import os
import re
import json
from functools import singledispatch

from planner_wrapper import planner_invoker
from planner_wrapper.point import Point


class ActionParseException(Exception):
    pass


class IAction:

    def __init__(self, name):
        self.name = name

    @staticmethod
    def _remove_double_spaces(string):
        return re.sub("\s+", " ", string)

    @staticmethod
    def convert_pos_into_cell(pos_value: str) -> Point:
        """
        Concvert stringa s like "POS-05-08" into points like <5,8>
        :param pos_value: the string to convert
        :return: the point converted
        """
        return Point(*list(map(lambda x: int(x), pos_value.split('-')[1:2])))

    @classmethod
    def parse(cls, string: str):
        m = re.search("^\s*\d+:\s*\((?P<actionname>[^\s]+)\s*(?P<actionparameters>[^\)]+)\)\s*(\[\d+\])?$", string)
        if m is None:
            raise ValueError(f"{string} can't be parsed")
        action_name = m.group('action')
        action_parameters = m.group('actionparameters')
        action_classes = [SokobanMove]
        for action_class in action_classes:
            try:
                action_parameters = IAction._remove_double_spaces(action_parameters)
                ret_val = action_class.parse(action_name, action_parameters)
                return ret_val
            except ActionParseException as e:
                continue
        else:
            raise ValueError(f"Can't convert action!")


class Direction(enum.Enum):
    LEFT = 0
    RIGHT = 1
    UP = 2
    DOWN = 3

    @classmethod
    def parse(string: str):
        m = {
            "DIR-LEFT": Direction.LEFT,
            "DIR-RIGHT": Direction.RIGHT,
            "DIR-UP": Direction.UP,
            "DIR-DOWN": Direction.DOWN
        }
        if string not in m:
            raise ValueError(f"Can't convert {string} in Direction enum!")
        return m[string]


class SokobanMove(IAction):
    """
    0:   (MOVE PLAYER-01 POS-05-08 POS-04-08 DIR-LEFT) [1]
    """

    def __init__(self, name: str, player: str, start_pos: Point, end_pos: Point, direction: Direction):
        super().__init__(name)
        self.player = player
        self.start_pos = start_pos
        self.end_pos = end_pos
        self.direction = direction

    @classmethod
    def parse(cls, action_name: str, action_parameters: str):
        if action_name != 'MOVE':
            raise ActionParseException()
        action_parameters = action_parameters.split(' ')
        return SokobanMove('MOVE',
            player=action_parameters[0],
            start_pos=IAction.convert_pos_into_cell(action_parameters[1]),
            end_pos=IAction.convert_pos_into_cell(action_parameters[2]),
            direction=Direction.parse(action_parameters[3]),
        )


class SokobanPushToNonGoal(IAction):
    #(PUSH-TO-NONGOAL PLAYER-01 STONE-02 POS-08-05 POS-08-06 POS-08-07 DIR-DOWN) [1]
    #  (:action push-to-nongoal 
    #:parameters (?p - player ?s - stone ?ppos ?from ?to - location ?dir - direction)

    def __init__(self, name: str, player: str, stone: Point, player_pos: Point, start_pos: Point, end_pos: Point, direction: Direction):
        super().__init__(name)
        self.player = player
        self.stone = stone
        self.player_pos = player_pos
        self.start_pos = start_pos
        self.end_pos = end_pos
        self.direction = direction

    @classmethod
    def parse(cls, action_name, action_parameters):
        if action_name != 'PUSH-TO-NONGOAL':
            raise ActionParseException()
        action_parameters = action_parameters.split(' ')
        return SokobanPushToNonGoal('PUSH-TO-NONGOAL',
            player=action_parameters[0],
            stone=IAction.convert_pos_into_cell(action_parameters[1]),
            player_pos=IAction.convert_pos_into_cell(action_parameters[2]),
            start_pos=IAction.convert_pos_into_cell(action_parameters[3]),
            end_pos=IAction.convert_pos_into_cell(action_parameters[4]),
            direction=Direction.parse(action_parameters[5]),
        )


class SokobanPushToGoal(IAction):
    #(PUSH-TO-GOAL PLAYER-01 STONE-02 POS-08-03 POS-08-04 POS-08-05 DIR-DOWN) [1]
    # (:action push-to-goal
   #:parameters (?p - player ?s - stone ?ppos ?from ?to - location ?dir - direction)

    def __init__(self, name: str, player: str, stone: Point, player_pos: Point, start_pos: Point, end_pos: Point, direction: Direction):
        super().__init__(name)
        self.player = player
        self.stone = stone
        self.player_pos = player_pos
        self.start_pos = start_pos
        self.end_pos = end_pos
        self.direction = direction

    @classmethod
    def parse(cls, action_name: str, action_parameters: str):
        if action_name != 'PUSH-TO-GOAL':
            raise ActionParseException()
        action_parameters = action_parameters.split(' ')
        return SokobanPushToNonGoal('PUSH-TO-GOAL',
            player=action_parameters[0],
            stone=IAction.convert_pos_into_cell(action_parameters[1]),
            player_pos=IAction.convert_pos_into_cell(action_parameters[2]),
            start_pos=IAction.convert_pos_into_cell(action_parameters[3]),
            end_pos=IAction.convert_pos_into_cell(action_parameters[4]),
            direction=Direction.parse(action_parameters[5])
        )


@singledispatch
def to_serializable(val):
    """Used by default."""
    return str(val)


@to_serializable.register(Point)
def ts_point(val: Point):
    return {"x": val.x, "y": val.y}


@to_serializable.register(Direction)
def ts_direction(val: Direction):
    return str(val.name)


@to_serializable.register(SokobanMove)
def ts_sokoban_move(val: SokobanMove):
    """Used if *val* is an instance of SokobanMove."""
    return {"move": {
        "player": val.player,
        "from": val.start_pos,
        "to": val.end_pos,
        "direction": val.direction
        }
    }


@to_serializable.register(SokobanPushToGoal)
def ts_sokoban_push_to_goal(val: SokobanPushToGoal):
    """Used if val is an instance of SokobanPushToGoal class"""
    return {"push_goal": {
        "player": val.player,
        "stone_position": val.stone,
        "player_position": val.player_pos,
        "start": val.start_pos,
        "end": val.end_pos,
        "direction": val.direction,
    }}


@to_serializable.register(SokobanPushToNonGoal)
def ts_sokoban_push_to_non_goal(val: SokobanPushToNonGoal):
    """Used if val is an instance of SokobanPushToNonGoal class"""
    return {"push_non_goal": {
        "player": val.player,
        "stone_position": val.stone,
        "player_position": val.player_pos,
        "start": val.start_pos,
        "end": val.end_pos,
        "direction": val.direction,
    }}


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
    def check_planner_solution(self, call_result: planner_invoker.CallProgram) -> bool:
        """

        :param call_result: the return value of the function call_string
        :return: true if the plaanner has return a solution, fale otherwise
        """
        raise NotImplementedError()

    def invoke(self, domain_filename: str, problem_filename: str, working_directory: str=".") -> bool:
        ret_val = planner_invoker.call_program(
            program=self.call_string(domain_filename=domain_filename, problem_filename=problem_filename),
            working_directory=working_directory
        )
        return self.check_planner_solution(ret_val)

    @abstractmethod
    def convert_plan_to_json(self, plan_filename: str):
        raise NotImplementedError()


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

    def check_planner_solution(self, call_result: planner_invoker.CallProgram) -> bool:
        if call_result.stdout.find("Solution number") > 0:
            return True
        else:
            return False

    def convert_plan_to_json(self, plan_filename: str):
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
        actions = list(map(lambda x: IAction.parse(x), plan))
        #now we dump actions within a json

        ret_val = {}
        ret_val['version'] = "1.0"
        ret_val['plan'] = {}

        return ret_val
