#ifndef BUZZER2_H
#define BUZZER2_H

#define DO 262
#define RE 294
#define MI 330
#define FA 349
#define SOL 392
#define LA 440
#define SI 494
#define DO_M 523
#define SILENCIO 1

void initBuzzer(void);
int GetMilis(void);
void resetMilis(void);
void setNota(int f_nota);

#endif
