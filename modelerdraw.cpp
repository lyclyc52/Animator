#include "modelerdraw.h"
#include <FL/gl.h>
#include <GL/glu.h>
#include <cstdio>
#include "vec.h"
#include <gl/glu.h>
#include "bitmap.h"
#include <FL/fl_ask.H>
// ********************************************************
// Support functions from previous version of modeler
// ********************************************************
void _dump_current_modelview( void )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile == NULL)
    {
        fprintf(stderr, "No .ray file opened for writing, bailing out.\n");
        exit(-1);
    }
    
    GLdouble mv[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, mv );
    fprintf( mds->m_rayFile, 
        "transform(\n    (%f,%f,%f,%f),\n    (%f,%f,%f,%f),\n     (%f,%f,%f,%f),\n    (%f,%f,%f,%f),\n",
        mv[0], mv[4], mv[8], mv[12],
        mv[1], mv[5], mv[9], mv[13],
        mv[2], mv[6], mv[10], mv[14],
        mv[3], mv[7], mv[11], mv[15] );
}

void _dump_current_material( void )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile == NULL)
    {
        fprintf(stderr, "No .ray file opened for writing, bailing out.\n");
        exit(-1);
    }
    
    fprintf( mds->m_rayFile, 
        "material={\n    diffuse=(%f,%f,%f);\n    ambient=(%f,%f,%f);\n}\n",
        mds->m_diffuseColor[0], mds->m_diffuseColor[1], mds->m_diffuseColor[2], 
        mds->m_diffuseColor[0], mds->m_diffuseColor[1], mds->m_diffuseColor[2]);
}

// ****************************************************************************

// Initially assign singleton instance to NULL
ModelerDrawState* ModelerDrawState::m_instance = NULL;

ModelerDrawState::ModelerDrawState() : m_drawMode(NORMAL), m_quality(MEDIUM)
{
    float grey[]  = {.5f, .5f, .5f, 1};
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    
    memcpy(m_ambientColor, black, 4 * sizeof(float));
    memcpy(m_diffuseColor, grey, 4 * sizeof(float));
    memcpy(m_specularColor, white, 4 * sizeof(float));
    
    m_shininess = 0.5;
    
    m_rayFile = NULL;
}

// CLASS ModelerDrawState METHODS
ModelerDrawState* ModelerDrawState::Instance()
{
    // Return the singleton if it exists, otherwise, create it
    return (m_instance) ? (m_instance) : m_instance = new ModelerDrawState();
}

// ****************************************************************************
// Modeler functions for your use
// ****************************************************************************
// Set the current material properties

void setAmbientColor(float r, float g, float b)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_ambientColor[0] = (GLfloat)r;
    mds->m_ambientColor[1] = (GLfloat)g;
    mds->m_ambientColor[2] = (GLfloat)b;
    mds->m_ambientColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mds->m_ambientColor);
}

void setDiffuseColor(float r, float g, float b, float a)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_diffuseColor[0] = (GLfloat)r;
    mds->m_diffuseColor[1] = (GLfloat)g;
    mds->m_diffuseColor[2] = (GLfloat)b;
    mds->m_diffuseColor[3] = (GLfloat)a;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mds->m_diffuseColor);
    else
        glColor3f(r,g,b);
}

void setSpecularColor(float r, float g, float b)
{	
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_specularColor[0] = (GLfloat)r;
    mds->m_specularColor[1] = (GLfloat)g;
    mds->m_specularColor[2] = (GLfloat)b;
    mds->m_specularColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mds->m_specularColor);
}

void setShininess(float s)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_shininess = (GLfloat)s;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialf( GL_FRONT, GL_SHININESS, mds->m_shininess);
}

void setDrawMode(DrawModeSetting_t drawMode)
{
    ModelerDrawState::Instance()->m_drawMode = drawMode;
}

void setQuality(QualitySetting_t quality)
{
    ModelerDrawState::Instance()->m_quality = quality;
}

