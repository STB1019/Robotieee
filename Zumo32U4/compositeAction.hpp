 /**
 * @file
 * 
 * API specifying a class rapresenting an action of the robot
 *
 * @date Apr 12, 2018
 */

#ifndef C_ACTION_HPP_
#define C_ACTION_HPP_

#include <Zumo32U4.h>
#include "IMessage.hpp"

namespace robotieee {

    /**
     * Represents an action that the robot has to do.
     * Composite means an action with repetitions: the equal actions on series are write as one action with n repetitions.
     */
    class compositeAction : public IMessage {

    public:
        /**
         * Initializes the action.
         */
        compositeAction();
        
        /**
         * Set the type of the action.
         * 
         * @param[in]   pType   action's type
         */
        void setType(char pType);

        /**
         * Get the type of the action.
         * 
         * @return action's type
         */
        char getType();

        /**
         * Set the args of the action.
         * 
         * @param[in]   pArgs   action's args
         */
        void setArgs(char* pArgs);

        /**
         * Get the args of the action.
         * 
         * @return  action's args
         */
        char* getArgs();

        /**
         * Add a repetition on the action.
         * 
         * @param[in]   num number of repetition to add
         */
        void addRepetition(uint8_t num = 1);

        /**
         * Set a number of repetition on the action.
         * 
         * @param[in] num number of repetition to set
         */
        void setRepetition(uint8_t num = 1);
        
        /**
         * Get the repetition of the action
         * 
         * @return  action's repetition
         */
        uint8_t getRepetition();

        
        /**
         * Returns the message data coded into a string.
         * NB: the string has to be deallocated after the use.
         * 
         * @return  the message data in string form
         */
        string<MAX_MESSAGE_LENGTH>* toString();

        /**
         * dispose the action
         */
        ~compositeAction();

    private:
        uint8_t _repetition;

    };

}

#endif /* C_ACTION_HPP_ */
