#include "card.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <vector>

using namespace std;
using namespace sf;

void message(RenderWindow& window, const bool pcTakeGard) {
	Font font;
	font.loadFromFile("C:/WINDOWS/Fonts/arial.ttf");
	Text t(L"", font, 70);

	if (pcTakeGard) t.setString(L"Компьютер взял карты");
	else t.setString(L"Компютер сказал: 'Бита!'");
	t.setPosition(600 - t.getGlobalBounds().width / 2, 170);

	int n = 0;
	while (n < 30) {
		window.draw(t);
		window.display();
		n++;
	}
}

Button::Button(const Text& t, const int x, const int y) {
	text = t;
	text.setFillColor(Color::Black);
	text.setCharacterSize(90);
	rectangle.setSize(Vector2f(305.0, 90.0));
	rectangle.setPosition(x, y);
	text.setPosition(x + (305 / 2 - text.getGlobalBounds().width / 2) - 5, y - 10);
}

void Button::Draw(RenderWindow& window) {
	if (MouseHovered(window)) rectangle.setFillColor(Color(255, 243, 240));
	else rectangle.setFillColor(Color(255, 188, 173));
	window.draw(rectangle);
	window.draw(text);
}

Card::Card(const int Price, const int Suit) {
	price = Price;
	suit = Suit;
	textureName = textureName = "img/" + to_string(price) + "_" + to_string(suit) + ".png";
	rectangle.setSize(Vector2f(246, 344));
	rectangle.setOutlineThickness(2);
	rectangle.setOutlineColor(Color::Black);
}

void Card::Draw(RenderWindow& window, const int x, const int y) {
	rectangle.setTexture(&texture);
	rectangle.setPosition(x, y);
	window.draw(rectangle);
}

bool Card::operator >(const Card& c) const {
	if (suit != c.suit && !trump) return false;
	return (price > c.price || (trump && !c.trump));
}

bool Card::operator <(const Card& c) const {
	if (suit != c.suit && !c.trump) return false;
	return (c.price > price || (c.trump && !trump));
}

Deck::Deck() {
	for (int i = 6; i < 15; i++)
		for (int j = 3; j < 7; j++) {
			Card card1(i, j);
			Card* card1_copy = new Card(card1);
			cards.push_back(card1_copy);
		}
	srand((unsigned)time(0));
	int random;
	for (int i = 36; i > 1; i--) {
		random = rand() % i;
		cards.push_back(cards[random]);
		cards.erase(cards.begin() + random);
	}
	for (int i = 0; i < 36; i++) {
		if (cards[i]->OneSuit(cards[cards.size() - 1]->suit)) cards[i]->ThisIsTrump();
	}
	trumpSuit = cards[cards.size() - 1]->suit;
	for (int i = 0; i < cards.size(); i++) {
		cards[i]->Scale(0.5);
		cards[i]->headUp = true;
		cards[i]->SetTexture();
	}
	cards[cards.size() - 1]->Rotate(90);
	cards[cards.size() - 1]->headUp = false;
	cards[cards.size() - 1]->SetTexture();

	texture.loadFromFile("img/" + to_string(cards[cards.size() - 1]->suit) + ".png");
	suit.setSize(Vector2f(123, 172));
	suit.setPosition(336, 10);
}

void Deck::Draw(RenderWindow& window) {
	suit.setTexture(&texture);
	window.draw(suit);
	if (cards.size() > 0) {
		cards[cards.size() - 1]->Draw(window, 520, 30);
		for (int i = 0; i < cards.size() - 1; i++) {
			cards[i]->Draw(window, 520 - 184 + 1.5 * i, 10 - 1.5 * i);
		}
	}
}

bool Arm::TakeCard(Deck& deck, const bool headUp = false) {
	if (deck.cards.size() == 0 || cards.size() >= 6) return true;
	if (deck.cards.size() == 1) deck.cards[0]->Rotate(0);
	cards.push_back(deck.cards[0]);
	cards[cards.size() - 1]->headUp = headUp;
	cards[cards.size() - 1]->SetTexture();
	cards[cards.size() - 1]->Scale(2.0);
	deck.cards.erase(deck.cards.begin());
	return TakeCard(deck, headUp);
}

void Arm::Sort() {
	for (int i = 6; i < 29; i++) {
		int Pass = 0;
		for (int j = 0; j < int(cards.size() - Pass); j++)
			if (cards[j]->Price() == i) {
				cards.push_back(cards[j]);
				cards.erase(cards.begin() + j);
				Pass++; j--;
			}
	}
}

void Table::Draw(RenderWindow& window) {
	for (int i = 0; i < attack.size(); i++) attack[i]->Draw(window, 120 + (150 * i), 250);
	for (int i = 0; i < protection.size(); i++) protection[i]->Draw(window, 120 + (150 * i), 310);
}

