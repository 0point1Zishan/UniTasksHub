#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>

// --- WINDOW SETTINGS ---
// Set initial window size to match your sand's original design perfectly
int winW = 1301; 
int winH = 764;
float gTime = 0.0f;
int paused = 0;

// --- SAND COLORS ---
void setSandBase()      { glColor3f(0.88f, 0.79f, 0.62f); } 
void setSandShadow()    { glColor3f(0.76f, 0.66f, 0.51f); } 
void setSandHighlight() { glColor3f(0.95f, 0.89f, 0.76f); } 

// =========================================================================
//                             SAND DRAWING FUNCTIONS
// =========================================================================

void drawFullBigSand() {
    setSandBase();
    glBegin(GL_POLYGON);
    glVertex2f(2.383, 624.289);    glVertex2f(47.656, 617.141);
    glVertex2f(55.995, 615.950);   glVertex2f(61.952, 613.567);
    glVertex2f(67.909, 614.758);   glVertex2f(83.397, 621.907);
    glVertex2f(98.886, 625.481);   glVertex2f(357.418, 626.672);
    glVertex2f(400.308, 633.821);  glVertex2f(415.796, 635.012);
    glVertex2f(434.858, 637.395);  glVertex2f(461.069, 637.395);
    glVertex2f(490.853, 638.586);  glVertex2f(514.681, 639.777);
    glVertex2f(539.701, 642.160);  glVertex2f(574.251, 645.734);
    glVertex2f(594.505, 648.117);  glVertex2f(604.036, 648.117);
    glVertex2f(619.524, 646.926);  glVertex2f(649.309, 642.160);
    glVertex2f(675.519, 637.395);  glVertex2f(704.113, 632.629);
    glVertex2f(730.323, 627.864);  glVertex2f(752.960, 624.289);
    glVertex2f(769.639, 621.907);  glVertex2f(787.510, 619.524);
    glVertex2f(807.764, 618.332);  glVertex2f(825.635, 618.332);
    glVertex2f(843.505, 617.141);  glVertex2f(861.376, 617.141);
    glVertex2f(880.439, 617.141);  glVertex2f(895.927, 619.524);
    glVertex2f(920.946, 620.715);  glVertex2f(940.008, 621.907);
    glVertex2f(951.922, 623.098);  glVertex2f(984.090, 626.672);
    glVertex2f(1023.406, 620.715); glVertex2f(1040.085, 614.758);
    glVertex2f(1068.679, 611.184); glVertex2f(1183.052, 604.036);
    glVertex2f(1225.942, 607.610); glVertex2f(1297.426, 618.332);
    glVertex2f(1310.0, 618.332); 
    glVertex2f(1310.0, 770.0);   
    glVertex2f(-10.0, 770.0);    
    glVertex2f(1.191, 623.098);
    glEnd();
}

void drawBottomRightSand() {
    setSandShadow();
    glBegin(GL_POLYGON);
    glVertex2f(1310.0, 687.739); glVertex2f(1310.0, 770.0);   
    glVertex2f(750.940, 761.857); glVertex2f(766.453, 758.409);
    glVertex2f(781.391, 754.962); glVertex2f(796.330, 752.089);
    glVertex2f(812.992, 749.791); glVertex2f(834.250, 746.344);
    glVertex2f(857.232, 742.322); glVertex2f(878.491, 737.151);
    glVertex2f(895.153, 731.980); glVertex2f(913.538, 727.958);
    glVertex2f(933.073, 722.787); glVertex2f(952.033, 717.616);
    glVertex2f(972.717, 713.594); glVertex2f(995.699, 709.572);
    glVertex2f(1014.660, 706.700); glVertex2f(1038.791, 703.827);
    glVertex2f(1064.646, 701.529); glVertex2f(1092.799, 698.081);
    glVertex2f(1118.654, 694.059); glVertex2f(1150.829, 688.314);
    glVertex2f(1179.556, 685.441); glVertex2f(1203.687, 685.441);
    glVertex2f(1216.902, 686.590); glVertex2f(1228.393, 687.165);
    glVertex2f(1240.459, 687.739); glVertex2f(1251.375, 687.739);
    glVertex2f(1261.143, 687.165); glVertex2f(1269.186, 687.165);
    glVertex2f(1282.401, 687.739); glVertex2f(1292.743, 688.314);
    glVertex2f(1310.0, 686.590); 
    glEnd();
}

