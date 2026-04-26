#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ========== SCENE LAYOUT CONSTANTS ========== */
#define W             1200
#define H             700
#define BORDER_LEFT   25
#define BORDER_RIGHT  1175
#define BORDER_BOTTOM 45
#define BORDER_TOP    655
#define FLOOR_Y       130        /* sandy floor top edge */
#define SURFACE_Y     590        /* water surface wave Y */
#define SUN_X         780.0f
#define SUN_Y         620.0f
#define SUN_RADIUS    45.0f
#define TITLE_Y       670
#define HUD_Y         18

/* ========== OBJECT CAPACITIES ========== */
#define MAX_FISH      30
#define MAX_BUBBLES   60
#define MAX_SEAWEED   8
#define MAX_CORAL     6
#define MAX_ROCKS     10
#define MAX_FOOD      5
#define MAX_RAYS      7

/* ========== DATA STRUCTURES ========== */
typedef struct {
    float x, y, dx, dy, size;
    float r, g, b;
    int   type;          /* 1=orange 2=blue 3=yellow 4=pink */
    int   facingRight;
    float tailAngle;
    int   alive;
} Fish;

typedef struct {
    float x, y, radius, speed, alpha;
    int   alive;
} Bubble;

typedef struct {
    float baseX, baseY, height, phase;
    float r, g, b;
} Seaweed;

typedef struct {
    float x, y, size;
    float r, g, b;
    int   type;          /* 0=branching 1=round 2=tube */
} Coral;

typedef struct {
    float x, y, size;
} Rock;

typedef struct {
    float x, y, dy, timer;
    int   active;
} FoodParticle;

typedef struct {
    float alpha;
    float startX, startY;
    float endX1, endY1;
    float endX2, endY2;
} LightRay;

/* ========== GLOBAL STATE ========== */
Fish         fishArray[MAX_FISH];
Bubble       bubbleArray[MAX_BUBBLES];
Seaweed      seaweedArray[MAX_SEAWEED];
Coral        coralArray[MAX_CORAL];
Rock         rockArray[MAX_ROCKS];
FoodParticle foodArray[MAX_FOOD];
LightRay     lightRays[MAX_RAYS];

int   fishCount      = 6;
int   currentSpecies = 1;
int   isNight        = 0;
int   isPaused       = 0;
float globalTime     = 0.0f;
int   windowW        = W;
int   windowH        = H;

/* Octopus state */
float octX = 480.0f, octY = 300.0f;
float octSize = 55.0f, tentaclePhase = 0.0f;

/* Turtle state */
float turtX = 720.0f, turtY = 280.0f;
float turtSize = 45.0f, turtDX = 0.3f;
float flipperAngle = 0.0f;

