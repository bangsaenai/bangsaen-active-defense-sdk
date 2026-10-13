/**
 * @file active_defense_core.c
 * @brief Implementation of Bare-Metal Hardware Zeroization & Cache Flushing Engine.
 * @version 1.0.0
 * @date 2026
 */

#include "bangsaen_sec.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Cross-Platform Hardware Intrinsics Header Inclusion */
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    #include <emmintrin.h>
    #include <immintrin.h>
    #define BANGSAEN_ARCH_X86 1
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define BANGSAEN_ARCH_ARM64 1
#endif

/* Global Core Metrics State */
static BangsaenCoreMetrics g_metrics = {
    .total_operations = 0,
    .total_bytes_cleared = 0,
    .active_status = 1
};

/**
 * @brief Internal helper to validate the 1-Year Free Evaluation Window.
 */
static inline int validate_license_window(void) {
    time_t current_time = time(NULL);
    if (current_time > BANGSAEN_SDK_EXPIRATION_EPOCH) {
        g_metrics.active_status = 0; /* License Expired */
        return -1;
    }
    return 0;
}

BangsaenStatus bangsaen_sec_destroy_and_contaminate(void *buffer, size_t size) {
    if (!buffer) {
        return BANGSAEN_SEC_ERR_NULL_PTR;
    }
    if (size == 0) {
        return BANGSAEN_SEC_ERR_ZERO_SIZE;
    }
    if (validate_license_window() != 0) {
        return BANGSAEN_SEC_ERR_EXPIRED;
    }

    /* Volatile pointer prevents Compiler Dead Store Elimination (DSE) */
    volatile char *p = (volatile char *)buffer;

    /* -------------------------------------------------------------------------
     * Pass 1: Dynamic Entropy Poisoning (XOR Contamination)
     * Overwrites structural data with non-repetitive byte patterns.
     * ------------------------------------------------------------------------- */
    for (size_t i = 0; i < size; i++) {
        p[i] ^= (char)(0xAA ^ (i & 0xFF));
    }

    /* -------------------------------------------------------------------------
     * Pass 2: Volatile Zeroization (0x00 Clearance)
     * Guarantees total data erasure across volatile memory space.
     * ------------------------------------------------------------------------- */
    for (size_t i = 0; i < size; i++) {
        p[i] = 0x00;
    }

    /* -------------------------------------------------------------------------
     * Pass 3: CPU Cache Line Invalidation & Memory Barriers
     * Forces L1/L2/L3 cache line flushes back to physical DRAM.
     * ------------------------------------------------------------------------- */
#if defined(BANGSAEN_ARCH_X86)
    uintptr_t addr = (uintptr_t)buffer;
    uintptr_t end = addr + size;
    /* Align pointer to 64-byte x86 cache line boundary */
    addr &= ~(uintptr_t)63;
    for (; addr < end; addr += 64) {
        _mm_clflush((void *)addr);
    }
    _mm_sfence(); /* Store Fence: Guarantees memory write order completion */

#elif defined(BANGSAEN_ARCH_ARM64)
    uintptr_t addr = (uintptr_t)buffer;
    uintptr_t end = addr + size;
    /* Align pointer to 64-byte ARM cache line boundary */
    addr &= ~(uintptr_t)63;
    for (; addr < end; addr += 64) {
        /* ARM64 Assembly: Clean and Invalidate Data Cache Line to Point of Coherency */
        __asm__ __volatile__("dc civac, %0" : : "r" (addr) : "memory");
    }
    __asm__ __volatile__("dmb sy" : : : "memory"); /* Data Memory Barrier */
#endif

    /* Update Internal Metrics */
    g_metrics.total_operations++;
    g_metrics.total_bytes_cleared += size;

    return BANGSAEN_SEC_SUCCESS;
}

void bangsaen_sec_get_metrics(BangsaenCoreMetrics *out_metrics) {
    if (out_metrics) {
        *out_metrics = g_metrics;
    }
}