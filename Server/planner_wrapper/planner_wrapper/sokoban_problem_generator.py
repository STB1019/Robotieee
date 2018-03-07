import os

from planner_wrapper.point import Point
from planner_wrapper.sokoban_actions import Direction

__doc__ = """
Given a sokoban world, creates both a problem filename representing said world 
"""

import typing

from planner_wrapper import sokoban_world


class TabFile:

    def __init__(self, f, start_indent: int = 0, element_per_indent: int = 1,tabs_instead_of_spaces: bool = True):
        self.f = f
        self.indent = start_indent
        self.tabs_instead_of_spaces = tabs_instead_of_spaces
        self.new_line = True

    def __iadd__(self, other: int):
        self.indent += 1
        return self

    def __isub__(self, other: int):
        self.indent -= 1
        return self

    def write(self, string: str = ""):
        if '\n' not in string:
            if self.new_line:
                #generate indentation
                if self.tabs_instead_of_spaces:
                    self.f.write('\t' * self.indent)
                else:
                    self.f.write(' ' * self.indent)
            #write real line
            self.f.write(string)
            self.new_line = False
        else:
            lines = string.split('\n')
            for line in lines:
                self.writeln(line)

    def writeln(self, string: str = ""):
        self.write(string)
        self.f.write('\n')
        self.new_line = True


class Clause:

    def __init__(self, f: TabFile, parent_clause=None, name: str =None, colon: bool =False, fake: bool =False, carriage_return: bool =True):
        """

        :param f: the file where to write on
        :param parent_clause: the clause containing this new cluase of PDDL
        :param name: the name of the clause
        :param colon: true if you want to prefix the name of the clause with a ":"
        :param fake: if true we won't write anything on the file
        :param carriage_return: if true we will append a carriage return after the clause has terminated
        """
        self.tf = f
        self.name = name
        self.colon = colon
        self.fake = fake
        self.parent_clause = parent_clause
        self.carriage_return = carriage_return

    def __enter__(self):
        self.tf += 1
        if self.fake:
            return self

        if self.colon:
            self.tf.write("(:")
        else:
            self.tf.write("(")

        if self.name is not None:
            self.tf.write(self.name)
            self.tf.write(" ")

        return self

    def __exit__(self, t, value, traceback):
        self.tf -= 1
        if self.fake:
            return

        if self.carriage_return:
            self.tf.writeln(")")
        else:
            self.tf.write(")")

    @staticmethod
    def write_predicate(f, name: str, value: typing.Union[str, typing.Iterable[str]]):
        """
        Write a first order logic predicate in the file
        :param f: the file where to put the predicate into
        :param name: the name of the predicate
        :param value: the values of the rpedicate. It can either be a string (predicate with a single value) or a list of them
                (multi value predicate)
        """
        with Clause(f, name=name, carriage_return=False):
            if type(value) == str:
                f.write(value)
            else:
                f.write(" ".join(value))


class IPddlSokobanConverter:

    def generate_problem(self, problem_filename: str, domain_name: str, problem_name: str,
                         world: sokoban_world.SokobanWorld) -> str:
        """
        Build a new problem file representing the state in then sokoban world given
        :param problem_filename: the name of the problem file to generate
        :param domain_name: the name of the domain file to use
        :param problem_name: the name of the problem. An unique string representing the given problem
        :param world: the world we must inspect to generate the actual pddl problem file
        :return: the absolute path of problem_filename
        """
        raise NotImplementedError()


class PddlSokobanConverterVersion1(IPddlSokobanConverter):

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
            tf = TabFile(f, start_indent=0, element_per_indent=2, tabs_instead_of_spaces=False)
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

                    # blocks
                    blocks = map(
                        lambda x: f"stone-{x:02d} - stone",
                        range(len(world.blocks))
                    )
                    for block in blocks:
                        tf.writeln(block)
                    tf.writeln()

                with Clause(tf, name="init", colon=True):
                    # position of the robot
                    tf.writeln()
                    for cell in world.cells:
                        if not world.is_traversable(row=cell.y, col=cell.x):
                            continue
                        tf.writeln(";; *****************************************")
                        tf.writeln(";; CELL: y={} x={}\n".format(cell.y, cell.x))
                        tf.writeln(";; *****************************************")

                        #cell contains a robot
                        if sokoban_world.BaseCellContent.ROBOT in world[cell]:
                            Clause.write_predicate(tf, name="at", value=[
                                "player-01",
                                self.cell_predicate(cell)
                            ])
                            tf.writeln()

                        #cell contains a block
                        if sokoban_world.BaseCellContent.BLOCK in world[cell]:
                            blocks = list(filter(lambda x: x.position == cell, world.blocks))
                            if len(blocks) != 1:
                                raise ValueError(f"problem while looking for block in cell {cell.y}:{cell.x}. It should be only one but instead it's: {blocks}")
                            block = blocks[0]
                            Clause.write_predicate(tf, name="at", value=[
                                block.to_pddl_predicate(),
                                self.cell_predicate(cell)
                            ])
                            tf.writeln()

                        #cell contains a goal
                        if sokoban_world.BaseCellContent.GOAL in world[cell]:
                            Clause.write_predicate(tf, name="IS-GOAL", value=self.cell_predicate(cell))
                            tf.writeln()

                        #cell does not contain a goal
                        if sokoban_world.BaseCellContent.GOAL not in world[cell]:
                            Clause.write_predicate(tf, name="IS-NONGOAL", value=self.cell_predicate(cell))
                            tf.writeln()

                        #cell contains both a block and a goal
                        if sokoban_world.BaseCellContent.GOAL in world[cell] and sokoban_world.BaseCellContent.BLOCK in world[cell]:
                            blocks = list(filter(lambda x: x.position == cell, world.blocks))
                            if len(blocks) != 1:
                                raise ValueError(
                                    f"problem while looking for block in cell {cell.y}:{cell.x}. It should be only one but instead it's: {blocks}")
                            block = blocks[0]
                            Clause.write_predicate(tf, name="at-goal", value=[
                                block.to_pddl_predicate()
                            ])
                            tf.writeln()

                        #cell contains nothing
                        if world.is_empty(row=cell.y, col=cell.x):
                            Clause.write_predicate(tf, name="clear", value=[self.cell_predicate(cell)])
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
                    with Clause(tf, name="and", fake=len(world.goals) == 1):
                        for b in world.blocks:
                            Clause.write_predicate(tf, name="at-goal", value=b.to_pddl_predicate())
                            tf.write("\n")

        return os.path.abspath(problem_filename)
