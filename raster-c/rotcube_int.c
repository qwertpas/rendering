#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} BMPHeader;

typedef struct {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size;
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t important_colors;
} BMPInfoHeader;
#pragma pack(pop)

void write_bmp(const char *filename, uint8_t *data, int width, int height) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    BMPHeader header = {0x4D42, sizeof(BMPHeader) + sizeof(BMPInfoHeader) + width * height * 3, 0, 0, sizeof(BMPHeader) + sizeof(BMPInfoHeader)};
    BMPInfoHeader info_header = {sizeof(BMPInfoHeader), width, height, 1, 24, 0, width * height * 3, 0, 0, 0, 0};

    fwrite(&header, sizeof(BMPHeader), 1, file);
    fwrite(&info_header, sizeof(BMPInfoHeader), 1, file);
    fwrite(data, sizeof(uint8_t), width * height * 3, file);

    fclose(file);
}

int pt_in_tri(int16_t p[2], int16_t p0[2], int16_t p1[2], int16_t p2[2]) {
    int16_t s = (p0[0] - p2[0]) * (p[1] - p2[1]) - (p0[1] - p2[1]) * (p[0] - p2[0]);
    //            r0    - r4    *  r7    - r5    -  r1     -  r5   * r6    - r4
    int16_t t = (p1[0] - p0[0]) * (p[1] - p0[1]) - (p1[1] - p0[1]) * (p[0] - p0[0]);
    //           r2     - r0    *  r7   - r1     -   r3    - r1    *  r6   -  r0
    if ((s < 0) != (t < 0) && s != 0 && t != 0) {
        return 0;
    }
    int16_t d = (p2[0] - p1[0]) * (p[1] - p1[1]) - (p2[1] - p1[1]) * (p[0] - p1[0]);
            //    r4    - r2     *   r7   - r3    -    r5    - r3   *  r6    - r2
    return d == 0 || (d < 0) == (s + t <= 0);
}

int max3(int a, int b, int c){
    return (a>=b&&a>=c)?a:(b>=c)?b:c;
}

int min3(int a, int b, int c){
    return (a<=b&&a<=c)?a:(b<=c)?b:c;
}

float fmin3(float a, float b, float c){
    return (a<=b&&a<=c)?a:(b<=c)?b:c;
}

