#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// ── Original variables ───────────────────────────────────────
float fanAngle = 0.0f;
float clockAngle = 0.0f;
float mouseX = -120.0f;
bool lampOn = false;
bool isAsleep = false;
bool isDreaming = false;
float sleepTimer = 0.0f;
float zoomFactor = 1.0f;
bool isSpaceScene = false;
float starX[300], starY[300];
bool starsInitialized = false;
float astroYOffset = 0.0f;
float astroRotation = 0.0f;
float mercuryAngle = 0.0f;
float venusAngle = 0.0f;
float earthAngle = 0.0f;
float rocketY = -120.0f;

// ── YOUR PART: Mars scene variables ─────────────────────────
bool isMarsScene       = false;   // true once ENTER pressed
bool marsTransition    = false;   // fade/travel in progress
float marsRocketY      = -120.0f; // rocket launching from space
float marsRocketLandY  = 0.0f;    // rocket descending to land
float marsTimer        = 0.0f;    // counts time in mars scene

// Mars cinematic phases:
//  0 = rocket landing
//  1 = astronaut exploration
//  2 = alien gathering
//  3 = detection
//  4 = attack
//  5 = escape launch
//  6 = warning transmission
int marsPhase = 0;

float girlWalkX    = 0.0f;   // girl walking out of rocket
float alienAppear  = 0.0f;   // 0..1 fade-in for aliens
float alien1X      = -55.0f; // alien 1 position
float alien2X      =  55.0f; // alien 2 position
float alienBobY    = 0.0f;   // aliens bobbing up/down
float marsPhaseTimer = 0.0f;
float escapeRocketY = -45.0f;
float warningAlpha = 0.0f;

bool isWakeUpScene = false;
bool isWallScene = false;
bool isWindowSpaceScene = false;
float wallSceneTimer = 0.0f;
float windowSpaceTimer = 0.0f;
float wakeTimer = 0.0f;
float wakeHeadLift = 0.0f;
float wakeStandProgress = 0.0f;
float wakeHeadTurn = 0.0f;

const float FrameTime = 1.0f / 60.0f;
const float WakeHeadLiftStart = 0.4f;
const float WakeHeadLiftMax = 3.0f;
const float WakeHeadTurnStart = 5.9f;
const float WakeHeadTurnMax = 18.0f;
const float WakeDreamTextEnd = 2.9f;
const float WakeSurpriseTextStart = 3.9f;
const float WakeSurpriseTextEnd = 5.9f;

const float WallWaitTime = 1.5f;
const float WallZoomTime = 6.0f;
const float WallTotalTime = WallWaitTime + WallZoomTime;
const float WallMaxZoom = 6.0f;

const float HouseTextStart = 2.0f;
const float HouseTextFirstEnd = 3.5f;
const float HouseTextEnd = 5.0f;

struct Alien {
    float x, y;
    float bobOffset;
    int type;
    float facingAngle;
    bool isLeader;
};

Alien aliens[5];
bool aliensInitialized = false;

// rocket travel across screen
float travelRocketX = 60.0f;
float travelRocketY = -120.0f;

// ── NEW: Travel cinematic variables ──────────────────────────
// Inserted as Phase -1: rocket flies toward Mars, Mars grows
bool  isTravelScene    = false;  // true = show travel cinematic
float marsApproachSize = 5.0f;   // Mars starts tiny, grows to ~80
float travelTimer      = 0.0f;   // time elapsed in travel
float rocketTravelX    = 0.0f;   // rocket X (slight left-right wobble)
float rocketTravelY    = -75.0f; // rocket fixed near bottom

// ── Helper: draw rectangle ────────────────────────────────────
void drawRect(float x, float y, float w, float h,
              float r, float g, float b)
{
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x,     y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x,     y + h);
    glEnd();
}

// ── Helper: draw circle ───────────────────────────────────────
void drawCircle(float cx, float cy, float r, int segs,
                float red, float green, float blue)
{
    glColor3f(red, green, blue);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segs; i++)
    {
        float theta = i * 2.0f * 3.1416f / segs;
        glVertex2f(cx + r * cosf(theta), cy + r * sinf(theta));
    }
    glEnd();
}

// ── Draw text string ─────────────────────────────────────────
void drawText(float x, float y, const char* str,
              void* font = GLUT_BITMAP_HELVETICA_12)
{
    glRasterPos2f(x, y);
    while (*str) { glutBitmapCharacter(font, *str++); }
}

float clampFloat(float value, float minValue, float maxValue)
{
    if (value < minValue) return minValue;
    if (value > maxValue) return maxValue;
    return value;
}

