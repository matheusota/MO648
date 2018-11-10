//
//  CTPBasic.hpp
//  graspCTP
//
//  Created by Ulysses Rocha on 09/08/2018.
//  Copyright © 2018 Ulysses Rocha. All rights reserved.
//
#pragma once
#include <stdio.h>
#include <ctime>
#include <ratio>
#include <chrono>



class CTPBasic {
private:
    std::chrono::time_point<std::chrono::system_clock> startTime;
protected:
    void startTimer();
    double secondsFromStart();
    double microsecondsFromStart();
    void resetTimer();
};
