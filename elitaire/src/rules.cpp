/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Rules.h"

using namespace std;

/* playingCard ***************************************************************/
char playingCard::getNumValues()
{
    return myDeck->getNumValues();
}

int playingCard::getStackNumber()
{
    return myStack->getStackNumber();
}

/* ***************************************************************************
 * The Decks
 * ***************************************************************************/
void bakersCardDeck::shuffle()
{
    int s;

    random_shuffle(pcards.begin(), pcards.end(), random_range);
    s = pcards.size();
    
    for (int i = 8; i < s; i++) {
        if (pcards[i]->getValue() == 12) {
            playingCard * card;

            card = pcards[i];
            pcards.erase(pcards.begin() + i);
            pcards.insert(pcards.begin(), card);
            i--;
        }
    }

    random_shuffle(pcards.begin(), pcards.begin() + 12, random_range);

}

spiderDeck::spiderDeck(int numSuits) : cardDeck(true, 0, 0)
{
    int suit = 0;
    
    numValues = 13;
    cards.clear();
    pcards.clear();

    for (int i = 0; i < 8; i++) {
        for (int k = suit; k < numValues + suit; k++) {
            cards.push_back(playingCard(k, false, this));
        }
        if (!((i + 1) % (8 / numSuits))) suit += numValues;
    }

    for (int i = 0; i < numValues * 8; i++) {
        pcards.push_back(&cards[i]);
    }
}

/* ***************************************************************************
 * The Stacks
 * ***************************************************************************/
int cardStack::moveable(playingCard * pcard)
{
    card_iterator it = cards.begin();
    card_iterator it_end = cards.end();
    card_iterator it_card;

    while (*it != pcard) it++;
        
    it_card = it;
    
    while (++it != it_end) {
        if (!rule(*(it - 1), *it)) return false;
    }

    return it - it_card;
}

int spiderStack::moveable(playingCard * pcard)
{
    card_iterator it = cards.begin();
    card_iterator it_end = cards.end();
    card_iterator it_card;

    while (*it != pcard) it++;
    
    it_card = it;
    
    while (++it != it_end)
        if (!rule(*(it - 1), *it)
            || !((*(it - 1))->getSuit() == (*it)->getSuit()))
            return false;

    return it - it_card;
}

int bakersStack::moveable(playingCard * pcard)
{
    return getLastCard() == pcard;
}

/* ***************************************************************************
 * The Undo Class
 * ***************************************************************************/
void unDoClickOn::undo(cardGame * game)
{
    game->unClickOn(clickOn);
    game->addPoints(points);
}

void unDoMove::undo(cardGame * game)
{
    game->unMove(pcard, fromStack);
    game->addPoints(points);
}

/* ***************************************************************************
 * The Games
 * ***************************************************************************/
cardGame::~cardGame()
{
    while (!stacks.empty()) {
        delete stacks.back();
        stacks.pop_back();
    }

    while (!decks.empty()) {
        delete decks.back();
        decks.pop_back();
    }

    while (!undos.empty()) {
        unDoJob * job;

        job = undos.top();
        undos.pop();
        delete job;
    }
}

void cardGame::moveAllUnder(playingCard * pcard, cardStack * tostack,
                            bool sendmove)
{
    card_iterator it, it_end;
    cardStack * fromstack;
    int n;

    fromstack = pcard->myStack;
    it = fromstack->begin();
    it_end = fromstack->end();


    while (*it != pcard) it++;
        
    n = it_end - it;
    
    while (it != it_end) tostack->add(*it++);
    while (n--) fromstack->takeCard();
    
    if (sendmove) sendMoveCard(pcard, false);
}

bool cardGame::clickOn(cardStack * clstack)
{
    if (clstack->clickOn()) {
        sendFlipCard(clstack->getLastCard());
        undos.push(new unDoClickOn(0.0, clstack));
        return true;
    }
    return false;
}

bool cardGame::undo()
{
    if (undos.empty()) return false;
    else {
        unDoJob *job;

        job = undos.top();
        job->undo(this);
        undos.pop();
        delete job;

        return true;
    }
}

