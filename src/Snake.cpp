// Makeing 2D snake game with OpenGL
// #include <GL/gl.h>  

// - list body
// - int speed
// - direction currentDirection

// + move()
// + grow()
// + checkSelfCollision(): bool

#if defined(_WIN32) || defined(__CYGWIN__)

#include <GL/gl.h>    // Windows

#elif defined(__APPLE__)

#include <OpenGL/gl3.h>  // macOS

#else

#include <GL/gl.h>    // Linux / other Unix

#endif

