/*****************************************************************************
 * Raise73 - Texas Holdem No Limit software, offline game against custom AIs *
 * Copyright (C) 2024 Marc Ennaji                                            *
 *                                                                           *
 * This program is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU Affero General Public License as            *
 * published by the Free Software Foundation, either version 3 of the        *
 * License, or (at your option) any later version.                           *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU Affero General Public License for more details.                       *
 *                                                                           *
 * You should have received a copy of the GNU Affero General Public License  *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
 *****************************************************************************/

#include "game.h"

#include <engine/enginefactory.h>
#include <qt/guiinterface.h>
#include <engine/log.h>

#include <engine/exception.h>
#include <engine/engine_msg.h>

#include <session.h>

#include <iostream>

using namespace std;

Game::Game(GuiInterface* gui, std::shared_ptr<EngineFactory> factory,
		   const PlayerList &playerList, const GameData &gameData,
		   const StartData &startData, int gameId, Log* log)
	: myFactory(factory), myGui(gui), myLog(log), startQuantityPlayers(startData.numberOfPlayers),
	  startCash(gameData.startMoney), startSmallBlind(gameData.firstSmallBlind),
	  myGameID(gameId), currentSmallBlind(gameData.firstSmallBlind), currentHandID(0), dealerPosition(0),
	  lastHandBlindsRaised(1), lastTimeBlindsRaised(0), myGameData(gameData)
{

	blindsList = myGameData.manualBlindsList;

	dealerPosition = startData.startDealerPlayerId;

	// determine dealer position
	PlayerListConstIterator player_i = playerList->begin();
	PlayerListConstIterator player_end = playerList->end();

	while (player_i != player_end) {
		if ((*player_i)->getID() == dealerPosition)
			break;
		++player_i;
	}
	if (player_i == player_end)
		throw Exception(__FILE__, __LINE__, ERR_DEALER_NOT_FOUND);

	// create board
	currentBoard = myFactory->createBoard(dealerPosition);

	// create player lists
	seatsList.reset(new std::list<std::shared_ptr<Player> >);
	activePlayerList.reset(new std::list<std::shared_ptr<Player> >);
	runningPlayerList.reset(new std::list<std::shared_ptr<Player> >);

	(*runningPlayerList) = (*playerList);
	(*activePlayerList) = (*playerList);
	(*seatsList) = (*playerList);

	currentBoard->setPlayerLists(seatsList, activePlayerList, runningPlayerList);
	
	if(myLog) 
		myLog->logPlayedGames(activePlayerList);
}

Game::~Game()
{
	runningPlayerList->clear();
	activePlayerList->clear();
	seatsList->clear();
}

std::shared_ptr<HandInterface> Game::getCurrentHand()
{
	return currentHand;
}

const std::shared_ptr<HandInterface> Game::getCurrentHand() const
{
	return currentHand;
}

void Game::initHand()
{

	size_t i;
	PlayerListConstIterator it_c;
	PlayerListIterator it;

	currentHandID++;

	// calculate smallBlind
	raiseBlinds();

	// set player action none
	for(it=seatsList->begin(); it!=seatsList->end(); ++it) {
		(*it)->setAction(PLAYER_ACTION_NONE);
	}

	// set player with empty cash inactive
	it = activePlayerList->begin();
	while( it!=activePlayerList->end() ) {

		if((*it)->getCash() == 0) {
			(*it)->setActiveStatus(false);
			it = activePlayerList->erase(it);
		} else {
			++it;
		}
	}

	runningPlayerList->clear();
	(*runningPlayerList) = (*activePlayerList);

	// create Hand
	currentHand = myFactory->createHand(myFactory, myGui, currentBoard, myLog, seatsList, activePlayerList, runningPlayerList, currentHandID, startQuantityPlayers, dealerPosition, currentSmallBlind, startCash);

	// shifting dealer button -> TODO exception-rule !!!
	bool nextDealerFound = false;
	PlayerListConstIterator dealerPositionIt = currentHand->getSeatIt(dealerPosition);
	if(dealerPositionIt == seatsList->end()) {
		throw Exception(__FILE__, __LINE__, ERR_SEAT_NOT_FOUND);
	}

	for(i=0; i<seatsList->size(); i++) {

		++dealerPositionIt;
		if(dealerPositionIt == seatsList->end()) dealerPositionIt = seatsList->begin();

		it_c = currentHand->getActivePlayerIt( (*dealerPositionIt)->getID() );
		if(it_c != activePlayerList->end() ) {
			nextDealerFound = true;
			dealerPosition = (*it_c)->getID();
			break;
		}
	}
	if(!nextDealerFound) {
		throw Exception(__FILE__, __LINE__, ERR_NEXT_DEALER_NOT_FOUND);
	}
}

