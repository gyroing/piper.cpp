#include <stdio.h>
#include <stdlib.h>
#include "piper.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

int main(int argc, char *argv[]) {
    char *modelPath = "./en_US-curie-high-V2.onnx";
    char *inputText = "Hello world. This is a test of the curie AI voice model.";

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
