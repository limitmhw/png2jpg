// file: png2jpg.cpp
// Usage:
//   ./png2jpg input.png output.jpg [quality] [downsample_factor]
//   cat input.png | ./png2jpg - output.jpg 85 2   # 支持从 stdin 读取（用 - 表示 stdin）
//   downsample_factor: 1 = 无下采样，2 = 下采样2倍，4 = 下采样4倍

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <memory>
#include <array>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static bool file_exists(const char * path) {
    struct stat st;
    return stat(path, & st) == 0;
}

static std::vector < unsigned char > read_stdin_all() {
    std::vector < unsigned char > buf;
    const size_t CHUNK = 8192;
    unsigned char tmp[CHUNK];
    while (true) {
        size_t r = fread(tmp, 1, CHUNK, stdin);
        if (r > 0) buf.insert(buf.end(), tmp, tmp + r);
        if (r < CHUNK) break;
    }
    return buf;
}

static std::vector < unsigned char > read_file_all(const char * path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) return {};
    std::vector < unsigned char > buf((std::istreambuf_iterator < char > (ifs)),
        std::istreambuf_iterator < char > ());
    return buf;
}

// Downsample image by skipping pixels, ensuring no out-of-bounds access
static void downsample_image(unsigned char * & img, int & w, int & h, int factor) {
    if (factor == 1) return; // No downsampling

    // Compute new width and height (using ceil to avoid truncation)
    int new_w = (w + factor - 1) / factor;
    int new_h = (h + factor - 1) / factor;

    // Allocate memory for the new downsampled image
    unsigned char * new_img = (unsigned char * ) malloc(new_w * new_h * 3);

    // Downsample image by skipping pixels, ensuring no out-of-bounds access
    for (int y = 0; y < new_h; ++y) {
        for (int x = 0; x < new_w; ++x) {
            // Calculate the corresponding pixel in the original image
            int old_x = std::min(x * factor, w - 1); // Ensure within bounds
            int old_y = std::min(y * factor, h - 1); // Ensure within bounds

            new_img[(y * new_w + x) * 3 + 0] = img[(old_y * w + old_x) * 3 + 0];
            new_img[(y * new_w + x) * 3 + 1] = img[(old_y * w + old_x) * 3 + 1];
            new_img[(y * new_w + x) * 3 + 2] = img[(old_y * w + old_x) * 3 + 2];
        }
    }

    // Free original image memory, then point to the new image
    free(img);
    img = new_img;
    w = new_w;
    h = new_h;
}

int main(int argc, char ** argv) {
    if (argc < 1) {
        std::fprintf(stderr, "Model 1 Usage: %s <input.png or -> <output.jpg> [quality] [downsample_factor]\n", argv[0]);
        std::fprintf(stderr, "Model 2 Usage: %s <output.jpg> [quality] [downsample_factor]\n", argv[0]);
    }
    const char * mode = argv[1];
    if (*mode == 'i') {
        if (argc < 4) {
            std::fprintf(stderr, "Usage: %s <input.png or -> <output.jpg> [quality] [downsample_factor]\n", argv[0]);
            return 1;
        }

        const char * input_path = argv[2];
        const char * output_path = argv[3];
        int quality = 85;
        int downsample_factor = 1; // Default no downsampling

        if (argc >= 4) quality = std::atoi(argv[4]);
        if (argc >= 5) downsample_factor = std::atoi(argv[5]);

        if (quality < 1) quality = 1;
        if (quality > 100) quality = 100;
        if (downsample_factor != 1 && downsample_factor != 2 && downsample_factor != 4) {
            std::fprintf(stderr, "Invalid downsample factor: %d. Supported values are 1, 2, 4.\n", downsample_factor);
            return 7;
        }

        std::vector < unsigned char > file_buf;
        if (std::string(input_path) == "-") {
            file_buf = read_stdin_all();
            if (file_buf.empty()) {
                std::fprintf(stderr, "No data read from stdin.\n");
                return 2;
            }
        } else {
            if (!file_exists(input_path)) {
                std::fprintf(stderr, "Input file not found: %s\n", input_path);
                return 3;
            }
            file_buf = read_file_all(input_path);
            if (file_buf.empty()) {
                std::fprintf(stderr, "Failed to read file: %s\n", input_path);
                return 4;
            }
        }

        int w = 0, h = 0, channels = 0;
        // force load as RGB (3 channels) to simplify JPEG output
        unsigned char * img = stbi_load_from_memory(file_buf.data(), (int) file_buf.size(), & w, & h, & channels, 3);
        if (!img) {
            std::fprintf(stderr, "stb_image failed to decode image: %s\n", stbi_failure_reason());
            return 5;
        }

        // Downsample image if necessary
        downsample_image(img, w, h, downsample_factor);

        // write JPEG
        int comp = 3; // RGB
        if (!stbi_write_jpg(output_path, w, h, comp, img, quality)) {
            std::fprintf(stderr, "stb_image_write failed to write %s\n", output_path);
            stbi_image_free(img);
            return 6;
        }

        std::printf("Converted: %s  ->  %s (%dx%d, quality=%d, downsample_factor=%d)\n",
            (std::string(input_path) == "-" ? "stdin" : input_path),
            output_path, w, h, quality, downsample_factor);
        stbi_image_free(img);

    } else if (*mode == 'p') {
        if (argc < 3) {
            std::fprintf(stderr, "Usage: %s <output.jpg> [quality] [downsample_factor]\n", argv[1]);
            return 1;
        }

        const char * output_path = argv[2];
        int quality = 85;
        int downsample_factor = 1; // Default no downsampling

        if (argc >= 4) quality = std::atoi(argv[3]);
        if (argc >= 5) downsample_factor = std::atoi(argv[4]);

        if (quality < 1) quality = 1;
        if (quality > 100) quality = 100;
        if (downsample_factor != 1 && downsample_factor != 2 && downsample_factor != 4) {
            std::fprintf(stderr, "Invalid downsample factor: %d. Supported values are 1, 2, 4.\n", downsample_factor);
            return 7;
        }

        // Read the PNG data from stdin (from pipe)
        std::vector < unsigned char > file_buf = read_stdin_all();
        if (file_buf.empty()) {
            std::fprintf(stderr, "No data read from stdin.\n");
            return 2;
        }

        int w = 0, h = 0, channels = 0;
        // force load as RGB (3 channels) to simplify JPEG output
        unsigned char * img = stbi_load_from_memory(file_buf.data(), (int) file_buf.size(), & w, & h, & channels, 3);
        if (!img) {
            std::fprintf(stderr, "stb_image failed to decode image: %s\n", stbi_failure_reason());
            return 5;
        }

        // Downsample image if necessary
        downsample_image(img, w, h, downsample_factor);

        // write JPEG
        int comp = 3; // RGB
        if (!stbi_write_jpg(output_path, w, h, comp, img, quality)) {
            std::fprintf(stderr, "stb_image_write failed to write %s\n", output_path);
            stbi_image_free(img);
            return 6;
        }

        std::printf("Converted: stdin -> %s (%dx%d, quality=%d, downsample_factor=%d)\n",
            output_path, w, h, quality, downsample_factor);
        stbi_image_free(img);


    } else {
        std::printf("mode error! mode is i or p");
    }


    return 0;
}
