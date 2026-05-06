#include <GL/glut.h>
#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

// ── Original variables ───────────────────────────────────────
float fanAngle = 0.0f;        // Ceiling fan angle, 0 to 360 degrees.
float clockAngle = 0.0f;      // Clock hand angle, 0 to 360 degrees.
float mouseX = -120.0f;       // Mouse position, screen x about -120 to 120.
bool lampOn = false;          // Lamp state, false off and true on.
bool isAsleep = false;        // Girl sleeping flag, false awake or true asleep.
bool isDreaming = false;      // Dream bubble flag, false hidden or true visible.
float sleepTimer = 0.0f;      // Room elapsed time, seconds from scene start.
float zoomFactor = 1.0f;      // Dream zoom scale, starts 1 and grows past 20.
bool isSpaceScene = false;    // Space scene flag, false room or true space.
float starX[300], starY[300]; // Star coordinates, each about -100 to 100.
bool starsInitialized = false;// Prevents random stars being regenerated every frame.
float astroYOffset = 0.0f;    // Astronaut floating offset, about -10 to 10.
float astroRotation = 0.0f;   // Astronaut tilt angle, about -5 to 5 degrees.
float mercuryAngle = 0.0f;    // Mercury orbit angle, 0 to 360 repeating.
float venusAngle = 0.0f;      // Venus orbit angle, 0 to 360 repeating.
float earthAngle = 0.0f;      // Earth orbit angle, 0 to 360 repeating.
float rocketY = -120.0f;      // Space rocket y-position, -120 to 120.

// ── Mars scene variables ─────────────────────────────────────
bool isMarsScene       = false;  // Mars scene flag, false inactive or true active.
bool marsTransition    = false;  // Mars arrival flag, true during landing setup.
float marsRocketY      = -120.0f;// Mars rocket y-position, about -120 to 120.
float marsRocketLandY  = 0.0f;   // Landing rocket y-position, 90 down to -24.
float marsTimer        = 0.0f;   // Mars elapsed time, seconds while Mars runs.
int marsPhase = 0;               // Mars story phase, integer 0 through 6.

float girlWalkX    = 0.0f;       // Girl Mars x-position, about -38 to 8.
float alienAppear  = 0.0f;       // Alien visibility amount, 0 hidden to 1 visible.
float alien1X      = -55.0f;     // First alien x-position, screen units -100 to 100.
float alien2X      =  55.0f;     // Second alien x-position, screen units -100 to 100.
float alienBobY    = 0.0f;       // Alien vertical bob, about -3 to 3.
float marsPhaseTimer = 0.0f;     // Current phase seconds, resets each phase.
float escapeRocketY = -50.0f;    // Escape rocket y-position, -50 to above 110.
float warningAlpha = 0.0f;       // Warning fade amount, 0 transparent to 1 visible.

struct Alien {
    float x, y;          // Alien base position, screen units -100 to 100.
    float bobOffset;     // Unique bob phase, offsets sine timing.
    int type;            // Alien design type, 0 green or 1 purple.
    float facingAngle;   // Base rotation angle, degrees.
    bool isLeader;       // Leader flag, true for main alien only.
};

Alien aliens[5];                 // Fixed alien group, five members.
bool aliensInitialized = false;  // Formation setup flag, false until assigned.

float travelRocketX = 60.0f;     // Travel rocket x-position, about -100 to 100.
float travelRocketY = -120.0f;   // Travel rocket y-position, about -120 to 120.

// ── Travel cinematic variables ──────────────────────────────
bool  isTravelScene    = false;  // Travel scene flag, false inactive or true active.
float marsApproachSize = 5.0f;   // Mars radius during approach, 5 to 82.
float travelTimer      = 0.0f;   // Travel elapsed time, seconds.
float rocketTravelX    = 0.0f;   // Rocket sway x-offset, about -4 to 4.
float rocketTravelY    = -75.0f; // Rocket y-position in travel scene.

// ── NEW PART 7: Dream Wake-up and School Scene Variables ────
// ONLY ACTIVE AFTER ORIGINAL SCENARIOS END
bool isDreamWakeup     = false;  // Wake-up scene flag, false inactive or true active.
bool isSchoolScene     = false;  // School scene flag, false inactive or true active.
float dreamWakeupTimer = 0.0f;   // Wake-up elapsed time, seconds.
float schoolSceneTimer = 0.0f;   // School elapsed time, seconds.
int girlBlinkCount     = 0;      // Blink count, usually 0 to 2.
float girlBlinkTimer   = 0.0f;   // Blink cycle timer, seconds.
bool girlEyesClosed    = false;  // Eye state, false open or true closed.
float ufoTravelX       = 100.0f; // UFO x-position, about -100 to 100.
float ufoTravelY       = 65.0f;  // UFO y-position, about 65 to 80.
bool girlLookingUp     = false;  // Reaction flag, false forward or true skyward.
float girlSchoolX      = -15.0f; // School girl x-position, about -82 to -26.
float girlSchoolY      = -10.0f; // School girl y-position, about -41 ground level.
float postMarsWaitTimer = 0.0f;  // Wait after warning, 0 to 5 seconds.
bool finalSuspenseSoundStarted = false; // Audio guard, false stopped or true playing.

// ── Helper: draw rectangle ────────────────────────────────────
void drawRect(float x, float y, float w, float h,
              float r, float g, float b)
{
    /* GL_QUADS draws one four-corner polygon.
       Vertices are ordered clockwise around the rectangle.
       x,y is bottom-left; width and height extend it. */
    glColor3f(r, g, b);      // Set rectangle color, each channel 0 to 1.
    glBegin(GL_QUADS);       // Start drawing one quadrilateral.
    glVertex2f(x,     y);    // Bottom-left corner.
    glVertex2f(x + w, y);    // Bottom-right corner.
    glVertex2f(x + w, y + h);// Top-right corner.
    glVertex2f(x,     y + h);// Top-left corner.
    glEnd();                 // Finish rectangle drawing.
}

// ── Helper: draw circle ───────────────────────────────────────
void drawCircle(float cx, float cy, float r, int segs,
                float red, float green, float blue)
{
    /* GL_TRIANGLE_FAN draws triangles sharing one center.
       sinf/cosf convert angles into circular x,y points.
       More segments make the circle smoother. */
    glColor3f(red, green, blue); // Set circle color, channels 0 to 1.
    glBegin(GL_TRIANGLE_FAN);    // Start center-fan circle drawing.
    glVertex2f(cx, cy);          // Center point for all triangles.
    for (int i = 0; i <= segs; i++)
    {
        float theta = i * 2.0f * 3.1416f / segs; // Angle around full circle.
        glVertex2f(cx + r * cosf(theta), cy + r * sinf(theta)); // Edge point.
    }
    glEnd(); // Finish triangle fan.
}

// ── Draw text string ─────────────────────────────────────────
void drawText(float x, float y, const char* str,
              void* font = GLUT_BITMAP_HELVETICA_12)
{
    glRasterPos2f(x, y);
    while (*str) { glutBitmapCharacter(font, *str++); }
}

float clamp01(float v)
{
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

float smooth01(float v)
{
    v = clamp01(v);
    return v * v * (3.0f - 2.0f * v);
}

float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

void drawAlphaRect(float x, float y, float w, float h, float r, float g, float b, float a)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
    glEnd();
    glDisable(GL_BLEND);
}

void drawSoftCircle(float cx, float cy, float r, float red, float green, float blue, float alpha)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(red, green, blue, alpha);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= 48; i++)
        {
            float t = i * 2.0f * 3.1416f / 48.0f;
            glVertex2f(cx + cosf(t) * r, cy + sinf(t) * r);
        }
    glEnd();
    glDisable(GL_BLEND);
}

void playFinalSuspenseSound()
{
    if (finalSuspenseSoundStarted) return;
    finalSuspenseSoundStarted = true;
    mciSendString("close finalSuspense", NULL, 0, NULL);
    mciSendString("open \"sounds\\danger.mp3\" type mpegvideo alias finalSuspense", NULL, 0, NULL);
    mciSendString("play finalSuspense repeat", NULL, 0, NULL);
}

void stopFinalSuspenseSound()
{
    if (!finalSuspenseSoundStarted) return;
    mciSendString("stop finalSuspense", NULL, 0, NULL);
    mciSendString("close finalSuspense", NULL, 0, NULL);
    finalSuspenseSoundStarted = false;
}

// ============================================================
//  DRAW: Rocket (reusable, drawn at local origin)
// ============================================================
void drawRocketAt(float cx, float cy, float flicker)
{
    /* flicker controls flame size and brightness.
       Use 0.0f when landed, because engine is off.
       Use values above 0 when flying, for animated thrust. */
    glPushMatrix();          // Save current transform state.
    glTranslatef(cx, cy, 0); // Move rocket to requested position.

    drawRect(-5, 0, 10, 25, 0.9f, 0.9f, 0.9f); // Main rocket body.
    glColor3f(1.0f, 0.0f, 0.0f); // Nose and fin color.
    glBegin(GL_TRIANGLES);       // Triangle makes pointed nose cone.
        glVertex2f(-5, 25); glVertex2f(5, 25); glVertex2f(0, 35); // Nose.
    glEnd();                     // End nose triangle.
    glBegin(GL_TRIANGLES);       // Fins are triangular for rocket shape.
        glVertex2f(-5, 0); glVertex2f(-12, 0); glVertex2f(-5, 8); // Left fin.
        glVertex2f( 5, 0); glVertex2f( 12, 0); glVertex2f( 5, 8); // Right fin.
    glEnd();                     // End fin triangles.
    drawCircle(0, 15, 3, 20, 0.1f, 0.2f, 0.5f); // Window.

    if (flicker > 0.0f) // Draw flame only while flying.
    {
        float flame = 0.65f + flicker * 0.55f; // Flame length scale.
        glColor3f(1.0f, 0.35f + flicker * 0.35f, 0.0f); // Orange thrust.
        glBegin(GL_TRIANGLES); // Outer flame is a triangle.
            glVertex2f(-4, 0); glVertex2f(4, 0); glVertex2f(0, -12.0f * flame);
        glEnd(); // End outer flame.
        glColor3f(1.0f, 0.90f, 0.20f); // Yellow inner flame.
        glBegin(GL_TRIANGLES); // Inner flame triangle.
            glVertex2f(-3, 0); glVertex2f(3, 0); glVertex2f(0, -7.5f * flame);
        glEnd(); // End inner flame.
    }
    glPopMatrix(); // Restore previous transform state.
}