// кладем атакующую карту и проверяем можем ли мы её положить
bool Table::PutAttack(const Card& card, const int numberCardsOpponentHas) {
	bool onePrice = (attack.size() == 0);
	if (!onePrice)
		for (int i = 0; i < protection.size(); i++)
			if (protection[i]->OnePrice(card)) { onePrice = true; break; }
	if (!onePrice)
		for (int i = 0; i < attack.size(); i++)
			if (attack[i]->OnePrice(card)) { onePrice = true; break; }
	if (onePrice && numberCardsOpponentHas > attack.size() - protection.size()
		&& (attack.size() < 6 || resetNumber > 0)) {
		Card* card_copy = new Card(card);
		attack.push_back(card_copy);
		attack[attack.size() - 1]->Scale(0.5);
		attack[attack.size() - 1]->headUp = false;
		attack[attack.size() - 1]->SetTexture();
		return true;
	}
	return false;
}

// кладем бьющую карту и проверяем можем ли мы её положить
bool Table::PutProtection(const Card& card) {
	if (protection.size() == attack.size()) return 0;
	if (card > *attack[protection.size()]) {
		Card* card_copy = new Card(card);
		protection.push_back(card_copy);
		protection[protection.size() - 1]->Scale(0.5);
		protection[protection.size() - 1]->headUp = false;
		protection[protection.size() - 1]->SetTexture();
		return true;
	}
	return false;
}

// Передача карт со стола в руку
void Table::GiveAwayCards(Arm& arm, const bool headUp = false) {
	for (int i = 0; i < protection.size(); i++) {
		Card* card_copy = new Card(*protection[i]);
		arm.cards.push_back(card_copy);
		arm.cards[arm.cards.size() - 1]->Scale(2.0);
		arm.cards[arm.cards.size() - 1]->headUp = headUp;
		arm.cards[arm.cards.size() - 1]->SetTexture();
	}
	for (int i = 0; i < attack.size(); i++) {
		Card* card_copy = new Card(*attack[i]);
		arm.cards.push_back(card_copy);
		arm.cards[arm.cards.size() - 1]->Scale(2.0);
		arm.cards[arm.cards.size() - 1]->headUp = headUp;
		arm.cards[arm.cards.size() - 1]->SetTexture();
	}
	protection.clear();
	attack.clear();
}

int Computer::IndexMinCardToAttack() {
	int minInd = 0;
	for (int i = 1; i < cards.size(); i++) if (cards[minInd]->operator> (*cards[i])) minInd = i;
	return minInd;
}

int Computer::IndexMinCardToProtect(const Card& attackCard) {
	int minInd = -1;
	for (int i = 0; i < cards.size(); i++)
		if (cards[i]->operator> (attackCard) && (minInd == -1 || cards[minInd]->operator> (*cards[i])))
			minInd = i;
	return minInd;
}

int Computer::Choose(RenderWindow& window, Table& table, Deck& deck,
	Computer& pc, const int numberCardsOpponentHas) {
	if (strategyNumber == 0) return Eight_Nine(window, table, numberCardsOpponentHas, deck.cards.size());
	else return Smaller(window, table, numberCardsOpponentHas, deck.cards.size());
}

int Computer::Eight_Nine(RenderWindow& window, Table& table, int numberCardsOpponentHas, int numberCardsInDeck) {
	if (IAmOnDefensive && table.attack.size() == table.protection.size()) return 1;
	bool response = false;
	if (IAmOnDefensive) {
		if (numberCardsInDeck > 8) {
			table.GiveAwayCards(*this, true);
			message(window, true);
			return 0;
		}
		else {
			for (int i = table.protection.size(); i < table.attack.size(); i++) {
				int minInd = IndexMinCardToProtect(*table.attack[i]);
				if (minInd > -1) {
					response = table.PutProtection(*cards[minInd]);
					if (response) cards.erase(cards.begin() + minInd);
				}
				else {
					response = false;
					break;
				}
			}
			if (!response) {
				table.GiveAwayCards(*this, true);
				message(window, true);
				return 0;
			}
		}
	}
	else {
		if (table.attack.size() == 0) {
			int minInd = IndexMinCardToAttack();
			response = table.PutAttack(*cards[minInd], numberCardsOpponentHas);
			if (response) {
				cards.erase(cards.begin() + minInd);
				if (numberCardsInDeck > 8) return 1;
			}
		}
		else {
			if (numberCardsInDeck <= 8)
				for (int i = 0; i < cards.size(); i++)
					if (table.PutAttack(*cards[i], numberCardsOpponentHas)) {
						cards.erase(cards.begin() + i);
						i--;
					}
			table.resetNumber++;
			if (table.attack.size() == table.protection.size()) message(window, false);
			return 0;
		}
	}
	return Eight_Nine(window, table, numberCardsOpponentHas, numberCardsInDeck);
}