/* ========== OCTOPUS POLYGON DATA (pre-traced outline) ========== */
static const float octopus[][2] = {
    {408.5f,237.0f},{408.6f,233.5f},{408.4f,230.1f},{407.7f,227.8f},
    {407.2f,225.7f},{405.3f,223.1f},{404.1f,222.1f},{402.4f,220.5f},
    {399.9f,218.1f},{398.6f,217.7f},{396.8f,216.9f},{395.1f,216.5f},
    {393.5f,216.2f},{391.3f,215.8f},{388.8f,215.1f},{387.1f,215.1f},
    {384.9f,215.1f},{383.3f,215.3f},{381.2f,215.8f},{378.8f,216.2f},
    {376.2f,216.5f},{374.3f,217.0f},{372.2f,218.3f},{369.8f,220.2f},
    {371.0f,219.1f},{368.0f,221.2f},{366.3f,222.8f},{364.8f,224.5f},
    {363.9f,225.7f},{362.8f,227.3f},{360.8f,229.4f},{361.6f,228.3f},
    {360.1f,230.6f},{359.2f,232.3f},{358.3f,233.5f},{358.0f,234.9f},
    {357.1f,237.0f},{356.3f,238.9f},{355.7f,239.8f},{354.4f,239.6f},
    {353.1f,239.8f},{352.3f,240.3f},{351.2f,241.5f},{350.4f,242.9f},
    {349.1f,243.9f},{348.1f,246.5f},{348.1f,245.5f},{347.6f,247.6f},
    {347.2f,249.6f},{347.4f,252.1f},{348.6f,254.0f},{348.1f,255.7f},
    {347.4f,258.3f},{346.9f,259.9f},{346.0f,261.4f},{345.3f,262.6f},
    {344.1f,263.3f},{342.7f,264.5f},{341.5f,265.1f},{340.0f,265.4f},
    {338.2f,265.8f},{336.3f,265.8f},{334.6f,265.8f},{332.9f,265.2f},
    {331.1f,264.5f},{329.0f,263.7f},{327.5f,262.1f},{326.1f,260.0f},
    {324.7f,258.5f},{323.5f,256.4f},{321.6f,253.8f},{322.1f,254.8f},
    {320.4f,252.6f},{319.2f,251.4f},{318.1f,250.5f},{316.9f,249.8f},
    {316.0f,249.3f},{314.8f,249.1f},{313.1f,248.4f},{311.7f,248.3f},
    {310.1f,248.1f},{308.4f,247.9f},{306.8f,248.3f},{305.8f,249.3f},
    {304.8f,249.8f},{303.6f,250.3f},{302.3f,252.1f},{302.7f,251.0f},
    {301.3f,252.4f},{300.6f,253.6f},{299.9f,254.7f},{299.2f,255.5f},
    {298.9f,256.2f},{298.7f,257.1f},{298.7f,258.1f},{298.7f,259.2f},
    {299.2f,259.9f},{299.7f,260.6f},{300.4f,261.3f},{300.8f,259.9f},
    {301.1f,259.0f},{302.0f,258.1f},{302.5f,257.3f},{303.4f,256.2f},
    {304.9f,255.2f},{304.1f,255.5f},{305.8f,255.0f},{307.5f,254.8f},
    {308.8f,254.1f},{310.1f,254.0f},{311.5f,254.1f},{313.1f,255.0f},
    {313.8f,255.7f},{315.2f,256.9f},{316.4f,258.3f},{317.3f,259.3f},
    {317.8f,260.9f},{318.1f,262.3f},{318.8f,264.4f},{320.0f,265.8f},
    {320.7f,267.3f},{321.6f,268.4f},{322.6f,269.6f},{324.4f,271.0f},
    {326.1f,272.0f},{327.5f,273.0f},{329.6f,275.0f},{328.3f,273.7f},
    {328.7f,275.8f},{327.7f,276.3f},{326.4f,276.7f},{325.2f,277.0f},
    {324.0f,277.2f},{322.5f,277.6f},{321.2f,277.7f},{319.7f,278.2f},
    {317.6f,278.6f},{315.9f,278.9f},{314.3f,280.0f},{313.1f,280.3f},
    {311.0f,281.4f},{312.1f,280.7f},{310.0f,281.7f},{308.6f,282.4f},
    {307.4f,282.8f},{306.0f,283.4f},{304.6f,284.8f},{305.1f,284.0f},
    {303.9f,285.0f},{302.7f,286.0f},{301.8f,287.1f},{301.0f,288.0f},
    {299.9f,289.0f},{299.2f,290.0f},{298.4f,291.1f},{297.5f,292.3f},
    {297.0f,293.2f},{296.3f,294.0f},{295.9f,294.9f},{295.4f,295.6f},
    {294.7f,297.1f},{294.4f,298.2f},{294.0f,299.2f},{293.7f,300.6f},
    {293.3f,302.3f},{293.0f,304.2f},{293.0f,306.8f},{293.8f,310.8f},
    {293.8f,310.8f},{293.3f,309.3f},{294.4f,311.9f},{295.8f,313.4f},
    {296.8f,314.0f},{298.2f,314.0f},{299.0f,313.1f},{300.1f,311.9f},
    {300.1f,310.3f},{299.2f,310.5f},{298.5f,310.8f},{297.5f,311.2f},
    {297.3f,310.5f},{297.1f,309.4f},{297.1f,308.4f},{297.1f,308.4f},
    {297.3f,305.5f},{297.3f,305.5f},{297.1f,306.5f},{297.5f,304.6f},
    {297.8f,303.0f},{298.0f,301.8f},{298.5f,300.8f},{299.0f,299.7f},
    {299.7f,299.0f},{301.1f,297.7f},{301.8f,296.6f},{302.9f,295.9f},
    {304.6f,295.9f},{306.2f,296.1f},{307.2f,295.8f},{307.7f,294.9f},
    {308.4f,293.8f},{309.1f,293.2f},{310.1f,292.3f},{311.7f,290.9f},
    {312.2f,290.4f},{313.8f,291.1f},{314.8f,291.6f},{316.0f,291.8f},
    {317.3f,291.6f},{317.4f,290.6f},{317.6f,289.7f},{317.3f,288.8f},
    {316.4f,288.3f},{317.6f,288.1f},{319.3f,287.6f},{320.0f,287.6f},
    {321.2f,288.0f},{322.3f,289.0f},{323.3f,289.9f},{324.5f,290.2f},
    {325.6f,289.9f},{326.4f,289.3f},{326.6f,288.3f},{327.1f,287.3f},
    {328.3f,286.6f},{330.8f,287.3f},{330.8f,287.3f},{332.0f,286.9f},
    {333.0f,286.9f},{333.9f,286.0f},{334.4f,286.2f},{334.2f,287.1f},
    {333.9f,288.5f},{333.5f,289.9f},{333.2f,290.9f},{333.2f,291.9f},
    {333.0f,293.0f},{333.0f,293.0f},{333.0f,293.7f},{332.7f,300.3f},
    {332.7f,302.0f},{333.2f,303.6f},{333.7f,305.8f},{334.6f,308.2f},
    {334.6f,311.0f},{334.4f,313.6f},{334.2f,315.9f},{333.7f,318.6f},
    {332.3f,320.7f},{331.8f,321.8f},{331.5f,322.3f},{330.8f,322.6f},
    {330.1f,323.5f},{329.0f,324.0f},{328.0f,324.7f},{322.8f,324.9f},
    {321.2f,324.0f},{320.7f,323.3f},{320.5f,322.5f},{320.5f,321.6f},
    {320.5f,320.7f},{320.5f,319.7f},{320.7f,318.5f},{321.1f,317.3f},
    {320.2f,317.1f},{319.3f,317.3f},{318.5f,317.8f},{317.4f,318.6f},
    {316.7f,319.9f},{316.6f,321.1f},{316.2f,324.9f},{316.2f,324.9f},
    {316.6f,326.4f},{317.3f,327.5f},{318.5f,328.5f},{319.2f,329.4f},
    {320.2f,330.4f},{321.2f,331.1f},{329.4f,331.5f},{331.6f,330.8f},
    {333.0f,330.3f},{334.8f,329.4f},{336.3f,328.3f},{337.7f,327.5f},
    {338.9f,326.3f},{340.1f,324.9f},{341.2f,322.8f},{341.9f,320.7f},
    {342.7f,318.6f},{342.9f,316.6f},{343.1f,315.5f},{343.4f,313.4f},
    {343.6f,311.5f},{343.4f,310.1f},{343.4f,308.4f},{343.3f,306.7f},
    {344.1f,306.0f},{343.6f,305.5f},{343.4f,304.1f},{343.6f,302.3f},
    {343.8f,300.1f},{343.9f,298.4f},{344.3f,295.9f},{345.3f,291.9f},
    {347.4f,288.6f},{350.0f,285.5f},{351.6f,284.1f},{353.1f,283.3f},
    {354.0f,283.1f},{355.7f,283.4f},{355.7f,295.4f},{358.0f,300.3f},
    {359.6f,302.7f},{360.8f,305.1f},{362.3f,307.4f},{363.7f,308.8f},
    {365.4f,310.5f},{367.2f,311.5f},{369.8f,313.6f},{372.6f,316.7f},
    {373.8f,319.5f},{375.0f,322.3f},{375.3f,324.4f},{375.0f,327.0f},
    {374.3f,329.7f},{372.9f,332.0f},{371.3f,333.2f},{370.3f,333.2f},
    {369.1f,333.2f},{367.5f,332.3f},{367.2f,330.9f},{367.2f,329.7f},
    {367.0f,328.7f},{366.0f,328.9f},{365.1f,329.6f},{364.2f,330.6f},
    {363.9f,332.7f},{363.9f,335.3f},{367.5f,338.1f},{370.1f,338.7f},
    {371.5f,338.7f},{368.6f,338.9f},{372.2f,338.6f},{373.2f,338.1f},
    {374.5f,337.5f},{376.5f,336.0f},{375.7f,336.3f},{376.9f,335.5f},
    {377.9f,334.4f},{379.0f,333.0f},{380.0f,331.8f},{380.9f,330.1f},
    {381.6f,328.2f},{382.6f,323.8f},{382.6f,323.8f},{382.4f,325.4f},
    {382.8f,322.8f},{382.6f,321.2f},{382.8f,320.4f},{382.6f,319.0f},
    {382.4f,317.9f},{381.9f,316.7f},{381.6f,315.3f},{381.2f,314.1f},
    {380.0f,312.7f},{378.8f,311.4f},{377.8f,309.8f},{376.7f,308.6f},
    {375.2f,307.0f},{373.6f,304.9f},{372.6f,303.4f},{371.2f,301.3f},
    {370.3f,299.2f},{370.1f,298.5f},{369.8f,297.0f},{369.4f,295.4f},
    {369.4f,294.0f},{369.4f,292.6f},{369.4f,291.2f},{369.4f,289.3f},
    {369.6f,288.0f},{370.0f,286.6f},{370.6f,285.5f},{372.9f,285.4f},
    {375.7f,287.1f},{377.9f,289.2f},{378.8f,291.6f},{380.4f,294.4f},
    {382.1f,297.3f},{383.5f,300.3f},{384.2f,302.9f},{384.9f,306.0f},
    {384.9f,308.4f},{385.0f,310.5f},{385.6f,312.6f},{385.6f,314.0f},
    {389.4f,317.6f},{389.4f,317.6f},{387.5f,315.9f},{390.6f,318.8f},
    {392.1f,320.2f},{393.4f,320.9f},{395.4f,322.1f},{398.6f,323.1f},
    {399.9f,323.7f},{401.9f,324.0f},{403.8f,324.2f},{406.0f,324.2f},
    {407.4f,324.0f},{408.3f,323.8f},{410.3f,323.3f},{411.6f,323.0f},
    {412.9f,322.1f},{414.3f,320.9f},{414.7f,319.9f},{415.0f,318.6f},
    {415.0f,317.8f},{414.0f,317.6f},{412.8f,317.6f},{411.7f,318.1f},
    {411.0f,318.8f},{410.2f,319.7f},{408.4f,320.0f},{407.4f,319.7f},
    {406.0f,319.7f},{403.9f,319.0f},{402.0f,318.6f},{400.5f,317.3f},
    {399.1f,316.2f},{398.0f,315.3f},{396.3f,313.6f},{394.9f,311.9f},
    {394.2f,310.5f},{393.4f,308.1f},{392.5f,306.5f},{392.0f,304.9f},
    {391.6f,303.2f},{391.3f,301.0f},{391.1f,299.6f},{390.9f,298.5f},
    {390.8f,297.1f},{390.6f,296.1f},{390.4f,295.1f},{390.2f,293.5f},
    {390.1f,292.1f},{389.5f,290.4f},{388.7f,288.5f},{388.3f,287.1f},
    {387.8f,285.9f},{387.1f,285.4f},{385.9f,283.8f},{389.0f,285.2f},
    {391.1f,286.4f},{393.2f,286.7f},{394.4f,286.7f},{398.9f,287.6f},
    {396.3f,287.6f},{399.8f,287.8f},{401.5f,287.6f},{403.9f,288.0f},
    {405.8f,288.3f},{408.6f,289.0f},{410.0f,290.0f},{411.6f,291.2f},
    {412.9f,292.8f},{414.2f,295.6f},{414.9f,298.5f},{415.0f,300.4f},
    {414.3f,302.2f},{413.5f,303.4f},{412.3f,303.9f},{411.4f,303.9f},
    {409.7f,304.1f},{409.0f,303.4f},{408.3f,302.7f},{407.4f,301.8f},
    {406.4f,301.3f},{406.4f,303.0f},{406.2f,304.9f},{406.7f,306.7f},
    {407.6f,307.7f},{408.4f,308.2f},{409.5f,308.6f},{411.0f,309.3f},
    {412.8f,309.3f},{413.8f,309.3f},{414.9f,309.1f},{416.1f,308.8f},
    {417.6f,307.9f},{418.5f,306.8f},{419.7f,305.5f},{420.9f,303.2f},
    {421.4f,301.8f},{422.0f,300.4f},{422.1f,299.2f},{422.1f,297.7f},
    {421.8f,295.8f},{421.6f,293.0f},{421.3f,291.9f},{420.6f,290.7f},
    {420.1f,289.5f},{418.7f,288.0f},{417.1f,286.2f},{415.4f,284.5f},
    {414.0f,283.1f},{412.8f,282.4f},{410.9f,281.5f},{409.7f,281.0f},
    {408.3f,280.7f},{406.0f,280.0f},{403.6f,279.8f},{402.0f,279.8f},
    {399.9f,279.5f},{397.7f,279.6f},{395.1f,279.3f},{392.5f,278.9f},
    {390.1f,278.2f},{388.2f,277.2f},{385.4f,276.0f},{383.3f,274.3f},
    {381.4f,273.4f},{381.4f,272.7f},{380.7f,271.8f},{380.4f,270.3f},
    {380.0f,269.6f},{379.8f,268.5f},{379.7f,267.5f},{379.7f,266.3f},
    {380.0f,264.4f},{381.9f,263.7f},{383.3f,263.7f},{384.2f,263.3f},
    {384.9f,263.0f},{385.9f,262.6f},{386.8f,262.6f},{388.3f,261.9f},
    {390.1f,261.8f},{391.6f,260.9f},{393.2f,260.0f},{394.6f,259.7f},
    {396.0f,258.7f},{397.5f,257.8f},{398.7f,257.3f},{400.3f,256.2f},
    {401.5f,255.5f},{402.5f,254.7f},{403.6f,253.3f},{405.3f,250.7f},
    {406.0f,249.3f},{407.1f,247.0f},{407.4f,245.7f},{407.9f,244.1f},
    {408.3f,242.0f},{408.4f,239.9f},{408.3f,237.2f}
};
static const int OP_COUNT = sizeof(octopus) / sizeof(octopus[0]);
static float opCX = 0, opCY = 0;
static float opMinX, opMinY, opMaxX, opMaxY;
static int   opInitDone = 0;
static const float OCT_SCALE = 1.5f;


