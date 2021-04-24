//
//  MaPLFile.cpp
//  MaPLCompiler
//
//  Created by Brad Bambara on 3/27/21.
//

#include "MaPLFile.h"

MaPLFile::MaPLFile(std::__fs::filesystem::path scriptFilePath) :
    _rawScriptText("")
{
    if (!scriptFilePath.is_absolute()) {
        scriptFilePath = std::__fs::filesystem::absolute(scriptFilePath);
    }
    _normalizedFilePath = scriptFilePath.lexically_normal();
}

bool MaPLFile::readRawScriptFromDisk() {
    if (!_rawScriptText.empty()) {
        return true;
    }
    std::ifstream inputStream(_normalizedFilePath);
    if (!inputStream) {
        return false;
    }
    std::stringstream stringBuffer;
    stringBuffer << inputStream.rdbuf();
    _rawScriptText = stringBuffer.str();
    return true;
}

std::__fs::filesystem::path MaPLFile::getNormalizedFilePath() {
    return _normalizedFilePath;
}

std::__fs::filesystem::path MaPLFile::getNormalizedParentDirectory() {
    return _normalizedFilePath.parent_path();
}

std::string MaPLFile::getRawScriptText() {
    return _rawScriptText;
}
