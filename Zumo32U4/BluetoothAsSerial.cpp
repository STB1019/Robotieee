/**
 * @file    BluetoothAsSerial.cpp
 * 
 * @date    May 4, 2018
 * @author  paolopasqua
 */

#include "BluetoothAsSerial.hpp"
#include <Zumo32U4.h>

using namespace robo_utils;

namespace robotieee {
    /* frequency used by Serial1 */
    #define BAUD_RATE           9600
    /* it try ACK_WAIT_ATTEMPT times to receive the ack.. then re-send the package */
    #define ACK_WAIT_ATTEMPT    10


    BluetoothAsSerial::BluetoothAsSerial() {
        Serial1.begin(BAUD_RATE);

        receivedOnePackage = false;
    }

    bool BluetoothAsSerial::somethingToRead() {
        return Serial1.available() > 0;
    }

    bool BluetoothAsSerial::waitForPackage(IPackage* package, uint16_t timeout = DEFAULT_READ_TIMEOUT) {
        char buffer[MAX_PACKAGE_LENGTH];
        long startTime = millis();
        bool packageOk = false;
        uint16_t bufIndex = 0;
        uint8_t payloadLength = 0;

        //while timeout occurs or package is ok
        while (millis()-startTime < timeout && !packageOk) {
            //continue while there is something to read and the number of bytes read is less than the number of bytes to read
            while(somethingToRead() && bufIndex < PACKAGE_HEADER_BYTE_LENGTH + payloadLength) {
                buffer[bufIndex++] = Serial1.read();

                //if all the bytes of the header are read, init the payload length
                if (bufIndex == PACKAGE_HEADER_BYTE_LENGTH) {
                    payloadLength = CommunicationPackage::getPayloadLengthFromString(buffer);
                }
            }

            if (bufIndex == PACKAGE_HEADER_BYTE_LENGTH + payloadLength)
                packageOk = true;
        }

        //init the package from string
        packageOk = packageOk & CommunicationPackage::initFromString(package, buffer);

        //verify the package read
        packageOk = packageOk & package->verify(PROTOCOL_VERSION);

        //more checks
        if (packageOk && receivedOnePackage) {
            //if the package received has the same id of the last received, the ack has been lost. 
            //So it will re-send it (supposed in lastSent)
            if(package->getID() == lastReceived.getID()) {
                sendPackage(&lastSent);
                packageOk = false;
            }
        }

        //set first package read flag
        receivedOnePackage = packageOk || receivedOnePackage; 

        if (packageOk)
            lastReceived.clone(package);

        return packageOk;
    }

    bool BluetoothAsSerial::sendPackage(const IPackage* package) {
        IPackage* ack =  NULL;
        string<MAX_PACKAGE_LENGTH>* pkgStr = NULL;
        bool sentCorrectly = false;
        uint8_t i = 0;

        if (package == NULL)
            return false;

        ack = new CommunicationPackage();
        pkgStr = package->toString();

        while (!sentCorrectly) {
            //sends byte to byte the package 
            for(i = 0; i < pkgStr->getSize(); i++)
                Serial1.print(pkgStr->getBuffer()[i]);
            
            Serial1.flush(); //waits for all data is sent

            //wait for the ack if the package sent isn't an ack
            if (package->getType() != PACKAGE_TYPE_ACKNOWLEDGE) {
                //wait for the ack
                for(i = 0; i < ACK_WAIT_ATTEMPT && !waitForPackage(ack); i++);

                //verify the package received
                if (i < ACK_WAIT_ATTEMPT && ack->getType() == PACKAGE_TYPE_ACKNOWLEDGE && ack->getID() == package->getID()) {
                    sentCorrectly = true; //exit
                }
            }
        }

        lastSent.clone(package);

        delete(ack);
        delete(pkgStr);
        return true;
    }

    BluetoothAsSerial::~BluetoothAsSerial() {
        Serial1.end();
    }

    //STATIC

    static IPackage* BluetoothAsSerial::initAcknowledge(const IPackage* package, const IMessage* message = NULL) {
        IPackage* ack = NULL;

        if (package == NULL)
            return NULL;

        ack = new CommunicationPackage(PROTOCOL_VERSION, package->getID(), 0, PACKAGE_TYPE_ACKNOWLEDGE);

        if (message != NULL) {
            string<MAX_MESSAGE_LENGTH>* msgStr = message->toString();
            ack->setPayloadLength(msgStr->getSize());

            for(int i = 0; i < msgStr->getSize(); i++)
                ack->appendToPayload(msgStr->getBuffer()[i]);

            delete(msgStr);
        }

        return ack;
    }

    static IPackage* BluetoothAsSerial::initFoundAck(const IPackage* package, point robotPos, point blockPos) {
        point blocksPos[1] = { point(blockPos) };

        return initFoundAck(package, robotPos, 1, blocksPos);
    }

    static IPackage* BluetoothAsSerial::initFoundAck(const IPackage* package, point robotPos, int blocksNum, point* blocksPos) {
        compositeAction msg = compositeAction();
        string<ARGS_LENGHT> args = string<ARGS_LENGHT>{};

        msg.setType(MESSAGE_TYPE_FOUND);
        /*
        //0123456789abcdefghil...
        args.append(robotPos.x > 9 ? (char)('a' + robotPos.x-10) : (char)('0' + robotPos.x));
        //0123456789abcdefghil...
        args.append(robotPos.y > 9 ? (char)('a' + robotPos.y-10) : (char)('0' + robotPos.y));
        for(int i = 0; i< blocksNum; i++) {
            //0123456789abcdefghil...
            args.append(blocksPos[i].x > 9 ? (char)('a' + blocksPos[i].x-10) : (char)('0' + blocksPos[i].x));
            //0123456789abcdefghil...
            args.append(blocksPos[i].y > 9 ? (char)('a' + blocksPos[i].y-10) : (char)('0' + blocksPos[i].y));
        }
        */
        args.append((char)robotPos.x);
        args.append((char)robotPos.y);
        for(uint8_t i = 0; i< blocksNum; i++) {
            args.append((char)blocksPos[i].x);
            args.append((char)blocksPos[i].y);
        }

        msg.setArgs(args.getBuffer());

        return initAcknowledge(package, &msg);
    }
    
    static IPackage* BluetoothAsSerial::initNotFoundAck(const IPackage* package, point robotPos) {
        compositeAction msg = compositeAction();
        string<ARGS_LENGHT> args = string<ARGS_LENGHT>{};

        msg.setType(MESSAGE_TYPE_FOUND);
        /*
        //0123456789abcdefghil...
        args.append(robotPos.x > 9 ? (char)('a' + robotPos.x-10) : (char)('0' + robotPos.x));
        //0123456789abcdefghil...
        args.append(robotPos.y > 9 ? (char)('a' + robotPos.y-10) : (char)('0' + robotPos.y));
        */
        args.append((char)robotPos.x);
        args.append((char)robotPos.y);

        msg.setArgs(args.getBuffer());

        return initAcknowledge(package, &msg);
    }
}