/* ═══════════════════════════════════════════════════════
   FORWARD DECLARATIONS (serves as function index)
   ═══════════════════════════════════════════════════════ */

/* Utility */
void  drawCircle(float cx, float cy, float r, int filled);
void  drawEllipse(float cx, float cy, float rx, float ry);
float lerp(float a, float b, float t);
float randFloat(float minV, float maxV);
float distanceBetween(float x1, float y1, float x2, float y2);
void  renderBitmapString(float x, float y, void *font, const char *str);
void  computeOctopusCentroid(void);

/* Initialization */
void initOpenGL(void);
void initFishArray(void);
void initBubbleArray(void);
void initSeaweed(void);
void initCorals(void);
void initRocks(void);
void initLightRays(void);
void initFood(void);

/* Background & Environment */
void drawWaterBackground(void);
void drawSandyFloor(void);
void drawWaterSurface(void);
void drawLightRaysScene(void);
void drawSun(void);
void drawMoon(void);
void drawAquariumBorder(void);

/* Static Scenery */
void drawSingleCoral(Coral c);
void drawAllCorals(void);
void drawSingleRock(Rock r);
void drawAllRocks(void);
void drawStarfish(float cx, float cy, float size);
void drawShell(float cx, float cy, float size);

/* Seaweed */
void drawSingleSeaweed(Seaweed s);
void drawAllSeaweed(void);
void updateSeaweed(void);

