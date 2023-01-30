// Simulation Source File:
#include "stdafx.h"
#include "simulation.h"

// MACROS:
#define SMALL_VEL (0.01f)

// GLOBALS:
curlingSheet gCurlingSheet;

float gCoeffRestitution = 0.5f;
float gCoeffFriction = 0.03f;
float gGravityAcc = 9.8f;

static const float gRackPositionX[] = { 0.0f, 0.0f,(STONE_RADIUS * 2.0f),(-STONE_RADIUS * 2.0f),(STONE_RADIUS * 4.0f) };
static const float gRackPositionZ[] = { 0.5f, 0.0f,(-STONE_RADIUS * 3.0f),(-STONE_RADIUS * 3.0f) };

/**
	EDGE CLASS MEMBERS:
		Creates two vectors - temp & normal. The first is in the direction of the edge in the X-direction
		while the second is in the direction of the edge in the Y-direction.
	It then calcs two new vectors and normalises and centres - normailises scales both vectors so they have a len of 1.
	Centre calcs where on the screen [0,0], should be placed for each vec, based on their respective lens and angles.
**/
void edge::MakeNormal(void) {
	// Provided - can do in 2-D
	vec2 tempVertices = vertices[1] - vertices[0];
	normal(0) = tempVertices(1);
	normal(1) = -tempVertices(0);
	normal.Normalise();
}

void edge::MakeCentre(void) {
	centre = vertices[0];
	centre = vertices[1];
	centre /= 2.0;
}

/**
	STONE CLASS MEMBERS:
		Defines two variables - rowIndex [stores the current row number], row [stores the current column number.]
	Starts by setting the velocity to Zero [IMPORTANT - It'll help keep the stines in place as they're drawn on screen].
	Iteration through the rows in turn - Starts at rowOne works down to rowZero:
		For each row it calcs how far away from (and to) RowOne (the starting point), that psrticular row is located.
		This info is then used to calc where each stone should be pos onscreen. Final line sets up the pos of all stones in the game.
		It calculates the row and column coordinates of each stone based on its current position and radius.
		Updates the pos of each stone based on these Coords.
**/

int stone::stoneIndxCnt = 0;
void stone::Reset(void) {
	velocity = 0.0;
	if (indx == 0) {
		stonePos(1) = TABLE_Z - 0.25;
		stonePos(0) = 0;
	}
	else {
		stonePos(1) = TABLE_Z + (0.1 * indx);
		stonePos(0) = -0.5;
	}
	if (indx % 2 == 0) {
		isTeamOne = true;
	}
	else { isTeamOne = false; }

}

void stone::ApplyImpulse(vec2 imp) {
	velocity = imp;
}

void stone::ApplyFrictionForce(int ms) {
	if (velocity.Magnitude() <= 0.0) return;
	// Acceleration is opposite to the direction of Motion:
	vec2 accelaration = -velocity.Normalised();
	// F=(MA), thus acceleration = force/mass = constant*g
	accelaration *= (gCoeffFriction * gGravityAcc);
	// Integrate velocity [find the change in V].
	vec2 velChange = ((accelaration * ms) / 1000.0f);
	// Cap magnitude of change in velocity to remove intergration errs.
	if (velChange.Magnitude() > velocity.Magnitude())
		velocity = 0.0;
	else
		velocity += velChange;
}

void stone::DoStoneCollision(stone& s) {
	if (HasHitStone(s))
		HitStone(s);
}

void stone::DoPlaneCollision(const edge& e) {
	if (HasHitPlane(e))
		HitPlane(e);
}

void stone::DoTargetCollision(target& t) {

}

void stone::Update(int ms) {
	// Applying Friction:
	ApplyFrictionForce(ms);
	stonePos += ((velocity * ms) / 1000.f);
	// Setting small velocities to zero.
	if (velocity.Magnitude() < SMALL_VEL)
		velocity = 0.0;
}

bool stone::HasHitPlane(const edge& e) const {
	if (tTarget != NULL)
		return false;
	// If moving away from plane, cannot hit.
	if (velocity.Dot(e.normal) >= 0.0)
		return false;
	// If in front of the Plane, then haven't hit:
	vec2 relPos = stonePos - e.vertices[0];
	double sep = relPos.Dot(e.normal);
	if (sep > radius)
		return false;
	return true;
}

