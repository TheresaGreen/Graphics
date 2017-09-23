/* **************************
 * CSCI 420
 * Assignment 3 Raytracer
 * Name: Theresa Green
 * *************************
 */

#ifdef WIN32
#include <windows.h>
#endif

#if defined(WIN32) || defined(linux)
#include <GL/gl.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#define strcasecmp _stricmp
#endif

#include <imageIO.h>
#include <cmath>
#include <iostream>

#define MAX_TRIANGLES 20000
#define MAX_SPHERES 100
#define MAX_LIGHTS 100

char * filename = NULL;

//different display modes
#define MODE_DISPLAY 1
#define MODE_JPEG 2

#define DBL_EPSILON 2.2204460492503131e-16


int mode = MODE_DISPLAY;

//you may want to make these smaller for debugging purposes
static const int WIDTH = 640;
static const int HEIGHT = 480;

//the field of view of the camera
#define fov 60.0
static const double PI = atan(1) * 4;

unsigned char buffer[HEIGHT][WIDTH][3];

double dot(double vec1[3], double vec2[3]){
    return vec1[0]*vec2[0]+vec1[1]*vec2[1]+vec1[2]*vec2[2];

}

struct Light
{
    double position[3];
    double color[3];
    
};
struct Vertex
{
    double position[3];
    double color_diffuse[3];
    double color_specular[3];
    double normal[3];
    double shininess;
};

