#include <SFML/Graphics.hpp>
#include <iostream>
#include <Windows.h>
#include "card.cpp"

using namespace sf;
using namespace std;

void victoryScreen(RenderWindow& window, const bool playerWon) {
    Font font;
    font.loadFromFile("C:/WINDOWS/Fonts/arial.ttf");
    Text t(L"", font, 70);

    if (playerWon) t.setString(L"Поздравляю! Вы выиграли!");
    else t.setString(L"Спасибо за игру!");
    t.setPosition(600 - t.getGlobalBounds().width / 2, 250);

    int n = 0;
    while (n < 120) {
        window.clear(Color(0, 64, 0));
        window.draw(t);
        window.display();
        n++;
    }
}

template<typename Type>
int battle(RenderWindow& window, Table& table, Player& pl_1, Type& pl_2, Deck& deck, bool attack = true) {
    int response = 1;
    if (attack) {
        if (!pl_1.IAmOnDefensive) response = pl_1.Choose(window, table, deck, pl_2, pl_2.cards.size());
        else response = pl_2.Choose(window, table, deck, pl_2, pl_1.cards.size());
    }
    else {
        if (pl_1.IAmOnDefensive) response = pl_1.Choose(window, table, deck, pl_2, pl_2.cards.size());
        else response = pl_2.Choose(window, table, deck, pl_2, pl_1.cards.size());
    }

    if ((pl_1.cards.size() == 0 || pl_2.cards.size() == 0) && table.attack.size() == table.protection.size()) {
        table.Clear();
        return 1;
    }
    if (response == 2) { return response; };
    if (response == 1 || table.attack.size() != table.protection.size()) return battle(window, table, pl_1, pl_2, deck, !attack);
    if (attack && response == 0) {
        pl_1.IAmOnDefensive = !pl_1.IAmOnDefensive;
        pl_2.IAmOnDefensive = !pl_2.IAmOnDefensive;
    }
    table.Clear();
    return 1;
}

void Game(RenderWindow& window) {
    Deck deck;
    Table table;
    Player pl_1;
    Computer pl_2;
    srand((unsigned)time(0));
    if (rand() % 2 == 0) {
        pl_1.IAmOnDefensive = false;
        pl_1.TakeCard(deck);
        pl_2.TakeCard(deck, true);
    }
    else {
        pl_2.IAmOnDefensive = false;
        pl_2.TakeCard(deck, true);
        pl_1.TakeCard(deck);
    }
    pl_1.Sort();
    pl_2.Sort();
    while (deck.cards.size() > 0 || (pl_1.cards.size() > 0 && pl_2.cards.size() > 0)) {

        if (battle(window, table, pl_1, pl_2, deck) == 2) {
            break;
        }
        if (pl_1.IAmOnDefensive) {
            pl_2.TakeCard(deck, true);
            pl_1.TakeCard(deck);
        }
        else {
            pl_1.TakeCard(deck);
            pl_2.TakeCard(deck, true);
        }
        pl_1.Sort();
        pl_2.Sort();
    }
    if (pl_1.cards.size() == 0 || pl_2.cards.size() == 0) victoryScreen(window, (pl_1.cards.size() == 0));
}

int main()
{
    setlocale(LC_ALL, "Russian");

    RenderWindow window(VideoMode(1280, 768), L"Новый проект", Style::Default);
    window.setVerticalSyncEnabled(true);

    Font font;
    font.loadFromFile("C:/WINDOWS/Fonts/arial.ttf");
    Text st(L"Начать", font, 90), ex(L"Выйти", font, 90);
    Button Start(st, 470, 270);
    Button Exit(ex, 470, 450);

    Text name(L"Игра в 'Дурака'", font, 90), proprogrammer(L"Разработчик: Yunabi", font, 30), version(L"Версия: 3.0", font, 30);
    name.setPosition(600 - name.getGlobalBounds().width / 2, 100);
    proprogrammer.setPosition(15, 730);
    version.setPosition(15, 680);

    Image icon;
    if (!icon.loadFromFile("img/icon.png")) return 1;
    window.setIcon(32, 32, icon.getPixelsPtr());

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed) window.close();
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                if (Exit.MouseHovered(window)) window.close();
                if (Start.MouseHovered(window)) Game(window);
            }
        }
        window.clear(Color(0, 64, 0));
        Start.Draw(window);
        Exit.Draw(window);
        window.draw(name);
        window.draw(proprogrammer);
        window.draw(version);
        window.display();
    }
    return 0;
}
