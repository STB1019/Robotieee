import typing
import json
import sys

from functools import singledispatch

from planner_wrapper.interfaces import IPlanToJsonConverter, IPlannerAction
from planner_wrapper.domains.sokoban.sokoban_actions import SokobanPushToNonGoal, SokobanPushToGoal, SokobanMove, \
    Direction
from planner_wrapper.utils import Point


class LPG_V1_ExplorationPlanToJsonConverter(IPlanToJsonConverter):

    def __init__(self):
        self.parsable_classes = [SokobanMove]

    def convert_plan(self, actions: typing.List[IPlannerAction]) -> typing.Dict[typing.AnyStr, typing.Any]:
        # now we dump actions within a json
        ret_val = {}
        ret_val['version'] = "1.0"
        ret_val['actions'] = []

        # get method whose name involve class name of the action to conveert to json
        self_module = sys.modules[self.__module__]
        for i, action in enumerate(actions):
            for method_name in filter(lambda x: x.startswith("json_to_"), self_module.__dict__):
                if f"json_to_{action.__class__.__name__}" == method_name:
                    method = self_module.__dict__[method_name]
                    break
            else:
                raise ValueError(f"no to_json methods available for class {action.__class__.__name__}!")

            ret_val['actions'].append(method(action))

        return json.dumps(ret_val, default=to_serializable)


@singledispatch
def to_serializable(val):
    """Used by default."""
    return str(val)


@to_serializable.register(Point)
def json_to_Point(val: Point):
    return {"x": val.y, "y": val.x}


@to_serializable.register(Direction)
def json_to_Direction(val: Direction):
    d = {
        Direction.LEFT: "dir-left",
        Direction.RIGHT: "dir-right",
        Direction.UP: "dir-up",
        Direction.DOWN: "dir-down",
    }
    return d[val]


@to_serializable.register(SokobanMove)
def json_to_SokobanMove(val: SokobanMove):
    return {
        "action": "move",
        "player": val.player.lower(),
        "from": val.start_pos,
        "to": val.end_pos,
        "direction": val.direction
    }
