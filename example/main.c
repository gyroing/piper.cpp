#include <stdio.h>
#include <stdlib.h>
#include "piper.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

int main(int argc, char *argv[]) {
    char *modelPath = NULL;
    char *inputText = NULL;

    if (argc == 2) {
        // If there's only one argument, treat it as inputText
        inputText = argv[1];
    } else if (argc == 3) {
        // If there are two arguments, the first is modelPath and the second is inputText
        modelPath = argv[1];
        inputText = argv[2];
    } else {
        fprintf(stderr, "Usage: %s <inputText> or %s <modelPath> <inputText>\n", argv[0], argv[0]);
        return 1;
    }

    // Generate speech
    char *wavFilePath = piper_generate_speech(modelPath, inputText);
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