/* Fish */
void drawFishType1(Fish f);
void drawFishType2(Fish f);
void drawFishType3(Fish f);
void drawFishType4(Fish f);
void drawSingleFish(Fish f);
void drawAllFish(void);
void updateFish(void);
void fishChaseFood(void);
void addFish(void);
void removeFish(void);

/* Octopus & Turtle */
void drawOctopus(void);
void drawTurtle(void);
void updateOctopus(void);
void updateTurtle(void);

/* Bubbles */
void spawnBubble(float x, float y);
void spawnBubbleBurst(void);
void spawnAmbientBubbles(void);
void drawSingleBubble(Bubble b);
void drawAllBubbles(void);
void updateBubbles(void);

/* Food */
void dropFood(float x, float y);
void drawFood(void);
void updateFood(void);

/* Day/Night */
void toggleDayNight(void);
void getDayColour(float depth, float *r, float *g, float *b);
void getNightColour(float depth, float *r, float *g, float *b);

/* HUD & Text */
void drawTitle(void);
void drawHUD(void);
void drawControlsBar(void);

/* GLUT Callbacks */
void display(void);
void reshape(int w, int h);
void timer(int value);
void keyboard(unsigned char key, int x, int y);
void mouseClick(int button, int state, int x, int y);


/* ═══════════════════════════════════════════════════════
   UTILITY FUNCTIONS
   ═══════════════════════════════════════════════════════ */

/* Compute centroid & bounding box of octopus outline for positioning */
void computeOctopusCentroid(void) {
    int i;
    float sx = 0, sy = 0;
    opMinX = opMaxX = octopus[0][0];
    opMinY = opMaxY = octopus[0][1];
    for (i = 0; i < OP_COUNT; i++) {
        sx += octopus[i][0];
        sy += octopus[i][1];
        if (octopus[i][0] < opMinX) opMinX = octopus[i][0];
        if (octopus[i][0] > opMaxX) opMaxX = octopus[i][0];
        if (octopus[i][1] < opMinY) opMinY = octopus[i][1];
        if (octopus[i][1] > opMaxY) opMaxY = octopus[i][1];
    }
    opCX = sx / OP_COUNT;
    opCY = sy / OP_COUNT;
    opInitDone = 1;
}

/* Draw circle: filled=1 for solid, filled=0 for outline */
void drawCircle(float cx, float cy, float r, int filled) {
    int i, seg = 40;
    float angle;
    if (filled) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (i = 0; i <= seg; i++) {
            angle = 2.0f * (float)M_PI * i / seg;
            glVertex2f(cx + r * cosf(angle), cy + r * sinf(angle));
        }
        glEnd();
    } else {
        glBegin(GL_LINE_LOOP);
        for (i = 0; i < seg; i++) {
            angle = 2.0f * (float)M_PI * i / seg;
            glVertex2f(cx + r * cosf(angle), cy + r * sinf(angle));
        }
        glEnd();
    }
}

/* Draw filled ellipse with separate X/Y radii */
void drawEllipse(float cx, float cy, float rx, float ry) {
    int i, seg = 40;
    float angle;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (i = 0; i <= seg; i++) {
        angle = 2.0f * (float)M_PI * i / seg;
        glVertex2f(cx + rx * cosf(angle), cy + ry * sinf(angle));
    }
    glEnd();
}

/* Linear interpolation: returns a when t=0, b when t=1 */
float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

/* Random float in [minV, maxV] */
float randFloat(float minV, float maxV) {
    return minV + (float)rand() / (float)RAND_MAX * (maxV - minV);
}

