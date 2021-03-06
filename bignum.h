// Copyright (c) 2009 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_BIGNUM_H
#define BITCOIN_BIGNUM_H

#include <algorithm>
#include <openssl/bn.h>
#include <stdexcept>
#include <vector>

#include "uint256.h"

class bignum_error : public std::runtime_error {
  public:
    explicit bignum_error(const std::string& str) : std::runtime_error(str)
    {
    }
};

class CAutoBN_CTX {
  protected:
    BN_CTX* pctx;
    BN_CTX* operator=(BN_CTX* pnew)
    {
        return pctx = pnew;
    }

  public:
    CAutoBN_CTX()
    {
        pctx = BN_CTX_new();
        if (pctx == NULL)
            throw bignum_error("CAutoBN_CTX : BN_CTX_new() returned NULL");
    }

    ~CAutoBN_CTX()
    {
        if (pctx != NULL)
            BN_CTX_free(pctx);
    }

    operator BN_CTX*()
    {
        return pctx;
    }
    BN_CTX& operator*()
    {
        return *pctx;
    }
    BN_CTX** operator&()
    {
        return &pctx;
    }
    bool operator!()
    {
        return (pctx == NULL);
    }
};

class CBigNum {
  public:
    BIGNUM * big_pointer;
    CBigNum()
    {
        big_pointer = BN_new();
    }

    CBigNum(const CBigNum& b)
    {
        big_pointer = BN_new();
        if (!BN_copy(big_pointer, b.big_pointer)) {
            BN_clear_free(big_pointer);
            throw bignum_error(
                "CBigNum::CBigNum(const CBigNum&) : BN_copy failed");
        }
    }

    explicit CBigNum(const std::string& str)
    {
        big_pointer = BN_new();
        SetHex(str);
    }

    CBigNum& operator=(const CBigNum& b)
    {
        if (!BN_copy(big_pointer, b.big_pointer))
            throw bignum_error("CBigNum::operator= : BN_copy failed");
        return (*this);
    }

    ~CBigNum()
    {
        BN_clear_free(big_pointer);
    }

    CBigNum(char n)
    {
        big_pointer = BN_new();
        if (n >= 0)
            setulong(n);
        else
            setint64(n);
    }
    CBigNum(short n)
    {
        big_pointer = BN_new();
        if (n >= 0)
            setulong(n);
        else
            setint64(n);
    }
    CBigNum(int n)
    {
        big_pointer = BN_new();
        if (n >= 0)
            setulong(n);
        else
            setint64(n);
    }
    CBigNum(long n)
    {
        big_pointer = BN_new();
        if (n >= 0)
            setulong(n);
        else
            setint64(n);
    }
    CBigNum(int64 n)
    {
        big_pointer = BN_new();
        setint64(n);
    }
    CBigNum(unsigned char n)
    {
        big_pointer = BN_new();
        setulong(n);
    }
    CBigNum(unsigned short n)
    {
        big_pointer = BN_new();
        setulong(n);
    }
    CBigNum(unsigned int n)
    {
        big_pointer = BN_new();
        setulong(n);
    }
    CBigNum(unsigned long n)
    {
        big_pointer = BN_new();
        setulong(n);
    }
    CBigNum(uint64 n)
    {
        big_pointer = BN_new();
        setuint64(n);
    }
    CBigNum(uint256 n)
    {
        big_pointer = BN_new();
        setuint256(n);
    }

    explicit CBigNum(const std::vector<unsigned char>& vch)
    {
        big_pointer = BN_new();
        setvch(vch);
    }

    void setulong(unsigned long n)
    {
        if (!BN_set_word(big_pointer, n))
            throw bignum_error(
                "CBigNum conversion from unsigned long : BN_set_word failed");
    }

    unsigned long getulong() const
    {
        return BN_get_word(big_pointer);
    }

    unsigned int getuint() const
    {
        return BN_get_word(big_pointer);
    }

    int getint() const
    {
        unsigned long n = BN_get_word(big_pointer);
        if (!BN_is_negative(big_pointer))
            return (n > INT_MAX ? INT_MAX : n);
        else
            return (n > INT_MAX ? INT_MIN : -(int)n);
    }

