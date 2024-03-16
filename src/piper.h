#ifndef PIPER_H
#define PIPER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
   #define EXPORT __declspec(dllexport)
#else
   #define EXPORT __attribute__((visibility("default"))) __attribute__((used))
#endif

// Function to generate speech from text.
// Returns a heap-allocated string with the path to the generated WAV file.
// The caller is responsible for freeing the string.
EXPORT char* generate_speech(const char* modelPath, const char* prompt);

#ifdef __cplusplus
}
#endif

#endif // PIPER_H
