cmake_minimum_required(VERSION 3.21.1)

set(BUILD_QDS_COMPONENTS ON CACHE BOOL "Build design studio components")
option(LINK_INSIGHT "Link Qt Insight Tracker library" ON)

project(Raise73App LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 COMPONENTS Gui Qml Quick)
find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick)
find_package(Qt6 REQUIRED COMPONENTS  Sql LinguistTools)

# To build this application you need Qt 6.2.0 or higher

if (Qt6_VERSION VERSION_LESS 6.2.0)
message(FATAL_ERROR "You need Qt 6.2.0 or newer to build the application.")
endif()


qt_add_executable(${CMAKE_PROJECT_NAME} 
    src/main.cpp
    src/engine/psim/psim.cpp
    src/engine/psim/psim.hpp
    src/engine/psim/simDefs.hpp
    src/engine/psim/simulate.cpp
    src/engine/psim/simulate.hpp
    src/engine/arraydata.cpp
    src/engine/arraydata.h
    src/engine/BettingRound.cpp
    src/engine/BettingRound.h
    src/engine/BettingRoundFlop.cpp
    src/engine/BettingRoundFlop.h
    src/engine/BettingRoundInterface.cpp
    src/engine/BettingRoundInterface.h
    src/engine/BettingRoundPostRiver.cpp
    src/engine/BettingRoundPostRiver.h
    src/engine/BettingRoundPreflop.cpp
    src/engine/BettingRoundPreflop.h
    src/engine/BettingRoundRiver.cpp
    src/engine/BettingRoundRiver.h
    src/engine/BettingRoundTurn.cpp
    src/engine/BettingRoundTurn.h
    src/engine/Board.cpp
    src/engine/Board.h
    src/engine/BoardInterface.cpp
    src/engine/boardinterface.h
    src/engine/cardsvalue.cpp
    src/engine/cardsvalue.h
    src/engine/DatabaseManager.h
    src/engine/DatabaseManager.cpp
    src/engine/GlobalObjects.h
    src/engine/engine.h
    src/engine/engine_msg.h
    src/engine/EngineFactory.cpp
    src/engine/EngineFactory.h
    src/engine/Exception.cpp
    src/engine/Exception.h
    src/engine/game.cpp
    src/engine/game.h
    src/engine/Hand.cpp
    src/engine/Hand.h
    src/engine/handinterface.cpp
    src/engine/handinterface.h
    src/engine/HumanPlayer.cpp
    src/engine/HumanPlayer.h
    src/engine/log.cpp
    src/engine/log.h
    src/engine/LooseAggressivePlayer.cpp
    src/engine/LooseAggressivePlayer.h
    src/engine/ManiacPlayer.cpp
    src/engine/ManiacPlayer.h
    src/engine/player.cpp
    src/engine/player.h
    src/engine/PlayerStatistics.cpp
    src/engine/PlayerStatistics.h
    src/engine/ranges.h
    src/engine/replay.h
    src/engine/TightAgressivePlayer.cpp
    src/engine/TightAgressivePlayer.h
    src/engine/tools.cpp
    src/engine/tools.h
    src/engine/UltraTightPlayer.cpp
    src/engine/UltraTightPlayer.h
    src/third_party/poker-eval/include/inlines/eval.h
    src/third_party/poker-eval/include/inlines/eval_joker.h
    src/third_party/poker-eval/include/inlines/eval_joker_low.h
    src/third_party/poker-eval/include/inlines/eval_joker_low8.h
    src/third_party/poker-eval/include/inlines/eval_low.h
    src/third_party/poker-eval/include/inlines/eval_low27.h
    src/third_party/poker-eval/include/inlines/eval_low8.h
    src/third_party/poker-eval/include/inlines/eval_type.h
    src/third_party/poker-eval/include/inlines/evx5.h
    src/third_party/poker-eval/include/inlines/evx7.h
    src/third_party/poker-eval/include/inlines/evx_action.h
    src/third_party/poker-eval/include/inlines/evx_inlines.h
    src/third_party/poker-eval/include/combinations.h
    src/third_party/poker-eval/include/config.h
    src/third_party/poker-eval/include/deck.h
    src/third_party/poker-eval/include/deck_joker.h
    src/third_party/poker-eval/include/deck_std.h
    src/third_party/poker-eval/include/deck_undef.h
    src/third_party/poker-eval/include/enumdefs.h
    src/third_party/poker-eval/include/enumerate.h
    src/third_party/poker-eval/include/enumord.h
    src/third_party/poker-eval/include/evx_defs.h
    src/third_party/poker-eval/include/game_joker.h
    src/third_party/poker-eval/include/game_std.h
    src/third_party/poker-eval/include/handval.h
    src/third_party/poker-eval/include/handval_low.h
    src/third_party/poker-eval/include/md5c.h
    src/third_party/poker-eval/include/poker_config.h
    src/third_party/poker-eval/include/poker_config.h.in
    src/third_party/poker-eval/include/poker_defs.h
    src/third_party/poker-eval/include/poker_wrapper.h
    src/third_party/poker-eval/include/pokereval_export.h
    src/third_party/poker-eval/include/rules_joker.h
    src/third_party/poker-eval/include/rules_std.h
    src/third_party/poker-eval/include/rules_undef.h
    src/third_party/tinystr.cpp
    src/third_party/tinystr.h
    src/third_party/tinyxml.cpp
    src/third_party/tinyxml.h
    src/third_party/tinyxmlerror.cpp
    src/third_party/tinyxmlparser.cpp
    src/configfile.cpp
    src/configfile.h
    src/convhelper.cpp
    src/convhelper.h
    src/game_defs.h
    src/gamedata.h
    src/loghelper.h
    src/loghelper_client.cpp
    src/main.cpp
    src/pch_game.h
    src/pch_lib.h
    src/session.cpp
    src/session.h
    src/third_party/poker-eval/lib/combinations.c
    src/third_party/poker-eval/lib/deck.c
    src/third_party/poker-eval/lib/deck_joker.c
    src/third_party/poker-eval/lib/deck_std.c
    src/third_party/poker-eval/lib/enumerate.c
    src/third_party/poker-eval/lib/enumord.c
    src/third_party/poker-eval/lib/evx.c
    src/third_party/poker-eval/lib/lowball.c
    src/third_party/poker-eval/lib/mktable.c
    src/third_party/poker-eval/lib/mktable_utils.c
    src/third_party/poker-eval/lib/poker_wrapper.c
    src/third_party/poker-eval/lib/rules_joker.c
    src/third_party/poker-eval/lib/rules_std.c
    src/third_party/poker-eval/lib/t_botcard.c
    src/third_party/poker-eval/lib/t_botfivecards.c
    src/third_party/poker-eval/lib/t_botfivecardsj.c
    src/third_party/poker-eval/lib/t_cardmasks.c
    src/third_party/poker-eval/lib/t_evx_flushcards.c
    src/third_party/poker-eval/lib/t_evx_pairval.c
    src/third_party/poker-eval/lib/t_evx_strval.c
    src/third_party/poker-eval/lib/t_evx_tripsval.c
    src/third_party/poker-eval/lib/t_jokercardmasks.c
    src/third_party/poker-eval/lib/t_jokerstraight.c
    src/third_party/poker-eval/lib/t_maskrank.c
    src/third_party/poker-eval/lib/t_nbits.c
    src/third_party/poker-eval/lib/t_nbitsandstr.c
    src/third_party/poker-eval/lib/t_straight.c
    src/third_party/poker-eval/lib/t_topbit.c
    src/third_party/poker-eval/lib/t_topcard.c
    src/third_party/poker-eval/lib/t_topfivebits.c
    src/third_party/poker-eval/lib/t_topfivecards.c
    src/third_party/poker-eval/lib/t_toptwobits.c
    src/third_party/poker-eval/include/config.h.in
    src/third_party/poker-eval/include/stamp-h1
    src/third_party/poker-eval/include/stamp-h2
    src/qt/qttools/qthelper/qthelper.cpp 
    src/qt/qttools/qthelper/qthelper.h 
    src/qt/qttools/qttoolswrapper.cpp 
    src/qt/qttools/qttoolswrapper.h
    src/qttoolsinterface.cpp
    src/qttoolsinterface.h       
    )

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
    src
    src/engine
    src/third_party
    src/third_party/poker-eval/include
)

# qt_standard_project_setup() requires Qt 6.3 or higher. See https://doc.qt.io/qt-6/qt-standard-project-setup.html for details.

if (${QT_VERSION_MINOR} GREATER_EQUAL 3)
qt6_standard_project_setup()
endif()



qt_add_resources(${CMAKE_PROJECT_NAME} "configuration"
    PREFIX "/"
    FILES
        qtquickcontrols2.conf
)



target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE

    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Sql
)



if (${BUILD_QDS_COMPONENTS})
    include(${CMAKE_CURRENT_SOURCE_DIR}/qmlcomponents)
endif ()



include(${CMAKE_CURRENT_SOURCE_DIR}/qmlmodules)



if (LINK_INSIGHT)
    include(${CMAKE_CURRENT_SOURCE_DIR}/insight)
endif ()



install(TARGETS ${CMAKE_PROJECT_NAME}

    BUNDLE DESTINATION .

    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