    void setint64(int64 n)
    {
        unsigned char  pch[sizeof(n) + 6];
        unsigned char* p         = pch + 4;
        bool           fNegative = false;
        if (n < (int64)0) {
            n         = -n;
            fNegative = true;
        }
        bool fLeadingZeroes = true;
        for (int i = 0; i < 8; i++) {
            unsigned char c = (n >> 56) & 0xff;
            n <<= 8;
            if (fLeadingZeroes) {
                if (c == 0)
                    continue;
                if (c & 0x80)
                    *p++ = (fNegative ? 0x80 : 0);
                else if (fNegative)
                    c |= 0x80;
                fLeadingZeroes = false;
            }
            *p++ = c;
        }
        unsigned int nSize = p - (pch + 4);
        pch[0]             = (nSize >> 24) & 0xff;
        pch[1]             = (nSize >> 16) & 0xff;
        pch[2]             = (nSize >> 8) & 0xff;
        pch[3]             = (nSize)&0xff;
        BN_mpi2bn(pch, p - pch, big_pointer);
    }

    void setuint64(uint64 n)
    {
        unsigned char  pch[sizeof(n) + 6];
        unsigned char* p              = pch + 4;
        bool           fLeadingZeroes = true;
        for (int i = 0; i < 8; i++) {
            unsigned char c = (n >> 56) & 0xff;
            n <<= 8;
            if (fLeadingZeroes) {
                if (c == 0)
                    continue;
                if (c & 0x80)
                    *p++       = 0;
                fLeadingZeroes = false;
            }
            *p++ = c;
        }
        unsigned int nSize = p - (pch + 4);
        pch[0]             = (nSize >> 24) & 0xff;
        pch[1]             = (nSize >> 16) & 0xff;
        pch[2]             = (nSize >> 8) & 0xff;
        pch[3]             = (nSize)&0xff;
        BN_mpi2bn(pch, p - pch, big_pointer);
    }

    void setuint256(uint256 n)
    {
        unsigned char  pch[sizeof(n) + 6];
        unsigned char* p              = pch + 4;
        bool           fLeadingZeroes = true;
        unsigned char* pbegin         = (unsigned char*)&n;
        unsigned char* psrc           = pbegin + sizeof(n);
        while (psrc != pbegin) {
            unsigned char c = *(--psrc);
            if (fLeadingZeroes) {
                if (c == 0)
                    continue;
                if (c & 0x80)
                    *p++       = 0;
                fLeadingZeroes = false;
            }
            *p++ = c;
        }
        unsigned int nSize = p - (pch + 4);
        pch[0]             = (nSize >> 24) & 0xff;
        pch[1]             = (nSize >> 16) & 0xff;
        pch[2]             = (nSize >> 8) & 0xff;
        pch[3]             = (nSize >> 0) & 0xff;
        BN_mpi2bn(pch, p - pch, big_pointer);
    }

    uint256 getuint256()
    {
        unsigned int nSize = BN_bn2mpi(big_pointer, NULL);
        if (nSize < 4)
            return 0;
        std::vector<unsigned char> vch(nSize);
        BN_bn2mpi(big_pointer, &vch[0]);
        if (vch.size() > 4)
            vch[4] &= 0x7f;
        uint256 n = 0;
        for (int i = 0, j = vch.size() - 1; i < sizeof(n) && j >= 4; i++, j--)
            ((unsigned char*)&n)[i] = vch[j];
        return n;
    }

    void setvch(const std::vector<unsigned char>& vch)
    {
        std::vector<unsigned char> vch2(vch.size() + 4);
        unsigned int               nSize = vch.size();
        vch2[0]                          = (nSize >> 24) & 0xff;
        vch2[1]                          = (nSize >> 16) & 0xff;
        vch2[2]                          = (nSize >> 8) & 0xff;
        vch2[3]                          = (nSize >> 0) & 0xff;
        reverse_copy(vch.begin(), vch.end(), vch2.begin() + 4);
        BN_mpi2bn(&vch2[0], vch2.size(), big_pointer);
    }

