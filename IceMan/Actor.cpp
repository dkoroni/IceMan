#include "Actor.h"
#include "StudentWorld.h"
//#include <memory>
#include <algorithm>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(int imageID, int startX, int startY, Direction startDirection, StudentWorld* _world, float size, unsigned int depth) 
: GraphObject(imageID, startX, startY, startDirection, size, depth) {
	this->setVisible(true);
	myWorld = _world;
	
}

Actor::~Actor() {
	this->setVisible(false);
}

int Actor::getState() const { return state; }

StudentWorld* Actor::getWorld() const{
	return myWorld;
}


void Actor::setState(int _state) { state = _state; }


//ICEMAN=========================================================================================================================

Iceman::Iceman(StudentWorld* _world)
	: Actor(IID_PLAYER, 30, 60, right, _world, 1.0, 0) {

}
Iceman::~Iceman() {

}

auto Iceman::spawnSquirt(int x, int y) {
	switch (getDirection()) {
	case up:
		return make_shared<Squirt>(x, y + 1, getDirection(), getWorld());
		break;
	case down:
		return make_shared<Squirt>(x, y - 1, getDirection(), getWorld());
		break;
	case left:
		return make_shared<Squirt>(x - 1, y, getDirection(), getWorld());
		break;
	case right:
		return make_shared<Squirt>(x + 1, y, getDirection(), getWorld());
		break;
	}
	return make_shared<Squirt>(x, y, getDirection(), getWorld());
}

void Iceman::doSomething() {
	if (health <= 0)
		setState(0);
	if (getState() == 0)
		return;
	int ch;
	if (getWorld()->getKey(ch) == true) {
		switch (ch) {
			//this part checks to see what button has been pressed
			//if a movement button has been pressed it will check if there is any boulder there
			//if no boulder exist, then remove the ice and move into the position
		case KEY_PRESS_UP:
			if (getDirection() != up) {
				setDirection(up);
				break;
			}
			if (getY() == 60)
				break;
			if (!(getWorld()->anyBoulder(getX(), getY() + 4)) && !(getWorld()->anyBoulder(getX() + 4, getY() + 4))) {
				getWorld()->removeIce(getX(), getY() + 1);
				moveTo(getX(), getY() + 1);
			}
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() != down) {
				setDirection(down);
				break;
			}
			if (getY() == 0)
				break;
			if (!(getWorld()->anyBoulder(getX(), getY() - 1)) && !(getWorld()->anyBoulder(getX() + 4, getY() - 1))) {
				getWorld()->removeIce(getX(), getY() - 1);
				moveTo(getX(), getY() - 1);
			}
			break;
		case KEY_PRESS_LEFT:
			if (getDirection() != left) {
				setDirection(left);
				break;
			}
			if (getX() == 0)
				break;
			if (!(getWorld()->anyBoulder(getX() - 1, getY())) && !(getWorld()->anyBoulder(getX() - 1, getY() + 4))) {
				getWorld()->removeIce(getX() - 1, getY());
				moveTo(getX() - 1, getY());
			}
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() != right) {
				setDirection(right);
				break;
			}
			if (getX() == 60)
				break;
			if (!(getWorld()->anyBoulder(getX() + 4,getY())) && !(getWorld()->anyBoulder(getX() + 4, getY() + 4))) {
				getWorld()->removeIce(getX() + 1, getY());
				moveTo(getX() + 1, getY());
			}
			break;
		//Lose a life
		case KEY_PRESS_ESCAPE:
			getWorld()->decLives();
			setState(0);
			break;
		//Squirt out some water
		case KEY_PRESS_SPACE:
			if (squirt >= 1) {
				squirt--;
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
				getWorld()->addSquirt(spawnSquirt(getX(), getY()));
			}
			break;
			//drop a gold
		case KEY_PRESS_TAB:
			if (gold >= 1) {
				gold--;
				getWorld()->spawnGold();
			}
			break;
		case'z':
		case 'Z':
			//use a sonar that im not sure if its working right but i think it does sometimes
			if (sonar >= 1) {
				sonar--;
				getWorld()->useSonar();
			}
			break;
		}
	}
}	

