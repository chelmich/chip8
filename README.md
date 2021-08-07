# chip8

This emulator is still very much a work in progress.
Some large essential features are missing.

## Usage

To load a ROM add it as an argument when running from the command line, e.g.

```shell
./chip8 awesome_rom.ch8
```

Currently there is no way to load a different ROM once the program is running.

Once a ROM is loaded the emulator can be controlled via the following keys:

| Key | Description |
| --- | --- |
| `Esc` | Quit the program |
| `S` | Step the emulator forward by one CPU cycle |

## Building

The emulator can be built as a standard CMake project.
Simply run the following commands from the main project directory:

```shell
mkdir build
cd build
cmake ..
make
```

This will produce a `chip8` exectuable that you can then run with a ROM of your choice.
