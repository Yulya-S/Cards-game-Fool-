#include "card.h"
#include <Windows.h>
using namespace std;

void SetColor(ConsoleColor text) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | text));
}

void Card::naming(int Price, char Suit){
	if (Price > 9)
		switch (Price){
		case 10:
			name = '0';
			break;
		case 11:
			name = 'B';
			break;
		case 12:
			name = 'D';
			break;
		case 13:
			name = 'K';
			break;
		case 14:
			name = 'T';
			break;
		case 16: //Фальшифое значение карты нужно, для выбора подходящей для хода карты компьютера
			name = 'F';
			break;
		}
	else name = Price + '0';
	price = Price;
	suit = Suit;
	trump = false;
}

bool Card::Trump() const { return trump; }
bool Card::ruleOfDifference(Card c) const { return (c.price - price > 4); }
void Card::itTrump() { trump = true; } //отметка о козырности карты

Card& Card::operator =(const Card& c) {
	name = c.name;
	suit = c.suit;
	price = c.price;
	trump = c.trump;
	return *this;
}

bool Card::operator >(const Card& c) const {
	if (suit != c.suit && (!trump) && (!c.trump)) return false;
	if (c.trump) return ((price > c.price) && (trump));
	else return ((price > c.price) || (trump));
}
bool Card::operator ==(const Card& c) const { return (price == c.price); }

ostream& operator <<(ostream& out, const Card c) {
	if (c.name == '0') out << "10";
	else out << c.name;
	if (c.suit == 5 || c.suit == 6) SetColor(DarkGray);
	else SetColor(LightRed);
	out << c.suit;
	SetColor(White);
	return out;
}

int Ruka::kolvo() const { return needTakeCard; }
int Ruka::psThingFirstAttack() {
	Card min; int n;
	min.naming(16, 2);
	min.itTrump();
	for (int i = 0; i < needTakeCard; i++)
		if (min > card[i]) {
			min = card[i];
			n = i;
		}
	return n;
}
bool Ruka::amITakeCard() const { return (needTakeCard < 6); }
void Ruka::deliteCard(int i) {
	needTakeCard--;
	card[i] = card[needTakeCard];
}

ostream& operator <<(ostream& out, const Ruka r) {
	for (int i = 0; i < r.needTakeCard; i++) {
		SetColor(Cyan);
		out << (i + 1) << ".";
		SetColor(White);
		out << r.card[i] << "\t";
	}
	return out;
}

void Koloda::clear() { //очистка колоды(сортировка)/перемешивание колоды
	nomer = 0;
	int n = 0;
	for (int i = 6; i < 15; i++)
		for (int j = 3; j < 7; j++) {
			card[n].naming(i, j);
			n++;
		}
	srand((unsigned)time(0));
	whoGo = (rand() % 2 == 0);
	int i; Card rem;
	while (n != 0) {
		i = rand() % n;
		rem = card[i];
		card[i] = card[n - 1];
		card[n - 1] = rem;
		n--;
	}
	trumps();
}
void Koloda::trumps() {
	for (int i = 0; i < 36; i++) if (card[i].suit == card[35].suit) card[i].itTrump();
	trump = card[35].suit;
}
void Koloda::takeCard(Ruka& a, Ruka& b) {
	for (nomer; nomer != 36 && a.amITakeCard(); nomer++, a.needTakeCard++) a.card[a.needTakeCard] = card[nomer];
	for (nomer; nomer != 36 && b.amITakeCard(); nomer++, b.needTakeCard++) b.card[b.needTakeCard] = card[nomer];
}
bool Koloda::WhoGo() const { return whoGo; }
bool Koloda::TopGranKolva() const { return (36 - nomer > topGranKolva); }
bool Koloda::endGame(Ruka a, Ruka b) const { return ((nomer != 36) || ((a.needTakeCard != 0) && (b.needTakeCard != 0))); }
void Koloda::goNext() { whoGo = !whoGo; }
void Koloda::whoWiner(Ruka a, Ruka b) {
	setlocale(LC_ALL, "Russian");
	if ((a.needTakeCard == 0) && (b.needTakeCard == 0)) cout << "Поздравляю у нас ничья!" << endl;
	else {
		if (a.needTakeCard == 0) cout << "Поздравляю вы выиграли)" << endl;
		else cout << "Вы проиграли, ничего, в следующий раз получится лучше." << endl;
	}
}

