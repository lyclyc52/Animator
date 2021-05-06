#pragma warning(disable : 4786)

#include "particleSystem.h"
#include "modelerdraw.h"
#include "modelerapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include "bitmap.h"
#include <FL/fl_ask.H>
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
	if (flocking)
	{
		for (int i = 0; i < numFlockings; i++)
		{
			Vec3f pBias = getBias() * localThresh;
			Vec3f vBias = getBias() / 8 / 10 / 10;
			Vec3f initialV(0.1, 0, 0);
			Vec3f initialP(0, 0, 0);
			particles.push_back(ParticleSystem::Particle(mass, initialV + vBias, Vec3f{ 0, 0, 0 },
				Vec3f{ 0, 0, 0 }, initialP + pBias));
		}
	}
	else if (fireworks)
	{
		for (int i = 0; i < num_fireworks; i++)
		{
		
			Vec3f pBias = getBias();
			Vec3f vBias = getBias();
			Vec3f initialV = getBias() * fireworksV;
			Vec3f initialP(0, 0, 0);
			particles.push_back(ParticleSystem::Particle(mass, initialV + vBias, Vec3f{ 0, 0, 0 },
				Vec3f{ 0, 0, 0 }, initialP + pBias));
		}
	}
	else if (snow)
	{
		for (int i = 0; i < num_snow; i++)
		{
			Vec3f pBias = getBias() * 8;
			Vec3f initialP(0, 16, 0);
			particles.push_back(ParticleSystem::Particle(mass, Vec3f{ 0, 0, 0 }, Vec3f{ 0, 0, 0 },
				Vec3f{ 0, 0, 0 }, initialP + pBias));
		}
	}
	else if (fire)
	{
		for (int i = 0; i < num_fire; i++)
		{
			Vec3f pBias = getBias() * 0.3;
			pBias[1] = 0;
			Vec3f initialP(0, 0, 0);
			Vec3f initialV = Vec3f(0, 2, 0) + getBias();
			particles.push_back(ParticleSystem::Particle(mass, initialV, Vec3f{ 0, 0, 0 },
				Vec3f{ 0, 0, 0 }, initialP + pBias));
		}
	}
	else
	{
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
				Vec3f pos = topLeft + Vec3f(((float)c / (numParticlesWidth - 1)) * clothWidth,
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
}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t)
{
	// update the particles
	float dt = t - time_stamp;
	time_stamp = t;
	if (flocking)
	{
		
		for (int i = 0; i < numFlockings; i++)
		{
			int neighbourNum = 0;
			Vec3f aSep(0, 0, 0), vAlign(0, 0, 0), aCohe;
			Vec3f pAv(0, 0, 0);
			Vec3f pAvAll(0, 0, 0);
			for (int j = 0; j < numFlockings; j++)
			{
				pAvAll += particles[j].p / numFlockings;
				if (j == i)
					continue;
				float dist = (particles[j].p - particles[i].p).length();
				Vec3f dir = (particles[j].p - particles[i].p) / dist;
				if (dist >= localThresh)
					continue;
				neighbourNum++;
				
				if (dist >= collisionThresh)
					aSep = Vec3f(0, 0, 0);
				else
				{
					aSep += -dir * kSep * (collisionThresh - dist);
				}
				vAlign += particles[j].v;
				pAv += particles[j].p;
				
			}
			Vec3f vAverage = vAlign / neighbourNum;
			Vec3f aAlign = (vAlign - particles[i].v) * kAlign;
			pAv /= neighbourNum;
			cout << neighbourNum << endl;
			// cohesion
			if (neighbourNum >= minNeighbour)
			{
				if ((particles[i].p - pAv).length() >= cohesionThresh)
				{
					float dist = (particles[i].p - pAv).length();
					aCohe = (pAv - particles[i].p) / dist * (dist - cohesionThresh) * kCohe;
				}
			}
			else
			{
				float dist = (pAvAll - particles[i].p).length();
				cout << "dist" << dist << endl;
				aCohe = (pAvAll - particles[i].p) * kCohe * 25;
			}
			particles[i].a = (aAlign + aCohe + aSep) / 3;
			if (particles[i].p.length() > farThresh)
				particles[i].a += kFar * (-particles[i].p) * (particles[i].p.length() - farThresh);
			if (particles[i].v.length() > fastVThresh)
				particles[i].v *= 0.9;
			particles[i].v += particles[i].a * dt;
			particles[i].p += dt * particles[i].v;
		}

	}
	else if (fireworks)
	{
		for (int i = 0; i < particles.size(); i++)
		{
			float ageBias = getBias()[0] / 1.5f;
			if (particles[i].age > 2)
			{
				Vec3f pBias = getBias();
				Vec3f vBias = getBias();
				Vec3f initialV = getBias() * fireworksV;
				Vec3f initialP(0, 0, 0);
				particles[i].age = 0;
				particles[i].p = initialP + pBias;
				particles[i].v = initialV + vBias;
				particles[i].a = { 0, 0, 0 };
			}
			else
			{
				Vec3f g{ 0, -14, 0 };
				// todo: recalculate another F
				//particles[i].a = particles[i].F / particles[i].m;
				Vec3f gBias = getBias() / 2.0f;
				particles[i].a = g + gBias;
				if (particles[i].v.length() > 3)
				{
					particles[i].v *= (1 - (particles[i].v.length() - 4) * 0.015);
				}
				particles[i].v += dt * particles[i].a;
				particles[i].p += dt * particles[i].v;
				particles[i].age += dt;
			}

		}
		/*if (simulate)
			baked_particles.push_back(particles);*/
	}
	else if (snow)
	{
		for (int i = 0; i < particles.size(); i++)
		{
			float ageBias = getBias()[0] / 1.5f;
			if (particles[i].p[1] < -4)
			{
				Vec3f pBias = getBias() * 8;
				Vec3f initialP(0, 16, 0);
				particles[i].age = 0;
				particles[i].p = initialP + pBias;
				particles[i].v = { 0, 0, 0 };
				particles[i].a = { 0, 0, 0 };
			}
			else
			{
				Vec3f g{ 0, -10, 0 };
				Vec3f gBias = getBias() * 7;
				gBias[1] = 0;
				particles[i].a = g + gBias;
				if (particles[i].v.length() > 1)
				{
					particles[i].v *= (1 - (particles[i].v.length() - 1) * 0.015);
				}
				particles[i].v += dt * particles[i].a;
				particles[i].p += dt * particles[i].v;
				particles[i].age += dt;
			}

		}
	}
	else if (fire)
	{
		for (int i = 0; i < particles.size(); i++)
		{
			float ageBias = getBias()[0] / 1.5f;
			if (particles[i].age > 1 + ageBias)
			{
				Vec3f pBias = getBias() * 0.3;
				pBias[1] = 0;
				Vec3f initialP(0, 0, 0);
				Vec3f initialV = Vec3f(0, 2, 0) + getBias();
				particles[i].age = 0;
				particles[i].p = initialP + pBias;
				particles[i].v = initialV;
				particles[i].a = { 0, 0, 0 };
			}
			else
			{
				Vec3f g{ 0, 3, 0 };
				Vec3f gBias = getBias() * 2;
				particles[i].a = g + gBias;
				if (particles[i].v.length() > 1)
				{
					particles[i].v *= (1 - (particles[i].v.length() - 1) * 0.015);
				}
				particles[i].v += dt * particles[i].a;
				particles[i].p += dt * particles[i].v;
				particles[i].age += dt;
			}

		}
	}
	else
	{
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
		// inter-collision detection
		for (int i = 0; i < particles.size(); i++)
		{
			for (int j = i; j < particles.size(); j++)
			{
				float dist = (particles[i].p - particles[j].p).length();
				if (dist < 2 * 0.01)
				{
					Vec3f tempV = particles[i].v;
					particles[i].v = particles[j].v;
					particles[j].v = tempV;
				}
			}
		}
		if (simulate)
			baked_particles.push_back(particles);
		// ---------------------------------------------------------------- //
		// cloth
		for (int r = 0; r < numParticlesHeight; r++)
		{
			for (int c = numParticlesWidth - 1; c >= 0; c--)
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
}

void drawPyramid(Vec3f p, Vec3f center, float length)
{
	Vec3f helper(0, 1, 0);
	Vec3f dir1 = ((p - center) ^ helper);
	dir1.normalize();
	Vec3f dir2 = (p - center) ^ dir1;
	dir2.normalize();
	Vec3f p1 = center + dir1 * length / 2;
	Vec3f p2 = center + dir2 * length / 2;
	Vec3f p3 = center - dir1 * length / 2;
	Vec3f p4 = center - dir2 * length / 2;
	setDiffuseColor(1, 1, 1);
	drawTriangle(p[0], p[1], p[2],
				 p1[0], p1[1], p1[2], 
				 p2[0], p2[1], p2[2]);
	drawTriangle(p[0], p[1], p[2],
		p2[0], p2[1], p2[2],
		p3[0], p3[1], p3[2]);
	drawTriangle(p[0], p[1], p[2],
		p3[0], p3[1], p3[2],
		p4[0], p4[1], p4[2]);
	drawTriangle(p[0], p[1], p[2],
		p1[0], p1[1], p1[2],
		p4[0], p4[1], p4[2]);
	// square
	drawTriangle(p1[0], p1[1], p1[2],
		p3[0], p3[1], p3[2],
		p2[0], p2[1], p2[2]);
	drawTriangle(p1[0], p1[1], p1[2],
		p3[0], p3[1], p3[2],
		p4[0], p4[1], p4[2]);

	setDiffuseColor(1, 0, 0);

	glPushMatrix();
	glTranslated(p[0], p[1], p[2]);
	drawSphere(length/10);
	glPopMatrix();
	glPushMatrix();
	glTranslated(p1[0], p1[1], p1[2]);
	drawSphere(length / 10);
	glPopMatrix();
	glPushMatrix();
	glTranslated(p2[0], p2[1], p2[2]);
	drawSphere(length / 10);
	glPopMatrix();
	glPushMatrix();
	glTranslated(p3[0], p3[1], p3[2]);
	drawSphere(length / 10);
	glPopMatrix();
	glPushMatrix();
	glTranslated(p4[0], p4[1], p4[2]);
	drawSphere(length / 10);
	glPopMatrix();

}
/** Render particles */
void ParticleSystem::drawParticles(float t)
{
	if (flocking)
	{
		setDiffuseColor(1, 1, 1);
		for (int i = 0; i < numFlockings; i++)
		{
			float size = collisionThresh / 3;
			Vec3f p = particles[i].p;
			Vec3f center;
			if(particles[i].v.length() == 0)
				center = p - Vec3f(1, 0, 0) * size;
			else
				center = p - (particles[i].v / particles[i].v.length()) * size;
			//cout << "p: " << p << "center: " << center << endl;
			drawPyramid(p, center, size);
		}
		//drawPyramid({ 1, 0, 0 }, { 0, 0, 0 }, 1);
	}
	else if (snow)
	{
		for (int i = 0; i < particles.size(); i++)
		{
			setDiffuseColor(0, 0, 0);
			glPushMatrix();
			glTranslated(particles[i].p[0],
				particles[i].p[1],
				particles[i].p[2]);
			glScaled(0.2, 0.2, 0.2);
			if (i == 0)
			{
				// test
				int w, h; // should be same
				unsigned char* im = readBMP("./samples/snow512.bmp", w, h);
				if (im == NULL)
				{
					fl_alert("Can't load bitmap file");
					return;
				}
				for (int i = 0; i < w * h * 3; i += 3)
				{
					if (im[i] < 10)
						im[i] = 0;
					if (im[i + 1] < 10)
						im[i + 1] = 0;
					if (im[i + 2] < 10)
						im[i + 2] = 0;
					//im[i] = (unsigned char)(im[i] * (1 - particles[0].age * 0.8));
				}
				initTexture(im, w, h);
			}

			drawBillboard(camera);
			glPopMatrix();

		}
		glDisable(GL_TEXTURE_2D);
	}
	else if (fireworks)
	{
		for (int i = 0; i < particles.size(); i++)
		{
			setDiffuseColor(0, 0, 0);
			glPushMatrix();
			glTranslated(particles[i].p[0],
				particles[i].p[1],
				particles[i].p[2]);
			glScaled(0.2, 0.2, 0.2);

			if (i == 0)
			{
				int w, h; // should be same
				unsigned char* im = readBMP("./samples/spark.bmp", w, h);
				if (im == NULL)
				{
					fl_alert("Can't load bitmap file");
					return;
				}
				for (int i = 0; i < w * h * 3; i += 3)
				{
					if (im[i] < 10)
						im[i] = 0;
					if (im[i + 1] < 10)
						im[i + 1] = 0;
					if (im[i + 2] < 10)
						im[i + 2] = 0;
					im[i] = (unsigned char)(im[i] * (1 - particles[0].age * 0.8));
					if (im[i] < 10)
						im[i] = 0;
					if (particles[0].age > 1.2)
						im[i] = 0;
					im[i + 1] = 0;
					im[i + 2] = 0;
				}
				initTexture(im, w, h);
			}

			drawBillboard(camera);
			glPopMatrix();

		}
		glDisable(GL_TEXTURE_2D);
	}
	else if (fire)
	{
		//int w = 128, h = 128; // should be same
		//unsigned char* im = new unsigned char[w * h * 3];
		//for (int i = 0; i < w * h * 3; i += 3)
		//{
		//	im[i] = 255;
		//	im[i + 1] = 0;
		//	im[i + 2] = 0;
		//}
		
		//initTexture(im, w, h, false);
		for (int i = 0; i < particles.size(); i++)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			setDiffuseColor(1, 0, 0, max(0, 0.5 - particles[i].age * 0.9));
			glPushMatrix();
			glTranslated(particles[i].p[0],
				particles[i].p[1],
				particles[i].p[2]);
			glScaled(0.025, 0.025, 0.025);
			drawBillboardFire(camera);
			glPopMatrix();
		}
	}
	else
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
				Particle pt2 = clothParticles[r + 1][c];
				Particle pt3 = clothParticles[r + 1][c + 1];
				Particle pt4 = clothParticles[r][c + 1];

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