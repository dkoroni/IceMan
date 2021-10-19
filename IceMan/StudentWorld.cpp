#include "StudentWorld.h"
#include <string>
#include <queue>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

StudentWorld::~StudentWorld() {

}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp



//checks to see if protestors are nearby gold and updates score
bool StudentWorld::protestorNearbyGold(int x, int y) {
	for (auto thing : allProtestors) {
		if (thing != nullptr && thing->getState() != 0 && getDistance(x, y, thing) <= 3) {
			thing->addTicks(max(50, 100 - (int)getLevel() * 10));
			if (thing->isHardcore) {
				increaseScore(50);
			}
			else {
				increaseScore(25);
			}
			return true;
		}
	}
	return false;
}


//checks to see if protestor is nearby a water squirt and updatse score
bool StudentWorld::protestorNearbySquirt(int x, int y) {
	for (auto thing : allProtestors) {
		if (thing != nullptr && thing->getState() != 0 && getDistance(x, y, thing) <= 3) {
			thing->setHealth(thing->getHealth() - 2);
			if (thing->getHealth() < 0) {
				if (thing->isHardcore())
					increaseScore(250);
				else
					increaseScore(100);
			}
			return true;
		}
	}
	return false;
}

//reveals all objects nearby the character if theyre not dead
void StudentWorld::useSonar() {
	for (auto thing : allActors) {
		if (thing != nullptr && thing->getState()!= 0) {
			if (getDistance(mainChar->getX(), mainChar->getY(), thing) <= 12)
				thing->setVisible(true);
		}
	}
}

//spawns a specific type of gold that is only pickable by protestors
void StudentWorld::spawnGold() {
	allActors.push_back(make_shared<Gold>(true, mainChar->getX(), mainChar->getY(), this));
}

//returns a position for an eligible water spot by reference
void StudentWorld::findRandomWater(int& x, int& y) {
	x = rand() % 61;
	y = rand() % 61;
	if (isWalkable(x, y) == false)
		findRandomWater(x, y);
}


//counts to see if a goodie should be spawned and if enough time has passed it spawns a random goodie
void StudentWorld::spawnGoodie() {
	if (tick % ((int)getLevel() * 25 + 30) == 0) {
		if (rand() % 101 <= 20) {
			allActors.push_back(make_shared<Sonar>(0, 60, this));
		}
		else {
			int x = 0;
			int y = 0;
			findRandomWater(x,y);
			allActors.push_back(make_shared<Water>(x, y, this));
		}
	}
}


//checks to see what type of protestor to spawn and how often depending on the level
void StudentWorld::spawnProtestor() {
	int hardcoreProbability = min(90, (int)getLevel() * 10 + 30);
	int timeUntilNextProtestor = max(25, 200 - (int)getLevel());
	int targetNumberOfProtestors = min(15, 2 + (int)(getLevel()*1.5));
	shared_ptr<Protestor> pro;
	if (tick == 0) {
		if (rand() % 101 < hardcoreProbability) {
			pro = make_shared<HardcoreProtestor>(this);
			allActors.push_back(pro);
			allProtestors.push_back(pro);
			incrementProtestors();
		}
		else {
			pro = make_shared<Protestor>(this);
			allActors.push_back(pro);
			allProtestors.push_back(pro);
			incrementProtestors();
		}
	}
	else if (tick % timeUntilNextProtestor == 0) {
		if (rand() % 101 < hardcoreProbability) {
			pro = make_shared<HardcoreProtestor>(this);
			allActors.push_back(pro);
			allProtestors.push_back(pro);
			incrementProtestors();
		}
		else {
			pro = make_shared<Protestor>(this);
			allActors.push_back(pro);
			allProtestors.push_back(pro);
			incrementProtestors();
		}
	}

}

//returns the iceField(i didnt use this helper function as much as i thought i would)
auto StudentWorld::getIceField() const { return iceField; }


//removes ice at the specific location in the size of the character's body
void StudentWorld::removeIce(int x, int y) {
	if (y >= 60) {
		y = 60;
	}
	if (x >= 60) {
		x = 60;
	}
	for (int i = x; i < x + 4; i++) {
		for (int j = y; j < y + 4; j++) {
			if (iceField[i][j] != nullptr) {
				iceField[i][j].reset();
			}
		}
	}
}


