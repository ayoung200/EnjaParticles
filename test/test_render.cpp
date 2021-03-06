/****************************************************************************************
* Real-Time Particle System - An OpenCL based Particle system developed to run on modern GPUs. Includes SPH fluid simulations.
* version 1.0, September 14th 2011
*
* Copyright (C) 2011 Ian Johnson, Andrew Young, Gordon Erlebacher, Myrna Merced, Evan Bollig
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
****************************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <map>
#include <time.h>
#include <float.h>

//#include <utils.h>
//#include <string.h>
//#include <string>
#include <sstream>
#include <iomanip>

#include <fstream>
#include <GL/glew.h>
#if defined __APPLE__ || defined(MACOSX)
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
//OpenCL stuff
#endif

#include <RTPS.h>
#include <render/ParticleEffect.h>
#include <render/SSEffect.h>
#include <render/StreamlineEffect.h>
#include <render/ShaderLibrary.h>
#include <system/common/Sample.h>
//#include "timege.h"
#include "../rtpslib/render/util/stb_image_write.h"
#include "BunnyMesh.h"
#include "ParamParser.h"
#include "util.h"
#include <system/ParticleShape.h>

using namespace rtps;

int window_width = 640;
int window_height = 480;
int glutWindowHandle = 0;


#define DTR 0.0174532925

struct camera
{
    GLdouble leftfrustum;
    GLdouble rightfrustum;
    GLdouble bottomfrustum;
    GLdouble topfrustum;
    GLfloat modeltranslation;
} leftCam, rightCam;

bool stereo_enabled = false;
bool render_movie = false;
GLubyte* image = new GLubyte[window_width*window_height*4];
const char* render_dir = "./frames/";

char filename[512] = {'\0'};
unsigned int frame_counter = 0;
float depthZ = -10.0;                                      //depth of the object drawing

double fovy = 65.;                                          //field of view in y-axis
double aspect = double(window_width)/double(window_height);  //screen aspect ratio
double nearZ = 0.3;                                        //near clipping plane
double farZ = 100.0;                                        //far clipping plane
double screenZ = 10.0;                                     //screen projection plane
double IOD = 0.5;                                          //intraocular distance

float translate_x = -2.00f;
float translate_y = -2.70f;//300.f;
float translate_z = 3.50f;

float mass = 1.0f;
float sizeScale = 1.0f;

// mouse controls
int mouse_old_x, mouse_old_y;
int mouse_buttons = 0;
float rotate_x = 0.0, rotate_y = 0.0;
std::vector<Triangle> triangles;



void init_gl();
void render_stereo();
void setFrustum();

void appKeyboard(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void appRender();
void appDestroy();

void appMouse(int button, int state, int x, int y);
void appMotion(int x, int y);
void resizeWindow(int w, int h);

void timerCB(int ms);

void drawString(const char *str, int x, int y, float color[4], void *font);
void showMass();
void showFPS(float fps, std::string *report);
int write_movie_frame(const char* name);
void draw_collision_boxes();
void rotate_img(GLubyte* img, int size);

void *font = GLUT_BITMAP_8_BY_13;

rtps::CL* cli=NULL;
rtps::RTPS* sph=NULL;
rtps::RTPS* rb=NULL;
rtps::Domain* grid=NULL;
rtps::Domain* grid2=NULL;
rtps::Sample* sampleKernel=NULL;
std::map<std::string,rtps::ParticleEffect*> effects;
rtps::StreamlineEffect* streamline = NULL;
rtps::ShaderLibrary* lib = NULL;
rtps::ParticleShape* bunnyShape = NULL;
std::string renderType = "default";
bool renderVelocity = false;
bool paused = false;
bool voxelized = false;
GLuint bunnyVBO=0;
GLuint bunnyIBO=0;

#define NUM_PARTICLES 524288
//#define NUM_PARTICLES 262144
//#define NUM_PARTICLES 65536
//#define NUM_PARTICLES 32768
//#define NUM_PARTICLES 16384
//#define NUM_PARTICLES 10000
//#define NUM_PARTICLES 8192
//#define NUM_PARTICLES 4096
//#define NUM_PARTICLES 2048
//#define NUM_PARTICLES 1024
//#define NUM_PARTICLES 256
//#define DT .003f
#define DT .003f
//#define DT .015f




void write3DTextureToDisc(GLuint tex,int voxelResolution, const char* filename)
{
    printf("writing %s texture to disc.\n",filename);
    glBindTexture(GL_TEXTURE_3D_EXT,tex);
    GLubyte* image = new GLubyte[voxelResolution*voxelResolution*voxelResolution*4];
    //GLubyte* image = new GLubyte[voxelResolution*voxelResolution*voxelResolution*3];
    glGetTexImage(GL_TEXTURE_3D_EXT,0,GL_RGBA,GL_UNSIGNED_BYTE,image);
    //glGetTexImage(GL_TEXTURE_3D_EXT,0,GL_RGB,GL_UNSIGNED_BYTE,image);
    GLubyte* tmp2Dimg = new GLubyte[voxelResolution*voxelResolution*4];
    //GLubyte* tmp2Dimg = new GLubyte[voxelResolution*voxelResolution*3];
    for(int i = 0; i<voxelResolution; i++)
    {
        char fname[128];
        sprintf(fname,"%s-%03d.png",filename,i);
        memcpy(tmp2Dimg,image+(i*(voxelResolution*voxelResolution*4)),sizeof(GLubyte)*voxelResolution*voxelResolution*4);
        //memcpy(tmp2Dimg,image+(i*(voxelResolution*voxelResolution*3)),sizeof(GLubyte)*voxelResolution*voxelResolution*3);
        if (!stbi_write_png(fname,voxelResolution,voxelResolution,4,(void*)tmp2Dimg,0))
        //if (!stbi_write_png(fname,voxelResolution,voxelResolution,3,(void*)tmp2Dimg,0))
        {
            printf("failed to write image %s",filename);
        }
    }
    glBindTexture(GL_TEXTURE_3D_EXT,0);
    delete[] image;

}
//timers
//GE::Time *ts[3];

//================
//#include "materials_lights.h"

//----------------------------------------------------------------------
float rand_float(float mn, float mx)
{
    float r = rand() / (float) RAND_MAX;
    return mn + (mx-mn)*r;
}
//----------------------------------------------------------------------
//----------------------------------------------------------------------
int main(int argc, char** argv)
{
/*
    //initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA //| GLUT_ALPHA| GLUT_INDEX
		//|GLUT_STEREO //if you want stereo you must uncomment this.
		);
    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition (glutGet(GLUT_SCREEN_WIDTH)/2 - window_width/2,
                            glutGet(GLUT_SCREEN_HEIGHT)/2 - window_height/2);


    std::stringstream ss;
    ss << "Real-Time Particle System: " << NUM_PARTICLES << std::ends;
    glutWindowHandle = glutCreateWindow(ss.str().c_str());

    glutDisplayFunc(appRender); //main rendering function
    glutTimerFunc(30, timerCB, 30); //determin a minimum time between frames
    glutKeyboardFunc(appKeyboard);
    glutMouseFunc(appMouse);
    glutMotionFunc(appMotion);
    glutReshapeFunc(resizeWindow);

    //define_lights_and_materials();

    // initialize necessary OpenGL extensions
    glewInit();
    GLboolean bGLEW = glewIsSupported("GL_VERSION_2_0 GL_ARB_pixel_buffer_object");
    printf("GLEW supported?: %d\n", bGLEW);


    printf("before we call enjas functions\n");


    cli = new CL();
    //default constructor
    //rtps::RTPSettings settings;
    //rtps::Domain grid = Domain(float4(-5,-.3,0,0), float4(2, 2, 12, 0));
    grid = new Domain(float4(0,0,0,0), float4(10, 10, 10, 0));
    //rtps::Domain grid = Domain(float4(0,0,0,0), float4(2, 2, 2, 0));
    rtps::RTPSettings *settings = new rtps::RTPSettings(rtps::RTPSettings::SPH, NUM_PARTICLES, DT, grid);


    //should be argv[0]
#ifdef WIN32
    settings->SetSetting("rtps_path", ".");
#else
    settings->SetSetting("rtps_path", "./bin");
    //settings->SetSetting("rtps_path", argv[0]);
    //printf("arvg[0]: %s\n", argv[0]);
#endif

    //settings->setRenderType(RTPSettings::SCREEN_SPACE_RENDER);
    settings->setRenderType(RTPSettings::RENDER);
    //settings.setRenderType(RTPSettings::SPRITE_RENDER);
    settings->SetSetting("render_use_alpha", true);
    //settings->SetSetting("render_use_alpha", false);
    settings->SetSetting("render_alpha_function", "add");
    settings->setRadiusScale(0.4);
    settings->setBlurScale(1.0);
    settings->setUseGLSL(1);

    settings->SetSetting("sub_intervals", 1);

    sph = new rtps::RTPS(settings,cli);
    //ps = new rtps::RTPS();

    sph->settings->SetSetting("Gravity", -9.8f); // -9.8 m/sec^2
    sph->settings->SetSetting("Gas Constant", 1.f);
    sph->settings->SetSetting("Viscosity", .001f);
    sph->settings->SetSetting("Velocity Limit", 600.0f);
    sph->settings->SetSetting("XSPH Factor", .15f);
    sph->settings->SetSetting("Friction Kinetic", 0.0f);
    sph->settings->SetSetting("Friction Static", 0.0f);
    sph->settings->SetSetting("Boundary Stiffness", 20000.0f);
    sph->settings->SetSetting("Boundary Dampening", 256.0f);


    grid2 = new Domain(float4(0,0,0,0), float4(10, 10, 10, 0));
    rtps::RTPSettings* rb_settings = new rtps::RTPSettings(rtps::RTPSettings::PARTICLE_RIGIDBODY, NUM_PARTICLES, DT , grid2);


    //should be argv[0]
#ifdef WIN32
    rb_settings->SetSetting("rtps_path", ".");
#else
    rb_settings->SetSetting("rtps_path", "./bin");
    //settings->SetSetting("rtps_path", argv[0]);
    //printf("arvg[0]: %s\n", argv[0]);
#endif

    //rb_settings->setRenderType(RTPSettings::SCREEN_SPACE_RENDER);
    rb_settings->setRenderType(RTPSettings::RENDER);
    settings->SetSetting("render_use_alpha", true);
    //settings->SetSetting("render_use_alpha", false);
    settings->SetSetting("render_alpha_function", "add");
    //rb_settings.setRenderType(RTPSettings::SPRITE_RENDER);
    rb_settings->setRadiusScale(0.4);
    rb_settings->setBlurScale(1.0);
    rb_settings->setUseGLSL(1);

    rb_settings->SetSetting("sub_intervals", 1);
    //settings->SetSetting("render_texture", "firejet_blast.png");
    //settings->SetSetting("render_frag_shader", "sprite_tex_frag.glsl");
    //settings->SetSetting("render_use_alpha", true);
    //settings->SetSetting("render_use_alpha", false);
    //settings->SetSetting("render_alpha_function", "add");
    //settings->SetSetting("lt_increment", -.00);
    //settings->SetSetting("lt_cl", "lifetime.cl");

    rb = new rtps::RTPS(rb_settings,cli);
    //ps = new rtps::RTPS();


    rb->settings->SetSetting("Gravity", -9.8f); // -9.8 m/sec^2
    rb->settings->SetSetting("Velocity Limit", 600.0f);
    rb->settings->SetSetting("Friction Kinetic", 0.0f);
    rb->settings->SetSetting("Friction Static", 0.0f);
    rb->settings->SetSetting("Boundary Stiffness", 20000.0f);
    rb->settings->SetSetting("Boundary Dampening", 256.0f);
    //rb->settings->SetSetting("Boundary Stiffness", 5.f);
    //rb->settings->SetSetting("Boundary Dampening", 2.f);
    rb->settings->SetSetting("Penetration Factor", .5f);
    rb->settings->SetSetting("Restitution",0.95f);

    sph->system->addInteractionSystem(rb->system);
    rb->system->addInteractionSystem(sph->system);
    //initialize the OpenGL scene for rendering
    init_gl();

    int gIndices[3*BUNNY_NUM_TRIANGLES];
    for(int i = 0; i<BUNNY_NUM_TRIANGLES;i++)
    {
        gIndices[(i*3)]=gIndicesBunny[i][0];
        gIndices[(i*3)+1]=gIndicesBunny[i][1];
        gIndices[(i*3)+2]=gIndicesBunny[i][2];
    }


    bunnyVBO = createVBO(gVerticesBunny, 3*BUNNY_NUM_VERTICES*sizeof(float),GL_ARRAY_BUFFER,GL_STATIC_DRAW );
    bunnyIBO = createVBO(gIndices, 3*BUNNY_NUM_TRIANGLES*sizeof(int),GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW );

    RenderSettings rs;
    //rs.blending=false;
    rs.blending=false;
    float nf[2];
    glGetFloatv(GL_DEPTH_RANGE,nf);
    rs.near = nf[0];
    rs.far = nf[1];
    rs.particleRadius = sph->system->getSpacing()*20.f;
    rs.windowWidth=window_width;
    rs.windowHeight=window_height;
    lib = new ShaderLibrary();
    lib->initializeShaders(GLSL_BIN_DIR);
    effects["default"]=new ParticleEffect(rs,*lib);
    //effects["sprite"]=new ParticleEffect();
    rs.blending=true;
    rs.particleRadius = sph->system->getSpacing()*.4f;
    effects["ssfr"]=new SSEffect(rs, *lib);
    int tmp = NUM_PARTICLES/100;
    vector<unsigned int> indices(100);
    for(int i = 0;i<100;i++)
    {
        indices[i]=tmp*i;
    }
    streamline=new StreamlineEffect(rs, *lib,100,100,indices,cli);
    float4 point(2.5f,2.5f,2.5f,1.0f);
    float4 point2(7.5f,7.5f,7.5f,1.0f);
    sph->system->addPointSource(point,1.0f);
    sph->system->addPointSource(point2,1.5f);
    sph->system->setAlpha(0.05f);*/

    rtps::ParamParser p;
    ifstream file("./bin/test1.xml", std::ifstream::in);
    p.readParameterFile(file);
    //glutMainLoop();
    return 0;
}


