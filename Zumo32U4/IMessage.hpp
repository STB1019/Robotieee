/**
 * @file    IMessage.hpp
 * 
 * Interface for the message contained in a package.
 * Suitable for different protocol version implementations.
 * 
 * @date    May 4, 2018
 * @author  paolopasqua
 */

#ifndef INTERFACE_MESSAGE_HPP_
#define INTERFACE_MESSAGE_HPP_

#include "string.hpp"

using namespace robo_utils;

namespace robotieee {
    /* Max length for arguments */
    #define ARGS_LENGHT     10
    /* Max length of a message */
    #define MAX_MESSAGE_LENGTH  1 + ARGS_LENGHT

    /**
     * Represents interface for the message contained in a package.
     * Suitable for implementations of different protocol version.
     */
    class IMessage {
    public:

        IMessage() {}
        virtual ~IMessage() {}

        /**
         * Set the type of the message.
         * 
         * @param[in]   pType   message's type
         */
        virtual void setType(char pType) = 0;

        /**
         * Get the type of the message.
         * 
         * @return message's type
         */
        virtual char getType() = 0;

        /**
         * Set the args of the message.
         * 
         * @param[in]   pArgs   message's args
         */
        virtual void setArgs(char* pArgs) = 0;

        /**
         * Get the args of the message.
         * 
         * @return  message's args
         */
        virtual char* getArgs() = 0;

        /**
         * Returns the message data coded into a string.
         * NB: the string has to be deallocated after the use.
         * 
         * @return  the message data in string form
         */
        virtual string<MAX_MESSAGE_LENGTH>* toString() = 0;

    protected:
        /** Message type */
        char _type;
        /** Message arguments */
        char _args[ARGS_LENGHT+1];

    };
}

#endif /* INTERFACE_MESSAGE_HPP_ */