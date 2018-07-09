// EvolutionaryNationsSim.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "State.h"
#include "MapHandler.h"
#include "InputHandler.h"
#include "ButtonTemplate.h"
#include <iostream>
#include <sfml/Graphics.hpp>

using namespace std;
using namespace sf;

float GLOBALSCALE = 5;
Vector2f GLOBALOFFSET = Vector2f(0, 0);

RenderWindow g_window(VideoMode(512, 512), "Evolutionary Nations");

//the entire collection of states the entire simulation is based off of
MapHandler g_map;

//all input is handled here
InputHandler g_inputHandler;

void testFunc()
{
	printf("gu hello\n");
}

//everythime the screen needs to be updated this is called
void g_update(vector<ButtonTemplate> &buttons)
{
	g_window.clear();
	g_inputHandler.buttons.clear();	//want to get rid of all buttons and states because the update functions add them back in
	g_inputHandler.states.clear();
	g_map.updateStates();
	for (int i = 0; i < buttons.size(); i++)
	{
		buttons[i].update();
	}
	g_window.display();
}

void g_updateNations(vector<Nation> &nations)
{
	for (int i = 0; i < nations.size(); i++)
	{
		nations[i].update();
	}
}

int main()
{
	void(*callBackFunc)() = &testFunc;

	ButtonTemplate testButton("Assets/TestButton.bmp", Vector2f(10, 10), callBackFunc, 5);

	vector<ButtonTemplate> allButtons{ testButton };

	Nation testNation(Color(255, 0, 0, 255));
	testNation.addContolledState(&g_map.states[0][0]);
	testNation.addContolledState(&g_map.states[1][0]);

	Nation testNation2(Color(104, 0, 204, 255));
	testNation2.addContolledState(&g_map.states[2][0]);

	Nation testNation3(Color(255, 255, 100, 255));
	testNation3.addContolledState(&g_map.states[4][4]);

	vector<Nation> allNations{ testNation, testNation2, testNation3 };

	//initially draw stuff to screen.
	g_update(allButtons);

	//For refreshing the screen periodically (currently 30fps)
	Clock screenRefreshClock;
	Time screenRefreshTime = seconds(0.0333);

	//For refreshing the nations periodically
	Clock nationUpdateClock;
	Time nationUpdateTime = seconds(1.5);

	//main loop
	while (g_window.isOpen())
	{
		//event handling
		Event evnt;
		while (g_window.pollEvent(evnt))
		{
			if (evnt.type == Event::Closed)
			{
				g_window.close();
				break;
			}

			//keyboard input
			if (g_inputHandler.getInput(&evnt))
			{
				//may need to fix this issue later on...
				//every time there is a new event, we want to redraw stuff to screen (makes screen it seem smoother)
				g_update(allButtons);
			}
		}
		
		//updating screen periodically
		if (screenRefreshClock.getElapsedTime() >= screenRefreshTime)
		{
			screenRefreshClock.restart();
			g_update(allButtons);
		}

		//updating nations periodically
		if (nationUpdateClock.getElapsedTime() >= nationUpdateTime)
		{
			g_updateNations(allNations);
			/*testNation.update();
			testNation2.update();*/
			nationUpdateClock.restart();
		}
	}

	return 0;
}
