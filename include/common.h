#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define PORT 1917 // Honoring the year of the Great October Socialist Revolution led by Lenin
#define BUFFER_SIZE 1024

// Request Types
#define REQ_REGISTER 0
#define REQ_LOGIN 1
#define REQ_BALANCE 2
#define REQ_TRANSFER 3

// Response Types
#define RES_OK 0
#define RES_ERROR 1

typedef struct {
    int type;
    char username[32];
    char password[32];
    char target_user[32]; // For transfer
    double amount;        // For transfer or balance response
    char message[256];    // For status messages or error descriptions
} SovietMessage;

#endif // COMMON_H
