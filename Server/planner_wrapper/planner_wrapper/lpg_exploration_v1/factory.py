import os

from planner_wrapper import config
from planner_wrapper.interfaces import IJsonToWorld, IWorldToPddlProblemConverter, IPlanner, \
    IPlanFilenameToPlanConverter, IPlanToJsonConverter, IPlannerFactory
from planner_wrapper.lpg_exploration_v1.explorationworld_to_pddlproblem import ExplorationWorldToPddlConverter
from planner_wrapper.lpg_exploration_v1.json_to_world import JsonToExplorationWorld_V1
from planner_wrapper.lpg_exploration_v1.plan_to_json import LPG_V1_ExplorationPlanToJsonConverter
from planner_wrapper.lpg_exploration_v1.planneroutput_to_plan import LPG_V1_FilenameToExplorationPlanConverter
from planner_wrapper.planners.FakePlanner import FakePlanner
from planner_wrapper.planners.lpg_planner import LPGPlanner
from planner_wrapper.lpg_sokoban_v1.json_to_sokobanworld import JsonToSokobanWorld_V1
from planner_wrapper.lpg_sokoban_v1.plan_to_json import LPG_V1_PlanToJsonConverter
from planner_wrapper.lpg_sokoban_v1.planneroutput_to_plan import LPG_V1_FilenameToPlanConverter
from planner_wrapper.lpg_sokoban_v1.sokobanworld_to_pddlproblem import V1_SokobanWorldToPddlConverter


class LPG_Exploration_V1_Factory(IPlannerFactory):

    @property
    def domain_filename(self) -> str:
        return os.path.abspath("../Problems/Exploration/domain.pddl")

    def json_to_world(self)-> IJsonToWorld:
        return JsonToExplorationWorld_V1()

    def world_to_pddl_problem(self) -> IWorldToPddlProblemConverter:
        return ExplorationWorldToPddlConverter()

    @property
    def planner(self) -> IPlanner:
        if config.USE_PLANNER == "lpg":
            planner = LPGPlanner(lpg_location=config.PLANNER_LOCATION)

            planner.use_best_first = False
            planner.use_lpg = True
            planner.solutions_to_find = 1
            planner.output_filename = os.path.join(".", "computed.plan")
        elif config.USE_PLANNER == "fake":
            planner = FakePlanner()
        else:
            raise ValueError("{} invalid!".format(config.USE_PLANNER))

        return planner

    def plan_filename_to_plan(self) -> IPlanFilenameToPlanConverter:
        return LPG_V1_FilenameToExplorationPlanConverter()

    def plan_to_json(self) -> IPlanToJsonConverter:
        return LPG_V1_ExplorationPlanToJsonConverter()