void cardGame::restart()
{
    vector<cardStack*>::const_iterator it, end;

    it = stacks.begin();
    end = stacks.end();

    while (it != end) {
        (*it++)->clear();
    }

    if (afterRestart.flipCards) {
        cout << endl << decks.size() << endl;
        for (int i = decks.size(); i; i--) {
            card_iterator c, c_end;

            c = getDeckBegin(i - 1);
            c_end = getDeckEnd(i - 1);

            while (c != c_end) {
                bool v;

                v = (*c)->isVisible();
                (*c)->setVisible(afterRestart.flipValue);
                if (v != afterRestart.flipValue)
                    sendFlipCard(*c);

                c++;
            }
        }
    }

    deal();
}

vector<playingCard*> * cardGame::getHints()
{
    vector<playingCard*> * hints = new vector<playingCard*>;

    /* search in every stack if a card is moveable to every stack excluding
     * to itself */
    for (size_t from = 0; from < stacks.size(); from++) {
        card_iterator it, it_end;

        it = stacks[from]->begin();
        it_end = stacks[from]->end();
        
        for (;it != it_end; it++) {
            if (!moveableCard(*it))
                continue;

            for (size_t to = 0; to < stacks.size(); to++) {
                if (from != to && moveable(*it, to)) {
                    hints->push_back(*it);
                    break;
                }
            }
        }
    }

    return hints;
}


/* Klondike ******************************************************************/
klondike::klondike(bool thoughtful):isThoughtful(thoughtful)
{

    int i = 0;
    int num = 0;

    decks.push_back(new cardDeck(false));

    for (i = 1; i <= 2; i++) {
        looks.push_back(STACK);
        stacks.push_back(new closedStack(num++));
    }
    looks.push_back(SEPERATOR);
    for (i = 1; i <= 4; i++) {
        looks.push_back(LAST);
        stacks.push_back(new sameSuitStack(num++));
    }
    looks.push_back(ENDL);
    for (i = 1; i <= 7; i++) {
        looks.push_back(ALL);
        stacks.push_back(new contraryColorStack(num++));
    }
    looks.push_back(ENDL);
    looks.push_back(ENDL);

    decks[0]->shuffle();
    pType = POINTS_TYPE_INTEGER_BAD;

    afterRestart.flipCards = true;
    afterRestart.flipValue = false;
}

bool klondike::deal()
{
    int i;
    card_iterator it, it_end;

    setPoints(0.0);

    it = getDeckBegin(0);
    it_end = getDeckEnd(0);

    while (it != it_end) {
        stacks[0]->add(*it);
        sendMoveCard(*it++, true);
    }

    for (i = 0; i != 7; i++) {
        for (int j = i; j != 7; j++) {
            playingCard * pcard;
            
            pcard = stacks[0]->takeCard();
            stacks[j + 6]->add(pcard);
            sendMoveCard(pcard, true);
            if (isThoughtful) clickOn(pcard->myStack);
        }
    }
    for (i = 0; i != 7; i++) cardGame::clickOn(6 + i);
    return true;
}

bool klondike::clickOn0()
{
    cardStack * cs0 = stacks[0];
    cardStack * cs1 = stacks[1];
    playingCard *pcard;

    undos.push(new unDoClickOn(0.0, cs0));

    if (cs0->empty() && cs1->empty()) return false;
    else if (cs0->empty()) {
        int i = 0;

        while (!cs1->empty()) {
            pcard = cs1->takeCard();
            pcard->setVisible(false);
            cs0->add(pcard);
            sendFlipCard(pcard);
            sendMoveCard(pcard, true);
            i++;
        }
        addPoints((float) i);
    }
    else {
        pcard = cs0->takeCard();
        pcard->setVisible();
        cs1->add(pcard);
        sendFlipCard(pcard);
        sendMoveCard(pcard, true);
    }
    checkForLazy(0);
    return true;
}