bool openRayFile(const char rayFileName[])
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	fprintf(stderr, "Ray file format output is buggy (ehsu)\n");
    
    if (!rayFileName)
        return false;
    
    if (mds->m_rayFile) 
        closeRayFile();
    
    mds->m_rayFile = fopen(rayFileName, "w");
    
    if (mds->m_rayFile != NULL) 
    {
        fprintf( mds->m_rayFile, "SBT-raytracer 1.0\n\n" );
        fprintf( mds->m_rayFile, "camera { fov=30; }\n\n" );
        fprintf( mds->m_rayFile, 
            "directional_light { direction=(-1,-1,-1); color=(0.7,0.7,0.7); }\n\n" );
        return true;
    }
    else
        return false;
}

void _setupOpenGl()
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
	switch (mds->m_drawMode)
	{
	case NORMAL:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_SMOOTH);
		break;
	case FLATSHADE:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_FLAT);
		break;
	case WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glShadeModel(GL_FLAT);
	default:
		break;
	}

}

void closeRayFile()
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile) 
        fclose(mds->m_rayFile);
    
    mds->m_rayFile = NULL;
}

void drawSphere(double r)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, "scale(%f,%f,%f,sphere {\n", r, r, r );
        _dump_current_material();
        fprintf(mds->m_rayFile, "}))\n" );
    }
    else
    {
        int divisions; 
        GLUquadricObj* gluq;
        
        switch(mds->m_quality)
        {
        case HIGH: 
            divisions = 32; break;
        case MEDIUM: 
            divisions = 20; break;
        case LOW:
            divisions = 12; break;
        case POOR:
            divisions = 8; break;
        }
        
        gluq = gluNewQuadric();
        gluQuadricDrawStyle( gluq, GLU_FILL );
        gluQuadricTexture( gluq, GL_TRUE );
        gluSphere(gluq, r, divisions, divisions);
        gluDeleteQuadric( gluq );
    }
}


void drawBox( double x, double y, double z )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile,  
            "scale(%f,%f,%f,translate(0.5,0.5,0.5,box {\n", x, y, z );
        _dump_current_material();
        fprintf(mds->m_rayFile,  "})))\n" );
    }
    else
    {
        /* remember which matrix mode OpenGL was in. */
        int savemode;
        glGetIntegerv( GL_MATRIX_MODE, &savemode );
        
        /* switch to the model matrix and scale by x,y,z. */
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glScaled( x, y, z );
        
        glBegin( GL_QUADS );
        
        glNormal3d( 0.0, 0.0, -1.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 0.0, 1.0, 0.0 );
        glVertex3d( 1.0, 1.0, 0.0 ); glVertex3d( 1.0, 0.0, 0.0 );
        
        glNormal3d( 0.0, -1.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 1.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        
        glNormal3d( -1.0, 0.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 1.0, 1.0 ); glVertex3d( 0.0, 1.0, 0.0 );
        
        glNormal3d( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 0.0, 1.0 ); glVertex3d( 1.0, 0.0, 1.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 0.0, 1.0, 1.0 );
        
        glNormal3d( 0.0, 1.0, 0.0 );
        glVertex3d( 0.0, 1.0, 0.0 ); glVertex3d( 0.0, 1.0, 1.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        
        glNormal3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 0.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 0.0, 1.0 );
        
        glEnd();
        
        /* restore the model matrix stack, and switch back to the matrix
        mode we were in. */
        glPopMatrix();
        glMatrixMode( savemode );
    }
}