//NOT SURE THIS IS NEEDED AS IT IS FROM THE OG CODE - decide if required and adjust as needed!!
bool stone::HasHitTarget(const target& t) const {
	// work out relative position of stone from target,	distance between stone and target and relative velocity
	vec2 relPosn = stonePos - t.targetCenter;
	float dist = (float)relPosn.Magnitude();
	vec2 relPosnNorm = relPosn.Normalised();

	//if moving away from plane, cannot hit
	if (velocity.Dot(relPosnNorm) >= 0.0) return false;
	//if distnce is more than sum of radi, have not hit
	if (dist > (radius + t.targetRad)) return false;
	return true;
}

bool stone::HasHitStone(const stone& s) const {
	// Works out the relative pos od stone from other stones, distance between stones and relative velocity
	vec2 relPos = stonePos - s.stonePos;
	float dist = (float)relPos.Magnitude();
	vec2 relPosNorm = relPos.Normalised();
	vec2 relVel = velocity - s.velocity;
	// Moving apart cannot hit:
	if (relVel.Dot(relPosNorm) >= 0.0)
		return false;
	// If distnce is more than sum of radii, have not hit
	if (dist > (radius + s.radius))
		return false;
	return true;
}

void stone::HitPlane(const edge& e) {
	// Reverse velocity component perpendicular to plane  
	double comp = velocity.Dot(e.normal) * (1.0 + gCoeffRestitution);
	vec2 delta = -(e.normal * comp);
	velocity += delta;
}



void stone::HitStone(stone& s) {
	// Find direction from other stone to this stone
	vec2 relDir = (stonePos - s.stonePos).Normalised();

	/**
		Split velocities into 2 parts:  one component perpendicular, and one parallel to the collision plane, for both stones.
		(NB the collision plane is defined by the point of contact and the contact normal).
	**/
	float perpV = (float)velocity.Dot(relDir);
	float perpV2 = (float)s.velocity.Dot(relDir);
	vec2 parallelV = velocity - (relDir * perpV);
	vec2 parallelV2 = s.velocity - (relDir * perpV2);

	// Calcs new perpendicular comps.
	float sumMass = mass + s.mass;
	float perpVNew = (float)((perpV * (mass - s.mass)) / sumMass) + (float)((perpV2 * (2.0 * s.mass)) / sumMass);
	float perpVNew2 = (float)((perpV2 * (s.mass - mass)) / sumMass) + (float)((perpV * (2.0 * mass)) / sumMass);

	// Find new velocities by adding unchanged parallel component to new perpendicluar component.
	velocity = parallelV + (relDir * perpVNew);
	s.velocity = parallelV2 + (relDir * perpVNew2);

	// Adding of particles for clashes between stones:
	int randNumParts = (1 % 5) + 5;
	vec3 pos(stonePos(0), radius / 2.0, stonePos(1));
	vec3 oset(relDir(0), 0.0, relDir(1));
	pos += (oset * radius);
	for (int i = 0; i < randNumParts; i++) {
		gCurlingSheet.parts.AddParticle(pos);
	}
}

void stone::SetPlayerStone(void) {
	isPlayerStone = true;
}

/**
	PARTICLE CLASS MEMBERS:
	The particle position is updated every millisec.
	partPos += (partVel * ms) / 1000.0; - The first line adds partPos to the current value of partPos and / 1,000 to get a new value for partVel every millisecond.
	This will cause the velocity of particles to increase over time as they move away from their starting point in space.
	partVel(1) -= (4.0 * ms) / 1000.0; - The second line subtracts 4 times per millisecond from the current velocity and divides that number by 9800 seconds per minute to get a new velocity 
	for each particle at each step in time.
	
**/
void particle::update(int ms) {
	partPos += (partVel * ms) / 1000.0;
	partVel(1) -= (4.0 * ms) / 1000.0; //(9.8*ms)/1000.0;
}

/**
	PARTICLE SET CLASS MEMBERS:
	 Starts by checking to see if the particleSet has reached its maximum number of particles.
		If yes - deletes all particles and starts the count at 0.
	Then a rand value is generated in range (0-200) - this value is used as the velocity for each induvidual particle in the set.
	 The velocities are multiplied by 2, 1, or -1 depending on which side of the center line they fall on respectively (0-100).
	 Finally, every time ms milliseconds pass since last update(), 'i' will be incremented by one until it reaches num whereupon 
	 'i' will stop updating and delete itself from the list so that there are no more updates to do.
**/
void particleSet::AddParticle(const vec3& pos) {
	if (num >= MAX_PARTICLES) return;
	particles[num] = new particle;
	particles[num]-> partPos = pos;

	particles[num]->partVel(0) = ((1% 200) - 100) / 200.0;
	particles[num]->partVel(2) = ((1 % 200) - 100) / 200.0;
	particles[num]->partVel(1) = 2.0 * ((1 % 100) / 100.0);

	num++;
}

