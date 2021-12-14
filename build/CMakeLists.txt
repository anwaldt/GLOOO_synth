message("generator is set to ${CMAKE_GENERATOR}")

cmake_minimum_required(VERSION 3.10)
project(glooo VERSION 0.1.0)

set(CMAKE_INSTALL_PREFIX "/usr/local/bin")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELEASE)
endif()

# set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

message("COMPILER: ${CMAKE_CXX_COMPILER_ID}")
message("CURRENT_SRC_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message("BUILD_DIR: ${CMAKE_BINARY_DIR}")
message("Build type: ${CMAKE_BUILD_TYPE}")

Set(sources
        ../src/main.cpp
        ../src/VoiceManager.cpp 
        ../src/Sinusoid.cpp 
        ../src/GlobalMath.cpp 
        ../src/SingleVoice.cpp 
        ../src/Trajectory.cpp 
        ../src/GloooConfig.cpp 
        ../src/LinearSlope.cpp 
        ../src/frameinterpolator.cpp
        ../src/DeterministicData.cpp
        ../src/StochasticData.cpp 
        ../src/OscManager.cpp 
        ../src/MidiManager.cpp
        ../src/TextOutputManager.cpp
        ../src/ResidualSynth.cpp
        ../src/BarkCoefficients.cpp
        ../src/IirFilter.cpp
        ../src/StochasticSound.cpp
        ../src/StatisticalParameter.cpp
        ../src/ExponentialDecay.cpp
        ../src/DeterministicSound.cpp
        ../src/GloooSynth.cpp
        ../src/DataSet.cpp
        ../src/StochasticTester.cpp
        ../src/HyperbolicSlope.cpp
        ../src/settings.h
        ../src/VoiceManager.h
        ../src/Sinusoid.h
        ../src/GlobalMath.h
        ../src/SingleVoice.h
        ../src/glooo.h
        ../src/Trajectory.h
        ../src/GloooConfig.h
        ../src/LinearSlope.h
        ../src/frameinterpolator.h
        ../src/DeterministicData.h
        ../src/StochasticData.h
        ../src/OscManager.h
        ../src/TextOutputManager.h 
        ../src/ResidualSynth.h
        ../src/BarkCoefficients.h 
        ../src/IirFilter.h 
        ../src/StochasticSound.h 
        ../src/StatisticalParameter.h 
        ../src/ExponentialDecay.h 
        ../src/DeterministicSound.h 
        ../src/GloooSynth.h 
        ../src/DataSet.h 
        ../src/StochasticTester.h
        ../src/HyperbolicSlope.h
)

add_executable(GLOOO_synth ${sources})


target_link_libraries(GLOOO_synth PRIVATE jack yaml-cpp lo rtmidi)

install(TARGETS GLOOO_synth DESTINATION /usr/local/bin)
