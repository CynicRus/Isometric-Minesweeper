#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <set>
#include <string>
#include "Animation.hpp"

//#define DEBUG

enum GameState { g_played, g_endgame };

struct Cell {
	float x, y;
	int state; //current state of the cell

};


//game field
int SX = 0, SY = 0;
int ci = 0, cj = 0;//clicked cell index
float stx = 0, sty = 0;//move mouse cell index
Cell map[16][16];
int BombMap[16][16];
// counters, etc
int BombCount = 0;
bool boom = false, win = false;
GameState currentState;
unsigned char mode = 0;


//making game field
void buildField(int Cx, int Cy, int nbrx, int nbry, sf::Sprite s) {
	for (int stop = nbrx - 1, k = 0; k <= stop; k++)
		for (int stop = nbry - 1, k2 = 0; k2 <= stop; k2++)
		{
			{

				sf::FloatRect bounds = s.getGlobalBounds();
				map[k][k2].x = Cx + (bounds.width * (k + k2) / 2);
				map[k][k2].y = Cy + (bounds.height * (k - k2) / 2);
				map[k][k2].state = 0;
			}
		}


}

//create bombs
void placeBombs()
{
	int k = 0, k2 = 0, k3 = 0;
	for (int stop = 15, k = 0; k <= stop; k++)
		for (int stop = 15, k2 = 0; k2 <= stop; k2++)
			map[k][k2].state = 0;
	for (int stop = 15, k = 0; k <= stop; k++)
		for (int stop = 15, k2 = 0; k2 <= stop; k2++)
			BombMap[k][k2] = 0;
	BombCount = 16;
	do
	{
		k2 = rand() % 15;
		k3 = rand() % 15;
#ifdef DEBUG
		std::cout << "k2 " << k2 << " k3 " << k3 << std::endl;
#endif
		if (BombMap[k2][k3] != 15)
		{
			BombMap[k2][k3] = 15;
			BombCount--;
		}
	} while (!(BombCount == 0));
	for (int stop = 15, k = 0; k <= stop; k++)
		for (int stop = 15, k2 = 0; k2 <= stop; k2++)
			if (BombMap[k][k2] == 15)
			{
				if ((BombMap[k + 1][k2] != 15) && (k < 15))
					BombMap[k + 1][k2]++;
				if ((BombMap[k][k2 + 1] != 15) && (k2 < 15))
					BombMap[k][k2 + 1]++;
				if ((BombMap[k + 1][k2 + 1] != 15) && (k2 < 15) && (k < 15))
					BombMap[k + 1][k2 + 1]++;
				if ((BombMap[k - 1][k2] != 15) && (k > 0))
					BombMap[k - 1][k2]++;
				if ((BombMap[k][k2 - 1] != 15) && (k2 > 0))
					BombMap[k][k2 - 1]++;
				if ((BombMap[k + 1][k2 - 1] != 15) && (k2 > 0) && (k < 15))
					BombMap[k + 1][k2 - 1]++;
				if ((BombMap[k - 1][k2 + 1] != 15) && (k > 0) && (k2 < 15))
					BombMap[k - 1][k2 + 1]++;
				if ((BombMap[k - 1][k2 - 1] != 15) && (k > 0) && (k2 > 15))
					BombMap[k - 1][k2 - 1]++;
			}
	boom = false;
	win = false;
	currentState = g_played;
}

void addClearField(int a, int b, int& ClearCount, int* ClearC)
{
	bool fieldexist = false;
	int w = 0;
	fieldexist = false;
	for (int stop = ClearCount, w = 1; w <= stop; w++)
		if (ClearC[w - 1] == a * 15 + b)
			fieldexist = true;
	if (!fieldexist)
	{
		ClearCount++;
		ClearC[ClearCount - 1] = a * 15 + b;
	}
}