    std::vector<unsigned char> getvch() const
    {
        unsigned int nSize = BN_bn2mpi(big_pointer, NULL);
        if (nSize < 4)
            return std::vector<unsigned char>();
        std::vector<unsigned char> vch(nSize);
        BN_bn2mpi(big_pointer, &vch[0]);
        vch.erase(vch.begin(), vch.begin() + 4);
        reverse(vch.begin(), vch.end());
        return vch;
    }

    CBigNum& SetCompact(unsigned int nCompact)
    {
        unsigned int               nSize = nCompact >> 24;
        std::vector<unsigned char> vch(4 + nSize);
        vch[3] = nSize;
        if (nSize >= 1)
            vch[4] = (nCompact >> 16) & 0xff;
        if (nSize >= 2)
            vch[5] = (nCompact >> 8) & 0xff;
        if (nSize >= 3)
            vch[6] = (nCompact >> 0) & 0xff;
        BN_mpi2bn(&vch[0], vch.size(), big_pointer);
        return *this;
    }

    unsigned int GetCompact() const
    {
        unsigned int               nSize = BN_bn2mpi(big_pointer, NULL);
        std::vector<unsigned char> vch(nSize);
        nSize -= 4;
        BN_bn2mpi(big_pointer, &vch[0]);
        unsigned int nCompact = nSize << 24;
        if (nSize >= 1)
            nCompact |= (vch[4] << 16);
        if (nSize >= 2)
            nCompact |= (vch[5] << 8);
        if (nSize >= 3)
            nCompact |= (vch[6] << 0);
        return nCompact;
    }

