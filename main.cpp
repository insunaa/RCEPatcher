#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <filesystem>

const uint32_t PatchOffset = 0x2A7;
const uint8_t PatchValue = 0xC0;

bool IsPatchedExe(const std::string& filePath) {
    std::ifstream stream(filePath, std::ios::binary);
    if (!stream) {
        return false;
    }

    stream.seekg(PatchOffset);
    uint8_t value = 0;
    stream.read(reinterpret_cast<char*>(&value), sizeof(value));

    return value == PatchValue;
}

void PatchExe(const std::string& filePath, const std::string& newPath) {
    std::ifstream src(filePath, std::ios::binary);
    std::ofstream dst(newPath, std::ios::binary);
    dst << src.rdbuf();

    std::fstream stream(newPath, std::ios::in | std::ios::out | std::ios::binary);
    stream.seekp(PatchOffset);
    stream.write(reinterpret_cast<const char*>(&PatchValue), sizeof(PatchValue));
}

std::string GetNewPath(const std::string& filePath) {
    std::filesystem::path path(filePath);
    std::string fileName = path.stem().string();
    std::string directory = path.parent_path().string();
    std::string defaultSeperator = "";
    std::string separator;
    if (!directory.empty())
        separator = std::filesystem::path::preferred_separator;
    return directory + separator + fileName + "_patched.exe";
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cerr << "[ERROR] Input your WoW.exe file path to patch." << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    try {
        if (!std::filesystem::exists(filePath)) {
            std::cerr << "[ERROR] Unable to find file: " << filePath << std::endl;
            return 1;
        }

        if (IsPatchedExe(filePath)) {
            std::cout << "[COMPLETE] File is already patched: " << filePath << std::endl;
            return 0;
        }

        std::string newPath = GetNewPath(filePath);
        if (std::filesystem::exists(newPath)) {
            std::cerr << "[ERROR] File already exists: " << newPath << std::endl;
            return 1;
        }

        PatchExe(filePath, newPath);

        std::cout << "[COMPLETE] Created patched executable: " << newPath << std::endl;
    } catch (const std::exception& exception) {
        std::cerr << "[ERROR] Unexpected: " << exception.what() << std::endl;
    }

    return 0;
}

