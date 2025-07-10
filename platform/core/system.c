#include "system.h"

const char *en_result_to_string(en_result_t result)
{
    switch (result)
    {
        case Ok: return "Ok";
        case Error: return "Error";
        case ErrorAddressAlignment: return "ErrorAddressAlignment";
        case ErrorAccessRights: return "ErrorAccessRights";
        case ErrorInvalidParameter: return "ErrorInvalidParameter";
        case ErrorOperationInProgress: return "ErrorOperationInProgress";
        case ErrorInvalidMode: return "ErrorInvalidMode";
        case ErrorUninitialized: return "ErrorUninitialized";
        case ErrorBufferFull: return "ErrorBufferFull";
        case ErrorTimeout: return "ErrorTimeout";
        case ErrorNotReady: return "ErrorNotReady";
        case OperationInProgress: return "OperationInProgress";
    }
    return "Unknown";
}