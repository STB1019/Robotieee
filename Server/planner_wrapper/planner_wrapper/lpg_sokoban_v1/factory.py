import os

from planner_wrapper import config
from planner_wrapper.interfaces import IJsonToWorld, ISokobanWorldToPddlProblemConverter, IPlanner, \
    IPlanFilenameToPlanConverter, IPlanToJsonConverter, IPlannerFactory
from planner_wrapper.planners.lpg_planner import LPGPlanner
from planner_wrapper.lpg_sokoban_v1.json_to_sokobanworld import JsonToSokobanWorld_V1
from planner_wrapper.lpg_sokoban_v1.plan_to_json import LPG_V1_PlanToJsonConverter
from planner_wrapper.lpg_sokoban_v1.planneroutput_to_plan import LPG_V1_FilenameToPlanConverter
from planner_wrapper.lpg_sokoban_v1.sokobanworld_to_pddlproblem import V1_SokobanWorldToPddlConverter


class LPG_V1_Factory(IPlannerFactory):

    @property
    def domain_filename(self) -> str:
        return os.path.abspath(config.SOKOBAN_DOMAIN_FILENAME)

    def json_to_world(self)-> IJsonToWorld:
        return JsonToSokobanWorld_V1()

    def sokoban_world_to_pddl_problem(self) -> ISokobanWorldToPddlProblemConverter:
        return V1_SokobanWorldToPddlConverter()

    @property
    def planner(self) -> IPlanner:
        planner = LPGPlanner(lpg_location=config.PLANNER_LOCATION)

        planner.use_best_first = True
        planner.use_lpg = False
        planner.solutions_to_find = 1
        planner.output_filename = os.path.join(".", "computed.plan")

        return planner

    def plan_filename_to_plan(self) -> IPlanFilenameToPlanConverter:
        return LPG_V1_FilenameToPlanConverter()

    def plan_to_json(self) -> IPlanToJsonConverter:
        return LPG_V1_PlanToJsonConverter()
