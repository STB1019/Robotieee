/**
 * @file    ICommunicator.hpp
 * 
 * Interface for the communication management between robot and host.
 * Suitable for different protocol version implementations.
 * 
 * @date    May 4, 2018
 * @author  paolopasqua
 */

#ifndef INTERFACE_COMMUNICATOR_HPP_
#define INTERFACE_COMMUNICATOR_HPP_

#include "IPackage.hpp"

namespace robotieee {
    
    /* Default value for read timeout in millis */
    #define DEFAULT_READ_TIMEOUT     10000 //10 seconds

    /**
     * Represents interface for the communication management between robot and host.
     * Suitable for implementations of different protocol version.
     */
    class ICommunicator {
    public:

        ICommunicator() {}
        virtual ~ICommunicator() {}

        /**
         * Checks if there's something to read.
         * 
         * @return  true: somenthing to read in buffer; false: otherwise
         */
        virtual bool somethingToRead() = 0;

        /**
         * Waits until it can read a package. If timeout occurs, the fuction will return false and the package parameter will not initialized.
         * 
         * @param[out]  package package to fill up
         * @param[in]   timeout timeout in millis for the package reading
         * @return  true: package read correctly; false: otherwise
         */
        virtual bool waitForPackage(IPackage* package, uint16_t timeout = DEFAULT_READ_TIMEOUT) = 0;

        /**
         * Sends an acknowledge package.
         * If specified, it include the message in the package.
         * 
         * @param[in]   package package to send the acknowledge for
         * @param[in]   message message to include into the acknowledge
         */
        //virtual void sendAcknowledge(const IPackage* package, const IMessage* message = NULL) = 0;

        /**
         * Sends a package.
         * 
         * @param[in]   package package to send
         * @return  true: the package sent correctly; false: otherwise
         */
        virtual bool sendPackage(IPackage* package) = 0;

    private:

    };
}

#endif /* INTERFACE_COMMUNICATOR_HPP_ */