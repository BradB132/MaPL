# MaPL Tests
The goal of the tests in this directory is to exercise every feature of the MaPL language, and ensure that both the compiled bytecode and runtime behavior stay consistent over time.

### Methodology
Within the `Scripts/` directory are a series of directories which each test a different set of language features. Each directory contains the script under test, as well as several output / log files which are the canonical expected outputs. These outputs are compared with newly-generated outputs each time the testing program runs. Any mis-matches are flagged as errors.