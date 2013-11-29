#ifndef MAIN_H_
#define MAIN_H_
// i make make happy
struct color {
float r,g,b;
    color(float r, float g, float b) : r(r), g(g), b(b) {};
    color() : r(0), g(0), b(0) {};
};

inline color operator +(const color& a, const color &b ) {
    return color(a.r+b.r, a.g+b.g, a.b+b.b);
}
inline color operator -(const color& a, const color &b ) {
    return color(a.r-b.r, a.g-b.g, a.b-b.b);
}
inline color operator +=(color& a, const color &b ) {
    a.r += b.r;
    a.g +=b.g;
    a.b +=b.b;
    return a;
}
inline color operator -=(color& a, const color &b ) {
    a.r -= b.r;
    a.g -=b.g;
    a.b -=b.b;
    return a;
}
inline color operator *(const color& a, const float &b ) {
    return color(a.r* b, a.g* b, a.b* b);
}

struct cell {
    color c;
    int cnt;
    cell() : c(0,0,0), cnt(0) {};
    void reset() { c = color(); cnt = 0;};
    color value() {
        return c * (1.0f/(float) cnt);
    };
    void addColor(const color _c) {
        c = c + _c;
        cnt++;
    }
    void subColor(const color _c) {
        c = c - _c;
        cnt--;
    }
};

inline cell operator +=(cell& a, const cell &b ) {
    a.cnt += b.cnt;
    a.c   += b.c;
    return a;
}
inline cell operator -=(cell& a, const cell &b ) {
    a.cnt -= b.cnt;
    a.c   -= b.c;
    return a;
}

struct ACC {
    cell enter, exit;
    void reset() { enter.reset(); exit.reset();};
};

struct test_case {
    color c;
    float x,y,r;
    void init() {
        c = color(drand48(), drand48(), drand48());
        x= drand48(); y= drand48(); r = 0.1 * drand48();
    }
    inline bool operator <(const test_case& o) const {
        // sort by y
        return y < o.y;
    }
};

#endif /* !MAIN_H_ */
