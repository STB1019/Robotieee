/*
    compositeAction.cpp

    Created on: Apr 11, 2018
*/
#include "compositeAction.hpp"

namespace robotieee {

    compositeAction::compositeAction() {
        _type = '\0';
        for(uint8_t i = 0; i < ARGS_LENGHT+1; i++)
            _args[i] = '\0';
        _repetition = 0;
    }

    void compositeAction::setType(char pType) {
        _type = pType;
    }
    
    char compositeAction::getType() {
        return _type;
    }

    void compositeAction::setArgs(char* pArgs) {
        for(uint8_t i = 0; i < ARGS_LENGHT; i++)
            _args[i] = pArgs[i];
    }

    char* compositeAction::getArgs() {
        return _args;
    }

    void compositeAction::addRepetition(uint8_t num /* = 1 */) {
        _repetition += num;
    }

    void compositeAction::setRepetition(uint8_t num /* = 1 */) {
        _repetition = num;
    }
    
    uint8_t compositeAction::getRepetition() {
        return _repetition;
    }

    string<MAX_MESSAGE_LENGTH>* compositeAction::toString() {
        string<MAX_MESSAGE_LENGTH>* data = new string<MAX_MESSAGE_LENGTH>{};
        
        data->append(_type);
        data->append(_args);

        return data;
    }

    compositeAction::~compositeAction() {

    }

}