bool Iceman::isGoodie() const { return false; }

bool Iceman::isAnnoy() const { return true; }

//setters-getters
int Iceman::getHealth() const { return health; }

void Iceman::setHealth(int _health) { health = _health; }

int Iceman::getSquirt() const { return squirt; }

void Iceman::setSquirt(int _squirt) { squirt = _squirt; }

int Iceman::getSonar() const { return sonar; }

void Iceman::setSonar(int _sonar) { sonar = _sonar; }

int Iceman::getGold() const { return gold; }

void Iceman::setGold(int _gold) { gold = _gold; }




//ICEMAN=========================================================================================================================

//STATIONARY====================================================================================================================

Stationary::Stationary(int imageID, int startX, int startY, Direction startDirection, StudentWorld* _world, float size, unsigned int depth) 
	: Actor(imageID, startX, startY, startDirection, _world, size, depth)
{
	_x = startX;
	_y = startY;
}

Stationary::~Stationary() {

}

bool Stationary::isAnnoy() const { return false; }

int Stationary::getStartX() const { return _x; }

int Stationary::getStartY() const { return _y; }


//STATIONARY====================================================================================================================

//ICE============================================================================================================================

Ice::Ice(int startX, int startY, StudentWorld* _world)
	:Stationary(IID_ICE, startX, startY, right, _world, 0.25, 3) {

}

Ice::~Ice() {}

void Ice::doSomething() {}

bool Ice::isGoodie() const {
	return false;
}

bool Ice::isAnnoy() const {
	return false;
}
//ICE============================================================================================================================


//BOULDER===========================================================================================================================

Boulder::Boulder(int startX, int startY, StudentWorld* _world)
	:Stationary(IID_BOULDER, startX, startY, down, _world, 1.0, 1) {

}

Boulder::~Boulder() {

}
//	fall state of 0 = hasn't fallen
//  fall state of 1 = waiting to fall
//  fall state of 2 = actively falling
void Boulder::doSomething() {
	if (getState() == 0)
		return;
	if (getFall() == 2) {
		getWorld()->dropBoulder(this);
	}
	if (getFall() == 1 && timer > 0) {
		if (getWorld()->getTick() == timer) {
			changeFall();
			getWorld()->playSound(SOUND_FALLING_ROCK);
		}
	}
	if (getFall() == 1 && timer < 0) {
		timer = getWorld()->getTick() + 30;
	}
	if (getFall() == 0) {
		if (getWorld()->checkIce(getStartX(), getStartY()) == false) {
			changeFall();
		}
	}
	
}
bool Boulder::isGoodie() const { return false; }
bool Boulder::isAnnoy() const { return false; }
int Boulder::getFall() const { return fall; }
void Boulder::changeFall() {
	fall++;
}

//BOULDER===========================================================================================================================

//SQUIRT================================================================================================================================

Squirt::Squirt(int startX, int startY, Direction startDirection, StudentWorld* _world)
	: Stationary(IID_WATER_SPURT, startX, startY, startDirection, _world, 1.0, 1) {

}


Squirt::~Squirt() {

}

void Squirt::doSomething() {
	if (getState() == 0) {
		return;
	}
	//if it hits any obstacles it dies
	if (getWorld()->anyIceBoulder(getX(), getY()) == true) {
		setVisible(false);
		this->setState(0);
		return;
	}
	if (travelDistance == 0) {
		setVisible(false);
		this->setState(0);
		return;
	}
	if (getWorld()->protestorNearbySquirt(getX(), getY())) {
		setState(0);
	}
	if (travelDistance > 0) {
		switch (this->getDirection()) {

			//this switch case updates the squirts movement and if it hits any ice or boulder it dies
		case up:
			if (getWorld()->anyIceBoulder(getX(), getY() + 3) == true) {
				setVisible(false);
				setState(0);
				return;
			}
			moveTo(getX(), getY() + 1);
			break;
		case down:
			if (getWorld()->anyIceBoulder(getX(), getY() - 1) == true) {
				setState(0);
				return;
			}
			moveTo(getX(), getY() - 1);
			break;
		case left:
			if (getWorld()->anyIceBoulder(getX() - 1, getY()) == true) {
				setState(0);
				return;
			}
			moveTo(getX() - 1, getY());
			break;
		case right:
			if (getWorld()->anyIceBoulder(getX() + 3, getY()) == true) {
				setVisible(false);
				setState(0);
				return;
			}
			moveTo(getX() + 1, getY());
			break;
		}
	}
	travelDistance--;
}
//SQUIRT================================================================================================================================

