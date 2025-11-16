#ifndef MUSIC_H
#define MUSIC_H

// beat 1/4 = 1.65/4 seconds
#define RIT_SEMIMINIMA 8
#define RIT_MINIMA 16
#define RIT_INTERA 32

#define UPTICKS 1

//Default: 1.65
#define SPEEDUP 1.5

#define TIMERSCALER 1

#define SECOND 0x17D7840 * TIMERSCALER


typedef char BOOL;
#define TRUE 1
#define FALSE 0

typedef enum note_durations
{
	time_semibiscroma = (unsigned int)(SECOND * SPEEDUP / 64.0f + 0.5), // 1/128
	time_biscroma = (unsigned int)(SECOND * SPEEDUP / 32.0f + 0.5), // 1/64
	time_semicroma = (unsigned int)(SECOND * SPEEDUP / 16.0f + 0.5), // 1/32
	time_croma = (unsigned int)(SECOND * SPEEDUP / 8.0f + 0.5), // 1/16
	time_semiminima = (unsigned int)(SECOND * SPEEDUP / 4.0f + 0.5), // 1/4
	time_minima = (unsigned int)(SECOND * SPEEDUP / 2.0f + 0.5), // 1/2
	time_semibreve = (unsigned int)(SECOND * SPEEDUP + 0.5), // 1
} NOTE_DURATION;

typedef enum frequencies
{
	a2b = 5351,	// 103Hz	k=5351 a2b
	b2 = 4500,	// 123Hz	k=4500 b2
	c3b = 4370,	// 127Hz	k)4370 c3b
	c3 = 4240,	// 131Hz	k=4240 c3
	d3 = 3779,	// 147Hz	k=3779 d3
	e3 = 3367,	// 165Hz	k=3367 e3
	f3 = 3175,	// 175Hz	k=3175 f3
	g3 = 2834,	// 196Hz	k=2834 g3
	a3b = 2670, // 208Hz  k=2670 a4b
	a3 = 2525,	// 220Hz	k=2525 a3
	b3 = 2249,	// 247Hz	k=2249 b3
	c4 = 2120,	// 262Hz	k=2120 c4
	d4 = 1890,	// 294Hz	k=1890 d4
	e4 = 1684,	// 330Hz	k=1684 e4
	f4 = 1592,	// 349Hz	k=1592 f4
	g4 = 1417,	// 392Hz	k=1417 g4
	a4 = 1263,	// 440Hz	k=1263 a4
	b4 = 1125,	// 494Hz	k=1125 b4
	c5 = 1062, 	// 523Hz	k=1062 c5
	pause = 0		// DO NOT SOUND
} FREQUENCY;


typedef struct 
{
	FREQUENCY freq;
	NOTE_DURATION duration;
} NOTE;

static const NOTE background_melody[] = {
    {b4, time_semicroma},
    {f4, time_semicroma},
    {d4, time_semicroma},
    {b4, time_croma},
    {f4, time_semicroma},
    {d4, time_semiminima},
    {c4, time_semicroma},
    {c5, time_semicroma},
    {g4, time_semicroma},
    {e4, time_semicroma},
    {c5, time_croma},
    {g4, time_semicroma},
    {e4, time_semiminima},
    {pause, time_semicroma},
    
};

static const NOTE pill_eaten_melody[] = {
    {c5, time_semicroma}, // High note to catch attention (1062 Hz)
    {e4, time_semicroma}, // 330 Hz
    {g4, time_croma},     // 392 Hz, short and cheerful
};

static const NOTE win_melody[] = {
    {c4, time_semicroma},  // 262 Hz
    {e4, time_semicroma},  // 330 Hz
    {g4, time_semicroma},  // 392 Hz
    {c5, time_croma},      // 523 Hz
    {g4, time_semicroma},  // 392 Hz
    {e4, time_semicroma},  // 330 Hz
    {c4, time_croma},      // 262 Hz
    {f4, time_semicroma},  // 349 Hz
    {g4, time_semicroma},  // 392 Hz
    {c5, time_semiminima}, // 523 Hz
    {pause, time_semicroma},
    {c4, time_semicroma},  // 262 Hz
    {e4, time_semicroma},  // 330 Hz
    {g4, time_semicroma},  // 392 Hz
    {c5, time_semiminima}, // 523 Hz, triumphant finish
};

static const NOTE lose_melody[] = {
    {c5, time_semicroma},  // 523 Hz, start on a high note
    {g4, time_semicroma},  // 392 Hz, immediate descent
    {e4, time_semicroma},  // 330 Hz
    {c4, time_croma},      // 262 Hz, slow and somber
    {pause, time_semicroma},
    {d4, time_semicroma},  // 294 Hz
    {e4, time_semicroma},  // 330 Hz
    {f4, time_croma},      // 349 Hz
    {g4, time_semicroma},  // 392 Hz
    {e4, time_croma},      // 330 Hz
    {d4, time_croma},      // 294 Hz
    {c4, time_semiminima}, // 262 Hz, descending finish
};


void playNote(NOTE note);
BOOL isNotePlaying(void);

#endif
/* EOF */