struct Triangle
{
    Vertex v[3];
    void AddLight(Light light, double* color, double* pos)
    {
        
        double vertexA[3] = {v[0].position[0],
                             v[0].position[1],
                             v[0].position[2]};
       
        double vertexB[3] = {v[1].position[0],
                             v[1].position[1],
                             v[1].position[2]};
        
        double vertexC[3] = {v[2].position[0],
                             v[2].position[1],
                             v[2].position[2]};
        
        
        
        
        double AB[3] ={ vertexB[0] - vertexA[0],
                        vertexB[1] - vertexA[1],
                        vertexB[2] - vertexA[2]};
        
        double AD[3] = { pos[0] - vertexA[0],
                         pos[1] - vertexA[1],
                         pos[2] - vertexA[2]};
        
        
        double crossA[3] = {AB[1]*AD[2] - AB[2]*AD[1],
                            AB[2]*AD[0] - AB[0]*AD[2],
                            AB[0]*AD[1] - AB[1]*AD[0] };
        
        
        double normalA[3] = {v[0].normal[0],
                             v[0].normal[1],
                             v[0].normal[2]};
        
        // normalize
        double mag = dot(normalA, normalA);
        mag= sqrt(mag);
        normalA[0]=normalA[0]/mag;
        normalA[1]=normalA[1]/mag;
        normalA[2]=normalA[2]/mag;
        
        
        
        double CB[3] = {vertexC[0] - vertexB[0],
                        vertexC[1] - vertexB[1],
                        vertexC[2] - vertexB[2]};
        
        
        double BD[3] = {pos[0] - vertexB[0], pos[1] - vertexB[1], pos[2] - vertexB[2]};
        
        //CB cross BD
        
        double crossB[3] = {CB[1]*BD[2] - CB[2]*BD[1],
                            CB[2]*BD[0] - CB[0]*BD[2],
                            CB[0]*BD[1] - CB[1]*BD[0] };
        
        
        
        double normalB[3] = {v[1].normal[0],
                             v[1].normal[1],
                             v[1].normal[2]};
        
        // normalize
        double mag1 = dot(normalB, normalB);
        mag1= sqrt(mag1);
        normalB[0]=normalB[0]/mag1;
        normalB[1]=normalB[1]/mag1;
        normalB[2]=normalB[2]/mag1;
        
        
        
        double AC[3] = {vertexA[0] - vertexC[0],
                        vertexA[1] - vertexC[1],
                        vertexA[2] - vertexC[2]};
        double CD[3] = {pos[0] - vertexC[0],
                        pos[1] - vertexC[1],
                        pos[2] - vertexC[2]};
        
        //CA cross CD
        
        double crossC[3] = {AC[1]*CD[2] - AC[2]*CD[1],
                            AC[2]*CD[0] - AC[0]*CD[2],
                            AC[0]*CD[1] - AC[1]*CD[0] };
        
        double normalC[3] = {v[2].normal[0],
                             v[2].normal[1],
                             v[2].normal[2]};
        
        // normalize
        double mag2 = dot(normalC, normalC);
        mag2= sqrt(mag2);
        normalC[0]=normalC[0]/mag2;
        normalC[1]=normalC[1]/mag2;
        normalC[2]=normalC[2]/mag2;
        
        
        
        
        double magA = crossA[0]*crossA[0]+
                      crossA[1]*crossA[1]+
                      crossA[2]*crossA[2];
        magA =sqrt(magA);
        
        double magB = crossB[0]*crossB[0]+
                      crossB[1]*crossB[1]+
                      crossB[2]*crossB[2];
        magB =sqrt(magB);
        
        double magC = crossC[0]*crossC[0]+
                      crossC[1]*crossC[1]+
                      crossC[2]*crossC[2];
        magC =sqrt(magC);

         float areaA = magA * 0.5f;
         float areaB = magB * 0.5f;
         float areaC = magC * 0.5f;
    
         float totalArea = areaA + areaB + areaC;
         
         float ratioA = areaA / totalArea;
        
          float ratioB = areaB / totalArea;
        
         float ratioC = areaC / totalArea;
        
        
        double lightPos[3] = {light.position[0],
                              light.position[1],
                              light.position[2]};
        
        double lightNormal[3] = {light.position[0]-pos[0],
                                 light.position[1]-pos[1],
                                 light.position[2]-pos[2]};
        
        
        // normalize
        double mag3 = dot(lightNormal, lightNormal);
        mag3= sqrt(mag3);
        lightNormal[0]=lightNormal[0]/mag3;
        lightNormal[1]=lightNormal[1]/mag3;
        lightNormal[2]=lightNormal[2]/mag3;
        
        
        double lightA[3] ={lightPos[0]-vertexA[0],
                           lightPos[1]-vertexA[1],
                           lightPos[2]-vertexA[2]};
        // normalize
        double woo = dot(lightA, lightA);
        woo= sqrt(woo);
        lightA[0]=lightA[0]/woo;
        lightA[1]=lightA[1]/woo;
        lightA[2]=lightA[2]/woo;
        
        
        
        double lightB[3] ={lightPos[0]-vertexB[0],
                            lightPos[1]-vertexB[1],
                            lightPos[2]-vertexB[2]};
        
        // normalize
        double woo2 = dot(lightB, lightB);
        woo2= sqrt(woo2);
        lightB[0]=lightB[0]/woo2;
        lightB[1]=lightB[1]/woo2;
        lightB[2]=lightB[2]/woo2;
        
        
        double lightC[3] ={lightPos[0]-vertexC[0],
                            lightPos[1]-vertexC[1],
                            lightPos[2]-vertexC[2]};
        // normalize
        double lord = dot(lightC, lightC);
        lord= sqrt(lord);
        lightC[0]=lightC[0]/lord;
        lightC[1]=lightC[1]/lord;
        lightC[2]=lightC[2]/lord;
        
        
        double LLA[3] ={ lightPos[0] - vertexA[0],
                         lightPos[1] - vertexA[1],
                         lightPos[2] - vertexA[2]};
        
        double LLB[3] ={ lightPos[0] - vertexB[0],
                         lightPos[1] - vertexB[1],
                         lightPos[2] - vertexB[2]};
        
        double LLC[3] ={ lightPos[0] - vertexC[0],
                         lightPos[1] - vertexC[1],
                         lightPos[2] - vertexC[2]};
        
         double dotScaleA = 2 * dot(LLA, normalA);
         double dotScaleB = 2 * dot(LLB, normalB);
         double dotScaleC = 2 * dot(LLC, normalC);
        
        double nA[3] ={normalA[0] *dotScaleA,
                       normalA[1] *dotScaleA,
                       normalA[2] *dotScaleA};
        
        double nB[3] ={ normalB[0] *dotScaleB,
                        normalB[1] *dotScaleB,
                        normalB[2] *dotScaleB};
        
        
        double nC[3] ={ normalC[0] *dotScaleC,
                        normalC[1] *dotScaleC,
                        normalC[2] *dotScaleC};
        
        double lA[3] = {lightA[0]*2,
                        lightA[1]*2,
                        lightA[2]*2 };
        
        double lB[3] = {lightB[0]*2,
                        lightB[1]*2,
                        lightB[2]*2 };
        
        double lC[3] = {lightC[0]*2,
                        lightC[1]*2,
                        lightC[2]*2 };
        
        
        double reflectionA[3] ={nA[0] - lA[0],
                                nA[1] - lA[1],
                                nA[2] - lA[2]};
        
        
        // normalize
        double crazy = dot(reflectionA, reflectionA);
        crazy= sqrt(crazy);
        reflectionA[0]=reflectionA[0]/crazy;
        reflectionA[1]=reflectionA[1]/crazy;
        reflectionA[2]=reflectionA[2]/crazy;
        
        
        
        double reflectionB[3] ={nB[0] - lB[0],
                                nB[1] - lB[1],
                                nB[2] - lB[2]};
        
        
        // normalize
        double madness = dot(reflectionB, reflectionB);
        madness= sqrt(madness);
        reflectionB[0]=reflectionB[0]/madness;
        reflectionB[1]=reflectionB[1]/madness;
        reflectionB[2]=reflectionB[2]/madness;
        
        
        double reflectionC[3] ={nC[0] - lC[0],
                                nC[1] - lC[1],
                                nC[2] - lC[2]};
        
        
        // normalize
        double ahhh = dot(reflectionC, reflectionC);
        ahhh= sqrt(ahhh);
        reflectionC[0]=reflectionC[0]/ahhh;
        reflectionC[1]=reflectionC[1]/ahhh;
        reflectionC[2]=reflectionC[2]/ahhh;
        
        
        double camA[3] ={vertexA[0]*-1, vertexA[1]*-1, vertexA[2]*-1};
        
        // normalize
        double hate = dot(camA, camA);
        hate= sqrt(hate);
        camA[0]=camA[0]/hate;
        camA[1]=camA[1]/hate;
        camA[2]=camA[2]/hate;
        
        double camB[3] ={vertexB[0]*-1, vertexB[1]*-1, vertexB[2]*-1};
        
        // normalize
        double hate1 = dot(camB, camB);
        hate1= sqrt(hate1);
        camB[0]=camB[0]/hate1;
        camB[1]=camB[1]/hate1;
        camB[2]=camB[2]/hate1;
        
        double camC[3] ={vertexC[0]*-1, vertexC[1]*-1, vertexC[2]*-1};
        
        // normalize
        double hate2 = dot(camC, camC);
        hate2= sqrt(hate2);
        camC[0]=camC[0]/hate2;
        camC[1]=camC[1]/hate2;
        camC[2]=camC[2]/hate2;
        
        double colorA[3];
        double colorB[3];
        double colorC[3];
        
        
         colorA[0] = v[0].color_diffuse[0]  * 255.0f * dot(lightA, normalA) + v[0].color_specular[0]  * 255.0f * pow(dot(reflectionA, camA), v[0].shininess);
         colorA[1] = v[0].color_diffuse[1]  * 255.0f * dot(lightA, normalA) + v[0].color_specular[1]  * 255.0f * pow(dot(reflectionA, camA), v[0].shininess);
         colorA[2] = v[0].color_diffuse[2]  * 255.0f * dot(lightA, normalA) + v[0].color_specular[2]  * 255.0f * pow(dot(reflectionA, camA), v[0].shininess);
       
        colorA[0]= colorA[0]* ratioB;
        colorA[1]= colorA[1]* ratioB;
        colorA[2]= colorA[2]* ratioB;
        
    
         colorB[0] = v[1].color_diffuse[0]  * 255.0f * dot(lightA, normalA) + v[1].color_specular[0]  * 255.0f * pow(dot(reflectionA, camA), v[1].shininess);
         colorB[1] = v[1].color_diffuse[1]  * 255.0f * dot(lightA, normalA) + v[1].color_specular[1]  * 255.0f * pow(dot(reflectionA, camA), v[1].shininess);
         colorB[2] = v[1].color_diffuse[2]  * 255.0f * dot(lightA, normalA) + v[1].color_specular[2]  * 255.0f * pow(dot(reflectionA, camA), v[1].shininess);
      
        colorB[0]= colorB[0]* ratioC;
        colorB[1]= colorB[1]* ratioC;
        colorB[2]= colorB[2]* ratioC;
         
        colorC[0] += v[2].color_diffuse[0]  * 255.0f * dot(lightA, normalA) + v[2].color_specular[0]  * 255.0f * pow(dot(reflectionA, camA), v[2].shininess);
        
        colorC[1] += v[2].color_diffuse[1]  * 255.0f * dot(lightA, normalA) + v[2].color_specular[1]  * 255.0f * pow(dot(reflectionA, camA), v[2].shininess);
        
        colorC[2] += v[2].color_diffuse[2]  * 255.0f * dot(lightA, normalA) + v[2].color_specular[2]  * 255.0f * pow(dot(reflectionA, camA), v[2].shininess);
        
        colorC[0]= colorC[0]* ratioA;
        colorC[1]= colorC[1]* ratioA;
        colorC[2]= colorC[2]* ratioA;
        
         color[0] += (colorA[0] + colorB[0] + colorC[0]);
         color[1] += (colorA[1] + colorB[1] + colorC[1]);
         color[2] += (colorA[2] + colorB[2] + colorC[2]);
        
        
        if(color[0]>255){
            color[0]=255;
        }
        if(color[1]>255){
            color[1]=255;
            
        }
        if(color[2]>255){
            color[2]=255;
            
        }
        
        if(color[0]<0){
            color[0]=0;
        }
        if(color[1]<0){
            color[1]=0;
            
        }
        if(color[2]<0){
            color[2]=0;
            
        }
    }

};



