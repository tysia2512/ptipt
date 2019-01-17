// g++ -g -O2 -std=c++11 -pthread -march=native aks.cpp -o aks -lntl -lgmp -lm

#include <iostream>
#include <algorithm>
#include <vector>
#include <NTL/ZZ.h>
#include <NTL/ZZX.h>
using namespace std;
using namespace NTL;

enum RESULT{PRIME, COMPOSITE};
const ZZ not_found(-1);

ZZ log_floor(const ZZ& n) {
    ZZ p(1), ret(0);
    while (p < n) {
        p *= 2;
        ret++;
    }
    if (p > n)
        return ret - 1;
    return ret;
}

ZZ gcd(const ZZ& a, const ZZ& b) {
    if (a < b)
        return gcd(b, a);

    if (b == 0)
        return a;
    return gcd(b, a%b);
}

ZZ phi(const ZZ& n) {
    ZZ ret(0);
    for (ZZ p(2); p < n; p++) {
        if (gcd(p, n) == 1) {
            ret++;
        }
    }
    return ret;
}

ZZ find_power(const ZZ& lower, const ZZ& upper, long e, const ZZ& n) {
    ZZ mid = (lower + upper) / 2;

    // nie powinno się wydarzyć, sprawdzić później
    if (upper <= lower) {
        return not_found;
    }
    
    if (lower == mid) {
        if (power(mid, e) == n)
            return mid;
        else 
            return not_found;
    }

    if (power(mid, e) > n)
        return find_power(lower, mid, e, n);

    return find_power(mid, upper, e, n);
    
}

bool is_order_greater(const ZZ& n, const ZZ& mod, const ZZ& value) {
    ZZ pd(n);
    for (ZZ i(1); i <= value; i++) {
        pd %= mod;
        if (pd == 1)
            return false;
        pd *= n;
    }
    return true;
}

ZZ sqrt_ceil(const ZZ& n) {
    ZZ ret(1);
    while(ret * ret < n)
        ret++;
    return ret;
}

ZZX polynomial_mod(const ZZX& f, const ZZ& mod) {
    ZZX res(f);
    for (long i = 0; i <= deg(res); i++) {
        res[i] %= mod;
    }
    return res;
}

ZZX polynomial_power(const ZZX& f, const ZZ& n, const ZZX& mod) {
    ZZ exp_left(n);
    ZZX poly_power(f);
    ZZX res(1);
    while(exp_left > 0) {
        if (exp_left % 2) {
            res *= poly_power;
        }
        poly_power *= poly_power;
        poly_power %= mod;
        res %= mod;
        exp_left /= 2;
        poly_power = polynomial_mod(poly_power, n);
        res = polynomial_mod(res, n);
    }
    return res;
}

RESULT aks(const ZZ& n) {
    // cout << n << "\n";
    ZZ ln_floor(log_floor(n));
    
    for (long b = 2; b <= log_floor(n); b++) {
        if (find_power(ZZ(2), max(n, ZZ(2)), b, n) != not_found) {
            cout << "KROK 1\n";
            return COMPOSITE;
        }
    }

    ZZ ln_ceil(ln_floor + 1);
    long r = 2;
    while (!is_order_greater(n, ZZ(r), ln_ceil * ln_ceil)) {
        r++;
    }
    cout << "r: " << r << "\n";

    for (ZZ a(2); a <= ZZ(r); a++) {
        if (1 < gcd(a, n) && gcd(a, n) < n) {
            cout << "KROK 2: " << a << "\n";
            return COMPOSITE;
        }
    }
    
    if (n <= ZZ(r)) {
        cout << "KROK 3\n";
        return PRIME;
    }
    
    ZZ l(sqrt_ceil(phi(ZZ(r))) * ln_ceil);
    cout << l << "\n";
    for (ZZ a(1); a <= l; a++) {
        cout << a << "\n";
        ZZX mod;
        mod.SetLength(r + 1);
        mod[0] = -1;
        mod[r] = 1;

        // cout << "X^r - 1: " << mod << "\n";

        ZZX left;
        left.SetLength(2);
        left[0] = a;
        left[1] = 1;
        left = polynomial_power(left, n, mod);

        ZZX right;
        SetX(right);
        right = polynomial_power(right, n, mod);
        right[0] += a;
        
        ZZX diff;
        diff = (left - right) % mod;
        diff = polynomial_mod(diff, n);
        diff.normalize();
        if (!IsZero(diff)) {
            cout << "KROK 4\n";
            cout << diff << "\n";
            return COMPOSITE;
        }
    }
    cout << "KROK 5\n";
    return PRIME;
}

int main() {

    ZZ n;
    cin >> n;
    cout << aks(n) << "\n";

}