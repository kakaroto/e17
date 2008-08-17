#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include "points.h"

using namespace std;

enum stackLook{
	LAST,	// show only the last card
	ALL,	// show one below the other
	VALL,   // show one right to other
	STACK,	// show it like a stack of cards
	ENDL,	// End of Line
	SEPERATOR 
};

int random_range(int n);
inline int minimal(int n1, int n2);
inline int maximal(int n1, int n2);

class cardStack;
class cardDeck;

// A Class for every kind of a playing card
class playingCard
{
	friend class cardDeck;
	
	short name;	// each card has it's own Number e.g. As = 0, 2 = 1 ...
	bool visible;
	cardDeck *myDeck;
	
	short setName(short iname) {return name = iname;}
	
	public:
	
	cardStack *myStack;
	void* data;
	
	playingCard(short iname = 0, bool iVisible = true, cardDeck* iDeck = NULL)
	{
		name = iname;
		visible = iVisible;
		myDeck = iDeck;
	}
	char  getNumValues();
	short getName() {return name;}
	short getValue() {return name % getNumValues();}
	short getSuit() {return name / getNumValues();}
	cardStack * getStack() {return myStack;}
	int getStackNumber();
	bool  isVisible() {return visible;}
	bool  setVisible(bool iVisible = true) {return visible = iVisible;}	
	
};

typedef vector<playingCard*>::const_iterator card_iterator;

// the unvisble deck
// Pik 0, Herz 1, Kreuz 2, Karo 3
class cardDeck
{
	protected:
	char numValues;
	vector<playingCard> cards;
	vector<playingCard*> pcards;
	
	public:
		
	cardDeck(bool iVisible=true, int firstC=0, int lastC=51, char iValues=13)
	{
		numValues = iValues;
		
		for(int i = firstC; i <= lastC ; i++)
		{
			cards.push_back(playingCard(i,iVisible,this));
		}			
		for(int i = firstC; i <= lastC ; i++)
		{
			pcards.push_back(&cards[i]);
		}	
	}
	virtual ~cardDeck() {}
	
	char getNumValues() {return numValues;}
	char getNumCards() {return pcards.size();}
	
	virtual void shuffle() {
		random_shuffle(pcards.begin(), pcards.end(),random_range);
	}
	card_iterator begin(){
		return pcards.begin();
	}
	card_iterator end(){
		return pcards.end();
	}
	
};

class bakersCardDeck : public cardDeck
{
	public:
	bakersCardDeck() : cardDeck(true) {}
	virtual void shuffle();
};

class spiderDeck : public cardDeck
{
	public:
	spiderDeck(int numSuits);
};

/* ***************************************************************************
 * The Stacks
 * ***************************************************************************/

// abstract class for any kind of stack
class cardStack
{
	protected:
	friend class cardGame;
	
	vector<playingCard*> cards;
	int number; // the number of the stack in the cardGame
	
	public:

	cardStack(int num) {
		number = num;
	}
	virtual ~cardStack() {}

	int getStackNumber() {return number;}
	bool acceptCard(playingCard *pcard){
		if(empty()) return ruleEmpty(pcard);
		return rule(getLastCard(),pcard) && getLastCard()->isVisible();
	}
	playingCard * getCard(short position){return cards[position];}
	short getCardName(short position){
		if(!getCard(position)->isVisible()) return -1;
		else  return getCard(position)->getName();
	}
	playingCard * getLastCard(){
		if(cards.empty()) return NULL;
		return *(--cards.end());
	}
	short getLastCardName(){
		if(!getLastCard()->isVisible()) return -1;
		else  return getLastCard()->getName();
	}
	short getNumCards() {return cards.size();}
	bool empty() {return cards.empty();}
	short add(playingCard *pcard){
		cards.push_back(pcard);
		pcard->myStack = this;
		return getNumCards() -1;
	}
	playingCard * takeCard(){
		playingCard* pcard = getLastCard();
		cards.pop_back();
		return pcard;
	}
	bool clickOn(){
		if(empty()) return false;
		if(getLastCard()->isVisible()) return false;
		return getLastCard()->setVisible();
	}
	bool unClickOn(){
		if(empty()) return false;
		if(!getLastCard()->isVisible()) return false;
		return getLastCard()->setVisible(false);
	}
	card_iterator begin(){return cards.begin();}
	card_iterator end()  {return cards.end();}
	void clear() { cards.clear();}
	
