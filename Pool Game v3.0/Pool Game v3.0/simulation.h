// Simulation Header File:
#include "vecmath.h"

// Macros - Identifiers that represent statements or expressions:
#define TABLE_X			(0.25f) 
#define TABLE_Z			(1.25f)
#define TABLE_Y			(0.2f)
#define STONE_RADIUS	(0.035f)
#define STONE_MASS		(0.1f)
#define TWO_PI			(6.2832f)
#define	SIM_UPDATE_MS	(10)
#define NUM_STONES		(16) // Needs to be changed.		
#define NUM_EDGES		(4)	
#define NUM_RINGS		(4)
#define TARGET_SPACING	(0.065f)
#define MAX_PARTICLES	(150)
#define MAX_PLAYERS		(4)
#define MIN_PLAYERS		(2)

/**
	EDGE CLASS:
		Defines three variables - vertices, centre, and normal.
			Vertices - stores the coords of the 1st two points in a 2D space, Centre - stores the point @ which the
			vectors cross, Normal - stores the direction perpendicular to that vector.
		The MakeNormal() & MakeCentre() funcs are called from in the main() - they take no arguments and return their
		respective values.
		Vertices store the X & Y coords of each vertex and while centre stores the Z coord of each vertex
		They're used to create a normal vector for each vertex and centre
		**/
class edge {
public:
	vec2 vertices[2]; // 2D
	vec2 centre;
	vec2 normal;

	void MakeNormal(void);
	void MakeCentre(void);
};

/**
	TARGET CLASS:
		 The code attempts to create a vector2 object with the center of the target and its radius.
**/
class target {
	static int targetIndxCnt;
public:
	vec2 targetCenter;
	float targetRad;
	target() {
		targetCenter = 0;
		targetRad = TARGET_SPACING;
	}
};


/**
	STONE CLASS:
	Members/ Variables of the Stone Class:
		static int stoneIndxCnt [keep track of how many stones are in the game, a vec2 position, a velocity,
		a float radius, and a float mass.]
		stonePos [vector that represents the position of the stone on the screen]
		velocity [vector that represents the velocity of the stone.]
		radius [The radius of the stone]
		mass [Mass of the stone]
		index [integer that keeps track of which stone is being displayed on-screen] - this increments
		by one each time a new stone is added to the scence.

	Reset() method that sets all member variables to their default values it's called once at startup of the program so all
	stones have an equal starting point and are moving at uniform speeds around the screen.
	ApplyImpulse(), ApplyFrictionForce(), DoPlaneCollision(), DoStoneCollision(), and Update() methods are responsible for
	performing actions todo with collisions between stones, targets, or players.
	[These methods take two args, vec2 Obj - collision vector between objs involved in the interaction, int Obj - indicates
	which method should be called].
**/
class stone {
	static int stoneIndxCnt;
public:
	vec2 stonePos;
	vec2 velocity;
	float radius;
	float mass;
	int indx;
	target* tTarget;
	bool isPlayerStone;

	stone() : stonePos(0.0), velocity(0.0), radius(STONE_RADIUS), mass(STONE_MASS), tTarget(NULL), isPlayerStone(false) {
		indx = stoneIndxCnt++;
		Reset();
	}

	void Reset(void);
	void ApplyImpulse(vec2 imp);
	void ApplyFrictionForce(int ms);
	void DoPlaneCollision(const edge& e);
	void DoStoneCollision(stone& s);
	void DoTargetCollision(target& t);
	void Update(int ms);

	bool HasHitPlane(const edge& e) const;
	bool HasHitStone(const stone& s) const;
	bool HasHitTarget(const target& t) const;

	void HitPlane(const edge& e);
	void HitStone(stone& s);

	void SetPlayerStone(void);
};

/**
	Particle Class:
	Contains  two functions consisting of both the constructor and destructor.
		Constructor - creates an empty vector of particles, 
		Destructor  - deletes all particles in memory.
	Update - called every frame to move each particle according to its velocity and position.
	partPos - vec3, which will be used for storing the position of each particle in 3D space.
	partVel - vel3, which will store the velocity of each particle as it moves through time.
	AddParticle (Function) - takes one argument: a point on 3D space where we want to add our new particle at partPos.
		Calls update with ms set to 1 so that this function will be executed once per frame.
	An array called particles that stores pointers to all active particles in memory;
	Num is set equal 0 when this class is created so no other objects are added into memory yet.
**/
class particle {
public:
	vec3 partPos;
	vec3 partVel;

	particle() {
		partPos = 0;
		partVel = 0;
	}
	void update(int ms);
};

class particleSet {
public:
	particle* particles[MAX_PARTICLES];
	int num;
	particleSet() {
		for (int i = 0; i < MAX_PARTICLES; i++) particles[i] = 0;
		num = 0;
	}
	~particleSet() {
		for (int i = 0; i < MAX_PARTICLES; i++) {
			if (particles[i]) delete particles[i];
		}
	}
	void AddParticle(const vec3& partPos);
	void update(int ms);
};

/**
	CURLING SHEET CLASS:
		Defines three variables stones, edges, rings
	The SetUpEdges() function is called first [Sets up the edges of the game board] - creates an array of integers called edges
	Also creates an array of integers called targets, finally, it sets up a loop that will run repeatedly until you
	stop it by calling the Stop() function.
	SetUpRings() function is then called [sets up the rings on the game board] - creates an array of integers called rings
	The SetUpEdges() function will be used to initialize the edges and rings variables.
	The SetUpRings() function will be used to initialize the target rings variable.
	The AnyStonesMoving() function will be used to check if any of the stones, edges, target rings or particles are moving.
**/
class curlingSheet {
public:
	stone stones[NUM_STONES];
	edge edges[NUM_EDGES];
	target rings[NUM_RINGS];
	particleSet parts;
	int currntStone = 0;
	void SetUpEdges(void);
	void SetUpRings(void);
	void Update(int ms);
	bool AnyStonesMoving(void) const;
};


// Global curling sheet: 
// Extern [tells the compiler that a variable is defined in another source module (outside of the current scope)]
extern curlingSheet gCurlingSheet;