struct Sphere
{
    double position[3];
    double color_diffuse[3];
    double color_specular[3];
    double shininess;
    double radius;
    
    void AddLight(Light light, double* color, double* pos)
    {
        
        double normal[3] ={ pos[0] - position[0],
                            pos[1] - position[1],
                            pos[2] - position[2]};
        
        
        double lightLoc[3] = {light.position[0],
                              light.position[1],
                              light.position[2]} ;
        if (light.position[0]==0){
            lightLoc[0]=.001;
        
        }
        if (light.position[1]==0){
            lightLoc[1]=.001;
            
        }
        if (light.position[2]==0){
            lightLoc[2]=.001;
            
        }
        
        double lightNormal[3] ={lightLoc[0] - pos[0],
                                lightLoc[1] - pos[1],
                                lightLoc[2] - pos[2]};
        
        // normalize
        double mag = dot(lightNormal, lightNormal);
        mag= sqrt(mag);
        lightNormal[0]=lightNormal[0]/mag;
        lightNormal[1]=lightNormal[1]/mag;
        lightNormal[2]=lightNormal[2]/mag;
    
        double mag1 = dot(lightLoc, lightLoc);
        mag1= sqrt(mag1);
        lightLoc[0]=lightLoc[0]/mag1;
        lightLoc[1]=lightLoc[1]/mag1;
        lightLoc[2]=lightLoc[2]/mag1;
        
        
        
        double LL[3] = {lightLoc[0]-pos[0],
                        lightLoc[1]-pos[1],
                        lightLoc[2]-pos[2]};
        
        double dotScale = 2 * dot(LL, normal);
        
        double normNorm[3] = { normal[0], normal[1], normal[2]};
        
        
        // normalize
        double mag2 = dot(normal, normal);
        mag2= sqrt(mag2);
        normal[0]=normal[0]/mag2;
        normal[1]=normal[1]/mag2;
        normal[2]=normal[2]/mag2;
        
        
        // normalize
        double mag3 = dot(normNorm, normNorm);
        mag3= sqrt(mag3);
        normNorm[0]=normNorm[0]/mag3;
        normNorm[1]=normNorm[1]/mag3;
        normNorm[2]=normNorm[2]/mag3;
        
        normNorm[0]=normNorm[0]*dotScale;
        normNorm[1]=normNorm[1]*dotScale;
        normNorm[2]=normNorm[2]*dotScale;
        
        
        
        
        double l[3] = { lightLoc[0]*2,
                        lightLoc[1]*2,
                        lightLoc[2]*2};
        
        double reflection[3] = {normNorm[0]-l[0],
                                normNorm[1]-l[1],
                                normNorm[2]-l[2]};
        
        // normalize
        double mag4 = dot(reflection, reflection);
        mag4= sqrt(mag4);
        reflection[0]=reflection[0]/mag4;
        reflection[1]=reflection[1]/mag4;
        reflection[2]=reflection[2]/mag4;
        
        
        double cam[3] ={pos[0] * -1,
                        pos[1] * -1,
                        pos[2] * -1};
        
        // normalize
        double mag5 = dot(cam, cam);
        mag5= sqrt(mag5);
        cam[0]=cam[0]/mag5;
        cam[1]=cam[1]/mag5;
        cam[2]=cam[2]/mag5;
        
        
        color[0] =  color_diffuse[0] * 255.0f * dot(lightNormal, normal);
        color[0] +=  color_specular[0] * 255.0f * pow(dot(reflection, cam), shininess);
        
        color[1] =  color_diffuse[1] * 255.0f * dot(lightNormal, normal);
        color[1] +=  color_specular[1] * 255.0f * pow(dot(reflection, cam), shininess);
        
        color[2] =  color_diffuse[2] * 255.0f * dot(lightNormal, normal);
        color[2] +=  color_specular[2] * 255.0f * pow(dot(reflection, cam), shininess);
        
        if(color[0]>255){
            color[0]=255;
        }
        if(color[1]>255){
            color[1]=255;
            
        }
        if(color[2]>255){
            color[2]=255;
            
        }
        
        if(color[0]<0){
            color[0]=0;
        }
        if(color[1]<0){
            color[1]=0;
            
        }
        if(color[2]<0){
            color[2]=0;
            
        }
        
        
    }
    void AddBlack(Light light, double* color, double* pos)
    {
        color[0] = 0;
        color[1] = 0;
        color[2] = 0;
    }

    
    
};


