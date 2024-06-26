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
}

int pt_in_tri(int32_t p[2], int32_t p0[2], int32_t p1[2], int32_t p2[2]) {
    int32_t s = (p0[0] - p2[0]) * (p[1] - p2[1]) - (p0[1] - p2[1]) * (p[0] - p2[0]);
    int32_t t = (p1[0] - p0[0]) * (p[1] - p0[1]) - (p1[1] - p0[1]) * (p[0] - p0[0]);

    if ((s < 0) != (t < 0) && s != 0 && t != 0) {
        return 0;
    }

    int32_t d = (p2[0] - p1[0]) * (p[1] - p1[1]) - (p2[1] - p1[1]) * (p[0] - p1[0]);
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

float vertices_unrot[] = {
    0, 0, 0,
    1, 0, 0,
    1, 1, 0,
    0, 1, 0,
    0, 0, 1,
    1, 0, 1,
    1, 1, 1,
    0, 1, 1
};
const int n_vertices = sizeof(vertices_unrot) / (3 * sizeof(float));
uint8_t faces[] = {
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
int n_faces = sizeof(faces) / (6 * sizeof(uint8_t));

const int width = 640;
const int height = 480;
uint8_t do_perspective = 0;




int main() {
    
    int n_visible_faces = 0;

    int nsteps = 1;
    
    for(int step = 0; step < nsteps; step++){

        printf("step: %d\n", step);

        float roll = 0;
        float pitch = step*6.28/nsteps + 0;
        float yaw = 2*step*6.28/nsteps + 0.3;

        float offset_x = width/2 * (1 + 0.5*sin(6.28*step/nsteps));
        float offset_y = height/2 * (1 + 0.5*sin(6.28*step/nsteps + 2));
        float scale = 1600;
        float zdist = 10;

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
            vertices[3*i + 0] = R[0]*vx + R[1]*vy + R[2]*vz;
            vertices[3*i + 1] = R[3]*vx + R[4]*vy + R[5]*vz;
            vertices[3*i + 2] = R[6]*vx + R[7]*vy + R[8]*vz + zdist;
        }

        // printf("vertices: %d \nfaces: %d\n", n_vertices, n_faces);

        int32_t tri_list[3 * 2 * n_faces + n_faces];
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

            // float area = (v1x - v0x) * (v2y - v0y) - (v2x - v0x) * (v1y - v0y);
            // if(area < 0){
            //     continue; //skip this triangle
            // }
            // n_visible_faces++;
            
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
            
            float face_min_z = fmin(fmin(v0z, v1z), v2z);
            float face_avg = (v0z + v1z + v2z) / 3.;
            float face_z = 0.99*face_min_z + 0.01*face_avg;

            tri_list[7 * i + 0] = (int32_t) v0x_screen;
            tri_list[7 * i + 1] = (int32_t) v0y_screen;
            tri_list[7 * i + 2] = (int32_t) v1x_screen;
            tri_list[7 * i + 3] = (int32_t) v1y_screen;
            tri_list[7 * i + 4] = (int32_t) v2x_screen;
            tri_list[7 * i + 5] = (int32_t) v2y_screen;

            tri_list[7 * i + 6] = (int32_t) face_z;
        }

        // //print out tri_list
        // for (int i = 0; i < sizeof(tri_list) / sizeof(float); i += 7) {
        //     for (int j = 0; j < 7; j++) {
        //         printf("%f ", tri_list[i+j]);
        //     }
        //     printf("\n");
        // }

        //initialize rgb array
        uint8_t image[width * height * 3] = {0};

        // Iterate through each pixel
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {

                float min_z = 1e12;

                for (int face_i = 0; face_i < n_faces; face_i++) {

                    uint8_t is_in_tri = 0;

                    int32_t v0x = tri_list[7*face_i + 0];
                    int32_t v0y = tri_list[7*face_i + 1];
                    int32_t v1x = tri_list[7*face_i + 2];
                    int32_t v1y = tri_list[7*face_i + 3];
                    int32_t v2x = tri_list[7*face_i + 4];
                    int32_t v2y = tri_list[7*face_i + 5];
                    int32_t face_min_z = tri_list[7*face_i + 6];

                    int32_t p[2] = {x, y};
                    int32_t p0[2] = {v0x, v0y};
                    int32_t p1[2] = {v1x, v1y};
                    int32_t p2[2] = {v2x, v2y};

                    if(
                        face_i == 2 ||
                        face_i == 3
                    ){
                        printf("red %d %d %d %d %d %d \n", v0x, v0y, v1x, v1y, v2x, v2y);
                        return 0;
                    }

                    // if(x > max3(v0x, v1x, v2x)){
                    //     // is_in_tri = 0;
                    // }else if(x < min3(v0x, v1x, v2x)){
                    //     // is_in_tri = 0;
                    // }else if(y > max3(v0y, v1y, v2y)){
                    //     // is_in_tri = 0;
                    //     if(max3(v0y, v1y, v2y) > 480){
                    //         printf("ymax %d %d %d %d\n", y, v0y, v1y, v2y);
                    //     }
                    // }else if(y < min3(v0y, v1y, v2y)){
                    //     // is_in_tri = 0;
                    //     if(max3(v0y, v1y, v2y) < 0){
                    //         printf("ymin %d %d %d %d\n", y, v0y, v1y, v2y);
                    //     }
                    // }else{
                        is_in_tri = pt_in_tri(p, p0, p1, p2);
                    // }

                    if(is_in_tri){
                        if(face_min_z < min_z){
                            min_z = face_min_z;

                            //draw pixel
                            uint8_t r = faces[6*face_i + 3];
                            uint8_t g = faces[6*face_i + 4];
                            uint8_t b = faces[6*face_i + 5];

                            image[3 * (width * (height-y-1) + x) + 0] = r;
                            image[3 * (width * (height-y-1) + x) + 1] = g;
                            image[3 * (width * (height-y-1) + x) + 2] = b;
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

    return 0;
}
