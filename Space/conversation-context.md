# Conversation Context

Project path: `E:\project\Space`

Main file: `main.cpp`

## Project

C++ OpenGL GLUT animation project with multiple scenes:
- Room scene
- Space scene
- Travel-to-Mars scene
- Mars scene

The Mars scene is controlled with `marsPhase` and updated in `timerFunc()`.

## Current Goal

Keep the project as a short cinematic sci-fi animation:

Exploration -> Discovery -> Suspense -> Detection -> Attack -> Escape -> Warning

The scene should feel like a short animated story, not a simple demo.

## Work Already Done

### Flicker Fixes

- Confirmed double buffering:
  ```cpp
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  ```
- Changed rendering so `glutSwapBuffers()` is used once per frame.
- Replaced binary flame flicker with smooth sinusoidal flame animation.
- Added:
  ```cpp
  glDisable(GL_DITHER);
  ```
- Stars are initialized once using `starsInitialized`.

### Friendly Alien Behavior Removed

Removed friendly interaction from the Mars story:
- No `"HELLO"`
- No `"WELCOME"`
- No friendly speech bubble behavior

Aliens should feel threatening and serious.

## Mars Story Flow

### Phase 0

Rocket lands on Mars.

### Phase 1

Astronaut exits rocket and explores calmly.

Alien group is already present in front of the astronaut.

Threat text appears above the alien group:
- `"We will attack humans"`
- `"Humanity must be destroyed"`
- `"Prepare for invasion"`

### Phase 2

Astronaut stops and listens.

This moment should feel like the astronaut discovered a secret meeting.

### Phase 3

Detection moment.

Leader alien notices the astronaut first.

Detection dialogue:
```text
I think the astronaut knows our secret...
```

Other aliens freeze or react subtly.

### Phase 4

Attack begins after a short delay.

Aliens shoot laser beams using `glBegin(GL_LINES)`.

### Phase 5

Astronaut runs back very fast.

Rocket launches when astronaut reaches it.

### Phase 6

Warning transmission appears:

```text
WARNING: Aliens are planning to attack Earth!
```

## Alien Formation

Added:

```cpp
struct Alien {
    float x, y;
    float bobOffset;
    int type;
    float facingAngle;
    bool isLeader;
};

Alien aliens[5];
```

Formation is fixed and should not be randomized.

Target structure:

```text
        A1
   A2       A3

   A4       A5
```

Rules:
- No overlap
- Minimum spacing around 15-25 world units
- Leader is slightly forward
- Aliens bob subtly and face the group center before detection
- Leader turns toward astronaut first
- All aliens turn hostile during attack

## Important Functions

Relevant functions in `main.cpp`:

- `drawMarsScene()`
- `timerFunc()`
- `drawAlienGroup()`
- `drawLaser()`
- `drawWarningText()`
- `drawAlienThreatText()`
- `initAlienFormation()`

## Build Command

Use:

```powershell
g++ main.cpp -o bin\Debug\Space.exe -lfreeglut -lopengl32 -lglu32
```

Last known compile result: successful.

## Next Session Prompt

Use this prompt next time:

```text
Continue work in E:\project\Space. Read conversation-context.md first. The project is a C++ OpenGL GLUT animation. Keep changes scoped to main.cpp, especially the Mars scene interaction. Preserve the cinematic flow and structured alien formation.
```
