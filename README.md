# FloppyOS 🧵

FloppyOS is a tiny hobby operating system that boots from a floppy image, switches to 32-bit protected mode and prints to the VGA text buffer.  
It’s not meant to be “usable”; it’s a playground to learn how an OS actually boots and runs on bare metal.

---

## ✨ Current Features

- Boots from a floppy image (sample image included in the repo)
- Multistage boot process:
  - 16-bit boot code (floppy)
  - 32-bit C kernel
- Switches to 32-bit Protected Mode
- Prints directly to VGA text buffer at `0xB8000`
- Builds using **Open Watcom** (C) + **NASM** (Assembly)
- Runs in emulator (QEMU / Bochs)

---

## 🧱 Tech Stack

| Component        | Tool |
|------------------|------|
| OS Language       | Assembly (boot), C (kernel) |
| C Compiler        | Open Watcom v2 |
| Assembler         | NASM |
| Emulator / Debug  | QEMU / Bochs |
| Build System      | Make + custom floppy builder |

---

## 📁 Project Structure

FloppyOS/
├── src/ # Kernel + boot/low-level code (C + ASM)
│
├── tools/
│ └── fat/ # Helper tools for building the disk image
│
├── test.img # Sample floppy image (for immediate testing)
├── bochs_config # Configuration file for Bochs
├── Makefile # Main build script
├── run.sh # Run the built OS in QEMU
├── debug.sh # Debug OS in Bochs
└── README.md

yaml
Copy code

---

## 🔧 Requirements

### Software Needed

Install these (Linux / Ubuntu):

```bash
sudo apt update
sudo apt install nasm make qemu-system-x86 bochs bochs-sdl
Open Watcom Requirement
FloppyOS needs Open Watcom v2 as the C compiler.
It must exist on your system.

The Makefile expects Watcom here by default:

swift
Copy code
/usr/bin/watcom/binl/wcc
If yours is installed somewhere else (like /opt/watcom), then edit the top of your Makefile:

make
Copy code
# Edit paths based on your installation
WATCOM = /opt/watcom
CC     = $(WATCOM)/binl/wcc
Or set a symlink to avoid editing:

bash
Copy code
sudo ln -s /opt/watcom /usr/bin/watcom
▶️ Build & Run
🏗️ Build
bash
Copy code
make
This will:

Assemble boot + kernel sources

Link the kernel

Build/update the floppy image

Produce a bootable .img

If you get:

swift
Copy code
make[1]: /usr/bin/watcom/binl/wcc: No such file or directory
→ Fix the Watcom path as shown above.

▶️ Run in QEMU
bash
Copy code
make run
If that fails, manually:

bash
Copy code
qemu-system-i386 -fda test.img
(Replace with your own build image if different.)

🐞 Debug in Bochs (optional)
Use the provided Bochs config:

bash
Copy code
./debug.sh
If Bochs can’t find the floppy, open bochs_config and update the path.

🧠 Learning Outcomes
From this project you can learn:

How BIOS boot happens on x86

How a floppy bootloader works

How to switch from 16-bit real mode → 32-bit protected mode

How to write freestanding C (without libc)

How to write text to VGA memory

How to build a bootable OS image manually

🗺️ Roadmap Ideas
 Implement basic keyboard input

 Add a basic terminal/shell

 GDT/IDT setup improvements

 Paging / Memory Manager

 Simple filesystem (FAT12 or custom)

 More drivers (timer, disk, etc.)

📜 License
This is an educational hobby OS project.
You are free to study, change, experiment, and copy code for learning purposes.
Attribution is appreciated but not required.

