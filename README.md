# ein

[![CMake](https://github.com/ekmett/ein/actions/workflows/cmake.yml/badge.svg)](https://github.com/ekmett/ein/actions/workflows/cmake.yml)

This is a playground for wide SIMD evaluation.

More to come.

## Install requirements

You'll need a fairly bleeding edge compiler for this.

Currently the intention is to target AVX-512, and any support for something other than that (e.g. Apple Silicon, AVX-2)
will be nominal at best, so make sure your CPU is up to snuff. Build instructions below are for Linux. I'll very likely
take patches that broaden coverage, though.

### ninja 1.12.1

You'll want to upgrade Ninja to a version that supports scanning for C++ modules.

```bash
wget https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-linux.zip
sudo apt-get install -y unzip
unzip ninja-linux.zip -d /usr/local/bin/
sudo chmod +x /usr/local/bin/ninja
```

### clang++-19

Next you'll need a version of clang (and clang-tools) that supports c++23 pretty well. Currently this is clang++-19

```bash
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 19
sudo apt install clang-19 clang-tools-19 clangd-19 lldb-19 lld-19
```

### doxygen 1.12.0

Documentation requires doxygen 1.12.0.

```bash
sudo apt-get uninstall doxygen
wget https://www.doxygen.nl/files/doxygen-1.12.0.linux.bin.tar.gz
tar xvfpz doxygen-1.12.0.linux.bin.tar.gz
cd doxygen-1.12.0/
sudo make install
doxygen --version
```

## Contact Information

Contributions and bug reports are welcome!

Please feel free to contact me through GitHub or on the [\#haskell-lens](https://web.libera.chat/#haskell-lens) or [\#haskell](https://web.libera.chat/#haskell) IRC channel on Libera Chat.

-Edward Kmett
