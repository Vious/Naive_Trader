#pragma once

/* for some saving, just using only ostream */

#include "basic_declarations.h"

namespace naiveTrader
{

constexpr size_t CODE_LEN = 24;

// codeId for constract
#define CODEID_START    0
// exchange id
#define EXCGID_START      8
// commodity type id
#define COMDID_START    14
// commodity No.
#define COMDNUM_START   18

struct ExCode {
private:
    char mData[CODE_LEN];

public:
    ExCode(const char *codeID) {
        size_t exId, comdNumIdx;
        exId = comdNumIdx = 0;
        std::memset(&mData, 0, sizeof(mData));
        for (int i = 0; i < CODE_LEN && codeID[i] != '\0'; i++) {
            if (codeID[i] == '.') {
                exId = i;
                continue;
            }
            if (exId == 0) {
                mData[i + EXCGID_START] = codeID[i];
            } else {
                mData[i - exId - 1] = codeID[i];
                if (!comdNumIdx && codeID[i] >= '0' && codeID[i] <= '9') {
                    comdNumIdx = i - exId - 1;
                } else {
                    mData[i - exId - 1 + COMDID_START] = codeID[i];
                }
            }
        }
        *reinterpret_cast<uint16_t*> (mData + COMDNUM_START) = static_cast<uint16_t> (std::atoi(mData + comdNumIdx));
    }

    ExCode() {
        std::memset(&mData, 0, sizeof(mData));
    }

    ExCode(const ExCode &other) {
        std::memcpy(mData, other.mData, CODE_LEN);
    }

    ExCode(const char *id, const char *excgID) {
        std::memset(&mData, 0, sizeof(mData));
        size_t comdNumIdx;
        comdNumIdx = 0;
        for (int i = 0; i < EXCGID_START && id[i] != '\0'; i++) {
            mData[i] = id[i];
            if (!comdNumIdx && id[i] >= '0' && id[i] <= '9') {
                comdNumIdx = i;
            } else {
                mData[i + COMDID_START] = id[i];
            }
        }
        *reinterpret_cast<uint16_t*> (mData + COMDNUM_START) = static_cast<uint16_t>(std::atoi(mData + comdNumIdx));
        if (std::strlen(excgID) < COMDID_START - EXCGID_START) {
            std::strcpy(mData + EXCGID_START, excgID);
        }
    }

    ExCode(const char *excgId, const char *comdId, const char *comdNum) {
        std::memset(&mData, 0, sizeof(mData));
        if (EXCGID_START > (std::strlen(comdId) + std::strlen(comdNum))) {
            std::strcpy(mData, comdId);
            size_t comdNumIdx = std::strlen(mData);
            std::strcpy(mData + comdNumIdx, comdNum);
            std::strcpy(mData + COMDID_START, comdId);
            *reinterpret_cast<uint16_t*> (mData + COMDNUM_START) = static_cast<uint16_t>(std::atoi(mData + comdNumIdx));
        }
        if ((COMDID_START - EXCGID_START) > std::strlen(excgId)) {
            std::strcpy(mData + EXCGID_START, excgId);
        }
    }

    bool operator == (const ExCode &other) const {
        return std::memcmp(mData, other.mData, CODE_LEN) == 0; 
    }

    bool operator < (const ExCode &other) const {
        return std::memcmp(mData, other.mData, CODE_LEN) < 0;
    }

    bool operator != (const ExCode &other) const {
        return std::memcmp(mData, other.mData, CODE_LEN) != 0;
    }

    const char* getCode() const {
		return mData;
	}

    const char* getExcgId() const {
        return mData + EXCGID_START;
    }

    const char* getComdId() const {
        return mData + COMDID_START;
    }

    uint16_t getComdNum() const {
        return *reinterpret_cast<const uint16_t*>(mData + COMDNUM_START);
    }

    std::ostream &operator << (std::ostream &os) {
        return os << getCode();
    }

    bool isDistinct() const {
        if (std::strcmp(EXCHANGE_SHFE, getExcgId()) == 0) {
            return true;
        } else if (std::strcmp(EXCHANGE_CFFEX, getExcgId()) == 0) {
            return true;
        } else if (std::strcmp(EXCHANGE_DCE, getExcgId()) == 0) {
            return true;
        } else if (std::strcmp(EXCHANGE_GFEX, getExcgId()) == 0) {
            return true;
        } else if (std::strcmp(EXCHANGE_INE, getExcgId()) == 0) {
            return true;
        } else if (std::strcmp(EXCHANGE_CZCE, getExcgId()) == 0) {
            return true;
        } 
        return false;
    }

};

const ExCode defualtCode;

struct TickData {
    /* data */
    ExCode codeId;

    
};
    
}