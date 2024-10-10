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
#include <sys/types.h>
#include <sys/stat.h>

#include "log.h"

#include "configfile.h"
#include "cardsvalue.h"

#include "DatabaseManager.h"
#include "Player.h"
#include <sstream>
#include "GlobalObjects.h"

#include "game_defs.h"

using namespace std;

Log::Log(ConfigFile *c) : mySqliteLogDb(0), mySqliteLogFileName(""), myConfig(c), uniqueGameID(0), currentHandID(0), 
						currentRound(GAME_STATE_PREFLOP), sql("")
{
}

Log::~Log()
{
	if(SQLITE_LOG) {
        dbManager->closeDatabase();
	}
}

void
Log::init()
{

	if(SQLITE_LOG) {

		// logging activated
		if(myConfig->readConfigInt("LogOnOff")) {


			struct stat info;
			bool dirExists;

			if (stat(myConfig->readConfigString("LogDir").c_str(), &info) != 0)
				dirExists = false;
			else if (info.st_mode & S_IFDIR)  // S_ISDIR() doesn't exist on my windows 
				dirExists = true;

			// check if logging path exist
			if(myConfig->readConfigString("LogDir") != "" && dirExists) {

				mySqliteLogFileName.clear();
				mySqliteLogFileName /= myConfig->readConfigString("LogDir");
				mySqliteLogFileName /= string(SQL_LOG_FILE);

				bool databaseExists = false;
				if (FILE * file = fopen(mySqliteLogFileName.string().c_str(), "r"))  {
					fclose(file);
					databaseExists = true;
				}else{
#ifdef LOG_POKER_EXEC
					cout << "warning : database does not exist, will be created" << endl;
#endif
				}

                dbManager->openDatabase(mySqliteLogFileName.string().c_str());

				if( mySqliteLogDb != 0 ) {
					
					if (!  databaseExists)
						createDatabase();
				}
			}
		}
	}
}

void Log::createDatabase(){

	createRankingTable();
	createUnplausibleHandsTable();
    createPlayersStatisticsTable();

	for(int i=0; i < MAX_NUMBER_OF_PLAYERS; i++) {

		for (int j = 2; j <= MAX_NUMBER_OF_PLAYERS; j++){ 
			InitializePlayersStatistics(TightAgressivePlayerName[i], j);
			InitializePlayersStatistics(LooseAggressivePlayerName[i], j);
			InitializePlayersStatistics(ManiacPlayerName[i], j);
			InitializePlayersStatistics(UltraTightPlayerName[i], j);
		}
	}
	for (int j = 2; j <= MAX_NUMBER_OF_PLAYERS; j++){ 
		InitializePlayersStatistics(HumanPlayerName[0], j);
	}
}

void Log::InitializePlayersStatistics(const string playerName, const int nbPlayers){

	sql += "INSERT OR REPLACE INTO PlayersStatistics (";
	sql += "player_name,nb_players";
	sql += ",pf_hands,pf_checks,pf_calls,pf_raises,pf_3Bets,pf_call3Bets,pf_call3BetsOpportunities,pf_4Bets,pf_folds,pf_limps";
	sql += ",f_hands,f_checks,f_bets,f_calls,f_raises,f_3Bets,f_4Bets,f_folds,f_continuationBets,f_continuationBetsOpportunities" ;
	sql += ",t_hands,t_checks,t_bets,t_calls,t_raises,t_3Bets,t_4Bets,t_folds";
	sql += ",r_hands,r_checks,r_bets,r_calls,r_raises,r_3Bets,r_4Bets,r_folds";
	sql += ") VALUES (";
	sql += "'" ;
	sql += playerName;
	sql += "',";
	sql += std::to_string(nbPlayers);
	sql += ",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);";

	exec_transaction();
}

