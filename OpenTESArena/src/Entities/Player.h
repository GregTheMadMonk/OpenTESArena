#ifndef PLAYER_H
#define PLAYER_H

#include "Camera3D.h"
#include "CharacterClass.h"

class Game;

enum class GenderName;

class Player
{
private:
	CharacterClass charClass;
	Camera3D camera;
	Double3 velocity;
	double maxWalkSpeed, maxRunSpeed; // Eventually a function of 'Speed'.
	GenderName gender;
	int raceID;
	std::string displayName;
	int portraitID;
	// Other stats...
public:
	Player(const std::string &displayName, GenderName gender, int raceID,
		const CharacterClass &charClass, int portraitID, const Double3 &position,
		const Double3 &direction, const Double3 &velocity, double maxWalkSpeed,
		double maxRunSpeed);
	~Player();

	const Double3 &getPosition() const;
	const std::string &getDisplayName() const;
	std::string getFirstName() const;
	int getPortraitID() const;
	GenderName getGenderName() const;
	int getRaceID() const;
	const CharacterClass &getCharacterClass() const;

	// Gets the direction the player is facing.
	const Double3 &getDirection() const;

	// Gets the direction pointing right from the player's direction.
	const Double3 &getRight() const;

	// Gets the bird's eye view of the player's direction (in the XZ plane).
	Double2 getGroundDirection() const;

	// Teleports the player to a point.
	void teleport(const Double3 &position);

	// Rotates the player's camera based on some change in X (left/right) and Y (up/down).
	void rotate(double dx, double dy, double hSensitivity, double vSensitivity);

	// Changes the velocity (as a force) given a normalized direction, magnitude, 
	// and delta time, as well as whether the player is running.
	void accelerate(const Double3 &direction, double magnitude,
		bool isRunning, double dt);

	// Tick the player by delta time for motion, etc..
	void tick(Game &game, double dt);
};

#endif