ostream& operator <<(ostream& out, const Koloda k) {
	for (int i = 0; i < 36; i++) out << k.card[i] << ' ';
	return out;
}
void Table::clear() { kolvoAttack = kolvoProtect = 0; }
bool Table::pustota() const { return (kolvoAttack == 0); }
bool Table::proverka(Ruka a, int n) const {
	setlocale(LC_ALL, "Russian");
	for (int i = 0; i < kolvoAttack; i++) if (a.card[n] == attack[i]) return true;
	for (int i = 0; i < kolvoProtect; i++) if (a.card[n] == protect[i]) return true;
	if (n == 0) cout << "На столе нет карт похожих на выбранную карту!" << endl;
	return false;
}
bool Table::endHod() const { return (kolvoAttack == kolvoProtect); }
void Table::trueDubleGo() { dubleGo=!dubleGo; }
bool Table::DubleGo() const { return dubleGo; }
void Table::writeElem(Ruka& a, int i, bool isItProtect) { //если true - то карта кладётся на сторону защиты, false - атака
	if (i > -1) {
		if (isItProtect) {
			protect[kolvoProtect] = a.card[i];
			kolvoProtect++;
		}
		else {
			attack[kolvoAttack] = a.card[i];
			kolvoAttack++;
		}
		a.deliteCard(i);
	}
}
void Table::iFoundProtect(Ruka& a, Koloda koloda) {
	Card min; int n;
	min.naming(16, 2);
	min.itTrump();
	int i = kolvoProtect;
	for (i; i < kolvoAttack; i++) {
		n = -1;
		for (int j = 0; j < a.needTakeCard; j++) {
			if ((a.card[j] > attack[i]) && (n == -1 || (a.card[n] > a.card[j]))) {
				min = a.card[j];
				n = j;
			}
		}
		if ((min.Trump() && attack[i].Trump()) || (min.Trump() && !attack[i].Trump() && (!koloda.TopGranKolva() || (min.ruleOfDifference(attack[i])))) || !min.Trump())
			writeElem(a, n, true);
		else n = -1;
		if (n == -1) {
			iTakeAllCard(a);
			trueDubleGo();
			break;
		}
	}
}
void Table::searchSimilarCard(Ruka& a, const Koloda koloda, const Ruka& pl) {
	for (int i = 0; i < kolvoAttack; i++) {
		for (int j = 0; j < a.needTakeCard; j++) {
			if (kolvoAttack - (pl.kolvo() + kolvoProtect) == 0) break;
			if ((a.card[j] == attack[i]) || ((i < kolvoProtect) && (a.card[j] == protect[i])))
				if (!a.card[j].Trump() || !koloda.TopGranKolva())
					writeElem(a, j, false);
		}
		if (kolvoAttack - (pl.kolvo() + kolvoProtect) == 0) break;
	}
}
void Table::proverkaProtecterCard(Ruka& a, int n,  Koloda koloda) {
	setlocale(LC_ALL, "Russian");
	if (n < 0) {
		iTakeAllCard(a);
		trueDubleGo();
	}
	else {
		if (a.card[n] > attack[kolvoProtect]) writeElem(a, n, true);
		else cout << "Выбранная вами карта не подходит что бы побить карту на столе!" << endl << endl;
	}
}
void Table::iTakeAllCard(Ruka& a) {
	for (int i = 0; i < kolvoAttack; i++, a.needTakeCard++) a.card[a.needTakeCard] = attack[i];
	for (int i = 0; i < kolvoProtect; i++, a.needTakeCard++) a.card[a.needTakeCard] = protect[i];
	kolvoAttack = kolvoProtect = 0;
}

ostream& operator <<(ostream& out, const Table t) {
	SetColor(LightCyan);
	cout << "Стол: ";
	SetColor(White);
	for (int i = 0; i < t.kolvoAttack; i++) {
		out << t.attack[i];
		if (i < t.kolvoProtect) out << " | " << t.protect[i];
		out << ";\t";
	}
	return out;
}

void cardEnding(int i) {
	if (i % 10 == 0 || (i > 4 && i < 20)) cout << " карт;\t";
	else {
		if (i % 10 == 1) cout << " карта;\t";
		else cout << " карты;\t";
	}
}