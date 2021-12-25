# Surveillant 
simple embedded dashboard written in C++ for linux embedded device, using LVGL graphical library

submodules (yet to be added):
```
lvgl/lvgl -> release/v8.1
lvgl/lv_drivers -> release/v8.1
```

build instructions:
```
cd src
cd libraries
git clone https://github.com/lvgl/lvgl.git --branch=release/v8.1
git clone https://github.com/lvgl/lv_drivers.git --branch=release/v8.1
cd..
cmake -S ./ -B ./build
cd build && make
```

running target:
```
./surveillant
```

