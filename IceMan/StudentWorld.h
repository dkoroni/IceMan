#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <mutex>
using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

const int faceUp = 1;
const int faceDown = 2;
const int faceRight = 3;
const int faceLeft = 4;

struct point {
	int x;
	int y;
	int value;
};

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
	}

	~StudentWorld();

	void setIceField();

	auto getIceField() const;

	virtual int init()
	{
		mainChar = make_shared<Iceman>(this);
		/*allActors.push_back(make_shared<Boulder>(12, 30, this));*/
		barrelsLeft = 0;
		setIceField();
		setBoulder();
		setBarrel();
		setGold();
		tick = 0;
		protestors = 0;
		//shared_ptr<HardcoreProtestor> single = make_shared<HardcoreProtestor>(this);
		//traverseIcemanMaze(60, 60);
		//for (int i = 60; i > 48; i--) {
		//	for (int j = 30; j < 61; j++) {
		//		cout << icemanSolution[j][i] << " ";
		//	}
		//	cout << endl;
		//}
		//allActors.push_back(single);
		return GWSTATUS_CONTINUE_GAME;
	}


	virtual int move()
	{
		setGameStatText("Lvl: " + to_string((int)getLevel()) + "  Lives: " + to_string((int)getLives()) + "  Hlth: " + to_string(mainChar->getHealth() * 10) + "%  Wtr: "
			+ to_string(mainChar->getSquirt()) + "  Gld: " + to_string(mainChar->getGold()) + "  Oil Left: " + to_string(barrelsLeft) + "  Sonar: "
			+ to_string(mainChar->getSonar()) + "  Scr: " + to_string((int)getScore()));

		// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
		spawnProtestor();
		spawnGoodie();
		
		tick++;
		if (barrelsLeft == 0) {
			playSound(SOUND_FINISHED_LEVEL);
			return GWSTATUS_FINISHED_LEVEL;
		}
		for (auto thing : allActors) {
			if (thing != nullptr && thing->getState() == 0) {
				thing->setVisible(false);
				thing.reset();
			}
		}
		for (auto thing : allProtestors) {
			if (thing != nullptr && thing->getState() == 0) {
				thing->setVisible(false);
				thing.reset();
			}
		}
		for (auto thing : allBoulders) {
			if (thing != nullptr && thing->getState() == 0) {
				thing->setVisible(false);
				thing.reset();
			}
		}
		
		mainChar->doSomething();
		for (auto thing : allActors) {
			thing->doSomething();
		}
		if (mainChar->getState() == 0) {
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}
		return GWSTATUS_CONTINUE_GAME;

	}

	virtual void cleanUp()
	{

		for (auto thing : allActors) {
			thing.reset();
		}
		allActors.clear();
		for (auto thing : allBoulders) {
			thing.reset();
		}
		allBoulders.clear();
		for (auto thing : allProtestors) {
			thing.reset();
		}
		allProtestors.clear();
	}




	StudentWorld* getWorld() {
		return this;
	}

	bool protestorNearbySquirt(int x, int y);

	bool protestorNearbyGold(int x, int y);

	void useSonar();

	void spawnGold();

	void findRandomWater(int& x, int& y);

	void spawnGoodie();

	void spawnProtestor();

	int getTick() { return tick; }

	void removeIce(int x, int y);

	void removeIcePoint(int x, int y);

	bool checkIce(int x, int y) const;

	bool checkIceArea(int x, int y) const;

	void dropBoulder(Boulder* rock);

	void setBoulder();

	double getDistance(int x, int y, shared_ptr<Actor> b) const;

	void getBoulderPosition(int& x, int& y);

	void getNuggetOilPosition(int&x, int&y);

	bool anyIceBoulder(int x, int y) const;

	bool anyBoulder(int x, int y) const;

	void addSquirt(shared_ptr<Squirt>& squirt);

	bool barrelShow(int x, int y) const;

	bool barrelPickUp(int x, int y) const;

	void setBarrel();

	void incrementGold();

	void decrementGold();

	void incrementSonar();

	void decrementSonar();

	void incrementSquirt();

	void decrementSquirt();

	void setGold();

	void decrementBarrel() { barrelsLeft--; }

	void traverseIcemanMaze(int x, int y);

	void traverseMazeExit(int x, int y);

	void threadIcemanMaze(int x, int y);

	void threadMazeExit(int x, int y);
	
	bool isValidPoint(int x, int y, int value);

	bool isValidIcemanPoint(int x, int y, int value);

	bool isWalkable(int x, int y);

	int checkForIceman(int x, int y) const;

	int checkForExit(int x, int y) const;

	bool checkFacingIceman(int x, int y, int direction) const;

	void decrementHealth(int x);
	
	bool checkLineofSightIceman(int x, int y, int& decision);

	double icemanDistance(int x, int y) const;

	int getIcemanMazeValue(int x, int y) const { return icemanSolution[x][y]; }

	void decrementProtestors() { protestors++; }

	void incrementProtestors() { protestors--; }

private:
	mutex m;
	mutex n;
	shared_ptr<Iceman> mainChar;
	shared_ptr<Ice> iceField[64][64];
	vector<shared_ptr<Actor>> allActors;
	vector<shared_ptr<Boulder>> allBoulders;
	vector<shared_ptr<Protestor>> allProtestors;
	int tick;
	int current_level = 1;
	int protestors = 0;
	int barrelsLeft = 0;
	int mazeSolution[61][61];
	int icemanSolution[61][61];
};



#endif // STUDENTWORLD_H_
