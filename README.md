# 🧠 FloppyOS

FloppyOS is a minimal hobby operating system written in C/C++ with a bit of assembly.  
It boots from GRUB, enters 32-bit protected mode, and prints to the VGA text buffer.  
This project is purely for learning how an OS works under the hood.

---

## 🚀 Features (Current)

- Boots using GRUB (Multiboot)
- 32-bit kernel written in C/C++
- Minimal assembly for entry
- Text mode output (VGA `0xB8000`)
- Runs on QEMU or VirtualBox

---

## 🛠️ Tech Used

- **C / C++ (freestanding)**
- **NASM**
- **GRUB (Multiboot)**
- **LD (linker script)**
- **QEMU** for testing

---

## ▶️ How to Build & Run

### 🔧 Build

```bash
make
