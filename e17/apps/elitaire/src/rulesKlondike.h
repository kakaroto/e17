class klondike : public cardGame
{
	protected:
	
	bool isThoughtful;
	
	virtual bool clickOn0();
	virtual void unClickOn0();
	void checkForWin(int t){
		int cards = 0;
		for(int i = 2;i <= 5;i++) 
			cards += stacks[i]->end() - stacks[i]->begin();
		if(cards == getNumCards()) sendEndGame(WON);
		checkForLazy(t+1);
	}
	
	virtual void unClickOn(cardStack* clstack){
		if(clstack == stacks[0]) unClickOn0();
		else cardGame::unClickOn(clstack);			
	}
	virtual bool rightClickOn(playingCard * pcard, int t){
		for(int i = 2; i < 6; i++)
			if(move(pcard,stacks[i], t))
				return true;
		return false;
	}
	void checkForLazy(int t);
	virtual bool move(playingCard* pcard,cardStack* tostack, int t);
	
	public:
		
	virtual bool deal();
	
	klondike(bool thoughtful = false);
	
	virtual ~klondike(){
	}
	
	virtual bool moveable(playingCard* pcard,cardStack* tostack);
	virtual bool clickOn(cardStack* clstack) {
		if(clstack == stacks[0]) return clickOn0();
		if(cardGame::clickOn(clstack)) {
			checkForLazy(0);
			return true;
		}
		return false;
	}
	
	virtual bool rightClickOn(playingCard * pcard){
		return rightClickOn(pcard, 0);
	}
};

/* vegas solitaire */

class vegas : public klondike
{
	protected:
	
	virtual bool clickOn0() { 
		if(stacks[0]->empty()) return false;
		else return klondike::clickOn0();
	}
	bool move(playingCard* pcard,cardStack* tostack, int t);
	public:
	
	vegas(){
		pType = POINTS_TYPE_MONEY;
		points = -52.0;
		mustwin = false;
	}
	virtual bool deal();
};

/* klondike 3 */

class klondike3 : public klondike
{
	protected:
	
	virtual bool clickOn0();
	virtual void unClickOn0();
	
	public:
		
	klondike3(){
	}
};
