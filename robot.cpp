// The sample model.  You should build a file
// very similar to this for when you make your model.
#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include <FL/gl.h>
#include <cmath>
#include "bitmap.h"
#include <gl/glu.h>
#include <FL/fl_ask.H>
#include "mat.h"
#include "vec.h"
#include "particleSystem.h"
#include "modelerui.h"
#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502
#endif

#ifndef VAL
#define VAL(x) (ModelerApplication::Instance()->GetControlValue(x))
#endif
enum SampleModelControls
{
	TEXTURE_MAPPING,
	LIGHT_POS_X, LIGHT_POS_Y, LIGHT_POS_Z, LIGHT1_POS_X, LIGHT1_POS_Y, LIGHT1_POS_Z,
	LIGHT_INTENSITY,
	XPOS, YPOS, ZPOS,



	HEAD_ROTATE, LEFT_FEET_ROTATE, RIGHT_FEET_ROTATE,

	LEFT_ARM_X_ROTATE, RIGHT_ARM_X_ROTATE,
	LEFT_ARM_Y_ROTATE, RIGHT_ARM_Y_ROTATE,
	LEFT_ARM_Z_ROTATE, RIGHT_ARM_Z_ROTATE,

	LEFT_LOWER_ARM_ROTATE, RIGHT_LOWER_ARM_ROTATE,

	ROCKET_GRAB,

	LEFT_HAND_ROTATE, RIGHT_HAND_ROTATE,



	LEFT_THUMB_UP, RIGHT_THUMB_UP,

	LEFT_FINGER1_FIRST_JOINT_ROTATE, LEFT_FINGER1_SECOND_JOINT_ROTATE,
	RIGHT_FINGER1_FIRST_JOINT_ROTATE, RIGHT_FINGER1_SECOND_JOINT_ROTATE,

	LEFT_FINGER2_FIRST_JOINT_ROTATE, LEFT_FINGER2_SECOND_JOINT_ROTATE,
	RIGHT_FINGER2_FIRST_JOINT_ROTATE, RIGHT_FINGER2_SECOND_JOINT_ROTATE,

	LEFT_FINGER3_FIRST_JOINT_ROTATE, LEFT_FINGER3_SECOND_JOINT_ROTATE,
	RIGHT_FINGER3_FIRST_JOINT_ROTATE, RIGHT_FINGER3_SECOND_JOINT_ROTATE,

	CHANGE_HAND, JETTING, LASING,

	PARTICLE_CLOTH_POS_X, PARTICLE_CLOTH_POS_Y, PARTICLE_CLOTH_POS_Z,

	FLOCKING, HEIGHT_FIELD,

	NUMCONTROLS
};


enum LeftOrRight
{
	LEFT, RIGHT
};

// To make a SampleModel, we inherit off of ModelerView
class RobotModel : public ModelerView
{
public:
	RobotModel(int x, int y, int w, int h, char* label)
		: ModelerView(x, y, w, h, label) { }

	virtual void draw();
private:
	bool init{ false };
};

// We need to make a creator function, mostly because of
// nasty API stuff that we'd rather stay away from.
ModelerView* createSampleModel(int x, int y, int w, int h, char* label)
{
	return new RobotModel(x, y, w, h, label);
}

void draw_feet_helper(float v[][3])
{
	// actually a Triangular prism
	glBegin(GL_TRIANGLES);
	glVertex3fv(v[1]);
	glVertex3fv(v[5]);
	glVertex3fv(v[3]);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex3fv(v[0]);
	glVertex3fv(v[4]);
	glVertex3fv(v[2]);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3fv(v[0]);
	glVertex3fv(v[1]);
	glVertex3fv(v[5]);
	glVertex3fv(v[4]);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3fv(v[0]);
	glVertex3fv(v[1]);
	glVertex3fv(v[3]);
	glVertex3fv(v[2]);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3fv(v[2]);
	glVertex3fv(v[3]);
	glVertex3fv(v[5]);
	glVertex3fv(v[4]);

	glEnd();

}
void draw_prism5_helper(float v[][3])
{
	// pentagonal prism
	// up surface
	glBegin(GL_TRIANGLES);
	glVertex3fv(v[0]);
	glVertex3fv(v[1]);
	glVertex3fv(v[4]);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3fv(v[1]);
	glVertex3fv(v[2]);
	glVertex3fv(v[3]);
	glVertex3fv(v[4]);
	glEnd();

	// lower surface
	glBegin(GL_TRIANGLES);
	glVertex3fv(v[5]);
	glVertex3fv(v[1 + 5]);
	glVertex3fv(v[4 + 5]);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3fv(v[1 + 5]);
	glVertex3fv(v[2 + 5]);
	glVertex3fv(v[3 + 5]);
	glVertex3fv(v[4 + 5]);
	glEnd();

	// draw side surfaces
	glBegin(GL_QUADS);
	glVertex3fv(v[0]);
	glVertex3fv(v[1]);
	glVertex3fv(v[6]);
	glVertex3fv(v[5]);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3fv(v[0 + 1]);
	glVertex3fv(v[1 + 1]);
	glVertex3fv(v[6 + 1]);
	glVertex3fv(v[5 + 1]);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3fv(v[0 + 2]);
	glVertex3fv(v[1 + 2]);
	glVertex3fv(v[6 + 2]);
	glVertex3fv(v[5 + 2]);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3fv(v[0 + 3]);
	glVertex3fv(v[1 + 3]);
	glVertex3fv(v[6 + 3]);
	glVertex3fv(v[5 + 3]);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3fv(v[4]);
	glVertex3fv(v[0]);
	glVertex3fv(v[5]);
	glVertex3fv(v[9]);
	glEnd();
}

void drawShouder()
{
	glPushMatrix();
	glScaled(3, 3, 0.5);

	//First part of arm
	float points[5][2] = {
		{0.0f, 0.0f},
		{0.5f, 0.0f},
		{0.8f, 0.7f},
		{0.3f, 0.7f},
		{-0.2f, 0.2f}
	};
	//Upper part
	glBegin(GL_POLYGON);
	for (int i = 0; i < 5; i++)
		glVertex3f(points[i][0], points[i][1], -2.5);
	glEnd();

	glBegin(GL_POLYGON);
	for (int i = 0; i < 5; i++)
		glVertex3f(points[i][0], points[i][1], 0);
	glEnd();

	for (int i = 0; i < 5; i++)
	{
		glBegin(GL_POLYGON);
		glVertex3f(points[i % 5][0], points[i % 5][1], -2.5);
		glVertex3f(points[(i + 1) % 5][0], points[(i + 1) % 5][1], -2.5);
		glVertex3f(points[(i + 1) % 5][0], points[(i + 1) % 5][1], 0);
		glVertex3f(points[i % 5][0], points[i % 5][1], 0);
		glEnd();
	}

	glPopMatrix();


}