	//returns 0 if not moveable else how many cards to move
	virtual int	moveable(playingCard * pcard);	
	virtual bool	rule(playingCard * lower, playingCard* upper)=0;
	virtual bool	ruleEmpty(playingCard * pcard)=0;
	
};

// This stack only accept cards  with the same suit
class sameSuitStack : public cardStack
{	
	short startWith;
	short step;
	
	public:
		
	sameSuitStack(int num, short iStep = 1, short iStartWith = 0)
			: cardStack(num) {
		step = iStep;
		startWith = iStartWith;
	}
	
	virtual bool	rule(playingCard* lower, playingCard* upper){
		return lower->getSuit() == upper->getSuit() && 
			lower->getValue() + step == upper->getValue();
	}
	virtual bool	ruleEmpty(playingCard* pcard){
		return pcard->getValue() == startWith || startWith == -1;
	}
};

class allSuitStack : public cardStack
{	
	short startWith;
	short step;
	
	public:
		
	allSuitStack(int num, short iStep = 1, short iStartWith = 0)
			: cardStack(num) {
		step = iStep;
		startWith = iStartWith;
	}
	
	virtual bool	rule(playingCard* lower, playingCard* upper){
		return lower->getValue() + step == upper->getValue();
	}
	virtual bool	ruleEmpty(playingCard* pcard){
		return pcard->getValue() == startWith || startWith == -1;
	}
};

class contraryColorStack : public cardStack
{
	short startWith;
	short step;		
	
	public:
	
	contraryColorStack(int num, short iStep = -1, short iStartWith = 12)
			: cardStack(num){
		step = iStep;
		startWith = iStartWith;
	}
	virtual bool rule(playingCard* lower, playingCard* upper){
		return lower->getSuit() % 2 != upper->getSuit() % 2
			&& lower->getValue() + step == upper->getValue();
	}
	virtual bool ruleEmpty(playingCard* pcard){
		return pcard->getValue() == startWith || startWith == -1;
	}
};

class singleCardStack : public cardStack
{		
	public:
	
	singleCardStack(int num) : cardStack(num) {}
	virtual bool rule(playingCard* lower, playingCard* upper) {
		return false;
	}
	virtual bool ruleEmpty(playingCard* pcard) {return true;}
};

class closedStack : public cardStack
{
	bool take;
	public:
	
	closedStack(int num, bool takeable=true) : cardStack(num) { 
		take = takeable;
	}
	virtual bool rule(playingCard * lower, playingCard * upper) {return false;}
	virtual bool ruleEmpty(playingCard * pcard) {return false;}
	virtual int  moveable(playingCard * pcard){
		if(take) return cardStack::moveable(pcard);
		return false;
	}
};

class golfStack : public cardStack
{
	public:
	
	golfStack(int num) : cardStack(num) {}
	virtual bool rule(playingCard * lower, playingCard * upper) {
        	return lower->getValue() == upper->getValue() - 1
               		|| lower->getValue() == upper->getValue() + 1;
	}
	virtual bool ruleEmpty(playingCard * pcard) {return false;}
	virtual int  moveable(playingCard * pcard){
		return false;
	}
};

class spiderStack : public allSuitStack
{
	public:
		
	spiderStack(int num) : allSuitStack(num, -1,-1) {}

	virtual int moveable(playingCard * pcard);
};

class bakersStack : public allSuitStack
{
	public:
		
	bakersStack(int num) : allSuitStack(num, -1,-2) {}

