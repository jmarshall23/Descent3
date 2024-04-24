CMAKE_MINIMUM_REQUIRED( VERSION 3.20 )

project(D3GameScript)

# Definitions
add_definitions(-DCHECKSUM=2273873307UL)

# Source files
set(HEADERS )
set(CPPS )

# List of scripts
set(SCRIPTS
      aigame2
    AIGame3
    aigame4
    AIGame
    barney
    BatteriesIncluded
    BossCamera
    CanyonsCTF
    CellTestLevel
    ChrisTest
    clutter
    GameGauge
    generic
    Geodomes
    HalfPipe
    InfernalBolt
    Inversion
    LEVEL0
    level10
    level11
    Level12
    level13
    level14
    LEVEL15
    Level16
    level17
    level1
    level2
    level3
    level4
    level5
    Level6
    level7
    level8
    Level9
    LevelS1
    levelS2
    Merc02
    Merc1
    Merc3
    Merc4
    merc5
    Merc6
    Merc7
    myPowerHouse
    Mysterious_Isle
    orbital
    Paranoia
    PiccuStation
    Polaris
    Quadsomniac
    RudeAwakening
    SewerRat
    testscript
    TrainingMission
    Y2K
)

set(SCRIPTTARGETS )

# Create shared libraries for each script
foreach(SCRIPT ${SCRIPTS})
  add_library(${SCRIPT} SHARED ${CPPS} "${SCRIPT}.cpp")
  set_target_properties(${SCRIPT} PROPERTIES PREFIX "")
  if(WIN32)
    set_target_properties(${SCRIPT} PROPERTIES SUFFIX ".dll")
    set_target_properties(${SCRIPT} PROPERTIES OUTPUT_NAME "${SCRIPT}")
  endif()
  if(WIN32)
    target_sources(${SCRIPT} PRIVATE "exports.def")  # Link .def file
  endif()
  
    set_target_properties(${SCRIPT} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../../scripts/
    )


  
  list(APPEND SCRIPTTARGETS ${SCRIPT})
endforeach()