#include <curses.h>

/*
q - quit
w - write
p - pen
d - dither
e - export
v - make svg
arrow keys - move around
bug: sometimes, the color gets messed up and turns into hexadecimal
other bug: current prn is not being displayed
*/

typedef struct {
	unsigned int y, x, prev_y, prev_x, current_color;
	char moveArrow, pen;
} User;

typedef struct {
	unsigned int max_y, max_x;
	bool currently_drawing, erasing;
	char current_pen, current_color;
} WindowInfo;

typedef struct {
	char letter, color;
} Pixel;

void debug(const char* msg) {
	FILE *file = fopen("log.txt", "a");
	fputs(msg, file);
	fclose(file);
}

#define ASSIGN_COLOR(color) strColor = color; break;

void makeSquare(FILE* file, short x, short y, char color) {
	char* strColor;
	switch (color) {
		case 0: ASSIGN_COLOR("darkgrey");
		case 1: ASSIGN_COLOR("red");
		case 2: ASSIGN_COLOR("green");
		case 3: ASSIGN_COLOR("yellow");
		case 4: ASSIGN_COLOR("blue");
		case 5: ASSIGN_COLOR("purple");
		case 6: ASSIGN_COLOR("cyan");
		case 7: ASSIGN_COLOR("gray");
		case 8: ASSIGN_COLOR("black");
	}

	fprintf(file, "<rect x=\"%d\" y=\"%d\" width=\"2\" height=\"2\" stroke=\"%s\"\n\
				fill=\"%s\" stroke-width=\"1\"/>\n", x, y, strColor, strColor);
}

void displayInfo(WindowInfo wInfo) {
	for (int x = 1; x < wInfo.max_x / 10; x++)
		mvprintw(wInfo.max_y - 4, x, "_");
	for (int y = 3; y >= 1; y--)
		mvprintw(wInfo.max_y - y, wInfo.max_x / 10, "|");

	char cursorMsg[5] = "Pen ";
	cursorMsg[4] = wInfo.current_pen;

	mvprintw(wInfo.max_y - 3, 2, cursorMsg);

	char* msg;
	if (wInfo.erasing)
		msg = "Erasing";
	else if (wInfo.currently_drawing)
		msg = "Drawing";
	else
		msg = "Moving  ";

	mvprintw(wInfo.max_y - 2, 2, msg);

	char colorMsg[10];
	sprintf(colorMsg, ", Color %d", wInfo.current_color);
	mvprintw(wInfo.max_y - 3, 7, colorMsg);

	int color_ind = 0;
	for (int y = 1; y < wInfo.max_y - 4; y++) {
		attron(COLOR_PAIR(color_ind));
		mvprintw(y, 1, "*****");
		mvprintw(y, 2, "*****");
		attroff(COLOR_PAIR(color_ind));
		if (y % (wInfo.max_y / 9) == 0)
			color_ind++;

	}
}

void displayEraser(int max_x, int max_y) {
	for (int x = max_x / 10 + 1; x < (max_x / 10) * 2; x++)
		mvprintw(max_y - 4, x, "_");

	for (int y = max_y - 3; y < max_y; y++)
		mvprintw(y, (max_x / 10) * 2, "|");

	for (int y = max_y - 3; y < max_y; y++) {
		for (int x = max_x / 10 + 2; x < (max_x / 10) * 2 - 1; x++) {
			mvprintw(y, x, "*");
		}
	}
	mvprintw(max_y - 2, ((max_x / 10) + ((max_x / 10) * 1.6)) / 2, " Eraser ");
}

bool updateEraserStatus(User user, int max_x, int max_y, bool erasing) {
	MEVENT mouseClick;
	if (getmouse(&mouseClick) == OK) {
		for (int y = max_y - 3; y < max_y; y++) {
			for (int x = max_x / 10 + 1; x < (max_x / 10) * 2; x++) {
				if (mouseClick.y == y && mouseClick.x == x) {
					return !erasing;
				}
			}
		}
	}
	return erasing;
}


// add a "fill" option

