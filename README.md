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
💻 Run (QEMU)
make run


If make run fails, you can manually do:

qemu-system-i386 -cdrom build/FloppyOS.iso

📂 Project Structure
FloppyOS/
│
├── src/             # kernel + drivers
├── build/           # compiled binaries + ISO (ignored by git)
├── Makefile
└── linker.ld

🎯 Goals (Future)

Basic keyboard driver

Simple shell

Memory management (paging)

File system (custom or FAT12)

📜 License

This project is open to learn from — use, modify, and experiment freely.


---

### 💾 Save the file

- Press `Ctrl + O` → Enter
- Press `Ctrl + X`

---

## 🔐 Add, commit, and push to GitHub

```bash
git add README.md
git commit -m "Added README"
git push
