#include "stdafx.h"
#include "Nation.h"
#include <stdlib.h>

extern MapHandler g_map;

Nation::Nation(Color color, string name)
{
	this->nationColor = color;
	this->nationName = name;
}

Nation::~Nation()
{
}

void Nation::printDiplomaticView(Nation * nation)
{
	DiplomaticView nationView = diplomaticViews[nation];
	switch (nationView)
	{
	case close:
		SetConsoleTextAttribute(hConsole, 10);	// light green
		cout << "close";
		break;
	case friendly:
		SetConsoleTextAttribute(hConsole, 2);	// dark green
		cout << "friendly";
		break;
	case warm:
		SetConsoleTextAttribute(hConsole, 3);	// blue-ish
		cout << "warm";
		break;
	case neutral:
		SetConsoleTextAttribute(hConsole, 9);	// blue
		cout << "neutral";
		break;
	case uneasy:
		SetConsoleTextAttribute(hConsole, 12);	// dark red
		cout << "unstable";
		break;
	case hostile:
		SetConsoleTextAttribute(hConsole, 4);	// bright red
		cout << "hostile";
		break;
	default:
		cout << "Nation::printDiplomaticView needs to be updated. Diplomatic View is not understood";
		system("pause");
	}

	SetConsoleTextAttribute(hConsole, 7);
}

void Nation::printDiplomaticRelation(Nation * nation)
{
	

	DiplomaticRelation nationRelation = diplomaticRelations[nation];
	switch (nationRelation)
	{
	case nothing:
		SetConsoleTextAttribute(hConsole, 9);	// blue
		cout << "nothing";
		break;
	case ally:
		SetConsoleTextAttribute(hConsole, 10);	// light green
		cout << "ally";
		break;
	case belligerent:
		SetConsoleTextAttribute(hConsole, 12);	// red
		cout << "belligerent";
		break;
	case vassal:
		SetConsoleTextAttribute(hConsole, 14);	// yellow
		cout << "vasal";
		break;
	case overlord:
		SetConsoleTextAttribute(hConsole, 5);	// purple
		cout << "overlord";

	default:
		cout << "Nation::printDiplomaticRelation needs to be updated. Diplomatic relation is not understood";
		system("pause");
	}

	SetConsoleTextAttribute(hConsole, 7);
}

void Nation::printInfo()
{
	cout << "Name: " << nationName << endl << endl;

	cout << "Size: " << controlledStates.size() << " states" << endl;
	cout << "Total development: " << getDevelopment() << endl;
	cout << "Wallet: " << resources << endl;
	cout << "Revenue: "; 
	//if nation making money, revenue is green, otherwise, it is red:
	if (revenue > 0)
		SetConsoleTextAttribute(hConsole, 2);	// green
	else
		SetConsoleTextAttribute(hConsole, 12);	// red
	cout << revenue << endl;
	SetConsoleTextAttribute(hConsole, 7);	// white
	cout << "Army size: " << army << endl;
	cout << "Army strength: " << armyStrength << endl;
	
	cout << endl;

	//printing out the relationships with everyone:
	if (diplomaticViews.size() != 0)
	{
		cout << "Dipolmacy: " << endl;
		for (auto& x : diplomaticViews)
		{
			cout << "   feels ";
			printDiplomaticView(x.first); 
			cout << " towards " << x.first->nationName << endl;
			cout << "   sees " << x.first->nationName << " as a ";
			printDiplomaticRelation(x.first);
			cout << endl;
		}
	}

	//displaying who the nation is at war with
	if (wars.size() != 0)
	{
		cout << endl;
		cout << "Involved in the following wars: " << endl;
		for (int i = 0; i < wars.size(); i++)
		{
			wars[i]->printInfo(this);
		}
	}

}

void Nation::addContolledState(State * state)
{
	//right away, no nation can own a water province;
	if (state->isWater)
	{
		return;
	}

	//if it has a controller, get the controlled to give up the state
	if (state->controller != NULL)
	{
		for (int i = 0; i < state->controller->controlledStates.size(); i++)
		{
			if (state->controller->controlledStates[i] == state)
			{
				state->controller->controlledStates.erase(state->controller->controlledStates.begin() + i);
			}
		}
	}

	//otherwise we want to set up the two-way link between the nation and the state
	this->controlledStates.push_back(state);
	state->controller = this;

}

int Nation::getDevelopment()
{
	int development = 0;

	for (int i = 0; i < controlledStates.size(); i++)
	{
		development += controlledStates[i]->development;
	}

	return development;
}

