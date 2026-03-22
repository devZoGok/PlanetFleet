cmake_minimum_required(VERSION 3.10)

set(GAME_NAME battleship)
project(${GAME_NAME})

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std:c++17")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pedantic")
endif()

set(CMAKE_CXX_STANDART 17)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_BUILD_TYPE Debug)
set(BUILD_TESTS OFF)
set(BUILD_SHARED_LIBS OFF CACHE BOOL FORCED)
cmake_policy(SET CMP0015 NEW)

set(STATES src/Core/activeGameState.cpp src/Core/inGameAppState.cpp src/Core/guiAppState.cpp src/Core/mapEditorAppState.cpp src/Core/loadingAppState.cpp)

set(UNIT_BUTTONS src/UI/unitButton.cpp src/UI/tradeButton.cpp src/UI/buildButton.cpp src/UI/trainButton.cpp src/UI/researchButton.cpp src/UI/orderButton.cpp src/UI/stateToggleButton.cpp) 
set(OPTIONS_BUTTONS src/UI/optionsButton.cpp src/UI/tabButton.cpp src/UI/okButton.cpp src/UI/defaultsButton.cpp) 
set(TRADING_BUTTONS src/UI/activeStateBackButton.cpp src/UI/playerTradeButton.cpp src/UI/tradingScreenButton.cpp src/UI/offerButton.cpp src/UI/resourceAmmountButton.cpp) 
set(MENU_BUTTONS src/UI/mainMenuButton.cpp src/UI/singlePlayerButton.cpp src/UI/exitButton.cpp src/UI/backButton.cpp src/UI/playButton.cpp) 
set(MAP_EDITOR_BUTTONS src/UI/mapEditorButton.cpp src/UI/newMapButton.cpp src/UI/loadMapButton.cpp src/UI/exportButton.cpp) 
set(BUTTONS src/UI/statsButton.cpp src/UI/activeStateButton.cpp src/UI/minimapButton.cpp ${TRADING_BUTTONS} ${OPTIONS_BUTTONS} ${MENU_BUTTONS} ${MAP_EDITOR_BUTTONS} ${UNIT_BUTTONS})

set(LISTBOXES src/UI/skyboxTextureListbox.cpp src/UI/landTextureListbox.cpp src/UI/gameObjectListbox.cpp src/UI/mapListbox.cpp) 
set(GUI src/UI/tooltip.cpp src/Core/concreteGuiManager.cpp ${BUTTONS} ${LISTBOXES})

set(CONTROLLERS src/UI/gameObjectFrameController.cpp src/Map/cameraController.cpp) 
set(CONSOLE src/Core/console.cpp src/Commands/abstractCommand.cpp src/Commands/addUnitCommand.cpp src/Commands/addResourceCommand.cpp src/Commands/addTechnologyCommand.cpp src/Commands/toggleDebugCommand.cpp) 
set(CORE src/Core/gameManager.cpp src/Core/game.cpp src/Core/pathfinder.cpp src/Core/environment.cpp src/Core/fxManager.cpp src/Core/defConfigs.cpp src/Core/player.cpp src/Core/trader.cpp ${CONSOLE} ${CONTROLLERS})

set(PROJECTILES src/Entities/projectile.cpp src/Entities/missile.cpp src/Entities/cruiseMissile.cpp src/Entities/shell.cpp src/Entities/depthCharge.cpp src/Entities/torpedo.cpp src/Entities/torpedo.h) 
set(VEHICLES src/Entities/vehicle.cpp src/Entities/submarine.cpp src/Entities/engineer.cpp src/Entities/resourceRover.cpp src/Entities/freezer.cpp) 
set(STRUCTURES src/Entities/structure.cpp src/Entities/factory.cpp src/Entities/pointDefense.cpp src/Entities/extractor.cpp src/Entities/researchStruct.cpp src/Entities/iceSheet.cpp) 
set(WEAPONS src/Entities/weapon.cpp) 
set(UNITS src/Entities/unit.cpp ${WEAPONS} ${STRUCTURES} ${VEHICLES}) 
set(CONTENT src/Map/map.cpp src/Entities/gameObject.cpp src/EntityComponents/destructable.cpp src/UI/gameObjectFrame.h src/Entities/gameObjectFactory.cpp src/Entities/resourceDeposit.cpp ${UNITS} ${PROJECTILES})

set(UTIL src/Core/util.cpp src/Core/binds.h)
set(GAME_SRC ${STATES} ${GUI} ${CORE} ${CONTENT} ${UTIL})

set(SFML_DIR external/gameBase/external/SFML)
set(VB01_DIR external/vb01)
set(VB01_GUI_DIR external/vb01Gui)
set(GAME_BASE_DIR external/gameBase)

add_executable(${GAME_NAME} src/Core/main.cpp ${GAME_SRC})

include_directories(external/tinydir)
include_directories(external/vb01/external/glm)
include_directories(external/vb01/external/glm/glm)
include_directories(external/gameBase/external/SFML/include)

target_include_directories(${GAME_NAME} PRIVATE 
    src/Core 
    src/Entities 
    src/EntityComponents 
    src/UI 
    src/Commands 
    src/Map
)

add_subdirectory(${VB01_DIR})
set(VB01_LIB_DIR ${VB01_DIR}/build)
target_include_directories(${GAME_NAME} PUBLIC ${VB01_DIR})
target_link_directories(${GAME_NAME} PUBLIC ${VB01_LIB_DIR})

add_subdirectory(${GAME_BASE_DIR})
set(GAME_BASE_LIB_DIR build/${GAME_BASE_DIR}/src)
target_include_directories(${GAME_NAME} PUBLIC ${GAME_BASE_DIR})
target_link_directories(${GAME_NAME} PUBLIC ${GAME_BASE_LIB_DIR})

set(DEPS vb01 gameBase)
target_link_libraries(${GAME_NAME} ${DEPS})

if(BUILD_TESTS)
	if(UNIX)
		include_directories(/usr/include/cppunit)
		link_directories(/usr/lib)
	elseif(WIN32)
		include_directories(C:/Program\ Files\ \(x86\)/cppunit/include)
		link_directories(C:/Program\ Files\ \(x86\)/cppunit/lib)
	endif()
	
	set(TEST_SRC testMain.cpp pathfinderTest.cpp ${GAME_SRC})
	set(TEST_NAME battleshipTests)
	
	add_executable(${TEST_NAME} ${TEST_SRC})
	target_include_directories(${TEST_NAME} PUBLIC ${VB01_DIR})
	target_include_directories(${TEST_NAME} PUBLIC ${VB01_GUI_DIR})
	target_include_directories(${TEST_NAME} PUBLIC ${GAME_BASE_DIR})
	target_link_libraries(${TEST_NAME} ${DEPS} cppunit)
endif()
