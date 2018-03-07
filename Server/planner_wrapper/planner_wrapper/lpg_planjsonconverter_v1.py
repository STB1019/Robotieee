import typing
import os
import re
import json
import sys

from planner_wrapper.interfaces import IPlanJsonConverter
from functools import singledispatch

from planner_wrapper.utils import Point
from planner_wrapper.sokoban_actions import Direction, SokobanPushToGoal, SokobanPushToNonGoal, SokobanMove, \
    IPlannerAction

__doc__ = """
The file is not used right now since it is necessary to Massimo method to convert the model of the plan returned by the
planner into a json
"""


class LPGPlanJsonConverterV1(IPlanJsonConverter):

    def __init__(self):
        self.parsable_classes = [SokobanMove, SokobanPushToGoal, SokobanPushToNonGoal]

    def convert_plan(self, plan_filename: str) -> typing.Dict[typing.AnyStr, typing.Any]:
        j = self._convert_plan_to_json_structure(plan_filename)
        return json.dumps(j, default=to_serializable)

    def _convert_plan_to_json_structure(self, plan_filename: str) -> typing.Dict[typing.AnyStr, typing.Any]:
        plan_filename = os.path.abspath(plan_filename)
        with open(plan_filename, "r") as f:
            plan = f.readlines()

        # trim lines
        plan = map(lambda x: x.strip(' '), plan)
        plan = map(lambda x: x.strip('\t'), plan)
        plan = map(lambda x: x.strip('\n'), plan)
        # filter out empty lines
        plan = filter(lambda x: len(x) > 0, plan)
        # filter out lines starting with ";" a comment
        plan = filter(lambda x: x[0] != ';', plan)
        # the lines starting with a number represents a step of the solution
        plan = filter(lambda x: re.match('^[0-9]+', x), plan)
        # with the lines in plan we now build the actual plan
        actions = list(map(lambda x: IPlannerAction.parse(x, acceptable_classes=self.parsable_classes), plan))

        # now we dump actions within a json

        ret_val = {}
        ret_val['version'] = "1.0"
        ret_val['plan'] = []

        # get method whose name involve class name of the action to conveert to json
        self_module = sys.modules[self.__module__]
        for i, action in enumerate(actions):
            for method_name in filter(lambda x: x.startswith("json_to_"), self_module.__dict__):
                if f"json_to_{action.__class__.__name__}" == method_name:
                    method = self_module.__dict__[method_name]
                    break
            else:
                raise ValueError("no to_json methods available for class {action.__class__.__name__}!")

            ret_val['plan'].append(method(action))

        return ret_val


@singledispatch
def to_serializable(val):
    """Used by default."""
    return str(val)


@to_serializable.register(Point)
def json_to_Point(val: Point):
    return {"x": val.x, "y": val.y}


@to_serializable.register(Direction)
def json_to_Direction(val: Direction):
    return val.name


@to_serializable.register(SokobanMove)
def json_to_SokobanMove(val: SokobanMove):
    return {"move": {
        "player": val.player,
        "from": val.start_pos,
        "to": val.end_pos,
        "direction": val.direction
    }}

@to_serializable.register(SokobanPushToGoal)
def json_to_SokobanPushToGoal(val: SokobanPushToGoal):
    return {"push_goal": {
        "player": val.player,
        "stone_position": val.stone,
        "player_position": val.player_pos,
        "start": val.start_pos,
        "end": val.end_pos,
        "direction": val.direction,
    }}


@to_serializable.register(SokobanPushToNonGoal)
def json_to_SokobanPushToNonGoal(val: SokobanPushToNonGoal):
    return {"push_non_goal": {
        "player": val.player,
        "stone_position": val.stone,
        "player_position": val.player_pos,
        "start": val.start_pos,
        "end": val.end_pos,
        "direction": val.direction,
    }}