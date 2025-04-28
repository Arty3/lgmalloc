/* ******************************************** */
/*                                              */
/*   lgmalloc_cpp_operators.hpp                 */
/*                                              */
/*   Author: https://github.com/Arty3           */
/*                                              */
/* ******************************************** */

/* Redundant C++ check but better to have */
#ifdef __cplusplus

#pragma once

#include <new>
#include "lgmalloc.h"

extern void operator delete(void *ptr) noexcept
{
	lgfree(ptr);
}

extern void operator delete[](void *ptr) noexcept
{
	lgfree(ptr);
}

extern void *operator new(std::size_t size) noexcept(false)
{
	return lgmalloc(size);
}

extern void *operator new[](std::size_t size) noexcept(false)
{
	return lgmalloc(size);
}

/* Using `static_cast` to avoid errors with -Wold-style-cast and -Wunused-parameter */

extern void *operator new(std::size_t size, const std::nothrow_t& tag) noexcept
{
	static_cast<void>(tag);
	return lgmalloc(size);
}

extern void *operator new[](std::size_t size, const std::nothrow_t& tag) noexcept
{
	static_cast<void>(tag);
	return lgmalloc(size);
}

#if __cplusplus >= 201402L

extern void operator delete(void *ptr, std::size_t size) noexcept
{
	static_cast<void>(size);
	lgfree(ptr);
}

extern void operator delete[](void *ptr, std::size_t size) noexcept
{
	static_cast<void>(size);
	lgfree(ptr);
}

#endif /* __cplusplus >= 201402L */

#if __cplusplus > 201402L || defined(__cpp_aligned_new)

extern void operator delete(void *ptr, std::align_val_t align) noexcept
{
	static_cast<void>(align);
	lgfree(ptr);
}

extern void operator delete[](void *ptr, std::align_val_t align) noexcept
{
	static_cast<void>(align);
	lgfree(ptr);
}

extern void operator delete(void *ptr, std::size_t size, std::align_val_t align) noexcept
{
	static_cast<void>(size);
	static_cast<void>(align);
	lgfree(ptr);
}

extern void operator delete[](void *ptr, std::size_t size, std::align_val_t align) noexcept
{
	static_cast<void>(size);
	static_cast<void>(align);
	lgfree(ptr);
}

#endif /* __cplusplus > 201402L || defined(__cpp_aligned_new) */
#endif /* __cplusplus */
