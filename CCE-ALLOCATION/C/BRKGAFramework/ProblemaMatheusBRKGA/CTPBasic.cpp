//
//  CTPBasic.cpp
//  graspCTP
//
//  Created by Ulysses Rocha on 09/08/2018.
//  Copyright © 2018 Ulysses Rocha. All rights reserved.
//

#include "CTPBasic.hpp"

void CTPBasic::startTimer() {
    startTime = std::chrono::system_clock::now();
}
double CTPBasic::secondsFromStart(){
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
    return seconds.count();
}
double CTPBasic::microsecondsFromStart(){
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime);
    return microseconds.count();
}

void CTPBasic::resetTimer() {
    startTimer();
}
