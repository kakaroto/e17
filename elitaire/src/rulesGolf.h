class golf : public cardGame 	
{
	protected:
	
    bool clickOn7();
	void unClickOn7();
	
    void checkForWin(int t){
		for(int i = 0;i < 7;i++) {
            if ( !stacks[i]->empty() ) return;
        }
		sendEndGame(WON);
	}
	
	virtual bool move(playingCard * pcard, cardStack * tostack, int t);
    virtual void unClickOn(cardStack* clstack){
		if(clstack == stacks[7]) unClickOn7();
		else cardGame::unClickOn(clstack);			
	}
	bool rightClickOn(playingCard * pcard, int t);
    
	public:
	
	virtual bool deal();
	
	golf();
	
	virtual ~golf(){
	}
	
	virtual bool moveable(playingCard* pcard,cardStack* tostack);
	virtual bool rightClickOn(playingCard * pcard){
		return rightClickOn(pcard, 0);
	}
    virtual bool clickOn(cardStack* clstack) {
		if(clstack == stacks[7]) return clickOn7();
		return cardGame::clickOn(clstack);
	}
};