void appRender()
{

        //ps->system->sprayHoses();;
    if(!voxelized)
    {
       float3 min(FLT_MAX,FLT_MAX,FLT_MAX);
        float3 max(-FLT_MAX,-FLT_MAX,-FLT_MAX);
        for(int i = 0; i<BUNNY_NUM_VERTICES; i++)
        {
            float x = gVerticesBunny[(i*3)];
            float y = gVerticesBunny[(i*3)+1];
            float z = gVerticesBunny[(i*3)+2];
            if(x<min.x)
                min.x=x;
            if(x>max.x)
                max.x=x;
            if(y<min.y)
                min.y=y;
            if(y>max.y)
                max.y=y;
            if(z<min.z)
                min.z=z;
            if(z>max.z)
                max.z=z;
        }
        cout<<"min ("<<min.x<<","<<min.y<<","<<min.z<<")"<<endl;
        cout<<"max ("<<max.x<<","<<max.y<<","<<max.z<<")"<<endl;
        bunnyShape = new ParticleShape(min,max,rb->system->getSpacing(),3.0f);
        bunnyShape->voxelizeMesh(bunnyVBO,bunnyIBO,3*BUNNY_NUM_TRIANGLES);
        //write3DTextureToDisc(bunnyShape->getVoxelTexture(),bunnyShape->getVoxelResolution(),"bunnytex");
        voxelized=true;
    }
    glEnable(GL_DEPTH_TEST);
    if (stereo_enabled)
    {
        render_stereo();
    }
    else
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(fovy, aspect, nearZ, farZ);

        // set view matrix
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(-90, 1.0, 0.0, 0.0);
        glRotatef(rotate_x, 1.0, 0.0, 0.0);
        glRotatef(rotate_y, 0.0, 0.0, 1.0); //we switched around the axis so make this rotate_z
        glTranslatef(translate_x, translate_z, translate_y);


        glBindBuffer(GL_ARRAY_BUFFER, bunnyVBO);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyIBO);
        glEnableClientState( GL_VERTEX_ARRAY );
        glDrawElements(GL_TRIANGLES,3*BUNNY_NUM_TRIANGLES,GL_UNSIGNED_INT,0);
        glDisableClientState( GL_VERTEX_ARRAY );

        RenderUtils::renderBox(grid->getBndMin(),grid->getBndMax(),float4(0.0f,1.0,0.0f,1.0f));
        if(renderVelocity)
        {
            effects[renderType]->renderVector(sph->system->getPosVBO(),sph->system->getVelocityVBO(),sph->system->getNum());
            effects[renderType]->renderVector(rb->system->getPosVBO(),rb->system->getVelocityVBO(),rb->system->getNum());
        }
        //streamline->render();
        effects["default"]->render(rb->system->getPosVBO(),rb->system->getColVBO(),rb->system->getNum());
        effects[renderType]->render(sph->system->getPosVBO(),sph->system->getColVBO(),sph->system->getNum());
