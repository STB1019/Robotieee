import typing

from planner_wrapper.domains.exploration.exploration_world import ExplorationWorld
from planner_wrapper.domains.sokoban.sokoban_world import BaseCellContent
from planner_wrapper.interfaces import IJsonToWorld


class JsonToExplorationWorld_V1(IJsonToWorld):

    def convert_json_to_model_world(self, js: typing.Dict[str, typing.Any]) -> typing.Any:
        world = js['world']
        rows = int(world['rows'])
        cols = int(world['columns'])
        retval = ExplorationWorld(rows, cols)

        for cell in world['cells']:
            for ch in cell['entities']:
                retval.add_content_in(
                    col=int(cell['x']),
                    row=int(cell['y']),
                    content=BaseCellContent.parse(str(ch))
                )

        return retval