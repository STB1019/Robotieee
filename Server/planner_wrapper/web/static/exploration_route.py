import os
from flask import Blueprint, request
from flask import current_app
from werkzeug.local import LocalProxy

from planner_wrapper import solution_converter
from planner_wrapper.lpg_sokoban_v1.factory import LPG_V1_Factory
from planner_wrapper.lpg_sokoban_v2.factory import LPG_V2_Factory
from web.static.flask_exceptions import SolutionNotFoundException, MalformedRequestException

simple_page = Blueprint('index', __name__)

# logger
logger = LocalProxy(lambda: current_app.logger)

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

    #get request version
    if "version" not in content:
        raise MalformedRequestException("version key is not present in the json request!")

    #ok, let's decide which factory to use
    if content["version"] == "1.0":
        factory = LPG_V1_Factory()
    elif content["version"] == "1.1":
        factory = LPG_V2_Factory()
    else:
        raise MalformedRequestException("unsupported version number!")

    logger.info('generating sokoban world from received json...')
    sokoban_world = factory.json_to_world().convert_json_to_sokoban_world(content)
    logger.info('generating pddl problem file from sokoban world...')
    problem_filename = factory.sokoban_world_to_pddl_problem().generate_problem(
        problem_filename="problem_instance",
        domain_name="sokobanSequential",
        problem_name="sokobanSequential-01",
        world=sokoban_world,
    )

    logger.info('generating planner instance manager...')
    planner = factory.planner

    logger.info('invoking planner (this may take quite time!)...')
    ret = planner.invoke(
        domain_filename=factory.domain_filename,
        problem_filename=problem_filename,
        working_directory="."
    )
    if ret is False:
        logger.info("planner didn't find any solutions!")
        # the planner didn't generate anything
        raise SolutionNotFoundException(f"Couldn't find solution for problem!")

    logger.info('Solution found! computing the json of it!')

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

    actions = factory.plan_filename_to_plan().convert_plan_filename_into_plan(planner.output_filename)
    json_string = factory.plan_to_json().convert_plan(actions)
    logger.info('Returning json...')
    return json_string
