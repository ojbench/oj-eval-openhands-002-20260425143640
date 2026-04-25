#pragma once
#ifndef SJTU_BIGINTEGER
#define SJTU_BIGINTEGER

// Integer 1:
// Implement a signed big integer class that only needs to support simple addition and subtraction

// Integer 2:
// Implement a signed big integer class that supports addition, subtraction, multiplication, and division, and overload related operators

// Do not use any header files other than the following
#include <complex>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <string>

// Do not use "using namespace std;"

namespace sjtu {
class int2048 {
private:
  bool neg;              // sign flag, false for >=0
  std::vector<int> d;    // little-endian digits in base 1000
public:
  // Constructors
  int2048();
  int2048(long long);
  int2048(const std::string &);
  int2048(const int2048 &);

  // The parameter types of the following functions are for reference only, you can choose to use constant references or not
  // If needed, you can add other required functions yourself
  // ===================================
  // Integer1
  // ===================================

  // Read a big integer
  void read(const std::string &);
  // Output the stored big integer, no need for newline
  void print();

  // Add a big integer
  int2048 &add(const int2048 &);
  // Return the sum of two big integers
  friend int2048 add(int2048, const int2048 &);

  // Subtract a big integer
  int2048 &minus(const int2048 &);
  // Return the difference of two big integers
  friend int2048 minus(int2048, const int2048 &);

  // ===================================
  // Integer2
  // ===================================

  int2048 operator+() const;
  int2048 operator-() const;

  int2048 &operator=(const int2048 &);

  int2048 &operator+=(const int2048 &);
  friend int2048 operator+(int2048, const int2048 &);

  int2048 &operator-=(const int2048 &);
  friend int2048 operator-(int2048, const int2048 &);

  int2048 &operator*=(const int2048 &);
  friend int2048 operator*(int2048, const int2048 &);

  int2048 &operator/=(const int2048 &);
  friend int2048 operator/(int2048, const int2048 &);

  int2048 &operator%=(const int2048 &);
  friend int2048 operator%(int2048, const int2048 &);

  friend std::istream &operator>>(std::istream &, int2048 &);
  friend std::ostream &operator<<(std::ostream &, const int2048 &);

  friend bool operator==(const int2048 &, const int2048 &);
  friend bool operator!=(const int2048 &, const int2048 &);
  friend bool operator<(const int2048 &, const int2048 &);
  friend bool operator>(const int2048 &, const int2048 &);
  friend bool operator<=(const int2048 &, const int2048 &);
  friend bool operator>=(const int2048 &, const int2048 &);
};
} // namespace sjtu

#endif

// merged header above

