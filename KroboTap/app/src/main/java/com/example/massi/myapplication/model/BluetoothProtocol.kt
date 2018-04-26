package com.example.massi.myapplication.model

import com.example.massi.myapplication.InvalidRobotieeeMessage
import com.example.massi.myapplication.KroboTappException
import com.example.massi.myapplication.WhenClauseException

public enum class RobotieeeMessageID {

    LOCATION {
        override fun id() : Char { return 'L';}
    },
    WARNING {
        override fun id() : Char { return 'W';}
    },
    DONE {
        override fun id() : Char { return 'D';}
    },
    MOVEMENT {
        override fun id() : Char { return 'M';}
    },
    END {
        override fun id() : Char { return 'E';}
    },
    STATE_CHANGE {
        override fun id() : Char { return 'S';}
    };

    val id: Char
        get() = this.id()

    protected abstract fun id() : Char
}

/**
 * Represents a message detected from the robot
 *
 * Messages can be built in 2 ways:
 *
 * <ul>
 *     <li>from a string received from the bluetooth: in this way you need to call the constructor acceppting a String</li>
 *     <li>from the application itself, which want to generate a message to send to the bluetooth; you need to call the primary constructor</li>
 * </ul>
 */
public open abstract class AbstractRobotieeeMessage {

    companion object {
        public const val SEPARATOR = '#'
    }


    protected abstract val pattern : Regex
    public open val message : String
    protected val matchResult: MatchResult?


    /**
     * Empty constructor
     *
     * Does nothing
     *
     * You need to set the field #message and all the other fields yourself
     */
    protected constructor() {
        this.message = ""
        this.matchResult = null
    }

    /**
     * Parse a message from a string
     *
     * Note that the string does not need to contain <b>only</b> the specification of the message.
     * Hoowever, it <b>does need</b> to start with the specification of the message.
     *
     * @throws InvalidRobotieeeMessage if the string does not start with the specification of this message
     */
    public constructor(msg: String) {
        val matchResult = pattern.find(msg,0)
        this.matchResult = matchResult ?: throw InvalidRobotieeeMessage(String.format("Can't build %s from message \"%s\"!", javaClass.simpleName, msg))
        this.message = this.matchResult.groups!!.get(0)!!.value
    }

    /**
     * Convert this message object into a string which can be sent via bluetooth to the robot itself
     *
     * @return a String which the robot can handle
     */
    public abstract fun toRobotieeeProtocol() : String

}

public open class WarningMessage : AbstractRobotieeeMessage {

    override val pattern = Regex(String.format("""%s(?<instructions>\d)""", RobotieeeMessageID.WARNING.id))

    public val instructionsNumber : Int
    override val message: String by lazy {this.toRobotieeeProtocol()}

    public constructor(instructionNumber: Int) {
        this.instructionsNumber = instructionNumber
    }

    public constructor(msg: String) : super(msg) {
        instructionsNumber = this.matchResult?.groups?.get(2)!!.value.toInt()
    }

    override fun toRobotieeeProtocol(): String {
        return String.format("%s%d", RobotieeeMessageID.WARNING.id, this.instructionsNumber)
    }
}

/**
 * Represents a DONE message received from the robot
 */
public open class DoneMessage : AbstractRobotieeeMessage {

    override val pattern = Regex(String.format("""%s""", RobotieeeMessageID.DONE.id))

    override val message: String by lazy {this.toRobotieeeProtocol()}

    public constructor() {

    }

    public constructor(msg: String) : super(msg) {

    }

    override fun toRobotieeeProtocol(): String {
        return String.format("%s", RobotieeeMessageID.DONE.id)
    }
}

/**
 * Reprepsents a message specifying that the robot is on a particular cell
 */
public open class LocationMessage: AbstractRobotieeeMessage {

    override val pattern = Regex(String.format("""%s(?<robotx>\d)(?<roboty>\d)\?\?""", RobotieeeMessageID.LOCATION.id))

    override val message: String by lazy {this.toRobotieeeProtocol()}
    public val robot: Point by lazy { Point(0,0) }

    /**
     * Alias of #LocationMessage(y, x)
     */
    constructor(p: Point) : this(p.y, p.x)

    public constructor(y: Int, x: Int) {
        this.robot.x = x
        this.robot.y = y
    }

    public constructor(msg: String) : super(msg) {
        robot.x = this.matchResult?.groups!!.get(1)!!.value.toInt()
        robot.y = this.matchResult?.groups!!.get(2)!!.value.toInt()
    }

    override fun toRobotieeeProtocol(): String {
        return String.format("%s%d%d??", RobotieeeMessageID.LOCATION.id, robot.x, robot.y)
    }

}

/**
 * Message detected when the robot informs you it detected a block  ahead of it
 */
public class BlockDetectedMessage : LocationMessage {

    override val pattern = Regex(String.format("""%s(?<robotx>\d)(?<roboty>\d)(?<blockx>\d)(?<blocky>\d)""", RobotieeeMessageID.LOCATION.id))

    override val message: String by lazy {this.toRobotieeeProtocol()}
    private val block: Point

