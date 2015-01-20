#include "types.h"
#include "defs.h"
#include "window.h"
#define MAXWINDOW 20

WindowLink window_list = 0;
WindowLink activated_window = 0;
WindowLink list_head = 0;
WindowLink list_tail = 0;

Window window_array[MAXWINDOW];
int next_window_id = 1;

//static color16* vesa_array = (color16*)VESA_ADDR;
color16* vesa_array;

void drawScreen();

void windowlistinit()
{
//	WindowLink p;
    vesa_array = (color16*)VESA_ADDR;
	memset(window_array, 0, sizeof(Window) * MAXWINDOW);
}

WindowLink getActivated()
{
	return activated_window;
}

WindowLink allocWindow(int left_x, int left_y, int right_x, int right_y, int pid)
{
	WindowLink p;
	for (p = window_array; p < &window_array[MAXWINDOW]; p++)
	{
		if (p->window_id <= 0)
		{
			p->window_id = next_window_id++;
			p->pid = pid;
			(p->window_position).left_x = left_x;
			(p->window_position).left_y = left_y;
			(p->window_position).right_x = right_x;
			(p->window_position).right_y = right_y;
			if (list_head == 0) list_head = p;
			if (list_tail != 0) list_tail->next_window = p;
			p->prior_window = list_tail;
			list_tail = p;
			p->next_window = 0;
			createUpdateMsg(p->pid);
			cprintf("UpdateMsg created for process: %d\n", p->pid);
			return p;
		}
	}
	return 0;
}

int releaseWindow(int window_id)
{
	WindowLink p;
	for (p = list_head; p != 0; p = p->next_window)
	{
		if (p->window_id == window_id)
		{
			if (p->prior_window != 0) 
				p->prior_window->next_window = p->next_window;
			else
				list_head = p->next_window;
			if (p->next_window != 0) 
				p->next_window->prior_window = p->prior_window;
			else
				list_tail = p->prior_window;
			p->window_id = -1;
			drawScreen();
			return 0;
		}
	}
	return -1;
}

int inClientRect(WindowLink pWindow, int position_x, int position_y)
{
	//cprintf("WindowID: %d, x: %d, y: %d\n", pWindow->window_id, position_x, position_y);
	return (pWindow->window_position).left_x <= position_x &&
		(pWindow->window_position).right_x > position_x &&
		(pWindow->window_position).left_y <= position_y &&
		(pWindow->window_position).right_y > position_y ? 1 : 0;
}

void setActivated(int window_id)
{
	WindowLink p = list_head;
	while (p != 0 && p->window_id != window_id) p = p->next_window;
	if (p == 0) return;

	if (p->prior_window != 0) 
		p->prior_window->next_window = p->next_window;
	else
		list_head = p->next_window;
	if (p->next_window != 0) 
		p->next_window->prior_window = p->prior_window;
	else
		list_tail = p->prior_window;

	if (list_head == 0) list_head = p;
	if (list_tail != 0) list_tail->next_window = p;
	p->prior_window = list_tail;
	list_tail = p;
	p->next_window = 0;
	createUpdateMsg(p->pid);
}

int getClickedPid(int position_x, int position_y)
{
	WindowLink p = getWindowByPoint(position_x, position_y);
	return p == 0 ? -1 : p->pid;
}

WindowLink getWindowById(int window_id)
{
	WindowLink p = list_head;
	while (p != 0 && p->window_id != window_id) 
	{
		cprintf("now: %d, target: %d\n", p->window_id, window_id);
		cprintf("next: %d\n", p->next_window);
		p = p->next_window;
	}
	return p;
}

WindowLink getWindowByPoint(int position_x, int position_y)
{
	WindowLink p = list_tail;
	while (p != 0 && !inClientRect(p, position_x, position_y)) p = p->prior_window;
	return p;
}

void drawWindow(WindowLink pWindow, color16* context)
{
	int i, j;
	int x1 = (pWindow->window_position).left_x;
	int y1 = (pWindow->window_position).left_y;
	int x2 = (pWindow->window_position).right_x;
	int y2 = (pWindow->window_position).right_y;
	cprintf("window_id: %d, x1: %d, y1: %d, x2: %d, y2: %d\n", pWindow->window_id, x1, y1, x2, y2);
	for (i = x1; i < x2; i++)
		for (j = y1; j < y2; j++)
		{	
			WindowLink qWindow = list_tail;
			while (qWindow != pWindow && inClientRect(qWindow, i, j)) qWindow = qWindow->prior_window;
			vesa_array[j * SCREEN_WIDTH + i] = qWindow == pWindow ? context[(j - y1) * (x2 - x1) + i - x1] : vesa_array[j * SCREEN_WIDTH + i];
			//vesa_array[i*SCREEN_HEIGHT + j] = 2016;
		}
}

void drawScreen()
{
	WindowLink p;
	for (p = list_head; p != 0; p = p->next_window)
	{
		createUpdateMsg(p->pid);
	}
}


static color16 mouse[10][15] = {
	{65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 2016, 2016, 2016},
	{65535, 2113, 0, 0, 0, 0, 0, 0, 0, 0, 27501, 65535, 2016, 2016, 2016},
	{65535, 65535, 6307, 0, 0, 0, 0, 0, 0, 27502, 65535, 65535, 2016, 2016, 2016},
	{2016, 65535, 65535, 2113, 0, 0, 0, 0, 0, 29614, 65535, 2016, 2016, 2016, 2016},
	{2016, 2016, 65535, 65535, 2113, 0, 0, 0, 0, 0, 6371, 65535, 65535, 65535, 2016},
	{2016, 2016, 2016, 65535, 65535, 32, 0, 0, 27437, 16968, 0, 0, 12710, 65535, 65535},
	{2016, 2016, 2016, 2016, 65535, 65535, 0, 0, 65535, 65535, 42292, 6371, 0, 0, 65535},
	{2016, 2016, 2016, 2016, 2016, 65535, 65535, 0, 65535, 65535, 65535, 65535, 31727, 2145, 65535},
	{2016, 2016, 2016, 2016, 2016, 2016, 65535, 65535, 65535, 2016, 2016, 65535, 65535, 65535, 65535},
	{2016, 2016, 2016, 2016, 2016, 2016, 2016, 65535, 65535, 2016, 2016, 2016, 2016, 2016, 2016}
};

static color16 mouseBuffer[10][15];

static int mouseX = -1;
static int mouseY = -1;
static int drawingMouse = 0;
void drawMouse(int newX, int newY)
{
	int i, j;
	//cprintf("x: %d, y: %d\n", newX, newY);
	if (drawingMouse) return;
	drawingMouse = 1;
	if (mouseX >= 0)
	{
		for (i = 0; i < 10; i++)
			for (j = 0; j < 15; j++)
			{
				vesa_array[(j + mouseY) * SCREEN_WIDTH + i + mouseX] = mouseBuffer[i][j];
			}
	}
	mouseX = newX;
	mouseY = newY;
	for (i = 0; i < 10; i++)
		for (j = 0; j < 15; j++)
		{
			mouseBuffer[i][j] = vesa_array[(j + mouseY) * SCREEN_WIDTH + i + mouseX];
		}
	for (i = 0; i < 10; i++)
		for (j = 0; j < 15; j++)
		{
			if (mouse[i][j] != 2016)
				vesa_array[(j + mouseY) * SCREEN_WIDTH + i + mouseX] = mouse[i][j];
		}
	drawingMouse = 0;
}
