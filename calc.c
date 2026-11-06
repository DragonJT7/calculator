#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* ---------- integer functions (saturate on overflow) ---------- */

int add(int a, int b) {
    if (b > 0 && a > INT_MAX - b) return INT_MAX;
    if (b < 0 && a < INT_MIN - b) return INT_MIN;
    return a + b;
}

int sub(int a, int b) {
    if (b < 0 && a > INT_MAX + b) return INT_MAX;
    if (b > 0 && a < INT_MIN + b) return INT_MIN;
    return a - b;
}

int mul(int a, int b) {
    long long product = (long long)a * b;
    if (product > INT_MAX) return INT_MAX;
    if (product < INT_MIN) return INT_MIN;
    return (int)product;
}

/* Division by zero returns 0. INT_MIN / -1 saturates to INT_MAX. */
int divide(int a, int b) {
    if (b == 0) return 0;
    if (a == INT_MIN && b == -1) return INT_MAX;
    return a / b;
}

/* Negative exponents return 0 (truncated), except for bases 1 and -1. */
int power(int base, int exp) {
    int result = 1;
    int i;

    if (exp == 0) return 1;
    if (base == 0) return 0;
    if (base == 1) return 1;
    if (base == -1) return (exp % 2 == 0) ? 1 : -1;
    if (exp < 0) return 0;
    /* |base| >= 2, so 2^32 or more always overflows */
    if (exp >= 32) return (base < 0 && exp % 2 != 0) ? INT_MIN : INT_MAX;

    for (i = 0; i < exp; i++) {
        result = mul(result, base);
    }
    return result;
}

/* ---------- floating point functions ---------- */

double addf(double a, double b)    { return a + b; }
double subf(double a, double b)    { return a - b; }
double mulf(double a, double b)    { return a * b; }
double dividef(double a, double b) { return (b == 0.0) ? NAN : a / b; }
double powerf(double a, double b)  { return pow(a, b); }

/* ---------- calculate() ---------- */

static const char *skip_spaces(const char *s) {
    while (isspace((unsigned char)*s)) s++;
    return s;
}

/* Reads a number such as 7, -3, +4.2 or 0.5 starting at *s.
   On success, advances *s past it and returns 1. */
static int read_number(const char **s, double *value, long long *ivalue,
                       int *is_float) {
    const char *p = skip_spaces(*s);
    const char *start = p;
    int digits = 0;
    char buf[64];
    size_t len;

    *is_float = 0;
    if (*p == '+' || *p == '-') p++;
    while (isdigit((unsigned char)*p)) { p++; digits++; }
    if (*p == '.') {
        *is_float = 1;
        p++;
        while (isdigit((unsigned char)*p)) { p++; digits++; }
    }
    if (digits == 0) return 0;

    len = (size_t)(p - start);
    if (len >= sizeof buf) return 0;
    memcpy(buf, start, len);
    buf[len] = '\0';

    *value = strtod(buf, NULL);
    *ivalue = strtoll(buf, NULL, 10);
    *s = p;
    return 1;
}

const char *calculate(const char *expr) {
    static char out[64];
    const char *p;
    double fa, fb, fr;
    long long ia, ib;
    int a_float, b_float, use_float;
    char op;

    if (expr == NULL) return "error";
    p = expr;

    if (!read_number(&p, &fa, &ia, &a_float)) return "error";
    p = skip_spaces(p);
    op = *p;
    if (op != '+' && op != '-' && op != '*' && op != '/' && op != '^')
        return "error";
    p++;
    if (!read_number(&p, &fb, &ib, &b_float)) return "error";
    p = skip_spaces(p);
    if (*p != '\0') return "error";

    use_float = a_float || b_float || (op == '^' && ib < 0);

    if (use_float) {
        switch (op) {
            case '+': fr = addf(fa, fb);    break;
            case '-': fr = subf(fa, fb);    break;
            case '*': fr = mulf(fa, fb);    break;
            case '/': fr = dividef(fa, fb); break;
            default:  fr = powerf(fa, fb);  break;
        }
        if (!isfinite(fr)) return "error";
        snprintf(out, sizeof out, "%.10g", fr);
        if (strpbrk(out, ".e") == NULL) strcat(out, ".0");
        return out;
    } else {
        int r;
        if (ia < INT_MIN || ia > INT_MAX || ib < INT_MIN || ib > INT_MAX)
            return "error";
        if (op == '/' && ib == 0) return "error";
        switch (op) {
            case '+': r = add((int)ia, (int)ib);    break;
            case '-': r = sub((int)ia, (int)ib);    break;
            case '*': r = mul((int)ia, (int)ib);    break;
            case '/': r = divide((int)ia, (int)ib); break;
            default:  r = power((int)ia, (int)ib);  break;
        }
        snprintf(out, sizeof out, "%d", r);
        return out;
    }
}
