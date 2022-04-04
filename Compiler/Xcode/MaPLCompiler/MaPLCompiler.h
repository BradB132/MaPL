//
//  MaPLCompiler.h
//  libMaPLCompiler
//
//  Created by Brad Bambara on 4/3/22.
//

#ifndef MaPLCompiler_h
#define MaPLCompiler_h

#include <filesystem>
#include <map>
#include <string>
#include <vector>

struct MaPLCompileOptions {
    /// If true, the resulting bytecode will include debug information about what
    /// line number is being evaluated and the values of variables as they change.
    /// Setting this value to true increases bytecode bloat and slows runtime speed.
    bool includeDebugBytes = false;
    
    /// The name that is prepended to all symbols in the symbol table.
    std::string symbolsPrefix = "MaPLSymbols";
};

struct MaPLCompileResult {
    /// A header file meant to be included in the host app which runs the compiled script.
    /// The header contains the symbols that the script uses to query the host app.
    std::string symbolTable;
    
    /// A mapping of file paths to the resulting bytecode. If compilation failed, this mapping will be empty.
    std::map<std::filesystem::path, std::vector<u_int8_t>> compiledFiles;
    
    /// A list of error messages. This list is only populated if compilation fails.
    std::vector<std::string> errorMessages;
};

/**
 * Compiles a list of MaPL scripts. It is most efficient to batch scripts together in one compile call.
 *
 * @param scriptPaths A list of paths to all script files that will be compiled. Paths must be absolute.
 * @param options Allows the caller to configure some details of the compilation.
 *
 * @return The results of the compilation. If compilation fails, only the @c errorMessages list will be populated.
 */
MaPLCompileResult compileMaPL(const std::vector<std::filesystem::path> &scriptPaths, const MaPLCompileOptions &options);

#endif /* MaPLCompiler_h */