void Nation::borderConflicts()
{
	//for every state that borders another nation, there is a 1% chance of the diplomatic view of this nation decreasing
	for (int i = 0; i < controlledStates.size(); i++)
	{
		//checking states around current one
		//some relative coordinates
		int x = controlledStates[i]->positionInMap.x;
		int y = controlledStates[i]->positionInMap.y;
		//checking to the left
		if (x != 0 && g_map.states[x - 1][y].controller != NULL && g_map.states[x - 1][y].controller != this)
			if (rand() % 100 == 1)	//1% chance
				decreaseRelations(g_map.states[x - 1][y].controller);

		//to the right
		if (x != g_map.width - 1 && g_map.states[x + 1][y].controller != NULL && g_map.states[x + 1][y].controller != this)
			if (rand() % 100 == 1)
				decreaseRelations(g_map.states[x + 1][y].controller);
		//down
		if (y != g_map.height - 1 && g_map.states[x][y + 1].controller != NULL && g_map.states[x][y + 1].controller != this)
			if (rand() % 100 == 1)
				decreaseRelations(g_map.states[x][y + 1].controller);
		//up
		if (y != 0 && g_map.states[x][y - 1].controller != NULL && g_map.states[x][y - 1].controller != this)
			if (rand() % 100 == 1)
				decreaseRelations(g_map.states[x][y - 1].controller);
	}
}

void Nation::spread()
{
	//first, check around every state that the nation controls,
	//if the nations around the controlled states does not have any owner then "addContolledState" that state
	int controlledStatesSize = controlledStates.size();

	for (int i = 0; i < controlledStatesSize; i++)
	{
		//checking states around current one
		//some relative coordinates
		int x = controlledStates[i]->positionInMap.x;
		int y = controlledStates[i]->positionInMap.y;
		//checking to the left
		if (x != 0 && g_map.states[x - 1][y].controller == NULL)
		{
			//if there a valid state to the left, then we will append it to the temp list of states
			addContolledState(&g_map.states[x - 1][y]);
		}
		//to the right
		if (x != g_map.width-1 && g_map.states[x + 1][y].controller == NULL)
		{
			addContolledState(&g_map.states[x + 1][y]);
		}
		//down
		if (y != g_map.height-1 && g_map.states[x][y + 1].controller == NULL)
		{
			addContolledState(&g_map.states[x][y + 1]);
		}
		//up
		if (y != 0 && g_map.states[x][y - 1].controller == NULL)
		{
			addContolledState(&g_map.states[x][y - 1]);
		}
	}
}

void Nation::colonize()
{
	//it will take 10 resources to complete this task:
	if (this->resources < 10)
	{
		return;
	}

	//to keep track of the best looking state to colonize
	State *bestOption = NULL;

	for (int i = 0; i < controlledStates.size(); i++)
	{
		//checking states around current one
		//some relative coordinates
		int x = controlledStates[i]->positionInMap.x;
		int y = controlledStates[i]->positionInMap.y;
		//checking to the left
		if (x != 0 && g_map.states[x - 1][y].controller == NULL)
		{
			//if there a valid state to the left, then we will see if it is any better than the current "bestOption"
			if (bestOption == NULL || g_map.states[x - 1][y].development > bestOption->development)
			{
				bestOption = &g_map.states[x - 1][y];
			}
		}
		//to the right
		if (x != g_map.width - 1 && g_map.states[x + 1][y].controller == NULL)
		{
			if (bestOption == NULL || g_map.states[x + 1][y].development > bestOption->development)
			{
				bestOption = &g_map.states[x + 1][y];
			}
		}
		//down
		if (y != g_map.height - 1 && g_map.states[x][y + 1].controller == NULL)
		{
			if (bestOption == NULL || g_map.states[x][y + 1].development > bestOption->development)
			{
				bestOption = &g_map.states[x][y + 1];
			}
		}
		//up
		if (y != 0 && g_map.states[x][y - 1].controller == NULL)
		{
			if (bestOption == NULL || g_map.states[x][y - 1].development > bestOption->development)
			{
				bestOption = &g_map.states[x][y - 1];
			}
		}
	}

	//if there are no states possible to colonize, then just stop:
	if (bestOption == NULL)
	{
		return;
	}

	//otherwise we want to add the bestOption to the control of the nation and move take away resources:
	addContolledState(bestOption);
	resources -= 10;
}

void Nation::improveArmy()
{
	if (resources > 10)
	{
		resources -= 10;
		this->army += 1;
	}
}