void drawTextureBox( double x, double y, double z )
{
    // NOT IMPLEMENTED, SORRY (ehsu)

    ModelerDrawState* mds = ModelerDrawState::Instance();

    _setupOpenGl();

    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile,
            "scale(%f,%f,%f,translate(0.5,0.5,0.5,box {\n", x, y, z);
        _dump_current_material();
        fprintf(mds->m_rayFile, "})))\n");
    }
    else
    {
        /* remember which matrix mode OpenGL was in. */
        int savemode;
        glGetIntegerv(GL_MATRIX_MODE, &savemode);

        /* switch to the model matrix and scale by x,y,z. */
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glScaled(x, y, z);

        glBegin(GL_QUADS);

        glNormal3d(0.0, 0.0, -1.0);
        glTexCoord2f(0.0, 0.0); glVertex3d(0.0, 0.0, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3d(0.0, 1.0, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3d(1.0, 1.0, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3d(1.0, 0.0, 0.0);

        glNormal3d(0.0, -1.0, 0.0);
        glTexCoord2f(0.0, 0.0); glVertex3d(0.0, 0.0, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3d(1.0, 0.0, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3d(1.0, 0.0, 1.0);
        glTexCoord2f(1.0, 0.0); glVertex3d(0.0, 0.0, 1.0);

        glNormal3d(-1.0, 0.0, 0.0);
        glTexCoord2f(0.0, 0.0); glVertex3d(0.0, 0.0, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3d(0.0, 0.0, 1.0);
        glTexCoord2f(1.0, 1.0); glVertex3d(0.0, 1.0, 1.0);
        glTexCoord2f(1.0, 0.0); glVertex3d(0.0, 1.0, 0.0);

        glNormal3d(0.0, 0.0, 1.0);
        glTexCoord2f(0.0, 0.0); glVertex3d(0.0, 0.0, 1.0);
        glTexCoord2f(0.0, 1.0); glVertex3d(1.0, 0.0, 1.0);
        glTexCoord2f(1.0, 1.0); glVertex3d(1.0, 1.0, 1.0);
        glTexCoord2f(1.0, 0.0); glVertex3d(0.0, 1.0, 1.0);

        glNormal3d(0.0, 1.0, 0.0);
        glTexCoord2f(0.0, 0.0); glVertex3d(0.0, 1.0, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3d(0.0, 1.0, 1.0);
        glTexCoord2f(1.0, 1.0); glVertex3d(1.0, 1.0, 1.0);
        glTexCoord2f(1.0, 0.0); glVertex3d(1.0, 1.0, 0.0);

        glNormal3d(1.0, 0.0, 0.0);
        glTexCoord2f(0.0, 0.0); glVertex3d(1.0, 0.0, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3d(1.0, 1.0, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3d(1.0, 1.0, 1.0);
        glTexCoord2f(1.0, 0.0); glVertex3d(1.0, 0.0, 1.0);

        glEnd();

        /* restore the model matrix stack, and switch back to the matrix
        mode we were in. */
        glPopMatrix();
        glMatrixMode(savemode);
    }
}

void drawCylinder( double h, double r1, double r2 )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    int divisions;

	_setupOpenGl();
    
    switch(mds->m_quality)
    {
    case HIGH: 
        divisions = 32; break;
    case MEDIUM: 
        divisions = 20; break;
    case LOW:
        divisions = 12; break;
    case POOR:
        divisions = 8; break;
    }
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, 
            "cone { height=%f; bottom_radius=%f; top_radius=%f;\n", h, r1, r2 );
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n" );
    }
    else
    {
        GLUquadricObj* gluq;
        
        /* GLU will again do the work.  draw the sides of the cylinder. */
        gluq = gluNewQuadric();
        gluQuadricDrawStyle( gluq, GLU_FILL );
        gluQuadricTexture( gluq, GL_TRUE );
        gluCylinder( gluq, r1, r2, h, divisions, divisions);
        gluDeleteQuadric( gluq );
        
        if ( r1 > 0.0 )
        {
        /* if the r1 end does not come to a point, draw a flat disk to
            cover it up. */
            
            gluq = gluNewQuadric();
            gluQuadricDrawStyle( gluq, GLU_FILL );
            gluQuadricTexture( gluq, GL_TRUE );
            gluQuadricOrientation( gluq, GLU_INSIDE );
            gluDisk( gluq, 0.0, r1, divisions, divisions);
            gluDeleteQuadric( gluq );
        }
        
        if ( r2 > 0.0 )
        {
        /* if the r2 end does not come to a point, draw a flat disk to
            cover it up. */
            
            /* save the current matrix mode. */	
            int savemode;
            glGetIntegerv( GL_MATRIX_MODE, &savemode );
            
            /* translate the origin to the other end of the cylinder. */
            glMatrixMode( GL_MODELVIEW );
            glPushMatrix();
            glTranslated( 0.0, 0.0, h );
            
            /* draw a disk centered at the new origin. */
            gluq = gluNewQuadric();
            gluQuadricDrawStyle( gluq, GLU_FILL );
            gluQuadricTexture( gluq, GL_TRUE );
            gluQuadricOrientation( gluq, GLU_OUTSIDE );
            gluDisk( gluq, 0.0, r2, divisions, divisions);
            gluDeleteQuadric( gluq );
            
            /* restore the matrix stack and mode. */
            glPopMatrix();
            glMatrixMode( savemode );
        }
    }
    
}
void drawTriangle( double x1, double y1, double z1,
                   double x2, double y2, double z2,
                   double x3, double y3, double z3 )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();

    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, 
            "polymesh { points=((%f,%f,%f),(%f,%f,%f),(%f,%f,%f)); faces=((0,1,2));\n", x1, y1, z1, x2, y2, z2, x3, y3, z3 );
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n" );
    }
    else
    {
        double a, b, c, d, e, f;
        
        /* the normal to the triangle is the cross product of two of its edges. */
        a = x2-x1;
        b = y2-y1;
        c = z2-z1;
        
        d = x3-x1;
        e = y3-y1;
        f = z3-z1;
        
        glBegin( GL_TRIANGLES );
        glNormal3d( b*f - c*e, c*d - a*f, a*e - b*d );
        glVertex3d( x1, y1, z1 );
        glVertex3d( x2, y2, z2 );
        glVertex3d( x3, y3, z3 );
        glEnd();
    }
}

void drawHeightField(unsigned char* m_nHeight_field, int m_nHeight_field_width,
    int m_nHeight_field_height)
{

    double scale = 3;
    for (int w = 0; w < m_nHeight_field_width - 1; w++)
    {
        for (int h = 0; h < m_nHeight_field_height - 1; h++)
        {
            double y1 =
                (*(m_nHeight_field + (h * (m_nHeight_field_width)+w) * 3) * 0.299
                    + *(m_nHeight_field + (h * (m_nHeight_field_width)+w) * 3 + 1) * 0.587
                    + *(m_nHeight_field + (h * (m_nHeight_field_width)+w) * 3 + 2) * 0.114)
                / 255.0 * scale;
            double y2 =
                (*(m_nHeight_field + (h * (m_nHeight_field_width)+w + 1) * 3) * 0.299
                    + *(m_nHeight_field + (h * (m_nHeight_field_width)+w + 1) * 3 + 1) * 0.587
                    + *(m_nHeight_field + (h * (m_nHeight_field_width)+w + 1) * 3 + 2) * 0.114)
                / 255.0 * scale;
            double y3 =
                (*(m_nHeight_field + ((h + 1) * (m_nHeight_field_width)+w + 1) * 3) * 0.299
                    + *(m_nHeight_field + ((h + 1) * (m_nHeight_field_width)+w + 1) * 3 + 1) * 0.587
                    + *(m_nHeight_field + ((h + 1) * (m_nHeight_field_width)+w + 1) * 3 + 2) * 0.114)
                / 255.0 * scale;
            double y4 =
                (*(m_nHeight_field + ((h + 1) * (m_nHeight_field_width)+w) * 3) * 0.299
                    + *(m_nHeight_field + ((h + 1) * (m_nHeight_field_width)+w) * 3 + 1) * 0.587
                    + *(m_nHeight_field + ((h + 1) * (m_nHeight_field_width)+w) * 3 + 2) * 0.114)
                / 255.0 * scale;

            Vec3f p1((double)h / (m_nHeight_field_height - 1) * scale, y1,
                (double)w / (m_nHeight_field_width - 1) * scale);
            Vec3f p2((double)(h) / (m_nHeight_field_height - 1) * scale, y2,
               (double)(w + 1) / (m_nHeight_field_width - 1) * scale);
            Vec3f p3((double)(h + 1) / (m_nHeight_field_height - 1) * scale, y3,
                (double)(w + 1) / (m_nHeight_field_width - 1) * scale);
            Vec3f p4((double)(h + 1) / (m_nHeight_field_height - 1) * scale, y4,
                (double)(w) / (m_nHeight_field_width - 1) * scale);

            setDiffuseColor(0.8f, 0.8f, 0.8f);
            // todo : scale the color
            drawTriangle(p1[0], p1[1], p1[2],
                p3[0], p3[1], p3[2],
                p2[0], p2[1], p2[2]);
            drawTriangle(p1[0], p1[1], p1[2],
                p3[0], p3[1], p3[2],
                p4[0], p4[1], p4[2]);
        }
    }
}

void initTexture(unsigned char* im, int w, int h, bool alpha_flag)
{
    GLuint texture_id;

    
    /*unsigned char* im_scaled[512*512*4];
    gluScaleImage(GL_RGB, w, h,
        GL_UNSIGNED_BYTE, im, 512, 512,
        GL_UNSIGNED_BYTE, im_scaled);*/
        //delete[] im;
        // enable textures
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    // sample: specify texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    // set the active texture
    // test
    if (alpha_flag)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, im);
    }
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, im);
    //delete[] im;
    /*if(glGetError() != GL_NO_ERROR)
        fl_alert("%d", glGetError());*/


}