//	sph->render();
//        rb->render();
        //ps3->render();
        draw_collision_boxes();
        if(render_movie)
        {
            write_movie_frame("image");
        }

    }


    if(render_movie)
    {
        frame_counter++;
    }
    showMass();
    glutSwapBuffers();

    //glDisable(GL_DEPTH_TEST);
}



void appKeyboard(unsigned char key, int x, int y)
{
    int nn;
    float4 min;
    float4 max;
    switch (key)
    {
        case ' ':
            paused=!paused;
            return;
        case 'e': //dam break
        {
            nn = NUM_PARTICLES/2;
            float4 col1 = float4(0.05, 0.15, 8., 0.1);
            sph->system->addBox(nn, grid->getBndMin()+float4(0.5f,0.5f,0.5f,1.0f), grid->getBndMax()-float4(0.5f,0.5f,0.5f,1.0f), false,col1);
            //ps2->system->addBox(nn, min, max, false);
            return;
        }
        case 'g':
        {
            //nn = 16384;
            nn = NUM_PARTICLES/8;
            min = float4(2.5f, 2.5f, 2.5f, 1.0f);
            max = float4(7.5f, 7.5f, 7.5f, 1.0f);
            float4 col1 = float4(0., 0., 1., 0.05);
            sph->system->addBox(nn, min, max, false,col1);
            //ps2->system->addBox(nn, min, max, false);
            return;
        }
        case 'p': //print timers
            printf("SPH timers:\n");
            sph->system->printTimers();
            printf("RB timers:\n");
            rb->system->printTimers();
            return;
        case '\033': // escape quits
        case '\015': // Enter quits
        case 'Q':    // Q quits
        case 'q':    // q (or escape) quits
            // Cleanup up and quit
            appDestroy();
            return;
        case 'b':
        {
            //matrix is to position the rigidbody at 7,7,7 with no rotations.
            float16 mat(1.0f,0.0f,0.0f,7.0f,
                    0.0f,1.0f,0.0f,7.0f,
                    0.0f,0.0f,1.0f,7.0f,
                    0.0f,0.0f,0.0f,1.0f);
            float4 velocity(0.0f,0.0f,0.0f,0.0f);
            float4 color(1.0f,0.0f,0.0f,1.0f);
            rb->system->addParticleShape(bunnyShape->getVoxelTexture(),bunnyShape->getMaxDim(),float4(bunnyShape->getMin(),0.0f),mat,bunnyShape->getVoxelResolution(),velocity,color,mass);
            /*printf("deleting willy nilly\n");
            sph->system->testDelete();
            rb->system->testDelete();*/
            return;
        }
        case 'h':
        {
            //spray hose
            printf("about to make hose\n");
            float4 center(2., 2., .2, 1.);
            //float4 velocity(.6, -.6, -.6, 0);
            //float4 velocity(2., 5., -.8, 0);
            float4 velocity(0., 0., 2., 0);
            //sph sets spacing and multiplies by radius value
            float4 col1 = float4(0., 0., 1., 1.);


            sph->system->addHose(5000, center, velocity, 5, col1);
            return;
		}
        case 'n':
            render_movie=!render_movie;
            break;
        case '`':
            stereo_enabled = !stereo_enabled;
            break;
        case 't': //place a cube for collision
            {
                float4 center=(grid->getBndMin()+grid->getBndMax());
                center/=2.0f;
                float innerRadius=1.0f;
                float outerRadius=4.0f;
                float thickness=2.0f;
                sph->system->addTorus(NUM_PARTICLES,center,innerRadius,outerRadius,thickness);
                return;
            }
        case 'r': //drop a rectangle
            {

                float4 col1 = float4(1., 0.9, 0., 1.);

                float4 size = float4(1.,1.,1.,0.f);
                size=size*sizeScale;
                float4 mid = (grid->getMax()-grid->getMin());
                mid = mid/2.0f;
                mid.z=0.f;
                float4 position = float4(0.0f, 0.0f,grid->getMax().z-(size.z/2.f),1.0f);
                position.x = mid.x-(size.x/2.0f);
                position.y = mid.y-(size.y/2.0f);
                rb->system->addBox(NUM_PARTICLES, position, position+size, false, col1,mass);
                return;
            }
        case 'v':
            renderVelocity=!renderVelocity;
            return;
        case 'o':
            renderType="default";
            return;
        case 'c':
            renderType="ssfr";
            return;
        case 'C':
            return;
        case 'w':
            translate_z -= 0.1;
            break;
        case 'a':
            translate_x += 0.1;
            break;
        case 's':
            translate_z += 0.1;
            break;
        case 'd':
            translate_x -= 0.1;
            break;
        case 'z':
            translate_y += 0.1;
            break;
        case 'x':
            translate_y -= 0.1;
            break;
        case '+':
            mass*=10.0f;
            break;
        case '-':
            mass/=10.0f;
            break;
        case '[':
            sizeScale+=0.5f;
            break;
        case ']':
            sizeScale-=0.5f;
            break;
        default:
            return;
    }

    glutPostRedisplay();
    // set view matrix
    /*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(-90, 1.0, 0.0, 0.0);
    glRotatef(rotate_x, 1.0, 0.0, 0.0);
    glRotatef(rotate_y, 0.0, 0.0, 1.0); //we switched around the axis so make this rotate_z
    glTranslatef(translate_x, translate_z, translate_y);*/
}

