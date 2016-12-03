/*This file is where all unit testing will be going. All you need to do
 *is include the appropriate header files and then perform your test.
 */

#define BOOST_TEST_MODULE const string test;

// Project Includes
#include "source/GameLogic/CrazyEightsLogic.hpp"
#include "source/GameLogic/SpadesLogic.hpp"
#include "source/PlayerAPI/Card.hpp"
#include "source/PlayerAPI/Player.hpp"

// Standard Includes
#include <sstream>
#include <vector>

// Boost Includes
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/serialization/access.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(startNewRound)
{
  boost::asio::io_service service;
  Player player(1, TCPConnection::create(service));
  player.setRoundScore(10);
  player.setBid(5);
  player.setTricksWon(1);
  player.startNewRound();
  BOOST_CHECK_EQUAL(player.getRoundScore(), 0);
  BOOST_CHECK_EQUAL(player.getBid(), 0);
  BOOST_CHECK_EQUAL(player.getTricksWon(), 0);
  BOOST_CHECK_EQUAL(player.getOverallScores().size(), 1);
}

BOOST_AUTO_TEST_CASE(startNewGame)
{
  boost::asio::io_service service;
  Player player(1, TCPConnection::create(service));
  player.setRoundScore(10);
  player.setBid(5);
  player.setBags(2);
  player.setTricksWon(1);
  player.startNewGame();
  BOOST_CHECK_EQUAL(player.getRoundScore(), 0);
  BOOST_CHECK_EQUAL(player.getBid(), 0);
  BOOST_CHECK_EQUAL(player.getBags(), 0);
  BOOST_CHECK_EQUAL(player.getTricksWon(), 0);
  BOOST_CHECK_EQUAL(player.getOverallScores().empty(), 1);
}

BOOST_AUTO_TEST_CASE(insertCardToHand)
{
  boost::asio::io_service service;
  Player player(1, TCPConnection::create(service));
  Card card(HEARTS, TWO);
  player.insertCardToHand(card);
  BOOST_CHECK_EQUAL(player.getHand().size(), 1);
}

BOOST_AUTO_TEST_CASE(removeCardFromHand)
{
  boost::asio::io_service service;
  Player player(1, TCPConnection::create(service));
  Card card(HEARTS, TWO);
  player.insertCardToHand(card);
  BOOST_CHECK_EQUAL(player.removeCardFromHand(card), 1);
  BOOST_CHECK_EQUAL(player.removeCardFromHand(card), 0);
}

BOOST_AUTO_TEST_CASE(SerializeCard)
{
  std::stringstream serialize;
  Card serializeCard(CLUBS, ACE);
  boost::archive::text_oarchive oArchive(serialize);
  oArchive << serializeCard;

  Card deserializeCard;
  std::stringstream deserialize(serialize.str());
  boost::archive::text_iarchive iArchive(deserialize);
  iArchive >> deserializeCard;

  BOOST_CHECK_EQUAL(deserializeCard.getSuit(), CLUBS);
  BOOST_CHECK_EQUAL(deserializeCard.getValue(), ACE);
}

BOOST_AUTO_TEST_CASE(initializeCrazyEights)
{
  boost::asio::io_service service;
  auto player1 = std::make_shared<Player>(1, TCPConnection::create(service));
  auto player2 = std::make_shared<Player>(2, TCPConnection::create(service));
  auto player3 = std::make_shared<Player>(3, TCPConnection::create(service));
  auto player4 = std::make_shared<Player>(4, TCPConnection::create(service));
  std::vector<std::shared_ptr<Player>> players;

  players.push_back(player1);
  players.push_back(player2);
  players.push_back(player3);
  players.push_back(player4);

  CrazyEightsLogic crazyEights(players);

  int count = 4;
  for (int i = 0; i < count; i++) {
    BOOST_CHECK_EQUAL(players.at(i)->getId(), crazyEights.getPlayers().at(i)->getId());
    BOOST_CHECK_EQUAL(crazyEights.getPlayers().at(i)->getHand().size(), 5);
  }

  BOOST_CHECK_EQUAL(crazyEights.getTurn(), 0);
  BOOST_CHECK_EQUAL(crazyEights.getCardsDrawnCounter(), 0);

}