void drawBillboard(Camera* eye)
{
    glBegin(GL_QUADS);

    //Vec3f camera;
    Vec3f camera = eye->getPosition();
    Vec3f lookAt = eye->getLookAt();
    Vec3f up = eye->getUp();
    Vec3f normal = camera - lookAt;
    normal.normalize();
    Vec3f right = up ^ normal;
    right.normalize();
    up = normal ^ right;
    up.normalize();
    //Vec3f cur;
    glNormal3d(normal[0], normal[1], normal[2]);

    Vec3f p1 = up - right;
    Vec3f p2 = -up - right;
    Vec3f p3 = -up + right;
    Vec3f p4 = up + right;
    glTexCoord2f(0.0, 0.0); glVertex3d(p1[0], p1[1], p1[2]);
    glTexCoord2f(0.0, 1.0); glVertex3d(p2[0], p2[1], p2[2]);
    glTexCoord2f(1.0, 1.0); glVertex3d(p3[0], p3[1], p3[2]);
    glTexCoord2f(1.0, 0.0); glVertex3d(p4[0], p4[1], p4[2]);

    glEnd();
}

void drawBillboardFire(Camera* eye)
{
 
    glBegin(GL_QUADS);

    //Vec3f camera;
    Vec3f camera = eye->getPosition();
    Vec3f lookAt = eye->getLookAt();
    Vec3f up = eye->getUp();
    Vec3f normal = camera - lookAt;
    normal.normalize();
    Vec3f right = up ^ normal;
    right.normalize();
    up = normal ^ right;
    up.normalize();
    //Vec3f cur;
    glNormal3d(normal[0], normal[1], normal[2]);

    Vec3f p1 = up - right;
    Vec3f p2 = -up - right;
    Vec3f p3 = -up + right;
    Vec3f p4 = up + right;
    glVertex3d(p1[0], p1[1], p1[2]);
    glVertex3d(p2[0], p2[1], p2[2]);
    glVertex3d(p3[0], p3[1], p3[2]);
    glVertex3d(p4[0], p4[1], p4[2]);

    glEnd();
}

