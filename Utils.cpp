//
// Created by tom on 26.02.18.
//

#include <iostream>
#include "Utils.h"





bool Utils::askUserForBool(std::string message) {
    while (true){
        std::cout << message << std::endl;
        std::cout << "Please type y or n."<< std::endl;

        char yesOrNo=0;
        std::cin >> yesOrNo;

        if (yesOrNo== 'n' || yesOrNo == 'N'){
            return false;
        }
        else if (yesOrNo== 'y' || yesOrNo == 'Y'){
            return true;
        }
        std::cout << "Invalid input" << std::endl;
    }
}

int Utils::askUserForInt(std::string message, int min, int max) {
    while (true){
        std::cout << message << std::endl;
        std::cout << "Please type an integer between ."<< min << " and " << max << std::endl;

        int  result =0;
        std::cin >> result;

        if (result>=min && result <= max){
            return result;
        }
        std::cout << "Invalid input" << std::endl;
    }
}







