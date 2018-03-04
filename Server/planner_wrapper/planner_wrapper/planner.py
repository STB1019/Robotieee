from abc import ABCMeta, abstractmethod
import typing
import os
import re
from planner_wrapper import planner_invoker


class ActionParseException(Exception):
    pass


class IAction:

    def __init__(self, name):
        self.name = name

    @staticmethod
    def _remove_double_spaces(string):
        return re.sub("\s+", " ", string)

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


class SokobanMove(IAction):

    def __init__(self, name, player, start_pos, end_pos, direction):
        super().__init__(name)
        self.player = player
        self.start_pos = start_pos
        self.end_pos = end_pos
        self.direction = direction

    @classmethod
    def parse(cls, action_name, action_parameters):
        if action_name != 'MOVE':
            raise ActionParseException()
        action_parameters = action_parameters.split(' ')
        return SokobanMove('MOVE',
            player=action_parameters[0],
            start_pos=action_parameters[1],
            end_pos=action_parameters[2],
            direction=action_parameters[3]
        )

class SokobanPushToNonGoal(IAction):
    #(PUSH-TO-NONGOAL PLAYER-01 STONE-02 POS-08-05 POS-08-06 POS-08-07 DIR-DOWN) [1]
    #  (:action push-to-nongoal 
    #:parameters (?p - player ?s - stone ?ppos ?from ?to - location ?dir - direction)

    def __init__(self, name, player, stone, player_pos, start_pos, end_pos, direction):
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
            stone=action_parameters[1],
            player_pos=action_parameters[2],
            start_pos=action_parameters[3]
            end_pos=action_parameters[4],
            direction=action_parameters[5]
        )
        
class SokobanPushToGoal(IAction):
    #(PUSH-TO-GOAL PLAYER-01 STONE-02 POS-08-03 POS-08-04 POS-08-05 DIR-DOWN) [1]
    # (:action push-to-goal
   #:parameters (?p - player ?s - stone ?ppos ?from ?to - location ?dir - direction)

    def __init__(self, name, player, stone, player_pos, start_pos, end_pos, direction):
        super().__init__(name)
        self.player = player
        self.stone = stone
        self.player_pos = player_pos
        self.start_pos = start_pos
        self.end_pos = end_pos
        self.direction = direction

    @classmethod
    def parse(cls, action_name, action_parameters):
        if action_name != 'PUSH-TO-GOAL':
            raise ActionParseException()
        action_parameters = action_parameters.split(' ')
        return SokobanPushToNonGoal('PUSH-TO-GOAL',
            player=action_parameters[0],
            stone=action_parameters[1],
            player_pos=action_parameters[2],
            start_pos=action_parameters[3]
            end_pos=action_parameters[4],
            direction=action_parameters[5]
        )

#TODO need to explain this section below
class IPlanner(metaclass=ABCMeta):

    @property
    @abstractmethod
    def name(self):
        raise NotImplementedError()

    @abstractmethod
    def call_string(self, domain_filename: str, problem_filename: str) -> typing.List[str]:
        raise NotImplementedError()

    @abstractmethod
    def check_planner_solution(self, call_result: planner_invoker.CallProgram) -> bool:
        raise NotImplementedError()

    def invoke(self, domain_filename: str, problem_filename: str, working_directory: str=".") -> bool:
        ret_val = planner_invoker.call_program(
            program=self.call_string(domain_filename=domain_filename,
                                    problem_filename=problem_filename),
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

        ret_val = {}
        ret_val['version'] = "1.0"
        ret_val['plan'] = []

