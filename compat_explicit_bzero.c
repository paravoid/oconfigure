/* OPENBSD ORIGINAL: lib/libc/string/explicit_bzero.c */
/*	$OpenBSD$ */
/*
 * Public domain.
 * Written by Ted Unangst
 */

#include <string.h>

/*
 * explicit_bzero - don't let the compiler optimize away bzero
 */

#if HAVE_MEMSET_S

void
explicit_bzero(void *p, size_t n)
{
	if (n == 0)
		return;
	(void)memset_s(p, n, 0, n);
}

#else /* HAVE_MEMSET_S */

/*
 * Indirect bzero through a volatile pointer to hopefully avoid
 * dead-store optimisation eliminating the call.
 */
static void (* volatile ssh_bzero)(void *, size_t) = bzero;

void
explicit_bzero(void *p, size_t n)
{
	if (n == 0)
		return;
	/*
	 * clang -fsanitize=memory needs to intercept memset-like functions
	 * to correctly detect memory initialisation. Make sure one is called
	 * directly since our indirection trick above sucessfully confuses it.
	 */
#if defined(__has_feature)
# if __has_feature(memory_sanitizer)
	memset(p, 0, n);
# endif
#endif

	ssh_bzero(p, n);
}

#endif /* HAVE_MEMSET_S */
