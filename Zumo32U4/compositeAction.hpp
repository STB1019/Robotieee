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

namespace robotieee {

    #define HEADER_TYPE_MOVE        'M'
    #define HEADER_TYPE_STATE       'S'
    #define HEADER_TYPE_END         'E'
    #define HEADER_TYPE_LOCATION    'L'
    #define HEADER_TYPE_WARNING     'W'
    #define HEADER_TYPE_DONE        'D'

    #define ARGS_LENGHT             5

    /**
     * Represents an action that the robot has to do.
     * Composite means an action with repetitions: the equal actions on series are write as one action with n repetitions.
     */
    class compositeAction {

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
         * dispose the action
         */
        ~compositeAction();

    private:
        char _type;
        char _args[ARGS_LENGHT+1];
        uint8_t _repetition;

    };

}

#endif