	virtual int moveable(playingCard * pcard);
};
/* ***************************************************************************
 * The Undo Classes
 * ***************************************************************************/
class cardGame;
	
class unDoJob
{
	protected:
	float points;
	
	public:
	virtual ~unDoJob() {}
	unDoJob(float iPoints) { points = iPoints;}
	virtual void undo(cardGame* game)=0;
};

class unDoClickOn : public unDoJob
{
	protected:
	cardStack* clickOn;
	
	public:
	unDoClickOn(float iPoints, cardStack* iClickOn) 
			: unDoJob(iPoints) { clickOn = iClickOn;}
	virtual void undo(cardGame* game);
};

class unDoMove : public unDoJob
{
	protected:
	playingCard* 	pcard;
	cardStack*	fromStack;
	
	public:
	unDoMove(float iPoints, playingCard* card, cardStack* stack) 
			: unDoJob(iPoints) 
	{
		pcard = card;
		fromStack = stack;
	}
	virtual void undo(cardGame* game);
};

/* ****************************************************************************
 * The Games
 * ****************************************************************************/

/**
 * There are some virtual function in class cardGame
 * 	4 of them must be overloaded:
 *		bool move(playingCard* pcard,cardStack* tostack, int t)
 *		bool moveable(playingCard* pcard,cardStack* tostack)
 *		bool rightClickOn(playingCard * pcard)
 *		bool deal()
 * 	the rest could, but doesn"t need to be overloaded
 *		void unClickOn(cardStack* clstack)
 *		bool clickOn(cardStack* clstack)
 *		bool undo()
 */

class cardGame
{
	friend class unDoMove; friend class unDoClickOn;
	protected:
		
	enum endGameType{
		WON     = 0,
		LOST    = 1,
		GIVEUP  = 2,
		PLAYING = 3
	};
	
	vector<cardDeck*>	decks;
	vector<cardStack*> 	stacks;
	vector<stackLook> 	looks;
	stack<unDoJob*> 	undos;
	float 		    	points;
	pointsType   		pType;
	int	            	lazy;
	bool	    		mustwin;
	endGameType         gameState;
	
	struct {
		bool            flipCards;
		bool            flipValue;
	} afterRestart;
	
	void (*move_card)	(playingCard*, bool single);
	void (*flip_card)	(playingCard*);
	void (*wait_card)	(playingCard*,int);
	void (*win)    		(int,float,pointsType,void*);
	void (*points_cb)	(float,pointsType,void*);
	void * winData;
	void * pointsData;
	
	//nice way to use the callbacks
	void sendMoveCard(playingCard * pcard, bool single){
		if(move_card) (*move_card)(pcard, single);
	}
	void sendFlipCard(playingCard * pcard){
		if(flip_card) (*flip_card)(pcard);
	}
	void sendWaitCard(playingCard * pcard, int t){
		if(t && wait_card) (*wait_card)(pcard,t);
	}
	void sendEndGame(endGameType type){
		gameState = type;
		if(win) (*win) (type, points, pType, winData);
	}
	float addPoints(float mod){
		points += mod;
		if(points_cb) (*points_cb)(points, pType, pointsData);
		return points;
	}
	float setPoints(float p){
		points = p;
		if(points_cb) (*points_cb)(points, pType, pointsData);
		return points;
	}
	
	void moveAllUnder(playingCard * pcard, cardStack * tostack, bool sendmove = true);
	virtual bool move(playingCard * pcard, cardStack * tostack, int t)=0;
	virtual void unMove(playingCard * pcard, cardStack * tostack) {
		moveAllUnder(pcard, tostack);
	}
	virtual void unClickOn(cardStack* clstack){
		if(!clstack->unClickOn()) sendFlipCard(clstack->getLastCard());
	}
	
	public:
		
	cardGame(){
		/* set callbacks to sane values */
		move_card = NULL;
		flip_card = NULL;
		wait_card = NULL;
		win       = NULL;
		winData   = NULL;
		points_cb = NULL;

		lazy      = 0;
		points    = 0;
		pType     = POINTS_TYPE_UNKNOWN;
		mustwin   = true;
		gameState = PLAYING;
		
		afterRestart.flipCards = false;
		afterRestart.flipValue = false;
	}
			
