# FloppyOS 🧵

FloppyOS is a tiny hobby operating system that boots from a floppy image, switches to 32-bit protected mode, and prints to the VGA text buffer.

It’s not meant to be “usable”; it’s a playground to learn how an OS actually boots and runs on bare metal.

---

## ✨ Current Features

* **Bootloader**: Boots from a floppy image (16-bit real mode).
* **Protected Mode**: Switches to 32-bit Protected Mode.
* **Kernel**: Minimal C kernel compiled with Open Watcom.
* **VGA Driver**: Prints directly to VGA text buffer at `0xB8000`.
* **Build System**: Automates the build process with `Make` and custom tools.
* **Simulation**: Runs easily in QEMU or Bochs.

---

## 🧱 Tech Stack

| Component | Tool |
| :--- | :--- |
| **OS Language** | Assembly (boot), C (kernel) |
| **C Compiler** | Open Watcom v2 |
| **Assembler** | NASM |
| **Emulator** | QEMU / Bochs |
| **Build** | Make + custom FAT tools |

---

## 📁 Project Structure

```text
FloppyOS/
├── src/            # Kernel + boot/low-level code (C + ASM)
├── tools/
│   └── fat/        # Helper tools for building the FAT disk image
├── test.img        # Sample floppy image (for immediate testing)
├── test.txt        # Sample text file for filesystem testing
├── bochs_config    # Configuration file for Bochs
├── Makefile        # Main build script
├── run.sh          # Helper script to run the OS in QEMU
├── debug.sh        # Helper script to debug the OS in Bochs
└── README.md       # This documentation
````

-----

## 🔧 Requirements

### Software Needed (Linux / Ubuntu)

```bash
sudo apt update
sudo apt install nasm make qemu-system-x86 bochs bochs-sdl
```

### Open Watcom v2

FloppyOS relies on **Open Watcom v2** for C compilation because of its distinct calling conventions suitable for this OS structure.

The `Makefile` defaults to:

```makefile
WATCOM = /usr/bin/watcom
CC     = $(WATCOM)/binl/wcc
```

If your installation is different (e.g., `/opt/watcom`), you can either:

1.  Edit the `Makefile` paths.
2.  Create a symlink: `sudo ln -s /opt/watcom /usr/bin/watcom`

-----

## ▶️ Build & Run

### 🏗️ Build

Compile the bootloader, kernel, and generate the floppy image:

```bash
make
```

### 🚀 Run (QEMU)

To boot the OS in QEMU:

```bash
make run
# OR manually:
qemu-system-i386 -fda test.img
```

### 🐞 Debug (Bochs)

To run with the Bochs debugger (using the included `bochs_config`):

```bash
./debug.sh
```

-----

## 🧠 Learning Outcomes

  * BIOS boot process on x86
  * Writing a floppy bootloader (Boot Sector)
  * Real Mode (16-bit) → Protected Mode (32-bit) transition
  * Freestanding C programming (no `libc`)
  * VGA text mode memory manipulation (`0xB8000`)
  * Manual OS image construction

-----

## 🗺️ Roadmap Ideas

  * [ ] Basic keyboard input driver
  * [ ] Simple terminal/shell
  * [ ] GDT/IDT setup improvements
  * [ ] Paging / Memory Management
  * [ ] FAT12 Filesystem implementation (WIP in `tools/fat`)
  * [ ] Additional drivers (Timer, Disk, etc.)

-----

## 📜 License

This is an educational hobby OS project.
You are free to study, change, experiment, and copy code for learning purposes.
Attribution is appreciated but not required.
