/**
 * @file    CommunicationPackage.cpp
 * 
 * @date    May 4, 2018
 * @author  paolopasqua
 */

#include "CommunicationPackage.hpp"

using namespace robo_utils;

namespace robotieee {

    #define ID_INT_BITMASK                      0x1         //00000001
    #define ID_APPLICATION_ZERO_BITMASK         0xFFFE      //11111111 11111110
    #define ID_APPLICATION_ONE_BITMASK          0x0001      //00000000 00000001
    #define VERSION_APPLICATION_ZERO_BITMASK    0xF0FF      /*11110000 11111111*/
    #define VERSION_APPLICATION_ONE_BITMASK     0xF00       /*00001111 00000000*/

    CommunicationPackage::CommunicationPackage(uint8_t version, uint8_t id, uint8_t payloadLength, char type) : CommunicationPackage() {
        setVersion(version);
        setID(id);
        setPayloadLength(payloadLength);
        setType(type);
    }
    
    CommunicationPackage::CommunicationPackage() {
        _version = 0;
        _flags = 0;
        _payloadLength = 0;
        _type = '\0';
        _payload = new string<MAX_PAYLOAD_LENGTH>{};
    }

    void CommunicationPackage::setVersion(uint8_t version) {
        _version = version;
    }

    uint8_t CommunicationPackage::getVersion() {
        return _version;
    }
    
    void CommunicationPackage::setID(uint8_t id) {
        setIDFromInt(id);
    }

    uint8_t CommunicationPackage::getID() {
        return getIDAsInt();
    }
    
    void CommunicationPackage::setPayloadLength(uint8_t payloadLength) {
        _payloadLength = payloadLength;
    }
    
    uint8_t CommunicationPackage::getPayloadLength() {
        return _payloadLength;
    }

    void CommunicationPackage::setType(char type) {
        _type = type;
    }
    
    char CommunicationPackage::getType() {
        return _type;
    }
    
    void CommunicationPackage::setPayload(string<MAX_PAYLOAD_LENGTH>* payload) {
        if (payload == nullptr)
            return;

        delete(_payload);
        _payload = new string<MAX_PAYLOAD_LENGTH>{};
        for(int i = 0; i < payload->getSize(); i++)
            _payload->append(payload->getBuffer()[i]);
    }

    bool CommunicationPackage::appendToPayload(char data) {
        return _payload->append(data);
    }

    string<MAX_PAYLOAD_LENGTH>* CommunicationPackage::getPayload() {
        return _payload;
    }
    
    void CommunicationPackage::getPayloadAsMessage(IMessage* payload) {
        payload->setType(_payload->getBuffer()[0]);
        payload->setArgs((char*)_payload->getBuffer() + 1);
    }

    void CommunicationPackage::clone(IPackage* package) {
        setVersion(package->getVersion());
        setID(package->getID());
        setPayloadLength(package->getPayloadLength());
        setType(package->getType());

        setPayload(package->getPayload());
    }
    
    bool CommunicationPackage::verify(uint8_t protocolVersion /* = NO_VERIFY_VERSION_VALUE */) {
        //Check the version
        if (protocolVersion != NO_VERIFY_VERSION_VALUE && getVersion() != protocolVersion)
            return false;

        //Check the id
        if (getID() != 0 && getID() != 1)
            return false;

        //Check the type
        switch(getType()) {
            case PACKAGE_TYPE_ACKNOWLEDGE:
            case PACKAGE_TYPE_COMMUNICATION:
            case PACKAGE_TYPE_INSTRUCTION:
                    break;
            
            default:
                    return false;
        }

        //Check the payload length
        if (getPayloadLength() != getPayload()->getSize())
            return false;

        return true;
    }

    bool CommunicationPackage::equals(IPackage* package) {
        if (package == nullptr)
            return false;
        
        return package->getID() == getID();
    }

    string<MAX_PACKAGE_LENGTH>* CommunicationPackage::toString() {
        string<MAX_PACKAGE_LENGTH>* data = new string<MAX_PACKAGE_LENGTH>{};
        uint8_t byteToAppend = 0;
        uint16_t tmpMask = _flags;

        byteToAppend = getVersion()<<(8/*bit in a byte*/-VERSION_BIT_LENGTH);
        //Applaing flags if value is 1
        tmpMask = _flags;
        tmpMask &= VERSION_APPLICATION_ONE_BITMASK;
        tmpMask >>= 8/*bit in a byte*/;
        byteToAppend |= (uint8_t)(tmpMask);
        //Applaing flags if value is 0
        tmpMask = _flags;
        tmpMask |= VERSION_APPLICATION_ZERO_BITMASK;
        tmpMask >>= 8/*bit in a byte*/;
        byteToAppend &= (uint8_t)(tmpMask);

        data->append((char)byteToAppend);

        byteToAppend = (uint8_t)_flags;
        data->append((char)byteToAppend);

        data->append((char)_payloadLength);
        data->append(_type);

        //data->append(&_payload);
        for(int i = 0; i < _payload->getSize(); i++)
            data->append(_payload->getBuffer()[i]);

        return data;
    }
    
