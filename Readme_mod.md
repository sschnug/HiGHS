# Intro
This fork will be trying to make HiGHS more portable for incorporation into other codebases. This might lead to removal of some unimportant code-parts and additional cleanups.

# Phases

## Phase 1

### Goal
- Compilation without **C++11**!
- No **cmake**-usage -> simple one-line compilation
- Remove **non-MIT** licensed code

### Approach
- Remove problematic code (license)
- Add some boost-headers and adapt code
- Make sure to use C++99? (e.g. tuple-access)

### Status
```g++ *.cpp -o BINARY``` works
with: ```gcc version 5.4.0 20160609 (Ubuntu 5.4.0-6ubuntu1~16.04.9)```

### Unclear / Rough edges
- Is there hidden-usage (+need) of a system-wide boost-install?

### Details

#### Removed stuff
- Removed src/ext/pdqsort
- Removed src/ext/Hash.hpp
- Removed src/CMakeLists.txt
- Removed src/.travis.yml
- Removed src/clang-format
- Removed src/getoptDOTh
- Removed src/highs-config.cmake.in

#### Code changes

##### HApp.h
- made ```void printHelp(std::string execName)``` static

#### HDual.hpp
- def constructor added
- prepared for initializer list usage

#### HDual.cpp
- def constructor with initializer list added

#### HDualRHS.cpp
- template bracketing

#### HModel.hpp
- prepared for initializer list usage

#### HModel.cpp
- initializer lists

#### HPresolve.h
- prepared initializer lists

#### HPresolve.cpp
- added usage of boost
  - ```ext/boost/algorithm/cxx11/none_of.hpp```
  - ```ext/boost/assign.hpp```
- replaced lambda-function with real function
- various changes related to above external boost code

### KktChStep.cpp
- template bracketing
- tuple-access

### KktCheck.cpp
- removed class-var usage in loops -> name lookup of ‘k’ changed for ISO ‘for’ scoping [-fpermissive]

### HMPSIO.cpp
- added include of stdlib

### HinOut.hpp

### HinOut.cpp
- template bracketing
- tuple-access
- ifstream takes a const char*, not a string pre-C++11