struct Ray{
    double origin[3];
    double direction[3];
    double range;
    Ray(double one[3], double two[3], double dist){
        origin[0] = one[0];
        origin[1] = one[1];
        origin[2] = one[2];
        direction[0] = two[0];
        direction[1] = two[1];
        direction[2] = two[2];
        range=dist;
    }
    bool IntersectSphere(Sphere s, double* out, double* normal){
        double center[3] = {s.position[0], s.position[1], s.position[2]};
        double b = 2 * (direction[0] * (origin[0] - center[0]) + direction[1] * (origin[1] - center[1]) + direction[2] * (origin[2] - center[2]));
        double c = (origin[0] - center[0]) * (origin[0] - center[0]) + (origin[1] - center[1]) * (origin[1] - center[1]) + (origin[2] - center[2]) * (origin[2] - center[2]) - s.radius * s.radius;
        double t0 = (-b + sqrtf(b * b - 4 * c)) / 2;
        double t1 = (-b - sqrtf(b * b - 4 * c)) / 2;
        
        if ((b*b - 4*c) < 0){
            return false;
        }
        
        if (t0 >= 0 || t1 >= 0)
        {
            if (t0 >= 0 && t1 >= 0)
            {
                double t = t0 < t1 ? t0 : t1;
                double tProgress[3] = {direction[0]*t, direction[1]*t, direction[2]*t};
                out[0] = origin[0] + tProgress[0];
                out[1] = origin[1] + tProgress[1];
                out[2] = origin[2] + tProgress[2];
                normal[0] = out[0] - center[0];
                normal[1] = out[1] - center[1];
                normal[2] = out[2] - center[2];
                return true;
            }
            else if (t0 >= 0)
            {
                double tProgress[3] = {direction[0]*t0, direction[1]*t0, direction[2]*t0};
                out[0] = origin[0] + tProgress[0];
                out[1] = origin[1] + tProgress[1];
                out[2] = origin[2] + tProgress[2];
                normal[0] = out[0] - center[0];
                normal[1] = out[1] - center[1];
                normal[2] = out[2] - center[2];
                return true;
            }
            else
            {
                double tProgress[3] = {direction[0]*t1, direction[1]*t1, direction[2]*t1};
                out[0] = origin[0] + tProgress[0];
                out[1] = origin[1] + tProgress[1];
                out[2] = origin[2] + tProgress[2];
                normal[0] = out[0] - center[0];
                normal[1] = out[1] - center[1];
                normal[2] = out[2] - center[2];
                return true;
            }
        }
        return false;
        
        
    }
    
    
    bool IntersectTriangle(Triangle tri, double * out, double * normal){
        double vertex1[3] = {tri.v[0].position[0],
                            tri.v[0].position[1],
                            tri.v[0].position[2]}
        ;
        double vertex2[3] = {tri.v[1].position[0],
                            tri.v[1].position[1],
                            tri.v[1].position[2]};
        
        double vertex3[3] = {tri.v[2].position[0],
                            tri.v[2].position[1],
                            tri.v[2].position[2]};
        
        //2 minus 1
        double vertex21[3] = {vertex2[0]-vertex1[0],
                              vertex2[1]-vertex1[1],
                              vertex2[2]-vertex1[2]};
        //3 minus 1
        double vertex31[3] = {vertex3[0]-vertex1[0],
                              vertex3[1]-vertex1[1],
                              vertex3[2]-vertex1[2]};
    
        //need for later
                double vertex32[3] = {vertex3[0]-vertex2[0],
                                    vertex3[1]-vertex2[1],
                                    vertex3[2]-vertex2[2]};
        
                double vertex13[3] = {vertex1[0]-vertex3[0],
                                    vertex1[1]-vertex3[1],
                                    vertex1[2]-vertex3[2]};
        
             //cross 21 and 31
        normal[0]=vertex21[1]*vertex31[2] - vertex21[2]*vertex31[1];
        normal[1]=vertex21[2]*vertex31[0] - vertex21[0]*vertex31[2];
        normal[2]=vertex21[0]*vertex31[1] - vertex21[1]*vertex31[0];
    
        // normalize
        double mag = dot(normal, normal);
        mag= sqrt(mag);
        normal[0]=normal[0]/mag;
        normal[1]=normal[1]/mag;
        normal[2]=normal[2]/mag;

        
        
        double vertex1O[3] = {vertex1[0]-origin[0],
                              vertex1[1]-origin[1],
                              vertex1[2]-origin[2]};
        
        double numer = dot(normal, vertex1O);
        double denom=dot(normal, direction);
        
        if (std::abs(denom)< 0){
            return false;
        }
        
    
        double t = numer/denom;
        if( t<= 0 ||t> range){
            return false;
        }
        
        double tProgress[3] = { direction[0]*t,
                                direction[1]*t,
                                direction[2]*t};
        
        out[0] = origin[0] + tProgress[0];
        out[1] = origin[1] + tProgress[1];
        out[2] = origin[2] + tProgress[2];
        
        //out minus vertex 1
        double out1[3]={out[0]-vertex1[0],
                        out[1]-vertex1[1],
                        out[2]-vertex1[2]};
        //out minus vertex two
        double out2[3]={out[0]-vertex2[0],
                        out[1]-vertex2[1],
                        out[2]-vertex2[2]};
        //out minus vertex three
        double out3[3]={out[0]-vertex3[0],
                        out[1]-vertex3[1],
                        out[2]-vertex3[2]};
        
        
        
        
        
        //cross 21 out1
        double Cross1[3] = {vertex21[1]*out1[2]-vertex21[2]*out1[1],
                            vertex21[2]*out1[0]-vertex21[0]*out1[2],
                            vertex21[0]*out1[1]-vertex21[1]*out1[0]};
        
        
        //cross 32 out2
        double Cross2[3] = {vertex32[1]*out2[2]-vertex32[2]*out2[1],
                            vertex32[2]*out2[0]-vertex32[0]*out2[2],
                            vertex32[0]*out2[1]-vertex32[1]*out2[0]};
        
        //cross 13 out3
        double Cross3[3] = {vertex13[1]*out3[2]-vertex13[2]*out3[1],
                            vertex13[2]*out3[0]-vertex13[0]*out3[2],
                            vertex13[0]*out3[1]-vertex13[1]*out3[0]};
        double dot1 = dot(normal, Cross1);
        double dot2 = dot(normal, Cross2);
        double dot3 = dot(normal, Cross3);
        
        if (dot1> 0 && dot2 > 0 && dot3 > 0){
        
            return true;
        }
        
        return false;
        
    }
    
};