BOOST_AUTO_TEST_CASE(crazyEightsDrawCard)
{
  boost::asio::io_service service;
  auto player1 = std::make_shared<Player>(1, TCPConnection::create(service));
  std::vector<std::shared_ptr<Player>> players;
  players.push_back(player1);

  CrazyEightsLogic crazyEights(players);

  crazyEights.drawCard();

  BOOST_CHECK_EQUAL(crazyEights.getPlayers().at(0)->getHand().size(), 6);
}

BOOST_AUTO_TEST_CASE(crazyEightsGameOver)
{
  boost::asio::io_service service;
  auto player1 = std::make_shared<Player>(1, TCPConnection::create(service));
  std::vector<std::shared_ptr<Player>> players;
  players.push_back(player1);

  CrazyEightsLogic crazyEights(players);
  std::vector<Card> playerHand = crazyEights.getPlayers().at(0)->getHand();
  Card card1 = playerHand[0];
  Card card2 = playerHand[1];
  Card card3 = playerHand[2];
  Card card4 = playerHand[3];
  Card card5 = playerHand[4];

  crazyEights.playCard(card1);
  crazyEights.playCard(card2);
  crazyEights.playCard(card3);
  crazyEights.playCard(card4);
  crazyEights.playCard(card5);

  BOOST_CHECK_EQUAL(crazyEights.isGameOver(), 1);

}

BOOST_AUTO_TEST_CASE(checkCardScoreVals)
{
  Card card1(HEARTS, EIGHT);
  Card card2(CLUBS, ACE);
  Card card3(SPADES, KING);

  boost::asio::io_service service;
  auto player = std::make_shared<Player>(1, TCPConnection::create(service));
  std::vector<std::shared_ptr<Player>> players;
  players.push_back(player);

  CrazyEightsLogic crazyEights(players);

  BOOST_CHECK_EQUAL(crazyEights.getCardScoreValue(card1), 50);
  BOOST_CHECK_EQUAL(crazyEights.getCardScoreValue(card2), 1);
  BOOST_CHECK_EQUAL(crazyEights.getCardScoreValue(card3), 10);
}

BOOST_AUTO_TEST_CASE(ValidMove)
{
  boost::asio::io_service service;
  auto player = std::make_shared<Player>(1, TCPConnection::create(service));
  std::vector<std::shared_ptr<Player>> players;
  players.push_back(player);

  CrazyEightsLogic crazyEights(players);

  Card discardCard(HEARTS, SEVEN);
  Card newCard(HEARTS, KING);

  std::vector<Card> currentDiscard = crazyEights.getDiscardPile();

  currentDiscard.clear();
  crazyEights.setDiscardPile(currentDiscard);

  currentDiscard.push_back(discardCard);
  crazyEights.setDiscardPile(currentDiscard);

  crazyEights.getPlayers().at(0)->insertCardToHand(newCard);
  std::vector<Card> playerHand = crazyEights.getPlayers().at(0)->getHand();
  BOOST_CHECK_EQUAL(playerHand.size(), 6);

  auto pos = std::find(playerHand.begin(), playerHand.end(), newCard);
  auto index = std::distance(playerHand.begin(), pos);
  crazyEights.validCard(crazyEights.getPlayers().at(0)->getHand().at(index));
  playerHand = crazyEights.getPlayers().at(0)->getHand();
  BOOST_CHECK_EQUAL(playerHand.size(), 5);
}

BOOST_AUTO_TEST_CASE(SpadesGetNextPlayer)
{
  Spades s;
  for (int i = 0; i < 4; i++)
  {
    BOOST_CHECK_EQUAL(s.getNextPlayer(i), ((i + 1) % 4));
  }
}
