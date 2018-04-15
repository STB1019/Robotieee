/*
   actionComunicator.cpp

    Created on: Apr 11, 2018
*/
#include "actionComunicator.hpp"
#include <Zumo32U4.h>
#include "string.hpp"

using namespace robo_utils;

#define BAUD_RATE           9600
#define TIMEOUT_RECEIVING   50
#define NO_DATA             '\0'

namespace robotieee {

    actionComunicator::actionComunicator() {
        //Transmit on pin 1 and receive on pin 0
        Serial1.begin(BAUD_RATE); //set baud rate

        _clusterLength = CLUSTER_LENGTH_DEFAULT;
        _clusterIndex = 0;
        _actionsOnCluster = 0;
    }

    void actionComunicator::setClusterLength(uint8_t length) {
        _clusterLength = length > CLUSTER_LENGTH_MAX ? CLUSTER_LENGTH_MAX : length;
    }

    uint8_t actionComunicator::getClusterLength() {
        return _clusterLength;
    }

    void actionComunicator::flush() {
        Serial1.flush();
    }

    bool actionComunicator::receiveCluster() {
        uint8_t argsLength = 0;
        char type = 'a'; //random init
        char args[ARGS_LENGHT] = "aaaaa"; //random init

        //reset the index of the message to return
        _clusterIndex = 0;

        //reset the number of action on cluster
        _actionsOnCluster = 0;

        #ifdef DEBUG_SERIAL
            Serial1.print("INITIALIZATION ");
            Serial1.print(_clusterIndex);
            Serial1.print(" ");
            Serial1.println(_actionsOnCluster);
        #endif

        // Wait until the buffer has received something
        while (Serial1.available() != 0) ;
        delay(2000);

        //stops if max number of message reads or no more message (timeout error)
        for (uint8_t i = 0; i < _clusterLength && type != NO_DATA; i++) {
            bool consolidated = false;
            uint8_t j = 0;

            #ifdef DEBUG_SERIAL
                Serial1.print("ITERATION ");
                Serial1.println(i);
            #endif

            type = readByte();
            readByte(); //HEADER_SEPARATOR

            #ifdef DEBUG_SERIAL
                Serial1.print("TYPE ");
                Serial1.print(type);
                Serial1.print(" ; ");
            #endif

            //consolidation: if the last message is the same, add a repetition to it
            if (i != 0 && _cluster[_actionsOnCluster-1].getType() == type) {
                //true assumption on same message's type, verified on the next test
                consolidated = true;
                
                #ifdef DEBUG_SERIAL
                    Serial1.print("CONSOLIDATION ; ");
                #endif
            }

            //If no error occurs
            if (type != NO_DATA) {
                switch(type) {
                    case HEADER_TYPE_MOVE:
                        argsLength = ARGS_LENGTH_MOVE;
                        break;

                    case HEADER_TYPE_STATE:
                        argsLength = ARGS_LENGTH_STATE;
                        break;

                    case HEADER_TYPE_END:
                        argsLength = ARGS_LENGTH_END;
                        break;

                    default:
                        argsLength = -1;
                        type = NO_DATA; //Error
                }

                #ifdef DEBUG_SERIAL
                    Serial1.print("ARGS ");
                    Serial1.print(argsLength);
                    Serial1.print(" ; ");
                #endif

                //reads the arguments: stops if reads all the char or if the last read is empty
                for(; j < argsLength && (j == 0 || args[j-1] != NO_DATA); j++) {
                    args[j] = readByte();
                    
                    #ifdef DEBUG_SERIAL
                        Serial1.print(args[j]);
                    #endif

                    //consolidation: test if arguments are the same for the last message and the actual one
                    if (consolidated && _cluster[_actionsOnCluster-1].getArgs()[j] != args[j]) {
                        consolidated = false;
                    }
                }

                #ifdef DEBUG_SERIAL
                    Serial1.print(" ; ");
                #endif

                //if not enought arguments are read --> Error
                if (j < argsLength) {
                    type = NO_DATA;
                    
                    #ifdef DEBUG_SERIAL
                        Serial1.print("ARGS ERROR ; ");
                    #endif
                }

                //clear args' cells not used
                for(; j < ARGS_LENGHT; j++)
                    args[j] = '\0';

                if (type != NO_DATA) {
                    //if consolidated: increments the repetition of the last message
                    if (consolidated) {
                        _cluster[_actionsOnCluster-1].addRepetition();
                        
                        #ifdef DEBUG_SERIAL
                            Serial1.print("CONSOLIDATED ");
                            Serial1.print(_actionsOnCluster-1);
                            Serial1.print(" ; ");
                        #endif
                    }
                    //if not consolidated: adds a new message
                    else {
                    //if (!consolidated){
                        _cluster[_actionsOnCluster].setType(type);
                        _cluster[_actionsOnCluster].setArgs(args);
                        _cluster[_actionsOnCluster].setRepetition(1);
                        _actionsOnCluster+=1;
                        
                        #ifdef DEBUG_SERIAL
                            Serial1.print("NEW ACTION ");
                            Serial1.print(_actionsOnCluster);
                            Serial1.print(" ");
                            Serial1.print(_actionsOnCluster-1);
                            Serial1.print(" ; ");
                        #endif
                    }
                }
            }

            #ifdef DEBUG_SERIAL
                Serial1.println("");
            #endif
        }

        //Error: no all cluster has been received
        if (type == NO_DATA) {
            return false;
        } 
        else {
            return true;
        }
    }

