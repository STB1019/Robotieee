import os

from planner_wrapper.domains.sokoban.sokoban_actions import Direction
from planner_wrapper.interfaces import ISokobanWorldToPddlProblemConverter
from planner_wrapper.utils import Point, Clause

__doc__ = """
Given a sokoban world, creates both a problem filename representing said world 
"""

from planner_wrapper.domains.sokoban import sokoban_world
from planner_wrapper.utils import TabFileWriter


class ExplorationWorldToPddlConverter(IWorldToPddlProblemConverter):

    def cell_predicate(self, p: Point) -> str:
        return f"cell-{p.y:02d}-{p.x:02d}"

    def get_direction(self, p_start: Point, p_end: Point) -> Direction:
        if p_start.x - p_end.x == 0:
            #moving along y
            if p_start.y < p_end.y:
                return Direction.DOWN
            elif p_start.y > p_end.y:
                return Direction.UP
            else:
                raise ValueError(f"invalid point {p_start} and next point {p_end}")
        elif p_start.y - p_end.y == 0:
            #moving along x
            if p_start.x < p_end.x:
                return Direction.RIGHT
            elif p_start.x > p_end.x:
                return Direction.LEFT
            else:
                raise ValueError(f"invalid point {p_start} and next point {p_end}")
        else:
            raise ValueError(f"invalid point {p_start} and next point {p_end}")

    def generate_problem(self, problem_filename: str, domain_name: str, problem_name: str, world: sokoban_world.SokobanWorld) -> str:
        with open(problem_filename, "w") as f:
            tf = TabFileWriter(f, start_indent=0, element_per_indent=2, tabs_instead_of_spaces=False)
            with Clause(tf, name="define"):

                with Clause(tf, name="problem", carriage_return=True):
                    tf.write(f"{problem_name}")

                with Clause(tf, name="domain", colon=True, carriage_return=True):
                    tf.write(f"{domain_name}")

                with Clause(tf, name="objects", colon=True, carriage_return=True):
                    # write the 4 direction sokoban can move on
                    tf.writeln()
                    tf.writeln("{} - direction".format(Direction.LEFT.parsable_str))
                    tf.writeln("{} - direction".format(Direction.RIGHT.parsable_str))
                    tf.writeln("{} - direction".format(Direction.UP.parsable_str))
                    tf.writeln("{} - direction".format(Direction.DOWN.parsable_str))
                    tf.writeln()

                    # player
                    tf.writeln("player-01 - player")
                    tf.writeln()

                    # the objects in sokoban are the traversable cells
                    locations = map(
                        # filter out cells in the world which are untraversable
                        lambda p: "{} - location".format(self.cell_predicate(p)),
                        filter(
                            lambda p2: sokoban_world.BaseCellContent.UNTRAVERSABLE not in world[p2],
                            world.cells
                        )
                    )
                    for location in locations:
                        tf.writeln(location)
                    tf.writeln()

                    # there are no stones. Locations occupied by stones are not "clear"

                with Clause(tf, name="init", colon=True):
                    # position of the robot
                    tf.writeln()
                    for cell in world.cells:
                        if not world.is_traversable(row=cell.y, col=cell.x):
                            continue
                        tf.writeln(";; *****************************************")
                        tf.writeln(";; CELL: y={} x={}".format(cell.y, cell.x))
                        tf.writeln(";; *****************************************")

                        # cell contains a robot
                        # we need to mark such cell as "visited"
                        if sokoban_world.BaseCellContent.ROBOT in world[cell]:
                            Clause.write_predicate(tf, name="at", value=[
                                "player-01",
                                self.cell_predicate(cell)
                            ])
                            tf.writeln()
                            Clause.write_predicate(tf, name="visited", value=self.cell_predicate(cell))
                            tf.writeln()

                        # cell contains a block
                        # cell which do not contain a block are clear. Otherwise they are not
                        if sokoban_world.BaseCellContent.BLOCK not in world[cell]:
                            Clause.write_predicate(tf, name="clear", value=self.cell_predicate(cell))
                            tf.writeln()
                        else:
                            # perform the check but do not do anything
                            blocks = list(filter(lambda x: x.position == cell, world.blocks))
                            if len(blocks) != 1:
                                raise ValueError(f"problem while looking for block in cell {cell.y}:{cell.x}. It should be only one but instead it's: {blocks}")

                        # cell contains a goal
                        # goal are ignored in the exploration domain

                        #cell does not contain a goal
                        # non goals are ignored in the exploration domain

                        #cell contains both a block and a goal
                        #nothing should be doen in this scenario

                        #cell contains nothing
                        if world.is_empty(row=cell.y, col=cell.x):
                            Clause.write_predicate(tf, name="clear", value=self.cell_predicate(cell))
                        tf.writeln()

                        #adjacent cells
                        adjacent_cell_callbacks = [world.get_up, world.get_down, world.get_left, world.get_right]
                        for adjacent_cell_callback in adjacent_cell_callbacks:
                            try:
                                next_cell = adjacent_cell_callback(row=cell.y, col=cell.x)
                                if world.is_traversable(row=next_cell.y, col=next_cell.x):
                                    Clause.write_predicate(tf,
                                        name="MOVE-DIR",
                                        value=[
                                            self.cell_predicate(cell),
                                            self.cell_predicate(next_cell),
                                            self.get_direction(cell, next_cell).parsable_str,
                                        ])
                                    tf.writeln()
                            except ValueError as e:
                                # there is no such cell
                                pass

                with Clause(tf, name="goal", colon=True):
                    with Clause(tf, name="and", fake=False):
                        # player needs to visit every cell

                        for cell in world.cells:
                            if not world.is_traversable(row=cell.y, col=cell.x):
                                # if a cell is not traversable, we can't visit it
                                continue
                            if sokoban_world.BaseCellContent.BLOCK in world[cell]:
                                # if a cell contains a block, we can't visited it!
                                continue
                            Clause.write_predicate(tf, name="visited", value=self.cell_predicate(cell))
                            tf.writeln()

        return os.path.abspath(problem_filename)