Triangle triangles[MAX_TRIANGLES];
Sphere spheres[MAX_SPHERES];
Light lights[MAX_LIGHTS];
double ambient_light[3];

int num_triangles = 0;
int num_spheres = 0;
int num_lights = 0;

void plot_pixel_display(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel_jpeg(int x,int y,unsigned char r,unsigned char g,unsigned char b);
void plot_pixel(int x,int y,unsigned char r,unsigned char g,unsigned char b);

bool Shadow(Light light, double* color, double* pos, void* shape)
{
    double lightDir[3]={light.position[0] - pos[0],
                        light.position[1] - pos[1],
                        light.position[2] - pos[2] };
    
    double lightMag = lightDir[0] * lightDir[0] + lightDir[1]*lightDir[1] + lightDir[2]*lightDir[2];
    lightMag = sqrt(lightMag);
    
    // normalize
    double mag = dot(lightDir, lightDir);
    mag= sqrt(mag);
    lightDir[0]=lightDir[0]/mag;
    lightDir[1]=lightDir[1]/mag;
    lightDir[2]=lightDir[2]/mag;
    
    Ray ray = Ray(pos, lightDir, lightMag);
    
    
    static double furthest[3] = {0,0,0};
    static double intersect[3] = {0,0,0};
    static double normal[3] = {0,0,0};
    
    bool hit = false;
    
    
    for (int i = 0; i < num_spheres; i++){
        if (ray.IntersectSphere(spheres[i], intersect, normal) && &spheres[i] != shape){
            hit = true;
        }
    }
    
    
    for (int i = 0; i < num_triangles; i++){
        if (ray.IntersectTriangle(triangles[i], intersect, normal) && &triangles[i] != shape){

            hit = true;
        }
    }
    
    
    double furthestPos[3] = {furthest[0]-pos[0],
                             furthest[1]-pos[1],
                             furthest[2]-pos[2]};
    
    double magFP = furthestPos[0]*furthestPos[0] + furthestPos[1]*furthestPos[1] + furthestPos[2]*furthestPos[2];
    magFP = sqrt(magFP);
    
    if (hit && (magFP > 0.5)  ){
        return true;
    }else{
        return false;
    }
    
}

void AddAmbient(double* color)
{
    color[0] = ambient_light[0] * 255 + color[0];
    color[1] = ambient_light[1] * 255 + color[1];
    color[2] = ambient_light[2] * 255 + color[2];
    
    if (color[0] < 0){
        color[0] = 0;
    }
    if (color[1] < 0){
        color[1] = 0;
    }
    if (color[2] < 0){
        color[2] = 0;
    }
    
    if (color[0] > 255){
        color[0] = 255;
    }
    if (color[1] > 255){
        color[1] = 255;
    }
    if (color[2] > 255){
        color[2] = 255;
    }
}


//MODIFY THIS FUNCTION
void draw_scene()
{   int xPixel, yPixel;
    xPixel = yPixel =0;
    
  double screenHeight =tan((fov * PI / 180)/2);
  double screenWidth= (static_cast<double>(WIDTH)/HEIGHT) *tan((fov * PI / 180)/ 2);
  double  pixelSpace=( screenWidth *2) /WIDTH;
    
    //scan through the entire screen
    
    for (double x = screenWidth * -1; x < screenWidth; x += pixelSpace)
    {           glBegin(GL_POINTS);
                glPointSize(2.0);
        for (double y = screenHeight * -1; y < screenHeight; y += pixelSpace)
        {

                //send ray through each pixel
                double vec1[3]={0,0,0};
                double vec2[3]={x+ pixelSpace /2,y +pixelSpace /2 ,-1};
                // normalize direction vec
                double mag = dot(vec2, vec2);
                mag= sqrt(mag);
                vec2[0]=vec2[0]/mag;
                vec2[1]=vec2[1]/mag;
                vec2[2]=vec2[2]/mag;

                double closestIntersection[3]={0,0,-100};

                Ray ray(vec1, vec2, 100);
            
            ///intersect triangles
            
            for (int  i = 0; i < num_triangles; i++)
            {
                double  intersect[3] = {0, 0, -100};
                double normal[3]={0,0,0};
                if (ray.IntersectTriangle(triangles[i] , intersect , normal))
                {
                    if (intersect[2] > closestIntersection[2])
                    {
                        closestIntersection[0] = intersect[0];
                        closestIntersection[1] = intersect[1];
                        closestIntersection[2] = intersect[2];
                    }
                    
                    
                    double TriangleColor[3] = {0, 0, 0};
                    for (int   j = 0;j < num_lights; j++)
                    {
                        if (!Shadow(lights[j], TriangleColor, closestIntersection, &triangles[i]))
                        {
                            triangles[i].AddLight(lights[j], TriangleColor, closestIntersection);
                        }
                    }
                    AddAmbient(TriangleColor);
                    plot_pixel(xPixel, yPixel, TriangleColor[0],   TriangleColor[1],  TriangleColor[2]);
                    break;
                }
                
                
            }//end for
            
            

            
            
            ///intersect circles
                for (int i=0; i < num_spheres; i++){
                    double intersect[3]={0,0,-100};
                    double normal[3];
                    
                    if (ray.IntersectSphere(spheres[i], intersect, normal))
                    {
                        if (intersect[2] > closestIntersection[2])
                        {
                            closestIntersection[0] = intersect[0];
                            closestIntersection[1] = intersect[1];
                            closestIntersection[2] = intersect[2];
                        }
                        double sphereColor[3] = {0, 0, 0};
                        
                        for (int j = 0; j < num_lights; j++)
                        {
                            if (!Shadow(lights[j], sphereColor, closestIntersection, &spheres[i]))
                            {
                                spheres[i].AddLight(lights[j], sphereColor, closestIntersection);
                            }
                            //else black shadow
                            //i'm not sure if I need this
                            else{
                                spheres[i].AddBlack(lights[j], sphereColor, closestIntersection);
                            
                            }
                            
                        }
                        AddAmbient(sphereColor);
                        plot_pixel(xPixel, yPixel, sphereColor[0], sphereColor[1], sphereColor[2]);
                        break;
                    }
                }
            
            if (closestIntersection[2] == -100)
            {
                plot_pixel(xPixel, yPixel, 255, 255, 255);
            }
    
            yPixel++;
        }//end for y
        glEnd();
        glFlush();
        xPixel++;
        yPixel = 0;
    }//end for x
    
    printf("Done!\n"); fflush(stdout);
}

void plot_pixel_display(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
    glColor3f(((float)r) / 255.0f, ((float)g) / 255.0f, ((float)b) / 255.0f);
    glVertex2i(x,y);
}

void plot_pixel_jpeg(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
    buffer[y][x][0] = r;
    buffer[y][x][1] = g;
    buffer[y][x][2] = b;
}

void plot_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
    plot_pixel_display(x,y,r,g,b);
    if(mode == MODE_JPEG)
        plot_pixel_jpeg(x,y,r,g,b);
}

