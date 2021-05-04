#pragma warning(disable : 4786)

#include "particleSystem.h"
#include "modelerdraw.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>


/***************
 * Constructors
 ***************/

ParticleSystem::ParticleSystem() 
{
	srand(time(NULL));
	/*for (int i = 0; i < num_particles; i++)
	{
		particles.push_back(ParticleSystem::Particle(mass, Vec3f{ 0, 0, 0 }, Vec3f{ 0, 0, 0 },
			Vec3f{ 0, 0, 0 }, emitPos));
	}*/
}





/*************
 * Destructor
 *************/

ParticleSystem::~ParticleSystem() 
{
}


/******************
 * Simulation fxns
 ******************/

/** Start the simulation */
void ParticleSystem::startSimulation(float t)
{
    
	resetSimulation(t);
	bake_start_time = t;

	// These values are used by the UI ...
	// -ve bake_end_time indicates that simulation
	// is still progressing, and allows the
	// indicator window above the time slider
	// to correctly show the "baked" region
	// in grey.
	bake_end_time = -1;
	simulate = true;
	dirty = true;

}

/** Stop the simulation */
void ParticleSystem::stopSimulation(float t)
{
    
	bake_end_time = t;
	bake_fps = (baked_particles.size() - 1) / (bake_end_time - bake_start_time);
	// These values are used by the UI
	simulate = false;
	dirty = true;

}

// random bias: range(-1,1)
Vec3f getBias()
{
	float y = (rand() % 100 - 50) / 50.0;
	float z = (rand() % 100 - 50) / 50.0;
	float x = (rand() % 100 - 50) / 50.0;
	return Vec3f(x, y, z);
}
/** Reset the simulation */
void ParticleSystem::resetSimulation(float t)
{
	baked_particles.clear();
	time_stamp = t;
	particles.clear();
	for (int i = 0; i < num_particles; i++)
	{
		Vec3f bias = getBias() / 7.0f;
		particles.push_back(ParticleSystem::Particle(mass, Vec3f{ 0, 0, 0 }, Vec3f{ 0, 0, 0 },
			Vec3f{ 0, 0, 0 }, emitPos + bias));
	}
	// These values are used by the UI
	simulate = false;
	dirty = true;

}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t)
{
	// update the particles
	float dt = t - time_stamp;
	time_stamp = t;
	for (int i = 0; i < particles.size(); i++)
	{
		float ageBias = getBias()[0] / 1.5f;
		if (particles[i].age > 1.5 + ageBias)
		{
			Vec3f g{ 0, 2, 0 };
			particles[i].a = g;
			if (lastEmitPos[0] == 0 && lastEmitPos[1] == 0 && lastEmitPos[2] == 0)
				particles[i].v = Vec3f(0, 0, 0);
			else
				particles[i].v = (emitPos - lastEmitPos) / dt;
			Vec3f bias = getBias() / 7.0;
			particles[i].p = emitPos + bias;
			particles[i].age = 0;
		}
		else
		{
			Vec3f g{ 0, 2, 0 };
			// todo: recalculate another F
			//particles[i].a = particles[i].F / particles[i].m;
			Vec3f gBias = getBias() / 2.0f;
			particles[i].a = g + gBias;
			particles[i].v += dt * particles[i].a;
			particles[i].p += dt * particles[i].v;
			particles[i].age += dt;
		}
		
	}
	if (simulate)
		baked_particles.push_back(particles);
}


/** Render particles */
void ParticleSystem::drawParticles(float t)
{
	// check whether have a brake frame at t
	int frame_i = (int)((t - bake_start_time) * bake_fps);
	setDiffuseColor(1, 1, 1);
	if (frame_i < baked_particles.size())
	{
		for (int i = 0; i < baked_particles[frame_i].size(); i++)
		{
			glPushMatrix();

			glTranslated(baked_particles[frame_i][i].p[0],
				baked_particles[frame_i][i].p[1],
				baked_particles[frame_i][i].p[2]);
			drawSphere(0.01);

			glPopMatrix();

			// second spawn pos
			glPushMatrix();

			glTranslated(baked_particles[frame_i][i].p[0],
				baked_particles[frame_i][i].p[1],
				baked_particles[frame_i][i].p[2]);
			Vec3f diff = emitPos2 - emitPos;
			glTranslated(diff[0], diff[1], diff[2]);
			drawSphere(0.01);
			
			glPopMatrix();
		}
	}
	else
	{
		for (int i = 0; i < particles.size(); i++)
		{
			glPushMatrix();

			glTranslated(particles[i].p[0],
				particles[i].p[1],
				particles[i].p[2]);
			drawSphere(0.01);

			glPopMatrix();

			// second spawn pos
			glPushMatrix();

			glTranslated(particles[i].p[0],
				particles[i].p[1],
				particles[i].p[2]);
			Vec3f diff = emitPos2 - emitPos;
			glTranslated(diff[0], diff[1], diff[2]);
			drawSphere(0.01);

			glPopMatrix();
		}
	}
}





/** Adds the current configuration of particles to
  * your data structure for storing baked particles **/
void ParticleSystem::bakeParticles(float t) 
{
}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{
}



Mat4f getModelViewMatrix()
{
	/**************************
	**
	**	GET THE OPENGL MODELVIEW MATRIX
	**
	**	Since OpenGL stores it's matricies in
	**	column major order and our library
	**	use row major order, we will need to
	**	transpose what OpenGL gives us before returning.
	**
	**	Hint:  Use look up glGetFloatv or glGetDoublev
	**	for how to get these values from OpenGL.
	**
	*******************************/

	GLfloat m[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
	Mat4f matMV(m[0], m[1], m[2], m[3],
		m[4], m[5], m[6], m[7],
		m[8], m[9], m[10], m[11],
		m[12], m[13], m[14], m[15]);

	return matMV.transpose(); // convert to row major
}