    void SetHex(const std::string& str)
    {
        // skip 0x
        const char* psz = str.c_str();
        while (isspace(*psz))
            psz++;
        bool fNegative = false;
        if (*psz == '-') {
            fNegative = true;
            psz++;
        }
        if (psz[0] == '0' && tolower(psz[1]) == 'x')
            psz += 2;
        while (isspace(*psz))
            psz++;

        // hex string to bignum
        static char phexdigit[256] = {
            0, 0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 1,   2,   3,   4,   5,   6,   7, 8, 9, 0, 0, 0, 0, 0, 0,
            0, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        *this = 0;
        while (isxdigit(*psz)) {
            *this <<= 4;
            int n = phexdigit[*psz++];
            *this += n;
        }
        if (fNegative)
            *this = 0 - *this;
    }

    bool operator!() const
    {
        return BN_is_zero(big_pointer);
    }

    CBigNum& operator+=(const CBigNum& b)
    {
        if (!BN_add(big_pointer, big_pointer, b.big_pointer))
            throw bignum_error("CBigNum::operator+= : BN_add failed");
        return *this;
    }

    CBigNum& operator-=(const CBigNum& b)
    {
        *this = *this - b;
        return *this;
    }

    CBigNum& operator*=(const CBigNum& b)
    {
        CAutoBN_CTX pctx;
        if (!BN_mul(big_pointer, big_pointer, b.big_pointer, pctx))
            throw bignum_error("CBigNum::operator*= : BN_mul failed");
        return *this;
    }

    CBigNum& operator/=(const CBigNum& b)
    {
        *this = *this / b;
        return *this;
    }

    CBigNum& operator%=(const CBigNum& b)
    {
        *this = *this % b;
        return *this;
    }

    CBigNum& operator<<=(unsigned int shift)
    {
        if (!BN_lshift(big_pointer, big_pointer, shift))
            throw bignum_error("CBigNum:operator<<= : BN_lshift failed");
        return *this;
    }

    CBigNum& operator>>=(unsigned int shift)
    {
        if (!BN_rshift(big_pointer, big_pointer, shift))
            throw bignum_error("CBigNum:operator>>= : BN_rshift failed");
        return *this;
    }

    CBigNum& operator++()
    {
        // prefix operator
        if (!BN_add(big_pointer, big_pointer, BN_value_one()))
            throw bignum_error("CBigNum::operator++ : BN_add failed");
        return *this;
    }

    const CBigNum operator++(int)
    {
        // postfix operator
        const CBigNum ret = *this;
        ++(*this);
        return ret;
    }

    CBigNum& operator--()
    {
        // prefix operator
        CBigNum r;
        if (!BN_sub(r.big_pointer, big_pointer, BN_value_one()))
            throw bignum_error("CBigNum::operator-- : BN_sub failed");
        *this = r;
        return *this;
    }

    const CBigNum operator--(int)
    {
        // postfix operator
        const CBigNum ret = *this;
        --(*this);
        return ret;
    }

    friend inline const CBigNum operator-(const CBigNum& a, const CBigNum& b);
    friend inline const CBigNum operator/(const CBigNum& a, const CBigNum& b);
    friend inline const CBigNum operator%(const CBigNum& a, const CBigNum& b);
};

inline const CBigNum operator+(const CBigNum& a, const CBigNum& b)
{
    CBigNum r;
    if (!BN_add(r.big_pointer, a.big_pointer, b.big_pointer))
        throw bignum_error("CBigNum::operator+ : BN_add failed");
    return r;
}

inline const CBigNum operator-(const CBigNum& a, const CBigNum& b)
{
    CBigNum r;
    if (!BN_sub(r.big_pointer, a.big_pointer, b.big_pointer))
        throw bignum_error("CBigNum::operator- : BN_sub failed");
    return r;
}

inline const CBigNum operator-(const CBigNum& a)
{
    CBigNum r(a);
    BN_set_negative(r.big_pointer, !BN_is_negative(r.big_pointer));
    return r;
}

inline const CBigNum operator*(const CBigNum& a, const CBigNum& b)
{
    CAutoBN_CTX pctx;
    CBigNum     r;
    if (!BN_mul(r.big_pointer, a.big_pointer, b.big_pointer, pctx))
        throw bignum_error("CBigNum::operator* : BN_mul failed");
    return r;
}

inline const CBigNum operator/(const CBigNum& a, const CBigNum& b)
{
    CAutoBN_CTX pctx;
    CBigNum     r;
    if (!BN_div(r.big_pointer, NULL, a.big_pointer, b.big_pointer, pctx))
        throw bignum_error("CBigNum::operator/ : BN_div failed");
    return r;
}

inline const CBigNum operator%(const CBigNum& a, const CBigNum& b)
{
    CAutoBN_CTX pctx;
    CBigNum     r;
    if (!BN_mod(r.big_pointer, a.big_pointer, b.big_pointer, pctx))
        throw bignum_error("CBigNum::operator% : BN_div failed");
    return r;
}

inline const CBigNum operator<<(const CBigNum& a, unsigned int shift)
{
    CBigNum r;
    if (!BN_lshift(r.big_pointer, a.big_pointer, shift))
        throw bignum_error("CBigNum:operator<< : BN_lshift failed");
    return r;
}

inline const CBigNum operator>>(const CBigNum& a, unsigned int shift)
{
    CBigNum r;
    if (!BN_rshift(r.big_pointer, a.big_pointer, shift))
        throw bignum_error("CBigNum:operator>> : BN_rshift failed");
    return r;
}

inline bool operator==(const CBigNum& a, const CBigNum& b)
{
    return (BN_cmp(a.big_pointer, b.big_pointer) == 0);
}
inline bool operator!=(const CBigNum& a, const CBigNum& b)
{
    return (BN_cmp(a.big_pointer, b.big_pointer) != 0);
}
inline bool operator<=(const CBigNum& a, const CBigNum& b)
{
    return (BN_cmp(a.big_pointer, b.big_pointer) <= 0);
}
inline bool operator>=(const CBigNum& a, const CBigNum& b)
{
    return (BN_cmp(a.big_pointer, b.big_pointer) >= 0);
}
inline bool operator<(const CBigNum& a, const CBigNum& b)
{
    return (BN_cmp(a.big_pointer, b.big_pointer) < 0);
}
inline bool operator>(const CBigNum& a, const CBigNum& b)
{
    return (BN_cmp(a.big_pointer, b.big_pointer) > 0);
}

#endif  // BITCOIN_BIGNUM_H