void klondike::unClickOn0()
{
    cardStack * cs1 = stacks[0];
    cardStack * cs0 = stacks[1];
    playingCard * pcard;

    if (cs0->empty() && cs1->empty()) return;
    else if (cs0->empty()) {
        int i = 0;

        while (!cs1->empty()) {
            pcard = cs1->takeCard();
            pcard->setVisible();
            cs0->add(pcard);
            sendFlipCard(pcard);
            sendMoveCard(pcard, true);
            i++;
        }
        addPoints((float) i);
    }
    else {
        pcard = cs0->takeCard();
        pcard->setVisible(false);
        cs1->add(pcard);
        sendFlipCard(pcard);
        sendMoveCard(pcard, true);
    }
}

bool klondike::moveable(playingCard * pcard, cardStack * tostack)
{
    card_iterator it = pcard->myStack->begin();
    card_iterator it_end = pcard->myStack->end();

    if (!(tostack->acceptCard(pcard) && moveableCard(pcard)))
        return false;
    
    while (*it != pcard) it++;
    
    it++;
    
    if (it == it_end) return true;
    else return tostack->rule(pcard, *it);
}

void klondike::checkForLazy(int t)
{
    int min = 14;

    if (lazy < 1) return;

    for (int i = 2; i < 6; i++)
        min = minimal(min, stacks[i]->end() - stacks[i]->begin());
    min = maximal(min, 1);

    int numstacks = getNumStacks();

    for (int i = 1; i < numstacks; i++) {
        playingCard *pcard;

        if ((pcard = getLastCard(i))) {
            if (pcard->getValue() <= min) {
                if (rightClickOn(pcard, t)) return;
            }
        }
        if (i == 1) i = 5;
    }
}

bool klondike::move(playingCard * pcard, cardStack * tostack, int t)
{
    if (!moveable(pcard, tostack)) return false;
    else {
        undos.push(new unDoMove(1.0, pcard, pcard->myStack));
        sendWaitCard(pcard, t);
        moveAllUnder(pcard, tostack);
        addPoints(1.0);
        checkForWin(t + 1);
    }
    return true;
}

/* vegas */
bool vegas::move(playingCard * pcard, cardStack * tostack, int t)
{
    if (!moveable(pcard, tostack)) return false;
    else {
        float cash = 1.0;

        for (int i = 2; i < 6; i++) {
            if (stacks[i] == tostack) {
                addPoints(5.0);
                cash = -6.0;
                break;
            }
            else if (stacks[i] == pcard->myStack) {
                addPoints(-5.0);
                cash = 4.0;
                break;
            }
        }

        undos.push(new unDoMove(cash, pcard, pcard->myStack));
        sendWaitCard(pcard, t);
        moveAllUnder(pcard, tostack);
        checkForWin(t + 1);
    }
    return true;
}

bool vegas::deal()
{
    bool re;

    re = klondike::deal();
    setPoints(-52.0);
    return re;
}

/* klondike3 */

bool klondike3::clickOn0()
{
    cardStack * cs0 = stacks[0];
    cardStack * cs1 = stacks[1];
    

    undos.push(new unDoClickOn(0.0, cs0));

    if (cs0->empty())
        klondike::clickOn0();
    else {
        for (int i = 0; i < 3 && !cs0->empty(); i++) {
            playingCard * pcard;
            
            pcard = cs0->takeCard();
            pcard->setVisible();
            cs1->add(pcard);
            sendWaitCard(pcard, i * 2);
            sendFlipCard(pcard);
            sendMoveCard(pcard, true);
        }
    }
    
    checkForLazy(0);
    return true;
}

void klondike3::unClickOn0()
{
    cardStack * cs1 = stacks[1];
    cardStack * cs0 = stacks[0];

    if (cs1->empty()) klondike::unClickOn0();
    else
        for (int i = 0; i < 3 && !cs1->empty(); i++) {
            playingCard * pcard;
            
            pcard = cs1->takeCard();
            pcard->setVisible(false);
            cs0->add(pcard);
            sendWaitCard(pcard, i * 2);
            sendFlipCard(pcard);
            sendMoveCard(pcard, true);
        }
}

