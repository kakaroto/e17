/* freecell ******************************************************************/

class freeCell : public cardGame 	
{
	protected:
	
	int freeSCS(){
		int freescs = 0;
		for(int i = 0;i < 4;i++) freescs += stacks[i]->empty();
		return freescs;
	}
	int freeCSS(cardStack * tostack){
		int freecss = 0;
		for(int i = 8;i < 16;i++) 
			if(stacks[i] != tostack) freecss += stacks[i]->empty();
		return freecss;
	}
	
	void checkForWin(int t){
		int cards = 0;
		for(int i = 4;i <= 7;i++) cards += stacks[i]->end() - stacks[i]->begin();
		
		if( cards == getNumCards()) sendEndGame(WON);
		checkForLazy(t);
	}
	
	bool rightClickOnLeftSide(playingCard* pcard, int t){
		for(int i = 0; i < 4; i++){
			if(move(pcard,stacks[i], t)) return true;
		}
		return false;
	}
	bool rightClickOnRightSide(playingCard* pcard, int t){
		for(int i = 4; i < 8; i++){
			if(move(pcard,stacks[i], t)) return true;
		}
		return false;
	}
	
	virtual bool move(playingCard * pcard, cardStack * tostack, int t);
	void checkForLazy(int t);
	
	public:
	
	virtual bool deal();
	
	freeCell();
	
	virtual ~freeCell(){
	}
	
	virtual bool moveable(playingCard* pcard,cardStack* tostack);
	virtual bool rightClickOn(playingCard * pcard){
		if(rightClickOnRightSide(pcard,0)) return true;
		else return rightClickOnLeftSide(pcard,0);
	}
};