	virtual ~cardGame();
	virtual bool deal()=0;
	
	// card stack functions
	card_iterator getStackBegin(int number){
		return stacks[number]->begin();
	}
	card_iterator getStackEnd(int number){
		return stacks[number]->end();
	}
	card_iterator getStackBegin(playingCard* pcard){
		return pcard->myStack->begin();
	}
	card_iterator getStackEnd(playingCard* pcard){
		return pcard->myStack->end();
	}
	int getNumStacks(){
		return stacks.size();
	}
	
	// card deck functions
	card_iterator getDeckBegin(int number){
		return decks[number]->begin();
	}
	card_iterator getDeckEnd(int number){
		return decks[number]->end();
	}
	int getNumDecks(){
		return decks.size();
	}
	
	playingCard* getLastCard(int number){
		return stacks[number]->getLastCard();
	}
	char getLastCardName(int number){
		return stacks[number]->getLastCardName();
	}
	
	vector<stackLook>::const_iterator getStackLooksBegin() {return looks.begin();}
	vector<stackLook>::const_iterator getStackLooksEnd() {return looks.end();}
	int moveableCard(playingCard* pcard){
		if(pcard->isVisible()) return pcard->myStack->moveable(pcard);
		else return 0;
	}
	virtual bool moveable(playingCard* pcard,cardStack* tostack)=0;
	bool moveable(playingCard* pcard,int tostack){
		return moveable(pcard,stacks[tostack]);
	}
	
	bool move(playingCard* pcard,cardStack* tostack){
		return move(pcard, tostack, 0);
	}
	bool move(playingCard* pcard,int tostack){
		return move(pcard,stacks[tostack]);
	}
	virtual bool clickOn(cardStack * clstack);
	bool clickOn(int number){
		return clickOn(stacks[number]);
	}
	bool clickOn(playingCard* pcard){
		return clickOn(pcard->myStack);
	}
	
	int getNumCards(){
		int num = 0, numDecks, i = 0;
		numDecks = getNumDecks();
		while(i < numDecks) num += decks[i++]->getNumCards();
		return num;
	}
	
	virtual bool rightClickOn(playingCard * pcard)=0;
	
	void addFlipCallback(void (*func)(playingCard*) ){
		flip_card = func;
	}
	void addMoveCallback(void (*func)(playingCard*, bool single) ){
		move_card = func;
	}
	void addWaitCallback(void (*func)(playingCard*, int) ){
		wait_card = func;
	}
	
	void addWinCallback(void (*func)(int,float,pointsType,void*), void * data){
		win = func;
		winData = data;
	}
	void addPointsCallback(void (*func)(float,pointsType,void*), void * data){
		points_cb = func;
		pointsData = data;
		if(func) (*func)(points, pType, data);
	}
	
	float getPoints() { return points;}
	pointsType getPointsType() { return pType;}
	int getLazyMode() { return lazy;}
	void setLazyMode(int mode) { lazy = mode;}
	bool giveUp() {
		if(gameState != PLAYING) return false;
		if(mustwin) sendEndGame(LOST);
		else sendEndGame(GIVEUP);
		return true; 
	}
	
	bool undo();
	void restart();
	vector<playingCard*> * getHints();
};

#include "rulesFreeCell.h"
#include "rulesKlondike.h"
#include "rulesBakersDozen.h"
#include "rulesGolf.h"
#include "rulesSpider.h"
/* Randomizer ****************************************************************/
inline int random_range(int n)
{
	return (int) (( double) n *rand()/(RAND_MAX + 1.0));
}

/* minmax */
inline int minimal(int n1, int n2)
{
	return (n1 > n2) ? n2 : n1;
}

inline int maximal(int n1, int n2)
{
	return (n1 < n2) ? n2 : n1;
}

