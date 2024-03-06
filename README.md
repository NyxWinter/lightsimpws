# Lightsim in C - By someone who can't program 
This is a very simple lightsimulation written in C only using SDL.

## Setting up:

Dependencies:

- SDL2



``` shell
sudo apt install libsdl2-dev
```

then

``` shell
git clone https://github.com/NyxWinter/lightsimpws
cd lightsimpws
make
```

## Running the program

This runs the program with an example 3d model:

``` shell
./project examples/updatedroom.r3f
```

## Making your own 3d models

I was too lazy to implement obj parsing so you'll have to model by hand. You can see examples in the _examples_ folder.