void
Log::logGameLosers(PlayerList activePlayerList)
{
	createRankingTable();

	int losers = 0;
	PlayerListConstIterator it_c;
	for (it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {
		if ((*it_c)->getCash() == 0){
			sql += "INSERT OR IGNORE INTO Ranking VALUES ('" + (*it_c)->getName() + "', 0, 0, 0);";
			const int lostStack = getIntegerValue((*it_c)->getName(), "Ranking", "lost_stack") + 1;
			sql +=  "UPDATE Ranking SET lost_stack = " + std::to_string(lostStack);
			sql += " WHERE player_name = '" + (*it_c)->getName() + "';";
		}
	}

	exec_transaction();

}
void
Log::logGameWinner(PlayerList activePlayerList)
{
	createRankingTable();

	int playersPositiveCashCounter = 0;
	PlayerListConstIterator it_c;
	for (it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {
		if ((*it_c)->getCash() > 0) playersPositiveCashCounter++;
	}
	if (playersPositiveCashCounter==1) {
		for (it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {
			if ((*it_c)->getCash() > 0) {
				sql += "INSERT OR IGNORE INTO Ranking VALUES ('" + (*it_c)->getName() + "', 0, 0, 0);";
				const int wonGame = getIntegerValue((*it_c)->getName(), "Ranking", "won_game") + 1;
				sql +=  "UPDATE Ranking SET won_game = " + std::to_string(wonGame);
				sql += " WHERE player_name = '" + (*it_c)->getName() + "';";
			}
		}
	}

	exec_transaction();

}
void
Log::logPlayedGames(PlayerList activePlayerList)
{
	createRankingTable();

	int losers = 0;
	PlayerListConstIterator it_c;
	for (it_c=activePlayerList->begin(); it_c!=activePlayerList->end(); ++it_c) {
		sql += "INSERT OR IGNORE INTO Ranking VALUES ('" + (*it_c)->getName() + "', 0, 0, 0);";
		const int playedGames = getIntegerValue((*it_c)->getName(), "Ranking", "played_games") + 1;
		sql +=  "UPDATE Ranking SET played_games = " + std::to_string(playedGames);
		sql += " WHERE player_name = '" + (*it_c)->getName() + "';";
	}

	exec_transaction();

}
int Log::getIntegerValue(const std::string playerName, const std::string tableName, const std::string attributeName){

	int result = 0;

	if( mySqliteLogDb != 0 ) {
		// sqlite-db is open

		char **result_Player=0;
		int nRow_Player=0;
		int nCol_Player=0;
		char *errmsg = 0;

		// read seat
		string sql_select = "SELECT " + attributeName + " FROM " + tableName + " WHERE player_name = \"" + playerName + "\"";
		QVariantList queryResult = dbManager->executeQuery(QString::fromStdString(sql_select));
		
		if (queryResult.isEmpty()) {
			cout << "Error in statement: " << sql_select << endl;
		} else {
			if (queryResult.size() == 1) {
				QVariantMap row = queryResult[0].toMap();
				result = row[QString::fromStdString(attributeName)].toInt();
			} else {
				cout << "Implausible information about player " << playerName << " in log-db!" << endl;
			}
		}
	}
	return result;
}


void Log::createRankingTable(){

	// create table if doesn't exist
	sql = "CREATE TABLE IF NOT EXISTS Ranking (";
	sql += "player_name VARCHAR NOT NULL";
	sql += ",lost_stack LARGEINT";
	sql += ",won_game LARGEINT";
	sql += ",played_games LARGEINT";
	sql += ", PRIMARY KEY(player_name));";
	exec_transaction();
}
void
Log::logUnplausibleHand(const std::string card1, const std::string card2, const bool human, 
						const char bettingRound, const int nbPlayers)
{
	createUnplausibleHandsTable();

	std::stringstream hand;

	if (card1.at(1) == card2.at(1)){
		// suited hand
		if (CardsValue::CardStringOrdering[card1] > CardsValue::CardStringOrdering[card2])
			hand << card1 << card2;
		else
		if (CardsValue::CardStringOrdering[card1] < CardsValue::CardStringOrdering[card2])
			hand <<  card2 <<  card1;
	}else{
		// unsuited hand
		if (CardsValue::CardStringOrdering[card1] > CardsValue::CardStringOrdering[card2])
			hand << card1.at(0) << card2.at(0) <<  'o';
		else
		if (CardsValue::CardStringOrdering[card1] < CardsValue::CardStringOrdering[card2])
			hand <<  card2.at(0) <<  card1.at(0) <<  'o';
		else
			hand <<  card1.at(0) <<  card2.at(0);
	}

	int losers = 0;
	sql += "INSERT OR IGNORE INTO UnplausibleHands VALUES ('" 
		+ hand.str() + "','" 
		+ bettingRound + "'," 
		+ std::to_string(nbPlayers)
		+ (human ? ", 1" : ", 0") 
		+ ", 0);";

	// get previous count value
	int previousCount = 0;

	if( mySqliteLogDb != 0 ) {

		char **result=0;
		int nRow=0;
		int nCol=0;
		char *errmsg = 0;

		// read seat
		string sql_select = "SELECT hands_count FROM UnplausibleHands WHERE hand = \"" + hand.str() + "\"";
		sql_select += " AND betting_round = \"" + std::to_string(bettingRound) + "\""; 
		sql_select += " AND nb_players = " + std::to_string(nbPlayers);
		sql_select += " AND human_player = " + std::to_string(human) + ";";
		
		QVariantList queryResult = dbManager->executeQuery(QString::fromStdString(sql_select));
		
		if (queryResult.isEmpty()) {
			cout << "Error in statement: " << sql_select << endl;
		} else {
			if (queryResult.size() == 1) {
				QVariantMap row = queryResult[0].toMap();
				previousCount = row["hands_count"].toInt();
			}
		}
	}

	sql +=  "UPDATE UnplausibleHands SET hands_count = " + std::to_string(++previousCount);
	sql += " WHERE hand = '" + hand.str() + "'"; 
	sql	+= " AND betting_round = '" + std::to_string(bettingRound) + "'"; 
	sql	+= " AND nb_players = " + std::to_string(nbPlayers); 
	sql += " AND human_player = " + std::to_string(human) + ";";

	exec_transaction();
}
void Log::createUnplausibleHandsTable(){

	// create table if doesn't exist
	sql = "CREATE TABLE IF NOT EXISTS UnplausibleHands (";
	sql += "hand CHAR(3) NOT NULL";
	sql += ",betting_round CHAR";
	sql += ",nb_players INT";
	sql += ",human_player CHAR";
	sql += ",hands_count LARGEINT DEFAULT 0";
	sql += ", PRIMARY KEY(hand, betting_round, nb_players, human_player));";
	exec_transaction();
}

void Log::createPlayersStatisticsTable(){

    sql += "CREATE TABLE  IF NOT EXISTS PlayersStatistics (";
    sql += "player_name VARCHAR NOT NULL";
    sql += ", nb_players SMALLINT NOT NULL";
    sql += ", pf_hands LARGEINT ";
    // preflop stats :
    sql += ", pf_folds LARGEINT ";
    sql += ", pf_limps LARGEINT ";
    sql += ", pf_checks LARGEINT ";
    sql += ", pf_calls LARGEINT ";
    sql += ", pf_raises LARGEINT ";
    sql += ", pf_3Bets LARGEINT ";
    sql += ", pf_call3Bets LARGEINT ";
    sql += ", pf_call3BetsOpportunities LARGEINT ";
    sql += ", pf_4Bets LARGEINT ";
    // flop stats :
    sql += ", f_hands LARGEINT ";
    sql += ", f_folds LARGEINT ";
    sql += ", f_checks LARGEINT ";
    sql += ", f_bets LARGEINT ";
    sql += ", f_calls LARGEINT ";
    sql += ", f_raises LARGEINT ";
    sql += ", f_3Bets LARGEINT ";
    sql += ", f_4Bets LARGEINT ";
    sql += ", f_continuationBets LARGEINT ";
    sql += ", f_continuationBetsOpportunities LARGEINT ";
    // turn stats :
    sql += ", t_hands LARGEINT ";
    sql += ", t_folds LARGEINT ";
    sql += ", t_checks LARGEINT ";
    sql += ", t_calls LARGEINT ";
    sql += ", t_bets LARGEINT ";
    sql += ", t_raises LARGEINT ";
    sql += ", t_3Bets LARGEINT ";
    sql += ", t_4Bets LARGEINT ";
    // river stats :
    sql += ", r_hands LARGEINT ";
    sql += ", r_folds LARGEINT ";
    sql += ", r_bets LARGEINT ";
    sql += ", r_checks LARGEINT ";
    sql += ", r_calls LARGEINT ";
    sql += ", r_raises LARGEINT ";
    sql += ", r_3Bets LARGEINT ";
    sql += ", r_4Bets LARGEINT ";

    sql += ", PRIMARY KEY(player_name, nb_players));";

    exec_transaction();


}

void
Log::exec_transaction()
{
	string sql_transaction = "BEGIN;" + sql + "COMMIT;";
	sql = ""; 
	//cout << endl << "SQL : " << sql_transaction << endl << endl;
	QVariantList queryResult = dbManager->executeQuery(QString::fromStdString(sql_transaction));
	
	if (queryResult.isEmpty()) {
		cout << "Error in statement: " << sql_transaction << endl;
	}
}

void
Log::logPlayersStatistics(PlayerList activePlayerList)
{

	PlayerListConstIterator it_c;

	const int i = activePlayerList->size();

	for(it_c=activePlayerList->begin(); it_c != activePlayerList->end(); ++it_c) {

		if ((*it_c)->getStatistics(i).getPreflopStatistics().m_hands == 0){
			return;
		}

		sql =  "UPDATE PlayersStatistics SET ";

		sql += "pf_hands = "		+ std::to_string((*it_c)->getStatistics(i).getPreflopStatistics().m_hands);
		sql += ",pf_checks = "		+ std::to_string((*it_c)->getStatistics(i).getPreflopStatistics().m_checks);
		sql += ",pf_calls = "		+ std::to_string((*it_c)->getStatistics(i).getPreflopStatistics().m_calls);
		sql += ",pf_raises = "		+ std::to_string((*it_c)->getStatistics(i).getPreflopStatistics().m_raises);
		sql += ",pf_3Bets = "	+ std::to_string((*it_c)->getStatistics(i).getPreflopStatistics().m_3Bets);
		sql += ",pf_call3Bets = "	+ std::to_string((*it_c)->getStatistics(i).getPreflopStatistics().m_call3Bets);
		sql += ",pf_call3BetsOpportunities = "	+ std::to_string((*it_c)->getStatistics(i).getPreflopStatistics().m_call3BetsOpportunities);
		sql += ",pf_4Bets = "	+ std::to_string((*it_c)->getStatistics(i).getPreflopStatistics().m_4Bets);
		sql += ",pf_folds = "		+ std::to_string((*it_c)->getStatistics(i).getPreflopStatistics().m_folds);
		sql += ",pf_limps = "		+ std::to_string((*it_c)->getStatistics(i).getPreflopStatistics().m_limps);

		sql += ",f_hands = "		+ std::to_string((*it_c)->getStatistics(i).getFlopStatistics().m_hands);
		sql += ",f_bets = "			+ std::to_string((*it_c)->getStatistics(i).getFlopStatistics().m_bets);
		sql += ",f_checks = "		+ std::to_string((*it_c)->getStatistics(i).getFlopStatistics().m_checks);
		sql += ",f_calls = "		+ std::to_string((*it_c)->getStatistics(i).getFlopStatistics().m_calls);
		sql += ",f_raises = "		+ std::to_string((*it_c)->getStatistics(i).getFlopStatistics().m_raises);
		sql += ",f_3Bets = "	+ std::to_string((*it_c)->getStatistics(i).getFlopStatistics().m_3Bets);
		sql += ",f_4Bets = "		+ std::to_string((*it_c)->getStatistics(i).getFlopStatistics().m_4Bets);
		sql += ",f_folds = "		+ std::to_string((*it_c)->getStatistics(i).getFlopStatistics().m_folds);
		sql += ",f_continuationBets = "	+ std::to_string((*it_c)->getStatistics(i).getFlopStatistics().m_continuationBets);
		sql += ",f_continuationBetsOpportunities = "	+ std::to_string((*it_c)->getStatistics(i).getFlopStatistics().m_continuationBetsOpportunities);

		sql += ",t_hands = "		+ std::to_string((*it_c)->getStatistics(i).getTurnStatistics().m_hands);
		sql += ",t_checks = "		+ std::to_string((*it_c)->getStatistics(i).getTurnStatistics().m_checks);
		sql += ",t_bets = "			+ std::to_string((*it_c)->getStatistics(i).getTurnStatistics().m_bets);
		sql += ",t_calls = "		+ std::to_string((*it_c)->getStatistics(i).getTurnStatistics().m_calls);
		sql += ",t_raises = "		+ std::to_string((*it_c)->getStatistics(i).getTurnStatistics().m_raises);
		sql += ",t_3Bets = "	+ std::to_string((*it_c)->getStatistics(i).getTurnStatistics().m_3Bets);
		sql += ",t_4Bets = "		+ std::to_string((*it_c)->getStatistics(i).getTurnStatistics().m_4Bets);
		sql += ",t_folds = "		+ std::to_string((*it_c)->getStatistics(i).getTurnStatistics().m_folds);

		sql += ",r_hands = "		+ std::to_string((*it_c)->getStatistics(i).getRiverStatistics().m_hands);
		sql += ",r_checks = "		+ std::to_string((*it_c)->getStatistics(i).getRiverStatistics().m_checks);
		sql += ",r_bets = "			+ std::to_string((*it_c)->getStatistics(i).getRiverStatistics().m_bets);
		sql += ",r_calls = "		+ std::to_string((*it_c)->getStatistics(i).getRiverStatistics().m_calls);
		sql += ",r_raises = "		+ std::to_string((*it_c)->getStatistics(i).getRiverStatistics().m_raises);
		sql += ",r_3Bets = "	+ std::to_string((*it_c)->getStatistics(i).getRiverStatistics().m_3Bets);
		sql += ",r_4Bets = "		+ std::to_string((*it_c)->getStatistics(i).getRiverStatistics().m_4Bets);
		sql += ",r_folds = "		+ std::to_string((*it_c)->getStatistics(i).getRiverStatistics().m_folds);

		sql += " WHERE player_name = '" + (*it_c)->getName() + "' AND nb_players = " + std::to_string(i) + ";";

		exec_transaction();
	}

}
