// 3DEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "olcConsoleGameEngine.h"

using namespace std;

struct vec_3d
{
    float x, y, z;
};
struct Triangle
{
    vec_3d p[3];
};
struct mesh
{
    vector<Triangle> tris;
};
struct mat4x4
{
    float m[4][4] = { 0 };
};
class olcGameEngine3d : public olcConsoleGameEngine
{
public:
    olcGameEngine3d()
    {
        m_sAppName = L'3d_Engine';
    }
public:
    bool OnUserCreate() override
    {
        MeshCube.tris = {

            // SOUTH
            { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

            // EAST                                                      
            { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

            // NORTH                                                     
            { 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

            // WEST                                                      
            { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

            // TOP                                                       
            { 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

            // BOTTOM                                                    
            { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

        };
        float fNear = 0.1f, 
            fNar = 1000.0f, 
            fFov = 90.0f, 
            fAspectRation = (float)ScreenHeight()/ (float)ScreenWidth(),
            fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);


        matProj.m[0][0] = fAspectRation * fFovRad;
        matProj.m[1][1] = fFovRad;
        matProj.m[2][2] = fNar / (fNar - fNear);
        matProj.m[3][2] = (-fNar * fNear) / (fNar - fNear);
        matProj.m[2][3] = 1.0f;
        matProj.m[3][3] = 0.0f;

        return true;
    }
    bool OnUserUpdate(float fElipsTime) override
    {
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
        // Set up rotation matrices
        fTheta += 1.0f * fElipsTime;

        // Rotation Z
        matRotZ.m[0][0] = cosf(fTheta);
        matRotZ.m[0][1] = sinf(fTheta);
        matRotZ.m[1][0] = -sinf(fTheta);
        matRotZ.m[1][1] = cosf(fTheta);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

        // Rotation X
        matRotx.m[0][0] = 1;
        matRotx.m[1][1] = cosf(fTheta * 0.5f);
        matRotx.m[1][2] = sinf(fTheta * 0.5f);
        matRotx.m[2][1] = -sinf(fTheta * 0.5f);
        matRotx.m[2][2] = cosf(fTheta * 0.5f);
        matRotx.m[3][3] = 1;

        for (auto tri : MeshCube.tris)
        {

            Triangle triProjected, triTransled, triRotatedZ, triRotatedZX;

            MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
            MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
            MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

            // Rotate in X-Axis
            MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotx);
            MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotx);
            MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotx);

            // Offset into the screen
            triTransled = triRotatedZX;
            triTransled.p[0].z = triRotatedZX.p[0].z + 3.0f;
            triTransled.p[1].z = triRotatedZX.p[1].z + 3.0f;
            triTransled.p[2].z = triRotatedZX.p[2].z + 3.0f;

            // Project triangles from 3D --> 2D
            MultiplyMatrixVector(triTransled.p[0], triProjected.p[0], matProj);
            MultiplyMatrixVector(triTransled.p[1], triProjected.p[1], matProj);
            MultiplyMatrixVector(triTransled.p[2], triProjected.p[2], matProj);

            // Scale into view
            triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
            triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
            triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
            triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
            triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
            triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
            triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
            triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
            triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

            DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
                triProjected.p[1].x, triProjected.p[1].y,
                triProjected.p[2].x, triProjected.p[2].y,
                PIXEL_SOLID, FG_WHITE);
        }
        return true;
    }
private:
    mesh MeshCube;
    mat4x4 matProj, matRotx, matRotZ;
    float  fTheta;
    void MultiplyMatrixVector(vec_3d& i, vec_3d& o, mat4x4& m)
    {
        o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
        o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
        o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
        float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

        if (w != 0.0f)
        {
            o.x /= w; o.y /= w; o.z /= w;
        }

    }
};

int main()
{
    olcGameEngine3d demo;
    if (demo.ConstructConsole(200, 70, 4, 4))
        demo.Start();
    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
