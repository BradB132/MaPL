#!/usr/bin/swift

import Foundation

let scriptPath = FileManager.default.currentDirectoryPath
let bytecodePath = scriptPath+"/MaPLCompiler/MaPLBytecodeConstants.h"
var output = ""
var byteCode = 1

try! String(contentsOfFile: bytecodePath).components(separatedBy: "\n").forEach { line in
    if !line.contains("#define MAPL_BYTE_") {
        output += "\(line)\n"
        return
    }
    let splitLine = line.components(separatedBy: " ")
    output += "#define \(splitLine[1]) \(byteCode)\n"
    byteCode += 1
}

print(output)