//removes ice at a specific point (used for spawning rocks)
void StudentWorld::removeIcePoint(int x, int y) {
	if (iceField[x][y] != nullptr)
		iceField[x][y].reset();

}

//sets up the original icefield at the start of every level
void StudentWorld::setIceField() {
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 60; j++) {
			if (i < 30 || i > 33) {
				iceField[i][j] = (make_shared<Ice>(i, j, this));
				//cout << "made an icecube at (" << i << ", " << j << ")\n";
			}
			else if (j < 4) {
				iceField[i][j] = make_shared<Ice>(i, j, this);
			}
		}
	}
}

//checks if there is any ice directly below the x,y point up to x + 3 (used for checking below boulders)
bool StudentWorld::checkIce(int x, int y) const {
	bool isIceBelow = false;
	for (int i = 0; i < 4; i++) {
		if (iceField[x + i][y - 1] != nullptr)
			isIceBelow = true;
	}
	return isIceBelow;
}


//checks if there is any ice in the specific area 4x4 grid from x,y to x+3,y+3
bool StudentWorld::checkIceArea(int x, int y) const {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (iceField[x + i][y + j] == nullptr)
				return false;
		}
	}
	return true;
}


//function that moves the rock passed to it down 1 movement at a time while checking for objects for collision
void StudentWorld::dropBoulder(Boulder* rock) {
	if (rock->getY() == 0)
		rock->setState(0);
	for (auto thing : allProtestors) {
		if (getDistance(rock->getX(), rock->getY(), thing) <= 3) {
			thing->setHealth(thing->getHealth() - 100);
			increaseScore(500);			
		}
	}
	if (getDistance(rock->getX(), rock->getY(), mainChar) < 3) {
		mainChar->setHealth(0);
		return;
	}
	for (int i = 0; i < 4; i++) {
		if (anyIceBoulder(rock->getX() + i,rock->getY() - 1)){
			rock->setState(0);
			return;
		}
	}
	rock->moveTo(rock->getX(), rock->getY() - 1);
	//depecrated code
	//if (checkIce(rock->getX(), rock->getY()) == false) {
	//	rock->moveTo(rock->getX(), rock->getY() - 1);
	//}
	//else {
	//	rock->setState(0);
	//}
}


//sets up the boulder fields at the beginning of the level
void StudentWorld::setBoulder() {
	int amount = min((int)getLevel() / 2 + 2, 9);
	
	int x = rand() % 61;
	int y = (rand() % 37) + 20;
	shared_ptr<Boulder> boulderHolder;
	for (int i = 0; i < amount; i++) {
		//boulderHolder = make_shared<Boulder>(x, y, this);
		
		// for loop checking to see if the current boulder is far enough away from all other boulders
		getBoulderPosition(x, y);
		boulderHolder = make_shared<Boulder>(x, y, this);
		
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++)
				this->removeIcePoint(x + j, y + k);
		}

		allBoulders.push_back(boulderHolder);
		allActors.push_back(boulderHolder);
		
	}
}


//returns the distance between 2 points as a double
double StudentWorld::getDistance(int x, int y, shared_ptr<Actor> b) const{
	double xDistance = x - b->getX();
	double yDistance = y - b->getY();
	return pow(xDistance*xDistance + yDistance*yDistance, 0.5);
}


//gets an eligible boulder position for spawn (must be 6 away from all other objects)
void StudentWorld::getBoulderPosition(int& x, int& y) {
	while ((x <= 34 && x >= 25)) {
		x = rand() % 61;
	}
	for (auto thing : allActors) {
		if (thing != nullptr) {
			while (getDistance(x, y, thing) <= 6) {
				x = rand() % 61;
				y = (rand() % 37) + 20;
				getBoulderPosition(x, y);
			}
		}
	}
}


//gets an eligible oil/nugget position for spawn (must be 6 away from all other objects)
void StudentWorld::getNuggetOilPosition(int&x, int&y) {
	for (auto thing : allActors) {
		if (thing != nullptr) {
			while (!checkIceArea(x,y) || getDistance(x, y, thing) <= 6) {
				x = rand() % 61;
				y = rand() % 57;
				getNuggetOilPosition(x, y);
			}
		}
	}
}


