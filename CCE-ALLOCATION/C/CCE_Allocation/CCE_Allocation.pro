TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    filereader.cpp \
    user.cpp \
    heuristic.cpp \
    measures.cpp \
    segtree.cpp \
    brkgaheuristic.cpp \
    decoder.cpp \
    population.cpp \
    schedulingmodel.cpp \
    simulator.cpp \
    besteffort.cpp \
    bruteforce.cpp \
    sppmodel.cpp \
    reshuffle.cpp \
    schedulingmodel2.cpp \
    cutscallback.cpp \
    bestmodel.cpp

HEADERS += \
    filereader.h \
    user.h \
    main.h \
    heuristic.h \
    measures.h \
    segtree.h \
    brkga.h \
    population.h \
    mtrand.h \
    brkgaheuristic.h \
    decoder.h \
    schedulingmodel.h \
    besteffort.h \
    bruteforce.h \
    simulator.h \
    params.h \
    sppmodel.h \
    reshuffle.h \
    schedulingmodel2.h \
    cutscallback.h \
    bestmodel.h

DISTFILES +=
