# Install requirements

## Upgrade clang to clang-19

## Upgrade doxygen to 1.12.0

```
sudo apt-get uninstall doxygen
wget https://www.doxygen.nl/files/doxygen-1.12.0.linux.bin.tar.gz
tar xvfpz doxygen-1.12.0.linux.bin.tar.gz 
cd doxygen-1.12.0/
sudo make install
doxygen --version
```