//checks if there is any ice or bouldrs in the specific x,y point
bool StudentWorld::anyIceBoulder(int x, int y) const {
	if (iceField[x][y] != nullptr)
		return true;
	for (auto thing : allBoulders) {
		if (thing != nullptr) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (thing->getX() + i == x && thing->getY() + j == y)
						return true;
				}
			}
		}
	}
	return false;
}


//checks if there is any boulders in the specific x,y point
bool StudentWorld::anyBoulder(int x, int y) const {
	for (auto thing : allBoulders) {
		if (thing != nullptr && thing->getState() != 0) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (thing->getX() + i == x && thing->getY() + j == y)
						return true;
				}
			}
		}
	}
	return false;
}


//spawns a water squirt on the field and adds it to the actors
void StudentWorld::addSquirt(shared_ptr<Squirt>& squirt) {
	allActors.push_back(squirt);
}


//shows a barrel if the character is close enough
bool StudentWorld::barrelShow(int x, int y) const {
	if (getDistance(x, y, mainChar) <= 4.0)
		return true;
	return false;
}


//picks up a barrel if the character is close enough
bool StudentWorld::barrelPickUp(int x, int y) const {
	if (getDistance(x, y, mainChar) <= 3.0)
		return true;
	return false;
}

//sets down all the barrels needed to be collected for the current level
void StudentWorld::setBarrel() {

	int amount = min(2 + (int)getLevel(), 21);

	int x = rand() % 61;
	int y = rand() % 57;
	shared_ptr<Barrel> barrelHolder;
	for (int i = 0; i < amount; i++) {


		// for loop checking to see if the current barrel is far enough away
		getNuggetOilPosition(x, y);
		barrelHolder = make_shared<Barrel>(x, y, this);
		
		allActors.push_back(barrelHolder);
		barrelsLeft++;

	}
}



void StudentWorld::incrementGold() {
	mainChar->setGold(mainChar->getGold() + 1);
}

void StudentWorld::decrementGold() {
	mainChar->setGold(mainChar->getGold() - 1);
}

void StudentWorld::incrementSonar() {
	mainChar->setSonar(mainChar->getSonar() + 1);
}

void StudentWorld::decrementSonar() {
	mainChar->setSonar(mainChar->getSonar() - 1);
}

void StudentWorld::incrementSquirt() {
	mainChar->setSquirt(mainChar->getSquirt() + 5);
}

void StudentWorld::decrementSquirt() {
	mainChar->setSquirt(mainChar->getSquirt() - 1);
}


//spawns all the gold needed for the current map
void StudentWorld::setGold() {
	int amount = max(5 - (int)getLevel()/2, 2);
	int x = rand() % 61;
	int y = rand() % 57;
	shared_ptr<Gold> goldHolder;
	for (int i = 0; i < amount; i++) {
		
		// for loop checking to see if the current barrel is far enough away
		getNuggetOilPosition(x, y);
		goldHolder = make_shared<Gold>(false, x, y, this);

		allActors.push_back(goldHolder);

	}
}


void StudentWorld::traverseIcemanMaze(int x, int y) {
	thread thre([this, x, y] {this->threadIcemanMaze(x, y); });
	thre.detach();
}

void StudentWorld::traverseMazeExit(int x, int y) {
	thread th([this,x,y] {this->threadMazeExit(x,y); });
	th.detach();
}

//threaded iceman maze solution that doesnt always work and is currently bugged :(
//i have no idea how to fix this
void StudentWorld::threadIcemanMaze(int x, int y) {
	n.lock();
	queue<point> q;
	point start = { mainChar->getX(), mainChar->getY(), 0 };
	point guess;
	point hold;
	q.push(start);
	while (!q.empty()) {
		guess = q.front();
		icemanSolution[guess.x][guess.y] = guess.value;
		q.pop();
		if (guess.x == x && guess.y == y)
			return;
		//up
		if (isValidIcemanPoint(guess.x, guess.y + 1, guess.value) == true) {
			hold.x = guess.x;
			hold.y = guess.y + 1;
			hold.value = guess.value + 1;
			q.push(hold);
		}
		//down
		if (isValidIcemanPoint(guess.x, guess.y - 1, guess.value) == true) {
			hold.x = guess.x;
			hold.y = guess.y - 1;
			hold.value = guess.value + 1;
			q.push(hold);
		}
		//left
		if (isValidIcemanPoint(guess.x - 1, guess.y, guess.value) == true) {
			hold.x = guess.x - 1;
			hold.y = guess.y;
			hold.value = guess.value + 1;
			q.push(hold);
		}
		//right
		if (isValidIcemanPoint(guess.x + 1, guess.y, guess.value) == true) {
			hold.x = guess.x + 1;
			hold.y = guess.y;
			hold.value = guess.value + 1;
			q.push(hold);
		}
	}
	n.unlock();
}