// ============================================================
//  DRAW: Girl in Astronaut Suit (used on Mars surface)
// ============================================================
void drawGirlStanding(float cx, float cy)
{
    glPushMatrix();
    glTranslatef(cx, cy, 0);

    // ── BOOTS ────────────────────────────────────────────────
    drawRect(-9,  -5, 9, 6, 0.85f, 0.85f, 0.88f);
    drawRect( 1,  -5, 9, 6, 0.85f, 0.85f, 0.88f);
    drawRect(-10, -6, 10, 2, 0.25f, 0.25f, 0.28f);
    drawRect(  1, -6, 10, 2, 0.25f, 0.25f, 0.28f);
    drawRect(-9,  1, 9, 2, 0.65f, 0.65f, 0.70f);
    drawRect( 1,  1, 9, 2, 0.65f, 0.65f, 0.70f);

    // ── LEGS ─────────────────────────────────────────────────
    drawRect(-8,  2, 7, 18, 0.90f, 0.90f, 0.93f);
    drawRect( 1,  2, 7, 18, 0.90f, 0.90f, 0.93f);
    drawRect(-9, 10, 9, 3, 0.65f, 0.65f, 0.70f);
    drawRect( 0, 10, 9, 3, 0.65f, 0.65f, 0.70f);

    // ── BACKPACK / PLSS ───────────────────────────────────────
    drawRect(-10, 20, 20, 18, 0.55f, 0.55f, 0.60f);
    glColor3f(0.40f, 0.40f, 0.45f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
        glVertex2f(-10, 26); glVertex2f(10, 26);
        glVertex2f(-10, 30); glVertex2f(10, 30);
    glEnd();
    drawRect(-8, 20, 3, 2, 0.30f, 0.30f, 0.35f);
    drawRect(-3, 20, 3, 2, 0.30f, 0.30f, 0.35f);
    drawRect( 3, 20, 3, 2, 0.30f, 0.30f, 0.35f);

    // ── TORSO ─────────────────────────────────────────────────
    drawRect(-9, 20, 18, 18, 0.92f, 0.92f, 0.95f);

    // ── CHEST PANEL ───────────────────────────────────────────
    drawRect(-5, 24, 10, 7, 0.70f, 0.72f, 0.75f);
    glColor3f(0.40f, 0.40f, 0.45f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-5, 24); glVertex2f(5, 24);
        glVertex2f( 5, 31); glVertex2f(-5, 31);
    glEnd();
    drawCircle(-3, 29, 1.0f, 8, 1.0f, 0.2f, 0.2f);
    drawCircle( 0, 29, 1.0f, 8, 0.2f, 1.0f, 0.2f);
    drawCircle( 3, 29, 1.0f, 8, 0.2f, 0.6f, 1.0f);
    drawRect(-4, 24, 8, 3, 0.10f, 0.30f, 0.50f);

    // ── OXYGEN TUBES ──────────────────────────────────────────
    glColor3f(0.70f, 0.70f, 0.75f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(-9, 32); glVertex2f(-12, 30);
        glVertex2f(-12, 25); glVertex2f(-10, 23);
    glEnd();
    glBegin(GL_LINE_STRIP);
        glVertex2f( 9, 32); glVertex2f( 12, 30);
        glVertex2f( 12, 25); glVertex2f( 10, 23);
    glEnd();

    // ── NECK RING ─────────────���───────────────────────────────
    drawRect(-6, 37, 12, 4, 0.65f, 0.65f, 0.70f);
    drawCircle(-4, 39, 0.8f, 8, 0.40f, 0.40f, 0.45f);
    drawCircle( 0, 39, 0.8f, 8, 0.40f, 0.40f, 0.45f);
    drawCircle( 4, 39, 0.8f, 8, 0.40f, 0.40f, 0.45f);

    // ── LEFT ARM ──────────────────────────────────────────────
    drawRect(-17, 28, 8, 9, 0.90f, 0.90f, 0.93f);
    drawRect(-18, 28, 3, 9, 0.65f, 0.65f, 0.70f);
    drawRect(-24, 29, 7, 7, 0.88f, 0.88f, 0.92f);
    drawRect(-26, 29, 3, 7, 0.65f, 0.65f, 0.70f);
    drawRect(-31, 28, 6, 9, 0.30f, 0.30f, 0.35f);

    // ── RIGHT ARM ─────────────────────────────────────────────
    drawRect( 9, 28, 8, 9, 0.90f, 0.90f, 0.93f);
    drawRect(15, 28, 3, 9, 0.65f, 0.65f, 0.70f);
    drawRect(17, 29, 7, 7, 0.88f, 0.88f, 0.92f);
    drawRect(23, 29, 3, 7, 0.65f, 0.65f, 0.70f);
    drawRect(25, 28, 6, 9, 0.30f, 0.30f, 0.35f);

    // ── HELMET ────────────────────────────────────────────────
    drawCircle(0, 48, 10, 40, 1.00f, 1.00f, 1.00f);
    glColor3f(0.80f, 0.65f, 0.10f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 40; i++) {
        float t = i * 2.0f * 3.1416f / 40;
        glVertex2f(0 + 10.5f * cosf(t), 48 + 10.5f * sinf(t));
    }
    glEnd();
    drawCircle(0, 48, 7, 30, 0.10f, 0.20f, 0.50f);
    glColor3f(0.70f, 0.55f, 0.05f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0, 48);
        for (int i = 0; i <= 20; i++) {
            float t = i * 3.1416f / 20;
            glVertex2f(0 + 7.0f * cosf(t), 48 + 7.0f * sinf(t));
        }
    glEnd();
    drawCircle(-3, 51, 1.8f, 15, 1.00f, 1.00f, 1.00f);
    drawCircle(-2, 50, 0.8f, 10, 1.00f, 1.00f, 0.90f);
    drawCircle(-6, 54, 1.5f, 12, 1.00f, 1.00f, 0.60f);

    glPopMatrix();
}
// ============================================================
//  DRAW: Alien
// ============================================================
void drawAlien(float cx, float cy, int style, float bob)
{
    glPushMatrix();
    glTranslatef(cx, cy + bob, 0);

    if (style == 0)
    {
        // ── Green alien ──
        glColor3f(0.1f, 0.7f, 0.1f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-8, 0); glVertex2f(-12, -14); glVertex2f(-5, -14);
            glVertex2f( 0, 0); glVertex2f( -3, -14); glVertex2f(  3, -14);
            glVertex2f( 8, 0); glVertex2f(  5, -14); glVertex2f( 12, -14);
        glEnd();
        drawCircle(0, 8, 10, 30, 0.1f, 0.8f, 0.1f);
        drawCircle(0, 22, 12, 30, 0.1f, 0.9f, 0.1f);
        drawCircle(-5, 24, 4, 20, 0.0f, 0.0f, 0.0f);
        drawCircle( 5, 24, 4, 20, 0.0f, 0.0f, 0.0f);
        drawCircle(-5, 24, 2, 20, 1.0f, 0.0f, 0.0f);
        drawCircle( 5, 24, 2, 20, 1.0f, 0.0f, 0.0f);
        drawCircle(-5, 24, 1, 10, 1.0f, 1.0f, 1.0f);
        drawCircle( 5, 24, 1, 10, 1.0f, 1.0f, 1.0f);
        glColor3f(0.0f, 0.6f, 0.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            glVertex2f(-4, 34); glVertex2f(-7, 42);
            glVertex2f( 4, 34); glVertex2f( 7, 42);
        glEnd();
        drawCircle(-7, 43, 2, 10, 1.0f, 1.0f, 0.0f);
        drawCircle( 7, 43, 2, 10, 1.0f, 1.0f, 0.0f);
        glColor3f(0.0f, 0.4f, 0.0f);
        glBegin(GL_LINE_STRIP);
            glVertex2f(-4, 16); glVertex2f(0, 14); glVertex2f(4, 16);
        glEnd();
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
        glColor3f(0.5f, 0.0f, 0.7f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-10,  0); glVertex2f(-14, -18); glVertex2f(-7, -18);
            glVertex2f( -3,  0); glVertex2f( -5, -18); glVertex2f( 2, -18);
            glVertex2f(  3,  0); glVertex2f(  2, -18); glVertex2f( 7, -18);
            glVertex2f( 10,  0); glVertex2f(  7, -18); glVertex2f(14, -18);
        glEnd();
        drawCircle(0, 10, 12, 30, 0.6f, 0.0f, 0.8f);
        drawCircle(0, 24, 13, 30, 0.7f, 0.0f, 0.9f);
        drawCircle(-6, 26, 3, 20, 1.0f, 1.0f, 0.0f);
        drawCircle( 6, 26, 3, 20, 1.0f, 1.0f, 0.0f);
        drawCircle( 0, 30, 3, 20, 1.0f, 1.0f, 0.0f);
        drawCircle(-6, 26, 1.2f, 10, 0.0f, 0.0f, 0.0f);
        drawCircle( 6, 26, 1.2f, 10, 0.0f, 0.0f, 0.0f);
        drawCircle( 0, 30, 1.2f, 10, 0.0f, 0.0f, 0.0f);
        glColor3f(0.4f, 0.0f, 0.6f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            glVertex2f(0, 37); glVertex2f(0, 46);
        glEnd();
        drawCircle(0, 47, 3, 15, 1.0f, 0.0f, 1.0f);
        glColor3f(1.0f, 0.5f, 0.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
            glVertex2f(-5, 18); glVertex2f(-2, 16);
            glVertex2f( 2, 19); glVertex2f( 5, 17);
        glEnd();
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
    /* Each alien gets its own transform.
       glPushMatrix isolates movement, rotation, and scale.
       alert is 0 calm to 1 fully hostile. */
    glPushMatrix();              // Save world transform before alien.
    glTranslatef(cx, cy + bob, 0);// Position alien with bob offset.
    glRotatef(angle, 0, 0, 1);   // Rotate alien around local z-axis.
    glScalef(scale, scale, 1.0f);// Resize alien without changing model code.
    drawAlien(0, 0, style, 0);   // Draw alien at local origin.

    float glow = 0.35f + alert * 0.65f; // Eye glow rises with alert.
    if (style == 0) // Green alien eye style.
    {
        drawCircle(-5, 24, 3.0f + alert * 1.8f, 20, 1.0f, 0.05f, 0.0f); // Left hostile eye.
        drawCircle( 5, 24, 3.0f + alert * 1.8f, 20, 1.0f, 0.05f, 0.0f); // Right hostile eye.
    }
    else // Purple alien eye style.
    {
        drawCircle(-6, 26, 2.8f + alert * 1.5f, 20, 1.0f, glow, 0.0f); // Left glowing eye.
        drawCircle( 6, 26, 2.8f + alert * 1.5f, 20, 1.0f, glow, 0.0f); // Right glowing eye.
        drawCircle( 0, 30, 2.8f + alert * 1.5f, 20, 1.0f, glow, 0.0f); // Center glowing eye.
    }

    glColor3f(0.9f, 0.05f, 0.05f); // Weapon color.
    glLineWidth(2.5f);             // Make weapon lines visible.
    glBegin(GL_LINES);             // Draw simple weapon arms.
        glVertex2f(-14, 10); glVertex2f(-27, 8); // Left weapon.
        glVertex2f( 14, 10); glVertex2f( 27, 8); // Right weapon.
    glEnd();                       // End weapon lines.
    glPopMatrix();                 // Restore transform for next alien.
}

void drawAlienGroup()
{
    /* Group logic computes shared emotion.
       bobOffset staggers each alien's sine movement.
       Leader reacts stronger than background aliens. */
    initAlienFormation(); // Create alien positions once.

    float alert = 0.0f; // Alert level, 0 calm to 1 attack.
    if (marsPhase >= 3) // Aliens notice girl from phase 3.
        alert = marsPhase == 3 ? marsPhaseTimer / 1.2f : 1.0f; // Fade alert in.
    if (alert > 1.0f) alert = 1.0f; // Clamp alert maximum.

    float bobScale = marsPhase == 3 ? 0.05f : 0.65f; // Less bob while noticing.
    if (marsPhase >= 4) bobScale = 1.1f; // More movement during attack.

    drawCircle(56, -61, 22, 36, 0.20f, 0.04f, 0.03f); // Alien base shadow.
    drawCircle(56, -61, 10, 24, 0.85f, 0.18f, 0.02f); // Glowing center.
    glColor3f(1.0f, 0.20f + alert * 0.50f, 0.0f); // Ring brightens.
    glLineWidth(1.5f); // Thin energy ring.
    glBegin(GL_LINE_LOOP); // Closed oval loop.
    for (int i = 0; i < 40; i++)
    {
        float t = i * 2.0f * 3.1416f / 40; // Angle around oval.
        glVertex2f(56 + cosf(t) * 30, -61 + sinf(t) * 12); // Oval point.
    }
    glEnd(); // End energy ring.

    for (int i = 0; i < 5; i++) // Draw every alien.
    {
        Alien alien = aliens[i]; // Copy current alien data.
        float bob = sinf(marsTimer * (alien.isLeader ? 3.4f : 2.4f) + alien.bobOffset) *
                    (alien.isLeader ? 2.4f : 1.4f) * bobScale; // Sine bob.
        float angle = alien.facingAngle + sinf(marsTimer * 1.5f + alien.bobOffset) * 3.0f; // Wobble angle.
        float alienAlert = alert; // Individual alert level.
        float scale = alien.isLeader ? 0.68f : 0.58f; // Leader is larger.

        if (marsPhase == 3) // Leader notices the girl.
        {
            if (alien.isLeader) // Leader turns first.
            {
                angle = -54.0f; // Face toward girl.
                scale += 0.10f * alert; // Leader grows visually threatening.
            }
            else // Other aliens remain calmer.
            {
                angle = alien.facingAngle; // Keep normal facing.
                alienAlert *= 0.45f; // Reduce background alien alert.
            }
        }
        else if (marsPhase >= 4) // Attack phase.
        {
            angle = -48.0f - i * 4.0f; // Group aims toward girl.
            scale += 0.06f; // Slightly larger during attack.
        }

        drawHostileAlien(alien.x, alien.y, alien.type, bob, angle, alienAlert, scale); // Draw one alien.
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
    /* Laser is drawn twice.
       Thick colored line creates outer energy beam.
       Thin bright line creates hot white core. */
    float pulse = (sinf(phase) + 1.0f) * 0.5f; // Sine pulse, 0 to 1.
    glLineWidth(4.0f + pulse * 3.0f); // Beam width expands.
    glColor3f(r, g, b); // Outer laser color.
    glBegin(GL_LINES); // Draw outer beam.
        glVertex2f(x1, y1); // Beam start point.
        glVertex2f(x2, y2); // Beam target point.
    glEnd(); // End outer beam.
    glLineWidth(1.5f); // Thin core line.
    glColor3f(1.0f, 0.9f, 0.55f); // Bright inner beam.
    glBegin(GL_LINES); // Draw inner beam.
        glVertex2f(x1, y1); // Same start point.
        glVertex2f(x2, y2); // Same target point.
    glEnd(); // End inner beam.
    drawCircle(x2, y2, 3.0f + pulse * 3.0f, 18, 1.0f, 0.25f + pulse * 0.55f, 0.0f); // Impact flash.
}

void drawWarningText()
{
    /* warningAlpha fades the transmission from invisible to visible.
       marsTimer*9 makes fast blinking because sine cycles quickly.
       Scan-line loop suggests an electronic warning screen. */
    float blink = (sinf(marsTimer * 9.0f) + 1.0f) * 0.5f; // Fast blink, 0 to 1.
    float intensity = warningAlpha * (0.45f + blink * 0.55f); // Fade plus blink.

    drawRect(-100, -100, 200, 200, 0.01f, 0.0f, 0.0f); // Dark warning background.

    glColor3f(intensity, 0.0f, 0.0f); // Red scan intensity.
    glLineWidth(2.0f); // Visible scan-line thickness.
    for (int y = -70; y <= 70; y += 18) // Repeat horizontal scan lines.
    {
        glBegin(GL_LINES); // One scan line.
            glVertex2f(-90, (float)y); // Left endpoint.
            glVertex2f( 90, (float)y); // Right endpoint.
        glEnd(); // End scan line.
    }

    glColor3f(1.0f, 0.10f + intensity * 0.35f, 0.05f); // Warning text glow.
    drawText(-88, 12, "WARNING: Aliens are planning to attack Earth!", GLUT_BITMAP_HELVETICA_18); // Main warning.
    glColor3f(0.75f, 0.95f, 0.85f); // Confirmation color.
    drawText(-44, -8, "TRANSMISSION RECEIVED", GLUT_BITMAP_HELVETICA_12); // Transmission status.
}

// ============================================================
// ── NEW PART 7: Dream Wake-up Drawing Functions
// ============================================================
void drawGirlBlinkingEyes(float cx, float cy, bool eyesClosed)
{
    glPushMatrix();
    glTranslatef(cx, cy, 0);

    drawCircle(0, 0, 10, 30, 1.0f, 0.8f, 0.6f);

    drawCircle(-7, 5, 5, 20, 0.6f, 0.3f, 0.1f);
    drawCircle(7, 5, 5, 20, 0.6f, 0.3f, 0.1f);
    drawCircle(0, 7, 6, 20, 0.6f, 0.3f, 0.1f);

    if (!eyesClosed)
    {
        drawCircle(-4, 2, 2, 15, 1.0f, 1.0f, 1.0f);
        drawCircle(4, 2, 2, 15, 1.0f, 1.0f, 1.0f);
        drawCircle(-4, 2, 1.2f, 10, 0.2f, 0.2f, 0.4f);
        drawCircle(4, 2, 1.2f, 10, 0.2f, 0.2f, 0.4f);
        drawCircle(-3.2f, 2.5f, 0.6f, 8, 1.0f, 1.0f, 1.0f);
        drawCircle(4.8f, 2.5f, 0.6f, 8, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(2.5f);
        glBegin(GL_LINES);
            glVertex2f(-5, 2);   glVertex2f(-2, 2);
            glVertex2f(2, 2);    glVertex2f(5, 2);
        glEnd();
    }

    glColor3f(0.5f, 0.2f, 0.05f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(-5, 4);   glVertex2f(-2, 5);
        glVertex2f(2, 5);    glVertex2f(5, 4);
    glEnd();

    glColor3f(1.0f, 0.75f, 0.55f);
    glBegin(GL_TRIANGLES);
        glVertex2f(0, 2);    glVertex2f(-0.8f, -1);   glVertex2f(0.8f, -1);
    glEnd();

    glColor3f(0.8f, 0.2f, 0.2f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(-2.5f, -3);  glVertex2f(0, -4.5f);  glVertex2f(2.5f, -3);
    glEnd();

    glPopMatrix();
}

void drawSchoolBuilding()
{
    drawRect(20, -100, 80, 180, 0.7f, 0.4f, 0.2f);

    glColor3f(0.5f, 0.3f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(20, -100);   glVertex2f(100, -100);
        glVertex2f(100, 80);    glVertex2f(20, 80);
    glEnd();

    glColor3f(0.5f, 0.1f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(20, 80);     glVertex2f(100, 80);    glVertex2f(60, 110);
    glEnd();

    glColor3f(0.3f, 0.05f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(20, 80);     glVertex2f(60, 110);    glVertex2f(100, 80);
    glEnd();

    drawRect(57, 110, 4, 18, 0.4f, 0.2f, 0.1f);
    drawRect(61, 125, 12, 7, 1.0f, 0.0f, 0.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(33, 88, "SCHOOL", GLUT_BITMAP_HELVETICA_18);

    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            float wx = 28.0f + col * 15.5f;
            float wy = 60.0f - row * 19.0f;

            drawRect(wx, wy, 11, 11, 0.2f, 0.35f, 0.55f);
            drawRect(wx + 0.5f, wy + 0.5f, 5, 5, 0.5f, 0.75f, 0.95f);
            drawRect(wx + 5.5f, wy + 0.5f, 5, 5, 0.5f, 0.75f, 0.95f);
            drawRect(wx + 0.5f, wy + 5.5f, 5, 5, 0.5f, 0.75f, 0.95f);
            drawRect(wx + 5.5f, wy + 5.5f, 5, 5, 0.5f, 0.75f, 0.95f);
        }
    }

    drawRect(49, -100, 18, 32, 0.4f, 0.2f, 0.05f);
    drawCircle(66, -84, 1.5f, 12, 1.0f, 0.8f, 0.0f);

    drawRect(20, -100, 80, 2, 0.4f, 0.2f, 0.1f);
}

void drawGirlAtSchool(float cx, float cy, bool lookingUp)
{
    glPushMatrix();
    glTranslatef(cx, cy, 0);

    drawRect(-5.5f, -23, 5.5f, 8, 0.3f, 0.3f, 0.3f);
    drawRect(0, -23, 5.5f, 8, 0.3f, 0.3f, 0.3f);

    drawRect(-5, -15, 5, 18, 0.8f, 0.6f, 0.4f);
    drawRect(0, -15, 5, 18, 0.8f, 0.6f, 0.4f);

    drawRect(-7, 3, 14, 13, 1.0f, 0.7f, 0.5f);

    if (lookingUp)
    {
        drawRect(-13, 10, 6, 5, 1.0f, 0.8f, 0.6f);
        drawRect(7, 10, 6, 5, 1.0f, 0.8f, 0.6f);
        drawRect(-15, 15, 2.5f, 7, 1.0f, 0.8f, 0.6f);
        drawRect(12.5f, 15, 2.5f, 7, 1.0f, 0.8f, 0.6f);
    }
    else
    {
        drawRect(-13, 3, 6, 11, 1.0f, 0.8f, 0.6f);
        drawRect(7, 3, 6, 11, 1.0f, 0.8f, 0.6f);
        drawRect(-15, 3, 2.5f, 8, 1.0f, 0.8f, 0.6f);
        drawRect(12.5f, 3, 2.5f, 8, 1.0f, 0.8f, 0.6f);
    }

    drawRect(-2.5f, 16, 5, 2, 1.0f, 0.8f, 0.6f);

    drawCircle(0, 22, 8, 28, 1.0f, 0.8f, 0.6f);

    drawCircle(-5.5f, 27, 4.5f, 18, 0.6f, 0.3f, 0.1f);
    drawCircle(5.5f, 27, 4.5f, 18, 0.6f, 0.3f, 0.1f);
    drawCircle(0, 29, 5.5f, 18, 0.6f, 0.3f, 0.1f);

    if (lookingUp)
    {
        drawCircle(-3.5f, 23, 1.8f, 14, 1.0f, 1.0f, 1.0f);
        drawCircle(3.5f, 23, 1.8f, 14, 1.0f, 1.0f, 1.0f);
        drawCircle(-3.5f, 23, 0.9f, 10, 0.2f, 0.2f, 0.4f);
        drawCircle(3.5f, 23, 0.9f, 10, 0.2f, 0.2f, 0.4f);
        drawCircle(-2.8f, 24, 0.5f, 8, 1.0f, 1.0f, 1.0f);
        drawCircle(4.2f, 24, 0.5f, 8, 1.0f, 1.0f, 1.0f);

        glColor3f(0.8f, 0.3f, 0.1f);
        drawCircle(0, 17, 1.8f, 16, 0.8f, 0.3f, 0.1f);
    }
    else
    {
        drawCircle(-3.5f, 23, 1.8f, 14, 1.0f, 1.0f, 1.0f);
        drawCircle(3.5f, 23, 1.8f, 14, 1.0f, 1.0f, 1.0f);
        drawCircle(-3.5f, 23, 0.9f, 10, 0.2f, 0.2f, 0.4f);
        drawCircle(3.5f, 23, 0.9f, 10, 0.2f, 0.2f, 0.4f);

        glColor3f(0.8f, 0.2f, 0.2f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
            glVertex2f(-2, 17);   glVertex2f(0, 15.5f);   glVertex2f(2, 17);
        glEnd();
    }

    glPopMatrix();
}

void drawCloud(float cx, float cy, float scale, float r, float g, float b)
{
    glPushMatrix();
    glTranslatef(cx, cy, 0);
    glScalef(scale, scale, 1.0f);
    drawSoftCircle(-14, 0, 13, r, g, b, 0.68f);
    drawSoftCircle(0, 5, 16, r, g, b, 0.74f);
    drawSoftCircle(16, 0, 12, r, g, b, 0.62f);
    drawAlphaRect(-26, -8, 52, 12, r, g, b, 0.54f);
    glPopMatrix();
}

void drawMorningStable(float t)
{
    glBegin(GL_QUADS);
        glColor3f(0.43f, 0.68f, 0.86f);
        glVertex2f(-100, -100); glVertex2f(100, -100);
        glColor3f(0.86f, 0.91f, 0.88f);
        glVertex2f(100, 100); glVertex2f(-100, 100);
    glEnd();

    drawRect(-100, -100, 200, 88, 0.33f, 0.20f, 0.12f);
    drawRect(-100, -12, 200, 18, 0.22f, 0.12f, 0.07f);
    drawRect(-100, 6, 200, 94, 0.42f, 0.25f, 0.14f);
    for (int i = 0; i < 9; i++)
        drawRect(-96.0f + i * 24.0f, -12, 5, 112, 0.23f, 0.12f, 0.06f);

    drawRect(44, -10, 38, 72, 0.15f, 0.09f, 0.05f);
    drawRect(50, -4, 26, 60, 0.84f, 0.76f, 0.54f);
    for (int i = 0; i < 5; i++)
    {
        float x = 58.0f + i * 16.0f;
        drawAlphaRect(50, 56 - i * 4, x - 8, -76, 1.0f, 0.78f, 0.38f, 0.07f);
    }

    for (int i = 0; i < 42; i++)
    {
        float x = -88.0f + (float)((i * 37) % 176);
        float y = -3.0f + (float)((i * 29) % 92);
        float drift = sinf(t * 0.7f + i * 1.37f) * 2.5f;
        drawSoftCircle(x + drift, y, 0.55f + (i % 3) * 0.18f, 1.0f, 0.86f, 0.56f, 0.24f);
    }

    drawRect(-70, -60, 82, 18, 0.77f, 0.58f, 0.25f);
    drawRect(-74, -72, 90, 12, 0.65f, 0.44f, 0.18f);
    for (int i = 0; i < 12; i++)
        drawRect(-72 + i * 8.0f, -54 + sinf(i) * 2.0f, 9, 3, 0.90f, 0.75f, 0.35f);
}

void drawWakeGirl(float cx, float cy, float t)
{
    float startle = 1.0f - smooth01(t / 2.4f);
    float breathe = sinf(t * 7.0f) * 1.2f * startle + sinf(t * 1.8f) * 0.55f;
    bool eyesClosed = (t < 0.45f) || (t > 1.05f && t < 1.23f);

    glPushMatrix();
    glTranslatef(cx, cy + breathe, 0);
    glRotatef(-7.0f + startle * 16.0f, 0, 0, 1);

    drawRect(-26, -5, 42, 15, 0.82f, 0.37f, 0.30f);
    drawRect(-18, 8, 28, 20, 0.95f, 0.70f, 0.50f);
    drawRect(-29, -1, 9, 20, 0.96f, 0.72f, 0.52f);
    drawRect(8, 0, 9, 19, 0.96f, 0.72f, 0.52f);

    drawCircle(-4, 35, 12, 32, 1.0f, 0.78f, 0.58f);
    drawCircle(-11, 40, 7, 24, 0.34f, 0.17f, 0.07f);
    drawCircle(5, 41, 8, 24, 0.34f, 0.17f, 0.07f);
    drawCircle(-3, 47, 8, 24, 0.34f, 0.17f, 0.07f);

    if (eyesClosed)
    {
        glColor3f(0.08f, 0.05f, 0.03f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            glVertex2f(-9, 35); glVertex2f(-4, 34);
            glVertex2f(2, 34); glVertex2f(7, 35);
        glEnd();
    }
    else // Default original room scene.
    {
        drawCircle(-7, 35, 2.2f, 16, 1.0f, 1.0f, 1.0f);
        drawCircle(4, 35, 2.2f, 16, 1.0f, 1.0f, 1.0f);
        drawCircle(-7, 35, 1.1f + startle * 0.45f, 12, 0.13f, 0.13f, 0.18f);
        drawCircle(4, 35, 1.1f + startle * 0.45f, 12, 0.13f, 0.13f, 0.18f);
    }

    glColor3f(0.55f, 0.14f, 0.11f);
    glLineWidth(1.7f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(-7, 27); glVertex2f(-3, 25.5f - startle * 2.0f); glVertex2f(3, 27);
    glEnd();
    glPopMatrix();
}

void drawSchoolMorningWorld(float t, bool revealMode)
{
    glBegin(GL_QUADS);
        glColor3f(0.70f, 0.86f, 0.96f);
        glVertex2f(-100, -100); glVertex2f(100, -100);
        glColor3f(0.36f, 0.60f, 0.82f);
        glVertex2f(100, 100); glVertex2f(-100, 100);
    glEnd();

    drawCloud(-62 + sinf(t * 0.12f) * 5.0f, 64, 1.05f, 0.94f, 0.96f, 0.96f);
    drawCloud(18 + sinf(t * 0.15f + 2.0f) * 4.0f, 74, 1.28f, 0.90f, 0.93f, 0.94f);
    drawCloud(66 - sinf(t * 0.11f) * 5.0f, 58, 0.88f, 0.92f, 0.95f, 0.96f);

    drawCircle(-72, 75, 13, 32, 1.0f, 0.80f, 0.35f);
    drawAlphaRect(-100, -100, 200, 64, 0.28f, 0.57f, 0.23f, 1.0f);
    glBegin(GL_QUADS);
        glColor3f(0.52f, 0.48f, 0.42f);
        glVertex2f(-100, -100); glVertex2f(100, -100);
        glColor3f(0.62f, 0.59f, 0.52f);
        glVertex2f(100, -60); glVertex2f(-100, -60);
    glEnd();
    drawRect(-100, -67, 200, 3, 0.91f, 0.86f, 0.64f);

    drawRect(48, -58, 33, 70, 0.58f, 0.48f, 0.36f);
    drawRect(43, 12, 43, 8, 0.44f, 0.33f, 0.22f);
    for (int i = 0; i < 3; i++)
        drawRect(54 + i * 9.0f, -5, 5, 9, 0.50f, 0.72f, 0.82f);
    drawRect(62, -58, 10, 23, 0.25f, 0.16f, 0.10f);

    if (revealMode)
        drawAlphaRect(-100, -100, 200, 200, 0.03f, 0.05f, 0.08f, 0.12f);
}

void drawUFO(float cx, float cy)
{
    float reveal = smooth01((schoolSceneTimer - 8.1f) / 2.0f);
    float shimmer = (sinf(schoolSceneTimer * 12.0f) + 1.0f) * 0.5f;

    glPushMatrix();
    glTranslatef(cx, cy, 0);
    glScalef(0.78f + reveal * 0.18f, 0.78f + reveal * 0.18f, 1.0f);

    drawSoftCircle(0, 2, 24, 0.50f, 0.78f, 0.88f, 0.12f + shimmer * 0.08f);
    drawAlphaRect(-29, -5, 58, 9, 0.14f, 0.17f, 0.18f, 0.92f);
    drawSoftCircle(0, 6, 14, 0.33f, 0.42f, 0.47f, 0.92f);
    drawAlphaRect(-21, -1, 42, 8, 0.58f, 0.62f, 0.60f, 0.85f);

    glColor3f(0.06f, 0.08f, 0.09f);
    glLineWidth(1.4f);
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 56; i++)
        {
            float a = i * 2.0f * 3.1416f / 56.0f;
            glVertex2f(cosf(a) * 29.0f, -1.5f + sinf(a) * 8.5f);
        }
    glEnd();

    glColor3f(0.70f, 0.88f, 0.95f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glVertex2f(-18, 3); glVertex2f(18, 3);
        glVertex2f(-12, -5); glVertex2f(12, -5);
    glEnd();

    drawSoftCircle(-15, -4, 3.0f, 0.56f, 0.96f, 1.0f, 0.42f + shimmer * 0.20f);
    drawSoftCircle(0, -5, 3.8f, 0.56f, 0.96f, 1.0f, 0.48f + shimmer * 0.25f);
    drawSoftCircle(15, -4, 3.0f, 0.56f, 0.96f, 1.0f, 0.42f + shimmer * 0.20f);
    glPopMatrix();
}
// ============================================================
//  DRAW: Travel Cinematic Scene
// ============================================================
void drawTravelScene()
{
    drawRect(-100, -100, 200, 200, 0.0f, 0.0f, 0.04f);

    glLineWidth(1.0f);
    for (int i = 0; i < 300; i++)
    {
        float speed = 0.3f + (marsApproachSize / 80.0f) * 2.5f;
        float sx = starX[i];
        float sy = starY[i];
        float streakLen = speed * 4.0f;
        float brightness = 0.5f + (marsApproachSize / 80.0f) * 0.5f;
        glColor3f(brightness, brightness, brightness);
        glBegin(GL_LINES);
            glVertex2f(sx, sy);
            glVertex2f(sx * (1.0f - streakLen / 100.0f),
                       sy * (1.0f - streakLen / 100.0f));
        glEnd();
    }

    float r = marsApproachSize;
    drawCircle(0, 30, r, 60, 0.75f, 0.25f, 0.05f);

    if (r > 20)
    {
        glColor3f(0.45f, 0.12f, 0.02f);
        glBegin(GL_QUADS);
            glVertex2f(-r*0.90f, 30 - r*0.05f);
            glVertex2f( r*0.90f, 30 - r*0.05f);
            glVertex2f( r*0.80f, 30 + r*0.12f);
            glVertex2f(-r*0.80f, 30 + r*0.12f);
        glEnd();

        glColor3f(0.82f, 0.38f, 0.12f);
        glBegin(GL_QUADS);
            glVertex2f(-r*0.85f, 30 + r*0.25f);
            glVertex2f( r*0.85f, 30 + r*0.25f);
            glVertex2f( r*0.75f, 30 + r*0.42f);
            glVertex2f(-r*0.75f, 30 + r*0.42f);
        glEnd();

        drawCircle(-r*0.28f, 30 + r*0.15f, r*0.16f, 25, 0.60f, 0.20f, 0.04f);
        drawCircle(-r*0.28f, 30 + r*0.15f, r*0.07f, 15, 0.50f, 0.15f, 0.02f);

        drawCircle( r*0.38f, 30 - r*0.22f, r*0.18f, 25, 0.55f, 0.16f, 0.03f);

        drawCircle( r*0.10f, 30 + r*0.35f, r*0.05f, 12, 0.50f, 0.14f, 0.02f);
        drawCircle(-r*0.50f, 30 + r*0.10f, r*0.04f, 10, 0.50f, 0.14f, 0.02f);
        drawCircle( r*0.55f, 30 + r*0.10f, r*0.04f, 10, 0.50f, 0.14f, 0.02f);

        drawCircle(0, 30 + r*0.78f, r*0.22f, 25, 0.93f, 0.93f, 1.00f);
        drawCircle(0, 30 - r*0.80f, r*0.14f, 20, 0.90f, 0.90f, 0.97f);

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

        float phAngle = travelTimer * 2.5f;
        float phX = 0   + (r + 14) * cosf(phAngle);
        float phY = 30  + (r + 14) * 0.35f * sinf(phAngle);
        drawCircle(phX, phY, 3.5f, 12, 0.70f, 0.60f, 0.50f);
    }

    if (r > 45)
    {
        glColor3f(1.0f, 1.0f, 0.5f);
        glLineWidth(1.0f);
        glBegin(GL_LINES);
            glVertex2f(-r*0.28f, 30 + r*0.15f);
            glVertex2f(-r*0.28f - 12, 30 + r*0.15f + 12);
        glEnd();
        drawText(-r*0.28f - 40, 30 + r*0.15f + 14,
                 "Olympus Mons", GLUT_BITMAP_HELVETICA_10);

        glBegin(GL_LINES);
            glVertex2f(4, 30 + r*0.78f);
            glVertex2f(14, 30 + r*0.78f + 8);
        glEnd();
        drawText(15, 30 + r*0.78f + 7,
                 "N. Ice Cap", GLUT_BITMAP_HELVETICA_10);
    }

    glPushMatrix();
    glTranslatef(rocketTravelX, rocketTravelY, 0);
    glRotatef(sinf(travelTimer * 1.5f) * 3.0f, 0, 0, 1);

    drawRect(-5, 0, 10, 25, 0.90f, 0.90f, 0.92f);
    glColor3f(1.0f, 0.20f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-5, 25); glVertex2f(5, 25); glVertex2f(0, 37);
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(-5, 0); glVertex2f(-13, 0); glVertex2f(-5, 9);
        glVertex2f( 5, 0); glVertex2f( 13, 0); glVertex2f( 5, 9);
    glEnd();
    drawCircle(0, 15, 3, 20, 0.10f, 0.20f, 0.55f);

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
    drawCircle(0, -3, 4 + flicker*2, 15, 1.0f, 0.6f, 0.1f);

    glPopMatrix();

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

    glColor3f(0.45f, 0.45f, 0.50f);
    drawText(-40, -95, "Press R to restart", GLUT_BITMAP_HELVETICA_12);
}

// ============================================================
//  DRAW: Mars surface scene
// ============================================================
void drawMarsScene()
{
    if (marsPhase == 6)
    {
        drawWarningText();
        return;
    }

    glBegin(GL_QUADS);
        glColor3f(0.6f, 0.2f, 0.05f);
        glVertex2f(-100, -100);
        glVertex2f( 100, -100);
        glColor3f(0.3f, 0.05f, 0.0f);
        glVertex2f( 100,  100);
        glVertex2f(-100,  100);
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
    for (int i = 0; i < 100; i++)
        drawCircle(starX[i], starY[i], 0.2f, 5, 0.8f, 0.7f, 0.7f);

    drawRect(-100, -100, 200, 50, 0.6f, 0.2f, 0.0f);
    drawRect(-100, -52, 200, 5, 0.7f, 0.3f, 0.1f);
    drawCircle(-70, -50, 8, 20, 0.5f, 0.15f, 0.0f);
    drawCircle(-68, -50, 5, 20, 0.45f, 0.12f, 0.0f);
    drawCircle( 60, -50, 10, 20, 0.5f, 0.15f, 0.0f);
    drawCircle( 62, -50, 6,  20, 0.45f, 0.12f, 0.0f);
    drawCircle(-40, -51, 3, 10, 0.55f, 0.2f, 0.05f);
    drawCircle( 30, -51, 4, 10, 0.55f, 0.2f, 0.05f);
    drawCircle( 10, -51, 2, 10, 0.55f, 0.2f, 0.05f);

    glColor3f(0.5f, 0.15f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-100, -50); glVertex2f(-70, -10); glVertex2f(-40, -50);
        glVertex2f( -20, -50); glVertex2f( 10,  -5); glVertex2f( 40, -50);
        glVertex2f(  50, -50); glVertex2f( 80,  -8); glVertex2f(100, -50);
    glEnd();

    drawCircle(75, 70, 6, 20, 0.7f, 0.6f, 0.5f);
    drawCircle(73, 72, 1, 10, 0.6f, 0.5f, 0.4f);

    if (marsPhase == 0)
    {
        float flicker = (sinf(marsTimer * 15.0f) + 1.0f) * 0.5f;
        drawRocketAt(0, marsRocketLandY, flicker);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(-35, 80, "Landing on Mars...", GLUT_BITMAP_HELVETICA_18);
    }

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

    if (marsPhase >= 1 && marsPhase < 5)
    {
        drawRocketAt(0, -50, 0.0f);
        glColor3f(0.6f, 0.6f, 0.6f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
            glVertex2f(-5, -50); glVertex2f(-14, -56);
            glVertex2f( 5, -50); glVertex2f( 14, -56);
        glEnd();
        drawRect(-18, -58, 8, 3, 0.6f, 0.6f, 0.6f);
        drawRect(  9, -58, 8, 3, 0.6f, 0.6f, 0.6f);
    }

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
        glColor3f(1.0f, 0.5f, 0.05f);
        drawCircle(0, escapeRocketY - 7, 10.0f + flame * 8.0f, 24, 1.0f, 0.35f, 0.0f);
        glColor3f(1.0f, 0.8f, 0.25f);
        drawText(-36, 80, "Escape!", GLUT_BITMAP_HELVETICA_18);
    }

    glColor3f(0.6f, 0.4f, 0.3f);
    drawText(-43, -95, "Mars mission in progress. Press R to restart.", GLUT_BITMAP_HELVETICA_12);
}

// ============================================================
// ── Dream Wake-up & School Scene Display (AFTER MARS ENDS)
// ============================================================
void drawDreamWakeupScreen()
{
    float t = dreamWakeupTimer;
    drawMorningStable(t);

    float dissolve = 1.0f - smooth01(t / 2.1f);
    if (dissolve > 0.01f)
    {
        drawAlphaRect(-100, -100, 200, 200, 0.01f, 0.02f, 0.07f, dissolve * 0.70f);
        for (int i = 0; i < 90; i++)
        {
            float sx = -96.0f + (float)((i * 53) % 192);
            float sy = -90.0f + (float)((i * 31) % 180);
            drawSoftCircle(sx, sy, 0.35f + (i % 4) * 0.12f, 0.75f, 0.90f, 1.0f, dissolve * 0.35f);
        }
    }

    drawWakeGirl(-24, -34, t);

    float subtitleIn = smooth01((t - 2.3f) / 1.2f) * (1.0f - smooth01((t - 5.5f) / 1.0f));
    if (subtitleIn > 0.02f)
    {
        drawAlphaRect(-34, -82, 82, 15, 0.03f, 0.025f, 0.02f, 0.26f * subtitleIn);
        glColor3f(0.96f * subtitleIn, 0.92f * subtitleIn, 0.84f * subtitleIn);
        drawText(-26, -77, "\"Oh... it was just a dream.\"", GLUT_BITMAP_HELVETICA_18);
    }

    float fade = smooth01((t - 6.2f) / 1.4f);
    if (fade > 0.0f)
        drawAlphaRect(-100, -100, 200, 200, 0.98f, 0.92f, 0.80f, fade * 0.65f);
}

void drawSchoolSceneScreen()
{
    float t = schoolSceneTimer;
    bool revealMode = t > 8.2f;
    drawSchoolMorningWorld(t, revealMode);

    float walk = smooth01(t / 6.5f);
    girlSchoolX = lerp(-82.0f, -26.0f, walk);
    float step = sinf(t * 8.0f) * 1.1f * (1.0f - smooth01((t - 7.0f) / 0.7f));
    girlLookingUp = t > 8.4f;

    glPushMatrix();
    float faceZoom = smooth01((t - 8.6f) / 2.2f);
    if (faceZoom > 0.0f)
    {
        glTranslatef(lerp(0.0f, 17.0f, faceZoom), lerp(0.0f, -8.0f, faceZoom), 0);
        glScalef(1.0f + faceZoom * 0.38f, 1.0f + faceZoom * 0.38f, 1.0f);
    }
    drawGirlAtSchool(girlSchoolX, -41.0f + step, girlLookingUp);
    glPopMatrix();

    if (t > 8.1f)
    {
        float u = smooth01((t - 8.1f) / 6.2f);
        ufoTravelX = lerp(70.0f, 22.0f, u) + sinf(t * 1.7f) * 2.0f;
        ufoTravelY = lerp(66.0f, 78.0f, u) + sinf(t * 0.9f) * 1.2f;
        drawCloud(24, 73, 1.15f, 0.88f, 0.91f, 0.93f);
        drawUFO(ufoTravelX, ufoTravelY);
        if (t > 13.2f)
            drawCloud(23, 78, 1.38f + (t - 13.2f) * 0.08f, 0.86f, 0.89f, 0.91f);
    }

    if (t > 9.4f && t < 12.3f)
    {
        float pulse = smooth01((t - 9.4f) / 0.8f) * (1.0f - smooth01((t - 11.5f) / 0.8f));
        drawAlphaRect(-100, -100, 200, 200, 0.02f, 0.03f, 0.05f, 0.16f * pulse);
    }

    float finalFade = smooth01((t - 14.8f) / 2.5f);
    if (finalFade > 0.0f)
        drawAlphaRect(-100, -100, 200, 200, 0.0f, 0.0f, 0.0f, finalFade);
}
// ============================================================
//  ORIGINAL: mouse callback
// ============================================================
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

// ============================================================
//  YOUR PART: keyboard callback
// ============================================================
void keyboard(unsigned char key, int x, int y)
{
    /* ENTER is ASCII code 13 in GLUT keyboard input.
       R reset returns all animation variables to startup values. */
    if (key == 13) // ENTER key starts Mars travel.
    {
        if (isSpaceScene && !isMarsScene && !isTravelScene) // Only from space.
        {
            isTravelScene      = true;   // Activate travel scene.
            travelTimer        = 0.0f;   // Restart travel timing.
            marsApproachSize   = 5.0f;   // Mars starts small.
            rocketTravelX      = 0.0f;   // Clear rocket sway.
            rocketTravelY      = -75.0f; // Start rocket low.
        }
    }

    if (key == 'r' || key == 'R') // Restart everything.
    {
        fanAngle = 0.0f; clockAngle = 0.0f; mouseX = -120.0f; // Reset room motion.
        lampOn = false; isAsleep = false; isDreaming = false; // Reset room states.
        sleepTimer = 0.0f; zoomFactor = 1.0f; // Restart dream timing.
        isSpaceScene = false; starsInitialized = false; // Return to room.
        astroYOffset = 0.0f; astroRotation = 0.0f; // Reset astronaut pose.
        mercuryAngle = 0.0f; venusAngle = 0.0f; earthAngle = 0.0f; // Reset orbits.
        rocketY = -120.0f; // Reset space rocket.
        isTravelScene = false; marsApproachSize = 5.0f; travelTimer = 0.0f; // Reset travel.
        isMarsScene = false; marsTransition = false; // Disable Mars.
        marsPhase = 0; marsTimer = 0.0f; marsPhaseTimer = 0.0f; // Reset Mars story.
        alienAppear = 0.0f; girlWalkX = 8.0f; escapeRocketY = -50.0f; warningAlpha = 0.0f; // Reset Mars actors.

        // Reset dream/school variables
        isDreamWakeup = false;      // Disable wake-up scene.
        isSchoolScene = false;      // Disable school scene.
        postMarsWaitTimer = 0.0f;   // Clear post-warning wait.
        dreamWakeupTimer = 0.0f;    // Restart wake-up timer.
        schoolSceneTimer = 0.0f;    // Restart school timer.
        girlBlinkCount = 0;         // Clear blink count.
        girlBlinkTimer = 0.0f;      // Clear blink timing.
        girlEyesClosed = false;     // Open eyes.
        girlLookingUp = false;      // Reset reaction.
        ufoTravelX = 100.0f;        // Reset UFO x.
        ufoTravelY = 65.0f;         // Reset UFO y.
        stopFinalSuspenseSound();   // Stop final audio.
    }
}

// ============================================================
//  display()
// ============================================================
void display()
{
    /* display selects exactly one scene each frame.
       The if-else chain prevents scene overlap.
       Double buffering avoids flicker during animation. */
    glClear(GL_COLOR_BUFFER_BIT); // Clear previous frame pixels.
    glMatrixMode(GL_MODELVIEW);   // Use object transform matrix.
    glLoadIdentity();             // Reset transforms for this frame.

    // ── Original Scenarios: Room → Space → Travel → Mars → Aliens → Warning
    if (isTravelScene) // Travel has highest priority.
    {
        drawTravelScene(); // Draw rocket-to-Mars travel.
    }
    else if (isMarsScene) // Then Mars story.
    {
        drawMarsScene(); // Draw current Mars phase.
    }
    // ── NEW: Dream Wake-up & School (AFTER original scenarios end)
    else if (isDreamWakeup) // Then wake-up scene.
    {
        drawDreamWakeupScreen(); // Draw stable wake-up.
    }
    else if (isSchoolScene) // Then school ending.
    {
        drawSchoolSceneScreen(); // Draw UFO reveal.
    }
    // ── Original space scene ───────────────────────────────
    else if (isSpaceScene) // Otherwise space dream.
    {
        glBegin(GL_QUADS);
            glColor3f(0.1f, 0.0f, 0.2f);
            glVertex2f(-100, -100); glVertex2f(100, -100);
            glColor3f(0.0f, 0.0f, 0.3f);
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
        for (int i = 0; i < 300; i++)
            drawCircle(starX[i], starY[i], 0.3f, 5, 1.0f, 1.0f, 1.0f);

        drawCircle(-70, 0, 20, 50, 1.0f, 0.7f, 0.0f);

        glPushMatrix();
        glTranslatef(-70, 0, 0); glRotatef(mercuryAngle, 0, 0, 1);
        drawCircle(30, 0, 3, 20, 0.7f, 0.5f, 0.3f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-70, 0, 0); glRotatef(venusAngle, 0, 0, 1);
        drawCircle(45, 0, 5, 20, 0.9f, 0.6f, 0.2f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-70, 0, 0); glRotatef(earthAngle, 0, 0, 1);
        drawCircle(65, 0, 6, 20, 0.2f, 0.4f, 0.8f);
        glPushMatrix();
        glTranslatef(65, 0, 0); glRotatef(earthAngle * 2, 0, 0, 1);
        drawCircle(10, 0, 1.5f, 15, 0.8f, 0.8f, 0.8f);
        glPopMatrix();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-70, 0, 0); glRotatef(earthAngle * 0.5f, 0, 0, 1);
        drawCircle(85, 0, 5, 20, 0.8f, 0.3f, 0.1f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(60, rocketY, 0);
        drawRect(-5, 0, 10, 25, 0.9f, 0.9f, 0.9f);
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-5, 25); glVertex2f(5, 25); glVertex2f(0, 35);
        glEnd();
        glBegin(GL_TRIANGLES);
            glVertex2f(-5, 0); glVertex2f(-12, 0); glVertex2f(-5, 8);
            glVertex2f( 5, 0); glVertex2f( 12, 0); glVertex2f( 5, 8);
        glEnd();
        drawCircle(0, 15, 3, 20, 0.1f, 0.2f, 0.5f);
        float spaceFlame = (sinf(sleepTimer * 16.0f) + 1.0f) * 0.5f;
        glColor3f(1.0f, 0.35f + spaceFlame * 0.35f, 0.0f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-4, 0); glVertex2f(4, 0); glVertex2f(0, -10.0f - spaceFlame * 8.0f);
        glEnd();
        glColor3f(1.0f, 0.90f, 0.20f);
        glBegin(GL_TRIANGLES);
            glVertex2f(-2, 0); glVertex2f(2, 0); glVertex2f(0, -6.0f - spaceFlame * 5.0f);
        glEnd();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, astroYOffset, 0);
        glRotatef(astroRotation, 0, 0, 1);
        drawRect(-12, -25, 24, 30, 0.7f, 0.7f, 0.7f);
        drawRect(-10, -30, 20, 35, 0.9f, 0.9f, 0.9f);
        glPushMatrix(); glTranslatef(-10, 0, 0);
        glRotatef(20 + sinf(sleepTimer * 2) * 10, 0, 0, 1);
        drawRect(-10, -5, 15, 10, 0.9f, 0.9f, 0.9f); glPopMatrix();
        glPushMatrix(); glTranslatef(10, 0, 0);
        glRotatef(-20 - sinf(sleepTimer * 2) * 10, 0, 0, 1);
        drawRect(0, -5, 15, 10, 0.9f, 0.9f, 0.9f); glPopMatrix();
        glPushMatrix(); glTranslatef(-5, -30, 0);
        drawRect(-4, -15, 8, 15, 0.9f, 0.9f, 0.9f); glPopMatrix();
        glPushMatrix(); glTranslatef(5, -30, 0);
        drawRect(-4, -15, 8, 15, 0.9f, 0.9f, 0.9f); glPopMatrix();
        drawCircle(0, 15, 12, 30, 1.0f, 1.0f, 1.0f);
        drawCircle(0, 15,  9, 30, 0.1f, 0.2f, 0.5f);
        drawCircle(-3, 18, 2, 20, 1.0f, 1.0f, 1.0f);
        glPopMatrix();

        glColor3f(1.0f, 1.0f, 0.3f);
        drawText(-55, -90, "Press ENTER to fly to Mars!", GLUT_BITMAP_HELVETICA_18);
    }
    else
    {
        // ---------- Original Room Scene ----------
        if (zoomFactor > 1.0f)
        {
            glTranslatef(12, 65, 0);
            glScalef(zoomFactor, zoomFactor, 1.0f);
            glTranslatef(-12, -65, 0);
        }

        if (lampOn)
            drawRect(-100, -20, 200, 120, 0.53f, 0.81f, 0.98f);
        else
            drawRect(-100, -20, 200, 120, 0.0f, 0.2f, 2.0f);

        drawRect(-75, 40, 25, 30, 0.3f, 0.2f, 0.1f);
        drawRect(-72, 43, 19, 24, 0.9f, 0.9f, 1.0f);
        drawCircle(-60, 58, 4, 20, 1.0f, 0.9f, 0.0f);
        drawRect(-72, 43, 19, 8, 0.2f, 0.6f, 0.2f);
        drawRect(50, 40, 25, 30, 0.3f, 0.2f, 0.1f);
        drawRect(53, 43, 19, 24, 1.0f, 0.9f, 0.8f);
        drawCircle(63, 50, 6, 20, 1.0f, 0.4f, 0.0f);
        drawRect(53, 43, 19, 5, 0.4f, 0.2f, 0.1f);

        drawRect(-100, -100, 200, 80, 1.0f, 0.5f, 0.0f);

        drawRect(-1, 80, 2, 20, 0.2f, 0.2f, 0.2f);
        glPushMatrix();
        glTranslatef(0, 80, 0);
        glRotatef(fanAngle, 0, 0, 1);
        drawCircle(0, 0, 4, 20, 0.3f, 0.3f, 0.3f);
        drawRect(-25, -2, 20, 4, 0.4f, 0.2f, 0.1f);
        drawRect(5,   -2, 20, 4, 0.4f, 0.2f, 0.1f);
        glPopMatrix();

        drawRect(-5, -60, 4, 35, 0.3f, 0.15f, 0.05f);
        drawRect(-5, -60, 4, 35, 0.3f, 0.15f, 0.05f);
        drawRect(-35, -30, 40, 10, 0.4f, 0.2f, 0.1f);

        drawRect(-25, -60, 8, 39, 1.0f, 0.8f, 0.6f);
        drawRect(-13, -60, 8, 39, 1.0f, 0.8f, 0.6f);
        drawRect(-27, -65, 10, 5, 0.5f, 0.0f, 0.0f);
        drawRect(-14, -65, 10, 5, 0.5f, 0.0f, 0.0f);

        drawRect(-65, -80, 8, 60, 0.36f, 0.18f, 0.05f);
        drawRect(57,  -80, 8, 60, 0.36f, 0.18f, 0.05f);
        drawRect(-80, -24, 160, 17, 0.54f, 0.27f, 0.07f);
        drawRect(-80, -24, 160,  4, 0.44f, 0.22f, 0.05f);

        if (lampOn) {
            drawCircle(-50, 18, 3, 20, 1.0f, 1.0f, 0.8f);
            glBegin(GL_TRIANGLES);
                glColor3f(1.0f, 1.0f, 0.7f); glVertex2f(-50, 15);
                glColor3f(1.0f, 1.0f, 0.4f);
                glVertex2f(-85, -7); glVertex2f(-15, -7);
            glEnd();
        }
        drawRect(-60, -7, 20, 4, 0.2f, 0.2f, 0.2f);
        if (lampOn) drawCircle(-50, -5, 1.5f, 20, 1.0f, 1.0f, 0.0f);
        else        drawCircle(-50, -5, 1.5f, 20, 0.4f, 0.4f, 0.4f);
        glColor3f(0.8f, 0.1f, 0.1f);
        glBegin(GL_QUADS);
            glVertex2f(-65, 15); glVertex2f(-35, 15);
            glVertex2f(-42, 28); glVertex2f(-58, 28);
        glEnd();
        drawRect(-51, -3, 2, 18, 0.3f, 0.3f, 0.3f);

        glPushMatrix();
        if (isAsleep) glTranslatef(0, -6, 0);

        drawCircle(-15, 22, 11, 30, 0.1f, 0.1f, 0.1f);
        drawCircle(-20, 15, 9,  30, 0.1f, 0.1f, 0.1f);
        drawCircle(-10, 15, 9,  30, 0.1f, 0.1f, 0.1f);
        drawRect(-27.5f, 1, 25, 17, 0.1f, 0.1f, 0.1f);

        glColor3f(0.8f, 0.3f, 0.3f);
        glBegin(GL_POLYGON);
            glVertex2f(-35, -10); glVertex2f(-25, 5);
            glVertex2f( -5,   5); glVertex2f(  7,-10);
        glEnd();
        drawRect(-35.0f, -12.0f, 41.0f, 5.0f, 1.0f, 0.8f, 0.6f);
        drawCircle(-15, 4,  4, 20, 1.0f, 0.8f, 0.6f);
        drawCircle(-15, 15,10, 30, 1.0f, 0.8f, 0.6f);
        drawCircle(-15,-10, 5, 10, 1.0f, 0.8f, 0.6f);

        if (isAsleep) {
            glColor3f(0.0f, 0.0f, 0.0f);
            glLineWidth(2.0f);
            glBegin(GL_LINES);
                glVertex2f(-19.5f,16.5f); glVertex2f(-16.5f,16.5f);
                glVertex2f(-13.5f,16.5f); glVertex2f(-10.5f,16.5f);
            glEnd();
            drawCircle(-15, 10, 1, 10, 0.0f, 0.0f, 0.0f);
            glBegin(GL_LINES);
                glVertex2f(-20,19); glVertex2f(-17,19);
                glVertex2f(-13,19); glVertex2f(-10,19);
            glEnd();
        } else {
            drawCircle(-18,17, 1.5f,10,1.0f,1.0f,1.0f);
            drawCircle(-12,17, 1.5f,10,1.0f,1.0f,1.0f);
            drawCircle(-18,16.5f,0.8f,10,0.0f,0.0f,0.0f);
            drawCircle(-12,16.5f,0.8f,10,0.0f,0.0f,0.0f);
            glColor3f(0.8f,0.2f,0.2f);
            glLineWidth(2.0f);
            glBegin(GL_LINES);
                glVertex2f(-18,11); glVertex2f(-15,12);
                glVertex2f(-15,12); glVertex2f(-12,11);
                glColor3f(0.0f,0.0f,0.0f);
                glVertex2f(-21,20); glVertex2f(-17,22);
                glVertex2f(-14,22); glVertex2f(-10,20);
            glEnd();
        }
        glPopMatrix();

        glColor3f(0.6f, 0.3f, 0.2f);
        glBegin(GL_QUADS);
            glVertex2f(50,8); glVertex2f(70,8);
            glVertex2f(66,-8); glVertex2f(54,-8);
        glEnd();
        drawRect(59, 8, 2, 15, 0.13f, 0.35f, 0.13f);
        drawCircle(60, 23, 10, 20, 0.13f, 0.55f, 0.13f);
        drawCircle(52, 18,  7, 20, 0.13f, 0.45f, 0.13f);
        drawCircle(68, 18,  7, 20, 0.13f, 0.45f, 0.13f);

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
        glBegin(GL_TRIANGLES);
            glVertex2f(25,0); glVertex2f(20,-8); glVertex2f(30,-8);
        glEnd();

        drawRect(-75, -7, 22, 3, 0.2f, 0.3f, 0.7f);
        drawRect(-74, -4, 22, 3, 0.3f, 0.6f, 0.3f);
        drawRect(-76, -1, 22, 3, 0.8f, 0.4f, 0.2f);
        drawRect(-75,  2, 22, 3, 0.8f, 0.7f, 0.2f);
        drawRect(-76,-10, 22, 3, 0.2f, 0.1f, 0.7f);
        drawRect(-75,-13, 22, 3, 0.2f, 0.3f, 0.1f);

        drawCircle(mouseX,    -88, 3,   20, 0.6f, 0.3f, 0.1f);
        drawCircle(mouseX+3,  -88, 1.8f,20, 0.6f, 0.3f, 0.1f);
        drawCircle(mouseX+2,  -86, 1,   10, 0.7f, 0.5f, 0.3f);
        glColor3f(0.4f, 0.4f, 0.4f);
        glBegin(GL_LINES);
            glVertex2f(mouseX-3,-88); glVertex2f(mouseX-8,-86);
        glEnd();

        if (isDreaming) {
            if (sleepTimer > 8.0f)  drawCircle(-5, 25, 1.5f,20,1.0f,1.0f,1.0f);
            if (sleepTimer > 8.5f)  drawCircle( 0, 30, 2.5f,20,1.0f,1.0f,1.0f);
            if (sleepTimer > 9.0f)  drawCircle( 5, 35, 3.0f,20,1.0f,1.0f,1.0f);
            if (sleepTimer > 9.0f)  drawCircle( 5, 45, 5.0f,20,1.0f,1.0f,1.0f);
            if (sleepTimer > 9.5f) {
                drawCircle(12,65,10,30,1.0f,1.0f,1.0f);
                drawCircle(20,67, 9,30,1.0f,1.0f,1.0f);
                drawCircle( 5,67, 9,30,1.0f,1.0f,1.0f);
                drawCircle(12,72, 8,30,1.0f,1.0f,1.0f);
            }
        }
    }

    glutSwapBuffers(); // Show completed back buffer.
}
// ============================================================
//  timerFunc() - ORIGINAL SCENES + NEW DREAM/SCHOOL LOGIC
// ============================================================
void timerFunc(int)
{
    /* Timer runs animation logic about 60 times per second.
       1000/60 milliseconds is about 16.67 ms per frame.
       dt = 1/60 means one frame of simulated seconds. */
    glutPostRedisplay(); // Request display() redraw.
    glutTimerFunc(1000/60, timerFunc, 0); // Schedule next 60fps tick.

    // ========== ORIGINAL SCENARIOS ==========
    if (isTravelScene)
    {
        travelTimer += 1.0f / 60.0f; // Advance travel time.
        marsApproachSize += 0.38f;   // Enlarge Mars each frame.
        rocketTravelX = sinf(travelTimer * 1.5f) * 4.0f; // Sway rocket.

        if (marsApproachSize >= 82.0f) // Mars fills screen.
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
            escapeRocketY   = -50.0f;
            warningAlpha    = 0.0f;
        }
        return;
    }

    if (isMarsScene)
    {
        const float dt = 1.0f / 60.0f; // Seconds per frame.
        marsTimer += dt;              // Advance total Mars time.
        marsPhaseTimer += dt;         // Advance current phase time.
        alienBobY  = sinf(marsTimer * 3.0f) * 3.0f; // Alien bob.

        /* Phase 0: rocket lands on Mars.
           Driven by marsRocketLandY decreasing each frame.
           Next phase triggers when y reaches -24. */
        if (marsPhase == 0)
        {
            marsRocketLandY -= 55.0f * dt;
            if (marsRocketLandY <= -24.0f)
            {
                marsRocketLandY = -24.0f;
                marsPhase = 1;
                marsPhaseTimer = 0.0f;
                girlWalkX = 8.0f;
                alienAppear = 1.0f;
            }
        }

        /* Phase 1: astronaut girl walks left.
           Driven by girlWalkX moving toward -38.
           Next phase waits until timer exceeds 3.2 seconds. */
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

        /* Phase 2: aliens are revealed.
           Driven by alienAppear rising toward 1.
           Next phase triggers after 4 seconds. */
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

        /* Phase 3: leader notices astronaut.
           Driven by marsPhaseTimer suspense pause.
           Next phase triggers after 1.4 seconds. */
        if (marsPhase == 3 && marsPhaseTimer > 1.4f)
        {
            marsPhase = 4;
            marsPhaseTimer = 0.0f;
        }

        /* Phase 4: aliens prepare and aim lasers.
           Driven by marsPhaseTimer attack buildup.
           Next phase triggers after 3 seconds. */
        if (marsPhase == 4 && marsPhaseTimer > 3.0f)
        {
            marsPhase = 5;
            marsPhaseTimer = 0.0f;
            escapeRocketY = -50.0f;
        }

        /* Phase 5: escape rocket launches upward.
           Driven by girlWalkX, then escapeRocketY.
           Next phase triggers when rocket y exceeds 110. */
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
                postMarsWaitTimer = 0.0f;  // Start wait timer after Mars ends
            }
        }

        /* Phase 6: emergency warning transmission.
           Driven by warningAlpha and postMarsWaitTimer.
           Next scene triggers after 5 seconds. */
        if (marsPhase == 6)
        {
            if (warningAlpha < 1.0f)
                warningAlpha += 0.65f * dt;

            // NEW: Wait 5 seconds after warning, then start dream wake-up
            postMarsWaitTimer += dt;
            if (postMarsWaitTimer >= 5.0f)
            {
                isMarsScene = false;
                isDreamWakeup = true;
                dreamWakeupTimer = 0.0f;
                girlBlinkTimer = 0.0f;
                girlBlinkCount = 0;
                girlEyesClosed = false;
            }
        }

        return;
    }

    // ========== NEW SCENARIOS: DREAM WAKE-UP & SCHOOL ==========
    if (isDreamWakeup)
    {
        dreamWakeupTimer += 1.0f / 60.0f;
        girlBlinkTimer += 1.0f / 60.0f;

        // Blink logic: 0.6 sec per blink cycle
        if (girlBlinkTimer >= 0.6f && girlBlinkCount < 2)
        {
            girlBlinkCount++;
            girlBlinkTimer = 0.0f;
            girlEyesClosed = false;
        }

        // Eyes close/open animation
        if (girlBlinkCount < 2)
        {
            if (girlBlinkTimer < 0.3f)
                girlEyesClosed = true;  // Closing
            else
                girlEyesClosed = false; // Opening
        }

        // After the wake-up settles, transition into ordinary morning life.
        if (dreamWakeupTimer >= 7.8f)
        {
            isDreamWakeup = false;
            isSchoolScene = true;
            schoolSceneTimer = 0.0f;
            girlLookingUp = false;
            girlSchoolX = -82.0f;
            girlSchoolY = -41.0f;
        }

        return;
    }

    if (isSchoolScene)
    {
        schoolSceneTimer += 1.0f / 60.0f;
        if (schoolSceneTimer > 8.2f)
            playFinalSuspenseSound();
        if (schoolSceneTimer > 17.4f)
            stopFinalSuspenseSound();
        return;
    }

    // ========== ORIGINAL ROOM ANIMATION ==========
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
        rocketY      += 0.8f;
        if (rocketY > 120.0f) rocketY = -120.0f;
    }
}

// ============================================================
//  init() and main()
// ============================================================
void init()
{
    /* Initialize random seed, background, and 2D projection.
       gluOrtho2D maps screen coordinates directly to world units.
       This project uses -100 to 100 for both axes. */
    srand(time(NULL)); // Seed rand() using current time.
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f); // Default sky clear color.
    glDisable(GL_DITHER); // Disable color dithering for stable flat colors.
    glMatrixMode(GL_PROJECTION); // Select camera/projection matrix.
    glLoadIdentity(); // Reset projection to identity.
    gluOrtho2D(-100, 100, -100, 100); // Set 2D world bounds.
    glMatrixMode(GL_MODELVIEW); // Return to object transform matrix.
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 800);
    glutInitWindowPosition(100, 80);
    glutCreateWindow("Animated Room - Mars Adventure - School Dream");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timerFunc, 0);
    glutMainLoop();
    return 0;
}
