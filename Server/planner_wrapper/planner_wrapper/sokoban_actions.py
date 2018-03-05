import enum

import re

from planner_wrapper.exceptions import ActionParseException
from planner_wrapper.parsable_enum import ParsableEnum
from planner_wrapper.point import Point


class Direction(ParsableEnum):
    LEFT = ("DIR-LEFT", 0)
    RIGHT = ("DIR-RIGHT", 1)
    UP = ("DIR-UP", 2)
    DOWN = ("DIR-DOWN", 3)

    def to_json(self):
        return str(self.name)


class ISokobanAction:

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
        x, y = list(map(lambda x: int(x), pos_value.split('-')[1:3])) # consider only the last 2 position in the arrya splitted by -
        return Point(x, y)

    @classmethod
    def parse(cls, string: str):
        m = re.search("^\s*\d+:\s*\((?P<actionname>[^\s]+)\s*(?P<actionparameters>[^\)]+)\)\s*(\[\d+\])?$", string)
        if m is None:
            raise ValueError(f"{string} can't be parsed")
        action_name = m.group('actionname')
        action_parameters = m.group('actionparameters')
        action_classes = [SokobanMove, SokobanPushToGoal, SokobanPushToNonGoal]
        for action_class in action_classes:
            try:
                action_parameters = ISokobanAction._remove_double_spaces(action_parameters)
                ret_val = action_class.parse(action_name, action_parameters)
                return ret_val
            except ActionParseException as e:
                continue
        else:
            raise ValueError(f"Can't convert action {string}!")


class SokobanMove(ISokobanAction):
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
                           start_pos=ISokobanAction.convert_pos_into_cell(action_parameters[1]),
                           end_pos=ISokobanAction.convert_pos_into_cell(action_parameters[2]),
                           direction=Direction.parse(action_parameters[3]),
                           )

    def to_json(self):
        """Used if *val* is an instance of SokobanMove."""
        return {"move": {
            "player": self.player,
            "from": self.start_pos,
            "to": self.end_pos,
            "direction": self.direction
        }
        }


class SokobanPushToNonGoal(ISokobanAction):
    #(PUSH-TO-NONGOAL PLAYER-01 STONE-02 POS-08-05 POS-08-06 POS-08-07 DIR-DOWN) [1]
    #  (:action push-to-nongoal
    #:parameters (?p - player ?s - stone ?ppos ?from ?to - location ?dir - direction)

    def __init__(self, name: str, player: str, stone: str, player_pos: Point, start_pos: Point, end_pos: Point, direction: Direction):
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
                                    player_pos=ISokobanAction.convert_pos_into_cell(action_parameters[2]),
                                    start_pos=ISokobanAction.convert_pos_into_cell(action_parameters[3]),
                                    end_pos=ISokobanAction.convert_pos_into_cell(action_parameters[4]),
                                    direction=Direction.parse(action_parameters[5]),
                                    )

    def to_json(self):
        """Used if val is an instance of SokobanPushToGoal class"""
        return {"push_goal": {
            "player": self.player,
            "stone_position": self.stone,
            "player_position": self.player_pos,
            "start": self.start_pos,
            "end": self.end_pos,
            "direction": self.direction,
        }}


class SokobanPushToGoal(ISokobanAction):
    #(PUSH-TO-GOAL PLAYER-01 STONE-02 POS-08-03 POS-08-04 POS-08-05 DIR-DOWN) [1]
    # (:action push-to-goal
   #:parameters (?p - player ?s - stone ?ppos ?from ?to - location ?dir - direction)

    def __init__(self, name: str, player: str, stone: str, player_pos: Point, start_pos: Point, end_pos: Point, direction: Direction):
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
                                    stone=action_parameters[1],
                                    player_pos=ISokobanAction.convert_pos_into_cell(action_parameters[2]),
                                    start_pos=ISokobanAction.convert_pos_into_cell(action_parameters[3]),
                                    end_pos=ISokobanAction.convert_pos_into_cell(action_parameters[4]),
                                    direction=Direction.parse(action_parameters[5])
                                    )

    def to_json(self):
        """Used if val is an instance of SokobanPushToNonGoal class"""
        return {"push_non_goal": {
            "player": self.player,
            "stone_position": self.stone,
            "player_position": self.player_pos,
            "start": self.start_pos,
            "end": self.end_pos,
            "direction": self.direction,
        }}