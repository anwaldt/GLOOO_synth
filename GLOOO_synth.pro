TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    src/VoiceManager.cpp \
    src/Sinusoid.cpp \
    src/GlobalMath.cpp \
    src/SingleVoice.cpp \
    src/Trajectory.cpp \
    src/GloooConfig.cpp \
    src/LinearSlope.cpp \
    src/frameinterpolator.cpp \
    src/DeterministicData.cpp \
    src/StochasticData.cpp \
    src/OscManager.cpp \
    src/MidiManager.cpp \
    src/TextOutputManager.cpp \
    src/ResidualSynth.cpp \
    src/BarkCoefficients.cpp \
    src/IirFilter.cpp \
    src/StochasticSound.cpp \
    src/StatisticalParameter.cpp \
    src/ExponentialDecay.cpp \
    src/DeterministicSound.cpp \
    src/GloooSynth.cpp \
    src/DataSet.cpp \
    src/StochasticTester.cpp \
    src/main.cpp \
    src/HyperbolicSlope.cpp

HEADERS += \
    src/settings.h \
    src/VoiceManager.h \
    src/Sinusoid.h \
    src/GlobalMath.h \
    src/SingleVoice.h \
    src/glooo.h \
    src/Trajectory.h \
    src/GloooConfig.h \
    src/LinearSlope.h \
    src/frameinterpolator.h \
    src/DeterministicData.h \
    src/StochasticData.h \
    src/OscManager.h \
    src/MidiManager.h \
    src/TextOutputManager.h \
    src/ResidualSynth.h \
    src/BarkCoefficients.h \
    src/IirFilter.h \
    src/StochasticSound.h \
    src/StatisticalParameter.h \
    src/ExponentialDecay.h \
    src/DeterministicSound.h \
    src/GloooSynth.h \
    src/DataSet.h \
    src/StochasticTester.h \
    src/HyperbolicSlope.h

LIBS += -ljack -lyaml-cpp -llo -lrtmidi

DISTFILES += \
    setup/glooo_settings.yml
