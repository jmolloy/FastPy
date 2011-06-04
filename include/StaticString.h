/*
 * Copyright (c) 2008 James Molloy, Jörg Pfähler, Matthew Iselin
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef STATICSTRING_H
#define STATICSTRING_H

#include <string.h>

/** @addtogroup kernelutilities
 * @{ */

/**
 * Derivative of StringBase that uses a statically allocated chunk of memory.
 */
template<unsigned int N>
class StaticString
{
public:
  /**
   * Default constructor.
   */
  StaticString()
    : m_Length(0)
  {
    m_pData[0] = '\0';
  }

  /**
   * Creates a StaticString from a const char *.
   * This creates a new copy of pSrc - pSrc can be safely
   * deallocated afterwards.
   */
  explicit StaticString(const char *pSrc)
    : m_Length(strlen(pSrc))
  {
    strncpy(m_pData, pSrc, N);

    if (m_Length >= N)
    {
      m_Length = N - 1;
      m_pData[N-1] = '\0';
    }
  }

  /**
   * Copy constructor - creates a StaticString from another StaticString.
   * Copies the memory associated with src.
   */
  template<unsigned int N2>
  explicit StaticString(const StaticString<N2> &src)
    : m_Length(src.length())
  {
    strncpy(m_pData, src, N);

    if (m_Length >= N)
    {
      m_Length = N - 1;
      m_pData[N-1] = '\0';
    }
  }

  /**
   * Destructor.
   */
  inline virtual ~StaticString()
  {
  }

  operator const char*() const
  {
    return m_pData;
  }

  template<unsigned int N2>
  StaticString &operator+=(const StaticString<N2> &str)
  {
    append(str);
    return *this;
  }

  template<typename T>
  StaticString &operator+=(T i)
  {
    append(i);
    return *this;
  }

  void clear()
  {
    m_Length = 0;
    m_pData[0] = '\0';
  }

  StaticString &operator = (const char *str)
  {
    m_Length = strlen(str);
    strncpy(m_pData, str, N);
    if (m_Length >= N)
    {
      m_pData[N - 1] = '\0';
      m_Length = N - 1;
    }
    return *this;
  }

  bool operator==(const char* pStr) const
  {
    return (strcmp(m_pData, pStr) == 0);
  }

  template<unsigned int N2>
  bool operator==(const StaticString<N2> &other) const
  {
    return (strcmp(m_pData, other) == 0);
  }

  int last(const char search) const
  {
    for (int i = length(); i >= 0; i--)
      if (m_pData[i] == search)
        return i;
    return -1;
  }

  int first(const char search) const
  {
    for (size_t i = 0; i < length(); i++)
      if (m_pData[i] == search)
        return i;
    return -1;
  }

  void stripLast()
  {
    if (m_Length)
      m_pData[--m_Length] = '\0';
  }

  bool contains(const char *other) const
  {
    if (strlen(other) >= length())
      return (strcmp(m_pData, other) == 0);

    size_t mylen = length();
    size_t itslen = strlen(other);
    for (size_t i = 0; i < mylen-itslen+1; i++)
      if (strncmp(&m_pData[i], other, strlen(other)) == 0)
        return true;
    return false;
  }

  template<unsigned int N2>
  bool contains(const StaticString<N2> &other) const
  {
    if (other.length() >= length())
      return (strcmp(m_pData, other) == 0);

    size_t mylen = length();
    size_t itslen = other.length();
    for (size_t i = 0; i < mylen-itslen+1; i++)
      if (strncmp(&m_pData[i], other.m_pData, strlen(other.m_pData)) == 0)
        return true;
    return false;
  }

  int intValue(int nBase=0) const
  {
    const char *pEnd;
    int ret = strtoul(m_pData, const_cast<char **> (&pEnd), nBase);
    if (pEnd == m_pData)
      return -1; // Failed to find anything.
    else
      return ret;
  }

  StaticString left(int n) const
  {
    StaticString<N> str;
    strncpy(str.m_pData, m_pData, n);
    str.m_pData[n] = '\0';
    return str;
  }

  StaticString right(int n) const
  {
    StaticString<N> str;
    strncpy(str.m_pData, &m_pData[length()-n], n);
    str.m_pData[n] = '\0';
    return str;
  }

  StaticString &stripFirst(size_t n=1)
  {
    if (n > length())
    {
      m_pData[0] = '\0';
      m_Length = 0;
      return *this;
    }
    int i;
    for (i = n; m_pData[i] != '\0'; i++)
      m_pData[i-n] = m_pData[i];
    m_pData[i-n] = '\0';
    m_Length -= n;
    return *this;
  }

  template<typename T>
  StaticString &operator << (T t)
  {
    append(t);
    return *this;
  }

