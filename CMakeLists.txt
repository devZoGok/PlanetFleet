cmake_minimum_required(VERSION 3.5)

set(GAME_NAME battleship)
project(${GAME_NAME})

option(BUILD_GAME_TESTS "Build game unit tests" OFF)

set(STATES
    Source/Systems/Private/activeGameState.cpp
    Source/Systems/Private/inGameAppState.cpp
    Source/Systems/Private/guiAppState.cpp
    Source/Systems/Private/mapEditorAppState.cpp
    Source/Systems/Private/loadingAppState.cpp)

set(CONSOLE
    Source/Systems/Private/console.cpp
    Source/Systems/Private/abstractCommand.cpp
    Source/Systems/Private/addUnitCommand.cpp
    Source/Systems/Private/addResourceCommand.cpp
    Source/Systems/Private/addTechnologyCommand.cpp
    Source/Systems/Private/toggleDebugCommand.cpp)

set(UNIT_BUTTONS
    Source/UI/Private/unitButton.cpp
    Source/UI/Private/tradeButton.cpp
    Source/UI/Private/buildButton.cpp
    Source/UI/Private/trainButton.cpp
    Source/UI/Private/researchButton.cpp
    Source/UI/Private/orderButton.cpp
    Source/UI/Private/stateToggleButton.cpp)

set(OPTIONS_BUTTONS
    Source/UI/Private/optionsButton.cpp
    Source/UI/Private/tabButton.cpp
    Source/UI/Private/okButton.cpp
    Source/UI/Private/defaultsButton.cpp)

set(TRADING_BUTTONS
    Source/UI/Private/activeStateBackButton.cpp
    Source/UI/Private/playerTradeButton.cpp
    Source/UI/Private/tradingScreenButton.cpp
    Source/UI/Private/offerButton.cpp
    Source/UI/Private/resourceAmmountButton.cpp)

set(MENU_BUTTONS
    Source/UI/Private/mainMenuButton.cpp
    Source/UI/Private/singlePlayerButton.cpp
    Source/UI/Private/exitButton.cpp
    Source/UI/Private/backButton.cpp
    Source/UI/Private/playButton.cpp)

set(MAP_EDITOR_BUTTONS
    Source/UI/Private/mapEditorButton.cpp
    Source/UI/Private/newMapButton.cpp
    Source/UI/Private/loadMapButton.cpp
    Source/UI/Private/exportButton.cpp)

set(BUTTONS
    Source/UI/Private/statsButton.cpp
    Source/UI/Private/activeStateButton.cpp
    Source/UI/Private/minimapButton.cpp
    ${TRADING_BUTTONS} ${OPTIONS_BUTTONS} ${MENU_BUTTONS}
    ${MAP_EDITOR_BUTTONS} ${UNIT_BUTTONS})

set(LISTBOXES
    Source/UI/Private/skyboxTextureListbox.cpp
    Source/UI/Private/landTextureListbox.cpp
    Source/UI/Private/gameObjectListbox.cpp
    Source/UI/Private/mapListbox.cpp)

set(GUI_SRC
    Source/UI/Private/tooltip.cpp
    Source/UI/Private/concreteGuiManager.cpp
    Source/UI/Private/gameObjectFrame.cpp
    Source/UI/Private/gameObjectFrameController.cpp
    ${BUTTONS} ${LISTBOXES})

set(AI_SRC
    Source/AI/Private/pathfinder.cpp
    Source/AI/Private/cameraController.cpp)

set(PROJECTILES
    Source/Gameplay/Private/projectile.cpp
    Source/Gameplay/Private/missile.cpp
    Source/Gameplay/Private/cruiseMissile.cpp
    Source/Gameplay/Private/shell.cpp
    Source/Gameplay/Private/depthCharge.cpp
    Source/Gameplay/Private/torpedo.cpp)

set(VEHICLES
    Source/Gameplay/Private/vehicle.cpp
    Source/Gameplay/Private/submarine.cpp
    Source/Gameplay/Private/engineer.cpp
    Source/Gameplay/Private/resourceRover.cpp
    Source/Gameplay/Private/freezer.cpp
    Source/Gameplay/Private/vessel.cpp)

set(STRUCTURES
    Source/Gameplay/Private/structure.cpp
    Source/Gameplay/Private/factory.cpp
    Source/Gameplay/Private/pointDefense.cpp
    Source/Gameplay/Private/extractor.cpp
    Source/Gameplay/Private/researchStruct.cpp
    Source/Gameplay/Private/iceSheet.cpp)

set(GAMEPLAY_SRC
    Source/Gameplay/Private/game.cpp
    Source/Gameplay/Private/gameManager.cpp
    Source/Gameplay/Private/gameObject.cpp
    Source/Gameplay/Private/gameObjectFactory.cpp
    Source/Gameplay/Private/player.cpp
    Source/Gameplay/Private/trader.cpp
    Source/Gameplay/Private/tradeCenter.cpp
    Source/Gameplay/Private/environment.cpp
    Source/Gameplay/Private/map.cpp
    Source/Gameplay/Private/fxManager.cpp
    Source/Gameplay/Private/explosion.cpp
    Source/Gameplay/Private/destructable.cpp
    Source/Gameplay/Private/resourceDeposit.cpp
    Source/Gameplay/Private/garrisonable.cpp
    Source/Gameplay/Private/unit.cpp
    Source/Gameplay/Private/weapon.cpp
    ${PROJECTILES} ${VEHICLES} ${STRUCTURES})

set(CORE_SRC
    Source/Core/Private/util.cpp
    Source/Core/Private/defConfigs.cpp)

set(GAME_SRC ${STATES} ${CONSOLE} ${GUI_SRC} ${AI_SRC} ${GAMEPLAY_SRC} ${CORE_SRC})

add_executable(${GAME_NAME} Source/Core/Private/main.cpp ${GAME_SRC})

target_include_directories(${GAME_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Public
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Systems/Public
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/Gameplay/Public
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/UI/Public
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/AI/Public)

target_link_libraries(${GAME_NAME} PRIVATE Renderer Engine)

if(BUILD_GAME_TESTS)
    if(UNIX)
        include_directories(/usr/include/cppunit)
        link_directories(/usr/lib)
    elseif(WIN32)
        include_directories("C:/Program Files (x86)/cppunit/include")
        link_directories("C:/Program Files (x86)/cppunit/lib")
    endif()

    set(TEST_SRC
        Tests/testMain.cpp
        Tests/pathfinderTest.cpp
        ${GAME_SRC})

    add_executable(battleshipTests ${TEST_SRC})
    target_include_directories(battleshipTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Core/Public
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Systems/Public
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Gameplay/Public
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/UI/Public
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/AI/Public)
    target_link_libraries(battleshipTests PRIVATE Renderer Engine cppunit)
endif()
