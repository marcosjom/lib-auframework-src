
#include "NBColor.h"

bool operator==(const NBColorF &one, const NBColorF &otro) {
    return (  one.r == otro.r && one.g == otro.g && one.b == otro.b && one.a == otro.a );
}
bool operator!=(const NBColorF &one, const NBColorF &otro) {
    return !( one.r == otro.r && one.g == otro.g && one.b == otro.b && one.a == otro.a );
}
bool operator<(const NBColorF &one, const NBColorF &otro) {
    return !( one.r < otro.r || (one.r == otro.r && one.g < otro.g) || (one.r == otro.r && one.g == otro.g && one.b < otro.b) || (one.r == otro.r && one.g == otro.g && one.b == otro.b && one.a < otro.a));
}
bool operator<=(const NBColorF &one, const NBColorF &otro) {
    return !( one.r < otro.r || (one.r == otro.r && one.g < otro.g) || (one.r == otro.r && one.g == otro.g && one.b < otro.b) || (one.r == otro.r && one.g == otro.g && one.b == otro.b && one.a <= otro.a));
}
bool operator>(const NBColorF &one, const NBColorF &otro) {
    return !( one.r > otro.r || (one.r == otro.r && one.g > otro.g) || (one.r == otro.r && one.g == otro.g && one.b > otro.b) || (one.r == otro.r && one.g == otro.g && one.b == otro.b && one.a > otro.a));
}
bool operator>=(const NBColorF &one, const NBColorF &otro) {
    return !( one.r > otro.r || (one.r == otro.r && one.g > otro.g) || (one.r == otro.r && one.g == otro.g && one.b > otro.b) || (one.r == otro.r && one.g == otro.g && one.b == otro.b && one.a >= otro.a));
}

bool operator==(const NBColor8 &one, const NBColor8 &otro) {
    return (  one.r == otro.r && one.g == otro.g && one.b == otro.b && one.a == otro.a );
}
bool operator!=(const NBColor8 &one, const NBColor8 &otro) {
    return !( one.r == otro.r && one.g == otro.g && one.b == otro.b && one.a == otro.a );
}
bool operator<(const NBColor8 &one, const NBColor8 &otro) {
    return !( one.r < otro.r || (one.r == otro.r && one.g < otro.g) || (one.r == otro.r && one.g == otro.g && one.b < otro.b) || (one.r == otro.r && one.g == otro.g && one.b == otro.b && one.a < otro.a));
}
bool operator<=(const NBColor8 &one, const NBColor8 &otro) {
    return !( one.r < otro.r || (one.r == otro.r && one.g < otro.g) || (one.r == otro.r && one.g == otro.g && one.b < otro.b) || (one.r == otro.r && one.g == otro.g && one.b == otro.b && one.a <= otro.a));
}
bool operator>(const NBColor8 &one, const NBColor8 &otro) {
    return !( one.r > otro.r || (one.r == otro.r && one.g > otro.g) || (one.r == otro.r && one.g == otro.g && one.b > otro.b) || (one.r == otro.r && one.g == otro.g && one.b == otro.b && one.a > otro.a));
}
bool operator>=(const NBColor8 &one, const NBColor8 &otro) {
    return !( one.r > otro.r || (one.r == otro.r && one.g > otro.g) || (one.r == otro.r && one.g == otro.g && one.b > otro.b) || (one.r == otro.r && one.g == otro.g && one.b == otro.b && one.a >= otro.a));
}