void drawUpperArm()
{
	//Lower part
	drawBox(0.5, 2.5, 0.5);
}

void drawHexagonalPrisms()
{
	float s_60 = sin(M_PI / 3);

	float points[6][2] = { {2,0},
		{1, 2 * s_60},
		{-1, 2 * s_60},
		{-2, 0},
		{-1, -2 * s_60},
		{1, -2 * s_60} };

	glBegin(GL_POLYGON);
	for (int i = 0; i < 6; i++)
		glVertex3f(points[i][0], points[i][1], -2.5);
	glEnd();

	glBegin(GL_POLYGON);
	for (int i = 0; i < 6; i++)
		glVertex3f(points[i][0], points[i][1], 0);
	glEnd();

	for (int i = 0; i < 6; i++)
	{
		glBegin(GL_POLYGON);
		glVertex3f(points[i % 6][0], points[i % 6][1], -2.5);
		glVertex3f(points[(i + 1) % 6][0], points[(i + 1) % 6][1], -2.5);
		glVertex3f(points[(i + 1) % 6][0], points[(i + 1) % 6][1], 0);
		glVertex3f(points[i % 6][0], points[i % 6][1], 0);
		glEnd();
	}

}

void drawLowerArm() {
	glPushMatrix();
	glScaled(0.4, 0.4, 0.4);

	setDiffuseColor(0.8f, 0.8f, 0.8f);
	glPushMatrix();
	glRotated(90, 0.0, 1.0, 0.0);
	drawCylinder(1.2, 0.8, 0.8);
	glPopMatrix();

	float delta = 0.25;

	setDiffuseColor(1.0f, 1.0f, 0.0f);

	glPushMatrix();
	glTranslated(0.6, 0, 0.5 * 2.5 + delta);
	glRotated(30, 0.0, 0.0, 1.0);
	glScaled(0.7, 0.7, 0.5);
	drawHexagonalPrisms();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.6, 0, (0.5 + 0.7) * 2.5 + delta);
	glRotated(30, 0.0, 0.0, 1.0);
	glScaled(1, 1, 0.7);
	drawHexagonalPrisms();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.6, 0, (0.5 + 0.7 + 1) * 2.5 + delta);
	glRotated(30, 0.0, 0.0, 1.0);
	glScaled(1.3, 1.3, 1);
	drawHexagonalPrisms();
	glPopMatrix();

	glPopMatrix();

}

void drawHand()
{
	setDiffuseColor(0.8f, 0.8f, 0.8f);
	glPushMatrix();
	glRotated(90, 1.0, 0, 0);
	glTranslated(0.5, 0.3, -1);
	drawCylinder(1, 0.3, 0.3);
	glPopMatrix();

	setDiffuseColor(1.0f, 1.0f, 0.0f);
	glPushMatrix();
	glTranslated(-0.25, -0.5, 0.6);
	drawBox(1.5, 2, 1.7);
	glPopMatrix();
	//glPushMatrix();
	//glTranslated(0.15,1.4, 1.7);
	//drawBox(0.2, 0.5, 0.5);
	//glPopMatrix();

	//glPushMatrix();
	//glTranslated(0.15, 0.75, 1.7);
	//drawBox(0.2, 0.5, 0.5);
	//glPopMatrix();

	//glPushMatrix();
	//glTranslated(0.15, 0.1, 1.7);
	//drawBox(0.2, 0.5, 0.5);
	//glPopMatrix();




}

void drawFinger(LeftOrRight rl, int num) {

	int angle = 70 * VAL(ROCKET_GRAB) / 100;
	glPushMatrix();
	{
		glRotated(90, 1.0, 0.0, 0.0);
		setDiffuseColor(0.8f, 0.8f, 0.8f);
		drawCylinder(0.5, 0.2, 0.2);

		setDiffuseColor(1.0f, 1.0f, 0.0f);
		glPushMatrix();
		{
			glTranslated(0, -0.1, 0);
			if (rl == LEFT)
				glRotated(VAL(LEFT_FINGER1_FIRST_JOINT_ROTATE + (num - 1) * 4), 0, 0, 1);
			else {
				int angle1 = VAL(RIGHT_FINGER1_FIRST_JOINT_ROTATE + (num - 1) * 4) + angle;
				if (angle1 > 90)
					angle1 = 90;
				glRotated(angle1, 0, 0, 1);
			}


			drawBox(1.4, 0.4, 0.5);

			glPushMatrix();
			{
				glTranslated(1.4, 0, 0);

				if (rl == LEFT)
					glRotated(VAL(LEFT_FINGER1_SECOND_JOINT_ROTATE + (num - 1) * 4), 0, 0, 1);
				else {
					int angle2 = VAL(RIGHT_FINGER1_SECOND_JOINT_ROTATE + (num - 1) * 4) + angle;
					if (angle2 > 90)
						angle2 = 90;
					glRotated(angle2, 0, 0, 1);
				}

				setDiffuseColor(0.8f, 0.8f, 0.8f);
				drawCylinder(0.5, 0.2, 0.2);

				setDiffuseColor(1.0f, 1.0f, 0.0f);
				glTranslated(0, -1, 0);
				drawBox(0.4, 1.0, 0.5);
			}
			glPopMatrix();
		}
		glPopMatrix();
	}
	glPopMatrix();

}

void drawThumb(LeftOrRight rl)
{
	int angle;
	if (rl == LEFT)
		angle = VAL(LEFT_THUMB_UP);
	else {
		angle = VAL(RIGHT_THUMB_UP) + 60 * VAL(ROCKET_GRAB) / 100;
		if (angle > 70)
			angle = 70;
	}


	glPushMatrix();
	{
		glRotated(-90, 0.0, 1.0, 0.0);

		setDiffuseColor(0.8f, 0.8f, 0.8f);
		drawCylinder(1, 0.2, 0.2);

		setDiffuseColor(1.0f, 1.0f, 0.0f);

		glPushMatrix();
		{
			glRotated(20 + angle, 0.0, 0.0, 1.0);
			glTranslated(0, -0.1, 0);
			drawBox(0.8, 0.4, 1);

			setDiffuseColor(0.8f, 0.8f, 0.8f);
			glPushMatrix();
			{
				glTranslated(0.8, 0.1, 0);
				drawCylinder(1, 0.2, 0.2);

				setDiffuseColor(1.0f, 1.0f, 0.0f);
				glPushMatrix();
				glRotated(-20 + 20 * angle / 70, 0.0, 0.0, 1.0);
				glTranslated(0, -0.1, 0);
				drawBox(1.0, 0.4, 1);
				glPopMatrix();
			}
			glPopMatrix();
		}
		glPopMatrix();
	}
	glPopMatrix();
}