//cube
float vertices_unrot[] = {
    -1.554023265838623, -3.6070477962493896, 6.780168056488037,
    -1.2657760381698608, -5.52766752243042, 2.1892383098602295,
    -0.36034271121025085, -0.10692957788705826, 7.344461917877197,
    0.12298047542572021, 4.180109977722168, 0.12289828807115555,
    -7.1773786544799805, -0.24774070084095, 4.795849323272705,
    5.517859935760498, 1.0702383518218994, 3.6172149181365967,
    7.715725421905518, 0.3959411382675171, 6.204093933105469,
    -0.34558969736099243, 1.185247778892517, 8.393171310424805,
    -1.9347853660583496, 3.6259677410125732, 6.823418140411377,
    0.7923155426979065, -3.722553014755249, 0.08501292020082474,
    -2.3268072605133057, -3.4187676906585693, 0.3264550268650055,
    1.9781111478805542, 5.53239631652832, 2.050018310546875,
    -5.271108627319336, 0.4879162311553955, 1.504428744316101,
    -4.045803546905518, 1.2834076881408691, 0.12626740336418152,
    0.24653145670890808, 4.062447547912598, 6.672523498535156,
    1.1270792484283447, 0.47046270966529846, 8.422286033630371,
    6.6268086433410645, 1.035324215888977, 3.4297780990600586,
    7.633116722106934, -0.5822495222091675, 6.218839645385742,
    1.7216330766677856, -5.390579700469971, 2.135227680206299,
    3.3747334480285645, -2.29473876953125, 6.721019268035889,
    5.5397844314575195, -0.06766846776008606, 3.7994115352630615,
    -3.909205198287964, -4.092564105987549, 2.135376214981079,
    -7.776984691619873, -0.09651803970336914, 4.038656711578369,
    4.21771240234375, -3.846834897994995, 2.1353771686553955,
    -0.014792372472584248, 0.5443404912948608, 7.358057498931885,
    -7.516426086425781, -0.32713398337364197, 5.603129863739014,
    -7.476844787597656, 0.2853914797306061, 5.641313076019287,
    8.567665100097656, -0.05415871739387512, 6.1772871017456055,
    0.40836694836616516, -0.9977337718009949, 8.393171310424805,
    -5.7537407875061035, 0.2595020532608032, 2.5770673751831055,
    5.468304634094238, 0.8529198169708252, 1.8739209175109863,
    -4.154933452606201, 4.034353733062744, 2.135378122329712,
    3.7206242084503174, 2.001966714859009, 0.1907617747783661,
    -4.053524971008301, -0.7890342473983765, 6.700836181640625,
    -4.932112216949463, 0.07215619087219238, 5.534263610839844,
    4.228124618530273, 3.779486894607544, 3.2380001544952393,
    -3.899946451187134, 0.9197883009910583, 6.663968563079834,
    5.649521827697754, -1.3076125383377075, 2.163717031478882,
    -1.6588534116744995, 5.578097343444824, 2.1352314949035645,
    -5.474849224090576, -0.5823951363563538, 1.8360265493392944,
    3.2999842166900635, 2.5337939262390137, 6.707733154296875,
    3.4712207317352295, -2.2188260555267334, 0.21603001654148102,
    -3.589770793914795, -1.5925722122192383, 6.708871841430664,
    8.028903007507324, 0.007895305752754211, 5.79894495010376,
    0.8639388680458069, -3.762444019317627, 6.673946857452393,
    -1.060101866722107, -0.2832212746143341, 8.393171310424805,
    0.4433821141719818, -0.1041593849658966, 7.345036029815674,
};
const int n_vertices = sizeof(vertices_unrot) / (3 * sizeof(float));
// uint8_t faces[] = {
//     15, 28, 46, 36, 149, 113,
//     45, 28, 15, 42, 10, 220,
//     46, 24, 15, 80, 71, 178,
//     7, 15, 24, 31, 161, 187,
//     2, 46, 28, 25, 29, 184,
//     19, 40, 24, 121, 58, 207,
//     24, 46, 19, 139, 223, 96,
//     28, 45, 2, 133, 32, 173,
//     7, 45, 15, 44, 192, 176,
//     44, 19, 46, 188, 162, 87,
//     24, 2, 7, 110, 148, 223,
//     40, 14, 24, 38, 0, 210,
//     46, 2, 44, 40, 174, 226,
//     45, 7, 2, 94, 160, 59,
//     42, 44, 2, 79, 74, 241,
//     14, 36, 24, 74, 157, 12,
//     36, 42, 2, 86, 107, 230,
//     2, 24, 36, 217, 59, 207,
//     4, 26, 34, 91, 221, 195,
//     36, 34, 26, 80, 147, 190,
//     36, 29, 34, 153, 185, 30,
//     33, 34, 42, 248, 166, 12,
//     33, 4, 34, 97, 143, 48,
//     4, 33, 25, 154, 127, 40,
//     31, 29, 36, 134, 56, 66,
//     34, 21, 42, 167, 53, 188,
//     36, 8, 31, 115, 236, 134,
//     29, 39, 34, 152, 71, 34,
//     0, 42, 21, 108, 103, 236,
//     38, 31, 8, 117, 45, 191,
//     21, 1, 0, 129, 50, 29,
//     33, 36, 26, 165, 51, 173,
//     26, 25, 33, 63, 168, 17,
//     11, 38, 8, 209, 204, 28,
//     8, 14, 11, 68, 179, 130,
//     21, 34, 39, 114, 41, 141,
//     1, 18, 44, 49, 221, 102,
//     44, 0, 1, 169, 103, 18,
//     31, 12, 29, 72, 45, 127,
//     19, 44, 18, 112, 156, 73,
//     35, 11, 14, 128, 82, 42,
//     14, 40, 35, 143, 5, 170,
//     12, 22, 29, 56, 117, 169,
//     39, 12, 13, 238, 180, 16,
//     18, 23, 19, 197, 137, 49,
//     13, 10, 21, 221, 243, 104,
//     21, 39, 13, 49, 135, 155,
//     39, 22, 12, 85, 222, 171,
//     12, 31, 13, 87, 127, 16,
//     8, 36, 14, 221, 234, 113,
//     20, 19, 23, 249, 63, 36,
//     25, 26, 22, 49, 165, 59,
//     5, 35, 40, 154, 18, 46,
//     1, 21, 10, 201, 117, 79,
//     4, 29, 26, 95, 167, 222,
//     4, 25, 22, 199, 167, 3,
//     3, 13, 31, 248, 187, 50,
//     31, 38, 3, 43, 111, 130,
//     0, 44, 42, 31, 113, 230,
//     39, 29, 4, 210, 15, 196,
//     20, 5, 19, 242, 147, 52,
//     4, 22, 39, 149, 193, 59,
//     22, 26, 29, 70, 171, 236,
//     40, 19, 5, 182, 107, 250,
//     10, 9, 18, 65, 240, 59,
//     18, 1, 10, 157, 236, 15,
//     20, 23, 37, 197, 149, 76,
//     5, 30, 35, 182, 231, 45,
//     32, 11, 35, 131, 134, 54,
//     38, 11, 3, 2, 117, 198,
//     30, 32, 35, 107, 192, 72,
//     9, 41, 23, 77, 163, 95,
//     23, 18, 9, 57, 121, 238,
//     13, 9, 10, 207, 97, 191,
//     5, 20, 16, 140, 46, 181,
//     32, 3, 11, 46, 245, 45,
//     3, 9, 13, 79, 66, 168,
//     37, 23, 41, 87, 218, 218,
//     16, 30, 5, 127, 14, 67,
//     30, 16, 37, 141, 45, 50,
//     37, 17, 20, 210, 96, 51,
//     41, 32, 30, 17, 198, 137,
//     30, 37, 41, 165, 195, 2,
//     37, 43, 17, 193, 95, 23,
//     20, 17, 6, 195, 34, 231,
//     6, 16, 20, 85, 221, 197,
//     43, 37, 16, 221, 72, 9,
//     16, 6, 43, 69, 166, 246,
//     27, 17, 43, 72, 134, 38,
//     32, 41, 9, 3, 232, 61,
//     9, 3, 32, 159, 65, 142,
//     17, 27, 6, 10, 183, 18,
//     43, 6, 27, 206, 31, 32,
//     42, 36, 33, 96, 215, 49,
// };

