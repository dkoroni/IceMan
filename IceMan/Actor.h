#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <memory>
#include <thread>
//#include "StudentWorld.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


class StudentWorld;
class Actor : public GraphObject
{
public:
	Actor(int imageID, int startX, int startY, Direction startDirection, StudentWorld* _world, float size = 1.0, unsigned int depth = 0);
	~Actor();
	StudentWorld* getWorld() const; 
	int getState() const;
	void setState(int state);
	virtual void doSomething() = 0;
	virtual bool isGoodie() const = 0;
	virtual bool isAnnoy() const = 0;
private:
	StudentWorld* myWorld;
	int state = 1;
};



//ICEMAN=========================================================================================================================
class Iceman :
	public Actor
{
public:
	Iceman(StudentWorld* _world);
	~Iceman();
	void doSomething();
	bool isGoodie() const;
	bool isAnnoy() const;
	int getHealth() const;
	void setHealth(int);
	int getSquirt() const;
	void setSquirt(int);
	int getSonar() const;
	void setSonar(int);
	int getGold() const;
	void setGold(int);
	auto spawnSquirt(int x, int y);
private:
	int health = 10;
	int squirt = 5;
	int sonar = 1;
	int gold = 0;

};

//Stationary====================================================================================================================

class Stationary :
	public Actor 
{
public:
	Stationary(int imageID, int startX, int startY, Direction startDirection, StudentWorld* _world, float size = 1.0, unsigned int depth = 0);
	~Stationary();
	virtual void doSomething() = 0;
	virtual bool isGoodie() const = 0;
	virtual bool isAnnoy() const;
	int getStartX() const;
	int getStartY() const;
private:
	int _x;
	int _y;
};


//ICE============================================================================================================================
class Ice :
	public Stationary
{
public:
	Ice(int startX, int startY, StudentWorld* _world);
	~Ice();
	void doSomething();
	bool isGoodie() const;
	bool isAnnoy() const;
};


//BOULDER========================================================================================================================

class Boulder :
	public Stationary
{
public:
	Boulder(int startX, int startY, StudentWorld* _world);
	~Boulder();
	void doSomething();
	bool isGoodie() const;
	bool isAnnoy() const;
	int getFall() const;
	void changeFall();
private:
	int fall = 0;
	int timer = -1;
};


//SQUIRT================================================================================================================================

class Squirt :
	public Stationary
{
public:
	Squirt(int startX, int startY, Direction startDirection, StudentWorld* _world);
	~Squirt();
	void doSomething();
	bool isGoodie() const { return false; }
	bool isAnnoy() const { return false; }
private:
	int travelDistance = 4;
};

//BARREL================================================================================================================================

class Barrel :
	public Stationary
{
public:
	Barrel(int startX, int startY, StudentWorld* _world);
	~Barrel();
	void doSomething();
	bool isGoodie() const { return true; }
	bool isAnnoy() const { return false; }
private:
	bool revealed = false;
};


//GOLD================================================================================================================================
class Gold :
	public Stationary
{
public:
	Gold(bool startDropped, int startX, int startY, StudentWorld* _world);
	~Gold();
	void doSomething();
	bool isGoodie() const { return true; }
	bool isAnnoy() const { return false; }
private:
	bool revealed = false;
	bool dropped;
	int life = 100;
};

//SONAR==================================================================================================================================
class Sonar :
	public Stationary
{
public:
	Sonar(int startX, int startY, StudentWorld* _world);
	~Sonar();
	void doSomething();
	bool isGoodie() const { return true; }
	bool isAnnoy() const { return false; }
private:
	int tickLife;
};


//WATER==================================================================================================================================


class Water :
	public Stationary
{
public:
	Water(int startX, int startY, StudentWorld* _world);
	~Water();
	void doSomething();
	bool isGoodie() const { return true; }
	bool isAnnoy() const { return false; }
private:
	int tickLife;
};

//PROTESTER==================================================================================================================================

class Protestor :
	public Actor
{
public:
	Protestor(StudentWorld* _world, int image_ID = IID_PROTESTER);
	~Protestor();
	void doSomething();
	bool isGoodie() const { return false; }
	bool isAnnoy() const { return true; }
	int getHealth() const { return health; }
	void setHealth(int setHealth);
	void addTicks(int ticksToAdd) { ticks += ticksToAdd; }
	bool isHardcore() const { return hardcore; }
	void setHardcore(bool _hardcore) { hardcore = _hardcore; }
private:
	int health = 5;
	bool angry = false;
	int currentMovementSquares;
	int ticks = 0;
	int cooldown = 0;
	int lineOfSightMove = 0;
	int cooldownTurning = 0;
	bool hardcore;
};


//HARDCORE_PROTESTOR==============================================================================================================

class HardcoreProtestor :
	public Protestor
{
public:
	HardcoreProtestor(StudentWorld* _world);
	~HardcoreProtestor();
private:

};


#endif // ACTOR_H_