void loadProjectTexture(unsigned char* texture, int w, int h)
{
    GLfloat borderColor[4] =
    { 1.0, 1.0, 1.0, 1.0 };

    bool linearFilter = true;
    if (linearFilter) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h,
        GL_RGB, GL_UNSIGNED_BYTE, texture);
}

void setUpProjection(unsigned char* texture, int w, int h)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glEnable(GL_TEXTURE_GEN_Q);

    GLfloat eyePlaneS[] =
    { 1.0, 0.0, 0.0, 0.0 };
    GLfloat eyePlaneT[] =
    { 0.0, 1.0, 0.0, 0.0 };
    GLfloat eyePlaneR[] =
    { 0.0, 0.0, 1.0, 0.0 };
    GLfloat eyePlaneQ[] =
    { 0.0, 0.0, 0.0, 1.0 };

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    loadProjectTexture(texture, w, h); // check this

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_S, GL_EYE_PLANE, eyePlaneS);

    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_T, GL_EYE_PLANE, eyePlaneT);

    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_R, GL_EYE_PLANE, eyePlaneR);

    glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_Q, GL_EYE_PLANE, eyePlaneQ);
}



//void loadTextureProjection(GLfloat m[16])
void loadTextureProjection()
{
    float xmin = -0.035, xmax = 0.035;
    float ymin = -0.035, ymax = 0.035;
    float nnear = 0.1;
    float ffar = 1.9;
    float distance = -1.0;
    // inprove this ?


    GLfloat mInverse[4][4];

    /* Should use true inverse, but since m consists only of rotations, we can
       just use the transpose. */
    //matrixTranspose((GLfloat*)mInverse, m);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glTranslatef(0.5, 0.5, 0.0);
    glScalef(0.5, 0.5, 1.0);
    glFrustum(xmin, xmax, ymin, ymax, nnear, ffar);
    glTranslatef(0.0, 0.0, distance);
    
    // mult model view matrix
    //glMultMatrixf((GLfloat*)mInverse);
    gluLookAt(0, 0, -1,
        0, 0, 0,
        0, 1, 0);
    glMatrixMode(GL_MODELVIEW);
}