uint8_t faces[] = { //porcelain color #EBF1F4
    15, 28, 46, 235, 241, 244,
    45, 28, 15, 235, 241, 244,
    46, 24, 15, 235, 241, 244,
    7, 15, 24, 235, 241, 244,
    2, 46, 28, 235, 241, 244,
    19, 40, 24, 235, 241, 244,
    24, 46, 19, 235, 241, 244,
    28, 45, 2, 235, 241, 244,
    7, 45, 15, 235, 241, 244,
    44, 19, 46, 235, 241, 244,
    24, 2, 7, 235, 241, 244,
    40, 14, 24, 235, 241, 244,
    46, 2, 44, 235, 241, 244,
    45, 7, 2, 235, 241, 244,
    42, 44, 2, 235, 241, 244,
    14, 36, 24, 235, 241, 244,
    36, 42, 2, 235, 241, 244,
    2, 24, 36, 235, 241, 244,
    4, 26, 34, 235, 241, 244,
    36, 34, 26, 235, 241, 244,
    36, 29, 34, 235, 241, 244,
    33, 34, 42, 235, 241, 244,
    33, 4, 34, 235, 241, 244,
    4, 33, 25, 235, 241, 244,
    31, 29, 36, 235, 241, 244,
    34, 21, 42, 235, 241, 244,
    36, 8, 31, 235, 241, 244,
    29, 39, 34, 235, 241, 244,
    0, 42, 21, 235, 241, 244,
    38, 31, 8, 235, 241, 244,
    21, 1, 0, 235, 241, 244,
    33, 36, 26, 235, 241, 244,
    26, 25, 33, 235, 241, 244,
    11, 38, 8, 235, 241, 244,
    8, 14, 11, 235, 241, 244,
    21, 34, 39, 235, 241, 244,
    1, 18, 44, 235, 241, 244,
    44, 0, 1, 235, 241, 244,
    31, 12, 29, 235, 241, 244,
    19, 44, 18, 235, 241, 244,
    35, 11, 14, 235, 241, 244,
    14, 40, 35, 235, 241, 244,
    12, 22, 29, 235, 241, 244,
    39, 12, 13, 235, 241, 244,
    18, 23, 19, 235, 241, 244,
    13, 10, 21, 235, 241, 244,
    21, 39, 13, 235, 241, 244,
    39, 22, 12, 235, 241, 244,
    12, 31, 13, 235, 241, 244,
    8, 36, 14, 235, 241, 244,
    20, 19, 23, 235, 241, 244,
    25, 26, 22, 235, 241, 244,
    5, 35, 40, 235, 241, 244,
    1, 21, 10, 235, 241, 244,
    4, 29, 26, 235, 241, 244,
    4, 25, 22, 235, 241, 244,
    3, 13, 31, 235, 241, 244,
    31, 38, 3, 235, 241, 244,
    0, 44, 42, 235, 241, 244,
    39, 29, 4, 235, 241, 244,
    20, 5, 19, 235, 241, 244,
    4, 22, 39, 235, 241, 244,
    22, 26, 29, 235, 241, 244,
    40, 19, 5, 235, 241, 244,
    10, 9, 18, 235, 241, 244,
    18, 1, 10, 235, 241, 244,
    20, 23, 37, 235, 241, 244,
    5, 30, 35, 235, 241, 244,
    32, 11, 35, 235, 241, 244,
    38, 11, 3, 235, 241, 244,
    30, 32, 35, 235, 241, 244,
    9, 41, 23, 235, 241, 244,
    23, 18, 9, 235, 241, 244,
    13, 9, 10, 235, 241, 244,
    5, 20, 16, 235, 241, 244,
    32, 3, 11, 235, 241, 244,
    3, 9, 13, 235, 241, 244,
    37, 23, 41, 235, 241, 244,
    16, 30, 5, 235, 241, 244,
    30, 16, 37, 235, 241, 244,
    37, 17, 20, 235, 241, 244,
    41, 32, 30, 235, 241, 244,
    30, 37, 41, 235, 241, 244,
    37, 43, 17, 235, 241, 244,
    20, 17, 6, 235, 241, 244,
    6, 16, 20, 235, 241, 244,
    43, 37, 16, 235, 241, 244,
    16, 6, 43, 235, 241, 244,
    27, 17, 43, 235, 241, 244,
    32, 41, 9, 235, 241, 244,
    9, 3, 32, 235, 241, 244,
    17, 27, 6, 235, 241, 244,
    43, 6, 27, 235, 241, 244,
    42, 36, 33, 235, 241, 244,
};
const int n_faces = sizeof(faces) / (6 * sizeof(uint8_t));
uint8_t face_colors[n_faces * 3] = {0};