void drawTopMiddleSand() {
    setSandHighlight();
    glBegin(GL_POLYGON);
    glVertex2f(595.236, 649.244); glVertex2f(609.600, 648.095);
    glVertex2f(625.113, 646.946); glVertex2f(645.222, 644.073);
    glVertex2f(662.459, 641.201); glVertex2f(682.568, 637.179);
    glVertex2f(704.401, 633.157); glVertex2f(717.041, 630.859);
    glVertex2f(731.405, 627.986); glVertex2f(752.089, 624.539);
    glVertex2f(766.453, 622.815); glVertex2f(784.839, 620.517);
    glVertex2f(803.799, 619.368); glVertex2f(815.864, 618.793);
    glVertex2f(826.206, 618.793); glVertex2f(838.847, 618.793);
    glVertex2f(849.763, 618.793); glVertex2f(865.276, 618.793);
    glVertex2f(878.491, 618.793); glVertex2f(893.429, 618.793);
    glVertex2f(905.495, 619.942); glVertex2f(915.262, 620.517);
    glVertex2f(922.731, 621.091); glVertex2f(931.924, 621.666);
    glVertex2f(938.244, 621.666); glVertex2f(945.139, 622.815);
    glVertex2f(952.608, 622.815); glVertex2f(960.077, 622.815);
    glVertex2f(970.419, 623.964); glVertex2f(975.590, 624.539);
    glVertex2f(981.336, 626.262); glVertex2f(976.165, 627.986);
    glVertex2f(971.568, 627.411); glVertex2f(966.397, 627.986);
    glVertex2f(961.801, 628.560); glVertex2f(956.055, 629.135);
    glVertex2f(944.564, 631.433); glVertex2f(934.222, 633.157);
    glVertex2f(923.880, 636.030); glVertex2f(915.262, 637.179);
    glVertex2f(904.346, 640.051); glVertex2f(892.280, 642.924);
    glVertex2f(877.916, 645.222); glVertex2f(865.851, 646.946);
    glVertex2f(847.465, 650.393); glVertex2f(834.825, 651.542);
    glVertex2f(825.632, 653.266); glVertex2f(815.290, 654.415);
    glVertex2f(799.777, 656.713); glVertex2f(783.690, 658.437);
    glVertex2f(765.878, 660.161); glVertex2f(744.045, 661.310);
    glVertex2f(731.980, 662.459); glVertex2f(721.638, 661.884);
    glVertex2f(697.507, 660.161); glVertex2f(680.845, 659.586);
    glVertex2f(658.437, 657.288); glVertex2f(642.350, 656.139);
    glVertex2f(632.582, 655.564); glVertex2f(622.815, 653.841);
    glVertex2f(607.876, 652.117); glVertex2f(595.236, 649.244);
    glEnd();
}