void init_gl()
{
    // default initialization
    //glDisable(GL_DEPTH_TEST);

    // viewport
    glViewport(0, 0, window_width, window_height);

    // projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(60.0, (GLfloat)window_width / (GLfloat) window_height, 0.1, 100.0);
    //gluPerspective(fov, (GLfloat)window_width / (GLfloat) window_height, 0.3, 100.0);
    //gluPerspective(90.0, (GLfloat)window_width / (GLfloat) window_height, 0.1, 10000.0); //for lorentz

    // set view matrix
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(.9, .9, .9, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //glTranslatef(0, 10, 0);
    /*
    gluLookAt(  0,10,0,
                0,0,0,
                0,0,1);
    */


    //glTranslatef(0, translate_z, translate_y);
    //glRotatef(-90, 1.0, 0.0, 0.0);

    return;

}
void timerCB(int ms)
{
    glutTimerFunc(ms, timerCB, ms);
    if(!paused)
    {
        glFinish();
        sph->system->acquireGLBuffers();
        rb->system->acquireGLBuffers();
        sph->system->update();
        rb->system->update();
        sph->system->interact();
        rb->system->interact();
        sph->system->integrate();
        rb->system->integrate();
        sph->system->postProcess();
        rb->system->postProcess();
        streamline->addStreamLine(sph->system->getPositionBufferUnsorted(),sph->system->getColorBufferUnsorted(),sph->system->getNum());
        //streamline->addStreamLine(rb->system->getPositionBufferUnsorted(),rb->system->getColorBufferUnsorted(),rb->system->getNum());
        sph->system->releaseGLBuffers();
        rb->system->releaseGLBuffers();
    }
    //ps3->update();
    glutPostRedisplay();
}


void appDestroy()
{

    delete rb;
    delete sph;
    delete cli;
    delete grid;
    for(map<string,ParticleEffect*>::iterator i = effects.begin(); i!=effects.end(); i++)
    {
        delete i->second;
    }
    delete lib;
    delete streamline;


    if (glutWindowHandle)glutDestroyWindow(glutWindowHandle);
    printf("about to exit!\n");

    exit(0);
}




void appMouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)
    {
        mouse_buttons |= 1<<button;
    }
    else if (state == GLUT_UP)
    {
        mouse_buttons = 0;
    }

    mouse_old_x = x;
    mouse_old_y = y;

    //glutPostRedisplay();
}