void save_jpg()
{
    printf("Saving JPEG file: %s\n", filename);
    
    ImageIO img(WIDTH, HEIGHT, 3, &buffer[0][0][0]);
    if (img.save(filename, ImageIO::FORMAT_JPEG) != ImageIO::OK)
        printf("Error in Saving\n");
    else
        printf("File saved Successfully\n");
}

void parse_check(const char *expected, char *found)
{
    if(strcasecmp(expected,found))
    {
        printf("Expected '%s ' found '%s '\n", expected, found);
        printf("Parse error, abnormal abortion\n");
        exit(0);
    }
}

void parse_doubles(FILE* file, const char *check, double p[3])
{
    char str[100];
    fscanf(file,"%s",str);
    parse_check(check,str);
    fscanf(file,"%lf %lf %lf",&p[0],&p[1],&p[2]);
    printf("%s %lf %lf %lf\n",check,p[0],p[1],p[2]);
}

void parse_rad(FILE *file, double *r)
{
    char str[100];
    fscanf(file,"%s",str);
    parse_check("rad:",str);
    fscanf(file,"%lf",r);
    printf("rad: %f\n",*r);
}

void parse_shi(FILE *file, double *shi)
{
    char s[100];
    fscanf(file,"%s",s);
    parse_check("shi:",s);
    fscanf(file,"%lf",shi);
    printf("shi: %f\n",*shi);
}

