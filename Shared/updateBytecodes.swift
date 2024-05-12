#!/usr/bin/swift

import Foundation

let scriptPath = URL(fileURLWithPath: CommandLine.arguments[0], relativeTo: URL(fileURLWithPath: FileManager.default.currentDirectoryPath)).deletingLastPathComponent().path
let bytecodePath = scriptPath+"/MaPLBytecodeConstants.h"
var output = ""
var byteCode = 0

var bytecodeValues:[String] = []
var insertedInstructions = false

try! String(contentsOfFile: bytecodePath).components(separatedBy: "\n").forEach { line in
    if line.contains("#define MAPL_INSTRUCTION_") {
        let splitLine = line.components(separatedBy: " ")
        bytecodeValues.append(splitLine[1])
        
        output += "#define \(splitLine[1]) \(byteCode)\n"
        byteCode += 1
    } else if line.contains("const MaPLInstruction MaPLInstruction_") {
        if !insertedInstructions {
            insertedInstructions = true
            for string in bytecodeValues {
                let substringIndex = string.index(string.startIndex, offsetBy: 17)
                output += "const MaPLInstruction MaPLInstruction_\(string.suffix(from: substringIndex).lowercased()) = \(string);\n"
            }
        }
    } else {
        output += "\(line)\n"
    }
}
output = output.trimmingCharacters(in: .whitespacesAndNewlines) + "\n"
try! output.write(to: URL(fileURLWithPath: bytecodePath), atomically: true, encoding: String.Encoding.utf8)