void appMotion(int x, int y)
{
    float dx, dy;
    dx = x - mouse_old_x;
    dy = y - mouse_old_y;

    if (mouse_buttons & 1)
    {
        rotate_x += dy * 0.2;
        rotate_y += dx * 0.2;
    }
    else if (mouse_buttons & 4)
    {
        translate_z -= dy * 0.1;
    }

    mouse_old_x = x;
    mouse_old_y = y;

    // set view matrix
    glutPostRedisplay();
}


///////////////////////////////////////////////////////////////////////////////
// write 2d text using GLUT
// The projection matrix must be set to orthogonal before call this function.
///////////////////////////////////////////////////////////////////////////////
void drawString(const char *str, int x, int y, float color[4], void *font)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color

    glColor4fv(color);          // set text color
    glRasterPos2i(x, y);        // place text position

    // loop all characters in the string
    while (*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }

    glEnable(GL_LIGHTING);
    glPopAttrib();
}
void showMass()
{
    static std::stringstream ss;

    // backup current model-view matrix
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix

    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);        // switch to projection matrix
    glPushMatrix();                     // save current projection matrix
    glLoadIdentity();                   // reset projection matrix
    gluOrtho2D(0, 400, 0, 300);         // set to orthogonal projection

    float color[4] = {1, .5, .25, 1};

    ss.str("");
    ss << "Mass (Press '+' to increase and '-' to decrease): " << mass;
    drawString(ss.str().c_str(), 15, 15, color, font);
    ss.str("");
    ss << "Size (Press '[' to increase and ']' to decrease): " << sizeScale;
    drawString(ss.str().c_str(), 15, 25, color, font);

    // restore projection matrix
    glPopMatrix();                      // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);         // switch to modelview matrix
    glPopMatrix();                      // restore to previous modelview matrix
}
//----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
// display frame rates
///////////////////////////////////////////////////////////////////////////////
void showFPS(float fps, std::string* report)
{
    static std::stringstream ss;

    // backup current model-view matrix
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix

    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);        // switch to projection matrix
    glPushMatrix();                     // save current projection matrix
    glLoadIdentity();                   // reset projection matrix
    gluOrtho2D(0, 400, 0, 300);         // set to orthogonal projection

    float color[4] = {1, 1, 0, 1};

    // update fps every second
    ss.str("");
    ss << std::fixed << std::setprecision(1);
    ss << fps << " FPS" << std::ends; // update fps string
    ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
    drawString(ss.str().c_str(), 15, 286, color, font);
    drawString(report[0].c_str(), 15, 273, color, font);
    drawString(report[1].c_str(), 15, 260, color, font);
    ss.str("Mass: ");
    ss << mass;
    drawString(ss.str().c_str(), 15, 259, color, font);

    // restore projection matrix
    glPopMatrix();                      // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);         // switch to modelview matrix
    glPopMatrix();                      // restore to previous modelview matrix
}
//----------------------------------------------------------------------
void resizeWindow(int w, int h)
{
    if (h==0)
    {
        h=1;
    }
    glViewport(0, 0, w, h);

    // projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //gluPerspective(fov, aspect, nearZ, farZ);
    window_width = w;
    window_height = h;
    delete[] image;
    image = new GLubyte[w*h*4];
    setFrustum();
    glutPostRedisplay();
}

