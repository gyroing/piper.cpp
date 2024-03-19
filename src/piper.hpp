#ifndef PIPER_HPP_
#define PIPER_HPP_

#include <fstream>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include <onnxruntime_cxx_api.h>

#include "phoneme_ids.hpp"
#include "phonemize.hpp"
#include "json.hpp"

using json = nlohmann::json;

namespace piper {

typedef int64_t SpeakerId;

enum PhonemeType { eSpeakPhonemes, TextPhonemes };

struct PhonemizeConfig {
  PhonemeType phonemeType = eSpeakPhonemes;
  std::optional<std::map<Phoneme, std::vector<Phoneme>>> phonemeMap;
  std::map<Phoneme, std::vector<PhonemeId>> phonemeIdMap;

  PhonemeId idPad = 0; // padding (optionally interspersed)
  PhonemeId idBos = 1; // beginning of sentence
  PhonemeId idEos = 2; // end of sentence
  bool interspersePad = true;

  std::string locale = "en-us";
};

struct SynthesisConfig {
  // VITS inference settings
  float noiseScale = 0.667f;
  float lengthScale = 1.0f;
  float noiseW = 0.8f;

  // Audio settings
  int sampleRate = 22050;
  int sampleWidth = 2; // 16-bit
  int channels = 1;    // mono

  // Speaker id from 0 to numSpeakers - 1
  std::optional<SpeakerId> speakerId;

  // Extra silence
  float sentenceSilenceSeconds = 0.2f;
  std::optional<std::map<piper::Phoneme, float>> phonemeSilenceSeconds;
};

struct ModelConfig {
  int numSpeakers;

  // speaker name -> id
  std::optional<std::map<std::string, SpeakerId>> speakerIdMap;
};

struct ModelSession {
  Ort::Session onnx;
  Ort::AllocatorWithDefaultOptions allocator;
  Ort::SessionOptions options;
  Ort::Env env;

  ModelSession() : onnx(nullptr){};
};

struct SynthesisResult {
  double inferSeconds;
  double audioSeconds;
  double realTimeFactor;
};

struct Voice {
  json configRoot;
  PhonemizeConfig phonemizeConfig;
  SynthesisConfig synthesisConfig;
  ModelConfig modelConfig;
  ModelSession session;
};

// Get the path to the share directory
std::filesystem::path get_share_path();

// Convert a path to a C string
char* get_c_str_path(std::filesystem::path datapath);

// True if the string is a single UTF-8 codepoint
bool isSingleCodepoint(std::string s);

// Get the first UTF-8 codepoint of a string
Phoneme getCodepoint(std::string s);

// Get version of Piper
std::string getVersion();

// Load Onnx model and JSON config file
void loadVoice(std::string modelPath,
               std::string modelConfigPath, Voice &voice,
               std::optional<SpeakerId> &speakerId, bool useCuda);

// Phonemize text and synthesize audio
void textToAudio(Voice &voice, std::string text,
                 std::vector<int16_t> &audioBuffer, SynthesisResult &result,
                 const std::function<void()> &audioCallback);

// Phonemize text and synthesize audio to WAV file
void textToWavFile(Voice &voice, std::string text,
                   std::ostream &audioFile, SynthesisResult &result);

} // namespace piper

#endif // PIPER_HPP_
