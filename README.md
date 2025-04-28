<h1 align="center">✨ lgmalloc ✨</h1>

<h6 align="center"><em>The 'm' stands for mid</em></h6>

## 📝 Overview

#### Currently WIP

This malloc implementation aims to provide a viable alternative to bulky allocators in a modernized manner.

The allocator is implemented in C.

## 🚀 Using

### 💻 Basic Setup

Include the respective API header in your project:

- **[lgmalloc.h](./src/api/lgmalloc.h)** - The main exposed API
- **[lgmalloc_cpp_operators.hpp](./src/api/lgmalloc_cpp_operators.hpp)** - C++ heap operator overloads

Implement the [build](#-building) system and [configure](#-configuration) it in your project.

Depending on your preferences you can now use `lgmalloc()`, `malloc()` or `new` to call the allocator.

### 📚 Examples

### 🌐 API

The API is primarily exposed via [lgmalloc.h](./src/api/lgmalloc.h).

Here, the main standardized definitions are available:

```c
void *lgmalloc(size_t size);

void lgfree(void *ptr);

void *lgcalloc(size_t nmemb, size_t size);

void *lgrealloc(void *ptr, size_t size);
```

These are best used for explicit usage of the allocator.

Optionally, an aliasing system is available, which can replace `malloc(3)` with lgmalloc.<br>
This is configurable via the build system, see [configuration](#-configuration).

For further details on malloc and related, see the [`malloc(3)`](https://man7.org/linux/man-pages/man3/malloc.3.html) man page.

The `new` and `delete` operators from C++ are also present.<br>
These are implemented in [lgmalloc_cpp_operators.hpp](./src/api/lgmalloc_cpp_operators.hpp).<br>
You can use these overloads within your C++ code to use lgmalloc.

## 🛠 Building

### 🧱 Platform

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