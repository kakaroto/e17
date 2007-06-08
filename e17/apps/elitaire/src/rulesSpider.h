
class spider : public cardGame
{
	protected:
	
	bool checkForCleanUp(card_iterator it, card_iterator it_end);	
	void cleanUp(card_iterator it, card_iterator it_end);	
	void checkForWin();
	bool clickOn0();
	void unClickOn0();
	virtual bool move(playingCard * pcard, cardStack * tostack, int t);
	virtual void unClickOn(cardStack* clstack){
		if(clstack == stacks[0]) unClickOn0();
		else cardGame::unClickOn(clstack);			
	}
	
	public:
	
	virtual bool deal();
	spider(int numSuits);	
	virtual ~spider(){
	}
	virtual bool moveable(playingCard* pcard,cardStack* tostack){
		return tostack->acceptCard(pcard);
	}
	
	virtual bool clickOn(cardStack* clstack){
		if(clstack == stacks[0]) return clickOn0();
		else return cardGame::clickOn(clstack);
	}
	
	virtual bool rightClickOn(playingCard * pcard){
		return false;
	}
};
