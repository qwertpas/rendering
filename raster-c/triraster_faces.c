
#include <stdio.h>

int pt_in_tri(float p[2], float p0[2], float p1[2], float p2[2]) {
    float s = (p0[0] - p2[0]) * (p[1] - p2[1]) - (p0[1] - p2[1]) * (p[0] - p2[0]);
    float t = (p1[0] - p0[0]) * (p[1] - p0[1]) - (p1[1] - p0[1]) * (p[0] - p0[0]);

    if ((s < 0) != (t < 0) && s != 0 && t != 0) {
        return 0;
    }

    float d = (p2[0] - p1[0]) * (p[1] - p1[1]) - (p2[1] - p1[1]) * (p[0] - p1[0]);
    return d == 0 || (d < 0) == (s + t <= 0);
}


int main() {
    float p[2] = {0.1, 0.1};
    float p0[2] = {0.0, 0.0};
    float p1[2] = {1.0, 0.0};
    float p2[2] = {0.0, 1.0};

    int result = pt_in_tri(p, p0, p1, p2);
    
    if (result) {
        printf("Point is inside the triangle.\n");
    } else {
        printf("Point is outside the triangle.\n");
    }

    return 0;
}