  void append(char Char, size_t nLen=0, char c = '0')
  {
    char Characters[] = {Char, '\0'};
    append(Characters, nLen, c);
  }

  void append(short nInt, size_t nRadix=10, size_t nLen=0, char c='0')
  {
    if (nInt < 0)
    {
      append("-");
      nInt = -nInt;
    }
    append(static_cast<unsigned short>(nInt), nRadix, nLen, c);
  }

  void append(int nInt, size_t nRadix=10, size_t nLen=0, char c='0')
  {
    if (nInt < 0)
    {
      append("-");
      nInt = -nInt;
    }
    append(static_cast<unsigned int>(nInt), nRadix, nLen, c);
  }

  void append(long nInt, size_t nRadix=10, size_t nLen=0, char c='0')
  {
    if (nInt < 0)
    {
      append("-");
      nInt = -nInt;
    }
    append(static_cast<unsigned long>(nInt), nRadix, nLen, c);
  }

  void append(long long nInt, size_t nRadix=10, size_t nLen=0, char c='0')
  {
    if (nInt < 0)
    {
      append("-");
      nInt = -nInt;
    }
    append(static_cast<unsigned long long>(nInt), nRadix, nLen, c);
  }

  void append(unsigned char nInt, size_t nRadix=10, size_t nLen=0, char c='0')
  {
    appendInteger<sizeof(char)>(nInt, nRadix, nLen, c);
  }

  void append(unsigned short nInt, size_t nRadix=10, size_t nLen=0, char c='0')
  {
    appendInteger<sizeof(short)>(nInt, nRadix, nLen, c);
  }

  void append(unsigned int nInt, size_t nRadix=10, size_t nLen=0, char c='0')
  {
    appendInteger<sizeof(int)>(nInt, nRadix, nLen, c);
  }

  void append(unsigned long nInt, size_t nRadix=10, size_t nLen=0, char c='0')
  {
    appendInteger<sizeof(long)>(nInt, nRadix, nLen, c);
  }

  void append(unsigned long long nInt, size_t nRadix=10, size_t nLen=0, char c='0')
  {
    appendInteger<sizeof(long long)>(nInt, nRadix, nLen, c);
  }

  template<unsigned int size, typename T>
  void appendInteger(T nInt, size_t nRadix, size_t nLen, char c)
  {
    char pStr[size * 8 + 1];
    size_t index = 0;
    do
    {
      size_t tmp = nInt % nRadix;
      nInt /= nRadix;
      if (tmp < 10)pStr[index++] = '0' + tmp;
      else pStr[index++] = 'a' + (tmp - 10);
    }
    while (nInt != 0);

    for (size_t i = 0;i < (index / 2);i++)
    {
      char tmp = pStr[i];
      pStr[i] = pStr[index - i - 1];
      pStr[index - i - 1] = tmp;
    }

    pStr[index] = '\0';

    append(pStr, nLen, c);
  }

  void append(const char *str, size_t nLen=0, char c=' ')
  {
    size_t length2 = strlen(str);

    // Pad, if needed
    if (nLen > length2)
    {
      size_t i;
      for(i = 0; i < nLen - length2; i++)
      {
        m_pData[i + length()] = c;
      }
      m_pData[i + length()] = '\0';
      m_Length += nLen - length2;
    }

    // Add the string
    strncat(m_pData, str, N-length());
    m_Length += length2;

    if (m_Length >= N)
    {
      m_pData[N-1] = '\0';
      m_Length = N - 1;
    }
  }

  template<unsigned int N2>
  void append(const StaticString<N2> &str, size_t nLen=0, char c=' ')
  {
    // Pad, if needed
    if (nLen > str.length())
    {
      size_t i;
      for(i = 0; i < nLen - str.length(); i++)
      {
        m_pData[i + length()] = c;
      }
      m_pData[i + length()] = '\0';
      m_Length += nLen - str.length();
    }

    // Add the string
    strncat(m_pData, str, N-length());
    m_Length += str.length();

    if (m_Length >= N)
    {
      m_pData[N-1] = '\0';
      m_Length = N - 1;
    }
  }

  void pad(size_t nLen, char c=' ')
  {
    // Pad, if needed
    if (nLen > length())
    {
      size_t i;
      for(i = 0; i < nLen - length(); i++)
      {
        m_pData[i + length()] = c;
      }
      m_pData[i + length()] = '\0';
      m_Length += nLen - length();
    }
  }

  size_t length() const
  {
    return m_Length;
  }

  private:
  /**
   * Our actual static data.
   */
  char m_pData[N];

  size_t m_Length;
};

typedef StaticString<32>   TinyStaticString;
typedef StaticString<64>   NormalStaticString;
typedef StaticString<128>  LargeStaticString;
typedef StaticString<1024> HugeStaticString;

/** @} */

#endif
