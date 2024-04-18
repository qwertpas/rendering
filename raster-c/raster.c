#include <stdint.h>
#include <stdio.h>
#include <math.h>

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

    printf("finish writing file\n");
}

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

    // rotated cube
    const float vertices[] = {
        0.0, 0.0, 0.0,
        -0.22340065, -0.44180209, 0.86885158,
        -0.26478835, -1.32807836, 0.40754753,
        -0.0413877, -0.88627627, -0.46130405,
        0.97384763, -0.13901539, 0.17970952,
        0.75044698, -0.58081749, 1.0485611,
        0.70905928, -1.46709376, 0.58725705,
        0.93245993, -1.02529167, -0.28159453};
    const int n_vertices = sizeof(vertices) / (3 * sizeof(float));
    const uint8_t faces[] = {
        0, 1, 2, 255, 0, 0, // Face 0: red
        0, 2, 3, 255, 0, 0,

        1, 5, 6, 50, 50, 255, // Face 1: light blue
        1, 6, 2, 50, 50, 255,

        5, 4, 7, 255, 0, 255, // Face2 : purple
        5, 7, 6, 255, 0, 255,

        4, 0, 3, 128, 128, 128, // Face 3: gray
        4, 3, 7, 128, 128, 128,

        3, 2, 6, 0, 255, 255, // Face 4: cyan
        3, 6, 7, 0, 255, 255,

        4, 5, 1, 255, 255, 0, // Face 5: yellow
        4, 1, 0, 255, 255, 0};
    const int n_faces = sizeof(faces) / (6 * sizeof(uint8_t));

    const int width = 400;
    const int height = 400;

    const float offset = 200;
    const float scale = 800;
    const float zdist = 10;

    printf("vertices: %d \nfaces: %d\n", n_vertices, n_faces);

    float tri_list[3 * 2 * n_faces + n_faces];
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

        // float v0x_screen = v0x * scale / (v0z + zdist) + offset;
        // float v0y_screen = v0y * scale / (v0z + zdist) + offset;
        // float v1x_screen = v1x * scale / (v1z + zdist) + offset;
        // float v1y_screen = v1y * scale / (v1z + zdist) + offset;
        // float v2x_screen = v2x * scale / (v2z + zdist) + offset;
        // float v2y_screen = v2y * scale / (v2z + zdist) + offset;

        float v0x_screen = v0x * scale*0.1 + offset;
        float v0y_screen = v0y * scale*0.1 + offset;
        float v1x_screen = v1x * scale*0.1 + offset;
        float v1y_screen = v1y * scale*0.1 + offset;
        float v2x_screen = v2x * scale*0.1 + offset;
        float v2y_screen = v2y * scale*0.1 + offset;
        float face_min_z = fmin(fmin(v0z, v1z), v2z);

        tri_list[7 * i + 0] = v0x_screen;
        tri_list[7 * i + 1] = v0y_screen;
        tri_list[7 * i + 2] = v1x_screen;
        tri_list[7 * i + 3] = v1y_screen;
        tri_list[7 * i + 4] = v2x_screen;
        tri_list[7 * i + 5] = v2y_screen;
        tri_list[7 * i + 6] = face_min_z;
    }

    //print out tri_list
    for (int i = 0; i < sizeof(tri_list) / sizeof(float); i += 7) {
        for (int j = 0; j < 7; j++) {
            printf("%f ", tri_list[i+j]);
        }
        printf("\n");
    }

    //initialize rgb array
    uint8_t image[width * height * 3] = {0};

    // Iterate through each pixel
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            float min_z = 1e12;

            // printf("%d %d\n", x,y);

            for (int face_i = 0; face_i < n_faces; face_i++) {

                uint8_t is_in_tri = 0;

                float v0x = tri_list[7*face_i + 0];
                float v0y = tri_list[7*face_i + 1];
                float v1x = tri_list[7*face_i + 2];
                float v1y = tri_list[7*face_i + 3];
                float v2x = tri_list[7*face_i + 4];
                float v2y = tri_list[7*face_i + 5];
                float face_min_z = tri_list[7*face_i + 6];

                float s = (v0x - v2x) * (y - v2y) - (v0y - v2y) * (x - v2x);
                float t = (v1x - v0x) * (y - v0y) - (v1y - v0y) * (x - v0x);
                if ((s < 0) != (t < 0) && s != 0 && t != 0) {
                    is_in_tri = 0;
                }
                float d = (v2x - v1x) * (y - v1y) - (v2y - v1y) * (x - v1x);
                is_in_tri = (d == 0 || (d < 0) == (s + t <= 0));

                

                if(is_in_tri){
                    if(face_min_z < min_z){
                        min_z = face_min_z;

                        //draw pixel
                        uint8_t r = faces[6*face_i + 3];
                        uint8_t g = faces[6*face_i + 4];
                        uint8_t b = faces[6*face_i + 5];

                        image[3 * (width * (width-y) + x) + 0] = 255;
                        image[3 * (width * (width-y) + x) + 1] = g;
                        image[3 * (width * (width-y) + x) + 2] = 0;
                    }
                }

                if(x == 399 && y==399){
                    printf("399\n");
                    image[3 * (width * (width-y) + x) + 0] = 255;
                    image[3 * (width * (width-y) + x) + 1] = 255;
                    image[3 * (width * (width-y) + x) + 2] = 255;
                }

            }
                
        }
    }

    // Write the image data to a BMP file
    write_bmp("output.bmp", image, 400, 400);

    printf("return 0\n");

    return 0;
}