void particleSet::update(int ms) {
	int i = 0;
	while (i < num) {
		particles[i]->update(ms);
		if ((particles[i]->partPos(1) < 0.0) && (particles[i]->partVel(1) < 0.0)) {
			delete particles[i];
			particles[i] = particles[num - 1];
			num--;
		}
		else i++;
	}
}

Player::Player() {
	playerNum = 0;
	score = 0;
}

/**
	CURLINGSHEET CLASS MEMBERS:
	Starts by setting up the four edges of the curling sheet.
		First edge is set to have vertices [-TABLE_X, -TABLE_Z].
		Second edge is set to have vertices [0,0].
		Third edge is set to have vertices [1,1].
		Fourth edge is set to have verticies [-TABLE_X, TABLE_Z].
	It then loops through all of the edges setting up the vertices using two variables; one for the x value,
	the other for the z - it sets these variables to be equal to -TABLE_X and TABLE_Z, respectively.
		This ensures that each vertex will be located on either side of the center line
	Finally, it writes these values out to memory so that they can be used later when drawing graphics.
	Variables declared as i and j - which stores the current pos of the left most and right most stones, respectivley.
	It then checks to see if any of the stones have collided with each other.
	The code first sets up a variable to store the number of stones on the curling sheet, as well as the number of rings.
	It then checks for any collisions between each stone and the rings.
	If there are any collisions, the code will then proceed to update each individual stone's position.

		ADD TO WITH A FINISHED DEFINITIONS
**/

void curlingSheet::SetUpEdges(void) {
	edges[0].vertices[0](0) = -TABLE_X;
	edges[0].vertices[0](1) = -TABLE_Z;
	edges[0].vertices[1](0) = -TABLE_X;
	edges[0].vertices[1](1) = TABLE_Z;

	edges[1].vertices[0](0) = -TABLE_X;
	edges[1].vertices[0](1) = TABLE_Z;
	edges[1].vertices[1](0) = TABLE_X;
	edges[1].vertices[1](1) = TABLE_Z;

	edges[2].vertices[0](0) = TABLE_X;
	edges[2].vertices[0](1) = TABLE_Z;
	edges[2].vertices[1](0) = TABLE_X;
	edges[2].vertices[1](1) = -TABLE_Z;

	edges[3].vertices[0](0) = TABLE_X;
	edges[3].vertices[0](1) = -TABLE_Z;
	edges[3].vertices[1](0) = -TABLE_X;
	edges[3].vertices[1](1) = -TABLE_Z;

	for (int i = 0; i < NUM_EDGES; i++) {
		edges[i].MakeCentre();
		edges[i].MakeNormal();
	}
}

void curlingSheet::SetUpRings(void) {
	for (int i = 0; i < NUM_RINGS; i++) {
		rings[i].targetCenter(0) = 0;
		float zLoc = (TABLE_X * 2) - (TARGET_SPACING * NUM_RINGS);
		rings[i].targetCenter(1) = zLoc - TABLE_Z;
	}
}


void curlingSheet::Update(int ms) {

	// Checks for collisions for each stone
	for (int i = 0; i < NUM_STONES; i++) {
		for (int j = (i + 1); j < NUM_RINGS; j++) {
			stones[i].DoTargetCollision(rings[j]);
		}
		for (int j = 0; j < NUM_EDGES; j++) {
			stones[i].DoPlaneCollision(edges[j]);
		}
		for (int j = (i + 1); j < NUM_STONES; j++) {
			stones[i].DoStoneCollision(stones[j]);
		}
	}
	// Update all stones:
	for (int i = 0; i < NUM_STONES; i++) stones[i].Update(ms);
}

bool curlingSheet::AnyStonesMoving(void) const {
	// Return true if any stone has a non-zero velocity.
	for (int i = 0; i < NUM_STONES; i++) {
		if (stones[i].velocity(0) != 0.0) return true;
		if (stones[i].velocity(1) != 0.0) return true;
	}
	return false;
}