void gameUpdate()
{

	int k = 0, k2 = 0, k11 = 0, k22 = 0;
	int ClearC[225];
	int ClearCount = 0  //ClearCount - count of the empty cells
		, Pointer = 0;
	if ((!boom) && (!win))
	{
		ClearCount = 0;
		Pointer = 0;
		k = ci;
		k2 = cj;

		int X = SX;
		int Y = SY;
		if ((mode == 1) && (BombMap[k][k2] != 15) && (map[k][k2].state == 2))
		{
			mode = 3;
			map[k][k2].state = 1;
			BombCount--;
		}
		else
			if ((mode == 1) && (BombMap[k][k2] != 15))
			{
				map[k][k2].state = 1;
				mode = 3;
			}
			else if ((mode == 1) && (BombMap[k][k2] == 15))
			{
				boom = true;
				int bx = k;
				int by = k2;
				mode = 0;
			}
		if ((mode == 2) && (map[k][k2].state == 0) && (BombCount < 16))
		{
			map[k][k2].state = 2;
			BombCount++;
			mode = 0;
		}
		if ((mode == 2) && (map[k][k2].state == 2) && (BombCount > 0))
		{
			map[k][k2].state = 0;
			BombCount--;
			mode = 0;
		}
		if ((BombMap[k][k2] == 0) && (mode == 3))
		{
			ClearCount++;
			ClearC[ClearCount - 1] = ci * 15 + cj;
			Pointer = 1;
		}
		while ((ClearCount < 225) && (ClearCount > 0)) //open the empty cells
		{
			k = ClearC[Pointer - 1] / 15;
			k2 = ClearC[Pointer - 1] % 15;
			if (k < 15)
			{
				if ((BombMap[k + 1][k2] < 15) && (map[k + 1][k2].state != 2))
					map[k + 1][k2].state = 1;
				if ((BombMap[k + 1][k2] == 0) && (map[k + 1][k2].state != 2))
				{
					k11 = k + 1;
					addClearField(k11, k2, ClearCount, ClearC);
				}
			}
			if (k2 < 15)
			{
				if ((BombMap[k][k2 + 1] < 15) && (map[k][k2 + 1].state != 2))
					map[k][k2 + 1].state = 1;
				if ((BombMap[k][k2 + 1] == 0) && (map[k][k2 + 1].state != 2))
				{
					k22 = k2 + 1;
					addClearField(k, k22, ClearCount, ClearC);
				}
			}
			if ((k2 < 15) && (k < 15))
			{
				if ((BombMap[k + 1][k2 + 1] < 15) && (map[k + 1][k2 + 1].state != 2))
					map[k + 1][k2 + 1].state = 1;
				if ((BombMap[k + 1][k2 + 1] == 0) && (map[k + 1][k2 + 1].state != 2))
				{
					k22 = k2 + 1;
					k11 = k + 1;
					addClearField(k11, k22, ClearCount, ClearC);
				}
			}
			if (k > 0)
			{
				if ((BombMap[k - 1][k2] < 15) && (map[k - 1][k2].state != 2))
					map[k - 1][k2].state = 1;
				if ((BombMap[k - 1][k2] == 0) && (map[k - 1][k2].state != 2))
				{
					k11 = k - 1;
					addClearField(k11, k2, ClearCount, ClearC);
				}
			}
			if ((k > 0) && (k2 > 0))
			{
				if ((BombMap[k - 1][k2 - 1] < 15) && (map[k - 1][k2 - 1].state != 2))
					map[k - 1][k2 - 1].state = 1;
				if ((BombMap[k - 1][k2 - 1] == 0) && (map[k - 1][k2 - 1].state != 2))
				{
					k11 = k - 1;
					k22 = k2 - 1;
					addClearField(k11, k22, ClearCount, ClearC);
				}
			}
			if (k2 > 0)
			{
				if ((BombMap[k][k2 - 1] < 15) && (map[k][k2 - 1].state != 2))
					map[k][k2 - 1].state = 1;
				if ((BombMap[k][k2 - 1] == 0) && (map[k][k2 - 1].state != 2))
				{
					k22 = k2 - 1;
					addClearField(k, k22, ClearCount, ClearC);
				}
			}
			if ((k > 0) && (k2 < 15))
			{
				if ((BombMap[k - 1][k2 + 1] < 15) && (map[k - 1][k2 + 1].state != 2))
					map[k - 1][k2 + 1].state = 1;
				if ((BombMap[k - 1][k2 + 1] == 0) && (map[k - 1][k2 + 1].state != 2))
				{
					k11 = k - 1;
					k22 = k2 + 1;
					addClearField(k11, k22, ClearCount, ClearC);
				}
			}
			if ((k < 15) && (k2 > 0))
			{
				if ((BombMap[k + 1][k2 - 1] < 15) && (map[k + 1][k2 - 1].state != 2))
					map[k + 1][k2 - 1].state = 1;
				if ((BombMap[k + 1][k2 - 1] == 0) && (map[k + 1][k2 - 1].state != 2))
				{
					k11 = k + 1;
					k22 = k2 - 1;
					addClearField(k11, k22, ClearCount, ClearC);
				}
			}
			Pointer++;
			if (Pointer > ClearCount)
				ClearCount = 225; //Leave the loop
		}
	}
	k11 = 0;
	for (int stop = 15, k = 0; k <= stop; k++)
		for (int stop = 15, k2 = 0; k2 <= stop; k2++)
			if ((map[k][k2].state) == 0)
				k11 = 1;
	if ((k11 == 0) && (BombCount == 16))
		win = true;
}

