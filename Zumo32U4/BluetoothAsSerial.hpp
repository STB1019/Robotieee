/**
 * @file    BluetoothAsSerial.hpp
 * 
 * API specifying a class that represent a package in the communication between Host and Robot.
 * Suitable for use with protocol version 2 onwards.
 * 
 * @date    May 4, 2018
 * @author  paolopasqua
 */

#ifndef BLUETOOTH_SERIAL_HPP_
#define BLUETOOTH_SERIAL_HPP_

#include <Zumo32U4.h>
#include "ICommunicator.hpp"
#include "CommunicationPackage.hpp"
#include "compositeAction.hpp"
#include "string.hpp"
#include "point.hpp"

using namespace robo_utils;

namespace robotieee {
    /* Protocol version implemented */
    #define PROTOCOL_VERSION            2

    /* Message type: move */
    #define MESSAGE_TYPE_MOVE           'M'
    /* Message type: state change */
    #define MESSAGE_TYPE_STATE_CHANGE   'C'
    /* Message type: block found */
    #define MESSAGE_TYPE_FOUND          'F'
    /* Message type: block not found */
    #define MESSAGE_TYPE_NOT_FOUND      'N'
    /* Message type: error */
    #define MESSAGE_TYPE_ERROR          'E'
    
    /**
     * Implements ICommunicator interface.
     * Represents the communication manager between Host and Robot.
     * Suitable for implementations of second version protocol onwards.
     * The communication is with a Bluetooth module that Robot see as a serial module. The class uses Serial1 integrated class.
     */
    class BluetoothAsSerial : public ICommunicator {
    public:
        
        /**
         * Initialize the object
         */
        BluetoothAsSerial();

        /**
         * Checks if there's something to read.
         * 
         * @return  true: somenthing to read in buffer; false: otherwise
         */
        virtual bool somethingToRead();

        /**
         * Waits until it can read a package. If timeout occurs, the fuction will return false and the package parameter will not initialized.
         * Verifies the package received and if it's the same of the last received, it will re-send the last sent (that has to be the ack) 
         * 
         * @param[out]  package package to fill up
         * @param[in]   timeout timeout in millis for the package reading
         * @return  true: package read correctly; false: otherwise
         */
        virtual bool waitForPackage(IPackage* package, uint16_t timeout = DEFAULT_READ_TIMEOUT);

        /**
         * Sends a package.
         * 
         * @param[in]   package package to send
         * @return  true: the package sent correctly; false: otherwise
         */
        virtual bool sendPackage(const IPackage* package);

        /** 
         * Dispose the object
         */
        ~BluetoothAsSerial();

    //STATIC

        /**
         * Prepares an acknowledge package.
         * If specified, it include the message in the package.
         * 
         * @param[in]   package package to send the acknowledge for
         * @param[in]   message message to include into the acknowledge
         * @return  the acknowledge package
         */
        static IPackage* initAcknowledge(const IPackage* package, const IMessage* message = NULL);

        /**
         * Prepares an acknowledge package with block found message.
         * 
         * @param[in]   package     package to send the acknowledge for
         * @param[in]   robotPos    robot position
         * @param[in]   blockPos    block position
         * @return  the acknowledge package
         */
        static IPackage* initFoundAck(const IPackage* package, point robotPos, point blockPos);
        
        /**
         * Prepares an acknowledge package with block not found.
         * 
         * @param[in]   package     package to send the acknowledge for
         * @param[in]   robotPos    robot position
         * @return  the acknowledge package
         */
        static IPackage* initNotFoundAck(const IPackage* package, point robotPos);


    private:
        /** Last package sent to host */
        CommunicationPackage lastSent;
        /** Last package received from host */
        CommunicationPackage lastReceived;
        /** Boolean to know if one package has been received */
        bool receivedOnePackage;
        
    };
}

#endif /* BLUETOOTH_SERIAL_HPP_ */