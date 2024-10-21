# ein

This is a playground for wide SIMD evaluation.

## Install requirements

You'll need a fairly bleeding edge compiler for this.

### Upgrade clang to clang-19

```bash
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 19
sudo apt install clang-19 clang-tools-19 clangd-19 lldb-19 lld-19
```

### Upgrade doxygen to 1.12.0

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

Please feel free to contact me through GitHub or on the [#haskell-lens](https://web.libera.chat/#haskell-lens) or [#haskell](https://web.libera.chat/#haskell) IRC channel on Libera Chat.

-Edward Kmett
