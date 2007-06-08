class bakersDozen : public cardGame
{
	protected:
	
	void checkForLazy(int t);
	void checkForWin(int t){
		int cards = 0;
		for(int i = 7;i < 10;i++) cards += stacks[i]->end() - stacks[i]->begin();
		cards += stacks[16]->end() - stacks[16]->begin();
		
		if( cards == getNumCards()) sendEndGame(WON);
		checkForLazy(t+1);
	}
	virtual void unMove(playingCard * pcard, cardStack * tostack){
		tostack->add(pcard->myStack->takeCard());
		sendMoveCard(pcard, true);
	}
	virtual bool move(playingCard* pcard,cardStack* tostack, int t){
		if(!moveable(pcard,tostack)) return false;
			
		undos.push(new unDoMove(1.0,pcard,pcard->myStack));
		tostack->add(pcard->myStack->takeCard());
		sendWaitCard(pcard, t);
		sendMoveCard(pcard, true);
		addPoints(1.0);
		checkForWin(t);
		
		return true;
	}
	bool rightClickOn(playingCard * pcard, int t);
	
	public:
	
	virtual bool deal();
	
	bakersDozen();	
	virtual ~bakersDozen(){
	}
	
	virtual bool moveable(playingCard* pcard,cardStack* tostack){
		return tostack->acceptCard(pcard) && moveableCard(pcard) == 1;
	}
	
	virtual bool clickOn(cardStack* clstack){
		return false;
	}
	virtual bool rightClickOn(playingCard * pcard){
		return rightClickOn(pcard,0);
	}
};
