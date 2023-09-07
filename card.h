#ifndef _CARD_
#define _CARD_

using namespace sf;
using namespace std;

void message(RenderWindow& window, const bool pcTakeGard);

class Button {
	Text text;
	RectangleShape rectangle;
public:
	Button(const Text& t, const int x, const int y);
	bool MouseHovered(const RenderWindow& window) const { return (rectangle.getGlobalBounds().contains(window.mapPixelToCoords(Mouse::getPosition(window)))); };
	void SetPosition(const int x, const int y) { rectangle.setPosition(x, y); };
	void SetString(const Text& t) { text = t; };
	void Draw(RenderWindow& window);
};

class Card {
	int price;
	RectangleShape rectangle;
	string textureName;
	Texture texture;
public:
	bool headUp = false;
	bool trump = false;
	int suit; // 3, 4, 5, 6
	Card(const int Price, const int Suit);
	void Draw(RenderWindow& window, const int x, const int y);

	// Изменения
	void SetTexture() {
		if (headUp) texture.loadFromFile("img/head_up.png");
		else texture.loadFromFile(textureName);
	}
	void SetPosition(const int x, const int y) { rectangle.setPosition(x, y); }
	void Rotate(const int angle) { rectangle.setRotation(angle); }
	void Scale(const float meaning) { rectangle.scale(meaning, meaning); };
	void ThisIsTrump() { trump = true; };

	// Проверки
	bool MouseHovered(const RenderWindow& window) const { return (rectangle.getGlobalBounds().contains(window.mapPixelToCoords(Mouse::getPosition(window)))); };
	bool operator >(const Card& c) const;
	bool operator <(const Card& c) const;
	int Price() const {
		int pr = trump ? 14 : 0;
		return price + pr;
	};
	bool OnePrice(const Card& c) const { return (price == c.price); };
	bool OneSuit(const char c) const { return (suit == c); };
};

class Deck {
	RectangleShape suit;
	Texture texture;
public:
	char trumpSuit;
	vector<Card*> cards;
	Deck();
	void Draw(RenderWindow& window);
};

class Arm {
public:
	bool IAmOnDefensive = true;
	vector<Card*> cards;
	bool TakeCard(Deck& deck, const bool headUp);
	void Sort();
};

class Table {
public:
	int resetNumber = 0;
	vector<Card*> protection;
	vector<Card*> attack;
	void Draw(RenderWindow& window);
	//int AttackSize() { return attack.size(); };
	//int ProtectionSize() { return protection.size(); };
	bool PutAttack(const Card& card, const int numberCardsOpponentHas);
	bool PutProtection(const Card& card);
	void GiveAwayCards(Arm& arm, const bool headUp);
	void Clear() { attack.clear(); protection.clear(); };
};

class Computer : public Arm {
	int strategyNumber;
public:
	Computer() {
		srand((unsigned)time(0));
		strategyNumber = rand() % 2;
	};
	void Draw(RenderWindow& window);
	int Choose(RenderWindow& window, Table& table, Deck& deck, Computer& pc, const int numberCardsOpponentHas);
	void ChoosingStrategy();
	int IndexMinCardToAttack();
	int IndexMinCardToProtect(const Card& attackCard);

	// Функции стратегий
	int Eight_Nine(RenderWindow& window, Table& table, int numberCardsOpponentHas, int numberCardsInDeck);
	int Smaller(RenderWindow& window, Table& table, int numberCardsOpponentHas, int numberCardsInDeck);
};

class Player : public Arm {
public:
	void Draw(RenderWindow& window);
	int Choose(RenderWindow& window, Table& table, Deck& deck, Computer& pc, const int numberCardsOpponentHas);
};

#endif