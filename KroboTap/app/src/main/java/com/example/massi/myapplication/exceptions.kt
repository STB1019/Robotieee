package com.example.massi.myapplication

class KroboTappException(override var message:String): Exception(message) {

}

/**
 * Thrown when a "when clause" unexpectedly goes into the "else" case
 */
class WhenClauseException(override var message:String): Exception(message) {

}

/**
 * Thrown when a message from the robot is (for some reason) invalid
 */
class InvalidRobotieeeMessage(override var message:String) : Exception(message) {

}