float smoothStep(float t)
{
    t = clampFloat(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

// ============================================================
//  DRAW: Rocket (reusable, drawn at local origin)
// ============================================================
void drawRocketAt(float cx, float cy, float flicker)
{
    glPushMatrix();
    glTranslatef(cx, cy, 0);
    glScalef(1.7f, 1.7f, 1.0f); // Rebalanced cinematic scale

    // Rocket Body (taller and slightly wider)
    drawRect(-8, 0, 16, 45, 0.85f, 0.85f, 0.88f);
    drawRect(-4, 0, 8, 45, 0.95f, 0.95f, 0.95f); // Highlight

    // Nose Cone
    glColor3f(0.8f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-8, 45); glVertex2f(8, 45); glVertex2f(0, 60);
    glEnd();
    glColor3f(1.0f, 0.2f, 0.2f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-4, 45); glVertex2f(4, 45); glVertex2f(0, 58);
    glEnd();

    // Fins
    glColor3f(0.7f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-8, 5); glVertex2f(-18, -5); glVertex2f(-8, 15);
        glVertex2f( 8, 5); glVertex2f( 18, -5); glVertex2f( 8, 15);
    glEnd();
    
    // Landing Gear/Struts
    glColor3f(0.4f, 0.4f, 0.45f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(-8, 8); glVertex2f(-14, -4);
        glVertex2f( 8, 8); glVertex2f( 14, -4);
    glEnd();

    // Windows
    drawCircle(0, 32, 4.5f, 20, 0.4f, 0.4f, 0.45f); // Rim
    drawCircle(0, 32, 3.5f, 20, 0.1f, 0.2f, 0.5f);  // Glass
    drawCircle(-1, 33, 1.0f, 10, 0.6f, 0.8f, 1.0f); // Reflection
    
    drawCircle(0, 18, 4.5f, 20, 0.4f, 0.4f, 0.45f); // Rim
    drawCircle(0, 18, 3.5f, 20, 0.1f, 0.2f, 0.5f);  // Glass
    drawCircle(-1, 19, 1.0f, 10, 0.6f, 0.8f, 1.0f); // Reflection

    // Body details
    drawRect(-8, 25, 16, 2, 0.7f, 0.1f, 0.1f);
    drawRect(-8, 11, 16, 2, 0.7f, 0.1f, 0.1f);

    // Smooth engine flame
    if (flicker > 0.0f)
    {
        float flame = 0.65f + flicker * 0.55f;
        glColor3f(1.0f, 0.35f + flicker * 0.35f, 0.0f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-6, 0); glVertex2f(6, 0); glVertex2f(0, -18.0f * flame);
        glEnd();
        glColor3f(1.0f, 0.90f, 0.20f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-3, 0); glVertex2f(3, 0); glVertex2f(0, -10.0f * flame);
        glEnd();
    }
    glPopMatrix();
}

// ============================================================
//  DRAW: Girl in Astronaut Suit (used on Mars surface)
//  cx,cy = feet position
//  Wears the SAME suit as the floating space astronaut
// ============================================================
void drawGirlStanding(float cx, float cy)
{
    glPushMatrix();
    glTranslatef(cx, cy, 0);
    glScalef(0.65f, 0.65f, 1.0f); // Reduce size slightly as requested to let rocket dominate

    // Animation cycle based on marsTimer
    float walkCycle = sinf(marsTimer * 18.0f);
    if (marsPhase > 1 && marsPhase < 5) walkCycle = 0.0f;

    // ── BACKPACK / PLSS ───────────────────────────────────────
    drawRect(-11, 20, 22, 18, 0.55f, 0.55f, 0.60f); // backpack body
    drawRect(-12, 22, 2, 14, 0.45f, 0.45f, 0.50f); // side detail
    drawRect( 10, 22, 2, 14, 0.45f, 0.45f, 0.50f); // side detail

    // ── RIGHT ARM (Background) ────────────────────────────────
    glPushMatrix();
    glTranslatef(7.0f, 35.0f, 0.0f); // shoulder hinge
    glRotatef(walkCycle * 20.0f - 10.0f, 0, 0, 1);
    // Upper arm
    drawRect(-3.0f, -10.0f, 6.0f, 10.0f, 0.85f, 0.85f, 0.88f);
    // Elbow
    drawRect(-3.5f, -12.0f, 7.0f, 3.0f, 0.65f, 0.65f, 0.70f);
    // Forearm
    drawRect(-2.5f, -18.0f, 5.0f, 6.0f, 0.85f, 0.85f, 0.88f);
    // Wrist ring
    drawRect(-3.0f, -19.0f, 6.0f, 2.0f, 0.5f, 0.5f, 0.55f);
    // Glove/Hand (Clearly visible ball with thumb hint)
    drawCircle(0.0f, -21.0f, 3.5f, 16, 0.4f, 0.4f, 0.45f);
    drawCircle(-2.5f, -20.0f, 1.5f, 10, 0.4f, 0.4f, 0.45f); // thumb
    glPopMatrix();

    // ── RIGHT LEG (Background) ─────────────────────────────────
    glPushMatrix();
    glTranslatef(4.0f, 20.0f, 0.0f); // hinge at hip
    glRotatef(-walkCycle * 15.0f, 0, 0, 1);
    drawRect(-3.5f, -18.0f, 7.0f, 18.0f, 0.85f, 0.85f, 0.88f); 
    drawRect(-4.5f, -10.0f, 9.0f, 3.0f, 0.65f, 0.65f, 0.70f);
    drawRect(-4.5f, -24.0f, 9.0f, 6.0f, 0.75f, 0.75f, 0.80f);
    drawRect(-5.5f, -25.0f, 11.0f, 2.0f, 0.3f, 0.3f, 0.35f); // sole
    glPopMatrix();

    // ── LEFT LEG (Foreground) ─────────────────────────────────
    glPushMatrix();
    glTranslatef(-4.0f, 20.0f, 0.0f); // hinge at hip
    glRotatef(walkCycle * 15.0f, 0, 0, 1);
    drawRect(-3.5f, -18.0f, 7.0f, 18.0f, 0.90f, 0.90f, 0.93f); 
    drawRect(-4.5f, -10.0f, 9.0f, 3.0f, 0.70f, 0.70f, 0.75f);
    drawRect(-4.5f, -24.0f, 9.0f, 6.0f, 0.80f, 0.80f, 0.85f);
    drawRect(-5.5f, -25.0f, 11.0f, 2.0f, 0.25f, 0.25f, 0.28f); // sole
    glPopMatrix();

    // ── TORSO ─────────────────────────────────────────────────
    drawRect(-9, 20, 18, 18, 0.92f, 0.92f, 0.95f);
    drawRect(-9, 20, 3, 18, 0.85f, 0.85f, 0.88f); // Shading
    
    // Belt
    drawRect(-10, 18, 20, 3, 0.6f, 0.6f, 0.65f);
    drawRect(-3, 17, 6, 5, 0.7f, 0.7f, 0.75f); // buckle

    // ── CHEST PANEL ───────────────────────────────────────────
    drawRect(-5, 24, 10, 9, 0.70f, 0.72f, 0.75f);
    glColor3f(0.40f, 0.40f, 0.45f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-5, 24); glVertex2f(5, 24);
        glVertex2f( 5, 33); glVertex2f(-5, 33);
    glEnd();
    drawCircle(-2.5f, 30.5f, 1.2f, 10, 1.0f, 0.2f, 0.2f); // red
    drawCircle( 0.0f, 30.5f, 1.2f, 10, 0.2f, 1.0f, 0.2f); // green
    drawCircle( 2.5f, 30.5f, 1.2f, 10, 0.2f, 0.6f, 1.0f); // blue
    drawRect(-3.5f, 25.5f, 7.0f, 3.5f, 0.1f, 0.8f, 0.9f); // bright cyan screen

    // ── LEFT ARM (Foreground) ─────────────────────────────────
    glPushMatrix();
    glTranslatef(-7.0f, 35.0f, 0.0f); // shoulder hinge
    glRotatef(-walkCycle * 20.0f + 10.0f, 0, 0, 1);
    drawCircle(0.0f, 0.0f, 4.0f, 15, 0.9f, 0.9f, 0.93f); // Shoulder joint
    drawCircle(0.0f, 0.0f, 2.5f, 10, 0.7f, 0.7f, 0.75f);
    drawRect(-3.0f, -10.0f, 6.0f, 10.0f, 0.90f, 0.90f, 0.93f);
    drawRect(-3.5f, -12.0f, 7.0f, 3.0f, 0.70f, 0.70f, 0.75f);
    drawRect(-2.5f, -18.0f, 5.0f, 6.0f, 0.90f, 0.90f, 0.93f);
    drawRect(-3.0f, -19.0f, 6.0f, 2.0f, 0.55f, 0.55f, 0.60f); // Wrist ring
    // Glove/Hand (Clearly visible ball with thumb hint)
    drawCircle(0.0f, -21.0f, 3.5f, 16, 0.45f, 0.45f, 0.50f);
    drawCircle(2.5f, -20.0f, 1.5f, 10, 0.45f, 0.45f, 0.50f); // thumb
    glPopMatrix();

    // ── NECK RING ─────────────────────────────────────────────
    drawRect(-7, 37, 14, 4, 0.65f, 0.65f, 0.70f);

    // ── HELMET ────────────────────────────────────────────────
    drawCircle(0, 48, 11, 40, 1.00f, 1.00f, 1.00f);
    glColor3f(0.85f, 0.85f, 0.88f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0, 48);
        for (int i = 20; i <= 40; i++) {
            float t = i * 3.14159f / 20;
            glVertex2f(0 + 11.0f * cosf(t), 48 + 11.0f * sinf(t));
        }
    glEnd();
    drawCircle(0, 48, 8, 30, 0.05f, 0.1f, 0.2f);
    glColor3f(0.8f, 0.6f, 0.1f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0, 48);
        for (int i = 0; i <= 15; i++) {
            float t = i * 3.14159f / 15;
            glVertex2f(0 + 8.0f * cosf(t), 48 + 8.0f * sinf(t));
        }
    glEnd();
    drawCircle(-3, 51, 2.5f, 15, 1.0f, 1.0f, 1.0f);
    drawCircle(-1.5f, 49.5f, 1.0f, 10, 0.9f, 0.9f, 0.9f);
    drawCircle(-9, 55, 2.0f, 15, 1.0f, 1.0f, 0.5f);

    glPopMatrix();
}

// ============================================================
//  DRAW: Alien
//  style 0 = green classic alien
//  style 1 = purple squid alien
// ============================================================
void drawAlien(float cx, float cy, int style, float bob)
{
    glPushMatrix();
    glTranslatef(cx, cy + bob, 0);

    if (style == 0)
    {
        // ── Green alien ──
        // Legs / tentacles
        glColor3f(0.1f, 0.7f, 0.1f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-8, 0); glVertex2f(-12, -14); glVertex2f(-5, -14);
            glVertex2f( 0, 0); glVertex2f( -3, -14); glVertex2f(  3, -14);
            glVertex2f( 8, 0); glVertex2f(  5, -14); glVertex2f( 12, -14);
        glEnd();
        // Body (oval)
        drawCircle(0, 8, 10, 30, 0.1f, 0.8f, 0.1f);
        // Head
        drawCircle(0, 22, 12, 30, 0.1f, 0.9f, 0.1f);
        // Big eyes
        drawCircle(-5, 24, 4, 20, 0.0f, 0.0f, 0.0f);
        drawCircle( 5, 24, 4, 20, 0.0f, 0.0f, 0.0f);
        drawCircle(-5, 24, 2, 20, 1.0f, 0.0f, 0.0f);
        drawCircle( 5, 24, 2, 20, 1.0f, 0.0f, 0.0f);
        drawCircle(-5, 24, 1, 10, 1.0f, 1.0f, 1.0f);
        drawCircle( 5, 24, 1, 10, 1.0f, 1.0f, 1.0f);
        // Antenna
        glColor3f(0.0f, 0.6f, 0.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            glVertex2f(-4, 34); glVertex2f(-7, 42);
            glVertex2f( 4, 34); glVertex2f( 7, 42);
        glEnd();
        drawCircle(-7, 43, 2, 10, 1.0f, 1.0f, 0.0f);
        drawCircle( 7, 43, 2, 10, 1.0f, 1.0f, 0.0f);
        // Mouth
        glColor3f(0.0f, 0.4f, 0.0f);
        glBegin(GL_LINE_STRIP);
            glVertex2f(-4, 16); glVertex2f(0, 14); glVertex2f(4, 16);
        glEnd();
        // Arms
        glColor3f(0.1f, 0.8f, 0.1f);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
            glVertex2f(-10, 10); glVertex2f(-20, 18);
            glVertex2f( 10, 10); glVertex2f( 20, 18);
        glEnd();
        drawCircle(-20, 18, 2, 10, 0.1f, 0.8f, 0.1f);
        drawCircle( 20, 18, 2, 10, 0.1f, 0.8f, 0.1f);
    }
    else
    {
        // ── Purple squid alien ──
        // Tentacles
        glColor3f(0.5f, 0.0f, 0.7f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-10,  0); glVertex2f(-14, -18); glVertex2f(-7, -18);
            glVertex2f( -3,  0); glVertex2f( -5, -18); glVertex2f( 2, -18);
            glVertex2f(  3,  0); glVertex2f(  2, -18); glVertex2f( 7, -18);
            glVertex2f( 10,  0); glVertex2f(  7, -18); glVertex2f(14, -18);
        glEnd();
        // Body
        drawCircle(0, 10, 12, 30, 0.6f, 0.0f, 0.8f);
        // Head dome
        drawCircle(0, 24, 13, 30, 0.7f, 0.0f, 0.9f);
        // Eyes (three eyes!)
        drawCircle(-6, 26, 3, 20, 1.0f, 1.0f, 0.0f);
        drawCircle( 6, 26, 3, 20, 1.0f, 1.0f, 0.0f);
        drawCircle( 0, 30, 3, 20, 1.0f, 1.0f, 0.0f);
        drawCircle(-6, 26, 1.2f, 10, 0.0f, 0.0f, 0.0f);
        drawCircle( 6, 26, 1.2f, 10, 0.0f, 0.0f, 0.0f);
        drawCircle( 0, 30, 1.2f, 10, 0.0f, 0.0f, 0.0f);
        // Antenna (one, glowing)
        glColor3f(0.4f, 0.0f, 0.6f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            glVertex2f(0, 37); glVertex2f(0, 46);
        glEnd();
        drawCircle(0, 47, 3, 15, 1.0f, 0.0f, 1.0f);
        // Mouth (wavy)
        glColor3f(1.0f, 0.5f, 0.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
            glVertex2f(-5, 18); glVertex2f(-2, 16);
            glVertex2f( 2, 19); glVertex2f( 5, 17);
        glEnd();
        // Arms
        glColor3f(0.6f, 0.0f, 0.8f);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
            glVertex2f(-12,  8); glVertex2f(-22, 18);
            glVertex2f( 12,  8); glVertex2f( 22, 18);
        glEnd();
        drawCircle(-22, 18, 2, 10, 0.7f, 0.0f, 0.9f);
        drawCircle( 22, 18, 2, 10, 0.7f, 0.0f, 0.9f);
    }

    glPopMatrix();
}

void initAlienFormation()
{
    if (aliensInitialized) return;

    aliens[0] = {56.0f, -31.0f, 0.0f, 1,   0.0f, true};
    aliens[1] = {24.0f, -52.0f, 1.2f, 0,  14.0f, false};
    aliens[2] = {88.0f, -52.0f, 2.4f, 0, -14.0f, false};
    aliens[3] = {38.0f, -75.0f, 3.6f, 1,   8.0f, false};
    aliens[4] = {74.0f, -75.0f, 4.8f, 0,  -8.0f, false};

    aliensInitialized = true;
}

void drawHostileAlien(float cx, float cy, int style, float bob, float angle, float alert, float scale)
{
    glPushMatrix();
    glTranslatef(cx, cy + bob, 0);
    glRotatef(angle, 0, 0, 1);
    glScalef(scale, scale, 1.0f);
    drawAlien(0, 0, style, 0);

    float glow = 0.35f + alert * 0.65f;
    if (style == 0)
    {
        drawCircle(-5, 24, 3.0f + alert * 1.8f, 20, 1.0f, 0.05f, 0.0f);
        drawCircle( 5, 24, 3.0f + alert * 1.8f, 20, 1.0f, 0.05f, 0.0f);
    }
    else
    {
        drawCircle(-6, 26, 2.8f + alert * 1.5f, 20, 1.0f, glow, 0.0f);
        drawCircle( 6, 26, 2.8f + alert * 1.5f, 20, 1.0f, glow, 0.0f);
        drawCircle( 0, 30, 2.8f + alert * 1.5f, 20, 1.0f, glow, 0.0f);
    }

    glColor3f(0.9f, 0.05f, 0.05f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
        glVertex2f(-14, 10); glVertex2f(-27, 8);
        glVertex2f( 14, 10); glVertex2f( 27, 8);
    glEnd();
    glPopMatrix();
}

void drawAlienGroup()
{
    initAlienFormation();

    float alert = 0.0f;
    if (marsPhase >= 3)
        alert = marsPhase == 3 ? marsPhaseTimer / 1.2f : 1.0f;
    if (alert > 1.0f) alert = 1.0f;

    float bobScale = marsPhase == 3 ? 0.05f : 0.65f;
    if (marsPhase >= 4) bobScale = 1.1f;

    drawCircle(56, -61, 22, 36, 0.20f, 0.04f, 0.03f);
    drawCircle(56, -61, 10, 24, 0.85f, 0.18f, 0.02f);
    glColor3f(1.0f, 0.20f + alert * 0.50f, 0.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 40; i++)
    {
        float t = i * 2.0f * 3.1416f / 40;
        glVertex2f(56 + cosf(t) * 30, -61 + sinf(t) * 12);
    }
    glEnd();

    for (int i = 0; i < 5; i++)
    {
        Alien alien = aliens[i];
        float bob = sinf(marsTimer * (alien.isLeader ? 3.4f : 2.4f) + alien.bobOffset) *
                    (alien.isLeader ? 2.4f : 1.4f) * bobScale;
        float angle = alien.facingAngle + sinf(marsTimer * 1.5f + alien.bobOffset) * 3.0f;
        float alienAlert = alert;
        float scale = alien.isLeader ? 0.68f : 0.58f;

        if (marsPhase == 3)
        {
            if (alien.isLeader)
            {
                angle = -54.0f;
                scale += 0.10f * alert;
            }
            else
            {
                angle = alien.facingAngle;
                alienAlert *= 0.45f;
            }
        }
        else if (marsPhase >= 4)
        {
            angle = -48.0f - i * 4.0f;
            scale += 0.06f;
        }

        drawHostileAlien(alien.x, alien.y, alien.type, bob, angle, alienAlert, scale);
    }
}

void drawAlienThreatText()
{
    const char* msg = "We will attack humans";
    if (marsPhaseTimer > 1.3f) msg = "Humanity must be destroyed";
    if (marsPhaseTimer > 2.6f) msg = "Prepare for invasion";

    float pulse = (sinf(marsTimer * 7.0f) + 1.0f) * 0.5f;
    glColor3f(1.0f, 0.08f + pulse * 0.18f, 0.03f);
    drawText(12, 28, msg, GLUT_BITMAP_HELVETICA_18);

    glColor3f(0.35f + pulse * 0.35f, 0.0f, 0.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
        glVertex2f(35, 22); glVertex2f(58, -8);
    glEnd();
}

void drawLaser(float x1, float y1, float x2, float y2, float phase, float r, float g, float b)
{
    float pulse = (sinf(phase) + 1.0f) * 0.5f;
    glLineWidth(4.0f + pulse * 3.0f);
    glColor3f(r, g, b);
    glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    glEnd();
    glLineWidth(1.5f);
    glColor3f(1.0f, 0.9f, 0.55f);
    glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    glEnd();
    drawCircle(x2, y2, 3.0f + pulse * 3.0f, 18, 1.0f, 0.25f + pulse * 0.55f, 0.0f);
}

void drawWarningText()
{
    float blink = (sinf(marsTimer * 9.0f) + 1.0f) * 0.5f;
    float intensity = warningAlpha * (0.45f + blink * 0.55f);

    drawRect(-100, -100, 200, 200, 0.01f, 0.0f, 0.0f);

    glColor3f(intensity, 0.0f, 0.0f);
    glLineWidth(2.0f);
    for (int y = -70; y <= 70; y += 18)
    {
        glBegin(GL_LINES);
            glVertex2f(-90, (float)y);
            glVertex2f( 90, (float)y);
        glEnd();
    }

    glColor3f(1.0f, 0.10f + intensity * 0.35f, 0.05f);
    drawText(-88, 12, "WARNING: Aliens are planning to attack Earth!", GLUT_BITMAP_HELVETICA_18);
    glColor3f(0.75f, 0.95f, 0.85f);
    drawText(-44, -8, "TRANSMISSION RECEIVED", GLUT_BITMAP_HELVETICA_12);
}

// ============================================================
//  DRAW: Travel Cinematic Scene  ← NEW EXTRA SCENARIO
//  Rocket at bottom flies toward Mars.
//  Mars starts as a tiny dot and grows bigger and bigger
//  as the rocket approaches — filling the whole screen.
// ============================================================
void drawTravelScene()
{
    // ── Deep black space background ───────────────────────────
    drawRect(-100, -100, 200, 200, 0.0f, 0.0f, 0.04f);

    // ── Streaking stars (speed lines — gives sense of movement)
    glLineWidth(1.0f);
    for (int i = 0; i < 300; i++)
    {
        // Stars streak from centre outward the faster we go
        float speed = 0.3f + (marsApproachSize / 80.0f) * 2.5f;
        float sx = starX[i];
        float sy = starY[i];
        // Streak length grows as Mars gets closer (faster travel)
        float streakLen = speed * 4.0f;
        float brightness = 0.5f + (marsApproachSize / 80.0f) * 0.5f;
        glColor3f(brightness, brightness, brightness);
        glBegin(GL_LINES);
            glVertex2f(sx, sy);
            glVertex2f(sx * (1.0f - streakLen / 100.0f),
                       sy * (1.0f - streakLen / 100.0f));
        glEnd();
    }

    // ── MARS — grows from tiny dot to full planet ─────────────
    float r = marsApproachSize; // grows 5 → 80 over time

    // Mars base colour
    drawCircle(0, 30, r, 60, 0.75f, 0.25f, 0.05f);

    // Surface details only visible when close enough (r > 20)
    if (r > 20)
    {
        // Canyon band (Valles Marineris)
        glColor3f(0.45f, 0.12f, 0.02f);
        glBegin(GL_QUADS);
            glVertex2f(-r*0.90f, 30 - r*0.05f);
            glVertex2f( r*0.90f, 30 - r*0.05f);
            glVertex2f( r*0.80f, 30 + r*0.12f);
            glVertex2f(-r*0.80f, 30 + r*0.12f);
        glEnd();

        // Lighter highland band
        glColor3f(0.82f, 0.38f, 0.12f);
        glBegin(GL_QUADS);
            glVertex2f(-r*0.85f, 30 + r*0.25f);
            glVertex2f( r*0.85f, 30 + r*0.25f);
            glVertex2f( r*0.75f, 30 + r*0.42f);
            glVertex2f(-r*0.75f, 30 + r*0.42f);
        glEnd();

        // Olympus Mons volcano
        drawCircle(-r*0.28f, 30 + r*0.15f, r*0.16f, 25, 0.60f, 0.20f, 0.04f);
        drawCircle(-r*0.28f, 30 + r*0.15f, r*0.07f, 15, 0.50f, 0.15f, 0.02f);

        // Hellas impact basin
        drawCircle( r*0.38f, 30 - r*0.22f, r*0.18f, 25, 0.55f, 0.16f, 0.03f);

        // Small craters
        drawCircle( r*0.10f, 30 + r*0.35f, r*0.05f, 12, 0.50f, 0.14f, 0.02f);
        drawCircle(-r*0.50f, 30 + r*0.10f, r*0.04f, 10, 0.50f, 0.14f, 0.02f);
        drawCircle( r*0.55f, 30 + r*0.10f, r*0.04f, 10, 0.50f, 0.14f, 0.02f);

        // North polar ice cap (white top)
        drawCircle(0, 30 + r*0.78f, r*0.22f, 25, 0.93f, 0.93f, 1.00f);

        // South polar ice cap (smaller)
        drawCircle(0, 30 - r*0.80f, r*0.14f, 20, 0.90f, 0.90f, 0.97f);

        // Atmosphere glow ring
        glColor3f(0.80f, 0.35f, 0.10f);
        glLineWidth(2.5f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 60; i++) {
            float t = i * 2.0f * 3.1416f / 60;
            glVertex2f(0 + (r+3) * cosf(t), 30 + (r+3) * sinf(t));
        }
        glEnd();
        glColor3f(0.55f, 0.20f, 0.05f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 60; i++) {
            float t = i * 2.0f * 3.1416f / 60;
            glVertex2f(0 + (r+6) * cosf(t), 30 + (r+6) * sinf(t));
        }
        glEnd();

        // Phobos moon orbiting
        float phAngle = travelTimer * 2.5f;
        float phX = 0   + (r + 14) * cosf(phAngle);
        float phY = 30  + (r + 14) * 0.35f * sinf(phAngle);
        drawCircle(phX, phY, 3.5f, 12, 0.70f, 0.60f, 0.50f);
    }

    // Feature labels appear when Mars is big enough
    if (r > 45)
    {
        glColor3f(1.0f, 1.0f, 0.5f);
        glLineWidth(1.0f);
        // Olympus Mons label
        glBegin(GL_LINES);
            glVertex2f(-r*0.28f, 30 + r*0.15f);
            glVertex2f(-r*0.28f - 12, 30 + r*0.15f + 12);
        glEnd();
        drawText(-r*0.28f - 40, 30 + r*0.15f + 14,
                 "Olympus Mons", GLUT_BITMAP_HELVETICA_10);

        // North Ice Cap label
        glBegin(GL_LINES);
            glVertex2f(4, 30 + r*0.78f);
            glVertex2f(14, 30 + r*0.78f + 8);
        glEnd();
        drawText(15, 30 + r*0.78f + 7,
                 "N. Ice Cap", GLUT_BITMAP_HELVETICA_10);
    }

    // ── ROCKET at bottom — fixed, pointing up toward Mars ─────
    glPushMatrix();
    glTranslatef(rocketTravelX, rocketTravelY, 0);

    // Slight left-right wobble as it flies
    glRotatef(sinf(travelTimer * 1.5f) * 3.0f, 0, 0, 1);

    // Rocket body
    drawRect(-5, 0, 10, 25, 0.90f, 0.90f, 0.92f);
    // Nose cone
    glColor3f(1.0f, 0.20f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-5, 25); glVertex2f(5, 25); glVertex2f(0, 37);
    glEnd();
    // Fins
    glBegin(GL_TRIANGLES);
        glVertex2f(-5, 0); glVertex2f(-13, 0); glVertex2f(-5, 9);
        glVertex2f( 5, 0); glVertex2f( 13, 0); glVertex2f( 5, 9);
    glEnd();
    // Window
    drawCircle(0, 15, 3, 20, 0.10f, 0.20f, 0.55f);

    // Engine flame — flickers
    float flicker = (sinf(travelTimer * 20.0f) + 1.0f) * 0.5f;
    glColor3f(1.0f, 0.45f + flicker * 0.2f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-5, 0); glVertex2f(5, 0);
        glVertex2f(0, -18 - flicker * 8);
    glEnd();
    glColor3f(1.0f, 1.0f, 0.3f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-3, 0); glVertex2f(3, 0);
        glVertex2f(0, -10 - flicker * 5);
    glEnd();
    // Exhaust glow dot
    drawCircle(0, -3, 4 + flicker*2, 15, 1.0f, 0.6f, 0.1f);

    glPopMatrix();

    // ── Cinematic text messages ────────────────────────────────
    if (travelTimer < 2.0f)
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(-45, 85, "Heading to Mars...", GLUT_BITMAP_HELVETICA_18);
    }
    else if (travelTimer < 4.5f)
    {
        glColor3f(1.0f, 0.85f, 0.30f);
        drawText(-35, 85, "There it is!", GLUT_BITMAP_HELVETICA_18);
    }
    else
    {
        glColor3f(0.50f, 1.0f, 0.50f);
        drawText(-40, 85, "Preparing to land...", GLUT_BITMAP_HELVETICA_18);
    }

    // Hint at bottom
    glColor3f(0.45f, 0.45f, 0.50f);
    drawText(-40, -95, "Press R to restart", GLUT_BITMAP_HELVETICA_12);
}

// ============================================================
//  DRAW: Wake-up ending after the warning transmission
// ============================================================
void wakeUpFunction()
{
    glPushMatrix();

    // Morning lighting
    glBegin(GL_QUADS);
        glColor3f(0.53f, 0.81f, 0.98f);
        glVertex2f(-100, 100); glVertex2f(100, 100);
        glColor3f(0.26f, 0.40f, 0.49f);
        glVertex2f(100, -20); glVertex2f(-100, -20);
    glEnd();
    // Floor with depth
    glBegin(GL_QUADS);
        glColor3f(0.85f, 0.45f, 0.1f);
        glVertex2f(-100, -20); glVertex2f(100, -20);
        glColor3f(0.5f, 0.25f, 0.05f);
        glVertex2f(100, -100); glVertex2f(-100, -100);
    glEnd();

    drawRect(-75, 40, 25, 30, 0.3f, 0.2f, 0.1f);
    drawRect(-72, 43, 19, 24, 0.9f, 0.9f, 1.0f);
    drawCircle(-60, 58, 4, 20, 1.0f, 0.9f, 0.0f);
    drawRect(-72, 43, 19, 8, 0.2f, 0.6f, 0.2f);

    drawRect(50, 40, 25, 30, 0.3f, 0.2f, 0.1f);
    drawRect(53, 43, 19, 24, 1.0f, 0.9f, 0.8f);
    drawCircle(63, 50, 6, 20, 1.0f, 0.4f, 0.0f);
    drawRect(53, 43, 19, 5, 0.4f, 0.2f, 0.1f);

    drawRect(-1, 80, 2, 20, 0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glTranslatef(0, 80, 0);
    glRotatef(fanAngle, 0, 0, 1);
    drawCircle(0, 0, 4, 20, 0.3f, 0.3f, 0.3f);
    drawRect(-25, -2, 20, 4, 0.4f, 0.2f, 0.1f);
    drawRect(5,   -2, 20, 4, 0.4f, 0.2f, 0.1f);
    glPopMatrix();

    // GIRL BODY (Behind Desk)
    glPushMatrix();
    glTranslatef(0, -6, 0);

    // Chair (behind girl)
    drawRect(-28, -45, 26, 25, 0.35f, 0.18f, 0.08f);
    drawRect(-26, -60, 3, 15, 0.2f, 0.1f, 0.02f);
    drawRect( -7, -60, 3, 15, 0.2f, 0.1f, 0.02f);

    // Legs (shorter, natural)
    drawRect(-24, -55, 7, 25, 0.95f, 0.8f, 0.65f);
    drawRect(-13, -55, 7, 25, 0.95f, 0.8f, 0.65f);
    // Feet
    drawRect(-26, -60, 9, 5, 0.4f, 0.1f, 0.1f);
    drawRect(-14, -60, 9, 5, 0.4f, 0.1f, 0.1f);

    // Torso
    glColor3f(0.85f, 0.35f, 0.35f);
    glBegin(GL_POLYGON);
        glVertex2f(-26, -28); glVertex2f(-22, 5);
        glVertex2f( -8,   5); glVertex2f( -4,-28);
    glEnd();

    // Neck
    drawCircle(-15, 5, 3.5f, 20, 1.0f, 0.8f, 0.6f);

    // Head
    glPushMatrix();
    glTranslatef(-15.0f, 16.0f + wakeHeadLift, 0.0f);
    glRotatef(wakeHeadTurn, 0, 0, 1);
    glTranslatef(15.0f, -16.0f - wakeHeadLift, 0.0f);

    // Hair Back
    drawCircle(-15, 20 + wakeHeadLift, 12, 30, 0.1f, 0.1f, 0.1f);
    drawCircle(-22, 12 + wakeHeadLift, 10, 30, 0.1f, 0.1f, 0.1f);
    drawCircle(-8,  12 + wakeHeadLift, 10, 30, 0.1f, 0.1f, 0.1f);
    drawRect(-28, -2 + wakeHeadLift, 26, 16, 0.1f, 0.1f, 0.1f);

    drawCircle(-15, 16 + wakeHeadLift, 10.0f, 30, 1.0f, 0.8f, 0.6f);

    if (wakeHeadLift < 2.0f)
    {
        // Still groggy / waking up
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            glVertex2f(-19.5f,15.5f + wakeHeadLift); glVertex2f(-16.5f,15.5f + wakeHeadLift);
            glVertex2f(-13.5f,15.5f + wakeHeadLift); glVertex2f(-10.5f,15.5f + wakeHeadLift);
        glEnd();
        drawCircle(-15, 11 + wakeHeadLift, 1, 10, 0.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
            glVertex2f(-20,18 + wakeHeadLift); glVertex2f(-17,18 + wakeHeadLift);
            glVertex2f(-13,18 + wakeHeadLift); glVertex2f(-10,18 + wakeHeadLift);
        glEnd();
    }
    else
    {
        // Awake and surprised
        drawCircle(-18,17 + wakeHeadLift, 2.0f,15,1.0f,1.0f,1.0f);
        drawCircle(-12,17 + wakeHeadLift, 2.0f,15,1.0f,1.0f,1.0f);
        drawCircle(-18,16.5f + wakeHeadLift,1.0f,10,0.1f,0.1f,0.2f);
        drawCircle(-12,16.5f + wakeHeadLift,1.0f,10,0.1f,0.1f,0.2f);
        glColor3f(0.8f,0.2f,0.2f);
        glLineWidth(2.0f);
        drawCircle(-15, 12.0f + wakeHeadLift, 1.5f, 10, 0.1f, 0.0f, 0.0f);
        glColor3f(0.2f,0.1f,0.1f);
        glBegin(GL_LINES);
            glVertex2f(-21,20 + wakeHeadLift); glVertex2f(-17,22 + wakeHeadLift);
            glVertex2f(-14,22 + wakeHeadLift); glVertex2f(-10,20 + wakeHeadLift);
        glEnd();
    }

    glPopMatrix();
    glPopMatrix();

    // DESK (In Front of Girl Body)
    // Wider Desk for better composition
    drawRect(-40, -60, 4, 30, 0.3f, 0.15f, 0.05f); // Left leg
    drawRect( 25, -60, 4, 30, 0.3f, 0.15f, 0.05f); // Right leg
    drawRect(-45, -30, 75, 10, 0.45f, 0.25f, 0.15f); // Desk top (X: -45 to 30)
    drawRect(-45, -35, 75,  5, 0.3f, 0.15f, 0.05f);  // Desk rim

    // GIRL ARMS (Over Desk)
    glPushMatrix();
    glTranslatef(0, -6, 0);
    // Arms on desk (slightly moved up by headLift to simulate waking stretch)
    glPushMatrix(); glTranslatef(-25, wakeHeadLift*0.5f, 0); glRotatef(30 - wakeHeadLift*5, 0, 0, 1);
    drawRect(-3, -20, 6, 22, 0.75f, 0.25f, 0.25f);
    drawCircle(0, -20, 3, 10, 1.0f, 0.8f, 0.6f); glPopMatrix();
    
    glPushMatrix(); glTranslatef(-5, wakeHeadLift*0.5f, 0); glRotatef(-30 + wakeHeadLift*5, 0, 0, 1);
    drawRect(-3, -20, 6, 22, 0.75f, 0.25f, 0.25f);
    drawCircle(0, -20, 3, 10, 1.0f, 0.8f, 0.6f); glPopMatrix();
    glPopMatrix();

    // OBJECTS ON DESK
    // Lamp Base (Lamp OFF)
    glPushMatrix();
    glTranslatef(20, -13, 0); // Move lamp onto the right side of the desk
    drawRect(-60, -7, 20, 4, 0.15f, 0.15f, 0.15f);
    drawRect(-51, -3, 2, 18, 0.25f, 0.25f, 0.25f);
    glColor3f(0.8f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
        glVertex2f(-65, 15); glVertex2f(-35, 15);
        glVertex2f(-42, 28); glVertex2f(-58, 28);
    glEnd();
    drawCircle(-50, -5, 1.5f, 20, 0.4f, 0.4f, 0.4f); // base button OFF
    glPopMatrix();

    // Moderate two-layer cabinet beside the window.
    drawRect(44, -20, 36, 56, 0.45f, 0.23f, 0.08f);
    drawRect(47, 8, 30, 25, 0.58f, 0.32f, 0.12f);
    drawRect(47, -17, 30, 25, 0.58f, 0.32f, 0.12f);
    drawRect(60, 9, 2, 23, 0.35f, 0.17f, 0.06f);
    drawRect(60, -16, 2, 23, 0.35f, 0.17f, 0.06f);
    drawCircle(57, 20, 1.5f, 12, 0.95f, 0.78f, 0.25f);
    drawCircle(67, 20, 1.5f, 12, 0.95f, 0.78f, 0.25f);
    drawCircle(57, -5, 1.5f, 12, 0.95f, 0.78f, 0.25f);
    drawCircle(67, -5, 1.5f, 12, 0.95f, 0.78f, 0.25f);

    // Plant on top of cabinet
    glColor3f(0.6f, 0.3f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(50, 52); glVertex2f(70, 52);
        glVertex2f(66, 36); glVertex2f(54, 36);
    glEnd();
    drawRect(59, 52, 2, 15, 0.13f, 0.35f, 0.13f);
    drawCircle(60, 67, 10, 20, 0.13f, 0.55f, 0.13f);
    drawCircle(52, 62,  7, 20, 0.13f, 0.45f, 0.13f);
    drawCircle(68, 62,  7, 20, 0.13f, 0.45f, 0.13f);

    // Laptop
    glPushMatrix();
    glTranslatef(-2, -20, 0);
    drawRect(-6, 0, 12, 8, 0.8f, 0.8f, 0.85f);
    drawRect(-5, 1, 10, 6, 0.1f, 0.1f, 0.15f);
    drawRect(-8, -2, 16, 2, 0.7f, 0.7f, 0.75f);
    glPopMatrix();

    // Books on Table
    glPushMatrix();
    glTranslatef(10, -20, 0);
    drawRect(0, 0, 10, 3, 0.2f, 0.4f, 0.8f);
    drawRect(0, 3, 9, 3, 0.8f, 0.3f, 0.2f);
    drawRect(-1, 6, 10, 3, 0.1f, 0.6f, 0.3f);
    glPushMatrix();
    glTranslatef(-1, 9, 0);
    glRotatef(12, 0, 0, 1);
    drawRect(0, 0, 10, 2.5f, 0.8f, 0.7f, 0.1f);
    glPopMatrix();
    glPopMatrix();

    // Clock on Table
    glPushMatrix();
    glTranslatef(-3, -12, 0);
    drawCircle(25, 0, 8, 30, 0.8f, 0.0f, 0.0f);
    drawCircle(25, 0, 7, 30, 1.0f, 1.0f, 1.0f);
    
    glPushMatrix();
    glTranslatef(25, 0, 0);
    glColor3f(0.8f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glPushMatrix();
    glRotatef(-clockAngle * 12, 0, 0, 1);
    glBegin(GL_LINES); glVertex2f(0,0); glVertex2f(0,6); glEnd();
    glPopMatrix();
    glPushMatrix();
    glRotatef(-clockAngle, 0, 0, 1);
    glBegin(GL_LINES); glVertex2f(0,0); glVertex2f(4,0); glEnd();
    glPopMatrix();
    glPopMatrix();
    
    // Clock legs
    glBegin(GL_TRIANGLES);
        glVertex2f(25,0); glVertex2f(20,-8); glVertex2f(30,-8);
    glEnd();
    glPopMatrix();

    drawCircle(mouseX,    -88, 3,   20, 0.6f, 0.3f, 0.1f);
    drawCircle(mouseX+3,  -88, 1.8f,20, 0.6f, 0.3f, 0.1f);
    drawCircle(mouseX+2,  -86, 1,   10, 0.7f, 0.5f, 0.3f);
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_LINES);
        glVertex2f(mouseX-3,-88); glVertex2f(mouseX-8,-86);
    glEnd();

    if (wakeHeadLift >= 2.9f && wakeTimer <= WakeDreamTextEnd)
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(-50, 33 + wakeHeadLift, "Oh! it was just a dream ", GLUT_BITMAP_HELVETICA_18);
    }
    else if (wakeTimer >= WakeSurpriseTextStart && wakeTimer <= WakeSurpriseTextEnd)
    {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(-50, 33 + wakeHeadLift, "Wait what is that?", GLUT_BITMAP_HELVETICA_18);
    }

    glPopMatrix();
}
void drawWallScene()
{
    glPushMatrix();

    float zoom = 1.0f;
    if (wallSceneTimer > WallWaitTime)
    {
        float zoomTime = wallSceneTimer - WallWaitTime;
        //zoom time
        float zoomProgress = smoothStep(zoomTime / WallZoomTime);
        zoom = 1.0f + zoomProgress * (WallMaxZoom - 1.0f);
    }

    glTranslatef(0.0f, 43.0f, 0.0f);
    glScalef(zoom, zoom, 1.0f);
    glTranslatef(0.0f, -43.0f, 0.0f);

    // original room wall with depth gradient
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.25f, 1.0f); // Top wall
        glVertex2f(-100, 100); glVertex2f(100, 100);
        glColor3f(0.0f, 0.2f, 0.8f);
        glVertex2f(100, 40);   glVertex2f(-100, 40);
        
        glColor3f(0.0f, 0.2f, 0.8f);
        glVertex2f(-100, 40);  glVertex2f(100, 40);
        glColor3f(0.0f, 0.1f, 0.4f); // Bottom wall
        glVertex2f(100, -20);  glVertex2f(-100, -20);
    glEnd();

    drawRect(-100, -100, 200, 80, 1.0f, 0.5f, 0.0f);

    // Ceiling Fan in Wall Scene
    drawRect(-1, 80, 2, 20, 0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glTranslatef(0, 80, 0);
    glRotatef(fanAngle, 0, 0, 1);
    drawCircle(0, 0, 4, 20, 0.3f, 0.3f, 0.3f);
    drawRect(-25, -2, 20, 4, 0.4f, 0.2f, 0.1f);
    drawRect(5,   -2, 20, 4, 0.4f, 0.2f, 0.1f);
    glPopMatrix();

    // Center window on the wall.
    drawRect(-35, 11, 70, 64, 0.28f, 0.16f, 0.08f);
    drawRect(-31, 15, 62, 56, 0.0f, 0.0f, 0.0f);
    drawCircle(-20, 60, 0.6f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(-10, 48, 0.6f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(-2, 62, 0.6f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(8, 52, 0.6f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(18, 63, 0.6f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(22, 42, 0.6f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(12, 31, 0.6f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(-18, 34, 0.6f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(-5, 25, 0.6f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(24, 23, 0.6f, 8, 1.0f, 1.0f, 1.0f);
    glColor3f(0.28f, 0.16f, 0.08f);
    glLineWidth(4.0f);
    glBegin(GL_LINES);
        glVertex2f(0, 15);  glVertex2f(0, 71);
        glVertex2f(-31, 43); glVertex2f(31, 43);
    glEnd();

    // Moderate two-layer cabinet beside the window.
    drawRect(44, -20, 36, 56, 0.45f, 0.23f, 0.08f);
    drawRect(47, 8, 30, 25, 0.58f, 0.32f, 0.12f);
    drawRect(47, -17, 30, 25, 0.58f, 0.32f, 0.12f);
    drawRect(60, 9, 2, 23, 0.35f, 0.17f, 0.06f);
    drawRect(60, -16, 2, 23, 0.35f, 0.17f, 0.06f);
    drawCircle(57, 20, 1.5f, 12, 0.95f, 0.78f, 0.25f);
    drawCircle(67, 20, 1.5f, 12, 0.95f, 0.78f, 0.25f);
    drawCircle(57, -5, 1.5f, 12, 0.95f, 0.78f, 0.25f);
    drawCircle(67, -5, 1.5f, 12, 0.95f, 0.78f, 0.25f);

    // Plant on top of cabinet (Consistent with room scenes)
    glColor3f(0.6f, 0.3f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(50, 52); glVertex2f(70, 52);
        glVertex2f(66, 36); glVertex2f(54, 36);
    glEnd();
    drawRect(59, 52, 2, 15, 0.13f, 0.35f, 0.13f);
    drawCircle(60, 67, 10, 20, 0.13f, 0.55f, 0.13f);
    drawCircle(52, 62,  7, 20, 0.13f, 0.45f, 0.13f);
    drawCircle(68, 62,  7, 20, 0.13f, 0.45f, 0.13f);

    // Three shoeboxes
    drawRect(-76, -20, 14, 8, 0.72f, 0.18f, 0.10f);
    drawRect(-76, -12, 14, 2, 0.95f, 0.75f, 0.30f);
    drawRect(-76, -10, 14, 8, 0.16f, 0.42f, 0.72f);
    drawRect(-76, -2, 14, 2, 0.90f, 0.90f, 0.95f);
    drawRect(-59, -20, 14, 8, 0.18f, 0.62f, 0.28f);
    drawRect(-59, -12, 14, 2, 0.95f, 0.82f, 0.28f);

    glPopMatrix();
}




void drawWindowSpaceScene()
{
    // Deep space background with a bright glow behind the floating house
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.0f, 0.02f);
        glVertex2f(-100, -100); glVertex2f(100, -100);
        glColor3f(0.03f, 0.06f, 0.12f);
        glVertex2f(100, 100); glVertex2f(-100, 100);
    glEnd();

    drawCircle(38, 32, 54, 60, 0.95f, 0.55f, 0.22f);
    drawCircle(38, 32, 42, 60, 1.0f, 0.68f, 0.34f);
    drawCircle(38, 32, 26, 60, 1.0f, 0.82f, 0.52f);

    //Earth view
    drawCircle(-92, -20, 118, 120, 0.02f, 0.12f, 0.22f);
    drawCircle(-88, -16, 108, 120, 0.05f, 0.30f, 0.48f);
    drawCircle(-112, 28, 22, 40, 0.20f, 0.55f, 0.32f);
    drawCircle(-78, 48, 28, 40, 0.22f, 0.62f, 0.36f);
    drawCircle(-48, -4, 24, 40, 0.18f, 0.50f, 0.30f);
    drawCircle(-106, -44, 28, 40, 0.12f, 0.45f, 0.28f);
    drawCircle(-68, -58, 20, 40, 0.12f, 0.40f, 0.26f);
    drawCircle(-68, 48, 28, 40, 0.92f, 0.92f, 0.88f);
    drawCircle(-116, -8, 34, 40, 0.86f, 0.88f, 0.86f);
    drawCircle(-58, -28, 28, 40, 0.88f, 0.88f, 0.84f);

    // Stars
    drawCircle(8, 86, 0.6f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(-18, 86, 0.5f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(58, 88, 0.6f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(82, 66, 0.5f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(92, 12, 0.5f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(72, -52, 0.5f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(96, 72, 0.5f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(96, -6, 0.4f, 8, 1.0f, 1.0f, 1.0f);
    drawCircle(95, -28, 0.5f, 8, 1.0f, 1.0f, 1.0f);

    drawCircle(-4, 10, 3.0f, 9, 0.22f, 0.15f, 0.08f);
    drawCircle(10, 4, 2.4f, 9, 0.22f, 0.15f, 0.08f);
    drawCircle(22, 10, 2.8f, 9, 0.22f, 0.15f, 0.08f);
    drawCircle(78, 16, 5.0f, 10, 0.22f, 0.15f, 0.08f);
    drawCircle(91, 30, 3.0f, 10, 0.22f, 0.15f, 0.08f);

    // Floating island ground below the house
    glColor3f(0.22f, 0.15f, 0.08f);
    glBegin(GL_POLYGON);
        glVertex2f(28, -8);
        glVertex2f(78, -8);
        glVertex2f(88, -18);
        glVertex2f(70, -38);
        glVertex2f(56, -62);
        glVertex2f(42, -38);
        glVertex2f(20, -18);
    glEnd();
    drawRect(26, -9, 54, 8, 0.20f, 0.48f, 0.16f);
    drawCircle(32, -2, 3.2f, 10, 0.22f, 0.15f, 0.08f);
    drawCircle(38, -4, 2.4f, 10, 0.22f, 0.15f, 0.08f);
    drawCircle(72, -4, 3.0f, 10, 0.22f, 0.15f, 0.08f);
    drawCircle(82, -15, 3.6f, 10, 0.22f, 0.15f, 0.08f);
    drawCircle(24, -16, 2.8f, 10, 0.22f, 0.15f, 0.08f);

    glColor3f(0.30f, 0.20f, 0.12f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(42, -20); glVertex2f(35, -42);
        glVertex2f(55, -24); glVertex2f(50, -58);
        glVertex2f(66, -22); glVertex2f(72, -48);
    glEnd();

    // house.
    drawRect(42, -2, 28, 28, 0.50f, 0.32f, 0.18f);
    drawRect(45, 1, 22, 22, 0.66f, 0.46f, 0.28f);
    drawRect(53.5f, 1, 6, 13, 0.22f, 0.12f, 0.06f);
    drawCircle(58.5f, 7, 0.8f, 8, 0.95f, 0.75f, 0.25f);
    drawRect(61, 11, 6, 7, 0.95f, 0.72f, 0.30f);
    drawRect(45, 11, 6, 7, 0.95f, 0.72f, 0.30f);

    glColor3f(0.12f, 0.16f, 0.20f);
    glBegin(GL_QUADS);
        glVertex2f(38, 24); glVertex2f(74, 24);
        glVertex2f(65, 42); glVertex2f(47, 42);
    glEnd();
    glColor3f(0.20f, 0.28f, 0.34f);
    glBegin(GL_TRIANGLES);
        glVertex2f(42, 24); glVertex2f(56, 45); glVertex2f(72, 24);
    glEnd();

    if (windowSpaceTimer >= HouseTextStart && windowSpaceTimer < HouseTextEnd)
    {
        glColor3f(1.0f, 0.92f, 0.35f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            glVertex2f(48, 14); glVertex2f(62, -70);
        glEnd();

        if (windowSpaceTimer < HouseTextFirstEnd)
            drawText(57, -76, "What!!", GLUT_BITMAP_HELVETICA_18);
        else
            drawText(57, -76, "My house is in space.", GLUT_BITMAP_HELVETICA_18);
    }
}

void drawMarsScene()
{
    if (marsPhase == 6)
    {
        drawWarningText();
        return;
    }

    // ── Sky (Deep atmospheric Martian sky) ──────────────────────
    glBegin(GL_QUADS);
        glColor3f(0.7f, 0.25f, 0.08f);  // horizon glow
        glVertex2f(-100, -100);
        glVertex2f( 100, -100);
        glColor3f(0.2f, 0.02f, 0.0f);  // top deep space red
        glVertex2f( 100,  100);
        glVertex2f(-100,  100);
    glEnd();

    //Stars (dim, mars has thin atmosphere)
    if (!starsInitialized)
    {
        for (int i = 0; i < 300; i++)
        {
            starX[i] = (float)(rand() % 200) - 100.0f;
            starY[i] = (float)(rand() % 200) - 100.0f;
        }
        starsInitialized = true;
    }
    for (int i = 0; i < 100; i++) // fewer stars visible
        drawCircle(starX[i], starY[i], 0.2f, 5, 0.8f, 0.7f, 0.7f);

    // Phobos & Deimos (Mars moons)
    drawCircle(75, 70, 6, 20, 0.6f, 0.5f, 0.45f);
    drawCircle(73, 72, 1, 10, 0.5f, 0.4f, 0.35f); // crater
    drawCircle(-60, 65, 3.5f, 20, 0.55f, 0.45f, 0.4f);

    // Distant mountain layer (Parallax depth 1)
    glColor3f(0.4f, 0.1f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-100, -20); glVertex2f(-60, 20); glVertex2f(-20, -20);
        glVertex2f( -30, -20); glVertex2f(  5,  25); glVertex2f( 40, -20);
        glVertex2f(  30, -20); glVertex2f( 70,  15); glVertex2f(100, -20);
    glEnd();

    // Mid mountain layer (Parallax depth 2)
    glColor3f(0.5f, 0.15f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-100, -50); glVertex2f(-70, -10); glVertex2f(-40, -50);
        glVertex2f( -20, -50); glVertex2f( 10,  -5); glVertex2f( 40, -50);
        glVertex2f(  50, -50); glVertex2f( 80,  -8); glVertex2f(100, -50);
    glEnd();

    // Mars surface ground
    drawRect(-100, -100, 200, 50, 0.65f, 0.22f, 0.02f);
    // Surface layer edge
    drawRect(-100, -52, 200, 5, 0.75f, 0.32f, 0.1f);
    
    // Rocks / craters with shading
    drawCircle(-70, -50, 8, 20, 0.55f, 0.18f, 0.0f);
    drawCircle(-68, -50, 5, 20, 0.45f, 0.12f, 0.0f);
    drawCircle( 60, -50, 10, 20, 0.55f, 0.18f, 0.0f);
    drawCircle( 62, -50, 6,  20, 0.45f, 0.12f, 0.0f);
    // Small rocks
    drawCircle(-40, -51, 3, 10, 0.6f, 0.22f, 0.05f);
    drawCircle( 30, -51, 4, 10, 0.6f, 0.22f, 0.05f);
    drawCircle( 10, -51, 2, 10, 0.6f, 0.22f, 0.05f);

    //  Rocket flying from space -> Mars
    if (marsPhase == 0)
    {
        float flicker = (sinf(marsTimer * 15.0f) + 1.0f) * 0.5f;
        drawRocketAt(0, marsRocketLandY, flicker);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(-35, 80, "Landing on Mars...", GLUT_BITMAP_HELVETICA_18);
    }

    // Rocket descending to land
    if (marsPhase == 1)
    {
        glColor3f(0.75f, 0.95f, 1.0f);
        drawText(-32, 80, "Exploring the surface...", GLUT_BITMAP_HELVETICA_18);
    }
    else if (marsPhase == 2)
    {
        glColor3f(1.0f, 0.72f, 0.20f);
        drawText(-45, 80, "A secret meeting...", GLUT_BITMAP_HELVETICA_18);
    }

    // Rocket landed on surface
    if (marsPhase >= 1 && marsPhase < 5)
    {
        // Rocket sitting on ground (no flame)
        drawRocketAt(0, -45.0f, 0.0f); // Rebalanced Y for 1.7f scale
    }

    // Girl walks out of rocket
    if (marsPhase >= 1 && marsPhase <= 5 && escapeRocketY < -18.0f)
    {
        float walkBounce = 0.0f;
        if (marsPhase == 1 || marsPhase == 5)
            walkBounce = sinf(marsTimer * 18.0f) * 1.4f;
        drawGirlStanding(girlWalkX, -56 + walkBounce);

        if (marsPhase == 1 && marsPhaseTimer > 1.4f)
        {
            float scan = (sinf(marsTimer * 5.0f) + 1.0f) * 0.5f;
            glColor3f(0.25f, 0.95f, 1.0f);
            glLineWidth(2.0f);
            glBegin(GL_LINES);
                glVertex2f(girlWalkX + 8, -22);
                glVertex2f(girlWalkX + 28 + scan * 12.0f, -12 + scan * 6.0f);
            glEnd();
        }
    }

    // Aliens appear
    if (marsPhase >= 1 && marsPhase <= 5)
    {
        glPushMatrix();
        glTranslatef((1.0f - alienAppear) * 85.0f, 0, 0);
        drawAlienGroup();
        glPopMatrix();
    }

    if (marsPhase == 1 || marsPhase == 2)
    {
        drawAlienThreatText();
    }

    if (marsPhase == 3)
    {
        float alertFlash = (sinf(marsTimer * 20.0f) + 1.0f) * 0.5f;
        drawCircle(girlWalkX + 4, -18, 4.0f + alertFlash * 3.0f, 18, 1.0f, 0.05f, 0.0f);
        if (marsPhaseTimer < 1.1f)
        {
            glColor3f(1.0f, 0.20f, 0.10f);
            drawText(6, 18, "I think the astronaut knows our secret...", GLUT_BITMAP_HELVETICA_12);
        }
    }

    if ((marsPhase == 4 && marsPhaseTimer > 1.0f) || marsPhase == 5)
    {
        float targetX = girlWalkX + 2.0f;
        float targetY = -20.0f;
        drawLaser(56, -12, targetX, targetY, marsTimer * 17.0f, 1.0f, 0.0f, 0.0f);
        drawLaser(24, -31, targetX + 4.0f, targetY - 3.0f, marsTimer * 19.0f + 1.0f, 0.0f, 1.0f, 0.25f);
        drawLaser(88, -31, targetX - 4.0f, targetY + 3.0f, marsTimer * 15.0f + 2.0f, 1.0f, 0.15f, 0.0f);
    }

    if (marsPhase == 2)
    {
        drawCircle(girlWalkX - 8, -55, 9, 16, 0.35f, 0.10f, 0.02f);
    }

    if (marsPhase == 5)
    {
        float flame = (sinf(marsTimer * 18.0f) + 1.0f) * 0.5f;
        drawRocketAt(0, escapeRocketY, flame);
        glColor3f(1.0f, 0.8f, 0.25f);
        drawText(-36, 80, "Escape!", GLUT_BITMAP_HELVETICA_18);
    }

    glColor3f(0.6f, 0.4f, 0.3f);
    drawText(-43, -95, "Mars mission in progress.", GLUT_BITMAP_HELVETICA_12);
}

//mouse callback
void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        float worldX = ((float)x / 1000.0f) * 200.0f - 100.0f;
        float worldY = 100.0f - ((float)y / 800.0f) * 200.0f;
        float dx = worldX - (-50.0f);
        float dy = worldY - (-5.0f);
        if (sqrt(dx*dx + dy*dy) <= 2.5f)
        {
            lampOn = !lampOn;
            glutPostRedisplay();
        }
    }
}


//  keyboard callback

void keyboard(unsigned char key, int x, int y)
{
    // Scene jump system
    if (key >= '1' && key <= '7')
    {
        // Global resets
        fanAngle = 0.0f; clockAngle = 0.0f; mouseX = -120.0f;
        lampOn = false; isAsleep = false; isDreaming = false;
        sleepTimer = 0.0f; zoomFactor = 1.0f;
        isSpaceScene = false; isTravelScene = false; isMarsScene = false;
        isWakeUpScene = false; isWallScene = false; isWindowSpaceScene = false;
        starsInitialized = false; aliensInitialized = false;
        
        if (key == '1') { /* Original Room - already reset */ }
        if (key == '2') { isSpaceScene = true; sleepTimer = 11.0f; zoomFactor = 21.0f; rocketY = -120.0f; }
        if (key == '3') { isTravelScene = true; travelTimer = 0.0f; marsApproachSize = 5.0f; }
        if (key == '4') { isMarsScene = true; marsPhase = 0; marsTimer = 0.0f; marsPhaseTimer = 0.0f; marsRocketLandY = 90.0f; }
        if (key == '5') { isWakeUpScene = true; wakeTimer = 0.0f; wakeHeadLift = 0.0f; wakeHeadTurn = 0.0f; }
        if (key == '6') { isWallScene = true; wallSceneTimer = 0.0f; }
        if (key == '7') { isWindowSpaceScene = true; windowSpaceTimer = 0.0f; }
        
        glutPostRedisplay();
        return;
    }

    if (key == 'r' || key == 'R') // Restart everything
    {
        fanAngle = 0.0f; clockAngle = 0.0f; mouseX = -120.0f;
        lampOn = false; isAsleep = false; isDreaming = false;
        sleepTimer = 0.0f; zoomFactor = 1.0f;
        isSpaceScene = false; starsInitialized = false;
        astroYOffset = 0.0f; astroRotation = 0.0f;
        mercuryAngle = 0.0f; venusAngle = 0.0f; earthAngle = 0.0f;
        rocketY = -120.0f;
        isTravelScene = false; marsApproachSize = 5.0f; travelTimer = 0.0f;
        isMarsScene = false; marsTransition = false;
        marsPhase = 0; marsTimer = 0.0f; marsPhaseTimer = 0.0f;
        alienAppear = 0.0f; girlWalkX = 8.0f; escapeRocketY = -45.0f; warningAlpha = 0.0f;
        isWakeUpScene = false;
        isWallScene = false;
        isWindowSpaceScene = false;
        wakeTimer = 0.0f; wakeHeadLift = 0.0f;
        wakeStandProgress = 0.0f; wakeHeadTurn = 0.0f;
        aliensInitialized = false;
        glutPostRedisplay();
    }
}


//  display()

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //  Travel cinematic — rocket flies toward Mars
    if (isWindowSpaceScene)
    {
        drawWindowSpaceScene();
    }
    else if (isWallScene)
    {
        drawWallScene();
    }
    else if (isWakeUpScene)
    {
        wakeUpFunction();
    }
    else if (isTravelScene)
    {
        drawTravelScene();
    }

    //  Mars surface scene
    else if (isMarsScene)
    {
        drawMarsScene();
    }

    // space scene
    else if (isSpaceScene)
    {
        glBegin(GL_QUADS);
            glColor3f(0.02f, 0.0f, 0.08f);
            glVertex2f(-100, -100); glVertex2f(100, -100);
            glColor3f(0.0f, 0.0f, 0.15f);
            glVertex2f(100, 100);   glVertex2f(-100, 100);
        glEnd();

        if (!starsInitialized)
        {
            for (int i = 0; i < 300; i++)
            {
                starX[i] = (float)(rand() % 200) - 100.0f;
                starY[i] = (float)(rand() % 200) - 100.0f;
            }
            starsInitialized = true;
        }
        
        // Background Stars & subtle space debris
        for (int i = 0; i < 300; i++) {
            float twinkle = (sinf(sleepTimer * 3.0f + i) + 1.0f) * 0.5f;
            drawCircle(starX[i], starY[i], 0.3f + twinkle*0.2f, 5, 0.8f + twinkle*0.2f, 0.8f + twinkle*0.2f, 0.9f + twinkle*0.1f);
        }
        
        // Fast moving meteors (parallax)
        for (int i = 0; i < 5; i++) {
            float mX = fmodf((starX[i*10] + sleepTimer * (40.0f + i*10.0f)), 250.0f) - 125.0f;
            float mY = starY[i*10];
            glColor3f(0.8f, 0.9f, 1.0f);
            glBegin(GL_LINES);
                glVertex2f(mX, mY);
                glVertex2f(mX - 15.0f - i*5.0f, mY - 2.0f);
            glEnd();
            drawCircle(mX, mY, 0.4f, 6, 1.0f, 1.0f, 1.0f);
        }

        // Solar System Sun Glow
        drawCircle(-70, 0, 26, 40, 1.0f, 0.3f, 0.0f); // outer glow
        drawCircle(-70, 0, 22, 50, 1.0f, 0.6f, 0.0f); // mid glow
        drawCircle(-70, 0, 18, 50, 1.0f, 0.8f, 0.2f); // core

        // Mercury
        glPushMatrix();
        glTranslatef(-70, 0, 0); glRotatef(mercuryAngle, 0, 0, 1);
        drawCircle(30, 0, 3, 20, 0.6f, 0.4f, 0.3f);
        drawCircle(30, 0, 1.5f, 20, 0.8f, 0.6f, 0.5f); // highlight
        glPopMatrix();

        // Venus
        glPushMatrix();
        glTranslatef(-70, 0, 0); glRotatef(venusAngle, 0, 0, 1);
        drawCircle(45, 0, 5, 20, 0.9f, 0.5f, 0.1f);
        drawCircle(45, 0, 3.5f, 20, 0.95f, 0.7f, 0.3f);
        glPopMatrix();

        // Earth & Moon
        glPushMatrix();
        glTranslatef(-70, 0, 0); glRotatef(earthAngle, 0, 0, 1);
        drawCircle(65, 0, 7, 25, 0.1f, 0.3f, 0.7f); // Ocean
        drawCircle(65, 0, 5, 20, 0.2f, 0.5f, 0.8f); // inner
        // Continents approx
        drawCircle(63, 2, 2.5f, 12, 0.2f, 0.7f, 0.2f);
        drawCircle(66, -1, 3.0f, 12, 0.2f, 0.6f, 0.1f);
        // Atmosphere
        glColor3f(0.5f, 0.7f, 1.0f);
        glLineWidth(1.0f);
        glBegin(GL_LINE_LOOP);
        for(int i=0; i<30; i++) {
            float a = i * 2.0f * 3.14159f / 30;
            glVertex2f(65 + 7.5f*cosf(a), 0 + 7.5f*sinf(a));
        }
        glEnd();

        glPushMatrix();
        glTranslatef(65, 0, 0); glRotatef(earthAngle * 2, 0, 0, 1);
        drawCircle(12, 0, 1.8f, 15, 0.7f, 0.7f, 0.7f);
        drawCircle(12, 0, 1.0f, 10, 0.9f, 0.9f, 0.9f);
        glPopMatrix();
        glPopMatrix();

        // Mars & Moons
        glPushMatrix();
        glTranslatef(-70, 0, 0); glRotatef(earthAngle * 0.5f, 0, 0, 1);
        drawCircle(88, 0, 5.5f, 25, 0.7f, 0.2f, 0.05f); // Dark Mars
        drawCircle(88, 0, 4.0f, 20, 0.85f, 0.35f, 0.1f); // Bright Mars
        // Phobos
        drawCircle(88 + 9*cosf(sleepTimer*1.5f), 9*sinf(sleepTimer*1.5f), 1.0f, 10, 0.6f, 0.5f, 0.5f);
        // Deimos
        drawCircle(88 + 14*cosf(sleepTimer*0.8f), 14*sinf(sleepTimer*0.8f), 0.8f, 8, 0.7f, 0.6f, 0.6f);
        glPopMatrix();

        // Big Cinematic Rocket
        glPushMatrix();
        glTranslatef(30, rocketY, 0);
        glScalef(2.5f, 2.5f, 1.0f); // Make rocket 2.5x larger, overall ~ 4x taller relative to astronaut

        // Engine and Flame
        float spaceFlame = (sinf(sleepTimer * 20.0f) + 1.0f) * 0.5f;
        glColor3f(1.0f, 0.3f + spaceFlame * 0.3f, 0.0f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-6, 0); glVertex2f(6, 0); glVertex2f(0, -18.0f - spaceFlame * 10.0f);
        glEnd();
        glColor3f(1.0f, 0.8f, 0.2f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-3, 0); glVertex2f(3, 0); glVertex2f(0, -10.0f - spaceFlame * 6.0f);
        glEnd();
        
        // Rocket Body
        drawRect(-8, 0, 16, 45, 0.85f, 0.85f, 0.88f);
        drawRect(-4, 0, 8, 45, 0.95f, 0.95f, 0.95f); // Highlight

        // Nose Cone
        glColor3f(0.8f, 0.1f, 0.1f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-8, 45); glVertex2f(8, 45); glVertex2f(0, 60);
        glEnd();
        glColor3f(1.0f, 0.2f, 0.2f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-4, 45); glVertex2f(4, 45); glVertex2f(0, 58);
        glEnd();

        // Fins
        glColor3f(0.7f, 0.1f, 0.1f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-8, 5); glVertex2f(-16, -5); glVertex2f(-8, 15);
            glVertex2f( 8, 5); glVertex2f( 16, -5); glVertex2f( 8, 15);
        glEnd();
        
        // Landing Gear/Struts
        glColor3f(0.4f, 0.4f, 0.45f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            glVertex2f(-8, 8); glVertex2f(-12, -2);
            glVertex2f( 8, 8); glVertex2f( 12, -2);
        glEnd();

        // Windows
        drawCircle(0, 32, 4.5f, 20, 0.4f, 0.4f, 0.45f); // Rim
        drawCircle(0, 32, 3.5f, 20, 0.1f, 0.2f, 0.5f);  // Glass
        drawCircle(-1, 33, 1.0f, 10, 0.6f, 0.8f, 1.0f); // Reflection
        
        drawCircle(0, 18, 4.5f, 20, 0.4f, 0.4f, 0.45f); // Rim
        drawCircle(0, 18, 3.5f, 20, 0.1f, 0.2f, 0.5f);  // Glass
        drawCircle(-1, 19, 1.0f, 10, 0.6f, 0.8f, 1.0f); // Reflection
        
        // Body details
        drawRect(-8, 25, 16, 2, 0.7f, 0.1f, 0.1f);
        drawRect(-8, 11, 16, 2, 0.7f, 0.1f, 0.1f);

        glPopMatrix();

        // Astronaut (Polished & Shaded, smaller)
        glPushMatrix();
        glTranslatef(-30, astroYOffset, 0); // Moved to left
        glRotatef(astroRotation, 0, 0, 1);
        glScalef(0.6f, 0.6f, 1.0f); // Reduce size to 0.6

        // Oxygen Backpack
        drawRect(-14, -25, 28, 32, 0.6f, 0.6f, 0.65f);
        drawRect(-12, -22, 24, 26, 0.75f, 0.75f, 0.8f);
        drawRect(-6, -18, 12, 10, 0.4f, 0.4f, 0.45f); // vent

        // Main Suit Torso
        drawRect(-10, -30, 20, 38, 0.85f, 0.85f, 0.88f);
        drawRect(-7, -28, 14, 34, 0.95f, 0.95f, 0.95f); // Highlight

        // Chest Control Panel
        drawRect(-6, -8, 12, 12, 0.7f, 0.7f, 0.75f);
        drawCircle(-3, -2, 1.5f, 10, 1.0f, 0.2f, 0.2f);
        drawCircle( 3, -2, 1.5f, 10, 0.2f, 0.8f, 0.2f);
        drawRect(-4, -6, 8, 3, 0.1f, 0.2f, 0.5f);

        // Legs
        glPushMatrix(); glTranslatef(-10, 0, 0);
        glRotatef(20 + sinf(sleepTimer * 2) * 15, 0, 0, 1);
        drawRect(-10, -5, 15, 12, 0.85f, 0.85f, 0.88f); // thigh
        drawRect(-8, -3, 11, 8, 0.95f, 0.95f, 0.95f);
        drawRect(-12, -7, 10, 8, 0.7f, 0.7f, 0.75f); // boot
        glPopMatrix();
        
        glPushMatrix(); glTranslatef(10, 0, 0);
        glRotatef(-20 - sinf(sleepTimer * 2) * 15, 0, 0, 1);
        drawRect(0, -5, 15, 12, 0.85f, 0.85f, 0.88f); // thigh
        drawRect(2, -3, 11, 8, 0.95f, 0.95f, 0.95f);
        drawRect(2, -7, 10, 8, 0.7f, 0.7f, 0.75f); // boot
        glPopMatrix();

        // Arms
        glPushMatrix(); glTranslatef(-5, -30, 0);
        glRotatef(-10 + sinf(sleepTimer * 1.5f) * 10, 0, 0, 1);
        drawRect(-6, -18, 10, 18, 0.85f, 0.85f, 0.88f); // arm
        drawRect(-4, -16, 6, 14, 0.95f, 0.95f, 0.95f);
        drawCircle(-1, -18, 4.0f, 15, 0.4f, 0.4f, 0.45f); // glove
        glPopMatrix();
        
        glPushMatrix(); glTranslatef(5, -30, 0);
        glRotatef(10 - sinf(sleepTimer * 1.5f) * 10, 0, 0, 1);
        drawRect(-4, -18, 10, 18, 0.85f, 0.85f, 0.88f); // arm
        drawRect(-2, -16, 6, 14, 0.95f, 0.95f, 0.95f);
        drawCircle(1, -18, 4.0f, 15, 0.4f, 0.4f, 0.45f); // glove
        glPopMatrix();

        // Helmet
        drawCircle(0, 18, 14, 30, 1.0f, 1.0f, 1.0f); // Helmet shell
        drawCircle(0, 18, 10, 30, 0.05f, 0.1f, 0.2f); // Visor base
        
        // Visor gold reflection
        glColor3f(0.8f, 0.6f, 0.1f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex2f(0, 18);
            for (int i = 0; i <= 15; i++) {
                float t = i * 3.14159f / 15;
                glVertex2f(0 + 10.0f * cosf(t), 18 + 10.0f * sinf(t));
            }
        glEnd();
        // Visor glare
        drawCircle(-4, 22, 2.5f, 15, 1.0f, 1.0f, 1.0f);
        drawCircle(-2, 20, 1.0f, 10, 0.9f, 0.9f, 0.9f);
        
        // Helmet light
        drawCircle(-10, 26, 2.0f, 15, 1.0f, 1.0f, 0.5f);

        glPopMatrix();
    }
    else
    {
        // Original Room Scene
        if (zoomFactor > 1.0f)
        {
            glTranslatef(12, 65, 0);
            glScalef(zoomFactor, zoomFactor, 1.0f);
            glTranslatef(-12, -65, 0);
        }

        // Smooth cinematic dimming synchronized with sleepiness
        float dim = smoothStep(sleepTimer / 7.0f);
        if (!lampOn) dim = 1.0f;
        float bgR = 0.53f * (1.0f - dim) + 0.08f * dim;
        float bgG = 0.81f * (1.0f - dim) + 0.12f * dim;
        float bgB = 0.98f * (1.0f - dim) + 0.25f * dim;
        
        // Enhanced wall shading with vertical gradient
        glBegin(GL_QUADS);
            glColor3f(bgR * 1.1f, bgG * 1.1f, bgB * 1.1f);
            glVertex2f(-100, 100); glVertex2f(100, 100);
            glColor3f(bgR, bgG, bgB);
            glVertex2f(100, 40);   glVertex2f(-100, 40);
            
            glColor3f(bgR, bgG, bgB);
            glVertex2f(-100, 40);  glVertex2f(100, 40);
            glColor3f(bgR * 0.5f, bgG * 0.5f, bgB * 0.5f);
            glVertex2f(100, -20);  glVertex2f(-100, -20);
        glEnd();

        drawRect(-75, 40, 25, 30, 0.3f, 0.2f, 0.1f);
        drawRect(-72, 43, 19, 24, 0.9f, 0.9f, 1.0f);
        drawCircle(-60, 58, 4, 20, 1.0f, 0.9f, 0.0f);
        drawRect(-72, 43, 19, 8, 0.2f, 0.6f, 0.2f);
        drawRect(50, 40, 25, 30, 0.3f, 0.2f, 0.1f);
        drawRect(53, 43, 19, 24, 1.0f, 0.9f, 0.8f);
        drawCircle(63, 50, 6, 20, 1.0f, 0.4f, 0.0f);
        drawRect(53, 43, 19, 5, 0.4f, 0.2f, 0.1f);

        // Floor with depth
        glBegin(GL_QUADS);
            glColor3f(0.85f * (1.0f - dim*0.6f), 0.45f * (1.0f - dim*0.6f), 0.1f * (1.0f - dim*0.6f));
            glVertex2f(-100, -20); glVertex2f(100, -20);
            glColor3f(0.5f * (1.0f - dim*0.6f), 0.25f * (1.0f - dim*0.6f), 0.05f * (1.0f - dim*0.6f));
            glVertex2f(100, -100); glVertex2f(-100, -100);
        glEnd();

        drawRect(-1, 80, 2, 20, 0.2f, 0.2f, 0.2f);
        glPushMatrix();
        glTranslatef(0, 80, 0);
        glRotatef(fanAngle, 0, 0, 1);
        drawCircle(0, 0, 4, 20, 0.3f, 0.3f, 0.3f);
        drawRect(-25, -2, 20, 4, 0.4f, 0.2f, 0.1f);
        drawRect(5,   -2, 20, 4, 0.4f, 0.2f, 0.1f);
        glPopMatrix();
        // GIRL BODY (Behind Desk)
        glPushMatrix();
        if (isAsleep) glTranslatef(0, -8, 0); // slump down

        // Chair (behind girl)
        drawRect(-28, -45, 26, 25, 0.35f, 0.18f, 0.08f); // chair back
        drawRect(-26, -60, 3, 15, 0.2f, 0.1f, 0.02f);    // left leg
        drawRect( -7, -60, 3, 15, 0.2f, 0.1f, 0.02f);    // right leg
        
        // Legs (shorter, start below lap)
        drawRect(-24, -55, 7, 25, 0.95f, 0.8f, 0.65f);
        drawRect(-13, -55, 7, 25, 0.95f, 0.8f, 0.65f);
        // Feet
        drawRect(-26, -60, 9, 5, 0.4f, 0.1f, 0.1f);
        drawRect(-14, -60, 9, 5, 0.4f, 0.1f, 0.1f);

        // Torso
        glColor3f(0.85f, 0.35f, 0.35f);
        glBegin(GL_POLYGON);
            glVertex2f(-26, -28); glVertex2f(-22, 5);
            glVertex2f( -8,   5); glVertex2f( -4,-28);
        glEnd();
        
        // Hair Back
        drawCircle(-15, 20, 12, 30, 0.1f, 0.1f, 0.1f);
        drawCircle(-22, 12, 10, 30, 0.1f, 0.1f, 0.1f);
        drawCircle(-8,  12, 10, 30, 0.1f, 0.1f, 0.1f);
        drawRect(-28, -2, 26, 16, 0.1f, 0.1f, 0.1f);

        // Neck
        drawCircle(-15, 5, 3.5f, 20, 1.0f, 0.8f, 0.6f);
        
        // Head
        drawCircle(-15, 16, 10.0f, 30, 1.0f, 0.8f, 0.6f);

        // Facial details
        if (isAsleep) {
            glColor3f(0.0f, 0.0f, 0.0f);
            glLineWidth(2.0f);
            glBegin(GL_LINE_STRIP);
                glVertex2f(-19.5f,15.5f); glVertex2f(-18.0f,14.5f); glVertex2f(-16.5f,15.5f);
            glEnd();
            glBegin(GL_LINE_STRIP);
                glVertex2f(-13.5f,15.5f); glVertex2f(-12.0f,14.5f); glVertex2f(-10.5f,15.5f);
            glEnd();
            drawCircle(-15, 11, 1.5f, 10, 0.0f, 0.0f, 0.0f);
            glBegin(GL_LINES);
                glVertex2f(-20,18); glVertex2f(-16,19);
                glVertex2f(-14,19); glVertex2f(-10,18);
            glEnd();
        } else {
            float sleepiness = smoothStep(sleepTimer / 7.0f);
            drawCircle(-18,17, 2.0f,15,1.0f,1.0f,1.0f);
            drawCircle(-12,17, 2.0f,15,1.0f,1.0f,1.0f);
            float eyeDrop = sleepiness * 1.5f;
            drawCircle(-18,16.5f - eyeDrop*0.5f, 1.0f,10,0.1f,0.1f,0.2f);
            drawCircle(-12,16.5f - eyeDrop*0.5f, 1.0f,10,0.1f,0.1f,0.2f);
            
            if (sleepiness > 0.05f) {
                glColor3f(0.9f, 0.7f, 0.5f); 
                glBegin(GL_QUADS);
                    glVertex2f(-20.5f, 19); glVertex2f(-15.5f, 19);
                    glVertex2f(-15.5f, 19 - sleepiness * 2.8f); glVertex2f(-20.5f, 19 - sleepiness * 2.8f);
                    glVertex2f(-14.5f, 19); glVertex2f(-9.5f, 19);
                    glVertex2f(-9.5f, 19 - sleepiness * 2.8f); glVertex2f(-14.5f, 19 - sleepiness * 2.8f);
                glEnd();
                glColor3f(0.2f, 0.1f, 0.1f);
                glLineWidth(1.5f);
                glBegin(GL_LINES);
                    glVertex2f(-20.5f, 19 - sleepiness * 2.8f); glVertex2f(-15.5f, 19 - sleepiness * 2.8f);
                    glVertex2f(-14.5f, 19 - sleepiness * 2.8f); glVertex2f(-9.5f, 19 - sleepiness * 2.8f);
                glEnd();
            }

            glColor3f(0.2f,0.1f,0.1f);
            glLineWidth(2.5f);
            glBegin(GL_LINES);
                glVertex2f(-20, 20.5f - sleepiness); glVertex2f(-16, 21.5f - sleepiness*1.5f);
                glVertex2f(-14, 21.5f - sleepiness*1.5f); glVertex2f(-10, 20.5f - sleepiness);
            glEnd();

            glColor3f(0.8f,0.2f,0.2f);
            glLineWidth(2.0f);
            if (sleepiness > 0.6f) {
                drawCircle(-15, 11.5f, 0.8f + sleepiness*0.6f, 10, 0.1f, 0.0f, 0.0f);
            } else {
                glBegin(GL_LINE_STRIP);
                    glVertex2f(-17,11.5f); glVertex2f(-15,11.0f); glVertex2f(-13,11.5f);
                glEnd();
            }
        }
        glPopMatrix();

        // DESK (In Front of Girl Body)
        drawRect(-40, -60, 4, 30, 0.3f, 0.15f, 0.05f); // Left leg
        drawRect( 25, -60, 4, 30, 0.3f, 0.15f, 0.05f); // Right leg
        drawRect(-45, -30, 75, 10, 0.45f, 0.25f, 0.15f); // Desk top (X: -45 to 30)
        drawRect(-45, -35, 75,  5, 0.3f, 0.15f, 0.05f);  // Desk rim

        // GIRL ARMS (Over Desk)
        glPushMatrix();
        if (isAsleep) glTranslatef(0, -8, 0); // slump down
        if (isAsleep) {
            glPushMatrix(); glTranslatef(-22, -10, 0); glRotatef(15, 0, 0, 1);
            drawRect(-3, -15, 6, 20, 0.75f, 0.25f, 0.25f);
            drawCircle(0, -15, 3, 10, 1.0f, 0.8f, 0.6f); glPopMatrix();
            
            glPushMatrix(); glTranslatef(-8, -10, 0); glRotatef(-15, 0, 0, 1);
            drawRect(-3, -15, 6, 20, 0.75f, 0.25f, 0.25f);
            drawCircle(0, -15, 3, 10, 1.0f, 0.8f, 0.6f); glPopMatrix();
        } else {
            glPushMatrix(); glTranslatef(-25, 0, 0); glRotatef(30, 0, 0, 1);
            drawRect(-3, -20, 6, 22, 0.75f, 0.25f, 0.25f);
            drawCircle(0, -20, 3, 10, 1.0f, 0.8f, 0.6f); glPopMatrix();
            
            glPushMatrix(); glTranslatef(-5, 0, 0); glRotatef(-30, 0, 0, 1);
            drawRect(-3, -20, 6, 22, 0.75f, 0.25f, 0.25f);
            drawCircle(0, -20, 3, 10, 1.0f, 0.8f, 0.6f); glPopMatrix();
        }
        glPopMatrix();

        // OBJECTS ON DESK
        // Animated Table Lamp
        float lampPulse = lampOn ? (sinf(sleepTimer * 8.0f) * 0.1f + 0.9f) : 0.0f;
        glPushMatrix();
        glTranslatef(20, -13, 0); // Move lamp onto the right side of the desk

        // Lamp Base
        drawRect(-60, -7, 20, 4, 0.15f, 0.15f, 0.15f);
        drawRect(-51, -3, 2, 18, 0.25f, 0.25f, 0.25f);
        
        // Lamp Shade
        glColor3f(0.8f, 0.1f, 0.1f);
        glBegin(GL_QUADS);
            glVertex2f(-65, 15); glVertex2f(-35, 15);
            glVertex2f(-42, 28); glVertex2f(-58, 28);
        glEnd();
        
        // Lamp Bulb and Glow
        if (lampOn) {
            drawCircle(-50, 18, 3, 20, 1.0f, 1.0f, 0.8f * lampPulse);
            drawCircle(-50, -5, 1.5f, 20, 1.0f, 1.0f, 0.0f); // base button ON
            
            // Soft Light Cone
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBegin(GL_TRIANGLES);
                glColor4f(1.0f, 0.9f, 0.4f, 0.6f * lampPulse); glVertex2f(-50, 15);
                glColor4f(1.0f, 0.8f, 0.2f, 0.0f); glVertex2f(-85, -15); 
                glColor4f(1.0f, 0.8f, 0.2f, 0.0f); glVertex2f(-15, -15);
            glEnd();
            glDisable(GL_BLEND);
        } else {
            drawCircle(-50, -5, 1.5f, 20, 0.4f, 0.4f, 0.4f); // base button OFF
        }
        glPopMatrix();

        // Laptop
        glPushMatrix();
        glTranslatef(-2, -20, 0);
        drawRect(-6, 0, 12, 8, 0.8f, 0.8f, 0.85f);
        drawRect(-5, 1, 10, 6, 0.1f, 0.1f, 0.15f);
        drawRect(-8, -2, 16, 2, 0.7f, 0.7f, 0.75f);
        if (lampOn) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBegin(GL_QUADS);
                glColor4f(0.5f, 0.8f, 1.0f, 0.2f); glVertex2f(-5, 1);
                glColor4f(0.5f, 0.8f, 1.0f, 0.2f); glVertex2f(5, 1);
                glColor4f(0.5f, 0.8f, 1.0f, 0.0f); glVertex2f(10, -10);
                glColor4f(0.5f, 0.8f, 1.0f, 0.0f); glVertex2f(-10, -10);
            glEnd();
            glDisable(GL_BLEND);
        }
        glPopMatrix();

        // Books on Table
        glPushMatrix();
        glTranslatef(10, -20, 0);
        drawRect(0, 0, 10, 3, 0.2f, 0.4f, 0.8f);
        drawRect(0, 3, 9, 3, 0.8f, 0.3f, 0.2f);
        drawRect(-1, 6, 10, 3, 0.1f, 0.6f, 0.3f);
        glPushMatrix();
        glTranslatef(-1, 9, 0);
        glRotatef(12, 0, 0, 1);
        drawRect(0, 0, 10, 2.5f, 0.8f, 0.7f, 0.1f);
        glPopMatrix();
        glPopMatrix();

        // Moderate two-layer cabinet beside the window.
        drawRect(44, -20, 36, 56, 0.45f, 0.23f, 0.08f);
        drawRect(47, 8, 30, 25, 0.58f, 0.32f, 0.12f);
        drawRect(47, -17, 30, 25, 0.58f, 0.32f, 0.12f);
        drawRect(60, 9, 2, 23, 0.35f, 0.17f, 0.06f);
        drawRect(60, -16, 2, 23, 0.35f, 0.17f, 0.06f);
        drawCircle(57, 20, 1.5f, 12, 0.95f, 0.78f, 0.25f);
        drawCircle(67, 20, 1.5f, 12, 0.95f, 0.78f, 0.25f);
        drawCircle(57, -5, 1.5f, 12, 0.95f, 0.78f, 0.25f);
        drawCircle(67, -5, 1.5f, 12, 0.95f, 0.78f, 0.25f);

        // Plant on top of cabinet
        glColor3f(0.6f, 0.3f, 0.2f);
        glBegin(GL_QUADS);
            glVertex2f(50, 52); glVertex2f(70, 52);
            glVertex2f(66, 36); glVertex2f(54, 36);
        glEnd();
        drawRect(59, 52, 2, 15, 0.13f, 0.35f, 0.13f);
        drawCircle(60, 67, 10, 20, 0.13f, 0.55f, 0.13f);
        drawCircle(52, 62,  7, 20, 0.13f, 0.45f, 0.13f);
        drawCircle(68, 62,  7, 20, 0.13f, 0.45f, 0.13f);

        // Clock on Table
        glPushMatrix();
        glTranslatef(-3, -12, 0);
        drawCircle(25, 0, 8, 30, 0.8f, 0.0f, 0.0f);
        drawCircle(25, 0, 7, 30, 1.0f, 1.0f, 1.0f);
        
        glPushMatrix();
        glTranslatef(25, 0, 0);
        glColor3f(0.8f, 0.0f, 0.0f);
        glLineWidth(2.0f);
        glPushMatrix();
        glRotatef(-clockAngle * 12, 0, 0, 1);
        glBegin(GL_LINES); glVertex2f(0,0); glVertex2f(0,6); glEnd();
        glPopMatrix();
        glPushMatrix();
        glRotatef(-clockAngle, 0, 0, 1);
        glBegin(GL_LINES); glVertex2f(0,0); glVertex2f(4,0); glEnd();
        glPopMatrix();
        glPopMatrix();
        
        // Clock legs
        glBegin(GL_TRIANGLES);
            glVertex2f(25,0); glVertex2f(20,-8); glVertex2f(30,-8);
        glEnd();
        glPopMatrix();

        drawCircle(mouseX,    -88, 3,   20, 0.6f, 0.3f, 0.1f);
        drawCircle(mouseX+3,  -88, 1.8f,20, 0.6f, 0.3f, 0.1f);
        drawCircle(mouseX+2,  -86, 1,   10, 0.7f, 0.5f, 0.3f);
        glColor3f(0.4f, 0.4f, 0.4f);
        glBegin(GL_LINES);
            glVertex2f(mouseX-3,-88); glVertex2f(mouseX-8,-86);
        glEnd();

        // Animated Dream Cloud
        if (isDreaming) {
            float floatY = sinf(sleepTimer * 3.0f) * 2.0f;
            float pulse = (sinf(sleepTimer * 4.0f) + 1.0f) * 0.2f;
            
            glPushMatrix();
            glTranslatef(0, floatY, 0);
            
            // Trail bubbles
            if (sleepTimer > 8.0f)  drawCircle(-5, 25, 1.5f, 20, 0.9f+pulse, 0.9f+pulse, 1.0f);
            if (sleepTimer > 8.5f)  drawCircle( 0, 30, 2.5f, 20, 0.9f+pulse, 0.9f+pulse, 1.0f);
            if (sleepTimer > 9.0f)  drawCircle( 5, 38, 3.5f, 20, 0.9f+pulse, 0.9f+pulse, 1.0f);
            
            // Main Dream Cloud
            if (sleepTimer > 9.5f) {
                drawCircle(12, 65, 12, 30, 0.9f+pulse, 0.9f+pulse, 1.0f);
                drawCircle(22, 68, 10, 30, 0.9f+pulse, 0.9f+pulse, 1.0f);
                drawCircle( 2, 68, 10, 30, 0.9f+pulse, 0.9f+pulse, 1.0f);
                drawCircle(12, 74,  9, 30, 0.9f+pulse, 0.9f+pulse, 1.0f);
                
                // Animated Dream Text
                glColor3f(0.2f, 0.4f, 0.8f);
                drawText(-2, 65, "Dreaming...", GLUT_BITMAP_HELVETICA_18);
            }
            glPopMatrix();
        }
    }

    glutSwapBuffers();
}


void timerFunc(int)
{
    glutPostRedisplay();
    glutTimerFunc(1000/60, timerFunc, 0);

    // Continuous updates for room objects
    fanAngle += 15.0f;
    if (fanAngle > 360) fanAngle -= 360;
    clockAngle += 0.05f;
    if (clockAngle > 360) clockAngle -= 360;
    mouseX += 1.5f;
    if (mouseX > 120) mouseX = -120;

    if (isWakeUpScene)
    {
        //wake up, show dialogue, then turn the head.
        const float dt = FrameTime;
        wakeTimer += dt;

        if (wakeTimer > WakeHeadLiftStart)
            wakeHeadLift += 6.0f * dt;

        if (wakeTimer > WakeHeadTurnStart)
            wakeHeadTurn += 40.0f * dt;

        wakeHeadLift = clampFloat(wakeHeadLift, 0.0f, WakeHeadLiftMax);
        wakeHeadTurn = clampFloat(wakeHeadTurn, 0.0f, WakeHeadTurnMax);
        if (fanAngle   > 360) fanAngle   -= 360;
        if (clockAngle > 360) clockAngle -= 360;
        if (mouseX > 120)     mouseX = -120;

        if (wakeHeadTurn >= WakeHeadTurnMax)
        {
            isWakeUpScene = false;
            isWallScene = true;
            wallSceneTimer = 0.0f;
        }

        return;
    }

    if (isWallScene)
    {
        // \wait, zoom into the window, then enter the space house scene.
        wallSceneTimer += FrameTime;
        if (wallSceneTimer >= WallTotalTime)
        {
            isWallScene = false;
            isWindowSpaceScene = true;
            windowSpaceTimer = 0.0f;
        }
        return;
    }

    if (isWindowSpaceScene)
    {
        // keep the final space house scene alive while its text timer runs.
        windowSpaceTimer += FrameTime;
        return;
    }


    if (isTravelScene)
    {
        travelTimer += FrameTime;

        marsApproachSize += 0.38f;


        rocketTravelX = sinf(travelTimer * 1.5f) * 4.0f;


        if (marsApproachSize >= 82.0f)
        {
            isTravelScene   = false;
            isMarsScene     = true;
            marsTransition  = true;
            marsPhase       = 0;
            marsTimer       = 0.0f;
            marsPhaseTimer  = 0.0f;
            travelRocketX   = 0.0f;
            travelRocketY   = rocketY;
            marsRocketLandY = 90.0f;
            girlWalkX       = 8.0f;
            alienAppear     = 0.0f;
            escapeRocketY   = -45.0f;
            warningAlpha    = 0.0f;
        }
        return;
    }


    if (isMarsScene)
    {
        const float dt = 1.0f / 60.0f;
        marsTimer += dt;
        marsPhaseTimer += dt;
        alienBobY  = sinf(marsTimer * 3.0f) * 3.0f;

        if (marsPhase == 0)
        {
            marsRocketLandY -= 55.0f * dt;
            if (marsRocketLandY <= -45.0f)
            {
                marsRocketLandY = -45.0f;
                marsPhase = 1;
                marsPhaseTimer = 0.0f;
                girlWalkX = 8.0f;
                alienAppear = 1.0f;
            }
        }


        if (marsPhase == 1)
        {
            if (girlWalkX > -38.0f)
                girlWalkX -= 24.0f * dt;
            else if (marsPhaseTimer > 3.2f)
            {
                marsPhase = 2;
                marsPhaseTimer = 0.0f;
                alienAppear = 1.0f;
            }
        }


        if (marsPhase == 2)
        {
            if (alienAppear < 1.0f)
                alienAppear += 0.65f * dt;
            if (marsPhaseTimer > 4.0f)
            {
                marsPhase = 3;
                marsPhaseTimer = 0.0f;
            }
        }


        if (marsPhase == 3 && marsPhaseTimer > 1.4f)
        {
            marsPhase = 4;
            marsPhaseTimer = 0.0f;
        }


        if (marsPhase == 4 && marsPhaseTimer > 3.0f)
        {
            marsPhase = 5;
            marsPhaseTimer = 0.0f;
            escapeRocketY = -45.0f;
        }


        if (marsPhase == 5)
        {
            if (girlWalkX < 4.0f)
                girlWalkX += 55.0f * dt;
            if (girlWalkX >= 4.0f)
                escapeRocketY += 82.0f * dt;
            if (escapeRocketY > 110.0f)
            {
                marsPhase = 6;
                marsPhaseTimer = 0.0f;
                warningAlpha = 0.0f;
            }
        }


        if (marsPhase == 6)
        {
            if (warningAlpha < 1.0f)
                warningAlpha += 0.65f * dt;

            if (marsPhaseTimer >= 3.0f)
            {
                isMarsScene = false;
                isWakeUpScene = true;
                wakeTimer = 0.0f;
                wakeHeadLift = 0.0f;
                wakeStandProgress = 0.0f;
                wakeHeadTurn = 0.0f;
            }
        }

        return;
    }


    fanAngle   += 15.0f;
    clockAngle += 0.05f;
    mouseX     += 1.5f;
    sleepTimer += 1.0f / 60.0f;

    if (sleepTimer >= 7.0f) isAsleep   = true;
    if (sleepTimer >= 8.0f) isDreaming = true;

    if (sleepTimer >= 11.0f && !isSpaceScene)
    {
        zoomFactor += 0.2f;
        if (zoomFactor > 20.0f) isSpaceScene = true;
    }

    if (fanAngle   > 360) fanAngle   -= 360;
    if (clockAngle > 360) clockAngle -= 360;
    if (mouseX > 120)     mouseX = -120;

    if (isSpaceScene)
    {
        astroYOffset  = sinf(sleepTimer * 1.5f) * 10.0f;
        astroRotation = sinf(sleepTimer * 0.5f) * 5.0f;
        mercuryAngle += 2.0f;
        venusAngle   += 1.2f;
        earthAngle   += 0.7f;
        rocketY      += 0.9f; // Slightly faster rocket
        
        if (rocketY > 150.0f && !isTravelScene && !isMarsScene)
        {
            isTravelScene      = true;
            travelTimer        = 0.0f;
            marsApproachSize   = 5.0f;   // Mars starts tiny
            rocketTravelX      = 0.0f;   // rocket centred
            rocketTravelY      = -75.0f; // near bottom of screen
        }
    }
}

// ============================================================
//  init() and main()
// ============================================================
void init()
{
    srand(time(NULL));
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
    glDisable(GL_DITHER);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-100, 100, -100, 100);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 800);
    glutInitWindowPosition(100, 80);
    glutCreateWindow("Animated Room Scene - Mars Adventure");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);          // ← YOUR keyboard handler
    glutTimerFunc(0, timerFunc, 0);
    glutMainLoop();
    return 0;
}
