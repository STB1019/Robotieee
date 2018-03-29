from planner_wrapper.domains.sokoban.sokoban_world import SokobanWorld, BaseCellContent
from planner_wrapper.utils import Point


class ExplorationWorld(SokobanWorld):
    """
    Represents a model which can represents the world of exploration problem
    """

    def is_visited(self, row, col):
        return BaseCellContent.VISITED in self[Point(y=row, x=col)]