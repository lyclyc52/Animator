/***********************
 * ParticleSystem class
 ***********************/

/**
 * The particle system class simply "manages" a collection of particles.
 * Its primary responsibility is to run the simulation, evolving particles
 * over time according to the applied forces using Euler's method.
 * This header file contains the functions that you are required to implement.
 * (i.e. the rest of the code relies on this interface)
 * In addition, there are a few suggested state variables included.
 * You should add to this class (and probably create new classes to model
 * particles and forces) to build your system.
 */

#ifndef __PARTICLE_SYSTEM_H__
#define __PARTICLE_SYSTEM_H__

#include "vec.h"
#include <vector>
#include "mat.h"

using namespace std;
class ParticleSystem {

public:


	/** Constructor **/
	ParticleSystem();


	/** Destructor **/
	virtual ~ParticleSystem();

	/** Simulation fxns **/
	// This fxn should render all particles in the system,
	// at current time t.
	virtual void drawParticles(float t);

	// This fxn should save the configuration of all particles
	// at current time t.
	virtual void bakeParticles(float t);

	// This function should compute forces acting on all particles
	// and update their state (pos and vel) appropriately.
	virtual void computeForcesAndUpdateParticles(float t);

	// This function should reset the system to its initial state.
	// When you need to reset your simulation, PLEASE USE THIS FXN.
	// It sets some state variables that the UI requires to properly
	// update the display.  Ditto for the following two functions.
	virtual void resetSimulation(float t);

	// This function should start the simulation
	virtual void startSimulation(float t);

	// This function should stop the simulation
	virtual void stopSimulation(float t);

	// This function should clear out your data structure
	// of baked particles (without leaking memory).
	virtual void clearBaked();	



	// These accessor fxns are implemented for you
	float getBakeStartTime() { return bake_start_time; }
	float getBakeEndTime() { return bake_end_time; }
	float getBakeFps() { return bake_fps; }
	bool isSimulate() { return simulate; }
	bool isDirty() { return dirty; }
	void setDirty(bool d) { dirty = d; }
	Vec3f getInitialV() { return initialV; }
	void setInitialV(Vec3f v) { initialV = v; }
	void setEmitPos(Vec3f p) { lastEmitPos = emitPos; emitPos = p; }
	void setEmitPos2(Vec3f p) { emitPos2 = p; }
	Vec3f topLeft{ -2, 1.5, -2 };
protected:
	class Particle
	{
	public:
		Particle(float m, Vec3f v, Vec3f a, Vec3f F, Vec3f p)
			: m(m), v(v), a(a), F(F), p(p) {}
		float m;
		Vec3f v;
		Vec3f a;
		Vec3f p;
		Vec3f F;
		Vec3f p_prev;
		float age{ 0 };
	};

	int num_particles{ 2000 };
	//Particle* particles;
	
	vector<Particle> particles;
	vector<vector<Particle>> baked_particles;

	float time_stamp;
	Vec3f emitPos{ 0, 0, 0 };
	Vec3f emitPos2{ 0, 0, 0 };
	Vec3f lastEmitPos{ 0, 0, 0 };
	Vec3f initialV{ 0, 0, 0 };
	float mass{ 1 };

	// spring cloth
	int numParticlesWidth = 20;
	int numParticlesHeight = 20;
	float clothHeight = 4.0f;
	float clothWidth = 4.0f;
	
	float structualL0{ clothHeight / (numParticlesHeight - 1) };
	float structualK{ 10 * 10/ structualL0 };
	float shearL0{ clothHeight / (numParticlesHeight - 1) * sqrtf(2) };
	float shearK{ 10 * 10 / shearL0 };
	vector<vector<Particle>> clothParticles;
	/** Some baking-related state **/
	float bake_fps;						// frame rate at which simulation was baked
	float bake_start_time;				// time at which baking started 
										// These 2 variables are used by the UI for
										// updating the grey indicator 
	float bake_end_time;				// time at which baking ended

	/** General state variables **/
	bool simulate;						// flag for simulation mode
	bool dirty;							// flag for updating ui (don't worry about this)

};

Mat4f getModelViewMatrix();
#endif	// __PARTICLE_SYSTEM_H__