    CommunicationPackage::~CommunicationPackage() {
        delete(_payload);
    }

    /* STATIC */

    bool CommunicationPackage::initFromString (IPackage* package, char* str) {

        if (package == nullptr)
            return false;

        uint8_t tmpByte = 0;
        char tmpType;
        string<MAX_PAYLOAD_LENGTH>* tmpPayload = nullptr;

        tmpByte = getVersionFromString(str);
        if (tmpByte == STRING_PARSE_VERSION_ERROR)
            return false;
        package->setVersion(tmpByte);

        tmpByte = getIDFromString(str);
        if (tmpByte == STRING_PARSE_ID_ERROR)
            return false;
        package->setID(tmpByte);
        
        tmpByte = getPayloadLengthFromString(str);
        if (tmpByte == STRING_PARSE_PAYLOADLENGHT_ERROR)
            return false;
        package->setPayloadLength(tmpByte);

        tmpType = getTypeFromString(str);
        if (tmpType == STRING_PARSE_TYPE_ERROR)
            return false;
        package->setType(tmpType);

        tmpPayload = getPayloadFromString(str);
        if (tmpPayload == STRING_PARSE_PAYLOAD_ERROR)
            return false;
        package->setPayload(tmpPayload);
        delete(tmpPayload);
    }

    uint8_t CommunicationPackage::getVersionFromString(char* str) {
        if (str != nullptr) {
            return ((uint8_t)str[0])>>(8/*bit in a byte*/-VERSION_BIT_LENGTH);
        } else {
            return STRING_PARSE_VERSION_ERROR;
        }
    }

    uint8_t CommunicationPackage::getIDFromString(char* str) {
        uint16_t flags = ((uint8_t)str[0])<<8/*bit in a byte*/ | ((uint8_t)str[1]); //get the first 2 byte
        
        if (str != nullptr) {
            return (flags>>(FLAG_ID_POSITION-1)) & ID_INT_BITMASK;
        } else {
            return STRING_PARSE_ID_ERROR;
        }
    }

    uint8_t CommunicationPackage::getPayloadLengthFromString(char* str) {
        if (str != nullptr) {
            return ((uint8_t)str[2]);
        } else {
            return STRING_PARSE_PAYLOADLENGHT_ERROR;
        }
    }

    char CommunicationPackage::getTypeFromString(char* str) {
        if (str != nullptr) {
            return str[3];
        } else {
            return STRING_PARSE_TYPE_ERROR;
        }
    }

    string<MAX_PAYLOAD_LENGTH>* CommunicationPackage::getPayloadFromString(char* str) {
        uint8_t payloadLength = getPayloadLengthFromString(str);

        if (payloadLength != -1) {
            string<MAX_PAYLOAD_LENGTH>* res = new string<MAX_PAYLOAD_LENGTH>{};
            int i = 0;
            
            for(i = PACKAGE_HEADER_BYTE_LENGTH; i < payloadLength + PACKAGE_HEADER_BYTE_LENGTH; i++)
                res->append(str[i]);
            
            return res;
        }
        
        return STRING_PARSE_PAYLOAD_ERROR;
    }

    /* PRIVATE */

    void CommunicationPackage::setIDFromInt(uint8_t id) {
        uint16_t settingMask = 0;
        uint16_t idMask = id;
        
        /*Example: 
        id start from yyyyyyyx,
        bitmask: 0000000x
        */
        idMask &= ID_INT_BITMASK;       //Select only the rigth first bit of the integer

        settingMask = ID_APPLICATION_ZERO_BITMASK;   //11111111 11111110
        idMask |= settingMask;  //in this way the correct bit will be set with an & operation if it's 0
        //in the example: 11111111 1111111x
        _flags &= idMask;       //Set the correct bit

        settingMask = ID_APPLICATION_ONE_BITMASK;   //00000000 00000001
        idMask &= settingMask;  //in this way the correct bit will be set with an | operation if it's 1
        //in the example: 00000000 x0000000
        _flags |= idMask;       //Set the correct bit
    }

    uint8_t CommunicationPackage::getIDAsInt() {
        uint16_t idMask = _flags;

        idMask >>= FLAG_ID_POSITION-1;  //Shift the id to the first right bit
        idMask &= ID_INT_BITMASK;       //Select only the first right bit of the integer

        return idMask;
    }
}