void drawBottomRightUpperSand() {
    setSandHighlight();
    glBegin(GL_POLYGON);
    glVertex2f(1151.403, 688.314); glVertex2f(1123.250, 692.910);
    glVertex2f(1111.759, 695.783); glVertex2f(1095.097, 698.081);
    glVertex2f(1084.180, 698.656); glVertex2f(1073.839, 699.805);
    glVertex2f(1064.646, 700.954); glVertex2f(1053.729, 702.103);
    glVertex2f(1035.918, 704.401); glVertex2f(1012.936, 707.274);
    glVertex2f(993.976, 710.147);  glVertex2f(981.910, 711.870);
    glVertex2f(967.546, 714.743);  glVertex2f(953.182, 717.616);
    glVertex2f(939.393, 720.489);  glVertex2f(926.179, 724.511);
    glVertex2f(911.240, 727.958);  glVertex2f(888.258, 734.278);
    glVertex2f(874.469, 738.300);  glVertex2f(862.403, 740.598);
    glVertex2f(849.189, 742.896);  glVertex2f(838.272, 745.195);
    glVertex2f(826.206, 746.918);  glVertex2f(814.715, 749.216);
    glVertex2f(803.224, 750.940);  glVertex2f(791.733, 753.238);
    glVertex2f(782.540, 754.962);  glVertex2f(772.773, 757.260);
    glVertex2f(768.177, 757.835);  glVertex2f(756.111, 761.857);
    glVertex2f(749.791, 762.431);  glVertex2f(756.111, 757.260);
    glVertex2f(762.431, 754.962);  glVertex2f(768.751, 753.238);
    glVertex2f(775.646, 750.940);  glVertex2f(783.115, 749.216);
    glVertex2f(791.733, 746.344);  glVertex2f(799.202, 744.045);
    glVertex2f(808.395, 742.896);  glVertex2f(818.163, 741.173);
    glVertex2f(827.356, 738.874);  glVertex2f(835.974, 737.151);
    glVertex2f(845.741, 736.576);  glVertex2f(853.785, 735.427);
    glVertex2f(862.403, 733.129);  glVertex2f(873.894, 729.107);
    glVertex2f(884.236, 726.809);  glVertex2f(895.153, 723.362);
    glVertex2f(906.644, 720.489);  glVertex2f(920.433, 716.467);
    glVertex2f(930.200, 714.743);  glVertex2f(940.542, 712.445);
    glVertex2f(964.674, 706.700);  glVertex2f(981.336, 703.827);
    glVertex2f(994.550, 702.103);  glVertex2f(1011.787, 699.805);
    glVertex2f(1036.493, 696.358); glVertex2f(1053.729, 694.634);
    glVertex2f(1064.071, 693.485); glVertex2f(1073.839, 691.761);
    glVertex2f(1084.180, 691.761); glVertex2f(1095.097, 691.761);
    glVertex2f(1110.610, 690.037); glVertex2f(1122.675, 688.314);
    glVertex2f(1133.592, 687.165); glVertex2f(1144.508, 686.016);
    glVertex2f(1153.127, 687.165); glVertex2f(1153.127, 688.888);
    glEnd();
}

void drawBottomLeftUpperSand() {
    setSandHighlight();
    glBegin(GL_POLYGON);
    glVertex2f(3.447, 640.626);   glVertex2f(9.193, 638.902);
    glVertex2f(13.215, 638.328);  glVertex2f(18.386, 637.753);
    glVertex2f(24.131, 637.179);  glVertex2f(29.302, 635.455);
    glVertex2f(35.622, 636.030);  glVertex2f(40.793, 636.030);
    glVertex2f(46.539, 637.179);  glVertex2f(52.859, 637.179);
    glVertex2f(57.455, 637.179);  glVertex2f(61.477, 638.328);
    glVertex2f(101.696, 636.604); glVertex2f(372.310, 660.735);
    glVertex2f(374.608, 663.608); glVertex2f(375.183, 665.332);
    glVertex2f(375.183, 667.055); glVertex2f(372.885, 671.652);
    glVertex2f(367.714, 673.950); glVertex2f(360.819, 675.099);
    glVertex2f(351.626, 675.099); glVertex2f(344.157, 676.248);
    glVertex2f(337.837, 677.397); glVertex2f(332.091, 677.972);
    glVertex2f(327.495, 678.546); glVertex2f(320.600, 679.696);
    glVertex2f(313.131, 682.568); glVertex2f(305.662, 684.292);
    glVertex2f(297.618, 685.441); glVertex2f(287.851, 686.016);
    glVertex2f(280.382, 686.590); glVertex2f(270.040, 687.165);
    glVertex2f(257.974, 690.037); glVertex2f(246.483, 691.187);
    glVertex2f(237.865, 692.910); glVertex2f(228.097, 693.485);
    glVertex2f(218.904, 694.059); glVertex2f(213.734, 696.358);
    glVertex2f(206.839, 698.656); glVertex2f(200.519, 698.081);
    glVertex2f(191.901, 697.507); glVertex2f(184.431, 695.783);
    glVertex2f(177.537, 693.485); glVertex2f(169.493, 691.187);
    glVertex2f(163.747, 691.187); glVertex2f(156.853, 693.485);
    glVertex2f(143.064, 696.358); glVertex2f(132.722, 702.103);
    glVertex2f(118.358, 702.103); glVertex2f(103.419, 702.103);
    glVertex2f(87.332, 702.678);  glVertex2f(68.372, 703.252);
    glVertex2f(48.837, 707.274);  glVertex2f(35.048, 709.572);
    glVertex2f(22.982, 713.020);  glVertex2f(16.662, 716.467);
    glVertex2f(12.066, 717.041);  glVertex2f(5.746, 718.765);
    glVertex2f(1.149, 720.489);   glVertex2f(1.724, 640.051);
    glEnd();
}

