/*
 * Copyright 2022. Dongwon Kim All rights reserved.
 *
 * File name : main.cpp
 * 
 * Written by Dongwon Kim
 *
 * STM32F103RBTx & MPU6080 air mouse
 *    windows application for air mouse
 *
 * Modificatoin history
 *    written by Dongwon Kim on May 04, 2022
 *
 * Notice:
 *	serial.h is written by Mincoding
 *	INTERVAL value should be same with HAL_Delay value of STM chip
 *	ONE_MOVE can be changed for DPI
 *	
 */
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "Serial.h"
#include <stdio.h>
#include <windows.h>  //  for GetCursorPos, SetCursorPos
#include <WinUser.h>  //  for mouse_event

#define THRES_X 5000  //  threshold for sensor value direction x
#define THRES_Y 5000  //  threshold for sensor value direction x
#define ONE_MOVE 30   //  pixel value to move at once
#define INTERVAL 100  //  time interval to read value [ms]

// struct to store parsed data
typedef struct _DATA {
	float temp;
	int g_x, g_y, g_z;
	int a_x, a_y, a_z;
	int l_bttn, r_bttn;
}DATA;


// function declare
void get_data(Serial *serial, DATA *data);
void move_pos(int move_y, int move_x);
void set_pos(DATA data, int *move_y, int *move_x);
void mouse_click(DATA data);

/*
 * UART connect with STM board
 * get data from the board(sensor value, buttons value)
 * parse the data and store in DATA data
 * set move_y, move_x according to the sensor value
 * move cursor
 * if button is clicked, generate click event
 * sleep
 */
int main()
{
	// serial init
	Serial* serial = new Serial("COM3", 115200);
	if (!serial->isConnected()) return 0;

	/*
	 * if move_y == 1 : move cursor left
	 *           == -1: move cursor right
	 *           == 0 : stay in x direction
	 *
	 * if move_x == 1 : move cursor down
	 *           == -1: move cursor up
	 *           == 0 : stay in y direction
	 */
	char buf[255];
	DATA data;
	int move_y, move_x;
	
	while (1) {
		get_data(serial, &data);
		set_pos(data, &move_y, &move_x);
		move_pos(move_y, move_x);
		mouse_click(data);
		Sleep(INTERVAL);
	}

	return 0;
}

/*
 * get and parse the data
 *
 * format
 * temper\n
 * g_x g_y g_z\n
 * a_x a_y a_z\n
 * l_bttn r_bttn\n
 *
 * input:
 *	Serial *serial: for UART connection
 *
 * output:
 *	DATA *data: to save parsed data
 *
 * Notice:
 *	when button is pressed, value = 0(pull_up)
 */
void get_data(Serial *serial, DATA *data) {
	char buf[255];
	int ret = serial->read(buf, 255);
	if (!ret) return;

	sscanf(buf, "%f\n%d %d %d\n%d %d %d\n %d %d\n", &(data->temp), 
		&(data->g_x), &(data->g_y), &(data->g_z), 
		&(data->a_x), &(data->a_y), &(data->a_z), 
		&(data->l_bttn), &(data->r_bttn));

	printf("%s\n", buf);
	printf("%d %d %d\n", data->a_x, data->a_y, data->a_z);
	printf("%d %d\n", data->l_bttn, data->r_bttn);
	printf("-----------------\n");
}

/*
 * move position of the cursor according to flags
 *
 * input:
 *	int move_y: move cursor along the x-axis
 *	int move_x: move cursor along the y-axis
 *
 * output:
 *	change cursor position using SetCursorPos Windows API
 */
void move_pos(int move_y, int move_x) {
	POINT pos;
	GetCursorPos(&pos);

	int dy = 0, dx = 0;
	if (move_y == 1) dx -= ONE_MOVE;
	else if (move_y == -1) dx += ONE_MOVE;

	if (move_x == 1) dy -= ONE_MOVE;
	else if (move_x == -1) dy += ONE_MOVE;

	SetCursorPos(pos.x + dx, pos.y + dy);
}

/*
 * set flags for cursor
 *
 * input:
 *	DATA data: struct that contains parsed data
 * 
 * output:
 *	int *move_y:
 *		1: when data.a_y is greater than threshold value
 *		-1: when data.a_y is lesser than -threshold value
 *		0: when data.a_y is between -threshold and threshold
 *	int *move_x:
 *		1: when data.a_x is greater than threshold value
 *		-1: when data.a_x is lesser than -threshold value
 *		0: when data.a_x is between -threshold and threshold
 */
void set_pos(DATA data, int *move_y, int *move_x) {
	if (data.a_x > THRES_X) *move_x = 1;
	else if (data.a_x < -THRES_X) *move_x = -1;
	else *move_x = 0;

	if (data.a_y > THRES_Y) *move_y = 1;
	else if (data.a_y < -THRES_Y) *move_y = -1;
	else *move_y = 0;
}

/*
 * if buttn is clicked, make click event
 *
 * input:
 *	DATA data: struct that contains parsed data
 *
 * output:
 *	cause mouse event when l_bttn/r_bttn is pressed(value == 0)
 *	using mouse_event()
 */
void mouse_click(DATA data) {
	POINT pos;
	GetCursorPos(&pos);

	if (!data.l_bttn) {
		mouse_event(MOUSEEVENTF_LEFTDOWN, pos.x, pos.y, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, pos.x, pos.y, 0, 0);
	}
	else if (!data.r_bttn) {
		mouse_event(MOUSEEVENTF_RIGHTDOWN, pos.x, pos.y, 0, 0);
		mouse_event(MOUSEEVENTF_RIGHTUP, pos.x, pos.y, 0, 0);
	}
}