/* Freecell ******************************************************************/
freeCell::freeCell()
{
    int i;
    int num = 0;

    decks.push_back(new cardDeck(true));

    for (i = 1; i <= 4; i++) {
        looks.push_back(LAST);
        stacks.push_back(new singleCardStack(num++));
    }
    looks.push_back(SEPERATOR);
    for (i = 1; i <= 4; i++) {
        looks.push_back(LAST);
        stacks.push_back(new sameSuitStack(num++));
    }
    looks.push_back(ENDL);
    for (i = 1; i <= 8; i++) {
        looks.push_back(ALL);
        stacks.push_back(new contraryColorStack(num++, -1, -1));
    }
    looks.push_back(ENDL);
    looks.push_back(ENDL);

    decks[0]->shuffle();

    pType = POINTS_TYPE_INTEGER_BAD;
}

bool freeCell::deal()
{
    card_iterator it;
    int num;

    setPoints(0.0);

    it = getDeckBegin(0);
    num = getNumCards();

    for (int i = 0; i <= num / 8; i++) {
        for (int j = 0; j != 8 && (i * 8 + j) != num; j++) {
            stacks[j + 8]->add(*it);
            sendMoveCard(*it++, true);
        }
    }
    return true;
}

void freeCell::checkForLazy(int t)
{
    int min;
    int numstacks;
    if (lazy < 1) return;

    min = 14;
    numstacks = getNumStacks();
    
    for (int i = 4; i <= 7; i++)
        min = minimal(min, stacks[i]->end() - stacks[i]->begin());
    min = maximal(min, 1);

    for (int i = 0; i < 4; i++) {
        playingCard *pcard;

        if ((pcard = getLastCard(i))) {
            if (pcard->getValue() <= min) {
                if (rightClickOnRightSide(pcard, t))
                    return;
            }
        }
    }

    for (int i = 8; i < numstacks; i++) {
        playingCard *pcard;

        if ((pcard = getLastCard(i))) {
            if (pcard->getValue() <= min) {
                if (rightClickOnRightSide(pcard, t))
                    return;
            }
        }
    }
}

bool freeCell::moveable(playingCard * pcard, cardStack * tostack)
{
    if (!(moveableCard(pcard) <= (1 + freeSCS()) * (1 + freeCSS(tostack))
         && tostack->acceptCard(pcard)
         && moveableCard(pcard))) return false;

    card_iterator it = pcard->myStack->begin();
    card_iterator it_end = pcard->myStack->end();

    while (*it++ != pcard);
        
    if (it == it_end) return true;
    else return tostack->rule(pcard, *it);
}

bool freeCell::move(playingCard * pcard, cardStack * tostack, int t)
{
    if (!moveable(pcard, tostack)) return false;
    else {
        undos.push(new unDoMove(1.0, pcard, pcard->myStack));
        addPoints(1.0);
        sendWaitCard(pcard, t);
        moveAllUnder(pcard, tostack);
        checkForWin(t + 1);
        return true;
    }
}

/* Baker's Dozen *************************************************************/
bakersDozen::bakersDozen()
{
    int i;
    int num = 0;

    decks.push_back(new bakersCardDeck);

    for (i = 1; i <= 7; i++) {
        looks.push_back(ALL);
        stacks.push_back(new bakersStack(num++));
    }

    looks.push_back(LAST);
    stacks.push_back(new sameSuitStack(num++));
    looks.push_back(ENDL);

    for (i = 1; i <= 2; i++) {
        for (int i2 = 1; i2 <= 7; i2++)
            looks.push_back(SEPERATOR);
        looks.push_back(LAST);
        stacks.push_back(new sameSuitStack(num++));
        looks.push_back(ENDL);
    }

    for (i = 1; i <= 6; i++) {
        looks.push_back(ALL);
        stacks.push_back(new bakersStack(num++));
    }
    looks.push_back(LAST);
    stacks.push_back(new sameSuitStack(num++));

    looks.push_back(ENDL);
    looks.push_back(ENDL);

    decks[0]->shuffle();

    pType = POINTS_TYPE_INTEGER_BAD;
}

bool bakersDozen::deal()
{
    int i, i_card, i_stack;
    card_iterator it;

    setPoints(0.0);

    it = getDeckBegin(0);

    for (i_card = 0; i_card < 4; i_card++) {
        for (i = 0, i_stack = 0; i < 16; i++, i_stack++) {
            if (i < 7 || i > 9) {
                stacks[i_stack]->add(*it);
                sendMoveCard(*it++, true);
            }
        }
    }
    return true;
}