void drawBottomLeftSand() {
    setSandShadow();
    glBegin(GL_POLYGON);
    glVertex2f(0.575, 718.191);   glVertex2f(-10.0, 770.0);
    glVertex2f(431.489, 761.857); glVertex2f(409.081, 756.686);
    glVertex2f(395.292, 754.387); glVertex2f(379.779, 752.664);
    glVertex2f(366.564, 750.940); glVertex2f(353.350, 748.642);
    glVertex2f(338.986, 745.769); glVertex2f(325.197, 742.896);
    glVertex2f(313.706, 740.024); glVertex2f(304.513, 737.725);
    glVertex2f(295.895, 735.427); glVertex2f(287.276, 734.278);
    glVertex2f(275.785, 731.405); glVertex2f(264.294, 729.107);
    glVertex2f(253.378, 728.532); glVertex2f(243.036, 727.958);
    glVertex2f(224.650, 725.660); glVertex2f(209.712, 724.511);
    glVertex2f(195.348, 723.362); glVertex2f(180.409, 722.212);
    glVertex2f(168.918, 721.638); glVertex2f(155.704, 721.063);
    glVertex2f(141.914, 719.914); glVertex2f(128.700, 718.765);
    glVertex2f(118.358, 718.191); glVertex2f(106.867, 717.041);
    glVertex2f(95.376, 716.467);  glVertex2f(75.266, 716.467);
    glVertex2f(62.626, 717.041);  glVertex2f(47.113, 717.616);
    glVertex2f(47.113, 717.616);  glVertex2f(29.302, 718.191);
    glVertex2f(1.724, 717.616);   glVertex2f(318.877, 740.598);
    glVertex2f(109.740, 718.191);
    glEnd();
}

// =========================================================================
//                             WATER & WAVE SYSTEM
// =========================================================================

float waveY(float x, float time, int layer){
    switch (layer){
    case 0:
        return sinf(x * 0.0085f + time)          * 14.0f
             + sinf(x * 0.0180f + time * 1.40f)  *  6.0f
             + sinf(x * 0.0035f + time * 0.60f)  *  5.0f;
    case 1:
        return sinf(x * 0.0075f + time * 0.85f + 0.90f) * 11.0f
             + sinf(x * 0.0140f + time * 1.10f + 0.40f) *  5.0f;
    case 2:
        return sinf(x * 0.0090f + time * 0.70f + 1.80f) *  9.0f
             + sinf(x * 0.0160f + time * 0.95f + 1.00f) *  4.0f;
    }
    return 0.0f;
}

