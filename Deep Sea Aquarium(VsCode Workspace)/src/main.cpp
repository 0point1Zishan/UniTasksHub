#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#ifndef CALLBACK
#define CALLBACK
#endif
#define MAX_FISH    100
#define MAX_BUBBLES 60

typedef struct{
    float x, y;
    float vx, vy;
    float size;
    int   species;
    float phase;     
    int   dir;
}Fish;

typedef struct {
    float x, y;
    float vy;
    float radius;
    float alpha;
    int   alive;
}Bubble;

Fish   fish[MAX_FISH];
int    fishCount      = 15;
int    currentSpecies = 0;
Bubble bubbles[MAX_BUBBLES];
int    bubblesEnabled = 0;

float  foodX = 0.0f, foodY = 0.0f;
int    foodActive = 0;
int    dayMode = 1;
int   winW  = 1301;
int   winH  = 764;
float gTime = 0.0f;
int   paused = 0;

static void drawFilledPolygon(const float pts[][2], int n, float r, float g, float b, float a){
    glColor4f(r, g, b, a);
    glBegin(GL_POLYGON);
    for(int i = 0; i < n; i++) glVertex2f(pts[i][0], pts[i][1]);
    glEnd();
}

//* Closed smooth outline
static void drawOutlineLoop(const float pts[][2], int n, float r, float g, float b, float a, float lineWidth){
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glLineWidth(lineWidth);
    glColor4f(r, g, b, a);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < n; i++) glVertex2f(pts[i][0], pts[i][1]);
    glEnd();
    glDisable(GL_LINE_SMOOTH);
}


void CALLBACK vertexCallback(void* vertexData) {
    const float* pt = (const float*)vertexData;
    glVertex2f(pt[0], pt[1]); 
}

void drawFilledTessCall(const float pts[][2], int numPoints, float r, float g, float b, float a) {
    GLUtesselator* tess = gluNewTess();

    gluTessCallback(tess, GLU_TESS_BEGIN, (void (CALLBACK*)())glBegin);
    gluTessCallback(tess, GLU_TESS_VERTEX, (void (CALLBACK*)())vertexCallback); 
    gluTessCallback(tess, GLU_TESS_END, (void (CALLBACK*)())glEnd);

    glColor4f(r, g, b, a);
    gluTessBeginPolygon(tess, NULL);
    gluTessBeginContour(tess);

    for(int i = 0; i < numPoints; i++){
        GLdouble tessCoords[3];
        tessCoords[0] = (GLdouble)pts[i][0];
        tessCoords[1] = (GLdouble)pts[i][1];
        tessCoords[2] = 0.0;                 

        gluTessVertex(tess, tessCoords, (void*)pts[i]);
    }

    gluTessEndContour(tess);
    gluTessEndPolygon(tess);
    gluDeleteTess(tess);
}

static void drawOutlineStrip(const float pts[][2], int n, float r, float g, float b, float a, float lineWidth){
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glLineWidth(lineWidth);
    glColor4f(r, g, b, a);
    glBegin(GL_LINE_STRIP);
    for(int i = 0; i < n; i++) glVertex2f(pts[i][0], pts[i][1]);
    glEnd();
    glDisable(GL_LINE_SMOOTH);
}

//* Smooth point cloud
static void drawPointCloud(const float pts[][2], int n, float r, float g, float b, float pointSize){
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glPointSize(pointSize);
    glColor3f(r, g, b);
    glBegin(GL_POINTS);

    for(int i = 0; i < n; i++) glVertex2f(pts[i][0], pts[i][1]);
    glEnd();
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_BLEND);
}

//* sand band cruve top-edge 
static void drawSandFill(const float curve[][2], int n, float bottomY, float r, float g, float b){
    int i;
    glColor3f(r, g, b);
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i < n; i++) {
        glVertex2f(curve[i][0], curve[i][1]);
        glVertex2f(curve[i][0], bottomY);
    }
    glEnd();
}

static void drawEdgeStrip(const float curve[][2], int n, float stripH, float r, float g, float b){
    glColor3f(r, g, b);
    glBegin(GL_QUAD_STRIP);
    for(int i = 0; i < n; i++){
        glVertex2f(curve[i][0], curve[i][1] - stripH);
        glVertex2f(curve[i][0], curve[i][1]);
    }
    glEnd();
}


//! =========================       SAND        ========================
void drawFullBigSand(void){
    static const float curve[][2] = {
        {  0.000f, 623.098f}, {  2.383f, 624.289f}, { 47.656f, 617.141f},
        { 55.995f, 615.950f}, { 61.952f, 613.567f}, { 67.909f, 614.758f},
        { 83.397f, 621.907f}, { 98.886f, 625.481f}, {357.418f, 626.672f},
        {400.308f, 633.821f}, {415.796f, 635.012f}, {434.858f, 637.395f},
        {461.069f, 637.395f}, {490.853f, 638.586f}, {514.681f, 639.777f},
        {539.701f, 642.160f}, {574.251f, 645.734f}, {594.505f, 648.117f},
        {595.236f, 649.244f}, {607.876f, 652.117f}, {622.815f, 653.841f},
        {632.582f, 655.564f}, {642.350f, 656.139f}, {658.437f, 657.288f},
        {680.845f, 659.586f}, {697.507f, 660.161f}, {721.638f, 661.884f},
        {731.980f, 662.459f}, {744.045f, 661.310f}, {765.878f, 660.161f},
        {783.690f, 658.437f}, {799.777f, 656.713f}, {815.290f, 654.415f},
        {825.632f, 653.266f}, {834.825f, 651.542f}, {847.465f, 650.393f},
        {865.851f, 646.946f}, {877.916f, 645.222f}, {892.280f, 642.924f},
        {904.346f, 640.051f}, {915.262f, 637.179f}, {923.880f, 636.030f},
        {934.222f, 633.157f}, {944.564f, 631.433f}, {956.055f, 629.135f},
        {961.801f, 628.560f}, {966.397f, 627.986f}, {971.568f, 627.411f},
        {976.165f, 627.986f}, {984.090f, 626.672f}, {1023.406f,620.715f},
        {1040.085f,614.758f}, {1068.679f,611.184f}, {1183.052f,604.036f},
        {1225.942f,607.610f}, {1297.426f,618.332f}, {1299.809f,618.332f}
    };
    int n = (int)(sizeof(curve) / sizeof(curve[0]));
    drawSandFill(curve, n, 761.299f, 0.88f, 0.79f, 0.62f);
}

void drawBottomRightSand(void){
    static const float p[][2] = {
        {1310.0f,687.739f},{1310.0f,770.0f},  {750.940f,761.857f},{766.453f,758.409f},
        {781.391f,754.962f},{796.330f,752.089f},{812.992f,749.791f},{834.250f,746.344f},
        {857.232f,742.322f},{878.491f,737.151f},{895.153f,731.980f},{913.538f,727.958f},
        {933.073f,722.787f},{952.033f,717.616f},{972.717f,713.594f},{995.699f,709.572f},
        {1014.660f,706.700f},{1038.791f,703.827f},{1064.646f,701.529f},{1092.799f,698.081f},
        {1118.654f,694.059f},{1150.829f,688.314f},{1179.556f,685.441f},{1203.687f,685.441f},
        {1216.902f,686.590f},{1228.393f,687.165f},{1240.459f,687.739f},{1251.375f,687.739f},
        {1261.143f,687.165f},{1269.186f,687.165f},{1282.401f,687.739f},{1292.743f,688.314f},
        {1310.0f,686.590f}
    };
    int n = (int)(sizeof(p) / sizeof(p[0]));
    drawFilledPolygon(p, n, 0.76f, 0.66f, 0.51f, 1.0f);
}

void drawTopMiddleSand(void){
    static const float p[][2] = {
        {595.236f,649.244f},{609.600f,648.095f},{625.113f,646.946f},{645.222f,644.073f},
        {662.459f,641.201f},{682.568f,637.179f},{704.401f,633.157f},{717.041f,630.859f},
        {731.405f,627.986f},{752.089f,624.539f},{766.453f,622.815f},{784.839f,620.517f},
        {803.799f,619.368f},{815.864f,618.793f},{826.206f,618.793f},{838.847f,618.793f},
        {849.763f,618.793f},{865.276f,618.793f},{878.491f,618.793f},{893.429f,618.793f},
        {905.495f,619.942f},{915.262f,620.517f},{922.731f,621.091f},{931.924f,621.666f},
        {938.244f,621.666f},{945.139f,622.815f},{952.608f,622.815f},{960.077f,622.815f},
        {970.419f,623.964f},{975.590f,624.539f},{981.336f,626.262f},{976.165f,627.986f},
        {971.568f,627.411f},{966.397f,627.986f},{961.801f,628.560f},{956.055f,629.135f},
        {944.564f,631.433f},{934.222f,633.157f},{923.880f,636.030f},{915.262f,637.179f},
        {904.346f,640.051f},{892.280f,642.924f},{877.916f,645.222f},{865.851f,646.946f},
        {847.465f,650.393f},{834.825f,651.542f},{825.632f,653.266f},{815.290f,654.415f},
        {799.777f,656.713f},{783.690f,658.437f},{765.878f,660.161f},{744.045f,661.310f},
        {731.980f,662.459f},{721.638f,661.884f},{697.507f,660.161f},{680.845f,659.586f},
        {658.437f,657.288f},{642.350f,656.139f},{632.582f,655.564f},{622.815f,653.841f},
        {607.876f,652.117f},{595.236f,649.244f}
    };
    int n = (int)(sizeof(p) / sizeof(p[0]));
    drawFilledPolygon(p, n, 0.969f, 0.863f, 0.627f, 1.0f);
}

void drawBottomRightUpperSand(void){
    static const float p[][2] = {
        {1151.403f,688.314f},{1123.250f,692.910f},{1111.759f,695.783f},{1095.097f,698.081f},
        {1084.180f,698.656f},{1073.839f,699.805f},{1064.646f,700.954f},{1053.729f,702.103f},
        {1035.918f,704.401f},{1012.936f,707.274f},{993.976f,710.147f}, {981.910f,711.870f},
        {967.546f,714.743f}, {953.182f,717.616f}, {939.393f,720.489f}, {926.179f,724.511f},
        {911.240f,727.958f}, {888.258f,734.278f}, {874.469f,738.300f}, {862.403f,740.598f},
        {849.189f,742.896f}, {838.272f,745.195f}, {826.206f,746.918f}, {814.715f,749.216f},
        {803.224f,750.940f}, {791.733f,753.238f}, {782.540f,754.962f}, {772.773f,757.260f},
        {768.177f,757.835f}, {756.111f,761.857f}, {749.791f,762.431f}, {756.111f,757.260f},
        {762.431f,754.962f}, {768.751f,753.238f}, {775.646f,750.940f}, {783.115f,749.216f},
        {791.733f,746.344f}, {799.202f,744.045f}, {808.395f,742.896f}, {818.163f,741.173f},
        {827.356f,738.874f}, {835.974f,737.151f}, {845.741f,736.576f}, {853.785f,735.427f},
        {862.403f,733.129f}, {873.894f,729.107f}, {884.236f,726.809f}, {895.153f,723.362f},
        {906.644f,720.489f}, {920.433f,716.467f}, {930.200f,714.743f}, {940.542f,712.445f},
        {964.674f,706.700f}, {981.336f,703.827f}, {994.550f,702.103f}, {1011.787f,699.805f},
        {1036.493f,696.358f},{1053.729f,694.634f},{1064.071f,693.485f},{1073.839f,691.761f},
        {1084.180f,691.761f},{1095.097f,691.761f},{1110.610f,690.037f},{1122.675f,688.314f},
        {1133.592f,687.165f},{1144.508f,686.016f},{1153.127f,687.165f},{1153.127f,688.888f}
    };
    int n = (int)(sizeof(p) / sizeof(p[0]));
    drawFilledPolygon(p, n, 0.969f, 0.863f, 0.627f, 1.0f);
}

void drawBottomLeftUpperSand(void){
    static const float p[][2] = {
        {3.447f,640.626f},  {9.193f,638.902f},  {13.215f,638.328f}, {18.386f,637.753f},
        {24.131f,637.179f}, {29.302f,635.455f},  {35.622f,636.030f}, {40.793f,636.030f},
        {46.539f,637.179f}, {52.859f,637.179f},  {57.455f,637.179f}, {61.477f,638.328f},
        {101.696f,636.604f},{372.310f,660.735f},  {374.608f,663.608f},{375.183f,665.332f},
        {375.183f,667.055f},{372.885f,671.652f},  {367.714f,673.950f},{360.819f,675.099f},
        {351.626f,675.099f},{344.157f,676.248f},  {337.837f,677.397f},{332.091f,677.972f},
        {327.495f,678.546f},{320.600f,679.696f},  {313.131f,682.568f},{305.662f,684.292f},
        {297.618f,685.441f},{287.851f,686.016f},  {280.382f,686.590f},{270.040f,687.165f},
        {257.974f,690.037f},{246.483f,691.187f},  {237.865f,692.910f},{228.097f,693.485f},
        {218.904f,694.059f},{213.734f,696.358f},  {206.839f,698.656f},{200.519f,698.081f},
        {191.901f,697.507f},{184.431f,695.783f},  {177.537f,693.485f},{169.493f,691.187f},
        {163.747f,691.187f},{156.853f,693.485f},  {143.064f,696.358f},{132.722f,702.103f},
        {118.358f,702.103f},{103.419f,702.103f},  {87.332f,702.678f}, {68.372f,703.252f},
        {48.837f,707.274f}, {35.048f,709.572f},   {22.982f,713.020f}, {16.662f,716.467f},
        {12.066f,717.041f}, {5.746f,718.765f},    {1.149f,720.489f},  {1.724f,640.051f}
    };
    int n = (int)(sizeof(p) / sizeof(p[0]));
    drawFilledPolygon(p, n, 0.76f, 0.66f, 0.51f, 1.0f);
}

void drawBottomLeftSand(void){
    static const float p[][2] = {
        {0.575f,718.191f},  {-10.0f,770.0f},   {431.489f,761.857f},{409.081f,756.686f},
        {395.292f,754.387f},{379.779f,752.664f},{366.564f,750.940f},{353.350f,748.642f},
        {338.986f,745.769f},{325.197f,742.896f},{313.706f,740.024f},{304.513f,737.725f},
        {295.895f,735.427f},{287.276f,734.278f},{275.785f,731.405f},{264.294f,729.107f},
        {253.378f,728.532f},{243.036f,727.958f},{224.650f,725.660f},{209.712f,724.511f},
        {195.348f,723.362f},{180.409f,722.212f},{168.918f,721.638f},{155.704f,721.063f},
        {141.914f,719.914f},{128.700f,718.765f},{118.358f,718.191f},{106.867f,717.041f},
        {95.376f,716.467f}, {75.266f,716.467f}, {62.626f,717.041f}, {47.113f,717.616f},
        {47.113f,717.616f}, {29.302f,718.191f}, {1.724f,717.616f}
    };
    int n = (int)(sizeof(p) / sizeof(p[0]));
    drawFilledPolygon(p, n, 0.76f, 0.66f, 0.51f, 1.0f);
}

void drawSandDarkEdge(void){
    static const float bigCurve[][2] = {
        {  0.000f,623.098f},{  2.383f,624.289f},{ 47.656f,617.141f},
        { 55.995f,615.950f},{ 61.952f,613.567f},{ 67.909f,614.758f},
        { 83.397f,621.907f},{ 98.886f,625.481f},{357.418f,626.672f},
        {400.308f,633.821f},{415.796f,635.012f},{434.858f,637.395f},
        {461.069f,637.395f},{490.853f,638.586f},{514.681f,639.777f},
        {539.701f,642.160f},{574.251f,645.734f},{594.505f,648.117f},
        {595.236f,649.244f},{607.876f,652.117f},{622.815f,653.841f},
        {632.582f,655.564f},{642.350f,656.139f},{658.437f,657.288f},
        {680.845f,659.586f},{697.507f,660.161f},{721.638f,661.884f},
        {731.980f,662.459f},{744.045f,661.310f},{765.878f,660.161f},
        {783.690f,658.437f},{799.777f,656.713f},{815.290f,654.415f},
        {825.632f,653.266f},{834.825f,651.542f},{847.465f,650.393f},
        {865.851f,646.946f},{877.916f,645.222f},{892.280f,642.924f},
        {904.346f,640.051f},{915.262f,637.179f},{923.880f,636.030f},
        {934.222f,633.157f},{944.564f,631.433f},{956.055f,629.135f},
        {961.801f,628.560f},{966.397f,627.986f},{971.568f,627.411f},
        {976.165f,627.986f},{984.090f,626.672f},{1023.406f,620.715f},
        {1040.085f,614.758f},{1068.679f,611.184f},{1183.052f,604.036f},
        {1225.942f,607.610f},{1297.426f,618.332f},{1299.809f,618.332f}
    };
    static const float midCurve[][2] = {
        {595.236f,649.244f},{609.600f,648.095f},{625.113f,646.946f},{645.222f,644.073f},
        {662.459f,641.201f},{682.568f,637.179f},{704.401f,633.157f},{717.041f,630.859f},
        {731.405f,627.986f},{752.089f,624.539f},{766.453f,622.815f},{784.839f,620.517f},
        {803.799f,619.368f},{815.864f,618.793f},{826.206f,618.793f},{838.847f,618.793f},
        {849.763f,618.793f},{865.276f,618.793f},{878.491f,618.793f},{893.429f,618.793f},
        {905.495f,619.942f},{915.262f,620.517f},{922.731f,621.091f},{931.924f,621.666f},
        {938.244f,621.666f},{945.139f,622.815f},{952.608f,622.815f},{960.077f,622.815f},
        {970.419f,623.964f},{975.590f,624.539f},{981.336f,626.262f}
    };
    int nBig = (int)(sizeof(bigCurve) / sizeof(bigCurve[0]));
    int nMid = (int)(sizeof(midCurve) / sizeof(midCurve[0]));
    drawEdgeStrip(midCurve, nMid, 2.0f, 0.612f, 0.529f, 0.357f);
    drawEdgeStrip(bigCurve, nBig, 2.0f, 0.612f, 0.529f, 0.357f);
}


//! ==========================       STONES      ========================== 
void drawDarkStones(void){
    static const float pts[][2] = {
        { 23.828f,655.266f},{ 40.507f,663.605f},{ 58.378f,664.797f},
        { 58.378f,681.476f},{ 34.550f,681.476f},{244.235f,706.495f},
        {272.829f,704.113f},{307.379f,706.495f},{301.422f,713.644f},
        {528.978f,663.605f},{558.763f,681.476f},{580.208f,727.940f},
        {586.165f,727.940f},{676.711f,727.940f},{677.902f,727.940f},
        {688.625f,721.984f},{711.261f,733.897f},{733.897f,729.132f},
        {735.089f,727.940f},{671.945f,683.859f},{673.136f,686.242f},
        {1247.387f,651.691f},{1272.407f,643.352f},{1274.789f,663.605f},
        {1230.708f,669.562f}
    };
    int n = (int)(sizeof(pts) / sizeof(pts[0]));
    drawPointCloud(pts, n, 0.396f, 0.357f, 0.267f, 5.0f);
}

void drawLightStones(void){
    static const float pts[][2] = {
        {1260.493f,656.457f},{1241.430f,665.988f},{1266.450f,663.605f},
        { 917.372f,677.902f},{ 936.434f,652.883f},{ 874.482f,667.179f},
        { 711.261f,724.366f},{ 284.743f,700.538f},{ 321.676f,692.199f},
        { 334.781f,706.495f},{ 347.886f,701.730f},{ 368.140f,688.625f},
        { 455.112f,663.605f},{ 545.658f,662.414f}
    };
    int n = (int)(sizeof(pts) / sizeof(pts[0]));
    drawPointCloud(pts, n, 0.890f, 0.859f, 0.792f, 5.0f);
}


//! ============= WATER ===============
static float waveY(float x, float time, int layer){
    switch (layer) {
    case 0:
        return sinf(x*0.0085f + time)         * 14.0f
             + sinf(x*0.0180f + time*1.40f)   *  6.0f
             + sinf(x*0.0035f + time*0.60f)   *  5.0f;
    case 1:
        return sinf(x*0.0075f + time*0.85f + 0.90f) * 11.0f
             + sinf(x*0.0140f + time*1.10f + 0.40f) *  5.0f;
    case 2:
        return sinf(x*0.0090f + time*0.70f + 1.80f) *  9.0f
             + sinf(x*0.0160f + time*0.95f + 1.00f) *  4.0f;
    }
    return 0.0f;
}

void water(void){
    int   x;
    float wy;
    float waveBase = winH * 0.82f;

    //* sky to deep water
    glBegin(GL_QUADS);
        glColor3f(0.04f,0.24f,0.47f); glVertex2f(0.0f,  (float)winH);
        glColor3f(0.04f,0.24f,0.47f); glVertex2f((float)winW,(float)winH);
        glColor3f(0.52f,0.80f,0.82f); glVertex2f((float)winW,0.0f);
        glColor3f(0.52f,0.80f,0.82f); glVertex2f(0.0f,  0.0f);
    glEnd();

    //* deep water
    glBegin(GL_QUAD_STRIP);
    for(int x = 0; x <= winW; x += 2){
        wy = waveBase + waveY((float)x, gTime, 0);
        glColor3f(0.01f,0.01f,0.07f); glVertex2f((float)x,(float)winH);
        glColor3f(0.05f,0.09f,0.24f); glVertex2f((float)x,wy);
    }
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    //* wave
    glBegin(GL_QUAD_STRIP);
    for(int x = 0; x <= winW; x += 2){
        wy = waveBase + waveY((float)x, gTime, 0);
        glColor4f(0.55f,0.78f,0.88f,0.50f); glVertex2f((float)x,wy);
        glColor4f(0.20f,0.50f,0.65f,0.00f); glVertex2f((float)x,wy-28.0f);
    }
    glEnd();

    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    for(int x = 0; x <= winW; x += 2){
        wy = waveBase + waveY((float)x, gTime, 0);
        glColor4f(0.72f,0.89f,0.95f,0.90f); glVertex2f((float)x,wy);
    }
    glEnd();

    glLineWidth(1.2f);
    glBegin(GL_LINE_STRIP);
    for(int x = 0; x <= winW; x += 2){
        wy = waveBase + waveY((float)x, gTime, 0) - 3.0f;
        glColor4f(0.85f,0.95f,1.00f,0.45f); glVertex2f((float)x,wy);
    }
    glEnd();

    {
        float w2Base = waveBase - 18.0f;
        glBegin(GL_QUAD_STRIP);
        for(int x = 0; x <= winW; x += 3){
            wy = w2Base + waveY((float)x, gTime, 1);
            glColor4f(0.30f,0.58f,0.72f,0.30f); glVertex2f((float)x,wy);
            glColor4f(0.18f,0.44f,0.58f,0.00f); glVertex2f((float)x,wy-22.0f);
        }
        glEnd();
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        for(int x = 0; x <= winW; x += 2){
            wy = w2Base + waveY((float)x, gTime, 1);
            glColor4f(0.52f,0.74f,0.84f,0.45f); glVertex2f((float)x,wy);
        }
        glEnd();
    }

    {
        float w3Base = waveBase - 38.0f;
        glBegin(GL_QUAD_STRIP);
        for(int x = 0; x <= winW; x += 3){
            wy = w3Base + waveY((float)x, gTime, 2);
            glColor4f(0.25f,0.52f,0.66f,0.20f); glVertex2f((float)x,wy);
            glColor4f(0.15f,0.40f,0.54f,0.00f); glVertex2f((float)x,wy-18.0f);
        }
        glEnd();
        glLineWidth(1.5f);
        glBegin(GL_LINE_STRIP);
        for(int x = 0; x <= winW; x += 2){
            wy = w3Base + waveY((float)x, gTime, 2);
            glColor4f(0.42f,0.66f,0.78f,0.30f); glVertex2f((float)x,wy);
        }
        glEnd();
    }

    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
}


//! ================== SUN  ==================
static void fillCircleMidpoint(int cx, int cy, int radius, float r, float g, float b, float a){
    int x = 0, y = radius, d = 1 - radius;
    glColor4f(r, g, b, a);
    while (x <= y) {
        glBegin(GL_LINES);
            glVertex2i(cx-y,cy+x); glVertex2i(cx+y,cy+x);
        glEnd();
        glBegin(GL_LINES);
            glVertex2i(cx-y,cy-x); glVertex2i(cx+y,cy-x);
        glEnd();
        glBegin(GL_LINES);
            glVertex2i(cx-x,cy+y); glVertex2i(cx+x,cy+y);
        glEnd();
        glBegin(GL_LINES);
            glVertex2i(cx-x,cy-y); glVertex2i(cx+x,cy-y);
        glEnd();
        if (d < 0) d += 2*x+3; else { d += 2*(x-y)+5; y--; }
        x++;
    }
}

void drawSun(void){
    fillCircleMidpoint(1055, 15, 55, 1.0f, 0.95f, 0.55f, 0.95f);
    fillCircleMidpoint(1055, 15, 50, 1.0f, 1.00f, 0.80f, 0.98f);
}

void drawSunRays(void){
    static const float rays[][5] = {
        {-145.0f, 1.8f, 14.0f, 150.0f, 0.035f},
        {-128.0f, 2.0f, 18.0f, 175.0f, 0.045f},
        {-112.0f, 2.2f, 22.0f, 195.0f, 0.055f},
        { -96.0f, 2.4f, 24.0f, 210.0f, 0.060f},
        { -80.0f, 2.6f, 26.0f, 220.0f, 0.065f},
        { -60.0f, 3.5f, 90.0f,1050.0f, 0.080f},
        { -44.0f, 4.0f, 80.0f, 880.0f, 0.120f},
        { -28.0f, 5.0f, 85.0f, 760.0f, 0.160f},
        { -10.0f, 6.0f, 95.0f, 680.0f, 0.200f},
        {   8.0f, 5.0f, 80.0f, 680.0f, 0.150f},
        {  26.0f, 4.0f, 68.0f, 640.0f, 0.110f},
        {  46.0f, 3.5f, 52.0f, 560.0f, 0.070f},
        {  64.0f, 2.6f, 26.0f, 220.0f, 0.065f},
        {  80.0f, 2.4f, 24.0f, 210.0f, 0.060f},
        {  96.0f, 2.2f, 22.0f, 195.0f, 0.055f},
        { 112.0f, 2.0f, 18.0f, 175.0f, 0.045f},
        { 130.0f, 1.8f, 14.0f, 150.0f, 0.035f}
    };
    int nRays = (int)(sizeof(rays) / sizeof(rays[0]));
    const float PI  = 3.14159265f;
    const float DEG = PI / 180.0f;
    const float cx  = 1055.0f, cy = 15.0f, arcR = 75.0f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(int pass = 0; pass < 2; pass++){
        float wMul = (pass == 0) ? 2.0f  : 1.0f;
        float aMul = (pass == 0) ? 0.35f : 1.0f;

        for(int i = 0; i < nRays; i++){
            float ang = rays[i][0] * DEG;
            float hwT = rays[i][1] * wMul;
            float hwB = rays[i][2] * wMul;
            float len = rays[i][3];
            float al  = rays[i][4] * aMul;
            float dx  =  sinf(ang), dy =  cosf(ang);
            float px  =  cosf(ang), py = -sinf(ang);
            float ox  = cx + dx*arcR, oy = cy + dy*arcR;
            float ex  = cx + dx*(arcR+len), ey = cy + dy*(arcR+len);

            glBegin(GL_QUADS);
                glColor4f(1.0f,1.0f,0.85f,al);
                glVertex2f(ox - px*hwT, oy - py*hwT);
                glVertex2f(ox + px*hwT, oy + py*hwT);
                glColor4f(1.0f,1.0f,0.85f,0.0f);
                glVertex2f(ex + px*hwB, ey + py*hwB);
                glVertex2f(ex - px*hwB, ey - py*hwB);
            glEnd();
        }
    }
    glDisable(GL_BLEND);
}

//! ================== MOON  ==================
void circle(GLfloat rx, GLfloat ry, GLfloat cx, GLfloat cy, int stD, int endD){
    int i;
    glBegin(GL_POLYGON);
    glVertex2f(cx, cy);
    for (i = stD; i <= endD; i++) {
        float angle = i * 3.1416f / 180.0f;
        float x = rx * cosf(angle);
        float y = ry * sinf(angle);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

void drawMoon(void) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float x = 157.22f;
    float y = 55.21f;
    float R = 55.0f;

    //* Enable stencil test
    glEnable(GL_STENCIL_TEST);
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);
    
    //* White moon circle
    glStencilFunc(GL_ALWAYS, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
    glColor4f(0.95f, 0.95f, 0.95f, 0.90f);
    circle(R, R, x, y, 0, 360);
    
    //* Now mask out
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    //* shadow circle
    float shadowX = x + 22.0f;
    glColor4f(0.0f, 0.01f, 0.06f, 1.0f);
    circle(R, R, shadowX, y, 0, 360);
    
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);
}

void drawNightOverlay(void){
    int s;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //* dark tint 
    glColor4f(0.00f, 0.01f, 0.06f, 0.62f);
    glBegin(GL_QUADS);
        glVertex2f(0,0); glVertex2f(1301,0);
        glVertex2f(1301,764); glVertex2f(0,764);
    glEnd();

    //* stars 
    srand(777);
    glColor4f(1.0f, 1.0f, 0.85f, 0.85f);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for(s = 0; s < 35; s++) {
        float sx = (float)(rand() % 1280);
        float sy = (float)(rand() % 80); 
        glVertex2f(sx, sy);
    }
    glEnd();
    glDisable(GL_BLEND);
}

float angle;        
float targetAngle;  
float speed;
float turnRate;     
float swimOffset;   

static void drawSingleFish(float sz, int sp) {
    static const float BODY[5][3] = {
        {1.00f, 0.55f, 0.10f},  // Koi - enhanced orange
        {0.00f, 0.50f, 1.00f},  // Tang - deeper blue
        {0.80f, 0.20f, 1.00f},  // Angel - vibrant purple
        {1.00f, 0.30f, 0.60f},  // Clown - coral pink
        {0.16f, 0.34f, 0.78f}   // Species 4 - cobalt blue
    };
    static const float ACCENT[5][3] = {
        {1.00f, 0.95f, 0.30f},  // Koi accent - gold
        {1.00f, 0.95f, 0.20f},  // Tang accent - yellow
        {1.00f, 1.00f, 0.95f},  // Angel accent - white
        {1.00f, 1.00f, 0.95f},  // Clown accent - white
        {0.88f, 0.92f, 0.98f}   // Species 4 accent - silver
    };

    float r = BODY[sp][0], g = BODY[sp][1], b = BODY[sp][2];
    float ar = ACCENT[sp][0], ag = ACCENT[sp][1], ab = ACCENT[sp][2];

    float body[40][2];
    int points = 40;

    for (int i = 0; i < points; i++) {
        float a = (float)i / points * 2.0f * 3.14159f;
        float sx = 1.0f, sy = 0.6f;

        switch(sp){
            case 0: sx = 1.6f; sy = 0.50f + 0.08f * sinf(a * 3); break;
            case 1: sx = 1.3f; sy = 0.75f;  break;
            case 2: sx = 0.9f; sy = 1.4f;  break;
            case 3: sx = 1.1f; sy = 0.80f; break;
            case 4: sx = 1.25f; sy = 0.72f; break;
        }

        body[i][0] = cosf(a) * sz * sx;
        body[i][1] = sinf(a) * sz * sy;
    }

    // Main body
    drawFilledPolygon(body, points, r, g, b, 1.0f);
    
    // Add body shading for 3D effect
    for (int i = 0; i < points; i++) {
        float a = (float)i / points * 2.0f * 3.14159f;
        float sx = 1.0f, sy = 0.6f;
        
        switch(sp){
            case 0: sx = 1.6f; sy = 0.50f + 0.08f * sinf(a * 3); break;
            case 1: sx = 1.3f; sy = 0.75f;  break;
            case 2: sx = 0.9f; sy = 1.4f;  break;
            case 3: sx = 1.1f; sy = 0.80f; break;
            case 4: sx = 1.25f; sy = 0.72f; break;
        }
        
        float shade_sx = sx * 0.6f;
        float shade_sy = sy * 0.6f;
        float shade_x = cosf(a) * sz * shade_sx;
        float shade_y = sinf(a) * sz * shade_sy;
        body[i][0] = shade_x;
        body[i][1] = shade_y;
    }
    drawFilledPolygon(body, points, r*0.6f, g*0.6f, b*0.6f, 0.4f);

    // Koi - fancy tail
    if (sp == 0) {
        float tail2[4][2] = {
            {-sz*1.5f,  0},
            {-sz*2.8f,  sz*1.3f},
            {-sz*2.3f,  0},
            {-sz*2.8f, -sz*1.3f}
        };
        drawFilledPolygon(tail2, 4, ar, ag, ab, 1.0f);
        // Tail shine
        float tail_shine[4][2] = {
            {-sz*1.6f,  0},
            {-sz*2.5f,  sz*0.8f},
            {-sz*2.2f,  0.2f},
            {-sz*2.5f, -sz*0.8f}
        };
        drawFilledPolygon(tail_shine, 4, ar, ag, ab, 0.6f);
    }

    // Tang - detailed pattern
    if (sp == 1) {
        // Stripes
        for(int j = 0; j < 3; j++) {
            float y_off = (j - 1) * sz * 0.35f;
            glColor4f(0.1f, 0.2f, 0.7f, 0.6f);
            glBegin(GL_LINES);
            glVertex2f(-sz*0.8f, y_off);
            glVertex2f(sz*0.6f, y_off);
            glEnd();
        }
        // Eye circle
        glColor4f(0.1f, 0.1f, 0.2f, 0.95f);
        circle(sz*0.5f, sz*0.3f, -sz*0.1f, sz*0.12f, 0, 360);
    } else if (sp == 2) {
        // Angel - vertical stripes pattern
        for(float x = -0.5f; x <= 0.5f; x += 0.35f) {
            float stripe[4][2] = {
                {x*sz,          sz*0.9f},
                {(x+0.12f)*sz,  sz*0.9f},
                {(x+0.12f)*sz, -sz*0.9f},
                {x*sz,         -sz*0.9f}
            };
            drawFilledPolygon(stripe, 4, ar, ag, ab, 0.75f);
        }
    } else if (sp == 3) {
        // Clown - white stripes pattern
        for(int k = -1; k <= 1; k++){
            float stripe[4][2] = {
                {k*sz*0.45f - sz*0.12f, -sz*0.85f},
                {k*sz*0.45f + sz*0.12f, -sz*0.85f},
                {k*sz*0.45f + sz*0.12f,  sz*0.85f},
                {k*sz*0.45f - sz*0.12f,  sz*0.85f}
            };
            drawFilledPolygon(stripe, 4, 1.0f, 1.0f, 1.0f, 0.9f);
        }
    } else if (sp == 4) {
        // Species 4 - banded, spotted design
        float bodyBand[4][2] = {
            {-sz*0.18f, -sz*0.80f},
            { sz*0.10f, -sz*0.80f},
            { sz*0.42f,  sz*0.80f},
            { sz*0.14f,  sz*0.80f}
        };
        drawFilledPolygon(bodyBand, 4, 0.10f, 0.16f, 0.32f, 0.78f);

        float tailBand[4][2] = {
            {-sz*0.66f, -sz*0.62f},
            {-sz*0.48f, -sz*0.62f},
            {-sz*0.48f,  sz*0.62f},
            {-sz*0.66f,  sz*0.62f}
        };
        drawFilledPolygon(tailBand, 4, 0.08f, 0.12f, 0.24f, 0.85f);

        glColor4f(ar, ag, ab, 0.95f);
        circle(sz*0.17f, sz*0.17f, -sz*0.05f, -sz*0.18f, 0, 360);
        circle(sz*0.15f, sz*0.15f,  sz*0.26f,  sz*0.02f,  0, 360);
        circle(sz*0.13f, sz*0.13f, -sz*0.14f,  sz*0.20f,  0, 360);
    }

    // Enhanced tail
    float tail[5][2] = {
        {-sz*0.9f,  0},
        {-sz*1.9f,  sz*0.85f},
        {-sz*1.3f,  0},
        {-sz*1.9f, -sz*0.85f},
        {-sz*0.9f,  0}
    };
    if (sp == 0 || sp == 3) {
        tail[1][0] = -sz*2.3f; tail[1][1] =  sz*1.05f;
        tail[3][0] = -sz*2.3f; tail[3][1] = -sz*1.05f;
    }
    drawFilledPolygon(tail, 5, ar, ag, ab, 1.0f);

    // Dorsal fin - enhanced
    float dFin[4][2] = {
        {-sz*0.2f, -sz*0.45f},
        { sz*0.5f, -sz*0.45f},
        { sz*0.15f, -sz*1.0f},
        {-sz*0.2f, -sz*0.85f}
    };
    drawFilledPolygon(dFin, 4, r*0.8f, g*0.8f, b*0.8f, 1.0f);

    // Gill details
    glColor4f(r*0.7f, g*0.7f, b*0.7f, 0.7f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    for(int i = 0; i < 3; i++) {
        float y_pos = (i - 1) * sz * 0.25f;
        glVertex2f(sz*0.3f, y_pos);
        glVertex2f(sz*0.55f, y_pos + sz*0.1f);
    }
    glEnd();
    glLineWidth(1.0f);

    // Eye - enhanced and more detailed
    // White of eye
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    circle(sz*0.35f, sz*0.35f, sz*0.50f, -sz*0.12f, 0, 360);

    // Iris
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    circle(sz*0.20f, sz*0.20f, sz*0.52f, -sz*0.18f, 0, 360);

    // Pupil highlight
    glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
    circle(sz*0.08f, sz*0.08f, sz*0.58f, -sz*0.22f, 0, 360);
    
    // Extra shine
    glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
    circle(sz*0.04f, sz*0.04f, sz*0.48f, -sz*0.08f, 0, 360);
    
    // Eye outline for definition
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < 20; i++) {
        float angle = i * 3.14159f * 2.0f / 20.0f;
        glVertex2f(sz*0.50f + sz*0.35f*cosf(angle), -sz*0.12f + sz*0.35f*sinf(angle));
    }
    glEnd();
    glLineWidth(1.0f);
}

void drawFish(void){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for(int i = 0; i < fishCount; i++){
        glPushMatrix();
        glTranslatef(fish[i].x, fish[i].y, 0.0f);
        if (fish[i].dir < 0) glScalef(-1.0f, 1.0f, 1.0f);
        glRotatef(sinf(fish[i].phase) * 5.0f, 0.0f, 0.0f, 1.0f);
        drawSingleFish(fish[i].size, fish[i].species);
        glPopMatrix();
    }
    glDisable(GL_BLEND);
}

void initFish(void){
    srand((unsigned int)time(NULL));
    for(int i = 0; i < MAX_FISH; i++){
        fish[i].x       = (float)(rand() % 1200 + 50);
        fish[i].y       = (float)(rand() % 400  + 170);
        fish[i].vx      = ((float)(rand() % 25 + 8) / 10.0f) * (rand() % 2 ? 1.0f : -1.0f);
        fish[i].vy      = ((float)(rand() % 6) - 3) / 10.0f;
        fish[i].size    = (float)(rand() % 9 + 12);
        fish[i].species = rand() % 5;
        fish[i].phase   = (float)(rand() % 628) / 100.0f;
        fish[i].dir     = fish[i].vx > 0 ? 1 : -1;
    }
}

void initBubbles(void){
    for(int i = 0; i < MAX_BUBBLES; i++){
        bubbles[i].x      = 0.0f;
        bubbles[i].y      = 0.0f;
        bubbles[i].vy     = 0.0f;
        bubbles[i].radius = 0.0f;
        bubbles[i].alpha  = 0.0f;
        bubbles[i].alive  = 0;
    }
    bubblesEnabled = 1;
}

void drawBubbles(void){
    if (!bubblesEnabled) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(1.2f);

    for(int i = 0; i < MAX_BUBBLES; i++){
        if (!bubbles[i].alive) continue;
        float cx = bubbles[i].x;
        float cy = bubbles[i].y;
        float al = bubbles[i].alpha;
        int   ri = (int)bubbles[i].radius;
        int   xi = 0, yi = ri, d = 1 - ri;

        glColor4f(0.75f, 0.90f, 1.0f, al);
        glBegin(GL_POINTS);
        while (xi <= yi) {
            glVertex2f(cx+xi,cy+yi); glVertex2f(cx-xi,cy+yi);
            glVertex2f(cx+xi,cy-yi); glVertex2f(cx-xi,cy-yi);
            glVertex2f(cx+yi,cy+xi); glVertex2f(cx-yi,cy+xi);
            glVertex2f(cx+yi,cy-xi); glVertex2f(cx-yi,cy-xi);
            if (d < 0) d += 2*xi+3;
            else { d += 2*(xi-yi)+5; yi--; }
            xi++;
        }
        glEnd();

        //* specular highlight
        glColor4f(1.0f, 1.0f, 1.0f, al * 0.55f);
        circle(bubbles[i].radius * 0.22f,
               bubbles[i].radius * 0.22f,
               cx - bubbles[i].radius * 0.30f,
               cy - bubbles[i].radius * 0.30f,
               0, 360);
    }
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_BLEND);
}


void drawFood(void){
    if (!foodActive) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float size = 6.0f;
    float pts[6][2];
    
    for(int i = 0; i < 6; i++){
        float angle = (i * 60.0f) * 3.14159f / 180.0f;
        pts[i][0] = foodX + size * cosf(angle);
        pts[i][1] = foodY + size * sinf(angle);
    }
    
    glColor4f(0.88f, 0.58f, 0.10f, 1.0f);
    drawFilledPolygon(pts, 6, 0.88f, 0.58f, 0.10f, 1.0f);
    glDisable(GL_BLEND);
}

void releaseBubbles(void){
    bubblesEnabled = 1;
    int i, cnt = 0;
    for(i = 0; i < MAX_BUBBLES && cnt < 10; i++){
        if(bubbles[i].alive) continue;

        bubbles[i].x      = (float)(rand() % 800 + 250);
        bubbles[i].y      = 590.0f;
        bubbles[i].vy     = (float)(rand() % 12 + 5) / 10.0f;
        bubbles[i].radius = (float)(rand() % 4  + 2);
        bubbles[i].alpha  = 0.85f;
        bubbles[i].alive  = 1;
        cnt++;
    }
}

void updateFish(void){
    for(int i = 0; i < fishCount; i++){
        fish[i].phase += 0.11f;

        //* Chase food: gradual pull, not instant jump
        if(foodActive){
            float dx   = foodX - fish[i].x;
            float dy   = foodY - fish[i].y;
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist > 8.0f && dist < 380.0f){
                float pull = 0.20f / (dist / 90.0f + 1.0f);
                fish[i].vx += (dx / dist) * pull;
                fish[i].vy += (dy / dist) * pull;
            }
            if(dist < 14.0f) foodActive = 0;
        }

        //* random water current (small, smooth noise)
        fish[i].vx += ((float)(rand() % 100) - 50) * 0.0014f;
        fish[i].vy += ((float)(rand() % 100) - 50) * 0.0007f;

        //* fish verti
        fish[i].vx *= 0.976f;
        fish[i].vy *= 0.955f;

        //* fish horizontal
        if (fabsf(fish[i].vy) > 0.80f) fish[i].vy *= 0.90f;

        //* ── Speed 
        float spd = sqrtf(fish[i].vx*fish[i].vx + fish[i].vy*fish[i].vy);
        if(spd > 2.8f){
            fish[i].vx = fish[i].vx / spd * 2.8f;
            fish[i].vy = fish[i].vy / spd * 2.8f;
        }

        //* Minimum forward push
        if(spd < 0.25f){
            fish[i].vx += (fish[i].dir > 0 ? 1.0f : -1.0f) * 0.18f;
        }

        fish[i].x += fish[i].vx;
        fish[i].y += fish[i].vy;

        //* fisht boundary
        if(fish[i].x <   42.0f){ 
            fish[i].x=42.0f;   
            fish[i].vx= fabsf(fish[i].vx)*0.75f; 
        }
        if(fish[i].x > 1258.0f){ 
            fish[i].x=1258.0f; 
            fish[i].vx=-fabsf(fish[i].vx)*0.75f; 
        }
        if(fish[i].y <  165.0f){ 
            fish[i].y=165.0f;  
            fish[i].vy= fabsf(fish[i].vy)*0.60f; 
        }
        if(fish[i].y >  585.0f){ 
            fish[i].y=585.0f;  
            fish[i].vy=-fabsf(fish[i].vy)*0.60f; 
        }
        fish[i].dir = fish[i].vx >= 0 ? 1 : -1;
    }
}

//* Update bubble positions
void updateBubbles(void){
    if (!bubblesEnabled) return;
    const float bubbleSurfaceY = 165.0f;
    int aliveCount = 0;
    for(int i = 0; i < MAX_BUBBLES; i++) {
        if (!bubbles[i].alive) continue;

        bubbles[i].y     -= bubbles[i].vy; 
        bubbles[i].x     += sinf(gTime * 1.5f + i) * 0.35f;
        bubbles[i].alpha -= 0.0012f;

        if(bubbles[i].y < bubbleSurfaceY || bubbles[i].alpha <= 0.0f)
            bubbles[i].alive = 0;
        if (bubbles[i].alive) aliveCount++;
    }
    if (aliveCount < 8) releaseBubbles();
}


//! ==================   SEASHELL   ====================
void drawSeashell(void){
    //* shadow
    static const float shadow[][2] = {
        {790.511f,694.704f},{859.327f,699.514f},{859.861f,700.583f},{859.861f,702.053f},
        {859.594f,703.523f},{859.059f,704.725f},{858.124f,705.928f},{857.589f,706.997f},
        {856.520f,708.333f},{854.783f,709.937f},{852.645f,711.406f},{850.641f,712.475f},
        {848.637f,713.411f},{846.232f,713.945f},{843.826f,714.747f},{841.822f,715.281f},
        {839.818f,715.816f},{838.214f,716.484f},{836.343f,716.885f},{834.205f,717.152f},
        {832.468f,717.286f},{830.598f,717.687f},{828.727f,717.954f},{827.257f,718.221f},
        {825.921f,718.622f},{824.050f,718.889f},{822.313f,719.424f},{820.576f,719.958f},
        {818.705f,720.760f},{816.701f,721.294f},{814.964f,721.829f},{813.628f,722.497f},
        {812.024f,722.898f},{810.287f,723.432f},{808.015f,723.299f},{806.813f,723.032f},
        {805.343f,722.363f},{804.274f,721.829f},{802.804f,721.161f},{801.468f,720.225f},
        {800.265f,718.889f},{799.196f,717.954f},{798.261f,716.350f},{796.791f,715.148f},
        {795.722f,714.212f},{794.920f,713.277f},{793.985f,712.208f},{793.050f,711.540f},
        {792.248f,710.738f},{791.179f,709.669f},{790.244f,708.868f},{789.308f,708.333f},
        {788.106f,707.932f},{787.037f,707.398f},{785.834f,706.730f},{784.632f,706.061f},
        {783.563f,705.393f},{782.627f,704.859f},{781.425f,704.057f},{780.623f,703.523f},
        {779.955f,702.988f},{779.287f,702.454f},{779.019f,701.652f},{778.752f,700.850f},
        {778.752f,699.781f},{778.752f,698.979f},{779.153f,698.044f},{779.420f,697.376f},
        {779.955f,696.842f},{780.623f,696.307f},{781.291f,696.173f},{782.226f,695.906f},
        {783.429f,695.773f},{784.632f,695.639f},{785.567f,695.639f},{786.502f,695.505f},
        {788.106f,695.104f},{788.907f,694.971f},{790.244f,694.704f}
    };

    //* outer shell body
    static const float body[][2] = {
        {833.671f,667.845f},{835.943f,667.845f},{838.348f,667.845f},{840.753f,668.113f},
        {842.624f,668.914f},{845.029f,669.850f},{847.033f,671.052f},{848.236f,672.121f},
        {849.171f,673.190f},{849.305f,674.259f},{849.438f,675.328f},{850.507f,675.462f},
        {851.576f,675.729f},{852.512f,676.264f},{853.046f,676.932f},{853.848f,678.001f},
        {855.051f,679.203f},{855.719f,680.406f},{856.788f,682.143f},{857.322f,684.014f},
        {857.723f,685.751f},{857.322f,687.087f},{858.124f,688.156f},{859.059f,689.225f},
        {859.861f,690.294f},{860.262f,691.897f},{860.529f,692.966f},{860.529f,694.837f},
        {860.930f,696.574f},{860.262f,697.643f},{859.327f,699.380f},{858.391f,700.850f},
        {857.189f,702.454f},{855.852f,703.924f},{854.650f,705.126f},{853.447f,706.195f},
        {851.977f,707.264f},{849.973f,708.066f},{848.369f,709.001f},{846.098f,709.803f},
        {844.094f,709.803f},{841.688f,709.803f},{838.481f,709.937f},{835.141f,710.204f},
        {831.132f,710.204f},{827.391f,710.204f},{824.852f,710.070f},{824.050f,710.070f},
        {821.912f,710.070f},{820.442f,710.070f},{819.240f,710.070f},{817.770f,710.204f},
        {817.503f,710.204f},{817.503f,711.273f},{817.503f,712.208f},{817.369f,713.544f},
        {816.968f,714.346f},{816.434f,715.281f},{815.632f,716.217f},{814.563f,717.553f},
        {813.360f,718.756f},{812.024f,719.557f},{810.955f,719.825f},{809.352f,719.958f},
        {807.615f,719.156f},{805.878f,718.088f},{804.274f,717.019f},{802.537f,715.816f},
        {801.201f,714.881f},{800.533f,713.812f},{799.330f,712.074f},{798.395f,711.273f},
        {797.192f,710.872f},{795.455f,710.738f},{794.252f,710.337f},{793.718f,709.937f},
        {792.649f,709.669f},{791.446f,709.001f},{789.709f,708.066f},{788.106f,707.665f},
        {786.235f,706.462f},{784.632f,705.527f},{783.429f,704.592f},{781.692f,703.122f},
        {781.425f,701.652f},{782.093f,700.048f},{783.028f,698.445f},{784.632f,697.777f},
        {786.502f,696.842f},{788.373f,696.307f},{789.576f,696.173f},{790.645f,695.773f},
        {791.045f,694.704f},{791.045f,693.635f},{791.045f,692.298f},{791.045f,690.962f},
        {791.045f,689.225f},{791.045f,688.022f},{790.912f,685.751f},{791.179f,683.747f},
        {791.981f,681.074f},{792.782f,679.203f},{793.718f,677.199f},{794.787f,675.328f},
        {796.257f,673.591f},{797.994f,671.720f},{799.998f,670.384f},{801.735f,669.048f},
        {803.472f,668.380f},{805.610f,667.177f},{807.615f,666.242f},{809.218f,665.841f},
        {810.554f,665.574f},{812.158f,665.307f},{813.494f,665.173f},{814.830f,665.173f},
        {816.166f,665.307f},{816.567f,665.707f},{816.701f,665.975f},{817.235f,666.242f},
        {818.037f,665.841f},{819.240f,665.440f},{820.442f,665.039f},{822.046f,664.638f},
        {823.248f,664.505f},{824.585f,664.505f},{826.188f,664.505f},{827.524f,664.505f},
        {828.861f,664.638f},{830.063f,664.772f},{830.865f,665.173f},{831.399f,665.707f},
        {832.201f,666.108f},{832.335f,666.643f},{832.736f,667.044f},{833.270f,667.712f}
    };

    //* left hinge
    static const float leftHinge[][2] = {
        {790.645f,697.510f},{790.778f,698.445f},{790.912f,699.514f},{791.179f,700.850f},
        {791.580f,702.053f},{791.981f,702.855f},{792.515f,703.790f},{793.183f,704.725f},
        {793.584f,705.527f},{793.851f,706.195f},{794.252f,706.863f},{794.520f,707.398f},
        {793.851f,707.531f},{793.451f,707.130f},{792.649f,706.596f},{791.981f,706.195f},
        {791.313f,705.661f},{790.377f,705.260f},{789.308f,704.458f},{788.106f,703.790f},
        {786.769f,702.855f},{785.701f,702.053f},{785.032f,701.385f},{784.899f,700.583f},
        {785.166f,700.048f},{785.701f,699.514f},{786.369f,698.979f},{787.037f,698.579f},
        {787.705f,698.178f},{788.507f,698.044f},{789.308f,697.777f},{789.709f,697.643f},
        {790.110f,697.510f},{790.645f,697.376f}
    };

    //* right lip
    static const float rightLip[][2] = {
        {801.735f,710.872f},{803.472f,710.738f},{804.942f,710.872f},{806.145f,710.738f},
        {807.347f,710.738f},{808.550f,710.605f},{809.753f,710.605f},{810.688f,710.337f},
        {811.757f,710.204f},{812.826f,710.070f},{813.628f,709.937f},{814.563f,709.803f},
        {815.231f,709.937f},{815.766f,710.070f},{816.033f,710.605f},{815.632f,711.406f},
        {815.365f,712.475f},{814.964f,713.277f},{814.429f,714.212f},{813.628f,715.148f},
        {813.093f,715.816f},{812.559f,716.350f},{811.891f,716.751f},{811.089f,717.019f},
        {810.020f,716.751f},{809.218f,716.350f},{808.283f,715.816f},{807.347f,715.281f},
        {806.145f,714.480f},{804.942f,713.812f},{804.140f,713.143f},{803.071f,712.342f},
        {801.735f,711.006f}
    };

    //* ridge fan lines
    static const float ridges[][2] = {
        {798.796f,708.199f},{797.727f,707.130f},{796.658f,706.195f},{795.722f,704.859f},
        {794.653f,703.656f},{794.252f,702.053f},{793.584f,699.915f},{793.317f,697.910f},
        {793.183f,695.773f},{793.050f,693.501f},{793.050f,691.363f},{793.317f,688.958f},
        {793.718f,686.553f},{794.787f,682.678f},{795.321f,681.208f},{796.123f,679.604f},
        {796.925f,678.268f},{798.261f,676.531f},{799.597f,675.061f},{800.399f,673.858f},
        {801.869f,672.789f},{802.671f,671.720f},{803.740f,671.186f},{805.076f,670.251f},
        {806.412f,669.583f},{807.615f,668.914f},{808.817f,668.246f},{810.287f,667.712f},
        {811.623f,667.712f},{812.692f,667.712f},{814.162f,667.845f},{814.028f,668.781f},
        {813.093f,669.850f},{811.891f,670.652f},{810.421f,671.854f},{809.084f,673.057f},
        {807.481f,674.527f},{806.145f,675.863f},{804.942f,677.333f},{803.472f,679.070f},
        {802.002f,680.673f},{801.067f,682.009f},{800.265f,683.747f},{799.464f,685.484f},
        {799.063f,686.820f},{798.395f,688.557f},{797.994f,690.294f},{797.727f,691.229f},
        {797.459f,692.966f},{797.326f,694.570f},{797.326f,696.040f},{797.459f,697.376f},
        {797.727f,696.441f},{797.727f,695.238f},{797.860f,694.570f},{798.127f,693.635f},
        {798.395f,692.432f},{798.528f,691.497f},{799.063f,690.428f},{799.330f,689.225f},
        {799.864f,688.022f},{800.533f,687.087f},{801.067f,686.018f},{801.735f,684.815f},
        {802.270f,683.747f},{803.339f,682.811f},{803.740f,681.742f},{804.809f,680.673f},
        {805.878f,679.337f},{806.946f,678.134f},{807.882f,677.065f},{808.550f,676.264f},
        {809.485f,675.195f},{810.554f,674.259f},{811.757f,673.190f},{812.692f,672.255f},
        {813.895f,671.186f},{814.964f,670.384f},{815.766f,669.850f},{816.968f,668.781f},
        {818.171f,668.113f},{819.507f,667.445f},{820.843f,667.044f},{822.046f,666.910f},
        {823.248f,666.776f},{824.451f,666.643f},{825.520f,666.776f},{826.723f,667.044f},
        {827.658f,667.311f},{828.727f,667.712f},{829.796f,668.246f},{829.662f,668.781f},
        {829.128f,669.315f},{827.925f,669.983f},{826.322f,670.785f},{824.852f,671.186f},
        {823.382f,672.121f},{821.912f,672.789f},{820.843f,673.591f},{819.774f,674.393f},
        {818.572f,675.195f},{817.636f,676.130f},{816.166f,677.466f},{815.097f,678.402f},
        {814.028f,679.203f},{812.826f,680.406f},{811.490f,681.742f},{810.153f,683.078f},
        {808.817f,684.415f},{807.615f,686.018f},{806.145f,687.755f},{804.942f,689.225f},
        {803.740f,690.962f},{802.938f,692.432f},{801.869f,693.902f},{801.067f,695.372f},
        {800.666f,696.574f},{800.533f,697.376f},{801.602f,696.307f},{802.136f,695.372f},
        {802.671f,694.436f},{803.472f,693.501f},{804.140f,692.298f},{804.942f,691.229f},
        {805.878f,690.027f},{806.813f,689.225f},{807.748f,688.156f},{808.817f,686.820f},
        {810.020f,685.751f},{810.955f,684.682f},{812.024f,683.747f},{813.093f,682.277f},
        {814.162f,681.208f},{815.632f,680.272f},{816.567f,679.337f},{817.770f,678.535f},
        {818.705f,677.733f},{819.641f,677.065f},{820.710f,676.264f},{821.511f,675.596f},
        {822.580f,674.794f},{823.382f,674.259f},{824.585f,673.591f},{825.654f,673.057f},
        {826.990f,672.389f},{828.460f,671.720f},{829.930f,671.320f},{831.800f,670.785f},
        {833.137f,670.384f},{834.339f,669.983f},{835.408f,669.850f},{836.744f,669.850f},
        {837.947f,669.850f},{839.550f,670.117f},{841.020f,670.652f},{842.223f,670.919f},
        {843.425f,671.453f},{844.361f,671.988f},{845.296f,672.789f},{846.232f,673.458f},
        {847.033f,673.992f},{846.766f,674.527f},{845.964f,674.794f},{844.895f,674.927f},
        {843.425f,675.061f},{842.356f,675.462f},{839.951f,675.863f},{838.081f,676.264f},
        {836.477f,676.798f},{835.141f,677.199f},{833.671f,678.001f},{832.602f,678.134f},
        {831.132f,678.802f},{829.395f,679.604f},{827.792f,680.139f},{826.055f,681.208f},
        {824.050f,682.009f},{822.714f,683.078f},{821.378f,683.747f},{820.442f,684.415f},
        {819.106f,685.484f},{817.770f,686.419f},{816.701f,687.354f},{815.498f,688.290f},
        {814.429f,689.359f},{813.093f,690.428f},{811.891f,691.630f},{810.421f,692.966f},
        {809.218f,693.902f},{808.015f,695.238f},{807.080f,696.441f},{806.011f,697.777f},
        {807.481f,697.242f},{808.817f,696.441f},{809.753f,695.639f},{810.822f,694.704f},
        {811.891f,693.635f},{812.960f,692.566f},{814.296f,691.764f},{815.498f,690.962f},
        {816.701f,690.027f},{817.770f,689.225f},{818.973f,688.557f},{820.710f,687.488f},
        {821.511f,686.820f},{822.580f,686.018f},{823.382f,685.484f},{824.718f,684.815f},
        {825.921f,684.014f},{827.257f,683.479f},{828.593f,682.811f},{829.796f,682.009f},
        {831.533f,681.475f},{833.404f,680.940f},{835.007f,680.139f},{836.744f,679.471f},
        {838.081f,679.070f},{839.684f,678.669f},{841.287f,678.268f},{842.891f,678.268f},
        {844.361f,678.134f},{845.563f,678.001f},{846.632f,677.867f},{847.701f,678.001f},
        {849.038f,678.268f},{850.374f,679.070f},{851.176f,679.738f},{851.844f,680.406f},
        {852.779f,681.475f},{853.581f,682.544f},{854.382f,683.613f},{855.051f,684.815f},
        {855.318f,685.484f},{853.982f,685.617f},{852.378f,685.884f},{850.507f,686.018f},
        {849.038f,686.152f},{846.900f,686.419f},{845.163f,686.686f},{843.158f,687.087f},
        {840.486f,687.221f},{838.481f,687.622f},{836.611f,688.022f},{834.606f,688.557f},
        {832.602f,688.958f},{830.598f,689.626f},{828.727f,690.160f},{826.990f,690.962f},
        {825.119f,691.764f},{823.783f,692.298f},{822.313f,692.966f},{820.977f,693.768f},
        {819.774f,694.303f},{818.171f,695.238f},{816.701f,696.307f},{814.964f,697.376f},
        {813.494f,698.178f},{812.425f,698.979f},{811.089f,699.781f},{809.753f,700.984f},
        {808.817f,701.786f},{807.615f,702.988f},{809.084f,702.454f},{810.153f,701.652f},
        {811.089f,701.117f},{812.158f,700.449f},{813.360f,699.781f},{814.296f,699.113f},
        {815.365f,698.445f},{816.434f,697.910f},{817.369f,697.376f},{818.304f,696.708f},
        {819.507f,696.173f},{820.309f,695.639f},{821.110f,695.238f},{821.912f,694.837f},
        {823.248f,694.303f},{824.317f,693.902f},{825.520f,693.501f},{826.856f,692.966f},
        {828.059f,692.566f},{829.261f,692.165f},{830.598f,691.630f},{831.934f,691.096f},
        {833.270f,690.829f},{834.339f,690.428f},{835.943f,690.027f},{837.279f,690.027f},
        {838.348f,689.626f},{839.684f,689.225f},{841.154f,688.824f},{842.490f,688.691f},
        {844.094f,688.557f},{845.430f,688.423f},{846.632f,688.290f},{848.102f,688.022f},
        {849.706f,687.755f},{851.042f,687.622f},{852.245f,687.622f},{853.180f,687.755f},
        {853.982f,688.022f},{854.917f,688.958f},{855.719f,690.027f},{856.253f,690.962f},
        {856.788f,692.031f},{857.055f,693.234f},{857.055f,694.169f},{857.322f,694.971f},
        {857.322f,696.307f},{857.189f,697.510f},{855.585f,697.643f},{854.249f,697.510f},
        {852.512f,697.242f},{851.176f,697.376f},{849.305f,697.242f},{847.969f,697.376f},
        {846.232f,697.376f},{845.163f,697.376f},{843.826f,697.510f},{842.223f,697.510f},
        {840.887f,697.643f},{839.417f,697.643f},{837.813f,697.777f},{836.477f,697.777f},
        {835.007f,697.910f},{833.671f,698.044f},{832.068f,698.178f},{830.999f,698.445f},
        {829.796f,698.579f},{828.460f,698.712f},{827.123f,698.979f},{825.520f,699.380f},
        {823.917f,699.781f},{821.912f,700.316f},{820.576f,700.850f},{818.973f,701.117f},
        {817.904f,701.652f},{816.567f,701.919f},{815.097f,702.454f},{814.028f,702.855f},
        {813.093f,703.255f},{812.291f,703.523f},{811.490f,704.057f},{810.554f,704.592f},
        {810.020f,705.126f},{810.955f,704.859f},{811.757f,704.592f},{812.692f,704.191f},
        {813.494f,703.924f},{814.296f,703.656f},{815.097f,703.255f},{816.033f,702.988f},
        {816.968f,702.587f},{817.770f,702.454f},{818.705f,702.053f},{819.774f,701.919f},
        {820.843f,701.518f},{822.046f,701.251f},{823.382f,700.984f},{824.451f,700.583f},
        {825.921f,700.316f},{827.257f,700.048f},{828.727f,699.648f},{829.930f,699.648f},
        {831.399f,699.380f},{832.335f,699.380f},{833.270f,699.514f},{834.473f,699.380f},
        {835.675f,699.247f},{836.477f,698.979f},{837.680f,698.979f},{838.882f,698.979f},
        {840.085f,698.979f},{841.287f,698.846f},{842.490f,698.846f},{843.559f,698.846f},
        {844.762f,698.846f},{846.098f,698.712f},{847.434f,698.846f},{848.904f,698.712f},
        {850.107f,698.712f},{851.309f,698.712f},{852.645f,698.712f},{853.714f,698.712f},
        {854.783f,698.712f},{855.986f,698.979f},{855.986f,700.048f},{854.917f,701.518f},
        {854.249f,702.320f},{853.314f,703.389f},{851.977f,704.324f},{851.042f,704.992f},
        {849.973f,705.794f},{848.503f,706.329f},{847.568f,706.462f},{846.632f,706.730f},
        {845.163f,707.130f},{843.559f,707.398f},{842.223f,707.398f},{840.887f,707.398f},
        {839.684f,707.398f},{838.214f,707.264f},{836.744f,707.264f},{835.274f,707.264f},
        {833.805f,706.997f},{832.468f,706.997f},{830.999f,706.863f},{829.529f,706.863f},
        {828.059f,706.730f},{826.589f,706.730f},{825.654f,706.730f},{824.585f,706.730f},
        {823.649f,706.730f},{822.580f,706.863f},{821.511f,706.730f},{820.041f,706.863f},
        {818.572f,706.863f},{817.503f,706.462f},{815.899f,706.462f},{814.964f,706.596f},
        {814.028f,706.730f},{812.960f,706.863f},{812.024f,707.130f},{811.089f,707.264f},
        {810.020f,707.665f},{809.084f,707.932f},{807.882f,708.333f},{806.813f,708.734f},
        {806.011f,708.868f},{804.942f,708.868f},{803.606f,709.001f},{802.671f,709.135f},
        {801.735f,709.135f},{800.800f,709.135f},{799.864f,709.135f},{798.796f,708.199f}
    };

    //* shiny highlight ellipse
    int i;
    int nShadow   = (int)(sizeof(shadow)    / sizeof(shadow[0]));
    int nBody     = (int)(sizeof(body)      / sizeof(body[0]));
    int nHinge    = (int)(sizeof(leftHinge) / sizeof(leftHinge[0]));
    int nLip      = (int)(sizeof(rightLip)  / sizeof(rightLip[0]));
    int nRidges   = (int)(sizeof(ridges)    / sizeof(ridges[0]));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    drawFilledPolygon(shadow,    nShadow, 0.45f, 0.30f, 0.25f, 0.65f);
    drawFilledPolygon(body,      nBody,   1.00f, 0.88f, 0.84f, 1.00f);
    drawFilledPolygon(body,      nBody,   1.00f, 0.70f, 0.75f, 0.35f);
    drawFilledPolygon(leftHinge, nHinge,  0.95f, 0.65f, 0.60f, 0.80f);
    drawFilledPolygon(rightLip,  nLip,    0.95f, 0.65f, 0.60f, 0.80f);
    drawOutlineStrip(ridges, nRidges,     0.85f, 0.45f, 0.50f, 0.75f, 1.4f);

    //* highlight ellipse
    {
        float pts[32][2];
        float cx = 840.0f, cy = 680.0f, rx = 12.0f, ry = 8.0f;
        int seg = 32;
        for(i = 0; i < seg; i++){
            float a = (float)i / (float)seg * 2.0f * 3.14159265f;
            pts[i][0] = cx + rx * cosf(a);
            pts[i][1] = cy + ry * sinf(a);
        }
        drawFilledPolygon(pts, seg, 1.0f, 1.0f, 1.0f, 0.25f);
    }

    drawOutlineLoop(body, nBody, 0.75f, 0.35f, 0.40f, 0.90f, 2.0f);
    glDisable(GL_BLEND);
}


//! ================     STARFISH        ================

void drawStarfish(void){
    //* --- outer body 
    static const float outerBody[][2] = {
        {479.037f,686.686f},{481.175f,687.755f},{484.114f,688.824f},{486.520f,689.359f},
        {489.192f,690.428f},{492.132f,691.897f},{495.071f,692.699f},{498.011f,694.169f},
        {500.817f,695.372f},{502.955f,696.307f},{504.960f,697.376f},{506.697f,697.910f},
        {508.567f,699.247f},{510.705f,700.583f},{511.507f,702.454f},{511.373f,704.458f},
        {510.705f,705.928f},{508.835f,706.997f},{507.231f,707.799f},{505.628f,708.333f},
        {504.024f,708.600f},{502.287f,709.268f},{500.416f,709.669f},{498.546f,710.070f},
        {495.606f,710.204f},{493.334f,710.605f},{490.796f,710.471f},{487.989f,710.738f},
        {484.515f,710.738f},{481.041f,710.605f},{478.235f,710.872f},{475.696f,710.872f},
        {473.425f,711.006f},{473.291f,714.881f},{473.558f,720.225f},{474.494f,726.238f},
        {474.494f,728.911f},{474.494f,732.118f},{474.494f,735.859f},{474.226f,740.002f},
        {473.558f,743.476f},{472.489f,744.946f},{470.752f,745.881f},{468.748f,745.480f},
        {466.610f,744.411f},{464.472f,742.941f},{461.265f,740.135f},{458.192f,736.394f},
        {455.252f,733.721f},{453.381f,730.514f},{451.377f,727.842f},{449.506f,725.570f},
        {447.769f,723.032f},{446.567f,721.027f},{445.498f,719.825f},{444.161f,717.954f},
        {443.360f,716.885f},{442.291f,716.217f},{441.622f,716.618f},{440.687f,716.751f},
        {439.351f,717.152f},{437.614f,717.687f},{434.941f,718.221f},{431.734f,719.156f},
        {429.463f,719.691f},{427.058f,720.225f},{424.385f,721.027f},{420.644f,721.562f},
        {417.437f,722.230f},{413.695f,723.032f},{410.889f,723.566f},{407.415f,724.368f},
        {401.001f,724.769f},{398.730f,724.902f},{397.260f,724.769f},{396.592f,724.234f},
        {396.324f,723.566f},{396.324f,722.497f},{396.592f,721.428f},{396.725f,720.894f},
        {397.126f,719.825f},{398.062f,718.756f},{399.264f,717.152f},{400.868f,715.415f},
        {402.070f,713.678f},{403.674f,711.941f},{405.411f,710.337f},{407.148f,709.001f},
        {409.687f,706.596f},{412.226f,704.859f},{414.764f,702.988f},{416.902f,701.117f},
        {419.040f,699.781f},{420.644f,698.712f},{421.178f,698.044f},{421.980f,697.643f},
        {422.782f,696.842f},{423.049f,696.307f},{423.049f,695.906f},{422.782f,695.104f},
        {422.381f,694.704f},{421.980f,694.303f},{421.178f,693.768f},{420.243f,693.100f},
        {419.040f,692.031f},{417.570f,690.962f},{416.234f,690.027f},{415.032f,689.359f},
        {412.626f,688.022f},{410.889f,686.553f},{409.152f,685.484f},{407.816f,684.548f},
        {406.480f,683.479f},{405.678f,682.945f},{405.144f,682.143f},{404.609f,680.940f},
        {404.075f,679.738f},{404.075f,678.669f},{404.342f,677.733f},{404.743f,677.199f},
        {405.544f,676.531f},{406.213f,676.130f},{407.281f,675.863f},{408.484f,675.729f},
        {410.221f,675.462f},{411.290f,675.462f},{412.760f,675.462f},{414.898f,675.328f},
        {417.570f,675.462f},{419.708f,675.462f},{422.514f,675.462f},{425.454f,675.462f},
        {427.592f,675.729f},{430.933f,676.130f},{434.540f,676.798f},{437.614f,677.199f},
        {440.420f,678.001f},{443.226f,678.936f},{444.696f,679.203f},{445.498f,679.604f},
        {446.567f,679.604f},{447.502f,679.203f},{448.571f,678.936f},{449.506f,678.402f},
        {450.976f,677.733f},{452.580f,676.798f},{453.782f,676.397f},{455.519f,674.927f},
        {457.524f,673.858f},{460.330f,672.389f},{463.136f,670.785f},{465.675f,669.583f},
        {467.679f,668.113f},{470.084f,666.910f},{471.955f,666.242f},{473.558f,665.841f},
        {474.894f,665.173f},{476.097f,664.772f},{477.433f,664.371f},{478.369f,664.238f},
        {479.571f,663.970f},{480.774f,663.837f},{481.576f,663.837f},{482.912f,663.703f},
        {484.114f,664.238f},{485.317f,664.505f},{485.584f,665.307f},{485.584f,665.975f},
        {485.584f,666.910f},{485.718f,668.113f},{486.119f,669.449f},{485.852f,670.251f},
        {485.584f,671.186f},{485.183f,672.255f},{484.248f,674.259f},{483.313f,676.665f},
        {482.110f,678.802f},{481.041f,680.807f},{480.507f,681.876f},{479.972f,682.945f},
        {479.438f,684.014f},{478.903f,684.815f},{478.636f,685.751f},{478.903f,686.686f}
    };

    //* --- inner dark body
    static const float innerBody[][2] = {
        {409.553f,679.738f},{413.829f,680.005f},{418.372f,680.540f},{423.316f,681.074f},
        {429.196f,682.277f},{434.140f,682.277f},{438.549f,683.346f},{443.092f,684.147f},
        {446.433f,684.548f},{448.704f,684.548f},{451.243f,684.281f},{453.114f,683.613f},
        {454.851f,682.945f},{456.989f,682.009f},{459.261f,680.139f},{461.799f,678.802f},
        {464.338f,676.798f},{466.744f,675.328f},{468.748f,673.992f},{471.153f,672.389f},
        {473.558f,670.919f},{475.696f,669.850f},{477.567f,669.048f},{479.037f,668.246f},
        {480.373f,668.113f},{481.308f,668.113f},{482.110f,668.246f},{482.645f,669.048f},
        {482.511f,670.251f},{481.709f,672.255f},{480.507f,674.660f},{479.705f,676.798f},
        {478.770f,678.669f},{477.834f,681.074f},{476.899f,683.479f},{475.963f,686.018f},
        {476.097f,687.755f},{477.567f,689.225f},{480.239f,690.294f},{482.778f,690.962f},
        {485.050f,691.764f},{487.589f,692.833f},{489.994f,693.635f},{492.934f,694.570f},
        {495.873f,695.773f},{498.011f,696.708f},{500.283f,697.777f},{501.619f,698.579f},
        {501.886f,699.247f},{500.951f,700.316f},{499.882f,700.984f},{498.546f,701.117f},
        {496.675f,701.251f},{494.537f,701.518f},{491.998f,701.518f},{488.925f,701.786f},
        {485.852f,701.786f},{482.645f,702.053f},{480.239f,702.320f},{476.632f,702.320f},
        {473.692f,702.454f},{471.420f,702.454f},{469.817f,702.587f},{471.554f,735.859f},
        {471.554f,737.196f},{470.886f,737.730f},{469.817f,737.997f},{468.347f,736.928f},
        {466.877f,734.657f},{464.338f,730.915f},{461.799f,726.639f},{459.662f,722.363f},
        {458.192f,719.156f},{456.855f,716.350f},{455.920f,714.346f},{455.118f,713.010f},
        {454.049f,711.540f},{452.713f,709.937f},{451.911f,709.135f},{450.976f,707.531f},
        {450.041f,706.863f},{449.239f,706.195f},{448.304f,705.794f},{446.700f,705.393f},
        {445.364f,705.260f},{443.226f,705.126f},{440.420f,705.393f},{438.282f,705.661f},
        {436.010f,706.329f},{434.006f,706.730f},{431.334f,707.531f},{428.527f,708.600f},
        {426.122f,709.937f},{423.583f,711.006f},{421.312f,712.074f},{418.773f,713.277f},
        {416.635f,713.945f},{414.230f,714.881f},{411.958f,715.281f},{409.553f,715.950f},
        {407.950f,716.217f},{406.613f,716.350f},{405.277f,716.350f},{404.342f,716.350f},
        {407.950f,712.876f},{411.958f,708.868f},{414.898f,706.462f},{418.639f,703.656f},
        {423.183f,699.915f},{426.390f,697.777f},{428.394f,696.441f},{429.596f,695.104f},
        {430.665f,694.303f},{430.933f,692.966f},{430.131f,692.031f},{429.196f,691.229f},
        {427.993f,690.160f},{427.191f,689.225f},{425.989f,688.156f},{424.786f,687.488f},
        {423.984f,686.820f},{423.049f,685.751f},{421.045f,684.682f},{419.575f,684.014f},
        {418.372f,683.613f},{416.902f,682.811f},{415.165f,682.143f},{413.829f,681.742f},
        {412.626f,681.074f},{411.424f,680.673f},{409.687f,680.005f}
    };

    //* --- top highlight
    static const float topHighlight[][2] = {
        {407.281f,678.001f},{411.958f,677.867f},{416.902f,677.733f},{421.445f,677.867f},
        {426.390f,678.402f},{430.799f,678.802f},{434.407f,679.471f},{437.881f,680.139f},
        {441.355f,680.272f},{443.493f,680.807f},{445.364f,681.074f},{446.967f,681.208f},
        {448.304f,681.208f},{450.041f,680.540f},{452.446f,679.738f},{454.317f,678.802f},
        {456.722f,677.600f},{458.993f,676.130f},{461.131f,674.927f},{463.804f,673.324f},
        {466.075f,671.854f},{467.946f,670.919f},{469.550f,669.983f},{471.554f,669.048f},
        {473.558f,667.979f},{475.563f,667.311f},{477.300f,666.910f},{478.636f,666.376f},
        {479.972f,666.376f},{481.442f,666.242f},{482.778f,666.376f},{483.446f,666.643f},
        {483.179f,667.311f},{481.843f,667.845f},{479.839f,668.246f},{477.032f,669.182f},
        {474.360f,671.052f},{471.955f,672.923f},{469.015f,674.527f},{467.679f,675.596f},
        {465.808f,677.065f},{463.002f,678.802f},{460.196f,680.272f},{457.390f,681.876f},
        {455.786f,683.212f},{454.317f,684.014f},{452.847f,684.682f},{451.243f,684.949f},
        {449.640f,685.216f},{447.368f,685.083f},{445.898f,684.815f},{443.092f,684.415f},
        {441.622f,684.014f},{440.019f,683.613f},{438.148f,683.346f},{435.209f,682.811f},
        {433.872f,682.410f},{431.868f,682.143f},{430.265f,681.876f},{428.661f,681.609f},
        {427.191f,681.341f},{425.855f,680.940f},{424.118f,680.673f},{422.648f,680.406f},
        {420.911f,680.139f},{418.907f,679.871f},{416.635f,679.738f},{414.631f,679.337f},
        {412.894f,679.337f},{411.557f,679.070f},{410.088f,679.070f},{408.885f,678.936f},
        {407.816f,678.802f},{407.148f,678.535f},{407.281f,678.001f}
    };

    //* --- right shadow
    static const float rightShadow[][2] = {
        {474.137f,711.206f},{478.814f,711.206f},{481.932f,711.095f},{485.161f,711.206f},
        {487.722f,710.872f},{490.951f,710.649f},{493.401f,710.315f},{495.294f,710.315f},
        {497.187f,709.758f},{500.082f,709.313f},{502.087f,709.202f},{504.091f,708.979f},
        {505.984f,708.533f},{507.432f,707.977f},{508.434f,707.643f},{509.547f,707.309f},
        {510.104f,707.086f},{510.549f,706.418f},{509.993f,708.088f},{509.102f,708.979f},
        {508.322f,709.758f},{507.209f,710.872f},{506.318f,711.429f},{504.870f,712.319f},
        {503.757f,712.765f},{502.198f,712.988f},{500.416f,713.433f},{499.192f,713.544f},
        {498.301f,713.878f},{496.964f,714.101f},{494.960f,714.658f},{493.401f,714.769f},
        {491.620f,714.881f},{489.838f,715.103f},{488.056f,715.215f},{486.386f,715.549f},
        {485.273f,715.660f},{483.714f,715.771f},{481.932f,715.883f},{480.373f,715.883f},
        {478.369f,716.105f},{477.032f,716.328f},{475.474f,716.328f},{474.360f,716.551f},
        {474.026f,711.206f}
    };

    //* --- bottom-left shadow
    static const float blShadow[][2] = {
        {442.847f,717.553f},{445.743f,721.116f},{447.413f,723.566f},{449.083f,726.127f},
        {450.753f,728.577f},{452.758f,731.249f},{454.985f,733.922f},{457.323f,736.149f},
        {459.105f,738.376f},{461.220f,740.269f},{463.002f,741.939f},{465.229f,743.498f},
        {467.011f,744.834f},{468.792f,745.391f},{469.572f,745.502f},{470.240f,745.725f},
        {471.019f,745.836f},{471.019f,746.059f},{470.797f,746.393f},{469.795f,746.393f},
        {468.681f,746.505f},{467.902f,746.505f},{467.011f,746.282f},{466.120f,745.948f},
        {465.229f,745.725f},{464.450f,745.168f},{463.559f,745.057f},{462.445f,744.612f},
        {461.443f,744.055f},{460.330f,743.275f},{459.550f,742.719f},{458.437f,742.273f},
        {457.212f,741.716f},{455.430f,740.603f},{453.649f,739.489f},{452.758f,738.487f},
        {451.310f,737.040f},{449.640f,735.481f},{448.192f,733.810f},{446.522f,732.140f},
        {445.074f,730.581f},{443.181f,728.911f},{442.402f,727.797f},{441.288f,726.907f},
        {440.843f,726.350f},{440.175f,725.570f},{439.841f,725.459f},{439.618f,725.125f},
        {439.395f,725.014f},{438.950f,724.345f},{438.393f,724.011f},{438.059f,723.677f},
        {437.502f,723.566f},{437.168f,723.566f},{436.723f,723.566f},{435.832f,723.566f},
        {434.941f,723.566f},{433.494f,723.566f},{432.603f,723.789f},{431.712f,723.789f},
        {430.265f,724.011f},{429.040f,724.123f},{427.592f,724.345f},{426.033f,724.791f},
        {424.363f,725.125f},{422.915f,725.125f},{420.911f,725.459f},{419.352f,725.570f},
        {418.127f,725.904f},{415.789f,726.350f},{413.896f,726.573f},{412.226f,726.795f},
        {410.889f,726.684f},{410.110f,726.795f},{408.662f,727.018f},{407.326f,727.352f},
        {405.878f,727.352f},{404.320f,727.686f},{402.427f,727.686f},{400.868f,727.909f},
        {399.420f,727.797f},{398.084f,728.020f},{396.748f,728.131f},{395.745f,728.131f},
        {394.521f,728.131f},{393.852f,728.131f},{393.073f,728.131f},{392.516f,727.686f},
        {391.737f,727.463f},{390.957f,727.018f},{390.289f,726.684f},{389.955f,726.350f},
        {389.955f,725.682f},{389.732f,724.791f},{389.732f,723.677f},{389.955f,722.787f},
        {389.955f,722.007f},{390.401f,721.005f},{390.846f,720.114f},{392.182f,718.889f},
        {393.073f,717.776f},{394.075f,716.996f},{394.966f,715.994f},{396.191f,714.769f},
        {397.082f,713.990f},{397.861f,713.099f},{399.197f,712.097f},{400.199f,710.983f},
        {401.536f,709.981f},{402.427f,709.090f},{403.317f,708.422f},{403.985f,707.754f},
        {404.876f,707.420f},{405.544f,706.752f},{406.435f,706.306f},{407.326f,705.972f},
        {408.105f,705.750f},{408.885f,705.304f},{409.776f,704.859f},{410.555f,704.525f},
        {411.335f,704.413f},{412.226f,703.968f},{413.005f,703.189f},{413.450f,702.966f},
        {414.007f,702.409f},{414.675f,702.186f},{415.455f,701.518f},{416.568f,700.962f},
        {417.348f,700.516f},{417.570f,700.071f},{417.682f,699.737f},{417.682f,699.625f},
        {417.793f,699.180f},{417.793f,698.512f},{417.125f,698.066f},{416.457f,697.287f},
        {415.455f,696.507f},{414.787f,695.951f},{414.341f,695.505f},{413.562f,695.060f},
        {412.560f,694.503f},{411.557f,693.835f},{410.444f,693.167f},{409.553f,692.276f},
        {408.662f,691.942f},{407.771f,691.385f},{406.992f,690.717f},{406.213f,690.049f},
        {405.099f,689.381f},{404.431f,689.047f},{404.208f,688.601f},{403.763f,688.045f},
        {403.540f,687.488f},{402.983f,686.708f},{402.427f,686.374f},{401.981f,685.818f},
        {401.536f,685.150f},{401.313f,684.259f},{401.090f,683.257f},{400.868f,682.923f},
        {400.979f,682.254f},{401.090f,681.586f},{401.202f,681.030f},{401.536f,680.473f},
        {401.758f,679.805f},{402.092f,679.582f},{402.538f,679.582f},{403.317f,679.916f},
        {404.208f,680.584f},{404.654f,681.698f},{405.544f,682.700f},{406.658f,683.702f},
        {407.660f,684.481f},{408.774f,685.261f},{409.664f,686.152f},{410.889f,687.043f},
        {411.669f,687.488f},{412.448f,688.045f},{413.228f,688.379f},{414.119f,689.158f},
        {415.009f,689.715f},{415.900f,690.383f},{417.236f,691.051f},{418.461f,691.831f},
        {419.352f,692.276f},{420.243f,693.278f},{421.468f,693.946f},{422.136f,694.614f},
        {422.804f,695.283f},{422.136f,696.285f},{421.468f,697.064f},{420.688f,697.732f},
        {419.909f,698.623f},{419.241f,699.403f},{418.795f,699.737f},{418.461f,700.071f},
        {418.016f,700.516f},{417.570f,700.850f},{416.902f,701.184f},{416.234f,701.852f},
        {415.566f,702.298f},{415.009f,702.632f},{414.341f,703.077f},{413.562f,703.411f},
        {412.671f,704.191f},{411.446f,704.859f},{410.667f,705.750f},{409.330f,706.975f},
        {408.328f,707.643f},{407.326f,708.422f},{406.435f,709.424f},{405.322f,710.538f},
        {404.542f,711.317f},{403.317f,712.431f},{402.315f,713.656f},{401.090f,714.769f},
        {400.199f,715.883f},{399.309f,716.885f},{398.307f,718.332f},{397.082f,719.557f},
        {396.636f,720.560f},{396.414f,721.339f},{396.191f,722.118f},{396.191f,722.787f},
        {396.302f,723.566f},{396.748f,723.900f},{397.527f,724.345f},{398.307f,724.568f},
        {399.309f,724.791f},{400.756f,724.902f},{401.981f,724.902f},{403.095f,724.902f},
        {404.097f,724.680f},{405.544f,724.680f},{406.658f,724.680f},{407.771f,724.345f},
        {408.662f,724.345f},{409.776f,724.011f},{411.335f,723.566f},{412.337f,723.566f},
        {413.450f,723.455f},{414.787f,723.343f},{416.123f,723.121f},{416.568f,722.898f},
        {417.570f,722.675f},{418.573f,722.230f},{420.020f,722.118f},{421.022f,721.784f},
        {422.470f,721.450f},{423.361f,721.005f},{424.252f,721.005f},{425.365f,720.782f},
        {426.479f,720.560f},{427.592f,720.448f},{428.706f,720.114f},{429.596f,719.891f},
        {430.487f,719.669f},{431.378f,719.335f},{432.714f,718.778f},{433.382f,718.667f},
        {434.051f,718.555f},{434.719f,718.221f},{435.164f,717.998f},{435.944f,717.998f},
        {436.946f,717.776f},{437.614f,717.776f},{438.282f,717.664f},{438.950f,717.330f},
        {440.175f,717.330f},{440.732f,717.219f},{441.400f,717.108f},{441.957f,716.996f},
        {442.402f,716.996f},{442.847f,717.330f}
    };

    //* directional side 
    static const float rightPanel[][2] = {
        {469.950f,702.855f},{471.287f,702.454f},{473.425f,702.587f},{476.231f,702.454f},
        {480.239f,702.320f},{482.377f,702.186f},{485.451f,701.919f},{488.524f,701.919f},
        {491.731f,701.786f},{494.270f,701.652f},{496.541f,701.518f},{498.412f,701.251f},
        {499.882f,701.117f},{500.951f,700.583f},{501.753f,699.915f},{502.421f,699.247f},
        {504.024f,699.781f},{505.227f,700.449f},{506.429f,701.117f},{507.231f,701.919f},
        {507.766f,702.587f},{508.033f,703.122f},{507.498f,703.656f},{506.830f,704.191f},
        {505.761f,704.592f},{504.291f,704.859f},{502.688f,705.260f},{501.218f,705.393f},
        {499.748f,705.661f},{498.278f,705.928f},{496.408f,706.061f},{494.804f,706.061f},
        {493.334f,706.329f},{491.330f,706.462f},{489.860f,706.462f},{488.123f,706.596f},
        {486.520f,706.730f},{484.916f,706.863f},{483.580f,706.863f},{481.976f,706.997f},
        {480.373f,706.997f},{479.037f,707.130f},{477.166f,707.130f},{475.429f,707.398f},
        {473.959f,707.398f},{472.489f,707.531f},{471.019f,707.665f},{470.084f,707.665f},
        {469.683f,702.721f}
    };
    static const float bottomPanel[][2] = {
        {403.985f,716.328f},{404.988f,716.551f},{405.878f,716.662f},{407.103f,716.328f},
        {408.328f,716.328f},{409.219f,716.105f},{410.221f,715.994f},{411.112f,715.660f},
        {412.114f,715.437f},{414.119f,714.992f},{416.568f,714.324f},{418.795f,713.322f},
        {421.468f,712.208f},{423.806f,710.983f},{426.145f,710.204f},{428.594f,708.756f},
        {431.378f,707.531f},{434.162f,706.863f},{435.944f,706.418f},{438.282f,705.638f},
        {440.398f,705.527f},{443.181f,705.193f},{445.186f,705.304f},{446.522f,705.638f},
        {448.081f,705.972f},{449.083f,706.195f},{449.751f,707.197f},{450.531f,707.643f},
        {451.644f,709.090f},{452.312f,709.870f},{453.649f,711.540f},{454.762f,712.988f},
        {455.541f,714.435f},{456.544f,716.439f},{457.880f,719.001f},{459.550f,722.341f},
        {461.555f,726.573f},{464.116f,730.804f},{466.677f,734.701f},{468.124f,736.928f},
        {469.795f,738.153f},{470.685f,738.599f},{470.908f,739.267f},{471.131f,740.046f},
        {470.908f,740.714f},{470.574f,741.494f},{469.795f,741.716f},{468.124f,740.937f},
        {465.563f,738.710f},{463.447f,736.817f},{460.886f,734.033f},{457.323f,729.690f},
        {455.876f,727.909f},{454.428f,725.904f},{452.424f,723.232f},{451.310f,721.562f},
        {450.419f,720.337f},{449.640f,719.112f},{448.415f,717.442f},{447.747f,716.662f},
        {447.190f,715.771f},{446.745f,714.992f},{446.188f,714.435f},{445.520f,713.878f},
        {444.852f,713.544f},{443.850f,713.210f},{443.070f,713.210f},{442.068f,713.322f},
        {439.729f,713.767f},{438.282f,713.990f},{436.612f,714.324f},{434.830f,714.658f},
        {431.044f,715.326f},{429.374f,715.549f},{427.369f,716.217f},{425.254f,716.662f},
        {423.472f,716.996f},{421.356f,717.442f},{418.907f,717.998f},{416.791f,718.332f},
        {414.007f,718.778f},{411.557f,719.446f},{409.442f,719.780f},{407.215f,720.337f},
        {404.876f,720.337f},{403.095f,720.448f},{401.424f,720.560f},{399.531f,720.671f},
        {400.534f,719.780f},{401.424f,719.223f},{402.204f,718.444f},{402.649f,717.776f},
        {403.651f,716.439f}
    };
    static const float leftPanel[][2] = {
        {407.215f,679.359f},{409.442f,680.139f},{411.335f,680.695f},{412.448f,681.252f},
        {413.673f,681.809f},{415.009f,682.143f},{416.791f,682.923f},{418.239f,683.702f},
        {419.463f,684.147f},{420.911f,684.704f},{422.804f,685.929f},{423.806f,686.820f},
        {424.697f,687.599f},{425.922f,688.267f},{427.147f,689.381f},{427.926f,690.272f},
        {429.151f,691.274f},{429.931f,692.053f},{430.933f,692.944f},{431.155f,693.390f},
        {429.040f,694.726f},{424.808f,691.719f},{422.025f,690.049f},{419.909f,688.601f},
        {417.904f,687.377f},{416.234f,686.152f},{414.675f,685.261f},{413.116f,684.036f},
        {412.114f,683.368f},{410.889f,682.477f},{409.776f,681.809f},{408.662f,681.030f},
        {407.103f,679.582f},{407.103f,679.248f}
    };

    //* --- texture spots
    static const float spots[][10][2] = {
        {{412.348f,679.038f},{415.235f,678.845f},{416.774f,679.230f},{417.544f,680.000f},{416.197f,680.385f},{414.850f,680.385f},{413.118f,680.385f},{412.156f,679.615f},{412.156f,679.615f},{412.156f,679.615f}},
        {{421.777f,680.577f},{423.701f,680.000f},{425.240f,680.385f},{426.587f,681.154f},{427.742f,682.309f},{427.934f,683.078f},{423.893f,683.463f},{422.739f,682.694f},{421.584f,681.924f},{421.200f,680.769f}},
        {{431.398f,681.924f},{435.823f,682.501f},{436.785f,683.463f},{437.170f,684.233f},{436.978f,685.195f},{436.593f,685.772f},{435.823f,686.349f},{434.669f,685.965f},{431.205f,681.924f},{431.205f,681.924f}},
        {{442.558f,686.734f},{443.520f,685.387f},{444.674f,684.618f},{445.829f,685.003f},{446.983f,685.965f},{447.368f,686.734f},{447.945f,687.696f},{447.753f,688.658f},{446.406f,688.851f},{445.059f,688.658f}},
        {{456.989f,682.116f},{458.721f,682.886f},{459.683f,683.271f},{461.222f,684.233f},{461.607f,685.003f},{461.030f,685.965f},{459.490f,686.157f},{457.566f,686.349f},{456.219f,685.387f},{455.642f,684.425f}},
        {{462.184f,679.230f},{463.146f,677.883f},{464.301f,677.691f},{465.263f,678.076f},{465.840f,678.845f},{466.033f,679.422f},{465.840f,680.577f},{464.878f,681.154f},{463.724f,681.347f},{462.569f,680.577f}},
        {{469.496f,675.382f},{469.881f,673.842f},{471.228f,673.073f},{472.382f,672.303f},{473.344f,672.303f},{473.922f,673.073f},{474.114f,673.842f},{473.922f,674.420f},{473.152f,675.189f},{472.960f,675.959f}},
        {{476.423f,669.994f},{477.578f,669.032f},{478.732f,668.647f},{479.887f,668.455f},{480.464f,668.455f},{480.464f,668.840f},{480.079f,669.032f},{479.502f,669.609f},{478.540f,669.994f},{477.770f,670.379f}},
        {{465.263f,692.507f},{466.417f,692.507f},{467.572f,692.507f},{468.149f,693.084f},{469.111f,693.469f},{469.689f,694.239f},{469.496f,695.201f},{468.919f,695.970f},{467.764f,696.163f},{466.802f,696.355f}},
        {{474.691f,694.623f},{476.038f,694.239f},{477.000f,694.431f},{477.962f,694.623f},{478.925f,695.008f},{479.502f,695.585f},{479.887f,696.163f},{479.694f,696.932f},{478.732f,697.510f},{477.193f,697.702f}},
        {{483.543f,695.393f},{485.274f,694.816f},{486.429f,695.008f},{487.391f,695.778f},{488.353f,696.548f},{488.738f,696.932f},{487.968f,697.702f},{487.006f,697.702f},{485.852f,697.702f},{484.697f,697.510f}},
        {{491.816f,697.125f},{493.356f,696.932f},{494.510f,697.317f},{495.280f,698.087f},{494.510f,699.434f},{493.548f,698.664f},{491.816f,697.317f},{491.816f,697.317f},{491.816f,697.317f},{491.816f,697.317f}},
        {{456.219f,702.320f},{457.374f,702.705f},{458.144f,703.667f},{458.721f,704.629f},{458.721f,705.591f},{458.528f,706.168f},{457.951f,706.746f},{456.989f,707.323f},{455.835f,707.130f},{455.257f,706.361f}},
        {{458.336f,713.288f},{458.721f,712.133f},{459.490f,711.364f},{460.645f,711.171f},{461.415f,712.133f},{462.184f,713.288f},{461.992f,714.442f},{461.222f,715.212f},{459.683f,715.212f},{458.528f,714.827f}},
        {{461.030f,719.830f},{461.222f,718.291f},{462.184f,717.521f},{463.339f,718.098f},{464.108f,719.253f},{464.686f,720.407f},{464.301f,721.177f},{463.339f,721.562f},{462.184f,720.984f},{462.184f,720.984f}},
        {{464.686f,725.987f},{465.455f,725.795f},{466.225f,725.987f},{466.610f,726.565f},{466.995f,727.527f},{467.187f,728.296f},{466.995f,729.451f},{466.033f,729.643f},{465.263f,728.681f},{464.878f,727.911f}},
        {{442.365f,696.355f},{443.328f,696.355f},{444.290f,696.740f},{444.674f,697.510f},{444.867f,698.472f},{444.674f,699.241f},{444.097f,699.819f},{443.135f,700.203f},{442.173f,700.396f},{441.019f,700.011f}},
        {{430.628f,700.203f},{431.975f,699.819f},{433.322f,699.819f},{434.476f,700.011f},{434.861f,701.166f},{434.669f,702.128f},{433.707f,702.897f},{432.360f,703.090f},{431.013f,703.090f},{430.243f,702.320f}},
        {{420.238f,706.553f},{421.200f,705.206f},{422.354f,704.437f},{423.509f,704.052f},{424.663f,704.629f},{425.625f,705.206f},{425.818f,705.784f},{425.818f,706.746f},{425.048f,707.323f},{424.086f,708.093f}},
        {{410.039f,712.518f},{414.850f,708.093f},{416.582f,708.477f},{416.582f,709.247f},{416.774f,709.824f},{420.430f,710.402f},{420.430f,711.364f},{416.582f,711.364f},{411.964f,713.480f},{410.809f,714.057f}}
    };
    static const int spotCounts[20] = { 8,10,9,12,10,10,10,10,10,10,10,7,10,10,9,10,10,10,10,10 };

    int nOuter   = (int)(sizeof(outerBody)   / sizeof(outerBody[0]));
    int nInner   = (int)(sizeof(innerBody)   / sizeof(innerBody[0]));
    int nTop     = (int)(sizeof(topHighlight)/ sizeof(topHighlight[0]));
    int nRS      = (int)(sizeof(rightShadow) / sizeof(rightShadow[0]));
    int nBLS     = (int)(sizeof(blShadow)    / sizeof(blShadow[0]));
    int nRP      = (int)(sizeof(rightPanel)  / sizeof(rightPanel[0]));
    int nBP      = (int)(sizeof(bottomPanel) / sizeof(bottomPanel[0]));
    int nLP      = (int)(sizeof(leftPanel)   / sizeof(leftPanel[0]));
    int s;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    drawFilledTessCall(outerBody,    nOuter, 0.95f, 0.55f, 0.20f, 1.00f);
    drawFilledTessCall(innerBody,    nInner, 0.85f, 0.42f, 0.15f, 1.00f);
    drawFilledTessCall(topHighlight, nTop,   1.00f, 0.85f, 0.55f, 0.70f);
    drawFilledTessCall(rightShadow,  nRS,    0.72f, 0.30f, 0.10f, 0.65f);
    drawFilledTessCall(blShadow,     nBLS,   0.72f, 0.30f, 0.10f, 0.55f);
    drawFilledTessCall(rightPanel,   nRP,    0.90f, 0.48f, 0.18f, 0.75f);
    drawFilledTessCall(bottomPanel,  nBP,    0.90f, 0.48f, 0.18f, 0.75f);
    drawFilledTessCall(leftPanel,    nLP,    0.90f, 0.48f, 0.18f, 0.75f);

    for(s = 0; s < 20; s++)
        drawFilledTessCall(spots[s], spotCounts[s], 0.98f, 0.72f, 0.35f, 0.85f);

    drawOutlineLoop(outerBody, nOuter, 0.80f, 0.30f, 0.08f, 0.95f, 2.0f);

    glDisable(GL_BLEND);
}

//! =================      TURTLE         =================
static float turtleX      =  200.0f;
static float turtleY      =    65.0f;
static float turtleSpeedX =   -0.5f;
static float turtleTime   =    0.0f;

void updateTurtle(void){
    turtleTime += 0.05f;
    turtleX    += turtleSpeedX;
    turtleY     = 65.0f + sinf(turtleTime) * 3.0f;
    if (turtleX < -900.0f) turtleX = 500.0f;
}

void drawTurtle(void){
    int i, n;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(turtleX, turtleY, 0.0f);

    //? 1. OUTER BODY BASE (dark olive-green silhouette) 
    {
        static const float p[][2] = {
            {736.394f, 441.622f}, {736.260f, 439.217f}, {736.260f, 437.614f},
            {736.127f, 436.010f}, {736.260f, 434.808f}, {736.394f, 433.605f},
            {736.795f, 432.670f}, {737.329f, 430.933f}, {737.864f, 429.329f},
            {738.398f, 428.127f}, {739.066f, 427.058f}, {739.868f, 425.989f},
            {740.803f, 425.053f}, {741.739f, 424.118f}, {742.808f, 423.450f},
            {743.877f, 422.915f}, {745.213f, 422.381f}, {746.415f, 421.846f},
            {748.286f, 421.312f}, {749.622f, 421.045f}, {751.092f, 420.911f},
            {752.562f, 420.777f}, {754.299f, 420.644f}, {755.769f, 420.644f},
            {756.838f, 420.777f}, {758.308f, 421.045f}, {759.911f, 421.312f},
            {761.515f, 421.846f}, {763.118f, 422.782f}, {764.989f, 423.583f},
            {767.127f, 424.652f}, {768.730f, 425.454f}, {770.200f, 426.523f},
            {772.071f, 427.726f}, {773.674f, 429.062f}, {775.412f, 430.532f},
            {777.015f, 431.868f}, {778.351f, 432.670f}, {778.351f, 432.670f},
            {779.019f, 433.472f}, {784.364f, 431.066f}, {789.576f, 429.730f},
            {793.183f, 428.394f}, {795.989f, 427.325f}, {799.731f, 426.390f},
            {803.339f, 425.721f}, {806.946f, 425.321f}, {810.020f, 425.321f},
            {813.360f, 425.321f}, {815.899f, 425.187f}, {818.839f, 425.187f},
            {822.714f, 425.321f}, {825.520f, 425.454f}, {828.460f, 426.122f},
            {832.068f, 426.924f}, {834.606f, 427.458f}, {837.145f, 428.394f},
            {839.684f, 429.463f}, {843.025f, 430.532f}, {845.697f, 431.601f},
            {848.236f, 432.670f}, {850.908f, 434.006f}, {854.650f, 435.743f},
            {856.921f, 437.213f}, {859.727f, 439.217f}, {862.133f, 440.821f},
            {864.003f, 442.023f}, {866.008f, 443.760f}, {867.611f, 445.097f},
            {869.348f, 446.700f}, {871.085f, 448.571f}, {872.822f, 450.442f},
            {874.559f, 452.179f}, {876.430f, 454.317f}, {878.301f, 456.855f},
            {880.172f, 459.127f}, {881.641f, 461.265f}, {883.379f, 463.537f},
            {885.116f, 465.140f}, {886.185f, 466.877f}, {887.788f, 469.149f},
            {888.991f, 471.287f}, {890.594f, 473.959f}, {891.930f, 475.429f},
            {892.599f, 476.899f}, {893.133f, 477.701f}, {895.271f, 477.834f},
            {896.874f, 477.968f}, {898.745f, 478.502f}, {900.215f, 478.770f},
            {901.551f, 479.571f}, {902.620f, 480.106f}, {903.288f, 480.373f},
            {903.021f, 481.709f}, {902.219f, 482.377f}, {901.418f, 483.179f},
            {900.349f, 483.847f}, {899.413f, 484.382f}, {898.612f, 484.783f},
            {897.810f, 484.783f}, {897.142f, 484.916f}, {896.340f, 485.451f},
            {895.137f, 485.718f}, {892.732f, 485.852f}, {889.926f, 485.718f},
            {888.723f, 485.852f}, {887.387f, 485.985f}, {886.185f, 486.386f},
            {885.383f, 486.921f}, {884.982f, 487.722f}, {885.116f, 488.791f},
            {885.650f, 489.593f}, {887.521f, 490.796f}, {889.258f, 491.464f},
            {890.594f, 492.132f}, {892.064f, 493.468f}, {893.534f, 494.537f},
            {895.137f, 496.140f}, {896.073f, 497.209f}, {897.275f, 498.278f},
            {898.344f, 499.481f}, {899.814f, 501.085f}, {901.017f, 502.554f},
            {902.086f, 503.891f}, {903.021f, 505.093f}, {903.956f, 506.296f},
            {904.892f, 507.365f}, {905.025f, 512.442f}, {904.491f, 513.645f},
            {902.754f, 514.046f}, {900.081f, 514.046f}, {898.478f, 514.180f},
            {896.607f, 514.180f}, {895.004f, 514.313f}, {892.866f, 514.313f},
            {891.129f, 514.046f}, {889.258f, 514.046f}, {887.654f, 513.912f},
            {886.185f, 513.779f}, {884.715f, 513.244f}, {883.379f, 512.576f},
            {882.042f, 512.175f}, {880.706f, 511.373f}, {879.370f, 510.304f},
            {878.435f, 509.235f}, {877.366f, 508.300f}, {876.163f, 507.365f},
            {875.094f, 506.162f}, {874.426f, 506.029f}, {873.491f, 505.895f},
            {872.288f, 505.895f}, {870.417f, 505.761f}, {868.814f, 505.227f},
            {867.210f, 504.692f}, {865.874f, 503.891f}, {864.805f, 503.089f},
            {863.335f, 502.287f}, {862.133f, 501.352f}, {860.930f, 500.283f},
            {859.995f, 499.214f}, {859.193f, 498.412f}, {858.391f, 497.343f},
            {857.857f, 496.274f}, {855.852f, 495.740f}, {854.382f, 495.873f},
            {852.913f, 495.873f}, {851.042f, 496.007f}, {849.438f, 496.140f},
            {847.969f, 496.140f}, {846.232f, 496.140f}, {844.494f, 496.007f},
            {843.292f, 496.007f}, {841.688f, 495.740f}, {840.218f, 495.606f},
            {839.016f, 495.606f}, {837.412f, 495.606f}, {836.076f, 495.472f},
            {834.606f, 495.339f}, {833.137f, 494.938f}, {831.667f, 494.804f},
            {830.330f, 494.804f}, {828.994f, 494.671f}, {827.391f, 494.537f},
            {825.787f, 494.403f}, {824.184f, 494.136f}, {822.714f, 493.869f},
            {821.378f, 493.602f}, {819.641f, 493.334f}, {818.304f, 492.800f},
            {816.835f, 492.399f}, {815.231f, 491.464f}, {813.628f, 490.929f},
            {812.158f, 490.395f}, {811.222f, 489.994f}, {809.886f, 489.459f},
            {808.817f, 489.192f}, {807.481f, 488.658f}, {806.145f, 488.390f},
            {805.076f, 487.856f}, {804.274f, 487.321f}, {803.472f, 486.921f},
            {802.671f, 486.386f}, {801.602f, 485.852f}, {800.800f, 485.718f},
            {801.735f, 487.188f}, {802.537f, 488.524f}, {803.071f, 489.727f},
            {803.606f, 490.929f}, {804.408f, 491.998f}, {805.076f, 493.201f},
            {805.744f, 494.537f}, {806.546f, 496.274f}, {807.481f, 498.011f},
            {808.416f, 499.481f}, {808.684f, 500.416f}, {808.817f, 501.753f},
            {808.817f, 503.356f}, {808.817f, 504.826f}, {808.951f, 506.563f},
            {808.951f, 508.701f}, {809.084f, 510.304f}, {809.352f, 511.373f},
            {809.886f, 512.576f}, {810.822f, 513.779f}, {811.623f, 514.848f},
            {812.158f, 515.649f}, {812.826f, 516.451f}, {813.227f, 517.520f},
            {813.761f, 518.322f}, {814.830f, 520.059f}, {814.697f, 526.874f},
            {814.296f, 527.542f}, {813.360f, 527.542f}, {808.951f, 527.542f},
            {807.748f, 527.141f}, {806.946f, 526.473f}, {805.878f, 525.938f},
            {804.809f, 525.003f}, {803.205f, 524.335f}, {801.869f, 523.533f},
            {800.800f, 522.731f}, {799.597f, 522.063f}, {798.528f, 521.395f},
            {797.192f, 520.727f}, {796.123f, 520.059f}, {795.188f, 519.257f},
            {793.985f, 518.455f}, {792.782f, 517.386f}, {791.580f, 516.585f},
            {790.244f, 515.649f}, {789.175f, 514.848f}, {788.373f, 513.779f},
            {787.170f, 512.843f}, {786.101f, 511.641f}, {785.166f, 510.705f},
            {784.364f, 509.503f}, {783.563f, 508.300f}, {782.761f, 507.365f},
            {782.093f, 506.162f}, {781.291f, 504.826f}, {780.623f, 503.356f},
            {780.088f, 502.421f}, {779.287f, 500.951f}, {778.886f, 499.748f},
            {778.351f, 498.546f}, {777.817f, 497.076f}, {777.416f, 495.606f},
            {776.481f, 494.403f}, {776.347f, 493.334f}, {776.080f, 491.731f},
            {775.812f, 490.261f}, {775.278f, 489.058f}, {774.877f, 488.123f},
            {774.877f, 487.589f}, {772.071f, 493.201f}, {771.403f, 494.003f},
            {770.601f, 494.136f}, {769.799f, 493.334f}, {768.998f, 491.998f},
            {767.661f, 490.395f}, {766.325f, 488.658f}, {765.657f, 486.787f},
            {764.989f, 485.317f}, {764.321f, 483.981f}, {763.519f, 481.843f},
            {763.118f, 480.373f}, {762.450f, 479.170f}, {762.317f, 477.968f},
            {762.183f, 476.632f}, {762.049f, 475.429f}, {762.049f, 474.093f},
            {762.049f, 473.024f}, {762.183f, 471.955f}, {762.317f, 471.019f},
            {762.584f, 469.817f}, {762.851f, 468.080f}, {763.653f, 466.877f},
            {763.920f, 465.407f}, {764.722f, 464.472f}, {765.256f, 463.403f},
            {765.791f, 462.200f}, {766.592f, 461.265f}, {767.261f, 460.597f},
            {767.929f, 459.394f}, {768.597f, 458.726f}, {769.131f, 457.924f},
            {769.799f, 457.256f}, {769.131f, 456.722f}, {768.330f, 456.054f},
            {767.929f, 455.252f}, {767.261f, 454.985f}, {766.325f, 454.584f},
            {765.256f, 454.717f}, {763.786f, 454.985f}, {762.584f, 454.985f},
            {761.248f, 454.851f}, {759.778f, 454.717f}, {758.709f, 454.317f},
            {757.640f, 453.916f}, {756.304f, 453.515f}, {755.101f, 453.381f},
            {754.032f, 452.847f}, {752.562f, 452.446f}, {751.092f, 451.644f},
            {749.890f, 451.110f}, {748.553f, 450.308f}, {747.618f, 449.907f},
            {746.816f, 449.373f}, {745.747f, 448.704f}, {744.678f, 447.903f},
            {743.743f, 447.502f}, {742.941f, 446.700f}, {742.140f, 445.898f},
            {741.471f, 445.230f}, {740.803f, 444.562f}, {740.269f, 443.627f},
            {740.135f, 442.825f}, {740.002f, 441.890f}, {739.734f, 441.088f},
            {739.467f, 440.420f}, {738.933f, 440.286f}, {738.398f, 440.420f},
            {737.864f, 440.954f}, {737.062f, 441.489f}, {736.527f, 441.890f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.28f, 0.50f, 0.20f, 1.00f);
    }

    //* 2. FRONT FLIPPER 
    {
        static const float p[][2] = {
            {770.468f,458.753f},{772.552f,460.677f},{773.995f,462.281f},
            {776.080f,464.365f},{777.523f,465.808f},{778.806f,467.091f},
            {779.928f,468.534f},{778.485f,470.138f},{777.202f,471.420f},
            {776.240f,472.703f},{775.278f,474.146f},{774.476f,475.750f},
            {773.674f,477.995f},{773.514f,479.758f},{773.674f,481.041f},
            {773.835f,482.164f},{773.674f,483.286f},{773.193f,484.408f},
            {772.552f,485.691f},{771.911f,487.134f},{771.430f,488.417f},
            {770.949f,489.700f},{769.986f,489.860f},{768.864f,488.577f},
            {767.742f,487.295f},{767.100f,485.852f},{766.459f,484.569f},
            {765.978f,483.446f},{765.497f,482.324f},{765.016f,480.881f},
            {764.535f,479.438f},{764.535f,478.476f},{764.214f,477.193f},
            {763.893f,475.750f},{763.893f,474.467f},{763.573f,473.024f},
            {763.573f,471.741f},{763.733f,470.619f},{764.054f,469.496f},
            {764.374f,468.374f},{764.855f,467.251f},{765.336f,466.129f},
            {765.978f,465.006f},{766.299f,464.044f},{766.940f,463.082f},
            {767.581f,462.441f},{768.223f,461.479f},{769.024f,460.517f},
            {769.505f,459.875f},{770.147f,458.753f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.30f, 0.52f, 0.22f, 1.00f);
    }
    //* 3
    {
        static const float p[][2] = {
            {738.077f, 437.908f}, {738.398f, 436.785f}, {738.398f, 435.503f},
            {738.719f, 434.220f}, {739.040f, 432.777f}, {739.521f, 431.654f},
            {740.162f, 430.211f}, {740.964f, 428.928f}, {741.926f, 427.646f},
            {743.209f, 426.523f}, {744.491f, 425.561f}, {745.774f, 425.080f},
            {746.897f, 424.439f}, {748.500f, 423.958f}, {749.943f, 423.797f},
            {751.547f, 423.477f}, {752.829f, 423.477f}, {754.433f, 423.477f},
            {755.555f, 423.477f}, {757.159f, 423.797f}, {758.762f, 424.118f},
            {760.526f, 424.599f}, {762.129f, 425.401f}, {763.412f, 425.721f},
            {764.535f, 426.523f}, {765.657f, 427.165f}, {767.100f, 427.966f},
            {768.383f, 428.928f}, {769.345f, 429.730f}, {770.788f, 430.692f},
            {773.193f, 432.616f}, {773.995f, 433.258f}, {775.118f, 433.578f},
            {776.240f, 434.540f}, {777.042f, 435.663f}, {777.362f, 439.672f},
            {778.004f, 440.313f}, {779.287f, 440.634f}, {780.088f, 441.435f},
            {780.569f, 442.718f}, {780.249f, 445.925f}, {780.088f, 448.330f},
            {780.249f, 450.094f}, {780.249f, 451.858f}, {780.730f, 452.980f},
            {781.050f, 453.942f}, {781.852f, 455.386f}, {782.975f, 456.829f},
            {783.937f, 458.272f}, {785.219f, 459.875f}, {786.182f, 460.998f},
            {786.983f, 461.799f}, {788.266f, 462.922f}, {789.388f, 463.724f},
            {791.633f, 465.167f}, {792.916f, 465.648f}, {793.878f, 466.129f},
            {794.680f, 466.770f}, {795.802f, 467.893f}, {796.604f, 468.855f},
            {797.566f, 469.656f}, {798.689f, 470.619f}, {799.651f, 471.420f},
            {800.613f, 472.543f}, {802.056f, 473.986f}, {803.178f, 475.269f},
            {804.140f, 476.391f}, {805.584f, 477.834f}, {806.866f, 478.957f},
            {808.309f, 479.919f}, {809.913f, 480.720f}, {811.196f, 482.003f},
            {812.960f, 482.645f}, {814.723f, 483.446f}, {816.166f, 484.088f},
            {817.770f, 484.569f}, {818.892f, 485.370f}, {821.137f, 486.012f},
            {822.741f, 486.493f}, {824.825f, 487.455f}, {827.391f, 487.936f},
            {829.155f, 488.738f}, {830.918f, 489.058f}, {833.484f, 490.181f},
            {835.729f, 490.822f}, {837.653f, 491.143f}, {839.417f, 491.464f},
            {840.860f, 491.624f}, {842.143f, 491.624f}, {843.425f, 491.945f},
            {845.029f, 491.945f}, {847.113f, 492.426f}, {849.198f, 492.746f},
            {851.122f, 492.907f}, {852.886f, 492.907f}, {854.970f, 492.746f},
            {856.093f, 492.746f}, {857.215f, 492.746f}, {858.177f, 492.746f},
            {859.300f, 492.746f}, {860.422f, 492.265f}, {861.705f, 492.105f},
            {863.469f, 491.945f}, {864.591f, 491.784f}, {866.034f, 491.624f},
            {867.317f, 491.624f}, {868.760f, 491.303f}, {870.203f, 491.143f},
            {871.326f, 490.983f}, {872.609f, 490.822f}, {874.372f, 490.662f},
            {876.297f, 490.181f}, {877.740f, 489.700f}, {878.862f, 489.540f},
            {879.985f, 489.379f}, {880.947f, 489.219f}, {882.229f, 490.341f},
            {883.352f, 491.143f}, {884.314f, 491.784f}, {884.955f, 492.265f},
            {886.398f, 492.265f}, {887.200f, 492.907f}, {888.483f, 493.869f},
            {889.285f, 494.991f}, {890.247f, 495.953f}, {891.369f, 496.915f},
            {892.492f, 498.198f}, {894.095f, 499.802f}, {894.897f, 500.924f},
            {895.859f, 502.207f}, {896.821f, 503.169f}, {897.943f, 504.452f},
            {899.387f, 505.895f}, {900.509f, 507.017f}, {901.631f, 508.140f},
            {901.471f, 508.460f}, {900.349f, 509.262f}, {898.745f, 509.423f},
            {897.943f, 509.743f}, {896.821f, 509.743f}, {895.378f, 509.743f},
            {894.095f, 509.743f}, {892.652f, 509.743f}, {891.209f, 509.743f},
            {889.605f, 509.423f}, {887.842f, 509.262f}, {886.398f, 508.781f},
            {884.955f, 508.621f}, {883.673f, 508.140f}, {882.229f, 507.659f},
            {880.626f, 507.017f}, {878.862f, 505.735f}, {877.259f, 504.131f},
            {876.297f, 503.009f}, {875.495f, 502.207f}, {874.693f, 501.405f},
            {873.731f, 500.443f}, {872.609f, 500.122f}, {872.128f, 500.924f},
            {871.967f, 501.886f}, {871.486f, 502.848f}, {871.005f, 503.490f},
            {870.043f, 503.810f}, {867.157f, 501.405f}, {865.393f, 500.603f},
            {864.271f, 499.641f}, {862.988f, 498.519f}, {861.865f, 497.236f},
            {860.903f, 496.114f}, {860.102f, 494.831f}, {859.139f, 493.709f},
            {857.857f, 493.388f}, {856.734f, 493.548f}, {855.772f, 493.548f},
            {854.650f, 493.548f}, {854.169f, 493.709f}, {853.367f, 494.029f},
            {852.405f, 494.029f}, {851.282f, 494.190f}, {849.519f, 494.190f},
            {847.755f, 494.029f}, {846.151f, 494.029f}, {844.708f, 494.029f},
            {843.265f, 494.029f}, {841.501f, 493.869f}, {839.577f, 493.709f},
            {837.653f, 493.228f}, {836.049f, 492.907f}, {833.003f, 492.746f},
            {831.399f, 492.586f}, {829.796f, 492.586f}, {828.032f, 492.426f},
            {826.268f, 492.265f}, {824.504f, 491.784f}, {822.741f, 491.624f},
            {820.817f, 490.983f}, {819.373f, 490.662f}, {817.770f, 490.181f},
            {816.166f, 489.700f}, {813.922f, 489.058f}, {811.997f, 488.738f},
            {810.554f, 487.936f}, {808.951f, 487.295f}, {807.187f, 486.493f},
            {805.744f, 485.852f}, {804.140f, 484.889f}, {803.018f, 484.088f},
            {801.896f, 483.286f}, {800.933f, 482.484f}, {800.292f, 481.683f},
            {799.651f, 480.720f}, {799.170f, 479.758f}, {799.330f, 478.796f},
            {799.170f, 477.674f}, {799.009f, 477.032f}, {798.849f, 476.231f},
            {798.849f, 475.429f}, {797.887f, 474.788f}, {797.085f, 475.108f},
            {796.764f, 475.910f}, {796.764f, 477.193f}, {796.764f, 478.315f},
            {796.764f, 479.277f}, {796.925f, 480.400f}, {796.925f, 481.201f},
            {797.245f, 482.484f}, {797.727f, 483.607f}, {798.047f, 484.729f},
            {798.368f, 485.531f}, {798.849f, 486.814f}, {799.330f, 487.776f},
            {799.651f, 488.738f}, {800.132f, 490.021f}, {800.773f, 491.464f},
            {801.735f, 493.388f}, {803.018f, 495.953f}, {804.301f, 498.038f},
            {805.263f, 499.962f}, {806.065f, 501.566f}, {806.706f, 502.528f},
            {806.866f, 510.064f}, {807.187f, 512.469f}, {807.828f, 513.271f},
            {809.111f, 515.035f}, {810.234f, 516.638f}, {811.356f, 518.562f},
            {812.478f, 520.968f}, {812.799f, 522.892f}, {812.799f, 524.495f},
            {812.158f, 525.297f}, {811.035f, 525.297f}, {808.951f, 524.656f},
            {807.187f, 524.174f}, {805.584f, 523.212f}, {803.339f, 521.449f},
            {800.933f, 520.005f}, {798.689f, 518.723f}, {796.604f, 516.799f},
            {794.199f, 515.035f}, {791.794f, 512.790f}, {789.709f, 510.705f},
            {788.106f, 508.942f}, {786.342f, 507.178f}, {784.899f, 505.093f},
            {783.776f, 503.329f}, {782.814f, 501.886f}, {782.013f, 500.443f},
            {781.050f, 498.359f}, {780.249f, 496.915f}, {779.607f, 495.152f},
            {778.966f, 493.548f}, {778.164f, 491.464f}, {777.523f, 489.379f},
            {776.881f, 487.134f}, {776.561f, 484.729f}, {776.400f, 482.805f},
            {776.400f, 481.041f}, {776.561f, 479.438f}, {776.721f, 478.315f},
            {776.881f, 476.872f}, {777.202f, 475.750f}, {778.004f, 474.467f},
            {779.126f, 473.344f}, {780.088f, 472.222f}, {781.050f, 471.260f},
            {782.013f, 470.619f}, {782.975f, 469.656f}, {783.776f, 468.694f},
            {784.738f, 467.893f}, {785.380f, 466.770f}, {786.021f, 465.969f},
            {786.502f, 465.327f}, {786.983f, 464.686f}, {787.304f, 464.044f},
            {787.464f, 463.403f}, {786.983f, 462.601f}, {786.182f, 462.601f},
            {785.380f, 463.082f}, {784.738f, 463.884f}, {783.937f, 464.846f},
            {782.975f, 465.969f}, {782.173f, 466.450f}, {781.531f, 467.091f},
            {779.928f, 465.648f}, {778.966f, 464.525f}, {777.843f, 463.563f},
            {776.881f, 462.441f}, {776.080f, 461.479f}, {775.278f, 460.517f},
            {772.873f, 457.951f}, {771.590f, 456.668f}, {770.788f, 455.867f},
            {770.147f, 455.225f}, {770.628f, 454.424f}, {771.269f, 453.782f},
            {771.911f, 453.301f}, {772.071f, 452.660f}, {771.430f, 452.018f},
            {770.628f, 451.858f}, {769.826f, 452.018f}, {769.024f, 452.339f},
            {768.223f, 452.660f}, {767.100f, 452.820f}, {766.138f, 452.660f},
            {764.535f, 452.499f}, {763.412f, 452.339f}, {762.129f, 452.179f},
            {761.007f, 451.858f}, {759.564f, 451.537f}, {758.281f, 451.217f},
            {757.159f, 450.896f}, {755.876f, 450.415f}, {754.593f, 449.934f},
            {753.471f, 449.453f}, {752.348f, 449.132f}, {751.386f, 448.811f},
            {750.264f, 448.330f}, {749.302f, 447.689f}, {748.179f, 447.048f},
            {747.057f, 446.406f}, {746.095f, 445.925f}, {745.293f, 445.444f},
            {744.491f, 444.642f}, {743.529f, 444.001f}, {742.727f, 443.199f},
            {742.246f, 442.397f}, {742.246f, 441.756f}, {742.888f, 442.237f},
            {743.529f, 442.558f}, {744.171f, 443.039f}, {744.812f, 443.360f},
            {745.614f, 443.680f}, {746.736f, 444.161f}, {747.538f, 444.322f},
            {748.660f, 444.482f}, {749.622f, 444.642f}, {750.585f, 444.963f},
            {751.547f, 444.963f}, {752.188f, 445.123f}, {752.829f, 445.123f},
            {753.631f, 445.123f}, {754.433f, 445.123f}, {755.235f, 445.123f},
            {756.036f, 444.963f}, {756.838f, 444.642f}, {757.319f, 444.322f},
            {757.960f, 444.001f}, {758.281f, 443.360f}, {758.602f, 442.879f},
            {758.762f, 442.077f}, {758.762f, 441.435f}, {758.442f, 440.634f},
            {757.800f, 440.153f}, {756.998f, 440.153f}, {756.197f, 440.313f},
            {755.074f, 440.794f}, {754.593f, 441.435f}, {753.791f, 441.596f},
            {752.990f, 441.596f}, {751.867f, 441.596f}, {750.905f, 441.275f},
            {749.943f, 441.275f}, {748.981f, 440.954f}, {747.859f, 440.794f},
            {747.057f, 440.473f}, {746.255f, 440.313f}, {745.293f, 439.832f},
            {744.331f, 439.672f}, {743.529f, 439.191f}, {742.727f, 438.710f},
            {742.086f, 438.389f}, {741.284f, 438.228f}, {739.841f, 437.908f},
            {738.558f, 438.068f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.251f, 0.451f, 0.176f, 1.00f);
    }

    //* 4. LARGE LEFT-BODY SHELL PATCH 
    {
        static const float p[][2] = {
            {784.661f,469.906f},{785.218f,470.927f},{785.589f,472.040f},
            {786.053f,473.525f},{786.331f,475.195f},{785.775f,475.937f},
            {784.476f,477.237f},{784.197f,478.814f},{784.012f,480.020f},
            {784.104f,481.505f},{784.847f,482.711f},{785.775f,483.547f},
            {786.888f,484.289f},{788.187f,485.588f},{788.837f,486.423f},
            {788.837f,487.258f},{788.187f,488.465f},{788.002f,489.393f},
            {788.002f,490.413f},{788.837f,491.620f},{790.229f,493.197f},
            {791.528f,494.403f},{793.291f,495.053f},{793.569f,495.424f},
            {793.755f,496.259f},{793.755f,497.466f},{793.755f,498.672f},
            {794.405f,500.064f},{795.982f,501.827f},{796.539f,502.848f},
            {797.467f,504.332f},{798.116f,506.095f},{798.487f,506.838f},
            {798.487f,507.394f},{797.745f,508.044f},{797.188f,508.972f},
            {796.724f,510.178f},{796.724f,511.106f},{797.003f,512.127f},
            {797.652f,512.776f},{798.580f,513.333f},{800.065f,513.612f},
            {800.807f,513.612f},{801.550f,514.725f},{801.550f,515.839f},
            {801.921f,516.952f},{802.942f,518.344f},{803.591f,519.272f},
            {804.519f,520.200f},{805.633f,521.313f},{806.375f,522.149f},
            {806.746f,522.798f},{806.746f,523.077f},{806.096f,522.798f},
            {805.447f,522.427f},{804.705f,521.963f},{803.962f,521.499f},
            {803.313f,520.942f},{802.385f,520.293f},{801.642f,519.829f},
            {801.086f,519.458f},{800.251f,519.086f},{799.694f,518.622f},
            {798.859f,518.066f},{797.838f,517.509f},{797.003f,516.767f},
            {795.982f,516.117f},{795.240f,515.467f},{794.405f,515.096f},
            {793.291f,514.168f},{792.177f,513.148f},{790.971f,512.313f},
            {790.229f,511.385f},{789.115f,509.993f},{788.280f,509.158f},
            {787.167f,507.766f},{785.775f,505.817f},{784.847f,504.332f},
            {783.826f,502.940f},{782.898f,501.363f},{781.877f,499.971f},
            {781.042f,498.208f},{780.393f,496.630f},{779.650f,494.682f},
            {779.001f,492.455f},{778.630f,491.527f},{777.980f,489.949f},
            {777.609f,488.279f},{777.331f,486.702f},{777.052f,485.310f},
            {776.774f,483.825f},{776.681f,482.526f},{776.774f,481.041f},
            {776.774f,479.835f},{776.959f,478.536f},{777.052f,477.237f},
            {777.423f,476.494f},{777.795f,475.381f},{778.351f,474.360f},
            {778.908f,473.525f},{779.929f,472.875f},{780.764f,472.133f},
            {781.135f,471.391f},{781.877f,471.019f},{782.620f,470.370f},
            {783.269f,469.628f},{783.826f,468.885f},{784.476f,469.813f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.32f, 0.55f, 0.20f, 1.00f);
    }

    //* 5. RIGHT-BODY SHELL PATCH 
    {
        static const float p[][2] = {
            {875.339f,490.506f},{876.787f,490.172f},{878.123f,489.949f},
            {879.236f,489.504f},{880.573f,489.058f},{881.463f,490.061f},
            {882.800f,491.731f},{883.690f,492.622f},{885.027f,493.067f},
            {886.697f,493.624f},{887.699f,493.958f},{888.590f,494.181f},
            {889.926f,495.406f},{891.262f,496.408f},{892.599f,497.855f},
            {893.935f,499.526f},{895.382f,501.196f},{896.830f,502.866f},
            {898.389f,504.536f},{899.725f,506.207f},{900.950f,507.654f},
            {901.729f,508.434f},{901.173f,509.213f},{899.614f,509.213f},
            {897.498f,509.213f},{893.823f,508.768f},{890.817f,507.988f},
            {888.924f,507.543f},{888.144f,507.543f},{886.363f,507.098f},
            {885.806f,506.207f},{885.583f,503.980f},{885.027f,502.977f},
            {884.136f,501.975f},{883.579f,501.196f},{882.577f,501.530f},
            {881.909f,502.421f},{880.795f,502.643f},{880.350f,502.087f},
            {880.127f,501.419f},{880.350f,500.973f},{881.129f,500.750f},
            {881.909f,499.971f},{881.909f,499.303f},{881.797f,498.412f},
            {881.352f,497.410f},{879.904f,497.633f},{878.902f,497.744f},
            {877.455f,498.301f},{876.898f,497.633f},{876.341f,496.742f},
            {876.230f,495.962f},{876.230f,495.517f},{876.118f,494.737f},
            {875.673f,493.401f},{875.228f,492.733f},{875.005f,491.731f},
            {875.005f,491.063f},{875.228f,490.395f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.32f, 0.55f, 0.20f, 1.00f);
    }

    //* 6. HEAD / NECK AREA 
    {
        static const float p[][2] = {
            {741.048f,438.263f},{738.543f,437.521f},{739.842f,438.078f},
            {738.636f,437.985f},{738.079f,437.985f},{738.450f,436.593f},
            {738.728f,434.552f},{739.192f,433.160f},{739.656f,431.675f},
            {740.306f,430.376f},{740.956f,428.706f},{742.162f,427.685f},
            {743.275f,426.757f},{744.574f,425.736f},{745.966f,424.994f},
            {747.544f,424.344f},{749.493f,423.695f},{751.348f,423.509f},
            {753.854f,423.138f},{755.710f,423.138f},{757.194f,423.509f},
            {759.050f,423.695f},{760.721f,424.344f},{762.391f,424.994f},
            {763.875f,426.015f},{765.546f,426.757f},{767.123f,427.592f},
            {768.886f,428.427f},{770.185f,429.448f},{771.670f,430.933f},
            {772.784f,431.768f},{773.619f,432.696f},{775.382f,433.253f},
            {776.217f,434.923f},{776.774f,436.593f},{776.959f,438.635f},
            {777.609f,440.398f},{778.630f,441.047f},{779.465f,441.326f},
            {779.929f,443.460f},{780.207f,445.965f},{780.486f,448.378f},
            {780.578f,450.976f},{780.857f,452.739f},{781.599f,454.873f},
            {781.692f,455.801f},{780.486f,455.430f},{779.372f,454.224f},
            {778.073f,452.832f},{777.609f,451.440f},{776.681f,449.770f},
            {775.753f,449.584f},{774.268f,449.306f},{772.041f,448.656f},
            {770.185f,448.378f},{768.330f,448.192f},{766.474f,448.007f},
            {764.247f,448.007f},{762.205f,448.007f},{760.164f,447.450f},
            {758.679f,446.893f},{757.287f,446.522f},{755.895f,446.244f},
            {754.967f,445.780f},{754.225f,445.594f},{755.710f,444.944f},
            {756.916f,444.573f},{757.658f,444.295f},{758.308f,443.553f},
            {758.493f,442.810f},{758.679f,441.975f},{758.586f,441.047f},
            {758.215f,440.583f},{757.473f,440.398f},{756.452f,440.398f},
            {755.524f,440.862f},{754.875f,441.047f},{753.947f,441.233f},
            {753.204f,441.326f},{752.276f,441.326f},{751.070f,440.954f},
            {750.142f,440.862f},{749.121f,440.862f},{747.915f,440.398f},
            {747.080f,440.212f},{746.245f,440.212f},{745.595f,439.841f},
            {744.574f,439.470f},{743.925f,439.098f},{743.090f,439.006f},
            {742.626f,438.727f},{742.255f,438.542f},{741.512f,438.356f},
            {740.863f,438.263f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.35f, 0.58f, 0.23f, 1.00f);
    }

    //* 7. RIGHT-FLIPPER DETAIL (small patch) 
    {
        static const float p[][2] = {
            {893.614f,479.758f},{895.378f,479.919f},{896.661f,480.400f},
            {897.302f,480.560f},{898.104f,480.881f},{899.066f,480.881f},
            {899.707f,481.201f},{899.226f,482.324f},{898.264f,482.645f},
            {896.821f,482.805f},{895.218f,482.805f},{894.095f,482.645f},
            {892.973f,482.484f},{893.293f,480.079f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.32f, 0.55f, 0.20f, 1.00f);
    }

    // //* 8. HEAD HIGHLIGHT 
    {
        static const float p[][2] = {
            {740.322f,429.890f},{740.964f,428.768f},{741.765f,427.966f},
            {742.567f,427.004f},{743.369f,426.523f},{744.010f,426.042f},
            {744.812f,425.401f},{745.614f,424.920f},{746.576f,424.759f},
            {747.378f,424.439f},{748.500f,424.118f},{749.462f,423.958f},
            {750.424f,423.797f},{751.386f,423.477f},{752.348f,423.477f},
            {753.150f,423.477f},{754.112f,423.477f},{754.914f,423.637f},
            {756.036f,423.637f},{757.319f,423.958f},{758.442f,424.278f},
            {759.724f,424.599f},{761.167f,425.080f},{761.969f,425.561f},
            {763.252f,425.882f},{764.214f,426.523f},{765.016f,427.004f},
            {765.817f,427.646f},{766.780f,428.287f},{767.581f,428.928f},
            {768.704f,429.730f},{769.826f,430.371f},{771.109f,431.013f},
            {772.392f,431.654f},{773.033f,432.456f},{773.835f,433.258f},
            {774.957f,433.418f},{775.599f,433.739f},{775.759f,434.540f},
            {775.438f,435.182f},{774.476f,435.022f},{773.835f,434.861f},
            {773.033f,434.380f},{772.392f,433.899f},{771.430f,433.258f},
            {769.666f,432.456f},{768.864f,432.135f},{767.742f,431.815f},
            {766.619f,431.334f},{765.657f,430.692f},{764.855f,430.051f},
            {764.374f,429.249f},{763.733f,428.928f},{763.252f,428.447f},
            {762.450f,427.966f},{761.328f,427.485f},{760.366f,427.325f},
            {759.404f,427.004f},{758.281f,426.844f},{757.159f,426.523f},
            {756.036f,426.202f},{755.235f,426.202f},{754.112f,426.202f},
            {752.990f,426.363f},{751.867f,426.363f},{751.066f,426.363f},
            {750.264f,426.363f},{749.302f,426.683f},{748.179f,427.004f},
            {747.378f,427.325f},{746.576f,427.646f},{745.774f,427.966f},
            {744.972f,428.447f},{744.171f,428.768f},{743.369f,428.928f},
            {742.727f,429.409f},{741.926f,429.730f},{741.284f,430.051f},
            {740.322f,430.051f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.55f, 0.78f, 0.38f, 0.75f);
    }

    // //* 9. FLIPPER-EDGE LIGHT SHEEN 
    {
        static const float p[][2] = {
            {767.261f,464.044f},{766.619f,465.167f},{765.817f,466.450f},
            {765.016f,468.213f},{764.374f,469.977f},{764.054f,471.901f},
            {764.054f,473.344f},{763.893f,475.269f},{764.214f,477.193f},
            {765.016f,479.117f},{765.497f,480.881f},{766.138f,482.645f},
            {766.780f,484.088f},{767.581f,486.172f},{768.383f,487.615f},
            {769.345f,488.738f},{769.024f,485.852f},{768.543f,484.408f},
            {767.902f,482.965f},{767.581f,482.164f},{767.100f,480.720f},
            {766.459f,478.796f},{766.299f,477.353f},{765.978f,474.627f},
            {765.978f,473.665f},{766.780f,473.344f},{767.421f,472.863f},
            {767.581f,472.222f},{767.742f,471.581f},{767.742f,470.619f},
            {767.742f,469.015f},{767.421f,467.732f},{767.261f,467.091f},
            {767.100f,466.289f},{767.421f,465.327f},{767.421f,464.365f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.50f, 0.72f, 0.33f, 0.70f);
    }

    // //* 10. BODY SPECULAR SHEEN 
    {
        static const float p[][2] = {
            {778.485f,473.986f},{778.966f,473.505f},{779.126f,473.826f},
            {779.126f,474.467f},{778.645f,475.269f},{778.485f,476.231f},
            {778.164f,477.513f},{778.004f,478.796f},{777.683f,479.919f},
            {777.683f,481.201f},{777.843f,482.645f},{778.004f,484.088f},
            {778.164f,485.691f},{778.325f,487.776f},{778.645f,489.379f},
            {779.126f,490.983f},{779.607f,492.426f},{780.730f,494.029f},
            {781.371f,495.312f},{782.013f,496.915f},{782.814f,498.840f},
            {783.456f,500.443f},{784.418f,501.886f},{785.540f,503.650f},
            {786.823f,505.414f},{787.625f,506.697f},{788.266f,507.979f},
            {789.549f,509.423f},{790.671f,510.705f},{791.313f,511.507f},
            {792.275f,512.469f},{793.397f,513.431f},{794.359f,514.233f},
            {795.321f,515.035f},{798.208f,516.799f},{799.490f,517.921f},
            {800.773f,518.723f},{801.896f,519.685f},{803.339f,520.647f},
            {804.461f,521.449f},{805.584f,522.250f},{806.706f,523.052f},
            {807.347f,523.533f},{806.225f,523.533f},{805.102f,523.212f},
            {804.140f,522.571f},{803.018f,521.930f},{801.415f,520.647f},
            {799.811f,519.364f},{798.368f,518.723f},{797.406f,517.600f},
            {796.444f,516.638f},{795.321f,515.836f},{794.520f,515.195f},
            {793.718f,514.714f},{792.916f,514.233f},{792.275f,513.431f},
            {791.473f,512.790f},{790.832f,512.148f},{790.190f,510.866f},
            {789.228f,510.224f},{788.587f,509.262f},{787.945f,508.621f},
            {787.144f,507.498f},{786.502f,506.376f},{785.861f,505.414f},
            {785.059f,504.933f},{784.899f,503.971f},{784.257f,503.329f},
            {783.295f,502.367f},{782.975f,501.566f},{782.333f,499.962f},
            {781.692f,499.000f},{781.371f,497.717f},{780.890f,496.915f},
            {780.409f,495.633f},{780.088f,494.671f},{779.607f,493.869f},
            {779.126f,493.067f},{778.806f,492.265f},{778.485f,491.303f},
            {778.325f,490.502f},{778.004f,489.540f},{777.843f,488.577f},
            {777.683f,487.455f},{777.362f,486.653f},{777.362f,485.691f},
            {777.362f,484.729f},{777.202f,483.607f},{777.042f,482.645f},
            {776.881f,481.683f},{777.042f,481.041f},{776.881f,479.919f},
            {777.202f,478.957f},{777.362f,477.995f},{777.523f,477.193f},
            {777.683f,476.391f},{777.843f,475.429f},{778.164f,474.788f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.90f, 0.95f, 0.80f, 0.30f);
    }

    // //? 11. BODY SPOTS (shell texture dots) 
    //* spot 1 
    {
        static const float p[][2] = {
            {760.366f,429.890f},{760.366f,428.928f},{760.526f,428.127f},
            {761.488f,428.447f},{762.129f,428.768f},{762.611f,429.249f},
            {762.931f,429.890f},{763.092f,430.371f},{762.931f,431.013f},
            {762.611f,431.654f},{761.648f,431.654f},{760.847f,431.173f},
            {760.526f,430.692f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.20f, 0.42f, 0.12f, 0.80f);
    }
    //* spot 2 
    {
        static const float p[][2] = {
            {768.543f,433.418f},{769.505f,433.418f},{770.307f,433.578f},
            {770.628f,433.899f},{770.628f,434.540f},{770.147f,435.503f},
            {769.826f,436.144f},{768.864f,436.465f},{768.383f,435.342f},
            {768.223f,434.701f},{768.223f,434.220f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.20f, 0.42f, 0.12f, 0.80f);
    }
    //* spot 3 
    {
        static const float p[][2] = {
            {763.573f,435.022f},{764.214f,435.182f},{764.695f,435.663f},
            {764.695f,436.304f},{764.214f,436.785f},{763.412f,436.625f},
            {763.252f,435.984f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.20f, 0.42f, 0.12f, 0.80f);
    }
    //* spot 4 — single point 
    {
        static const float pts[][2] = {
            {770.468f,440.313f},{770.307f,441.115f}
        };
        int np = (int)(sizeof(pts) / sizeof(pts[0]));
        drawPointCloud(pts, np, 0.20f, 0.42f, 0.12f, 4.0f);
    }
    //* spot 5 
    {
        static const float p[][2] = {
            {782.527f,483.083f},{783.548f,483.175f},{784.476f,484.011f},
            {784.661f,485.495f},{784.104f,486.052f},{783.455f,486.423f},
            {782.434f,486.609f},{781.970f,486.609f},{781.135f,486.516f},
            {780.949f,485.866f},{780.578f,485.495f},{780.393f,484.846f},
            {780.393f,484.196f},{780.949f,484.011f},{781.321f,483.639f},
            {781.877f,483.547f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.20f, 0.42f, 0.12f, 0.80f);
    }

    //* 12. EYE 
    {
        static const float p[][2] = {
            {752.348f,428.768f},{752.990f,428.287f},{753.631f,427.966f},
            {754.754f,427.806f},{755.716f,427.806f},{756.357f,428.127f},
            {756.838f,428.928f},{757.319f,429.730f},{757.640f,430.371f},
            {757.640f,431.173f},{757.640f,431.975f},{757.319f,432.777f},
            {756.838f,433.578f},{755.876f,434.380f},{755.235f,434.861f},
            {754.433f,435.342f},{753.471f,435.503f},{752.669f,435.182f},
            {752.188f,434.861f},{752.188f,434.059f},{752.188f,433.258f},
            {752.188f,432.616f},{751.867f,431.975f},{751.707f,431.334f},
            {751.707f,430.853f},{751.867f,430.371f},{752.028f,429.570f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.08f, 0.12f, 0.08f, 1.00f);
    }

    //* 13. EYE WHITE BALL 
    {
        static const float p[][2] = {
            {755.074f,429.249f},{755.716f,429.730f},{756.036f,430.051f},
            {756.036f,430.692f},{756.036f,431.494f},{755.716f,431.975f},
            {754.914f,431.975f},{754.112f,431.975f},{753.952f,431.334f},
            {753.952f,430.853f},{753.791f,430.371f},{753.631f,430.051f},
            {753.952f,429.570f},{754.433f,429.409f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.95f, 0.95f, 0.95f, 1.00f);
    }

    //? 14. CARAPACE SCUTES (yellow shell plates with dotted edge) 
    //* scute A — large central plate 
    {
        static const float p[][2] = {
            {778.487f,434.559f},{780.252f,433.617f},{782.606f,432.087f},
            {785.078f,431.263f},{787.079f,430.910f},{789.315f,430.086f},
            {791.316f,429.615f},{793.553f,428.792f},{794.612f,428.438f},
            {796.142f,427.732f},{797.908f,427.261f},{798.849f,427.144f},
            {800.379f,427.026f},{801.556f,426.791f},{803.557f,426.202f},
            {805.441f,425.731f},{807.442f,425.260f},{808.736f,425.260f},
            {810.031f,425.260f},{811.208f,425.260f},{812.267f,425.260f},
            {813.327f,425.260f},{814.857f,425.260f},{816.269f,425.025f},
            {818.035f,425.143f},{819.212f,425.143f},{820.389f,425.260f},
            {821.331f,425.378f},{822.508f,425.496f},{823.802f,425.614f},
            {825.215f,425.849f},{826.863f,425.967f},{828.628f,426.202f},
            {829.687f,426.555f},{830.982f,426.791f},{832.630f,427.026f},
            {834.042f,427.732f},{835.455f,428.085f},{836.514f,428.556f},
            {837.927f,428.909f},{839.104f,429.380f},{840.752f,429.851f},
            {842.517f,430.557f},{843.576f,430.910f},{844.989f,431.499f},
            {846.284f,431.970f},{847.578f,432.676f},{848.755f,433.147f},
            {849.697f,433.617f},{850.874f,434.088f},{852.051f,434.794f},
            {853.110f,435.618f},{854.052f,436.325f},{855.111f,436.913f},
            {855.818f,437.737f},{856.759f,438.443f},{857.818f,438.914f},
            {858.760f,439.620f},{859.819f,440.562f},{860.879f,441.503f},
            {861.585f,442.092f},{862.527f,442.798f},{863.704f,443.504f},
            {864.998f,444.446f},{866.293f,445.976f},{867.470f,447.035f},
            {868.647f,448.213f},{870.177f,449.743f},{871.237f,450.802f},
            {872.531f,452.214f},{873.826f,453.627f},{875.003f,455.157f},
            {876.062f,456.099f},{877.239f,457.629f},{878.181f,458.688f},
            {879.123f,460.571f},{880.064f,461.513f},{881.124f,462.455f},
            {881.948f,463.514f},{882.771f,464.809f},{883.831f,465.986f},
            {885.008f,467.751f},{885.832f,469.046f},{886.538f,470.458f},
            {887.715f,471.988f},{888.304f,473.166f},{889.245f,474.343f},
            {889.834f,475.049f},{890.422f,475.990f},{890.893f,477.167f},
            {891.011f,478.109f},{891.246f,479.051f},{891.011f,480.110f},
            {890.775f,481.169f},{889.363f,482.817f},{888.068f,483.641f},
            {886.656f,484.347f},{885.243f,484.936f},{883.360f,485.642f},
            {881.948f,486.231f},{880.417f,486.466f},{878.534f,486.819f},
            {876.298f,487.408f},{874.415f,487.996f},{872.531f,488.349f},
            {869.589f,488.349f},{867.706f,488.702f},{866.528f,488.820f},
            {865.234f,489.055f},{864.174f,489.173f},{862.644f,489.409f},
            {860.996f,489.644f},{859.231f,489.762f},{857.230f,489.879f},
            {855.111f,489.997f},{852.640f,489.997f},{850.403f,489.997f},
            {847.931f,490.115f},{845.577f,489.762f},{843.812f,489.644f},
            {841.693f,489.173f},{839.928f,489.055f},{838.044f,488.702f},
            {836.161f,488.467f},{834.631f,488.231f},{832.395f,487.643f},
            {830.982f,487.290f},{829.217f,486.701f},{827.569f,486.113f},
            {826.274f,485.760f},{824.155f,485.053f},{822.390f,484.347f},
            {820.624f,483.994f},{819.565f,483.523f},{818.388f,482.935f},
            {816.858f,482.346f},{815.092f,481.522f},{813.562f,480.816f},
            {812.503f,480.228f},{811.090f,479.521f},{810.031f,478.933f},
            {809.089f,477.874f},{808.148f,477.050f},{807.206f,476.461f},
            {806.265f,475.520f},{805.323f,474.225f},{804.617f,473.283f},
            {803.557f,472.577f},{802.616f,471.635f},{802.027f,470.694f},
            {801.086f,469.634f},{800.144f,468.575f},{799.202f,467.398f},
            {798.143f,466.574f},{797.319f,465.986f},{796.731f,465.515f},
            {795.789f,464.809f},{794.847f,464.338f},{794.259f,463.985f},
            {793.435f,463.514f},{792.611f,462.925f},{791.905f,462.455f},
            {790.610f,461.513f},{789.786f,460.689f},{789.080f,459.747f},
            {788.256f,459.159f},{787.314f,458.100f},{786.491f,456.923f},
            {785.784f,455.981f},{784.725f,455.039f},{783.901f,453.745f},
            {783.430f,452.921f},{782.842f,451.626f},{782.724f,450.920f},
            {782.842f,450.096f},{782.724f,449.154f},{782.606f,447.977f},
            {782.606f,447.153f},{782.842f,446.447f},{782.489f,445.623f},
            {782.489f,444.564f},{782.489f,443.504f},{782.489f,442.445f},
            {782.253f,441.268f},{781.665f,440.209f},{780.841f,439.385f},
            {780.017f,438.325f},{779.428f,437.502f},{778.604f,435.971f},
            {778.369f,435.147f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.77f, 0.706f, 0.271f, 1.00f);
        drawPointCloud(p, n, 0.416f, 0.478f, 0.161f, 2.8f);
    }
    //* scute B 
    {
        static const float p[][2] = {
            {778.487f,434.559f},{779.075f,435.030f},{780.135f,433.970f},
            {781.076f,433.617f},{781.900f,433.147f},{782.724f,432.676f},
            {783.666f,432.205f},{784.843f,431.852f},{785.549f,431.734f},
            {786.373f,431.970f},{786.961f,433.029f},{787.903f,434.324f},
            {788.491f,435.383f},{788.845f,436.560f},{789.315f,438.090f},
            {789.786f,439.267f},{790.257f,440.797f},{790.728f,442.327f},
            {791.199f,444.211f},{791.552f,445.388f},{791.905f,446.565f},
            {792.376f,448.330f},{792.376f,449.507f},{792.140f,450.449f},
            {791.787f,451.155f},{790.963f,451.273f},{790.022f,451.155f},
            {789.315f,451.155f},{788.374f,451.390f},{787.432f,452.097f},
            {786.726f,452.921f},{786.137f,453.980f},{785.549f,454.451f},
            {784.725f,454.922f},{784.136f,454.333f},{783.783f,452.921f},
            {783.430f,452.214f},{782.959f,450.920f},{782.959f,449.743f},
            {782.371f,448.566f},{782.489f,447.624f},{782.606f,447.035f},
            {782.724f,445.976f},{782.842f,445.035f},{782.842f,443.857f},
            {782.724f,443.269f},{782.606f,442.092f},{782.371f,441.386f},
            {782.136f,440.797f},{781.900f,440.209f},{781.547f,439.620f},
            {781.076f,438.914f},{780.370f,437.972f},{779.428f,436.913f},
            {778.958f,436.325f},{778.487f,435.736f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.77f, 0.706f, 0.271f, 1.00f);
        drawPointCloud(p, n, 0.416f, 0.478f, 0.161f, 2.8f);
    }
    //* scute C 
    {
        static const float p[][2] = {
            {785.455f,454.239f},{786.302f,453.533f},{787.150f,452.685f},
            {787.997f,451.979f},{789.127f,451.697f},{790.257f,451.414f},
            {791.246f,451.273f},{792.376f,451.979f},{793.364f,452.544f},
            {794.353f,453.109f},{795.342f,453.674f},{796.613f,454.380f},
            {797.460f,455.228f},{798.732f,456.216f},{799.720f,457.205f},
            {800.285f,457.770f},{800.144f,458.617f},{799.720f,459.606f},
            {799.438f,460.595f},{798.732f,461.725f},{798.449f,462.996f},
            {797.602f,464.550f},{795.907f,464.408f},{794.777f,463.702f},
            {793.223f,462.431f},{792.093f,461.442f},{790.963f,460.171f},
            {789.692f,459.182f},{788.280f,457.770f},{787.150f,456.640f},
            {786.020f,455.793f},{785.031f,455.086f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.77f, 0.706f, 0.271f, 1.00f);
        drawPointCloud(p, n, 0.416f, 0.478f, 0.161f, 2.8f);
    }
    //* scute D 
    {
        static const float p[][2] = {
            {797.743f,464.126f},{798.025f,463.278f},{798.167f,462.572f},
            {798.449f,461.725f},{799.297f,459.747f},{800.003f,458.759f},
            {800.285f,458.194f},{800.992f,458.476f},{802.121f,459.465f},
            {802.686f,459.889f},{803.251f,460.454f},{803.675f,461.160f},
            {804.523f,462.007f},{805.229f,462.431f},{806.218f,463.137f},
            {806.924f,463.843f},{807.630f,464.973f},{808.619f,466.245f},
            {809.466f,467.092f},{810.031f,467.798f},{810.314f,468.222f},
            {809.890f,469.352f},{809.325f,470.341f},{808.901f,471.471f},
            {808.619f,472.459f},{807.912f,473.730f},{807.347f,474.154f},
            {806.359f,473.448f},{805.653f,472.459f},{804.664f,471.471f},
            {803.393f,470.341f},{802.686f,469.634f},{801.980f,468.787f},
            {801.133f,467.798f},{800.427f,467.092f},{799.297f,466.103f},
            {798.167f,465.397f},{797.319f,464.832f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.77f, 0.706f, 0.271f, 1.00f);
        drawPointCloud(p, n, 0.416f, 0.478f, 0.161f, 2.8f);
    }
    //* scute E 
    {
        static const float p[][2] = {
            {806.924f,474.437f},{807.206f,473.589f},{807.630f,472.601f},
            {808.195f,471.753f},{808.901f,470.623f},{809.325f,469.776f},
            {809.890f,468.787f},{810.455f,468.504f},{811.302f,469.211f},
            {812.009f,469.634f},{812.573f,470.341f},{813.703f,470.906f},
            {814.975f,471.753f},{815.963f,472.459f},{817.235f,473.166f},
            {818.364f,474.013f},{819.353f,474.578f},{820.342f,475.002f},
            {821.472f,475.567f},{822.743f,476.273f},{823.732f,476.838f},
            {824.862f,477.262f},{826.274f,478.109f},{827.687f,478.674f},
            {828.534f,479.098f},{828.110f,480.228f},{827.969f,481.499f},
            {827.969f,482.629f},{827.545f,484.041f},{827.263f,484.747f},
            {826.133f,484.606f},{825.144f,484.182f},{821.613f,482.911f},
            {821.048f,482.488f},{819.353f,481.781f},{818.223f,481.216f},
            {817.093f,480.651f},{816.105f,480.086f},{814.975f,479.239f},
            {813.986f,478.956f},{812.432f,478.392f},{811.302f,477.685f},
            {810.314f,477.120f},{809.466f,476.555f},{808.336f,475.990f},
            {807.347f,475.425f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.77f, 0.706f, 0.271f, 1.00f);
        drawPointCloud(p, n, 0.416f, 0.478f, 0.161f, 2.8f);
    }
    //* scute F 
    {
        static const float p[][2] = {
            {827.828f,484.747f},{827.828f,483.759f},{827.969f,482.770f},
            {828.110f,482.205f},{828.252f,481.358f},{828.393f,480.510f},
            {828.675f,479.663f},{828.958f,478.956f},{829.805f,479.098f},
            {830.794f,479.521f},{831.641f,479.945f},{832.630f,479.945f},
            {834.042f,480.228f},{835.314f,480.651f},{836.302f,480.793f},
            {837.150f,480.934f},{837.997f,480.934f},{839.268f,481.075f},
            {840.681f,481.075f},{841.670f,481.075f},{842.800f,481.075f},
            {843.647f,481.075f},{844.777f,481.358f},{845.624f,481.499f},
            {846.048f,482.488f},{846.048f,483.618f},{846.189f,484.889f},
            {846.472f,485.877f},{846.331f,487.007f},{846.331f,488.279f},
            {845.624f,488.420f},{844.495f,488.702f},{843.223f,488.702f},
            {842.376f,488.561f},{841.246f,488.420f},{840.257f,488.137f},
            {838.845f,487.996f},{837.432f,487.714f},{836.161f,487.572f},
            {834.890f,487.290f},{833.760f,487.149f},{832.771f,487.149f},
            {831.783f,486.725f},{830.653f,486.584f},{829.381f,486.442f},
            {828.252f,486.160f},{827.687f,485.595f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.77f, 0.706f, 0.271f, 1.00f);
        drawPointCloud(p, n, 0.416f, 0.478f, 0.161f, 2.8f);
    }
    //* scute G 
    {
        static const float p[][2] = {
            {846.331f,482.629f},{847.461f,482.488f},{848.591f,482.629f},
            {849.721f,482.911f},{850.850f,482.911f},{851.839f,482.911f},
            {852.828f,482.911f},{853.675f,483.053f},{854.664f,482.911f},
            {855.794f,482.911f},{856.924f,482.911f},{858.336f,482.911f},
            {859.890f,482.629f},{861.020f,482.770f},{862.009f,482.770f},
            {862.715f,482.770f},{863.421f,482.770f},{863.845f,482.770f},
            {863.986f,483.618f},{864.127f,487.855f},{863.280f,488.702f},
            {862.009f,488.844f},{860.737f,488.844f},{858.195f,488.702f},
            {857.489f,488.844f},{856.500f,488.844f},{855.370f,488.702f},
            {854.240f,488.702f},{853.252f,488.702f},{851.980f,488.702f},
            {850.709f,488.844f},{849.297f,488.702f},{848.026f,488.702f},
            {847.037f,488.702f},{846.472f,488.702f},{846.048f,483.335f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.77f, 0.706f, 0.271f, 1.00f);
        drawPointCloud(p, n, 0.416f, 0.478f, 0.161f, 2.8f);
    }
    //* scute H (small) 
    {
        static const float p[][2] = {
            {864.834f,483.053f},{872.602f,481.781f},{874.156f,486.584f},
            {864.269f,488.279f},{864.127f,483.335f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.77f, 0.706f, 0.271f, 1.00f);
        drawPointCloud(p, n, 0.416f, 0.478f, 0.161f, 2.8f);
    }
    //* scute I 
    {
        static const float p[][2] = {
            {873.732f,481.640f},{874.579f,481.358f},{875.709f,480.934f},
            {876.557f,480.793f},{877.263f,480.793f},{877.828f,480.369f},
            {878.817f,479.804f},{879.523f,479.380f},{880.229f,479.098f},
            {880.935f,478.815f},{881.924f,478.533f},{882.348f,479.239f},
            {882.771f,480.369f},{882.913f,481.075f},{883.195f,481.781f},
            {883.478f,482.770f},{883.760f,483.900f},{883.195f,484.465f},
            {882.206f,485.030f},{881.642f,485.312f},{881.077f,485.736f},
            {880.229f,486.019f},{879.523f,486.442f},{878.393f,486.725f},
            {876.839f,486.866f},{875.709f,487.007f},{874.297f,487.290f},
            {874.014f,486.019f},{873.873f,484.324f},{873.732f,483.335f},
            {873.591f,482.629f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.77f, 0.706f, 0.271f, 1.00f);
        drawPointCloud(p, n, 0.416f, 0.478f, 0.161f, 2.8f);
    }
    //* scute J 
    {
        static const float p[][2] = {
            {883.336f,481.499f},{883.336f,480.228f},{883.760f,479.098f},
            {884.325f,478.533f},{885.031f,477.968f},{885.596f,477.403f},
            {885.879f,476.979f},{886.868f,476.414f},{887.715f,476.132f},
            {888.280f,475.990f},{888.845f,476.555f},{889.410f,477.120f},
            {889.834f,477.685f},{890.257f,478.533f},{890.399f,479.521f},
            {889.975f,480.510f},{889.269f,481.358f},{888.704f,482.346f},
            {887.715f,482.911f},{886.726f,483.618f},{885.879f,484.041f},
            {884.466f,484.465f},{883.619f,483.759f},{883.195f,483.053f},
            {883.054f,482.488f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.77f, 0.706f, 0.271f, 1.00f);
        drawPointCloud(p, n, 0.416f, 0.478f, 0.161f, 2.8f);
    }

    //? 15. BACK PATTERN PLATES (olive-brown scutes on neck/back) 
    //* back plate A 
    {
        static const float p[][2] = {
            {788.845f,432.323f},{789.433f,431.852f},{790.139f,431.852f},
            {790.846f,431.499f},{791.552f,431.381f},{792.376f,431.028f},
            {794.141f,430.439f},{795.671f,429.851f},{796.378f,429.733f},
            {796.848f,429.615f},{797.908f,429.498f},{798.732f,429.145f},
            {799.320f,429.145f},{800.026f,429.027f},{800.968f,428.909f},
            {802.027f,428.674f},{802.969f,428.556f},{804.264f,428.438f},
            {805.676f,428.321f},{806.735f,428.321f},{807.324f,428.085f},
            {807.559f,428.909f},{807.559f,429.969f},{807.677f,430.675f},
            {807.677f,431.381f},{807.442f,432.558f},{807.442f,434.088f},
            {807.559f,435.147f},{807.442f,436.089f},{807.442f,437.031f},
            {806.735f,437.619f},{805.676f,438.208f},{804.852f,438.914f},
            {804.028f,439.267f},{802.263f,440.091f},{801.203f,440.326f},
            {800.026f,441.033f},{799.085f,441.503f},{798.143f,442.210f},
            {797.319f,442.680f},{796.613f,443.034f},{795.436f,443.504f},
            {794.494f,443.857f},{793.670f,444.211f},{793.317f,443.034f},
            {793.200f,441.974f},{793.082f,441.150f},{792.846f,440.326f},
            {792.611f,439.267f},{792.258f,438.325f},{792.023f,437.384f},
            {791.669f,436.560f},{791.199f,435.501f},{790.963f,434.912f},
            {790.139f,434.324f},{789.786f,434.206f},{789.433f,434.088f},
            {789.080f,433.500f},{788.845f,433.029f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.58f, 0.537f, 0.247f, 1.00f);
    }
    //* back plate B 
    {
        static const float p[][2] = {
            {809.443f,428.321f},{810.620f,428.203f},{811.797f,427.850f},
            {813.209f,427.732f},{814.386f,427.732f},{815.681f,427.615f},
            {817.446f,427.497f},{819.212f,427.497f},{820.271f,427.497f},
            {821.331f,427.615f},{822.508f,427.850f},{823.449f,427.850f},
            {824.744f,427.850f},{825.921f,428.203f},{826.980f,428.556f},
            {828.275f,428.909f},{829.334f,429.262f},{830.394f,429.733f},
            {831.218f,430.204f},{831.100f,431.263f},{830.982f,432.676f},
            {831.100f,433.970f},{831.218f,435.501f},{831.335f,436.913f},
            {831.218f,438.443f},{831.218f,439.856f},{830.982f,440.915f},
            {830.865f,441.857f},{829.923f,442.210f},{828.746f,442.445f},
            {827.804f,442.798f},{826.745f,443.269f},{825.686f,443.740f},
            {825.215f,444.211f},{824.509f,444.681f},{823.802f,445.270f},
            {823.214f,445.623f},{822.625f,445.976f},{821.801f,446.682f},
            {821.213f,447.035f},{820.389f,446.212f},{819.683f,445.270f},
            {819.212f,444.446f},{818.506f,443.504f},{817.917f,443.034f},
            {817.211f,442.445f},{816.622f,441.621f},{816.152f,441.033f},
            {815.445f,440.209f},{814.857f,439.502f},{814.033f,438.679f},
            {813.327f,437.855f},{812.503f,437.148f},{811.679f,436.795f},
            {811.679f,435.971f},{811.561f,435.147f},{811.326f,434.324f},
            {810.973f,433.617f},{810.267f,432.087f},{810.149f,431.263f},
            {810.149f,430.557f},{810.031f,429.969f},{809.678f,429.027f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.58f, 0.537f, 0.247f, 1.00f);
    }
    //* back plate C 
    {
        static const float p[][2] = {
            {831.218f,446.094f},{832.159f,446.800f},{833.454f,448.213f},
            {834.984f,449.860f},{836.043f,451.155f},{836.985f,452.450f},
            {838.397f,453.862f},{839.810f,455.157f},{840.752f,456.452f},
            {841.105f,457.040f},{841.222f,459.630f},{840.987f,462.808f},
            {841.222f,464.220f},{841.575f,465.868f},{841.811f,467.163f},
            {841.811f,468.811f},{842.164f,470.223f},{842.164f,471.988f},
            {842.399f,473.754f},{842.399f,475.637f},{842.282f,477.285f},
            {842.282f,478.933f},{842.399f,479.875f},{840.163f,479.521f},
            {838.044f,479.521f},{835.573f,479.051f},{831.924f,478.227f},
            {829.805f,477.521f},{827.687f,476.579f},{826.156f,475.637f},
            {824.509f,474.813f},{822.743f,473.636f},{821.331f,472.812f},
            {820.154f,472.342f},{818.741f,471.518f},{817.917f,470.694f},
            {817.211f,469.988f},{816.740f,469.634f},{818.153f,465.279f},
            {819.330f,459.277f},{819.800f,458.570f},{820.154f,457.982f},
            {820.624f,456.099f},{821.095f,453.980f},{821.331f,452.568f},
            {821.331f,451.861f},{821.213f,450.920f},{830.041f,446.212f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.58f, 0.537f, 0.247f, 1.00f);
    }
    //* back plate D 
    {
        static const float p[][2] = {
            {834.866f,430.792f},{836.750f,431.263f},{837.809f,431.616f},
            {838.633f,432.087f},{839.692f,432.558f},{840.987f,432.911f},
            {842.635f,433.617f},{843.930f,434.324f},{844.989f,434.794f},
            {846.519f,435.501f},{848.402f,436.442f},{850.050f,437.266f},
            {851.698f,437.855f},{853.110f,438.443f},{854.052f,439.149f},
            {854.052f,440.562f},{854.405f,441.621f},{854.405f,443.034f},
            {854.640f,444.328f},{854.523f,445.505f},{854.523f,446.447f},
            {854.287f,447.389f},{854.170f,448.448f},{853.581f,449.272f},
            {851.698f,450.213f},{850.168f,450.684f},{849.344f,451.626f},
            {848.167f,452.097f},{847.108f,452.685f},{845.813f,453.156f},
            {844.871f,453.627f},{844.047f,453.627f},{842.870f,452.685f},
            {842.399f,451.861f},{841.811f,450.449f},{841.105f,449.743f},
            {840.045f,448.801f},{839.457f,447.977f},{838.868f,447.271f},
            {838.044f,446.329f},{837.103f,445.505f},{836.279f,445.035f},
            {835.808f,444.446f},{834.984f,443.387f},{834.631f,442.563f},
            {834.396f,438.443f},{834.278f,435.618f},{834.160f,433.735f},
            {834.160f,432.558f},{834.160f,431.616f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.58f, 0.537f, 0.247f, 1.00f);
    }
    //* back plate E 
    {
        static const float p[][2] = {
            {854.170f,453.156f},{854.994f,454.568f},{855.935f,455.863f},
            {856.877f,457.040f},{857.583f,458.100f},{858.525f,459.277f},
            {859.231f,460.454f},{859.819f,461.631f},{860.526f,462.808f},
            {860.996f,464.102f},{861.350f,465.279f},{861.585f,466.221f},
            {861.703f,467.869f},{862.527f,469.752f},{862.644f,470.929f},
            {863.115f,472.106f},{863.586f,473.754f},{863.704f,474.931f},
            {864.174f,476.579f},{864.410f,478.109f},{864.528f,479.051f},
            {864.410f,480.110f},{862.880f,480.463f},{861.467f,480.345f},
            {859.937f,480.463f},{858.407f,480.463f},{856.641f,480.228f},
            {855.818f,480.345f},{854.758f,480.463f},{853.699f,480.463f},
            {852.286f,480.345f},{850.874f,479.992f},{849.697f,479.757f},
            {848.638f,479.639f},{847.461f,479.404f},{846.519f,479.286f},
            {845.577f,478.933f},{845.342f,458.453f},{848.402f,456.099f},
            {850.050f,455.275f},{851.345f,454.451f},{852.640f,453.862f},
            {853.581f,453.391f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.58f, 0.537f, 0.247f, 1.00f);
    }
    //* back plate F 
    {
        static const float p[][2] = {
            {858.172f,441.386f},{859.466f,442.445f},{860.290f,443.151f},
            {860.996f,443.857f},{861.703f,444.328f},{862.409f,444.799f},
            {863.351f,445.623f},{864.174f,446.447f},{864.998f,447.506f},
            {866.058f,448.448f},{866.999f,449.272f},{867.941f,449.978f},
            {869.000f,450.920f},{869.824f,452.097f},{870.648f,453.509f},
            {871.707f,454.804f},{872.531f,455.628f},{873.120f,456.216f},
            {873.238f,456.687f},{872.884f,457.040f},{871.943f,457.511f},
            {871.472f,457.511f},{869.824f,458.217f},{868.647f,458.923f},
            {867.941f,459.277f},{866.764f,459.865f},{865.705f,460.454f},
            {865.116f,460.807f},{864.645f,461.160f},{864.410f,460.218f},
            {863.115f,459.041f},{862.291f,457.629f},{861.350f,456.569f},
            {860.173f,455.039f},{859.113f,453.862f},{857.936f,452.568f},
            {856.995f,451.508f},{857.230f,441.857f},{857.112f,441.033f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.58f, 0.537f, 0.247f, 1.00f);
    }
    //* back plate G 
    {
        static const float p[][2] = {
            {865.822f,464.455f},{867.235f,463.985f},{869.000f,463.396f},
            {870.060f,462.690f},{871.354f,462.219f},{872.296f,461.748f},
            {873.473f,460.924f},{873.944f,460.218f},{874.532f,459.277f},
            {875.474f,458.570f},{875.945f,459.277f},{876.298f,459.983f},
            {877.239f,461.395f},{877.946f,462.455f},{878.887f,463.632f},
            {879.829f,464.926f},{880.888f,466.221f},{881.830f,467.280f},
            {882.418f,468.222f},{883.478f,469.752f},{884.537f,471.518f},
            {885.361f,472.695f},{885.714f,473.519f},{885.832f,474.225f},
            {884.655f,474.813f},{882.065f,475.990f},{880.535f,476.461f},
            {879.358f,477.050f},{878.181f,477.521f},{877.004f,478.109f},
            {875.356f,478.227f},{873.826f,478.698f},{872.414f,478.815f},
            {870.883f,479.168f},{869.589f,479.286f},{868.294f,479.521f},
            {867.706f,479.286f},{867.352f,478.109f},{867.470f,476.461f},
            {867.352f,475.049f},{867.235f,473.754f},{867.235f,472.459f},
            {866.646f,470.929f},{866.293f,469.517f},{865.822f,468.340f},
            {865.587f,467.398f},{865.234f,466.221f},{864.881f,465.279f},
            {864.763f,464.809f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.58f, 0.537f, 0.247f, 1.00f);
    }
    //* back plate H 
    {
        static const float p[][2] = {
            {808.972f,440.091f},{809.560f,439.973f},{809.913f,439.973f},
            {810.502f,440.562f},{810.973f,441.150f},{811.561f,441.857f},
            {812.385f,442.680f},{813.562f,444.093f},{814.739f,445.505f},
            {816.152f,447.035f},{816.976f,448.213f},{817.917f,449.272f},
            {818.035f,450.213f},{817.917f,451.037f},{817.799f,452.214f},
            {817.917f,453.509f},{817.799f,454.686f},{817.329f,456.569f},
            {816.976f,458.335f},{816.387f,459.983f},{816.034f,461.748f},
            {815.445f,463.632f},{814.857f,465.633f},{814.386f,467.045f},
            {814.033f,468.222f},{811.914f,467.045f},{810.620f,465.515f},
            {809.796f,464.691f},{808.619f,463.514f},{807.559f,462.101f},
            {806.618f,461.042f},{805.441f,459.512f},{804.028f,458.335f},
            {802.498f,457.158f},{801.321f,455.981f},{800.144f,455.039f},
            {798.967f,453.980f},{798.025f,453.038f},{796.848f,452.097f},
            {795.907f,451.390f},{794.965f,450.567f},{794.141f,449.860f},
            {794.259f,449.036f},{794.377f,447.859f},{795.554f,447.035f},
            {796.848f,446.682f},{797.908f,446.212f},{799.438f,445.388f},
            {800.968f,444.564f},{802.380f,443.622f},{803.440f,442.916f},
            {804.734f,442.327f},{805.912f,441.739f},{807.206f,441.033f},
            {808.148f,440.444f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledPolygon(p, n, 0.58f, 0.537f, 0.247f, 1.00f);
    }

    //* 16. OUTER BODY OUTLINE 
    {
        static const float p[][2] = {
            {736.394f,441.622f},{736.260f,439.217f},{736.260f,437.614f},
            {736.127f,436.010f},{736.260f,434.808f},{736.394f,433.605f},
            {736.795f,432.670f},{737.329f,430.933f},{737.864f,429.329f},
            {738.398f,428.127f},{739.066f,427.058f},{739.868f,425.989f},
            {740.803f,425.053f},{741.739f,424.118f},{742.808f,423.450f},
            {743.877f,422.915f},{745.213f,422.381f},{746.415f,421.846f},
            {748.286f,421.312f},{749.622f,421.045f},{751.092f,420.911f},
            {752.562f,420.777f},{754.299f,420.644f},{755.769f,420.644f},
            {756.838f,420.777f},{758.308f,421.045f},{759.911f,421.312f},
            {761.515f,421.846f},{763.118f,422.782f},{764.989f,423.583f},
            {767.127f,424.652f},{768.730f,425.454f},{770.200f,426.523f},
            {772.071f,427.726f},{773.674f,429.062f},{775.412f,430.532f},
            {777.015f,431.868f},{778.351f,432.670f},{779.019f,433.472f},
            {784.364f,431.066f},{789.576f,429.730f},{793.183f,428.394f},
            {795.989f,427.325f},{799.731f,426.390f},{803.339f,425.721f},
            {806.946f,425.321f},{810.020f,425.321f},{813.360f,425.321f},
            {815.899f,425.187f},{818.839f,425.187f},{822.714f,425.321f},
            {825.520f,425.454f},{828.460f,426.122f},{832.068f,426.924f},
            {834.606f,427.458f},{837.145f,428.394f},{839.684f,429.463f},
            {843.025f,430.532f},{845.697f,431.601f},{848.236f,432.670f},
            {850.908f,434.006f},{854.650f,435.743f},{856.921f,437.213f},
            {859.727f,439.217f},{862.133f,440.821f},{864.003f,442.023f},
            {866.008f,443.760f},{867.611f,445.097f},{869.348f,446.700f},
            {871.085f,448.571f},{872.822f,450.442f},{874.559f,452.179f},
            {876.430f,454.317f},{878.301f,456.855f},{880.172f,459.127f},
            {881.641f,461.265f},{883.379f,463.537f},{885.116f,465.140f},
            {886.185f,466.877f},{887.788f,469.149f},{888.991f,471.287f},
            {890.594f,473.959f},{891.930f,475.429f},{892.599f,476.899f},
            {893.133f,477.701f},{895.271f,477.834f},{896.874f,477.968f},
            {898.745f,478.502f},{900.215f,478.770f},{901.551f,479.571f},
            {902.620f,480.106f},{903.288f,480.373f},{903.021f,481.709f},
            {902.219f,482.377f},{901.418f,483.179f},{900.349f,483.847f},
            {899.413f,484.382f},{898.612f,484.783f},{897.810f,484.783f},
            {897.142f,484.916f},{896.340f,485.451f},{895.137f,485.718f},
            {892.732f,485.852f},{889.926f,485.718f},{888.723f,485.852f},
            {887.387f,485.985f},{886.185f,486.386f},{885.383f,486.921f},
            {884.982f,487.722f},{885.116f,488.791f},{885.650f,489.593f},
            {887.521f,490.796f},{889.258f,491.464f},{890.594f,492.132f},
            {892.064f,493.468f},{893.534f,494.537f},{895.137f,496.140f},
            {896.073f,497.209f},{897.275f,498.278f},{898.344f,499.481f},
            {899.814f,501.085f},{901.017f,502.554f},{902.086f,503.891f},
            {903.021f,505.093f},{903.956f,506.296f},{904.892f,507.365f},
            {905.025f,512.442f},{904.491f,513.645f},{902.754f,514.046f},
            {900.081f,514.046f},{898.478f,514.180f},{896.607f,514.180f},
            {895.004f,514.313f},{892.866f,514.313f},{891.129f,514.046f},
            {889.258f,514.046f},{887.654f,513.912f},{886.185f,513.779f},
            {884.715f,513.244f},{883.379f,512.576f},{882.042f,512.175f},
            {880.706f,511.373f},{879.370f,510.304f},{878.435f,509.235f},
            {877.366f,508.300f},{876.163f,507.365f},{875.094f,506.162f},
            {874.426f,506.029f},{873.491f,505.895f},{872.288f,505.895f},
            {870.417f,505.761f},{868.814f,505.227f},{867.210f,504.692f},
            {865.874f,503.891f},{864.805f,503.089f},{863.335f,502.287f},
            {862.133f,501.352f},{860.930f,500.283f},{859.995f,499.214f},
            {859.193f,498.412f},{858.391f,497.343f},{857.857f,496.274f},
            {855.852f,495.740f},{854.382f,495.873f},{852.913f,495.873f},
            {851.042f,496.007f},{849.438f,496.140f},{847.969f,496.140f},
            {846.232f,496.140f},{844.494f,496.007f},{843.292f,496.007f},
            {841.688f,495.740f},{840.218f,495.606f},{839.016f,495.606f},
            {837.412f,495.606f},{836.076f,495.472f},{834.606f,495.339f},
            {833.137f,494.938f},{831.667f,494.804f},{830.330f,494.804f},
            {828.994f,494.671f},{827.391f,494.537f},{825.787f,494.403f},
            {824.184f,494.136f},{822.714f,493.869f},{821.378f,493.602f},
            {819.641f,493.334f},{818.304f,492.800f},{816.835f,492.399f},
            {815.231f,491.464f},{813.628f,490.929f},{812.158f,490.395f},
            {811.222f,489.994f},{809.886f,489.459f},{808.817f,489.192f},
            {807.481f,488.658f},{806.145f,488.390f},{805.076f,487.856f},
            {804.274f,487.321f},{803.472f,486.921f},{802.671f,486.386f},
            {801.602f,485.852f},{800.800f,485.718f},{801.735f,487.188f},
            {802.537f,488.524f},{803.071f,489.727f},{803.606f,490.929f},
            {804.408f,491.998f},{805.076f,493.201f},{805.744f,494.537f},
            {806.546f,496.274f},{807.481f,498.011f},{808.416f,499.481f},
            {808.684f,500.416f},{808.817f,501.753f},{808.817f,503.356f},
            {808.817f,504.826f},{808.951f,506.563f},{808.951f,508.701f},
            {809.084f,510.304f},{809.352f,511.373f},{809.886f,512.576f},
            {810.822f,513.779f},{811.623f,514.848f},{812.158f,515.649f},
            {812.826f,516.451f},{813.227f,517.520f},{813.761f,518.322f},
            {814.830f,520.059f},{814.697f,526.874f},{814.296f,527.542f},
            {813.360f,527.542f},{808.951f,527.542f},{807.748f,527.141f},
            {806.946f,526.473f},{805.878f,525.938f},{804.809f,525.003f},
            {803.205f,524.335f},{801.869f,523.533f},{800.800f,522.731f},
            {799.597f,522.063f},{798.528f,521.395f},{797.192f,520.727f},
            {796.123f,520.059f},{795.188f,519.257f},{793.985f,518.455f},
            {792.782f,517.386f},{791.580f,516.585f},{790.244f,515.649f},
            {789.175f,514.848f},{788.373f,513.779f},{787.170f,512.843f},
            {786.101f,511.641f},{785.166f,510.705f},{784.364f,509.503f},
            {783.563f,508.300f},{782.761f,507.365f},{782.093f,506.162f},
            {781.291f,504.826f},{780.623f,503.356f},{780.088f,502.421f},
            {779.287f,500.951f},{778.886f,499.748f},{778.351f,498.546f},
            {777.817f,497.076f},{777.416f,495.606f},{776.481f,494.403f},
            {776.347f,493.334f},{776.080f,491.731f},{775.812f,490.261f},
            {775.278f,489.058f},{774.877f,488.123f},{774.877f,487.589f},
            {772.071f,493.201f},{771.403f,494.003f},{770.601f,494.136f},
            {769.799f,493.334f},{768.998f,491.998f},{767.661f,490.395f},
            {766.325f,488.658f},{765.657f,486.787f},{764.989f,485.317f},
            {764.321f,483.981f},{763.519f,481.843f},{763.118f,480.373f},
            {762.450f,479.170f},{762.317f,477.968f},{762.183f,476.632f},
            {762.049f,475.429f},{762.049f,474.093f},{762.049f,473.024f},
            {762.183f,471.955f},{762.317f,471.019f},{762.584f,469.817f},
            {762.851f,468.080f},{763.653f,466.877f},{763.920f,465.407f},
            {764.722f,464.472f},{765.256f,463.403f},{765.791f,462.200f},
            {766.592f,461.265f},{767.261f,460.597f},{767.929f,459.394f},
            {768.597f,458.726f},{769.131f,457.924f},{769.799f,457.256f},
            {769.131f,456.722f},{768.330f,456.054f},{767.929f,455.252f},
            {767.261f,454.985f},{766.325f,454.584f},{765.256f,454.717f},
            {763.786f,454.985f},{762.584f,454.985f},{761.248f,454.851f},
            {759.778f,454.717f},{758.709f,454.317f},{757.640f,453.916f},
            {756.304f,453.515f},{755.101f,453.381f},{754.032f,452.847f},
            {752.562f,452.446f},{751.092f,451.644f},{749.890f,451.110f},
            {748.553f,450.308f},{747.618f,449.907f},{746.816f,449.373f},
            {745.747f,448.704f},{744.678f,447.903f},{743.743f,447.502f},
            {742.941f,446.700f},{742.140f,445.898f},{741.471f,445.230f},
            {740.803f,444.562f},{740.269f,443.627f},{740.135f,442.825f},
            {740.002f,441.890f},{739.734f,441.088f},{739.467f,440.420f},
            {738.933f,440.286f},{738.398f,440.420f},{737.864f,440.954f},
            {737.062f,441.489f},{736.527f,441.890f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.15f, 0.30f, 0.10f, 0.95f, 2.0f);
    }

    //* 17. EYE OUTLINE 
    {
        static const float p[][2] = {
            {752.348f,428.768f},{752.990f,428.287f},{753.631f,427.966f},
            {754.754f,427.806f},{755.716f,427.806f},{756.357f,428.127f},
            {756.838f,428.928f},{757.319f,429.730f},{757.640f,430.371f},
            {757.640f,431.173f},{757.640f,431.975f},{757.319f,432.777f},
            {756.838f,433.578f},{755.876f,434.380f},{755.235f,434.861f},
            {754.433f,435.342f},{753.471f,435.503f},{752.669f,435.182f},
            {752.188f,434.861f},{752.188f,434.059f},{752.188f,433.258f},
            {752.188f,432.616f},{751.867f,431.975f},{751.707f,431.334f},
            {751.707f,430.853f},{751.867f,430.371f},{752.028f,429.570f}
        };
        n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.05f, 0.08f, 0.05f, 1.00f, 1.2f);
    }

    glPopMatrix();

    glDisable(GL_BLEND);
}

//! =================      OCTOPUS        =================
struct OctopusAnim {
    float bobY      = 0.0f;
    float swayX     = 0.0f;
    float tiltAngle = 0.0f;
    float legPhase[12] = {};
} octAnim;

void initOctopus(){
    for(int i = 0; i < 12; i++){
        octAnim.legPhase[i] = i * 0.4f;
    }
}

void updateOctopus(){
    octAnim.bobY = sinf(gTime * 1.2f) * 6.0f;
    octAnim.swayX = sinf(gTime * 0.7f) * 4.0f;
    octAnim.tiltAngle = sinf(gTime * 0.9f) * 1.5f;
}
void drawOctopus(){
     //* Leg 1
    {
        float legBob  = sinf(gTime + octAnim.legPhase[0]) * 2.5f;
        float legSway = cosf(gTime * 0.8f + octAnim.legPhase[0]) * 1.5f;
        glPushMatrix();
        glTranslatef(legSway, legBob, 0.0f);
        static const float p[][2] = {
            {622.191f, 457.296f}, {622.846f, 457.296f}, {623.687f, 457.389f},
            {624.715f, 457.670f}, {625.743f, 458.043f}, {627.052f, 458.324f},
            {628.267f, 458.417f}, {629.109f, 458.511f}, {630.511f, 458.604f},
            {631.726f, 458.511f}, {634.530f, 458.698f}, {636.213f, 458.791f},
            {637.989f, 458.698f}, {640.139f, 458.885f}, {641.728f, 458.978f},
            {643.691f, 459.072f}, {644.813f, 459.072f}, {645.935f, 459.446f},
            {647.056f, 459.352f}, {648.272f, 459.352f}, {649.674f, 459.446f},
            {650.889f, 459.820f}, {652.011f, 460.193f}, {653.506f, 460.661f},
            {654.909f, 461.222f}, {655.937f, 461.783f}, {656.872f, 462.250f},
            {657.806f, 462.998f}, {658.835f, 463.746f}, {659.956f, 464.587f},
            {660.611f, 465.615f}, {661.452f, 466.270f}, {661.919f, 467.391f},
            {662.387f, 468.139f}, {662.480f, 468.980f}, {662.387f, 469.822f},
            {662.293f, 470.476f}, {661.826f, 471.130f}, {661.078f, 471.317f},
            {660.050f, 471.224f}, {658.928f, 470.850f}, {657.900f, 470.289f},
            {657.152f, 469.728f}, {656.591f, 469.448f}, {655.937f, 469.074f},
            {655.563f, 468.513f}, {655.469f, 467.859f}, {655.376f, 467.298f},
            {655.376f, 466.643f}, {655.750f, 465.989f}, {656.030f, 465.241f},
            {656.030f, 464.867f}, {655.469f, 465.054f}, {654.722f, 465.148f},
            {654.067f, 465.241f}, {653.133f, 464.961f}, {652.759f, 464.493f},
            {652.478f, 463.652f}, {652.385f, 462.998f}, {652.385f, 462.343f},
            {652.291f, 461.596f}, {652.104f, 461.128f}, {651.543f, 460.848f},
            {651.076f, 460.661f}, {650.609f, 460.661f}, {650.235f, 461.222f},
            {649.954f, 461.783f}, {649.487f, 462.343f}, {649.113f, 462.530f},
            {648.459f, 462.904f}, {647.617f, 463.091f}, {646.309f, 462.904f},
            {645.467f, 462.904f}, {644.813f, 462.811f}, {643.972f, 462.624f},
            {643.317f, 462.343f}, {642.943f, 461.783f}, {642.850f, 461.128f},
            {642.850f, 460.661f}, {642.756f, 460.287f}, {642.102f, 460.193f},
            {641.541f, 460.006f}, {640.887f, 459.913f}, {640.326f, 459.820f},
            {639.672f, 459.820f}, {639.204f, 459.913f}, {638.643f, 460.006f},
            {638.456f, 460.754f}, {638.363f, 461.315f}, {638.176f, 461.783f},
            {637.709f, 462.343f}, {636.680f, 462.624f}, {636.026f, 462.811f},
            {635.372f, 462.717f}, {634.624f, 462.624f}, {633.876f, 462.250f},
            {633.128f, 461.596f}, {632.848f, 460.941f}, {632.567f, 460.193f},
            {632.380f, 459.352f}, {631.819f, 459.446f}, {631.726f, 460.287f},
            {631.446f, 460.941f}, {631.072f, 461.409f}, {630.417f, 461.783f},
            {629.389f, 462.063f}, {628.174f, 461.970f}, {627.239f, 461.876f},
            {626.304f, 461.876f}, {625.650f, 461.502f}, {624.996f, 461.035f},
            {624.435f, 460.567f}, {623.967f, 460.287f}, {623.500f, 459.820f},
            {622.752f, 459.165f}, {622.098f, 458.698f}, {621.817f, 458.417f},
            {621.537f, 458.137f}, {621.163f, 457.670f}, {621.630f, 457.296f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.529f, 0.341f, 0.659f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.878f, 0.745f, 0.855f, 1.0f);
        glPopMatrix(); 
    }

    //* Leg 2
    {
        float legBob  = sinf(gTime + octAnim.legPhase[0]) * 2.5f;
        float legSway = cosf(gTime * 0.8f + octAnim.legPhase[0]) * 1.5f;
        glPushMatrix();
        glTranslatef(legSway, legBob, 0.0f);
        static const float p[][2] = {
            {593.587f, 454.959f}, {594.241f, 455.520f}, {595.269f, 456.174f},
            {596.204f, 456.735f}, {597.326f, 457.483f}, {598.074f, 458.043f},
            {598.915f, 458.885f}, {599.663f, 459.539f}, {600.411f, 460.193f},
            {601.252f, 460.848f}, {602.187f, 461.596f}, {604.150f, 463.933f},
            {604.991f, 464.961f}, {605.739f, 466.830f}, {606.954f, 469.822f},
            {606.954f, 469.822f}, {607.141f, 471.037f}, {607.328f, 472.159f},
            {607.515f, 473.093f}, {608.637f, 474.776f}, {608.637f, 474.776f},
            {608.637f, 475.898f}, {608.637f, 476.646f}, {609.011f, 478.048f},
            {609.291f, 478.796f}, {609.759f, 479.356f}, {610.039f, 480.198f},
            {610.133f, 480.946f}, {610.133f, 481.693f}, {610.319f, 482.441f},
            {610.506f, 483.376f}, {610.506f, 483.843f}, {610.880f, 484.498f},
            {611.067f, 485.526f}, {611.535f, 486.180f}, {611.722f, 487.396f},
            {611.909f, 488.330f}, {612.096f, 489.078f}, {612.563f, 490.293f},
            {612.750f, 491.509f}, {613.217f, 492.443f}, {613.685f, 493.285f},
            {614.152f, 494.406f}, {614.433f, 494.967f}, {614.900f, 496.089f},
            {615.274f, 497.117f}, {615.648f, 498.052f}, {615.835f, 498.893f},
            {616.022f, 499.735f}, {616.396f, 500.483f}, {616.676f, 501.043f},
            {617.050f, 501.791f}, {617.424f, 502.633f}, {617.798f, 503.287f},
            {618.172f, 503.754f}, {618.639f, 504.502f}, {619.013f, 505.250f},
            {619.574f, 505.717f}, {620.135f, 506.278f}, {620.602f, 506.652f},
            {621.256f, 507.493f}, {622.004f, 508.241f}, {622.752f, 508.989f},
            {623.593f, 509.737f}, {624.435f, 510.578f}, {625.089f, 511.139f},
            {625.837f, 511.700f}, {626.398f, 512.167f}, {627.146f, 512.728f},
            {628.267f, 513.196f}, {629.015f, 513.663f}, {629.950f, 514.411f},
            {630.604f, 514.785f}, {631.539f, 515.252f}, {632.006f, 515.533f},
            {632.661f, 516.000f}, {633.689f, 516.561f}, {634.717f, 516.841f},
            {635.933f, 516.935f}, {636.587f, 516.654f}, {637.522f, 516.654f},
            {638.456f, 516.748f}, {639.111f, 516.935f}, {639.859f, 517.028f},
            {640.700f, 517.870f}, {640.700f, 517.870f}, {641.074f, 518.243f},
            {641.448f, 518.243f}, {642.009f, 518.150f}, {642.383f, 517.963f},
            {642.943f, 517.870f}, {643.598f, 517.870f}, {644.252f, 517.870f},
            {644.906f, 517.870f}, {645.654f, 517.963f}, {646.215f, 517.870f},
            {646.589f, 517.776f}, {647.056f, 517.776f}, {647.991f, 517.870f},
            {648.739f, 517.963f}, {649.393f, 517.870f}, {649.954f, 518.243f},
            {650.235f, 518.898f}, {650.235f, 519.365f}, {650.048f, 519.926f},
            {649.861f, 520.020f}, {649.300f, 520.393f}, {648.459f, 520.954f},
            {647.711f, 521.141f}, {646.402f, 521.235f}, {645.280f, 521.235f},
            {644.252f, 521.141f}, {643.504f, 521.048f}, {642.756f, 520.954f},
            {641.822f, 520.767f}, {641.448f, 520.580f}, {641.354f, 520.020f},
            {641.354f, 519.646f}, {641.448f, 519.272f}, {641.448f, 518.991f},
            {640.980f, 518.804f}, {640.793f, 519.085f}, {640.793f, 519.365f},
            {640.793f, 519.739f}, {640.793f, 520.393f}, {640.606f, 520.580f},
            {640.326f, 520.861f}, {639.765f, 520.861f}, {639.017f, 520.767f},
            {638.176f, 520.767f}, {637.335f, 520.767f}, {636.493f, 520.674f},
            {636.026f, 520.674f}, {635.465f, 520.487f}, {634.811f, 520.206f},
            {634.530f, 519.739f}, {634.343f, 519.085f}, {634.250f, 518.524f},
            {634.156f, 518.150f}, {634.250f, 517.589f}, {633.969f, 517.402f},
            {633.315f, 517.215f}, {632.567f, 516.748f}, {632.006f, 516.561f},
            {631.446f, 516.467f}, {630.885f, 516.280f}, {630.417f, 516.280f},
            {630.137f, 516.654f}, {629.483f, 517.589f}, {629.483f, 517.589f},
            {629.015f, 517.402f}, {628.828f, 516.841f}, {628.735f, 516.187f},
            {628.454f, 515.720f}, {628.267f, 515.533f}, {627.146f, 515.346f},
            {626.678f, 515.252f}, {626.678f, 515.252f}, {626.024f, 515.065f},
            {625.650f, 514.972f}, {625.650f, 514.972f}, {625.556f, 514.972f},
            {625.556f, 514.972f}, {624.996f, 514.598f}, {624.996f, 514.598f},
            {624.622f, 514.504f}, {624.622f, 514.504f}, {624.622f, 514.504f},
            {624.154f, 514.224f}, {624.154f, 514.224f}, {624.154f, 514.224f},
            {623.780f, 514.037f}, {623.780f, 514.037f}, {623.780f, 514.037f},
            {623.219f, 513.756f}, {622.565f, 513.196f}, {622.004f, 512.822f},
            {621.350f, 512.448f}, {620.602f, 511.980f}, {620.041f, 511.326f},
            {619.480f, 510.952f}, {619.013f, 510.485f}, {618.826f, 509.924f},
            {618.546f, 509.270f}, {618.452f, 508.709f}, {618.452f, 508.335f},
            {618.546f, 507.961f}, {618.639f, 507.400f}, {618.546f, 507.120f},
            {618.546f, 506.559f}, {618.546f, 506.091f}, {618.359f, 505.624f},
            {618.172f, 505.343f}, {617.985f, 504.876f}, {617.798f, 504.222f},
            {617.517f, 503.848f}, {617.143f, 503.474f}, {616.863f, 503.193f},
            {616.676f, 502.726f}, {616.396f, 502.259f}, {615.928f, 501.417f},
            {615.648f, 500.950f}, {615.461f, 501.324f}, {615.554f, 501.791f},
            {615.461f, 502.446f}, {615.367f, 502.913f}, {615.180f, 503.100f},
            {614.806f, 503.287f}, {614.152f, 503.193f}, {613.591f, 502.820f},
            {613.030f, 502.446f}, {612.376f, 501.885f}, {611.815f, 501.324f},
            {611.254f, 500.576f}, {610.974f, 500.015f}, {610.413f, 499.174f},
            {610.039f, 498.520f}, {609.946f, 497.959f}, {609.852f, 496.743f},
            {609.759f, 495.809f}, {609.572f, 494.780f}, {609.478f, 494.126f},
            {609.478f, 493.378f}, {609.478f, 492.817f}, {609.385f, 492.070f},
            {609.572f, 491.509f}, {610.600f, 490.667f}, {610.600f, 490.667f},
            {610.974f, 490.200f}, {610.880f, 489.639f}, {610.506f, 489.359f},
            {609.665f, 489.265f}, {609.198f, 489.265f}, {608.637f, 488.985f},
            {608.263f, 488.704f}, {607.889f, 488.237f}, {607.609f, 487.396f},
            {607.422f, 486.741f}, {607.235f, 485.806f}, {607.141f, 485.246f},
            {606.861f, 484.591f}, {606.954f, 483.937f}, {606.861f, 483.563f},
            {606.767f, 482.628f}, {606.674f, 481.787f}, {606.767f, 481.133f},
            {606.674f, 480.291f}, {606.767f, 479.824f}, {607.328f, 479.730f},
            {607.889f, 479.543f}, {608.263f, 479.356f}, {608.637f, 479.170f},
            {608.543f, 478.796f}, {608.169f, 478.702f}, {607.889f, 478.702f},
            {607.422f, 478.796f}, {606.954f, 478.889f}, {606.300f, 478.983f},
            {605.739f, 478.889f}, {605.459f, 478.235f}, {605.272f, 477.393f},
            {605.178f, 476.646f}, {604.991f, 475.617f}, {604.898f, 474.963f},
            {604.711f, 474.028f}, {604.711f, 473.561f}, {604.430f, 472.533f},
            {604.430f, 471.598f}, {604.430f, 470.756f}, {604.430f, 470.196f},
            {604.430f, 469.354f}, {604.337f, 468.887f}, {603.496f, 468.887f},
            {602.748f, 468.793f}, {602.280f, 468.700f}, {601.626f, 468.606f},
            {600.972f, 468.326f}, {600.411f, 467.859f}, {599.943f, 467.298f},
            {599.756f, 466.456f}, {599.383f, 465.522f}, {599.196f, 465.054f},
            {598.915f, 464.306f}, {598.822f, 463.746f}, {598.448f, 463.185f},
            {597.980f, 463.372f}, {597.326f, 463.746f}, {595.737f, 463.839f},
            {595.737f, 463.839f}, {594.989f, 463.746f}, {594.522f, 463.465f},
            {594.054f, 462.904f}, {593.867f, 462.624f}, {593.680f, 462.156f},
            {593.306f, 461.502f}, {593.119f, 460.941f}, {592.933f, 460.474f},
            {592.652f, 459.726f}, {592.185f, 459.259f}, {591.624f, 458.791f},
            {591.343f, 458.230f}, {591.063f, 457.950f}, {590.876f, 456.922f},
            {590.783f, 456.548f}, {590.783f, 455.987f}, {590.969f, 455.426f},
            {590.969f, 454.865f}, {591.063f, 454.304f}, {591.156f, 453.650f},
            {591.530f, 453.556f}, {592.091f, 453.930f}, {592.652f, 454.398f},
            {593.026f, 454.678f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.529f, 0.341f, 0.659f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.878f, 0.745f, 0.855f, 1.0f);
        glPopMatrix(); 
    }

    //* Leg 3
    {
        float legBob  = sinf(gTime + octAnim.legPhase[0]) * 2.5f;
        float legSway = cosf(gTime * 0.8f + octAnim.legPhase[0]) * 1.5f;
        glPushMatrix();
        glTranslatef(legSway, legBob, 0.0f);
        static const float p[][2] = {
            {569.096f, 449.724f}, {569.189f, 451.406f}, {569.283f, 452.996f},
            {569.376f, 454.585f}, {569.469f, 457.763f}, {569.469f, 459.259f},
            {569.469f, 461.502f}, {569.469f, 463.372f}, {569.283f, 465.148f},
            {569.096f, 467.204f}, {569.189f, 469.167f}, {569.469f, 470.850f},
            {569.656f, 471.785f}, {569.750f, 472.720f}, {569.937f, 473.841f},
            {570.217f, 474.776f}, {570.498f, 475.804f}, {570.872f, 476.833f},
            {571.433f, 477.954f}, {571.713f, 479.170f}, {571.900f, 480.478f},
            {572.367f, 481.506f}, {572.928f, 482.348f}, {573.489f, 483.283f},
            {574.143f, 484.217f}, {574.704f, 485.152f}, {575.452f, 486.180f},
            {576.387f, 487.676f}, {577.135f, 488.891f}, {578.069f, 489.826f},
            {578.724f, 490.667f}, {579.659f, 491.602f}, {580.406f, 492.443f},
            {581.809f, 493.098f}, {582.837f, 493.939f}, {583.959f, 494.593f},
            {584.800f, 495.154f}, {586.109f, 495.996f}, {587.230f, 497.024f},
            {588.539f, 497.959f}, {589.474f, 498.800f}, {590.596f, 499.454f},
            {591.250f, 500.296f}, {592.091f, 501.043f}, {593.119f, 502.165f},
            {593.774f, 503.287f}, {594.709f, 504.409f}, {595.456f, 505.250f},
            {595.737f, 505.904f}, {596.672f, 507.961f}, {596.672f, 507.961f},
            {596.952f, 508.802f}, {597.513f, 510.204f}, {598.261f, 511.233f},
            {598.635f, 513.289f}, {598.635f, 513.289f}, {599.196f, 516.748f},
            {599.196f, 516.748f}, {599.102f, 517.776f}, {599.009f, 518.711f},
            {598.915f, 519.739f}, {598.635f, 520.674f}, {598.261f, 521.328f},
            {597.980f, 522.076f}, {597.419f, 521.889f}, {596.765f, 521.141f},
            {596.111f, 520.206f}, {595.456f, 519.272f}, {595.456f, 518.243f},
            {595.456f, 517.309f}, {595.456f, 516.093f}, {595.456f, 515.159f},
            {595.456f, 514.504f}, {595.924f, 513.570f}, {596.578f, 512.915f},
            {597.233f, 512.635f}, {597.513f, 512.448f}, {597.419f, 511.700f},
            {597.233f, 510.859f}, {596.952f, 510.111f}, {596.765f, 509.550f},
            {596.485f, 508.989f}, {596.111f, 509.176f}, {595.737f, 509.363f},
            {595.363f, 509.550f}, {594.428f, 509.550f}, {593.867f, 509.083f},
            {593.400f, 508.709f}, {592.839f, 508.054f}, {592.372f, 507.306f},
            {592.185f, 506.652f}, {592.091f, 505.904f}, {591.998f, 505.437f},
            {591.998f, 505.063f}, {592.278f, 504.502f}, {592.465f, 504.128f},
            {592.746f, 503.848f}, {593.026f, 503.380f}, {593.119f, 503.006f},
            {592.746f, 503.100f}, {592.091f, 503.193f}, {591.437f, 503.193f},
            {591.063f, 502.820f}, {590.783f, 502.446f}, {590.222f, 502.352f},
            {589.567f, 502.446f}, {588.913f, 502.259f}, {588.913f, 501.885f},
            {588.913f, 501.043f}, {588.913f, 500.576f}, {588.913f, 499.922f},
            {588.352f, 499.922f}, {587.604f, 500.389f}, {587.043f, 500.856f},
            {586.296f, 501.230f}, {585.454f, 501.417f}, {584.333f, 501.230f},
            {583.585f, 500.856f}, {582.930f, 500.483f}, {582.276f, 499.828f},
            {582.089f, 499.174f}, {581.715f, 498.426f}, {581.715f, 497.959f},
            {581.528f, 497.398f}, {581.341f, 497.211f}, {580.780f, 496.837f},
            {580.033f, 496.370f}, {579.659f, 495.528f}, {579.472f, 494.967f},
            {579.472f, 494.220f}, {579.472f, 493.659f}, {579.565f, 493.098f},
            {579.191f, 492.443f}, {578.911f, 492.070f}, {578.537f, 492.350f},
            {578.350f, 493.004f}, {578.163f, 493.378f}, {577.883f, 493.939f},
            {577.322f, 494.406f}, {576.854f, 494.313f}, {576.293f, 494.126f},
            {575.826f, 493.846f}, {575.359f, 493.565f}, {574.704f, 493.098f},
            {574.143f, 492.630f}, {573.676f, 492.256f}, {572.928f, 491.789f},
            {572.461f, 491.228f}, {572.087f, 490.667f}, {571.900f, 490.200f},
            {571.619f, 489.639f}, {571.152f, 488.891f}, {571.059f, 488.424f},
            {571.059f, 487.489f}, {571.059f, 486.741f}, {571.059f, 486.274f},
            {571.152f, 485.620f}, {571.152f, 484.872f}, {571.059f, 484.404f},
            {570.591f, 484.404f}, {570.217f, 484.498f}, {569.843f, 484.778f},
            {569.376f, 484.872f}, {569.002f, 484.778f}, {568.722f, 484.498f},
            {568.535f, 484.030f}, {568.254f, 483.283f}, {568.161f, 482.628f},
            {568.254f, 482.348f}, {568.348f, 481.693f}, {568.535f, 481.320f},
            {569.002f, 480.852f}, {569.469f, 480.665f}, {570.124f, 480.478f},
            {570.778f, 480.385f}, {571.339f, 480.104f}, {571.059f, 479.637f},
            {570.591f, 479.543f}, {570.124f, 479.543f}, {569.376f, 479.917f},
            {568.628f, 479.824f}, {567.974f, 479.824f}, {567.413f, 479.824f},
            {567.319f, 479.450f}, {567.226f, 478.328f}, {567.226f, 477.861f},
            {567.226f, 477.206f}, {567.133f, 476.459f}, {567.133f, 476.085f},
            {567.133f, 475.150f}, {567.133f, 474.309f}, {567.133f, 473.561f},
            {567.039f, 472.533f}, {566.946f, 471.785f}, {566.946f, 470.850f},
            {566.946f, 469.822f}, {566.852f, 469.167f}, {566.852f, 468.139f},
            {566.852f, 467.485f}, {566.852f, 466.924f}, {567.413f, 466.643f},
            {568.067f, 466.550f}, {568.441f, 466.176f}, {568.441f, 465.802f},
            {568.067f, 465.802f}, {567.413f, 465.802f}, {566.946f, 465.709f},
            {566.011f, 465.709f}, {565.824f, 464.961f}, {565.824f, 464.120f},
            {565.730f, 463.559f}, {565.730f, 462.998f}, {565.917f, 462.250f},
            {565.917f, 461.222f}, {566.104f, 460.380f}, {566.198f, 459.446f},
            {566.385f, 458.791f}, {566.665f, 458.230f}, {567.133f, 457.763f},
            {567.506f, 457.576f}, {568.348f, 457.389f}, {568.722f, 457.202f},
            {568.722f, 456.735f}, {568.628f, 456.080f}, {568.254f, 455.520f},
            {567.693f, 455.613f}, {566.665f, 455.426f}, {566.104f, 455.239f},
            {565.824f, 454.398f}, {565.824f, 453.743f}, {565.730f, 453.089f},
            {565.637f, 452.341f}, {565.824f, 451.687f}, {566.011f, 451.220f},
            {566.011f, 451.126f}, {566.291f, 450.659f}, {566.478f, 450.472f},
            {566.852f, 450.285f}, {567.319f, 450.004f}, {567.693f, 449.817f},
            {568.067f, 449.724f}, {568.441f, 449.537f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.529f, 0.341f, 0.659f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.878f, 0.745f, 0.855f, 1.0f);
        glPopMatrix(); 
    }

    //* Leg 4
    {
        float legBob  = sinf(gTime + octAnim.legPhase[0]) * 2.5f;
        float legSway = cosf(gTime * 0.8f + octAnim.legPhase[0]) * 1.5f;
        glPushMatrix();
        glTranslatef(legSway, legBob, 0.0f);
        static const float p[][2] = {
            {559.935f, 458.511f}, {559.748f, 458.978f}, {559.374f, 459.913f},
            {558.813f, 460.380f}, {558.065f, 460.661f}, {557.224f, 460.754f},
            {556.663f, 460.474f}, {556.196f, 459.913f}, {555.915f, 458.978f},
            {556.009f, 458.230f}, {556.289f, 456.922f}, {556.943f, 456.454f},
            {557.691f, 456.080f}, {558.439f, 455.987f}, {559.374f, 456.548f},
            {559.841f, 457.483f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.529f, 0.341f, 0.659f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.878f, 0.745f, 0.855f, 1.0f);
        glPopMatrix(); 
    }

    //* Leg 5
    {
        float legBob  = sinf(gTime + octAnim.legPhase[0]) * 2.5f;
        float legSway = cosf(gTime * 0.8f + octAnim.legPhase[0]) * 1.5f;
        glPushMatrix();
        glTranslatef(legSway, legBob, 0.0f);
        static const float p[][2] = {
            {554.513f, 466.363f}, {555.261f, 467.017f}, {555.448f, 467.952f},
            {555.448f, 468.887f}, {555.635f, 469.822f}, {555.354f, 471.224f},
            {555.167f, 472.159f}, {554.419f, 473.000f}, {553.485f, 473.467f},
            {552.643f, 473.374f}, {551.802f, 473.187f}, {551.709f, 472.813f},
            {551.615f, 472.159f}, {551.615f, 470.850f}, {551.615f, 470.009f},
            {551.989f, 468.980f}, {552.550f, 467.952f}, {553.111f, 466.830f},
            {553.672f, 466.270f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.529f, 0.341f, 0.659f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.878f, 0.745f, 0.855f, 1.0f);
        glPopMatrix(); 
    }

    //* Leg 6
    {
        float legBob  = sinf(gTime + octAnim.legPhase[0]) * 2.5f;
        float legSway = cosf(gTime * 0.8f + octAnim.legPhase[0]) * 1.5f;
        glPushMatrix();
        glTranslatef(legSway, legBob, 0.0f);
        static const float p[][2] = {
            {548.250f, 478.889f}, {548.343f, 477.580f}, {548.530f, 476.365f},
            {548.904f, 475.617f}, {549.091f, 474.870f}, {549.652f, 473.841f},
            {550.213f, 473.467f}, {550.867f, 473.280f}, {551.335f, 475.150f},
            {551.335f, 475.150f}, {551.522f, 476.365f}, {551.709f, 477.580f},
            {551.615f, 478.609f}, {551.148f, 479.263f}, {550.306f, 479.356f},
            {549.465f, 479.356f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.529f, 0.341f, 0.659f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.878f, 0.745f, 0.855f, 1.0f);
        glPopMatrix(); 
    }

    //* Leg 7
    {
       float legBob  = sinf(gTime + octAnim.legPhase[0]) * 2.5f;
        float legSway = cosf(gTime * 0.8f + octAnim.legPhase[0]) * 1.5f;
        glPushMatrix();
        glTranslatef(legSway, legBob, 0.0f);
        static const float p[][2] = {
            {548.998f, 482.254f}, {549.652f, 481.693f}, {550.400f, 480.852f},
            {550.867f, 481.039f}, {551.335f, 481.600f}, {551.522f, 482.628f},
            {551.522f, 483.656f}, {551.802f, 485.059f}, {551.709f, 486.554f},
            {551.802f, 489.359f}, {551.802f, 489.359f}, {550.867f, 490.667f},
            {549.652f, 490.480f}, {548.904f, 490.480f}, {548.717f, 490.200f},
            {548.530f, 489.265f}, {548.343f, 488.143f}, {548.343f, 486.741f},
            {548.250f, 485.806f}, {548.343f, 484.965f}, {548.343f, 484.311f},
            {548.437f, 483.563f}, {548.717f, 482.722f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.529f, 0.341f, 0.659f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.878f, 0.745f, 0.855f, 1.0f);
        glPopMatrix(); 
    }

    //* Leg 8
    {
        float legBob  = sinf(gTime + octAnim.legPhase[0]) * 2.5f;
        float legSway = cosf(gTime * 0.8f + octAnim.legPhase[0]) * 1.5f;
        glPushMatrix();
        glTranslatef(legSway, legBob, 0.0f);
        static const float p[][2] = {
            {548.156f, 497.211f}, {548.717f, 496.837f}, {549.372f, 496.743f},
            {550.119f, 496.837f}, {550.306f, 498.146f}, {550.400f, 498.893f},
            {550.400f, 500.015f}, {550.400f, 500.950f}, {550.213f, 501.698f},
            {549.839f, 502.726f}, {549.372f, 503.661f}, {548.250f, 503.754f},
            {547.222f, 503.380f}, {546.380f, 502.446f}, {546.193f, 501.417f},
            {546.287f, 500.109f}, {546.661f, 499.361f}, {547.035f, 498.426f},
            {547.502f, 497.585f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.529f, 0.341f, 0.659f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.878f, 0.745f, 0.855f, 1.0f);
        glPopMatrix(); 
    }

    //* Leg 9
    {
        float legBob  = sinf(gTime + octAnim.legPhase[0]) * 2.5f;
        float legSway = cosf(gTime * 0.8f + octAnim.legPhase[0]) * 1.5f;
        glPushMatrix();
        glTranslatef(legSway, legBob, 0.0f);
        static const float p[][2] = {
            {542.454f, 511.139f}, {543.015f, 510.578f}, {543.483f, 510.017f},
            {543.856f, 509.830f}, {544.698f, 509.830f}, {545.072f, 509.924f},
            {545.726f, 510.391f}, {545.633f, 511.139f}, {545.259f, 511.793f},
            {544.978f, 512.448f}, {544.604f, 513.289f}, {543.950f, 514.224f},
            {543.669f, 514.878f}, {543.483f, 515.252f}, {543.015f, 515.813f},
            {542.548f, 516.000f}, {541.987f, 516.280f}, {541.239f, 516.280f},
            {540.678f, 515.720f}, {540.772f, 514.691f}, {540.959f, 513.850f},
            {541.519f, 512.822f}, {541.706f, 512.354f}, {541.987f, 511.606f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.529f, 0.341f, 0.659f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.878f, 0.745f, 0.855f, 1.0f);
        glPopMatrix(); 
    }

    //* Leg 10
    {
        float legBob  = sinf(gTime + octAnim.legPhase[0]) * 2.5f;
        float legSway = cosf(gTime * 0.8f + octAnim.legPhase[0]) * 1.5f;
        glPushMatrix();
        glTranslatef(legSway, legBob, 0.0f);
        static const float p[][2] = {
            {533.667f, 453.089f}, {534.228f, 452.996f}, {534.696f, 452.715f},
            {535.069f, 452.809f}, {535.163f, 453.276f}, {534.789f, 454.304f},
            {534.789f, 454.304f}, {534.415f, 454.678f}, {533.761f, 455.426f},
            {533.387f, 455.987f}, {533.200f, 456.361f}, {532.639f, 457.015f},
            {530.676f, 457.296f}, {530.676f, 457.296f}, {529.554f, 457.015f},
            {528.619f, 457.109f}, {527.685f, 456.922f}, {526.750f, 457.015f},
            {525.722f, 456.922f}, {524.880f, 456.828f}, {524.226f, 456.828f},
            {523.759f, 456.828f}, {523.478f, 457.109f}, {523.011f, 457.950f},
            {522.543f, 458.791f}, {522.076f, 459.539f}, {521.422f, 460.287f},
            {520.767f, 460.661f}, {518.898f, 461.783f}, {518.898f, 461.783f},
            {518.056f, 462.156f}, {516.935f, 462.437f}, {515.906f, 462.156f},
            {515.346f, 461.689f}, {515.346f, 461.128f}, {515.346f, 460.474f},
            {515.720f, 459.726f}, {515.906f, 459.165f}, {515.813f, 458.417f},
            {515.439f, 458.137f}, {514.785f, 458.604f}, {514.317f, 459.072f},
            {513.663f, 459.726f}, {513.102f, 460.474f}, {512.728f, 461.035f},
            {512.354f, 461.596f}, {511.326f, 461.315f}, {510.204f, 460.567f},
            {509.737f, 459.913f}, {508.896f, 459.259f}, {508.522f, 459.165f},
            {508.241f, 460.567f}, {508.241f, 460.567f}, {508.148f, 461.409f},
            {507.587f, 462.811f}, {507.587f, 462.811f}, {506.652f, 463.091f},
            {505.998f, 463.185f}, {504.409f, 463.746f}, {504.409f, 463.746f},
            {503.754f, 464.026f}, {503.193f, 464.306f}, {502.446f, 464.400f},
            {501.604f, 464.400f}, {500.856f, 464.026f}, {500.015f, 463.372f},
            {499.828f, 462.624f}, {499.548f, 461.876f}, {498.800f, 462.250f},
            {498.333f, 463.185f}, {497.211f, 464.026f}, {497.211f, 464.026f},
            {496.650f, 464.680f}, {495.809f, 464.961f}, {494.874f, 465.054f},
            {494.593f, 465.615f}, {494.313f, 466.456f}, {494.220f, 467.298f},
            {494.220f, 467.952f}, {494.126f, 468.513f}, {493.846f, 469.074f},
            {493.191f, 469.728f}, {492.443f, 470.196f}, {491.509f, 470.570f},
            {490.387f, 471.037f}, {489.920f, 471.411f}, {488.611f, 472.065f},
            {487.676f, 471.878f}, {486.554f, 472.346f}, {485.433f, 472.252f},
            {484.498f, 472.439f}, {483.843f, 472.533f}, {483.376f, 473.000f},
            {482.722f, 473.374f}, {482.348f, 474.028f}, {481.880f, 474.402f},
            {482.535f, 474.683f}, {483.096f, 475.056f}, {483.376f, 475.711f},
            {483.470f, 476.459f}, {483.376f, 477.206f}, {483.283f, 477.954f},
            {483.096f, 478.796f}, {482.628f, 479.637f}, {482.161f, 480.198f},
            {481.693f, 480.665f}, {481.226f, 481.039f}, {480.385f, 481.226f},
            {479.450f, 481.506f}, {478.422f, 481.506f}, {477.954f, 481.039f},
            {478.141f, 480.291f}, {478.515f, 479.263f}, {478.796f, 478.235f},
            {479.076f, 477.767f}, {479.356f, 477.206f}, {479.730f, 476.178f},
            {480.385f, 475.711f}, {480.852f, 475.056f}, {481.320f, 474.309f},
            {481.693f, 473.748f}, {482.067f, 473.093f}, {482.628f, 472.533f},
            {483.283f, 471.878f}, {483.656f, 471.411f}, {484.498f, 470.570f},
            {485.339f, 469.822f}, {486.180f, 468.980f}, {487.115f, 468.139f},
            {488.143f, 467.391f}, {489.078f, 466.643f}, {489.733f, 466.270f},
            {490.387f, 465.428f}, {491.509f, 464.680f}, {492.443f, 464.026f},
            {493.378f, 463.465f}, {494.593f, 462.904f}, {495.809f, 462.156f},
            {497.772f, 461.315f}, {499.080f, 460.567f}, {500.202f, 460.006f},
            {501.324f, 459.539f}, {502.165f, 459.165f}, {503.567f, 458.698f},
            {504.970f, 458.324f}, {505.998f, 458.043f}, {506.839f, 457.950f},
            {507.867f, 457.670f}, {509.550f, 457.576f}, {510.859f, 457.202f},
            {511.887f, 457.109f}, {513.009f, 456.922f}, {514.037f, 456.735f},
            {515.626f, 456.454f}, {516.748f, 456.361f}, {517.402f, 456.361f},
            {518.056f, 456.174f}, {518.898f, 456.080f}, {519.926f, 455.893f},
            {520.861f, 455.800f}, {521.983f, 455.520f}, {522.824f, 455.520f},
            {523.291f, 455.613f}, {523.852f, 455.613f}, {524.600f, 455.239f},
            {525.067f, 455.146f}, {525.628f, 455.052f}, {526.283f, 454.865f},
            {527.124f, 454.772f}, {528.152f, 454.678f}, {528.900f, 454.585f},
            {529.741f, 454.491f}, {530.863f, 454.398f}, {531.704f, 454.211f},
            {532.546f, 453.556f}, {533.200f, 453.183f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.529f, 0.341f, 0.659f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.878f, 0.745f, 0.855f, 1.0f);
        glPopMatrix(); 
    }

    //* Leg 11
    {
        float legBob  = sinf(gTime + octAnim.legPhase[0]) * 2.5f;
        float legSway = cosf(gTime * 0.8f + octAnim.legPhase[0]) * 1.5f;
        glPushMatrix();
        glTranslatef(legSway, legBob, 0.0f);
        static const float p[][2] = {
            {474.776f, 494.593f}, {474.496f, 493.659f}, {474.402f, 492.724f},
            {474.402f, 491.883f}, {474.589f, 489.546f}, {474.589f, 489.546f},
            {475.150f, 488.050f}, {475.711f, 487.115f}, {476.272f, 486.461f},
            {476.739f, 486.648f}, {476.833f, 488.424f}, {476.833f, 488.424f},
            {476.926f, 491.041f}, {476.926f, 491.041f}, {477.020f, 492.350f},
            {477.020f, 494.313f}, {477.020f, 494.313f}, {476.646f, 495.061f},
            {476.646f, 495.435f}, {477.113f, 496.183f}, {477.487f, 497.117f},
            {477.487f, 498.520f}, {477.300f, 499.267f}, {477.300f, 499.267f},
            {476.926f, 499.735f}, {476.085f, 500.109f}, {475.430f, 499.828f},
            {474.776f, 499.267f}, {474.776f, 498.706f}, {474.870f, 498.052f},
            {474.776f, 497.211f}, {474.776f, 496.650f}, {474.870f, 496.276f},
            {475.150f, 495.809f}, {475.524f, 495.715f}, {475.898f, 495.341f},
            {475.804f, 495.061f}, {475.430f, 494.874f}, {474.963f, 494.780f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.529f, 0.341f, 0.659f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.878f, 0.745f, 0.855f, 1.0f);
        glPopMatrix(); 
    }

    //* Leg 12
    {
        float legBob  = sinf(gTime + octAnim.legPhase[0]) * 2.5f;
        float legSway = cosf(gTime * 0.8f + octAnim.legPhase[0]) * 1.5f;
        glPushMatrix();
        glTranslatef(legSway, legBob, 0.0f);
        static const float p[][2] = {
            {488.050f, 405.322f}, {489.078f, 404.948f}, {489.639f, 404.761f},
            {490.480f, 404.667f}, {491.415f, 404.761f}, {492.256f, 404.761f},
            {493.846f, 405.041f}, {493.846f, 405.041f}, {494.593f, 405.135f},
            {495.622f, 405.509f}, {496.463f, 405.789f}, {497.117f, 405.976f},
            {497.585f, 406.350f}, {498.333f, 406.724f}, {498.893f, 407.285f},
            {499.735f, 407.659f}, {501.791f, 408.780f}, {501.791f, 408.780f},
            {502.446f, 409.996f}, {503.006f, 411.024f}, {504.035f, 412.613f},
            {504.689f, 414.015f}, {505.530f, 415.604f}, {506.091f, 416.446f},
            {506.278f, 417.287f}, {506.559f, 418.315f}, {507.213f, 419.717f},
            {507.867f, 420.559f}, {508.241f, 421.774f}, {508.802f, 422.989f},
            {509.550f, 424.204f}, {510.298f, 425.326f}, {510.859f, 425.980f},
            {511.420f, 426.635f}, {512.074f, 427.663f}, {513.196f, 428.972f},
            {513.756f, 429.720f}, {514.411f, 430.187f}, {514.785f, 430.654f},
            {515.533f, 431.215f}, {516.187f, 431.870f}, {516.935f, 432.524f},
            {518.056f, 433.272f}, {519.085f, 433.833f}, {520.020f, 434.487f},
            {520.954f, 434.954f}, {521.889f, 435.515f}, {522.730f, 435.889f},
            {523.852f, 436.263f}, {523.946f, 436.637f}, {523.572f, 438.039f},
            {523.572f, 438.039f}, {523.011f, 438.320f}, {522.263f, 438.880f},
            {521.515f, 439.067f}, {520.767f, 439.348f}, {519.739f, 439.441f},
            {518.991f, 439.348f}, {518.150f, 437.104f}, {518.150f, 437.104f},
            {518.056f, 436.170f}, {517.776f, 435.422f}, {517.402f, 434.954f},
            {516.000f, 435.702f}, {516.000f, 435.702f}, {515.533f, 435.609f},
            {515.065f, 434.861f}, {514.972f, 434.300f}, {514.878f, 433.552f},
            {514.598f, 433.833f}, {514.411f, 434.300f}, {514.037f, 434.954f},
            {513.756f, 435.141f}, {513.009f, 435.328f}, {512.261f, 435.328f},
            {511.606f, 435.141f}, {510.952f, 434.861f}, {510.391f, 434.206f},
            {510.111f, 433.459f}, {509.924f, 432.430f}, {509.924f, 431.028f},
            {509.924f, 431.028f}, {509.830f, 430.467f}, {509.270f, 430.187f},
            {508.615f, 430.187f}, {507.961f, 430.280f}, {507.493f, 430.187f},
            {507.493f, 429.252f}, {507.587f, 427.943f}, {507.587f, 426.635f},
            {507.680f, 424.859f}, {507.680f, 423.830f}, {507.493f, 424.017f},
            {507.306f, 424.578f}, {507.120f, 425.046f}, {506.746f, 425.700f},
            {506.185f, 426.074f}, {505.437f, 426.074f}, {504.783f, 425.700f},
            {504.222f, 425.326f}, {503.754f, 424.859f}, {503.754f, 424.111f},
            {503.754f, 423.456f}, {503.661f, 422.896f}, {503.661f, 422.054f},
            {503.754f, 421.213f}, {504.035f, 420.372f}, {504.315f, 419.811f},
            {504.315f, 419.156f}, {504.128f, 419.063f}, {503.567f, 418.783f},
            {503.006f, 418.689f}, {502.446f, 418.502f}, {501.885f, 418.315f},
            {501.417f, 418.035f}, {501.417f, 417.474f}, {501.417f, 416.446f},
            {501.511f, 415.230f}, {501.511f, 414.015f}, {501.511f, 413.361f},
            {500.670f, 413.174f}, {499.735f, 413.080f}, {498.800f, 412.800f},
            {498.800f, 411.865f}, {498.706f, 410.930f}, {498.800f, 410.089f},
            {498.706f, 409.528f}, {498.146f, 409.715f}, {497.959f, 410.089f},
            {497.117f, 410.183f}, {496.556f, 410.089f}, {496.183f, 410.089f},
            {495.248f, 409.622f}, {494.874f, 409.248f}, {494.874f, 408.780f},
            {494.874f, 408.220f}, {494.874f, 407.565f}, {494.500f, 407.378f},
            {493.939f, 407.285f}, {493.098f, 407.191f}, {492.443f, 407.191f},
            {491.976f, 407.004f}, {491.883f, 406.443f}, {491.415f, 406.350f},
            {490.948f, 406.163f}, {490.293f, 406.256f}, {489.639f, 406.163f},
            {489.078f, 406.163f}, {488.424f, 405.976f}, {487.770f, 405.883f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.529f, 0.341f, 0.659f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.878f, 0.745f, 0.855f, 1.0f);
        glPopMatrix(); 
    }

    //?         Body
    {
        static const float p[][2] = {
            {476.380f, 412.037f}, {477.188f, 409.749f}, {478.265f, 407.326f},
            {479.342f, 405.711f}, {480.149f, 403.961f}, {481.361f, 402.211f},
            {483.111f, 400.057f}, {484.726f, 398.711f}, {486.476f, 397.499f},
            {488.495f, 396.153f}, {490.245f, 395.346f}, {492.129f, 394.807f},
            {494.687f, 394.403f}, {497.379f, 394.269f}, {499.533f, 394.673f},
            {501.552f, 395.211f}, {504.110f, 396.423f}, {506.263f, 397.365f},
            {508.013f, 398.442f}, {509.898f, 400.057f}, {511.378f, 401.538f},
            {513.128f, 403.153f}, {514.744f, 405.038f}, {516.090f, 407.326f},
            {517.570f, 410.422f}, {519.320f, 413.114f}, {520.263f, 415.268f},
            {522.147f, 417.556f}, {523.628f, 419.306f}, {525.243f, 420.787f},
            {526.858f, 421.864f}, {528.608f, 422.806f}, {530.358f, 423.479f},
            {532.647f, 423.883f}, {535.204f, 424.152f}, {537.358f, 424.017f},
            {539.781f, 423.748f}, {541.531f, 423.210f}, {543.819f, 422.537f},
            {545.434f, 422.133f}, {546.780f, 421.729f}, {547.992f, 421.056f},
            {549.203f, 420.383f}, {550.146f, 419.441f}, {550.953f, 418.498f},
            {551.761f, 417.287f}, {552.434f, 416.075f}, {553.107f, 414.729f},
            {553.511f, 413.114f}, {554.184f, 411.499f}, {554.588f, 410.287f},
            {554.857f, 408.807f}, {555.261f, 407.595f}, {555.395f, 406.384f},
            {555.395f, 405.172f}, {554.992f, 404.499f}, {554.588f, 403.288f},
            {554.184f, 402.480f}, {553.780f, 401.403f}, {553.780f, 399.788f},
            {553.780f, 398.172f}, {553.780f, 396.557f}, {554.184f, 395.211f},
            {554.453f, 394.000f}, {555.126f, 392.519f}, {556.068f, 390.904f},
            {556.876f, 389.692f}, {557.818f, 388.481f}, {559.030f, 387.538f},
            {560.107f, 386.192f}, {561.318f, 385.654f}, {562.395f, 384.846f},
            {564.683f, 383.500f}, {565.760f, 382.558f}, {566.972f, 381.481f},
            {568.049f, 380.539f}, {568.856f, 380.000f}, {569.664f, 379.058f},
            {570.068f, 377.712f}, {570.875f, 376.770f}, {571.414f, 375.693f},
            {571.818f, 374.616f}, {572.491f, 373.404f}, {573.029f, 371.789f},
            {573.837f, 370.443f}, {574.914f, 368.828f}, {575.856f, 367.616f},
            {576.664f, 366.136f}, {577.337f, 364.924f}, {578.010f, 363.713f},
            {578.952f, 362.501f}, {580.163f, 360.886f}, {581.509f, 359.271f},
            {582.721f, 358.059f}, {583.529f, 356.578f}, {585.009f, 355.636f},
            {586.759f, 354.425f}, {587.836f, 353.213f}, {589.586f, 351.867f},
            {591.201f, 350.656f}, {592.278f, 349.983f}, {593.086f, 349.444f},
            {594.432f, 348.636f}, {596.047f, 348.098f}, {597.259f, 347.425f},
            {598.201f, 346.752f}, {599.278f, 346.079f}, {600.220f, 345.675f},
            {601.028f, 345.137f}, {601.835f, 344.867f}, {603.047f, 344.598f},
            {604.124f, 344.194f}, {605.335f, 343.656f}, {606.547f, 343.252f},
            {607.758f, 343.118f}, {607.758f, 343.118f}, {608.835f, 342.714f},
            {610.181f, 342.579f}, {611.393f, 342.175f}, {612.739f, 342.175f},
            {614.085f, 341.906f}, {615.027f, 341.906f}, {616.239f, 341.771f},
            {617.719f, 342.041f}, {618.931f, 342.041f}, {620.008f, 341.906f},
            {621.488f, 341.906f}, {622.834f, 342.175f}, {624.046f, 342.310f},
            {624.988f, 342.444f}, {626.738f, 342.579f}, {628.084f, 342.983f},
            {629.565f, 343.387f}, {630.911f, 343.791f}, {632.257f, 344.464f},
            {633.468f, 344.733f}, {634.411f, 345.271f}, {635.622f, 345.944f},
            {636.699f, 346.617f}, {638.045f, 347.156f}, {638.853f, 347.560f},
            {639.930f, 348.233f}, {640.872f, 348.771f}, {642.218f, 349.713f},
            {643.429f, 350.386f}, {644.237f, 351.194f}, {645.179f, 351.867f},
            {645.987f, 352.809f}, {646.795f, 353.886f}, {647.602f, 354.828f},
            {648.679f, 356.175f}, {649.621f, 357.924f}, {650.429f, 359.271f},
            {651.371f, 360.617f}, {651.910f, 362.097f}, {652.717f, 363.443f},
            {653.525f, 365.193f}, {653.929f, 366.674f}, {654.467f, 368.559f},
            {654.602f, 370.443f}, {654.871f, 372.328f}, {654.737f, 373.808f},
            {654.871f, 375.289f}, {654.871f, 376.770f}, {654.467f, 378.654f},
            {654.198f, 380.404f}, {654.064f, 381.616f}, {654.064f, 383.096f},
            {653.525f, 384.712f}, {653.525f, 386.327f}, {653.121f, 387.942f},
            {652.717f, 389.423f}, {652.179f, 391.173f}, {651.506f, 392.788f},
            {650.968f, 394.538f}, {650.295f, 396.153f}, {649.218f, 397.769f},
            {647.872f, 399.653f}, {647.199f, 400.865f}, {646.122f, 401.942f},
            {644.910f, 403.557f}, {643.699f, 405.038f}, {642.218f, 406.249f},
            {640.468f, 407.730f}, {638.853f, 409.076f}, {637.372f, 410.422f},
            {636.564f, 410.826f}, {635.218f, 411.499f}, {633.872f, 412.172f},
            {632.257f, 412.710f}, {630.776f, 413.518f}, {629.026f, 413.922f},
            {626.873f, 414.595f}, {625.257f, 415.402f}, {623.507f, 415.806f},
            {622.431f, 416.210f}, {621.488f, 416.479f}, {620.681f, 416.883f},
            {619.738f, 417.152f}, {618.796f, 417.287f}, {617.719f, 417.556f},
            {616.777f, 418.095f}, {615.565f, 418.364f}, {614.623f, 418.768f},
            {613.816f, 419.171f}, {613.008f, 419.710f}, {612.200f, 420.248f},
            {611.123f, 420.652f}, {610.047f, 421.191f}, {608.970f, 421.998f},
            {608.431f, 422.537f}, {608.027f, 423.344f}, {607.489f, 424.421f},
            {607.354f, 425.363f}, {606.681f, 426.440f}, {606.681f, 427.652f},
            {607.085f, 428.729f}, {607.489f, 429.806f}, {608.162f, 431.017f},
            {608.835f, 432.363f}, {609.643f, 433.305f}, {610.047f, 434.517f},
            {611.123f, 435.728f}, {612.335f, 436.671f}, {613.277f, 437.747f},
            {614.354f, 438.690f}, {615.700f, 439.767f}, {616.912f, 440.709f},
            {618.258f, 441.651f}, {619.873f, 442.324f}, {620.815f, 442.997f},
            {621.757f, 443.536f}, {622.700f, 443.939f}, {623.507f, 444.209f},
            {624.719f, 444.343f}, {625.527f, 444.478f}, {626.738f, 444.612f},
            {628.084f, 444.612f}, {629.834f, 444.747f}, {631.045f, 444.747f},
            {631.988f, 444.747f}, {633.065f, 444.747f}, {634.141f, 444.612f},
            {635.218f, 444.612f}, {636.295f, 444.747f}, {637.372f, 444.882f},
            {638.180f, 444.882f}, {639.526f, 444.882f}, {641.007f, 444.882f},
            {642.353f, 444.882f}, {643.564f, 445.016f}, {644.641f, 444.882f},
            {645.449f, 444.882f}, {646.256f, 445.151f}, {647.064f, 445.286f},
            {648.141f, 445.420f}, {649.218f, 445.824f}, {650.025f, 446.093f},
            {650.968f, 446.362f}, {652.044f, 446.632f}, {652.987f, 447.035f},
            {653.660f, 447.305f}, {654.333f, 447.708f}, {655.410f, 448.112f},
            {656.621f, 448.516f}, {657.563f, 449.055f}, {658.236f, 449.324f},
            {659.179f, 449.862f}, {660.121f, 450.535f}, {660.121f, 450.535f},
            {660.929f, 450.939f}, {661.602f, 451.612f}, {662.275f, 452.420f},
            {663.352f, 453.093f}, {664.294f, 453.900f}, {664.832f, 454.439f},
            {665.371f, 454.843f}, {665.775f, 455.516f}, {666.448f, 456.323f},
            {666.986f, 457.266f}, {666.986f, 457.266f}, {667.659f, 458.208f},
            {668.197f, 459.016f}, {668.871f, 459.958f}, {669.544f, 460.766f},
            {670.082f, 461.708f}, {670.620f, 462.515f}, {671.159f, 463.592f},
            {671.832f, 464.669f}, {672.370f, 465.881f}, {672.640f, 466.958f},
            {672.774f, 467.765f}, {673.178f, 468.707f}, {673.447f, 469.919f},
            {673.447f, 471.803f}, {673.716f, 473.553f}, {673.716f, 476.111f},
            {673.716f, 477.861f}, {673.447f, 479.476f}, {673.178f, 480.284f},
            {672.909f, 481.495f}, {672.370f, 482.438f}, {672.101f, 483.245f},
            {671.832f, 484.053f}, {671.428f, 484.726f}, {671.024f, 485.937f},
            {670.486f, 486.610f}, {670.082f, 487.014f}, {669.409f, 487.822f},
            {668.736f, 488.360f}, {667.794f, 488.899f}, {667.255f, 489.303f},
            {666.313f, 489.841f}, {665.909f, 490.514f}, {665.236f, 490.918f},
            {664.563f, 491.456f}, {663.621f, 491.860f}, {662.813f, 492.264f},
            {661.871f, 492.668f}, {660.794f, 492.802f}, {659.852f, 492.937f},
            {658.371f, 493.206f}, {657.160f, 493.206f}, {655.813f, 493.206f},
            {654.871f, 493.072f}, {653.929f, 492.533f}, {653.121f, 492.129f},
            {652.179f, 491.860f}, {650.968f, 491.052f}, {650.160f, 490.514f},
            {649.352f, 489.437f}, {648.410f, 488.360f}, {647.872f, 487.418f},
            {647.468f, 486.476f}, {647.333f, 485.399f}, {647.333f, 484.457f},
            {647.468f, 483.784f}, {647.737f, 483.245f}, {648.141f, 482.303f},
            {648.545f, 481.899f}, {648.948f, 482.034f}, {649.756f, 482.707f},
            {650.160f, 483.245f}, {650.833f, 483.784f}, {651.506f, 484.322f},
            {651.775f, 484.860f}, {652.314f, 485.264f}, {652.852f, 485.668f},
            {653.391f, 486.207f}, {653.929f, 486.610f}, {654.602f, 486.880f},
            {655.275f, 487.149f}, {656.352f, 487.149f}, {656.352f, 487.149f},
            {657.294f, 486.880f}, {658.102f, 486.610f}, {658.775f, 486.072f},
            {659.448f, 485.534f}, {659.717f, 484.995f}, {660.256f, 484.187f},
            {660.794f, 483.514f}, {661.063f, 482.841f}, {661.332f, 482.168f},
            {661.467f, 481.361f}, {661.602f, 480.553f}, {661.871f, 479.745f},
            {661.871f, 478.534f}, {662.005f, 477.726f}, {662.005f, 476.784f},
            {662.140f, 475.842f}, {662.140f, 474.899f}, {662.140f, 474.092f},
            {662.275f, 473.284f}, {662.409f, 472.073f}, {662.544f, 471.130f},
            {662.544f, 470.188f}, {662.544f, 469.246f}, {662.409f, 468.438f},
            {662.140f, 467.227f}, {661.602f, 466.015f}, {660.794f, 464.938f},
            {659.986f, 464.131f}, {658.909f, 463.458f}, {658.102f, 462.650f},
            {657.025f, 461.977f}, {656.487f, 461.304f}, {655.544f, 461.035f},
            {654.467f, 460.496f}, {653.525f, 459.958f}, {652.583f, 459.419f},
            {651.775f, 459.150f}, {650.833f, 458.612f}, {649.621f, 458.477f},
            {648.141f, 458.477f}, {646.795f, 458.477f}, {645.449f, 458.343f},
            {644.103f, 458.343f}, {643.160f, 458.343f}, {642.083f, 458.343f},
            {641.007f, 458.208f}, {639.660f, 458.477f}, {638.314f, 458.477f},
            {637.507f, 458.477f}, {636.699f, 458.477f}, {635.757f, 458.477f},
            {634.545f, 458.477f}, {633.334f, 458.477f}, {632.257f, 458.343f},
            {631.315f, 458.073f}, {630.103f, 457.939f}, {629.161f, 457.939f},
            {628.219f, 457.670f}, {627.007f, 457.535f}, {625.661f, 457.266f},
            {624.584f, 456.862f}, {623.507f, 456.593f}, {622.431f, 456.593f},
            {621.354f, 456.593f}, {620.681f, 456.189f}, {620.008f, 455.381f},
            {619.469f, 454.439f}, {618.796f, 453.766f}, {617.988f, 453.093f},
            {617.181f, 452.285f}, {616.642f, 451.612f}, {615.835f, 450.670f},
            {615.162f, 449.862f}, {614.623f, 449.189f}, {613.950f, 448.516f},
            {613.277f, 447.708f}, {612.604f, 447.035f}, {611.931f, 446.228f},
            {610.989f, 445.286f}, {610.181f, 444.612f}, {609.643f, 443.939f},
            {608.970f, 443.536f}, {608.297f, 442.997f}, {607.489f, 442.459f},
            {606.816f, 441.786f}, {606.143f, 441.113f}, {605.335f, 440.574f},
            {604.393f, 440.170f}, {604.931f, 440.574f}, {605.201f, 440.978f},
            {605.874f, 441.516f}, {606.277f, 441.786f}, {606.951f, 442.324f},
            {607.489f, 442.728f}, {608.297f, 443.401f}, {608.835f, 444.074f},
            {609.373f, 444.478f}, {610.181f, 445.151f}, {610.720f, 445.689f},
            {611.258f, 446.362f}, {611.662f, 446.766f}, {612.200f, 447.305f},
            {612.604f, 447.574f}, {613.143f, 448.516f}, {613.816f, 449.458f},
            {614.354f, 449.997f}, {614.892f, 450.535f}, {615.431f, 451.343f},
            {616.239f, 452.016f}, {616.912f, 452.958f}, {617.450f, 453.362f},
            {617.988f, 453.900f}, {618.661f, 454.977f}, {619.738f, 456.054f},
            {620.277f, 456.996f}, {621.084f, 458.343f}, {621.623f, 459.285f},
            {622.161f, 460.766f}, {622.431f, 462.381f}, {623.104f, 464.535f},
            {623.238f, 466.015f}, {623.373f, 467.496f}, {623.238f, 468.438f},
            {623.373f, 469.515f}, {623.911f, 470.861f}, {623.911f, 471.803f},
            {623.911f, 472.746f}, {623.911f, 473.553f}, {623.911f, 474.899f},
            {624.046f, 475.976f}, {624.046f, 477.322f}, {624.315f, 478.803f},
            {624.584f, 480.015f}, {624.584f, 480.957f}, {624.853f, 482.168f},
            {624.988f, 483.380f}, {625.123f, 484.726f}, {625.527f, 485.803f},
            {625.661f, 486.476f}, {625.930f, 487.149f}, {626.065f, 487.956f},
            {626.469f, 489.033f}, {626.738f, 489.706f}, {627.007f, 490.649f},
            {627.411f, 491.591f}, {627.815f, 492.533f}, {628.219f, 493.341f},
            {628.623f, 494.148f}, {629.296f, 495.091f}, {629.834f, 496.302f},
            {630.507f, 497.379f}, {631.045f, 498.321f}, {631.584f, 498.860f},
            {632.122f, 499.802f}, {632.661f, 500.475f}, {633.468f, 501.417f},
            {634.276f, 502.225f}, {635.084f, 502.898f}, {635.757f, 503.571f},
            {636.430f, 504.110f}, {637.372f, 504.783f}, {638.449f, 505.321f},
            {639.257f, 506.263f}, {640.199f, 506.802f}, {641.007f, 507.609f},
            {641.814f, 508.013f}, {642.756f, 508.417f}, {643.968f, 508.686f},
            {645.314f, 509.090f}, {646.525f, 509.225f}, {647.602f, 509.359f},
            {648.545f, 509.359f}, {649.487f, 509.359f}, {650.833f, 509.090f},
            {651.506f, 509.225f}, {652.448f, 509.090f}, {653.391f, 509.090f},
            {654.333f, 509.090f}, {655.275f, 508.821f}, {656.083f, 508.417f},
            {656.890f, 507.879f}, {657.967f, 507.609f}, {658.506f, 507.071f},
            {659.313f, 506.667f}, {659.986f, 506.129f}, {660.390f, 505.725f},
            {661.198f, 505.456f}, {661.871f, 505.052f}, {662.544f, 505.052f},
            {662.948f, 505.725f}, {663.082f, 506.802f}, {663.082f, 508.013f},
            {663.082f, 508.821f}, {662.948f, 509.898f}, {662.813f, 510.571f},
            {662.544f, 511.109f}, {662.140f, 511.782f}, {661.602f, 512.321f},
            {661.063f, 513.128f}, {660.390f, 513.936f}, {659.448f, 514.609f},
            {658.640f, 515.417f}, {657.833f, 516.090f}, {656.890f, 516.628f},
            {655.410f, 517.032f}, {654.064f, 517.301f}, {652.852f, 517.436f},
            {651.641f, 517.436f}, {650.698f, 517.570f}, {650.160f, 517.570f},
            {648.814f, 517.570f}, {647.064f, 517.570f}, {645.583f, 517.705f},
            {644.103f, 517.705f}, {642.756f, 517.570f}, {641.141f, 517.032f},
            {639.930f, 517.032f}, {638.853f, 516.763f}, {637.507f, 516.224f},
            {636.295f, 515.955f}, {634.545f, 515.417f}, {632.795f, 514.878f},
            {631.449f, 514.340f}, {629.699f, 513.532f}, {627.815f, 512.724f},
            {626.469f, 511.648f}, {625.123f, 510.705f}, {623.373f, 509.628f},
            {622.431f, 508.148f}, {621.354f, 507.071f}, {620.008f, 505.456f},
            {618.527f, 503.571f}, {617.854f, 502.494f}, {617.315f, 501.687f},
            {617.046f, 500.610f}, {616.508f, 499.533f}, {615.835f, 498.187f},
            {615.296f, 496.571f}, {614.489f, 494.956f}, {613.950f, 493.475f},
            {613.546f, 492.129f}, {613.277f, 490.649f}, {612.873f, 489.437f},
            {612.739f, 487.956f}, {612.335f, 486.476f}, {612.066f, 484.860f},
            {611.662f, 483.380f}, {611.123f, 481.630f}, {610.989f, 479.745f},
            {610.450f, 478.130f}, {609.912f, 476.784f}, {609.373f, 475.169f},
            {608.835f, 473.150f}, {608.297f, 471.938f}, {607.893f, 470.996f},
            {607.489f, 469.650f}, {607.085f, 468.169f}, {606.547f, 467.092f},
            {605.874f, 465.881f}, {605.335f, 464.938f}, {604.797f, 463.727f},
            {603.989f, 462.785f}, {603.316f, 461.573f}, {602.778f, 460.092f},
            {601.432f, 458.746f}, {600.355f, 457.804f}, {599.682f, 457.131f},
            {599.009f, 456.458f}, {598.066f, 455.650f}, {596.989f, 454.708f},
            {596.047f, 454.170f}, {595.105f, 453.631f}, {593.759f, 452.958f},
            {592.682f, 452.420f}, {592.009f, 451.478f}, {591.605f, 450.670f},
            {591.336f, 449.862f}, {591.471f, 450.939f}, {591.471f, 451.881f},
            {591.336f, 453.362f}, {591.067f, 454.708f}, {590.797f, 455.785f},
            {590.528f, 457.266f}, {590.394f, 458.477f}, {590.124f, 459.823f},
            {589.721f, 461.439f}, {589.451f, 463.188f}, {589.451f, 464.938f},
            {589.317f, 466.688f}, {589.182f, 468.438f}, {589.182f, 469.650f},
            {589.317f, 470.457f}, {589.317f, 471.130f}, {589.451f, 472.342f},
            {589.855f, 473.419f}, {590.394f, 474.361f}, {590.663f, 475.169f},
            {590.932f, 476.380f}, {591.605f, 477.861f}, {592.278f, 478.938f},
            {592.817f, 480.149f}, {593.490f, 481.495f}, {594.297f, 482.707f},
            {595.105f, 483.649f}, {595.913f, 484.726f}, {596.720f, 485.534f},
            {597.393f, 486.610f}, {598.201f, 487.418f}, {598.605f, 487.956f},
            {599.009f, 488.495f}, {599.951f, 489.303f}, {601.028f, 490.649f},
            {602.105f, 491.995f}, {603.181f, 493.206f}, {604.124f, 494.552f},
            {605.335f, 496.033f}, {606.412f, 497.783f}, {607.085f, 499.129f},
            {608.027f, 500.340f}, {608.431f, 501.552f}, {609.239f, 503.167f},
            {610.047f, 505.321f}, {610.450f, 506.802f}, {610.989f, 508.686f},
            {610.989f, 510.705f}, {610.989f, 512.724f}, {610.989f, 514.744f},
            {610.854f, 517.032f}, {610.585f, 519.051f}, {610.316f, 520.801f},
            {609.373f, 522.820f}, {608.700f, 524.705f}, {607.624f, 526.455f},
            {606.681f, 528.204f}, {605.604f, 529.954f}, {604.258f, 531.300f},
            {603.181f, 532.647f}, {602.374f, 533.185f}, {601.566f, 534.127f},
            {600.355f, 534.935f}, {598.874f, 536.146f}, {597.528f, 536.954f},
            {596.182f, 537.896f}, {594.701f, 538.435f}, {593.490f, 538.973f},
            {592.682f, 539.377f}, {591.605f, 539.646f}, {590.124f, 539.781f},
            {588.778f, 539.781f}, {587.567f, 539.915f}, {586.086f, 539.512f},
            {584.471f, 539.242f}, {582.990f, 538.031f}, {582.048f, 536.819f},
            {581.106f, 535.743f}, {580.836f, 534.262f}, {580.702f, 533.050f},
            {580.702f, 531.704f}, {580.702f, 530.224f}, {581.375f, 529.012f},
            {581.509f, 528.204f}, {582.048f, 527.531f}, {582.317f, 526.993f},
            {582.856f, 527.397f}, {583.259f, 528.339f}, {583.529f, 529.147f},
            {584.067f, 530.358f}, {584.740f, 531.973f}, {585.817f, 532.916f},
            {586.355f, 533.589f}, {586.759f, 533.993f}, {587.701f, 534.127f},
            {588.509f, 534.262f}, {589.586f, 533.993f}, {590.663f, 533.589f},
            {591.336f, 533.185f}, {592.144f, 532.647f}, {593.086f, 531.704f},
            {593.759f, 530.897f}, {594.432f, 530.089f}, {595.105f, 529.012f},
            {595.643f, 528.474f}, {596.047f, 527.666f}, {596.451f, 527.128f},
            {596.989f, 526.320f}, {597.393f, 525.378f}, {598.066f, 524.032f},
            {598.336f, 522.955f}, {598.874f, 521.609f}, {599.143f, 520.532f},
            {599.143f, 519.590f}, {599.278f, 518.513f}, {599.412f, 517.301f},
            {599.278f, 515.820f}, {599.143f, 514.609f}, {598.874f, 513.398f},
            {598.605f, 512.186f}, {598.470f, 511.378f}, {597.932f, 510.167f},
            {597.528f, 508.955f}, {596.720f, 507.340f}, {596.316f, 506.802f},
            {596.047f, 505.725f}, {595.374f, 504.917f}, {594.836f, 503.975f},
            {594.297f, 502.898f}, {593.490f, 501.821f}, {592.547f, 500.879f},
            {591.471f, 499.667f}, {590.528f, 498.994f}, {589.317f, 497.783f},
            {588.509f, 497.244f}, {587.836f, 496.841f}, {587.028f, 496.302f},
            {586.086f, 495.360f}, {585.009f, 494.552f}, {583.663f, 493.610f},
            {582.048f, 492.399f}, {581.509f, 491.726f}, {580.702f, 491.052f},
            {580.029f, 490.379f}, {579.221f, 489.437f}, {578.413f, 488.630f},
            {577.337f, 487.283f}, {576.664f, 486.207f}, {575.721f, 484.995f},
            {575.183f, 484.053f}, {574.510f, 483.111f}, {573.702f, 481.899f},
            {573.029f, 481.091f}, {572.625f, 479.611f}, {572.221f, 478.534f},
            {571.818f, 477.592f}, {571.548f, 477.053f}, {571.279f, 476.380f},
            {571.010f, 475.169f}, {570.741f, 473.957f}, {570.606f, 473.015f},
            {570.337f, 471.803f}, {570.202f, 470.861f}, {570.202f, 469.515f},
            {570.202f, 468.438f}, {570.202f, 467.361f}, {569.933f, 466.015f},
            {569.799f, 464.938f}, {569.933f, 463.862f}, {569.529f, 462.650f},
            {569.529f, 461.439f}, {569.529f, 460.362f}, {569.529f, 459.419f},
            {569.529f, 458.073f}, {569.395f, 456.996f}, {569.529f, 456.323f},
            {569.529f, 455.516f}, {569.529f, 454.304f}, {569.529f, 453.227f},
            {569.529f, 452.151f}, {569.529f, 450.939f}, {569.529f, 449.728f},
            {568.856f, 449.458f}, {568.049f, 449.324f}, {566.972f, 449.324f},
            {565.760f, 449.458f}, {563.607f, 450.131f}, {562.126f, 450.670f},
            {561.049f, 451.478f}, {559.837f, 452.420f}, {558.357f, 454.170f},
            {556.876f, 455.381f}, {555.665f, 456.727f}, {554.722f, 458.477f},
            {553.511f, 460.496f}, {552.972f, 462.381f}, {552.299f, 464.669f},
            {551.761f, 466.688f}, {551.088f, 468.169f}, {550.415f, 469.246f},
            {550.011f, 470.323f}, {549.338f, 471.803f}, {548.396f, 473.688f},
            {547.857f, 475.572f}, {547.184f, 479.072f}, {547.184f, 479.072f},
            {547.184f, 480.688f}, {547.319f, 482.438f}, {547.453f, 483.918f},
            {547.319f, 485.668f}, {547.723f, 487.956f}, {548.127f, 489.976f},
            {548.127f, 491.052f}, {548.396f, 493.610f}, {547.857f, 495.225f},
            {547.588f, 496.706f}, {546.780f, 498.456f}, {546.511f, 502.225f},
            {546.511f, 502.225f}, {547.050f, 503.571f}, {547.319f, 505.725f},
            {546.915f, 506.936f}, {547.050f, 508.282f}, {547.184f, 510.032f},
            {546.915f, 511.378f}, {546.780f, 512.590f}, {546.511f, 513.667f},
            {545.838f, 514.744f}, {545.031f, 516.224f}, {543.954f, 517.436f},
            {543.281f, 518.378f}, {542.204f, 519.186f}, {541.127f, 520.397f},
            {539.781f, 521.474f}, {538.569f, 522.012f}, {537.492f, 522.820f},
            {536.281f, 523.628f}, {535.204f, 524.166f}, {534.531f, 524.839f},
            {533.589f, 525.378f}, {532.512f, 526.051f}, {531.570f, 526.724f},
            {530.493f, 527.128f}, {529.820f, 527.531f}, {529.147f, 527.801f},
            {528.204f, 528.070f}, {526.455f, 528.339f}, {524.301f, 528.608f},
            {523.089f, 528.608f}, {521.743f, 528.877f}, {520.532f, 528.743f},
            {519.320f, 528.877f}, {518.513f, 528.877f}, {517.705f, 528.877f},
            {516.897f, 528.608f}, {515.282f, 528.339f}, {514.071f, 527.935f},
            {512.724f, 527.262f}, {511.917f, 526.993f}, {510.840f, 526.320f},
            {509.898f, 525.647f}, {509.359f, 525.243f}, {508.552f, 524.570f},
            {508.282f, 524.032f}, {507.879f, 523.628f}, {507.475f, 522.955f},
            {507.340f, 522.416f}, {506.936f, 521.609f}, {506.532f, 520.936f},
            {506.129f, 519.859f}, {505.859f, 518.782f}, {505.725f, 517.974f},
            {505.590f, 516.897f}, {505.590f, 516.359f}, {505.321f, 515.282f},
            {505.321f, 514.340f}, {505.321f, 513.532f}, {505.321f, 512.455f},
            {505.725f, 511.513f}, {505.994f, 510.571f}, {506.263f, 509.763f},
            {506.667f, 509.359f}, {507.071f, 508.552f}, {507.744f, 507.744f},
            {508.417f, 507.340f}, {508.955f, 507.071f}, {509.628f, 506.667f},
            {510.436f, 506.398f}, {511.378f, 506.263f}, {512.051f, 506.129f},
            {512.186f, 506.936f}, {511.917f, 508.148f}, {511.648f, 509.359f},
            {511.648f, 509.763f}, {511.513f, 510.436f}, {511.648f, 511.513f},
            {511.648f, 512.051f}, {511.648f, 512.994f}, {511.782f, 513.667f},
            {511.782f, 514.071f}, {511.917f, 514.878f}, {512.321f, 515.686f},
            {512.724f, 516.359f}, {513.263f, 517.032f}, {513.667f, 517.436f},
            {514.474f, 518.109f}, {515.013f, 518.513f}, {515.820f, 518.916f},
            {516.763f, 519.320f}, {517.570f, 519.320f}, {518.378f, 519.320f},
            {519.455f, 519.186f}, {520.397f, 519.051f}, {521.339f, 518.916f},
            {522.685f, 518.378f}, {523.762f, 517.840f}, {524.570f, 517.705f},
            {525.781f, 517.032f}, {526.858f, 516.359f}, {527.531f, 515.686f},
            {528.474f, 514.744f}, {529.012f, 514.071f}, {529.551f, 513.263f},
            {530.089f, 512.590f}, {530.627f, 511.782f}, {531.031f, 510.975f},
            {531.435f, 510.167f}, {532.243f, 508.955f}, {532.512f, 508.148f},
            {533.050f, 506.936f}, {533.320f, 505.994f}, {533.589f, 504.917f},
            {533.723f, 504.110f}, {533.993f, 503.167f}, {534.127f, 502.360f},
            {534.127f, 501.417f}, {533.993f, 500.475f}, {533.858f, 499.802f},
            {533.858f, 499.398f}, {533.858f, 498.456f}, {533.858f, 497.648f},
            {533.589f, 496.841f}, {533.454f, 496.033f}, {533.454f, 495.091f},
            {533.454f, 494.014f}, {533.320f, 492.937f}, {533.185f, 492.129f},
            {533.185f, 491.726f}, {533.050f, 490.783f}, {532.916f, 489.437f},
            {532.647f, 488.360f}, {532.377f, 487.418f}, {532.377f, 486.476f},
            {532.243f, 485.130f}, {531.839f, 484.187f}, {531.704f, 483.111f},
            {531.435f, 481.899f}, {531.166f, 480.553f}, {531.300f, 479.072f},
            {531.300f, 477.726f}, {531.166f, 476.649f}, {531.031f, 475.707f},
            {531.031f, 474.361f}, {531.300f, 473.150f}, {531.031f, 471.534f},
            {531.166f, 470.457f}, {531.704f, 468.842f}, {531.839f, 467.900f},
            {531.973f, 466.823f}, {531.973f, 465.881f}, {532.377f, 464.535f},
            {532.781f, 463.458f}, {533.050f, 462.650f}, {533.320f, 461.708f},
            {533.589f, 461.169f}, {533.993f, 460.362f}, {534.396f, 459.285f},
            {534.666f, 458.477f}, {535.473f, 456.862f}, {535.473f, 456.862f},
            {536.012f, 456.054f}, {536.550f, 455.247f}, {537.627f, 453.766f},
            {537.627f, 453.766f}, {538.031f, 452.824f}, {538.569f, 452.151f},
            {538.973f, 451.478f}, {539.377f, 450.670f}, {539.781f, 450.131f},
            {540.319f, 449.593f}, {541.261f, 448.785f}, {541.935f, 447.978f},
            {542.742f, 447.035f}, {543.684f, 446.362f}, {544.357f, 445.689f},
            {545.031f, 444.882f}, {545.434f, 444.209f}, {546.107f, 443.536f},
            {546.511f, 443.132f}, {547.184f, 442.324f}, {547.588f, 441.920f},
            {548.127f, 441.382f}, {548.665f, 440.843f}, {549.338f, 439.901f},
            {549.876f, 438.959f}, {550.549f, 438.151f}, {551.223f, 437.478f},
            {551.761f, 436.536f}, {552.434f, 435.728f}, {552.838f, 434.651f},
            {552.569f, 435.055f}, {552.165f, 435.459f}, {551.896f, 435.863f},
            {551.492f, 436.536f}, {551.223f, 436.805f}, {550.819f, 437.478f},
            {550.280f, 438.151f}, {550.011f, 438.420f}, {549.607f, 438.959f},
            {549.069f, 439.497f}, {548.800f, 439.901f}, {548.396f, 440.305f},
            {547.992f, 440.709f}, {547.588f, 441.382f}, {547.184f, 441.786f},
            {546.646f, 442.324f}, {545.973f, 443.132f}, {545.434f, 443.670f},
            {545.031f, 444.074f}, {544.492f, 444.612f}, {543.954f, 445.420f},
            {543.415f, 445.824f}, {542.742f, 446.362f}, {542.069f, 446.901f},
            {541.531f, 447.574f}, {540.858f, 448.247f}, {540.185f, 448.920f},
            {539.781f, 449.324f}, {538.973f, 449.593f}, {538.300f, 449.997f},
            {537.492f, 450.401f}, {536.550f, 450.939f}, {535.743f, 451.208f},
            {534.935f, 451.612f}, {534.262f, 452.151f}, {533.589f, 452.285f},
            {532.916f, 452.689f}, {531.973f, 452.824f}, {531.435f, 452.824f},
            {530.493f, 453.093f}, {529.685f, 453.093f}, {528.608f, 453.227f},
            {527.935f, 453.497f}, {527.262f, 453.766f}, {526.320f, 454.035f},
            {525.243f, 454.170f}, {524.301f, 454.304f}, {523.359f, 454.439f},
            {522.820f, 454.708f}, {522.012f, 454.977f}, {521.474f, 455.112f},
            {520.263f, 455.112f}, {519.320f, 455.247f}, {518.109f, 455.112f},
            {517.032f, 455.247f}, {515.282f, 455.381f}, {514.474f, 455.650f},
            {513.532f, 455.785f}, {512.455f, 455.920f}, {511.109f, 456.054f},
            {509.898f, 456.189f}, {509.090f, 456.593f}, {508.148f, 456.727f},
            {506.532f, 457.266f}, {505.052f, 457.266f}, {503.571f, 458.073f},
            {502.090f, 458.612f}, {501.148f, 459.016f}, {500.340f, 459.554f},
            {498.994f, 459.958f}, {497.648f, 460.631f}, {496.571f, 461.304f},
            {495.495f, 461.842f}, {494.418f, 462.381f}, {493.341f, 463.054f},
            {492.533f, 463.592f}, {491.456f, 464.131f}, {490.514f, 465.073f},
            {489.437f, 465.746f}, {488.226f, 466.554f}, {487.149f, 467.092f},
            {486.072f, 468.438f}, {485.264f, 468.977f}, {484.322f, 469.919f},
            {483.245f, 470.996f}, {482.438f, 472.207f}, {481.361f, 473.419f},
            {480.553f, 474.361f}, {479.476f, 475.438f}, {478.668f, 476.649f},
            {477.995f, 478.265f}, {477.188f, 480.015f}, {476.515f, 481.226f},
            {476.246f, 482.841f}, {476.246f, 484.457f}, {476.246f, 486.072f},
            {476.380f, 487.822f}, {476.515f, 489.033f}, {476.919f, 490.514f},
            {477.322f, 491.860f}, {477.457f, 493.072f}, {477.726f, 494.148f},
            {477.995f, 495.360f}, {478.130f, 496.571f}, {478.130f, 497.783f},
            {478.130f, 498.725f}, {477.995f, 499.667f}, {477.592f, 500.475f},
            {476.649f, 500.879f}, {475.438f, 501.148f}, {474.226f, 500.610f},
            {473.015f, 499.937f}, {472.207f, 499.398f}, {471.669f, 498.725f},
            {470.861f, 497.783f}, {470.188f, 496.302f}, {469.650f, 495.225f},
            {469.246f, 493.745f}, {468.707f, 492.533f}, {468.573f, 491.591f},
            {468.169f, 490.245f}, {468.034f, 487.822f}, {468.034f, 487.822f},
            {468.034f, 487.014f}, {468.034f, 486.072f}, {468.034f, 484.860f},
            {468.034f, 483.111f}, {468.169f, 482.303f}, {468.169f, 481.226f},
            {468.573f, 479.880f}, {468.707f, 478.938f}, {469.111f, 477.861f},
            {469.111f, 477.053f}, {469.515f, 475.572f}, {469.919f, 474.630f},
            {470.457f, 473.553f}, {470.592f, 472.611f}, {471.130f, 471.803f},
            {471.400f, 471.130f}, {472.207f, 469.784f}, {472.746f, 468.842f},
            {473.284f, 468.169f}, {473.688f, 467.496f}, {474.092f, 466.554f},
            {474.765f, 465.611f}, {475.707f, 464.804f}, {476.111f, 463.996f},
            {476.649f, 463.592f}, {477.188f, 462.650f}, {477.995f, 461.708f},
            {478.668f, 460.766f}, {479.476f, 459.823f}, {480.015f, 458.881f},
            {481.091f, 458.073f}, {481.630f, 457.535f}, {482.303f, 456.862f},
            {483.245f, 456.323f}, {484.187f, 455.650f}, {485.130f, 454.843f},
            {485.803f, 454.304f}, {486.610f, 453.631f}, {487.553f, 453.227f},
            {488.495f, 452.554f}, {489.437f, 452.151f}, {490.514f, 451.343f},
            {492.264f, 450.670f}, {493.206f, 450.266f}, {494.014f, 449.728f},
            {494.687f, 449.458f}, {495.495f, 449.189f}, {496.302f, 448.920f},
            {498.052f, 447.978f}, {499.264f, 447.305f}, {500.744f, 447.035f},
            {501.687f, 446.497f}, {502.763f, 446.093f}, {503.571f, 446.093f},
            {505.186f, 445.689f}, {506.532f, 445.555f}, {507.609f, 445.016f},
            {508.417f, 444.882f}, {510.167f, 444.478f}, {511.378f, 444.209f},
            {512.994f, 443.939f}, {514.340f, 443.536f}, {515.417f, 443.401f},
            {516.494f, 443.266f}, {517.840f, 442.863f}, {519.320f, 442.324f},
            {521.339f, 441.516f}, {524.301f, 440.709f}, {524.974f, 440.305f},
            {526.320f, 439.901f}, {527.128f, 439.497f}, {527.801f, 438.959f},
            {528.877f, 438.286f}, {530.089f, 437.747f}, {531.435f, 437.209f},
            {532.647f, 436.805f}, {534.262f, 435.863f}, {535.473f, 434.921f},
            {537.358f, 433.844f}, {538.973f, 432.767f}, {540.319f, 431.690f},
            {541.261f, 431.286f}, {542.204f, 430.209f}, {542.877f, 429.402f},
            {543.550f, 428.863f}, {544.357f, 427.921f}, {544.896f, 427.248f},
            {545.434f, 426.575f}, {546.107f, 425.902f}, {546.780f, 425.229f},
            {547.453f, 424.421f}, {546.646f, 424.556f}, {546.107f, 425.229f},
            {545.704f, 425.633f}, {544.896f, 426.710f}, {544.896f, 426.710f},
            {544.357f, 427.248f}, {543.819f, 427.921f}, {543.281f, 428.459f},
            {542.608f, 428.863f}, {541.935f, 429.536f}, {541.261f, 430.075f},
            {540.723f, 430.748f}, {539.512f, 431.421f}, {538.435f, 432.094f},
            {537.492f, 432.632f}, {536.685f, 433.305f}, {535.877f, 433.709f},
            {534.935f, 434.248f}, {533.858f, 434.921f}, {533.185f, 435.190f},
            {532.243f, 435.863f}, {530.897f, 436.132f}, {530.089f, 436.267f},
            {528.877f, 436.671f}, {528.070f, 436.671f}, {526.858f, 436.536f},
            {525.916f, 436.267f}, {524.974f, 435.863f}, {523.493f, 435.324f},
            {522.282f, 434.651f}, {521.205f, 434.113f}, {520.128f, 433.305f},
            {519.186f, 432.767f}, {517.974f, 432.094f}, {516.628f, 430.882f},
            {515.282f, 430.075f}, {514.340f, 429.132f}, {513.532f, 428.056f},
            {512.859f, 427.248f}, {512.051f, 426.306f}, {511.782f, 425.633f},
            {511.378f, 424.960f}, {510.705f, 423.883f}, {510.167f, 423.075f},
            {509.628f, 421.594f}, {508.821f, 420.652f}, {508.552f, 419.710f},
            {508.013f, 418.633f}, {507.744f, 417.556f}, {507.340f, 416.883f},
            {506.802f, 416.075f}, {506.532f, 414.999f}, {505.994f, 414.191f},
            {505.590f, 413.383f}, {505.052f, 412.441f}, {504.513f, 411.768f},
            {503.975f, 410.826f}, {503.436f, 409.749f}, {502.763f, 408.672f},
            {501.956f, 407.999f}, {501.148f, 407.191f}, {500.071f, 406.384f},
            {498.994f, 405.711f}, {497.379f, 404.768f}, {496.168f, 404.499f},
            {494.687f, 404.095f}, {493.475f, 404.095f}, {492.129f, 403.961f},
            {490.649f, 403.961f}, {489.033f, 403.961f}, {487.553f, 404.364f},
            {486.207f, 405.038f}, {484.726f, 405.576f}, {483.918f, 406.787f},
            {482.976f, 407.864f}, {482.168f, 409.480f}, {481.495f, 410.826f},
            {480.957f, 412.172f}, {480.822f, 412.845f}, {479.880f, 414.729f},
            {479.880f, 414.729f}, {479.072f, 415.537f}, {478.399f, 416.075f},
            {477.457f, 416.210f}, {476.784f, 415.537f}, {476.515f, 414.999f},
            {476.380f, 414.326f}, {476.246f, 413.249f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.302f, 0.122f, 0.431f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.91f, 0.455f, 0.682f, 1.0f);
    }

    //? Eye's Brow
    {
        static const float p[][2] = {
            {582.183f, 396.441f}, {582.650f, 395.693f}, {583.024f, 394.946f},
            {583.678f, 394.104f}, {584.333f, 393.263f}, {585.267f, 392.515f},
            {586.202f, 391.580f}, {587.230f, 391.113f}, {588.165f, 390.552f},
            {588.913f, 390.178f}, {589.941f, 389.711f}, {590.689f, 389.430f},
            {591.811f, 389.243f}, {592.559f, 389.243f}, {593.026f, 389.804f},
            {593.306f, 391.020f}, {593.119f, 391.393f}, {592.839f, 391.767f},
            {591.717f, 392.048f}, {590.876f, 392.422f}, {590.035f, 392.796f},
            {589.193f, 393.263f}, {588.352f, 393.730f}, {587.604f, 394.478f},
            {586.950f, 395.226f}, {586.109f, 395.787f}, {585.267f, 396.909f},
            {584.800f, 397.563f}, {584.426f, 397.843f}, {583.959f, 398.030f},
            {583.304f, 397.750f}, {582.743f, 397.376f}, {582.276f, 396.815f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.302f, 0.122f, 0.431f, 1.0f);
    }

    //? Eye's Brow Light
    {
        static const float p[][2] = {
            {581.248f, 408.780f}, {580.593f, 408.313f}, {580.126f, 407.752f},
            {580.033f, 407.004f}, {579.939f, 406.350f}, {579.939f, 405.322f},
            {579.939f, 404.480f}, {579.939f, 403.452f}, {580.033f, 402.517f},
            {580.219f, 401.583f}, {580.406f, 400.648f}, {580.593f, 399.620f},
            {580.780f, 398.685f}, {581.154f, 398.217f}, {581.341f, 397.656f},
            {581.435f, 397.096f}, {581.902f, 396.628f}, {582.369f, 395.974f},
            {582.650f, 395.506f}, {583.117f, 394.852f}, {583.398f, 394.198f},
            {583.865f, 393.637f}, {584.426f, 393.170f}, {584.987f, 392.609f},
            {585.454f, 392.141f}, {585.828f, 391.767f}, {586.296f, 391.487f},
            {586.856f, 391.113f}, {587.417f, 390.833f}, {587.698f, 390.739f},
            {588.165f, 390.459f}, {588.539f, 390.365f}, {588.913f, 390.085f},
            {589.474f, 389.804f}, {589.848f, 389.617f}, {590.315f, 389.430f},
            {590.969f, 389.150f}, {591.530f, 388.963f}, {592.185f, 388.870f},
            {592.746f, 388.683f}, {593.306f, 388.683f}, {594.054f, 388.402f},
            {594.896f, 388.402f}, {595.456f, 388.402f}, {596.204f, 388.402f},
            {596.859f, 388.496f}, {597.606f, 388.776f}, {598.448f, 389.056f},
            {599.289f, 389.430f}, {600.037f, 389.711f}, {600.785f, 390.178f},
            {601.439f, 390.459f}, {602.280f, 391.113f}, {603.028f, 391.954f},
            {603.402f, 392.328f}, {603.963f, 393.076f}, {604.243f, 394.291f},
            {604.430f, 396.441f}, {604.243f, 399.246f}, {604.150f, 400.554f},
            {603.963f, 401.489f}, {603.215f, 400.180f}, {602.935f, 399.339f},
            {602.748f, 398.498f}, {602.374f, 397.283f}, {601.626f, 395.880f},
            {601.159f, 395.039f}, {600.785f, 394.385f}, {600.224f, 393.917f},
            {599.569f, 393.450f}, {598.728f, 392.983f}, {598.354f, 392.889f},
            {597.793f, 392.609f}, {597.233f, 392.235f}, {596.765f, 392.048f},
            {596.204f, 391.767f}, {595.269f, 391.767f}, {594.802f, 391.767f},
            {593.961f, 391.487f}, {593.306f, 391.767f}, {592.746f, 391.954f},
            {592.185f, 391.954f}, {591.811f, 392.141f}, {591.437f, 392.235f},
            {590.969f, 392.515f}, {590.409f, 392.609f}, {590.035f, 392.889f},
            {589.567f, 393.076f}, {589.193f, 393.263f}, {588.819f, 393.543f},
            {588.633f, 393.730f}, {588.259f, 393.917f}, {587.978f, 394.104f},
            {587.698f, 394.478f}, {587.230f, 394.852f}, {586.856f, 395.226f},
            {586.483f, 395.693f}, {586.202f, 396.067f}, {585.735f, 396.441f},
            {585.454f, 396.722f}, {585.174f, 397.096f}, {584.800f, 397.656f},
            {584.333f, 397.937f}, {583.865f, 398.591f}, {583.678f, 399.152f},
            {583.491f, 399.620f}, {583.398f, 400.087f}, {583.211f, 400.461f},
            {583.024f, 401.302f}, {582.930f, 401.956f}, {582.930f, 402.517f},
            {582.743f, 403.265f}, {582.743f, 404.200f}, {582.837f, 404.948f},
            {582.930f, 405.789f}, {583.211f, 406.630f}, {583.398f, 407.378f},
            {583.585f, 408.033f}, {583.678f, 408.687f}, {583.678f, 409.154f},
            {583.304f, 409.248f}, {582.930f, 409.154f}, {582.463f, 409.061f},
            {582.089f, 409.061f}, {581.715f, 408.874f}, {581.435f, 408.874f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.659f, 0.502f, 0.659f, 1.0f);
    }
    //* Eye
    {
        static const float p[][2] = {
            {598.167f, 399.339f}, {598.915f, 400.087f}, {599.476f, 400.741f},
            {599.756f, 401.022f}, {600.130f, 401.396f}, {600.504f, 401.956f},
            {600.878f, 402.424f}, {601.159f, 403.078f}, {601.252f, 403.920f},
            {601.252f, 405.415f}, {601.252f, 405.415f}, {601.065f, 406.256f},
            {600.224f, 407.939f}, {600.224f, 407.939f}, {599.663f, 408.780f},
            {599.102f, 409.435f}, {598.354f, 410.089f}, {597.793f, 410.463f},
            {597.233f, 410.837f}, {595.737f, 411.491f}, {595.737f, 411.491f},
            {594.989f, 411.585f}, {593.867f, 411.491f}, {591.904f, 411.211f},
            {591.904f, 411.211f}, {591.063f, 410.837f}, {590.502f, 410.463f},
            {589.941f, 409.902f}, {589.567f, 409.341f}, {589.006f, 408.687f},
            {588.913f, 407.939f}, {588.819f, 407.098f}, {588.819f, 406.163f},
            {588.913f, 404.948f}, {589.100f, 404.574f}, {589.287f, 404.013f},
            {589.661f, 403.078f}, {590.222f, 402.330f}, {590.596f, 401.956f},
            {591.343f, 401.209f}, {592.091f, 400.648f}, {592.839f, 400.274f},
            {593.680f, 399.713f}, {594.522f, 399.526f}, {595.176f, 399.246f},
            {596.017f, 399.246f}, {596.672f, 399.246f}, {597.326f, 399.339f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.302f, 0.122f, 0.431f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    //* Eye Black Ball
    {
        static const float p[][2] = {
            {592.465f, 403.826f}, {592.933f, 403.546f}, {593.493f, 403.452f},
            {594.148f, 403.452f}, {594.709f, 403.452f}, {595.176f, 403.452f},
            {595.550f, 403.546f}, {595.924f, 403.733f}, {596.391f, 403.920f},
            {596.578f, 404.200f}, {596.952f, 404.574f}, {597.046f, 405.041f},
            {597.233f, 405.509f}, {597.139f, 405.883f}, {597.046f, 406.350f},
            {596.672f, 406.817f}, {596.017f, 407.191f}, {595.643f, 407.285f},
            {595.269f, 407.285f}, {594.802f, 407.378f}, {594.241f, 407.378f},
            {593.587f, 407.378f}, {593.026f, 407.004f}, {592.465f, 406.443f},
            {592.091f, 406.070f}, {591.998f, 405.509f}, {591.904f, 405.135f},
            {591.904f, 404.761f}, {591.998f, 404.387f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    //? Body Polygon

     //* Body white 1
    {
        static const float p[][2] = {
            {585.174f, 361.948f}, {586.669f, 360.639f}, {587.698f, 359.891f},
            {588.633f, 359.330f}, {589.661f, 359.704f}, {589.754f, 360.639f},
            {589.287f, 361.574f}, {588.539f, 362.322f}, {587.511f, 363.911f},
            {587.791f, 365.126f}, {587.698f, 365.967f}, {586.763f, 366.715f},
            {585.735f, 366.996f}, {584.519f, 366.435f}, {583.491f, 365.313f},
            {582.930f, 364.378f}, {583.772f, 363.443f}, {584.519f, 362.789f},
            {585.080f, 362.135f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.659f, 0.502f, 0.659f, 1.0f);
    }

    //* Body white 2
    {
        static const float p[][2] = {
            {603.776f, 363.256f}, {604.337f, 362.696f}, {604.898f, 362.228f},
            {606.019f, 361.480f}, {606.861f, 360.920f}, {607.889f, 360.452f},
            {608.543f, 360.359f}, {608.637f, 361.106f}, {608.730f, 361.667f},
            {608.824f, 362.415f}, {608.824f, 363.350f}, {608.450f, 364.098f},
            {607.702f, 364.565f}, {606.206f, 364.285f}, {605.272f, 364.098f},
            {604.524f, 363.724f}, {603.869f, 363.537f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.659f, 0.502f, 0.659f, 1.0f);
    }

     //* Body white 3
    {
        static const float p[][2] = {
            {630.885f, 382.700f}, {631.165f, 382.233f}, {631.352f, 381.765f},
            {631.633f, 381.111f}, {632.006f, 380.643f}, {632.567f, 380.176f},
            {633.689f, 379.896f}, {634.624f, 379.896f}, {635.559f, 380.737f},
            {635.652f, 381.672f}, {635.652f, 382.606f}, {635.652f, 383.354f},
            {635.278f, 384.663f}, {634.811f, 385.691f}, {634.250f, 386.439f},
            {633.596f, 387.280f}, {632.754f, 387.654f}, {632.006f, 388.028f},
            {630.978f, 387.841f}, {630.324f, 387.374f}, {630.230f, 386.626f},
            {630.230f, 385.598f}, {630.230f, 384.943f}, {630.417f, 384.009f},
            {630.604f, 383.541f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.659f, 0.502f, 0.659f, 1.0f);
    }

     //* Body white 4
    {
        static const float p[][2] = {
            {639.578f, 354.189f}, {639.298f, 353.722f}, {638.924f, 352.974f},
            {638.456f, 352.506f}, {637.989f, 352.039f}, {637.335f, 351.665f},
            {636.867f, 351.291f}, {636.493f, 350.450f}, {635.839f, 350.076f},
            {635.465f, 349.796f}, {634.904f, 349.796f}, {634.250f, 349.983f},
            {633.315f, 349.796f}, {632.287f, 349.141f}, {631.446f, 348.861f},
            {630.698f, 348.487f}, {630.137f, 348.020f}, {629.856f, 347.833f},
            {629.389f, 347.365f}, {629.202f, 346.898f}, {629.015f, 346.150f},
            {629.389f, 345.776f}, {629.950f, 345.589f}, {630.698f, 345.683f},
            {631.352f, 345.870f}, {632.100f, 346.243f}, {633.222f, 346.898f},
            {634.437f, 347.459f}, {635.465f, 347.926f}, {636.587f, 348.487f},
            {637.615f, 349.048f}, {638.176f, 349.515f}, {639.017f, 350.076f},
            {640.046f, 350.730f}, {640.980f, 351.478f}, {641.822f, 352.039f},
            {642.850f, 353.067f}, {643.878f, 354.189f}, {644.906f, 355.404f},
            {645.654f, 356.152f}, {646.496f, 357.274f}, {647.430f, 358.489f},
            {648.085f, 359.704f}, {648.739f, 360.920f}, {649.487f, 362.228f},
            {650.141f, 363.724f}, {650.515f, 364.752f}, {650.796f, 365.874f},
            {651.169f, 367.370f}, {651.730f, 368.865f}, {651.730f, 369.987f},
            {651.637f, 371.576f}, {651.730f, 372.885f}, {651.730f, 374.474f},
            {651.543f, 376.156f}, {651.543f, 377.746f}, {651.543f, 379.428f},
            {651.169f, 381.204f}, {651.076f, 382.326f}, {650.796f, 384.009f},
            {650.702f, 385.598f}, {650.422f, 387.374f}, {649.954f, 389.056f},
            {649.580f, 390.459f}, {648.833f, 391.954f}, {648.365f, 393.543f},
            {647.711f, 394.852f}, {647.056f, 396.441f}, {646.309f, 397.376f},
            {645.654f, 398.311f}, {644.906f, 399.246f}, {644.252f, 400.087f},
            {643.598f, 401.209f}, {642.476f, 402.330f}, {641.822f, 403.265f},
            {640.887f, 404.013f}, {639.672f, 404.948f}, {638.643f, 405.789f},
            {638.083f, 406.443f}, {637.335f, 407.378f}, {636.587f, 407.939f},
            {635.839f, 408.500f}, {634.904f, 408.874f}, {633.969f, 409.435f},
            {632.848f, 409.902f}, {632.006f, 410.370f}, {630.885f, 410.930f},
            {629.576f, 411.304f}, {628.267f, 411.585f}, {627.146f, 412.052f},
            {626.211f, 412.520f}, {625.556f, 412.987f}, {624.528f, 413.174f},
            {623.593f, 413.361f}, {622.191f, 413.548f}, {621.163f, 414.015f},
            {620.228f, 414.483f}, {619.200f, 415.043f}, {618.172f, 415.698f},
            {617.143f, 415.791f}, {616.022f, 415.978f}, {614.993f, 416.726f},
            {613.685f, 417.100f}, {612.750f, 417.567f}, {611.628f, 418.035f},
            {610.693f, 418.689f}, {609.759f, 419.156f}, {609.011f, 419.530f},
            {607.983f, 420.372f}, {607.515f, 421.026f}, {607.048f, 421.400f},
            {606.113f, 421.961f}, {605.739f, 422.615f}, {605.459f, 423.176f},
            {604.898f, 424.017f}, {604.711f, 425.139f}, {604.524f, 426.167f},
            {604.430f, 427.009f}, {604.243f, 428.037f}, {604.243f, 429.065f},
            {604.243f, 429.906f}, {604.617f, 430.935f}, {605.178f, 431.870f},
            {605.646f, 432.711f}, {606.300f, 433.552f}, {606.954f, 434.767f},
            {607.609f, 435.515f}, {608.543f, 436.637f}, {609.198f, 437.852f},
            {609.852f, 438.506f}, {610.974f, 439.535f}, {612.189f, 440.470f},
            {613.217f, 441.311f}, {614.152f, 441.872f}, {615.367f, 442.620f},
            {616.583f, 443.367f}, {617.798f, 444.115f}, {618.826f, 444.676f},
            {619.761f, 445.424f}, {621.069f, 445.891f}, {622.285f, 446.078f},
            {623.126f, 446.639f}, {623.874f, 447.293f}, {625.463f, 447.948f},
            {626.585f, 448.509f}, {627.519f, 448.976f}, {628.641f, 449.537f},
            {629.109f, 449.724f}, {630.230f, 450.472f}, {631.072f, 451.687f},
            {632.567f, 453.370f}, {633.596f, 454.678f}, {634.811f, 455.426f},
            {636.119f, 455.893f}, {637.148f, 456.548f}, {636.306f, 456.641f},
            {634.624f, 456.548f}, {633.128f, 456.454f}, {631.726f, 456.361f},
            {630.324f, 456.080f}, {629.202f, 455.800f}, {627.893f, 455.800f},
            {626.398f, 455.800f}, {624.996f, 455.800f}, {623.780f, 455.800f},
            {622.378f, 455.800f}, {621.069f, 455.520f}, {620.228f, 454.585f},
            {619.480f, 453.556f}, {618.733f, 452.528f}, {618.172f, 451.593f},
            {617.517f, 450.378f}, {616.676f, 449.817f}, {615.741f, 449.070f},
            {615.554f, 448.602f}, {615.274f, 447.761f}, {614.526f, 447.200f},
            {614.059f, 446.733f}, {612.937f, 445.704f}, {612.189f, 444.956f},
            {610.974f, 443.928f}, {610.319f, 443.274f}, {609.665f, 442.713f},
            {609.104f, 442.246f}, {608.543f, 441.778f}, {607.796f, 441.030f},
            {607.141f, 440.376f}, {606.674f, 440.189f}, {605.459f, 439.535f},
            {604.150f, 438.600f}, {603.028f, 438.226f}, {602.187f, 437.572f},
            {601.252f, 436.917f}, {600.411f, 436.263f}, {599.663f, 435.609f},
            {598.915f, 434.580f}, {598.728f, 434.113f}, {598.448f, 433.085f},
            {598.261f, 432.150f}, {597.793f, 431.122f}, {597.606f, 430.280f},
            {597.606f, 429.439f}, {597.419f, 428.317f}, {597.513f, 427.289f},
            {597.419f, 426.167f}, {597.419f, 425.046f}, {597.606f, 423.830f},
            {597.793f, 422.615f}, {598.074f, 421.587f}, {598.448f, 420.652f},
            {599.196f, 419.250f}, {599.663f, 418.035f}, {600.598f, 416.913f},
            {601.159f, 416.072f}, {602.093f, 415.137f}, {603.496f, 414.015f},
            {604.337f, 413.267f}, {605.646f, 412.800f}, {606.767f, 411.772f},
            {608.076f, 410.743f}, {609.291f, 409.622f}, {610.413f, 408.687f},
            {611.909f, 407.752f}, {612.843f, 407.285f}, {613.778f, 406.537f},
            {615.180f, 405.415f}, {616.583f, 404.667f}, {617.517f, 403.639f},
            {619.200f, 402.517f}, {620.509f, 402.050f}, {621.911f, 400.648f},
            {623.033f, 400.087f}, {624.622f, 399.433f}, {626.117f, 398.124f},
            {627.519f, 397.189f}, {628.735f, 396.441f}, {629.763f, 395.787f},
            {630.604f, 394.665f}, {631.726f, 393.824f}, {632.380f, 393.076f},
            {633.035f, 392.702f}, {633.689f, 392.328f}, {634.530f, 391.487f},
            {635.559f, 390.646f}, {636.119f, 390.085f}, {637.054f, 389.337f},
            {637.989f, 388.402f}, {638.550f, 387.748f}, {639.298f, 386.720f},
            {640.139f, 385.878f}, {640.700f, 385.130f}, {641.074f, 384.196f},
            {641.448f, 383.354f}, {642.102f, 382.326f}, {642.756f, 381.111f},
            {643.691f, 379.522f}, {643.878f, 378.493f}, {643.972f, 377.372f},
            {643.972f, 376.156f}, {643.878f, 374.941f}, {643.785f, 374.100f},
            {643.411f, 373.539f}, {642.943f, 373.259f}, {642.476f, 372.885f},
            {642.102f, 372.791f}, {641.728f, 372.511f}, {641.448f, 371.763f},
            {641.167f, 371.015f}, {641.167f, 370.267f}, {641.167f, 369.706f},
            {641.167f, 368.865f}, {641.074f, 368.024f}, {641.074f, 367.183f},
            {641.915f, 366.809f}, {642.196f, 366.154f}, {642.476f, 365.874f},
            {642.476f, 364.752f}, {642.289f, 364.004f}, {642.196f, 363.817f},
            {641.822f, 363.070f}, {641.541f, 362.135f}, {641.448f, 361.667f},
            {641.167f, 361.106f}, {640.980f, 360.452f}, {640.793f, 359.704f},
            {640.700f, 358.956f}, {640.606f, 358.115f}, {640.326f, 357.087f},
            {640.233f, 356.620f}, {640.139f, 355.965f}, {639.952f, 355.404f},
            {639.859f, 354.843f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.659f, 0.502f, 0.659f, 1.0f);
    }

    //* White poloygon
    {
        static const float p[][2] = {
            {588.446f, 354.656f}, {589.287f, 353.815f}, {590.689f, 352.506f},
            {591.624f, 351.759f}, {592.746f, 351.104f}, {593.961f, 350.263f},
            {595.643f, 349.141f}, {597.139f, 348.393f}, {598.541f, 347.739f},
            {599.663f, 347.365f}, {600.598f, 346.804f}, {602.093f, 346.243f},
            {603.309f, 345.683f}, {604.524f, 345.215f}, {606.113f, 344.748f},
            {607.515f, 344.093f}, {608.730f, 343.906f}, {609.759f, 343.720f},
            {611.441f, 343.252f}, {612.750f, 343.346f}, {613.685f, 343.252f},
            {615.180f, 342.972f}, {616.863f, 342.785f}, {617.798f, 342.972f},
            {619.200f, 342.785f}, {620.696f, 343.065f}, {621.630f, 343.439f},
            {622.191f, 344.000f}, {621.911f, 345.122f}, {620.976f, 345.963f},
            {620.322f, 346.524f}, {618.919f, 347.365f}, {617.143f, 348.580f},
            {617.143f, 348.580f}, {616.209f, 349.048f}, {615.274f, 349.422f},
            {614.526f, 349.515f}, {613.591f, 349.702f}, {612.937f, 349.889f},
            {612.283f, 349.983f}, {611.067f, 350.170f}, {610.133f, 350.356f},
            {608.917f, 350.824f}, {607.702f, 351.104f}, {606.580f, 351.572f},
            {605.178f, 351.665f}, {604.150f, 351.478f}, {603.309f, 351.104f},
            {602.654f, 350.637f}, {601.719f, 349.983f}, {601.065f, 349.702f},
            {600.598f, 349.983f}, {599.756f, 350.450f}, {598.822f, 351.011f},
            {598.261f, 351.385f}, {596.952f, 352.226f}, {596.111f, 352.974f},
            {595.176f, 353.815f}, {594.148f, 354.563f}, {593.026f, 354.656f},
            {591.904f, 354.470f}, {590.689f, 354.376f}, {589.848f, 354.563f},
            {589.006f, 355.311f}, {588.259f, 355.965f}, {587.511f, 356.433f},
            {587.137f, 356.152f}, {586.950f, 355.685f}, {587.230f, 355.498f},
            {587.791f, 355.124f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.980f, 0.824f, 0.847f, 1.0f);
    }
}

//! =================       Green Leaf    =================
void drawGreenLeaft(){
    {
        static const float p[][2] = {
            {434.345f, 635.555f}, {433.759f, 634.090f}, {431.416f, 631.162f},
            {430.830f, 629.990f}, {430.244f, 629.404f}, {426.730f, 627.940f},
            {426.730f, 627.940f}, {424.679f, 627.647f}, {423.215f, 627.061f},
            {421.165f, 626.475f}, {419.993f, 626.183f}, {418.822f, 624.718f},
            {417.943f, 623.547f}, {416.772f, 622.668f}, {416.772f, 621.204f},
            {417.357f, 620.325f}, {419.115f, 620.325f}, {421.458f, 620.325f},
            {423.801f, 621.204f}, {425.558f, 621.496f}, {427.023f, 621.789f},
            {429.366f, 622.961f}, {432.002f, 624.132f}, {434.052f, 625.304f},
            {435.223f, 626.475f}, {436.688f, 627.354f}, {437.859f, 628.233f},
            {439.909f, 629.990f}, {441.667f, 631.747f}, {443.131f, 632.919f},
            {444.303f, 634.969f}, {444.595f, 636.433f}, {441.081f, 635.262f},
            {438.738f, 635.555f}, {436.688f, 635.848f}, {434.345f, 636.141f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.102f, 0.42f, 0.149f, 1.00f, 4.0f);
        drawFilledTessCall(p, n, 0.176f, 0.71f, 0.259f, 1.00f);
    }
    {
        static const float p[][2] = {
            {445.181f, 636.141f}, {444.595f, 634.969f}, {443.424f, 632.626f},
            {441.960f, 630.869f}, {439.324f, 627.354f}, {437.566f, 626.183f},
            {435.809f, 624.718f}, {434.052f, 623.839f}, {432.880f, 622.375f},
            {431.709f, 620.325f}, {430.830f, 618.568f}, {429.658f, 616.810f},
            {429.073f, 614.760f}, {427.901f, 613.003f}, {427.023f, 610.953f},
            {427.023f, 609.195f}, {426.144f, 607.438f}, {426.144f, 605.388f},
            {426.437f, 602.752f}, {427.023f, 600.409f}, {427.901f, 598.944f},
            {428.194f, 597.773f}, {428.487f, 597.480f}, {429.658f, 599.237f},
            {430.244f, 600.995f}, {430.537f, 602.166f}, {430.830f, 603.045f},
            {431.416f, 605.095f}, {432.002f, 606.852f}, {432.587f, 608.317f},
            {433.173f, 609.488f}, {434.345f, 611.538f}, {435.516f, 613.296f},
            {436.688f, 615.053f}, {437.273f, 616.225f}, {438.445f, 617.689f},
            {439.909f, 619.446f}, {441.374f, 621.204f}, {442.838f, 622.668f},
            {443.424f, 623.547f}, {444.595f, 625.597f}, {445.767f, 627.940f},
            {446.353f, 629.697f}, {447.524f, 632.040f}, {448.403f, 634.090f},
            {448.696f, 635.555f}, {448.696f, 636.726f}, {445.474f, 637.312f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.102f, 0.42f, 0.149f, 1.00f, 4.0f);
        drawFilledTessCall(p, n, 0.176f, 0.71f, 0.259f, 1.00f);
    }
    {
        static const float p[][2] = {
            {449.574f, 636.141f}, {448.696f, 634.090f}, {447.817f, 631.747f},
            {446.939f, 629.404f}, {446.939f, 627.940f}, {445.474f, 626.183f},
            {445.181f, 623.839f}, {444.888f, 621.789f}, {444.888f, 620.325f},
            {444.888f, 618.568f}, {444.595f, 617.396f}, {444.010f, 615.639f},
            {442.838f, 613.589f}, {442.545f, 612.417f}, {441.374f, 609.488f},
            {441.374f, 609.488f}, {440.495f, 607.438f}, {439.616f, 605.974f},
            {439.616f, 604.509f}, {438.152f, 600.409f}, {437.566f, 598.066f},
            {437.273f, 595.723f}, {437.273f, 594.258f}, {437.273f, 592.794f},
            {437.273f, 591.037f}, {437.273f, 589.279f}, {437.273f, 587.815f},
            {437.273f, 586.058f}, {437.566f, 584.886f}, {437.566f, 583.715f},
            {437.859f, 581.957f}, {438.152f, 579.907f}, {438.738f, 578.443f},
            {439.324f, 576.685f}, {439.909f, 575.221f}, {440.495f, 574.342f},
            {441.667f, 574.049f}, {442.838f, 574.635f}, {444.303f, 576.100f},
            {445.181f, 577.564f}, {445.181f, 579.321f}, {445.181f, 581.079f},
            {445.181f, 583.422f}, {444.303f, 585.179f}, {443.717f, 587.229f},
            {442.838f, 589.279f}, {442.545f, 591.622f}, {442.545f, 593.965f},
            {442.545f, 596.309f}, {442.838f, 598.066f}, {443.717f, 600.116f},
            {444.595f, 601.580f}, {445.767f, 603.045f}, {447.231f, 603.338f},
            {447.817f, 605.388f}, {447.817, 606.852f}, {447.524f, 608.902f},
            {447.524f, 610.953f}, {447.524f, 613.003f}, {447.817f, 615.053f},
            {448.110f, 617.103f}, {448.696f, 618.860f}, {448.989f, 620.911f},
            {449.574f, 622.375f}, {450.160f, 625.011f}, {450.746f, 626.475f},
            {451.332f, 628.526f}, {451.918f, 631.747f}, {452.503f, 633.505f},
            {453.089f, 634.969f}, {453.382f, 636.141f}, {449.574f, 636.141f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.102f, 0.42f, 0.149f, 1.00f, 4.0f);
        drawFilledTessCall(p, n, 0.176f, 0.71f, 0.259f, 1.00f);
    }
    {
        static const float p[][2] = {
            {452.796f, 636.433f}, {452.503f, 634.383f}, {452.210f, 632.626f},
            {451.918f, 630.576f}, {451.332f, 627.940f}, {450.746f, 625.890f},
            {449.867f, 624.425f}, {449.574f, 622.082f}, {448.989f, 620.325f},
            {448.696f, 617.982f}, {448.403f, 615.639f}, {448.403f, 613.589f},
            {447.817f, 611.538f}, {447.817f, 608.902f}, {447.817f, 606.852f},
            {447.817f, 604.509f}, {448.110f, 602.459f}, {448.696f, 600.995f},
            {448.989f, 599.823f}, {448.989f, 598.652f}, {449.574f, 597.187f},
            {449.867f, 595.723f}, {451.332f, 591.915f}, {451.625f, 589.865f},
            {451.918f, 588.108f}, {452.210f, 587.229f}, {452.796f, 584.886f},
            {452.796f, 581.957f}, {453.089f, 579.614f}, {453.382f, 578.150f},
            {453.382f, 576.685f}, {453.089f, 575.221f}, {453.089f, 573.171f},
            {452.796f, 571.121f}, {452.210f, 569.070f}, {451.918f, 567.606f},
            {451.332f, 566.142f}, {450.746f, 564.677f}, {450.160f, 562.920f},
            {448.989f, 561.456f}, {448.403f, 559.991f}, {447.817f, 558.527f},
            {447.231f, 556.477f}, {447.231f, 554.719f}, {446.646f, 552.962f},
            {446.646f, 550.912f}, {446.353f, 548.569f}, {446.353f, 546.519f},
            {446.353f, 545.054f}, {446.646f, 543.883f}, {446.646f, 542.125f},
            {447.231f, 540.368f}, {447.524f, 539.196f}, {447.817f, 537.732f},
            {448.403f, 536.561f}, {448.989f, 535.389f}, {449.867f, 533.632f},
            {450.746f, 532.167f}, {451.625f, 531.289f}, {452.796f, 530.117f},
            {453.968f, 529.238f}, {454.846f, 528.360f}, {456.604f, 528.067f},
            {457.775f, 528.653f}, {457.775f, 530.117f}, {457.482f, 532.167f},
            {457.189f, 533.925f}, {456.897f, 535.975f}, {456.604f, 538.025f},
            {456.018f, 539.782f}, {456.018f, 541.247f}, {455.725f, 542.418f},
            {455.432f, 543.883f}, {455.432f, 546.226f}, {455.432f, 548.276f},
            {455.432f, 550.326f}, {456.018f, 552.376f}, {456.311f, 554.133f},
            {456.604f, 555.305f}, {456.897f, 556.769f}, {457.482f, 558.234f},
            {458.654f, 560.577f}, {459.532f, 562.041f}, {460.118f, 563.506f},
            {460.704f, 565.556f}, {461.290f, 566.435f}, {461.583f, 567.899f},
            {461.583f, 569.363f}, {461.876f, 571.706f}, {461.876f, 573.171f},
            {461.876f, 575.807f}, {461.876f, 577.564f}, {461.290f, 579.614f},
            {460.997f, 582.250f}, {460.411f, 583.715f}, {460.118f, 585.179f},
            {459.532f, 587.229f}, {458.361f, 588.986f}, {458.068f, 590.451f},
            {457.189f, 592.794f}, {456.604f, 594.551f}, {455.725f, 596.601f},
            {455.139f, 598.652f}, {454.553f, 600.409f}, {453.968f, 602.166f},
            {453.968f, 603.923f}, {453.968f, 605.388f}, {453.675f, 606.852f},
            {453.382f, 608.610f}, {453.675f, 610.660f}, {453.968f, 612.417f},
            {454.261f, 613.881f}, {454.553f, 615.932f}, {455.139f, 617.396f},
            {455.432f, 618.568f}, {456.018f, 620.618f}, {456.311f, 622.668f},
            {456.897f, 624.425f}, {457.189f, 626.183f}, {457.775f, 628.526f},
            {458.361f, 630.869f}, {458.947f, 633.212f}, {459.240f, 634.969f},
            {459.240f, 636.433f}, {452.503f, 637.019f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.102f, 0.42f, 0.149f, 1.00f, 4.0f);
        drawFilledTessCall(p, n, 0.176f, 0.71f, 0.259f, 1.00f);
    }
    {
        static const float p[][2] = {
            {465.097f, 637.019f}, {459.825f, 636.726f}, {458.947f, 634.969f},
            {458.947f, 633.505f}, {458.361f, 632.040f}, {458.361f, 629.990f},
            {458.068f, 629.111f}, {457.775f, 627.354f}, {457.482f, 625.890f},
            {456.897f, 623.839f}, {456.897f, 623.839f}, {456.897f, 622.082f},
            {456.604f, 620.325f}, {456.604f, 618.275f}, {456.604f, 616.517f},
            {456.604f, 615.053f}, {456.897f, 613.003f}, {456.897f, 610.953f},
            {457.189f, 609.195f}, {457.482f, 607.731f}, {457.775f, 605.974f},
            {458.068f, 604.216f}, {458.947f, 601.873f}, {459.825f, 599.823f},
            {460.997f, 598.066f}, {461.876f, 596.309f}, {463.340f, 593.965f},
            {465.683f, 591.622f}, {466.855f, 590.158f}, {467.733f, 588.986f},
            {469.198f, 587.229f}, {469.783f, 585.765f}, {470.662f, 584.007f},
            {471.248f, 582.836f}, {471.834f, 581.664f}, {472.712f, 579.614f},
            {473.005f, 578.443f}, {473.005f, 576.393f}, {473.591f, 574.342f},
            {473.884f, 572.585f}, {474.177f, 570.828f}, {474.177f, 569.656f},
            {474.177f, 567.899f}, {474.177f, 566.142f}, {473.884f, 564.677f},
            {473.591f, 562.920f}, {473.591f, 560.870f}, {473.005f, 559.698f},
            {473.005f, 558.527f}, {472.712f, 556.769f}, {472.419f, 554.719f},
            {472.419f, 554.719f}, {472.126f, 552.962f}, {472.126f, 551.205f},
            {472.126f, 549.447f}, {472.419f, 547.690f}, {473.005f, 546.519f},
            {473.884f, 545.054f}, {474.762f, 543.590f}, {475.348f, 542.125f},
            {476.813f, 540.368f}, {477.984f, 538.904f}, {479.741f, 538.611f},
            {480.913f, 538.904f}, {482.084f, 540.075f}, {482.084f, 541.832f},
            {481.792f, 543.883f}, {481.499f, 545.347f}, {480.913f, 547.397f},
            {481.206f, 551.205f}, {481.206f, 555.305f}, {481.792f, 559.991f},
            {481.499f, 563.506f}, {481.792f, 566.435f}, {481.792f, 569.363f},
            {481.206f, 571.999f}, {480.327f, 574.635f}, {480.034f, 577.271f},
            {479.156f, 579.614f}, {478.277f, 582.250f}, {477.691f, 583.129f},
            {477.398f, 584.886f}, {477.105f, 586.643f}, {476.813f, 588.986f},
            {476.227f, 590.451f}, {475.348f, 592.501f}, {474.177f, 594.258f},
            {472.712f, 595.723f}, {471.248f, 597.773f}, {469.783f, 599.823f},
            {467.733f, 602.459f}, {466.855f, 604.509f}, {465.683f, 606.559f},
            {464.511f, 609.488f}, {463.926f, 613.296f}, {463.926f, 616.810f},
            {463.926f, 619.739f}, {464.219f, 622.668f}, {464.511f, 625.597f},
            {464.804f, 628.526f}, {464.804f, 630.869f}, {464.804f, 632.333f},
            {465.097f, 634.090f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.102f, 0.42f, 0.149f, 1.00f, 4.0f);
        drawFilledTessCall(p, n, 0.176f, 0.71f, 0.259f, 1.00f);
    }
    {
        static const float p[][2] = {
            {466.855f, 636.726f}, {464.219f, 637.019f}, {464.511f, 634.969f},
            {464.804f, 632.919f}, {465.097f, 630.576f}, {465.097f, 628.233f},
            {465.683f, 625.890f}, {466.269f, 624.132f}, {466.269f, 624.132f},
            {467.440f, 622.961f}, {468.905f, 620.911f}, {471.248f, 616.517f},
            {472.712f, 614.760f}, {473.591f, 613.589f}, {475.055f, 612.124f},
            {475.934f, 610.367f}, {476.520f, 608.317f}, {477.105f, 605.974f},
            {477.691f, 603.338f}, {477.984f, 601.873f}, {478.570f, 599.823f},
            {478.570f, 597.480f}, {478.863f, 595.137f}, {479.156f, 593.673f},
            {479.741f, 591.915f}, {481.206f, 589.865f}, {482.377f, 587.815f},
            {483.256f, 586.058f}, {484.135f, 584.593f}, {485.013f, 583.422f},
            {485.013f, 583.422f}, {485.892f, 582.250f}, {486.478f, 581.079f},
            {487.063f, 580.200f}, {487.356f, 578.736f}, {488.235f, 577.271f},
            {488.528f, 576.100f}, {489.114f, 575.221f}, {489.406f, 573.464f},
            {489.406f, 571.999f}, {489.699f, 570.242f}, {489.699f, 568.485f},
            {489.992f, 566.435f}, {489.992f, 564.677f}, {489.992f, 563.213f},
            {490.285f, 562.041f}, {490.871f, 560.870f}, {491.750f, 559.112f},
            {492.042f, 557.941f}, {492.628f, 556.184f}, {494.093f, 554.719f},
            {494.971f, 553.548f}, {496.436f, 552.376f}, {498.486f, 551.498f},
            {499.072f, 552.669f}, {499.950f, 554.426f}, {499.657f, 556.477f},
            {499.072f, 559.112f}, {498.779f, 561.748f}, {498.486f, 564.384f},
            {498.193f, 567.020f}, {498.193f, 569.363f}, {497.900f, 572.292f},
            {497.314f, 574.635f}, {496.729f, 576.393f}, {495.850f, 578.150f},
            {494.678f, 580.493f}, {493.507f, 581.957f}, {492.921f, 583.422f},
            {491.750f, 584.886f}, {490.285f, 586.351f}, {489.406f, 588.108f},
            {488.528f, 590.451f}, {488.235f, 593.087f}, {487.942f, 595.723f},
            {487.649f, 598.066f}, {487.356f, 600.116f}, {487.063f, 602.459f},
            {487.063f, 605.095f}, {486.771f, 607.438f}, {485.599f, 609.781f},
            {485.013f, 611.538f}, {483.842f, 613.296f}, {482.084f, 614.467f},
            {480.620f, 615.932f}, {478.863f, 617.396f}, {477.398f, 618.275f},
            {475.934f, 619.446f}, {475.055f, 620.325f}, {473.884f, 621.789f},
            {472.712f, 623.839f}, {471.248f, 625.890f}, {469.783f, 627.940f},
            {468.905f, 630.283f}, {468.319f, 632.333f}, {467.440f, 634.676f},
            {466.855f, 636.433f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.102f, 0.42f, 0.149f, 1.00f, 4.0f);
        drawFilledTessCall(p, n, 0.176f, 0.71f, 0.259f, 1.00f);
    }
    {
        static const float p[][2] = {
            {474.762f, 637.312f}, {467.147f, 637.019f}, {467.440f, 634.969f},
            {468.319f, 634.090f}, {469.198f, 632.626f}, {470.076f, 631.454f},
            {470.955f, 630.869f}, {472.126f, 629.404f}, {473.591f, 628.233f},
            {474.762f, 627.647f}, {475.934f, 627.061f}, {476.813f, 626.768f},
            {479.448f, 625.597f}, {480.034f, 625.011f}, {482.084f, 624.425f},
            {482.084f, 624.425f}, {483.256f, 623.547f}, {484.135f, 622.961f},
            {485.306f, 622.375f}, {486.185f, 621.789f}, {487.649f, 620.911f},
            {488.528f, 620.325f}, {489.992f, 619.153f}, {490.871f, 617.982f},
            {492.042f, 616.810f}, {493.214f, 615.346f}, {493.800f, 614.467f},
            {494.093f, 613.296f}, {494.971f, 611.538f}, {495.264f, 609.195f},
            {496.143f, 607.731f}, {496.436f, 606.267f}, {496.729f, 605.095f},
            {497.314f, 603.338f}, {497.607f, 601.580f}, {498.193f, 599.823f},
            {498.779f, 598.066f}, {499.072f, 596.016f}, {499.950f, 593.673f},
            {500.243f, 592.208f}, {501.122f, 590.744f}, {502.000f, 589.279f},
            {502.879f, 587.815f}, {504.051f, 586.643f}, {505.222f, 585.765f},
            {506.101f, 585.179f}, {507.565f, 584.007f}, {508.737f, 583.422f},
            {509.908f, 584.300f}, {511.080f, 585.179f}, {512.251f, 586.351f},
            {512.837f, 587.522f}, {510.787f, 590.158f}, {509.908f, 590.744f},
            {506.979f, 592.501f}, {505.222f, 593.673f}, {505.222f, 593.673f},
            {504.343f, 595.137f}, {503.465f, 597.187f}, {504.051f, 601.288f},
            {504.051f, 601.288f}, {504.051f, 603.631f}, {504.343f, 605.388f},
            {503.758f, 609.488f}, {503.758f, 609.488f}, {503.465f, 611.831f},
            {502.879f, 614.467f}, {501.415f, 617.689f}, {499.950f, 619.739f},
            {498.486f, 622.668f}, {497.607f, 623.254f}, {496.729f, 624.132f},
            {495.850f, 625.011f}, {494.678f, 625.597f}, {493.214f, 626.475f},
            {492.042f, 626.768f}, {490.285f, 627.647f}, {488.235f, 629.111f},
            {485.892f, 629.990f}, {483.549f, 631.454f}, {481.792f, 632.333f},
            {480.620f, 633.212f}, {479.741f, 633.505f}, {477.984f, 634.676f},
            {475.934f, 636.141f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.102f, 0.42f, 0.149f, 1.00f, 4.0f);
        drawFilledTessCall(p, n, 0.176f, 0.71f, 0.259f, 1.00f);
    }
    {
        static const float p[][2] = {
            {476.520f, 636.726f}, {477.691f, 635.262f}, {479.156f, 634.090f},
            {480.034f, 633.797f}, {480.620f, 633.505f}, {482.084f, 632.626f},
            {483.256f, 631.747f}, {484.720f, 631.162f}, {486.478f, 630.283f},
            {488.235f, 629.404f}, {490.285f, 628.233f}, {494.678f, 625.890f},
            {495.850f, 624.718f}, {497.021f, 624.132f}, {498.193f, 623.254f},
            {499.657f, 621.789f}, {500.829f, 620.911f}, {502.293f, 619.446f},
            {503.465f, 617.689f}, {504.051f, 617.103f}, {505.222f, 616.225f},
            {506.101f, 615.346f}, {506.979f, 613.589f}, {508.444f, 611.831f},
            {509.615f, 610.074f}, {510.787f, 608.024f}, {511.666f, 606.559f},
            {512.251f, 604.802f}, {513.423f, 603.923f}, {514.594f, 603.631f},
            {515.766f, 603.631f}, {516.937f, 603.631f}, {517.816f, 604.802f},
            {518.402f, 605.681f}, {518.402f, 607.731f}, {517.230f, 609.488f},
            {514.594f, 613.589f}, {511.666f, 616.517f}, {511.666f, 616.517f},
            {510.201f, 618.860f}, {508.737f, 620.032f}, {506.687f, 621.789f},
            {504.636f, 623.547f}, {503.172f, 624.718f}, {501.708f, 626.183f},
            {499.657f, 627.647f}, {497.607f, 629.111f}, {496.143f, 630.576f},
            {494.678f, 631.747f}, {492.628f, 632.919f}, {490.578f, 633.797f},
            {488.235f, 634.676f}, {486.185f, 635.848f}, {484.427f, 637.019f},
            {483.256f, 637.605f}, {481.206f, 637.605f}, {476.813f, 637.605f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.102f, 0.42f, 0.149f, 1.00f, 4.0f);
        drawFilledTessCall(p, n, 0.176f, 0.71f, 0.259f, 1.00f);
    }
    {
        static const float p[][2] = {
            {483.256f, 637.019f}, {485.013f, 636.141f}, {485.013f, 636.141f},
            {489.406f, 634.090f}, {491.457f, 633.212f}, {493.507f, 632.040f},
            {495.850f, 630.869f}, {498.193f, 628.818f}, {500.243f, 627.061f},
            {501.415f, 625.890f}, {503.172f, 624.425f}, {504.929f, 623.547f},
            {506.394f, 622.082f}, {508.444f, 621.204f}, {510.201f, 620.032f},
            {512.251f, 619.739f}, {514.009f, 619.739f}, {515.180f, 620.032f},
            {516.059f, 620.032f}, {517.230f, 620.325f}, {518.695f, 620.618f},
            {519.280f, 620.911f}, {520.159f, 621.789f}, {521.331f, 622.375f},
            {521.916f, 622.961f}, {522.209f, 624.132f}, {522.209f, 625.011f},
            {521.331f, 625.890f}, {519.866f, 627.061f}, {518.695f, 627.061f},
            {516.937f, 627.061f}, {514.887f, 627.354f}, {514.009f, 627.061f},
            {512.544f, 626.475f}, {511.080f, 626.183f}, {509.322f, 625.890f},
            {508.151f, 625.890f}, {506.687f, 625.890f}, {504.929f, 626.475f},
            {503.758f, 627.647f}, {502.879f, 629.111f}, {502.293f, 630.283f},
            {501.415f, 631.747f}, {500.243f, 632.919f}, {499.364f, 634.383f},
            {497.607f, 635.848f}, {496.143f, 636.726f}, {495.264f, 637.898f},
            {482.963f, 637.605f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.102f, 0.42f, 0.149f, 1.00f, 4.0f);
        drawFilledTessCall(p, n, 0.176f, 0.71f, 0.259f, 1.00f);
    }
}

//! =================     CORAL      =================
void drawCoral(){
    {
        static const float p[][2] = {
            {205.408f, 662.079f}, {205.490f, 660.934f}, {205.490f, 659.790f},
            {205.571f, 658.564f}, {205.490f, 657.011f}, {205.490f, 655.703f},
            {205.081f, 654.232f}, {204.917f, 653.333f}, {204.836f, 652.597f},
            {204.754f, 651.698f}, {204.345f, 649.981f}, {204.100f, 648.347f},
            {204.100f, 647.202f}, {203.691f, 646.058f}, {203.610f, 644.750f},
            {203.364f, 643.769f}, {203.201f, 642.625f}, {202.547f, 641.154f},
            {202.384f, 640.009f}, {202.220f, 639.355f}, {201.975f, 638.620f},
            {201.403f, 637.639f}, {200.912f, 636.413f}, {200.667f, 635.759f},
            {200.258f, 634.860f}, {199.604f, 633.879f}, {198.951f, 632.653f},
            {198.460f, 631.999f}, {197.888f, 631.100f}, {197.316f, 630.528f},
            {196.662f, 629.710f}, {195.763f, 628.811f}, {194.945f, 627.749f},
            {194.046f, 626.931f}, {193.474f, 626.359f}, {192.902f, 625.787f},
            {192.248f, 625.460f}, {191.676f, 625.133f}, {190.940f, 624.643f},
            {190.041f, 623.989f}, {189.060f, 623.580f}, {188.079f, 623.090f},
            {187.344f, 622.926f}, {186.690f, 622.599f}, {185.709f, 622.190f},
            {184.483f, 621.537f}, {183.502f, 621.455f}, {182.603f, 621.291f},
            {181.786f, 621.210f}, {180.559f, 620.801f}, {179.252f, 620.719f},
            {178.598f, 620.637f}, {177.453f, 620.637f}, {175.982f, 620.310f},
            {174.429f, 619.983f}, {173.285f, 619.983f}, {172.059f, 619.983f},
            {170.833f, 620.147f}, {169.525f, 619.657f}, {168.626f, 619.493f},
            {167.400f, 619.330f}, {166.746f, 619.166f}, {165.847f, 618.921f},
            {164.375f, 618.594f}, {163.068f, 618.185f}, {162.168f, 617.940f},
            {161.188f, 617.286f}, {160.861f, 616.959f}, {159.961f, 616.224f},
            {159.226f, 615.570f}, {158.490f, 614.916f}, {157.918f, 614.017f},
            {157.591f, 613.608f}, {156.937f, 612.872f}, {156.610f, 612.300f},
            {155.956f, 611.483f}, {155.629f, 611.074f}, {155.221f, 610.257f},
            {154.812f, 609.848f}, {153.995f, 609.031f}, {153.177f, 607.723f},
            {152.196f, 607.151f}, {151.706f, 606.742f}, {151.052f, 605.843f},
            {149.908f, 605.025f}, {149.090f, 604.372f}, {148.191f, 603.963f},
            {147.374f, 603.718f}, {146.311f, 603.145f}, {144.922f, 602.737f},
            {143.941f, 602.410f}, {142.878f, 602.165f}, {141.979f, 601.838f},
            {141.080f, 601.838f}, {140.671f, 601.674f}, {140.263f, 601.511f},
            {139.936f, 601.347f}, {139.282f, 601.020f}, {138.464f, 600.612f},
            {137.483f, 599.958f}, {136.911f, 599.385f}, {136.421f, 598.732f},
            {135.849f, 597.996f}, {135.358f, 597.260f}, {134.950f, 596.525f},
            {134.132f, 595.871f}, {133.478f, 595.135f}, {132.988f, 594.399f},
            {132.252f, 593.255f}, {131.680f, 592.519f}, {130.863f, 591.539f},
            {130.454f, 590.721f}, {129.637f, 589.659f}, {128.901f, 588.841f},
            {128.411f, 587.779f}, {127.675f, 586.716f}, {126.694f, 585.572f},
            {125.550f, 584.427f}, {124.651f, 583.283f}, {123.425f, 581.894f},
            {122.607f, 580.994f}, {121.545f, 579.768f}, {120.482f, 578.869f},
            {119.746f, 578.134f}, {118.929f, 577.561f}, {118.030f, 577.153f},
            {116.722f, 576.172f}, {115.169f, 575.191f}, {114.106f, 574.619f},
            {113.044f, 574.128f}, {112.226f, 573.883f}, {111.409f, 573.556f},
            {110.183f, 572.984f}, {108.630f, 572.412f}, {106.750f, 571.921f},
            {105.115f, 571.595f}, {104.053f, 571.431f}, {102.990f, 571.431f},
            {101.600f, 571.513f}, {100.047f, 571.595f}, {99.148f, 571.513f},
            {97.841f, 571.513f}, {96.860f, 571.431f}, {95.307f, 571.268f},
            {93.999f, 571.104f}, {92.609f, 571.186f}, {90.893f, 570.287f},
            {89.503f, 569.469f}, {88.359f, 569.061f}, {87.378f, 568.407f},
            {86.479f, 567.671f}, {85.988f, 566.363f}, {85.825f, 565.219f},
            {86.234f, 563.993f}, {86.806f, 563.176f}, {87.460f, 562.358f},
            {88.277f, 561.786f}, {88.931f, 561.132f}, {89.994f, 560.723f},
            {90.893f, 560.560f}, {91.547f, 560.233f}, {92.609f, 560.151f},
            {93.508f, 559.824f}, {94.326f, 559.416f}, {95.307f, 558.925f},
            {96.614f, 558.353f}, {97.841f, 557.781f}, {98.821f, 557.372f},
            {100.620f, 556.391f}, {101.519f, 555.983f}, {102.418f, 555.819f},
            {103.235f, 555.656f}, {104.134f, 555.083f}, {105.360f, 554.756f},
            {106.750f, 554.511f}, {107.976f, 554.266f}, {109.284f, 554.021f},
            {110.755f, 553.857f}, {112.145f, 553.857f}, {113.126f, 554.184f},
            {114.106f, 554.756f}, {115.251f, 555.492f}, {116.395f, 556.391f},
            {117.131f, 557.209f}, {118.193f, 558.108f}, {119.092f, 559.416f},
            {119.665f, 560.151f}, {120.645f, 561.214f}, {121.381f, 562.031f},
            {122.362f, 562.767f}, {123.098f, 563.421f}, {124.405f, 564.483f},
            {125.468f, 564.892f}, {126.858f, 565.382f}, {128.002f, 565.464f},
            {129.391f, 564.974f}, {130.781f, 564.483f}, {131.190f, 563.748f},
            {132.089f, 562.767f}, {132.252f, 561.622f}, {132.497f, 560.887f},
            {132.497f, 559.743f}, {132.743f, 559.170f}, {132.743f, 558.026f},
            {132.906f, 557.209f}, {132.988f, 556.718f}, {133.070f, 555.819f},
            {133.233f, 555.083f}, {133.151f, 554.184f}, {133.151f, 553.449f},
            {133.151f, 552.877f}, {132.824f, 552.059f}, {132.497f, 550.915f},
            {131.844f, 550.097f}, {131.026f, 549.117f}, {130.127f, 548.054f},
            {129.228f, 546.991f}, {128.656f, 546.419f}, {127.511f, 545.684f},
            {126.612f, 545.030f}, {125.713f, 544.376f}, {124.487f, 543.477f},
            {123.506f, 542.986f}, {122.362f, 542.414f}, {121.299f, 542.005f},
            {119.828f, 541.515f}, {118.357f, 541.433f}, {117.049f, 541.024f},
            {115.741f, 540.861f}, {114.679f, 540.698f}, {112.635f, 541.106f},
            {110.837f, 541.270f}, {109.447f, 541.678f}, {107.894f, 541.842f},
            {106.995f, 542.005f}, {104.952f, 542.005f}, {103.399f, 542.169f},
            {102.009f, 541.924f}, {100.374f, 541.842f}, {98.494f, 541.597f},
            {97.432f, 541.597f}, {95.961f, 541.188f}, {95.143f, 541.106f},
            {93.917f, 541.270f}, {92.773f, 540.861f}, {91.465f, 540.943f},
            {90.239f, 540.861f}, {89.503f, 540.534f}, {88.114f, 540.207f},
            {86.888f, 540.125f}, {85.988f, 539.798f}, {85.253f, 539.390f},
            {84.027f, 539.063f}, {83.291f, 538.654f}, {82.392f, 538.245f},
            {81.411f, 537.755f}, {80.512f, 537.101f}, {79.695f, 536.774f},
            {78.959f, 536.202f}, {78.142f, 535.630f}, {77.733f, 535.221f},
            {77.161f, 534.731f}, {76.343f, 534.158f}, {75.281f, 533.178f},
            {74.627f, 532.605f}, {74.218f, 532.360f}, {73.728f, 532.033f},
            {73.401f, 531.543f}, {73.156f, 531.052f}, {72.829f, 530.644f},
            {72.420f, 529.990f}, {72.175f, 529.091f}, {72.011f, 528.437f},
            {71.766f, 527.946f}, {71.521f, 527.456f}, {71.276f, 526.720f},
            {71.194f, 525.985f}, {71.194f, 525.494f}, {71.112f, 524.922f},
            {71.030f, 524.268f}, {71.194f, 523.451f}, {71.521f, 522.797f},
            {71.930f, 522.225f}, {72.093f, 521.898f}, {72.420f, 521.571f},
            {73.237f, 521.162f}, {74.136f, 520.753f}, {75.036f, 520.672f},
            {76.425f, 520.508f}, {77.406f, 520.590f}, {78.142f, 520.753f},
            {79.286f, 520.753f}, {80.022f, 520.917f}, {80.757f, 521.162f},
            {81.575f, 521.326f}, {82.147f, 521.734f}, {83.209f, 522.061f},
            {83.945f, 522.470f}, {84.762f, 522.797f}, {85.743f, 523.206f},
            {86.888f, 523.451f}, {87.787f, 523.941f}, {89.013f, 524.432f},
            {90.157f, 525.004f}, {90.729f, 525.331f}, {91.465f, 525.658f},
            {92.119f, 525.903f}, {92.773f, 526.066f}, {93.672f, 526.312f},
            {94.326f, 526.639f}, {95.225f, 527.047f}, {96.369f, 527.292f},
            {97.187f, 527.538f}, {97.922f, 527.701f}, {98.740f, 527.865f},
            {99.802f, 528.110f}, {100.620f, 528.355f}, {101.764f, 528.355f},
            {102.663f, 528.519f}, {103.562f, 528.519f}, {104.625f, 528.600f},
            {105.606f, 528.600f}, {106.505f, 528.519f}, {107.404f, 528.519f},
            {108.466f, 528.437f}, {109.447f, 528.192f}, {110.265f, 527.865f},
            {110.837f, 527.619f}, {111.409f, 527.292f}, {112.226f, 526.884f},
            {112.635f, 526.393f}, {113.126f, 525.658f}, {113.207f, 525.167f},
            {113.371f, 524.759f}, {113.289f, 524.186f}, {113.371f, 523.614f},
            {113.289f, 522.960f}, {113.207f, 522.633f}, {113.126f, 521.980f},
            {113.044f, 521.489f}, {112.880f, 521.244f}, {112.308f, 520.508f},
            {111.981f, 520.263f}, {111.491f, 519.609f}, {110.919f, 518.955f},
            {110.428f, 518.547f}, {109.938f, 518.056f}, {109.366f, 517.566f},
            {108.957f, 517.239f}, {108.548f, 516.748f}, {107.976f, 516.258f},
            {107.404f, 515.767f}, {106.750f, 515.277f}, {106.178f, 514.868f},
            {105.769f, 514.623f}, {105.197f, 514.296f}, {104.707f, 513.887f},
            {103.971f, 513.315f}, {103.480f, 512.825f}, {102.745f, 512.253f},
            {102.254f, 511.926f}, {101.519f, 511.435f}, {100.865f, 511.108f},
            {100.293f, 510.700f}, {99.557f, 510.373f}, {98.985f, 509.882f},
            {98.331f, 509.555f}, {97.514f, 509.147f}, {96.614f, 508.820f},
            {95.961f, 508.493f}, {95.225f, 508.329f}, {94.244f, 508.084f},
            {93.508f, 507.839f}, {93.181f, 507.675f}, {92.773f, 507.430f},
            {92.201f, 507.348f}, {91.547f, 507.103f}, {90.648f, 506.694f},
            {90.157f, 506.694f}, {89.667f, 506.613f}, {88.849f, 506.531f},
            {88.114f, 506.286f}, {87.378f, 506.204f}, {86.479f, 506.204f},
            {85.498f, 506.204f}, {84.926f, 506.204f}, {84.435f, 506.204f},
            {83.618f, 506.122f}, {82.801f, 506.041f}, {81.983f, 506.122f},
            {81.166f, 506.122f}, {80.267f, 506.122f}, {79.368f, 506.204f},
            {78.632f, 506.286f}, {77.815f, 506.368f}, {77.079f, 506.286f},
            {76.098f, 506.204f}, {75.199f, 506.204f}, {74.790f, 506.204f},
            {73.973f, 506.204f}, {73.483f, 506.122f}, {72.665f, 505.795f},
            {72.175f, 505.387f}, {71.766f, 504.896f}, {71.521f, 504.488f},
            {71.030f, 503.752f}, {70.867f, 503.343f}, {70.540f, 502.935f},
            {70.295f, 502.362f}, {70.050f, 501.790f}, {69.723f, 501.300f},
            {69.477f, 500.482f}, {69.232f, 500.155f}, {68.905f, 499.502f},
            {68.660f, 498.684f}, {68.333f, 497.949f}, {68.006f, 497.458f},
            {67.843f, 497.131f}, {67.843f, 496.722f}, {67.679f, 496.150f},
            {67.434f, 495.415f}, {67.434f, 494.761f}, {67.434f, 494.025f},
            {67.516f, 493.371f}, {67.679f, 492.717f}, {68.006f, 491.982f},
            {68.497f, 491.409f}, {68.905f, 490.837f}, {69.886f, 490.347f},
            {70.622f, 490.183f}, {71.603f, 489.775f}, {72.175f, 489.693f},
            {73.483f, 489.448f}, {74.545f, 489.448f}, {75.363f, 489.284f},
            {75.935f, 489.203f}, {76.916f, 489.284f}, {77.569f, 489.366f},
            {78.223f, 489.611f}, {79.286f, 489.856f}, {80.022f, 489.938f},
            {80.594f, 489.938f}, {81.084f, 490.183f}, {81.820f, 490.429f},
            {82.310f, 490.674f}, {82.719f, 490.837f}, {83.291f, 491.083f},
            {83.863f, 491.409f}, {84.517f, 491.573f}, {85.580f, 492.309f},
            {86.642f, 492.962f}, {88.277f, 493.616f}, {89.340f, 494.107f},
            {90.484f, 494.924f}, {91.547f, 495.823f}, {92.528f, 496.641f},
            {93.427f, 497.458f}, {94.980f, 497.622f}, {95.961f, 498.112f},
            {97.023f, 498.357f}, {98.249f, 498.521f}, {99.394f, 498.439f},
            {100.538f, 498.275f}, {101.600f, 498.030f}, {103.072f, 497.703f},
            {104.053f, 497.458f}, {104.870f, 497.131f}, {105.524f, 496.968f},
            {106.096f, 496.804f}, {106.668f, 496.477f}, {106.832f, 495.905f},
            {107.077f, 495.578f}, {107.322f, 494.842f}, {107.486f, 493.780f},
            {107.649f, 493.289f}, {107.731f, 492.472f}, {107.731f, 491.900f},
            {107.649f, 490.837f}, {107.567f, 489.856f}, {107.322f, 489.366f},
            {106.913f, 488.549f}, {106.668f, 487.976f}, {106.341f, 487.077f},
            {106.014f, 486.669f}, {105.606f, 485.851f}, {105.197f, 485.361f},
            {104.707f, 484.789f}, {104.380f, 484.462f}, {103.807f, 483.890f},
            {103.153f, 483.563f}, {102.663f, 482.990f}, {102.091f, 482.582f},
            {101.519f, 482.255f}, {101.028f, 482.010f}, {100.456f, 481.764f},
            {99.884f, 481.601f}, {99.067f, 481.356f}, {97.841f, 480.865f},
            {96.941f, 480.620f}, {96.124f, 480.293f}, {95.225f, 480.130f},
            {94.571f, 479.803f}, {93.917f, 479.394f}, {93.100f, 479.149f},
            {92.119f, 478.822f}, {91.056f, 478.413f}, {90.157f, 478.168f},
            {88.768f, 477.514f}, {87.460f, 476.942f}, {86.315f, 476.370f},
            {85.171f, 475.879f}, {83.863f, 475.471f}, {82.801f, 474.980f},
            {81.575f, 474.490f}, {80.022f, 473.917f}, {78.959f, 472.855f},
            {77.896f, 472.038f}, {77.161f, 471.302f}, {76.343f, 470.239f},
            {75.935f, 469.585f}, {75.363f, 468.686f}, {75.117f, 467.869f},
            {74.627f, 466.234f}, {74.627f, 465.090f}, {74.627f, 464.027f},
            {74.790f, 462.556f}, {75.036f, 461.166f}, {75.526f, 460.512f},
            {75.853f, 459.695f}, {76.507f, 458.959f}, {76.752f, 458.306f},
            {77.161f, 457.733f}, {77.569f, 457.406f}, {78.060f, 456.916f},
            {78.550f, 456.752f}, {79.041f, 456.426f}, {79.940f, 456.262f},
            {80.676f, 456.180f}, {81.983f, 456.426f}, {82.555f, 456.998f},
            {83.209f, 457.815f}, {83.863f, 458.796f}, {85.089f, 460.431f},
            {85.825f, 461.657f}, {86.561f, 462.719f}, {87.215f, 463.700f},
            {88.441f, 465.090f}, {89.095f, 465.744f}, {89.912f, 466.234f},
            {90.484f, 466.398f}, {91.628f, 466.398f}, {92.936f, 466.316f},
            {94.244f, 466.152f}, {95.797f, 465.907f}, {97.350f, 465.744f},
            {98.821f, 465.744f}, {99.557f, 465.907f}, {100.865f, 466.398f},
            {102.663f, 467.051f}, {103.726f, 467.951f}, {104.870f, 468.605f},
            {106.178f, 469.585f}, {107.322f, 470.648f}, {108.140f, 471.384f},
            {109.039f, 472.201f}, {110.428f, 473.591f}, {111.573f, 474.735f},
            {112.553f, 475.879f}, {113.534f, 477.187f}, {114.679f, 478.413f},
            {115.659f, 479.884f}, {116.559f, 481.192f}, {117.866f, 482.745f},
            {118.602f, 483.971f}, {119.665f, 485.034f}, {121.299f, 486.669f},
            {122.689f, 487.976f}, {123.915f, 488.630f}, {125.305f, 489.611f},
            {126.367f, 490.183f}, {128.002f, 490.756f}, {130.127f, 491.083f},
            {131.271f, 491.655f}, {132.824f, 492.309f}, {133.724f, 492.636f},
            {134.377f, 492.717f}, {135.930f, 492.881f}, {137.238f, 492.962f},
            {138.301f, 492.799f}, {138.955f, 492.472f}, {139.690f, 491.900f},
            {140.590f, 490.837f}, {140.753f, 489.693f}, {140.835f, 488.794f},
            {140.753f, 487.731f}, {140.508f, 486.914f}, {140.099f, 485.770f},
            {139.527f, 484.789f}, {138.710f, 483.808f}, {137.974f, 483.481f},
            {136.830f, 483.072f}, {135.767f, 483.072f}, {134.868f, 482.909f},
            {133.805f, 482.827f}, {132.416f, 482.663f}, {131.271f, 482.255f},
            {129.718f, 481.764f}, {128.819f, 481.356f}, {128.329f, 480.784f},
            {127.348f, 480.048f}, {126.367f, 478.740f}, {125.795f, 477.759f},
            {124.896f, 476.533f}, {124.324f, 475.716f}, {123.915f, 474.817f},
            {123.179f, 473.427f}, {122.689f, 472.283f}, {122.280f, 470.893f},
            {121.790f, 469.994f}, {121.218f, 469.013f}, {120.564f, 467.951f},
            {119.992f, 466.806f}, {119.665f, 465.580f}, {119.256f, 464.926f},
            {118.929f, 464.354f}, {118.112f, 463.292f}, {117.539f, 462.474f},
            {116.885f, 461.820f}, {116.150f, 461.166f}, {115.414f, 460.594f},
            {114.924f, 460.349f}, {114.025f, 459.940f}, {113.289f, 459.859f},
            {113.289f, 459.859f}, {112.635f, 459.695f}, {111.899f, 459.532f},
            {111.491f, 459.532f}, {110.346f, 459.450f}, {109.120f, 459.532f},
            {107.404f, 459.450f}, {105.687f, 459.286f}, {103.726f, 458.796f},
            {102.336f, 458.224f}, {100.865f, 457.815f}, {99.720f, 457.406f},
            {98.658f, 456.998f}, {97.023f, 456.099f}, {95.715f, 455.363f},
            {94.571f, 454.546f}, {93.590f, 453.728f}, {92.609f, 452.829f},
            {91.792f, 451.685f}, {91.056f, 450.622f}, {90.729f, 449.723f},
            {90.566f, 448.579f}, {90.566f, 447.761f}, {90.484f, 446.535f},
            {90.648f, 445.718f}, {91.056f, 444.574f}, {91.547f, 443.347f},
            {92.119f, 442.530f}, {92.773f, 441.141f}, {93.590f, 439.996f},
            {94.408f, 439.587f}, {95.470f, 439.097f}, {96.369f, 438.688f},
            {97.432f, 438.443f}, {98.821f, 438.280f}, {100.211f, 438.607f},
            {101.600f, 438.852f}, {102.745f, 439.587f}, {104.053f, 440.160f},
            {105.442f, 440.732f}, {106.505f, 441.222f}, {107.567f, 441.467f},
            {108.466f, 441.631f}, {109.529f, 441.876f}, {110.101f, 441.549f},
            {110.837f, 440.895f}, {111.000f, 439.751f}, {111.246f, 438.525f},
            {111.246f, 437.299f}, {111.164f, 436.073f}, {111.327f, 434.683f},
            {111.246f, 432.967f}, {111.409f, 431.087f}, {111.000f, 429.370f},
            {111.082f, 427.409f}, {111.164f, 425.365f}, {111.327f, 424.302f},
            {111.491f, 422.504f}, {111.981f, 420.951f}, {112.553f, 419.970f},
            {113.943f, 418.499f}, {115.659f, 417.600f}, {116.722f, 416.946f},
            {118.439f, 416.292f}, {120.318f, 415.965f}, {122.117f, 415.475f},
            {123.425f, 415.720f}, {124.569f, 416.374f}, {126.531f, 417.273f},
            {128.165f, 418.744f}, {128.738f, 420.052f}, {129.637f, 421.360f},
            {131.190f, 423.567f}, {131.517f, 425.447f}, {132.416f, 427.981f},
            {132.661f, 430.515f}, {133.151f, 432.803f}, {132.824f, 438.116f},
            {132.906f, 440.241f}, {132.089f, 442.612f}, {131.844f, 444.410f},
            {131.435f, 445.800f}, {131.190f, 447.598f}, {131.353f, 450.213f},
            {131.108f, 452.666f}, {131.108f, 454.464f}, {131.271f, 455.690f},
            {131.435f, 456.998f}, {131.844f, 459.613f}, {132.579f, 461.412f},
            {133.151f, 462.719f}, {133.805f, 463.700f}, {134.459f, 464.681f},
            {135.277f, 465.825f}, {136.012f, 466.479f}, {136.748f, 466.643f},
            {137.810f, 466.316f}, {138.710f, 465.662f}, {138.873f, 464.027f},
            {138.955f, 462.801f}, {139.118f, 461.984f}, {139.363f, 460.839f},
            {139.854f, 460.512f}, {140.998f, 459.940f}, {142.306f, 459.613f},
            {143.205f, 459.695f}, {144.023f, 460.104f}, {144.758f, 460.676f},
            {145.494f, 461.330f}, {146.148f, 462.556f}, {146.720f, 463.782f},
            {147.129f, 465.172f}, {147.701f, 467.051f}, {148.109f, 468.686f},
            {148.518f, 469.831f}, {149.172f, 470.975f}, {149.908f, 472.038f},
            {150.643f, 473.100f}, {150.970f, 474.081f}, {151.297f, 474.980f},
            {151.624f, 476.043f}, {152.196f, 477.350f}, {152.605f, 478.250f},
            {152.932f, 479.149f}, {153.668f, 480.130f}, {154.648f, 481.519f},
            {155.466f, 482.745f}, {156.365f, 483.644f}, {157.101f, 484.543f},
            {158.000f, 485.688f}, {159.144f, 486.996f}, {160.370f, 488.303f},
            {161.514f, 489.121f}, {162.741f, 490.020f}, {163.967f, 490.837f},
            {165.274f, 491.655f}, {167.318f, 492.717f}, {169.280f, 493.535f},
            {171.568f, 494.025f}, {172.958f, 494.352f}, {174.593f, 494.597f},
            {176.391f, 494.924f}, {178.026f, 495.169f}, {179.415f, 495.251f},
            {180.314f, 494.842f}, {180.968f, 494.270f}, {181.050f, 493.044f},
            {181.132f, 491.736f}, {181.213f, 490.102f}, {181.213f, 488.303f},
            {180.723f, 486.996f}, {180.396f, 485.770f}, {180.233f, 484.298f},
            {179.660f, 482.745f}, {179.170f, 481.519f}, {179.006f, 480.375f},
            {178.189f, 478.495f}, {177.699f, 476.778f}, {177.208f, 475.225f},
            {176.636f, 473.754f}, {175.655f, 472.038f}, {175.165f, 470.811f},
            {174.593f, 469.749f}, {173.857f, 468.278f}, {173.285f, 467.297f},
            {172.549f, 466.398f}, {171.487f, 464.926f}, {170.751f, 463.945f},
            {169.852f, 463.046f}, {168.707f, 462.229f}, {167.563f, 461.330f},
            {166.746f, 460.512f}, {165.520f, 459.450f}, {164.212f, 458.387f},
            {162.741f, 457.652f}, {161.106f, 456.916f}, {159.308f, 456.262f},
            {157.591f, 455.363f}, {157.591f, 455.363f}, {155.793f, 454.627f},
            {154.567f, 454.219f}, {152.360f, 452.829f}, {151.542f, 452.502f},
            {150.562f, 451.766f}, {149.744f, 451.276f}, {148.682f, 450.295f},
            {147.864f, 449.886f}, {146.638f, 449.478f}, {145.412f, 449.233f},
            {144.349f, 448.987f}, {142.796f, 448.660f}, {140.998f, 448.170f},
            {139.690f, 448.007f}, {138.383f, 447.516f}, {137.157f, 446.944f},
            {136.176f, 446.372f}, {135.358f, 445.554f}, {135.849f, 444.655f},
            {136.339f, 444.247f}, {136.993f, 443.838f}, {137.565f, 443.347f},
            {138.219f, 443.102f}, {139.118f, 442.530f}, {139.854f, 441.958f},
            {140.426f, 441.059f}, {140.671f, 440.160f}, {140.671f, 439.587f},
            {140.508f, 437.871f}, {140.590f, 436.400f}, {140.426f, 435.255f},
            {140.426f, 434.029f}, {140.426f, 433.130f}, {140.426f, 431.741f},
            {140.753f, 430.351f}, {140.998f, 429.452f}, {141.243f, 429.125f},
            {141.570f, 428.308f}, {142.306f, 427.327f}, {143.123f, 426.673f},
            {143.696f, 426.264f}, {144.431f, 426.019f}, {145.494f, 425.610f},
            {146.393f, 425.283f}, {147.210f, 425.202f}, {148.273f, 425.038f},
            {149.662f, 425.038f}, {150.153f, 425.855f}, {150.480f, 426.673f},
            {151.052f, 428.062f}, {151.379f, 429.534f}, {151.542f, 430.433f},
            {151.461f, 431.986f}, {151.297f, 433.457f}, {151.297f, 434.928f},
            {151.297f, 436.645f}, {151.869f, 438.361f}, {152.769f, 439.751f},
            {154.076f, 440.650f}, {155.875f, 441.549f}, {157.346f, 442.040f},
            {158.817f, 442.530f}, {159.798f, 442.775f}, {161.188f, 443.102f},
            {162.250f, 443.102f}, {162.986f, 442.939f}, {162.250f, 440.650f},
            {161.841f, 439.424f}, {161.188f, 437.381f}, {160.615f, 435.337f},
            {159.961f, 433.621f}, {159.880f, 431.986f}, {159.471f, 430.678f},
            {159.144f, 429.125f}, {158.654f, 427.163f}, {158.081f, 425.038f},
            {157.673f, 423.567f}, {157.101f, 422.014f}, {156.692f, 420.297f},
            {156.283f, 418.826f}, {155.629f, 417.191f}, {155.139f, 415.720f},
            {154.567f, 413.840f}, {154.567f, 412.941f}, {153.995f, 411.715f},
            {153.913f, 411.143f}, {153.831f, 410.407f}, {153.341f, 409.590f},
            {153.341f, 408.690f}, {153.014f, 407.546f}, {152.932f, 406.157f},
            {152.687f, 404.931f}, {153.095f, 403.459f}, {153.504f, 402.151f},
            {153.749f, 401.089f}, {154.403f, 399.863f}, {155.057f, 398.637f},
            {155.548f, 397.738f}, {156.365f, 397.002f}, {157.264f, 396.103f},
            {158.408f, 395.531f}, {159.144f, 395.204f}, {160.043f, 394.877f},
            {160.861f, 394.877f}, {162.005f, 394.958f}, {163.068f, 395.122f},
            {164.457f, 395.776f}, {165.520f, 397.002f}, {166.092f, 398.228f},
            {166.337f, 399.372f}, {166.746f, 400.598f}, {166.582f, 401.906f},
            {166.419f, 403.132f}, {166.255f, 404.604f}, {166.010f, 406.565f},
            {165.928f, 408.609f}, {165.928f, 410.570f}, {165.928f, 413.023f},
            {166.419f, 414.576f}, {167.236f, 416.129f}, {168.217f, 417.191f},
            {168.789f, 417.273f}, {169.688f, 417.518f}, {170.833f, 417.518f},
            {171.813f, 416.783f}, {172.549f, 415.475f}, {172.876f, 413.922f},
            {173.367f, 412.205f}, {174.266f, 409.181f}, {174.266f, 409.181f},
            {174.920f, 407.383f}, {174.920f, 407.383f}, {175.492f, 405.666f},
            {175.900f, 404.767f}, {176.718f, 403.459f}, {177.208f, 402.887f},
            {177.699f, 402.397f}, {178.189f, 401.988f}, {179.415f, 400.925f},
            {180.069f, 400.762f}, {181.132f, 400.435f}, {181.786f, 400.190f},
            {183.012f, 399.945f}, {184.401f, 399.945f}, {185.300f, 400.517f},
            {185.872f, 400.844f}, {187.099f, 401.252f}, {188.325f, 402.070f},
            {189.224f, 402.969f}, {190.041f, 404.440f}, {190.613f, 406.157f},
            {190.613f, 408.364f}, {190.532f, 410.244f}, {189.796f, 412.123f},
            {189.387f, 413.922f}, {188.815f, 415.720f}, {188.815f, 417.682f},
            {188.161f, 419.398f}, {186.445f, 422.586f}, {185.791f, 425.610f},
            {185.300f, 427.409f}, {185.219f, 429.452f}, {184.238f, 431.495f},
            {184.074f, 433.457f}, {183.502f, 436.645f}, {183.175f, 438.525f},
            {183.093f, 440.814f}, {182.685f, 443.347f}, {182.603f, 446.045f},
            {182.848f, 448.579f}, {183.175f, 450.949f}, {183.420f, 454.137f},
            {183.992f, 455.608f}, {184.319f, 457.897f}, {185.055f, 459.695f},
            {185.954f, 461.820f}, {186.772f, 463.373f}, {187.589f, 464.599f},
            {188.815f, 466.071f}, {189.714f, 467.051f}, {190.695f, 467.787f},
            {191.676f, 468.441f}, {192.984f, 469.095f}, {194.210f, 469.422f},
            {195.354f, 469.258f}, {196.417f, 468.768f}, {197.479f, 467.787f},
            {198.869f, 466.071f}, {199.850f, 464.763f}, {200.422f, 463.046f},
            {201.239f, 461.248f}, {202.057f, 460.185f}, {202.302f, 458.387f},
            {202.710f, 456.998f}, {203.037f, 454.954f}, {203.528f, 452.993f},
            {204.182f, 451.440f}, {204.264f, 449.968f}, {204.100f, 448.660f},
            {204.018f, 447.107f}, {203.773f, 445.473f}, {203.610f, 444.328f},
            {203.364f, 443.429f}, {203.201f, 442.121f}, {202.874f, 441.059f},
            {202.710f, 439.914f}, {202.138f, 438.443f}, {201.321f, 436.808f},
            {200.422f, 435.255f}, {199.441f, 433.866f}, {198.460f, 432.967f},
            {197.724f, 432.149f}, {197.152f, 431.250f}, {196.744f, 429.697f},
            {195.926f, 428.553f}, {195.518f, 427.899f}, {195.354f, 426.428f},
            {194.700f, 424.793f}, {194.537f, 423.649f}, {194.618f, 422.668f},
            {195.191f, 421.769f}, {195.599f, 420.461f}, {196.253f, 418.908f},
            {197.643f, 418.336f}, {198.787f, 417.682f}, {200.340f, 417.682f},
            {201.484f, 418.254f}, {203.037f, 418.417f}, {203.855f, 417.927f},
            {204.182f, 416.374f}, {204.018f, 414.984f}, {204.100f, 413.431f},
            {204.018f, 411.960f}, {203.855f, 410.325f}, {203.855f, 408.772f},
            {203.691f, 407.383f}, {203.283f, 406.075f}, {202.792f, 404.604f},
            {202.384f, 402.969f}, {201.893f, 401.661f}, {201.321f, 400.517f},
            {200.667f, 398.882f}, {199.931f, 397.247f}, {199.114f, 396.103f},
            {198.378f, 394.713f}, {197.643f, 393.079f}, {197.071f, 392.261f},
            {196.662f, 391.689f}, {195.926f, 390.708f}, {195.191f, 389.891f},
            {194.128f, 388.992f}, {193.474f, 387.929f}, {192.657f, 386.948f},
            {191.594f, 385.967f}, {190.858f, 384.905f}, {190.532f, 383.842f},
            {190.123f, 382.534f}, {190.041f, 381.145f}, {189.959f, 379.837f},
            {190.041f, 378.938f}, {190.041f, 377.793f}, {189.878f, 376.486f},
            {190.041f, 375.423f}, {190.205f, 374.442f}, {190.450f, 372.971f},
            {190.777f, 372.235f}, {191.022f, 371.500f}, {191.921f, 370.274f},
            {192.902f, 369.211f}, {193.883f, 368.067f}, {195.109f, 367.494f},
            {195.763f, 367.249f}, {196.662f, 366.677f}, {197.970f, 366.514f},
            {199.196f, 366.514f}, {200.585f, 366.922f}, {201.648f, 367.740f},
            {202.057f, 368.475f}, {202.629f, 369.620f}, {202.874f, 370.764f},
            {203.364f, 372.317f}, {204.018f, 374.115f}, {204.672f, 375.914f},
            {205.163f, 377.793f}, {205.817f, 379.510f}, {206.552f, 381.553f},
            {208.105f, 384.169f}, {209.168f, 386.458f}, {210.230f, 387.520f},
            {211.538f, 388.174f}, {212.519f, 388.256f}, {214.317f, 388.338f},
            {215.625f, 388.092f}, {217.178f, 387.357f}, {217.996f, 386.785f},
            {219.058f, 385.150f}, {219.630f, 384.169f}, {220.121f, 383.106f},
            {220.366f, 382.371f}, {220.611f, 381.308f}, {220.856f, 380.164f},
            {220.938f, 378.938f}, {221.265f, 377.221f}, {221.347f, 375.995f},
            {221.347f, 374.279f}, {221.347f, 372.726f}, {221.347f, 371.500f},
            {221.510f, 369.783f}, {221.837f, 368.148f}, {222.246f, 366.595f},
            {222.900f, 365.860f}, {223.717f, 365.288f}, {224.126f, 364.879f},
            {225.025f, 364.715f}, {225.924f, 364.470f}, {227.068f, 364.143f},
            {227.968f, 364.143f}, {228.867f, 364.307f}, {229.684f, 364.552f},
            {230.665f, 364.797f}, {231.319f, 365.288f}, {232.300f, 366.514f},
            {233.117f, 367.249f}, {233.689f, 368.067f}, {234.670f, 369.048f},
            {235.161f, 370.355f}, {235.978f, 371.663f}, {236.550f, 373.216f},
            {236.877f, 374.769f}, {237.367f, 376.159f}, {237.694f, 378.120f},
            {237.613f, 379.347f}, {237.531f, 381.226f}, {237.286f, 382.616f},
            {236.305f, 384.496f}, {235.487f, 385.886f}, {234.425f, 387.112f},
            {232.872f, 388.665f}, {230.992f, 390.545f}, {229.357f, 391.934f},
            {228.131f, 392.997f}, {226.823f, 394.141f}, {226.006f, 395.285f},
            {224.862f, 396.920f}, {223.717f, 398.637f}, {222.818f, 400.271f},
            {221.919f, 401.824f}, {221.265f, 403.541f}, {220.529f, 405.421f},
            {220.039f, 407.383f}, {219.875f, 408.936f}, {219.875f, 410.652f},
            {220.039f, 412.205f}, {220.284f, 413.840f}, {221.020f, 414.984f},
            {221.755f, 416.047f}, {222.491f, 416.619f}, {222.982f, 416.129f},
            {224.044f, 414.984f}, {225.107f, 414.249f}, {225.597f, 413.758f},
            {226.415f, 412.777f}, {227.722f, 412.123f}, {228.458f, 411.470f},
            {229.194f, 411.143f}, {230.420f, 410.652f}, {231.237f, 410.979f},
            {232.136f, 411.306f}, {232.790f, 411.878f}, {233.281f, 412.450f},
            {234.098f, 413.186f}, {235.324f, 414.330f}, {235.978f, 415.638f},
            {236.387f, 416.210f}, {236.959f, 417.518f}, {236.959f, 418.908f},
            {236.795f, 420.052f}, {236.387f, 421.605f}, {235.978f, 422.831f},
            {235.406f, 424.302f}, {235.161f, 425.120f}, {234.834f, 426.019f},
            {235.896f, 425.447f}, {236.550f, 425.283f}, {237.204f, 424.793f},
            {238.021f, 424.302f}, {238.757f, 423.976f}, {239.329f, 423.730f},
            {240.392f, 423.076f}, {241.536f, 422.259f}, {242.680f, 421.360f},
            {243.580f, 420.379f}, {244.560f, 418.908f}, {245.623f, 417.845f},
            {246.849f, 416.701f}, {247.993f, 414.739f}, {248.566f, 414.003f},
            {249.138f, 413.023f}, {249.792f, 411.551f}, {250.282f, 410.652f},
            {250.772f, 409.017f}, {251.099f, 407.464f}, {251.018f, 405.748f},
            {251.181f, 404.522f}, {251.181f, 403.378f}, {251.263f, 401.743f},
            {251.263f, 400.680f}, {251.263f, 399.536f}, {251.181f, 398.637f},
            {251.263f, 397.084f}, {250.854f, 395.694f}, {250.854f, 394.468f},
            {250.854f, 393.732f}, {250.854f, 392.588f}, {250.936f, 390.953f},
            {251.263f, 389.646f}, {251.508f, 388.256f}, {251.672f, 387.193f},
            {251.999f, 385.477f}, {252.571f, 384.251f}, {253.143f, 383.188f},
            {253.715f, 382.126f}, {254.451f, 381.145f}, {255.186f, 380.327f},
            {256.167f, 379.347f}, {256.903f, 379.020f}, {257.884f, 378.938f},
            {258.783f, 378.611f}, {260.172f, 378.611f}, {261.889f, 378.774f},
            {263.442f, 379.428f}, {264.504f, 380.000f}, {265.240f, 380.736f},
            {266.221f, 382.126f}, {266.221f, 383.025f}, {266.303f, 384.333f},
            {266.139f, 385.804f}, {265.976f, 387.847f}, {265.485f, 389.564f},
            {264.831f, 391.362f}, {264.259f, 392.997f}, {263.851f, 394.223f},
            {263.360f, 395.939f}, {263.442f, 397.411f}, {263.524f, 398.718f},
            {264.096f, 400.271f}, {264.995f, 400.762f}, {265.894f, 401.171f},
            {266.711f, 401.171f}, {267.774f, 401.089f}, {269.245f, 400.844f},
            {270.308f, 400.598f}, {271.370f, 400.271f}, {272.106f, 400.026f},
            {273.250f, 399.618f}, {274.068f, 399.127f}, {274.885f, 398.800f},
            {275.784f, 398.555f}, {276.602f, 398.473f}, {277.337f, 398.718f},
            {277.991f, 398.964f}, {278.563f, 399.454f}, {278.809f, 400.108f},
            {279.217f, 401.007f}, {279.463f, 402.070f}, {279.544f, 403.051f},
            {279.544f, 404.113f}, {279.544f, 405.421f}, {279.544f, 406.157f},
            {279.381f, 406.974f}, {278.645f, 408.445f}, {277.746f, 409.835f},
            {277.174f, 410.897f}, {276.683f, 411.470f}, {275.866f, 412.369f},
            {275.294f, 412.614f}, {274.231f, 412.859f}, {271.779f, 413.268f},
            {271.779f, 413.268f}, {270.226f, 413.595f}, {268.837f, 414.249f},
            {267.447f, 415.230f}, {266.548f, 415.720f}, {264.995f, 417.110f},
            {263.932f, 417.927f}, {262.543f, 418.989f}, {261.562f, 420.134f},
            {260.663f, 421.278f}, {259.682f, 422.668f}, {257.312f, 427.000f},
            {257.312f, 427.000f}, {256.494f, 429.207f}, {256.004f, 431.332f},
            {255.759f, 433.948f}, {255.268f, 435.828f}, {255.023f, 437.626f},
            {254.696f, 439.261f}, {254.451f, 440.078f}, {254.124f, 441.304f},
            {253.388f, 443.102f}, {252.652f, 444.001f}, {250.691f, 445.963f},
            {248.811f, 447.353f}, {247.503f, 448.579f}, {246.195f, 449.233f},
            {244.397f, 451.276f}, {242.926f, 452.012f}, {240.964f, 453.074f},
            {237.040f, 456.180f}, {237.040f, 456.180f}, {235.242f, 458.306f},
            {234.261f, 459.613f}, {233.199f, 461.166f}, {232.054f, 463.864f},
            {231.237f, 466.234f}, {230.828f, 468.768f}, {230.828f, 471.138f},
            {230.665f, 473.100f}, {230.747f, 474.898f}, {230.501f, 476.860f},
            {230.992f, 478.250f}, {231.319f, 479.639f}, {232.300f, 482.909f},
            {232.790f, 484.298f}, {233.117f, 486.423f}, {233.689f, 488.385f},
            {234.588f, 490.102f}, {235.324f, 491.655f}, {235.569f, 492.554f},
            {235.814f, 493.289f}, {236.468f, 491.491f}, {236.632f, 490.347f},
            {236.877f, 488.630f}, {237.367f, 487.568f}, {237.449f, 486.669f},
            {237.776f, 484.625f}, {238.267f, 482.990f}, {238.430f, 482.173f},
            {238.594f, 481.356f}, {239.002f, 480.048f}, {239.166f, 478.331f},
            {239.329f, 476.778f}, {239.738f, 475.797f}, {240.228f, 474.490f},
            {240.964f, 472.937f}, {242.190f, 471.057f}, {242.926f, 470.566f},
            {243.661f, 470.158f}, {244.152f, 469.912f}, {244.560f, 469.831f},
            {245.214f, 469.831f}, {245.786f, 469.912f}, {246.604f, 470.158f},
            {247.258f, 470.403f}, {247.585f, 470.566f}, {248.157f, 470.975f},
            {249.138f, 472.446f}, {249.138f, 472.855f}, {249.301f, 473.754f},
            {249.301f, 474.408f}, {249.301f, 475.552f}, {249.301f, 475.552f},
            {249.219f, 476.533f}, {249.138f, 478.004f}, {248.811f, 479.230f},
            {248.647f, 480.784f}, {248.566f, 482.582f}, {248.484f, 483.971f},
            {248.566f, 485.034f}, {248.729f, 485.524f}, {249.138f, 486.342f},
            {249.301f, 486.587f}, {249.792f, 486.914f}, {250.446f, 487.077f},
            {251.181f, 487.159f}, {251.917f, 487.159f}, {252.816f, 487.241f},
            {254.042f, 487.323f}, {255.840f, 487.077f}, {257.312f, 486.669f},
            {257.965f, 486.423f}, {258.783f, 486.096f}, {259.682f, 485.770f},
            {260.418f, 485.361f}, {260.908f, 485.116f}, {261.235f, 484.870f},
            {261.971f, 484.462f}, {262.461f, 483.971f}, {263.197f, 483.317f},
            {263.932f, 482.827f}, {264.504f, 482.418f}, {265.485f, 481.846f},
            {266.303f, 480.947f}, {266.793f, 480.457f}, {267.447f, 479.803f},
            {267.937f, 479.312f}, {268.673f, 478.331f}, {269.082f, 477.596f},
            {269.327f, 476.697f}, {269.422f, 475.637f}, {268.716f, 474.696f},
            {268.009f, 473.872f}, {267.539f, 473.636f}, {266.479f, 472.930f},
            {265.420f, 472.106f}, {264.125f, 471.165f}, {263.066f, 470.341f},
            {261.771f, 469.517f}, {260.947f, 468.811f}, {260.123f, 468.104f},
            {259.417f, 467.280f}, {258.711f, 466.221f}, {258.358f, 465.044f},
            {257.887f, 464.220f}, {257.063f, 462.572f}, {257.063f, 461.513f},
            {257.063f, 460.336f}, {257.063f, 459.277f}, {257.063f, 458.335f},
            {257.181f, 457.393f}, {257.298f, 456.099f}, {257.652f, 454.804f},
            {257.887f, 453.980f}, {258.711f, 452.921f}, {259.417f, 452.214f},
            {260.006f, 451.626f}, {260.594f, 451.273f}, {260.594f, 451.273f},
            {261.300f, 450.684f}, {262.713f, 449.507f}, {263.772f, 448.683f},
            {264.714f, 447.977f}, {265.067f, 447.153f}, {265.420f, 446.329f},
            {266.126f, 445.152f}, {266.597f, 443.740f}, {266.597f, 442.445f},
            {266.597f, 440.444f}, {266.597f, 438.796f}, {266.597f, 437.384f},
            {266.479f, 435.618f}, {266.362f, 433.853f}, {266.597f, 432.323f},
            {266.597f, 431.499f}, {266.832f, 430.086f}, {267.185f, 429.145f},
            {268.009f, 428.438f}, {268.716f, 428.085f}, {269.657f, 427.968f},
            {270.834f, 427.968f}, {272.247f, 427.850f}, {273.659f, 427.968f},
            {274.483f, 428.203f}, {275.189f, 428.556f}, {276.131f, 429.145f},
            {276.602f, 429.498f}, {277.426f, 430.086f}, {278.132f, 430.910f},
            {278.603f, 431.734f}, {278.838f, 432.793f}, {278.838f, 433.853f},
            {278.956f, 436.325f}, {278.956f, 438.796f}, {278.956f, 440.797f},
            {279.191f, 442.445f}, {279.544f, 443.387f}, {280.133f, 444.328f},
            {280.721f, 445.270f}, {281.192f, 445.858f}, {281.663f, 446.329f},
            {282.487f, 447.153f}, {283.193f, 447.624f}, {284.135f, 448.213f},
            {286.842f, 448.095f}, {288.254f, 447.859f}, {289.314f, 447.389f},
            {289.784f, 447.035f}, {290.608f, 446.329f}, {290.961f, 445.741f},
            {291.079f, 443.622f}, {291.079f, 443.622f}, {290.608f, 442.445f},
            {290.373f, 441.739f}, {290.020f, 440.797f}, {289.902f, 440.091f},
            {289.902f, 438.914f}, {290.020f, 436.442f}, {290.020f, 436.442f},
            {290.726f, 433.264f}, {290.726f, 433.264f}, {291.079f, 431.852f},
            {291.432f, 429.969f}, {291.785f, 429.145f}, {292.256f, 428.085f},
            {293.198f, 426.673f}, {294.375f, 423.848f}, {295.434f, 422.200f},
            {296.611f, 420.317f}, {297.671f, 418.198f}, {298.259f, 416.197f},
            {299.083f, 414.903f}, {300.378f, 413.726f}, {301.202f, 412.549f},
            {302.026f, 411.489f}, {303.438f, 410.430f}, {304.615f, 409.724f},
            {305.557f, 409.488f}, {306.616f, 409.371f}, {307.911f, 409.371f},
            {309.441f, 409.841f}, {310.382f, 410.430f}, {311.913f, 411.254f},
            {312.854f, 412.784f}, {313.325f, 413.843f}, {313.914f, 415.373f},
            {313.914f, 417.257f}, {314.031f, 418.787f}, {313.914f, 420.552f},
            {313.560f, 421.612f}, {313.325f, 422.906f}, {312.737f, 424.319f},
            {311.795f, 426.084f}, {311.324f, 428.556f}, {310.736f, 430.204f},
            {310.500f, 431.970f}, {310.147f, 434.324f}, {309.794f, 436.089f},
            {309.794f, 437.266f}, {309.794f, 438.914f}, {309.912f, 440.209f},
            {310.147f, 441.739f}, {310.618f, 443.269f}, {311.206f, 444.328f},
            {312.266f, 445.623f}, {313.443f, 446.212f}, {314.855f, 446.918f},
            {316.738f, 446.682f}, {317.915f, 446.094f}, {319.328f, 445.152f},
            {320.976f, 443.857f}, {322.741f, 442.798f}, {324.154f, 441.621f},
            {325.566f, 440.797f}, {326.861f, 440.091f}, {328.391f, 439.032f},
            {330.392f, 438.090f}, {332.157f, 437.619f}, {334.158f, 437.619f},
            {334.982f, 438.090f}, {336.395f, 438.914f}, {337.219f, 440.444f},
            {337.925f, 441.974f}, {338.396f, 443.740f}, {338.513f, 445.741f},
            {338.160f, 447.271f}, {336.983f, 448.919f}, {335.453f, 449.743f},
            {333.688f, 451.273f}, {331.687f, 452.214f}, {330.274f, 453.391f},
            {329.450f, 453.862f}, {328.626f, 454.215f}, {327.096f, 455.157f},
            {326.037f, 456.099f}, {324.271f, 457.393f}, {322.624f, 459.041f},
            {321.564f, 459.865f}, {320.623f, 460.924f}, {319.328f, 462.455f},
            {317.915f, 464.220f}, {317.327f, 465.162f}, {316.385f, 467.398f},
            {315.444f, 469.164f}, {315.091f, 470.811f}, {314.855f, 473.166f},
            {315.208f, 474.578f}, {315.797f, 475.873f}, {316.856f, 476.579f},
            {318.151f, 477.050f}, {319.446f, 477.050f}, {320.858f, 476.814f},
            {322.153f, 476.579f}, {324.154f, 475.990f}, {326.390f, 475.166f},
            {327.920f, 474.460f}, {329.215f, 473.989f}, {330.627f, 472.695f},
            {332.040f, 471.988f}, {334.394f, 470.105f}, {334.394f, 470.105f},
            {336.042f, 468.811f}, {336.983f, 467.633f}, {338.396f, 466.692f},
            {339.337f, 465.986f}, {340.161f, 465.633f}, {341.338f, 465.397f},
            {342.751f, 465.515f}, {343.575f, 466.103f}, {344.281f, 467.045f},
            {344.634f, 467.751f}, {345.340f, 469.164f}, {345.576f, 470.811f},
            {345.693f, 472.812f}, {345.222f, 474.696f}, {344.869f, 476.461f},
            {343.222f, 478.462f}, {341.691f, 480.110f}, {340.161f, 481.169f},
            {338.278f, 483.053f}, {336.395f, 484.112f}, {334.747f, 485.642f},
            {333.099f, 486.348f}, {331.569f, 486.937f}, {329.333f, 488.114f},
            {327.214f, 488.585f}, {324.860f, 489.762f}, {322.859f, 490.703f},
            {321.682f, 491.292f}, {319.916f, 491.763f}, {318.386f, 492.940f},
            {316.150f, 493.175f}, {314.149f, 493.764f}, {312.501f, 493.999f},
            {308.970f, 494.823f}, {306.498f, 495.176f}, {305.086f, 495.411f},
            {303.556f, 496.000f}, {301.319f, 496.824f}, {299.083f, 498.472f},
            {297.317f, 500.473f}, {296.140f, 502.238f}, {294.493f, 505.534f},
            {294.375f, 507.535f}, {295.081f, 508.829f}, {296.023f, 509.889f},
            {297.082f, 510.360f}, {298.024f, 510.360f}, {299.201f, 510.007f},
            {300.025f, 509.300f}, {300.849f, 508.947f}, {302.261f, 508.241f},
            {303.673f, 507.888f}, {305.792f, 506.829f}, {306.969f, 506.829f},
            {308.028f, 507.064f}, {309.205f, 507.535f}, {309.794f, 508.476f},
            {310.147f, 509.653f}, {310.147f, 510.595f}, {310.147f, 511.772f},
            {309.323f, 513.184f}, {307.911f, 515.774f}, {307.087f, 517.893f},
            {307.087f, 519.187f}, {307.793f, 519.894f}, {309.205f, 520.247f},
            {310.971f, 520.011f}, {312.030f, 519.305f}, {312.972f, 518.599f},
            {314.149f, 517.304f}, {315.561f, 516.009f}, {316.856f, 514.362f},
            {318.269f, 512.596f}, {319.446f, 510.830f}, {320.740f, 509.183f},
            {322.035f, 507.535f}, {324.154f, 505.769f}, {325.095f, 505.181f},
            {325.802f, 504.945f}, {326.508f, 504.828f}, {327.685f, 504.828f},
            {328.273f, 504.945f}, {328.862f, 505.534f}, {329.686f, 506.240f},
            {330.039f, 507.182f}, {330.627f, 508.123f}, {330.980f, 509.300f},
            {331.098f, 510.477f}, {331.334f, 512.007f}, {331.216f, 513.655f},
            {331.216f, 515.303f}, {330.510f, 516.951f}, {329.686f, 518.128f},
            {328.156f, 519.894f}, {327.332f, 521.071f}, {326.155f, 522.483f},
            {324.742f, 524.013f}, {324.036f, 525.308f}, {322.624f, 526.838f},
            {320.623f, 529.898f}, {319.681f, 531.428f}, {318.504f, 533.429f},
            {317.445f, 534.842f}, {316.621f, 535.783f}, {315.208f, 537.196f},
            {314.502f, 538.726f}, {313.560f, 540.727f}, {312.737f, 544.023f},
            {312.148f, 547.201f}, {311.913f, 549.202f}, {311.795f, 551.085f},
            {311.677f, 553.439f}, {311.559f, 556.028f}, {311.913f, 558.618f},
            {312.501f, 559.677f}, {313.207f, 560.854f}, {314.149f, 561.913f},
            {314.855f, 562.384f}, {316.032f, 562.737f}, {317.092f, 561.443f},
            {317.915f, 560.266f}, {318.857f, 558.618f}, {319.446f, 556.735f},
            {320.387f, 554.380f}, {320.858f, 552.733f}, {321.211f, 551.085f},
            {321.564f, 547.083f}, {321.564f, 547.083f}, {321.917f, 545.553f},
            {322.859f, 543.787f}, {326.037f, 537.549f}, {326.743f, 535.313f},
            {327.567f, 533.782f}, {328.744f, 532.488f}, {329.686f, 531.664f},
            {330.392f, 531.428f}, {331.216f, 531.311f}, {332.157f, 531.075f},
            {332.981f, 531.193f}, {333.923f, 531.311f}, {334.865f, 531.664f},
            {335.689f, 532.135f}, {336.512f, 532.723f}, {337.101f, 533.312f},
            {337.572f, 533.900f}, {338.396f, 535.077f}, {338.867f, 536.137f},
            {339.102f, 536.960f}, {339.337f, 537.784f}, {339.455f, 539.079f},
            {339.455f, 540.256f}, {338.984f, 541.551f}, {338.160f, 543.316f},
            {337.336f, 544.493f}, {336.277f, 546.024f}, {335.453f, 547.671f},
            {334.747f, 548.848f}, {334.629f, 550.849f}, {334.747f, 552.733f},
            {334.982f, 554.851f}, {335.571f, 556.852f}, {336.630f, 557.205f},
            {337.925f, 556.970f}, {339.337f, 556.264f}, {340.279f, 555.204f},
            {341.103f, 554.380f}, {341.809f, 553.557f}, {342.633f, 552.262f},
            {343.575f, 551.203f}, {344.869f, 550.379f}, {346.164f, 549.908f},
            {347.812f, 549.790f}, {349.107f, 549.202f}, {350.048f, 548.378f},
            {350.637f, 547.671f}, {351.108f, 546.965f}, {351.814f, 545.670f},
            {352.285f, 544.258f}, {352.520f, 543.199f}, {352.520f, 542.492f},
            {352.285f, 541.315f}, {352.049f, 539.903f}, {351.814f, 538.726f},
            {350.755f, 535.313f}, {350.755f, 535.313f}, {350.519f, 533.194f},
            {350.166f, 531.664f}, {349.695f, 529.781f}, {349.460f, 528.015f},
            {348.754f, 526.838f}, {347.812f, 526.014f}, {347.106f, 525.426f},
            {346.164f, 524.719f}, {345.458f, 524.131f}, {344.752f, 523.425f},
            {343.575f, 522.365f}, {342.868f, 520.835f}, {342.398f, 519.776f},
            {341.927f, 519.070f}, {341.691f, 518.246f}, {341.338f, 517.186f},
            {341.338f, 515.774f}, {341.456f, 514.597f}, {342.045f, 513.420f},
            {342.633f, 511.890f}, {343.339f, 510.830f}, {343.810f, 510.007f},
            {344.752f, 509.065f}, {345.222f, 508.241f}, {345.929f, 507.064f},
            {346.517f, 505.769f}, {347.341f, 504.828f}, {348.047f, 503.651f},
            {348.636f, 502.591f}, {349.578f, 501.061f}, {350.401f, 499.649f},
            {350.990f, 498.472f}, {352.285f, 495.411f}, {354.050f, 490.232f},
            {355.345f, 486.584f}, {355.933f, 484.700f}, {356.404f, 483.170f},
            {356.757f, 481.993f}, {357.346f, 479.992f}, {357.699f, 478.815f},
            {357.934f, 477.403f}, {358.170f, 475.284f}, {358.170f, 472.812f},
            {358.641f, 471.165f}, {358.523f, 469.988f}, {358.405f, 469.046f},
            {358.405f, 467.633f}, {358.052f, 466.103f}, {357.699f, 464.455f},
            {356.875f, 462.808f}, {356.522f, 461.395f}, {356.169f, 459.277f},
            {355.816f, 457.746f}, {355.698f, 455.510f}, {355.345f, 453.274f},
            {355.580f, 450.920f}, {355.345f, 448.213f}, {355.580f, 447.035f},
            {355.816f, 445.270f}, {356.051f, 443.857f}, {356.169f, 442.563f},
            {356.757f, 440.680f}, {357.464f, 438.561f}, {357.817f, 436.913f},
            {358.405f, 434.677f}, {358.405f, 432.911f}, {358.641f, 427.144f},
            {359.465f, 424.437f}, {360.995f, 420.199f}, {361.583f, 418.669f},
            {362.525f, 417.374f}, {363.349f, 416.786f}, {364.408f, 416.080f},
            {365.350f, 415.844f}, {366.174f, 415.844f}, {367.115f, 415.844f},
            {368.057f, 416.315f}, {369.587f, 417.374f}, {370.411f, 418.787f},
            {370.764f, 419.964f}, {370.999f, 421.023f}, {371.235f, 422.906f},
            {371.235f, 423.730f}, {371.353f, 426.555f}, {371.117f, 429.851f},
            {370.764f, 432.323f}, {370.411f, 435.147f}, {370.176f, 437.737f},
            {369.705f, 441.033f}, {368.528f, 444.917f}, {368.410f, 448.683f},
            {368.410f, 451.273f}, {368.645f, 453.391f}, {369.352f, 455.628f},
            {369.822f, 456.569f}, {370.764f, 457.393f}, {371.353f, 457.629f},
            {372.059f, 457.511f}, {372.883f, 457.158f}, {373.353f, 456.099f},
            {373.942f, 455.157f}, {374.413f, 453.862f}, {374.884f, 452.214f},
            {375.354f, 451.037f}, {376.061f, 449.507f}, {376.885f, 447.859f},
            {377.591f, 446.918f}, {378.532f, 445.858f}, {379.827f, 445.270f},
            {380.886f, 445.270f}, {382.063f, 445.505f}, {383.005f, 445.976f},
            {384.182f, 447.035f}, {384.888f, 447.977f}, {385.595f, 449.507f},
            {385.712f, 450.802f}, {386.065f, 452.450f}, {386.065f, 453.509f},
            {386.065f, 454.922f}, {386.065f, 455.510f}, {386.065f, 456.452f},
            {385.359f, 458.335f}, {385.124f, 459.394f}, {384.300f, 460.924f},
            {383.711f, 462.337f}, {382.770f, 463.985f}, {381.475f, 465.633f},
            {379.945f, 468.575f}, {378.768f, 470.576f}, {376.296f, 473.519f},
            {376.296f, 473.519f}, {375.001f, 475.402f}, {374.177f, 476.343f},
            {370.882f, 480.934f}, {370.882f, 480.934f}, {369.587f, 483.523f},
            {369.469f, 484.936f}, {369.469f, 486.231f}, {368.998f, 487.761f},
            {368.881f, 489.055f}, {368.763f, 490.232f}, {368.292f, 491.763f},
            {367.468f, 493.175f}, {366.644f, 494.705f}, {366.291f, 495.411f},
            {365.350f, 497.295f}, {364.173f, 498.707f}, {362.996f, 500.590f},
            {361.936f, 501.885f}, {360.642f, 503.768f}, {359.347f, 505.298f},
            {358.523f, 506.593f}, {358.170f, 507.652f}, {357.346f, 509.536f},
            {357.464f, 510.477f}, {357.699f, 511.419f}, {358.170f, 512.596f},
            {358.994f, 513.655f}, {359.465f, 514.008f}, {360.995f, 514.950f},
            {361.819f, 515.421f}, {362.289f, 515.539f}, {364.173f, 515.539f},
            {366.527f, 515.656f}, {367.115f, 515.421f}, {368.645f, 515.185f},
            {369.940f, 514.832f}, {371.470f, 513.891f}, {372.176f, 513.302f},
            {373.353f, 512.243f}, {374.531f, 510.830f}, {374.884f, 509.536f},
            {375.001f, 507.535f}, {375.354f, 505.769f}, {375.472f, 504.239f},
            {375.590f, 502.709f}, {375.708f, 500.943f}, {375.825f, 499.060f},
            {376.061f, 497.059f}, {376.531f, 495.294f}, {377.002f, 493.764f},
            {377.708f, 491.763f}, {378.650f, 490.232f}, {378.886f, 489.644f},
            {379.709f, 488.114f}, {380.533f, 486.937f}, {381.475f, 485.877f},
            {382.887f, 484.583f}, {384.300f, 483.994f}, {385.830f, 483.876f},
            {386.889f, 484.230f}, {388.184f, 484.700f}, {389.126f, 485.053f},
            {390.420f, 486.348f}, {391.362f, 487.643f}, {392.186f, 489.291f},
            {392.774f, 490.586f}, {393.128f, 491.998f}, {393.481f, 493.528f},
            {393.481f, 495.882f}, {393.363f, 497.295f}, {393.128f, 499.413f},
            {392.421f, 501.414f}, {392.186f, 502.944f}, {391.244f, 504.828f},
            {390.656f, 506.122f}, {389.714f, 507.888f}, {388.537f, 510.007f},
            {387.360f, 512.478f}, {386.654f, 514.008f}, {385.595f, 515.656f},
            {385.006f, 517.539f}, {384.064f, 519.894f}, {382.887f, 522.012f},
            {382.063f, 524.719f}, {381.357f, 528.015f}, {381.122f, 530.016f},
            {380.651f, 532.959f}, {380.533f, 534.724f}, {380.651f, 535.901f},
            {381.357f, 537.196f}, {382.063f, 537.902f}, {383.358f, 537.784f},
            {384.182f, 537.431f}, {385.595f, 536.843f}, {387.007f, 535.901f},
            {388.419f, 534.606f}, {389.714f, 533.900f}, {392.539f, 530.369f},
            {392.539f, 530.369f}, {393.951f, 528.486f}, {396.070f, 525.426f},
            {397.600f, 523.189f}, {397.600f, 523.189f}, {398.542f, 522.012f},
            {400.190f, 521.188f}, {402.073f, 520.717f}, {403.485f, 520.364f},
            {405.486f, 520.364f}, {407.370f, 520.247f}, {407.840f, 520.247f},
            {408.782f, 520.247f}, {409.959f, 520.247f}, {410.901f, 520.129f},
            {412.078f, 519.423f}, {412.902f, 518.717f}, {413.843f, 517.893f},
            {414.314f, 517.069f}, {415.020f, 515.892f}, {415.727f, 514.597f},
            {416.550f, 513.184f}, {417.374f, 511.537f}, {418.669f, 509.889f},
            {420.082f, 509.065f}, {421.376f, 507.888f}, {421.965f, 507.652f},
            {423.024f, 507.535f}, {424.790f, 507.299f}, {425.378f, 507.417f},
            {426.320f, 507.652f}, {427.261f, 508.123f}, {428.438f, 509.065f},
            {429.380f, 510.360f}, {429.969f, 511.301f}, {430.557f, 512.596f},
            {431.263f, 514.244f}, {431.616f, 515.656f}, {431.616f, 518.010f},
            {431.852f, 519.658f}, {432.205f, 521.541f}, {433.264f, 524.249f},
            {433.617f, 526.132f}, {433.853f, 527.427f}, {433.853f, 528.957f},
            {433.029f, 530.722f}, {432.793f, 531.311f}, {432.440f, 531.899f},
            {431.499f, 533.076f}, {430.204f, 534.606f}, {429.380f, 535.195f},
            {427.968f, 536.019f}, {426.908f, 536.254f}, {425.496f, 536.372f},
            {422.906f, 536.607f}, {422.906f, 536.607f}, {421.376f, 536.725f},
            {417.492f, 537.078f}, {417.492f, 537.078f}, {416.197f, 537.314f},
            {414.903f, 537.431f}, {413.726f, 537.784f}, {412.431f, 538.020f},
            {411.254f, 538.726f}, {409.841f, 539.079f}, {409.135f, 539.432f},
            {407.134f, 540.021f}, {405.016f, 541.669f}, {404.309f, 542.022f},
            {402.779f, 542.257f}, {401.014f, 543.081f}, {399.484f, 544.964f},
            {398.071f, 546.259f}, {397.129f, 547.201f}, {396.423f, 548.025f},
            {394.658f, 548.848f}, {392.657f, 549.555f}, {391.127f, 549.790f},
            {389.596f, 550.379f}, {388.537f, 550.732f}, {387.360f, 551.085f},
            {386.536f, 551.320f}, {385.830f, 551.556f}, {384.771f, 552.026f},
            {383.711f, 553.321f}, {383.005f, 554.263f}, {382.299f, 554.969f},
            {381.475f, 556.381f}, {381.122f, 557.794f}, {380.769f, 559.559f},
            {381.004f, 561.090f}, {381.240f, 562.267f}, {381.828f, 563.444f},
            {382.652f, 564.385f}, {383.358f, 564.856f}, {384.300f, 565.562f},
            {385.477f, 565.798f}, {386.654f, 566.033f}, {387.831f, 566.151f},
            {389.479f, 566.268f}, {391.009f, 566.151f}, {392.892f, 565.680f},
            {394.775f, 565.327f}, {396.659f, 565.091f}, {397.247f, 564.856f},
            {398.542f, 564.738f}, {399.248f, 564.385f}, {400.307f, 563.914f},
            {402.191f, 560.619f}, {402.191f, 560.619f}, {403.132f, 558.971f},
            {403.956f, 557.323f}, {404.898f, 555.675f}, {405.486f, 554.380f},
            {406.310f, 553.203f}, {407.370f, 552.026f}, {408.782f, 550.732f},
            {410.312f, 549.319f}, {411.136f, 548.848f}, {412.313f, 548.731f},
            {413.843f, 548.731f}, {414.549f, 549.084f}, {415.373f, 549.319f},
            {416.080f, 550.025f}, {416.668f, 550.496f}, {417.257f, 551.203f},
            {418.081f, 552.380f}, {418.081f, 553.203f}, {418.081f, 554.027f},
            {418.316f, 555.204f}, {418.787f, 556.852f}, {418.081f, 557.558f},
            {417.963f, 558.853f}, {417.492f, 559.677f}, {416.668f, 560.501f},
            {415.727f, 561.443f}, {414.785f, 562.620f}, {414.196f, 563.326f},
            {412.195f, 565.091f}, {410.783f, 566.622f}, {409.135f, 567.563f},
            {408.311f, 568.623f}, {406.663f, 569.329f}, {404.662f, 569.800f},
            {402.308f, 570.859f}, {400.072f, 571.801f}, {398.424f, 572.624f},
            {397.012f, 573.448f}, {396.188f, 574.037f}, {395.246f, 574.155f},
            {394.069f, 574.743f}, {392.892f, 574.978f}, {391.715f, 575.096f},
            {390.420f, 575.096f}, {388.655f, 574.861f}, {387.596f, 574.743f},
            {386.301f, 574.508f}, {385.124f, 574.155f}, {384.182f, 573.801f},
            {383.123f, 573.331f}, {381.710f, 572.860f}, {380.063f, 572.154f},
            {378.532f, 571.683f}, {377.120f, 571.330f}, {375.943f, 570.623f},
            {375.119f, 570.506f}, {372.530f, 570.035f}, {371.470f, 570.035f},
            {370.058f, 570.035f}, {368.292f, 570.035f}, {364.997f, 570.153f},
            {362.878f, 570.270f}, {362.878f, 570.270f}, {361.465f, 570.623f},
            {359.818f, 570.977f}, {357.581f, 571.801f}, {355.580f, 572.507f},
            {354.403f, 573.331f}, {353.462f, 574.155f}, {352.755f, 574.508f},
            {351.578f, 575.214f}, {350.166f, 575.920f}, {348.989f, 576.744f},
            {347.459f, 577.333f}, {343.457f, 580.040f}, {343.457f, 580.040f},
            {341.691f, 580.864f}, {339.455f, 582.041f}, {336.983f, 583.100f},
            {334.276f, 584.866f}, {329.215f, 587.337f}, {329.215f, 587.337f},
            {327.332f, 587.808f}, {325.331f, 587.926f}, {323.683f, 588.161f},
            {322.388f, 588.750f}, {321.211f, 589.574f}, {320.858f, 590.633f},
            {320.740f, 591.810f}, {320.858f, 592.516f}, {321.447f, 593.458f},
            {321.917f, 594.399f}, {322.741f, 595.106f}, {323.330f, 595.576f},
            {324.036f, 595.812f}, {324.978f, 595.930f}, {326.272f, 596.047f},
            {327.567f, 596.283f}, {328.862f, 596.754f}, {330.157f, 597.107f},
            {330.980f, 597.577f}, {333.099f, 598.519f}, {333.099f, 598.519f},
            {334.276f, 598.872f}, {334.512f, 598.872f}, {334.865f, 598.990f},
            {336.042f, 598.754f}, {338.043f, 598.401f}, {338.749f, 598.166f},
            {339.455f, 597.342f}, {340.750f, 597.224f}, {341.456f, 596.518f},
            {342.633f, 595.930f}, {343.457f, 595.459f}, {344.045f, 595.106f},
            {344.752f, 594.517f}, {345.340f, 593.929f}, {346.164f, 593.340f},
            {347.223f, 592.634f}, {348.400f, 591.575f}, {349.460f, 589.809f},
            {350.401f, 588.985f}, {351.108f, 587.926f}, {352.049f, 586.513f},
            {353.109f, 584.866f}, {353.697f, 583.924f}, {354.639f, 583.100f},
            {355.463f, 582.276f}, {356.875f, 581.217f}, {358.170f, 580.275f},
            {358.994f, 579.922f}, {359.818f, 579.333f}, {360.995f, 579.098f},
            {362.643f, 578.745f}, {363.937f, 578.745f}, {365.232f, 578.980f},
            {366.056f, 579.333f}, {366.644f, 579.569f}, {367.821f, 580.393f},
            {368.645f, 581.452f}, {369.116f, 582.747f}, {369.116f, 584.042f},
            {368.998f, 585.101f}, {368.410f, 586.396f}, {367.704f, 587.808f},
            {366.998f, 590.044f}, {366.527f, 591.339f}, {365.585f, 594.282f},
            {365.820f, 595.459f}, {366.174f, 596.283f}, {366.762f, 597.107f},
            {367.468f, 597.931f}, {368.057f, 598.519f}, {369.234f, 599.578f},
            {370.529f, 600.285f}, {371.823f, 600.402f}, {373.471f, 600.638f},
            {375.590f, 600.755f}, {377.473f, 600.873f}, {379.356f, 600.755f},
            {382.534f, 600.638f}, {384.771f, 600.285f}, {387.596f, 600.167f},
            {389.126f, 599.814f}, {391.127f, 599.343f}, {392.892f, 598.519f},
            {394.305f, 597.931f}, {395.482f, 597.342f}, {396.776f, 596.871f},
            {398.071f, 596.047f}, {399.013f, 595.341f}, {399.484f, 595.106f},
            {400.190f, 594.753f}, {401.367f, 593.811f}, {403.485f, 592.634f},
            {405.957f, 592.045f}, {405.957f, 592.045f}, {407.252f, 591.810f},
            {408.664f, 591.575f}, {410.901f, 591.457f}, {410.901f, 591.457f},
            {411.960f, 591.575f}, {412.902f, 592.163f}, {413.490f, 592.987f},
            {413.843f, 593.811f}, {414.079f, 594.753f}, {414.314f, 595.812f},
            {414.314f, 596.754f}, {414.903f, 597.577f}, {415.844f, 597.342f},
            {417.139f, 596.989f}, {418.787f, 596.871f}, {420.082f, 597.224f},
            {421.259f, 597.695f}, {422.553f, 597.931f}, {423.142f, 598.754f},
            {423.848f, 600.167f}, {424.319f, 601.697f}, {424.437f, 602.639f},
            {424.437f, 605.699f}, {424.437f, 605.699f}, {424.201f, 606.523f},
            {423.848f, 607.347f}, {422.906f, 609.230f}, {422.436f, 609.819f},
            {421.612f, 610.760f}, {420.317f, 611.113f}, {419.375f, 611.466f},
            {418.434f, 611.819f}, {416.904f, 612.055f}, {414.432f, 612.879f},
            {413.137f, 612.879f}, {411.842f, 612.526f}, {410.430f, 611.937f},
            {409.135f, 611.584f}, {407.840f, 611.231f}, {406.663f, 610.878f},
            {405.016f, 610.760f}, {404.074f, 610.878f}, {403.015f, 610.760f},
            {401.131f, 610.760f}, {400.543f, 610.760f}, {399.719f, 610.760f},
            {398.424f, 610.760f}, {396.541f, 611.113f}, {394.775f, 611.231f},
            {393.245f, 611.584f}, {392.068f, 611.937f}, {390.656f, 612.290f},
            {389.950f, 612.408f}, {388.655f, 613.350f}, {387.007f, 613.938f},
            {384.888f, 614.527f}, {382.652f, 615.468f}, {380.769f, 616.410f},
            {379.709f, 616.881f}, {377.826f, 617.822f}, {375.708f, 618.646f},
            {374.060f, 619.235f}, {371.941f, 618.999f}, {370.058f, 618.529f},
            {368.881f, 618.293f}, {366.880f, 616.881f}, {365.350f, 616.174f},
            {363.113f, 615.233f}, {360.524f, 613.938f}, {358.994f, 613.350f},
            {357.817f, 612.879f}, {355.816f, 611.937f}, {353.109f, 611.466f},
            {353.109f, 611.466f}, {351.578f, 611.231f}, {348.636f, 611.231f},
            {348.636f, 611.231f}, {347.341f, 611.702f}, {344.281f, 612.761f},
            {344.281f, 612.761f}, {342.515f, 614.056f}, {341.221f, 615.586f},
            {340.397f, 616.881f}, {340.514f, 617.822f}, {341.574f, 618.058f},
            {342.515f, 618.175f}, {343.575f, 617.940f}, {344.399f, 617.822f},
            {344.752f, 617.822f}, {345.458f, 617.940f}, {346.753f, 617.940f},
            {348.636f, 618.058f}, {349.813f, 618.411f}, {350.401f, 618.764f},
            {351.108f, 619.235f}, {352.402f, 620.176f}, {353.226f, 621.236f},
            {353.815f, 621.942f}, {354.403f, 622.884f}, {354.874f, 623.943f},
            {355.227f, 624.767f}, {355.345f, 625.708f}, {355.463f, 626.650f},
            {355.345f, 627.003f}, {354.403f, 627.592f}, {353.226f, 628.651f},
            {352.402f, 628.769f}, {351.225f, 629.593f}, {350.401f, 629.946f},
            {349.460f, 631.123f}, {348.989f, 632.535f}, {348.989f, 633.830f},
            {348.989f, 635.242f}, {348.989f, 636.537f}, {349.695f, 637.596f},
            {352.402f, 640.539f}, {353.815f, 641.598f}, {354.756f, 642.422f},
            {356.051f, 643.717f}, {358.170f, 645.129f}, {359.347f, 645.483f},
            {360.995f, 645.247f}, {362.878f, 644.659f}, {365.820f, 643.599f},
            {368.292f, 643.482f}, {370.411f, 643.364f}, {372.412f, 643.482f},
            {374.060f, 644.070f}, {375.472f, 644.894f}, {376.178f, 645.365f},
            {377.708f, 646.306f}, {377.826f, 647.130f}, {377.826f, 649.484f},
            {377.473f, 651.485f}, {376.885f, 653.133f}, {375.943f, 655.252f},
            {374.766f, 657.488f}, {373.118f, 659.136f}, {370.882f, 660.195f},
            {368.645f, 661.255f}, {365.703f, 662.079f}, {362.289f, 662.079f},
            {359.700f, 662.196f}, {356.404f, 662.667f}, {352.991f, 662.549f},
            {350.284f, 662.549f}, {347.106f, 662.432f}, {344.399f, 661.372f},
            {341.927f, 659.725f}, {340.279f, 658.312f}, {337.807f, 656.664f},
            {336.395f, 654.781f}, {334.512f, 652.780f}, {332.746f, 651.485f},
            {331.334f, 650.426f}, {329.215f, 649.484f}, {326.979f, 648.660f},
            {324.507f, 647.601f}, {321.917f, 645.836f}, {319.916f, 644.894f},
            {318.622f, 643.952f}, {317.209f, 643.246f}, {316.385f, 642.775f},
            {314.737f, 641.716f}, {312.854f, 641.127f}, {310.736f, 640.421f},
            {308.852f, 640.421f}, {306.734f, 640.186f}, {304.733f, 640.186f},
            {302.379f, 640.186f}, {299.318f, 640.186f}, {297.671f, 640.421f},
            {296.140f, 640.892f}, {294.493f, 640.892f}, {293.080f, 641.481f},
            {291.197f, 642.187f}, {289.314f, 643.011f}, {287.548f, 643.482f},
            {286.489f, 644.541f}, {284.959f, 644.894f}, {283.664f, 645.247f},
            {282.840f, 645.365f}, {280.721f, 645.836f}, {278.956f, 645.247f},
            {277.779f, 644.776f}, {276.249f, 644.070f}, {274.836f, 643.246f},
            {274.130f, 643.011f}, {273.306f, 642.305f}, {271.541f, 642.069f},
            {269.893f, 641.481f}, {268.009f, 641.127f}, {265.420f, 640.892f},
            {263.301f, 641.245f}, {261.300f, 641.716f}, {259.064f, 642.540f},
            {257.181f, 644.070f}, {256.121f, 645.247f}, {254.709f, 646.306f},
            {254.474f, 646.660f}, {253.650f, 647.483f}, {251.884f, 649.602f},
            {250.236f, 651.485f}, {249.295f, 653.369f}, {248.000f, 654.781f},
            {246.234f, 656.547f}, {243.998f, 658.077f}, {241.762f, 659.371f},
            {240.702f, 659.607f}, {238.701f, 659.607f}, {236.936f, 659.489f},
            {235.523f, 659.489f}, {233.993f, 659.371f}, {232.581f, 659.371f},
            {229.991f, 659.607f}, {227.990f, 659.136f}, {226.578f, 658.312f},
            {224.930f, 657.370f}, {223.635f, 656.547f}, {221.988f, 655.487f},
            {220.811f, 655.487f}, {218.692f, 655.134f}, {217.279f, 655.723f},
            {215.749f, 656.193f}, {213.513f, 657.017f}, {211.630f, 658.312f},
            {210.453f, 659.254f}, {209.511f, 659.960f}, {208.805f, 660.431f},
            {207.275f, 661.490f}, {206.333f, 661.725f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.639, 0.275, 0.141, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.973f, 0.498f, 0.435f, 1.0f);
    }
    {
        static const float p[][2] = {
            {205.628f, 661.223f}, {205.628f, 659.189f}, {205.628f, 657.765f},
            {205.628f, 656.138f}, {205.424f, 654.918f}, {205.221f, 652.884f},
            {204.814f, 650.850f}, {204.814f, 649.019f}, {204.407f, 648.206f},
            {203.797f, 644.951f}, {203.797f, 644.951f}, {203.187f, 642.918f},
            {202.577f, 640.273f}, {202.170f, 638.646f}, {201.763f, 637.223f},
            {201.153f, 635.799f}, {200.746f, 634.375f}, {199.526f, 632.748f},
            {198.306f, 631.121f}, {197.085f, 629.901f}, {196.068f, 628.477f},
            {193.831f, 626.850f}, {192.001f, 625.426f}, {190.984f, 625.019f},
            {190.170f, 624.409f}, {187.323f, 622.985f}, {184.882f, 622.375f},
            {182.645f, 621.765f}, {181.424f, 621.155f}, {180.814f, 621.155f},
            {178.984f, 620.545f}, {176.136f, 619.934f}, {176.136f, 619.934f},
            {174.712f, 619.731f}, {172.068f, 618.917f}, {168.814f, 618.511f},
            {168.814f, 618.511f}, {167.390f, 618.104f}, {166.577f, 617.900f},
            {168.204f, 616.884f}, {168.814f, 616.477f}, {169.628f, 615.867f},
            {170.441f, 615.256f}, {171.051f, 613.426f}, {172.068f, 611.595f},
            {172.475f, 610.985f}, {172.475f, 609.765f}, {172.679f, 608.545f},
            {173.085f, 607.121f}, {173.289f, 605.087f}, {173.289f, 604.070f},
            {173.289f, 602.036f}, {172.882f, 598.985f}, {172.882f, 598.985f},
            {172.272f, 597.358f}, {171.051f, 595.731f}, {170.034f, 594.917f},
            {168.407f, 593.697f}, {165.153f, 592.477f}, {165.153f, 592.477f},
            {162.916f, 591.866f}, {160.679f, 591.053f}, {156.814f, 589.222f},
            {156.814f, 589.222f}, {154.170f, 587.392f}, {153.153f, 586.375f},
            {152.543f, 585.765f}, {151.933f, 585.358f}, {151.119f, 583.731f},
            {150.712f, 582.714f}, {150.712f, 581.290f}, {150.916f, 580.070f},
            {151.323f, 579.053f}, {151.729f, 578.239f}, {152.746f, 577.426f},
            {153.560f, 577.019f}, {154.780f, 576.612f}, {156.611f, 576.002f},
            {158.848f, 576.612f}, {159.865f, 577.426f}, {161.289f, 578.239f},
            {162.306f, 579.460f}, {163.933f, 580.477f}, {165.357f, 581.494f},
            {167.390f, 582.511f}, {169.424f, 583.324f}, {171.865f, 583.527f},
            {172.475f, 583.121f}, {173.289f, 582.307f}, {173.492f, 581.900f},
            {173.899f, 580.883f}, {173.899f, 580.070f}, {173.696f, 578.849f},
            {173.492f, 577.019f}, {173.492f, 577.019f}, {173.085f, 575.595f},
            {172.475f, 573.968f}, {171.458f, 572.544f}, {171.458f, 572.544f},
            {169.831f, 571.121f}, {168.407f, 570.104f}, {167.187f, 569.290f},
            {166.170f, 568.273f}, {165.763f, 568.070f}, {165.153f, 567.866f},
            {163.729f, 566.849f}, {162.916f, 566.646f}, {161.289f, 565.833f},
            {159.662f, 565.426f}, {158.441f, 564.816f}, {157.221f, 564.205f},
            {156.001f, 563.799f}, {154.170f, 562.578f}, {153.153f, 562.375f},
            {151.933f, 561.765f}, {151.119f, 561.561f}, {149.085f, 560.544f},
            {149.085f, 560.544f}, {147.865f, 559.527f}, {146.238f, 558.510f},
            {145.221f, 557.290f}, {144.407f, 556.477f}, {143.594f, 555.663f},
            {143.187f, 555.053f}, {142.780f, 553.832f}, {141.967f, 552.816f},
            {141.763f, 552.002f}, {141.763f, 550.782f}, {141.967f, 549.968f},
            {142.170f, 549.154f}, {143.187f, 548.544f}, {144.001f, 548.138f},
            {144.407f, 547.934f}, {145.221f, 547.731f}, {146.238f, 547.121f},
            {146.441f, 546.510f}, {146.238f, 544.680f}, {145.628f, 543.460f},
            {145.628f, 543.460f}, {144.407f, 542.036f}, {143.187f, 540.815f},
            {142.373f, 539.595f}, {141.560f, 538.985f}, {140.339f, 536.748f},
            {140.136f, 535.934f}, {139.729f, 534.104f}, {139.933f, 533.087f},
            {140.339f, 531.663f}, {140.339f, 530.849f}, {140.543f, 529.629f},
            {140.950f, 528.002f}, {141.763f, 527.188f}, {142.577f, 526.375f},
            {143.594f, 525.358f}, {144.407f, 525.154f}, {146.441f, 524.748f},
            {147.458f, 524.748f}, {149.695f, 525.154f}, {151.526f, 526.375f},
            {152.746f, 527.798f}, {153.560f, 529.832f}, {154.780f, 532.273f},
            {155.390f, 534.104f}, {157.018f, 537.154f}, {157.831f, 538.985f},
            {158.848f, 541.222f}, {159.865f, 542.849f}, {161.289f, 544.070f},
            {163.119f, 545.493f}, {163.933f, 545.493f}, {165.560f, 545.900f},
            {167.390f, 545.493f}, {168.204f, 545.493f}, {170.034f, 545.087f},
            {172.068f, 544.680f}, {173.696f, 544.680f}, {174.509f, 544.680f},
            {176.746f, 544.883f}, {177.967f, 545.493f}, {178.780f, 546.307f},
            {180.001f, 547.934f}, {181.018f, 549.358f}, {181.221f, 550.578f},
            {182.035f, 552.816f}, {182.645f, 554.036f}, {183.458f, 557.900f},
            {183.458f, 557.900f}, {183.865f, 560.748f}, {184.882f, 566.443f},
            {184.882f, 566.443f}, {186.102f, 569.494f}, {186.916f, 573.968f},
            {187.933f, 575.392f}, {188.746f, 576.816f}, {189.560f, 578.443f},
            {190.374f, 579.663f}, {191.391f, 581.290f}, {192.611f, 583.121f},
            {194.035f, 584.748f}, {195.255f, 585.155f}, {196.475f, 585.155f},
            {199.119f, 584.748f}, {199.119f, 584.748f}, {200.340f, 584.138f},
            {201.560f, 582.307f}, {202.780f, 579.256f}, {203.594f, 577.222f},
            {203.594f, 577.222f}, {204.204f, 574.985f}, {204.611f, 572.748f},
            {205.221f, 570.714f}, {205.424f, 569.087f}, {205.424f, 566.646f},
            {205.628f, 564.612f}, {205.831f, 561.968f}, {205.831f, 559.731f},
            {206.035f, 555.866f}, {206.035f, 555.866f}, {206.035f, 554.443f},
            {205.831f, 550.985f}, {205.831f, 550.985f}, {205.628f, 546.917f},
            {205.221f, 545.290f}, {204.814f, 544.273f}, {204.611f, 542.849f},
            {204.204f, 541.426f}, {203.797f, 540.002f}, {203.187f, 538.782f},
            {202.780f, 537.561f}, {201.560f, 535.527f}, {200.340f, 533.290f},
            {198.916f, 531.256f}, {196.882f, 527.798f}, {194.848f, 526.375f},
            {193.221f, 524.544f}, {191.594f, 523.324f}, {190.170f, 522.510f},
            {188.746f, 521.493f}, {187.323f, 520.273f}, {185.899f, 519.459f},
            {183.255f, 518.239f}, {182.035f, 517.019f}, {179.390f, 515.188f},
            {176.543f, 513.561f}, {174.916f, 512.544f}, {172.882f, 512.341f},
            {171.458f, 511.934f}, {168.611f, 511.934f}, {166.373f, 511.934f},
            {164.950f, 512.341f}, {163.119f, 512.544f}, {161.085f, 512.341f},
            {159.255f, 512.137f}, {157.628f, 511.934f}, {155.390f, 511.731f},
            {153.356f, 511.731f}, {151.526f, 511.527f}, {150.509f, 511.324f},
            {149.289f, 510.917f}, {147.458f, 510.510f}, {145.221f, 509.290f},
            {143.797f, 508.070f}, {142.373f, 507.053f}, {141.356f, 505.629f},
            {140.339f, 504.612f}, {138.916f, 502.985f}, {138.306f, 500.748f},
            {137.899f, 499.731f}, {137.085f, 498.103f}, {137.085f, 496.273f},
            {136.475f, 493.832f}, {136.475f, 493.832f}, {138.102f, 493.425f},
            {139.933f, 493.222f}, {141.153f, 491.798f}, {141.763f, 490.578f},
            {141.763f, 489.358f}, {141.763f, 487.934f}, {140.543f, 485.290f},
            {139.526f, 484.273f}, {138.306f, 483.053f}, {137.695f, 482.646f},
            {136.678f, 482.646f}, {135.458f, 482.442f}, {134.034f, 482.442f},
            {132.814f, 482.442f}, {131.187f, 482.239f}, {129.763f, 482.239f},
            {128.746f, 481.629f}, {127.526f, 480.408f}, {125.085f, 476.747f},
            {125.085f, 476.747f}, {124.475f, 474.917f}, {124.068f, 473.697f},
            {123.661f, 472.883f}, {122.441f, 470.442f}, {121.221f, 468.205f},
            {121.221f, 468.205f}, {120.407f, 466.375f}, {120.204f, 465.561f},
            {119.390f, 463.934f}, {118.373f, 462.510f}, {117.356f, 461.290f},
            {115.933f, 460.069f}, {111.255f, 458.442f}, {111.255f, 458.442f},
            {108.000f, 458.442f}, {105.966f, 458.239f}, {104.746f, 458.036f},
            {103.526f, 458.036f}, {102.102f, 457.425f}, {100.068f, 456.612f},
            {98.644f, 455.391f}, {97.831f, 454.781f}, {96.407f, 453.968f},
            {95.187f, 452.951f}, {93.966f, 452.137f}, {91.119f, 448.680f},
            {90.916f, 447.052f}, {91.119f, 445.425f}, {91.322f, 444.408f},
            {91.933f, 443.188f}, {92.746f, 441.764f}, {93.560f, 440.341f},
            {94.170f, 439.730f}, {95.797f, 439.324f}, {97.831f, 438.510f},
            {99.255f, 438.713f}, {101.085f, 439.730f}, {102.305f, 440.341f},
            {103.729f, 440.747f}, {104.950f, 441.968f}, {106.780f, 442.781f},
            {108.611f, 442.985f}, {109.831f, 442.781f}, {111.255f, 441.968f},
            {112.272f, 440.951f}, {112.475f, 438.917f}, {112.678f, 437.696f},
            {112.272f, 435.663f}, {112.272f, 434.239f}, {112.068f, 432.815f},
            {112.068f, 431.188f}, {112.068f, 428.951f}, {112.068f, 426.917f},
            {112.068f, 425.900f}, {112.272f, 424.476f}, {112.678f, 423.459f},
            {112.678f, 421.832f}, {113.695f, 420.612f}, {114.305f, 419.391f},
            {115.729f, 417.968f}, {117.967f, 417.154f}, {119.187f, 416.747f},
            {121.628f, 416.544f}, {123.661f, 416.747f}, {125.085f, 417.764f},
            {126.712f, 418.985f}, {129.560f, 423.256f}, {129.560f, 423.256f},
            {131.187f, 426.713f}, {132.000f, 430.985f}, {132.000f, 430.985f},
            {132.000f, 433.425f}, {132.000f, 435.052f}, {131.797f, 439.527f},
            {131.797f, 439.527f}, {131.594f, 441.154f}, {131.797f, 442.781f},
            {131.390f, 443.595f}, {131.187f, 444.408f}, {130.984f, 446.035f},
            {130.780f, 447.459f}, {130.170f, 449.696f}, {130.373f, 451.730f},
            {130.373f, 453.154f}, {130.373f, 454.781f}, {130.577f, 456.408f},
            {131.187f, 460.883f}, {131.187f, 460.883f}, {132.000f, 462.713f},
            {132.814f, 464.341f}, {134.034f, 465.968f}, {134.645f, 466.578f},
            {135.255f, 466.985f}, {137.085f, 467.391f}, {137.085f, 467.391f},
            {138.306f, 467.188f}, {138.712f, 466.375f}, {139.119f, 465.358f},
            {139.526f, 462.510f}, {139.729f, 461.290f}, {139.729f, 461.290f},
            {140.543f, 460.273f}, {142.170f, 460.273f}, {143.594f, 460.680f},
            {145.221f, 462.307f}, {145.831f, 463.120f}, {147.255f, 465.968f},
            {148.272f, 468.408f}, {148.475f, 469.832f}, {149.492f, 472.273f},
            {150.509f, 475.324f}, {151.526f, 477.358f}, {152.543f, 479.392f},
            {153.763f, 481.425f}, {155.390f, 484.070f}, {157.018f, 485.697f},
            {158.441f, 487.324f}, {159.662f, 488.341f}, {161.289f, 489.561f},
            {163.119f, 490.781f}, {164.136f, 492.409f}, {165.763f, 493.425f},
            {169.221f, 495.053f}, {171.051f, 495.663f}, {171.051f, 495.663f},
            {173.696f, 496.070f}, {176.543f, 496.273f}, {178.170f, 496.273f},
            {180.001f, 496.273f}, {180.611f, 496.070f}, {181.221f, 494.849f},
            {181.424f, 491.188f}, {181.424f, 491.188f}, {181.018f, 488.544f},
            {181.018f, 486.917f}, {180.204f, 485.290f}, {180.001f, 483.866f},
            {179.594f, 481.425f}, {178.170f, 478.781f}, {177.763f, 476.137f},
            {176.746f, 474.307f}, {175.933f, 472.273f}, {175.119f, 470.646f},
            {174.102f, 469.019f}, {172.882f, 466.578f}, {171.255f, 464.747f},
            {169.831f, 463.120f}, {168.611f, 461.697f}, {167.187f, 460.476f},
            {165.763f, 459.663f}, {163.729f, 458.442f}, {161.899f, 457.629f},
            {160.068f, 456.408f}, {158.645f, 455.595f}, {157.831f, 455.391f},
            {156.407f, 454.781f}, {153.967f, 453.764f}, {152.950f, 453.358f},
            {151.323f, 452.341f}, {149.289f, 451.120f}, {147.255f, 450.510f},
            {145.831f, 449.493f}, {144.001f, 448.680f}, {142.577f, 448.273f},
            {141.153f, 448.273f}, {140.136f, 448.273f}, {139.119f, 448.273f},
            {137.695f, 447.866f}, {136.068f, 447.256f}, {135.051f, 446.646f},
            {134.848f, 445.629f}, {134.848f, 444.815f}, {135.865f, 444.205f},
            {136.678f, 443.798f}, {137.289f, 443.595f}, {138.306f, 442.985f},
            {138.916f, 442.781f}, {139.729f, 442.374f}, {140.543f, 441.357f},
            {140.543f, 440.341f}, {140.339f, 438.917f}, {140.543f, 437.493f},
            {140.339f, 436.273f}, {140.339f, 434.646f}, {139.933f, 433.222f},
            {139.933f, 431.798f}, {140.136f, 430.781f}, {140.746f, 429.764f},
            {142.577f, 427.730f}, {142.577f, 427.730f}, {143.594f, 426.713f},
            {144.611f, 426.103f}, {145.831f, 425.696f}, {146.848f, 425.696f},
            {147.662f, 425.696f}, {148.678f, 425.696f}, {149.492f, 426.510f},
            {149.899f, 427.120f}, {150.102f, 427.730f}, {150.306f, 429.561f},
            {150.306f, 430.374f}, {150.509f, 432.408f}, {150.509f, 433.629f},
            {150.509f, 436.273f}, {150.509f, 440.137f}, {150.509f, 440.137f},
            {153.560f, 442.374f}, {156.001f, 443.798f}, {159.255f, 444.612f},
            {161.695f, 444.612f}, {165.153f, 444.408f}, {164.136f, 442.781f},
            {158.848f, 424.679f}, {153.356f, 407.595f}, {152.950f, 405.561f},
            {152.950f, 403.527f}, {153.967f, 400.069f}, {153.967f, 400.069f},
            {154.780f, 399.256f}, {155.594f, 398.239f}, {157.018f, 397.018f},
            {158.034f, 396.205f}, {159.865f, 395.595f}, {161.289f, 395.391f},
            {162.509f, 395.595f}, {163.526f, 396.205f}, {164.340f, 397.222f},
            {164.950f, 398.442f}, {165.357f, 399.866f}, {165.153f, 403.527f},
            {165.153f, 403.527f}, {164.340f, 405.357f}, {164.136f, 406.781f},
            {164.136f, 409.222f}, {164.340f, 411.052f}, {165.153f, 413.493f},
            {165.967f, 415.120f}, {166.984f, 416.747f}, {168.407f, 418.171f},
            {170.034f, 418.578f}, {171.865f, 418.578f}, {172.475f, 417.357f},
            {172.882f, 415.730f}, {174.509f, 411.662f}, {175.119f, 409.425f},
            {175.119f, 409.425f}, {176.746f, 404.544f}, {176.746f, 404.544f},
            {178.170f, 402.713f}, {178.984f, 401.696f}, {180.204f, 401.086f},
            {181.628f, 400.273f}, {183.051f, 400.273f}, {184.882f, 400.679f},
            {186.102f, 401.290f}, {187.933f, 402.510f}, {189.560f, 404.747f},
            {190.374f, 407.188f}, {190.780f, 409.629f}, {190.780f, 411.052f},
            {188.746f, 416.340f}, {188.746f, 416.340f}, {187.323f, 421.629f},
            {186.509f, 424.883f}, {185.492f, 428.137f}, {184.475f, 431.798f},
            {182.848f, 438.103f}, {182.645f, 441.154f}, {182.848f, 444.815f},
            {182.848f, 448.883f}, {183.458f, 453.561f}, {184.272f, 458.036f},
            {185.289f, 460.883f}, {186.713f, 463.730f}, {188.136f, 465.764f},
            {189.967f, 466.985f}, {190.780f, 467.798f}, {192.204f, 468.408f},
            {193.831f, 468.612f}, {195.052f, 468.815f}, {196.475f, 468.612f},
            {197.696f, 467.798f}, {198.916f, 466.781f}, {200.136f, 465.154f},
            {200.950f, 463.120f}, {201.560f, 461.086f}, {202.374f, 459.663f},
            {202.984f, 457.629f}, {203.391f, 455.188f}, {203.594f, 452.951f},
            {203.797f, 449.290f}, {203.797f, 449.290f}, {203.797f, 446.849f},
            {203.594f, 444.205f}, {203.391f, 442.374f}, {202.780f, 440.747f},
            {201.560f, 438.713f}, {200.543f, 437.086f}, {200.136f, 435.866f},
            {199.323f, 434.646f}, {196.475f, 431.798f}, {196.475f, 431.798f},
            {195.865f, 430.171f}, {195.255f, 428.544f}, {194.848f, 426.713f},
            {195.052f, 424.883f}, {195.255f, 423.866f}, {195.865f, 422.035f},
            {197.085f, 421.425f}, {199.730f, 420.205f}, {199.730f, 420.205f},
            {200.950f, 420.612f}, {202.577f, 421.832f}, {203.391f, 422.646f},
            {204.814f, 425.086f}, {206.238f, 427.120f}, {207.052f, 427.730f},
            {208.272f, 428.340f}, {209.492f, 428.340f}, {210.916f, 428.340f},
            {212.340f, 427.527f}, {214.374f, 426.103f}, {218.848f, 422.035f},
            {223.119f, 417.357f}, {225.764f, 414.510f}, {227.797f, 412.273f},
            {228.611f, 411.662f}, {229.831f, 411.052f}, {230.848f, 411.052f},
            {231.865f, 411.866f}, {233.086f, 412.679f}, {234.916f, 414.307f},
            {236.136f, 416.137f}, {236.543f, 417.561f}, {236.543f, 418.781f},
            {236.747f, 420.612f}, {236.543f, 421.425f}, {235.323f, 423.052f},
            {234.306f, 424.883f}, {232.882f, 425.696f}, {228.814f, 428.951f},
            {228.814f, 428.951f}, {227.187f, 431.188f}, {225.764f, 433.018f},
            {224.950f, 433.832f}, {223.526f, 436.680f}, {223.526f, 436.680f},
            {222.306f, 440.137f}, {221.289f, 442.985f}, {220.272f, 445.425f},
            {220.069f, 448.069f}, {219.052f, 451.527f}, {218.848f, 453.561f},
            {217.831f, 456.612f}, {216.204f, 460.273f}, {215.187f, 461.493f},
            {213.967f, 462.917f}, {212.340f, 465.561f}, {210.916f, 467.188f},
            {208.679f, 469.425f}, {206.238f, 472.680f}, {204.611f, 476.951f},
            {203.594f, 478.985f}, {202.780f, 480.612f}, {201.967f, 483.053f},
            {201.153f, 485.086f}, {200.950f, 487.934f}, {200.340f, 490.578f},
            {200.136f, 493.832f}, {200.340f, 497.086f}, {200.543f, 499.120f},
            {200.950f, 501.154f}, {201.763f, 503.188f}, {202.374f, 504.612f},
            {203.391f, 506.239f}, {204.814f, 508.476f}, {206.238f, 510.510f},
            {208.272f, 511.934f}, {210.102f, 512.951f}, {211.119f, 513.561f},
            {212.747f, 514.171f}, {214.577f, 514.375f}, {216.814f, 514.375f},
            {219.662f, 513.358f}, {221.492f, 512.544f}, {223.933f, 510.510f},
            {226.374f, 508.476f}, {228.204f, 505.832f}, {230.035f, 503.798f},
            {232.272f, 500.951f}, {236.136f, 494.442f}, {236.136f, 494.442f},
            {236.950f, 492.205f}, {237.560f, 490.171f}, {237.967f, 488.341f},
            {238.374f, 486.103f}, {238.374f, 486.103f}, {238.781f, 483.866f},
            {238.577f, 480.815f}, {239.391f, 477.764f}, {240.408f, 475.120f},
            {240.814f, 473.900f}, {241.628f, 472.680f}, {242.848f, 471.256f},
            {243.865f, 470.849f}, {245.086f, 470.646f}, {246.509f, 470.646f},
            {247.526f, 471.256f}, {248.340f, 472.069f}, {248.543f, 473.900f},
            {248.747f, 476.341f}, {248.543f, 480.408f}, {247.933f, 482.036f},
            {247.933f, 482.036f}, {247.933f, 484.070f}, {247.933f, 485.493f},
            {248.543f, 487.120f}, {249.764f, 488.341f}, {251.594f, 488.544f},
            {253.831f, 488.544f}, {256.679f, 488.137f}, {259.933f, 487.120f},
            {261.153f, 485.900f}, {262.374f, 485.697f}, {264.001f, 484.883f},
            {265.018f, 483.866f}, {266.645f, 481.629f}, {268.069f, 479.798f},
            {270.306f, 476.341f}, {270.306f, 476.341f}, {271.526f, 474.103f},
            {272.137f, 471.459f}, {272.543f, 469.425f}, {272.747f, 467.798f},
            {273.560f, 465.968f}, {273.967f, 464.951f}, {274.577f, 463.527f},
            {274.984f, 462.307f}, {276.001f, 460.680f}, {277.221f, 460.069f},
            {278.645f, 461.290f}, {278.848f, 462.713f}, {280.272f, 463.120f},
            {281.493f, 463.120f}, {282.509f, 463.324f}, {283.730f, 463.527f},
            {285.154f, 464.747f}, {285.764f, 466.578f}, {286.170f, 468.815f},
            {285.967f, 470.239f}, {285.357f, 471.663f}, {284.747f, 473.086f},
            {283.933f, 474.510f}, {283.323f, 475.324f}, {283.120f, 476.137f},
            {282.509f, 477.358f}, {281.493f, 478.985f}, {280.679f, 480.408f},
            {280.272f, 482.036f}, {280.882f, 483.459f}, {281.899f, 485.900f},
            {282.713f, 487.120f}, {282.916f, 488.544f}, {283.120f, 489.764f},
            {283.120f, 490.171f}, {283.120f, 490.985f}, {283.120f, 492.612f},
            {282.916f, 493.832f}, {281.696f, 495.053f}, {279.662f, 495.663f},
            {276.001f, 496.680f}, {276.001f, 496.680f}, {273.154f, 497.290f},
            {271.730f, 497.900f}, {269.086f, 498.714f}, {266.035f, 500.137f},
            {263.594f, 500.951f}, {257.899f, 503.392f}, {257.899f, 503.392f},
            {256.882f, 504.409f}, {254.645f, 505.426f}, {252.204f, 507.053f},
            {249.764f, 509.493f}, {248.340f, 510.714f}, {247.120f, 511.934f},
            {244.882f, 514.375f}, {243.458f, 516.612f}, {242.238f, 518.239f},
            {241.425f, 519.663f}, {240.408f, 521.697f}, {238.781f, 525.358f},
            {238.170f, 530.036f}, {239.187f, 533.087f}, {240.001f, 534.917f},
            {241.018f, 536.137f}, {242.238f, 537.561f}, {243.458f, 537.968f},
            {246.103f, 538.375f}, {248.543f, 537.968f}, {250.984f, 536.544f},
            {252.611f, 535.324f}, {253.221f, 533.900f}, {254.035f, 531.663f},
            {254.238f, 529.426f}, {254.238f, 527.392f}, {254.035f, 524.544f},
            {254.035f, 521.900f}, {254.848f, 519.459f}, {255.662f, 517.426f},
            {256.882f, 515.188f}, {258.306f, 513.154f}, {260.137f, 511.731f},
            {261.967f, 510.714f}, {264.001f, 510.917f}, {265.425f, 511.934f},
            {267.052f, 513.765f}, {267.865f, 519.053f}, {267.865f, 519.053f},
            {268.069f, 522.307f}, {268.069f, 523.934f}, {268.882f, 526.578f},
            {270.306f, 529.629f}, {271.730f, 531.053f}, {273.154f, 531.459f},
            {274.781f, 532.273f}, {277.831f, 532.476f}, {280.069f, 532.273f},
            {282.306f, 531.866f}, {284.340f, 531.256f}, {286.374f, 530.239f},
            {287.187f, 529.222f}, {288.204f, 526.782f}, {289.628f, 524.544f},
            {291.255f, 522.104f}, {292.476f, 519.459f}, {293.899f, 517.426f},
            {295.120f, 515.798f}, {295.933f, 514.781f}, {296.950f, 513.968f},
            {297.357f, 513.358f}, {298.374f, 512.544f}, {299.187f, 511.324f},
            {300.815f, 510.103f}, {302.238f, 509.087f}, {303.865f, 508.070f},
            {306.103f, 507.459f}, {307.527f, 507.459f}, {308.747f, 508.070f},
            {309.357f, 508.680f}, {309.967f, 510.917f}, {309.967f, 510.917f},
            {309.560f, 512.748f}, {309.154f, 513.968f}, {308.543f, 515.188f},
            {307.120f, 517.222f}, {306.510f, 518.036f}, {302.849f, 521.493f},
            {300.001f, 527.798f}, {300.001f, 530.646f}, {300.001f, 530.646f},
            {300.408f, 532.273f}, {301.425f, 534.714f}, {302.035f, 536.341f},
            {303.255f, 538.171f}, {303.865f, 539.799f}, {304.069f, 541.222f},
            {304.272f, 543.256f}, {304.272f, 544.273f}, {304.069f, 545.697f},
            {303.255f, 547.527f}, {301.628f, 550.375f}, {300.408f, 551.392f},
            {298.781f, 552.205f}, {296.950f, 553.222f}, {294.510f, 554.849f},
            {292.272f, 556.477f}, {289.425f, 558.307f}, {287.187f, 559.527f},
            {285.357f, 561.155f}, {282.916f, 562.985f}, {280.476f, 564.409f},
            {278.442f, 566.239f}, {276.815f, 568.070f}, {274.781f, 570.307f},
            {273.357f, 571.731f}, {271.933f, 573.765f}, {270.103f, 575.799f},
            {268.679f, 577.019f}, {267.459f, 578.646f}, {265.831f, 579.663f},
            {264.001f, 581.697f}, {262.170f, 583.324f}, {260.543f, 585.155f},
            {258.713f, 586.985f}, {256.679f, 588.612f}, {254.035f, 591.663f},
            {250.984f, 595.324f}, {249.357f, 597.561f}, {247.933f, 600.002f},
            {246.916f, 602.036f}, {245.899f, 603.256f}, {244.475f, 605.900f},
            {244.475f, 608.138f}, {243.865f, 610.782f}, {244.475f, 612.816f},
            {244.475f, 614.646f}, {245.289f, 617.087f}, {246.509f, 619.324f},
            {248.136f, 620.951f}, {249.764f, 621.968f}, {251.594f, 622.172f},
            {253.221f, 621.968f}, {254.645f, 620.951f}, {255.865f, 618.917f},
            {256.882f, 617.290f}, {258.103f, 614.646f}, {258.509f, 612.612f},
            {258.916f, 610.782f}, {258.916f, 609.561f}, {259.526f, 608.138f},
            {260.747f, 605.087f}, {260.747f, 605.087f}, {261.153f, 604.070f},
            {261.967f, 602.036f}, {262.577f, 601.019f}, {264.204f, 598.985f},
            {265.018f, 598.578f}, {266.442f, 598.578f}, {269.289f, 597.561f},
            {269.289f, 597.561f}, {271.933f, 597.358f}, {273.357f, 597.358f},
            {275.391f, 596.951f}, {276.408f, 596.341f}, {277.425f, 594.917f},
            {277.018f, 593.290f}, {275.798f, 591.460f}, {275.187f, 589.833f},
            {274.781f, 587.595f}, {274.984f, 585.561f}, {275.798f, 583.527f},
            {276.611f, 582.307f}, {277.425f, 581.290f}, {279.052f, 579.866f},
            {281.289f, 578.646f}, {283.120f, 578.443f}, {285.154f, 578.239f},
            {286.170f, 578.443f}, {288.001f, 579.663f}, {288.001f, 582.104f},
            {288.408f, 585.155f}, {288.611f, 587.595f}, {289.018f, 590.850f},
            {289.425f, 593.290f}, {289.832f, 594.714f}, {290.442f, 595.528f},
            {291.052f, 596.138f}, {292.272f, 596.544f}, {294.103f, 596.951f},
            {295.120f, 596.951f}, {296.950f, 596.544f}, {299.798f, 595.934f},
            {301.221f, 595.121f}, {301.221f, 595.121f}, {302.849f, 593.697f},
            {303.662f, 592.477f}, {304.272f, 591.460f}, {305.086f, 590.443f},
            {305.899f, 589.833f}, {306.306f, 589.426f}, {306.916f, 588.612f},
            {308.137f, 588.205f}, {309.764f, 587.799f}, {310.781f, 587.799f},
            {312.204f, 587.799f}, {314.238f, 588.205f}, {315.866f, 588.816f},
            {317.086f, 589.426f}, {317.899f, 590.443f}, {318.916f, 591.460f},
            {319.323f, 592.273f}, {319.933f, 593.290f}, {320.340f, 594.307f},
            {321.560f, 595.731f}, {322.577f, 596.748f}, {323.594f, 597.358f},
            {325.018f, 597.968f}, {326.442f, 598.172f}, {328.069f, 598.782f},
            {329.696f, 599.595f}, {331.933f, 600.409f}, {333.764f, 601.222f},
            {335.391f, 601.426f}, {337.425f, 601.833f}, {339.459f, 602.646f},
            {341.900f, 603.256f}, {344.137f, 604.070f}, {345.561f, 605.290f},
            {346.781f, 606.917f}, {347.594f, 608.138f}, {348.205f, 609.765f},
            {348.205f, 610.782f}, {347.798f, 611.189f}, {347.188f, 611.799f},
            {346.374f, 612.612f}, {344.137f, 613.629f}, {344.137f, 613.629f},
            {342.713f, 614.646f}, {340.883f, 616.273f}, {340.069f, 617.697f},
            {340.679f, 618.714f}, {342.103f, 619.121f}, {343.323f, 619.324f},
            {344.747f, 619.528f}, {345.561f, 619.528f}, {346.781f, 619.731f},
            {350.239f, 621.155f}, {350.239f, 621.155f}, {351.459f, 622.172f},
            {352.679f, 623.595f}, {353.289f, 624.409f}, {353.289f, 627.053f},
            {352.272f, 628.884f}, {350.442f, 629.901f}, {349.018f, 631.731f},
            {349.018f, 631.731f}, {347.798f, 633.358f}, {347.188f, 634.172f},
            {345.561f, 635.189f}, {344.137f, 636.206f}, {342.713f, 637.019f},
            {341.493f, 637.426f}, {340.272f, 637.833f}, {337.832f, 638.646f},
            {336.611f, 639.256f}, {335.594f, 641.087f}, {335.188f, 642.511f},
            {335.798f, 644.341f}, {336.611f, 645.562f}, {337.425f, 646.172f},
            {338.442f, 647.392f}, {339.662f, 648.206f}, {340.883f, 649.426f},
            {341.900f, 650.036f}, {342.916f, 651.460f}, {343.730f, 652.680f},
            {344.544f, 653.901f}, {344.747f, 654.918f}, {344.747f, 655.324f},
            {344.544f, 655.935f}, {344.340f, 656.748f}, {343.730f, 657.358f},
            {342.713f, 657.765f}, {341.493f, 658.172f}, {338.238f, 656.138f},
            {338.238f, 656.138f}, {337.018f, 655.121f}, {335.391f, 654.104f},
            {333.560f, 652.477f}, {331.933f, 651.257f}, {330.510f, 650.443f},
            {328.679f, 649.223f}, {327.459f, 648.612f}, {326.035f, 648.002f},
            {323.798f, 647.392f}, {322.984f, 647.189f}, {322.171f, 646.782f},
            {320.137f, 644.748f}, {320.137f, 644.748f}, {319.323f, 643.934f},
            {317.696f, 642.511f}, {316.679f, 641.901f}, {315.866f, 641.494f},
            {314.238f, 640.884f}, {313.018f, 640.477f}, {311.594f, 640.070f},
            {310.374f, 639.867f}, {308.950f, 639.867f}, {308.137f, 639.663f},
            {306.306f, 639.663f}, {305.289f, 639.663f}, {304.272f, 639.663f},
            {302.849f, 639.867f}, {302.035f, 639.867f}, {301.425f, 640.070f},
            {300.001f, 639.867f}, {298.984f, 640.070f}, {297.357f, 640.477f},
            {294.916f, 640.884f}, {294.916f, 640.884f}, {293.696f, 641.087f},
            {291.865f, 641.494f}, {289.628f, 641.901f}, {287.594f, 642.714f},
            {285.967f, 643.121f}, {283.730f, 643.324f}, {282.509f, 643.528f},
            {281.696f, 643.528f}, {280.069f, 643.731f}, {278.848f, 643.324f},
            {278.035f, 643.121f}, {276.611f, 642.511f}, {275.798f, 641.901f},
            {274.577f, 641.290f}, {273.154f, 640.884f}, {271.323f, 640.273f},
            {269.899f, 639.867f}, {268.476f, 639.867f}, {266.848f, 640.070f},
            {265.018f, 640.070f}, {263.798f, 640.680f}, {262.374f, 641.087f},
            {260.340f, 642.104f}, {258.509f, 643.528f}, {257.086f, 644.341f},
            {255.662f, 645.155f}, {254.645f, 645.765f}, {253.831f, 646.579f},
            {252.001f, 648.002f}, {250.984f, 649.426f}, {249.764f, 651.053f},
            {249.153f, 652.273f}, {248.136f, 653.697f}, {247.120f, 655.121f},
            {246.103f, 656.138f}, {245.289f, 656.951f}, {244.069f, 658.172f},
            {242.645f, 658.579f}, {241.221f, 658.985f}, {239.594f, 659.189f},
            {238.374f, 658.985f}, {237.153f, 659.189f}, {235.730f, 659.392f},
            {234.103f, 659.596f}, {232.882f, 659.596f}, {230.645f, 659.392f},
            {228.204f, 658.579f}, {228.204f, 658.579f}, {226.374f, 657.765f},
            {224.136f, 656.545f}, {222.509f, 655.935f}, {222.509f, 655.935f},
            {220.679f, 655.731f}, {219.052f, 655.731f}, {217.831f, 655.731f},
            {216.814f, 656.138f}, {215.797f, 656.341f}, {214.984f, 656.951f},
            {214.170f, 657.358f}, {213.357f, 658.172f}, {212.340f, 658.985f},
            {211.119f, 659.596f}, {209.696f, 660.409f}, {208.882f, 660.816f},
            {207.865f, 661.223f}, {207.052f, 661.629f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.529, 0.255, 0.443, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.871f, 0.482f, 0.729f, 1.0f);
    }
    {
        static const float p[][2] = {
            {104.763f, 650.339f}, {102.232f, 649.327f}, {101.220f, 648.821f},
            {98.690f, 647.809f}, {98.184f, 645.785f}, {98.184f, 643.760f},
            {98.690f, 642.242f}, {102.232f, 638.193f}, {102.232f, 638.193f},
            {103.245f, 637.181f}, {103.245f, 634.650f}, {103.245f, 632.626f},
            {102.738f, 631.108f}, {101.726f, 628.577f}, {101.220f, 624.022f},
            {101.220f, 624.022f}, {101.220f, 621.492f}, {101.220f, 618.961f},
            {101.726f, 616.937f}, {102.232f, 615.925f}, {103.751f, 614.406f},
            {105.269f, 613.394f}, {109.318f, 612.382f}, {109.318f, 612.382f},
            {110.330f, 612.382f}, {112.354f, 612.382f}, {115.391f, 612.888f},
            {117.921f, 613.394f}, {119.946f, 614.912f}, {121.464f, 615.419f},
            {126.525f, 616.431f}, {126.525f, 616.431f}, {131.080f, 616.937f},
            {134.623f, 616.937f}, {138.166f, 617.443f}, {138.166f, 617.443f},
            {144.745f, 617.949f}, {144.745f, 617.949f}, {145.757f, 620.480f},
            {147.781f, 621.998f}, {147.781f, 624.022f}, {146.263f, 629.589f},
            {145.251f, 631.614f}, {145.251f, 631.614f}, {144.239f, 634.650f},
            {143.733f, 635.663f}, {143.733f, 637.181f}, {144.239f, 638.193f},
            {146.769f, 640.217f}, {148.794f, 640.724f}, {150.818f, 641.230f},
            {155.373f, 641.736f}, {155.373f, 641.736f}, {157.397f, 641.736f},
            {162.964f, 641.230f}, {162.964f, 641.230f}, {169.038f, 640.724f},
            {173.086f, 640.217f}, {173.086f, 640.217f}, {175.617f, 639.711f},
            {178.147f, 639.711f}, {180.172f, 640.217f}, {183.715f, 643.254f},
            {183.715f, 643.254f}, {184.727f, 645.278f}, {186.751f, 647.303f},
            {188.269f, 652.870f}, {188.269f, 652.870f}, {188.269f, 655.907f},
            {188.269f, 658.437f}, {183.208f, 657.931f}, {183.208f, 657.931f},
            {181.184f, 657.425f}, {178.147f, 657.425f}, {176.629f, 657.931f},
            {172.074f, 658.943f}, {169.038f, 659.955f}, {169.038f, 659.955f},
            {166.507f, 660.462f}, {163.977f, 660.968f}, {162.458f, 661.980f},
            {160.434f, 661.980f}, {157.397f, 660.968f}, {155.879f, 660.462f},
            {152.842f, 659.449f}, {149.806f, 658.943f}, {146.769f, 658.437f},
            {144.745f, 658.437f}, {142.720f, 656.413f}, {141.202f, 653.882f},
            {138.166f, 649.833f}, {138.166f, 649.833f}, {135.635f, 648.315f},
            {133.105f, 646.797f}, {131.586f, 646.291f}, {129.056f, 644.772f},
            {126.525f, 644.266f}, {123.489f, 642.748f}, {122.476f, 642.748f},
            {117.921f, 643.254f}, {117.921f, 643.254f}, {114.885f, 644.266f},
            {112.354f, 645.278f}, {110.330f, 646.797f}, {108.812f, 647.809f},
            {106.281f, 648.821f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.51f, 0.31f, 0.243f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.722f, 0.404f, 0.298f, 1.0f);
    }
    {
        static const float p[][2] = {
            {100.208f, 625.541f}, {97.171f, 625.541f}, {93.629f, 625.541f},
            {91.098f, 624.528f}, {88.062f, 624.022f}, {85.025f, 623.516f},
            {83.507f, 622.504f}, {80.470f, 621.492f}, {77.433f, 620.480f},
            {73.891f, 619.467f}, {71.360f, 617.949f}, {68.324f, 616.431f},
            {65.287f, 615.925f}, {63.263f, 615.419f}, {60.226f, 614.912f},
            {56.683f, 614.912f}, {54.153f, 614.912f}, {51.116f, 614.912f},
            {48.080f, 615.419f}, {46.055f, 609.851f}, {44.537f, 608.333f},
            {42.006f, 607.827f}, {39.982f, 607.321f}, {37.958f, 607.321f},
            {35.933f, 608.333f}, {32.897f, 609.345f}, {30.366f, 610.358f},
            {27.836f, 610.864f}, {25.305f, 610.864f}, {22.775f, 610.864f},
            {19.738f, 609.851f}, {17.714f, 608.839f}, {15.689f, 607.321f},
            {14.171f, 604.284f}, {14.171f, 601.248f}, {14.677f, 599.223f},
            {17.207f, 598.211f}, {19.232f, 598.211f}, {21.762f, 599.729f},
            {22.775f, 600.742f}, {24.293f, 599.223f}, {24.799f, 598.211f},
            {25.305f, 596.187f}, {25.305f, 593.150f}, {25.305f, 591.126f},
            {24.799f, 588.595f}, {24.293f, 587.077f}, {23.281f, 583.534f},
            {22.268f, 581.004f}, {21.256f, 577.967f}, {19.232f, 574.424f},
            {17.714f, 571.894f}, {16.195f, 568.857f}, {15.183f, 565.315f},
            {13.665f, 563.290f}, {12.146f, 560.760f}, {10.122f, 555.699f},
            {10.122f, 555.699f}, {8.098f, 553.674f}, {5.567f, 552.156f},
            {4.049f, 551.144f}, {2.531f, 550.132f}, {0.506f, 549.119f},
            {0.000f, 513.186f}, {1.012f, 513.692f}, {2.531f, 515.211f},
            {4.555f, 516.223f}, {7.085f, 518.247f}, {8.604f, 520.272f},
            {10.628f, 522.802f}, {11.134f, 525.333f}, {12.653f, 527.863f},
            {13.665f, 531.406f}, {17.207f, 539.503f}, {17.207f, 539.503f},
            {19.232f, 542.540f}, {21.256f, 546.083f}, {23.281f, 549.625f},
            {23.787f, 551.650f}, {24.799f, 553.674f}, {26.317f, 555.699f},
            {27.329f, 553.168f}, {27.329f, 551.144f}, {27.836f, 548.107f},
            {27.836f, 545.577f}, {27.836f, 544.564f}, {27.836f, 541.528f},
            {27.329f, 538.997f}, {25.811f, 531.912f}, {24.799f, 529.888f},
            {23.787f, 527.863f}, {22.268f, 525.839f}, {21.256f, 524.320f},
            {20.244f, 522.802f}, {18.726f, 520.778f}, {16.701f, 519.765f},
            {15.689f, 518.753f}, {14.677f, 517.235f}, {14.677f, 514.704f},
            {14.677f, 512.680f}, {15.689f, 510.656f}, {18.726f, 509.643f},
            {21.256f, 510.150f}, {23.281f, 511.668f}, {25.811f, 513.186f},
            {28.342f, 515.717f}, {29.354f, 516.223f}, {30.366f, 516.223f},
            {32.897f, 513.186f}, {32.897f, 513.186f}, {33.403f, 510.656f},
            {32.897f, 508.631f}, {31.884f, 506.101f}, {30.366f, 503.570f},
            {29.354f, 502.052f}, {27.836f, 500.028f}, {25.305f, 497.497f},
            {23.281f, 495.473f}, {21.256f, 492.436f}, {19.738f, 490.918f},
            {17.714f, 489.399f}, {15.689f, 487.881f}, {14.677f, 486.869f},
            {13.159f, 485.857f}, {12.146f, 484.845f}, {10.122f, 483.832f},
            {8.098f, 482.820f}, {5.567f, 481.808f}, {3.543f, 481.302f},
            {0.506f, 480.796f}, {0.000f, 480.290f}, {0.506f, 454.478f},
            {2.024f, 453.466f}, {2.531f, 450.430f}, {3.543f, 447.899f},
            {4.049f, 446.887f}, {5.061f, 445.369f}, {6.073f, 444.356f},
            {7.592f, 443.850f}, {9.110f, 443.344f}, {11.134f, 443.344f},
            {12.146f, 443.344f}, {14.171f, 443.850f}, {17.714f, 444.863f},
            {18.726f, 446.381f}, {20.244f, 448.911f}, {21.762f, 451.442f},
            {22.268f, 454.478f}, {22.775f, 456.503f}, {23.281f, 460.046f},
            {23.281f, 462.576f}, {23.281f, 465.613f}, {23.281f, 468.649f},
            {23.787f, 470.674f}, {23.787f, 473.204f}, {24.293f, 474.723f},
            {24.799f, 477.253f}, {26.317f, 478.771f}, {27.836f, 480.290f},
            {28.848f, 480.290f}, {30.872f, 479.784f}, {32.390f, 478.771f},
            {34.921f, 477.759f}, {37.451f, 475.735f}, {38.970f, 474.216f},
            {39.982f, 472.192f}, {40.488f, 470.168f}, {41.500f, 467.637f},
            {43.019f, 465.107f}, {44.537f, 463.082f}, {45.549f, 460.552f},
            {47.067f, 458.021f}, {49.092f, 455.491f}, {51.622f, 453.972f},
            {53.141f, 453.466f}, {55.671f, 453.972f}, {57.695f, 454.478f},
            {59.720f, 455.491f}, {61.744f, 457.515f}, {63.263f, 459.539f},
            {64.275f, 461.564f}, {65.793f, 465.107f}, {65.793f, 467.637f},
            {64.781f, 471.180f}, {63.263f, 474.216f}, {61.744f, 477.253f},
            {60.732f, 478.771f}, {59.214f, 481.302f}, {57.695f, 483.832f},
            {56.177f, 485.351f}, {54.659f, 487.375f}, {53.647f, 489.906f},
            {53.141f, 492.436f}, {51.622f, 494.967f}, {50.610f, 498.003f},
            {48.586f, 502.052f}, {48.586f, 506.101f}, {49.092f, 510.656f},
            {49.092f, 514.704f}, {49.598f, 518.753f}, {51.622f, 522.296f},
            {54.153f, 525.839f}, {56.177f, 528.875f}, {58.708f, 531.406f},
            {60.732f, 534.442f}, {63.769f, 536.973f}, {65.793f, 540.010f},
            {68.324f, 543.552f}, {68.830f, 546.083f}, {69.336f, 548.613f},
            {68.830f, 550.132f}, {67.818f, 552.662f}, {66.805f, 555.699f},
            {65.793f, 557.217f}, {62.756f, 557.723f}, {59.720f, 558.229f},
            {52.634f, 557.723f}, {50.610f, 559.747f}, {47.573f, 561.772f},
            {46.561f, 563.796f}, {46.561f, 566.327f}, {48.586f, 568.351f},
            {50.610f, 569.363f}, {52.634f, 570.882f}, {55.671f, 571.388f},
            {59.720f, 571.894f}, {64.275f, 571.388f}, {66.805f, 571.388f},
            {68.830f, 571.388f}, {70.854f, 572.906f}, {71.866f, 575.437f},
            {72.879f, 579.485f}, {74.397f, 582.016f}, {75.915f, 583.534f},
            {76.927f, 585.052f}, {78.952f, 587.583f}, {80.976f, 589.101f},
            {81.988f, 589.607f}, {83.507f, 590.620f}, {85.025f, 591.126f},
            {86.037f, 591.632f}, {88.062f, 592.138f}, {90.592f, 591.632f},
            {92.110f, 592.138f}, {94.135f, 591.632f}, {95.147f, 590.620f},
            {97.171f, 588.595f}, {99.702f, 586.571f}, {101.726f, 583.534f},
            {103.751f, 582.522f}, {105.269f, 582.016f}, {108.306f, 581.510f},
            {111.342f, 581.004f}, {113.367f, 581.004f}, {115.897f, 581.510f},
            {116.909f, 581.510f}, {118.934f, 582.016f}, {121.464f, 583.028f},
            {124.501f, 585.052f}, {127.031f, 587.583f}, {130.574f, 592.644f},
            {132.598f, 594.668f}, {134.117f, 597.705f}, {136.647f, 599.729f},
            {140.696f, 602.260f}, {143.733f, 603.272f}, {147.781f, 605.803f},
            {149.806f, 607.321f}, {155.879f, 613.900f}, {155.879f, 613.900f},
            {157.903f, 615.925f}, {160.434f, 617.949f}, {162.964f, 618.961f},
            {164.989f, 619.467f}, {168.025f, 619.973f}, {171.568f, 619.973f},
            {176.123f, 620.986f}, {179.160f, 620.986f}, {183.208f, 622.504f},
            {187.763f, 624.022f}, {190.800f, 626.047f}, {193.330f, 628.071f},
            {194.849f, 630.095f}, {196.873f, 633.132f}, {198.392f, 637.181f},
            {200.416f, 640.724f}, {201.934f, 645.785f}, {202.440f, 647.809f},
            {202.946f, 649.833f}, {203.453f, 653.882f}, {203.453f, 656.413f},
            {204.465f, 659.449f}, {203.453f, 661.980f}, {200.416f, 661.474f},
            {197.379f, 660.462f}, {194.343f, 660.462f}, {191.812f, 659.955f},
            {189.282f, 658.943f}, {189.282f, 656.919f}, {189.282f, 653.376f},
            {188.776f, 650.846f}, {188.776f, 650.846f}, {187.257f, 648.315f},
            {185.739f, 644.772f}, {184.221f, 643.254f}, {182.702f, 641.736f},
            {180.678f, 639.205f}, {178.654f, 638.699f}, {175.617f, 639.205f},
            {172.580f, 639.711f}, {169.038f, 639.711f}, {166.001f, 639.711f},
            {162.964f, 639.711f}, {159.422f, 640.724f}, {156.385f, 640.724f},
            {153.349f, 640.724f}, {149.806f, 640.217f}, {146.769f, 639.205f},
            {145.251f, 637.687f}, {144.745f, 635.156f}, {146.263f, 632.626f},
            {147.275f, 630.095f}, {148.288f, 627.059f}, {148.794f, 625.034f},
            {148.794f, 623.516f}, {147.781f, 620.986f}, {146.263f, 619.467f},
            {143.733f, 617.443f}, {141.202f, 616.937f}, {138.166f, 616.431f},
            {136.141f, 616.431f}, {132.092f, 616.431f}, {129.562f, 615.925f},
            {127.537f, 615.925f}, {123.489f, 614.912f}, {121.464f, 614.406f},
            {117.921f, 612.888f}, {114.885f, 612.888f}, {113.367f, 612.382f},
            {109.824f, 612.888f}, {104.257f, 612.888f}, {101.726f, 616.431f},
            {101.220f, 618.961f}, {101.220f, 622.504f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.51f, 0.345f, 0.29f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.749f, 0.498f, 0.412f, 1.0f);
    }
    {
        static const float p[][2] = {
            {99.196f, 624.528f}, {97.171f, 624.528f}, {94.135f, 624.528f},
            {92.616f, 624.022f}, {90.592f, 623.516f}, {88.568f, 623.010f},
            {86.037f, 622.504f}, {84.013f, 621.492f}, {80.976f, 620.480f},
            {79.458f, 619.467f}, {75.915f, 617.949f}, {75.915f, 617.949f},
            {72.879f, 616.431f}, {70.854f, 616.431f}, {67.818f, 615.419f},
            {65.287f, 614.912f}, {61.744f, 614.912f}, {59.720f, 614.912f},
            {58.708f, 613.900f}, {58.202f, 613.394f}, {58.708f, 610.864f},
            {61.238f, 610.864f}, {65.287f, 610.864f}, {65.287f, 610.864f},
            {67.818f, 610.864f}, {70.854f, 610.864f}, {74.397f, 610.358f},
            {78.446f, 610.864f}, {82.494f, 611.876f}, {86.037f, 612.382f},
            {89.580f, 613.900f}, {92.616f, 614.912f}, {95.653f, 615.925f},
            {97.677f, 616.937f}, {98.184f, 617.949f}, {98.690f, 619.467f},
            {99.702f, 622.504f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.51f, 0.345f, 0.29f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.49f, 0.651f, 0.82f, 1.0f);
    }
    {
        static const float p[][2] = {
            {40.488f, 583.028f}, {42.512f, 582.522f}, {44.031f, 582.016f},
            {46.055f, 582.522f}, {49.598f, 582.522f}, {51.622f, 583.028f},
            {54.153f, 584.040f}, {56.177f, 585.052f}, {59.720f, 589.607f},
            {59.720f, 589.607f}, {60.732f, 595.175f}, {59.720f, 597.705f},
            {56.683f, 598.211f}, {56.683f, 598.211f}, {53.647f, 598.211f},
            {50.610f, 598.717f}, {47.573f, 597.705f}, {44.031f, 597.199f},
            {40.488f, 595.681f}, {38.970f, 593.656f}, {38.464f, 591.126f},
            {37.958f, 589.607f}, {37.958f, 586.571f}, {39.476f, 586.065f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.51f, 0.345f, 0.29f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.49f, 0.651f, 0.82f, 1.0f);
    }
    {
        static const float p[][2] = {
            {25.305f, 560.760f}, {24.293f, 558.735f}, {23.281f, 556.711f},
            {22.775f, 553.674f}, {22.775f, 551.650f}, {25.305f, 552.156f},
            {26.823f, 552.662f}, {27.836f, 552.156f}, {28.342f, 550.638f},
            {28.342f, 549.625f}, {28.342f, 547.601f}, {28.342f, 546.083f},
            {27.836f, 544.058f}, {27.836f, 542.540f}, {27.836f, 541.528f},
            {27.329f, 539.503f}, {27.329f, 536.973f}, {25.305f, 530.394f},
            {22.268f, 525.839f}, {22.268f, 525.839f}, {17.714f, 520.272f},
            {15.183f, 518.247f}, {15.183f, 518.247f}, {14.677f, 514.704f},
            {15.689f, 512.174f}, {16.701f, 511.668f}, {19.232f, 511.668f},
            {21.256f, 512.174f}, {23.787f, 513.692f}, {24.799f, 515.211f},
            {26.823f, 517.741f}, {29.354f, 519.765f}, {33.403f, 520.272f},
            {35.427f, 520.778f}, {36.439f, 522.296f}, {35.933f, 524.826f},
            {35.427f, 526.851f}, {34.415f, 529.381f}, {34.921f, 531.406f},
            {36.439f, 533.430f}, {38.970f, 536.467f}, {40.994f, 537.985f},
            {42.006f, 538.997f}, {43.525f, 541.022f}, {45.549f, 544.058f},
            {44.537f, 545.577f}, {44.031f, 547.601f}, {40.994f, 552.662f},
            {36.439f, 555.193f}, {33.403f, 557.217f}, {33.403f, 557.217f},
            {35.427f, 560.760f}, {36.439f, 562.784f}, {36.945f, 564.302f},
            {36.439f, 566.833f}, {35.933f, 568.351f}, {33.403f, 568.351f},
            {31.378f, 566.833f}, {30.366f, 565.821f}, {27.836f, 563.796f},
            {27.836f, 563.796f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.51f, 0.345f, 0.29f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.49f, 0.651f, 0.82f, 1.0f);
    }
    {
        static const float p[][2] = {
            {283.416f, 619.467f}, {282.910f, 616.431f}, {283.416f, 614.912f},
            {284.429f, 612.888f}, {285.947f, 611.370f}, {287.465f, 609.345f},
            {289.490f, 607.827f}, {291.514f, 606.815f}, {294.551f, 605.803f},
            {296.069f, 606.815f}, {296.069f, 606.815f}, {296.575f, 608.333f},
            {296.575f, 610.358f}, {295.057f, 612.888f}, {293.538f, 614.406f},
            {291.008f, 615.925f}, {288.984f, 617.949f}, {285.947f, 619.467f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.51f, 0.345f, 0.29f, 1.0f, 2.0f);
        drawFilledTessCall(p, n, 0.886f, 0.592f, 0.494f, 1.0f);
    }
    {
        static const float p[][2] = {
            {177.438f, 516.449f}, {179.146f, 518.158f}, {180.611f, 518.890f},
            {182.075f, 519.866f}, {183.540f, 520.598f}, {185.004f, 521.331f},
            {186.713f, 522.307f}, {186.713f, 522.307f}, {188.909f, 523.771f},
            {188.909f, 523.771f}, {189.885f, 524.504f}, {190.862f, 524.992f},
            {191.838f, 525.724f}, {191.838f, 527.676f}, {191.350f, 529.385f},
            {190.862f, 531.093f}, {190.129f, 532.558f}, {189.153f, 534.510f},
            {188.177f, 535.731f}, {187.445f, 536.707f}, {186.224f, 537.439f},
            {185.248f, 537.683f}, {183.540f, 534.754f}, {183.051f, 533.290f},
            {183.051f, 533.290f}, {181.831f, 530.117f}, {179.635f, 529.873f},
            {179.635f, 529.873f}, {177.926f, 529.873f}, {176.462f, 530.117f},
            {175.485f, 531.337f}, {174.753f, 531.582f}, {173.289f, 530.849f},
            {175.974f, 518.158f}, {176.218f, 516.693f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.51f, 0.024f, 0.196f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.592f, 0.796f, 0.941f, 1.0f);
    }
    {
        static const float p[][2] = {
            {198.916f, 574.782f}, {197.696f, 574.049f}, {196.719f, 572.341f},
            {195.255f, 571.121f}, {194.523f, 569.900f}, {193.791f, 568.192f},
            {193.058f, 566.971f}, {192.326f, 565.019f}, {192.326f, 563.066f},
            {192.570f, 561.114f}, {192.570f, 559.405f}, {193.302f, 556.965f},
            {193.791f, 555.256f}, {194.035f, 553.792f}, {194.523f, 552.327f},
            {195.011f, 550.375f}, {195.743f, 549.399f}, {195.987f, 548.178f},
            {196.719f, 546.714f}, {197.696f, 545.249f}, {199.160f, 543.541f},
            {200.869f, 542.565f}, {202.089f, 543.053f}, {202.821f, 544.517f},
            {203.309f, 546.714f}, {204.041f, 548.422f}, {204.285f, 550.375f},
            {204.530f, 552.571f}, {204.530f, 553.548f}, {205.018f, 555.256f},
            {204.530f, 558.429f}, {204.285f, 563.066f}, {204.041f, 567.216f},
            {203.797f, 568.436f}, {203.309f, 570.877f}, {201.845f, 573.073f},
            {201.113f, 574.294f}, {200.624f, 574.538f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.749f, 0.188f, 0.384f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.592f, 0.796f, 0.941f, 1.0f);
    }
    {
        static const float p[][2] = {
            {298.984f, 593.087f}, {298.008f, 592.355f}, {297.520f, 591.378f},
            {297.276f, 590.402f}, {297.276f, 588.938f}, {297.276f, 587.473f},
            {297.276f, 586.009f}, {297.032f, 585.277f}, {297.276f, 582.348f},
            {297.276f, 580.395f}, {297.276f, 580.395f}, {297.520f, 577.955f},
            {298.740f, 576.002f}, {299.228f, 574.782f}, {299.960f, 573.073f},
            {300.693f, 571.853f}, {303.133f, 570.144f}, {303.133f, 570.144f},
            {304.354f, 570.144f}, {305.818f, 570.144f}, {306.550f, 570.877f},
            {307.282f, 572.097f}, {307.527f, 573.073f}, {307.771f, 574.294f},
            {307.771f, 575.758f}, {307.771f, 576.978f}, {307.527f, 578.443f},
            {307.527f, 579.419f}, {307.282f, 580.639f}, {307.282f, 581.616f},
            {307.282f, 583.080f}, {306.550f, 584.544f}, {306.306f, 585.765f},
            {305.330f, 587.961f}, {304.598f, 589.182f}, {303.865f, 589.914f},
            {303.133f, 590.890f}, {302.401f, 591.622f}, {300.937f, 592.355f},
            {299.960f, 593.087f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.749f, 0.188f, 0.384f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.592f, 0.796f, 0.941f, 1.0f);
    }
    {
        static const float p[][2] = {
            {221.614f, 538.171f}, {222.591f, 537.927f}, {223.811f, 537.927f},
            {225.519f, 537.683f}, {226.740f, 537.927f}, {227.960f, 538.415f},
            {227.960f, 538.415f}, {229.425f, 539.148f}, {230.401f, 539.636f},
            {231.133f, 540.124f}, {232.109f, 541.100f}, {232.841f, 541.832f},
            {233.086f, 542.321f}, {233.574f, 543.053f}, {233.574f, 543.785f},
            {233.574f, 544.517f}, {233.330f, 545.493f}, {233.330f, 545.982f},
            {232.841f, 546.714f}, {231.865f, 547.934f}, {230.889f, 548.422f},
            {230.401f, 549.154f}, {229.669f, 549.643f}, {228.692f, 549.887f},
            {227.716f, 550.131f}, {226.740f, 550.619f}, {224.787f, 550.863f},
            {223.079f, 550.863f}, {221.858f, 550.863f}, {220.882f, 550.619f},
            {219.174f, 550.375f}, {218.686f, 550.131f}, {217.221f, 548.910f},
            {216.489f, 547.934f}, {215.757f, 545.738f}, {215.757f, 545.738f},
            {216.001f, 544.029f}, {216.489f, 543.053f}, {217.221f, 541.344f},
            {218.441f, 540.124f}, {219.418f, 539.636f}, {220.882f, 538.415f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.58, 0.243, 0.459, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.729, 0.341, 0.588, 1.0f);
    }
    {
        static const float p[][2] = {
            {215.513f, 589.914f}, {215.269f, 588.205f}, {215.269f, 586.985f},
            {215.513f, 586.009f}, {215.269f, 584.789f}, {215.513f, 583.324f},
            {216.001f, 582.348f}, {216.001f, 581.616f}, {216.489f, 580.395f},
            {216.977f, 579.663f}, {217.465f, 578.687f}, {217.953f, 577.955f},
            {219.174f, 576.246f}, {220.394f, 574.782f}, {221.370f, 573.561f},
            {222.102f, 572.829f}, {222.835f, 572.585f}, {223.811f, 572.097f},
            {224.787f, 572.097f}, {226.008f, 572.097f}, {226.984f, 572.341f},
            {227.716f, 572.585f}, {228.936f, 573.805f}, {228.936f, 574.538f},
            {228.936f, 575.514f}, {228.692f, 576.490f}, {228.204f, 577.466f},
            {227.716f, 578.931f}, {227.716f, 580.639f}, {227.716f, 582.348f},
            {228.204f, 583.812f}, {228.692f, 584.789f}, {228.936f, 585.277f},
            {229.425f, 586.253f}, {230.157f, 587.473f}, {230.645f, 588.205f},
            {230.889f, 589.914f}, {231.133f, 590.890f}, {231.133f, 592.355f},
            {231.133f, 593.087f}, {230.889f, 594.307f}, {230.157f, 595.772f},
            {228.936f, 596.748f}, {227.716f, 596.992f}, {226.252f, 597.236f},
            {224.543f, 596.992f}, {222.835f, 596.748f}, {221.370f, 596.016f},
            {219.662f, 595.283f}, {218.686f, 594.307f}, {217.709f, 593.331f},
            {216.977f, 592.355f}, {216.489f, 591.622f}, {216.001f, 590.890f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.58, 0.243, 0.459, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.729, 0.341, 0.588, 1.0f);
    }
    
    //* Coral Stone
    {
        static const float p[][2] = {
            {84.013f, 677.669f}, {84.013f, 675.138f}, {85.025f, 672.608f},
            {86.543f, 671.090f}, {90.086f, 667.547f}, {90.086f, 667.547f},
            {91.098f, 666.029f}, {93.123f, 664.510f}, {97.677f, 659.955f},
            {97.677f, 659.955f}, {100.714f, 656.919f}, {103.751f, 654.388f},
            {106.281f, 652.364f}, {108.812f, 650.339f}, {111.342f, 649.327f},
            {113.873f, 647.809f}, {117.415f, 647.303f}, {119.946f, 646.797f},
            {124.501f, 647.303f}, {128.043f, 648.315f}, {131.080f, 649.833f},
            {133.105f, 651.352f}, {136.141f, 654.388f}, {138.672f, 656.413f},
            {143.227f, 659.955f}, {143.227f, 659.955f}, {148.794f, 661.474f},
            {150.818f, 661.474f}, {154.361f, 661.980f}, {157.397f, 662.992f},
            {158.916f, 664.510f}, {161.952f, 663.498f}, {164.989f, 662.486f},
            {169.038f, 661.980f}, {171.568f, 660.968f}, {174.099f, 660.462f},
            {176.123f, 659.449f}, {178.654f, 658.943f}, {182.196f, 658.437f},
            {184.727f, 658.437f}, {188.776f, 659.449f}, {192.824f, 661.474f},
            {194.849f, 661.980f}, {197.885f, 661.980f}, {199.910f, 662.486f},
            {202.440f, 662.486f}, {205.477f, 662.486f}, {207.501f, 661.980f},
            {209.020f, 660.968f}, {210.538f, 660.462f}, {211.550f, 659.955f},
            {214.081f, 658.437f}, {215.599f, 657.931f}, {217.623f, 657.425f},
            {219.648f, 657.425f}, {223.190f, 658.437f}, {226.227f, 659.449f},
            {228.758f, 659.955f}, {231.794f, 660.462f}, {235.337f, 660.462f},
            {241.410f, 660.462f}, {241.410f, 660.462f}, {245.965f, 658.437f},
            {249.002f, 655.907f}, {249.002f, 655.907f}, {251.532f, 654.894f},
            {254.569f, 654.388f}, {256.593f, 655.400f}, {269.246f, 664.004f},
            {270.258f, 667.041f}, {269.752f, 669.571f}, {268.740f, 671.596f},
            {264.691f, 677.669f}, {261.654f, 678.681f}, {261.654f, 678.681f},
            {256.593f, 678.681f}, {251.026f, 678.681f}, {248.495f, 678.681f},
            {248.495f, 678.681f}, {245.459f, 678.681f}, {239.386f, 679.187f},
            {236.349f, 679.693f}, {236.349f, 679.693f}, {233.819f, 680.199f},
            {231.794f, 681.212f}, {230.276f, 682.224f}, {228.758f, 683.236f},
            {225.721f, 684.754f}, {222.684f, 685.260f}, {218.636f, 684.754f},
            {201.934f, 685.767f}, {201.934f, 685.767f}, {185.233f, 681.718f},
            {180.678f, 677.163f}, {178.147f, 676.151f}, {173.593f, 675.645f},
            {171.568f, 676.151f}, {169.544f, 677.669f}, {168.025f, 678.681f},
            {164.989f, 679.693f}, {161.446f, 680.706f}, {161.446f, 680.706f},
            {155.879f, 681.212f}, {152.336f, 680.706f}, {152.336f, 680.706f},
            {149.806f, 680.706f}, {146.263f, 680.706f}, {142.214f, 681.718f},
            {139.178f, 682.730f}, {133.611f, 684.248f}, {133.611f, 684.248f},
            {128.550f, 685.767f}, {125.007f, 686.273f}, {125.007f, 686.273f},
            {122.476f, 686.779f}, {119.440f, 686.273f}, {115.897f, 685.767f},
            {109.318f, 683.742f}, {105.269f, 683.236f}, {105.269f, 683.236f},
            {102.738f, 682.730f}, {99.702f, 682.730f}, {97.677f, 682.224f},
            {95.653f, 682.224f}, {90.086f, 681.718f}, {90.086f, 681.718f},
            {88.062f, 681.212f}, {86.037f, 680.706f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.616f, 0.431f, 0.749f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.745f, 0.549f, 0.8f, 1.0f);
    }
    {
        static const float p[][2] = {
            {182.196f, 678.681f}, {183.715f, 677.669f}, {185.233f, 676.151f},
            {186.751f, 675.138f}, {188.776f, 674.126f}, {190.294f, 672.608f},
            {192.824f, 671.090f}, {194.343f, 670.584f}, {195.861f, 669.571f},
            {198.392f, 668.053f}, {203.959f, 666.535f}, {203.959f, 666.535f},
            {211.044f, 665.016f}, {214.081f, 665.016f}, {214.081f, 665.016f},
            {217.117f, 665.016f}, {220.154f, 665.523f}, {222.178f, 666.029f},
            {223.190f, 667.041f}, {225.215f, 668.559f}, {226.733f, 670.077f},
            {228.251f, 671.090f}, {230.782f, 673.620f}, {232.806f, 676.151f},
            {233.819f, 677.669f}, {234.831f, 679.187f}, {232.300f, 680.706f},
            {230.782f, 680.706f}, {226.227f, 685.767f}, {225.721f, 685.767f},
            {217.623f, 685.260f}, {213.068f, 684.754f}, {208.007f, 685.767f},
            {202.440f, 686.273f}, {202.440f, 686.273f}, {183.208f, 681.212f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.616f, 0.431f, 0.749f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.745f, 0.549f, 0.8f, 1.0f);
    }
    {
        static const float p[][2] = {
            {249.002f, 653.882f}, {250.014f, 650.339f}, {252.038f, 648.315f},
            {256.087f, 645.785f}, {256.087f, 645.785f}, {259.124f, 644.266f},
            {261.654f, 642.748f}, {267.727f, 641.736f}, {267.727f, 641.736f},
            {271.270f, 642.748f}, {273.294f, 643.760f}, {275.825f, 644.266f},
            {277.849f, 644.772f}, {278.862f, 645.278f}, {282.404f, 644.772f},
            {284.429f, 644.266f}, {286.453f, 643.760f}, {287.971f, 642.748f},
            {290.502f, 642.242f}, {292.526f, 641.230f}, {298.093f, 640.724f},
            {298.093f, 640.724f}, {303.660f, 640.724f}, {307.203f, 640.217f},
            {312.770f, 641.230f}, {312.770f, 641.230f}, {315.301f, 642.748f},
            {318.337f, 644.266f}, {319.350f, 645.278f}, {321.374f, 646.291f},
            {322.892f, 647.303f}, {324.917f, 648.315f}, {326.435f, 649.327f},
            {328.459f, 650.339f}, {330.990f, 650.846f}, {332.508f, 651.858f},
            {334.533f, 654.894f}, {334.533f, 654.894f}, {338.075f, 658.437f},
            {341.112f, 660.462f}, {342.630f, 661.980f}, {344.655f, 663.498f},
            {342.124f, 664.510f}, {335.545f, 665.523f}, {335.545f, 665.523f},
            {331.496f, 666.029f}, {327.953f, 666.029f}, {324.917f, 666.029f},
            {322.892f, 666.029f}, {320.868f, 667.547f}, {317.831f, 669.571f},
            {314.289f, 670.077f}, {310.240f, 670.077f}, {308.215f, 668.559f},
            {301.636f, 667.547f}, {301.636f, 667.547f}, {299.106f, 667.547f},
            {295.563f, 668.053f}, {293.032f, 668.053f}, {289.996f, 668.053f},
            {281.392f, 668.559f}, {281.392f, 668.559f}, {277.343f, 668.559f},
            {274.307f, 668.559f}, {270.764f, 668.053f}, {270.258f, 665.523f},
            {269.752f, 664.510f}, {266.209f, 661.980f}, {264.185f, 660.462f},
            {262.160f, 658.943f}, {261.148f, 657.931f}, {258.617f, 655.907f},
            {255.075f, 654.388f}, {251.532f, 654.388f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.51f, 0.396f, 0.294f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.639f, 0.494f, 0.369f, 1.0f);
    }
    {
        static const float p[][2] = {
            {114.553f, 420.027f}, {115.357f, 420.831f}, {116.562f, 422.036f},
            {117.768f, 423.242f}, {120.582f, 423.242f}, {120.582f, 423.242f},
            {122.592f, 422.840f}, {124.199f, 421.634f}, {125.405f, 420.027f},
            {125.003f, 418.419f}, {124.601f, 416.409f}, {122.592f, 416.007f},
            {121.788f, 416.007f}, {120.582f, 416.007f}, {118.572f, 416.409f},
            {116.562f, 416.811f}, {115.759f, 417.615f}, {114.151f, 418.419f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.51f, 0.396f, 0.294f, 1.0f, 2.0f);
        drawFilledTessCall(p, n, 0.929, 0.506, 0.635, 1.0f);
    }
    //? Coloring
    //* 1
    {
        static const float p[][2] = {
            {243.941f, 662.486f}, {244.953f, 660.462f}, {246.977f, 658.943f},
            {247.989f, 657.931f}, {251.532f, 655.907f}, {251.532f, 655.907f},
            {255.581f, 655.907f}, {257.605f, 658.437f}, {260.136f, 659.955f},
            {260.136f, 659.955f}, {263.678f, 662.992f}, {263.678f, 662.992f},
            {265.703f, 664.510f}, {266.715f, 666.029f}, {267.221f, 668.559f},
            {267.221f, 668.559f}, {266.209f, 670.584f}, {265.197f, 671.596f},
            {243.941f, 664.004f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.886f, 0.667f, 0.902f, 1.0f);
    }
    {
        static const float p[][2] = {
            {226.694f, 506.444f}, {225.488f, 508.051f}, {223.076f, 509.257f},
            {221.469f, 510.061f}, {221.469f, 510.061f}, {220.263f, 510.865f},
            {219.459f, 512.071f}, {216.243f, 513.679f}, {213.832f, 513.277f},
            {211.018f, 511.267f}, {208.607f, 510.463f}, {206.597f, 506.846f},
            {204.989f, 504.032f}, {203.381f, 500.415f}, {202.175f, 498.003f},
            {201.774f, 495.189f}, {201.774f, 491.974f}, {201.774f, 491.974f},
            {202.175f, 489.562f}, {202.577f, 486.347f}, {202.577f, 486.347f},
            {203.381f, 483.533f}, {204.185f, 481.523f}, {204.587f, 479.514f},
            {205.391f, 477.906f}, {206.597f, 476.298f}, {207.803f, 474.690f},
            {209.008f, 472.681f}, {210.616f, 470.269f}, {211.822f, 468.259f},
            {213.832f, 465.446f}, {215.841f, 462.632f}, {217.449f, 460.623f},
            {217.851f, 459.819f}, {218.655f, 457.809f}, {219.861f, 455.397f},
            {220.665f, 452.584f}, {221.469f, 449.770f}, {222.272f, 447.760f},
            {223.478f, 445.349f}, {223.880f, 442.937f}, {224.282f, 440.124f},
            {225.086f, 437.712f}, {226.694f, 435.702f}, {227.900f, 433.693f},
            {229.105f, 432.889f}, {230.311f, 431.683f}, {231.517f, 429.673f},
            {232.723f, 429.673f}, {232.723f, 433.291f}, {231.517f, 438.516f},
            {230.311f, 443.339f}, {229.105f, 448.162f}, {227.096f, 452.986f},
            {225.488f, 457.005f}, {222.272f, 463.436f}, {221.469f, 466.250f},
            {221.469f, 469.063f}, {221.469f, 472.681f}, {220.665f, 475.896f},
            {219.057f, 479.916f}, {218.253f, 481.523f}, {217.851f, 484.337f},
            {217.851f, 487.151f}, {217.449f, 490.366f}, {218.655f, 493.180f},
            {219.057f, 494.787f}, {220.263f, 496.395f}, {221.871f, 499.209f},
            {223.076f, 502.424f}, {224.684f, 504.836f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.761, 0.435, 0.533, 1.0f);
    }
    //* 2
    {
        static const float p[][2] = {
            {211.044f, 672.102f}, {209.526f, 671.090f}, {209.020f, 669.571f},
            {209.020f, 668.053f}, {210.032f, 667.041f}, {211.550f, 666.535f},
            {216.105f, 666.029f}, {218.129f, 666.535f}, {220.154f, 667.547f},
            {221.672f, 668.053f}, {223.190f, 668.559f}, {224.203f, 669.571f},
            {225.721f, 670.584f}, {227.239f, 671.596f}, {228.758f, 673.114f},
            {232.300f, 677.669f}, {232.300f, 678.681f}, {229.770f, 678.175f},
            {223.697f, 678.175f}, {220.660f, 676.657f}, {217.623f, 675.138f},
            {217.623f, 675.138f}, {215.093f, 674.126f}, {212.562f, 673.114f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.886f, 0.667f, 0.902f, 1.0f);
    }
    //* 3
    {
        static const float p[][2] = {
            {116.909f, 647.809f}, {119.440f, 647.809f}, {120.958f, 647.809f},
            {122.476f, 648.315f}, {126.525f, 649.833f}, {126.525f, 649.833f},
            {130.068f, 651.858f}, {132.092f, 652.870f}, {134.623f, 654.388f},
            {136.141f, 655.400f}, {138.166f, 657.425f}, {139.684f, 659.449f},
            {140.696f, 660.968f}, {142.720f, 661.980f}, {146.263f, 661.980f},
            {150.312f, 662.486f}, {154.361f, 662.992f}, {156.385f, 664.004f},
            {157.903f, 665.016f}, {159.928f, 666.029f}, {164.989f, 664.510f},
            {168.532f, 662.486f}, {172.580f, 660.968f}, {176.123f, 660.462f},
            {180.172f, 659.449f}, {183.715f, 659.449f}, {187.257f, 660.968f},
            {190.294f, 660.968f}, {193.837f, 661.980f}, {197.379f, 662.486f},
            {200.416f, 662.992f}, {203.959f, 662.992f}, {207.501f, 662.486f},
            {210.032f, 661.474f}, {214.081f, 660.462f}, {218.129f, 659.955f},
            {218.129f, 659.955f}, {220.154f, 659.955f}, {221.166f, 659.955f},
            {221.672f, 663.498f}, {219.142f, 664.004f}, {216.105f, 664.004f},
            {209.526f, 664.510f}, {208.007f, 664.510f}, {202.440f, 666.029f},
            {199.404f, 667.041f}, {199.404f, 667.041f}, {197.379f, 668.053f},
            {195.861f, 669.065f}, {193.837f, 670.077f}, {187.763f, 669.571f},
            {187.763f, 669.571f}, {182.196f, 669.065f}, {173.593f, 668.053f},
            {173.593f, 668.053f}, {171.062f, 668.559f}, {163.977f, 670.077f},
            {160.434f, 670.077f}, {160.434f, 670.077f}, {158.916f, 671.090f},
            {157.397f, 671.596f}, {152.336f, 671.596f}, {149.300f, 671.090f},
            {146.263f, 671.090f}, {144.745f, 669.571f}, {142.720f, 667.547f},
            {141.202f, 665.523f}, {138.672f, 665.523f}, {139.178f, 667.547f},
            {140.190f, 669.571f}, {141.202f, 671.596f}, {143.227f, 673.620f},
            {144.745f, 675.645f}, {144.745f, 678.175f}, {139.684f, 677.669f},
            {137.153f, 677.669f}, {134.117f, 677.163f}, {129.562f, 677.163f},
            {124.501f, 676.657f}, {120.452f, 676.657f}, {117.921f, 675.138f},
            {116.403f, 673.620f}, {116.403f, 670.584f}, {116.403f, 668.559f},
            {116.403f, 666.535f}, {116.909f, 663.498f}, {118.934f, 662.486f},
            {121.970f, 660.968f}, {124.501f, 659.449f}, {127.031f, 657.931f},
            {128.043f, 656.919f}, {129.056f, 655.400f}, {126.525f, 653.882f},
            {125.007f, 653.376f}, {122.982f, 652.364f}, {120.958f, 651.858f},
            {119.946f, 651.352f}, {117.921f, 650.846f}, {115.897f, 650.339f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.886f, 0.667f, 0.902f, 1.0f);
    }
    {
        static const float p[][2] = {
            {193.837f, 671.596f}, {194.343f, 673.620f}, {196.367f, 675.645f},
            {199.404f, 677.669f}, {201.428f, 678.175f}, {203.453f, 678.681f},
            {210.538f, 678.681f}, {214.081f, 678.175f}, {217.117f, 677.669f},
            {217.117f, 677.669f}, {217.623f, 676.657f}, {215.599f, 676.151f},
            {215.599f, 676.151f}, {214.587f, 675.645f}, {213.068f, 675.138f},
            {210.538f, 674.126f}, {209.020f, 672.608f}, {208.007f, 670.077f},
            {208.514f, 668.053f}, {209.020f, 666.535f}, {207.501f, 666.535f},
            {205.477f, 666.535f}, {203.959f, 667.041f}, {201.934f, 667.547f},
            {199.404f, 669.065f}, {197.379f, 670.584f}, {195.355f, 671.596f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.659f, 0.475f, 0.671f, 1.0f);
    }
    {
        static const float p[][2] = {
            {296.069f, 642.242f}, {300.118f, 641.230f}, {306.697f, 641.230f},
            {309.734f, 642.242f}, {309.734f, 642.242f}, {312.264f, 643.254f},
            {315.301f, 646.291f}, {317.831f, 647.303f}, {318.843f, 647.809f},
            {320.868f, 649.327f}, {323.904f, 649.833f}, {325.423f, 650.339f},
            {327.447f, 651.352f}, {329.978f, 652.870f}, {337.569f, 659.449f},
            {337.569f, 659.449f}, {340.606f, 660.462f}, {335.039f, 660.968f},
            {329.978f, 659.955f}, {315.807f, 658.437f}, {311.758f, 658.943f},
            {307.709f, 658.943f}, {302.648f, 658.437f}, {298.599f, 658.437f},
            {294.045f, 656.919f}, {292.020f, 654.894f}, {291.008f, 652.870f},
            {292.526f, 647.809f}, {294.045f, 645.278f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.929f, 0.835f, 0.780f, 1.0f);
    }
    {
        static const float p[][2] = {
            {208.007f, 657.931f}, {208.514f, 654.894f}, {208.514f, 652.870f},
            {209.020f, 645.278f}, {209.526f, 636.675f}, {209.020f, 632.626f},
            {208.514f, 627.059f}, {206.995f, 622.504f}, {204.465f, 619.467f},
            {200.416f, 613.394f}, {194.849f, 609.851f}, {190.294f, 604.790f},
            {190.294f, 604.790f}, {185.233f, 600.742f}, {199.404f, 607.827f},
            {202.440f, 609.345f}, {206.489f, 611.370f}, {211.044f, 611.876f},
            {214.587f, 611.370f}, {217.117f, 612.888f}, {219.648f, 614.912f},
            {234.325f, 639.205f}, {238.373f, 646.797f}, {240.398f, 651.352f},
            {241.916f, 655.907f}, {242.422f, 658.437f}, {238.373f, 659.955f},
            {234.831f, 659.955f}, {229.770f, 658.943f}, {225.215f, 655.907f},
            {222.178f, 654.894f}, {218.129f, 654.388f}, {215.093f, 655.400f},
            {213.068f, 656.413f}, {210.538f, 657.931f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.949f, 0.459f, 0.835f, 1.0f);
    }
    {
        static const float p[][2] = {
            {252.544f, 624.022f}, {250.520f, 625.034f}, {248.495f, 627.059f},
            {245.459f, 628.071f}, {243.434f, 626.047f}, {241.916f, 624.022f},
            {238.373f, 617.949f}, {238.373f, 617.949f}, {238.373f, 614.912f},
            {237.867f, 612.382f}, {238.880f, 605.803f}, {238.880f, 605.803f},
            {239.386f, 602.260f}, {239.892f, 599.223f}, {241.410f, 597.199f},
            {242.422f, 594.162f}, {244.447f, 591.632f}, {246.977f, 588.595f},
            {249.508f, 586.065f}, {251.026f, 584.546f}, {253.050f, 583.028f},
            {256.087f, 581.004f}, {260.136f, 578.473f}, {263.678f, 576.449f},
            {267.221f, 573.918f}, {270.258f, 572.400f}, {266.715f, 576.449f},
            {264.185f, 578.473f}, {261.148f, 581.510f}, {258.111f, 584.040f},
            {254.569f, 586.571f}, {251.532f, 589.101f}, {249.002f, 591.632f},
            {247.483f, 594.668f}, {245.965f, 597.199f}, {243.941f, 602.260f},
            {242.928f, 607.321f}, {242.422f, 611.876f}, {243.434f, 614.912f},
            {243.941f, 616.937f}, {245.459f, 619.467f}, {246.471f, 621.998f},
            {247.989f, 623.010f}, {250.014f, 623.010f}, {251.532f, 623.010f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.949f, 0.765f, 0.906f, 1.0f);
    }
    {
        static const float p[][2] = {
            {145.251f, 529.888f}, {143.733f, 528.369f}, {145.251f, 526.851f},
            {146.769f, 526.851f}, {148.288f, 527.357f}, {150.818f, 529.888f},
            {152.842f, 532.418f}, {154.361f, 535.455f}, {156.385f, 540.516f},
            {157.903f, 543.046f}, {160.940f, 545.577f}, {163.977f, 546.589f},
            {168.532f, 546.589f}, {173.086f, 546.589f}, {175.617f, 548.613f},
            {178.147f, 549.625f}, {180.172f, 553.168f}, {181.690f, 557.723f},
            {182.196f, 560.760f}, {182.702f, 564.302f}, {183.715f, 568.857f},
            {184.727f, 573.918f}, {186.751f, 577.967f}, {188.269f, 580.498f},
            {189.788f, 582.522f}, {192.318f, 585.052f}, {194.343f, 586.065f},
            {195.861f, 587.077f}, {191.812f, 586.571f}, {189.788f, 584.546f},
            {189.788f, 584.546f}, {188.776f, 583.534f}, {185.233f, 578.473f},
            {183.715f, 574.424f}, {183.715f, 574.424f}, {182.196f, 570.376f},
            {180.678f, 568.351f}, {179.666f, 565.315f}, {178.654f, 558.735f},
            {177.135f, 556.711f}, {176.629f, 555.193f}, {175.617f, 553.168f},
            {175.111f, 553.168f}, {173.086f, 551.650f}, {170.556f, 551.144f},
            {167.013f, 551.144f}, {164.989f, 551.144f}, {163.471f, 550.638f},
            {161.446f, 550.132f}, {158.410f, 548.107f}, {156.385f, 544.564f},
            {154.867f, 543.552f}, {153.349f, 540.516f}, {151.830f, 538.491f},
            {150.818f, 536.973f}, {149.300f, 534.949f}, {148.288f, 532.924f},
            {146.769f, 530.900f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.949f, 0.765f, 0.906f, 1.0f);
    }
    {
        static const float p[][2] = {
            {228.758f, 509.137f}, {229.770f, 506.101f}, {231.794f, 504.582f},
            {233.819f, 502.052f}, {234.831f, 500.028f}, {235.843f, 497.497f},
            {236.855f, 494.967f}, {237.867f, 491.930f}, {238.373f, 488.893f},
            {238.880f, 486.869f}, {239.892f, 480.796f}, {240.398f, 477.759f},
            {240.398f, 477.759f}, {241.410f, 475.735f}, {242.422f, 473.710f},
            {244.447f, 471.686f}, {245.965f, 472.698f}, {247.483f, 474.723f},
            {246.977f, 476.747f}, {246.471f, 479.277f}, {245.459f, 482.820f},
            {245.459f, 486.363f}, {246.977f, 488.893f}, {248.495f, 489.399f},
            {251.026f, 490.412f}, {255.581f, 490.412f}, {260.642f, 488.893f},
            {262.666f, 487.375f}, {264.691f, 485.857f}, {266.209f, 484.338f},
            {267.727f, 481.808f}, {269.246f, 479.277f}, {269.752f, 478.265f},
            {270.764f, 476.747f}, {271.776f, 474.216f}, {272.282f, 471.686f},
            {273.801f, 467.637f}, {275.825f, 463.082f}, {277.849f, 464.094f},
            {277.849f, 464.094f}, {279.368f, 465.107f}, {280.380f, 465.107f},
            {282.910f, 465.613f}, {283.923f, 466.625f}, {283.923f, 468.143f},
            {283.923f, 469.662f}, {282.404f, 472.192f}, {280.886f, 474.723f},
            {279.368f, 477.253f}, {277.343f, 479.277f}, {275.319f, 481.302f},
            {270.258f, 485.351f}, {266.715f, 488.387f}, {266.715f, 488.387f},
            {265.703f, 489.906f}, {263.172f, 492.436f}, {260.136f, 493.954f},
            {256.593f, 495.473f}, {251.026f, 496.991f}, {246.977f, 498.003f},
            {246.977f, 498.003f}, {244.953f, 498.003f}, {242.422f, 499.015f},
            {239.386f, 500.028f}, {237.361f, 502.052f}, {235.843f, 503.570f},
            {233.819f, 505.089f}, {232.300f, 507.619f}, {230.782f, 509.643f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.949f, 0.765f, 0.906f, 1.0f);
    }
    {
        static const float p[][2] = {
            {250.014f, 443.850f}, {250.014f, 440.814f}, {249.508f, 438.283f},
            {250.014f, 434.741f}, {251.026f, 430.186f}, {252.038f, 426.643f},
            {253.556f, 423.100f}, {254.569f, 420.064f}, {257.099f, 416.521f},
            {259.630f, 413.484f}, {260.136f, 408.929f}, {259.630f, 405.387f},
            {259.630f, 401.844f}, {259.630f, 397.795f}, {258.111f, 395.265f},
            {258.111f, 391.722f}, {257.099f, 387.673f}, {257.099f, 386.661f},
            {258.111f, 383.118f}, {258.111f, 383.118f}, {258.617f, 382.612f},
            {262.666f, 382.106f}, {264.691f, 387.673f}, {263.678f, 391.722f},
            {263.678f, 396.783f}, {263.678f, 396.783f}, {263.678f, 399.820f},
            {263.678f, 401.844f}, {268.740f, 403.868f}, {270.258f, 404.375f},
            {273.801f, 403.362f}, {275.825f, 402.856f}, {276.331f, 405.387f},
            {275.825f, 408.929f}, {273.294f, 410.954f}, {270.258f, 410.954f},
            {266.715f, 412.978f}, {263.172f, 413.990f}, {261.654f, 417.027f},
            {259.630f, 419.051f}, {257.605f, 421.582f}, {256.593f, 424.112f},
            {256.087f, 425.125f}, {255.075f, 428.161f}, {254.063f, 430.692f},
            {253.050f, 433.728f}, {252.038f, 438.789f}, {250.520f, 444.356f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.949f, 0.765f, 0.906f, 1.0f);
    }
    {
        static const float p[][2] = {
            {301.636f, 416.521f}, {301.636f, 415.003f}, {302.142f, 413.484f},
            {305.685f, 411.460f}, {307.709f, 411.966f}, {307.709f, 411.966f},
            {309.734f, 412.978f}, {312.264f, 416.015f}, {312.264f, 417.027f},
            {311.758f, 419.051f}, {310.746f, 421.582f}, {310.746f, 423.100f},
            {309.734f, 424.619f}, {308.215f, 427.149f}, {307.709f, 429.680f},
            {307.203f, 432.210f}, {307.203f, 433.728f}, {307.203f, 435.753f},
            {307.203f, 438.283f}, {307.203f, 440.308f}, {307.709f, 442.332f},
            {308.215f, 444.356f}, {309.228f, 446.381f}, {310.746f, 447.393f},
            {312.264f, 448.911f}, {314.795f, 448.911f}, {316.819f, 448.405f},
            {318.843f, 446.887f}, {321.374f, 445.369f}, {325.929f, 442.332f},
            {325.929f, 442.332f}, {331.496f, 440.308f}, {333.014f, 441.320f},
            {333.014f, 441.320f}, {334.027f, 443.344f}, {335.039f, 445.369f},
            {333.520f, 448.911f}, {333.520f, 448.911f}, {331.496f, 449.924f},
            {328.459f, 451.948f}, {325.929f, 453.466f}, {324.411f, 454.478f},
            {320.868f, 457.009f}, {319.350f, 459.539f}, {317.325f, 462.070f},
            {315.301f, 465.107f}, {313.276f, 469.155f}, {312.770f, 465.107f},
            {315.301f, 460.552f}, {318.337f, 456.503f}, {322.386f, 451.948f},
            {327.953f, 447.393f}, {326.941f, 444.863f}, {326.941f, 444.863f},
            {324.411f, 445.369f}, {322.386f, 446.381f}, {318.337f, 448.405f},
            {315.807f, 449.417f}, {315.807f, 449.417f}, {313.276f, 449.417f},
            {310.746f, 449.417f}, {308.721f, 447.899f}, {307.709f, 446.381f},
            {307.203f, 444.863f}, {306.191f, 443.344f}, {305.685f, 441.320f},
            {305.179f, 438.789f}, {305.685f, 436.765f}, {305.179f, 434.234f},
            {305.179f, 431.704f}, {305.685f, 429.680f}, {306.191f, 427.149f},
            {307.203f, 424.619f}, {307.203f, 421.582f}, {307.203f, 420.064f},
            {306.191f, 417.027f}, {304.673f, 417.027f}, {302.142f, 417.027f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.949f, 0.765f, 0.906f, 1.0f);
    }
    {
        static const float p[][2] = {
            {301.636f, 416.521f}, {301.636f, 415.003f}, {302.142f, 413.484f},
            {305.685f, 411.460f}, {307.709f, 411.966f}, {307.709f, 411.966f},
            {309.734f, 412.978f}, {312.264f, 416.015f}, {312.264f, 417.027f},
            {311.758f, 419.051f}, {310.746f, 421.582f}, {310.746f, 423.100f},
            {309.734f, 424.619f}, {308.215f, 427.149f}, {307.709f, 429.680f},
            {307.203f, 432.210f}, {307.203f, 433.728f}, {307.203f, 435.753f},
            {307.203f, 438.283f}, {307.203f, 440.308f}, {307.709f, 442.332f},
            {308.215f, 444.356f}, {309.228f, 446.381f}, {310.746f, 447.393f},
            {312.264f, 448.911f}, {314.795f, 448.911f}, {316.819f, 448.405f},
            {318.843f, 446.887f}, {321.374f, 445.369f}, {325.929f, 442.332f},
            {325.929f, 442.332f}, {331.496f, 440.308f}, {333.014f, 441.320f},
            {333.014f, 441.320f}, {334.027f, 443.344f}, {335.039f, 445.369f},
            {333.520f, 448.911f}, {333.520f, 448.911f}, {331.496f, 449.924f},
            {328.459f, 451.948f}, {325.929f, 453.466f}, {324.411f, 454.478f},
            {320.868f, 457.009f}, {319.350f, 459.539f}, {317.325f, 462.070f},
            {315.301f, 465.107f}, {313.276f, 469.155f}, {312.770f, 465.107f},
            {315.301f, 460.552f}, {318.337f, 456.503f}, {322.386f, 451.948f},
            {327.953f, 447.393f}, {326.941f, 444.863f}, {326.941f, 444.863f},
            {324.411f, 445.369f}, {322.386f, 446.381f}, {318.337f, 448.405f},
            {315.807f, 449.417f}, {315.807f, 449.417f}, {313.276f, 449.417f},
            {310.746f, 449.417f}, {308.721f, 447.899f}, {307.709f, 446.381f},
            {307.203f, 444.863f}, {306.191f, 443.344f}, {305.685f, 441.320f},
            {305.179f, 438.789f}, {305.685f, 436.765f}, {305.179f, 434.234f},
            {305.179f, 431.704f}, {305.685f, 429.680f}, {306.191f, 427.149f},
            {307.203f, 424.619f}, {307.203f, 421.582f}, {307.203f, 420.064f},
            {306.191f, 417.027f}, {304.673f, 417.027f}, {302.142f, 417.027f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.949f, 0.765f, 0.906f, 1.0f);
    }
    {
        static const float p[][2] = {
            {385.143f, 485.857f}, {386.661f, 485.351f}, {390.710f, 488.387f},
            {390.710f, 488.387f}, {390.710f, 489.399f}, {391.722f, 490.918f},
            {392.228f, 493.954f}, {391.722f, 496.485f}, {390.710f, 498.509f},
            {390.204f, 500.534f}, {388.685f, 504.076f}, {387.673f, 505.595f},
            {386.155f, 508.631f}, {385.143f, 511.162f}, {384.130f, 513.692f},
            {382.612f, 516.729f}, {381.094f, 520.272f}, {380.082f, 522.802f},
            {378.563f, 527.357f}, {378.563f, 530.900f}, {378.057f, 534.949f},
            {378.563f, 537.985f}, {379.576f, 538.997f}, {381.094f, 538.997f},
            {384.130f, 538.997f}, {386.661f, 537.479f}, {389.191f, 535.455f},
            {392.228f, 531.912f}, {396.277f, 528.369f}, {397.289f, 525.839f},
            {399.820f, 523.308f}, {402.856f, 522.802f}, {404.881f, 523.308f},
            {408.929f, 522.802f}, {412.472f, 522.296f}, {414.497f, 521.284f},
            {416.015f, 517.235f}, {418.545f, 513.692f}, {420.064f, 510.656f},
            {424.112f, 510.150f}, {424.112f, 510.150f}, {426.137f, 511.162f},
            {428.161f, 513.186f}, {429.173f, 516.729f}, {429.680f, 520.272f},
            {431.198f, 523.308f}, {431.198f, 527.357f}, {430.186f, 530.900f},
            {428.667f, 526.851f}, {428.667f, 524.826f}, {428.161f, 522.296f},
            {426.643f, 520.778f}, {425.125f, 519.259f}, {423.606f, 519.765f},
            {419.558f, 521.284f}, {418.545f, 524.320f}, {416.521f, 526.345f},
            {413.990f, 526.851f}, {411.460f, 527.357f}, {408.423f, 528.369f},
            {403.362f, 528.369f}, {401.844f, 530.900f}, {398.807f, 533.936f},
            {396.277f, 535.961f}, {394.759f, 537.479f}, {390.710f, 538.491f},
            {386.155f, 539.503f}, {382.106f, 539.503f}, {378.563f, 540.010f},
            {376.033f, 541.528f}, {375.527f, 542.540f}, {375.021f, 540.010f},
            {375.021f, 536.973f}, {374.515f, 533.430f}, {374.515f, 529.381f},
            {376.539f, 526.345f}, {378.057f, 522.802f}, {380.082f, 518.753f},
            {381.600f, 515.211f}, {383.624f, 511.668f}, {385.143f, 508.125f},
            {386.155f, 504.076f}, {386.661f, 501.546f}, {388.179f, 498.509f},
            {388.685f, 495.473f}, {388.685f, 492.942f}, {388.685f, 491.930f},
            {388.179f, 491.930f}, {387.673f, 489.906f}, {386.661f, 489.399f},
            {385.143f, 487.881f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.949f, 0.765f, 0.906f, 1.0f);
    }
    {
        static const float p[][2] = {
            {383.624f, 605.803f}, {386.155f, 604.790f}, {388.685f, 603.272f},
            {390.710f, 601.754f}, {393.240f, 600.742f}, {395.265f, 600.236f},
            {396.783f, 599.223f}, {400.832f, 597.199f}, {403.868f, 595.175f},
            {403.868f, 595.175f}, {407.917f, 593.656f}, {409.942f, 594.162f},
            {409.942f, 594.162f}, {411.460f, 596.187f}, {412.472f, 598.211f},
            {413.484f, 599.223f}, {417.027f, 599.729f}, {420.064f, 600.236f},
            {420.064f, 600.236f}, {422.088f, 602.766f}, {398.807f, 602.260f},
            {394.759f, 602.766f}, {390.710f, 603.272f}, {386.155f, 605.297f},
            {382.106f, 606.815f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.949f, 0.765f, 0.906f, 1.0f);
    }
    {
            static const float p[][2] = {
            {49.598f, 457.009f}, {51.622f, 455.491f}, {53.647f, 455.491f},
            {60.226f, 457.515f}, {62.250f, 460.046f}, {62.250f, 460.046f},
            {64.275f, 463.588f}, {64.275f, 467.131f}, {64.275f, 469.662f},
            {62.250f, 472.698f}, {60.732f, 475.735f}, {58.708f, 479.277f},
            {56.177f, 482.314f}, {54.659f, 485.351f}, {53.141f, 487.375f},
            {50.610f, 491.424f}, {49.092f, 495.473f}, {49.092f, 499.521f},
            {48.080f, 508.631f}, {48.080f, 514.198f}, {48.080f, 517.235f},
            {49.598f, 520.272f}, {50.104f, 522.296f}, {51.116f, 527.357f},
            {55.165f, 531.406f}, {58.708f, 534.949f}, {61.744f, 537.985f},
            {63.769f, 542.034f}, {64.781f, 544.058f}, {66.299f, 547.095f},
            {66.299f, 549.625f}, {66.299f, 552.156f}, {64.781f, 555.193f},
            {61.238f, 555.193f}, {58.202f, 555.193f}, {57.695f, 552.662f},
            {54.153f, 545.071f}, {49.598f, 530.900f}, {49.598f, 530.900f},
            {46.055f, 525.333f}, {43.525f, 521.790f}, {42.006f, 518.247f},
            {42.006f, 514.704f}, {41.500f, 512.174f}, {41.500f, 507.619f},
            {42.512f, 504.076f}, {43.019f, 499.521f}, {44.031f, 496.485f},
            {45.043f, 492.942f}, {48.586f, 483.326f}, {48.586f, 483.326f},
            {50.104f, 479.784f}, {51.622f, 477.253f}, {53.647f, 473.204f},
            {55.671f, 469.662f}, {58.202f, 466.625f}, {58.202f, 463.588f},
            {57.695f, 461.058f}, {56.177f, 459.539f}, {54.659f, 459.539f},
            {52.128f, 459.033f}, {50.104f, 459.033f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.949f, 0.765f, 0.906f, 1.0f);
    }
    {
        static const float p[][2] = {
            {8.098f, 451.948f}, {5.567f, 449.924f}, {8.098f, 447.899f},
            {12.146f, 446.887f}, {14.171f, 447.899f}, {16.195f, 449.417f},
            {18.220f, 452.454f}, {20.244f, 457.515f}, {21.256f, 463.082f},
            {21.762f, 469.155f}, {22.268f, 474.723f}, {24.799f, 479.784f},
            {26.823f, 482.314f}, {22.268f, 479.277f}, {20.244f, 477.253f},
            {18.220f, 472.192f}, {17.207f, 468.143f}, {16.701f, 466.119f},
            {16.701f, 462.070f}, {15.183f, 459.033f}, {13.665f, 455.997f},
            {11.134f, 453.972f}, {9.110f, 452.960f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.949f, 0.765f, 0.906f, 1.0f);
    }
    {
        static const float p[][2] = {
            {103.700f, 586.430f}, {116.964f, 590.047f}, {117.366f, 592.459f},
            {117.366f, 596.076f}, {116.562f, 599.694f}, {120.582f, 600.899f},
            {122.190f, 602.105f}, {125.003f, 604.517f}, {125.807f, 605.723f},
            {127.415f, 607.330f}, {129.424f, 609.340f}, {134.650f, 608.938f},
            {139.071f, 608.938f}, {142.287f, 610.546f}, {145.100f, 614.163f},
            {147.914f, 618.987f}, {150.727f, 623.810f}, {152.335f, 626.623f},
            {154.747f, 629.839f}, {157.560f, 631.447f}, {161.580f, 632.251f},
            {168.011f, 632.653f}, {170.020f, 632.251f}, {177.657f, 632.251f},
            {177.657f, 632.251f}, {183.284f, 633.858f}, {188.510f, 635.064f},
            {193.735f, 637.476f}, {197.754f, 639.887f}, {199.764f, 641.897f},
            {198.156f, 635.868f}, {195.744f, 632.251f}, {194.539f, 630.241f},
            {192.127f, 627.829f}, {190.519f, 626.222f}, {186.902f, 624.212f},
            {184.088f, 623.408f}, {177.657f, 622.202f}, {177.657f, 622.202f},
            {174.442f, 621.800f}, {170.824f, 621.800f}, {169.216f, 621.800f},
            {166.001f, 620.594f}, {163.187f, 619.389f}, {160.374f, 618.183f},
            {159.168f, 617.379f}, {157.560f, 616.173f}, {155.551f, 613.761f},
            {153.541f, 611.752f}, {151.129f, 608.938f}, {148.316f, 607.330f},
            {146.306f, 605.723f}, {144.296f, 604.115f}, {141.483f, 603.713f},
            {138.669f, 603.311f}, {137.061f, 602.105f}, {134.650f, 600.497f},
            {133.444f, 599.292f}, {132.238f, 597.282f}, {130.228f, 594.870f},
            {128.621f, 592.861f}, {127.817f, 591.655f}, {126.611f, 590.449f},
            {125.405f, 588.841f}, {123.797f, 587.233f}, {122.190f, 586.430f},
            {120.180f, 585.224f}, {118.170f, 585.224f}, {116.160f, 584.822f},
            {113.347f, 584.822f}, {110.935f, 584.822f}, {108.926f, 585.224f},
            {106.112f, 585.626f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.969, 0.82, 0.706, 1.0f);
    }
    {
        static const float p[][2] = {
            {72.751f, 504.032f}, {70.339f, 500.013f}, {69.134f, 499.209f},
            {68.732f, 497.199f}, {68.732f, 495.993f}, {75.163f, 496.797f},
            {78.780f, 498.807f}, {82.800f, 500.013f}, {86.417f, 500.013f},
            {92.044f, 500.816f}, {100.083f, 502.424f}, {106.514f, 503.228f},
            {108.524f, 504.032f}, {110.533f, 507.649f}, {112.543f, 510.865f},
            {115.759f, 515.688f}, {119.778f, 520.110f}, {121.386f, 523.325f},
            {125.405f, 527.746f}, {125.807f, 530.962f}, {127.013f, 536.187f},
            {127.817f, 539.403f}, {129.826f, 541.814f}, {131.434f, 543.824f},
            {135.052f, 545.432f}, {137.061f, 546.638f}, {141.483f, 546.236f},
            {143.090f, 546.236f}, {141.081f, 552.265f}, {141.885f, 552.667f},
            {143.090f, 556.284f}, {148.718f, 560.705f}, {152.737f, 563.117f},
            {152.737f, 563.117f}, {156.756f, 564.725f}, {160.776f, 565.931f},
            {162.785f, 567.136f}, {165.599f, 567.940f}, {168.011f, 569.548f},
            {169.618f, 571.156f}, {171.226f, 573.969f}, {172.834f, 575.979f},
            {173.236f, 578.391f}, {172.834f, 580.802f}, {171.628f, 581.204f},
            {168.011f, 580.802f}, {164.795f, 579.195f}, {157.560f, 574.773f},
            {157.560f, 574.773f}, {155.149f, 575.175f}, {150.325f, 574.773f},
            {144.698f, 573.166f}, {139.875f, 572.764f}, {132.238f, 572.362f},
            {126.611f, 571.156f}, {122.592f, 569.950f}, {118.572f, 568.342f},
            {114.151f, 565.529f}, {107.720f, 563.117f}, {102.896f, 562.715f},
            {100.083f, 562.715f}, {96.465f, 563.519f}, {93.250f, 563.921f},
            {90.838f, 563.519f}, {88.427f, 563.519f}, {90.034f, 562.313f},
            {91.642f, 561.911f}, {93.652f, 560.705f}, {95.260f, 559.902f},
            {96.867f, 559.500f}, {99.279f, 558.294f}, {102.495f, 557.088f},
            {104.906f, 557.088f}, {107.318f, 556.686f}, {108.926f, 556.686f},
            {111.337f, 557.490f}, {112.945f, 559.098f}, {114.955f, 560.705f},
            {117.366f, 563.519f}, {119.778f, 565.529f}, {122.190f, 566.734f},
            {124.601f, 567.538f}, {127.415f, 567.538f}, {130.228f, 567.136f},
            {131.032f, 566.333f}, {133.042f, 564.725f}, {134.248f, 562.715f},
            {134.248f, 559.902f}, {134.650f, 557.088f}, {134.650f, 555.882f},
            {134.650f, 553.471f}, {134.248f, 551.863f}, {133.042f, 550.255f},
            {132.640f, 549.049f}, {131.032f, 547.441f}, {129.023f, 545.432f},
            {128.219f, 544.226f}, {126.611f, 543.422f}, {124.601f, 541.412f},
            {122.993f, 540.608f}, {120.582f, 540.207f}, {116.964f, 538.599f},
            {113.749f, 538.599f}, {110.935f, 538.599f}, {108.524f, 538.599f},
            {105.710f, 538.599f}, {102.093f, 539.403f}, {97.671f, 538.599f},
            {93.250f, 538.197f}, {90.838f, 537.795f}, {88.025f, 537.795f},
            {85.613f, 536.589f}, {83.201f, 536.187f}, {80.790f, 535.383f},
            {79.182f, 534.579f}, {77.976f, 533.775f}, {76.368f, 532.168f},
            {75.163f, 530.158f}, {74.359f, 529.354f}, {73.957f, 527.746f},
            {73.555f, 525.737f}, {73.555f, 524.129f}, {75.163f, 522.521f},
            {77.574f, 522.521f}, {79.986f, 522.521f}, {81.594f, 522.923f},
            {84.005f, 524.129f}, {86.015f, 524.933f}, {88.829f, 526.541f},
            {91.642f, 527.746f}, {95.260f, 528.550f}, {98.073f, 529.354f},
            {100.887f, 529.756f}, {103.700f, 530.158f}, {106.916f, 530.158f},
            {108.926f, 529.756f}, {110.935f, 528.148f}, {112.543f, 526.139f},
            {114.151f, 524.531f}, {113.749f, 523.727f}, {113.749f, 522.119f},
            {112.945f, 519.708f}, {111.739f, 516.894f}, {110.131f, 514.884f},
            {108.926f, 514.080f}, {107.720f, 513.277f}, {105.710f, 511.669f},
            {102.896f, 510.061f}, {100.485f, 508.453f}, {97.671f, 506.846f},
            {95.260f, 506.042f}, {93.250f, 505.238f}, {90.034f, 504.836f},
            {88.025f, 504.434f}, {83.201f, 504.836f}, {79.182f, 504.836f},
            {76.368f, 504.836f}, {73.957f, 504.836f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.82, 0.353, 0.529, 1.0f);
    }
}

//! =================   RIGHT GREEN LEAF  =================
static void drawLeafBone(const float pts[][2], int n, float centerR, float centerG, float centerB, 
                         float edgeR, float edgeG, float edgeB, float alpha, float coreWidth, float totalWidth){
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    glLineWidth(totalWidth);
    glColor4f(edgeR, edgeG, edgeB, alpha * 0.3f);
    glBegin(GL_LINE_STRIP);
    for(int i = 0; i < n; i++) {
        glVertex2f(pts[i][0], pts[i][1]);
    }
    glEnd();
    
    glLineWidth(totalWidth * 0.6f);
    glColor4f(edgeR, edgeG, edgeB, alpha * 0.6f);
    glBegin(GL_LINE_STRIP);
    for(int i = 0; i < n; i++) {
        glVertex2f(pts[i][0], pts[i][1]);
    }
    glEnd();
    
    glLineWidth(coreWidth);
    glColor4f(centerR, centerG, centerB, alpha);
    glBegin(GL_LINE_STRIP);
    for(int i = 0; i < n; i++) {
        glVertex2f(pts[i][0], pts[i][1]);
    }
    glEnd();
    
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
}
void drawRightLeaf(){
    {
        static const float p[][2] = {
            {962.075f, 661.952f}, {961.429f, 660.821f}, {961.106f, 659.852f},
            {960.944f, 659.044f}, {960.944f, 658.398f}, {961.267f, 657.429f},
            {961.591f, 656.783f}, {961.914f, 656.137f}, {962.560f, 655.329f},
            {963.367f, 654.683f}, {964.014f, 654.198f}, {964.660f, 653.552f},
            {965.306f, 653.067f}, {966.275f, 652.744f}, {967.083f, 652.583f},
            {967.890f, 652.421f}, {968.622f, 652.198f}, {969.276f, 652.104f},
            {969.930f, 652.011f}, {970.772f, 651.824f}, {972.080f, 651.730f},
            {973.109f, 651.730f}, {974.043f, 651.637f}, {975.072f, 651.450f},
            {976.287f, 651.450f}, {977.315f, 651.356f}, {978.250f, 651.356f},
            {979.185f, 651.356f}, {979.932f, 651.356f}, {980.680f, 651.356f},
            {981.615f, 651.263f}, {982.550f, 651.169f}, {983.391f, 651.169f},
            {983.859f, 651.076f}, {984.513f, 651.076f}, {985.261f, 650.983f},
            {986.196f, 650.889f}, {986.850f, 650.889f}, {987.691f, 650.889f},
            {988.346f, 650.796f}, {989.093f, 650.702f}, {989.841f, 650.609f},
            {990.589f, 650.609f}, {1047.143f, 648.085f}, {1048.265f, 648.178f},
            {1049.387f, 648.272f}, {1050.415f, 648.459f}, {1051.817f, 648.459f},
            {1053.032f, 648.459f}, {1053.780f, 648.552f}, {1054.528f, 648.646f},
            {1055.930f, 648.833f}, {1057.146f, 648.926f}, {1058.267f, 649.019f},
            {1059.950f, 649.206f}, {1061.165f, 649.674f}, {1062.287f, 649.674f},
            {1063.409f, 649.861f}, {1064.717f, 650.048f}, {1065.559f, 649.954f},
            {1066.587f, 649.954f}, {1067.709f, 649.861f}, {1069.298f, 649.861f},
            {1073.691f, 649.674f}, {1079.300f, 649.487f}, {1087.993f, 649.393f},
            {1144.735f, 644.346f}, {1147.601f, 644.377f}, {1149.938f, 644.455f},
            {1152.197f, 644.377f}, {1154.301f, 644.299f}, {1156.482f, 644.533f},
            {1157.728f, 644.688f}, {1158.429f, 644.844f}, {1159.442f, 645.000f},
            {1160.221f, 645.000f}, {1161.234f, 645.000f}, {1162.324f, 645.000f},
            {1163.181f, 645.000f}, {1163.882f, 645.078f}, {1164.895f, 645.156f},
            {1166.063f, 645.234f}, {1167.154f, 645.234f}, {1168.167f, 645.312f},
            {1169.569f, 645.467f}, {1210.621f, 645.156f}, {1211.322f, 645.234f},
            {1211.946f, 645.234f}, {1212.491f, 645.389f}, {1213.348f, 645.467f},
            {1213.893f, 645.623f}, {1214.672f, 645.701f}, {1215.139f, 645.779f},
            {1215.918f, 645.935f}, {1216.697f, 646.168f}, {1217.398f, 646.324f},
            {1218.099f, 646.324f}, {1218.567f, 646.636f}, {1219.034f, 646.714f},
            {1219.580f, 646.869f}, {1220.203f, 646.947f}, {1221.138f, 647.181f},
            {1221.683f, 647.415f}, {1221.994f, 647.493f}, {1222.462f, 647.726f},
            {1223.319f, 647.882f}, {1224.020f, 648.116f}, {1224.643f, 648.583f},
            {1225.500f, 649.051f}, {1226.123f, 649.440f}, {1226.590f, 649.752f},
            {1227.136f, 650.531f}, {1227.447f, 651.076f}, {1227.681f, 651.699f},
            {1227.837f, 652.400f}, {1227.837f, 653.023f}, {1227.759f, 654.036f},
            {1227.525f, 654.893f}, {1226.902f, 655.984f}, {1226.201f, 656.685f},
            {1225.500f, 657.152f}, {1224.487f, 657.931f}, {1223.319f, 658.243f},
            {1222.306f, 658.710f}, {1221.761f, 658.788f}, {1220.748f, 659.022f},
            {1220.047f, 659.333f}, {1218.567f, 659.645f}, {1217.165f, 659.645f},
            {1215.295f, 659.956f}, {1213.971f, 659.956f}, {1212.101f, 660.112f},
            {1209.453f, 660.268f}, {1205.792f, 660.346f}, {1200.884f, 660.502f},
            {1197.768f, 660.502f}, {1192.549f, 660.735f}, {1189.121f, 660.813f},
            {1180.864f, 661.748f}, {1179.618f, 661.982f}, {1179.072f, 662.060f},
            {1178.215f, 662.293f}, {1178.060f, 662.449f}, {1177.514f, 663.072f},
            {1177.125f, 663.773f}, {1176.891f, 664.475f}, {1176.190f, 665.721f},
            {1175.567f, 666.344f}, {1174.866f, 667.045f}, {1174.009f, 667.668f},
            {1172.840f, 668.214f}, {1171.750f, 668.915f}, {1170.893f, 669.226f},
            {1169.724f, 669.694f}, {1168.478f, 669.927f}, {1167.154f, 670.239f},
            {1165.985f, 670.395f}, {1164.661f, 670.551f}, {1163.882f, 670.706f},
            {1162.714f, 670.940f}, {1161.623f, 671.330f}, {1160.688f, 671.641f},
            {1160.065f, 672.031f}, {1159.208f, 672.264f}, {1158.273f, 672.498f},
            {1157.183f, 672.654f}, {1156.482f, 672.810f}, {1155.781f, 672.965f},
            {1154.690f, 673.121f}, {1153.911f, 673.277f}, {1152.821f, 673.277f},
            {1151.652f, 673.277f}, {1150.717f, 673.511f}, {1149.705f, 673.744f},
            {1148.458f, 673.900f}, {1147.679f, 674.056f}, {1146.355f, 674.212f},
            {1145.186f, 674.368f}, {1144.096f, 674.523f}, {1142.538f, 674.601f},
            {1140.980f, 674.679f}, {1139.500f, 674.835f}, {1130.308f, 675.770f},
            {1121.505f, 676.393f}, {1108.652f, 676.705f}, {1087.931f, 677.406f},
            {1063.315f, 678.340f}, {1058.563f, 678.263f}, {1057.395f, 678.263f},
            {1055.993f, 677.873f}, {1054.590f, 677.639f}, {1054.045f, 677.406f},
            {1052.487f, 677.016f}, {1050.773f, 676.627f}, {1049.839f, 676.237f},
            {1048.904f, 675.926f}, {1047.891f, 675.614f}, {1047.034f, 675.458f},
            {1045.866f, 674.991f}, {1044.775f, 674.446f}, {1043.763f, 674.134f},
            {1042.516f, 673.589f}, {1041.737f, 673.277f}, {1041.036f, 672.888f},
            {1040.257f, 672.498f}, {1039.556f, 671.953f}, {1039.089f, 671.641f},
            {1038.543f, 671.174f}, {1038.154f, 670.862f}, {1037.998f, 670.551f},
            {1037.764f, 670.239f}, {1037.686f, 669.927f}, {1037.764f, 669.304f},
            {1038.154f, 668.681f}, {1038.388f, 668.369f}, {1038.855f, 667.980f},
            {1039.634f, 667.590f}, {1040.023f, 667.123f}, {1040.413f, 666.889f},
            {1040.958f, 666.188f}, {1041.192f, 665.721f}, {1041.114f, 665.176f},
            {1040.958f, 664.864f}, {1040.179f, 664.630f}, {1038.543f, 664.630f},
            {1037.219f, 664.552f}, {1035.739f, 664.552f}, {1034.337f, 664.786f},
            {1031.922f, 665.020f}, {1029.975f, 665.020f}, {1027.404f, 664.942f},
            {1025.067f, 665.176f}, {1022.652f, 665.254f}, {1020.627f, 665.487f},
            {1017.589f, 665.643f}, {1009.487f, 666.578f}, {1008.007f, 666.889f},
            {1007.306f, 666.889f}, {1006.683f, 667.045f}, {1005.904f, 667.123f},
            {1005.125f, 667.123f}, {1004.580f, 666.967f}, {1004.112f, 666.967f},
            {1003.022f, 666.812f}, {1001.619f, 666.812f}, {1000.451f, 666.578f},
            {999.049f, 666.578f}, {997.569f, 666.500f}, {996.556f, 666.500f},
            {995.777f, 666.422f}, {994.764f, 666.266f}, {993.440f, 666.110f},
            {992.583f, 665.721f}, {991.648f, 665.721f}, {990.714f, 665.643f},
            {989.857f, 665.721f}, {988.844f, 665.799f}, {988.221f, 665.799f},
            {987.442f, 665.799f}, {986.585f, 665.721f}, {985.572f, 665.799f},
            {984.793f, 665.877f}, {984.248f, 665.955f}, {983.547f, 666.033f},
            {982.846f, 666.110f}, {982.223f, 666.110f}, {981.833f, 666.110f},
            {980.509f, 666.033f}, {979.107f, 666.110f}, {977.782f, 666.110f},
            {976.614f, 666.110f}, {975.368f, 666.188f}, {974.355f, 666.110f},
            {973.576f, 666.110f}, {972.407f, 666.033f}, {971.083f, 665.955f},
            {969.993f, 666.033f}, {969.292f, 665.955f}, {968.590f, 665.799f},
            {967.656f, 665.565f}, {967.188f, 665.487f}, {966.565f, 665.098f},
            {965.786f, 664.864f}, {965.085f, 664.475f}, {964.618f, 664.163f},
            {964.150f, 663.773f}, {963.683f, 663.462f}, {963.138f, 662.994f},
            {962.670f, 662.683f}, {962.281f, 662.293f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.761, 0.647, 0.498, 1.0f);
    }

    //? =================   LEFT GREEN LEAF  =================

    //?     Right 1st Leaft
    {   
        static const float p[][2] = {
            {1184.381f, 580.802f}, {1184.716f, 579.463f}, {1185.051f, 577.118f},
            {1185.721f, 574.773f}, {1186.056f, 573.433f}, {1187.061f, 570.419f},
            {1188.066f, 567.069f}, {1189.071f, 564.390f}, {1190.075f, 562.045f},
            {1191.415f, 559.366f}, {1192.755f, 556.686f}, {1193.425f, 554.676f},
            {1194.430f, 553.002f}, {1195.435f, 550.322f}, {1196.774f, 547.977f},
            {1197.779f, 545.968f}, {1198.784f, 544.293f}, {1199.454f, 542.283f},
            {1200.124f, 540.608f}, {1200.794f, 538.934f}, {1202.134f, 536.589f},
            {1203.138f, 534.579f}, {1203.808f, 532.905f}, {1204.813f, 530.560f},
            {1205.483f, 528.550f}, {1206.153f, 527.545f}, {1206.823f, 524.866f},
            {1207.493f, 522.856f}, {1208.498f, 520.846f}, {1209.168f, 518.167f},
            {1210.172f, 516.157f}, {1211.177f, 513.812f}, {1211.847f, 512.473f},
            {1212.852f, 509.458f}, {1214.192f, 507.783f}, {1214.862f, 506.109f},
            {1215.532f, 504.099f}, {1216.536f, 502.424f}, {1217.206f, 500.080f},
            {1218.211f, 497.735f}, {1218.546f, 495.725f}, {1218.881f, 494.050f},
            {1218.881f, 491.371f}, {1219.216f, 488.356f}, {1218.881f, 486.012f},
            {1218.881f, 483.667f}, {1218.211f, 480.987f}, {1217.541f, 478.978f},
            {1217.206f, 476.968f}, {1216.536f, 475.293f}, {1216.202f, 473.619f},
            {1215.867f, 471.944f}, {1215.532f, 470.604f}, {1215.532f, 468.929f},
            {1216.536f, 467.924f}, {1218.211f, 467.255f}, {1219.551f, 466.250f},
            {1221.226f, 465.245f}, {1221.896f, 464.240f}, {1222.231f, 462.230f},
            {1221.896f, 460.890f}, {1221.561f, 458.881f}, {1220.556f, 457.206f},
            {1220.221f, 455.196f}, {1219.216f, 453.187f}, {1218.211f, 450.842f},
            {1217.541f, 448.832f}, {1216.536f, 446.153f}, {1215.532f, 443.808f},
            {1215.532f, 441.128f}, {1215.197f, 439.119f}, {1214.862f, 437.444f},
            {1214.527f, 435.099f}, {1214.527f, 432.420f}, {1214.527f, 430.075f},
            {1215.197f, 427.730f}, {1215.532f, 426.056f}, {1216.202f, 423.711f},
            {1217.876f, 422.706f}, {1218.881f, 421.031f}, {1219.216f, 419.022f},
            {1220.221f, 417.347f}, {1220.556f, 414.667f}, {1220.556f, 411.653f},
            {1220.556f, 409.978f}, {1220.556f, 407.299f}, {1220.891f, 405.959f},
            {1221.561f, 400.935f}, {1221.896f, 398.255f}, {1222.231f, 395.910f},
            {1223.235f, 392.896f}, {1223.570f, 390.551f}, {1223.905f, 387.871f},
            {1224.575f, 386.532f}, {1224.910f, 384.187f}, {1225.580f, 381.842f},
            {1226.250f, 379.163f}, {1227.255f, 376.818f}, {1227.925f, 374.808f},
            {1228.595f, 372.799f}, {1229.934f, 371.124f}, {1231.274f, 369.449f},
            {1231.944f, 367.775f}, {1232.614f, 366.435f}, {1233.954f, 365.095f},
            {1234.959f, 363.420f}, {1235.964f, 361.410f}, {1237.303f, 359.736f},
            {1238.308f, 358.061f}, {1239.648f, 356.386f}, {1240.318f, 355.046f},
            {1241.323f, 353.707f}, {1242.328f, 352.032f}, {1243.667f, 351.027f},
            {1244.672f, 349.352f}, {1247.352f, 347.008f}, {1248.357f, 346.003f},
            {1250.031f, 344.663f}, {1250.701f, 344.328f}, {1251.036f, 344.663f},
            {1250.031f, 346.673f}, {1248.692f, 348.347f}, {1247.352f, 351.027f},
            {1246.012f, 353.037f}, {1245.007f, 355.381f}, {1244.002f, 357.391f},
            {1242.663f, 360.071f}, {1241.323f, 362.750f}, {1239.983f, 365.765f},
            {1238.978f, 369.449f}, {1237.973f, 372.129f}, {1237.303f, 374.808f},
            {1236.633f, 377.823f}, {1235.964f, 380.503f}, {1234.959f, 383.182f},
            {1234.289f, 385.192f}, {1233.284f, 387.871f}, {1232.949f, 391.221f},
            {1232.614f, 393.901f}, {1231.609f, 397.250f}, {1231.609f, 400.935f},
            {1231.274f, 403.614f}, {1230.939f, 406.294f}, {1231.274f, 409.643f},
            {1231.274f, 412.323f}, {1231.274f, 414.667f}, {1231.274f, 416.677f},
            {1231.274f, 419.357f}, {1231.609f, 422.371f}, {1231.944f, 425.386f},
            {1232.614f, 428.400f}, {1232.949f, 430.075f}, {1233.619f, 432.755f},
            {1234.289f, 434.764f}, {1235.294f, 436.774f}, {1235.629f, 438.784f},
            {1236.298f, 440.793f}, {1236.633f, 443.473f}, {1237.973f, 445.818f},
            {1238.643f, 448.497f}, {1238.978f, 450.842f}, {1239.983f, 452.852f},
            {1240.318f, 455.196f}, {1240.988f, 457.541f}, {1241.323f, 460.221f},
            {1241.993f, 462.900f}, {1241.993f, 464.575f}, {1242.663f, 466.250f},
            {1243.332f, 468.929f}, {1243.332f, 471.274f}, {1242.997f, 473.284f},
            {1242.997f, 475.293f}, {1242.663f, 478.308f}, {1242.328f, 479.983f},
            {1242.328f, 482.997f}, {1241.323f, 487.686f}, {1240.988f, 489.696f},
            {1240.318f, 493.046f}, {1239.313f, 496.060f}, {1238.308f, 498.405f},
            {1237.638f, 500.415f}, {1236.298f, 502.424f}, {1235.629f, 505.439f},
            {1234.624f, 507.448f}, {1233.284f, 510.128f}, {1232.614f, 512.138f},
            {1231.944f, 514.147f}, {1230.604f, 516.157f}, {1229.599f, 517.832f},
            {1228.595f, 519.507f}, {1227.590f, 521.851f}, {1226.250f, 523.526f},
            {1224.575f, 526.206f}, {1222.566f, 528.550f}, {1220.891f, 530.895f},
            {1219.551f, 533.240f}, {1216.202f, 536.589f}, {1213.522f, 539.939f},
            {1211.512f, 542.618f}, {1209.503f, 544.628f}, {1207.493f, 547.307f},
            {1204.813f, 549.987f}, {1203.138f, 552.332f}, {1201.129f, 554.006f},
            {1199.454f, 556.686f}, {1197.109f, 559.366f}, {1195.435f, 560.705f},
            {1193.425f, 563.720f}, {1191.750f, 566.400f}, {1190.745f, 568.744f},
            {1188.736f, 571.424f}, {1187.396f, 574.103f}, {1186.391f, 576.448f},
            {1185.386f, 578.458f}, {1184.716f, 579.798f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineStrip(p, n, 0.027, 0.451, 0.153, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.035, 0.58, 0.2, 1.0f);
    }
    {
        static const float p[][2] = {
            {1192.085f, 639.753f}, {1191.415f, 637.074f}, {1190.745f, 634.729f},
            {1189.741f, 632.720f}, {1189.406f, 629.705f}, {1189.071f, 626.021f},
            {1188.736f, 624.346f}, {1188.066f, 622.001f}, {1187.731f, 617.647f},
            {1187.731f, 614.967f}, {1187.731f, 614.967f}, {1187.731f, 612.288f},
            {1188.066f, 609.608f}, {1188.066f, 607.263f}, {1188.736f, 602.239f},
            {1188.736f, 602.239f}, {1189.406f, 599.225f}, {1190.745f, 597.550f},
            {1191.415f, 595.540f}, {1192.420f, 593.530f}, {1193.090f, 592.191f},
            {1194.095f, 590.181f}, {1195.435f, 588.171f}, {1197.109f, 585.492f},
            {1198.784f, 583.147f}, {1200.124f, 581.807f}, {1201.464f, 580.132f},
            {1202.804f, 578.793f}, {1204.143f, 576.448f}, {1205.483f, 574.103f},
            {1206.823f, 572.764f}, {1207.828f, 571.424f}, {1208.833f, 570.084f},
            {1209.837f, 568.744f}, {1210.842f, 566.734f}, {1212.182f, 564.390f},
            {1213.522f, 562.380f}, {1214.862f, 560.036f}, {1215.867f, 558.026f},
            {1216.536f, 556.016f}, {1216.871f, 555.011f}, {1218.211f, 557.691f},
            {1220.221f, 561.375f}, {1221.226f, 564.725f}, {1221.896f, 568.744f},
            {1222.231f, 572.094f}, {1222.566f, 575.443f}, {1222.231f, 578.458f},
            {1221.561f, 581.472f}, {1220.556f, 583.817f}, {1219.886f, 585.492f},
            {1218.546f, 587.501f}, {1217.541f, 589.511f}, {1215.867f, 591.186f},
            {1214.192f, 593.530f}, {1212.517f, 595.540f}, {1210.507f, 597.215f},
            {1208.833f, 598.555f}, {1207.158f, 600.899f}, {1205.483f, 603.244f},
            {1203.473f, 606.259f}, {1201.799f, 608.938f}, {1200.459f, 611.618f},
            {1198.784f, 613.962f}, {1197.444f, 616.977f}, {1196.105f, 619.991f},
            {1195.100f, 623.006f}, {1194.430f, 625.686f}, {1194.095f, 627.025f},
            {1193.760f, 629.035f}, {1193.090f, 631.380f}, {1192.755f, 633.054f},
            {1192.755f, 634.729f}, {1192.420f, 636.404f}, {1192.420f, 638.414f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineStrip(p, n, 0.027, 0.451, 0.153, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.035, 0.58, 0.2, 1.0f);
    }
    {
        static const float p[][2] = {
            {1193.090f, 639.419f}, {1193.090f, 635.734f}, {1193.090f, 632.720f},
            {1193.425f, 630.040f}, {1194.430f, 627.025f}, {1195.100f, 624.681f},
            {1196.105f, 622.671f}, {1196.439f, 619.991f}, {1197.779f, 617.312f},
            {1198.784f, 615.302f}, {1200.124f, 612.623f}, {1201.799f, 609.943f},
            {1203.138f, 607.933f}, {1204.143f, 606.259f}, {1205.148f, 604.584f},
            {1205.818f, 602.574f}, {1207.158f, 601.234f}, {1209.168f, 599.894f},
            {1210.172f, 598.555f}, {1211.512f, 597.215f}, {1212.517f, 596.210f},
            {1214.192f, 594.535f}, {1216.202f, 592.526f}, {1217.876f, 590.516f},
            {1219.216f, 588.506f}, {1220.221f, 586.162f}, {1220.891f, 583.482f},
            {1221.896f, 580.802f}, {1222.231f, 578.793f}, {1222.566f, 577.118f},
            {1222.901f, 574.438f}, {1222.901f, 572.429f}, {1222.566f, 571.424f},
            {1221.896f, 569.079f}, {1221.226f, 567.069f}, {1220.891f, 565.060f},
            {1220.556f, 563.050f}, {1220.221f, 561.040f}, {1219.551f, 560.036f},
            {1218.881f, 558.026f}, {1217.876f, 556.016f}, {1217.206f, 554.006f},
            {1216.536f, 551.997f}, {1216.202f, 549.987f}, {1215.532f, 547.977f},
            {1214.862f, 545.968f}, {1214.192f, 543.623f}, {1213.857f, 541.613f},
            {1219.886f, 534.244f}, {1220.891f, 538.264f}, {1221.896f, 540.943f},
            {1222.901f, 543.288f}, {1224.575f, 546.638f}, {1225.245f, 548.982f},
            {1226.920f, 551.997f}, {1227.925f, 554.676f}, {1228.930f, 557.691f},
            {1230.269f, 560.705f}, {1231.274f, 562.715f}, {1232.279f, 565.060f},
            {1232.949f, 567.739f}, {1234.289f, 571.089f}, {1234.624f, 573.768f},
            {1234.959f, 575.778f}, {1235.294f, 578.458f}, {1235.629f, 579.798f},
            {1235.294f, 581.807f}, {1235.294f, 585.157f}, {1235.294f, 588.841f},
            {1234.959f, 591.186f}, {1234.289f, 592.861f}, {1233.954f, 594.870f},
            {1232.949f, 597.550f}, {1231.944f, 599.225f}, {1230.604f, 600.899f},
            {1228.595f, 602.909f}, {1226.585f, 604.249f}, {1224.910f, 605.924f},
            {1222.901f, 607.263f}, {1221.226f, 608.603f}, {1219.886f, 609.943f},
            {1218.211f, 611.283f}, {1214.527f, 614.297f}, {1213.522f, 614.967f},
            {1212.182f, 615.637f}, {1210.842f, 616.977f}, {1209.503f, 617.647f},
            {1208.833f, 618.987f}, {1206.823f, 620.661f}, {1205.483f, 622.671f},
            {1203.808f, 624.681f}, {1202.134f, 626.690f}, {1201.464f, 628.030f},
            {1200.124f, 630.710f}, {1199.119f, 632.050f}, {1198.449f, 633.724f},
            {1197.444f, 636.069f}, {1196.105f, 637.744f}, {1194.430f, 638.414f},
            {1193.760f, 640.088f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineStrip(p, n, 0.027, 0.451, 0.153, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.035, 0.58, 0.2, 1.0f);
    }
    {
        static const float p[][2] = {
            {1184.716f, 641.093f}, {1182.372f, 639.084f}, {1181.032f, 638.079f},
            {1179.692f, 636.739f}, {1178.687f, 635.734f}, {1177.347f, 634.394f},
            {1176.343f, 633.389f}, {1175.338f, 632.050f}, {1174.333f, 630.375f},
            {1173.328f, 629.035f}, {1171.988f, 627.360f}, {1170.983f, 626.021f},
            {1169.644f, 624.011f}, {1168.639f, 622.336f}, {1167.634f, 620.661f},
            {1166.964f, 618.652f}, {1166.294f, 617.312f}, {1165.624f, 615.302f},
            {1165.289f, 613.627f}, {1164.619f, 611.618f}, {1164.284f, 609.273f},
            {1163.949f, 606.928f}, {1163.614f, 604.249f}, {1163.614f, 601.569f},
            {1164.284f, 599.894f}, {1171.653f, 569.414f}, {1171.988f, 567.739f},
            {1172.323f, 565.060f}, {1172.658f, 563.720f}, {1172.993f, 562.380f},
            {1172.993f, 561.710f}, {1173.328f, 559.701f}, {1173.663f, 558.026f},
            {1174.333f, 556.351f}, {1175.003f, 554.341f}, {1175.673f, 552.667f},
            {1176.677f, 549.987f}, {1178.017f, 547.642f}, {1179.022f, 545.968f},
            {1180.027f, 543.958f}, {1181.032f, 541.613f}, {1181.702f, 540.273f},
            {1182.372f, 538.934f}, {1183.711f, 537.259f}, {1184.716f, 535.584f},
            {1186.391f, 533.240f}, {1187.396f, 531.565f}, {1188.066f, 530.560f},
            {1188.736f, 529.220f}, {1189.741f, 527.545f}, {1190.745f, 525.871f},
            {1191.750f, 523.861f}, {1192.085f, 522.521f}, {1192.420f, 519.842f},
            {1192.755f, 517.832f}, {1193.090f, 515.487f}, {1193.090f, 513.812f},
            {1193.090f, 511.803f}, {1193.090f, 509.793f}, {1192.755f, 508.453f},
            {1192.085f, 503.764f}, {1191.750f, 501.419f}, {1191.080f, 500.415f},
            {1190.410f, 498.740f}, {1189.071f, 496.060f}, {1188.066f, 495.055f},
            {1187.396f, 492.376f}, {1187.396f, 492.376f}, {1186.726f, 490.366f},
            {1174.333f, 429.405f}, {1177.012f, 413.328f}, {1176.677f, 410.983f},
            {1176.677f, 409.308f}, {1176.677f, 406.964f}, {1176.677f, 404.619f},
            {1177.012f, 402.274f}, {1177.012f, 400.265f}, {1177.347f, 397.585f},
            {1177.012f, 395.240f}, {1177.347f, 393.231f}, {1177.682f, 391.221f},
            {1177.682f, 389.211f}, {1178.352f, 387.202f}, {1178.352f, 385.527f},
            {1179.022f, 383.852f}, {1179.357f, 382.177f}, {1180.027f, 380.168f},
            {1180.697f, 377.488f}, {1181.367f, 375.143f}, {1182.037f, 373.134f},
            {1182.707f, 371.794f}, {1183.376f, 370.119f}, {1184.381f, 368.109f},
            {1185.386f, 365.430f}, {1186.391f, 363.420f}, {1187.731f, 361.410f},
            {1188.736f, 360.071f}, {1190.075f, 358.061f}, {1191.415f, 356.051f},
            {1192.755f, 354.042f}, {1194.095f, 352.032f}, {1195.435f, 349.687f},
            {1197.444f, 347.678f}, {1199.454f, 346.003f}, {1200.794f, 344.328f},
            {1202.134f, 342.988f}, {1203.473f, 341.314f}, {1205.148f, 339.974f},
            {1205.148f, 341.648f}, {1204.478f, 342.988f}, {1203.808f, 344.328f},
            {1202.804f, 346.003f}, {1201.799f, 347.343f}, {1200.794f, 348.347f},
            {1199.454f, 350.357f}, {1198.449f, 351.697f}, {1197.109f, 354.042f},
            {1196.439f, 356.051f}, {1196.105f, 357.391f}, {1195.100f, 359.736f},
            {1194.095f, 362.080f}, {1193.425f, 364.425f}, {1192.420f, 367.105f},
            {1191.750f, 368.444f}, {1191.750f, 369.784f}, {1191.415f, 371.794f},
            {1191.080f, 374.139f}, {1191.080f, 376.483f}, {1190.410f, 378.158f},
            {1190.410f, 380.503f}, {1190.410f, 383.517f}, {1191.080f, 387.537f},
            {1191.415f, 390.551f}, {1191.415f, 391.556f}, {1190.075f, 448.832f},
            {1194.095f, 454.861f}, {1194.765f, 457.876f}, {1195.435f, 460.221f},
            {1196.774f, 463.235f}, {1198.449f, 466.920f}, {1199.789f, 470.604f},
            {1201.799f, 473.953f}, {1203.473f, 477.638f}, {1204.478f, 479.983f},
            {1205.483f, 482.327f}, {1205.818f, 485.007f}, {1206.488f, 486.682f},
            {1206.823f, 488.021f}, {1207.158f, 490.366f}, {1208.163f, 493.381f},
            {1208.498f, 496.395f}, {1208.498f, 499.075f}, {1208.833f, 501.754f},
            {1209.168f, 504.769f}, {1209.168f, 507.448f}, {1209.168f, 509.793f},
            {1208.833f, 513.478f}, {1208.498f, 516.492f}, {1208.163f, 519.842f},
            {1207.493f, 523.526f}, {1206.153f, 527.210f}, {1205.148f, 530.225f},
            {1204.143f, 533.575f}, {1202.804f, 536.924f}, {1201.464f, 540.273f},
            {1200.124f, 543.958f}, {1198.114f, 547.642f}, {1196.105f, 551.327f},
            {1194.765f, 554.341f}, {1193.425f, 556.686f}, {1191.080f, 559.366f},
            {1190.075f, 561.710f}, {1188.736f, 564.055f}, {1187.396f, 566.734f},
            {1186.056f, 568.409f}, {1184.381f, 572.094f}, {1183.042f, 575.443f},
            {1182.372f, 577.788f}, {1182.037f, 580.132f}, {1181.702f, 581.472f},
            {1181.367f, 583.817f}, {1181.032f, 587.501f}, {1180.362f, 590.516f},
            {1180.027f, 594.200f}, {1180.027f, 598.220f}, {1180.027f, 602.574f},
            {1181.032f, 606.259f}, {1181.032f, 609.943f}, {1182.037f, 613.292f},
            {1182.037f, 615.637f}, {1182.707f, 618.652f}, {1183.711f, 622.336f},
            {1184.716f, 625.016f}, {1185.721f, 628.030f}, {1186.726f, 630.375f},
            {1188.066f, 632.385f}, {1189.071f, 635.064f}, {1190.075f, 637.409f},
            {1191.415f, 639.753f}, {1191.415f, 640.758f}, {1185.386f, 641.763f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineStrip(p, n, 0.027, 0.451, 0.153, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.035, 0.58, 0.2, 1.0f);
    }
    {
        static const float p[][2] = {
            {993.218f, 650.593f}, {992.753f, 649.662f}, {991.823f, 648.499f},
            {990.427f, 647.336f}, {989.031f, 647.104f}, {987.171f, 647.104f},
            {981.123f, 646.871f}, {977.401f, 646.406f}, {977.401f, 646.406f},
            {974.377f, 645.010f}, {972.051f, 644.080f}, {970.190f, 643.382f},
            {968.795f, 642.219f}, {966.934f, 641.521f}, {965.538f, 640.126f},
            {964.608f, 639.428f}, {963.212f, 638.265f}, {962.282f, 637.102f},
            {961.351f, 636.171f}, {960.421f, 634.311f}, {959.025f, 632.217f},
            {958.095f, 629.426f}, {957.630f, 627.565f}, {956.932f, 625.472f},
            {956.467f, 624.076f}, {956.002f, 622.215f}, {955.769f, 620.587f},
            {955.304f, 618.726f}, {955.071f, 616.167f}, {955.071f, 614.074f},
            {954.606f, 612.678f}, {954.606f, 610.818f}, {954.141f, 608.724f},
            {953.676f, 606.863f}, {953.443f, 604.537f}, {952.745f, 602.444f},
            {951.815f, 600.583f}, {950.419f, 599.420f}, {949.256f, 598.257f},
            {947.395f, 596.861f}, {945.302f, 595.931f}, {943.906f, 595.466f},
            {941.580f, 594.303f}, {940.185f, 593.140f}, {938.789f, 592.209f},
            {937.859f, 591.279f}, {936.695f, 589.883f}, {935.765f, 587.790f},
            {934.835f, 586.162f}, {934.369f, 584.301f}, {934.137f, 582.905f},
            {933.904f, 581.044f}, {933.904f, 579.416f}, {933.439f, 577.788f},
            {933.206f, 576.392f}, {932.741f, 574.997f}, {932.509f, 572.903f},
            {931.578f, 571.275f}, {930.880f, 569.879f}, {928.787f, 566.855f},
            {928.787f, 566.855f}, {926.926f, 564.995f}, {925.763f, 564.064f},
            {924.600f, 563.832f}, {923.437f, 562.901f}, {921.809f, 561.971f},
            {919.948f, 560.808f}, {918.785f, 560.343f}, {917.855f, 560.110f},
            {916.226f, 559.412f}, {914.831f, 558.947f}, {913.203f, 558.482f},
            {911.807f, 558.017f}, {909.713f, 558.017f}, {907.853f, 557.551f},
            {905.992f, 557.551f}, {906.457f, 556.853f}, {908.318f, 556.156f},
            {910.411f, 555.923f}, {912.737f, 555.690f}, {914.831f, 555.225f},
            {916.924f, 554.993f}, {918.087f, 554.993f}, {920.181f, 555.458f},
            {921.809f, 555.690f}, {923.204f, 555.923f}, {925.065f, 556.156f},
            {926.461f, 556.621f}, {927.857f, 557.319f}, {929.252f, 558.482f},
            {930.648f, 559.180f}, {932.276f, 560.808f}, {933.904f, 562.436f},
            {937.161f, 566.855f}, {937.161f, 566.855f}, {938.789f, 569.647f},
            {940.185f, 571.740f}, {941.115f, 573.834f}, {942.278f, 575.694f},
            {943.441f, 577.788f}, {944.604f, 579.416f}, {945.302f, 580.579f},
            {946.465f, 582.207f}, {948.093f, 583.836f}, {949.256f, 584.533f},
            {951.117f, 585.464f}, {952.745f, 586.162f}, {954.606f, 586.859f},
            {956.932f, 586.859f}, {959.491f, 587.557f}, {961.351f, 588.022f},
            {962.515f, 589.651f}, {963.212f, 591.046f}, {964.375f, 592.907f},
            {965.771f, 595.698f}, {965.771f, 595.698f}, {966.236f, 597.094f},
            {966.701f, 598.257f}, {966.934f, 599.885f}, {967.632f, 599.653f},
            {968.562f, 597.094f}, {968.562f, 597.094f}, {969.725f, 594.303f},
            {969.958f, 592.209f}, {969.958f, 592.209f}, {970.190f, 590.814f},
            {970.190f, 589.418f}, {970.190f, 588.255f}, {969.725f, 586.394f},
            {969.260f, 585.464f}, {968.562f, 584.301f}, {967.864f, 582.905f},
            {966.934f, 581.975f}, {965.771f, 581.044f}, {964.608f, 579.881f},
            {963.445f, 578.951f}, {962.747f, 578.486f}, {961.817f, 577.788f},
            {959.491f, 575.927f}, {959.491f, 575.927f}, {958.560f, 574.997f},
            {957.862f, 574.299f}, {957.165f, 573.834f}, {956.002f, 572.903f},
            {955.071f, 571.508f}, {954.141f, 570.810f}, {953.210f, 569.647f},
            {952.513f, 568.949f}, {951.815f, 568.251f}, {950.884f, 567.088f},
            {949.954f, 565.460f}, {949.023f, 564.297f}, {948.326f, 563.366f},
            {947.628f, 562.436f}, {946.697f, 561.040f}, {946.232f, 559.877f},
            {945.767f, 558.017f}, {945.069f, 556.388f}, {944.604f, 555.225f},
            {943.906f, 553.597f}, {943.674f, 551.969f}, {942.976f, 550.108f},
            {942.278f, 548.712f}, {942.278f, 547.549f}, {942.511f, 544.991f},
            {942.511f, 544.991f}, {942.511f, 543.130f}, {942.743f, 541.269f},
            {943.441f, 539.641f}, {943.674f, 538.478f}, {944.139f, 536.850f},
            {944.371f, 535.919f}, {945.069f, 534.989f}, {946.000f, 532.430f},
            {946.697f, 530.337f}, {947.860f, 528.011f}, {949.256f, 525.917f},
            {950.419f, 523.591f}, {951.350f, 520.102f}, {952.513f, 517.311f},
            {952.978f, 514.752f}, {953.676f, 511.728f}, {954.141f, 508.705f},
            {954.606f, 504.750f}, {955.071f, 502.192f}, {955.071f, 500.098f},
            {954.839f, 498.005f}, {954.839f, 495.911f}, {954.606f, 493.585f},
            {954.141f, 491.724f}, {953.210f, 489.631f}, {952.513f, 487.072f},
            {951.350f, 484.514f}, {950.884f, 483.118f}, {949.954f, 481.722f},
            {949.023f, 480.559f}, {948.558f, 479.396f}, {947.860f, 477.768f},
            {947.628f, 477.070f}, {947.628f, 475.907f}, {947.163f, 474.977f},
            {947.860f, 475.210f}, {949.256f, 476.373f}, {949.954f, 477.303f},
            {951.117f, 478.233f}, {952.047f, 479.164f}, {952.978f, 480.559f},
            {954.141f, 481.490f}, {955.071f, 483.351f}, {955.769f, 484.281f},
            {957.165f, 486.840f}, {958.328f, 488.933f}, {959.491f, 491.492f},
            {961.119f, 494.981f}, {962.282f, 498.703f}, {962.747f, 501.959f},
            {962.515f, 506.844f}, {962.515f, 510.333f}, {962.515f, 514.287f},
            {962.282f, 517.078f}, {961.817f, 519.637f}, {961.817f, 522.428f},
            {961.119f, 524.987f}, {960.886f, 526.615f}, {961.817f, 529.174f},
            {962.515f, 530.337f}, {963.678f, 531.500f}, {964.608f, 533.593f},
            {965.073f, 535.221f}, {965.538f, 537.082f}, {965.538f, 538.478f},
            {965.306f, 540.106f}, {965.306f, 542.199f}, {965.073f, 543.595f},
            {965.073f, 545.456f}, {965.073f, 546.386f}, {965.073f, 548.480f},
            {965.771f, 549.410f}, {965.771f, 550.573f}, {966.236f, 552.201f},
            {966.701f, 553.364f}, {967.167f, 554.295f}, {968.795f, 557.551f},
            {970.423f, 559.877f}, {971.586f, 562.436f}, {973.447f, 565.460f},
            {975.308f, 568.018f}, {977.401f, 570.577f}, {979.262f, 572.438f},
            {981.355f, 575.229f}, {982.286f, 576.625f}, {983.216f, 578.020f},
            {984.379f, 579.649f}, {984.844f, 581.044f}, {985.310f, 582.673f},
            {985.775f, 584.301f}, {985.542f, 586.627f}, {985.310f, 588.255f},
            {985.310f, 589.883f}, {985.077f, 592.442f}, {984.844f, 593.837f},
            {984.612f, 595.931f}, {984.147f, 597.094f}, {983.449f, 598.490f},
            {982.751f, 600.816f}, {982.053f, 602.211f}, {981.588f, 604.537f},
            {981.355f, 607.329f}, {980.890f, 609.655f}, {980.192f, 611.981f},
            {979.960f, 613.841f}, {979.960f, 615.935f}, {979.727f, 618.493f},
            {979.960f, 620.354f}, {979.727f, 622.448f}, {980.658f, 625.006f},
            {981.588f, 626.867f}, {982.751f, 629.426f}, {984.147f, 631.287f},
            {985.542f, 632.682f}, {986.938f, 634.078f}, {987.403f, 635.008f},
            {987.403f, 633.613f}, {986.705f, 631.519f}, {986.473f, 629.891f},
            {986.240f, 628.495f}, {986.007f, 626.402f}, {986.240f, 624.541f},
            {986.240f, 622.680f}, {986.240f, 620.820f}, {986.705f, 618.493f},
            {987.171f, 615.935f}, {987.403f, 613.841f}, {988.101f, 611.283f},
            {989.031f, 608.492f}, {989.962f, 606.398f}, {990.892f, 604.305f},
            {991.590f, 602.211f}, {992.520f, 601.048f}, {993.218f, 599.187f},
            {994.381f, 597.327f}, {995.079f, 594.768f}, {995.544f, 590.348f},
            {995.544f, 582.440f}, {995.544f, 580.114f}, {996.242f, 578.486f},
            {996.707f, 577.090f}, {997.172f, 574.764f}, {997.172f, 572.903f},
            {997.172f, 571.042f}, {996.009f, 569.414f}, {994.614f, 568.251f},
            {993.218f, 567.088f}, {991.590f, 566.390f}, {990.194f, 565.460f},
            {988.566f, 564.529f}, {987.171f, 563.599f}, {985.775f, 562.669f},
            {984.147f, 561.506f}, {982.751f, 560.110f}, {981.588f, 558.249f},
            {980.192f, 556.388f}, {979.495f, 554.527f}, {979.029f, 552.434f},
            {978.797f, 550.806f}, {978.564f, 548.247f}, {978.564f, 545.921f},
            {978.332f, 542.665f}, {978.564f, 540.106f}, {978.797f, 537.780f},
            {979.262f, 535.454f}, {979.262f, 532.663f}, {979.495f, 530.569f},
            {979.029f, 528.708f}, {978.564f, 526.848f}, {977.634f, 525.452f},
            {977.169f, 523.591f}, {976.703f, 522.196f}, {975.773f, 520.335f},
            {975.308f, 518.009f}, {975.540f, 516.148f}, {976.006f, 514.287f},
            {976.238f, 512.426f}, {976.936f, 510.333f}, {977.169f, 508.007f},
            {976.703f, 505.681f}, {976.471f, 503.587f}, {976.238f, 501.959f},
            {975.540f, 500.796f}, {974.377f, 499.400f}, {973.214f, 498.005f},
            {972.051f, 496.842f}, {971.121f, 495.911f}, {969.958f, 494.981f},
            {968.795f, 493.585f}, {967.864f, 492.190f}, {966.701f, 490.329f},
            {966.236f, 488.468f}, {966.004f, 487.305f}, {966.236f, 485.677f},
            {966.004f, 484.746f}, {966.236f, 483.118f}, {966.934f, 481.490f},
            {967.399f, 479.862f}, {968.097f, 478.233f}, {968.795f, 477.070f},
            {969.725f, 475.907f}, {970.190f, 473.581f}, {970.656f, 471.488f},
            {971.121f, 469.162f}, {971.121f, 467.301f}, {971.121f, 464.277f},
            {971.353f, 460.788f}, {972.051f, 458.695f}, {972.982f, 456.601f},
            {973.912f, 452.414f}, {975.540f, 449.856f}, {977.634f, 446.134f},
            {980.425f, 443.110f}, {982.984f, 438.458f}, {984.844f, 435.900f},
            {987.403f, 431.713f}, {989.264f, 428.224f}, {990.892f, 425.665f},
            {991.590f, 423.804f}, {993.683f, 418.454f}, {994.614f, 414.733f},
            {995.544f, 411.941f}, {995.777f, 409.150f}, {995.777f, 407.522f},
            {995.777f, 406.591f}, {995.777f, 405.196f}, {996.009f, 403.800f},
            {996.009f, 402.637f}, {996.009f, 401.242f}, {996.940f, 402.637f},
            {997.172f, 403.800f}, {997.172f, 405.661f}, {996.940f, 407.754f},
            {996.707f, 410.313f}, {996.475f, 412.639f}, {996.475f, 415.198f},
            {996.242f, 418.222f}, {996.242f, 420.780f}, {995.544f, 426.130f},
            {995.079f, 430.084f}, {994.381f, 433.341f}, {993.916f, 436.365f},
            {993.218f, 438.458f}, {992.520f, 440.784f}, {991.590f, 443.343f},
            {990.660f, 445.204f}, {989.497f, 448.228f}, {988.101f, 450.554f},
            {986.473f, 452.647f}, {985.077f, 454.740f}, {983.449f, 457.532f},
            {982.286f, 459.392f}, {981.588f, 461.719f}, {981.588f, 463.347f},
            {982.053f, 466.371f}, {982.751f, 468.231f}, {983.681f, 470.092f},
            {984.612f, 472.186f}, {986.705f, 474.512f}, {986.705f, 474.512f},
            {987.868f, 476.838f}, {988.334f, 478.466f}, {988.799f, 480.327f},
            {988.566f, 483.583f}, {988.101f, 485.212f}, {987.868f, 487.072f},
            {987.403f, 488.933f}, {986.705f, 490.794f}, {987.171f, 491.724f},
            {987.868f, 492.655f}, {989.264f, 493.585f}, {992.055f, 495.213f},
            {992.055f, 495.213f}, {993.916f, 496.377f}, {995.079f, 497.074f},
            {995.777f, 498.237f}, {996.475f, 498.935f}, {997.405f, 500.331f},
            {997.405f, 501.959f}, {997.405f, 503.820f}, {996.940f, 505.215f},
            {996.707f, 507.076f}, {996.707f, 508.937f}, {996.475f, 510.798f},
            {996.707f, 512.194f}, {997.172f, 514.054f}, {997.870f, 515.915f},
            {998.801f, 517.078f}, {1000.196f, 518.706f}, {1001.127f, 520.102f},
            {1002.522f, 521.730f}, {1002.988f, 522.661f}, {1004.151f, 523.824f},
            {1005.081f, 525.219f}, {1006.244f, 527.080f}, {1007.407f, 528.476f},
            {1008.570f, 530.337f}, {1009.966f, 532.430f}, {1010.896f, 534.058f},
            {1011.361f, 535.919f}, {1012.757f, 537.547f}, {1013.687f, 538.943f},
            {1014.153f, 540.106f}, {1015.781f, 538.943f}, {1017.176f, 537.082f},
            {1018.107f, 535.221f}, {1018.572f, 533.361f}, {1019.037f, 531.267f},
            {1019.270f, 529.174f}, {1019.270f, 527.080f}, {1018.339f, 524.754f},
            {1017.874f, 523.359f}, {1016.479f, 521.730f}, {1015.781f, 520.567f},
            {1015.083f, 519.637f}, {1014.385f, 518.474f}, {1013.687f, 516.613f},
            {1012.524f, 514.752f}, {1011.827f, 513.357f}, {1010.896f, 511.496f},
            {1010.431f, 510.100f}, {1009.733f, 508.239f}, {1009.500f, 506.146f},
            {1009.268f, 504.518f}, {1009.268f, 502.657f}, {1009.500f, 501.261f},
            {1009.966f, 500.098f}, {1010.896f, 498.005f}, {1011.594f, 496.842f},
            {1012.292f, 494.516f}, {1012.524f, 492.422f}, {1011.827f, 490.329f},
            {1011.594f, 488.933f}, {1011.129f, 487.072f}, {1010.431f, 485.444f},
            {1010.198f, 483.816f}, {1009.966f, 482.188f}, {1009.500f, 480.792f},
            {1009.500f, 479.396f}, {1009.500f, 478.233f}, {1009.500f, 476.605f},
            {1010.198f, 474.977f}, {1010.431f, 472.418f}, {1011.129f, 469.860f},
            {1011.827f, 468.464f}, {1012.524f, 465.440f}, {1013.920f, 462.882f},
            {1015.083f, 461.486f}, {1017.176f, 456.834f}, {1019.037f, 454.275f},
            {1021.363f, 449.856f}, {1022.526f, 447.995f}, {1023.457f, 446.134f},
            {1024.387f, 443.110f}, {1025.318f, 441.017f}, {1025.783f, 438.691f},
            {1025.783f, 437.993f}, {1026.015f, 435.900f}, {1026.015f, 434.969f},
            {1026.248f, 432.876f}, {1026.015f, 431.480f}, {1025.783f, 430.084f},
            {1025.318f, 428.224f}, {1024.620f, 426.130f}, {1023.922f, 424.269f},
            {1022.991f, 420.780f}, {1022.294f, 419.152f}, {1021.828f, 417.524f},
            {1021.363f, 415.896f}, {1020.898f, 414.733f}, {1020.433f, 413.570f},
            {1019.968f, 411.941f}, {1019.502f, 410.546f}, {1019.037f, 409.150f},
            {1019.037f, 407.754f}, {1019.968f, 409.150f}, {1021.131f, 411.011f},
            {1022.526f, 413.104f}, {1024.155f, 415.663f}, {1025.318f, 418.222f},
            {1027.411f, 421.478f}, {1028.109f, 423.572f}, {1029.039f, 425.665f},
            {1029.970f, 428.224f}, {1030.667f, 430.782f}, {1031.598f, 433.341f},
            {1032.528f, 435.900f}, {1033.459f, 440.086f}, {1034.622f, 442.878f},
            {1035.320f, 446.134f}, {1036.017f, 450.088f}, {1036.250f, 453.112f},
            {1036.250f, 456.136f}, {1036.250f, 459.160f}, {1036.250f, 462.416f},
            {1036.017f, 465.208f}, {1035.552f, 469.162f}, {1035.320f, 472.651f},
            {1034.622f, 478.466f}, {1033.459f, 480.559f}, {1032.528f, 484.979f},
            {1032.063f, 488.933f}, {1031.830f, 492.887f}, {1032.296f, 495.446f},
            {1032.993f, 497.074f}, {1034.389f, 498.935f}, {1035.552f, 500.796f},
            {1036.715f, 503.355f}, {1038.809f, 506.844f}, {1039.972f, 509.868f},
            {1040.204f, 513.124f}, {1039.739f, 516.846f}, {1039.506f, 521.265f},
            {1039.274f, 525.452f}, {1039.041f, 528.243f}, {1038.343f, 532.430f},
            {1038.576f, 535.687f}, {1038.809f, 538.943f}, {1039.041f, 541.502f},
            {1039.041f, 544.293f}, {1039.972f, 547.084f}, {1039.739f, 549.643f},
            {1039.972f, 552.201f}, {1039.972f, 554.295f}, {1039.506f, 557.551f},
            {1039.274f, 560.110f}, {1038.576f, 562.901f}, {1037.646f, 564.995f},
            {1037.413f, 567.088f}, {1036.715f, 569.414f}, {1035.087f, 571.973f},
            {1034.156f, 574.531f}, {1032.528f, 576.857f}, {1030.900f, 579.183f},
            {1029.039f, 581.277f}, {1026.713f, 584.301f}, {1024.852f, 585.929f},
            {1023.689f, 588.255f}, {1022.526f, 590.814f}, {1021.596f, 592.907f},
            {1020.665f, 595.466f}, {1020.200f, 598.955f}, {1019.968f, 602.444f},
            {1019.735f, 604.770f}, {1019.735f, 608.259f}, {1020.200f, 611.283f},
            {1020.665f, 613.609f}, {1020.665f, 616.400f}, {1020.665f, 618.493f},
            {1020.665f, 621.285f}, {1021.131f, 622.913f}, {1021.363f, 624.076f},
            {1022.294f, 626.635f}, {1022.991f, 628.961f}, {1022.991f, 629.891f},
            {1023.689f, 628.263f}, {1024.620f, 626.635f}, {1025.318f, 625.006f},
            {1026.248f, 623.611f}, {1027.178f, 621.517f}, {1027.876f, 619.424f},
            {1028.574f, 617.330f}, {1028.807f, 615.237f}, {1029.272f, 611.515f},
            {1028.807f, 609.422f}, {1028.807f, 606.398f}, {1028.807f, 604.537f},
            {1029.272f, 600.816f}, {1029.039f, 597.559f}, {1029.039f, 594.768f},
            {1029.737f, 592.442f}, {1030.202f, 590.581f}, {1031.133f, 589.185f},
            {1032.296f, 587.557f}, {1033.226f, 586.162f}, {1034.389f, 584.766f},
            {1035.785f, 583.603f}, {1037.180f, 582.207f}, {1038.576f, 580.579f},
            {1040.204f, 578.951f}, {1042.065f, 577.323f}, {1043.461f, 575.927f},
            {1044.391f, 574.299f}, {1045.787f, 572.903f}, {1046.717f, 571.740f},
            {1047.415f, 569.647f}, {1048.578f, 567.786f}, {1049.043f, 564.995f},
            {1049.276f, 561.971f}, {1049.974f, 559.412f}, {1049.974f, 556.621f},
            {1050.439f, 553.830f}, {1050.671f, 551.504f}, {1050.671f, 549.410f},
            {1050.904f, 547.782f}, {1051.369f, 544.293f}, {1051.602f, 540.106f},
            {1051.834f, 535.454f}, {1052.300f, 520.102f}, {1052.532f, 518.706f},
            {1057.417f, 527.313f}, {1059.278f, 529.639f}, {1061.371f, 532.663f},
            {1062.767f, 534.291f}, {1061.371f, 535.919f}, {1060.906f, 537.315f},
            {1060.441f, 546.386f}, {1060.906f, 551.271f}, {1060.441f, 555.458f},
            {1059.743f, 562.436f}, {1059.510f, 564.995f}, {1059.278f, 567.088f},
            {1059.045f, 569.181f}, {1058.347f, 572.438f}, {1057.417f, 575.694f},
            {1056.021f, 577.788f}, {1054.858f, 579.881f}, {1053.463f, 581.277f},
            {1052.532f, 582.207f}, {1050.671f, 583.836f}, {1049.276f, 585.231f},
            {1048.113f, 586.627f}, {1046.484f, 588.255f}, {1045.089f, 589.883f},
            {1044.158f, 590.814f}, {1043.461f, 591.511f}, {1042.530f, 593.372f},
            {1041.832f, 594.535f}, {1040.669f, 596.629f}, {1039.506f, 599.653f},
            {1039.506f, 602.676f}, {1039.506f, 605.235f}, {1039.739f, 608.026f},
            {1039.506f, 611.050f}, {1038.809f, 614.307f}, {1037.646f, 616.400f},
            {1036.715f, 619.424f}, {1036.250f, 621.517f}, {1035.087f, 624.076f},
            {1034.156f, 626.635f}, {1032.993f, 628.495f}, {1031.830f, 630.821f},
            {1030.202f, 632.915f}, {1028.574f, 635.008f}, {1027.178f, 636.637f},
            {1025.783f, 638.730f}, {1023.922f, 639.893f}, {1022.991f, 641.056f},
            {1022.061f, 641.986f}, {1021.363f, 642.917f}, {1021.363f, 643.847f},
            {1021.363f, 645.010f}, {1020.433f, 645.243f}, {1019.037f, 645.708f},
            {1017.176f, 645.941f}, {1015.083f, 646.173f}, {1013.222f, 646.871f},
            {1011.827f, 648.732f}, {1009.966f, 650.825f}, {1007.872f, 653.151f},
            {1005.546f, 654.780f}, {1002.988f, 654.547f}, {1000.429f, 654.314f},
            {993.683f, 651.988f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.196f, 0.471f, 0.141f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.263f, 0.69f, 0.176f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1103.240f, 657.571f}, {1102.077f, 656.408f}, {1100.914f, 655.478f},
            {1099.053f, 654.314f}, {1097.425f, 653.151f}, {1096.029f, 652.454f},
            {1094.633f, 651.523f}, {1092.540f, 649.895f}, {1089.749f, 647.569f},
            {1087.190f, 644.778f}, {1084.864f, 642.219f}, {1082.771f, 639.195f},
            {1081.375f, 637.567f}, {1078.816f, 634.078f}, {1076.723f, 630.356f},
            {1074.862f, 626.867f}, {1073.699f, 624.309f}, {1073.234f, 621.285f},
            {1070.908f, 615.004f}, {1069.512f, 610.818f}, {1068.349f, 605.700f},
            {1067.651f, 601.979f}, {1066.954f, 598.257f}, {1066.488f, 594.070f},
            {1066.256f, 588.255f}, {1065.791f, 584.533f}, {1066.023f, 581.509f},
            {1066.256f, 578.486f}, {1067.651f, 575.462f}, {1068.814f, 570.112f},
            {1069.512f, 567.321f}, {1069.977f, 564.995f}, {1070.210f, 562.203f},
            {1071.140f, 558.017f}, {1071.373f, 554.993f}, {1071.373f, 552.899f},
            {1070.908f, 547.317f}, {1069.745f, 544.525f}, {1068.582f, 542.432f},
            {1067.651f, 540.571f}, {1066.256f, 538.943f}, {1065.093f, 537.082f},
            {1062.999f, 534.756f}, {1059.976f, 530.569f}, {1057.882f, 527.545f},
            {1056.021f, 524.987f}, {1054.393f, 521.963f}, {1052.300f, 517.543f},
            {1051.137f, 515.217f}, {1050.671f, 512.891f}, {1049.508f, 510.100f},
            {1048.578f, 506.844f}, {1048.113f, 504.518f}, {1047.648f, 501.959f},
            {1046.950f, 498.703f}, {1046.717f, 496.144f}, {1046.484f, 492.190f},
            {1046.252f, 487.305f}, {1046.484f, 484.281f}, {1046.717f, 481.490f},
            {1047.415f, 477.768f}, {1048.113f, 475.442f}, {1049.043f, 473.349f},
            {1049.741f, 469.627f}, {1050.904f, 467.301f}, {1051.602f, 465.440f},
            {1052.300f, 463.347f}, {1052.300f, 461.021f}, {1052.300f, 457.532f},
            {1052.300f, 453.112f}, {1052.300f, 453.112f}, {1052.067f, 450.786f},
            {1051.602f, 448.460f}, {1050.671f, 444.971f}, {1049.974f, 443.110f},
            {1049.276f, 441.947f}, {1048.811f, 440.552f}, {1048.578f, 439.389f},
            {1049.508f, 440.319f}, {1050.671f, 441.715f}, {1051.834f, 442.878f},
            {1053.230f, 444.273f}, {1054.393f, 445.669f}, {1055.789f, 447.995f},
            {1056.719f, 450.088f}, {1057.649f, 452.182f}, {1058.347f, 454.275f},
            {1059.510f, 457.066f}, {1059.743f, 458.927f}, {1060.208f, 460.788f},
            {1060.673f, 462.184f}, {1060.673f, 463.579f}, {1061.139f, 464.975f},
            {1061.139f, 466.836f}, {1061.836f, 469.860f}, {1062.302f, 472.884f},
            {1063.697f, 476.605f}, {1064.628f, 479.629f}, {1065.558f, 482.420f},
            {1066.954f, 486.142f}, {1066.954f, 486.142f}, {1068.349f, 489.166f},
            {1069.047f, 492.190f}, {1069.977f, 493.818f}, {1070.443f, 495.213f},
            {1072.071f, 497.540f}, {1073.699f, 500.796f}, {1074.862f, 502.424f},
            {1075.793f, 504.052f}, {1076.490f, 505.215f}, {1076.956f, 506.611f},
            {1078.584f, 508.472f}, {1079.514f, 509.635f}, {1080.910f, 510.565f},
            {1081.608f, 511.961f}, {1082.771f, 513.357f}, {1084.166f, 515.217f},
            {1085.097f, 516.846f}, {1086.492f, 518.706f}, {1088.586f, 521.730f},
            {1088.818f, 523.126f}, {1089.981f, 525.452f}, {1090.447f, 527.545f},
            {1090.679f, 529.406f}, {1090.912f, 531.732f}, {1091.144f, 533.826f},
            {1091.610f, 536.152f}, {1091.610f, 538.710f}, {1091.610f, 541.734f},
            {1091.610f, 545.456f}, {1091.610f, 550.108f}, {1091.842f, 555.458f},
            {1091.842f, 559.412f}, {1091.842f, 561.506f}, {1091.842f, 563.134f},
            {1091.144f, 565.227f}, {1090.912f, 566.623f}, {1089.981f, 568.949f},
            {1089.284f, 570.577f}, {1088.818f, 572.671f}, {1088.121f, 574.531f},
            {1087.655f, 577.090f}, {1087.190f, 579.416f}, {1086.725f, 581.975f},
            {1085.795f, 585.696f}, {1085.329f, 588.022f}, {1085.329f, 590.814f},
            {1085.562f, 593.140f}, {1085.795f, 596.396f}, {1085.795f, 598.722f},
            {1086.027f, 602.909f}, {1086.492f, 607.096f}, {1087.190f, 610.352f},
            {1087.888f, 613.841f}, {1088.818f, 617.563f}, {1089.981f, 620.587f},
            {1090.679f, 622.913f}, {1092.075f, 625.006f}, {1093.005f, 626.635f},
            {1094.168f, 628.961f}, {1095.564f, 631.519f}, {1096.727f, 633.380f},
            {1097.425f, 635.474f}, {1098.588f, 637.334f}, {1099.751f, 639.660f},
            {1101.146f, 641.986f}, {1102.309f, 645.476f}, {1104.170f, 647.569f},
            {1105.566f, 649.895f}, {1106.961f, 652.221f}, {1107.659f, 653.384f},
            {1108.822f, 655.012f}, {1109.288f, 655.943f}, {1109.520f, 657.106f},
            {1104.635f, 658.734f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.227f, 0.541f, 0.416f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.29f, 0.729f, 0.553f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1105.798f, 656.408f}, {1104.403f, 655.012f}, {1102.775f, 653.384f},
            {1101.612f, 651.988f}, {1099.286f, 649.895f}, {1097.425f, 648.034f},
            {1095.331f, 646.406f}, {1093.703f, 644.545f}, {1092.075f, 642.452f},
            {1089.981f, 639.428f}, {1088.586f, 636.404f}, {1087.423f, 632.450f},
            {1086.725f, 629.891f}, {1085.097f, 623.146f}, {1085.097f, 623.146f},
            {1084.166f, 619.424f}, {1083.934f, 612.446f}, {1083.934f, 606.165f},
            {1084.399f, 599.420f}, {1084.632f, 594.303f}, {1085.562f, 585.231f},
            {1085.795f, 581.742f}, {1087.190f, 578.486f}, {1088.353f, 571.740f},
            {1089.981f, 567.786f}, {1092.075f, 564.064f}, {1096.494f, 555.690f},
            {1098.820f, 551.736f}, {1100.449f, 548.480f}, {1102.077f, 545.921f},
            {1104.403f, 542.897f}, {1105.798f, 539.873f}, {1107.659f, 535.454f},
            {1108.357f, 533.593f}, {1109.288f, 531.732f}, {1110.218f, 530.104f},
            {1110.683f, 528.243f}, {1111.148f, 527.080f}, {1111.614f, 525.685f},
            {1112.311f, 523.126f}, {1113.009f, 520.335f}, {1113.242f, 517.776f},
            {1113.474f, 515.683f}, {1113.940f, 512.891f}, {1113.242f, 510.333f},
            {1112.544f, 507.774f}, {1112.079f, 505.215f}, {1111.148f, 502.889f},
            {1109.985f, 500.331f}, {1107.892f, 496.609f}, {1105.566f, 492.887f},
            {1103.705f, 489.398f}, {1101.612f, 486.375f}, {1099.286f, 483.118f},
            {1097.890f, 480.094f}, {1095.796f, 478.233f}, {1094.401f, 475.907f},
            {1092.540f, 473.581f}, {1091.610f, 471.023f}, {1089.981f, 468.464f},
            {1088.586f, 465.673f}, {1087.423f, 463.114f}, {1086.492f, 460.788f},
            {1085.562f, 457.532f}, {1084.632f, 454.043f}, {1083.701f, 451.251f},
            {1083.236f, 448.693f}, {1083.236f, 445.901f}, {1082.771f, 443.343f},
            {1082.305f, 440.086f}, {1082.305f, 436.365f}, {1082.305f, 433.573f},
            {1082.305f, 431.015f}, {1081.608f, 427.758f}, {1081.840f, 424.269f},
            {1082.073f, 421.013f}, {1081.840f, 417.059f}, {1082.305f, 413.337f},
            {1082.771f, 410.546f}, {1083.236f, 408.917f}, {1083.934f, 406.824f},
            {1084.864f, 404.265f}, {1085.562f, 401.939f}, {1086.260f, 399.148f},
            {1087.423f, 395.892f}, {1089.051f, 392.868f}, {1089.749f, 389.844f},
            {1090.912f, 387.518f}, {1091.842f, 385.657f}, {1092.307f, 383.796f},
            {1092.540f, 382.168f}, {1091.842f, 380.075f}, {1091.144f, 378.446f},
            {1090.447f, 377.051f}, {1089.749f, 375.423f}, {1089.749f, 373.794f},
            {1089.284f, 372.399f}, {1088.818f, 370.538f}, {1088.818f, 368.444f},
            {1088.121f, 366.816f}, {1087.888f, 365.188f}, {1088.121f, 364.258f},
            {1088.121f, 362.397f}, {1088.121f, 361.466f}, {1088.121f, 358.675f},
            {1088.121f, 356.582f}, {1088.121f, 354.953f}, {1088.586f, 352.860f},
            {1089.051f, 351.232f}, {1089.284f, 348.673f}, {1089.981f, 346.347f},
            {1090.679f, 344.719f}, {1091.377f, 343.323f}, {1091.610f, 342.160f},
            {1092.307f, 340.532f}, {1093.238f, 338.671f}, {1094.168f, 336.345f},
            {1094.866f, 334.717f}, {1095.564f, 332.856f}, {1096.262f, 331.228f},
            {1096.960f, 329.832f}, {1097.657f, 328.669f}, {1098.123f, 327.739f},
            {1098.123f, 326.111f}, {1097.657f, 324.482f}, {1097.192f, 322.389f},
            {1093.470f, 310.526f}, {1089.051f, 295.639f}, {1085.795f, 282.846f},
            {1085.562f, 280.753f}, {1085.329f, 278.892f}, {1084.864f, 277.496f},
            {1084.864f, 275.868f}, {1084.632f, 273.775f}, {1084.399f, 270.983f},
            {1084.166f, 269.123f}, {1083.701f, 267.029f}, {1083.701f, 265.634f},
            {1083.701f, 263.773f}, {1083.701f, 260.981f}, {1083.934f, 258.655f},
            {1084.166f, 256.562f}, {1084.166f, 254.236f}, {1084.632f, 251.677f},
            {1084.864f, 249.816f}, {1085.097f, 248.421f}, {1085.097f, 247.723f},
            {1085.795f, 246.095f}, {1086.260f, 243.769f}, {1086.725f, 240.978f},
            {1086.958f, 239.117f}, {1087.655f, 237.954f}, {1088.353f, 236.093f},
            {1088.818f, 233.767f}, {1089.981f, 230.510f}, {1090.679f, 228.882f},
            {1091.610f, 226.323f}, {1092.075f, 224.695f}, {1094.633f, 220.043f},
            {1095.564f, 217.252f}, {1096.960f, 214.461f}, {1100.914f, 206.087f},
            {1102.775f, 202.365f}, {1104.403f, 199.807f}, {1106.729f, 197.481f},
            {1108.590f, 195.387f}, {1109.753f, 193.992f}, {1110.451f, 193.526f},
            {1111.381f, 192.596f}, {1111.846f, 192.131f}, {1111.381f, 193.992f},
            {1110.451f, 196.085f}, {1109.985f, 197.713f}, {1109.055f, 200.737f},
            {1107.892f, 204.459f}, {1106.961f, 206.785f}, {1106.031f, 209.576f},
            {1105.333f, 213.763f}, {1104.635f, 217.252f}, {1103.705f, 220.508f},
            {1103.007f, 224.463f}, {1102.775f, 227.254f}, {1102.542f, 231.208f},
            {1102.542f, 233.999f}, {1102.309f, 237.023f}, {1102.077f, 240.512f},
            {1102.309f, 244.234f}, {1102.077f, 249.351f}, {1103.007f, 252.375f},
            {1103.240f, 254.701f}, {1103.240f, 256.562f}, {1103.472f, 260.051f},
            {1104.635f, 263.773f}, {1105.333f, 266.331f}, {1106.496f, 267.727f},
            {1107.194f, 268.657f}, {1108.124f, 269.588f}, {1108.822f, 271.216f},
            {1108.590f, 273.309f}, {1108.357f, 274.472f}, {1107.892f, 276.101f},
            {1107.194f, 277.496f}, {1106.961f, 279.590f}, {1106.961f, 281.683f},
            {1106.961f, 283.777f}, {1107.892f, 287.964f}, {1107.892f, 287.964f},
            {1108.822f, 289.824f}, {1109.288f, 291.685f}, {1110.451f, 293.779f},
            {1111.381f, 295.639f}, {1113.009f, 298.198f}, {1114.405f, 300.524f},
            {1115.335f, 302.618f}, {1116.498f, 304.246f}, {1117.661f, 306.339f},
            {1118.359f, 307.967f}, {1119.522f, 311.689f}, {1120.452f, 314.480f},
            {1121.848f, 318.667f}, {1123.011f, 322.854f}, {1123.942f, 327.739f},
            {1124.872f, 332.158f}, {1125.570f, 336.578f}, {1126.733f, 343.788f},
            {1127.663f, 347.045f}, {1128.361f, 353.790f}, {1128.594f, 357.977f},
            {1129.059f, 362.397f}, {1129.291f, 365.653f}, {1129.291f, 368.444f},
            {1129.059f, 370.538f}, {1128.826f, 372.631f}, {1128.594f, 375.655f},
            {1128.128f, 378.214f}, {1126.733f, 381.238f}, {1125.337f, 384.727f},
            {1124.407f, 386.820f}, {1123.476f, 389.379f}, {1121.848f, 391.240f},
            {1120.685f, 393.333f}, {1120.220f, 396.357f}, {1119.522f, 399.381f},
            {1118.592f, 402.172f}, {1117.661f, 406.359f}, {1116.731f, 407.754f},
            {1115.568f, 410.080f}, {1115.103f, 411.709f}, {1113.707f, 413.802f},
            {1111.846f, 417.524f}, {1110.451f, 420.082f}, {1109.753f, 423.106f},
            {1109.055f, 426.130f}, {1108.357f, 427.991f}, {1108.124f, 431.015f},
            {1107.892f, 434.039f}, {1107.892f, 437.760f}, {1108.124f, 440.784f},
            {1108.822f, 443.343f}, {1109.520f, 445.669f}, {1110.451f, 448.693f},
            {1111.614f, 451.717f}, {1112.311f, 454.740f}, {1114.172f, 458.695f},
            {1115.335f, 461.951f}, {1117.196f, 465.905f}, {1118.824f, 469.162f},
            {1119.987f, 472.186f}, {1121.150f, 474.977f}, {1122.081f, 477.536f},
            {1122.779f, 479.164f}, {1124.174f, 481.490f}, {1125.105f, 484.979f},
            {1126.035f, 488.003f}, {1127.896f, 491.492f}, {1128.826f, 494.283f},
            {1129.989f, 497.307f}, {1130.920f, 500.098f}, {1131.850f, 503.355f},
            {1132.780f, 506.844f}, {1133.013f, 510.333f}, {1133.013f, 514.287f},
            {1133.013f, 517.776f}, {1132.548f, 521.265f}, {1132.083f, 524.754f},
            {1131.385f, 529.871f}, {1129.757f, 535.454f}, {1128.361f, 541.502f},
            {1126.268f, 545.456f}, {1124.407f, 549.178f}, {1122.779f, 552.667f},
            {1120.685f, 556.156f}, {1118.359f, 561.040f}, {1115.800f, 565.460f},
            {1113.707f, 569.647f}, {1111.381f, 574.531f}, {1109.288f, 579.649f},
            {1107.892f, 585.231f}, {1106.264f, 590.814f}, {1105.333f, 595.233f},
            {1104.635f, 599.885f}, {1103.938f, 607.561f}, {1104.170f, 612.213f},
            {1104.170f, 618.028f}, {1104.170f, 621.285f}, {1104.403f, 624.076f},
            {1104.170f, 627.332f}, {1105.333f, 632.450f}, {1106.496f, 635.706f},
            {1107.427f, 638.265f}, {1108.357f, 640.591f}, {1109.985f, 644.313f},
            {1112.311f, 648.499f}, {1113.940f, 650.825f}, {1115.335f, 652.454f},
            {1116.498f, 653.849f}, {1117.429f, 655.245f}, {1117.661f, 656.408f},
            {1107.194f, 657.804f}, {1106.031f, 657.106f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.227f, 0.541f, 0.416f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.29f, 0.729f, 0.553f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1082.538f, 510.333f}, {1083.468f, 507.774f}, {1084.166f, 505.681f},
            {1085.097f, 502.657f}, {1086.027f, 499.633f}, {1086.958f, 497.074f},
            {1088.353f, 493.585f}, {1092.307f, 479.629f}, {1092.540f, 477.768f},
            {1093.005f, 475.907f}, {1094.633f, 477.536f}, {1096.727f, 480.327f},
            {1099.286f, 484.281f}, {1103.705f, 490.329f}, {1104.868f, 491.724f},
            {1106.496f, 494.516f}, {1108.357f, 497.772f}, {1109.753f, 499.866f},
            {1111.614f, 504.052f}, {1113.009f, 508.007f}, {1113.474f, 511.263f},
            {1113.474f, 514.752f}, {1113.707f, 518.241f}, {1113.474f, 521.033f},
            {1112.777f, 523.359f}, {1111.614f, 526.615f}, {1109.985f, 530.337f},
            {1108.590f, 533.128f}, {1106.264f, 538.245f}, {1104.403f, 540.804f},
            {1103.007f, 543.130f}, {1101.612f, 545.456f}, {1100.681f, 547.549f},
            {1099.518f, 549.178f}, {1098.355f, 551.504f}, {1096.494f, 554.527f},
            {1095.331f, 556.853f}, {1093.936f, 559.180f}, {1092.540f, 561.273f},
            {1092.075f, 556.156f}, {1091.377f, 550.573f}, {1091.377f, 544.060f},
            {1091.377f, 540.804f}, {1091.377f, 537.780f}, {1091.377f, 535.454f},
            {1091.144f, 533.128f}, {1090.447f, 530.802f}, {1090.214f, 528.476f},
            {1089.749f, 526.382f}, {1089.051f, 524.522f}, {1088.353f, 522.661f},
            {1087.423f, 520.567f}, {1086.492f, 518.241f}, {1085.562f, 516.613f},
            {1084.166f, 514.520f}, {1082.771f, 512.426f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.227f, 0.541f, 0.416f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.165f, 0.671f, 0.467f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1092.075f, 381.005f}, {1090.679f, 378.679f}, {1089.981f, 376.586f},
            {1089.516f, 374.260f}, {1089.051f, 371.701f}, {1089.051f, 369.142f},
            {1088.586f, 365.421f}, {1088.121f, 362.397f}, {1088.121f, 357.977f},
            {1088.353f, 354.721f}, {1088.586f, 352.162f}, {1089.284f, 349.371f},
            {1089.981f, 346.114f}, {1093.005f, 338.671f}, {1093.005f, 338.671f},
            {1094.168f, 336.112f}, {1095.564f, 333.554f}, {1096.494f, 331.460f},
            {1097.425f, 329.832f}, {1097.890f, 328.204f}, {1099.751f, 331.926f},
            {1100.216f, 335.647f}, {1100.914f, 340.765f}, {1101.146f, 345.417f},
            {1101.844f, 348.673f}, {1101.612f, 351.232f}, {1101.379f, 353.325f},
            {1101.146f, 355.186f}, {1100.914f, 357.512f}, {1100.681f, 359.605f},
            {1100.449f, 361.466f}, {1099.751f, 364.025f}, {1098.588f, 367.049f},
            {1097.657f, 369.375f}, {1096.727f, 371.933f}, {1096.262f, 374.260f},
            {1095.099f, 377.051f}, {1094.866f, 378.912f}, {1093.470f, 380.772f},
            {1092.773f, 381.935f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.227f, 0.541f, 0.416f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.165f, 0.671f, 0.467f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1127.431f, 346.114f}, {1126.500f, 343.091f}, {1125.570f, 339.602f},
            {1124.872f, 336.810f}, {1124.407f, 334.019f}, {1123.942f, 331.460f},
            {1123.476f, 328.669f}, {1122.546f, 325.645f}, {1121.848f, 321.924f},
            {1120.918f, 318.435f}, {1119.987f, 315.643f}, {1118.824f, 312.387f},
            {1118.126f, 309.596f}, {1119.057f, 307.037f}, {1120.452f, 304.013f},
            {1121.150f, 300.757f}, {1121.616f, 297.733f}, {1121.848f, 293.779f},
            {1123.011f, 290.290f}, {1123.476f, 287.033f}, {1124.174f, 284.009f},
            {1124.872f, 280.753f}, {1125.337f, 277.962f}, {1125.802f, 275.170f},
            {1126.035f, 272.844f}, {1126.035f, 271.681f}, {1126.500f, 274.472f},
            {1127.198f, 278.194f}, {1127.663f, 281.451f}, {1128.128f, 284.940f},
            {1128.826f, 289.359f}, {1128.826f, 295.872f}, {1129.989f, 299.594f},
            {1130.687f, 304.478f}, {1131.850f, 309.363f}, {1132.780f, 313.783f},
            {1133.013f, 317.272f}, {1133.246f, 320.761f}, {1133.246f, 324.715f},
            {1133.013f, 328.204f}, {1132.548f, 330.995f}, {1132.315f, 332.391f},
            {1131.385f, 335.880f}, {1129.989f, 338.904f}, {1129.524f, 341.928f},
            {1128.361f, 344.254f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.227f, 0.541f, 0.416f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.165f, 0.671f, 0.467f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1116.033f, 459.625f}, {1114.637f, 457.066f}, {1113.707f, 454.973f},
            {1112.777f, 452.182f}, {1111.614f, 450.321f}, {1111.148f, 448.460f},
            {1110.683f, 446.599f}, {1110.218f, 444.506f}, {1109.753f, 442.645f},
            {1109.288f, 440.319f}, {1108.822f, 437.063f}, {1108.822f, 433.806f},
            {1109.055f, 431.945f}, {1109.520f, 428.921f}, {1110.218f, 425.200f},
            {1111.381f, 422.176f}, {1112.079f, 420.315f}, {1113.242f, 418.222f},
            {1114.405f, 415.198f}, {1115.568f, 411.476f}, {1117.661f, 408.685f},
            {1118.824f, 405.661f}, {1119.522f, 402.870f}, {1119.987f, 401.009f},
            {1120.685f, 399.148f}, {1121.150f, 397.055f}, {1120.918f, 396.589f},
            {1121.150f, 398.218f}, {1121.383f, 401.009f}, {1121.616f, 404.033f},
            {1122.779f, 407.522f}, {1123.709f, 411.011f}, {1124.872f, 415.198f},
            {1124.872f, 418.222f}, {1124.407f, 422.176f}, {1123.942f, 424.269f},
            {1123.244f, 425.898f}, {1122.546f, 427.758f}, {1122.313f, 429.619f},
            {1122.081f, 430.782f}, {1121.616f, 431.945f}, {1121.383f, 434.271f},
            {1121.150f, 437.295f}, {1120.685f, 439.389f}, {1120.220f, 442.878f},
            {1119.987f, 444.971f}, {1119.755f, 447.530f}, {1119.289f, 450.554f},
            {1118.824f, 454.508f}, {1118.824f, 458.462f}, {1118.359f, 462.882f},
            {1117.196f, 461.486f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.227f, 0.541f, 0.416f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.165f, 0.671f, 0.467f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1116.731f, 655.710f}, {1115.103f, 653.151f}, {1113.474f, 651.523f},
            {1111.846f, 649.430f}, {1110.916f, 647.336f}, {1110.218f, 645.708f},
            {1108.590f, 642.917f}, {1107.427f, 640.591f}, {1106.496f, 638.032f},
            {1106.264f, 636.869f}, {1106.031f, 634.543f}, {1105.798f, 632.915f},
            {1105.333f, 631.287f}, {1104.868f, 629.193f}, {1104.635f, 627.100f},
            {1104.403f, 624.076f}, {1103.938f, 620.820f}, {1103.472f, 617.098f},
            {1103.472f, 614.307f}, {1103.472f, 610.585f}, {1103.705f, 606.631f},
            {1103.938f, 603.607f}, {1104.403f, 601.281f}, {1104.635f, 599.187f},
            {1104.868f, 596.861f}, {1105.566f, 594.535f}, {1105.798f, 592.209f},
            {1106.264f, 590.814f}, {1106.729f, 588.488f}, {1108.124f, 585.464f},
            {1108.822f, 581.742f}, {1109.520f, 579.881f}, {1110.683f, 577.323f},
            {1111.381f, 574.531f}, {1113.009f, 572.205f}, {1114.405f, 569.647f},
            {1115.568f, 566.390f}, {1116.731f, 563.832f}, {1117.429f, 562.901f},
            {1119.057f, 564.995f}, {1120.220f, 567.088f}, {1123.011f, 575.229f},
            {1126.500f, 581.509f}, {1127.896f, 583.603f}, {1128.594f, 584.999f},
            {1126.965f, 587.790f}, {1125.802f, 590.581f}, {1124.174f, 593.140f},
            {1123.011f, 595.466f}, {1122.313f, 597.792f}, {1120.685f, 601.048f},
            {1119.755f, 604.072f}, {1118.824f, 607.329f}, {1118.359f, 610.352f},
            {1118.126f, 612.911f}, {1117.429f, 615.935f}, {1116.731f, 621.285f},
            {1116.731f, 625.472f}, {1116.963f, 629.193f}, {1117.429f, 632.450f},
            {1117.661f, 635.706f}, {1118.824f, 640.126f}, {1119.755f, 643.615f},
            {1120.918f, 647.104f}, {1121.848f, 649.895f}, {1122.779f, 651.756f},
            {1123.942f, 654.082f}, {1125.105f, 656.641f}, {1123.476f, 657.106f},
            {1121.616f, 657.106f}, {1119.289f, 657.106f}, {1116.963f, 657.106f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.122f, 0.451f, 0.318f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.149f, 0.569f, 0.4f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1128.361f, 583.603f}, {1124.872f, 578.718f}, {1123.942f, 576.160f},
            {1123.244f, 573.834f}, {1121.848f, 571.508f}, {1120.452f, 568.949f},
            {1119.057f, 565.925f}, {1117.894f, 563.832f}, {1117.429f, 562.901f},
            {1119.289f, 559.180f}, {1121.383f, 554.993f}, {1123.244f, 552.201f},
            {1124.407f, 549.178f}, {1126.268f, 545.223f}, {1126.965f, 541.967f},
            {1128.826f, 539.641f}, {1129.291f, 536.617f}, {1130.454f, 533.361f},
            {1130.920f, 530.337f}, {1131.385f, 527.545f}, {1131.617f, 524.522f},
            {1132.315f, 522.196f}, {1133.013f, 518.939f}, {1133.478f, 516.613f},
            {1133.478f, 513.589f}, {1133.944f, 510.798f}, {1133.711f, 508.007f},
            {1135.339f, 513.124f}, {1136.502f, 516.846f}, {1137.898f, 520.567f},
            {1139.061f, 525.219f}, {1139.991f, 528.708f}, {1141.154f, 533.128f},
            {1142.085f, 538.013f}, {1143.248f, 543.828f}, {1143.480f, 548.247f},
            {1143.248f, 553.364f}, {1143.248f, 558.482f}, {1143.248f, 561.738f},
            {1142.782f, 564.064f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.169f, 0.529f, 0.384f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.224f, 0.722f, 0.522f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1067.651f, 487.072f}, {1062.348f, 468.371f}, {1063.185f, 465.301f},
            {1064.023f, 463.347f}, {1065.139f, 460.556f}, {1065.698f, 458.043f},
            {1065.977f, 456.090f}, {1066.535f, 453.019f}, {1067.093f, 449.670f},
            {1067.093f, 445.204f}, {1066.814f, 442.692f}, {1066.535f, 439.621f},
            {1066.256f, 437.667f}, {1065.698f, 435.713f}, {1064.023f, 433.201f},
            {1062.627f, 431.247f}, {1060.394f, 428.177f}, {1058.440f, 427.061f},
            {1057.324f, 424.548f}, {1055.928f, 423.711f}, {1054.812f, 421.757f},
            {1052.858f, 420.082f}, {1051.183f, 418.129f}, {1049.787f, 416.454f},
            {1048.671f, 414.779f}, {1047.554f, 413.104f}, {1046.438f, 408.359f},
            {1045.880f, 405.568f}, {1046.159f, 402.498f}, {1046.438f, 400.544f},
            {1046.438f, 400.544f}, {1046.996f, 397.752f}, {1047.275f, 396.357f},
            {1047.275f, 396.357f}, {1047.834f, 395.240f}, {1047.554f, 393.007f},
            {1046.717f, 391.054f}, {1045.601f, 389.379f}, {1044.205f, 387.704f},
            {1043.368f, 386.308f}, {1042.251f, 384.634f}, {1041.135f, 382.401f},
            {1039.739f, 380.447f}, {1038.902f, 378.772f}, {1037.785f, 377.376f},
            {1036.948f, 374.864f}, {1036.110f, 372.910f}, {1034.994f, 370.677f},
            {1033.877f, 368.724f}, {1033.319f, 367.607f}, {1031.923f, 366.491f},
            {1030.528f, 365.095f}, {1029.132f, 363.420f}, {1027.737f, 361.187f},
            {1026.341f, 359.233f}, {1025.224f, 357.559f}, {1024.666f, 355.884f},
            {1024.108f, 354.209f}, {1022.991f, 351.976f}, {1022.154f, 349.743f},
            {1021.596f, 348.068f}, {1021.317f, 346.114f}, {1021.038f, 344.440f},
            {1020.759f, 343.044f}, {1020.479f, 341.090f}, {1020.479f, 338.857f},
            {1020.479f, 337.462f}, {1020.479f, 335.508f}, {1020.479f, 332.716f},
            {1020.479f, 331.042f}, {1020.759f, 329.646f}, {1021.038f, 327.692f},
            {1021.317f, 326.017f}, {1021.596f, 324.064f}, {1022.154f, 321.551f},
            {1022.433f, 319.598f}, {1022.712f, 317.365f}, {1023.829f, 315.969f},
            {1025.224f, 313.457f}, {1025.224f, 313.457f}, {1025.783f, 311.782f},
            {1026.341f, 310.666f}, {1026.620f, 313.736f}, {1026.899f, 316.806f},
            {1027.178f, 319.598f}, {1027.178f, 322.389f}, {1027.457f, 326.017f},
            {1027.737f, 328.530f}, {1028.016f, 330.763f}, {1028.574f, 332.158f},
            {1029.132f, 333.833f}, {1029.690f, 336.066f}, {1030.249f, 338.020f},
            {1031.086f, 339.695f}, {1032.203f, 341.648f}, {1033.319f, 344.440f},
            {1033.877f, 345.835f}, {1034.994f, 347.231f}, {1036.110f, 350.022f},
            {1037.785f, 352.534f}, {1038.902f, 353.930f}, {1040.576f, 355.884f},
            {1041.414f, 357.000f}, {1042.530f, 357.838f}, {1044.205f, 359.512f},
            {1046.159f, 361.187f}, {1047.834f, 362.304f}, {1049.229f, 363.699f},
            {1051.462f, 364.816f}, {1053.695f, 366.770f}, {1055.928f, 368.444f},
            {1057.882f, 369.282f}, {1058.440f, 370.957f}, {1060.115f, 373.469f},
            {1061.511f, 375.981f}, {1062.906f, 377.935f}, {1064.302f, 380.168f},
            {1065.418f, 382.680f}, {1066.535f, 384.634f}, {1068.210f, 387.425f},
            {1069.047f, 389.937f}, {1069.605f, 391.612f}, {1070.443f, 392.728f},
            {1070.722f, 394.124f}, {1071.280f, 397.473f}, {1072.397f, 400.823f},
            {1073.513f, 404.451f}, {1073.513f, 408.638f}, {1074.071f, 411.709f},
            {1074.071f, 415.058f}, {1074.350f, 417.291f}, {1074.630f, 420.082f},
            {1074.630f, 422.874f}, {1075.188f, 425.386f}, {1075.467f, 428.735f},
            {1075.467f, 431.806f}, {1075.467f, 434.597f}, {1075.746f, 437.946f},
            {1075.746f, 441.017f}, {1075.746f, 445.204f}, {1075.746f, 449.111f},
            {1075.746f, 451.624f}, {1075.746f, 454.694f}, {1075.188f, 457.485f},
            {1074.630f, 461.114f}, {1073.792f, 466.696f}, {1072.676f, 470.883f},
            {1072.117f, 474.791f}, {1071.280f, 477.861f}, {1070.722f, 480.094f},
            {1069.884f, 482.606f}, {1069.047f, 484.281f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.118f, 0.49f, 0.341f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.145f, 0.612f, 0.424f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1125.765f, 656.166f}, {1124.090f, 653.821f}, {1122.416f, 651.142f},
            {1121.746f, 648.797f}, {1120.741f, 646.787f}, {1120.071f, 644.778f},
            {1119.066f, 642.098f}, {1118.731f, 639.419f}, {1117.391f, 634.059f},
            {1117.056f, 631.045f}, {1117.056f, 629.705f}, {1117.056f, 628.030f},
            {1116.722f, 626.021f}, {1117.056f, 623.676f}, {1117.056f, 622.336f},
            {1117.056f, 620.326f}, {1117.391f, 618.652f}, {1117.391f, 616.307f},
            {1118.061f, 613.962f}, {1118.396f, 611.953f}, {1119.066f, 609.608f},
            {1119.736f, 607.598f}, {1121.076f, 603.914f}, {1121.076f, 603.914f},
            {1122.081f, 601.234f}, {1123.086f, 598.220f}, {1124.760f, 594.870f},
            {1125.765f, 592.191f}, {1127.440f, 589.176f}, {1129.115f, 586.497f},
            {1130.454f, 584.152f}, {1131.794f, 581.472f}, {1133.469f, 579.463f},
            {1134.139f, 577.118f}, {1135.479f, 575.443f}, {1136.484f, 573.768f},
            {1137.488f, 571.759f}, {1139.163f, 569.749f}, {1140.503f, 567.404f},
            {1142.178f, 566.065f}, {1143.852f, 564.725f}, {1145.527f, 563.050f},
            {1147.537f, 559.701f}, {1148.877f, 557.021f}, {1150.886f, 555.681f},
            {1151.891f, 553.002f}, {1152.896f, 549.987f}, {1153.566f, 547.642f},
            {1154.236f, 544.963f}, {1154.571f, 542.618f}, {1154.571f, 540.273f},
            {1154.571f, 537.929f}, {1153.901f, 533.909f}, {1153.231f, 530.895f},
            {1152.896f, 528.550f}, {1152.226f, 526.206f}, {1151.891f, 522.521f},
            {1151.221f, 519.507f}, {1150.551f, 516.827f}, {1149.547f, 514.482f},
            {1149.547f, 512.473f}, {1148.207f, 509.793f}, {1148.207f, 507.783f},
            {1146.867f, 505.104f}, {1146.197f, 502.424f}, {1145.527f, 499.745f},
            {1145.192f, 498.070f}, {1144.857f, 495.390f}, {1144.522f, 493.046f},
            {1144.522f, 490.701f}, {1144.522f, 488.356f}, {1144.857f, 487.351f},
            {1144.857f, 485.677f}, {1144.857f, 483.667f}, {1144.857f, 481.992f},
            {1144.857f, 479.983f}, {1145.192f, 477.973f}, {1145.192f, 474.958f},
            {1145.527f, 472.614f}, {1145.862f, 470.939f}, {1146.532f, 469.264f},
            {1147.202f, 466.920f}, {1147.872f, 464.910f}, {1148.207f, 462.230f},
            {1149.212f, 459.216f}, {1150.551f, 456.871f}, {1151.556f, 455.196f},
            {1152.896f, 453.522f}, {1153.566f, 452.852f}, {1154.571f, 451.847f},
            {1155.241f, 451.177f}, {1156.246f, 450.172f}, {1157.250f, 448.832f},
            {1158.255f, 447.492f}, {1159.930f, 445.818f}, {1160.935f, 444.813f},
            {1162.275f, 443.473f}, {1162.945f, 441.463f}, {1163.280f, 439.119f},
            {1163.280f, 436.774f}, {1163.280f, 434.764f}, {1163.614f, 432.085f},
            {1163.949f, 429.740f}, {1164.284f, 428.065f}, {1164.954f, 426.726f},
            {1165.959f, 425.051f}, {1167.299f, 423.041f}, {1168.639f, 421.031f},
            {1169.644f, 420.027f}, {1170.983f, 419.022f}, {1171.988f, 417.347f},
            {1173.663f, 416.677f}, {1175.003f, 416.007f}, {1176.343f, 415.002f},
            {1177.347f, 415.002f}, {1178.352f, 413.663f}, {1179.357f, 411.318f},
            {1181.367f, 408.638f}, {1182.707f, 406.629f}, {1184.381f, 404.619f},
            {1186.056f, 401.939f}, {1187.731f, 398.255f}, {1189.406f, 395.240f},
            {1190.410f, 392.896f}, {1191.750f, 389.881f}, {1193.090f, 386.532f},
            {1193.425f, 384.187f}, {1192.420f, 395.910f}, {1191.750f, 398.925f},
            {1191.750f, 401.939f}, {1191.750f, 405.624f}, {1191.750f, 408.973f},
            {1192.420f, 411.318f}, {1193.090f, 415.002f}, {1194.095f, 417.347f},
            {1194.765f, 420.027f}, {1195.435f, 422.371f}, {1195.435f, 425.386f},
            {1195.770f, 428.065f}, {1196.105f, 432.420f}, {1196.439f, 436.104f},
            {1196.774f, 440.124f}, {1196.439f, 445.483f}, {1196.105f, 447.492f},
            {1194.765f, 452.182f}, {1193.425f, 454.191f}, {1192.420f, 456.871f},
            {1191.415f, 458.211f}, {1189.406f, 461.225f}, {1188.736f, 462.565f},
            {1187.396f, 464.910f}, {1186.056f, 467.589f}, {1185.386f, 469.599f},
            {1184.381f, 471.944f}, {1183.711f, 474.958f}, {1183.376f, 477.973f},
            {1183.376f, 479.983f}, {1184.716f, 480.652f}, {1188.736f, 480.987f},
            {1190.410f, 480.987f}, {1190.410f, 480.987f}, {1189.406f, 484.002f},
            {1188.066f, 486.012f}, {1186.391f, 488.021f}, {1185.051f, 490.366f},
            {1183.711f, 492.376f}, {1181.702f, 495.390f}, {1180.362f, 498.070f},
            {1178.352f, 501.419f}, {1177.347f, 503.764f}, {1176.343f, 505.104f},
            {1175.003f, 507.783f}, {1173.663f, 510.463f}, {1171.988f, 513.143f},
            {1170.983f, 516.157f}, {1169.309f, 520.511f}, {1167.634f, 525.201f},
            {1166.964f, 528.885f}, {1166.629f, 532.235f}, {1166.294f, 535.249f},
            {1166.294f, 538.264f}, {1166.294f, 540.943f}, {1166.294f, 544.963f},
            {1166.294f, 547.977f}, {1166.294f, 554.006f}, {1165.959f, 559.031f},
            {1165.959f, 562.715f}, {1163.949f, 567.404f}, {1163.280f, 570.084f},
            {1162.610f, 574.103f}, {1161.605f, 578.458f}, {1159.260f, 582.477f},
            {1157.920f, 585.827f}, {1156.581f, 589.176f}, {1155.241f, 591.856f},
            {1153.566f, 594.200f}, {1151.556f, 597.885f}, {1149.547f, 601.904f},
            {1147.537f, 605.254f}, {1145.527f, 607.933f}, {1143.852f, 610.613f},
            {1141.508f, 612.623f}, {1139.833f, 614.967f}, {1137.153f, 618.987f},
            {1135.144f, 622.001f}, {1132.799f, 625.686f}, {1131.794f, 628.700f},
            {1130.454f, 631.380f}, {1129.450f, 633.724f}, {1128.110f, 636.404f},
            {1127.775f, 639.084f}, {1127.105f, 642.098f}, {1126.770f, 645.113f},
            {1126.435f, 648.462f}, {1126.100f, 653.151f}, {1125.765f, 655.831f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.118f, 0.49f, 0.341f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.145f, 0.612f, 0.424f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1127.440f, 655.831f}, {1127.775f, 653.151f}, {1127.775f, 651.477f},
            {1127.775f, 649.802f}, {1127.775f, 647.792f}, {1127.775f, 646.118f},
            {1127.440f, 643.773f}, {1127.440f, 641.428f}, {1128.780f, 637.409f},
            {1128.780f, 637.409f}, {1129.115f, 635.734f}, {1129.450f, 634.394f},
            {1130.454f, 632.385f}, {1131.459f, 630.040f}, {1132.129f, 628.030f},
            {1132.799f, 627.025f}, {1133.804f, 625.016f}, {1135.144f, 623.006f},
            {1136.818f, 620.996f}, {1138.158f, 618.652f}, {1139.833f, 616.977f},
            {1141.508f, 614.967f}, {1142.513f, 613.292f}, {1143.517f, 611.953f},
            {1144.857f, 609.943f}, {1146.197f, 607.598f}, {1147.537f, 605.589f},
            {1149.547f, 603.579f}, {1150.551f, 601.234f}, {1151.556f, 599.560f},
            {1152.896f, 597.215f}, {1153.901f, 595.205f}, {1154.571f, 593.865f},
            {1155.241f, 592.191f}, {1156.246f, 590.516f}, {1157.250f, 588.506f},
            {1157.920f, 586.162f}, {1158.925f, 584.152f}, {1159.595f, 581.807f},
            {1160.265f, 580.132f}, {1160.935f, 578.123f}, {1161.605f, 575.778f},
            {1162.275f, 573.768f}, {1162.945f, 571.424f}, {1164.284f, 568.744f},
            {1165.289f, 566.065f}, {1165.624f, 564.390f}, {1165.959f, 561.375f},
            {1167.299f, 559.031f}, {1167.634f, 556.686f}, {1168.974f, 560.036f},
            {1168.974f, 563.050f}, {1169.978f, 567.069f}, {1170.313f, 570.084f},
            {1170.648f, 573.433f}, {1170.313f, 576.448f}, {1170.313f, 580.132f},
            {1169.978f, 583.147f}, {1168.974f, 585.492f}, {1167.969f, 587.836f},
            {1167.634f, 590.516f}, {1166.629f, 592.526f}, {1165.959f, 593.865f},
            {1164.284f, 597.550f}, {1162.610f, 599.560f}, {1160.935f, 602.239f},
            {1159.260f, 604.919f}, {1157.585f, 607.598f}, {1155.911f, 609.608f},
            {1154.236f, 611.953f}, {1152.561f, 613.627f}, {1150.551f, 615.302f},
            {1148.877f, 616.977f}, {1147.537f, 618.652f}, {1146.532f, 619.991f},
            {1145.527f, 621.331f}, {1144.522f, 622.336f}, {1142.848f, 623.676f},
            {1141.508f, 625.351f}, {1140.168f, 627.025f}, {1138.828f, 629.035f},
            {1137.488f, 631.045f}, {1135.814f, 633.724f}, {1134.474f, 636.069f},
            {1133.469f, 638.079f}, {1132.129f, 640.088f}, {1131.459f, 642.098f},
            {1130.454f, 644.108f}, {1129.785f, 646.787f}, {1128.780f, 649.467f},
            {1128.110f, 651.812f}, {1127.775f, 654.826f}, {1127.440f, 656.836f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.118f, 0.49f, 0.341f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.145f, 0.612f, 0.424f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1142.317f, 565.227f},
            {1142.782f, 562.203f}, {1143.248f, 560.343f}, {1143.480f, 558.249f},
            {1143.480f, 555.225f}, {1143.480f, 551.504f}, {1142.782f, 545.921f},
            {1142.317f, 542.199f}, {1141.852f, 539.408f}, {1141.619f, 537.082f},
            {1141.154f, 534.058f}, {1140.456f, 531.267f}, {1139.991f, 528.011f},
            {1138.596f, 523.359f}, {1137.433f, 519.637f}, {1135.107f, 512.891f},
            {1133.711f, 507.309f}, {1130.454f, 497.307f}, {1128.826f, 492.422f},
            {1126.965f, 487.538f}, {1125.337f, 482.885f}, {1123.942f, 478.466f},
            {1122.546f, 474.512f}, {1120.452f, 469.162f}, {1119.755f, 466.371f},
            {1118.824f, 461.951f}, {1118.824f, 461.951f}, {1118.824f, 460.323f},
            {1119.057f, 458.695f}, {1119.289f, 456.136f}, {1119.522f, 453.112f},
            {1120.452f, 447.995f}, {1120.452f, 447.995f}, {1120.452f, 445.436f},
            {1120.918f, 443.343f}, {1121.383f, 441.017f}, {1121.383f, 441.017f},
            {1121.616f, 438.923f}, {1121.848f, 435.667f}, {1122.546f, 432.643f},
            {1123.709f, 429.154f}, {1124.407f, 426.828f}, {1124.639f, 425.200f},
            {1125.337f, 423.804f}, {1125.802f, 422.176f}, {1126.733f, 420.315f},
            {1127.896f, 417.524f}, {1129.524f, 414.500f}, {1130.687f, 411.941f},
            {1131.850f, 409.150f}, {1133.944f, 406.591f}, {1135.339f, 403.568f},
            {1136.502f, 402.172f}, {1138.130f, 399.846f}, {1139.991f, 397.055f},
            {1141.154f, 395.194f}, {1143.248f, 392.403f}, {1144.876f, 390.309f},
            {1146.969f, 386.588f}, {1149.528f, 383.796f}, {1150.924f, 381.005f},
            {1152.087f, 377.981f}, {1154.413f, 374.260f}, {1156.041f, 371.236f},
            {1157.669f, 367.979f}, {1159.297f, 365.886f}, {1160.228f, 363.095f},
            {1160.693f, 361.234f}, {1161.623f, 358.908f}, {1162.321f, 357.279f},
            {1162.554f, 355.651f}, {1163.252f, 353.790f}, {1163.949f, 351.232f},
            {1164.880f, 347.277f}, {1165.112f, 344.254f}, {1165.578f, 341.695f},
            {1166.043f, 338.904f}, {1166.043f, 335.880f}, {1166.275f, 333.321f},
            {1166.508f, 330.297f}, {1166.275f, 326.343f}, {1166.508f, 322.854f},
            {1166.741f, 320.295f}, {1166.741f, 318.435f}, {1166.275f, 317.039f},
            {1166.043f, 314.946f}, {1165.810f, 313.783f}, {1166.741f, 316.806f},
            {1167.671f, 320.295f}, {1168.136f, 323.784f}, {1169.067f, 327.041f},
            {1170.230f, 330.297f}, {1171.393f, 334.019f}, {1172.556f, 337.508f},
            {1173.254f, 341.462f}, {1173.951f, 345.184f}, {1174.184f, 348.673f},
            {1174.417f, 351.464f}, {1174.417f, 354.256f}, {1174.184f, 357.745f},
            {1173.951f, 360.768f}, {1173.719f, 363.327f}, {1173.021f, 367.979f},
            {1172.323f, 370.770f}, {1171.393f, 374.260f}, {1170.230f, 377.283f},
            {1169.299f, 380.540f}, {1168.601f, 382.866f}, {1167.671f, 386.122f},
            {1165.578f, 389.379f}, {1164.647f, 391.937f}, {1162.786f, 395.426f},
            {1161.158f, 399.148f}, {1159.530f, 402.172f}, {1157.669f, 405.196f},
            {1156.273f, 407.754f}, {1154.645f, 410.313f}, {1153.250f, 412.639f},
            {1151.389f, 415.663f}, {1149.761f, 420.082f}, {1148.132f, 423.106f},
            {1146.504f, 426.130f}, {1145.806f, 429.154f}, {1144.411f, 431.480f},
            {1143.713f, 433.573f}, {1142.550f, 435.667f}, {1141.154f, 439.156f},
            {1139.991f, 442.645f}, {1139.061f, 446.599f}, {1137.898f, 449.623f},
            {1137.200f, 452.647f}, {1136.502f, 455.438f}, {1136.037f, 458.229f},
            {1136.037f, 461.253f}, {1136.037f, 463.812f}, {1136.502f, 466.603f},
            {1137.433f, 469.627f}, {1137.898f, 471.953f}, {1138.596f, 474.047f},
            {1139.526f, 476.373f}, {1140.224f, 478.699f}, {1141.387f, 481.257f},
            {1144.178f, 493.120f}, {1144.178f, 494.981f}, {1144.643f, 496.842f},
            {1144.876f, 498.470f}, {1145.574f, 501.261f}, {1146.504f, 504.285f},
            {1147.202f, 507.309f}, {1148.132f, 509.868f}, {1148.830f, 512.659f},
            {1150.226f, 516.148f}, {1150.924f, 519.172f}, {1152.087f, 522.893f},
            {1152.319f, 524.754f}, {1152.784f, 526.382f}, {1153.250f, 528.243f},
            {1153.482f, 530.802f}, {1153.947f, 532.895f}, {1154.180f, 535.687f},
            {1154.180f, 537.547f}, {1154.413f, 539.873f}, {1154.645f, 542.432f},
            {1154.180f, 544.293f}, {1153.482f, 546.386f}, {1152.784f, 548.480f},
            {1151.854f, 550.806f}, {1151.156f, 552.667f}, {1149.528f, 555.458f},
            {1147.667f, 557.784f}, {1146.039f, 561.040f}, {1144.411f, 563.599f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.114f, 0.49f, 0.341f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.176f, 0.741f, 0.514f, 1.0f);
    }
    {   
        static const float p[][2] = {
            {1159.918f, 362.668f}, {1159.918f, 361.311f}, {1159.530f, 358.404f},
            {1159.336f, 356.853f}, {1157.979f, 352.589f}, {1157.979f, 352.589f},
            {1157.398f, 350.844f}, {1156.429f, 348.906f}, {1155.459f, 346.773f},
            {1154.684f, 345.029f}, {1154.103f, 343.284f}, {1153.521f, 341.734f},
            {1152.746f, 339.795f}, {1151.970f, 338.051f}, {1151.389f, 336.112f},
            {1150.807f, 334.562f}, {1150.613f, 332.623f}, {1150.226f, 330.685f},
            {1149.838f, 328.359f}, {1149.644f, 326.033f}, {1149.257f, 324.288f},
            {1148.481f, 321.187f}, {1148.481f, 319.636f}, {1148.287f, 317.698f},
            {1148.287f, 312.852f}, {1147.900f, 310.526f}, {1147.706f, 306.843f},
            {1148.094f, 301.028f}, {1147.900f, 299.284f}, {1148.094f, 297.927f},
            {1148.287f, 296.764f}, {1148.481f, 294.825f}, {1148.481f, 293.662f},
            {1148.675f, 291.918f}, {1149.063f, 289.010f}, {1149.063f, 289.010f},
            {1149.257f, 287.266f}, {1149.838f, 284.940f}, {1149.838f, 284.940f},
            {1150.032f, 283.583f}, {1150.420f, 281.838f}, {1150.420f, 280.869f},
            {1150.613f, 279.900f}, {1151.001f, 278.737f}, {1151.389f, 277.380f},
            {1151.970f, 276.023f}, {1152.358f, 274.666f}, {1152.746f, 273.697f},
            {1152.939f, 272.922f}, {1153.133f, 271.759f}, {1153.715f, 270.208f},
            {1153.909f, 269.239f}, {1154.490f, 268.270f}, {1155.072f, 267.301f},
            {1155.653f, 266.331f}, {1156.235f, 264.199f}, {1156.622f, 263.036f},
            {1157.204f, 261.873f}, {1157.592f, 260.904f}, {1157.979f, 260.129f},
            {1158.561f, 258.966f}, {1159.336f, 257.803f}, {1160.111f, 255.864f},
            {1160.887f, 254.507f}, {1161.662f, 253.732f}, {1162.244f, 251.794f},
            {1163.213f, 251.018f}, {1163.988f, 250.631f}, {1164.570f, 250.243f},
            {1165.151f, 249.661f}, {1165.733f, 249.468f}, {1165.345f, 251.018f},
            {1164.764f, 252.957f}, {1164.570f, 254.701f}, {1164.376f, 256.058f},
            {1163.407f, 260.322f}, {1163.019f, 261.873f}, {1162.244f, 264.974f},
            {1161.856f, 266.913f}, {1161.856f, 269.239f}, {1161.081f, 273.309f},
            {1160.887f, 276.217f}, {1161.081f, 278.737f}, {1160.693f, 283.583f},
            {1160.499f, 286.103f}, {1160.693f, 289.786f}, {1160.887f, 292.693f},
            {1160.887f, 294.825f}, {1161.468f, 297.733f}, {1162.050f, 300.447f},
            {1162.825f, 304.323f}, {1163.213f, 306.262f}, {1163.988f, 308.588f},
            {1164.764f, 311.108f}, {1165.345f, 314.597f}, {1166.120f, 317.698f},
            {1166.314f, 320.799f}, {1166.508f, 324.482f}, {1166.508f, 327.390f},
            {1166.508f, 330.685f}, {1166.508f, 333.980f}, {1165.927f, 337.082f},
            {1165.927f, 339.989f}, {1165.733f, 342.509f}, {1165.539f, 344.835f},
            {1164.957f, 347.355f}, {1164.764f, 350.263f}, {1164.376f, 352.201f},
            {1163.988f, 354.333f}, {1162.825f, 357.241f}, {1162.050f, 359.373f},
            {1161.274f, 361.117f}, {1160.693f, 362.668f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.114f, 0.49f, 0.341f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.176f, 0.741f, 0.514f, 1.0f);
    }
    //? Left Leaft's lines
    {
        static const float p[][2] = {
            {995.470f, 649.802f}, {993.795f, 648.127f}, {991.785f, 646.787f},
            {990.446f, 646.118f}, {988.436f, 645.783f}, {986.761f, 645.448f},
            {985.756f, 645.448f}, {983.747f, 644.778f}, {982.407f, 644.108f},
            {981.067f, 643.103f}, {978.722f, 641.763f}, {976.713f, 640.423f},
            {974.703f, 639.084f}, {973.698f, 637.409f}, {972.358f, 636.404f},
            {971.688f, 635.399f}, {970.684f, 634.059f}, {970.349f, 633.389f},
            {970.014f, 632.385f}, {968.674f, 631.045f}, {968.339f, 629.035f},
            {967.334f, 627.025f}, {966.999f, 625.351f}, {966.664f, 624.681f},
            {965.994f, 623.006f}, {965.659f, 620.661f}, {965.659f, 618.317f},
            {965.659f, 616.307f}, {965.659f, 614.297f}, {965.324f, 611.953f},
            {965.659f, 608.938f}, {965.994f, 606.928f}, {966.329f, 604.584f},
            {966.329f, 603.244f}, {966.664f, 601.569f}, {966.999f, 599.225f},
            {966.664f, 601.569f}, {966.329f, 603.244f}, {966.329f, 604.584f},
            {965.994f, 606.928f}, {965.659f, 608.938f}, {965.324f, 611.953f},
            {965.659f, 614.297f}, {965.659f, 616.307f}, {965.659f, 618.317f},
            {965.659f, 620.661f}, {965.994f, 623.006f}, {966.664f, 624.681f},
            {966.999f, 625.351f}, {967.334f, 627.025f}, {968.339f, 629.035f},
            {968.674f, 631.045f}, {970.014f, 632.385f}, {970.349f, 633.389f},
            {970.684f, 634.059f}, {971.688f, 635.399f}, {972.358f, 636.404f},
            {973.698f, 637.409f}, {974.703f, 639.084f}, {976.713f, 640.423f},
            {978.722f, 641.763f}, {981.067f, 643.103f}, {982.407f, 644.108f},
            {983.747f, 644.778f}, {985.756f, 645.448f}, {986.761f, 645.448f},
            {988.436f, 645.783f}, {990.446f, 646.118f}, {991.785f, 646.787f},
            {993.795f, 648.127f}, {995.470f, 649.802f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.173f, 0.541f, 0.227f, 1.0f, 2.0f);
    }
    {
        static const float p[][2] = {
            {1006.188f, 650.807f}, {1006.188f, 648.462f}, {1005.853f, 645.783f},
            {1004.179f, 640.758f}, {1003.509f, 639.753f}, {1002.839f, 637.409f},
            {1001.834f, 635.064f}, {1001.164f, 632.720f}, {1000.494f, 629.370f},
            {999.489f, 626.690f}, {998.149f, 623.676f}, {997.145f, 621.331f},
            {997.145f, 618.987f}, {997.145f, 616.307f}, {997.145f, 613.962f},
            {997.480f, 611.618f}, {997.814f, 609.273f}, {998.819f, 605.924f},
            {999.489f, 602.574f}, {1000.494f, 599.225f}, {1001.834f, 595.875f},
            {1002.169f, 593.530f}, {1002.504f, 591.186f}, {1002.839f, 588.841f},
            {1004.179f, 584.487f}, {1004.848f, 582.812f}, {1005.518f, 581.472f},
            {1006.523f, 579.463f}, {1007.528f, 577.118f}, {1008.868f, 575.108f},
            {1009.873f, 572.764f}, {1010.877f, 569.749f}, {1011.882f, 566.400f},
            {1012.887f, 563.385f}, {1013.557f, 561.040f}, {1014.227f, 558.026f},
            {1014.227f, 554.676f}, {1014.897f, 551.327f}, {1014.562f, 548.647f},
            {1014.562f, 546.303f}, {1014.227f, 544.293f}, {1014.227f, 542.618f},
            {1014.897f, 541.278f}, {1015.567f, 539.604f}, {1016.572f, 537.929f},
            {1015.567f, 539.604f}, {1014.897f, 541.278f}, {1014.227f, 542.618f},
            {1014.227f, 544.293f}, {1014.562f, 546.303f}, {1014.562f, 548.647f},
            {1014.897f, 551.327f}, {1014.227f, 554.676f}, {1014.227f, 558.026f},
            {1013.557f, 561.040f}, {1012.887f, 563.385f}, {1011.882f, 566.400f},
            {1010.877f, 569.749f}, {1009.873f, 572.764f}, {1008.868f, 575.108f},
            {1007.528f, 577.118f}, {1006.523f, 579.463f}, {1005.518f, 581.472f},
            {1004.848f, 582.812f}, {1004.179f, 584.487f}, {1002.839f, 588.841f},
            {1002.504f, 591.186f}, {1002.169f, 593.530f}, {1001.834f, 595.875f},
            {1000.494f, 599.225f}, {999.489f, 602.574f}, {998.819f, 605.924f},
            {997.814f, 609.273f}, {997.480f, 611.618f}, {997.145f, 613.962f},
            {997.145f, 616.307f}, {997.145f, 618.987f}, {997.145f, 621.331f},
            {998.149f, 623.676f}, {999.489f, 626.690f}, {1000.494f, 629.370f},
            {1001.164f, 632.720f}, {1001.834f, 635.064f}, {1002.839f, 637.409f},
            {1003.509f, 639.753f}, {1004.179f, 640.758f}, {1005.853f, 645.783f},
            {1006.188f, 648.462f}, {1006.188f, 650.807f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.173f, 0.541f, 0.227f, 1.0f, 2.0f);
    }
    {
        static const float p[][2] = {
            {1006.188f, 650.137f}, {1006.858f, 647.792f}, {1006.858f, 645.113f},
            {1006.858f, 642.768f}, {1006.523f, 641.093f}, {1005.518f, 638.079f},
            {1004.848f, 635.734f}, {1004.848f, 633.724f}, {1004.513f, 631.715f},
            {1004.179f, 629.370f}, {1003.844f, 627.360f}, {1003.509f, 625.016f},
            {1003.509f, 623.676f}, {1003.509f, 621.666f}, {1003.509f, 618.987f},
            {1003.509f, 615.972f}, {1004.513f, 613.627f}, {1005.183f, 611.953f},
            {1006.523f, 608.938f}, {1007.863f, 605.254f}, {1012.552f, 599.894f},
            {1016.237f, 595.875f}, {1016.237f, 595.875f}, {1018.581f, 593.865f},
            {1019.921f, 592.526f}, {1021.596f, 589.846f}, {1022.936f, 587.501f},
            {1021.596f, 592.861f}, {1021.261f, 595.540f}, {1020.926f, 598.555f},
            {1020.591f, 601.569f}, {1020.591f, 604.249f}, {1020.591f, 606.593f},
            {1020.926f, 609.943f}, {1020.926f, 613.962f}, {1021.261f, 617.647f},
            {1020.926f, 622.001f}, {1019.921f, 625.351f}, {1018.916f, 628.365f},
            {1017.576f, 630.375f}, {1015.902f, 633.724f}, {1014.227f, 636.069f},
            {1011.882f, 640.088f}, {1009.873f, 643.103f}, {1008.533f, 646.118f},
            {1007.528f, 649.132f}, {1008.533f, 646.118f}, {1009.873f, 643.103f},
            {1011.882f, 640.088f}, {1014.227f, 636.069f}, {1015.902f, 633.724f},
            {1017.576f, 630.375f}, {1018.916f, 628.365f}, {1019.921f, 625.351f},
            {1020.926f, 622.001f}, {1021.261f, 617.647f}, {1020.926f, 613.962f},
            {1020.926f, 609.943f}, {1020.591f, 606.593f}, {1020.591f, 604.249f},
            {1020.591f, 601.569f}, {1020.926f, 598.555f}, {1021.261f, 595.540f},
            {1021.596f, 592.861f}, {1022.936f, 587.501f}, {1021.596f, 589.846f},
            {1019.921f, 592.526f}, {1018.581f, 593.865f}, {1016.237f, 595.875f},
            {1016.237f, 595.875f}, {1012.552f, 599.894f}, {1007.863f, 605.254f},
            {1006.523f, 608.938f}, {1005.183f, 611.953f}, {1004.513f, 613.627f},
            {1003.509f, 615.972f}, {1003.509f, 618.987f}, {1003.509f, 621.666f},
            {1003.509f, 623.676f}, {1003.509f, 625.016f}, {1003.844f, 627.360f},
            {1004.179f, 629.370f}, {1004.513f, 631.715f}, {1004.848f, 633.724f},
            {1004.848f, 635.734f}, {1005.518f, 638.079f}, {1006.523f, 641.093f},
            {1006.858f, 642.768f}, {1006.858f, 645.113f}, {1006.858f, 647.792f},
            {1006.188f, 650.137f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.173f, 0.541f, 0.227f, 1.0f, 2.0f);
    }

    //* Stones
    {
        static const float p[][2] = {
            {1005.779f, 655.478f}, {1006.477f, 653.849f}, {1007.174f, 652.686f},
            {1007.640f, 651.988f}, {1008.570f, 650.825f}, {1009.966f, 649.197f},
            {1011.361f, 648.034f}, {1012.292f, 647.104f}, {1013.920f, 646.406f},
            {1015.548f, 645.476f}, {1018.107f, 644.778f}, {1019.037f, 645.010f},
            {1020.665f, 644.778f}, {1022.061f, 645.243f}, {1023.224f, 645.708f},
            {1024.387f, 646.173f}, {1025.318f, 646.639f}, {1026.481f, 647.569f},
            {1027.411f, 648.499f}, {1028.341f, 649.895f}, {1029.039f, 651.058f},
            {1029.272f, 651.988f}, {1029.504f, 652.686f}, {1028.341f, 654.314f},
            {1027.411f, 654.780f}, {1025.783f, 655.478f}, {1023.689f, 656.408f},
            {1022.061f, 657.106f}, {1020.898f, 657.338f}, {1019.968f, 657.571f},
            {1018.805f, 657.804f}, {1017.176f, 657.804f}, {1014.618f, 658.501f},
            {1012.757f, 658.501f}, {1010.663f, 658.501f}, {1009.035f, 658.269f},
            {1007.174f, 657.571f}, {1006.244f, 657.106f}, {1005.779f, 656.641f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.388f, 0.341f, 0.42f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.588f, 0.573f, 0.506f, 1.0f);
    }
    {
        static const float p[][2] = {
            {989.497f, 652.454f}, {990.892f, 651.988f}, {992.055f, 651.523f},
            {993.683f, 651.291f}, {994.846f, 651.291f}, {996.242f, 651.291f},
            {996.940f, 651.523f}, {998.103f, 651.756f}, {999.033f, 652.221f},
            {999.964f, 652.686f}, {1001.127f, 653.384f}, {1002.057f, 654.082f},
            {1003.220f, 655.012f}, {1004.151f, 655.943f}, {1004.616f, 656.873f},
            {1004.151f, 658.269f}, {1002.988f, 659.432f}, {1002.290f, 660.130f},
            {1001.592f, 660.595f}, {1000.429f, 661.293f}, {999.499f, 661.525f},
            {997.638f, 661.758f}, {996.242f, 661.990f}, {995.312f, 661.990f},
            {993.916f, 662.223f}, {992.288f, 662.223f}, {990.427f, 661.990f},
            {989.031f, 661.525f}, {988.101f, 661.060f}, {987.171f, 660.362f},
            {986.240f, 659.432f}, {985.542f, 658.501f}, {985.310f, 656.873f},
            {985.310f, 656.175f}, {986.007f, 655.012f}, {986.705f, 654.314f},
            {987.636f, 653.384f}, {988.334f, 653.151f}, {988.334f, 653.151f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.388f, 0.341f, 0.42f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.588f, 0.573f, 0.506f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1029.737f, 639.195f}, {1030.667f, 638.730f}, {1030.667f, 638.730f},
            {1031.598f, 638.265f}, {1032.761f, 638.497f}, {1033.924f, 638.265f},
            {1035.320f, 638.497f}, {1036.250f, 638.963f}, {1037.646f, 639.195f},
            {1038.809f, 639.893f}, {1039.506f, 640.358f}, {1041.135f, 641.754f},
            {1043.228f, 643.149f}, {1044.391f, 644.313f}, {1044.856f, 645.243f},
            {1045.089f, 646.639f}, {1045.089f, 648.034f}, {1044.856f, 649.430f},
            {1044.158f, 650.825f}, {1042.763f, 651.523f}, {1041.135f, 651.988f},
            {1039.274f, 652.919f}, {1037.413f, 652.919f}, {1035.552f, 652.919f},
            {1033.924f, 652.919f}, {1032.296f, 652.686f}, {1031.133f, 652.221f},
            {1029.737f, 651.523f}, {1028.807f, 650.360f}, {1028.341f, 649.197f},
            {1027.411f, 647.569f}, {1026.248f, 647.104f}, {1024.852f, 646.173f},
            {1023.922f, 645.708f}, {1022.526f, 645.243f}, {1021.596f, 644.778f},
            {1021.131f, 644.080f}, {1022.294f, 643.382f}, {1023.224f, 642.684f},
            {1024.387f, 641.986f}, {1025.085f, 641.521f}, {1026.481f, 640.823f},
            {1027.876f, 639.660f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.388f, 0.341f, 0.42f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.588f, 0.573f, 0.506f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1094.866f, 665.945f}, {1092.773f, 665.712f}, {1090.912f, 665.479f},
            {1089.516f, 665.479f}, {1087.655f, 665.479f}, {1085.329f, 664.782f},
            {1083.701f, 664.316f}, {1082.073f, 663.619f}, {1080.910f, 662.688f},
            {1079.747f, 661.758f}, {1078.816f, 660.362f}, {1077.886f, 658.967f},
            {1078.119f, 657.571f}, {1078.584f, 656.408f}, {1079.514f, 655.245f},
            {1080.677f, 654.547f}, {1081.840f, 654.082f}, {1083.236f, 653.617f},
            {1085.097f, 653.151f}, {1086.725f, 652.919f}, {1088.586f, 652.919f},
            {1089.981f, 652.686f}, {1092.307f, 653.151f}, {1094.168f, 653.151f},
            {1096.262f, 654.082f}, {1098.123f, 655.012f}, {1099.983f, 655.710f},
            {1126.733f, 656.175f}, {1128.361f, 656.641f}, {1129.291f, 657.571f},
            {1130.222f, 658.269f}, {1131.385f, 659.664f}, {1132.315f, 661.060f},
            {1132.315f, 662.223f}, {1132.548f, 663.851f}, {1132.083f, 664.782f},
            {1131.385f, 666.410f}, {1130.222f, 668.038f}, {1128.594f, 668.969f},
            {1127.198f, 669.666f}, {1124.872f, 670.132f}, {1117.429f, 670.597f},
            {1110.683f, 671.062f}, {1108.124f, 671.062f}, {1106.496f, 671.062f},
            {1105.101f, 671.062f}, {1103.705f, 671.062f}, {1101.844f, 670.829f},
            {1100.216f, 670.597f}, {1099.053f, 670.132f}, {1098.123f, 669.434f},
            {1097.425f, 668.736f}, {1096.960f, 667.573f}, {1096.727f, 666.410f},
            {1095.796f, 665.945f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.388f, 0.341f, 0.42f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.588f, 0.573f, 0.506f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1100.681f, 659.897f}, {1101.844f, 659.199f}, {1102.542f, 658.036f},
            {1103.472f, 657.571f}, {1104.868f, 656.873f}, {1106.031f, 656.641f},
            {1107.194f, 656.408f}, {1108.357f, 656.408f}, {1109.985f, 656.175f},
            {1111.381f, 656.175f}, {1113.242f, 656.408f}, {1115.568f, 656.408f},
            {1116.498f, 656.408f}, {1119.289f, 656.175f}, {1120.918f, 656.175f},
            {1122.779f, 656.408f}, {1124.872f, 656.175f}, {1126.733f, 656.408f},
            {1128.128f, 656.873f}, {1129.524f, 657.338f}, {1130.454f, 658.036f},
            {1131.385f, 659.199f}, {1132.083f, 659.897f}, {1132.548f, 661.293f},
            {1132.548f, 662.456f}, {1132.548f, 664.084f}, {1131.850f, 665.712f},
            {1131.385f, 666.875f}, {1130.454f, 667.806f}, {1128.826f, 668.503f},
            {1127.663f, 669.201f}, {1125.802f, 669.434f}, {1124.174f, 669.666f},
            {1122.779f, 669.434f}, {1121.616f, 669.666f}, {1119.755f, 669.666f},
            {1117.661f, 670.132f}, {1115.800f, 670.364f}, {1114.172f, 670.597f},
            {1113.009f, 670.597f}, {1110.683f, 670.829f}, {1106.031f, 671.295f},
            {1103.240f, 671.062f}, {1099.983f, 670.597f}, {1099.053f, 669.899f},
            {1097.657f, 668.969f}, {1097.425f, 667.340f}, {1097.192f, 666.177f},
            {1097.425f, 665.247f}, {1097.425f, 664.316f}, {1097.890f, 663.153f},
            {1098.355f, 662.223f}, {1099.053f, 661.293f}, {1099.983f, 660.827f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.388f, 0.341f, 0.42f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.588f, 0.573f, 0.506f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1131.850f, 646.173f}, {1133.013f, 645.941f}, {1134.409f, 645.476f},
            {1136.270f, 645.243f}, {1137.898f, 645.010f}, {1140.224f, 645.010f},
            {1142.317f, 646.173f}, {1144.411f, 647.104f}, {1145.574f, 648.034f},
            {1146.969f, 648.965f}, {1148.132f, 649.662f}, {1150.458f, 650.825f},
            {1151.389f, 652.454f}, {1152.087f, 653.849f}, {1151.854f, 655.245f},
            {1151.156f, 656.408f}, {1149.761f, 657.106f}, {1148.132f, 657.804f},
            {1145.341f, 658.501f}, {1143.015f, 659.199f}, {1139.759f, 659.664f},
            {1137.433f, 659.897f}, {1135.107f, 659.664f}, {1133.478f, 659.664f},
            {1132.083f, 658.269f}, {1129.989f, 656.873f}, {1127.663f, 655.710f},
            {1128.128f, 654.547f}, {1128.826f, 653.384f}, {1129.757f, 652.221f},
            {1130.687f, 651.291f}, {1131.152f, 649.895f}, {1131.385f, 648.732f},
            {1132.083f, 647.336f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.388f, 0.341f, 0.42f, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.588f, 0.573f, 0.506f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1135.339f, 660.362f}, {1136.735f, 660.362f}, {1137.665f, 660.362f},
            {1138.596f, 660.130f}, {1140.689f, 659.897f}, {1142.317f, 659.664f},
            {1143.945f, 659.432f}, {1145.108f, 659.199f}, {1146.504f, 658.734f},
            {1148.132f, 658.269f}, {1149.295f, 658.269f}, {1150.691f, 658.269f},
            {1152.087f, 658.269f}, {1153.017f, 658.967f}, {1154.645f, 659.664f},
            {1154.878f, 661.060f}, {1155.110f, 662.688f}, {1154.878f, 664.316f},
            {1153.715f, 665.479f}, {1152.087f, 666.410f}, {1149.993f, 667.108f},
            {1147.900f, 667.340f}, {1146.504f, 667.573f}, {1145.341f, 667.806f},
            {1143.945f, 667.806f}, {1142.085f, 668.038f}, {1139.991f, 667.806f},
            {1138.363f, 667.573f}, {1136.967f, 667.573f}, {1135.804f, 667.340f},
            {1134.409f, 666.642f}, {1133.944f, 665.479f}, {1133.711f, 663.851f},
            {1133.711f, 662.456f}, {1133.944f, 661.293f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.388f, 0.341f, 0.42f, 1.0f, 8.0f);
        drawFilledTessCall(p, n, 0.588f, 0.573f, 0.506f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1179.534f, 639.660f}, {1181.162f, 639.893f}, {1182.325f, 639.893f},
            {1183.488f, 639.660f}, {1184.651f, 639.660f}, {1185.814f, 639.660f},
            {1186.977f, 639.660f}, {1188.140f, 639.428f}, {1189.303f, 639.428f},
            {1190.699f, 639.428f}, {1191.862f, 639.195f}, {1193.490f, 639.195f},
            {1194.420f, 638.963f}, {1195.584f, 638.497f}, {1196.514f, 638.265f},
            {1197.910f, 637.567f}, {1199.305f, 637.334f}, {1200.933f, 637.567f},
            {1201.864f, 638.265f}, {1203.027f, 639.428f}, {1203.957f, 639.893f},
            {1205.120f, 641.056f}, {1206.283f, 642.452f}, {1207.446f, 644.080f},
            {1208.144f, 645.708f}, {1208.377f, 647.104f}, {1207.912f, 648.499f},
            {1207.446f, 649.895f}, {1206.748f, 651.291f}, {1205.585f, 651.988f},
            {1203.957f, 652.221f}, {1201.864f, 652.919f}, {1200.701f, 652.919f},
            {1198.375f, 652.919f}, {1198.375f, 652.919f}, {1196.049f, 652.686f},
            {1194.653f, 652.686f}, {1193.025f, 652.686f}, {1191.164f, 652.919f},
            {1189.071f, 653.384f}, {1187.442f, 653.617f}, {1186.047f, 653.849f},
            {1185.116f, 654.547f}, {1183.488f, 655.012f}, {1181.860f, 655.245f},
            {1180.929f, 655.245f}, {1179.301f, 655.478f}, {1176.975f, 655.012f},
            {1175.114f, 654.082f}, {1173.719f, 653.384f}, {1172.556f, 652.221f},
            {1171.625f, 651.291f}, {1170.928f, 650.593f}, {1170.230f, 649.197f},
            {1170.462f, 647.104f}, {1171.160f, 645.941f}, {1172.091f, 644.778f},
            {1173.254f, 643.615f}, {1174.649f, 642.684f}, {1175.812f, 641.986f},
            {1176.743f, 641.521f}, {1177.440f, 640.591f}, {1178.138f, 640.126f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineLoop(p, n, 0.388f, 0.341f, 0.42f, 1.0f, 8.0f);
        drawFilledTessCall(p, n, 0.588f, 0.573f, 0.506f, 1.0f);
    }
    {
        static const float p[][2] = {
            {989.962f, 655.710f}, {990.427f, 654.780f}, {991.125f, 653.849f},
            {992.288f, 653.151f}, {993.218f, 653.151f}, {994.149f, 653.151f},
            {995.312f, 653.384f}, {996.242f, 653.617f}, {997.405f, 653.617f},
            {998.103f, 653.617f}, {999.033f, 653.617f}, {1000.196f, 653.849f},
            {1000.662f, 654.314f}, {1001.592f, 655.245f}, {1002.522f, 655.710f},
            {1003.918f, 656.408f}, {1002.988f, 657.338f}, {1001.127f, 657.106f},
            {999.964f, 656.873f}, {998.568f, 656.408f}, {997.405f, 655.710f},
            {996.475f, 655.478f}, {990.427f, 656.175f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.745f, 0.71f, 0.769f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1011.594f, 650.825f}, {1012.524f, 649.895f}, {1012.757f, 648.732f},
            {1013.687f, 648.034f}, {1015.083f, 647.569f}, {1016.479f, 646.639f},
            {1017.874f, 646.639f}, {1019.502f, 646.871f}, {1020.898f, 647.104f},
            {1020.433f, 648.499f}, {1019.270f, 649.197f}, {1017.642f, 650.128f},
            {1016.711f, 650.360f}, {1015.316f, 650.825f}, {1013.687f, 651.291f},
            {1011.827f, 651.291f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.745f, 0.71f, 0.769f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1022.759f, 643.615f}, {1023.224f, 642.219f}, {1024.155f, 641.754f},
            {1025.318f, 640.823f}, {1026.248f, 640.358f}, {1027.876f, 640.126f},
            {1029.272f, 639.660f}, {1030.900f, 639.195f}, {1032.063f, 639.195f},
            {1034.156f, 639.660f}, {1037.646f, 640.126f}, {1037.646f, 640.126f},
            {1038.111f, 641.289f}, {1036.017f, 642.452f}, {1033.691f, 643.149f},
            {1031.598f, 643.149f}, {1029.039f, 643.149f}, {1026.713f, 643.149f},
            {1024.852f, 643.149f}, {1023.457f, 643.615f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.745f, 0.71f, 0.769f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1098.588f, 663.619f}, {1099.518f, 661.990f}, {1100.449f, 661.293f},
            {1102.775f, 658.967f}, {1102.775f, 658.967f}, {1104.635f, 658.036f},
            {1105.566f, 657.571f}, {1118.359f, 656.408f}, {1119.987f, 657.106f},
            {1120.918f, 657.571f}, {1121.150f, 658.269f}, {1120.918f, 659.199f},
            {1120.220f, 660.362f}, {1118.126f, 662.223f}, {1115.335f, 663.386f},
            {1113.009f, 663.619f}, {1110.683f, 664.084f}, {1108.124f, 664.084f},
            {1105.566f, 664.316f}, {1103.007f, 664.549f}, {1100.449f, 664.084f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.745f, 0.71f, 0.769f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1128.826f, 654.547f}, {1129.059f, 653.151f}, {1131.152f, 647.336f},
            {1132.780f, 646.871f}, {1133.944f, 646.173f}, {1135.339f, 645.708f},
            {1136.967f, 645.476f}, {1137.898f, 645.708f}, {1138.828f, 645.941f},
            {1139.991f, 645.941f}, {1141.154f, 646.639f}, {1142.085f, 647.336f},
            {1143.480f, 648.267f}, {1144.411f, 648.965f}, {1144.876f, 649.430f},
            {1143.248f, 649.895f}, {1141.619f, 649.662f}, {1139.759f, 649.197f},
            {1138.363f, 649.197f}, {1134.176f, 649.662f}, {1134.176f, 649.662f},
            {1133.013f, 650.593f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.745f, 0.71f, 0.769f, 1.0f);
    }
    {
        static const float p[][2] = {
            {1172.323f, 647.336f}, {1173.486f, 645.476f}, {1176.045f, 643.847f},
            {1177.440f, 642.452f}, {1178.603f, 641.986f}, {1179.069f, 641.754f},
            {1180.929f, 641.289f}, {1182.790f, 641.289f}, {1184.186f, 641.056f},
            {1186.047f, 641.056f}, {1189.071f, 641.056f}, {1191.164f, 640.823f},
            {1193.723f, 640.591f}, {1195.118f, 640.591f}, {1196.979f, 639.428f},
            {1196.979f, 639.428f}, {1197.677f, 640.126f}, {1198.142f, 641.521f},
            {1198.840f, 642.452f}, {1199.538f, 643.382f}, {1199.770f, 644.545f},
            {1198.607f, 645.708f}, {1194.886f, 646.871f}, {1194.886f, 646.871f},
            {1192.560f, 646.871f}, {1190.466f, 647.336f}, {1188.373f, 647.336f},
            {1185.582f, 648.034f}, {1183.488f, 648.034f}, {1181.627f, 648.267f},
            {1179.301f, 648.267f}, {1177.440f, 648.267f}, {1175.347f, 648.267f},
            {1173.254f, 648.034f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawFilledTessCall(p, n, 0.745f, 0.71f, 0.769f, 1.0f);
    }
    {
        static const float p[][2] = {
            {999.266f, 612.120f}, {1000.382f, 609.608f}, {1002.615f, 604.305f},
            {1007.361f, 597.606f}, {1013.222f, 588.674f}, {1016.572f, 578.346f},
            {1021.317f, 569.414f}, {1023.829f, 562.715f}, {1026.620f, 555.179f},
            {1028.295f, 546.247f}, {1028.853f, 539.548f}, {1028.295f, 531.732f},
            {1027.178f, 524.196f}, {1026.062f, 518.055f}, {1023.829f, 513.310f},
            {1022.712f, 506.890f}, {1021.596f, 500.191f}, {1021.596f, 492.097f},
            {1022.712f, 484.839f}, {1025.504f, 476.745f}, {1026.341f, 468.371f},
            {1028.853f, 462.509f}, {1029.970f, 456.369f}, {1031.086f, 449.391f},
            {1032.203f, 443.529f}, {1032.203f, 440.459f}, {1034.156f, 446.878f},
            {1035.552f, 451.903f}, {1034.715f, 457.485f}, {1033.877f, 465.301f},
            {1033.877f, 469.767f}, {1033.319f, 472.837f}, {1033.319f, 477.582f},
            {1032.203f, 484.560f}, {1031.644f, 488.747f}, {1031.644f, 493.213f},
            {1032.203f, 497.400f}, {1033.877f, 499.633f}, {1035.831f, 504.099f},
            {1037.785f, 507.448f}, {1038.902f, 511.914f}, {1038.902f, 515.264f},
            {1038.902f, 520.009f}, {1038.902f, 527.266f}, {1038.622f, 532.570f},
            {1038.343f, 537.594f}, {1038.622f, 542.339f}, {1038.902f, 546.526f},
            {1039.460f, 551.271f}, {1039.739f, 555.737f}, {1039.739f, 559.645f},
            {1038.902f, 563.273f}, {1037.785f, 565.506f}, {1036.948f, 567.739f},
            {1036.389f, 571.368f}, {1033.319f, 575.555f}, {1029.970f, 580.300f},
            {1027.737f, 582.533f}, {1025.224f, 585.603f}, {1019.642f, 591.465f},
            {1014.897f, 595.931f}, {1010.989f, 600.955f}, {1006.523f, 606.538f},
            {1005.407f, 610.445f}, {1004.011f, 612.678f}, {1002.057f, 617.424f},
            {1001.499f, 622.448f}, {1001.220f, 624.960f}, {999.824f, 620.215f},
            {999.545f, 617.982f}, {998.987f, 614.353f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineStrip(p, n, 0.031, 0.541, 0.184, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.086, 0.631, 0.247, 1.0f);
    }
    {
        static const float p[][2] = {
            {989.217f, 646.732f}, {985.589f, 645.894f}, {982.798f, 644.499f},
            {980.844f, 643.103f}, {978.890f, 641.707f}, {974.703f, 638.079f},
            {974.703f, 638.079f}, {973.307f, 636.404f}, {972.191f, 634.171f},
            {970.516f, 632.775f}, {969.120f, 630.542f}, {968.004f, 628.030f},
            {966.887f, 625.797f}, {966.329f, 622.169f}, {966.050f, 618.261f},
            {965.771f, 615.749f}, {965.771f, 612.678f}, {966.329f, 609.887f},
            {966.329f, 607.375f}, {966.329f, 604.584f}, {967.167f, 601.234f},
            {968.004f, 597.885f}, {969.120f, 594.814f}, {969.958f, 591.186f},
            {970.237f, 588.953f}, {969.958f, 586.720f}, {969.120f, 584.208f},
            {967.167f, 581.696f}, {964.934f, 579.742f}, {962.421f, 577.788f},
            {959.909f, 575.555f}, {957.955f, 574.159f}, {955.722f, 571.926f},
            {954.048f, 569.972f}, {951.815f, 568.018f}, {949.861f, 564.669f},
            {947.907f, 562.994f}, {947.070f, 561.040f}, {945.395f, 556.853f},
            {944.557f, 554.900f}, {943.720f, 552.108f}, {943.441f, 549.317f},
            {943.441f, 545.968f}, {943.720f, 543.456f}, {944.278f, 540.106f},
            {945.395f, 536.477f}, {947.070f, 532.291f}, {949.023f, 529.220f},
            {950.140f, 526.708f}, {951.256f, 524.754f}, {951.536f, 523.079f},
            {950.698f, 531.453f}, {950.140f, 534.244f}, {950.698f, 538.431f},
            {950.698f, 542.060f}, {951.256f, 545.409f}, {953.210f, 548.480f},
            {954.327f, 552.387f}, {956.281f, 555.179f}, {957.955f, 557.970f},
            {959.909f, 561.040f}, {962.421f, 564.111f}, {964.934f, 566.902f},
            {966.887f, 569.693f}, {969.400f, 572.484f}, {970.795f, 574.997f},
            {972.749f, 577.788f}, {974.703f, 581.696f}, {976.378f, 585.045f},
            {977.773f, 588.953f}, {978.890f, 592.023f}, {980.006f, 595.931f},
            {981.402f, 600.118f}, {981.402f, 602.630f}, {981.123f, 604.863f},
            {980.285f, 608.771f}, {979.727f, 611.841f}, {979.169f, 614.632f},
            {979.448f, 617.982f}, {980.006f, 621.890f}, {980.844f, 625.797f},
            {982.518f, 628.309f}, {984.472f, 631.938f}, {985.589f, 633.334f},
            {985.868f, 635.008f}, {987.264f, 637.800f}, {988.380f, 641.986f},
            {989.776f, 645.615f}, {989.776f, 647.011f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineStrip(p, n, 0.031, 0.541, 0.184, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.086, 0.631, 0.247, 1.0f);
    }
    {
        static const float p[][2] = {
            {1006.244f, 578.625f}, {1006.244f, 576.113f}, {1006.523f, 573.880f},
            {1006.523f, 571.647f}, {1006.802f, 567.181f}, {1006.523f, 562.994f},
            {1006.244f, 559.366f}, {1006.244f, 555.737f}, {1005.686f, 552.108f},
            {1004.848f, 549.038f}, {1004.290f, 546.247f}, {1003.174f, 543.176f},
            {1001.499f, 540.385f}, {1000.382f, 537.594f}, {998.429f, 535.361f},
            {997.312f, 532.570f}, {995.358f, 530.058f}, {993.683f, 526.987f},
            {992.567f, 523.079f}, {990.334f, 519.730f}, {988.938f, 516.380f},
            {988.380f, 512.473f}, {987.543f, 509.681f}, {986.426f, 506.890f},
            {985.310f, 502.145f}, {982.239f, 493.213f}, {977.773f, 480.373f},
            {975.819f, 474.791f}, {974.424f, 472.279f}, {974.145f, 471.441f},
            {973.307f, 470.325f}, {981.402f, 459.718f}, {982.239f, 461.672f},
            {982.239f, 464.742f}, {982.798f, 466.417f}, {983.635f, 468.371f},
            {984.472f, 470.046f}, {985.310f, 472.000f}, {986.426f, 473.395f},
            {987.543f, 475.349f}, {988.380f, 477.303f}, {988.380f, 478.978f},
            {988.380f, 480.652f}, {988.380f, 482.606f}, {988.101f, 484.560f},
            {987.543f, 487.072f}, {986.984f, 489.864f}, {987.264f, 491.259f},
            {988.101f, 492.934f}, {989.776f, 494.050f}, {991.450f, 495.167f},
            {992.846f, 496.004f}, {995.358f, 497.400f}, {997.033f, 499.075f},
            {997.312f, 500.470f}, {997.312f, 501.866f}, {997.591f, 503.541f},
            {997.312f, 504.936f}, {997.033f, 506.611f}, {996.475f, 508.565f},
            {996.475f, 511.356f}, {996.754f, 513.310f}, {997.312f, 514.706f},
            {998.149f, 516.101f}, {998.987f, 517.497f}, {999.824f, 518.893f},
            {1001.220f, 520.288f}, {1002.336f, 521.963f}, {1003.453f, 523.359f},
            {1004.290f, 524.475f}, {1005.407f, 525.871f}, {1006.802f, 527.545f},
            {1008.198f, 529.499f}, {1009.314f, 531.174f}, {1010.710f, 533.686f},
            {1012.385f, 536.757f}, {1013.501f, 538.990f}, {1014.339f, 540.943f},
            {1014.618f, 543.176f}, {1014.897f, 545.130f}, {1014.897f, 547.084f},
            {1015.176f, 549.038f}, {1015.176f, 550.992f}, {1015.176f, 552.946f},
            {1015.176f, 554.900f}, {1014.618f, 557.412f}, {1014.060f, 559.924f},
            {1012.943f, 562.715f}, {1012.106f, 565.785f}, {1011.268f, 569.414f},
            {1010.431f, 571.368f}, {1009.314f, 572.484f}, {1009.035f, 574.159f},
            {1007.919f, 575.834f}, {1007.361f, 577.788f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawOutlineStrip(p, n, 0.031, 0.541, 0.184, 1.0f, 4.0f);
        drawFilledTessCall(p, n, 0.086, 0.631, 0.247, 1.0f);
    }
    {
        static const float p[][2] = {
            {1078.816f, 634.450f}, {1077.979f, 628.309f}, {1076.583f, 621.052f},
            {1074.630f, 612.120f}, {1074.350f, 603.746f}, {1074.630f, 598.722f},
            {1074.630f, 592.023f}, {1075.467f, 587.557f}, {1076.863f, 581.975f},
            {1077.979f, 576.392f}, {1079.375f, 569.414f}, {1080.491f, 562.157f},
            {1081.329f, 555.737f}, {1081.887f, 550.713f}, {1081.887f, 548.759f},
            {1081.329f, 545.130f}, {1081.049f, 540.664f}, {1080.491f, 537.315f},
            {1078.816f, 533.128f}, {1076.863f, 529.778f}, {1074.909f, 526.150f},
            {1072.676f, 522.242f}, {1070.164f, 517.218f}, {1061.790f, 502.982f},
            {1058.440f, 495.167f}, {1056.207f, 487.351f}, {1056.207f, 483.723f},
            {1056.207f, 482.048f}, {1056.207f, 483.723f}, {1056.207f, 487.351f},
            {1058.440f, 495.167f}, {1061.790f, 502.982f}, {1070.164f, 517.218f},
            {1072.676f, 522.242f}, {1074.909f, 526.150f}, {1076.863f, 529.778f},
            {1078.816f, 533.128f}, {1080.491f, 537.315f}, {1081.049f, 540.664f},
            {1081.329f, 545.130f}, {1081.887f, 548.759f}, {1081.887f, 550.713f},
            {1081.329f, 555.737f}, {1080.491f, 562.157f}, {1079.375f, 569.414f},
            {1077.979f, 576.392f}, {1076.863f, 581.975f}, {1075.467f, 587.557f},
            {1074.630f, 592.023f}, {1074.630f, 598.722f}, {1074.350f, 603.746f},
            {1074.630f, 612.120f}, {1076.583f, 621.052f}, {1077.979f, 628.309f},
            {1078.816f, 634.450f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawLeafBone(p, n, 0.114, 0.62, 0.416, 0.133, 0.722, 0.486, 1.0f, 2.0f, 4.0f);
    }
    {
        static const float p[][2] = {
            {1093.052f, 561.319f}, {1096.122f, 541.781f}, {1095.564f, 529.778f},
            {1096.122f, 523.359f}, {1096.960f, 517.218f}, {1098.076f, 512.752f},
            {1099.193f, 508.007f}, {1100.588f, 503.820f}, {1101.705f, 500.191f},
            {1103.379f, 495.446f}, {1105.891f, 491.817f}, {1103.379f, 495.446f},
            {1101.705f, 500.191f}, {1100.588f, 503.820f}, {1099.193f, 508.007f},
            {1098.076f, 512.752f}, {1096.960f, 517.218f}, {1096.122f, 523.359f},
            {1095.564f, 529.778f}, {1096.122f, 541.781f}, {1093.052f, 561.319f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawLeafBone(p, n, 0.114, 0.62, 0.416, 0.133, 0.722, 0.486, 1.0f, 2.0f, 4.0f);
    }
    {
        static const float p[][2] = {
            {1065.698f, 476.745f}, {1069.605f, 463.626f}, {1070.164f, 455.531f},
            {1071.280f, 448.553f}, {1071.280f, 442.133f}, {1070.164f, 435.434f},
            {1069.326f, 428.177f}, {1068.489f, 420.920f}, {1067.372f, 413.104f},
            {1064.302f, 405.010f}, {1061.790f, 398.311f}, {1058.999f, 392.170f},
            {1056.486f, 385.471f}, {1053.416f, 381.005f}, {1051.183f, 378.214f},
            {1045.601f, 371.794f}, {1042.251f, 368.165f}, {1037.785f, 363.141f},
            {1033.598f, 357.838f}, {1031.923f, 353.930f}, {1029.690f, 350.022f},
            {1028.295f, 346.114f}, {1026.899f, 341.648f}, {1026.620f, 337.182f},
            {1025.783f, 330.483f}, {1025.504f, 326.855f}, {1025.504f, 322.389f},
            {1025.504f, 326.855f}, {1025.783f, 330.483f}, {1026.620f, 337.182f},
            {1026.899f, 341.648f}, {1028.295f, 346.114f}, {1029.690f, 350.022f},
            {1031.923f, 353.930f}, {1033.598f, 357.838f}, {1037.785f, 363.141f},
            {1042.251f, 368.165f}, {1045.601f, 371.794f}, {1051.183f, 378.214f},
            {1053.416f, 381.005f}, {1056.486f, 385.471f}, {1058.999f, 392.170f},
            {1061.790f, 398.311f}, {1064.302f, 405.010f}, {1067.372f, 413.104f},
            {1068.489f, 420.920f}, {1069.326f, 428.177f}, {1070.164f, 435.434f},
            {1071.280f, 442.133f}, {1071.280f, 448.553f}, {1070.164f, 455.531f},
            {1069.605f, 463.626f}, {1065.698f, 476.745f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawLeafBone(p, n, 0.086, 0.51, 0.271, 0.122, 0.588, 0.325, 1.0f, 2.0f, 4.0f);
    }
    {
        static const float p[][2] = {
            {1113.149f, 520.288f}, {1113.707f, 515.543f}, {1113.986f, 511.356f},
            {1114.544f, 505.774f}, {1114.265f, 499.633f}, {1112.590f, 495.446f},
            {1111.195f, 489.584f}, {1108.404f, 484.002f}, {1106.171f, 478.140f},
            {1103.938f, 473.674f}, {1101.984f, 469.208f}, {1100.867f, 463.905f},
            {1098.913f, 459.718f}, {1098.076f, 454.973f}, {1096.401f, 449.949f},
            {1096.122f, 446.041f}, {1095.564f, 442.133f}, {1095.564f, 436.272f},
            {1095.285f, 430.131f}, {1096.122f, 425.944f}, {1096.960f, 420.362f},
            {1098.355f, 415.616f}, {1099.472f, 411.988f}, {1101.146f, 407.243f},
            {1102.821f, 402.498f}, {1104.496f, 396.636f}, {1105.891f, 389.937f},
            {1106.729f, 384.355f}, {1108.124f, 379.330f}, {1108.683f, 375.143f},
            {1109.241f, 370.957f}, {1109.520f, 365.653f}, {1110.078f, 362.304f},
            {1110.078f, 358.117f}, {1110.078f, 353.372f}, {1109.799f, 348.347f},
            {1108.683f, 342.765f}, {1108.124f, 337.741f}, {1107.287f, 331.879f},
            {1104.775f, 326.017f}, {1103.658f, 320.714f}, {1102.263f, 315.969f},
            {1100.588f, 309.828f}, {1098.634f, 302.292f}, {1097.518f, 296.989f},
            {1095.285f, 289.452f}, {1093.052f, 273.263f}, {1092.214f, 266.285f},
            {1092.494f, 261.819f}, {1092.214f, 251.770f}, {1092.494f, 249.258f},
            {1093.331f, 243.676f}, {1094.727f, 236.977f}, {1096.401f, 230.557f},
            {1098.355f, 224.137f}, {1100.309f, 217.717f}, {1102.821f, 212.135f},
            {1105.054f, 207.669f}, {1105.891f, 205.715f}, {1105.054f, 207.669f},
            {1102.821f, 212.135f}, {1100.309f, 217.717f}, {1098.355f, 224.137f},
            {1096.401f, 230.557f}, {1094.727f, 236.977f}, {1093.331f, 243.676f},
            {1092.494f, 249.258f}, {1092.214f, 251.770f}, {1092.494f, 261.819f},
            {1092.214f, 266.285f}, {1093.052f, 273.263f}, {1095.285f, 289.452f},
            {1097.518f, 296.989f}, {1098.634f, 302.292f}, {1100.588f, 309.828f},
            {1102.263f, 315.969f}, {1103.658f, 320.714f}, {1104.775f, 326.017f},
            {1107.287f, 331.879f}, {1108.124f, 337.741f}, {1108.683f, 342.765f},
            {1109.799f, 348.347f}, {1110.078f, 353.372f}, {1110.078f, 358.117f},
            {1110.078f, 362.304f}, {1109.520f, 365.653f}, {1109.241f, 370.957f},
            {1108.683f, 375.143f}, {1108.124f, 379.330f}, {1106.729f, 384.355f},
            {1105.891f, 389.937f}, {1104.496f, 396.636f}, {1102.821f, 402.498f},
            {1101.146f, 407.243f}, {1099.472f, 411.988f}, {1098.355f, 415.616f},
            {1096.960f, 420.362f}, {1096.122f, 425.944f}, {1095.285f, 430.131f},
            {1095.564f, 436.272f}, {1095.564f, 442.133f}, {1096.122f, 446.041f},
            {1096.401f, 449.949f}, {1098.076f, 454.973f}, {1098.913f, 459.718f},
            {1100.867f, 463.905f}, {1101.984f, 469.208f}, {1103.938f, 473.674f},
            {1106.171f, 478.140f}, {1108.404f, 484.002f}, {1111.195f, 489.584f},
            {1112.590f, 495.446f}, {1114.265f, 499.633f}, {1114.544f, 505.774f},
            {1113.986f, 511.356f}, {1113.707f, 515.543f}, {1113.149f, 520.288f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawLeafBone(p, n, 0.114, 0.62, 0.416, 0.133, 0.722, 0.486, 1.0f, 2.0f, 4.0f);
    }
    {
        static const float p[][2] = {
            {1132.967f, 506.611f}, {1132.408f, 502.703f}, {1131.850f, 496.004f},
            {1130.734f, 489.584f}, {1130.175f, 483.444f}, {1129.059f, 476.466f},
            {1127.105f, 470.046f}, {1126.826f, 468.092f}, {1127.105f, 465.022f},
            {1127.384f, 461.951f}, {1128.221f, 456.369f}, {1129.896f, 449.391f},
            {1131.013f, 444.087f}, {1131.850f, 439.063f}, {1133.525f, 434.876f},
            {1135.479f, 428.735f}, {1139.107f, 419.803f}, {1142.457f, 415.058f},
            {1143.852f, 412.546f}, {1149.156f, 407.522f}, {1153.064f, 403.614f},
            {1156.971f, 398.869f}, {1159.763f, 394.682f}, {1162.833f, 391.333f},
            {1165.345f, 387.704f}, {1168.415f, 382.680f}, {1170.369f, 378.214f},
            {1168.415f, 382.680f}, {1165.345f, 387.704f}, {1162.833f, 391.333f},
            {1159.763f, 394.682f}, {1156.971f, 398.869f}, {1153.064f, 403.614f},
            {1149.156f, 407.522f}, {1143.852f, 412.546f}, {1142.457f, 415.058f},
            {1139.107f, 419.803f}, {1135.479f, 428.735f}, {1133.525f, 434.876f},
            {1131.850f, 439.063f}, {1131.013f, 444.087f}, {1129.896f, 449.391f},
            {1128.221f, 456.369f}, {1127.384f, 461.951f}, {1127.105f, 465.022f},
            {1126.826f, 468.092f}, {1127.105f, 470.046f}, {1129.059f, 476.466f},
            {1130.175f, 483.444f}, {1130.734f, 489.584f}, {1131.850f, 496.004f},
            {1132.408f, 502.703f}, {1132.967f, 506.611f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawLeafBone(p, n, 0.114, 0.62, 0.416, 0.133, 0.722, 0.486, 1.0f, 2.0f, 4.0f);
    }
    {
        static const float p[][2] = {
            {1164.787f, 549.317f}, {1163.112f, 541.223f}, {1161.158f, 533.407f},
            {1160.321f, 525.871f}, {1160.042f, 518.055f}, {1160.321f, 509.961f},
            {1160.321f, 502.145f}, {1160.600f, 494.050f}, {1161.158f, 487.631f},
            {1161.716f, 481.769f}, {1163.112f, 475.628f}, {1165.066f, 471.441f},
            {1167.299f, 466.696f}, {1170.090f, 461.951f}, {1173.719f, 455.531f},
            {1177.627f, 447.995f}, {1179.860f, 443.250f}, {1182.372f, 436.830f},
            {1184.605f, 432.085f}, {1186.000f, 425.386f}, {1187.117f, 419.524f},
            {1188.512f, 414.500f}, {1188.791f, 410.871f}, {1189.350f, 405.010f},
            {1191.862f, 393.007f}, {1189.350f, 405.010f}, {1188.791f, 410.871f},
            {1188.512f, 414.500f}, {1187.117f, 419.524f}, {1186.000f, 425.386f},
            {1184.605f, 432.085f}, {1182.372f, 436.830f}, {1179.860f, 443.250f},
            {1177.627f, 447.995f}, {1173.719f, 455.531f}, {1170.090f, 461.951f},
            {1167.299f, 466.696f}, {1165.066f, 471.441f}, {1163.112f, 475.628f},
            {1161.716f, 481.769f}, {1161.158f, 487.631f}, {1160.600f, 494.050f},
            {1160.321f, 502.145f}, {1160.321f, 509.961f}, {1160.042f, 518.055f},
            {1160.321f, 525.871f}, {1161.158f, 533.407f}, {1163.112f, 541.223f},
            {1164.787f, 549.317f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawLeafBone(p, n, 0.086, 0.51, 0.271, 0.122, 0.588, 0.325, 1.0f, 2.0f, 4.0f);
    }
    {
        static const float p[][2] = {
            {1185.721f, 638.637f}, {1184.046f, 635.567f}, {1182.651f, 633.334f},
            {1180.418f, 629.705f}, {1178.185f, 625.518f}, {1176.789f, 622.448f},
            {1175.673f, 619.098f}, {1175.394f, 616.307f}, {1175.114f, 613.237f},
            {1174.556f, 609.050f}, {1173.440f, 598.164f}, {1173.719f, 591.186f},
            {1173.998f, 587.836f}, {1174.277f, 585.045f}, {1175.114f, 581.696f},
            {1176.510f, 576.392f}, {1178.743f, 570.810f}, {1180.976f, 564.669f},
            {1183.488f, 557.133f}, {1186.558f, 549.038f}, {1189.908f, 540.943f},
            {1192.141f, 533.686f}, {1193.257f, 527.825f}, {1194.932f, 520.846f},
            {1196.328f, 511.635f}, {1197.444f, 504.657f}, {1199.119f, 497.958f},
            {1199.956f, 492.934f}, {1200.236f, 488.189f}, {1200.515f, 484.281f},
            {1200.515f, 480.932f}, {1200.794f, 478.140f}, {1201.073f, 475.907f},
            {1201.073f, 475.070f}, {1201.073f, 475.907f}, {1200.794f, 478.140f},
            {1200.515f, 480.932f}, {1200.515f, 484.281f}, {1200.236f, 488.189f},
            {1199.956f, 492.934f}, {1199.119f, 497.958f}, {1197.444f, 504.657f},
            {1196.328f, 511.635f}, {1194.932f, 520.846f}, {1193.257f, 527.825f},
            {1192.141f, 533.686f}, {1189.908f, 540.943f}, {1186.558f, 549.038f},
            {1183.488f, 557.133f}, {1180.976f, 564.669f}, {1178.743f, 570.810f},
            {1176.510f, 576.392f}, {1175.114f, 581.696f}, {1174.277f, 585.045f},
            {1173.998f, 587.836f}, {1173.719f, 591.186f}, {1173.440f, 598.164f},
            {1174.556f, 609.050f}, {1175.114f, 613.237f}, {1175.394f, 616.307f},
            {1175.673f, 619.098f}, {1176.789f, 622.448f}, {1178.185f, 625.518f},
            {1180.418f, 629.705f}, {1182.651f, 633.334f}, {1184.046f, 635.567f},
            {1185.721f, 638.637f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawLeafBone(p, n, 0.012, 0.459, 0.204, 0.055, 0.549, 0.271, 1.0f, 2.0f, 4.0f);
    }
    {
        static const float p[][2] = {
            {1218.937f, 521.684f}, {1220.891f, 517.776f}, {1222.286f, 512.752f},
            {1224.519f, 508.844f}, {1226.194f, 504.657f}, {1229.823f, 490.422f},
            {1230.660f, 482.048f}, {1231.498f, 474.512f}, {1231.777f, 469.208f},
            {1230.381f, 463.626f}, {1229.823f, 458.323f}, {1228.427f, 454.973f},
            {1227.032f, 451.344f}, {1226.473f, 444.087f}, {1225.078f, 439.342f},
            {1224.519f, 433.760f}, {1223.682f, 428.456f}, {1223.682f, 422.315f},
            {1222.566f, 417.291f}, {1221.449f, 412.825f}, {1220.333f, 408.638f},
            {1221.449f, 412.825f}, {1222.566f, 417.291f}, {1223.682f, 422.315f},
            {1223.682f, 428.456f}, {1224.519f, 433.760f}, {1225.078f, 439.342f},
            {1226.473f, 444.087f}, {1227.032f, 451.344f}, {1228.427f, 454.973f},
            {1229.823f, 458.323f}, {1230.381f, 463.626f}, {1231.777f, 469.208f},
            {1231.498f, 474.512f}, {1230.660f, 482.048f}, {1229.823f, 490.422f},
            {1226.194f, 504.657f}, {1224.519f, 508.844f}, {1222.286f, 512.752f},
            {1220.891f, 517.776f}, {1218.937f, 521.684f}
        };
        int n = (int)(sizeof(p) / sizeof(p[0]));
        drawLeafBone(p, n, 0.012, 0.459, 0.204, 0.055, 0.549, 0.271, 1.0f, 2.0f, 4.0f);
    }
}

//! ================= DISPLAY & MAIN  =================
void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();

    water();
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);

    glPushMatrix();
    glScalef((float)winW / 1301.0f, (float)winH / 764.0f, 1.0f);
    glTranslatef(0.0f, 764.0f, 0.0f);
    glScalef(1.0f, -1.0f, 1.0f);

    if (dayMode) {
        drawSunRays();
        drawSun();
    }

    drawTopMiddleSand();
    drawFullBigSand();
    drawSandDarkEdge();
    drawBottomLeftSand();
    drawBottomRightSand();
    drawBottomRightUpperSand();
    drawBottomLeftUpperSand();

    drawDarkStones();
    drawLightStones();
    drawSeashell();
    drawStarfish();

    float octCenterX = 573.0f;
    float octCenterY = 430.0f;
    glPushMatrix();
        glTranslatef(octAnim.swayX, octAnim.bobY, 0.0f);
        glTranslatef(octCenterX, octCenterY, 0.0f);
        glRotatef(octAnim.tiltAngle, 0.0f, 0.0f, 1.0f);
        glTranslatef(-octCenterX, -octCenterY, 0.0f);
        drawOctopus();
    glPopMatrix();

    drawTurtle();
    drawGreenLeaft();
    drawCoral();

    drawFish();
    drawBubbles();
    drawFood();
    drawRightLeaf();

    if (!dayMode) {
        drawNightOverlay();
        drawMoon();
    }

    glPopMatrix();
    glutSwapBuffers();
}

void timer(int v){
    if(!paused){ 
        gTime += 0.022f;
        updateFish();
        updateBubbles();
        updateOctopus();
        glutPostRedisplay();
    }
    updateTurtle();
    glutTimerFunc(16, timer, 0);
}

void reshape(int w, int h){
    winW = w; winH = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y){
    (void)x; (void)y;
    switch (key) {
    case 27:  exit(0); break;
    case ' ': paused = !paused; break;

    case 'd': case 'D': dayMode = 1; break;
    case 'n': case 'N': dayMode = 0; break;

    case 'b': case 'B': releaseBubbles(); break;

    case '1': currentSpecies = 0; break;
    case '2': currentSpecies = 1; break;
    case '3': currentSpecies = 2; break;
    case '4': currentSpecies = 3; break;
    case '5': currentSpecies = 4; break;

    case '+': case '=':
        if(fishCount < MAX_FISH) fishCount++;
        break;
    case '-': case '_':
        if(fishCount > 1) fishCount--;
        break;

    case 'f':
        foodX = rand() % 1200 + 50;
        foodY = rand() % 400  + 170;
        foodActive = 1;
        break;

    case 'r': initFish(); break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int mx, int my){
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        foodX = (float)mx * (1301.0f / (float)winW);
        foodY = (float)my * ( 764.0f / (float)winH);
        foodActive = 1;
    }
}

int main(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 20);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Deep Sea Aquarium");
    glClearColor(0, 0, 0, 1);

    initFish();
    initBubbles();
    releaseBubbles();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutTimerFunc(16, timer, 0);
    glutMainLoop();
    return 0;
}