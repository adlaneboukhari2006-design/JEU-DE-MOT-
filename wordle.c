#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "wordle.h"

Word dict[MAX_WORDS];
int word_count = 0;

/* --- UTILITAIRES --- */

void clean_string(char *s) {
    // Supprime les espaces et met en minuscule
    char *start = s;
    while (*start && isspace((unsigned char)*start)) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);

    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
        s[--len] = '\0';

    for (size_t i = 0; s[i]; i++)
        s[i] = (char)tolower((unsigned char)s[i]);

    // Supprime le BOM UTF-8 si présent
    if (len >= 3 && (unsigned char)s[0] == 0xEF && (unsigned char)s[1] == 0xBB && (unsigned char)s[2] == 0xBF)
        memmove(s, s + 3, strlen(s + 3) + 1);
}

/* --- DICTIONNAIRE --- */

int load_dictionary(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;

    word_count = 0;
    char buf[128];
    while (word_count < MAX_WORDS && fgets(buf, sizeof(buf), file)) {
        clean_string(buf);
        if (strlen(buf) == WORD_LEN) {
            strcpy(dict[word_count].word, buf);
            dict[word_count].active = 1;
            word_count++;
        }
    }
    fclose(file);
    return word_count > 0;
}

void choose_random_word(char *target) {
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = 1;
    }
    if (word_count > 0)
        strcpy(target, dict[rand() % word_count].word);
}

/* --- LOGIQUE WORDLE --- */

void compute_feedback(const char *guess, const char *target, char *feedback) {
    int t_used[WORD_LEN] = {0}, g_used[WORD_LEN] = {0};

    // Premier passage : Verts (G)
    for (int i = 0; i < WORD_LEN; i++) {
        if (guess[i] == target[i]) {
            feedback[i] = 'G';
            t_used[i] = g_used[i] = 1;
        } else {
            feedback[i] = 'X';
        }
    }
    // Second passage : Jaunes (Y)
    for (int i = 0; i < WORD_LEN; i++) {
        if (g_used[i]) continue;
        for (int j = 0; j < WORD_LEN; j++) {
            if (!t_used[j] && guess[i] == target[j]) {
                feedback[i] = 'Y';
                t_used[j] = 1;
                break;
            }
        }
    }
    feedback[WORD_LEN] = '\0';
}

void print_colored_guess(const char *guess, const char *feedback) {
    for (int i = 0; i < WORD_LEN; i++) {
        if (feedback[i] == 'G')
            printf("\x1b[42m\x1b[30m %c \x1b[0m", toupper(guess[i]));
        else if (feedback[i] == 'Y')
            printf("\x1b[43m\x1b[30m %c \x1b[0m", toupper(guess[i]));
        else
            printf("\x1b[100m\x1b[37m %c \x1b[0m", toupper(guess[i]));
    }
    printf("\n");
}

/* --- SOLVEUR --- */

void filter_words(const char *guess, const char *feedback) {
    char test_fb[WORD_LEN + 1];
    int remaining = 0;
    for (int i = 0; i < word_count; i++) {
        if (dict[i].active) {
            // On vérifie si dict[i].word est compatible avec le feedback reçu
            compute_feedback(guess, dict[i].word, test_fb);
            if (strcmp(test_fb, feedback) != 0) {
                dict[i].active = 0; // Élimination
            } else {
                remaining++;
            }
        }
    }
    printf("-> %d mots possibles restants.\n", remaining);
}

int select_next_guess(char *guess) {
    for (int i = 0; i < word_count; i++) {
        if (dict[i].active) {
            strcpy(guess, dict[i].word);
            return 1;
        }
    }
    return 0;
}

int is_valid_word(const char *w) {
    for (int i = 0; i < word_count; i++) {
        if (strcmp(dict[i].word, w) == 0) return 1;
    }
    return 0;
}

/* --- MAIN --- */

int main() {
    char target[WORD_LEN + 1], guess[WORD_LEN + 1], fb[WORD_LEN + 1], input[64];

    if (!load_dictionary("words.txt")) {
        printf("Erreur : Fichier words.txt introuvable.\n");
        return 1;
    }

    printf("=== WORDLE MASTER ===\n1 - Jouer\n2 - Solveur\nChoix : ");
    fgets(input, sizeof(input), stdin);

    if (input[0] == '1') {
        choose_random_word(target);
        for (int a = 1; a <= MAX_TRIES; a++) {
            printf("\nEssai %d/%d : ", a, MAX_TRIES);
            if (!fgets(guess, sizeof(guess), stdin)) break;
            clean_string(guess);

            if (strlen(guess) != WORD_LEN || !is_valid_word(guess)) {
                printf("Mot invalide.\n");
                a--; continue;
            }

            compute_feedback(guess, target, fb);
            print_colored_guess(guess, fb);
            if (strcmp(guess, target) == 0) {
                printf("Félicitations ! 🎉\n");
                return 0;
            }
        }
        printf("Perdu. Le mot était : %s\n", target);
    } 
    else {
        printf("\nMODE SOLVEUR\nEntrez les feedbacks ainsi : G=Vert, Y=Jaune, X=Gris (ex: GXXYX)\n");
        for (int a = 1; a <= MAX_TRIES; a++) {
            if (!select_next_guess(guess)) {
                printf("Plus aucune solution possible.\n");
                break;
            }
            printf("\nSuggestion : %s\nFeedback : ", guess);
            
            if (scanf("%5s", fb) != 1) break;
            // Nettoyage du buffer stdin
            int c; while ((c = getchar()) != '\n' && c != EOF);

            for (int i = 0; fb[i]; i++) fb[i] = toupper(fb[i]);

            if (strcmp(fb, "GGGGG") == 0) {
                printf("Trouvé ! L'ordinateur a gagné. 🤖\n");
                return 0;
            }
            filter_words(guess, fb);
        }
    }
    return 0;
}
