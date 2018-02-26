//
// Created by tom on 26.02.18.
//

#ifndef BOTERKAASAI_UTILS_H
#define BOTERKAASAI_UTILS_H



class Utils{

public:
    static bool askUserForBool(std::string message);
    static int askUserForInt(std::string message, int min, int max);

};

#endif //BOTERKAASAI_UTILS_H