namespace sjtu {
namespace {
static const int BASE = 1000; // 10^3 per limb
static const int BASE_DIG = 3;

inline void trim(std::vector<int> &a) {
  while (!a.empty() && a.back() == 0) a.pop_back();
}
inline int cmp_vec(const std::vector<int> &a, const std::vector<int> &b) {
  if (a.size() != b.size()) return a.size() < b.size() ? -1 : 1;
  for (int i=(int)a.size()-1;i>=0;--i) if (a[i]!=b[i]) return a[i]<b[i]?-1:1;
  return 0;
}
inline std::vector<int> add_vec(const std::vector<int> &a, const std::vector<int> &b) {
  std::vector<int> r; r.resize(std::max(a.size(), b.size()) + 1, 0);
  int carry=0; size_t i=0;
  for (; i<r.size(); ++i) {
    long long s = carry;
    if (i<a.size()) s += a[i];
    if (i<b.size()) s += b[i];
    r[i] = (int)(s % BASE);
    carry = (int)(s / BASE);
    if (i>=a.size() && i>=b.size() && carry==0) break;
  }
  r.resize(i+1); trim(r); return r;
}
inline std::vector<int> sub_vec(const std::vector<int> &a, const std::vector<int> &b) { // assume a>=b
  std::vector<int> r; r.resize(a.size());
  int carry=0;
  for (size_t i=0;i<a.size();++i) {
    long long cur = (long long)a[i] - carry - (i<b.size()?b[i]:0);
    if (cur<0) { cur += BASE; carry=1; } else carry=0;
    r[i] = (int)cur;
  }
  trim(r); return r;
}

using cd = std::complex<double>;
inline void fft(std::vector<cd> &a, bool invert) {
  int n = (int)a.size();
  static std::vector<int> rev; static std::vector<cd> roots{cd(0,0), cd(1,0)};
  if ((int)rev.size() != n) {
    int k = 0; while ((1<<k) < n) ++k;
    rev.assign(n,0);
    for (int i=0;i<n;i++) rev[i] = (rev[i>>1]>>1) | ((i&1)<<(k-1));
  }
  if ((int)roots.size() < n) {
    int k = 1; while ((1<<k) < (int)roots.size()) ++k;
    roots.resize(n);
    const double PI = 3.141592653589793238462643383279502884;
    for (; (1<<k) < n; ++k) {
      double ang = 2 * PI / (1<<(k+1));
      for (int i=1<<(k-1); i<(1<<k); ++i) {
        roots[2*i] = roots[i];
        roots[2*i+1] = std::polar(1.0, ang*(2*i+1-(1<<k)));
      }
    }
  }
  for (int i=0;i<n;i++) if (i<rev[i]) std::swap(a[i], a[rev[i]]);
  for (int len=1; len<n; len<<=1) {
    for (int i=0; i<n; i+=2*len) {
      for (int j=0;j<len;++j) {
        cd z = a[i+j+len] * roots[j+len];
        a[i+j+len] = a[i+j] - z;
        a[i+j] += z;
      }
    }
  }
  if (invert) {
    for (int l=1, r=n-1; l<r; ++l, --r) { auto tmp=a[l]; a[l]=a[r]; a[r]=tmp; }
    for (int i=0;i<n;i++) a[i] /= n;
  }
}
inline std::vector<int> multiply_fft(const std::vector<int> &a, const std::vector<int> &b) {
  if (a.empty() || b.empty()) return {};
  if (std::min(a.size(), b.size()) < 32) {
    std::vector<long long> tmp(a.size()+b.size(), 0);
    for (size_t i=0;i<a.size();++i) {
      long long carry=0;
      for (size_t j=0;j<b.size()||carry;++j) {
        long long cur = tmp[i+j] + carry + 1LL*a[i]*(j<b.size()?b[j]:0);
        tmp[i+j] = cur % BASE; carry = cur / BASE;
      }
    }
    std::vector<int> res; res.reserve(tmp.size());
    for (auto v: tmp) res.push_back((int)v);
    trim(res); return res;
  }
  int n=1; while (n < (int)a.size() + (int)b.size()) n <<= 1;
  std::vector<cd> fa(n), fb(n);
  for (size_t i=0;i<a.size();++i) fa[i] = a[i];
  for (size_t i=0;i<b.size();++i) fb[i] = b[i];
  fft(fa, false); fft(fb, false);
  for (int i=0;i<n;i++) fa[i] *= fb[i];
  fft(fa, true);
  std::vector<int> res(n);
  long long carry=0;
  for (int i=0;i<n;i++) {
    double val = fa[i].real();
    long long t = (long long)(val + (val >= 0 ? 0.5 : -0.5)) + carry;
    res[i] = (int)(t % BASE);
    carry = t / BASE;
  }
  while (carry) { res.push_back((int)(carry % BASE)); carry/=BASE; }
  trim(res); return res;
}
inline void mul_small_inplace(std::vector<int> &a, int m) {
  long long carry=0;
  for (size_t i=0;i<a.size();++i) {
    long long cur = 1LL*a[i]*m + carry;
    a[i] = (int)(cur % BASE); carry = cur / BASE;
  }
  if (carry) a.push_back((int)carry);
  trim(a);
}
}

// Constructors
int2048::int2048() : neg(false) {}
int2048::int2048(long long v) : neg(false) {
  if (v<0) { neg=true; v = -v; }
  unsigned long long t = (unsigned long long)v;
  while (t) { d.push_back((int)(t % BASE)); t /= BASE; }
}
int2048::int2048(const std::string &s) : neg(false) { read(s); }
int2048::int2048(const int2048 &o) : neg(o.neg), d(o.d) {}

void int2048::read(const std::string &s) {
  neg=false; d.clear();
  int i=0; while (i<(int)s.size() && (unsigned char)s[i] <= ' ') ++i;
  bool n=false; if (i<(int)s.size() && (s[i]=='+'||s[i]=='-')) { n = s[i]=='-'; ++i; }
  while (i<(int)s.size() && s[i]=='0') ++i;
  std::vector<int> chunks; chunks.reserve((s.size()-i+BASE_DIG-1)/BASE_DIG);
  for (int j=(int)s.size(); j>i; j-=BASE_DIG) {
    int l = j-BASE_DIG; if (l<i) l=i;
    int val=0; for (int k=l;k<j;++k) val = val*10 + (s[k]-'0');
    chunks.push_back(val);
  }
  d.swap(chunks); trim(d); neg = n && !d.empty();
}

void int2048::print() {
  if (d.empty()) { std::cout << 0; return; }
  if (neg) std::cout << '-';
  std::cout << d.back();
  for (int i=(int)d.size()-2;i>=0;--i) {
    int v=d[i];
    if (v<100) { if (v<10) std::cout << "00"; else std::cout << '0'; }
    std::cout << v;
  }
}

int2048 &int2048::add(const int2048 &b) { return (*this += b); }
int2048 add(int2048 a, const int2048 &b) { a += b; return a; }
int2048 &int2048::minus(const int2048 &b) { return (*this -= b); }
int2048 minus(int2048 a, const int2048 &b) { a -= b; return a; }

int2048 int2048::operator+() const { return *this; }
int2048 int2048::operator-() const { int2048 r(*this); if (!r.d.empty()) r.neg = !r.neg; return r; }

int2048 &int2048::operator=(const int2048 &o) { if (this!=&o) { neg=o.neg; d=o.d; } return *this; }

int2048 &int2048::operator+=(const int2048 &b) {
  if (neg == b.neg) { d = add_vec(d, b.d); }
  else {
    int c = cmp_vec(d, b.d);
    if (c==0) { d.clear(); neg=false; }
    else if (c>0) { d = sub_vec(d, b.d); }
    else { auto t = sub_vec(b.d, d); d.swap(t); neg = b.neg; }
  }
  if (d.empty()) neg=false; return *this;
}
int2048 operator+(int2048 a, const int2048 &b) { a += b; return a; }

int2048 &int2048::operator-=(const int2048 &b) {
  int2048 nb = b; if (!nb.d.empty()) nb.neg = !nb.neg; *this += nb; return *this;
}
int2048 operator-(int2048 a, const int2048 &b) { a -= b; return a; }

int2048 &int2048::operator*=(const int2048 &b) {
  if (d.empty() || b.d.empty()) { d.clear(); neg=false; return *this; }
  d = multiply_fft(d, b.d); neg = (neg ^ b.neg); if (d.empty()) neg=false; return *this;
}
int2048 operator*(int2048 a, const int2048 &b) { a *= b; return a; }

static void divmod_abs_vec(const std::vector<int> &a, const std::vector<int> &b, std::vector<int> &q, std::vector<int> &r) {
  r.clear(); q.assign(a.size(), 0);
  if (b.empty()) return;
  if (cmp_vec(a,b) < 0) { r = a; q.clear(); return; }
  r.clear();
  for (int i=(int)a.size()-1; i>=0; --i) {
    // r = r * BASE + a[i]  (shift by one limb and insert a[i] at lowest position)
    r.insert(r.begin(), a[i]);
    trim(r);
    int low=0, high=BASE-1, best=0;
    while (low<=high) {
      int mid=(low+high)>>1;
      std::vector<int> prod = b; mul_small_inplace(prod, mid);
      int cmp = cmp_vec(prod, r);
      if (cmp<=0) { best=mid; low=mid+1; }
      else high=mid-1;
    }
    q[i] = best;
    if (best) {
      std::vector<int> prod = b; mul_small_inplace(prod, best);
      // r -= prod
      int carry2=0;
      for (size_t j=0;j<r.size();++j) {
        long long cur = (long long)r[j] - (j<prod.size()?prod[j]:0) - carry2;
        if (cur<0) { cur += BASE; carry2=1; } else carry2=0;
        r[j] = (int)cur;
      }
      trim(r);
    }
  }
  trim(q); trim(r);
}

int2048 &int2048::operator/=(const int2048 &b) {
  if (b.d.empty()) return *this; // undefined
  std::vector<int> q,rvec; std::vector<int> aa=d, bb=b.d;
  divmod_abs_vec(aa, bb, q, rvec);
  bool same = (neg == b.neg);
  if (!same && !rvec.empty()) {
    // floor adjustment for negative result with non-zero remainder
    int carry=1; for (size_t i=0;i<q.size()||carry;++i) {
      if (i==q.size()) q.push_back(0);
      int cur = q[i] + carry; if (cur>=BASE) { q[i]=cur-BASE; carry=1; } else { q[i]=cur; carry=0; }
    }
  }
  d.swap(q); neg = (!same && !d.empty()); if (d.empty()) neg=false; return *this;
}
int2048 operator/(int2048 a, const int2048 &b) { a /= b; return a; }

int2048 &int2048::operator%=(const int2048 &b) {
  int2048 q = (*this) / b;
  int2048 t = q * b;
  *this -= t;
  return *this;
}
int2048 operator%(int2048 a, const int2048 &b) { a %= b; return a; }

std::istream &operator>>(std::istream &is, int2048 &x) {
  std::string s; is >> s; x.read(s); return is;
}
std::ostream &operator<<(std::ostream &os, const int2048 &x) {
  if (x.d.empty()) { os << 0; return os; }
  if (x.neg) os << '-';
  os << x.d.back();
  for (int i=(int)x.d.size()-2;i>=0;--i) {
    int v = x.d[i];
    if (v<100) { if (v<10) os << "00"; else os << '0'; }
    os << v;
  }
  return os;
}

bool operator==(const int2048 &a, const int2048 &b) {
  if (a.neg != b.neg) return a.d.empty() && b.d.empty();
  return a.d == b.d;
}
bool operator!=(const int2048 &a, const int2048 &b) { return !(a==b); }
bool operator<(const int2048 &a, const int2048 &b) {
  if (a.neg != b.neg) return a.neg && !a.d.empty();
  int cmp = cmp_vec(a.d, b.d);
  return a.neg ? (cmp>0) : (cmp<0);
}
bool operator>(const int2048 &a, const int2048 &b) { return b<a; }
bool operator<=(const int2048 &a, const int2048 &b) { return !(b<a); }
bool operator>=(const int2048 &a, const int2048 &b) { return !(a<b); }

} // namespace sjtu