void Nation::declareWarOnEnemyNeighbor()
{
	//for every state that borders another nation, check to see if there is a enemy nation there:
	for (int i = 0; i < controlledStates.size(); i++)
	{
		//checking states around current one
		//some relative coordinates
		int x = controlledStates[i]->positionInMap.x;
		int y = controlledStates[i]->positionInMap.y;
		
		//checking to the left
		x -= 1;
		warWithStateOffset(x, y);
		//right
		x += 2;
		warWithStateOffset(x, y);
		//down
		x -= 1;
		y += 1;
		warWithStateOffset(x, y);
		//up
		y -= 2;
		warWithStateOffset(x, y);
	}
}

void Nation::warWithStateOffset(int x, int y)
{
	//return if the state we are trying to check is outside of the map
	if (x < 0 || y < 0 || x >= g_map.width || y >= g_map.height)
		return;

	Nation * possibleBelligerent = g_map.states[x][y].controller;

	//check to see if the neighbooring state is actualy a different nation
	if (possibleBelligerent != NULL && possibleBelligerent != this)
	{
		//checking to see if it is a valid target to declare war on
		if (diplomaticViews[possibleBelligerent] == DiplomaticView::hostile &&
			diplomaticRelations[possibleBelligerent] != DiplomaticRelation::belligerent &&
			diplomaticRelations[possibleBelligerent] != DiplomaticRelation::ally)
		{
			//changing color of console output
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, 12);
			cout << endl;
			cout << this->nationName << " declares war on " << possibleBelligerent->nationName << endl;
			SetConsoleTextAttribute(hConsole, 7);

			//actually do war stuff
			new War(this, possibleBelligerent);
			return;
		}
	}
}

void Nation::allyWith(Nation * newAlly)
{
	newAlly->diplomaticRelations[this] = DiplomaticRelation::ally;
	newAlly->increaseRelations(this);
	this->diplomaticRelations[newAlly] = DiplomaticRelation::ally;
	this->increaseRelations(newAlly);
}

void Nation::increaseRelations(Nation *nation)
{
	using IntType = typename std::underlying_type<DiplomaticView>::type;
	if (this->diplomaticViews[nation] == DiplomaticView::close)	//if we are at the max possible relations, then just leave
		return;
	this->diplomaticViews[nation] = static_cast<DiplomaticView>(static_cast<IntType>(this->diplomaticViews[nation]) + 1);
}

void Nation::decreaseRelations(Nation *nation)
{

	using IntType = typename std::underlying_type<DiplomaticView>::type;
	if (this->diplomaticViews[nation] == DiplomaticView::hostile)	//if we are at the min possible relations, then just leave
		return;
	this->diplomaticViews[nation] = static_cast<DiplomaticView>(static_cast<IntType>(this->diplomaticViews[nation]) - 1);
}

void Nation::repairArmy()
{
	while (armyStrength < 1 && resources > 0.5)
	{
		//takes more money to heal a bigger army:
		float armyRepairFactor = 1 / float(army);
		armyStrength += armyRepairFactor;
		if (armyStrength > 1)
			armyStrength = 1;
		resources -= 0.5;
	}
}

void Nation::attackEnemyArmy()
{
	//look at all wars
	//if it would be a half decent idea to attack the enemy, then do so
	for (int i = 0; i < wars.size(); i++)
	{
		//first, figure out if we are the defender
		bool isDefender = false;
		for (int j = 0; j < wars[i]->defenders.size(); j++)
		{
			if (wars[i]->defenders[j] == this)
				isDefender = true;
		}

		//if we are the attacker and the "chanceOfAttackerWinningBattle" is > 0.5, then do it
		if (float(wars[i]->chanceOfAttackerWinningBattle()) > 0.5 && !isDefender)
		{
			wars[i]->battle();
			continue;
		}
		//if we are the defender and the "chanceOfAttackerWinningBattle" is < 0.5, then do it
		if (float(wars[i]->chanceOfAttackerWinningBattle()) < 0.5 && isDefender)
		{
			wars[i]->battle();
			continue;
		}
	}
}

