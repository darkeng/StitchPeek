# StitchPeek 🧵

**StitchPeek** is a lightweight, blazing-fast Windows Shell Extension (Thumbnail Provider) written in C++ that allows you to preview embroidery files (like `.pes`, `.dst`, `.exp`, etc.) natively inside Windows File Explorer.

Powered by the robust [`libembroidery`](https://github.com/Embroidermodder/libembroidery) parsing core, StitchPeek translates raw stitch coordinates directly into a beautifully anti-aliased preview using Windows GDI+, all without needing to open slow external embroidery software!

![StitchPeek Preview](https://github.com/darkeng/StitchPeek/blob/master/assets/preview.png)

## Features

- **Native Explorer Integration:** Uses official COM `IThumbnailProvider` interfaces.
- **Support for Major Formats:** `.pes`, `.dst`, `.exp`, `.jef`, `.vp3`, `.xxx`, `.pec`.
- **Zero Configuration:** Just install and your folders will light up.
- **Non-Admin Installation:** Safe and clean registry writes to `HKEY_CURRENT_USER` meaning no UAC required!

## System Requirements

- Windows 10 or Windows 11 (64-bit)

---

## 🚀 For End Users: Installing

1. Go to the [Releases](#) page and download `StitchPeek_1.0.0_Installer.exe`.
2. Run it! It installs for your user locally in seconds without needing Administrator rights.
3. Open a folder full of embroidery files and set the View to "Large Icons".
4. Enjoy your beautiful thumbnails!

_(To uninstall, simply look up "StitchPeek" in the standard Windows Apps & Features menu, or use the "Uninstall StitchPeek" shortcut in your Start Menu)._

---

## 🛠️ For Developers: Working with the Source

StitchPeek was designed to be highly modular and easily compilable via **CMake** and **Visual Studio**.

### Prerequisites

1. [Git](https://git-scm.com/) installed.
2. [CMake](https://cmake.org/download/) installed (must be available in your PATH).
3. **Visual Studio 2017+** with the "Desktop development with C++" workload installed.

### Git Checkout

When cloning the repository, make sure to fetch the underlying C submodules!

```bash
git clone --recursive https://github.com/darkeng/StitchPeek.git
cd StitchPeek
```

_(If you already cloned it without `--recursive`, run `git submodule update --init`)_

### 1. Compiling the Engine

To cleanly compile the main DLL and the debugging tools, simply run the included utility batch file:

- **`build.bat`**
  _(This script will transparently find your Visual Studio toolchains, invoke CMake natively, and output `StitchPeek.dll` and `test_thumbnail.exe` to `build\Release\`)_

### 2. Local Testing & Registry Injection

During active development, if you wish to inject your freshly rebuilt DLL directly to your host Windows Explorer process:

- **`install_dev.bat`**
  _(WARNING: This script will kill any stalled `explorer.exe` or `dllhost.exe` sessions, delete the active Windows thumbnail cache database, and register your new DLL dynamically to HKCU)._

To completely unregister your development DLL and restore Windows to a pristine, clean environment:

- **`uninstall_dev.bat`**

### 3. Automated Format Testing

StitchPeek intentionally isolates the graphical GDI+ plotting logic from the COM boilerplate into a standalone executable. To verify that all supported embroidery formats render perfectly on your machine, simply run the automated test suite:

- **`test_thumbnail.bat`**
  _(This script iterates through the `test/` directory samples, feeding them to `test_thumbnail.exe` and verifying that the output `.png` images are mathematically correct)._

### 4. Compiling the Final Installer

StitchPeek uses [Inno Setup 6](https://jrsoftware.org/isinfo.php) to bundle the DLL into a professional `.exe` setup file. Once you are ready for a release:

- **`build_installer.bat`**
  _(This script calls the ISCC compiler natively and deposits the final Setup `.exe` into the `out\` directory)_.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
