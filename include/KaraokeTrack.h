// KaraokeTrack.h
#pragma once
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

struct Syllable {
    std::string text;
    int duration;
    std::string noteKey;
};

struct LyricLine {
    int startTime;
    struct {
        std::string original;
        std::optional<std::string> pronunciation;
        std::optional<std::string> meaning;
    } text;
    std::vector<Syllable> syllables;
};

struct Metadata {
    std::string title;
    std::string artist;
    std::string language;
    std::string audioFile;
};

class KaraokeTrack {
public:
    Metadata metadata;
    std::vector<LyricLine> lyrics;

    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename);
};