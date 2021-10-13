# TFHEppy

## How to build

```sh
git clone  --recursive https://github.com/kenmaro3/TFHEppy
mkdir TFHEppy/build
cd TFHEppy/build
cmake ..
make
```

## How to run tutrial

```sh
# after build
# shell on git project root
cp tfheppy.cpython-39-x86_64-linux-gnu.so ./tutorial/
cd ./tutorial
pip3 install --upgrade pip
pip3 install -r reqirements.txt
python3 train_iris.py && python3 test_iris.py
```
