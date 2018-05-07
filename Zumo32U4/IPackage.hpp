/**
 * @file    IPackage.hpp
 * 
 * Interface for the package in the communication between Host and Robot.
 * Suitable for different protocol version implementations.
 * 
 * @date    May 4, 2018
 * @author  paolopasqua
 */

#ifndef INTERFACE_PACKAGE_HPP_
#define INTERFACE_PACKAGE_HPP_

#include <Zumo32U4.h>
#include "IMessage.hpp"
#include "string.hpp"

using namespace robo_utils;

namespace robotieee {

    /* The length in byte of the package header */
    #define PACKAGE_HEADER_BYTE_LENGTH  4
    /* Max value for the payload length */
    #define MAX_PAYLOAD_LENGTH          255 + 1
    /* Max length of a package */
    #define MAX_PACKAGE_LENGTH          PACKAGE_HEADER_BYTE_LENGTH + MAX_PAYLOAD_LENGTH

    /* Value to inibite the version check in the verify method */
    #define NO_VERIFY_VERSION_VALUE     -1

    /**
     * Represents interface for the package in the communication between Host and Robot.
     * Suitable for implementations of different protocol version.
     */
    class IPackage {
    public:

        virtual IPackage() {}
        virtual ~IPackage() {}

        /**
         * Sets the protocol version of the package.
         * 
         * @param[in]   version protocol version
         */
        virtual void setVersion(uint8_t version) = 0; 

        /**
         * Gets the protocol version of the package.
         * 
         * @return  protocol version
         */
        virtual uint8_t getVersion() = 0;
        
        /**
         * Sets the ID of the package.
         * 
         * @param[in]   id  package ID
         */
        virtual void setID(uint8_t id) = 0;

        /**
         * Gets the ID of the package.
         * 
         * @return  package ID
         */
        virtual uint8_t getID() = 0;
        
        /**
         * Sets the payload length of the package.
         * 
         * @param[in]   payloadLength   payload length
         */
        virtual void setPayloadLength(uint8_t payloadLength) = 0;

        /**
         * Gets the payload length of the package.
         * 
         * @return  payload length
         */
        virtual uint8_t getPayloadLength() = 0;

        /**
         * Sets the type of the package
         * 
         * @param[in]   type    package type
         */
        virtual void setType(char type) = 0;

        /**
         * Gets the type of the package.
         * 
         * @return  package type
         */
        virtual char getType() = 0;
        
        /**
         * Sets the payload of the package.
         * 
         * @param[in]  payload reference to the data
         */
        virtual void setPayload(string<MAX_PAYLOAD_LENGTH>* payload) = 0;

        /**
         * Append a single character to the payload of the package.
         * 
         * @param[in]  data character to append to the payload
         * @return  the result of the operation
         */
        virtual bool appendToPayload(char data) = 0;

        /**
         * Gets the reference to the payload of the package.
         * 
         * @return  package payload reference
         */
        virtual string<MAX_PAYLOAD_LENGTH>* getPayload() = 0;
        
        /**
         * Gets the payload of the package masked as an compositeAction object.
         * 
         * @param[out]  payload reference to the compositeAction object to fill up
         */
        virtual void getPayloadAsMessage(IMessage* payload) = 0;

        /**
         * Clones the package passed into the actual object.
         * 
         * @param[in]   package package to clone
         */
        virtual void clone(const IPackage* package) = 0;

        /**
         * Verifies the package data setted. Return if the object represent a correct package.
         * 
         * @param[in]   protocolVersion the protocol version that the package has to implement. If no need this check, give the value IPackage::NO_VERIFY_VERSION_VALUE.
         * @return  true: the object data represent a package; false: otherwise.
         */
        virtual bool verify(uint8_t protocolVersion = NO_VERIFY_VERSION_VALUE) = 0;

        /**
         * Verifies if the package set as parameter equals the current object.
         * 
         * @param[in]   package package to check
         * @return  true: the package equal the current object; false: otherwise 
         */
        virtual bool equals(const IPackage* package) = 0;

        /**
         * Returns the package data coded into a string.
         * NB: the string has to be deallocated after the use.
         * 
         * @return  the package data in string form
         */
        virtual string<MAX_PACKAGE_LENGTH>* toString() = 0;

    protected:
        /* HEADER */

        /** protocol version */
        uint8_t _version;
        /** utility flags. Used to represent the package ID on the fifth position */
        uint16_t _flags;
        /** length of payload */
        uint8_t _payloadLength;
        /** package type */
        char _type;

        /* PAYLOAD */

        /** payload content */
        string<MAX_PAYLOAD_LENGTH> _payload = string<MAX_PAYLOAD_LENGTH>{};

    };
}

#endif /* INTERFACE_PACKAGE_HPP_ */