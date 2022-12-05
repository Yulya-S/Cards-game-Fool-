#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

#include <iostream>
#include "card.h"
using namespace std;

int main() {
	//изменения внешнего вида консоли
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_FONT_INFOEX fontInfo;
	fontInfo.cbSize = sizeof(fontInfo);
	GetCurrentConsoleFontEx(hConsole, TRUE, &fontInfo);
	wcscpy(fontInfo.FaceName, L"Lucida Console");
	fontInfo.dwFontSize.Y = 18;
	SetCurrentConsoleFontEx(hConsole, TRUE, &fontInfo);

	SetColor(White);

	setlocale(LC_ALL, "Russian");
	Table table;
	Koloda koloda;
	Ruka ps, player;
	int wibor = -1;
	koloda.clear();
	koloda.trumps();
	if (koloda.WhoGo()) koloda.takeCard(player, ps);
	else koloda.takeCard(ps, player);

	//cout << koloda << endl; //Как замешана колода

	while (koloda.endGame(ps, player)) {
		if (koloda.WhoGo()) {
			SetColor(LightGreen);
			cout << "Ходит: Игрок" << endl;
			SetColor(Yellow);
			cout << "Вводите номера выбранных карт(каждую подтверждайте нажатием кнопки enter), для окончания хода введите 0." << endl << endl;
			SetColor(White);
			while (wibor != -2) {
				wibor = -1;
				while (wibor != 0) {
					cout << "У компьютера: " << ps.kolvo();
					cardEnding(ps.kolvo());
					cout<<"В колоде осталось: " << 36 - koloda.nomer;
					cardEnding(36 - koloda.nomer);
					cout << "Козырная масть это: ";
					if (koloda.trump == 5 || koloda.trump == 6) SetColor(DarkGray);
					else SetColor(LightRed);
					cout << koloda.trump << endl;
					cout << player << endl;
					if (!table.pustota()) cout << table << endl;
					cin >> wibor;
					cout << endl;
					if ((table.pustota() && wibor == 0) || wibor > player.kolvo()) {
						if (wibor > player.kolvo()) cout << "Введённое вами число превышает кол-во карт у вас в руке" << endl;
						else cout << "Вы не можете закончить ход сейчас, вам нужно положить хотя бы одну карту" << endl;
						cout << endl;
						wibor = -1;
					}
					if (table.pustota() || table.proverka(player, wibor - 1)) table.writeElem(player, wibor - 1, false);
					if (table.kolvoAttack - (ps.kolvo() + table.kolvoProtect) == 0) { wibor = 0; }
				}
				if (table.endHod()) break;
				table.iFoundProtect(ps, koloda);
				if (table.kolvoAttack - (ps.kolvo() + table.kolvoProtect) == 0) break;
				if (table.pustota()) break;
			}
		}
		else {
			SetColor(LightGreen);
			cout << "Ходит: Компьютер" << endl;
			SetColor(Yellow);
			cout << "Вводите номера выбранных карт, по порядку карт на столе (каждую подтверждайте нажатием кнопки enter), введите 0 если хотите взять карты со стола." << endl << endl;
			SetColor(White);
			while (wibor != -2) {
				if (table.pustota()) {
					wibor = ps.psThingFirstAttack();
					table.writeElem(ps, wibor, false);
				}
				table.searchSimilarCard(ps, koloda, player);
				if (table.endHod()) break;
				while (wibor != -2) {
					cout << "Козырная масть это: ";
					if (koloda.trump == 5 || koloda.trump == 6) SetColor(DarkGray);
					else SetColor(LightRed);
					cout << koloda.trump << endl;
					cout << table << endl;
					cout << player << endl;
					cin >> wibor;
					cout << endl;
					if (wibor > player.kolvo()) cout << "Введённое вами число превышает кол-во карт у вас в руке" << endl << endl;
					if (wibor<=player.kolvo()) table.proverkaProtecterCard(player, wibor-1, koloda);
					if (table.endHod() || table.DubleGo()) break;
				}
				if (table.DubleGo()) break;
			}
		}
		table.clear();
		if (koloda.WhoGo()) koloda.takeCard(player, ps);
		else koloda.takeCard(ps, player);
		system("cls");
		SetColor(Yellow);
		if (table.DubleGo()) {
			if (koloda.WhoGo()) cout << "Я возьму." << endl;
			else cout << "Вы взяли карты." << endl;
			table.trueDubleGo();
		}
		else {
			cout << "Бита!" << endl;
			koloda.goNext();
		}
		SetColor(White);
	}
	system("cls");
	koloda.whoWiner(player, ps);
	system("PAUSE>nul");
}
