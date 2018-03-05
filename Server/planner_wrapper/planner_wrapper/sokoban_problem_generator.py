__doc__ = """
Given a sokoban world, creates both a problem filename representing said world 
"""

import typing

from planner_wrapper import sokoban_world


class Clause:

    def __init__(self, f, indent=0, name=None, colon=False, fake=False, carriage_return=True):
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


class PddlSokobanConverter:

    def __init__(self):
        pass

    @staticmethod
    def _write_predicate(f, name: str, value: typing.Union[str, typing.Iterable[str]]):
        with Clause(f, indent=0, name=name, carriage_return=False):
            if type(value) == str:
                f.write(value)
            else:
                f.write(" ".join(value))

    def convert_to_pddl(self, problem_filename: str, domain_name: str, problem_name: str, world: sokoban_world.SokobanWorld):
        with open(problem_filename, "w") as f:
            with Clause(f, indent=0, name="define"):

                with Clause(f, indent=1, name="problem"):
                    f.write(f"{problem_name}")

                with Clause(f, indent=1, name="domain", colon=True):
                    f.write(f"{domain_name}")

                with Clause(f, indent=1, name="objects", colon=True):
                    # write the 4 direction sokoban can move on
                    f.write("dir-down - direction\n")
                    f.write("dir-left - direction\nn")
                    f.write("dir-right - direction\n")
                    f.write("dir-up - direction\n")

                    # player
                    f.write("player-01 - player\n")

                    # the objects in sokoban are the traversable cells
                    f.write("\n".join(
                        # convert cells left in a string
                        map(
                            # filter out cells in the world which are untraversable
                            lambda p: f"pos-{p.y:02d}-{p.x:02d} - location",
                            filter(
                                lambda p2: sokoban_world.BaseCellContent.UNTRAVERSABLE not in world[p2],
                                world.cells
                            )
                        )
                    ))

                    # blocks
                    f.write("\n".join(
                        map(
                            lambda x: f"block-{x:02d}",
                            range(len(world.blocks))
                        )
                    ))

                with Clause(f, indent=1, name="init", colon=True):
                    # position of the robot
                    for cell in world.cells:
                        if not world.is_traversable(*cell):
                            continue

                        if sokoban_world.BaseCellContent.ROBOT in world[cell]:
                            PddlSokobanConverter._write_predicate(f, name="has_player", value=f"cell_{cell.y}_{cell.x}")
                            f.write(" ")
                        elif sokoban_world.BaseCellContent.BLOCK in world[cell]:
                            PddlSokobanConverter._write_predicate(f, name="has_box", value=f"cell_{cell.y}_{cell.x}")
                            f.write(" ")
                        elif sokoban_world.BaseCellContent.GOAL in world[cell]:
                            PddlSokobanConverter._write_predicate(f, name="IS-GOAL", value=f"pos-{cell.y:02d}-{cell.x:02d}")
                            f.write(" ")
                        elif sokoban_world.BaseCellContent.GOAL not in world[cell]:
                            PddlSokobanConverter._write_predicate(f, name="IS-NONGOAL", value=f"pos-{cell.y:02d}-{cell.x:02d}")
                            f.write(" ")

                        adjacent_cell_callbacks = [world.get_up, world.get_down, world.get_left, world.get_right]
                        for adjacent_cell_callback in adjacent_cell_callbacks:
                            try:
                                next_cell = adjacent_cell_callback(*cell)
                                if world.is_traversable(*next_cell):
                                    PddlSokobanConverter._write_predicate(f,
                                        name="adjacent",
                                        value=[
                                            f"cell_{cell.y}_{cell.x}",
                                            f"cell_{next_cell.y}_{next_cell.x}"
                                        ])
                                    f.write(" ")
                            except ValueError as e:
                                # there is no such cell
                                pass

                with Clause(f, indent=1, name="goal", colon=True):
                    with Clause(f, name="and", fake=len(world.goals) == 1):
                        for p in world.goals:
                            PddlSokobanConverter._write_predicate(f, name="has_box", value=f"cell_{p.y}_{p.x}")
                            f.write(" ")
