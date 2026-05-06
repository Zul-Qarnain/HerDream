# Her Dream: A Space Adventure

This is a 2D animated story created using C++ and OpenGL. It follows a young girl's dream of an adventure in space, a visit to Mars, and a mysterious encounter with aliens.

## The Story

The narrative unfolds in several parts:

1.  **The Bedroom:** The story begins in a young girl's bedroom. As she falls asleep, a dream bubble appears, and her adventure begins.
2.  **Deep Space:** The dream transports her into outer space, where she navigates through a starfield, passing by planets in our solar system.
3.  **Journey to Mars:** She pilots a rocket, traveling through space on a direct course for Mars.
4.  **Mars Landing:** The girl, now an astronaut, lands her rocket on the surface of Mars and begins to explore the red planet.
5.  **The Alien Encounter:** Her exploration is interrupted by the appearance of mysterious aliens. The initial curiosity turns into a tense standoff as the aliens reveal their hostile intentions and issue a warning of an impending attack on Earth.
6.  **Waking Up:** The intense dream causes the girl to wake up suddenly in her bed.
7.  **A New Day:** The story concludes with the girl at school. As she stands outside, a UFO appears in the sky, leaving the player to wonder if it was all just a dream.

## Features

*   **2D Graphics:** The entire animation is rendered using 2D shapes and transformations.
*   **Multiple Scenes:** The story transitions through various scenes, including a bedroom, outer space, the Martian surface, and a schoolyard.
*   **Character Animation:** The girl, aliens, and other elements have simple animations to bring the story to life.
*   **Sound Effects:** The project includes sound effects for key moments like rocket thrust, laser attacks, and warnings.

## How to Run

This project is built with C++ and relies on the OpenGL and GLUT libraries for graphics. It also uses Windows-specific libraries for sound.

### Dependencies

*   A C++ compiler
*   OpenGL
*   [FreeGLUT](httpg freeglut.sourceforge.net/) or a compatible GLUT library
*   On Windows, you will need to link against `winmm.lib` for sound.

### Building

The project includes a Code::Blocks project file (`Space.cbp`), which can be used to build and run the application on Windows.

1.  Open `Space.cbp` in Code::Blocks.
2.  Ensure you have a compiler and the GLUT development libraries configured.
3.  Build and run the project.

Alternatively, you can compile the project from the command line. For example, using MinGW on Windows:

```bash
g++ main.cpp -o HerDream.exe -lopengl32 -lglu32 -lfreeglut -lwinmm
```

Make sure the `sounds` directory is in the same location as the executable for the sound effects to work.
