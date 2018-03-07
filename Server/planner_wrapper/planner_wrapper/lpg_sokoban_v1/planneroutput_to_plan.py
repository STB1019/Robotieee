import typing

import os

import re

from planner_wrapper.exceptions import ActionParseException
from planner_wrapper.interfaces import IPlanFilenameToPlanConverter, IPlannerAction
from planner_wrapper.domains.sokoban.sokoban_actions import SokobanMove, SokobanPushToNonGoal, Direction
from planner_wrapper.utils import Point


class LPG_V1_FilenameToPlanConverter(IPlanFilenameToPlanConverter):

    def convert_plan_filename_into_plan(self, plan_filename: str) -> typing.List[IPlannerAction]:
        plan_filename = os.path.abspath(plan_filename)
        with open(plan_filename, "r") as f:
            plan = f.readlines()

        # trim lines
        plan = map(lambda x: x.strip(' '), plan)
        plan = map(lambda x: x.strip('\t'), plan)
        plan = map(lambda x: x.strip('\n'), plan)
        # filter out empty lines
        plan = filter(lambda x: len(x) > 0, plan)
        # filter out lines starting with ";" a comment
        plan = filter(lambda x: x[0] != ';', plan)
        # the lines starting with a number represents a step of the solution
        plan = filter(lambda x: re.match('^[0-9]+', x), plan)
        # with the lines in plan we now build the actual plan
        actions = map(lambda x: self.fetch_action_information(x), plan)
        actions = map(lambda x: self._convert_action(action_name=x[0], action_parameters=x[1]), actions)

        return list(actions)

    def _convert_pos_into_cell(self, pos_value: str) -> Point:
        """
        Concvert stringa s like "POS-05-08" into points like <5,8>
        :param pos_value: the string to convert
        :return: the point converted
        """
        x, y = list(map(lambda x: int(x),
                        pos_value.split('-')[1:3]))  # consider only the last 2 position in the arrya splitted by -
        return Point(x, y)

    def _convert_action(self, action_name: str, action_parameters: typing.List[str]) -> IPlannerAction:
        if action_name == 'MOVE':
            return SokobanMove(action_name,
               player=action_parameters[0],
               start_pos=self._convert_pos_into_cell(action_parameters[1]),
               end_pos=self._convert_pos_into_cell(action_parameters[2]),
               direction=Direction.parse(action_parameters[3]),
            )
        elif action_name == 'PUSH-TO-NONGOAL':
            return SokobanPushToNonGoal(action_name,
                player=action_parameters[0],
                stone=action_parameters[1],
                player_pos=self._convert_pos_into_cell(action_parameters[2]),
                start_pos=self._convert_pos_into_cell(action_parameters[3]),
                end_pos=self._convert_pos_into_cell(action_parameters[4]),
                direction=Direction.parse(action_parameters[5]),
            )
        elif action_name == 'PUSH-TO-GOAL':
            return SokobanPushToNonGoal(action_name,
                player=action_parameters[0],
                stone=action_parameters[1],
                player_pos=self._convert_pos_into_cell(action_parameters[2]),
                start_pos=self._convert_pos_into_cell(action_parameters[3]),
                end_pos=self._convert_pos_into_cell(action_parameters[4]),
                direction=Direction.parse(action_parameters[5])
            )
        else:
            raise ActionParseException(f"Can't decode action {action_name}")