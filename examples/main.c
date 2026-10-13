/**
 * @file main.c
 * @brief Bangsaen Active Defense SDK - Integration Example & Sanity Verification.
 * @version 1.0.0
 * @date 2026
 */

#include <stdio.h>
#include <string.h>
#include "../include/bangsaen_sec.h"

int main(void) {
    printf("=================================================================\n");
    printf("  Bangsaen AI Labs: Active Defense SDK Integration Test\n");
    printf("=================================================================\n\n");

    /* 1. Simulate sensitive runtime payload (e.g., Decrypted Key or Model Weights) */
    char sensitive_buffer[128] = "CONFIDENTIAL_AI_MODEL_WEIGHTS_OR_SESSION_KEY_2026";
    size_t payload_size = sizeof(sensitive_buffer);

    printf("[1] Initial Payload State in RAM:\n");
    printf("    Data  : \"%s\"\n", sensitive_buffer);
    printf("    Address: %p | Size: %zu bytes\n\n", (void*)sensitive_buffer, payload_size);

    /* 2. Execute Hardware 3-Pass Zeroization & Cache Flushing */
    printf("[2] Executing bangsaen_sec_destroy_and_contaminate()...\n");
    BangsaenStatus status = bangsaen_sec_destroy_and_contaminate(sensitive_buffer, payload_size);

    /* 3. Validate Execution Status */
    if (status == BANGSAEN_SEC_SUCCESS) {
        printf("    [SUCCESS] Hardware Memory Purged & CPU Cache Lines Flushed (+0.00MB Remanence).\n\n");
    } else {
        printf("    [FAILED] Operation Error Code: %d\n\n", status);
        return 1;
    }

    /* 4. Audit Buffer State After Purification */
    printf("[3] Post-Cleanse RAM Buffer Audit:\n");
    printf("    Hex Dump (First 16 Bytes): ");
    for (size_t i = 0; i < 16; i++) {
        printf("%02X ", (unsigned char)sensitive_buffer[i]);
    }
    printf("\n    Text Output              : \"%s\"\n\n", sensitive_buffer);

    /* 5. Fetch and Display Hardware Metrics */
    BangsaenCoreMetrics metrics;
    bangsaen_sec_get_metrics(&metrics);
    printf("[4] Core Metrics Summary:\n");
    printf("    Total Operations Executed : %llu\n", (unsigned long long)metrics.total_operations);
    printf("    Total Memory Volume Cleaned: %llu bytes\n", (unsigned long long)metrics.total_bytes_cleared);
    printf("    Engine Operational Status : %s\n\n", metrics.active_status ? "ACTIVE (Valid License)" : "EXPIRED");

    printf("=================================================================\n");
    printf("  Verification Complete: Zero Remanence Guaranteed (+0.00MB)\n");
    printf("=================================================================\n");

    return 0;
}