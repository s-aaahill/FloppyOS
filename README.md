# FloppyOS

**Tiny hobby operating system that boots from a floppy image, switches to 32-bit protected mode, and Creates Window GUI to VGA memory.**

> This repository exists for learning OS dev basics: real mode → protected mode, freestanding C, custom GCC toolchain, floppy images, and simple hardware interaction.

---

## ✨ Features

* **Bootloader** — 16‑bit real‑mode boot sector
* **Protected Mode** — sets up GDT and switches to 32‑bit mode
* **GDT & IDT** — Global Descriptor Table & Interrupt Descriptor Table setup
* **HAL** — Hardware Abstraction Layer initialization
* **Freestanding Kernel (C + ASM)** — compiled with custom `i686-elf-gcc` toolchain
* **Robust Bootloader** — supports loading large kernels (>64KB) via FAT12
* **GUI System** — Window Manager with **Splash Screen** & **Custom Bitmap Wallpaper**
*   **Interactive Shell** — Basic command-line interface (`help`, `clear`, `echo`, `reboot`)
*   **Applications** — Terminal, Notepad, Calculator, **Snake Game**
*   **Drivers** — PS/2 Keyboard (with **Shift & CapsLock**), Mouse, VGA Graphics
* **Standard Library Support** — subset implementation (printf, puts, memory operations)
* **VGA Text Output** — writes directly to `0xB8000`
* **Framebuffer Graphics** — 800x600 32-bit color support (BGA)
* **Floppy Image Build System** — generates a FAT12-compatible `main_floppy.img`
* **Emulator Support** — run using QEMU or Bochs

---

## 📁 Repository Structure

```
FloppyOS/
├── src/              # Kernel + low-level ASM
│   ├── apps/         # User applications (Terminal, Notepad, Calculator, Snake)
│   ├── boot/         # Bootloader code
│   ├── drivers/      # Hardware drivers (Keyboard, Mouse, VGA)
│   ├── gui/          # Window Manager & Graphics
│   ├── kernel/       # C kernel source
│   │   ├── arch/     # Architecture specific code (GDT, IDT, IO)
│   │   ├── hal/      # Hardware Abstraction Layer
│   │   └── util/     # Utilities
│   ├── shell/        # Command shell
│   └── util/         # Shared utilities
├── tools/            # FAT12 image creation helpers
│   └── fat/
├── toolchain/        # Custom-built i686-elf GCC toolchain
├── build/            # Build artifacts
│   └── main_floppy.img # Generated floppy image
├── bochs_config      # Config for Bochs debugging
├── Makefile          # Build system
├── run.sh            # QEMU run script
├── debug.sh          # Bochs run script
└── README.md         # This file
```

---

## 🔧 Requirements (Linux / Ubuntu)

Install basic build tools:

```bash
sudo apt update
sudo apt install -y nasm make qemu-system-i386 bochs bochs-sdl
```

This project uses **Own built GCC cross-compiler**, built in the `toolchain/` directory.

---

## 🛠 GCC Cross-Compiler (Custom Toolchain)

The Makefile expects binaries at:

```
./toolchain/i686-elf/bin/i686-elf-gcc
./toolchain/i686-elf/bin/i686-elf-ld
```

Set the compiler variables accordingly:

```
CC = ./toolchain/i686-elf/bin/i686-elf-gcc
AS = nasm
LD = ./toolchain/i686-elf/bin/i686-elf-ld
```

Ensure the binaries are executable:

```bash
chmod +x toolchain/i686-elf/bin/*
```

---

## 🚀 Build & Run

### Build the OS

```bash
make
```

This produces the final floppy image:

```
build/main_floppy.img
```

### Run on QEMU

```bash
./run.sh
# or
qemu-system-i386 -fda build/main_floppy.img
```

### Debug with Bochs

```bash
./debug.sh
```

---

## 📝 Development Notes

* This is a **freestanding** kernel → no libc, no OS services.
* VGA memory (`0xB8000`) is used for debug/console text.
* File system helpers inside `tools/fat` allow embedding files into `build/main_floppy.img`.
* If you modify the toolchain location, update paths in the Makefile.

---

## 📌 Roadmap

* Paging and memory management
* FAT12 improvements + file loading
* PIT timer + IRQ handling
* More GUI Applications

---

## 🤝 Contributing

1. Fork the repo
2. Create a feature branch:

   ```bash
   git checkout -b feature/my-change
   ```
3. Test changes using QEMU or Bochs
4. Commit & push, then open a pull request

---

## 📄 License

For educational use. Attribution appreciated but not required.

---

## 📬 Contact

Open an issue on GitHub for questions or discussions.

---

## Quick Start

```bash
make
./run.sh
# or: make run
```