int main() {
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, true);
	mousemask(BUTTON1_CLICKED | REPORT_MOUSE_POSITION, NULL);
	mouseinterval(0);

	use_default_colors();
	start_color();
	for (int i = 0; i < 8; i++)
		init_pair(i, i, i);
	
	int MAX_Y, MAX_X;
	getmaxyx(stdscr, MAX_Y, MAX_X);
	Pixel drawingBoard[MAX_Y][MAX_X];

	box(stdscr, '|', '_');

	for (int y = 0; y < MAX_Y; y++) {
		for (int x = 0; x < MAX_X; x++) {
			Pixel p = {'\0', 0};
			drawingBoard[y][x] = p;
		}
	}

	bool usingApp = true, drawing = false, erasing = false;
	User user = {MAX_Y / 10, MAX_X / 10, MAX_Y / 10, MAX_X / 10, 0, ',', '*'};
	WindowInfo wInfo = {MAX_Y, MAX_X, drawing, erasing, user.pen};

	bool makeNewPen = false, dithering = false;

	while (usingApp) {
		printf("Using\n");
		user.prev_y = user.y;
		user.prev_x = user.x;

		if (makeNewPen) {
			user.pen = getch();
			makeNewPen = false;
		}

		erasing = updateEraserStatus(user, MAX_X, MAX_Y, erasing);

		if (erasing)
			drawing = false;

		switch (getch()) {
			case 'q':
				usingApp = false; break;
			case 'w':
				drawing = !drawing; break;
			case 'p':
				makeNewPen = true; break;
			case 'd':
				dithering = !dithering; break;
			case KEY_MOUSE: {
				MEVENT mouseClick;
				if (getmouse(&mouseClick) == OK) {
					user.y = mouseClick.y;
					user.x = mouseClick.x;

					char colorInd = 0;
					for (int colorY = 1; colorY < wInfo.max_y - 4; colorY++) {
						if (colorY % (MAX_Y / 9) == 0)
							colorInd++;
						for (char colorX = 1; colorX < 7; colorX++) {
							if (user.y == colorY && user.x == colorX) {
								user.current_color = colorInd;
								drawing = false;
								break;
							}
						}
					}
				}
				break;
			}

			case 'e': {
				fclose(fopen("Out/output.txt", "w"));

				FILE* outFile = fopen("Out/output.txt", "a");
				for (int y = 0; y < MAX_Y; y++) {
					for (int x = 0; x < MAX_X; x++) {
						Pixel p = drawingBoard[y][x];
                        char buf[15];
                        sprintf(buf, "\033[48;5;%dm \033[0m", p.color);
                        p.letter == 0 ? fputs(" ", outFile) : fputs(buf, outFile);
                    }
                    fputs("\n", outFile);
                }
                fclose(outFile);
                break;
			}

			case 'v': {
				fclose(fopen("Out/output.svg", "w"));
				FILE* svgFile = fopen("Out/output.svg", "a");
				fprintf(svgFile, "<?xml version=\"1.0\" standalone=\"no\"?>\n\
				<svg width=\"%d\" height=\"%d\" version=\"1.1\"\n\
				xmlns=\"http://www.w3.org/2000/svg\">\n\n", MAX_X, MAX_Y * 2);

				for (int y = 0; y < MAX_Y; y++) {
					for (int x = 0; x < MAX_X; x++) {
						Pixel pixel = drawingBoard[y][x];
						if (pixel.letter == '\0')
							continue;
						makeSquare(svgFile, x, y * 1.5, pixel.color);
					}
				}
				fputs("\n</svg>\n", svgFile);
				fclose(svgFile);
				break;
			}

			case KEY_UP:
				user.y--; break;
			case KEY_DOWN:
				user.y++; break;
			case KEY_LEFT:
				if (dithering) user.x -= 2; else user.x--;
				break;
			case KEY_RIGHT:
				if (dithering) user.x += 2; else user.x++;
				break;
		}

		if (user.y < 0)
			user.y = 5;
		else if (user.y > MAX_Y)
			user.y = MAX_Y;
		else if (user.x < 0)
			user.x = 0;
		else if (user.y > MAX_X)
			user.x = MAX_X - 1;

	if (drawing) {
		Pixel p = {user.pen, user.current_color};
		drawingBoard[user.y][user.x] = p;
	}

	else {
		if (erasing) {
			Pixel p = {'Z', 0};
			drawingBoard[user.y][user.x] = p;
		}

		mvprintw(user.y, user.x, &user.moveArrow);
		mvprintw(user.y, user.x + 1, " ");
		mvprintw(user.prev_y, user.prev_x, " ");
	}
	
	for (int y = 0; y < MAX_Y; y++) {
		for (int x = 0; x < MAX_X; x++) {
			Pixel p = drawingBoard[y][x];
			attron(COLOR_PAIR(p.color));
			const char terminated[2] = {p.letter, '\0'};
			mvprintw(y, x, terminated);
			attroff(COLOR_PAIR(p.color));
		}
	}

	wInfo.currently_drawing = drawing;
	wInfo.erasing = erasing;
	wInfo.current_pen = user.pen;
	wInfo.current_color = user.current_color; // getting messed up

	displayInfo(wInfo);
	displayEraser(MAX_X, MAX_Y);

	refresh();
	flushinp();
	napms(80);

	}
	endwin();
	return 0;
}