//BARREL================================================================================================================================

Barrel::Barrel(int startX, int startY, StudentWorld* _world)
	:Stationary(IID_BARREL, startX, startY, right, _world, 1, 2) {
	setVisible(false);
}

Barrel::~Barrel() {

}


void Barrel::doSomething() {
	if (getState() == 0)
		return;
	//reveals itself if close enough
	if (!revealed && getWorld()->barrelShow(getX(), getY())) {
		setVisible(true);
		revealed = true;
		return;
	}
	//gets chugged if its touching
	if (revealed && getWorld()->barrelPickUp(getX(), getY())) {
		setState(0);
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
		getWorld()->decrementBarrel();
	}

}


//BARREL================================================================================================================================


//GOLD================================================================================================================================


Gold::Gold(bool startDropped, int startX, int startY, StudentWorld* _world)
	:Stationary(IID_GOLD, startX, startY, right, _world, 1, 2) {
	dropped = startDropped;
	if (!dropped) {
		setVisible(false);
	}
	else {
		setVisible(true);
		revealed = true;
	}
}

Gold::~Gold() {

}

void Gold::doSomething() {
	if (getState() == 0)
		return;
	//reveals if not dropped and the iceman is close enough
	if (!dropped && !revealed && getWorld()->barrelShow(getX(), getY())) {
		setVisible(true);
		revealed = true;
		return;
	}
	//if close enough it'll get piocked up by the iceman
	if (!dropped && revealed && getWorld()->barrelPickUp(getX(), getY())) {
		setState(0);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(10);
		getWorld()->incrementGold();
		return;
	}
	if (dropped) {
		//if the gold is dropped it starts ticking down and waiting for a protestor
		life--;
		if (getWorld()->protestorNearbyGold(getX(),getY())) {
			setState(0);
			getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
			//getWorld()->increaseScore(25);
		}
	}
	//if it has existed for too long it dies
	if (life <= 0) {
		setState(0);
		setVisible(false);
		return;
	}
}

//GOLD================================================================================================================================

//SONAR==================================================================================================================================

Sonar::Sonar(int startX, int startY, StudentWorld* _world) 
 : Stationary(IID_SONAR, startX, startY, right, _world, 1, 2) {
	tickLife = getWorld()->getTick() + max(100, 300-10*(int)getWorld()->getLevel());
}
Sonar::~Sonar() {

}
void Sonar::doSomething() {
	if (getState() == 0) { return; }
	if (getWorld()->getTick() == tickLife) {
		setState(0);
		return;
	}
	if (getWorld()->barrelPickUp(getX(), getY())) {
		setState(0);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(75);
		getWorld()->incrementSonar();
	}
}


//SONAR==================================================================================================================================


//WATER==================================================================================================================================

Water::Water(int startX, int startY, StudentWorld* _world)
	: Stationary(IID_WATER_POOL, startX, startY, right, _world, 1, 2) {
	tickLife = getWorld()->getTick() + max(100, 300 - 10 * (int)getWorld()->getLevel());
}
Water::~Water() {

}
void Water::doSomething() {
	if (getState() == 0) { return; }
	if (getWorld()->getTick() == tickLife) {
		setState(0);
		return;
	}
	if (getWorld()->barrelPickUp(getX(), getY())) {
		setState(0);
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(100);
		getWorld()->incrementSquirt();
	}
}

