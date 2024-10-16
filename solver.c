// Runs the wordle solver -- you can either specify a word for it to search
// towards, like this:
// $ ./solver <secret word>
//
// Or if you don't specify a secret word, then it will supply you with guesses
// and ask for the feedback from a different instance of wordle.

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "search_util.h"

// Returns true if secret word is guessed,
// Also fills in the result array that's used for tracking.
bool score_guess(char *secret, char *guess, char *result) {
  for (int i = 0; i < 5; i++) {
    result[i] = 'x';
  }

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      if (guess[i] == secret[j]) {
        result[i] = 'y';
      }
    }
  }

  for (int i = 0; i < 5; i++) {
    if (guess[i] == secret[i]) {
      result[i] = 'g';
    }
  }
  return strncmp(secret, guess, 5) == 0;
}


// Loads vocabulary and returns the array of strings.
// Uses fopen to read file to load vocabulary
char **load_vocabulary(char *filename, size_t *num_words) {
  char **out = NULL;
  size_t n = 0;
  char buf[10];
  size_t array_size = 1;
  out = (char **)calloc(sizeof(char *), array_size);
  FILE *infile = fopen(filename, "r");
  while (fgets(buf, 10, infile) != NULL) {
    n++;
    if (n >= array_size) {
      array_size *= 2;
      out = (char **)realloc(out, sizeof(char *) * array_size);
    }
    out[n - 1] = strndup(buf, 6);
    out[n - 1][5] = '\0';
  }
  fclose(infile);
  *num_words = n;
  return out;
}

int main(int argc, char **argv) {
  char **vocabulary;
  size_t num_words;
  int num_guesses = 0;

  char *secret = NULL;
  char *guess = NULL;

  char result[10] = {0};
  bool success = false;
  size_t removed;

  if (argc == 2) {
    secret = argv[1];
    if (strlen(secret) != 5) {
      printf("Please make sure your word is exactly 5 letters long! Try again.");
      return 0;
    }
  }
  vocabulary = load_vocabulary("vocabulary.txt", &num_words);
  do {
    free(guess);
    guess = get_guess(vocabulary, num_words);

    if (guess == NULL) {
      printf("ran out of ideas?\n");
      break;
    }
    num_guesses++;
    printf("GUESS #%d: %s\n", num_guesses, guess);

    if (secret != NULL) {
      success = score_guess(secret, guess, result);
    } else {
      bool sensible_result = false;

      while (!sensible_result) {
        printf("please enter result as 5 characters (g,y,x): ");
        fgets(result, 10, stdin);
        result[5] = '\0';
        sensible_result = true;
        for (int i = 0; i < 5; i++) {
          if (result[i] != 'g' && result[i] != 'y' && result[i] != 'x') {
            sensible_result = false;
          }
        }
      }
      success = (strncmp(result, "ggggg", 5) == 0);
    }

    if (!success) {
      // Filtering vocabulary
      for (int i = 0; i < 5; i++) {
        if (result[i] == 'x') {
          char letter = guess[i];
          bool non_gray_elsewhere = false;
          for (int j = 0; j < 5; j++) {
            char other_letter = guess[j];
            if ((j != i) && (other_letter == letter) && (result[i] != 'x')) {
              non_gray_elsewhere = true;
            }
          }
          if (!non_gray_elsewhere) {
            printf("filtering with gray letter: %c\n", guess[i]);
            removed = filter_vocabulary_gray(guess[i], vocabulary, num_words);
            printf("removed %lu words.\n", removed);
          }
        } else if (result[i] == 'y') {
          printf("filtering with yellow letter: %c\n", guess[i]);
          removed = filter_vocabulary_yellow(guess[i], i, vocabulary, num_words);
          printf("removed %lu words.\n", removed);
        } else if (result[i] == 'g') {
          printf("filtering with green letter: %c\n", guess[i]);
          removed = filter_vocabulary_green(guess[i], i, vocabulary, num_words);
          printf("removed %lu words.\n", removed);
        }
      }
    }
  } while (!success);

  if (success) {
    printf("correct! got it in %d guesses!\n", num_guesses);
  } else {
    printf("oh no, could not guess it, the word was outside the vocabulary!\n");
  }

  free(guess);
  free_vocabulary(vocabulary, num_words);

  return 0;
}