int Computer::Smaller(RenderWindow& window, Table& table, int numberCardsOpponentHas, int numberCardsInDeck) {
	if (IAmOnDefensive && table.attack.size() == table.protection.size()) return 1;
	bool response = false;
	if (IAmOnDefensive) {
		for (int i = table.protection.size(); i < table.attack.size(); i++) {
			int minInd = IndexMinCardToProtect(*table.attack[i]);
			if (minInd > -1 && (cards[minInd]->Price() - table.attack[i]->Price() < 4 || numberCardsInDeck < 9)) {
				response = table.PutProtection(*cards[minInd]);
				if (response) cards.erase(cards.begin() + minInd);
			}
			else {
				response = false;
				break;
			}
		}
		if (!response) {
			table.GiveAwayCards(*this, true);
			message(window, true);
			return 0;
		}
	}
	else {
		if (table.attack.size() == 0) {
			int minInd = IndexMinCardToAttack();
			response = table.PutAttack(*cards[minInd], numberCardsOpponentHas);
			if (response) cards.erase(cards.begin() + minInd);
		}
		else {
			for (int i = 0; i < cards.size(); i++)
				if (!cards[i]->trump || numberCardsInDeck < 9)
					if (table.PutAttack(*cards[i], numberCardsOpponentHas)) {
						cards.erase(cards.begin() + i);
						i--;
					}
			table.resetNumber++;
			if (table.attack.size() == table.protection.size()) message(window, false);
			return 0;
		}
	}
	return Smaller(window, table, numberCardsOpponentHas, numberCardsInDeck);
}

void Computer::Draw(RenderWindow& window) {
	for (int i = 0; i < cards.size(); i++) cards[i]->Draw(window, 650 + i * (300 / cards.size()), -200);
}

void Player::Draw(RenderWindow& window) {
	int activeСard = -1;
	for (int i = 0; i < cards.size(); i++) {
		cards[i]->SetPosition(50 + i * (580 / cards.size()), 600);
		if (cards[i]->MouseHovered(window)) activeСard = i;
	}
	for (int i = 0; i < cards.size(); i++)
		if (i != activeСard) cards[i]->Draw(window, 50 + i * (580 / cards.size()), 600);
	if (activeСard > -1) cards[activeСard]->Draw(window, 50 + activeСard * (580 / cards.size()), 550);
}

int Player::Choose(RenderWindow& window, Table& table, Deck& deck,
	Computer& pc, const int numberCardsOpponentHas) {
	if ((IAmOnDefensive && table.attack.size() == table.protection.size()) || cards.size() == 0) return 1;

	int number = -2;
	Event event;

	Font font;
	font.loadFromFile("C:/WINDOWS/Fonts/arial.ttf");
	Text conf(L"Пас", font, 90), ex(L"Выйти", font, 90);
	if (IAmOnDefensive) conf.setString(L"Взять");
	Button Confirm(conf, 900, 650);
	Button Exit(ex, 10, 10);

	conf.setString(L"Ваш ход");
	conf.setCharacterSize(90);
	conf.setFillColor(Color(59, 127, 59));
	conf.setPosition(600 - conf.getGlobalBounds().width / 2, 310 - conf.getGlobalBounds().height / 2);

	while (number == -2) {
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed) { window.close(); exit(0); }
			if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
				if (Confirm.MouseHovered(window)) number = 0;
				if (Exit.MouseHovered(window)) number = -1;
				if (number > -2) break;
				for (int i = 0; i < cards.size(); i++) if (cards[i]->MouseHovered(window)) number = i + 1;
			}
		}
		window.clear(Color(0, 64, 0));

		window.draw(conf);
		deck.Draw(window);
		table.Draw(window);
		pc.Draw(window);
		Draw(window);

		Confirm.Draw(window);
		Exit.Draw(window);

		window.display();

		bool responce;

		if (number == 0) {
			if (IAmOnDefensive) {
				table.GiveAwayCards(*this);
				return 0;
			}
			else {
				table.resetNumber++;
				if (table.attack.size() > 0) return false;
				else return Choose(window, table, deck, pc, numberCardsOpponentHas);
			}
		}
		if (number > -1) {
			if (IAmOnDefensive) responce = table.PutProtection(*cards[number - 1]);
			else responce = table.PutAttack(*cards[number - 1], numberCardsOpponentHas);
			if (responce) cards.erase(cards.begin() + number - 1);
		}
		if (number == -1) { return 2; }
	}
	return Choose(window, table, deck, pc, numberCardsOpponentHas);
}