void Nation::takeStates()
{
	//go through all wars
	for (int i = 0; i < wars.size(); i++)
	{
		//first find out if we are the lead defender/attacker
		bool isAttacker = true;
		if (wars[i]->leadAttacker != this)
		{
			isAttacker = false;
			if (wars[i]->leadDefender != this)
			{
				continue;	// only war leaders can make demands
			}
		}

		//we need at least 5 warscore to achieve this deal, so if we are the attacker and the warscore is less than 5, move on:
		if (wars[i]->warScore < 5 && wars[i]->leadAttacker == this)
		{
			continue;
		}
		//same with defenders, except negative
		if (wars[i]->warScore > -5 && wars[i]->leadDefender == this)
		{
			continue;
		}

		//figure out if it is time to end the war. If we can continue to obliterate the enemy then continue, otherwise, stop the war
		float alliesStrength = 0;
		float enemyStrength = 0;
		for (auto& diplomaticRelationPair : diplomaticRelations)
		{
			if (diplomaticRelationPair.second == DiplomaticRelation::ally)
			{
				alliesStrength += diplomaticRelationPair.first->army*diplomaticRelationPair.first->armyStrength;
			}
			if (diplomaticRelationPair.second == DiplomaticRelation::belligerent)
			{
				enemyStrength += diplomaticRelationPair.first->army*diplomaticRelationPair.first->armyStrength;
			}
		}

		//if our side is better than theirs AND we havent already obliterated them, then keep on going with the current war
		if (alliesStrength > enemyStrength && wars[i]->warScore < 100)
		{
			continue;
		}
		
		//for simplicity, the warscore will just be possitve, and every time a state is taken, 5 will be deducted
		wars[i]->warScore = abs(wars[i]->warScore);

		//we are trying to take as many states as possible
		while (wars[i]->warScore >= 5)
		{
			//for every state neighbooring this one, check to see if the controling nation is the lead beligerent to this nation, if so, take the state:
			for (int j = 0; j < controlledStates.size(); j++)
			{
				//checking states around current one
				//some relative coordinates
				int x = controlledStates[j]->positionInMap.x;
				int y = controlledStates[j]->positionInMap.y;

				//checking to the left
				x -= 1;
				if (x > -1 && isAttacker && (g_map.states[x][y].controller == wars[i]->leadDefender))
				{
					addContolledState(&g_map.states[x][y]);
					wars[i]->warScore -= 5;
					continue;
				}
				if (x > -1 && !isAttacker && (g_map.states[x][y].controller == wars[i]->leadAttacker))
				{
					addContolledState(&g_map.states[x][y]);
					wars[i]->warScore -= 5;
					continue;
				}
				//right
				x += 2;
				if (x < g_map.width && isAttacker && (g_map.states[x][y].controller == wars[i]->leadDefender))
				{
					addContolledState(&g_map.states[x][y]);
					wars[i]->warScore -= 5;
					continue;
				}
				if (x < g_map.width && !isAttacker && (g_map.states[x][y].controller == wars[i]->leadAttacker))
				{
					addContolledState(&g_map.states[x][y]);
					wars[i]->warScore -= 5;
					continue;
				}
				//down
				x -= 1;
				y += 1;
				if (y < g_map.height && isAttacker && (g_map.states[x][y].controller == wars[i]->leadDefender))
				{
					addContolledState(&g_map.states[x][y]);
					wars[i]->warScore -= 5;
					continue;
				}
				if (y < g_map.height && !isAttacker && (g_map.states[x][y].controller == wars[i]->leadAttacker))
				{
					addContolledState(&g_map.states[x][y]);
					wars[i]->warScore -= 5;
					continue;
				}
				//up
				y -= 2;
				if (y > -1 && isAttacker && (g_map.states[x][y].controller == wars[i]->leadDefender))
				{
					addContolledState(&g_map.states[x][y]);
					wars[i]->warScore -= 5;
					continue;
				}
				if (y > -1 && !isAttacker && (g_map.states[x][y].controller == wars[i]->leadAttacker))
				{
					addContolledState(&g_map.states[x][y]);
					wars[i]->warScore -= 5;
					continue;
				}
			}

			//if we got here without anything happening, then get out, no possible states left to take:
			break;
		}

		SetConsoleTextAttribute(hConsole, 5);	// purple
		cout << wars[i]->warName << " has ended" << endl;
		wars[i]->endWar();
		takeStates(); // recursion so that all possible wars can be ended
		SetConsoleTextAttribute(hConsole, 7);	// back to white
		return;	// once all possible wars have ended, return
	}
}

void Nation::initDiplomacy()
{
	for (int i = 0; i < g_map.nations.size(); i++)
	{
		//we don't need relationships with ourselves
		if (g_map.nations[i] == this)
		{
			continue;
		}
		this->diplomaticViews.emplace(g_map.nations[i], DiplomaticView::neutral);
		this->diplomaticRelations.emplace(g_map.nations[i], DiplomaticRelation::nothing);
	}
	//everybody is going to ally afganastan for testing purposes:
	if (this != g_map.nations[0])
		allyWith(g_map.nations[0]);
}