    constructor(robotY: Int, robotX: Int, blockY: Int, blockX: Int) : super(robotY, robotX) {
        this.block = Point(blockY, blockX)
    }

    constructor(robot:Point, block: Point) : this(robot.y, robot.x, block.y, block.x)

    constructor(str: String) : super(str)  {
        this.block = Point(
                this.matchResult?.groups!!.get(4)!!.value.toInt(),
                this.matchResult?.groups!!.get(3)!!.value.toInt()
        )
    }

    override fun toRobotieeeProtocol(): String {
        return String.format("%s%d%d%d%d", RobotieeeMessageID.LOCATION.id, robot.x, robot.y, block.x, block.y)
    }

}

public class MovementMessage : AbstractRobotieeeMessage {

    public enum class Type {
        IS_MOVE{
            override val type: Int
                get() = 0
        },
        IS_NOT_MOVE{
            override val type: Int
            get() = 1
        };

        abstract val type: Int

    }

    override val pattern = Regex(String.format("""%s%c(?<direction>\d)(?<type>\d)""", RobotieeeMessageID.MOVEMENT.id, AbstractRobotieeeMessage.SEPARATOR))

    override val message: String by lazy {this.toRobotieeeProtocol()}
    private val direction: Direction
    private val type : MovementMessage.Type

    public constructor(direction: Direction, type: MovementMessage.Type) : super() {
        this.direction = direction
        this.type = type
    }

    constructor(str: String) : super(str)  {
        when(this.matchResult?.groups!!.get(1)!!.value.toInt()) {
            0 -> direction = Direction.UP
            1 -> direction = Direction.RIGHT
            2 -> direction = Direction.DOWN
            3 -> direction = Direction.LEFT
            else -> throw WhenClauseException(String.format(""""%d" is invalid""", this.matchResult?.groups!!.get(1)!!.value.toInt()))
        }

        when (this.matchResult?.groups!!.get(2)!!.value.toInt()){
            Type.IS_MOVE.type -> type = Type.IS_MOVE
            Type.IS_NOT_MOVE.type -> type = Type.IS_NOT_MOVE
            else -> throw WhenClauseException(String.format(""""%d" is invalid!""", this.matchResult?.groups!!.get(2)!!.value.toInt()))
        }
    }

    override fun toRobotieeeProtocol(): String {
        var d : Int
        when (direction) {
            Direction.UP -> d = 0
            Direction.RIGHT -> d = 1
            Direction.DOWN -> d = 2
            Direction.LEFT -> d = 3
            else -> throw WhenClauseException(String.format(""""%s is invalid!""", direction))
        }

        return String.format("%s%c%d%d", RobotieeeMessageID.MOVEMENT.id, AbstractRobotieeeMessage.SEPARATOR, d, type.type)
    }

}

public class EndMessage : AbstractRobotieeeMessage {

    override val pattern = Regex(String.format("""%s%c""", RobotieeeMessageID.END.id, AbstractRobotieeeMessage.SEPARATOR))
    override val message: String by lazy {this.toRobotieeeProtocol()}

    constructor() : super() {
    }

    constructor(str: String) : super(str) {

    }

    override fun toRobotieeeProtocol(): String {
        return String.format("%s%c", RobotieeeMessageID.END.id, AbstractRobotieeeMessage.SEPARATOR)
    }
}

public class StateChangeMessage : AbstractRobotieeeMessage {

    override val pattern = Regex(String.format("""%s%cE""", RobotieeeMessageID.STATE_CHANGE.id, AbstractRobotieeeMessage.SEPARATOR))
    override val message: String by lazy {this.toRobotieeeProtocol()}

    constructor() : super() {
    }

    constructor(str: String) : super(str) {

    }

    override fun toRobotieeeProtocol(): String {
        return String.format("%s%cE", RobotieeeMessageID.STATE_CHANGE.id, AbstractRobotieeeMessage.SEPARATOR)
    }
}

public class RobotieeeProtocolMessageFactory() {

    /**
     * Parse a message from a string
     *
     * @param[message] the payload from the bluetooth. Note that this string may contain several message. The important factor is
     *  that there is a compliant message at the beginning of the string
     * @throws InvalidRobotieeeMessage if the string does not contain at the beginning a valid message
     */
    public fun get(message: String) : AbstractRobotieeeMessage {
        if (message.isEmpty()) {
            throw KroboTappException(String.format("The message received by the robot is empty!"))
        }

        val calls = arrayOf(
                {s: String -> StateChangeMessage(s)},
                {s: String -> EndMessage(s)},
                {s: String -> LocationMessage(s)},
                {s: String -> MovementMessage(s)},
                {s: String -> DoneMessage(s)},
                {s: String -> WarningMessage(s)}
        )

        var result: AbstractRobotieeeMessage
        for (call in calls) {
            try {
                result = call(message)
                //we find something!
                return result
            } catch (e: InvalidRobotieeeMessage) {
                continue
            }
        }
        //if we didn't find anything parsable, throw an exception
        throw InvalidRobotieeeMessage(String.format("""Couldn't parse message "%s"""", message))

    }

}

