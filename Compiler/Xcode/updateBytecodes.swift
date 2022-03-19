#!/usr/bin/swift

import Foundation

let scriptPath = URL(fileURLWithPath: CommandLine.arguments[0], relativeTo: URL(fileURLWithPath: FileManager.default.currentDirectoryPath)).deletingLastPathComponent().path
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

try! output.write(to: URL(fileURLWithPath: bytecodePath), atomically: true, encoding: String.Encoding.utf8)
