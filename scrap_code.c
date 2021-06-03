// scrap_code.c

/*
void bresenham_plot(int x,int y,int x2,int y2, int max_y)
{
	int dx,dy,p;
	dx=x2-x;
	dy=y2-y;
	p = 2 * (dy) - (dx);
	while(x <= x2)
	{
		if(p < 0)
		{
			x=x+1;
			y=y;
			p = p + 2 * (dy);
		}
		else
		{
			x=x+1;
			y=y+1;
			p = p + 2 * (dy - dx);
		}
		char str[20];
		sprintf(str, "Y and x: %d %d\n", y, x);
		debug(str);
	}
}
*/


/*
// won't graph when going from right to left
void bresenham_plot(int x1, int y1, int x2, int y2, int max_y) {
	int slope_new = 2 * (y2 - y1); 
	int slope_error = slope_new - (x2 - x1);
	for (int x = x1, y = y1; x <= x2; x++) {
		mvprintw(max_y -y, x, "*");
		slope_error += slope_new; 
  
		if (slope_error >= 0)  { 
			y++; 
			slope_error -= 2 * (x2 - x1); 
		} 
	}
}
*/

/*
void bresenham_plot(int x0, int y0, int x1, int y1) {

	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
	int err = (dx>dy ? dx : -dy)/2, e2;
 
	while (true) {
		mvprintw(y0, x0, "*");
		// setPixel(x0,y0);
		if (x0==x1 && y0==y1) 
			break;
		e2 = err;
		if (e2 >-dx) {err -= dy; x0 += sx;}
		if (e2 < dy) {err += dx; y0 += sy;}
  }
}
*/

/*
void bresenham_plot(int x0, int y0, int x1, int y1) {
 
  int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2, e2;
 
  for(;;){
    mvprintw(y0, y0, "*");
    if (x0==x1 && y0==y1) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}
*/