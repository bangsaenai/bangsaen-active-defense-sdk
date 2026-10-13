/**
 * @file bangsaen_sec.h
 * @brief Bangsaen AI Labs - Active Defense Core C-Native SDK
 * @version 1.0.0
 * @date 2026
 *
 * Bare-metal hardware zeroization, dynamic entropy poisoning, and 
 * L1/L2/L3 cache line purging engine for active threat mitigation.
 */

#ifndef BANGSAEN_SEC_H
#define BANGSAEN_SEC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 1-Year Free Community Tier Operational Epoch Boundary.
 * Hard-coded limit for community evaluation window (12 Months).
 */
#ifndef BANGSAEN_SDK_EXPIRATION_EPOCH
#define BANGSAEN_SDK_EXPIRATION_EPOCH 1824854400UL /* Oct 2027 */
#endif

/**
 * @brief Return Codes for Active Defense Core operations.
 */
typedef enum {
    BANGSAEN_SEC_SUCCESS          =  0,  /**< Operation succeeded with +0.00MB remanence */
    BANGSAEN_SEC_ERR_NULL_PTR    = -1,  /**< Null buffer pointer provided */
    BANGSAEN_SEC_ERR_ZERO_SIZE   = -2,  /**< Buffer size is zero */
    BANGSAEN_SEC_ERR_EXPIRED     = -3   /**< 1-Year Community evaluation period expired */
} BangsaenStatus;

/**
 * @brief Core Telemetry and Performance Metrics Structure.
 */
typedef struct {
    uint64_t total_operations;      /**< Total zeroization calls executed */
    uint64_t total_bytes_cleared;   /**< Total memory volume purged in bytes */
    uint32_t active_status;         /**< 1 = Active/Valid, 0 = Expired */
} BangsaenCoreMetrics;

/**
 * @brief Executes 3-Pass Active Memory Zeroization & CPU Cache Line Flush.
 *
 * Pass 1: Dynamic XOR Entropy Poisoning to disrupt memory patterns.
 * Pass 2: Volatile Zeroization (0x00) with Dead Store Elimination protection.
 * Pass 3: CPU Cache Line Invalidation (_mm_clflush / dc civac) + Memory Fence.
 *
 * @param buffer Pointer to sensitive memory region (e.g., model weights, API keys).
 * @param size   Size of the buffer in bytes.
 * @return BangsaenStatus Result code of the operation.
 */
BangsaenStatus bangsaen_sec_destroy_and_contaminate(void *buffer, size_t size);

/**
 * @brief Queries internal performance and telemetry metrics.
 *
 * @param out_metrics Pointer to metrics struct to receive execution counters.
 */
void bangsaen_sec_get_metrics(BangsaenCoreMetrics *out_metrics);

#ifdef __cplusplus
}
#endif

#endif /* BANGSAEN_SEC_H */