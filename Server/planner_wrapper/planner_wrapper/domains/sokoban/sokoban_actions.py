from planner_wrapper.interfaces import IPlannerAction
from planner_wrapper.utils import Point, ParsableEnum


class Direction(ParsableEnum):
    LEFT = ("DIR-LEFT", 0)
    RIGHT = ("DIR-RIGHT", 1)
    UP = ("DIR-UP", 2)
    DOWN = ("DIR-DOWN", 3)


class SokobanMove(IPlannerAction):
    """
    0:   (MOVE PLAYER-01 POS-05-08 POS-04-08 DIR-LEFT) [1]
    """

    def __init__(self, name: str, player: str, start_pos: Point, end_pos: Point, direction: Direction):
        super().__init__(name)
        self.player = player
        self.start_pos = start_pos
        self.end_pos = end_pos
        self.direction = direction

class SokobanPushToNonGoal(IPlannerAction):
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


class SokobanPushToGoal(IPlannerAction):
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