void drawTextureProjection()
{
    float xmin = -0.035, xmax = 0.035;
    float ymin = -0.035, ymax = 0.035;
    float nnear = 0.1;
    float ffar = 1.9;
    float distance = -1.0;

    float t = ffar / nnear;
    GLfloat n[4][3];
    GLfloat f[4][3];

    n[0][0] = xmin;
    n[0][1] = ymin;
    n[0][2] = -(nnear + distance);

    n[1][0] = xmax;
    n[1][1] = ymin;
    n[1][2] = -(nnear + distance);

    n[2][0] = xmax;
    n[2][1] = ymax;
    n[2][2] = -(nnear + distance);

    n[3][0] = xmin;
    n[3][1] = ymax;
    n[3][2] = -(nnear + distance);

    f[0][0] = xmin * t;
    f[0][1] = ymin * t;
    f[0][2] = -(ffar + distance);

    f[1][0] = xmax * t;
    f[1][1] = ymin * t;
    f[1][2] = -(ffar + distance);

    f[2][0] = xmax * t;
    f[2][1] = ymax * t;
    f[2][2] = -(ffar + distance);

    f[3][0] = xmin * t;
    f[3][1] = ymax * t;
    f[3][2] = -(ffar + distance);

    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINE_LOOP);
    glVertex3fv(n[0]);
    glVertex3fv(n[1]);
    glVertex3fv(n[2]);
    glVertex3fv(n[3]);
    glVertex3fv(f[3]);
    glVertex3fv(f[2]);
    glVertex3fv(f[1]);
    glVertex3fv(f[0]);
    glVertex3fv(n[0]);
    glVertex3fv(n[1]);
    glVertex3fv(f[1]);
    glVertex3fv(f[0]);
    glVertex3fv(f[3]);
    glVertex3fv(f[2]);
    glVertex3fv(n[2]);
    glVertex3fv(n[3]);
    glEnd();
}

void textureProjection()
{
    setDiffuseColor(1, 1, 1);
    int w, h;
    unsigned char* texture = readBMP("./samples/snow512.bmp", w, h);
    // load a texture

    setUpProjection(texture, w, h);
    loadTextureProjection();

    drawTextureProjection();


    glPushMatrix();
    glTranslated(-3, 0, -8);
    glRotated(30, 1, 0, 0);
    drawBox(6, 6, 0.1);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-3, -6, -5);
    glRotated(-30, 1, 0, 0);
    drawBox(6, 6, 0.1);
    glPopMatrix();
}