    compositeAction actionComunicator::getNextAction() {
        
        #ifdef DEBUG_SERIAL
            Serial1.print("INDEX TEST ");
            Serial1.print(_clusterIndex);
            Serial1.print(" ");
            Serial1.println(_actionsOnCluster);
        #endif
        
        if (_clusterIndex < _actionsOnCluster) {
            
            #ifdef DEBUG_SERIAL
                Serial1.print("ACTION RETURNED ");
                Serial1.println(_clusterIndex);
            #endif

            return _cluster[_clusterIndex++];
        }
        else {
            
            #ifdef DEBUG_SERIAL
                Serial1.println("ACTION NOT RETURNED");
            #endif

            return compositeAction();
        }
    }

    bool actionComunicator::isClusterEmpty() {
      return _actionsOnCluster <= 0;
    }
    
    bool actionComunicator::sendLocation(const point blockPosition, const point robotPosition) {
        string<10> args = string<10>{};
        //0123456789abcdefghil...
        args.append(blockPosition.x > 9 ? (char)('a' + blockPosition.x-10) : (char)('0' + blockPosition.x));
        //0123456789abcdefghil...
        args.append(blockPosition.y > 9 ? (char)('a' + blockPosition.y-10) : (char)('0' + blockPosition.y));
        //0123456789abcdefghil...
        args.append(robotPosition.x > 9 ? (char)('a' + robotPosition.x-10) : (char)('0' + robotPosition.x));
        //0123456789abcdefghil...
        args.append(robotPosition.y > 9 ? (char)('a' + robotPosition.y-10) : (char)('0' + robotPosition.y));
        
        return sendMessage(HEADER_TYPE_LOCATION, args.getBuffer());
    }

    bool actionComunicator::sendWarning(uint8_t index) {
        string<5> args = string<5>{};
        //0123456789abcdefghil...
        args.append(index > 9 ? (char)('a' + index-10) : (char)('0' + index));
        
        return sendMessage(HEADER_TYPE_WARNING, args.getBuffer());
    }

    bool actionComunicator::sendDone() {
        return sendMessage(HEADER_TYPE_DONE, "");
    }

    actionComunicator::~actionComunicator() {
        Serial1.end();
    }

    char actionComunicator::readByte() {
        char incomingByte = NO_DATA;

        // reply only when you receive data:
        if (Serial1.available() > 0) {
            // read the incoming byte:
            char readByte = (char) Serial1.read();
            incomingByte = readByte > 0 ? readByte : '\0';
        }

        return incomingByte;
    }

    bool actionComunicator::sendMessage(char type, char* args) {
        Serial1.print(createMessage(type, args).getBuffer());
        return true;
    }
    
    string<SEND_MESSAGE_LENGTH> actionComunicator::createMessage(char type, char* args) {
        string<SEND_MESSAGE_LENGTH> s = string<SEND_MESSAGE_LENGTH>{};
        s.append(type);
        s.append(HEADER_SEPARATOR);
        s.append(args);
        return s;
    }

}
