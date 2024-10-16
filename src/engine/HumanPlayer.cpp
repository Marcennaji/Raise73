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
#include <engine/HumanPlayer.h>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

using namespace std;

HumanPlayer::HumanPlayer(ConfigFile *c, int id, PlayerType type, std::string name, 
						std::string avatar, int sC, bool aS, bool sotS, int mB):
	Player(c, id, type, name, avatar, sC, aS, sotS, mB){

}

	
HumanPlayer::~HumanPlayer(){

}

const SimResults HumanPlayer::getHandSimulation() const{

	SimResults r;
	const string cards = (getCardsValueString() + getStringBoard()).c_str();

	SimulateHand(cards.c_str() , &r, 0, 1, 0);
	float win = r.win; //save the value
	
	const int nbOpponents = max(1, currentHand->getRunningPlayerList()->size() - 1); // note that allin opponents are not "running" any more
	SimulateHandMulti(cards.c_str() , &r, 1300, 350, nbOpponents); 
	r.win = win; // because SimulateHandMulti doesn't compute 'win'

	return r;
}