void Game::startHand()
{
	myGui->nextRoundCleanGui();

	// log new hand
	myGui->logNewGameHandMsg(myGameID, currentHandID);
	myGui->flushLogAtGame(myGameID);

	currentHand->start();
}

std::shared_ptr<Player> Game::getPlayerByUniqueId(unsigned id)
{
	std::shared_ptr<Player> tmpPlayer;
	PlayerListIterator i = getSeatsList()->begin();
	PlayerListIterator end = getSeatsList()->end();
	while (i != end) {
		if ((*i)->getID() == id) {
			tmpPlayer = *i;
			break;
		}
		++i;
	}
	return tmpPlayer;
}

std::shared_ptr<Player> Game::getPlayerByNumber(int number)
{
	std::shared_ptr<Player> tmpPlayer;
	PlayerListIterator i = getSeatsList()->begin();
	PlayerListIterator end = getSeatsList()->end();
	while (i != end) {
		if ((*i)->getID() == number) {
			tmpPlayer = *i;
			break;
		}
		++i;
	}
	return tmpPlayer;
}

std::shared_ptr<Player> Game::getCurrentPlayer()
{
	std::shared_ptr<Player> tmpPlayer = getPlayerByUniqueId(getCurrentHand()->getCurrentBettingRound()->getCurrentPlayersTurnId());
	if (!tmpPlayer.get())
		throw Exception(__FILE__, __LINE__, ERR_CURRENT_PLAYER_NOT_FOUND);
	return tmpPlayer;
}

std::shared_ptr<Player> Game::getPlayerByName(const std::string &name)
{
	std::shared_ptr<Player> tmpPlayer;
	PlayerListIterator i = getSeatsList()->begin();
	PlayerListIterator end = getSeatsList()->end();
	while (i != end) {
		if ((*i)->getName() == name) {
			tmpPlayer = *i;
			break;
		}
		++i;
	}
	return tmpPlayer;
}

void Game::raiseBlinds()
{

	bool raiseBlinds = false;

	if (myGameData.raiseIntervalMode == RAISE_ON_HANDNUMBER) {
		if (lastHandBlindsRaised + myGameData.raiseSmallBlindEveryHandsValue <= currentHandID) {
			raiseBlinds = true;
			lastHandBlindsRaised = currentHandID;
		}
	}
	if (raiseBlinds) {
		// At this point, the blinds must be raised
		// Now we check how the blinds should be raised
		if (myGameData.raiseMode == DOUBLE_BLINDS) {
			currentSmallBlind *= 2;
		} else {
			if(!blindsList.empty()) {
				currentSmallBlind = blindsList.front();
				blindsList.pop_front();
			} else {
				// The position exceeds the list
				if (myGameData.afterManualBlindsMode == AFTERMB_DOUBLE_BLINDS) {
					currentSmallBlind *= 2;
				} else {
					if(myGameData.afterManualBlindsMode == AFTERMB_RAISE_ABOUT) {
						currentSmallBlind += myGameData.afterMBAlwaysRaiseValue;
					}
				}
			}
		}
		currentSmallBlind = min(currentSmallBlind,startQuantityPlayers*startCash/2);
	}
}
