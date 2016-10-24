#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#define BUBBLE_WIDTH 100.0f
#define BUBBLE_HEIGTH 100.0f
#define MAX_PARTICLES 1000

GLuint Load_transparentpic(const char* file_name, int alpha);
void Particles_init(void);
void ParticlesRender(void);

#endif  //_PARTICLE_H_
