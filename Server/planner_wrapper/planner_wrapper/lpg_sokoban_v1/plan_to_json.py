import typing
import json
import sys

from functools import singledispatch

from planner_wrapper.interfaces import IPlanToJsonConverter, IPlannerAction
from planner_wrapper.domains.sokoban.sokoban_actions import SokobanPushToNonGoal, SokobanPushToGoal, SokobanMove, \
    Direction
from planner_wrapper.utils import Point


class LPG_V1_PlanToJsonConverter(IPlanToJsonConverter):

    def __init__(self):
        self.parsable_classes = [SokobanMove, SokobanPushToGoal, SokobanPushToNonGoal]

    def convert_plan(self, actions: typing.List[IPlannerAction]) -> typing.Dict[typing.AnyStr, typing.Any]:
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
                raise ValueError(f"no to_json methods available for class {action.__class__.__name__}!")

            ret_val['plan'].append(method(action))

        return json.dumps(ret_val, default=to_serializable)


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
