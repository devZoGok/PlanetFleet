cmake_minimum_required(VERSION 3.5)

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

set(STATES activeGameState.cpp inGameAppState.cpp guiAppState.cpp mapEditorAppState.cpp loadingAppState.cpp)

set(UNIT_BUTTONS unitButton.cpp tradeButton.cpp buildButton.cpp trainButton.cpp researchButton.cpp orderButton.cpp stateToggleButton.cpp)
set(OPTIONS_BUTTONS optionsButton.cpp tabButton.cpp okButton.cpp defaultsButton.cpp)
set(TRADING_BUTTONS activeStateBackButton.cpp playerTradeButton.cpp tradingScreenButton.cpp offerButton.cpp resourceAmmountButton.cpp)
set(MENU_BUTTONS mainMenuButton.cpp singlePlayerButton.cpp exitButton.cpp backButton.cpp playButton.cpp)
set(MAP_EDITOR_BUTTONS mapEditorButton.cpp newMapButton.cpp loadMapButton.cpp exportButton.cpp)
set(BUTTONS statsButton.cpp activeStateButton.cpp minimapButton.cpp ${TRADING_BUTTONS} ${OPTIONS_BUTTONS} ${MENU_BUTTONS} ${MAP_EDITOR_BUTTONS} ${UNIT_BUTTONS})

set(LISTBOXES skyboxTextureListbox.cpp landTextureListbox.cpp gameObjectListbox.cpp mapListbox.cpp)
set(GUI tooltip.cpp concreteGuiManager.cpp ${BUTTONS} ${LISTBOXES})

set(CONTROLLERS gameObjectFrameController.cpp cameraController.cpp)
set(CONSOLE console.cpp abstractCommand.cpp addUnitCommand.cpp addResourceCommand.cpp addTechnologyCommand.cpp toggleDebugCommand.cpp)
set(CORE gameManager.cpp game.cpp pathfinder.cpp environment.cpp fxManager.cpp defConfigs.cpp player.cpp trader.cpp ${CONSOLE} ${CONTROLLERS})

set(PROJECTILES projectile.cpp missile.cpp cruiseMissile.cpp shell.cpp)
set(VEHICLES vehicle.cpp submarine.cpp engineer.cpp resourceRover.cpp freezer.cpp)
set(STRUCTURES structure.cpp factory.cpp pointDefense.cpp extractor.cpp researchStruct.cpp iceSheet.cpp)
set(WEAPONS weapon.cpp cryoGun.cpp)
set(UNITS unit.cpp ${WEAPONS} ${STRUCTURES} ${VEHICLES})
set(CONTENT map.cpp gameObject.cpp destructable.cpp gameObjectFrame.h gameObjectFactory.cpp resourceDeposit.cpp ${UNITS} ${PROJECTILES})

set(UTIL util.cpp binds.h)

set(GAME_SRC ${STATES} ${GUI} ${CORE} ${CONTENT} ${UTIL})

set(SFML_DIR external/gameBase/external/SFML)
set(VB01_DIR external/vb01)
set(VB01_GUI_DIR external/vb01Gui)
set(GAME_BASE_DIR external/gameBase)

add_executable(${GAME_NAME} main.cpp ${GAME_SRC})

include_directories(external/tinydir)
include_directories(external/vb01/external/glm)
include_directories(external/vb01/external/glm/glm)
include_directories(external/gameBase/external/SFML/include)

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