int loadScene(char *argv)
{
    FILE * file = fopen(argv,"r");
    int number_of_objects;
    char type[50];
    Triangle t;
    Sphere s;
    Light l;
    fscanf(file,"%i", &number_of_objects);
    
    printf("number of objects: %i\n",number_of_objects);
    
    parse_doubles(file,"amb:",ambient_light);
    
    for(int i=0; i<number_of_objects; i++)
    {
        fscanf(file,"%s\n",type);
        printf("%s\n",type);
        if(strcasecmp(type,"triangle")==0)
        {
            printf("found triangle\n");
            for(int j=0;j < 3;j++)
            {
                parse_doubles(file,"pos:",t.v[j].position);
                parse_doubles(file,"nor:",t.v[j].normal);
                parse_doubles(file,"dif:",t.v[j].color_diffuse);
                parse_doubles(file,"spe:",t.v[j].color_specular);
                parse_shi(file,&t.v[j].shininess);
            }
            
            if(num_triangles == MAX_TRIANGLES)
            {
                printf("too many triangles, you should increase MAX_TRIANGLES!\n");
                exit(0);
            }
            triangles[num_triangles++] = t;
        }
        else if(strcasecmp(type,"sphere")==0)
        {
            printf("found sphere\n");
            
            parse_doubles(file,"pos:",s.position);
            parse_rad(file,&s.radius);
            parse_doubles(file,"dif:",s.color_diffuse);
            parse_doubles(file,"spe:",s.color_specular);
            parse_shi(file,&s.shininess);
            
            if(num_spheres == MAX_SPHERES)
            {
                printf("too many spheres, you should increase MAX_SPHERES!\n");
                exit(0);
            }
            spheres[num_spheres++] = s;
        }
        else if(strcasecmp(type,"light")==0)
        {
            printf("found light\n");
            parse_doubles(file,"pos:",l.position);
            parse_doubles(file,"col:",l.color);
            
            if(num_lights == MAX_LIGHTS)
            {
                printf("too many lights, you should increase MAX_LIGHTS!\n");
                exit(0);
            }
            lights[num_lights++] = l;
        }
        else
        {
            printf("unknown type in scene description:\n%s\n",type);
            exit(0);
        }
    }
    return 0;
}

void display()
{
}

void init()
{
    glMatrixMode(GL_PROJECTION);
    glOrtho(0,WIDTH,0,HEIGHT,1,-1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void idle()
{
    //hack to make it only draw once
    static int once=0;
    if(!once)
    {
        draw_scene();
        if(mode == MODE_JPEG)
            save_jpg();
    }
    once=1;
}

int main(int argc, char ** argv)
{
    if ((argc < 2) || (argc > 3))
    {  
        printf ("Usage: %s <input scenefile> [output jpegname]\n", argv[0]);
        exit(0);
    }
    if(argc == 3)
    {
        mode = MODE_JPEG;
        filename = argv[2];
    }
    else if(argc == 2)
        mode = MODE_DISPLAY;
    
    glutInit(&argc,argv);
    loadScene(argv[1]);
    
    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
    glutInitWindowPosition(0,0);
    glutInitWindowSize(WIDTH,HEIGHT);
    int window = glutCreateWindow("Ray Tracer");
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    init();
    glutMainLoop();
}
