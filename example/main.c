#include <stdio.h>
#include <stdlib.h>
#include "piper.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

int main(int argc, char *argv[]) {
    // Check for correct argument count
    if (argc != 3) {
        fprintf(stderr, "Usage: %s -m <path_to_model>\n", argv[0]);
        return 1;
    }

    // Verify the model path flag
    if (strcmp(argv[1], "-m") != 0) {
        fprintf(stderr, "Expected -m flag\n");
        return 1;
    }

    char *modelPath = argv[2];
    char inputText[1024];

    // Read input text from stdin
    if (fgets(inputText, sizeof(inputText), stdin) == NULL) {
        fprintf(stderr, "Failed to read input text\n");
        return 1;
    }

    // Remove newline character if present
    inputText[strcspn(inputText, "\n")] = 0;

    // Generate speech
    char *wavFilePath = generate_speech(modelPath, inputText);
    if (wavFilePath == NULL) {
        fprintf(stderr, "Failed to generate speech\n");
        return 1;
    }

    // Play the WAV file
#ifdef WIN32
    PlaySound(wavFilePath, NULL, SND_FILENAME);
#else
    char command[1024];
    snprintf(command, sizeof(command), "aplay %s", wavFilePath);
    system(command);
#endif

    // Clean up
    free(wavFilePath);

    return 0;
}
