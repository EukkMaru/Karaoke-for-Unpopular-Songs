// KaraokeTrack.cpp
#include "KaraokeTrack.h"
#include <fstream>

void KaraokeTrack::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    nlohmann::json j;
    file >> j;

    metadata = {
        j["metadata"]["title"],
        j["metadata"]["artist"],
        j["metadata"]["language"]
    };
    std::string directory = filename.substr(0, filename.find_last_of("/\\") + 1);
    metadata.audioFile = directory + metadata.audioFile;

    lyrics.clear();
    for (const auto& lyric : j["lyrics"]) {
        LyricLine line;
        line.startTime = lyric["startTime"];
        line.text.original = lyric["text"]["original"];
        if (lyric["text"].contains("pronunciation"))
            line.text.pronunciation = lyric["text"]["pronunciation"];
        if (lyric["text"].contains("meaning"))
            line.text.meaning = lyric["text"]["meaning"];

        for (const auto& syllable : lyric["syllables"]) {
            line.syllables.push_back({
                syllable["text"],
                syllable["duration"],
                syllable["noteKey"]
            });
        }
        lyrics.push_back(line);
    }
}

void KaraokeTrack::saveToFile(const std::string& filename) {
    nlohmann::json j;
    j["metadata"] = {
        {"title", metadata.title},
        {"artist", metadata.artist},
        {"language", metadata.language},
        {"audioFile", metadata.audioFile}
    };

    j["lyrics"] = nlohmann::json::array();
    for (const auto& lyric : lyrics) {
        nlohmann::json lyricJson;
        lyricJson["startTime"] = lyric.startTime;
        lyricJson["text"]["original"] = lyric.text.original;
        if (lyric.text.pronunciation)
            lyricJson["text"]["pronunciation"] = *lyric.text.pronunciation;
        if (lyric.text.meaning)
            lyricJson["text"]["meaning"] = *lyric.text.meaning;

        lyricJson["syllables"] = nlohmann::json::array();
        for (const auto& syllable : lyric.syllables) {
            lyricJson["syllables"].push_back({
                {"text", syllable.text},
                {"duration", syllable.duration},
                {"noteKey", syllable.noteKey}
            });
        }
        j["lyrics"].push_back(lyricJson);
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }
    file << j.dump(4);  // 4 spaces indent for pretty printing
}