const int width = 640;
const int height = 480;
uint8_t do_perspective = 1;




int main() {

    printf("nfaces %d\n", n_faces);

    clock_t begin = clock();

    int nsteps = 100;
    for(int step = 0; step < nsteps; step++){
        // float roll = 0;
        // float pitch = step*6.28/nsteps + 0.01;
        // float yaw = 2*step*6.28/nsteps + 0.3;

        float roll = 3.14;
        float pitch = step*6.28/nsteps*5 + 0.01;
        float yaw = 1.1;



        float offset_x = width/2;
        float offset_y = height/2 + 50;
        // float offset_x = width/2;
        // float offset_y = height/2 + 50;
        float scale = 800;


        float xdist = 10*(0.5*sin(6.28*step/nsteps));
        // float ydist = 0;
        float ydist = 10*(0.5*sin(6.28*step/nsteps + 2));
        float zdist = 30 + 20*(1 + 0.5*sin(6.28*step/nsteps + 2));

        float light[] = {300, 300, 0};

        //rotate vertices
        float R[] = {
            cos(roll)*cos(pitch), cos(roll)*sin(pitch)*sin(yaw)-sin(roll)*cos(yaw), cos(roll)*sin(pitch)*cos(yaw)+sin(roll)*sin(yaw),
            sin(roll)*cos(pitch), sin(roll)*sin(pitch)*sin(yaw)+cos(roll)*cos(yaw), sin(roll)*sin(pitch)*cos(yaw)-cos(roll)*sin(yaw),
            -sin(pitch), cos(pitch)*sin(yaw), cos(pitch)*cos(yaw)
        };
        float vertices[n_vertices*3] = {0}; //rotated version
        for(int i=0; i<n_vertices; i++){
            float vx = vertices_unrot[3*i + 0] - 0.5;
            float vy = vertices_unrot[3*i + 1] - 0.5;
            float vz = vertices_unrot[3*i + 2] - 0.5;
            vertices[3*i + 0] = R[0]*vx + R[1]*vy + -R[2]*vz + xdist;
            vertices[3*i + 1] = R[3]*vx + R[4]*vy + -R[5]*vz + ydist;
            vertices[3*i + 2] = R[6]*vx + R[7]*vy + -R[8]*vz + zdist;
        }

        // printf("vertices: %d \nfaces: %d\n", n_vertices, n_faces);


        int n_visible_faces = 0;
        float facing_ratio = 1;

        int16_t tri_list[8 * n_faces];
        for (int i = 0; i < n_faces; i++) {
            float v0x = vertices[3 * faces[6 * i + 0] + 0];
            float v0y = vertices[3 * faces[6 * i + 0] + 1];
            float v0z = vertices[3 * faces[6 * i + 0] + 2];

            float v1x = vertices[3 * faces[6 * i + 1] + 0];
            float v1y = vertices[3 * faces[6 * i + 1] + 1];
            float v1z = vertices[3 * faces[6 * i + 1] + 2];

            float v2x = vertices[3 * faces[6 * i + 2] + 0];
            float v2y = vertices[3 * faces[6 * i + 2] + 1];
            float v2z = vertices[3 * faces[6 * i + 2] + 2];

            // Calculate normal
            float edge1x = v1x - v0x;
            float edge1y = v1y - v0y;
            float edge1z = v1z - v0z;
            float edge2x = v2x - v0x;
            float edge2y = v2y - v0y;
            float edge2z = v2z - v0z;
            float nx = edge1y * edge2z - edge1z * edge2y;
            float ny = edge1z * edge2x - edge1x * edge2z;
            float nz = edge1x * edge2y - edge1y * edge2x; //kinda like area
            float norm_length = sqrtf(nx * nx + ny * ny + nz * nz);
            nx /= norm_length;
            ny /= norm_length;
            nz /= norm_length;
            if(nz <= 0){
                continue; //skip this triangle
            }
            
            float v0x_screen = 0;
            float v0y_screen = 0;
            float v1x_screen = 0;
            float v1y_screen = 0;
            float v2x_screen = 0;
            float v2y_screen = 0;
            if(do_perspective){
                float invz0 = 1 / (v0z + 0);
                float invz1 = 1 / (v1z + 0);
                float invz2 = 1 / (v2z + 0);

                v0x_screen = v0x * scale * invz0 + offset_x;
                v0y_screen = v0y * scale * invz0 + offset_y;
                v1x_screen = v1x * scale * invz1 + offset_x;
                v1y_screen = v1y * scale * invz1 + offset_y;
                v2x_screen = v2x * scale * invz2 + offset_x;
                v2y_screen = v2y * scale * invz2 + offset_y;
            }else{
                float ortho_scale = 0.1;
                v0x_screen = v0x * scale*ortho_scale + offset_x;
                v0y_screen = v0y * scale*ortho_scale + offset_y;
                v1x_screen = v1x * scale*ortho_scale + offset_x;
                v1y_screen = v1y * scale*ortho_scale + offset_y;
                v2x_screen = v2x * scale*ortho_scale + offset_x;
                v2y_screen = v2y * scale*ortho_scale + offset_y;
            }

            
            float face_min_z = fmin3(v0z, v1z, v2z);
            // float face_avg = (v0z + v1z + v2z) / 3.;
            // float face_z = 0.99*face_min_z + 0.01*face_avg;

            tri_list[8 * n_visible_faces + 0] = (int16_t) v0x_screen;
            tri_list[8 * n_visible_faces + 1] = (int16_t) v0y_screen;
            tri_list[8 * n_visible_faces + 2] = (int16_t) v1x_screen;
            tri_list[8 * n_visible_faces + 3] = (int16_t) v1y_screen;
            tri_list[8 * n_visible_faces + 4] = (int16_t) v2x_screen;
            tri_list[8 * n_visible_faces + 5] = (int16_t) v2y_screen;

            tri_list[8 * n_visible_faces + 6] = (int16_t) face_min_z; //precacluate
            tri_list[8 * n_visible_faces + 7] = (int16_t) i; //face index

            float avgx = 0.3333*(v0x+v1x+v2x + light[0]);
            float avgy = 0.3333*(v0y+v1y+v2y + light[1]);
            float avgz = 0.3333*(v0z+v1z+v2z + light[2]);
            float avg_length = sqrt(avgx * avgx + avgy * avgy + avgz * avgz);
            avgx /= avg_length;
            avgy /= avg_length;
            avgz /= avg_length;

            facing_ratio = fmaxf(avgx * nx + avgy * ny + avgz * nz, 0.2);
            face_colors[3 * i + 0] = faces[6 * i + 3] * facing_ratio; //b
            face_colors[3 * i + 1] = faces[6 * i + 4] * facing_ratio; //g
            face_colors[3 * i + 2] = faces[6 * i + 5] * facing_ratio; //r

            // printf("changing: %d %d\n", faces[6 * i + 3], face_colors[3 * i + 0]);

            n_visible_faces++;
        }

        printf("step: %d, faces: %d\n", step, n_visible_faces);

        // print out tri_list
        // printf("n_visible_faces: %d\n", n_visible_faces);
        // for (int i = 0; i < sizeof(tri_list) / sizeof(float); i += 8) {
        //     for (int j = 0; j < 8; j++) {
        //         printf("%d ", tri_list[i+j]);
        //     }
        //     printf("\n");
        // }

        //initialize rgb array
        uint8_t image[width * height * 3] = {0};

        // Iterate through each pixel
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {

                float min_z = 1e12;

                for (int i = 0; i < n_visible_faces; i++) {

                    uint8_t is_in_tri = 0;

                    int16_t v0x = tri_list[8*i + 0];
                    int16_t v0y = tri_list[8*i + 1];
                    int16_t v1x = tri_list[8*i + 2];
                    int16_t v1y = tri_list[8*i + 3];
                    int16_t v2x = tri_list[8*i + 4];
                    int16_t v2y = tri_list[8*i + 5];
                    int16_t face_min_z = tri_list[8*i + 6];
                    int16_t face_i = tri_list[8*i + 7];

                    int16_t p[2] = {x, y};
                    int16_t p0[2] = {v0x, v0y};
                    int16_t p1[2] = {v1x, v1y};
                    int16_t p2[2] = {v2x, v2y};

                    //saves around 6% time
                    if(x > max3(v0x, v1x, v2x)) is_in_tri = 0;
                    else if(x < min3(v0x, v1x, v2x)) is_in_tri = 0;
                    else if(y > max3(v0y, v1y, v2y)) is_in_tri = 0;
                    else if(y < min3(v0y, v1y, v2y)) is_in_tri = 0;
                    else{
                        is_in_tri = pt_in_tri(p, p0, p1, p2);
                    }
                    // is_in_tri = pt_in_tri(p, p0, p1, p2);
                    
                    if(is_in_tri){
                        // uint8_t r = faces[6*face_i + 3];
                        // uint8_t g = faces[6*face_i + 4];
                        // uint8_t b = faces[6*face_i + 5];

                        // image[3 * (width * (height-y-1) + x) + 0] = b;
                        // image[3 * (width * (height-y-1) + x) + 1] = g;
                        // image[3 * (width * (height-y-1) + x) + 2] = r;


                        if(face_min_z < min_z){
                            min_z = face_min_z;

                            //draw pixel
                            uint8_t r = face_colors[3*face_i + 0];
                            uint8_t g = face_colors[3*face_i + 1];
                            uint8_t b = face_colors[3*face_i + 2];

                            image[3 * (width * (height-y-1) + x) + 0] = b;
                            image[3 * (width * (height-y-1) + x) + 1] = g;
                            image[3 * (width * (height-y-1) + x) + 2] = r;
                        }
                    }

                }
                    
            }
        }

        // Write the image data to a BMP file
        // write_bmp("output.bmp", image, width, height);
        char filename[20];
        // sprintf(filename, "output/cube%d.bmp", step);
        sprintf(filename, "output/cube%03d.bmp", step);

        write_bmp(filename, image, width, height);


    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Total execution time: %f seconds\n", time_spent);

    return 0;
}