/* Euclidean distance between two points */
float distanceBetween(float x1, float y1, float x2, float y2) {
    return sqrtf((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

/* Render bitmap text at given position */
void renderBitmapString(float x, float y, void *font, const char *str) {
    const char *c;
    glRasterPos2f(x, y);
    for (c = str; *c != '\0'; c++)
        glutBitmapCharacter(font, *c);
}


/* ═══════════════════════════════════════════════════════
   INITIALIZATION
   ═══════════════════════════════════════════════════════ */

void initOpenGL(void) {
    glClearColor(0.0f, 0.05f, 0.15f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, W, 0, H);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    srand((unsigned int)time(NULL));
}

/* Create initial fish: 3 orange (left school) + 3 yellow/pink (right school) */
void initFishArray(void) {
    int i;
    for (i = 0; i < MAX_FISH; i++)
        fishArray[i].alive = 0;

    /* Left school: orange fish, upper-left area */
    for (i = 0; i < 3; i++) {
        fishArray[i].x    = randFloat(150, 350);
        fishArray[i].y    = randFloat(430, 490);
        fishArray[i].dx   = randFloat(0.5f, 1.5f);
        fishArray[i].dy   = randFloat(-0.2f, 0.2f);
        fishArray[i].size = randFloat(15, 22);
        fishArray[i].r = 1.0f;  fishArray[i].g = randFloat(0.3f, 0.5f);  fishArray[i].b = 0.0f;
        fishArray[i].type = 1;
        fishArray[i].facingRight = 1;
        fishArray[i].tailAngle = 0.0f;
        fishArray[i].alive = 1;
    }

    /* Right school: yellow/pink fish, upper-right area */
    for (i = 3; i < 6; i++) {
        fishArray[i].x    = randFloat(650, 870);
        fishArray[i].y    = randFloat(370, 430);
        fishArray[i].dx   = randFloat(0.5f, 1.2f);
        fishArray[i].dy   = randFloat(-0.2f, 0.2f);
        fishArray[i].size = randFloat(14, 20);
        if (i % 2 == 0) {
            fishArray[i].r = 1.0f; fishArray[i].g = 0.9f; fishArray[i].b = 0.2f;
            fishArray[i].type = 3;
        } else {
            fishArray[i].r = 1.0f; fishArray[i].g = 0.4f; fishArray[i].b = 0.6f;
            fishArray[i].type = 4;
        }
        fishArray[i].facingRight = 1;
        fishArray[i].tailAngle = 0.0f;
        fishArray[i].alive = 1;
    }
    fishCount = 6;
}

void initBubbleArray(void) {
    int i;
    for (i = 0; i < MAX_BUBBLES; i++)
        bubbleArray[i].alive = 0;
}

/* Place 5 seaweed on right side + 1 small on left */
void initSeaweed(void) {
    seaweedArray[0] = (Seaweed){ 930, FLOOR_Y, 280, 0.0f, 0.0f, 0.55f, 0.15f};
    seaweedArray[1] = (Seaweed){ 960, FLOOR_Y, 330, 0.8f, 0.0f, 0.50f, 0.10f};
    seaweedArray[2] = (Seaweed){ 990, FLOOR_Y, 250, 1.5f, 0.05f,0.60f, 0.20f};
    seaweedArray[3] = (Seaweed){1020, FLOOR_Y, 310, 2.3f, 0.0f, 0.45f, 0.12f};
    seaweedArray[4] = (Seaweed){1050, FLOOR_Y, 220, 3.0f, 0.08f,0.58f, 0.18f};
    seaweedArray[5] = (Seaweed){ 380, FLOOR_Y,  80, 1.0f, 0.0f, 0.50f, 0.15f};
}

/* Place corals on left side of floor */
void initCorals(void) {
    coralArray[0] = (Coral){180, FLOOR_Y, 130, 0.95f, 0.45f, 0.40f, 0};
    coralArray[1] = (Coral){110, FLOOR_Y,  80, 1.00f, 0.60f, 0.55f, 0};
    coralArray[2] = (Coral){290, FLOOR_Y,  35, 1.00f, 0.50f, 0.30f, 1};
    coralArray[3] = (Coral){350, FLOOR_Y,  25, 0.90f, 0.40f, 0.50f, 1};
    coralArray[4] = (Coral){450, FLOOR_Y,  28, 0.90f, 0.75f, 0.40f, 2};
    coralArray[5] = (Coral){ 90, FLOOR_Y,  40, 0.85f, 0.35f, 0.35f, 0};
}

/* Scatter small rocks on sandy floor */
void initRocks(void) {
    rockArray[0] = (Rock){160,  FLOOR_Y-10, 18};
    rockArray[1] = (Rock){320,  FLOOR_Y-8,  14};
    rockArray[2] = (Rock){500,  FLOOR_Y-12, 20};
    rockArray[3] = (Rock){600,  FLOOR_Y-6,  12};
    rockArray[4] = (Rock){750,  FLOOR_Y-10, 16};
    rockArray[5] = (Rock){850,  FLOOR_Y-8,  13};
    rockArray[6] = (Rock){430,  FLOOR_Y-5,  10};
    rockArray[7] = (Rock){1080, FLOOR_Y-9,  15};
}

/* Light rays spread down from sun position */
void initLightRays(void) {
    int i;
    float spread = 250.0f;
    for (i = 0; i < MAX_RAYS; i++) {
        float t = (float)i / (MAX_RAYS - 1);
        float offsetX = lerp(-spread, spread, t);
        lightRays[i].startX = SUN_X;
        lightRays[i].startY = SURFACE_Y;
        lightRays[i].alpha  = randFloat(0.06f, 0.14f);
        lightRays[i].endX1  = SUN_X + offsetX - 25;
        lightRays[i].endY1  = FLOOR_Y + 30;
        lightRays[i].endX2  = SUN_X + offsetX + 25;
        lightRays[i].endY2  = FLOOR_Y + 30;
    }
}

void initFood(void) {
    int i;
    for (i = 0; i < MAX_FOOD; i++)
        foodArray[i].active = 0;
}


/* ═══════════════════════════════════════════════════════
   BACKGROUND & ENVIRONMENT (TODO)
   ═══════════════════════════════════════════════════════ */

/* TODO: 2 GL_QUADS with vertex colours for gradient (surface→floor, above surface) */
void drawWaterBackground(void) {
}

/* TODO: GL_TRIANGLE_STRIP sandy floor with sine-wave top edge, beige colour */
void drawSandyFloor(void) {
}

/* TODO: GL_LINE_STRIP sine wave at y=SURFACE_Y, light blue, alpha blended */
void drawWaterSurface(void) {
}

/* TODO: GL_TRIANGLES for each lightRays[i], yellow-white with alpha; dimmer at night */
void drawLightRaysScene(void) {
}

/* TODO: Layered circles at (SUN_X,SUN_Y): outer glow, main sun, bright core */
void drawSun(void) {
}

/* TODO: White circle minus offset circle for crescent moon when isNight */
void drawMoon(void) {
}

/* TODO: GL_LINE_LOOP rectangle at BORDER_LEFT/RIGHT/TOP/BOTTOM, light blue */
void drawAquariumBorder(void) {
}


/* ═══════════════════════════════════════════════════════
   STATIC SCENERY (TODO)
   ═══════════════════════════════════════════════════════ */

/* TODO: type0=branching lines+tips, type1=circle cluster, type2=narrow rects */
void drawSingleCoral(Coral c) {
}

void drawAllCorals(void) {
    int i;
    for (i = 0; i < MAX_CORAL; i++)
        drawSingleCoral(coralArray[i]);
}

/* TODO: Irregular 5-6 vertex polygon, grey-brown colour */
void drawSingleRock(Rock r) {
}

void drawAllRocks(void) {
    int i;
    for (i = 0; i < MAX_ROCKS; i++)
        drawSingleRock(rockArray[i]);
}

/* TODO: 5-pointed star at (cx,cy) using GL_TRIANGLE_FAN, orange */
void drawStarfish(float cx, float cy, float size) {
}

/* TODO: Spiral shell using overlapping arcs/circles, beige-white */
void drawShell(float cx, float cy, float size) {
}


/* ═══════════════════════════════════════════════════════
   SEAWEED — SINE-WAVE ANIMATION (TODO draw)
   ═══════════════════════════════════════════════════════ */

/* TODO: GL_QUAD_STRIP from base upward, X offset by sin(globalTime + phase), narrowing */
void drawSingleSeaweed(Seaweed s) {
}

void drawAllSeaweed(void) {
    int i;
    for (i = 0; i < 6; i++)
        drawSingleSeaweed(seaweedArray[i]);
}

/* globalTime advance handles sine animation automatically */
void updateSeaweed(void) {
}


/* ═══════════════════════════════════════════════════════
   FISH — 2D TRANSFORMS, COMPOSITE SHAPES (TODO)
   ═══════════════════════════════════════════════════════ */

/* TODO: Orange tropical — ellipse body, animated tail triangle, eye, dorsal fin
   Use glPushMatrix/Translate/Scale(-1 for flip)/PopMatrix */
void drawFishType1(Fish f) {
}

/* TODO: Blue/cyan — rounder body ellipse, same structure as type1 */
void drawFishType2(Fish f) {
}

/* TODO: Yellow angelfish — taller body, larger dorsal/ventral fins */
void drawFishType3(Fish f) {
}

/* TODO: Pink small fish — compact ellipse body */
void drawFishType4(Fish f) {
}

void drawSingleFish(Fish f) {
    if (!f.alive) return;
    switch (f.type) {
        case 1: drawFishType1(f); break;
        case 2: drawFishType2(f); break;
        case 3: drawFishType3(f); break;
        case 4: drawFishType4(f); break;
    }
}

void drawAllFish(void) {
    int i;
    for (i = 0; i < MAX_FISH; i++)
        if (fishArray[i].alive)
            drawSingleFish(fishArray[i]);
}

/* TODO: Move fish, wag tail, boundary wrap/bounce, update facingRight */
void updateFish(void) {
}

/* TODO: Each food attracts nearest fish within range 200; fish eaten when within 15px */
void fishChaseFood(void) {
}

/* TODO: Find first dead slot, set alive=1 with currentSpecies type, random position */
void addFish(void) {
}

/* TODO: Find last alive fish, set alive=0, decrement fishCount */
void removeFish(void) {
}


/* ═══════════════════════════════════════════════════════
   OCTOPUS (FULLY IMPLEMENTED) — stencil-based fill
   ═══════════════════════════════════════════════════════ */

void drawOctopus(void) {
    int i;
    if (!opInitDone) computeOctopusCentroid();

    glPushMatrix();
    glTranslatef(octX, octY, 0.0f);
    glScalef(OCT_SCALE, -OCT_SCALE, 1.0f);
    glTranslatef(-opCX, -opCY, 0.0f);

    float hcx = 382.0f, hcy = 228.0f;  /* head center in raw coords */

    /* --- Stencil mask: fill only inside octopus outline --- */
    glEnable(GL_STENCIL_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(opCX, opCY);
    for (i = 0; i < OP_COUNT; i++)
        glVertex2f(octopus[i][0], octopus[i][1]);
    glVertex2f(octopus[0][0], octopus[0][1]);
    glEnd();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    /* --- Base fill (darker purple) --- */
    glColor3f(0.58f, 0.28f, 0.64f);
    glBegin(GL_QUADS);
    glVertex2f(opMinX-10, opMinY-10); glVertex2f(opMaxX+10, opMinY-10);
    glVertex2f(opMaxX+10, opMaxY+10); glVertex2f(opMinX-10, opMaxY+10);
    glEnd();

    /* --- Head shading layers (stencil-clipped inside body) --- */
    glColor4f(0.70f, 0.38f, 0.75f, 0.55f);
    drawEllipse(hcx, hcy, 27.0f, 17.0f);
    glColor4f(0.80f, 0.52f, 0.84f, 0.45f);
    drawEllipse(hcx+1, hcy+1, 19.0f, 13.0f);
    glColor4f(0.88f, 0.62f, 0.90f, 0.30f);
    drawEllipse(hcx+2, hcy+2, 12.0f, 9.0f);
    glColor4f(0.88f, 0.68f, 0.90f, 0.35f);
    drawEllipse(hcx-2, 218.0f, 16.0f, 7.0f);
    glColor4f(0.95f, 0.85f, 0.97f, 0.28f);
    drawEllipse(hcx-6, 216.0f, 9.0f, 4.0f);
    glColor4f(0.40f, 0.14f, 0.46f, 0.35f);
    drawEllipse(hcx, 243.0f, 25.0f, 8.0f);
    glColor4f(0.44f, 0.16f, 0.48f, 0.28f);
    drawEllipse(358.0f, hcy, 10.0f, 15.0f);
    glColor4f(0.76f, 0.46f, 0.80f, 0.18f);
    drawEllipse(402.0f, hcy-2, 10.0f, 13.0f);

    /* Left tentacle highlights */
    glColor4f(0.72f, 0.42f, 0.76f, 0.15f);
    drawEllipse(348, 258, 10, 14);  drawEllipse(328, 270, 8, 12);
    drawEllipse(305, 285, 7, 14);   drawEllipse(296, 300, 6, 10);

    /* Right tentacle highlights */
    drawEllipse(355, 295, 8, 14);   drawEllipse(370, 310, 7, 12);
    drawEllipse(380, 300, 8, 14);   drawEllipse(395, 300, 7, 12);
    drawEllipse(410, 295, 7, 12);

    /* Tentacle tips */
    glColor4f(0.68f, 0.36f, 0.72f, 0.12f);
    drawEllipse(320, 325, 8, 10);  drawEllipse(340, 315, 7, 10);
    drawEllipse(365, 330, 8, 10);

    /* Head texture spots */
    glColor4f(0.50f, 0.20f, 0.56f, 0.12f);
    drawCircle(hcx-12, hcy-4, 2.5f, 1);  drawCircle(hcx+10, hcy-7, 2.0f, 1);
    drawCircle(hcx-5, hcy+6, 1.8f, 1);   drawCircle(hcx+17, hcy+3, 2.2f, 1);
    drawCircle(hcx-17, hcy-1, 1.5f, 1);  drawCircle(hcx+4, 219, 1.5f, 1);
    drawCircle(hcx-9, 220, 1.8f, 1);     drawCircle(hcx+22, hcy-2, 1.3f, 1);

    glDisable(GL_STENCIL_TEST);

    /* --- Suction cups on tentacles (below head, y>255) --- */
    for (i = 0; i < OP_COUNT; i += 5) {
        float sx = octopus[i][0], sy = octopus[i][1];
        if (sy < 255.0f) continue;
        float dist = sqrtf((sx-hcx)*(sx-hcx) + (sy-hcy)*(sy-hcy));
        if (dist < 25.0f) continue;
        float dx = (opCX-sx)*0.06f, dy = (opCY-sy)*0.06f;
        glColor4f(0.52f, 0.22f, 0.56f, 0.30f);
        drawCircle(sx+dx, sy+dy, 2.0f, 1);
        glColor4f(0.80f, 0.52f, 0.84f, 0.22f);
        drawCircle(sx+dx, sy+dy, 1.0f, 1);
    }

    /* --- Outline --- */
    glColor3f(0.36f, 0.10f, 0.40f);
    glLineWidth(2.0f);
    glEnable(GL_LINE_SMOOTH);
    glBegin(GL_LINE_LOOP);
    for (i = 0; i < OP_COUNT; i++)
        glVertex2f(octopus[i][0], octopus[i][1]);
    glEnd();
    glDisable(GL_LINE_SMOOTH);
    glLineWidth(1.0f);

    /* --- Eye --- */
    float eyeX = 372.71f, eyeY = 254.19f;
    glColor4f(0.46f, 0.18f, 0.50f, 0.35f);
    drawEllipse(eyeX, eyeY, 9.5f, 10.5f);
    glColor3f(0.78f, 0.78f, 0.82f);  drawCircle(eyeX, eyeY, 7.0f, 1);
    glColor3f(1, 1, 1);              drawCircle(eyeX, eyeY, 6.0f, 1);
    glColor3f(0.10f, 0.06f, 0.14f);  drawCircle(eyeX+1, eyeY+0.5f, 4.0f, 1);
    glColor3f(0, 0, 0.02f);          drawCircle(eyeX+1.5f, eyeY+0.7f, 3.8f, 1);
    glColor3f(1, 1, 1);              drawCircle(eyeX+2.5f, eyeY-1.5f, 1.5f, 1);
    glColor3f(1, 1, 1);              drawCircle(eyeX-0.5f, eyeY+2, 0.8f, 1);

    /* --- Eyebrow --- */
    glColor3f(0.46f, 0.16f, 0.50f);
    glLineWidth(2.5f);  glEnable(GL_LINE_SMOOTH);
    glBegin(GL_LINE_STRIP);
    glVertex2f(eyeX-7, eyeY-11);   glVertex2f(eyeX-1, eyeY-13);
    glVertex2f(eyeX+5, eyeY-13);   glVertex2f(eyeX+10, eyeY-11);
    glVertex2f(eyeX+14, eyeY-8);
    glEnd();
    glDisable(GL_LINE_SMOOTH);  glLineWidth(1.0f);

    /* --- Mouth (smile) --- */
    glColor3f(0.40f, 0.14f, 0.44f);
    glLineWidth(2.0f);  glEnable(GL_LINE_SMOOTH);
    glBegin(GL_LINE_STRIP);
    glVertex2f(386, 236); glVertex2f(388, 238); glVertex2f(391, 239);
    glVertex2f(394, 238.5f); glVertex2f(397, 236);
    glEnd();
    glDisable(GL_LINE_SMOOTH);  glLineWidth(1.0f);

    /* --- Cheek blush --- */
    glColor4f(0.92f, 0.48f, 0.55f, 0.15f);
    drawCircle(eyeX+10, eyeY+6, 4.0f, 1);
    glColor4f(0.88f, 0.42f, 0.50f, 0.10f);
    drawCircle(eyeX-15, eyeY+4, 3.5f, 1);

    /* --- Second eye hint (left side, subtle) --- */
    glColor4f(0.48f, 0.20f, 0.52f, 0.20f);
    drawCircle(370, 227, 5.0f, 1);
    glColor4f(0.55f, 0.28f, 0.58f, 0.15f);
    drawCircle(370, 227, 3.5f, 1);

    glPopMatrix();
}

/* Octopus bobs gently in place */
void updateOctopus(void) {
    octY = 300.0f + 4.0f * sinf(globalTime * 0.7f);
    octX = 480.0f + 5.0f * sinf(globalTime * 0.25f);
    tentaclePhase = globalTime;
}


/* ═══════════════════════════════════════════════════════
   TURTLE (TODO)
   ═══════════════════════════════════════════════════════ */

/* TODO: Shell (dome ellipse + pattern), head, flippers, animated flipper angle */
void drawTurtle(void) {
}

/* TODO: turtX += turtDX; bounce at edges; flipperAngle = 20*sin(globalTime*3) */
void updateTurtle(void) {
}


/* ═══════════════════════════════════════════════════════
   BUBBLES — PARTICLE SYSTEM (TODO core logic)
   ═══════════════════════════════════════════════════════ */

/* TODO: Find first dead slot, set position/radius/speed/alpha/alive */
void spawnBubble(float x, float y) {
}

/* Spawn burst of 12 bubbles at random positions near floor */
void spawnBubbleBurst(void) {
    int i;
    for (i = 0; i < 12; i++)
        spawnBubble(randFloat(100, 1100), randFloat(FLOOR_Y, FLOOR_Y + 80));
}

/* Small chance each frame to spawn ambient bubble near seaweed */
void spawnAmbientBubbles(void) {
    if (randFloat(0, 1) < 0.02f) {
        int idx = rand() % 6;
        spawnBubble(seaweedArray[idx].baseX + randFloat(-10, 10),
                    seaweedArray[idx].baseY + randFloat(20, 80));
    }
}

/* TODO: Circle outline with highlight dot, alpha blended */
void drawSingleBubble(Bubble b) {
}

void drawAllBubbles(void) {
    int i;
    for (i = 0; i < MAX_BUBBLES; i++)
        if (bubbleArray[i].alive)
            drawSingleBubble(bubbleArray[i]);
}

/* TODO: Rise upward, wobble X with sine, fade alpha, kill at surface */
void updateBubbles(void) {
}


/* ═══════════════════════════════════════════════════════
   FOOD PARTICLES
   ═══════════════════════════════════════════════════════ */

/* Find first inactive food slot and place food at (x,y) */
void dropFood(float x, float y) {
    int i;
    for (i = 0; i < MAX_FOOD; i++) {
        if (!foodArray[i].active) {
            foodArray[i].x = x;
            foodArray[i].y = y;
            foodArray[i].dy = -0.5f;
            foodArray[i].active = 1;
            foodArray[i].timer = 500.0f;
            break;
        }
    }
}

/* Draw small brown circle for each active food */
void drawFood(void) {
    int i;
    for (i = 0; i < MAX_FOOD; i++) {
        if (foodArray[i].active) {
            glColor3f(0.55f, 0.35f, 0.1f);
            drawCircle(foodArray[i].x, foodArray[i].y, 4, 1);
        }
    }
}

/* Food sinks, stops at floor, disappears after timer expires */
void updateFood(void) {
    int i;
    for (i = 0; i < MAX_FOOD; i++) {
        if (foodArray[i].active) {
            foodArray[i].y += foodArray[i].dy;
            foodArray[i].timer -= 1.0f;
            if (foodArray[i].y < FLOOR_Y + 10)
                foodArray[i].dy = 0;
            if (foodArray[i].timer <= 0)
                foodArray[i].active = 0;
        }
    }
}


/* ═══════════════════════════════════════════════════════
   DAY / NIGHT MODE
   ═══════════════════════════════════════════════════════ */

void toggleDayNight(void) { isNight = !isNight; }

/* Interpolate day water colour by depth (0=surface, 1=floor) */
void getDayColour(float depth, float *r, float *g, float *b) {
    *r = lerp(0.1f, 0.0f, depth);
    *g = lerp(0.4f, 0.12f, depth);
    *b = lerp(0.7f, 0.3f, depth);
}

/* Interpolate night water colour (much darker) */
void getNightColour(float depth, float *r, float *g, float *b) {
    *r = lerp(0.02f, 0.0f, depth);
    *g = lerp(0.08f, 0.02f, depth);
    *b = lerp(0.2f, 0.08f, depth);
}


/* ═══════════════════════════════════════════════════════
   HUD & TEXT
   ═══════════════════════════════════════════════════════ */

void drawTitle(void) {
    glColor3f(0.2f, 0.85f, 0.95f);
    renderBitmapString(470, TITLE_Y, GLUT_BITMAP_TIMES_ROMAN_24, "Deep Sea Aquarium");
}

/* Fish count, mode, species; PAUSED overlay if paused */
void drawHUD(void) {
    char buf[100];
    glColor3f(1, 1, 1);
    sprintf(buf, "Fish: %d", fishCount);
    renderBitmapString(BORDER_LEFT+10, BORDER_TOP-25, GLUT_BITMAP_HELVETICA_18, buf);
    sprintf(buf, "Mode: %s", isNight ? "Night" : "Day");
    renderBitmapString(BORDER_LEFT+10, BORDER_TOP-45, GLUT_BITMAP_HELVETICA_18, buf);
    sprintf(buf, "Species: %d", currentSpecies);
    renderBitmapString(BORDER_LEFT+10, BORDER_TOP-65, GLUT_BITMAP_HELVETICA_18, buf);
    if (isPaused) {
        glColor3f(1.0f, 0.3f, 0.3f);
        renderBitmapString(W/2-40, H/2, GLUT_BITMAP_TIMES_ROMAN_24, "PAUSED");
    }
}

void drawControlsBar(void) {
    glColor3f(0.75f, 0.75f, 0.82f);
    renderBitmapString(BORDER_LEFT, HUD_Y, GLUT_BITMAP_HELVETICA_12,
        "Mouse:Food | +/-:Fish | D/N:Day/Night | B:Bubbles | 1-3:Species | SPACE:Pause | ESC:Quit");
}


/* ═══════════════════════════════════════════════════════
   DISPLAY — MASTER RENDER (back-to-front layer order)
   ═══════════════════════════════════════════════════════ */

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();

    drawWaterBackground();          /* Layer 1: water gradient */
    drawLightRaysScene();           /* Layer 2: light rays (alpha) */
    drawSandyFloor();               /* Layer 3: ocean floor */

    drawAllRocks();                 /* Layer 4: static floor scenery */
    drawAllCorals();
    drawStarfish(250.0f, FLOOR_Y - 25, 28.0f);
    drawShell(570.0f, FLOOR_Y - 30, 18.0f);

    drawAllSeaweed();               /* Layer 5: animated seaweed */

    drawOctopus();                  /* Layer 6: marine life */
    drawTurtle();
    drawAllFish();

    drawAllBubbles();               /* Layer 7: particles */
    drawFood();

    drawWaterSurface();             /* Layer 8: surface wave */

    if (isNight) drawMoon();        /* Layer 9: celestial body */
    else         drawSun();

    drawAquariumBorder();           /* Layer 10: UI overlay */
    drawTitle();
    drawHUD();
    drawControlsBar();

    glutSwapBuffers();
}


/* ═══════════════════════════════════════════════════════
   GLUT CALLBACKS
   ═══════════════════════════════════════════════════════ */

void reshape(int w, int h) {
    if (h == 0) h = 1;
    windowW = w;  windowH = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, W, 0, H);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/* ~60 FPS update loop */
void timer(int value) {
    if (!isPaused) {
        globalTime += 0.016f;
        updateFish();
        fishChaseFood();
        updateBubbles();
        updateFood();
        updateSeaweed();
        updateOctopus();
        updateTurtle();
        spawnAmbientBubbles();
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '+': case '=': addFish();                           break;
        case '-': case '_': removeFish();                        break;
        case 'd': case 'D': if (isNight)  toggleDayNight();      break;
        case 'n': case 'N': if (!isNight) toggleDayNight();      break;
        case 'b': case 'B': spawnBubbleBurst();                  break;
        case '1': currentSpecies = 1;                            break;
        case '2': currentSpecies = 2;                            break;
        case '3': currentSpecies = 3;                            break;
        case ' ': isPaused = !isPaused;                          break;
        case 27:  exit(0);                                       break;
    }
    glutPostRedisplay();
}

/* Left click → drop food at world coordinates */
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float worldX = (float)x * ((float)W / windowW);
        float worldY = (float)(windowH - y) * ((float)H / windowH);
        dropFood(worldX, worldY);
    }
}


/* ═══════════════════════════════════════════════════════
   MAIN — ENTRY POINT
   ═══════════════════════════════════════════════════════ */

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Deep Sea Aquarium");

    initOpenGL();
    initFishArray();
    initBubbleArray();
    initSeaweed();
    initCorals();
    initRocks();
    initLightRays();
    initFood();
    computeOctopusCentroid();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseClick);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}