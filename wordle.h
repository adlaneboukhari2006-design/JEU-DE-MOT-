#ifndef WORDLE_H
#define WORDLE_H

#define WORD_LEN 5
#define MAX_WORDS 12000
#define MAX_TRIES 6

typedef struct {
    char word[WORD_LEN + 1];
    int active; // 1 si le mot est encore possible, 0 sinon
} Word;

// Prototypes des fonctions
void clean_string(char *s);
int load_dictionary(const char *filename);
void choose_random_word(char *target);
void compute_feedback(const char *guess, const char *target, char *feedback);
void print_colored_guess(const char *guess, const char *feedback);
void filter_words(const char *guess, const char *feedback);
int select_next_guess(char *guess);
int is_valid_word(const char *w);

#endif