//solution to the maze exit using a breadth-first search
//and value concept  (only updates if the current protestor is not nearby
// an existing pathway)
void StudentWorld::threadMazeExit(int x, int y) {
	m.lock();
	queue<point> q;
	point start = { 60, 60, 0 };
	point guess;
	point hold;
	q.push(start);
	while (!q.empty()) {
		guess = q.front();
		mazeSolution[guess.x][guess.y] = guess.value;
		q.pop();
		if (guess.x == x && guess.y == y)
			return;
		//up
		if (isValidPoint(guess.x, guess.y + 1, guess.value) == true) {
			hold.x = guess.x;
			hold.y = guess.y + 1;
			hold.value = guess.value + 1;
			q.push(hold);
		}
		//down
		if (isValidPoint(guess.x, guess.y - 1, guess.value) == true) {
			hold.x = guess.x;
			hold.y = guess.y - 1;
			hold.value = guess.value + 1;
			q.push(hold);
		}
		//left
		if (isValidPoint(guess.x - 1, guess.y, guess.value) == true) {
			hold.x = guess.x - 1;
			hold.y = guess.y;
			hold.value = guess.value + 1;
			q.push(hold);
		}
		//right
		if (isValidPoint(guess.x + 1, guess.y, guess.value) == true) {
			hold.x = guess.x + 1;
			hold.y = guess.y;
			hold.value = guess.value + 1;
			q.push(hold);
		}		
	}
	m.unlock();

}

//checks to see if a point is valid to be checked for the exit maze solution
bool StudentWorld::isValidPoint(int x, int y, int value){
	if (x == 60 && y == 60)
		return false;
	if (x > 60 || y > 60 || x < 0 || y < 0)
		return false;
	if (mazeSolution[x][y] < value && mazeSolution[x][y] > 0)
		return false;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (anyIceBoulder(x + i, y + j) == true)
				return false;
		}
	}
	return true;
}


//checks to see if a point is valid to be checked for the iceman maze solution
bool StudentWorld::isValidIcemanPoint(int x, int y, int value) {
	if (y == 60 && value < icemanSolution[x][y]) {
		return true;
	}
	if (x > 60 || y > 60 || x < 0 || y < 0)
		return false;
	if (icemanSolution[x][y] < value)
		return false;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (anyIceBoulder(x + i, y + j) == true)
				return false;
		}
	}
	return true;
}


//returns if a point is walkable by a protestor (no boulder/ice)
bool StudentWorld::isWalkable(int x, int y) {
	if (y == 60)
		return true;
	if (y > 60 || y < 0)
		return false;
	if (x > 60 || x < 0)
		return false;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (anyIceBoulder(x + i, y + j) == true)
				return false;
		}
	}
	return true;
}


//returns the correct direction to walk to get closer to the iceman
int StudentWorld::checkForIceman(int x, int y) const {
	int upValue = 0;
	if (y < 60)
		upValue = icemanSolution[x][y] - icemanSolution[x][y + 1];

	int downValue = 0;
	if (y > 0)
		downValue = icemanSolution[x][y] - icemanSolution[x][y - 1];

	int leftValue = 0;
	if (x > 0)
		leftValue = icemanSolution[x][y] - icemanSolution[x - 1][y];

	int rightValue = 0;
	if (x < 60)
		rightValue = icemanSolution[x][y] - icemanSolution[x + 1][y];

	if (upValue >= downValue && upValue >= leftValue && upValue >= rightValue && upValue > 0)
		return 1;

	if (downValue >= upValue && downValue >= leftValue && downValue >= rightValue && downValue > 0)
		return 2;

	if (leftValue >= upValue && leftValue >= downValue && leftValue >= rightValue && leftValue > 0)
		return 3;

	if (rightValue >= upValue && rightValue >= downValue && rightValue >= leftValue && rightValue > 0)
		return 4;


	return -1;
}


