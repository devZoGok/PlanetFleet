cmake_minimum_required(VERSION 3.5)

set(GAME_NAME planetFleet)
project(${GAME_NAME})

option(BUILD_GAME_TESTS "Build game unit tests" OFF)

set(STATES
	source/core/appStates/activeGameState.cpp
    source/core/appStates/inGameAppState.cpp
    source/core/appStates/guiAppState.cpp
    source/core/appStates/mapEditorAppState.cpp
    source/core/appStates/loadingAppState.cpp)

set(CONSOLE
    source/core/console/console.cpp
    source/core/console/abstractCommand.cpp
    source/core/console/addUnitCommand.cpp
    source/core/console/addResourceCommand.cpp
    source/core/console/addTechnologyCommand.cpp
    source/core/console/toggleDebugCommand.cpp)

set(GAME_SRC
    source/core/game/game.cpp
    source/core/game/gameManager.cpp)

set(
	CORE_SRC
    source/core/pathfinding/pathfinder.cpp
    source/utils/util.cpp
	source/Core/defConfigs.cpp
    source/core/controllers/cameraController.cpp
	source/core/controllers/gameObjectFrameController.cpp)
	${STATES} ${CONSOLE} ${PATHFINDING_SRC} ${UTILS_SRC} ${GAME}

set(UNIT_BUTTONS
    source/ui/buttons/unitButton.cpp
    source/ui/buttons/tradeButton.cpp
    source/ui/buttons/buildButton.cpp
    source/ui/buttons/trainButton.cpp
    source/ui/buttons/researchButton.cpp
    source/ui/buttons/orderButton.cpp
    source/ui/buttons/stateToggleButton.cpp)

set(OPTIONS_BUTTONS
    source/ui/buttons/optionsButton.cpp
    source/ui/buttons/tabButton.cpp
    source/ui/buttons/okButton.cpp
    source/ui/buttons/defaultsButton.cpp)

set(TRADING_BUTTONS
    source/ui/buttons/activeStateBackButton.cpp
    source/ui/buttons/playerTradeButton.cpp
    source/ui/buttons/tradingScreenButton.cpp
    source/ui/buttons/offerButton.cpp
    source/ui/buttons/resourceAmmountButton.cpp)

set(MENU_BUTTONS
    source/ui/buttons/mainMenuButton.cpp
    source/ui/buttons/singlePlayerButton.cpp
    source/ui/buttons/exitButton.cpp
    source/ui/buttons/backButton.cpp
    source/ui/buttons/playButton.cpp)

set(MAP_EDITOR_BUTTONS
    source/ui/buttons/mapEditorButton.cpp
    source/ui/buttons/newMapButton.cpp
    source/ui/buttons/loadMapButton.cpp
    source/ui/buttons/exportButton.cpp)

set(BUTTONS
    source/ui/buttons/statsButton.cpp
    source/ui/buttons/activeStateButton.cpp
    source/ui/buttons/minimapButton.cpp
    ${TRADING_BUTTONS} ${OPTIONS_BUTTONS} ${MENU_BUTTONS}
    ${MAP_EDITOR_BUTTONS} ${UNIT_BUTTONS})

set(LISTBOXES
    source/ui/listboxes/skyboxTextureListbox.cpp
    source/ui/listboxes/landTextureListbox.cpp
    source/ui/listboxes/gameObjectListbox.cpp
    source/ui/listboxes/mapListbox.cpp)

set(GUI_SRC
    source/gui/tooltip.cpp
    source/gui/concreteGuiManager.cpp
    source/gui/gameObjectFrame.cpp
    ${BUTTONS} ${LISTBOXES})

set(ENVIRONMENT_SRC
    source/gameplay/Private/environment.cpp
    source/gameplay/Private/fxManager.cpp
    source/gameplay/Private/explosion.cpp
    source/gameplay/Private/map.cpp
)

set(PLAYER_SRC
    source/gameplay/Private/player.cpp
    source/gameplay/Private/trader.cpp
)

set(PROJECTILES
    source/gameplay/projectile.cpp
    source/gameplay/missile.cpp
    source/gameplay/cruiseMissile.cpp
    source/gameplay/shell.cpp
    source/gameplay/depthCharge.cpp
    source/gameplay/torpedo.cpp)

set(VEHICLES
    source/gameplay/vehicle.cpp
    source/gameplay/submarine.cpp
    source/gameplay/engineer.cpp
    source/gameplay/resourceRover.cpp
    source/gameplay/freezer.cpp
    source/gameplay/vessel.cpp)

set(STRUCTURES
    source/gameplay/structure.cpp
    source/gameplay/factory.cpp
    source/gameplay/pointDefense.cpp
    source/gameplay/extractor.cpp
    source/gameplay/researchStruct.cpp
    source/gameplay/iceSheet.cpp)

set(UNITS_SRC
    source/gameplay/unit.cpp
    source/gameplay/garrisonable.cpp
	${VEHICLES} ${STRUCTURES}
)

set(GAME_OBJECT_SRC
    source/gameplay/Private/resourceDeposit.cpp
	${UNITS} ${PROJECTILES}
)

set(GAMEPLAY_SRC
    source/gameplay/Private/gameObject.cpp
    source/gameplay/Private/gameObjectFactory.cpp
    source/gameplay/Private/tradeCenter.cpp
    source/gameplay/Private/destructable.cpp
    source/gameplay/Private/weapon.cpp
	${ENVIRONMENT_SRC} ${PLAYER_SRC} ${GAME_OBJECT_SRC})


set(GAME_SRC ${CORE_SRC} ${GUI_SRC} ${GAMEPLAY_SRC})

add_executable(${GAME_NAME} source/core/main.cpp ${GAME_SRC})

target_include_directories(${GAME_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source/core
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gameplay
    ${CMAKE_CURRENT_SOURCE_DIR}/source/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/source/utils)

target_link_libraries(${GAME_NAME} PRIVATE vb01 gameBase)

if(BUILD_GAME_TESTS)
    if(UNIX)
        include_directories(/usr/include/cppunit)
        link_directories(/usr/lib)
    elseif(WIN32)
        include_directories("C:/Program Files (x86)/cppunit/include")
        link_directories("C:/Program Files (x86)/cppunit/lib")
    endif()

    set(TEST_SRC
        tests/testMain.cpp
        tests/pathfinderTest.cpp
        ${GAME_SRC})

    add_executable(battleshipTests ${TEST_SRC})
	target_include_directories(planetFleetTests PRIVATE
    	${CMAKE_CURRENT_SOURCE_DIR}/source/core
    	${CMAKE_CURRENT_SOURCE_DIR}/source/gameplay
    	${CMAKE_CURRENT_SOURCE_DIR}/source/gui
    	${CMAKE_CURRENT_SOURCE_DIR}/source/utils)
        ${CMAKE_CURRENT_SOURCE_DIR}/source/tests)
    target_link_libraries(planetFleetTests PRIVATE vb01 gameBase cppunit)
endif()
