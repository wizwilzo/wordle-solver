#include "search_util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Returns the number of words in which that particular letter occurs
int score_letter(char letter, char **vocabulary, size_t num_words) {
  int ret = 0;
  for (int i = 0; i < (int)(num_words); i++) {
    if (vocabulary[i] == NULL) continue;
    for (int j = 0; j < 5; j++) {
      if (vocabulary[i][j] == letter) {
        ret++;
        break;
      }
    }
  }
  return ret;
}

// The score for a word is the sum of all of the letter scores for unique letters
int score_word(char *word, int *letter_scores) {
  int ret = 0;
  int *contained = calloc(6, sizeof(int));
  for (int i = 0; i < 6; i++) contained[i] = -1;
  
  for (int i = 0; i < 5; i++) {
    int id = (int)(word[i]) - (int)('a');    
    int found = 0;
    for (int j = 0; j < 5; j++) {
      if (contained[j] == id) {
        found = 1;
        break;
      }  
    }

    contained[i] = id;
    if (found) continue;
    ret += letter_scores[id];

  }
  free(contained);
  return ret;
}

// Returns the optimal guess based on our heuristic.
char *get_guess(char **vocabulary, size_t num_words) {
  int letter_scores[26];

  for (int i = 0; i < 26; i++) {
    letter_scores[i] = score_letter('a' + i, vocabulary, num_words);
  }
  char *best_guess = NULL;
  int best_score = 0;
  int score = 0;
  for (int i = 0; i < (int)(num_words); i++) {
    if (vocabulary[i] == NULL) {
      continue;
    }
    score = score_word(vocabulary[i], letter_scores);
    if (score > best_score) {
      best_guess = vocabulary[i];
      best_score = score;
    }
  }
  
  return best_guess ? strdup(best_guess) : NULL;
}

// Filters down the vocabulary based on gray letter
// Returns the number of filtered out words
size_t filter_vocabulary_gray(char letter, char **vocabulary,
                              size_t num_words) {

  int ret = 0;
  for (int i = 0; i < (int)(num_words); i++) {
    if (vocabulary[i] == NULL) continue;

    for (int j = 0; j < 5; j++) {
      if (vocabulary[i][j] == letter) {
        free(vocabulary[i]);
        vocabulary[i] = NULL;
        ret++;
        break;
      }
    }
  }
  return ret;
}

// Filters down the vocabulary based on yellow letter
// Returns the number of filtered out words
size_t filter_vocabulary_yellow(char letter, int position, char **vocabulary,
                                size_t num_words) {
  int ret = 0;
  for (int i = 0; i < (int)(num_words); i++) {
    if (vocabulary[i] == NULL) continue;
    int contained = 0;
    int removed = 0;
    for (int j = 0; j < 5; j++) {
      if (vocabulary[i][j] == letter) {
        if (j == position) {
          free(vocabulary[i]);
          vocabulary[i] = NULL;
          ret++;
          removed = 1;
          break;
        }
        else {
          contained = 1;
        }
      }
    }
    if (removed) continue;
    if (contained == 0) {
      free(vocabulary[i]);
      vocabulary[i] = NULL;
      ret++;

    }

  }
  return ret;
}

// Filters down the vocabulary based on green letter
// Returns the number of filtered out words
size_t filter_vocabulary_green(char letter, int position, char **vocabulary,
                               size_t num_words) {
  int ret = 0;
  for (int i = 0; i < (int)(num_words); i++) {
    if (vocabulary[i] == NULL) continue;
    if (vocabulary[i][position] != letter) {
      free(vocabulary[i]);
      vocabulary[i] = NULL;
      ret++;
    }
  }
  return ret;
}

// Frees array of strings vocabulary
void free_vocabulary(char **vocabulary, size_t num_words) {
  for (size_t i = 0; i < num_words; i++) {
    free(vocabulary[i]);
  }
  free(vocabulary);
}
