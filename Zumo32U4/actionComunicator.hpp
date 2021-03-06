 /**
 * @file
 * 
 * API specifying a class to manage the communication of the robot
 * Version 1 of communication protocol.
 *
 * @date Apr 11, 2018
 */

#ifndef COMUNICATOR_HPP_
#define COMUNICATOR_HPP_

#include "compositeAction.hpp"
#include "string.hpp"
#include "point.hpp"
#include <Zumo32U4.h>

using namespace robo_utils;

namespace robotieee {

    #define CLUSTER_LENGTH_MAX      10
    #define CLUSTER_LENGTH_DEFAULT  10

    #define HEADER_SEPARATOR        '#'

    #define HEADER_TYPE_MOVE        'M'
    #define HEADER_TYPE_STATE       'S'
    #define HEADER_TYPE_END         'E'
    #define HEADER_TYPE_LOCATION    'L'
    #define HEADER_TYPE_WARNING     'W'
    #define HEADER_TYPE_DONE        'D'

    #define SEND_MESSAGE_LENGTH     20

    #define ARGS_LENGTH_MOVE        2
    #define ARGS_LENGTH_STATE       1
    #define ARGS_LENGTH_END         0
    #define ARGS_LENGTH_LOCATION    4
    #define ARGS_LENGTH_WARNING     1
    #define ARGS_LENGTH_DONE        0    


    /**
     * Represents the action comunication manager for version 1 of protocol
     */
    [[deprecated("Replaced by ICommunicator, which interface the communication protocol")]]
    class actionComunicator {

    public:
        /**
         * Initializes the actionComunicator.
         */
        actionComunicator();
        
        /**
         * Set a new cluster length.
         * Max value CLUSTER_LENGTH_MAX
         * 
         * @param[in]   length  new cluster length.
         */
        void setClusterLength(uint8_t length);

        /**
         * Return the actual cluster length
         * 
         * @return  actual cluster length
         */
        uint8_t getClusterLength();

        /**
         * Clear the host buffer
         */
        void flush();

        /**
         * Reads from host buffer and create a cluster of action.
         * This actions are consolidated: the equal actions on series are write as one action with n repetitions.
         * 
         * @return true: all actions have been rececived.
         */
        bool receiveCluster();

        /**
         * Returns the next action to execute in the cluster read before.
         * NB: if there's no action to return, it returns an empty action with type = '\0'
         * 
         * @return action to execute
         */
        compositeAction getNextAction();

        /**
         * Returns if the last cluster read is empty or not.
         * 
         * @return true: last cluster read is empty
         */
         bool isClusterEmpty();
        
        /**
         * Send a LOCATION message with block position known.
         * Arguments are <x block><y block><x robot><y robot> with a numeration like: 0123456789abcdefghil...
         * 
         * @param[in]   blockPosition   the block position to send as argument
         * @param[in]   robotPosition   the robot position to send as argument
         * @return success of sending
         */
        bool sendLocation(const point blockPosition, const point robotPosition);

        /**
         * Send a LOCATION message with block position unknown.
         * Arguments are "??<robot_x><robot_y>". The "?" chars are used to indicate that
         * the block was not found yet. The purpose of this message is to merely let the
         * middleware know which cells we visited, in order to be able to tell it to the planner
         * and generate right plans when re-planification of the grid exploration is needed.
         * 
         * @param[in] robotPosition the position of the robot whitin the grid
         * @return success of sending
         */
        bool sendLocationRobotOnly(const point robotPosition);

        /**
         * Send a WARNING message.
         * 
         * @param[in]   index   index of the last action executed (counting the repetition) with a numeration like: 0123456789abcdefghil...
         * @return success of sending
         */
        bool sendWarning(uint8_t index);

        /**
         * Send a DONE message.
         * 
         * @return success of sending
         */
        bool sendDone();

        /**
         * dispose the receiver
         */
        ~actionComunicator();

    private:
        /** Buffer of actions read on bluetooth buffer */
        compositeAction _cluster[CLUSTER_LENGTH_MAX];
        /** Length of cluster of action to receive. Default: DEFAULT_CLUSTER_LENGTH */
        uint8_t _clusterLength;
        /** Index of the action to return as next to execute */
        uint8_t _clusterIndex;
        /** Number of actions saved on buffer after consolidation */
        uint8_t _actionsOnCluster;

        /**
         * Read one byte from bluetooth buffer.
         * Use a delay to allow the transmition of byte if reading while writing.
         * 
         * @return  byte read
         */
        char readByte();

        /**
         * Send a message to the host.
         * 
         * @param[in]   type    message's type
         * @param[in]   args    message's arguments
         * @return  success of sending
         */
        bool sendMessage(char type, char* args);

        /**
         * Create a message to send.
         * 
         * @param[in]   type    message's type
         * @param[in]   args    message's arguments
         * @return  message
         */
        robo_utils::string<SEND_MESSAGE_LENGTH> createMessage(char type, char* args);
    };

}

#endif /* COMUNICATOR_HPP_ */