//returns the correct direction to walk to get to the exit
int StudentWorld::checkForExit(int x, int y) const {
	if (y == 60)
		return 4;
	int upValue = 0;
	if (y < 60)
		upValue = mazeSolution[x][y] - mazeSolution[x][y + 1];

	int downValue = 0;
	if (y > 0)
		downValue = mazeSolution[x][y] - mazeSolution[x][y - 1];

	int leftValue = 0;
	if (x > 0)
		leftValue = mazeSolution[x][y] - mazeSolution[x - 1][y];

	int rightValue = 0;
	if (x < 60)
		rightValue = mazeSolution[x][y] - mazeSolution[x + 1][y];

	if (upValue >= downValue && upValue >= leftValue && upValue >= rightValue && upValue > 0)
		return 1;

	if (downValue >= upValue && downValue >= leftValue && downValue >= rightValue && downValue > 0)
		return 2;

	if (leftValue >= upValue && leftValue >= downValue && leftValue >= rightValue && leftValue > 0)
		return 3;

	if (rightValue >= upValue && rightValue >= downValue && rightValue >= leftValue && rightValue > 0)
		return 4;
	
	
	return -1;



	//up
	//if (y < 60 && mazeSolution[x][y + 1] < mazeSolution[x][y])
	//	return 1;
	////down
	//else if (y > 0 && mazeSolution[x][y - 1] < mazeSolution[x][y])
	//	return 2;
	////left
	//else if (x > 1 && mazeSolution[x - 1][y] < mazeSolution[x][y])
	//	return 3;
	////right
	//else if (x < 60 && mazeSolution[x + 1][y] < mazeSolution[x][y])
	//	return 4;
	//return -1;
}

//checks to see if the protestor is facing iceman 
bool StudentWorld::checkFacingIceman(int x, int y, int direction) const {
	if (getDistance(x, y, mainChar) <= 4) {
		switch (direction) {
		case 1:
			if (y < mainChar->getY())
				return true;
			break;
		case 2:
			if (y > mainChar->getY())
				return true;
			break;
		case 3:
			if (x > mainChar->getX())
				return true;
			break;
		case 4:
			if (x < mainChar->getX())
				return true;
			break;
		}
	}
	return false;
}


void StudentWorld::decrementHealth(int x) {
	mainChar->setHealth(mainChar->getHealth() - x);
}


//checks if the protestor has line of sight with iceman (no boulder/ice blocking)
bool StudentWorld::checkLineofSightIceman(int x, int y, int& decision) {
	if (getDistance(x, y, mainChar) < 4)
		return false;
	if (x == mainChar->getX()) {
		//directly above iceman
		if (y > mainChar->getY()) {
			decision = 2;
			for (int i = mainChar->getY(); i < y;  i++) {
				if (isWalkable(x, i) == false) {
					decision = 0;
					return false;
				}
			}
			return true;
		}
		//directly below iceman
		if (y < mainChar->getY()) {
			decision = 1;
			for (int i = y; i < mainChar->getY(); i++) {
				if (isWalkable(x, i) == false) {
					decision = 0;
					return false;
				}
			}
			return true;
		}
	}
	if (y == mainChar->getY()) {
		//directly to the right of iceman
		if (x > mainChar->getX()) {
			decision = 3;
			for (int i = mainChar->getX(); i < x; i++) {
				if (isWalkable(i, y) == false) {
					decision = 0;
					return false;
				}
			}
			return true;
		}
		//directly to the left of iceman
		if (x < mainChar->getX()) {
			decision = 4;
			for (int i = x; i < mainChar->getX(); i++) {
				if (isWalkable(i, y) == false) {
					decision = 0;
					return false;
				}
			}
			return true;
		}
	}
	return false;
}

double StudentWorld::icemanDistance(int x, int y) const{
	return getDistance(x, y, mainChar);
}