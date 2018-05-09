/**
 * @file    CommunicationPackage.hpp
 * 
 * API specifying a class that represent a package in the communication between Host and Robot.
 * Suitable for use with protocol version 2 onwards.
 * 
 * @date    May 4, 2018
 * @author  paolopasqua
 */

#ifndef COMMUNICATION_PACKAGE_HPP_
#define COMMUNICATION_PACKAGE_HPP_

#include "IPackage.hpp"
#include "IMessage.hpp"
#include "string.hpp"

using namespace robo_utils;

namespace robotieee {

    /* Package type: instruction */
    #define PACKAGE_TYPE_INSTRUCTION            'I'
    /* Package type: acknowledge */
    #define PACKAGE_TYPE_ACKNOWLEDGE            'A'
    /* Package type: communication */
    #define PACKAGE_TYPE_COMMUNICATION          'C'
    
    /* position of ID on private variable _flags bit from right */
    #define FLAG_ID_POSITION                    1
    /* number of bit that the version needs in the first byte */
    #define VERSION_BIT_LENGTH                  4

    /* Value returned by the parsing to extract version */
    #define STRING_PARSE_VERSION_ERROR          -1
    /* Value returned by the parsing to extract id */
    #define STRING_PARSE_ID_ERROR               -1
    /* Value returned by the parsing to extract payload length */
    #define STRING_PARSE_PAYLOADLENGHT_ERROR    -1
    /* Value returned by the parsing to extract type */
    #define STRING_PARSE_TYPE_ERROR             '\0'
    /* Value returned by the parsing to extract payload */
    #define STRING_PARSE_PAYLOAD_ERROR          nullptr

    /**
     * Implements IPackage interface.
     * Represents the package in the communication between Host and Robot.
     * Suitable for implementations of second version protocol onwards.
     */
    class CommunicationPackage : public IPackage {
    public:

        /**
         * Initializes the package.
         * 
         * @param[in]   version         protocol version
         * @param[in]   id              package id
         * @param[in]   payloadLength   payload length
         * @param[in]   type            package type
         */
        CommunicationPackage(uint8_t version, uint8_t id, uint8_t payloadLength, char type);

        /**
         * Initializes an empty package.
         */
        CommunicationPackage();

        /**
         * Sets the protocol version of the package.
         * 
         * @param[in]   version protocol version
         */
        void setVersion(uint8_t version); 

        /**
         * Gets the protocol version of the package.
         * 
         * @return  protocol version
         */
        uint8_t getVersion();
        
        /**
         * Sets the ID of the package.
         * 
         * @param[in]   id  package ID
         */
        void setID(uint8_t id);

        /**
         * Gets the ID of the package.
         * 
         * @return  package ID
         */
        uint8_t getID();
        
        /**
         * Sets the payload length of the package.
         * 
         * @param[in]   payloadLength   payload length
         */
        void setPayloadLength(uint8_t payloadLength);

        /**
         * Gets the payload length of the package.
         * 
         * @return  payload length
         */
        uint8_t getPayloadLength();

        /**
         * Sets the type of the package
         * 
         * @param[in]   type    package type
         */
        void setType(char type);

        /**
         * Gets the type of the package.
         * 
         * @return  package type
         */
        char getType();
        
        /**
         * Sets the payload of the package.
         * 
         * @param[in]  payload reference to the data
         */
        void setPayload(string<MAX_PAYLOAD_LENGTH>* payload);

        /**
         * Append a single character to the payload of the package.
         * 
         * @param[in]  data character to append to the payload
         * @return  the result of the operation
         */
        bool appendToPayload(char data);

        /**
         * Gets the reference to the payload of the package.
         * 
         * @return  package payload reference
         */
        string<MAX_PAYLOAD_LENGTH>* getPayload();
        
        /**
         * Gets the payload of the package masked as an compositeAction object.
         * 
         * @param[out]  payload reference to the compositeAction object to fill up
         */
        void getPayloadAsMessage(IMessage* payload);

        /**
         * Clones the package passed into the actual object.
         * 
         * @param[in]   package package to clone
         */
        void clone(IPackage* package);
        
        /**
         * Verifies the package data setted. Return if the object represent a correct package.
         * 
         * @param[in]   protocolVersion the protocol version that the package has to implement. If no need this check, give the value IPackage::NO_VERIFY_VERSION_VALUE.
         * @return  true: the object data represent a package; false: otherwise.
         */
        bool verify(uint8_t protocolVersion = NO_VERIFY_VERSION_VALUE);

        /**
         * Verifies if the package set as parameter equals the current object.
         * 
         * @param[in]   package package to check
         * @return  true: the package equal the current object; false: otherwise 
         */
        bool equals(IPackage* package);

        /**
         * Returns the package data coded into a string.
         * NB: the string has to be deallocated after the use.
         * 
         * @return  the package data in string form
         */
        string<MAX_PACKAGE_LENGTH>* toString();

        /**
         * dispose the receiver
         */
        ~CommunicationPackage();

        //STATIC

        /**
         * Initialize a package from string.
         * 
         * @param[in]   package package to Initialize
         * @param[in]   str     string of data
         * @return  true: package correctly initialized; false: otherwize
         */
        static bool initFromString (IPackage* package, char* str);

        /**
         * Extract the version from a package in string form.
         * 
         * @param[in]   str string of data
         * @return  protocol version; STRING_PARSE_VERSION_ERROR if it is not possible
         */
        static uint8_t getVersionFromString(char* str);

        /**
         * Extract the ID from a package in string form.
         * 
         * @param[in]   str string of data
         * @return  package ID; STRING_PARSE_ID_ERROR if it is not possible
         */
        static uint8_t getIDFromString(char* str);

        /**
         * Extract the payload length from a package in string form.
         * 
         * @param[in]   str string of data
         * @return  payload length; STRING_PARSE_PAYLOADLENGHT_ERROR if it is not possible
         */
        static uint8_t getPayloadLengthFromString(char* str);

        /**
         * Extract the type from a package in string form.
         * 
         * @param[in]   str string of data
         * @return  package type; STRING_PARSE_TYPE_ERROR if it is not possible
         */
        static char getTypeFromString(char* str);

        /**
         * Extract the payload from a package in string form.
         * NB: the string has to be deallocated after the use.
         * 
         * @param[in]   str string of data
         * @return  payload; STRING_PARSE_PAYLOAD_ERROR if it is not possible
         */
        static string<MAX_PAYLOAD_LENGTH>* getPayloadFromString(char* str);

    private:

        /* METHOD */

        /** 
         * Sets the ID from integer.
         * 
         * @param[in]   id  the id to set
         */
        void setIDFromInt(uint8_t id);

        /**
         * Gets the ID as integer.
         * 
         * @return  package ID as integer
         */
        uint8_t getIDAsInt();
    };
}

#endif /* COMMUNICATION_PACKAGE_HPP_ */