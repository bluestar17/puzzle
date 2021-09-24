#define _CRT_SECURE_NO_WARNINGS
#include <bangtal>
using namespace bangtal;

#include <cstdlib>
#include <ctime>
#include <Windows.h>

ScenePtr scene;
ScenePtr scene_game;

ObjectPtr board_game[16], board_original[16];
ObjectPtr start;

TimerPtr timer;
float animationTime = 0.001f;
int mixCount = 200;

int blank;

int game_index(ObjectPtr piece) {
	for (int i = 0; i < 16; i++) {
		if (board_game[i] == piece) return i;
	}
	return -1;
}

int index_to_x(int index) {
	return 318 + 180 * (index % 4);
}

int index_to_y(int index) {
	return 540 - 180 * (index / 4);
}

void game_move(int index) {
	auto piece = board_game[index];
	board_game[index] = board_game[blank];
	board_game[index]->locate(scene_game, index_to_x(index), index_to_y(index));
	board_game[blank] = piece;
	board_game[blank]->locate(scene_game, index_to_x(blank), index_to_y(blank));
	blank = index;
}

bool check_move(int index) {
	if (blank % 4 > 0 && index == blank - 1) return true;
	if (blank % 4 < 3 && index == blank + 1) return true;
	if (blank / 4 > 0 && index == blank - 4) return true;
	if (blank / 4 < 3 && index == blank + 4) return true;
	return false;
}

int random_move() { 
	int index = rand() % 16;
	while (!check_move(index)) {
		index = rand() % 16;
	}
	return index;
}

bool check_end() { 
	for (int i = 0; i < 16; i++) {
		if (board_game[i] != board_original[i]) return false;
	}
	return true;
}

void start_game() {
	mixCount = 200;
	timer->set(animationTime);
	timer->start();

	blank = 15;
	board_game[blank]->hide();
}

void end_game() {
	board_game[blank]->show();
	showMessage("성공!");
}

void init_game() {
	//장면 생성
	scene = Scene::create("puzzle game", "images/original.jpg");
	scene_game = Scene::create("puzzle game", "images/blank.jpg");

	//시작 버튼
	start = Object::create("images/start.png", scene, 50, 50);
	start->setOnMouseCallback([&](auto, auto, auto, auto)->bool {
		scene_game->enter();
		start_game();
		return true;
	});

	timer = Timer::create(animationTime);
	timer->setOnTimerCallback([&](auto)->bool {
		game_move(random_move());

		mixCount--;
		if (mixCount > 0) {
			timer->set(animationTime);
			timer->start();
		}

		return true;
	});


	//퍼즐 게임
	char game[20];
	for (int i = 0; i < 16; i++) {
		sprintf(game, "images/4/%d.jpg", i + 1);
		board_game[i] = Object::create(game, scene_game, index_to_x(i), index_to_y(i));
		board_game[i]->setOnMouseCallback([&](auto piece, auto x, auto y, auto action)->bool {
			int index = game_index(piece);
			if (check_move(index)) {
				game_move(index);

				if (check_end()) {
					end_game();
					scene->enter();
				}
			}
			return true;
		});

		board_original[i] = board_game[i];
	}

	startGame(scene);
}


int main()
{
	srand((unsigned int)time(NULL));

	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);

	init_game();

	return 0;
}