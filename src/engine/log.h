/*****************************************************************************
 * PokerTraining - THNL training software, based on the PokerTH GUI          *
 * Copyright (C) 2013 Marc Ennaji                                            *
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

#ifndef LOG_H
#define LOG_H

#define SQLLITE_COMPLETE_LOG

#include <string>
#include <filesystem>

#include "engine.h"
#include "game_defs.h"

#define SQL_LOG_FILE "pokerTraining-log-v0.9.pdb"

struct sqlite3;

class ConfigFile;

class Log
{

public:
	Log(ConfigFile *c);

	~Log();

	void init();
	void logGameLosers(PlayerList activePlayerList);
	void logGameWinner(PlayerList activePlayerList);
	void logPlayedGames(PlayerList activePlayerList);
	void logUnplausibleHand(const std::string card1, const std::string card2, const bool human, 
				const char bettingRound, const int nbPlayers);
	void logPlayersStatistics(PlayerList activePlayerList);
	void InitializePlayersStatistics(const std::string playerName, const int nbPlayers);
	void createDatabase();

	void setCurrentRound(GameState theValue) {
		currentRound = theValue;
	}

	std::string getSqliteLogFileName() {
		return mySqliteLogFileName.string();
	}

private:

	void exec_transaction();
	int getIntegerValue(const std::string playerName, const std::string tableName, const std::string attributeName);
	void createRankingTable();
	void createUnplausibleHandsTable();
    void createPlayersStatisticsTable();

	sqlite3 *mySqliteLogDb;
	std::filesystem::path mySqliteLogFileName;
	ConfigFile *myConfig;
	int uniqueGameID;
	int currentHandID;
	GameState currentRound;
	std::string sql;
};

#endif // LOG_H