void gripperHelper()
{
	setDiffuseColor(0.5f, 0.5f, 0);

	float points[8][2] = {
	{0,0},
	{0,-0.6},
	{-1 * sin(M_PI / 4),-0.6 - 1 * sin(M_PI / 4)},
	{-1 * sin(M_PI / 4),-0.6 - 1 * sin(M_PI / 4) - 1},
	{0,-0.6 - 2 * sin(M_PI / 4) - 1},
	{0,-1.2 - 2 * sin(M_PI / 4) - 1},
	{-1 * sin(M_PI / 4) - 0.6 * sin(M_PI / 4),-0.6 - 1 * sin(M_PI / 4) - 1},
	{-1 * sin(M_PI / 4) - 0.6 * sin(M_PI / 4),-0.6 - 1 * sin(M_PI / 4)}
	};

	float f = -0.6;
	glBegin(GL_POLYGON);
	glVertex3f(points[0][0], points[0][1], f);
	glVertex3f(points[1][0], points[1][1], f);
	glVertex3f(points[2][0], points[2][1], f);
	glVertex3f(points[7][0], points[7][1], f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(points[2][0], points[2][1], f);
	glVertex3f(points[3][0], points[3][1], f);
	glVertex3f(points[6][0], points[6][1], f);
	glVertex3f(points[7][0], points[7][1], f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(points[3][0], points[3][1], f);
	glVertex3f(points[4][0], points[4][1], f);
	glVertex3f(points[5][0], points[5][1], f);
	glVertex3f(points[6][0], points[6][1], f);
	glEnd();


	f = 0;
	glBegin(GL_POLYGON);
	glVertex3f(points[0][0], points[0][1], f);
	glVertex3f(points[1][0], points[1][1], f);
	glVertex3f(points[2][0], points[2][1], f);
	glVertex3f(points[7][0], points[7][1], f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(points[2][0], points[2][1], f);
	glVertex3f(points[3][0], points[3][1], f);
	glVertex3f(points[6][0], points[6][1], f);
	glVertex3f(points[7][0], points[7][1], f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(points[3][0], points[3][1], f);
	glVertex3f(points[4][0], points[4][1], f);
	glVertex3f(points[5][0], points[5][1], f);
	glVertex3f(points[6][0], points[6][1], f);
	glEnd();

	for (int i = 0; i < 8; i++)
	{
		glBegin(GL_POLYGON);
		glVertex3f(points[i % 8][0], points[i % 8][1], -0.6);
		glVertex3f(points[(i + 1) % 8][0], points[(i + 1) % 8][1], -0.6);
		glVertex3f(points[(i + 1) % 8][0], points[(i + 1) % 8][1], 0);
		glVertex3f(points[i % 8][0], points[i % 8][1], 0);
		glEnd();
	}
}
void drawGripper()
{
	glPushMatrix();
	{
		glRotated(-90, 1, 0, 0);

		setDiffuseColor(0.5f, 0.5f, 0);
		drawBox(1, 0.6, 0.6);

		setDiffuseColor(0.8f, 0.8f, 0.8f);
		glPushMatrix();
		{
			glTranslated(-0.3, 0.3, 0);
			drawCylinder(0.6, 0.3, 0.3);
			glTranslated(0, 0.3, 0.6);
			gripperHelper();
		}
		glPopMatrix();

		setDiffuseColor(0.8f, 0.8f, 0.8f);
		glPushMatrix();
		{
			glScaled(-1, 1, 1);
			glTranslated(-1.3, 0.3, 0);
			drawCylinder(0.6, 0.3, 0.3);
			glTranslated(0, 0.3, 0.6);
			gripperHelper();
		}
		glPopMatrix();
	}

	glPopMatrix();
}

void drawCircleRingOnBody(float r, float R);

void drawLeftArm()
{
	glPushMatrix();
	{
		glScaled(-1, 1, 1);
		setDiffuseColor(1, 1, 0);
		drawShouder();
		// rings
		/*glPushMatrix();
		{
			glRotated(-45, 0, 0, 1);
			glTranslated(-0.5, 2, -0.5);
			drawCircleRingOnBody(0.25, 0.8);
		}
		glPopMatrix();
		glPushMatrix();
		{
			glRotated(-45, 0, 0, 1);
			glTranslated(-1, 0.5, -0.5);
			drawCircleRingOnBody(0.25, 0.8);
		}
		glPopMatrix();*/
		glPushMatrix();
		{
			glTranslated(0.5, -2, -0.75);

			glRotated(-VAL(LEFT_ARM_Y_ROTATE), 0, 1.0, 0);

			glTranslated(0, 2, 0);
			glRotated(-VAL(LEFT_ARM_Z_ROTATE), 0, 0, 1.0);
			glTranslated(0, -2, 0);

			drawUpperArm();

			glPushMatrix();
			{
				glTranslated(0, 0, 0.5);
				glRotated(VAL(LEFT_LOWER_ARM_ROTATE), 1, 0, 0);

				drawLowerArm();

				glPushMatrix();
				{
					glTranslated(-0.2, -0.5, (0.5 + 0.7 + 1) * 2.5 * 0.4);

					glTranslated(0.5, 0.5, 0);
					glRotated(VAL(LEFT_HAND_ROTATE), 0, 0, 1.0);
					glTranslated(-0.5, -0.5, 0);


					if (VAL(CHANGE_HAND) == 0)
					{
						drawHand();

						glPushMatrix();
						{
							glTranslated(-0.25, -0.5, 0.6);
							glPushMatrix();
							{
								glTranslated(0.15, 1.4, 1.7);
								glTranslated(0.2, 0.5, 0.2);
								drawFinger(LEFT, 1);
							}
							glPopMatrix();

							glPushMatrix();
							{
								glTranslated(0.15, 0.75, 1.7);
								glTranslated(0.2, 0.5, 0.2);
								drawFinger(LEFT, 2);
							}
							glPopMatrix();

							glPushMatrix();
							{
								glTranslated(0.15, 0.1, 1.7);
								glTranslated(0.2, 0.5, 0.2);
								drawFinger(LEFT, 3);
							}
							glPopMatrix();

							glPushMatrix();
							{
								glTranslated(0.15, 0.1, 0.5);
								glTranslated(1.2, 1.9, 0);
								drawThumb(LEFT);
							}
							glPopMatrix();
						}
						glPopMatrix();
					}
					else
					{
						glPushMatrix();
						{
							glTranslated(0, 0.2, 0.6);
							drawGripper();
						}
						glPopMatrix();
					}
				}
				glPopMatrix();

			}
			glPopMatrix();
		}
		glPopMatrix();
	}
	glPopMatrix();
}





void drawRightArm()
{
	setDiffuseColor(1, 1, 0);
	drawShouder();
	// rings
	/*glPushMatrix();
	{
		glRotated(-45, 0, 0, 1);
		glTranslated(-0.5, 2, -0.5);
		drawCircleRingOnBody(0.25, 0.8);
	}
	glPopMatrix();
	glPushMatrix();
	{
		glRotated(-45, 0, 0, 1);
		glTranslated(-1, 0.5, -0.5);
		drawCircleRingOnBody(0.25, 0.8);
	}
	glPopMatrix();*/
	glPushMatrix();
	{
		glTranslated(0.5, -2, -0.75);

		glRotated(-VAL(RIGHT_ARM_Y_ROTATE), 0, 1.0, 0);

		glTranslated(0, 2, 0);
		glRotated(-VAL(RIGHT_ARM_Z_ROTATE), 0, 0, 1.0);
		glTranslated(0, -2, 0);

		drawUpperArm();

		glPushMatrix();
		{
			glTranslated(0, 0, 0.5);
			glRotated(VAL(RIGHT_LOWER_ARM_ROTATE), 1, 0, 0);

			drawLowerArm();

			glPushMatrix();
			{
				glTranslated(-0.2, -0.5, (0.5 + 0.7 + 1) * 2.5 * 0.4);

				float ratio = VAL(ROCKET_GRAB) / 100.0;

				glTranslated(0.25, 0.25, 0);
				setDiffuseColor(0.8f, 0.8f, 0.8f);
				drawBox(0.5, 0.5, 10 * ratio);
				glTranslated(-0.25, -0.25, 0);

				glTranslated(0, 0, 10 * ratio);

				glTranslated(0.5, 0.5, 0);
				int angle1 = VAL(RIGHT_HAND_ROTATE) + ratio * 90;
				if (angle1 > 90)
					angle1 = 90;
				glRotated(-angle1, 0, 0, 1.0);

				glTranslated(-0.5, -0.5, 0);


				if (VAL(CHANGE_HAND) == 0)
				{
					glRotated(-40 * ratio, 0, 1.0, 0);

					drawHand();

					glPushMatrix();
					{
						glTranslated(-0.25, -0.5, 0.6);
						glPushMatrix();
						{
							glTranslated(0.15, 1.4, 1.7);
							glTranslated(0.2, 0.5, 0.2);
							drawFinger(RIGHT, 1);
						}
						glPopMatrix();

						glPushMatrix();
						{
							glTranslated(0.15, 0.75, 1.7);
							glTranslated(0.2, 0.5, 0.2);
							drawFinger(RIGHT, 2);
						}
						glPopMatrix();

						glPushMatrix();
						{
							glTranslated(0.15, 0.1, 1.7);
							glTranslated(0.2, 0.5, 0.2);
							drawFinger(RIGHT, 3);
						}
						glPopMatrix();

						glPushMatrix();
						{
							glTranslated(0.15, 0.1, 0.5);
							glTranslated(1.2, 1.9, 0);
							drawThumb(RIGHT);
						}
						glPopMatrix();
					}
					glPopMatrix();
				}
				else
				{
					glPushMatrix();
					{
						glTranslated(0, 0.2, 0.6);
						drawGripper();
					}
					glPopMatrix();
				}

			}
			glPopMatrix();

		}
		glPopMatrix();
	}
	glPopMatrix();
}

void drawJetting()
{
	srand(0);
	setDiffuseColor(1, 1, 1);
	glPushMatrix();
	glScaled(0.5, 0.5, 0.5);
	glRotated(90, 0, 1, 0);
	glRotated(-90, 0, 0, 1);
	for (int i = 0; i < 8; i++)
	{
		glPushMatrix();
		{
			glTranslated(0, i / 2, 0);
			for (int a = 0; a < i + 1; a++)
			{
				for (int b = 0; b < i; b++)
				{
					glPushMatrix();
					{
						glTranslated(i / 2 * b / i * cos(M_PI * a / i), -(rand() % 6 - 3) / 2, i / 2 * b / i * sin(M_PI * a / i));
						glTranslated((rand() % 6 - 3) / 2, (rand() % 6 - 3) / 2, (rand() % 3 - 3) / 2);
						drawSphere((rand() % 5 + 1) / 3);
					}
					glPopMatrix();
				}
			}
		}
		glPopMatrix();
	}
	glPopMatrix();

	glPushMatrix();
	glScaled(-0.5, 0.5, 0.5);
	glRotated(90, 0, 1, 0);
	glRotated(-90, 0, 0, 1);
	for (int i = 0; i < 8; i++)
	{
		glPushMatrix();
		{
			glTranslated(0, i / 2, 0);
			for (int a = 0; a < i + 1; a++)
			{
				for (int b = 0; b < i; b++)
				{
					glPushMatrix();
					{
						glTranslated(i / 2 * b / i * cos(M_PI * a / i), -(rand() % 6 - 3) / 2, i / 2 * b / i * sin(M_PI * a / i));
						glTranslated((rand() % 6 - 3) / 2, (rand() % 6 - 3) / 2, (rand() % 3 - 3) / 2);
						drawSphere((rand() % 5 + 1) / 3);
					}
					glPopMatrix();
				}
			}
		}
		glPopMatrix();
	}
	glPopMatrix();


}



// given a curve, draw the shape by rotating the curve 360'
void draw_curve_shape(float h, float(*curve)(float))
{
	const int h_sample = 300;
	float v[360][h_sample][3];

	for (int i = 0; i < h_sample; i++)
	{
		v[0][i][0] = 0;
		v[0][i][1] = h * i / (h_sample - 1);
		v[0][i][2] = curve(h * i / h_sample);
	}
	for (int theta = 1; theta < 360; theta++)
	{
		for (int i = 0; i < h_sample; i++)
		{
			v[theta][i][0] = v[0][i][2] * sinf(theta / 360.0 * 2 * M_PI);
			v[theta][i][1] = h * i / h_sample;
			v[theta][i][2] = v[0][i][2] * cosf(theta / 360.0 * 2 * M_PI);
		}
	}

	for (int t = 0; t < 359; t += 1)
	{
		for (int i = 0; i < h_sample - 1; i++)
		{
			glBegin(GL_TRIANGLES);
			glVertex3fv(v[t][i]);
			glVertex3fv(v[t + 1][i]);
			glVertex3fv(v[t][i + 1]);
			glEnd();
			glBegin(GL_TRIANGLES);
			glVertex3fv(v[t + 1][i + 1]);
			glVertex3fv(v[t][i + 1]);
			glVertex3fv(v[t + 1][i]);
			glEnd();

		}
	}
	// the 359 to 0
	for (int i = 0; i < h_sample - 1; i++)
	{
		glBegin(GL_TRIANGLES);
		glVertex3fv(v[359][i]);
		glVertex3fv(v[0][i]);
		glVertex3fv(v[359][i + 1]);
		glEnd();
		glBegin(GL_TRIANGLES);
		glVertex3fv(v[0][i + 1]);
		glVertex3fv(v[359][i + 1]);
		glVertex3fv(v[0][i]);
		glEnd();
	}
}
// draw curve 1 along curve 2
// curve 1 will be assigned to xy plane, curve 2 will be assigned to zy plane
void draw_extruded_surface(float(*curve1)(float), float(*curve2)(float),
	float range1 = 5, float range2 = 5)
{
	float sampleX = range1 * 50, sampleZ = range2 * 50;
	float*** v = new float** [sampleX];
	for (int i = 0; i < sampleX; i++)
	{
		v[i] = new float* [sampleZ];
		for (int j = 0; j < sampleZ; j++)
			v[i][j] = new float[3];
	}
	for (int i = 0; i < sampleX; i++)
	{
		for (int j = 0; j < sampleZ; j++)
		{
			v[i][j][0] = range1 * i / sampleX;
			v[i][j][2] = range2 * j / sampleZ;
			v[i][j][1] = curve1(v[i][j][0]) + curve2(v[i][j][2]);
		}
	}
	for (int t = 0; t < sampleX - 1; t += 1)
	{
		for (int i = 0; i < sampleZ - 1; i++)
		{
			glBegin(GL_QUADS);
			glVertex3fv(v[t][i]);
			glVertex3fv(v[t + 1][i]);
			glVertex3fv(v[t + 1][i + 1]);
			glVertex3fv(v[t][i + 1]);
			glEnd();
		}
	}
	for (int i = 0; i < sampleX; i++)
	{
		for (int j = 0; j < sampleZ; j++)
			delete[] v[i][j];
		delete[] v[i];
	}
	delete[] v;
}
// need push/pop matrix outside

void drawCircleRing();
void drawCircleRingOnBody(float r, float R);

void drawBodyOut(float h)
{
	//glTranslated(10, -3, 0);
	setDiffuseColor(140 / 255.0, 243 / 255.0, 252 / 255.0);
	glPushMatrix();
	glTranslated(-2, h / 2, 0);
	glPushMatrix();
	glRotated(-75, 0, 0, 1); // show texture
	glPopMatrix();
	drawSphere(h / 4.5);
	if (VAL(LASING) == 1)
	{
		glPushMatrix();
		{
			const float inf = 10;
			glRotated(-90, 0, 1, 0);
			drawCylinder(4 * inf + 2, 1.5, 1.5);
			glRotated(90, 0, 1, 0);

			glRotated(90, 0, 0, 1);

			glPushMatrix();
			for (int i = 0; i < 6; i++)
			{
				glTranslated(0, 0.7 * i + 3, 0);
				drawCircleRing();
			}
			glPopMatrix();
		}
		glPopMatrix();
	}
	glPopMatrix();

	const int h_sample = 100;
	float v[360][h_sample][3];
	// float h = 6;
	for (int i = 0; i < h_sample; i++)
	{
		v[0][i][0] = 0;
		v[0][i][1] = h * i / (h_sample - 1);
		v[0][i][2] = 3.5 - (h * i / h_sample - 3) * (h * i / h_sample - 3) * 2.0 / 9;
	}
	for (int theta = 1; theta < 360; theta++)
	{
		for (int i = 0; i < h_sample; i++)
		{
			v[theta][i][0] = v[0][i][2] * sinf(theta / 360.0 * 2 * M_PI);
			v[theta][i][1] = h * i / h_sample;
			v[theta][i][2] = v[0][i][2] * cosf(theta / 360.0 * 2 * M_PI);
		}
	}
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	for (int t = 0; t < 359; t += 1)
	{
		for (int i = 0; i < h_sample - 1; i++)
		{
			if ((t - 270) * (t - 270) / 625.0 + (i - h_sample / 2.0) * (i - h_sample / 2.0) / 625 < 1)
				continue;

			// draw outside
			if (i % 25 == 0)
				setDiffuseColor(0.1f, 0.1f, 0.1f);
			else
				setDiffuseColor(0.5f, 0.5f, 0);

			glBegin(GL_TRIANGLES);
			glVertex3fv(v[t][i]);
			glVertex3fv(v[t + 1][i]);
			glVertex3fv(v[t][i + 1]);
			glEnd();
			glBegin(GL_TRIANGLES);
			glVertex3fv(v[t + 1][i + 1]);
			glVertex3fv(v[t][i + 1]);
			glVertex3fv(v[t + 1][i]);
			glEnd();

		}
	}
	for (int t = 1; t < 359; t++)
	{
		for (int i = 0; i < h_sample - 1; i++)
		{
			// inside 
			if (t % 2 == 0)
				setDiffuseColor(0, 0, 0);
			else
				setDiffuseColor(0.2, 0.2, 0.2);

			glBegin(GL_TRIANGLES);
			glVertex3fv(v[t][i]);
			glVertex3fv(v[t][i + 1]);
			glVertex3fv(v[t + 1][i]);
			glEnd();
			glBegin(GL_TRIANGLES);
			glVertex3fv(v[t + 1][i + 1]);
			glVertex3fv(v[t + 1][i]);
			glVertex3fv(v[t][i + 1]);
			glEnd();
		}
	}

	// the 359 to 0
	for (int i = 0; i < h_sample - 1; i++)
	{
		setDiffuseColor(0.5f, 0.5f, 0);
		glBegin(GL_TRIANGLES);
		glVertex3fv(v[359][i]);
		glVertex3fv(v[0][i]);
		glVertex3fv(v[359][i + 1]);
		glEnd();
		glBegin(GL_TRIANGLES);
		glVertex3fv(v[0][i + 1]);
		glVertex3fv(v[359][i + 1]);
		glVertex3fv(v[0][i]);
		glEnd();

	}
	glDisable(GL_CULL_FACE);
	glPushMatrix();
	{
		setDiffuseColor(0.2f, 0.2f, 0.2f);
		glRotated(-30, 0, 1, 0);
		glRotated(-90, 1, 0, 0);
		glTranslated(-2.5, 0.5, 5);
		//drawCircleRingOnBody(0.3, 1.2);
	}
	glPopMatrix();
	glPushMatrix();
	{
		setDiffuseColor(0.2f, 0.2f, 0.2f);
		glRotated(30, 0, 1, 0);
		glRotated(-90, 1, 0, 0);
		glTranslated(-2.5, -1, 5);
		//drawCircleRingOnBody(0.3, 1.2);
	}
	glPopMatrix();
	setDiffuseColor(0.5f, 0.5f, 0);
}


void drawCircleRing()
{
	setDiffuseColor(140 / 255.0, 243 / 255.0, 252 / 255.0);
	float r = 0.25f;
	float R = 2.0f;
	float delta = M_PI / 180;
	for (float angle = 0; angle < 2 * M_PI; angle += delta)
	{
		float s = sin(angle); float c = cos(angle);
		glPushMatrix();
		{
			glTranslated(c * R, 0, s * R);
			glRotated(angle, 0, 1, 0);
			drawCylinder(delta * R, r, r);
		}
		glPopMatrix();
	}
}
void drawCircleRingOnBody(float r, float R)
{
	float delta = M_PI / 180 / 2;
	for (float angle = 0; angle < 2 * M_PI; angle += delta)
	{
		int d = angle / M_PI * 180;
		if (d % 10 == 0)
			setDiffuseColor(0, 0, 0);
		else
			setDiffuseColor(0.1, 0.1, 0.1);
		float s = sin(angle); float c = cos(angle);
		glPushMatrix();
		{
			glTranslated(c * R, 0, s * R);
			glRotated(angle, 0, 1, 0);
			drawCylinder(delta * R, r, r);
		}
		glPopMatrix();
	}
	setDiffuseColor(0.5f, 0.5f, 0);
}
void draw_shoulder_up_helper()
{
	float currentColor[4];
	glGetFloatv(GL_DIFFUSE, currentColor);
	glScaled(0.9, 1, 1);
	for (int i = 0; i < 40; i++)
	{
		glTranslated(0, 0.01, 0);
		draw_extruded_surface([](float x) {return (2.25f - (x - 1.5f) * (x - 1.5f)) / 2.25f; },
			[](float z) {return 0.0f; }, 3, 1);
	}

}

float curve1(float x) { return (x - 2) * (x - 2); }
float curve2(float x) { return 0; }
// We are going to override (is that the right word?) the draw()
// method of ModelerView to draw out SampleModel
void RobotModel::draw()
{
	if (VAL(TEXTURE_MAPPING) == 1)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);

	ParticleSystem* ps = ModelerApplication::Instance()->GetParticleSystem();
	ps->flocking = VAL(FLOCKING);
	// This call takes care of a lot of the nasty projection 
	// matrix stuff.  Unless you want to fudge directly with the 
	// projection matrix, don't bother with this ...
	ModelerView::draw();
	// save camera
	Mat4f cameraM = getModelViewMatrix();

	// set light source
	GLfloat light_pos[] = { VAL(LIGHT_POS_X), VAL(LIGHT_POS_Y), VAL(LIGHT_POS_Z), 0 };
	GLfloat light1_pos[] = { VAL(LIGHT1_POS_X), VAL(LIGHT1_POS_Y), VAL(LIGHT1_POS_Z), 0 };
	float intensity = VAL(LIGHT_INTENSITY);
	GLfloat light_intensity[] = { intensity, intensity, intensity, 1 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_intensity);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_intensity);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_intensity);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_intensity);

	if (VAL(FLOCKING))
	{

	}
	else
	{


		// draw the floor
		setAmbientColor(.1f, .1f, .1f);
		setDiffuseColor(.1f, 0, 0);
		glPushMatrix();
		glTranslated(-25, -8, -25);

		drawTextureBox(50, 0.01f, 50);

		//drawBox(50, 0.01f, 50);

		glPopMatrix();



		// draw the model
		setDiffuseColor(0.5f, 0.5f, 0);

		//drawSphere(3);
		//glPopMatrix();
		// parameters
		float r1 = 3; // r of the body
		float h_head = 1.8;
		float r_top = 1.5, r_bottom = 2;
		float h_middle = 6;
		float h_bottom = 1.5;
		float h_top = 1;
		float h_leg = 1.8;
		float h_feet = 0.8;
		float body_width_scale = 1;
		float body_depth_scale = 0.8;


		glTranslated(VAL(XPOS), h_bottom + h_feet + h_leg + VAL(YPOS), VAL(ZPOS));
		glRotated(90, 0, 1, 0);
		glTranslated(0, -3, 0);
		glScaled(0.6, 0.6, 0.6);

		glPushMatrix();
		{
			// middle body


			glScaled(body_depth_scale, 1, body_width_scale);

			glTranslated(0, -h_middle, 0);
			setDiffuseColor(0.5, 0.5, 0);
			drawBodyOut(h_middle);
			glTranslated(0, h_middle, 0);
			glScaled(1 / body_depth_scale, 1, 1 / body_width_scale);


			glPushMatrix();
			{
				// up shoulder right
				glRotated(20, 1, 0, 0);
				glTranslated(-1.3, 0, 1.5);
				setDiffuseColor(0.2, 0.2, 0.2);
				//draw_shoulder_up_helper();
			}
			glPopMatrix();

			glPushMatrix();
			{
				// up shoulder left
				glRotated(-20, 1, 0, 0);
				glTranslated(-1.3, 0, -2);
				setDiffuseColor(0.2, 0.2, 0.2);
				//draw_shoulder_up_helper();
			}
			glPopMatrix();

			float delta1 = 1.5;
			glPushMatrix();
			{
				glRotated(-90, 0, 1.0, 0);
				glTranslated(-4 - delta1, -1, 0);
				glPushMatrix();
				{
					setDiffuseColor(1, 1, 0);
					glTranslated(1.5, 0, -1);
					drawBox(2, 1, 1);
				}
				glPopMatrix();
				glRotated(VAL(RIGHT_ARM_X_ROTATE), 1.0, 0, 0);
				drawRightArm();
			}
			glPopMatrix();

			glPushMatrix();
			{
				glRotated(-90, 0, 1.0, 0);

				glTranslated(4 + delta1, -1, 0);
				glPushMatrix();
				{
					setDiffuseColor(1, 1, 0);
					glTranslated(-3.5, 0, -1);
					drawBox(2, 1, 1);
				}
				glPopMatrix();

				glRotated(VAL(LEFT_ARM_X_ROTATE), 1.0, 0, 0);
				drawLeftArm();
			}
			glPopMatrix();

			glPushMatrix();
			{
				// head
				float diff = 0.12;
				float w_head = 2.2;
				float h1 = 0.2, h2 = 1.3, h3 = h_head - h1 - h2;
				float w1 = 0.1, w2 = w_head / 2 - w1;
				float h_depth = 1.3;

				glPushMatrix();  // whole head
				{
					glRotated(VAL(HEAD_ROTATE), 0, 1, 0);

					glPushMatrix();
					{
						setDiffuseColor(1, 1, 0);
						// left head
						float v[10][3] =
						{
							{0,       0,       0},
							{0,       h_head,  0},
							{0,       h2 + h3,   -w_head / 2},
							{0,       h3,      -w_head / 2},
							{0,       0,       -w2},
							{h_depth, 0,       0},
							{h_depth, h_head,  0},
							{h_depth, h2 + h3, -w_head / 2},
							{h_depth, h3,      -w_head / 2},
							{h_depth, 0,       -w2}
						};
						glTranslated(-h_depth / 2, 0, -diff / 2);
						draw_prism5_helper(v);

						glPushMatrix();
						{
							// eye
							setAmbientColor(.1f, .1f, .1f);
							setDiffuseColor(140 / 255.0, 243 / 255.0, 252 / 255.0);

							glTranslated(0, h_head / 2, -w_head / 4);
							drawSphere(0.2);

							setAmbientColor(.1f, .1f, .1f);
							setDiffuseColor(0.5f, 0.5f, 0);

						}
						glPopMatrix();

					}
					glPopMatrix();
					glPushMatrix();
					{
						// right head
						float v[10][3] =
						{
							{0,       0,       0},
							{0,       h_head,  0},
							{0,       h2 + h3, w_head / 2},
							{0,       h3,      w_head / 2},
							{0,       0,       w2},
							{h_depth, 0,       0},
							{h_depth, h_head,  0},
							{h_depth, h2 + h3, w_head / 2},
							{h_depth, h3,      w_head / 2},
							{h_depth, 0,       w2}
						};
						glTranslated(-h_depth / 2, 0, diff / 2);
						draw_prism5_helper(v);

						glPushMatrix();
						{
							// eye
							setAmbientColor(.1f, .1f, .1f);
							setDiffuseColor(140 / 255.0, 243 / 255.0, 252 / 255.0);


							glTranslated(0, h_head / 2, w_head / 4);
							drawSphere(0.28);

							setAmbientColor(.1f, .1f, .1f);
							setDiffuseColor(0.5f, 0.5f, 0);

						}
						glPopMatrix();

					}
					glPopMatrix();
				}
				glPopMatrix();
			}
			glPopMatrix();
			glPushMatrix();
			{
				//// bottom body

				float leg_width = 0.9;
				glPushMatrix();
				{
					// left leg
					glTranslated(-0.6, -h_middle - h_leg, -leg_width / 2 + 1.1);
					glRotated(20, 0, 1, 0);
					drawTextureBox(leg_width, h_leg, leg_width);
					glPushMatrix();
					{
						// feet
						glRotated(VAL(LEFT_FEET_ROTATE), 0, 0, 1);
						float v[6][3] =
						{
							{0 + leg_width, 0, 0},
							{0 + leg_width, 0, leg_width},
							{-2, -h_feet, 0 - 0.5},
							{-2, -h_feet, leg_width + 0.5},
							{0 + leg_width, -h_feet, 0},
							{0 + leg_width, -h_feet, leg_width},
						};

						draw_feet_helper(v);
					}
					glPopMatrix();
				}
				glPopMatrix();
				glPushMatrix();
				{
					// right leg
					glTranslated(-0.6, -h_middle - h_leg, -leg_width / 2 - 1.1);
					glRotated(-20, 0, 1, 0);
					drawTextureBox(leg_width, h_leg, leg_width);

					// feet
					glRotated(VAL(RIGHT_FEET_ROTATE), 0, 0, 1);
					float v[6][3] =
					{
						{0 + leg_width, 0, 0},
						{0 + leg_width, 0, leg_width},
						{-2, -h_feet, 0 - 0.5},
						{-2, -h_feet, leg_width + 0.5},
						{0 + leg_width, -h_feet, 0},
						{0 + leg_width, -h_feet, leg_width},
					};

					draw_feet_helper(v);
				}
				glPopMatrix();
			}
			glPopMatrix();

			glPushMatrix();
			int h = r1 + 2, h2 = 2.5;
			{
				// left jet
				glTranslated(r1 * body_depth_scale, r1 / 2 + 2, -2);
				glRotated(90, 1, 0, 0);
				drawCylinder(h, 0.35, 0.35);

				if (VAL(JETTING) == 1)
					drawJetting();
				setDiffuseColor(0.5, 0.5, 0);
				glPushMatrix();
				{
					// jet header
					drawCylinder(h2, 0.15, 0.55);
					// set particle spawn position
					Mat4f worldM = cameraM.inverse() * getModelViewMatrix();
					Vec4f p4 = worldM * Vec4f(0, 0, 0, 1);
					p4 = p4 / p4[3];
					ParticleSystem* ps = ModelerApplication::Instance()->GetParticleSystem();
					ps->setEmitPos({ p4[0], p4[1], p4[2] });
					ps->topLeft = Vec3f(VAL(PARTICLE_CLOTH_POS_X),
						VAL(PARTICLE_CLOTH_POS_Y), VAL(PARTICLE_CLOTH_POS_Z));
				}
				glPopMatrix();
			}
			glPopMatrix();

			glPushMatrix();
			{
				// right jet
				glTranslated(r1 * body_depth_scale, r1 / 2 + 2, 2);
				glRotated(90, 1, 0, 0);
				drawCylinder(h, 0.35, 0.35);

				if (VAL(JETTING) == 1)
					drawJetting();


				setDiffuseColor(0.5, 0.5, 0);
				glPushMatrix();
				{
					// jet header
					drawCylinder(h2, 0.15, 0.5);
					// set particle spawn position
					Mat4f worldM = cameraM.inverse() * getModelViewMatrix();
					Vec4f p4 = worldM * Vec4f(0, 0, 0, 1);
					p4 = p4 / p4[3];
					ParticleSystem* ps = ModelerApplication::Instance()->GetParticleSystem();
					ps->setEmitPos2({ p4[0], p4[1], p4[2] });
				}
				glPopMatrix();
			}
			glPopMatrix();
		}
		glPopMatrix();
	}
	if (VAL(HEIGHT_FIELD))
	{
		ModelerUI* ui = ModelerApplication::Instance()->getUI();
		unsigned char* field = ui->m_nHeight_field;
		if (field)
		{
			glPushMatrix();
			glTranslated(-4, -12, -2);
			glScaled(6, 2, 6);
			drawHeightField(field, ui->m_nHeight_field_width, ui->m_nHeight_field_height);
			glPopMatrix();
		}
	}
	/*}
	glPopMatrix();*/
	endDraw();

}

