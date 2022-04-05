#!/usr/bin/swift

import Foundation

let scriptPath = URL(fileURLWithPath: CommandLine.arguments[0], relativeTo: URL(fileURLWithPath: FileManager.default.currentDirectoryPath)).deletingLastPathComponent().path
let bytecodePath = scriptPath+"/MaPLBytecodeConstants.h"
var output = ""
var byteCode = 0

try! String(contentsOfFile: bytecodePath).components(separatedBy: "\n").forEach { line in
    if !line.contains("MaPLInstruction_") {
        output += "\(line)\n"
        return
    }
    let splitLine = line.components(separatedBy: " ")
    // 4th element because there's a leading 4 spaces.
    output += "    \(splitLine[4]) = \(byteCode),\n"
    byteCode += 1
}
output = output.trimmingCharacters(in: .whitespacesAndNewlines) + "\n"
try! output.write(to: URL(fileURLWithPath: bytecodePath), atomically: true, encoding: String.Encoding.utf8)