void GameTick() {
	int k = 0, k2 = 0, k11 = 0, k22 = 0;
	if (currentState != g_endgame) {
		gameUpdate();
		if (boom)
		{
			for (int stop = 15, k = 0; k <= stop; k++)
				for (int stop = 15, k2 = 0; k2 <= stop; k2++)
					if (BombMap[k][k2] == 15)
						map[k][k2].state = 5;
#ifdef DEBUG

			std::cout << "you lose" << std::endl;
#endif // DEBUG
			currentState = g_endgame;
		}
		if (win) {
#ifdef DEBUG
			std::cout << "you win" << std::endl;
#endif
			currentState = g_endgame;
		}
		mode = 0;
	}
}



int main()
{
	srand(time(NULL));
	sf::RenderWindow window(sf::VideoMode(1024, 640), "Achtung! Minen!");
	sf::Texture t, e, b, tf, expl, s, tr;
	sf::Vector2f mouse;
	t.loadFromFile("resources/grass_tile_a.png");
	tf.loadFromFile("resources/grass_tile_flag.png");
	e.loadFromFile("resources/empty_cell.png");
	b.loadFromFile("resources/bomb_cell.png");
	s.loadFromFile("resources/grass_tile_selected.png");
	tr.loadFromFile("resources/tile_rect.png");

	expl.loadFromFile("resources/explosion.png");
	AnimationManager explosion;
	explosion.create("explosion", expl, 0, 0, 64, 64, 15, 0.045, 1, false, false);
	explosion.set("explosion");

	sf::Font font, msgfont;
	if (!font.loadFromFile("resources/Arial-Black.ttf"))
	{

	}

	if (!msgfont.loadFromFile("resources/yoshisst.ttf"))
	{

	}

	sf::Text text;
	sf::Text bombText, endGameText;

	bombText.setFont(msgfont);
	endGameText.setFont(msgfont);

	text.setFont(font);


	sf::Sprite grass(t);
	sf::Sprite dirt(e);
	sf::Sprite bomb(b);
	sf::Sprite flag(tf);
	sf::Sprite selectedRect(s);
	sf::Sprite tileRect(tr);
	buildField(0, 300, 16, 16, grass);
	placeBombs();
	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed ||
				(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				window.close();

			if (event.type == sf::Event::MouseMoved) {
				mouse = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
				for (int i = 0; i <= 15; i++) {
					for (int j = 0; j <= 15; j++) {
						if ((mouse.x >= map[i][j].x) && (mouse.x <= map[i][j].x + 32) && (mouse.y >= map[i][j].y) && (mouse.y <= map[i][j].y + 32)) {
							stx = map[i][j].x;
							sty = map[i][j].y;
#ifdef DEBUG
							std::cout << "i = " << i << " j = " << j << std::endl;
							std::cout << "stx = " << stx << " stx = " << stx << std::endl;
#endif // DEBUG
							ci = i; cj = j;
							break;
						}
					}
				}
			}


			if (event.type == sf::Event::MouseButtonPressed) {
				mouse = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
				mode = 0;
				for (int i = 0; i <= 15; i++) {
					for (int j = 0; j <= 15; j++) {
						if ((mouse.x >= map[i][j].x) && (mouse.x <= map[i][j].x + 32) && (mouse.y >= map[i][j].y) && (mouse.y <= map[i][j].y + 64)) {
							SX = map[i][j].x;
							SY = map[i][j].y;
#ifdef DEBUG
							std::cout << "i = " << i << " j = " << j << std::endl;
							std::cout << "sx = " << SX << " sy = " << SY << std::endl;
#endif // DEBUG
							ci = i; cj = j;
							break;
						}
					}
				}
				if (event.mouseButton.button == sf::Mouse::Left) {
					mode = 1;
				}
				else if (event.mouseButton.button == sf::Mouse::Right) {
					mode = 2;
				}
			}

		}
		clock.restart();

		GameTick();

		window.clear();


		/*drawing the game field*/
		for (int i = 0; i <= 15; i++) {
			for (int j = 0; j <= 15; j++) {
				sf::Sprite sprite;
				switch (map[i][j].state)
				{
				case 0:
					sprite = grass;
					break;
				case 1:
					sprite = dirt;
					break;
				case 2:
					sprite = flag;
					break;
				case 5:
					sprite = bomb;
					break;
				default:
					break;
				}
				sprite.setPosition(map[i][j].x, map[i][j].y);
				window.draw(sprite);
				tileRect.setPosition(map[i][j].x, map[i][j].y);
				window.draw(tileRect);
				if ((sty > 0) && (stx >= 0)) {
					selectedRect.setPosition(stx, sty);
					window.draw(selectedRect);
				}

				if (map[i][j].state == 1) {
					//if (inSet.find(BombMap[i][j]) != inSet.end()) {
					if (BombMap[i][j] != 0) {
						text.setString(std::to_string(BombMap[i][j]));
						text.setFillColor(sf::Color::White);
						text.setCharacterSize(12);
						text.setPosition(map[i][j].x + sprite.getGlobalBounds().width / 4, map[i][j].y + sprite.getGlobalBounds().height / 4);
						window.draw(text);
					}
					//}
				}
#ifdef DEBUG
				text.setString(std::to_string(i) + ":" + std::to_string(j));
				text.setCharacterSize(12);
				text.setPosition(map[i][j].x + sprite.getGlobalBounds().width / 4, map[i][j].y + sprite.getGlobalBounds().height / 4);
				window.draw(text);
#endif

			}
		}

		if (!(boom) && (currentState != g_endgame)) {
			bombText.setString("Bombs " + std::to_string(BombCount));
			bombText.setCharacterSize(48);
			bombText.setPosition(10, 10);
			window.draw(bombText);
		}

		if ((boom) && (currentState = g_endgame)) {
			explosion.tick(clock.getElapsedTime().asMilliseconds());
			explosion.draw(window, map[ci][cj].x, map[ci][cj].y);
			endGameText.setString("You lose");
			endGameText.setFillColor(sf::Color::Red);
			endGameText.setCharacterSize(64);
			endGameText.setPosition(400, 240);
			window.draw(endGameText);

		}

		if ((win) && (currentState = g_endgame)) {
			endGameText.setString("You win");
			endGameText.setFillColor(sf::Color::Red);
			endGameText.setCharacterSize(64);
			endGameText.setPosition(400, 240);
			window.draw(endGameText);
		}

		window.display();
	}

	return 0;
}