bool bakersDozen::rightClickOn(playingCard * pcard, int t)
{
    int order[] = { 7, 8, 9, 16 };

    for (int i = 0; i < 4; i++) {
        if (move(pcard, stacks[order[i]], t)) {
            return true;
        }
    }

    return false;
}

void bakersDozen::checkForLazy(int t)
{
    int min;
    int numstacks;
    
    if (lazy < 1) return;
    
    min = 14;
    numstacks = getNumStacks();
    
    for (int i = 7; i <= 9; i++)
        min = minimal(min, stacks[i]->end() - stacks[i]->begin());
    min = minimal(min, stacks[16]->end() - stacks[16]->begin());
    min = maximal(min, 1);

    for (int i = 0; i < 7; i++) {
        playingCard * pcard;

        if ((pcard = getLastCard(i))) {
            if (pcard->getValue() <= min) {
                if (rightClickOn(pcard, t))
                    return;
            }
        }
    }

    for (int i = 10; i < numstacks - 1; i++) {
        playingCard * pcard;

        if ((pcard = getLastCard(i))) {
            if (pcard->getValue() <= min) {
                if (rightClickOn(pcard, t))
                    return;
            }
        }
    }
}

/* Golf  ******************************************************************/
golf::golf()
{
    int i;
    int num = 0;

    decks.push_back(new cardDeck(false));

    looks.push_back(SEPERATOR);
    for (i = 0; i < 7; i++) {
        looks.push_back(ALL);
        stacks.push_back(new closedStack(num++));
    }
    looks.push_back(ENDL);
    looks.push_back(ENDL);
    looks.push_back(LAST);
    stacks.push_back(new closedStack(num++, false));
    looks.push_back(VALL);
    stacks.push_back(new golfStack(num++));
    for (i = 0; i < 6; i++) {
        looks.push_back(SEPERATOR);
    }
    
    decks[0]->shuffle();

    pType = POINTS_TYPE_INTEGER_GOOD;
    
    afterRestart.flipCards = true;
    afterRestart.flipValue = false;
}

bool golf::deal()
{
    int i;
    card_iterator it, it_end;

    setPoints(17.0);

    it = getDeckBegin(0);
    it_end = getDeckEnd(0);

    while (it != it_end) {
        stacks[7]->add(*it);
        sendMoveCard(*it++, true);
    }

    for (i = 0; i < 5; i++) {
        for (int  j = 0; j < 7; j++) {
            playingCard * pcard;
            
            pcard = stacks[7]->takeCard();
            stacks[j]->add(pcard);
            sendMoveCard(pcard, true);
            cardGame::clickOn(pcard);
        }
    }
    
    return true;
}

bool golf::moveable(playingCard * pcard, cardStack * tostack)
{
    return tostack->acceptCard(pcard) && moveableCard(pcard);
}

bool golf::move(playingCard * pcard, cardStack * tostack, int t)
{
    if(!moveable(pcard,tostack)) return false;
	
    undos.push(new unDoMove(0.0, pcard,pcard->myStack));
    tostack->add(pcard->myStack->takeCard());
    sendWaitCard(pcard, t);
    sendMoveCard(pcard, true);
    checkForWin(t);
	
    return true;
}

bool golf::clickOn7()
{
    cardStack * cs0 = stacks[7];
    cardStack * cs1 = stacks[8];
    playingCard * pcard;

    if (cs0->empty()) return false;
    
    addPoints(-1.0);
    
    undos.push(new unDoClickOn(0.0, cs0));
    
    pcard = cs0->takeCard();
    pcard->setVisible();
    cs1->add(pcard);
    sendFlipCard(pcard);
    sendMoveCard(pcard, true);
    
    return true;
}

void golf::unClickOn7()
{
    cardStack * cs1 = stacks[7];
    cardStack * cs0 = stacks[8];
    playingCard * pcard;

    if (cs0->empty()) return;
    
    pcard = cs0->takeCard();
    pcard->setVisible(false);
    cs1->add(pcard);
    sendFlipCard(pcard);
    sendMoveCard(pcard, true);
    
}

