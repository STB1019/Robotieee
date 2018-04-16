import os
from flask import Blueprint, request
from flask import current_app
from werkzeug.local import LocalProxy
import logging

from planner_wrapper import solution_converter
from planner_wrapper.lpg_exploration_v1.factory import LPG_Exploration_V1_Factory
from planner_wrapper.lpg_sokoban_v1.factory import LPG_V1_Factory
from planner_wrapper.lpg_sokoban_v2.factory import LPG_V2_Factory
from web.static.flask_exceptions import SolutionNotFoundException, MalformedRequestException

simple_page = Blueprint('exploration', __name__)

@simple_page.route("/exploration_problem", methods=['POST'])
def sokoban_problem():
    """
    Example:

    PUSH
    curl -i http://localhost:5000/exploration_problem -X POST -d '{"version": "1.0","world": {"rows": 3,"columns": 3,"cells": [{ "y": 0, "x": 0, "entities": "DRG"},{ "y": 0, "x": 1, "entities": ""},{ "y": 0, "x": 2, "entities": "U"},{ "y": 1, "x": 0, "entities": ""},{ "y": 1, "x": 1, "entities": "B"},{ "y": 1, "x": 2, "entities": ""},{ "y": 2, "x": 0, "entities": ""},{ "y": 2, "x": 1, "entities": ""},{ "y": 2, "x": 2, "entities": ""}]}}' -H "Content-Type: application/json"

    :return:
    """
    content = request.get_json(silent=True)
    if content is None:
        raise MalformedRequestException("Couldn't fetch world json in this request. You should mark this request as a json one and put in the payload a compliant json!")

    # get request version
    if "version" not in content:
        raise MalformedRequestException("version key is not present in the json request!")

    # ok, let's decide which factory to use
    if content["version"] == "1.0":
        factory = LPG_Exploration_V1_Factory()
    else:
        raise MalformedRequestException("unsupported version number!")

    logging.info('generating exploration world from received json...')
    exploration_world = factory.json_to_world().convert_json_to_model_world(content)
    logging.info('generating pddl problem file from exploration world...')
    problem_filename = factory.world_to_pddl_problem().generate_problem(
        problem_filename="problem_exploration_instance",
        domain_name="exploration",
        problem_name="exploration-01",
        world=exploration_world,
    )

    logging.info('generating planner instance manager...')
    planner = factory.planner

    if "solution_number" not in content:
        logging.info("solution number not found in json. using 1 by default")
        solution_number = 1
    else:
        solution_number = int(content["solution_number"])

    planner.solutions_to_find = solution_number

    logging.info('invoking planner (this may take quite time!)...')
    ret = planner.invoke(
        domain_filename=factory.domain_filename,
        problem_filename=problem_filename,
        working_directory="."
    )
    if ret is False:
        logging.info("planner didn't find any solutions!")
        # the planner didn't generate anything
        raise SolutionNotFoundException(f"Couldn't find solution for problem!")

    logging.info('Solution found! computing the json of it!')

    ###############################
    #NICOLA implementation (slick)#
    ###############################

    # p = solution_converter.plan_to_dict(planner.output_filename)
    # json_outputfilename = "output.json"
    # solution_converter.dict_to_json(p, json_outputfilename)
    # with open(json_outputfilename, "r") as f:
    #     json_string = f.read()

    #######################################
    # MAX implementation (more structured)#
    #######################################

    logging.info("converting plan into a inmemory representation...")
    actions = factory.plan_filename_to_plan().convert_plan_filename_into_plan(planner.output_filename)
    logging.info("converting plan from planner into a JSON...")
    json_string = factory.plan_to_json().convert_plan(actions)
    logging.info('Returning json...')
    return json_string
