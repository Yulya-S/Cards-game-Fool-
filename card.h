#ifndef _CARD_
#define _CARD_

#include <iostream>
using namespace std;

enum ConsoleColor {
	Cyan = 3,
	DarkGray = 8,
	LightGreen = 10,
	LightCyan = 11,
	LightRed = 12,
	Yellow = 14,
	White = 15
};

void SetColor(ConsoleColor text);

class Card {
	char name;
	int price;
	bool trump;
public:
	char suit;
	void naming(int Price, char Suit);
	bool Trump() const;
	bool ruleOfDifference(Card c) const;
	void itTrump();
	Card& operator =(const Card& c);
	bool operator >(const Card& c) const;
	bool operator ==(const Card& c) const;
	friend ostream& operator <<(ostream& out, const Card c);
	friend class Koloda;
};

class Ruka {
	Card card[36];
	int needTakeCard = 0;
public:
	int kolvo() const;
	int psThingFirstAttack();
	bool amITakeCard() const;
	void deliteCard(int i);
	friend class Koloda;
	friend class Table;
	friend ostream& operator <<(ostream& out, const Ruka k);
};

class Koloda {
	int topGranKolva = 10;
	bool whoGo; //если true - игрок, false - компьютер
public:
	int nomer;
	char trump;
	Card card[36];
	void clear();
	void trumps();
	bool WhoGo() const;
	bool TopGranKolva() const;
	void goNext();
	void takeCard(Ruka& a, Ruka& b);
	bool endGame(Ruka a, Ruka b) const;
	void whoWiner(Ruka a, Ruka b);
	friend class Table;
	friend ostream& operator <<(ostream& out, const Koloda k);
};

class Table {
	Card attack[36], protect[36];
	bool dubleGo = false;
public:
	int kolvoAttack = 0, kolvoProtect = 0;
	void clear();
	bool pustota() const;
	bool proverka(Ruka a, int i) const;
	bool endHod() const;
	void trueDubleGo();
	bool DubleGo() const;
	void writeElem(Ruka& a, int i, bool isItProtect);
	void iFoundProtect(Ruka& a, Koloda kolod);
	void searchSimilarCard(Ruka& a, const Koloda koloda, const Ruka& pl);
	void proverkaProtecterCard(Ruka& a, int i, Koloda koloda);
	void iTakeAllCard(Ruka& a);
	friend ostream& operator <<(ostream& out, const Table t);
};

void cardEnding(int i);

#endif