__doc__ = """
The file is not used right now since it is necessary to Massimo method to convert the model of the plan returned by the
planner into a json
"""

from functools import singledispatch

from planner_wrapper.point import Point
from planner_wrapper.sokoban_actions import Direction, SokobanPushToGoal, SokobanPushToNonGoal, SokobanMove


@singledispatch
def to_serializable(val):
    """Used by default."""
    return str(val)


@to_serializable.register(Point)
def ts_point(val: Point):
    """Used if *val* is an instance of datetime."""
    return val.to_json()


@to_serializable.register(Direction)
def ts_point(val: Direction):
    """Used if *val* is an instance of datetime."""
    return str(val.to_json())


@to_serializable.register(SokobanMove)
def ts_point(val: SokobanMove):
    """Used if *val* is an instance of datetime."""
    return str(val.to_json())


@to_serializable.register(SokobanPushToGoal)
def ts_point(val: SokobanPushToGoal):
    """Used if *val* is an instance of datetime."""
    return str(val.to_json())


@to_serializable.register(SokobanPushToNonGoal)
def ts_point(val: SokobanPushToNonGoal):
    """Used if *val* is an instance of datetime."""
    return str(val.to_json())