void Nation::removeSelf()
{

	//go through all nations and wars and get rid of references to self
	for (int i = 0; i < g_map.nations.size(); i++)
	{
		//we don't need relationships with ourselves
		if (g_map.nations[i] == this)
		{
			continue;
		}

		g_map.nations[i]->diplomaticViews.erase(this);
		g_map.nations[i]->diplomaticRelations.erase(this);
	}

	//go through all the wars and get rid of this
	for (int i = 0; i < wars.size(); i++)
	{
		for (int j = 0; j < wars[i]->defenders.size(); j++)
		{
			//if we have came across ourselves, get rid of it
			if (wars[i]->defenders[j] == this)
			{
				wars[i]->defenders.erase(wars[i]->defenders.begin() + j);
				//now if all defenders are gone, end the war
				if (wars[i]->defenders.size() == 0)
					wars[i]->endWar();
			}
			
		}
		//same for attackers
		for (int j = 0; j < wars[i]->attackers.size(); j++)
		{
			if (wars[i]->attackers[j] == this)
			{
				wars[i]->attackers.erase(wars[i]->attackers.begin() + j);
				if (wars[i]->attackers.size() == 0)
					wars[i]->endWar();
			}
		}
	}

	//finally, get rid of self in the g_map so it will never be updated again
	for (int i = 0; i < g_map.nations.size(); i++)
	{
		//we don't need relationships with ourselves
		if (g_map.nations[i] == this)
		{
			g_map.nations.erase(g_map.nations.begin() + i);
		}
	}
}

void Nation::updateRelationships()
{
	//itterate through the nation's diplo relations, if the relation is not ally, overlord, or vasal, assume nothing
	for (auto &i : diplomaticRelations)
	{
		if (i.second == ally || i.second == vassal || i.second == overlord)
			continue;
		i.second = nothing;
	}

	//makes sure that any belligarants are actually in the opposing side in wars
	for (int warIndex = 0; warIndex < wars.size(); warIndex++)
	{
		//is this the defender?
		bool isDefender = false;
		for (int i = 0; i < wars[warIndex]->defenders.size(); i++)
		{
			if (wars[warIndex]->defenders[i] == this)
			{
				isDefender = true;
			}
		}

		//all of those who should be this nation's belligerents
		vector<Nation*> belligerents;
		if (isDefender)
			belligerents = wars[warIndex]->attackers;
		else
			belligerents = wars[warIndex]->defenders;
	
		for (int i = 0; i < belligerents.size(); i++)
		{
			belligerents[i]->diplomaticRelations[this] = DiplomaticRelation::belligerent;
			this->diplomaticRelations[belligerents[i]] = DiplomaticRelation::belligerent;
		}
	}
}


void Nation::update()
{
	//if the nation has 0 states, it needs to be removed:
	if (controlledStates.size() == 0)
	{
		removeSelf();
	}

	//for the first time that the update function is called, we want to initialize the diplomatic views/reationships with other nations
	if (diplomaticViews.size() == 0)
	{
		initDiplomacy();
	}

	updateRelationships();

	//IN FUTURE:
	//there will be an array of functions/actions that the nation can do 
	//every nation has the same set of functions/actions, however, every nation has the array in a unique order
	//this update function will cycle through the array from top to bottom and attempt to do the actions at the top first
	//this is how different nations differ from each other

	//MONEY STUFF

	//income: development * 0.05
	this->taxIncome = getDevelopment() * 0.05;
	//state maitenence: (0.3 x number of states)^2
	this->stateMaintenance = pow((controlledStates.size())*0.3, 1.5);
	//same with army
	this->armyMaintenance = pow((army)*0.3, 1.5);

	this->revenue = taxIncome - stateMaintenance - armyMaintenance;
	this->resources += revenue;

	//repair army as much as possible
	repairArmy();

	//DIPLOMATIC STUFF

	//chance of decreasing relations with other nations bordering the current country
	borderConflicts();

	//dont want to be in more than 1 war at a time
	if (wars.size() != 0)
	{
		//peace deals
		takeStates();
	}
	else
	{
		declareWarOnEnemyNeighbor();
	}

	//if at war attack the enemy army
	if (wars.size() != 0)
	{
		attackEnemyArmy();
	}

	//repair army as much as possible (again)
	repairArmy();

	//colonization
	while (true)
	{
		int statesBeforeColonizing = controlledStates.size();
		colonize();
		improveArmy();
		if (controlledStates.size() == statesBeforeColonizing)
			break;
	}
}