//WATER==================================================================================================================================

//PROTESTOR==================================================================================================================================

Protestor::Protestor(StudentWorld* _world, int image_ID)
	: Actor(image_ID, 60, 60, left, _world, 1.0, 0 ){
	currentMovementSquares = rand() % 53 + 8;
	ticks = max(0, 3 - (int)getWorld()->getLevel() / 4);
	hardcore = false;
}
Protestor::~Protestor() {
	getWorld()->decrementProtestors();
}

//this sets the health while checking to see if the protestor has died every time it takes damage
void Protestor::setHealth(int setHealth) {
	health = setHealth;
	if (health > 0) {
		getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
	}
	ticks += max(50, 100 - (int)(getWorld()->getLevel()) * 10);
}

void Protestor::doSomething() {
	if (getState() == 0)
		return;
	if (health < 0 && angry == false) {
		angry = true;
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		ticks = 0;
	}
	if (ticks > 0) {
		ticks--;
		return;
	}
	cooldown--;
	cooldownTurning--;
	ticks = max(0, 3 - (int)getWorld()->getLevel() / 4);

	//leaving the icefield when angry using maze solution and queue search thing algorithm
	if (angry == true) {
		if (getX() == 60 && getY() == 60) {
			setState(0);
			return;
		}
		int nextMove = getWorld()->checkForExit(getX(), getY());
		switch (nextMove) {
		case -1:
			getWorld()->traverseMazeExit(getX(), getY());
			break;
		case 1:
			setDirection(up);
			moveTo(getX(), getY() + 1);
			break;
		case 2:
			setDirection(down);
			moveTo(getX(), getY() - 1);
			break;
		case 3:
			setDirection(left);
			moveTo(getX() - 1, getY());
			break;
		case 4:
			setDirection(right);
			moveTo(getX() + 1, getY());
			break;
		}
		return;
	}
	//checking if iceman is within a range of 4
	if (getWorld()->checkFacingIceman(getX(), getY(), (int)getDirection())) {
		if (cooldown <= 0) {
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			getWorld()->decrementHealth(2);
			cooldown = 15;
			return;
		}
	}

	//looking for iceman as hardcore protestor only
	if (hardcore == true && getWorld()->icemanDistance(getX(), getY()) > 4.0) {
		getWorld()->traverseIcemanMaze(getX(), getY());
		if (getWorld()->getIcemanMazeValue(getX(), getY()) <= (16 + (int)getWorld()->getLevel() * 2)) {
			int nextMove = getWorld()->checkForIceman(getX(), getY());
			switch (nextMove) {
			case -1:
				break;
			case 1:
				if (getWorld()->isWalkable(getX(), getY() + 1)) {
					setDirection(up);
					moveTo(getX(), getY() + 1);
				}
				break;
			case 2:
				if (getWorld()->isWalkable(getX(), getY() - 1)) {
					setDirection(down);
					moveTo(getX(), getY() - 1);
				}
				break;
			case 3:
				if (getWorld()->isWalkable(getX() - 1, getY())) {
					setDirection(left);
					moveTo(getX() - 1, getY());
				}
				break;
			case 4:
				if (getWorld()->isWalkable(getX() + 1, getY())) {
					setDirection(right);
					moveTo(getX() + 1, getY());
				}
				break;
			}
			return;
		}
	
	}


	//checking if iceman is in line of sight horizontally/vertically
	if (getWorld()->checkLineofSightIceman(getX(), getY(), lineOfSightMove) == true) {
		switch (lineOfSightMove) {
		case 0: 
			break;
		case 1:
			setDirection(up);
			moveTo(getX(), getY() + 1);
			break;
		case 2:
			setDirection(down);
			moveTo(getX(), getY() - 1);
			break;
		case 3:
			setDirection(left);
			moveTo(getX() - 1, getY());
			break;
		case 4:
			setDirection(right);
			moveTo(getX() + 1, getY());
			break;
		}
		lineOfSightMove = 0;
		currentMovementSquares = 0;
		return;
	}
	currentMovementSquares--;

	//choosing a new direction at random when number of steps is 0
	if (currentMovementSquares <= 0) {
		bool directionUnknown = true;
		while (directionUnknown) {
			Direction possibleDirection = ((Direction)(rand() % 4 + 1));
			switch (possibleDirection) {
			case up:
				if (getWorld()->isWalkable(getX(), getY() + 1)) {
					setDirection(up);
					directionUnknown = false;
				}
				break;
			case down:
				if (getWorld()->isWalkable(getX(), getY() - 1)) {
					setDirection(down);
					directionUnknown = false;
				}
				break;
			case left:
				if (getWorld()->isWalkable(getX() - 1, getY())) {
					setDirection(left);
					directionUnknown = false;
				}
				break;
			case right:
				if (getWorld()->isWalkable(getX() + 1, getY())) {
					setDirection(right);
					directionUnknown = false;
				}
				break;
			}
		}
		currentMovementSquares = rand() % 53 + 8;
	}

	//choosing a new perpendicular direction if its been long enough
	if (cooldownTurning <= 0) {
		switch (getDirection()) {
		case up:
		case down:
			//can only turn right or left when moving up or down
			if (getWorld()->isWalkable(getX() + 1, getY()) || getWorld()->isWalkable(getX() - 1, getY()))
			{
				//moving right
				if (rand() % 10 >= 5 && getWorld()->isWalkable(getX() + 1, getY())) {
					setDirection(right);
					moveTo(getX() + 1, getY());
				}
				//moving left
				else if (getWorld()->isWalkable(getX() - 1, getY())) {
					setDirection(left);
					moveTo(getX() - 1, getY());
				}
				//moving right
				else {
					setDirection(right);
					moveTo(getX() + 1, getY());
				}
				currentMovementSquares = rand() % 53 + 8;
				cooldownTurning = 200;
			}
			break;
		case left:
		case right:
			//can only turn up or down when moving right or left
			if (getWorld()->isWalkable(getX(), getY() + 1) || getWorld()->isWalkable(getX(), getY() - 1))
			{
				//moving up
				if (rand() % 10 >= 5 && getWorld()->isWalkable(getX(), getY() + 1)) {
					setDirection(up);
					moveTo(getX(), getY() + 1);
				}
				//moving down
				else if (getWorld()->isWalkable(getX(), getY() - 1)) {
					setDirection(down);
					moveTo(getX(), getY() - 1);
				}
				//moving up
				else {
					setDirection(up);
					moveTo(getX(), getY() + 1);
				}
				currentMovementSquares = rand() % 53 + 8;
				cooldownTurning = 200;
			}
			break;
		}
	}

	//finally time to move based on the direction
	switch (getDirection()) {
	case up:
		if (getWorld()->isWalkable(getX(), getY() + 1))
			moveTo(getX(), getY() + 1);
		else
			currentMovementSquares = 0;
		break;
	case down:
		if (getWorld()->isWalkable(getX(), getY() - 1))
			moveTo(getX(), getY() - 1);
		else
			currentMovementSquares = 0;
		break;
	case left:
		if (getWorld()->isWalkable(getX() - 1, getY()))
			moveTo(getX() - 1, getY());
		else
			currentMovementSquares = 0;
		break;
	case right:
		if (getWorld()->isWalkable(getX() + 1, getY()))
			moveTo(getX() + 1, getY());
		else
			currentMovementSquares = 0;
		break;
	}
}
//PROTESTOR==================================================================================================================================

//HARDCORE_PROTESTOR==============================================================================================================


//this literally only changes the regular protestor's private variable hardcore into true
HardcoreProtestor::HardcoreProtestor(StudentWorld* _world) 
	:Protestor(_world, IID_HARD_CORE_PROTESTER) {
	setHardcore(true);
	setHealth(20);
}


HardcoreProtestor::~HardcoreProtestor() {

}



//HARDCORE_PROTESTOR==============================================================================================================