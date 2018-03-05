import os

from planner_wrapper.point import Point

__doc__ = """
Given a sokoban world, creates both a problem filename representing said world 
"""

import typing

from planner_wrapper import sokoban_world


class Clause:

    def __init__(self, f, indent: int =0, name: str =None, colon: bool =False, fake: bool =False, carriage_return: bool =True):
        """

        :param f: the file where to write on
        :param indent: the indentation level of this PDDL clause
        :param name: the name of the clause
        :param colon: true if you want to prefix the name of the clause with a ":"
        :param fake: if true we won't write anything on the file
        :param carriage_return: if true we will append a carriage return after the clause has terminated
        """
        self.f = f
        self.name = name
        self.colon = colon
        self.fake = fake
        self.indent = indent
        self.carriage_return = carriage_return

    def __enter__(self):
        if self.fake:
            return self

        if self.colon:
            self.f.write("{}(:".format('\t'*self.indent))
        else:
            self.f.write("{}(".format('\t'*self.indent))
        if self.name is not None:
            self.f.write(self.name)
            self.f.write(" ")

        return self

    def __exit__(self, t, value, traceback):
        if self.fake:
            return

        self.f.write(")")
        if self.carriage_return:
            self.f.write("\n")

    @staticmethod
    def write_predicate(f, name: str, value: typing.Union[str, typing.Iterable[str]]):
        """
        Write a first order logic predicate in the file
        :param f: the file where to put the predicate into
        :param name: the name of the predicate
        :param value: the values of the rpedicate. It can either be a string (predicate with a single value) or a list of them
                (multi value predicate)
        """
        with Clause(f, indent=0, name=name, carriage_return=False):
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

    def generate_problem(self, problem_filename: str, domain_name: str, problem_name: str, world: sokoban_world.SokobanWorld) -> str:
        with open(problem_filename, "w") as f:
            with Clause(f, indent=0, name="define"):

                with Clause(f, indent=1, name="problem"):
                    f.write(f"{problem_name}")

                with Clause(f, indent=1, name="domain", colon=True):
                    f.write(f"{domain_name}")

                with Clause(f, indent=1, name="objects", colon=True):
                    # write the 4 direction sokoban can move on
                    f.write("dir-down - direction\n")
                    f.write("dir-left - direction\n")
                    f.write("dir-right - direction\n")
                    f.write("dir-up - direction\n")
                    f.write("\n")

                    # player
                    f.write("player-01 - player\n")
                    f.write("\n")

                    # the objects in sokoban are the traversable cells
                    f.write("\n".join(
                        # convert cells left in a string
                        map(
                            # filter out cells in the world which are untraversable
                            lambda p: "{} - location".format(self.cell_predicate(p)),
                            filter(
                                lambda p2: sokoban_world.BaseCellContent.UNTRAVERSABLE not in world[p2],
                                world.cells
                            )
                        )
                    ))
                    f.write("\n")

                    # blocks
                    f.write("\n".join(
                        map(
                            lambda x: f"stone-{x:02d} - stone",
                            range(len(world.blocks))
                        )
                    ))
                    f.write("\n")

                with Clause(f, indent=1, name="init", colon=True):
                    # position of the robot
                    f.write("\n")
                    for cell in world.cells:
                        if not world.is_traversable(*cell):
                            continue
                        f.write(";; CELL: y={} x={}\n".format(cell.y, cell.x))
                        if sokoban_world.BaseCellContent.ROBOT in world[cell]:
                            Clause.write_predicate(f, name="has_player", value=self.cell_predicate(cell))
                            f.write("\n")
                        if sokoban_world.BaseCellContent.BLOCK in world[cell]:
                            Clause.write_predicate(f, name="has_box", value=self.cell_predicate(cell))
                            f.write("\n")
                        if sokoban_world.BaseCellContent.GOAL in world[cell]:
                            Clause.write_predicate(f, name="IS-GOAL", value=self.cell_predicate(cell))
                            f.write("\n")
                        if sokoban_world.BaseCellContent.GOAL not in world[cell]:
                            Clause.write_predicate(f, name="IS-NONGOAL", value=self.cell_predicate(cell))
                            f.write("\n")

                        adjacent_cell_callbacks = [world.get_up, world.get_down, world.get_left, world.get_right]
                        for adjacent_cell_callback in adjacent_cell_callbacks:
                            try:
                                next_cell = adjacent_cell_callback(*cell)
                                if world.is_traversable(*next_cell):
                                    Clause.write_predicate(f,
                                        name="adjacent",
                                        value=[
                                            self.cell_predicate(cell),
                                            self.cell_predicate(next_cell)
                                        ])
                                    f.write("\n")
                            except ValueError as e:
                                # there is no such cell
                                pass

                with Clause(f, indent=1, name="goal", colon=True):
                    with Clause(f, name="and", fake=len(world.goals) == 1):
                        for p in world.goals:
                            Clause.write_predicate(f, name="has_box", value=self.cell_predicate(p))
                            f.write("\n")

        return os.path.abspath(problem_filename)