int main()
{

	// Initialize the controls
	// Constructor is ModelerControl(name, minimumvalue, maximumvalue, 
	// stepsize, defaultvalue)
	ModelerControl controls[NUMCONTROLS];
	controls[TEXTURE_MAPPING] = ModelerControl("Texture Mapping", 0, 1, 1, 0);
	controls[XPOS] = ModelerControl("X Position", -5, 5, 0.1f, 0);
	controls[YPOS] = ModelerControl("Y Position", -5, 5, 0.1f, 0);
	controls[ZPOS] = ModelerControl("Z Position", -5, 5, 0.1f, 0);
	controls[HEAD_ROTATE] = ModelerControl("Head Rotate", -90, 90, 0.2f, 0);
	controls[LEFT_FEET_ROTATE] = ModelerControl("Left Feet Rotate", 0, 45, 0.2f, 0);
	controls[RIGHT_FEET_ROTATE] = ModelerControl("Right Feet Rotate", 0, 45, 0.2f, 0);
	controls[LIGHT_POS_X] = ModelerControl("Light 0 source position X", -100, 100, 1, 50);
	controls[LIGHT_POS_Y] = ModelerControl("Light 0 source position Y", -100, 100, 1, 50);
	controls[LIGHT_POS_Z] = ModelerControl("Light 0 source position Z", -100, 100, 1, 50);
	controls[LIGHT1_POS_X] = ModelerControl("Light 1 source position X", -100, 100, 1, -100);
	controls[LIGHT1_POS_Y] = ModelerControl("Light 1 source position Y", -100, 100, 1, -100);
	controls[LIGHT1_POS_Z] = ModelerControl("Light 1 source position Z", -100, 100, 1, -100);
	controls[LIGHT_INTENSITY] = ModelerControl("Light intensity", 0, 1, 0.01f, 1);

	controls[LEFT_ARM_X_ROTATE] = ModelerControl("Left Arm Rotate X", -90, 90, 0.2f, 0);
	controls[RIGHT_ARM_X_ROTATE] = ModelerControl("Right Arm Rotate X", -90, 90, 0.2f, 0);
	controls[LEFT_ARM_Y_ROTATE] = ModelerControl("Left Arm Rotate Y", 0, 90, 0.2f, 0);
	controls[RIGHT_ARM_Y_ROTATE] = ModelerControl("Right Arm Rotate Y", 0, 90, 0.2f, 0);
	controls[LEFT_ARM_Z_ROTATE] = ModelerControl("Left Arm Rotate Z", 0, 90, 0.2f, 0);
	controls[RIGHT_ARM_Z_ROTATE] = ModelerControl("Right Arm Rotate Z", 0, 90, 0.2f, 0);

	controls[LEFT_LOWER_ARM_ROTATE] = ModelerControl("Left Lower Arm Rotate", -15, 90, 0.2f, 0);
	controls[RIGHT_LOWER_ARM_ROTATE] = ModelerControl("Right Lower Arm Rotate", -15, 90, 0.2f, 0);

	controls[ROCKET_GRAB] = ModelerControl("Ability: Rocket Grab", 0, 100, 1.0f, 0);

	controls[LEFT_HAND_ROTATE] = ModelerControl("Left Hand Rotate", -90, 90, 0.2f, 0);
	controls[RIGHT_HAND_ROTATE] = ModelerControl("Right Hand Rotate", -90, 90, 0.2f, 0);

	controls[LEFT_THUMB_UP] = ModelerControl("Left Thumb Up", 0, 70, 0.2f, 0);
	controls[RIGHT_THUMB_UP] = ModelerControl("Right Thumb Up", 0, 70, 0.2f, 0);

	controls[LEFT_FINGER1_FIRST_JOINT_ROTATE] = ModelerControl("Left Finger1 First Joint Rotate", 0, 90, 0.2f, 0);
	controls[LEFT_FINGER1_SECOND_JOINT_ROTATE] = ModelerControl("Left Finger1 Second Joint Rotate", 0, 90, 0.2f, 0);
	controls[RIGHT_FINGER1_FIRST_JOINT_ROTATE] = ModelerControl("Right Finger1 First Joint Rotate", 0, 90, 0.2f, 0);
	controls[RIGHT_FINGER1_SECOND_JOINT_ROTATE] = ModelerControl("Right Finger1 Second Joint Rotate", 0, 90, 0.2f, 0);

	controls[LEFT_FINGER2_FIRST_JOINT_ROTATE] = ModelerControl("Left Finger2 First Joint Rotate", 0, 90, 0.2f, 0);
	controls[LEFT_FINGER2_SECOND_JOINT_ROTATE] = ModelerControl("Left Finger2 Second Joint Rotate", 0, 90, 0.2f, 0);
	controls[RIGHT_FINGER2_FIRST_JOINT_ROTATE] = ModelerControl("Right Finger2 First Joint Rotate", 0, 90, 0.2f, 0);
	controls[RIGHT_FINGER2_SECOND_JOINT_ROTATE] = ModelerControl("Right Finger2 Second Joint Rotate", 0, 90, 0.2f, 0);

	controls[LEFT_FINGER3_FIRST_JOINT_ROTATE] = ModelerControl("Left Finger3 First Joint Rotate", 0, 90, 0.2f, 0);
	controls[LEFT_FINGER3_SECOND_JOINT_ROTATE] = ModelerControl("Left Finger3 Second Joint Rotate", 0, 90, 0.2f, 0);
	controls[RIGHT_FINGER3_FIRST_JOINT_ROTATE] = ModelerControl("Right Finger3 First Joint Rotate", 0, 90, 0.2f, 0);
	controls[RIGHT_FINGER3_SECOND_JOINT_ROTATE] = ModelerControl("Right Finger3 Second Joint Rotate", 0, 90, 0.2f, 0);

	controls[CHANGE_HAND] = ModelerControl("Change Hands", 0, 1, 1, 0);
	controls[JETTING] = ModelerControl("Jetting", 0, 1, 1, 0);

	controls[LASING] = ModelerControl("Lasing!", 0, 1, 1, 0);

	controls[PARTICLE_CLOTH_POS_X] = ModelerControl("cloth pos x", -10, 10, 0.5, -2);
	controls[PARTICLE_CLOTH_POS_Y] = ModelerControl("cloth pos y", -10, 10, 0.5, 1.5);
	controls[PARTICLE_CLOTH_POS_Z] = ModelerControl("cloth pos z", -10, 10, 0.5, -2);

	controls[FLOCKING] = ModelerControl("Flocking", 0, 1, 1, 0);
	controls[HEIGHT_FIELD] = ModelerControl("Height field", 0, 1, 1, 0);

	ParticleSystem* ps = new ParticleSystem();
	
	ModelerApplication::Instance()->SetParticleSystem(ps);

	ModelerApplication::Instance()->Init(&createSampleModel, controls, NUMCONTROLS);

	return ModelerApplication::Instance()->Run();
}