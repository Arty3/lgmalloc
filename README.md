<h1 align="center">✨ lgmalloc ✨</h1>

<h6 align="center"><em>The 'm' stands for mid</em></h6>

## 📝 Overview

#### Currently WIP

This malloc implementation aims to provide a viable alternative to bulky allocators in a modernized manner.

The allocator is implemented in C.

## 🛠 Building

### 🌐 Platform

#### Currently requires:

- The [GCC](https://gcc.gnu.org/) or [clang](https://clang.llvm.org/) compilers
- Any POSIX standard operating system
- The C99 standard or above 

### ⚙ Configuration

Compile using the following flags for optional configuration:

| Flag | Description |
|:-----|:------------|
| `-DWEAK_LGMALLOC_REPLACE_STDLIB` | Replace the stdlib `malloc(3)` implementation (overrideable) |
| `-DFORCE_LGMALLOC_REPLACE_STDLIB` | Forcefully replace the stdlib `malloc(3)` implementation |
| `-DLGMALLOC_MMAP_THRESHOLD=` | Set the allocator's mmap call threshold as a positive number (currently 256kb)|
| `-DLGMALLOC_MAX_ALLOC_SIZE=` | Set the allocator's maximum allocation size as a positive number (currently `SIZE_MAX / 2`)|

#### See [lgmalloc_config.h](./src/api/lgmalloc_config.h) for more

### 🔨 Compiling

The build system uses [make](https://www.gnu.org/software/make/). You can create your own makefiles or modify the [Makefile](./src/Makefile).

There, you will find compilation target rules for your appropriate build.
You can apply the [configuration](#-configuration) flags to the build system.

## 🚀 Using

## 🧪 Testing

### 💻 Behavior

### 📈 Benchmarks

## 🛠 Design & Details

## ⚠ Limitations

- Only works on POSIX standard operating system
- Only for the [GCC](https://gcc.gnu.org/) or [clang](https://clang.llvm.org/) compilers
- Incompatible with the C89 Standard

## 📃 License
This project uses the `GNU GENERAL PUBLIC LICENSE v3.0` license
<br>
For more info, please find the `LICENSE` file here: [License](LICENSE)