void render_stereo()
{

    glDrawBuffer(GL_BACK_LEFT);                              //draw into back left buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                                        //reset projection matrix
    glFrustum(leftCam.leftfrustum, leftCam.rightfrustum,     //set left view frustum
              leftCam.bottomfrustum, leftCam.topfrustum,
              nearZ, farZ);
    glTranslatef(leftCam.modeltranslation, 0.0, 0.0);        //translate to cancel parallax
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    {
        //glTranslatef(0.0, 0.0, depthZ);                        //translate to screenplane
        glRotatef(-90, 1.0, 0.0, 0.0);
        glRotatef(rotate_x, 1.0, 0.0, 0.0);
        glRotatef(rotate_y, 0.0, 0.0, 1.0); //we switched around the axis so make this rotate_z
        glTranslatef(translate_x, translate_z, translate_y);
        RenderUtils::renderBox(grid->getBndMin(),grid->getBndMax(),float4(0.0f,1.0,0.0f,1.0f));
        if(renderVelocity)
        {
            effects[renderType]->renderVector(sph->system->getPosVBO(),sph->system->getVelocityVBO(),sph->system->getNum());
            effects[renderType]->renderVector(rb->system->getPosVBO(),rb->system->getVelocityVBO(),rb->system->getNum());
        }
        effects["default"]->render(rb->system->getPosVBO(),rb->system->getColVBO(),rb->system->getNum());
        effects[renderType]->render(sph->system->getPosVBO(),sph->system->getColVBO(),sph->system->getNum());
//        sph->render();
//        rb->render();
        draw_collision_boxes();
    }
    glPopMatrix();

    if(render_movie)
    {
        write_movie_frame("stereo/image_left_");
    }

    glDrawBuffer(GL_BACK_RIGHT);                             //draw into back right buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                                        //reset projection matrix
    glFrustum(rightCam.leftfrustum, rightCam.rightfrustum,   //set left view frustum
              rightCam.bottomfrustum, rightCam.topfrustum,
              nearZ, farZ);
    glTranslatef(rightCam.modeltranslation, 0.0, 0.0);       //translate to cancel parallax
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushMatrix();
    {
        glRotatef(-90, 1.0, 0.0, 0.0);
        glRotatef(rotate_x, 1.0, 0.0, 0.0);
        glRotatef(rotate_y, 0.0, 0.0, 1.0); //we switched around the axis so make this rotate_z
        glTranslatef(translate_x, translate_z, translate_y);
        RenderUtils::renderBox(grid->getBndMin(),grid->getBndMax(),float4(0.0f,1.0,0.0f,1.0f));
        if(renderVelocity)
        {
            effects[renderType]->renderVector(sph->system->getPosVBO(),sph->system->getVelocityVBO(),sph->system->getNum());
            effects[renderType]->renderVector(rb->system->getPosVBO(),rb->system->getVelocityVBO(),rb->system->getNum());
        }
        effects["default"]->render(rb->system->getPosVBO(),rb->system->getColVBO(),rb->system->getNum());
        effects[renderType]->render(sph->system->getPosVBO(),sph->system->getColVBO(),sph->system->getNum());
//        sph->render();
//        rb->render();
        draw_collision_boxes();
    }
    glPopMatrix();
    if(render_movie)
    {
        write_movie_frame("stereo/image_right_");
    }
}