bool golf::rightClickOn(playingCard * pcard, int t){
    return move(pcard, stacks[8], t);
}

/* Spider ********************************************************************/
spider::spider(int numSuits)
{
    int i;
    int num = 0;

    decks.push_back(new spiderDeck(numSuits));

    looks.push_back(LAST);
    stacks.push_back(new closedStack(num++));
    looks.push_back(SEPERATOR);

    for (i = 1; i <= 8; i++) {
        looks.push_back(LAST);
        stacks.push_back(new closedStack(num++, false));
    }

    looks.push_back(ENDL);

    for (i = 0; i < 10; i++) {
        looks.push_back(ALL);
        stacks.push_back(new spiderStack(num++));
    }

    looks.push_back(ENDL);
    looks.push_back(ENDL);
    looks.push_back(ENDL);

    decks[0]->shuffle();

    pType = POINTS_TYPE_INTEGER_BAD;

    afterRestart.flipCards = true;
    afterRestart.flipValue = false;
}

bool spider::deal()
{
    int i;
    playingCard * pcard;
    card_iterator it, it_end;

    setPoints(0.0);

    it = getDeckBegin(0);
    it_end = getDeckEnd(0);

    while (it != it_end) {
        stacks[0]->add(*it);
        sendMoveCard(*it++, true);
    }

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 10 && (i * 10 + j) < 54; j++) {
            pcard = stacks[0]->takeCard();
            stacks[j + 9]->add(pcard);
            sendMoveCard(pcard, true);
        }
    }

    for (i = 0; i != 10; i++) cardGame::clickOn(9 + i);
        
    return true;
}

bool spider::move(playingCard * pcard, cardStack * tostack, int t)
{
    if (!moveable(pcard, tostack)) return false;
    else {
        undos.push(new unDoMove(1.0, pcard, pcard->myStack));
        addPoints(1.0);
        moveAllUnder(pcard, tostack);
        checkForWin();
        return true;
    }
}

bool spider::checkForCleanUp(card_iterator it, card_iterator it_end)
{
    if (it_end - it != 13) return false;

    it_end--;
    while (it != it_end) {
        if ((*it)->getName() != (*(it + 1))->getName() + 1)
            return false;

        it++;
    }
    return true;
}

void spider::cleanUp(card_iterator it, card_iterator it_end)
{
    int i = 1;

    while (!stacks[i]->empty()) i++;

    undos.push(new unDoMove(0.0, *it, (*it)->myStack));
    moveAllUnder(*it, stacks[i], false);

    if (move_card) {
        while (it != it_end)
            (*move_card) (*it++, true);
    }

    checkForWin();
}

void spider::checkForWin()
{
    int stcks = 0;

    for (int i = 1; i < 9; i++) stcks += stacks[i]->empty();

    if (!stcks) sendEndGame(WON);

    // perhaps a street is full
    for (int i = 9; i < 19; i++) {
        card_iterator it, it_end;

        it = stacks[i]->begin();
        it_end = stacks[i]->end();

        if (it_end - it > 11) {
            while (it != it_end - 12) {
                if ((*it)->getValue() == 12) {
                    if (checkForCleanUp(it, it_end)) {
                        cleanUp(it, it_end);
                        it = it_end - 13;
                    }
                }
                it++;
            }
        }
    }

}

bool spider::clickOn0()
{
    if (stacks[0]->empty()) return false;
    for (int i = 9; i != 19; i++) {
        if (stacks[i]->empty()) return false;
    }
    undos.push(new unDoClickOn(0.0, stacks[0]));

    for (int j = 9; j < 19; j++) {
        playingCard * pcard;

        pcard = stacks[0]->takeCard();
        stacks[j]->add(pcard);
        sendMoveCard(pcard, true);
    }

    for (int i = 0; i != 10; i++) cardGame::clickOn(9 + i);

    return true;
}

void spider::unClickOn0()
{
    for (int j = 9; j < 19; j++) {
        playingCard * pcard;

        pcard = stacks[j]->takeCard();
        stacks[0]->add(pcard);
        sendMoveCard(pcard, true);
    }
}
