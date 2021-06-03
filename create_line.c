#include <curses.h>
#include <stdlib.h>

typedef struct {
	short x1, y1, x2, y2;
	bool will_draw;
} Line;


// https://stackoverflow.com/questions/10060046/drawing-lines-with-bresenhams-line-algorithm
void bresenham_plot(int x1, int y1, int x2, int y2) {
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	dx = x2-x1;
	dy = y2-y1;
	dx1 = abs(dx);
	dy1 = abs(dy);
	px = 2 * dy1 - dx1;
	py = 2 * dx1 - dy1;
	if (dy1 <= dx1) {
		if (dx >= 0) {
 			x = x1;
 			y = y1;
 			xe = x2;
		}
  		else {
  			x = x2;
  			y = y2;
  			xe = x1;
  		}
  		mvprintw(y, x, "*");

  	for (i = 0; x < xe; i++) {
		x = x + 1;
		if (px < 0) {
			px = px + 2 * dy1;
		}
		else {
			if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
				y = y + 1;
			}
    		else {
    			y = y - 1;
    		}
	
			px = px + 2 * (dy1 - dx1);
		}
		mvprintw(y, x, "*");
	}
}
	else {
		if (dy >= 0) {
			x = x1;
			y = y1;
			ye = y2;
		}
	else {
		x = x2;
		y = y2;
		ye = y1;
	}
	mvprintw(y, x, "*");
	for (i = 0; y < ye; i++) {
		y = y+1;
		if (py <= 0) {
			py = py + 2 * dx1;
		}
		else {
			if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
				x = x + 1;
			}
			else {
				x = x - 1;
			}
			py = py + 2 * (dx1 - dy1);
		}
		mvprintw(y, x, "*");
	}
}
}

int main() {
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, true);
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
	nodelay(stdscr, true);
	mouseinterval(0);

	int max_y = getmaxy(stdscr);

	bool screen_active = true;

	Line line = {0, 0, 0, 0, false};

	while (screen_active) {

		switch (getch()) {
			case 'q':
				screen_active = false;
				break;
			case KEY_MOUSE: {
				MEVENT event;
				if (getmouse(&event) == OK) {
					switch (event.bstate) {
						case BUTTON1_PRESSED:
							line.x1 = event.x;
							line.y1 = event.y;
							break;
						case BUTTON1_RELEASED:
							line.x2 = event.x;
							line.y2 = event.y;
							line.will_draw = true;
							break;
					}
				}
      			break;
      		}
      		default:
      			line.will_draw = false;
      			break;
		}
		if (line.will_draw)
			bresenham_plot(line.x1, line.y1, line.x2, line.y2);
		refresh();
		napms(100);
	}
	endwin();
}