void setFrustum(void)
{
    double top = nearZ*tan(DTR*fovy/2);                    //sets top of frustum based on fovy and near clipping plane
    double right = aspect*top;                             //sets right of frustum based on aspect ratio
    double frustumshift = (IOD/2)*nearZ/screenZ;

    leftCam.topfrustum = top;
    leftCam.bottomfrustum = -top;
    leftCam.leftfrustum = -right + frustumshift;
    leftCam.rightfrustum = right + frustumshift;
    leftCam.modeltranslation = IOD/2;

    rightCam.topfrustum = top;
    rightCam.bottomfrustum = -top;
    rightCam.leftfrustum = -right - frustumshift;
    rightCam.rightfrustum = right - frustumshift;
    rightCam.modeltranslation = -IOD/2;
}

int write_movie_frame(const char* name)
{
    GLubyte* image = new GLubyte[windowWidth*windowHeight*3];
    sprintf(filename,"%s%s_%08d.png",render_dir,name,frame_counter);
    glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, image);
    if (!stbi_write_png(filename, windowWidth, windowHeight,3,(void*)image,0))
    {
        printf("failed to write image %s\n",filename);
        return -1;
    }
    return 0;
}
void rotate_img(GLubyte* img, int size)
{
    GLubyte tmp=0;
    for(int i = 0; i<size; i++)
    {
        for(int j = 0; j<4; j++)
        {
            tmp = img[(i*4)+j];
            img[(i*4)+j] = img[size-((i*4)+j)-1];
            img[size-((i*4)+j)-1] = tmp;
        }
    }
}

void draw_collision_boxes()
{
    glColor4f(0,0,1,.5);

    //glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_TRIANGLES);
    //printf("num triangles %zd\n", triangles.size());
    for (int i=0; i < triangles.size(); i++)
    {
        //for (int i=0; i < 20; i++) {
        Triangle& tria = triangles[i];
        glNormal3fv(&tria.normal.x);
        glVertex3f(tria.verts[0].x, tria.verts[0].y, tria.verts[0].z);
        glVertex3f(tria.verts[1].x, tria.verts[1].y, tria.verts[1].z);
        glVertex3f(tria.verts[2].x, tria.verts[2].y, tria.verts[2].z);
    }
    glEnd();

    glDisable(GL_BLEND);
}
