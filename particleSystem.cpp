#pragma warning(disable : 4786)

#include "particleSystem.h"
#include "modelerdraw.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

constexpr float eps = 0.0001f;
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
	resetSimulation(0);
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

	// if cloth
	clothParticles.clear();
	for (int r = 0; r < numParticlesHeight; r++)
	{
		vector<Particle> row;
		for (int c = 0; c < numParticlesWidth; c++)
		{
			Vec3f pos = topLeft + Vec3f(((float)c / (numParticlesWidth- 1)) * clothWidth, 
				-((float)r / (numParticlesHeight - 1)) * clothHeight, 0);
			ParticleSystem::Particle p(mass, Vec3f{ 0, 0, 0 }, Vec3f{ 0, 0, 0 },
				Vec3f{ 0, 0, 0 }, pos);
			p.p_prev = pos;
			row.push_back(p);
		}
		clothParticles.push_back(row);
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

	// cloth
	for (int r = 0; r < numParticlesHeight; r++)
	{
		for (int c = numParticlesWidth - 1; c >=0; c--)
		{
			if (r == 0 && c == 0)
			{
				clothParticles[r][c].p = topLeft;
				continue;
			}
			else if (r == 0 && c == numParticlesWidth - 1)
			{
				clothParticles[r][c].p = topLeft + Vec3f(clothWidth, 0, 0);
				continue;
			}
			clothParticles[r][c].F = Vec3f{ 0, 0, 0 };
			if (c >= 1)
			{
				Vec3f dist = (clothParticles[r][c - 1].p_prev - clothParticles[r][c].p_prev);
				//if(dist.length() > eps)
					clothParticles[r][c].F += structualK * dist / dist.length() 
					* (dist.length() - structualL0);
				/*if (r == 1 && c == numParticlesHeight - 1)
					cout << "F(x-1,y): " << structualK * dist / dist.length()
					* (dist.length() - structualL0) << " dist: " << dist << endl;*/
			}
			if (r >= 1)
			{
				Vec3f dist = (clothParticles[r - 1][c].p_prev - clothParticles[r][c].p_prev);
				//if (dist.length() > eps)
					clothParticles[r][c].F += structualK * dist / dist.length()
					* (dist.length() - structualL0);
				/*if (r == 1 && c == numParticlesHeight - 1)
					cout << "F(x,y-1): " << structualK * dist / dist.length()
					* (dist.length() - structualL0) << " dist: " << dist << endl;*/
			}
			if (c < numParticlesWidth - 1)
			{
				Vec3f dist = (clothParticles[r][c + 1].p_prev - clothParticles[r][c].p_prev);
				//if (dist.length() > eps)
					clothParticles[r][c].F += structualK * dist / dist.length()
					* (dist.length() - structualL0);
				/*if (r == 1 && c == numParticlesHeight - 1)
					cout << "F(x+1, y): " << structualK * dist / dist.length()
					* (dist.length() - structualL0) << " dist: " << dist << endl;*/
			}
			if (r < numParticlesHeight - 1)
			{
				Vec3f dist = (clothParticles[r + 1][c].p_prev - clothParticles[r][c].p_prev);
				//if (dist.length() > eps)
					clothParticles[r][c].F += structualK * dist / dist.length()
					* (dist.length() - structualL0);
				/*if (r == 1 && c == numParticlesHeight - 1)
					cout << "F(x, y+1): " << structualK * dist / dist.length()
					* (dist.length() - structualL0) << " dist: " << dist << endl;*/
			}
			if (r >= 1 && c >= 1)
			{
				Vec3f dist = (clothParticles[r - 1][c - 1].p_prev - clothParticles[r][c].p_prev);
				//if (dist.length() > eps)
					clothParticles[r][c].F += shearK * dist / dist.length()
					* (dist.length() - shearL0);
			}
			if (r <= numParticlesHeight - 2 && c <= numParticlesWidth - 2)
			{
				Vec3f dist = (clothParticles[r + 1][c + 1].p_prev - clothParticles[r][c].p_prev);
				//if (dist.length() > eps)
					clothParticles[r][c].F += shearK * dist / dist.length()
					* (dist.length() - shearL0);
			}
			if (r <= numParticlesHeight - 2 && c >= 1)
			{
				Vec3f dist = (clothParticles[r + 1][c - 1].p_prev - clothParticles[r][c].p_prev);
				//if (dist.length() > eps)
					clothParticles[r][c].F += shearK * dist / dist.length()
					* (dist.length() - shearL0);
			}
			if (r >= 1 && c <= numParticlesWidth - 2)
			{
				Vec3f dist = (clothParticles[r - 1][c + 1].p_prev - clothParticles[r][c].p_prev);
				//if (dist.length() > eps)
					clothParticles[r][c].F += shearK * dist / dist.length()
					* (dist.length() - shearL0);
			}
			Vec3f g{ 0, -2, 0 };
			Vec3f wind{ 0, 0, -0.6 };
			Vec3f windBias = getBias() / 3;
			wind += windBias;
			clothParticles[r][c].F += g * mass + wind;
			clothParticles[r][c].a = clothParticles[r][c].F / mass;
			/*if (r == 1 && c == 0)
			{
				cout << "k: " << structualK << endl;
				cout << "l: " << structualL0 << endl;
				cout << "F cumu: " << clothParticles[r][c].F << endl;
				cout << "a: " << clothParticles[r][c].a << endl;
				cout << "----------------------------------------" << endl;
			}*/
			clothParticles[r][c].v += dt * clothParticles[r][c].a;
			clothParticles[r][c].p += dt * clothParticles[r][c].v;
			clothParticles[r][c].v *= 0.9;
			clothParticles[r][c].a *= 0.8;
		}
		
	}
	// update prev
	for (int r = 0; r < numParticlesHeight; r++)
	{
		for (int c = numParticlesWidth - 1; c >= 0; c--)
		{
			clothParticles[r][c].p_prev = clothParticles[r][c].p;
		}
	}
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

	// draw cloth
	setDiffuseColor(1, 0, 0);
	for (int r = 0; r < numParticlesHeight - 1; r++)
	{
		for (int c = 0; c < numParticlesWidth - 1; c++)
		{
			glPushMatrix();

			/*glTranslated(clothParticles[r][c].p[0],
				clothParticles[r][c].p[1],
				clothParticles[r][c].p[2]);
			drawSphere(0.3);*/
			Particle pt1 = clothParticles[r][c];
			Particle pt2 = clothParticles[r+1][c];
			Particle pt3 = clothParticles[r+1][c+1];
			Particle pt4 = clothParticles[r][c+1];
			
			drawTriangle(pt1.p[0], pt1.p[1], pt1.p[2],
				pt2.p[0], pt2.p[1], pt2.p[2], 
				pt4.p[0], pt4.p[1], pt4.p[2]);
			drawTriangle(pt2.p[0], pt2.p[1], pt2.p[2],
				pt3.p[0], pt3.p[1], pt3.p[2],
				pt4.p[0], pt4.p[1], pt4.p[2]);

			/*if (r == 0 && c == 0)
			{
				cout << "pt1: " << pt1.p  << endl
					<< "pt2: " << pt2.p << endl;
			}*/
			/*drawTriangle(0, 0, 0,
				0, 4, 0, 0, 4, 4);*/
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