void water(){
    int   x;
    float wy;
    float waveBase = winH * 0.82f;

    // Beautiful Water Background Gradient
    glBegin(GL_QUADS);
        glColor3f(0.04f, 0.24f, 0.47f); // Deep Ocean Night Blue
        glVertex2f(0.0f,        (float)winH);
        glVertex2f((float)winW, (float)winH);
        glColor3f(0.52f, 0.80f, 0.82f); // Lighter Cyan bottom
        glVertex2f((float)winW, 0.0f);
        glVertex2f(0.0f,        0.0f);
    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (x = 0; x <= winW; x += 2) {
        wy = waveBase + waveY((float)x, gTime, 0);
        glColor3f(0.01f, 0.01f, 0.07f);
        glVertex2f((float)x, (float)winH);
        glColor3f(0.05f, 0.09f, 0.24f);
        glVertex2f((float)x, wy);
    }
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glBegin(GL_QUAD_STRIP);
    for (x = 0; x <= winW; x += 2) {
        wy = waveBase + waveY((float)x, gTime, 0);
        glColor4f(0.55f, 0.78f, 0.88f, 0.50f);
        glVertex2f((float)x, wy);
        glColor4f(0.20f, 0.50f, 0.65f, 0.00f);
        glVertex2f((float)x, wy - 28.0f);
    }
    glEnd();

    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    for (x = 0; x <= winW; x += 2) {
        wy = waveBase + waveY((float)x, gTime, 0);
        glColor4f(0.72f, 0.89f, 0.95f, 0.90f);
        glVertex2f((float)x, wy);
    }
    glEnd();

    glLineWidth(1.2f);
    glBegin(GL_LINE_STRIP);
    for (x = 0; x <= winW; x += 2) {
        wy = waveBase + waveY((float)x, gTime, 0) - 3.0f;
        glColor4f(0.85f, 0.95f, 1.00f, 0.45f);
        glVertex2f((float)x, wy);
    }
    glEnd();

    {
        float w2Base = waveBase - 18.0f;

        glBegin(GL_QUAD_STRIP);
        for (x = 0; x <= winW; x += 3) {
            wy = w2Base + waveY((float)x, gTime, 1);
            glColor4f(0.30f, 0.58f, 0.72f, 0.30f);
            glVertex2f((float)x, wy);
            glColor4f(0.18f, 0.44f, 0.58f, 0.00f);
            glVertex2f((float)x, wy - 22.0f);
        }
        glEnd();

        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        for (x = 0; x <= winW; x += 2) {
            wy = w2Base + waveY((float)x, gTime, 1);
            glColor4f(0.52f, 0.74f, 0.84f, 0.45f);
            glVertex2f((float)x, wy);
        }
        glEnd();
    }

    {
        float w3Base = waveBase - 38.0f;

        glBegin(GL_QUAD_STRIP);
        for(x = 0; x <= winW; x += 3){
            wy = w3Base + waveY((float)x, gTime, 2);
            glColor4f(0.25f, 0.52f, 0.66f, 0.20f);
            glVertex2f((float)x, wy);
            glColor4f(0.15f, 0.40f, 0.54f, 0.00f);
            glVertex2f((float)x, wy - 18.0f);
        }
        glEnd();

        glLineWidth(1.5f);
        glBegin(GL_LINE_STRIP);
        for(x = 0; x <= winW; x += 2){
            wy = w3Base + waveY((float)x, gTime, 2);
            glColor4f(0.42f, 0.66f, 0.78f, 0.30f);
            glVertex2f((float)x, wy);
        }
        glEnd();
    }
}

// =========================================================================
//                             MAIN RENDERING
// =========================================================================

void display(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    // 1. Draw Water, Waves, and Background (Uses Bottom-Left Origin logic)
    water();
    
    // Disable blend/smooth from water code so sand draws completely solid
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);
    
    // 2. Transform the coordinates for the Sand
    // Your sand was drawn with a (0,0) Top-Left origin on a 1301x764 screen.
    // The water uses a Bottom-Left origin. We flip and scale it here perfectly!
    glPushMatrix();
    
    // A. Scale the sand design if the user resizes the window
    glScalef((float)winW / 1301.0f, (float)winH / 764.0f, 1.0f);
    // B. Push the sand to the bottom of the screen
    glTranslatef(0.0f, 764.0f, 0.0f);
    // C. Flip the Y-Axis so it's not upside down
    glScalef(1.0f, -1.0f, 1.0f);
    
    // 3. Draw the Sand Dunes
    drawFullBigSand();         
    drawBottomLeftSand();      
    drawBottomRightSand();     
    drawTopMiddleSand();       
    drawBottomRightUpperSand(); 
    drawBottomLeftUpperSand();  
    
    glPopMatrix(); // Restore coordinates for the next frame

    glutSwapBuffers();
}

void timer(int v){
    (void)v;
    if(!paused){
        gTime += 0.022f;
        glutPostRedisplay();
    }
    glutTimerFunc(16, timer, 0);
}

void reshape(int w, int h){
    winW = w;  winH = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h); // Standard Bottom-Left Origin
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y){
    (void)x; (void)y;
    if (key == 27)  exit(0);
    if (key == ' ') paused = !paused;
}

int main(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 20);
    
    // Set to your original window size
    glutInitWindowSize(winW, winH); 
    
    glutCreateWindow("Deep Sea Aquarium - Water Waves & Sand Base");
    glClearColor(0, 0, 0, 1);
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);
    
    glutMainLoop();
    return 0;
}