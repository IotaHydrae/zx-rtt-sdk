#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pud_qoi.h"

#define PIXELS 4096u
#define WIDTH 64u

static unsigned failures;

static void check(int condition, const char *what)
{
    if (!condition) {
        fprintf(stderr, "qoi test: %s\n", what);
        failures++;
    }
}

struct callback_state {
    uint16_t *pixels;
    unsigned callbacks;
};

static void collect(const uint16_t *pixels, size_t count,
                    uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye,
                    void *user_data)
{
    struct callback_state *state = (struct callback_state *)user_data;
    size_t pos = 0;
    uint16_t x = xs;
    uint16_t y = ys;

    (void)xe;

    state->callbacks++;
    while (pos < count) {
        size_t row = (size_t)y * WIDTH + x;
        size_t row_count = (size_t)WIDTH - x;

        if (row_count > count - pos)
            row_count = count - pos;
        memcpy(state->pixels + row, pixels + pos, row_count * sizeof(*pixels));
        pos += row_count;
        x = (uint16_t)(x + row_count);
        if (x >= WIDTH) {
            x = 0u;
            y++;
        }
    }
    check(y <= (uint16_t)(ye + 1u), "callback rectangle exceeds its end row");
    check(pos == count, "callback rectangle does not cover its pixel count");
}

static void make_pixels(uint16_t *pixels)
{
    unsigned i;

    for (i = 0; i < PIXELS; i++) {
        switch (i % 8u) {
        case 0:
        case 1:
        case 2:
            pixels[i] = 0x1234u;             /* RUN */
            break;
        case 3:
            pixels[i] = (uint16_t)(pixels[i - 1u] + 0x0821u); /* DIFF */
            break;
        case 4:
            pixels[i] = (uint16_t)(pixels[i - 1u] + 0x0841u); /* LUMA */
            break;
        case 5:
            pixels[i] = 0xf81fu;             /* RGB565 */
            break;
        default:
            pixels[i] = (uint16_t)((i * 251u) ^ (i >> 3)); /* INDEX/RGB */
            break;
        }
    }
}

static void roundtrip(const uint16_t *source)
{
    uint8_t *encoded;
    uint16_t *decoded;
    uint16_t *streamed;
    uint16_t batch_a[31];
    uint16_t batch_b[31];
    struct callback_state state;
    size_t capacity = rgb565_qoi_max_compressed_size(PIXELS);
    size_t encoded_size;
    size_t decoded_count;
    size_t streamed_count;

    encoded = (uint8_t *)malloc(capacity);
    decoded = (uint16_t *)calloc(PIXELS, sizeof(*decoded));
    streamed = (uint16_t *)calloc(PIXELS, sizeof(*streamed));
    check(encoded != NULL && decoded != NULL && streamed != NULL, "allocation");
    if (!encoded || !decoded || !streamed)
        goto out;

    encoded_size = rgb565_qoi_compress(source, PIXELS, encoded, capacity);
    check(encoded_size != 0u, "compress");
    decoded_count = rgb565_qoi_decompress(encoded, encoded_size, decoded, PIXELS);
    check(decoded_count == PIXELS, "whole-image decode count");
    check(memcmp(source, decoded, PIXELS * sizeof(*source)) == 0,
          "whole-image pixels");

    state.pixels = streamed;
    state.callbacks = 0u;
    streamed_count = rgb565_qoi_decompress_callback(
        encoded, encoded_size, WIDTH, batch_a, batch_b, 31u, collect, &state);
    check(streamed_count == PIXELS, "callback decode count");
    check(state.callbacks > 1u, "callback did not exercise batching");
    if (memcmp(source, streamed, PIXELS * sizeof(*source)) != 0) {
        size_t bad;
        for (bad = 0; bad < PIXELS; bad++) {
            if (source[bad] != streamed[bad]) {
                fprintf(stderr, "qoi test: callback mismatch at %zu (%04x != %04x)\n",
                        bad, source[bad], streamed[bad]);
                break;
            }
        }
        failures++;
    }

out:
    free(streamed);
    free(decoded);
    free(encoded);
}

static void malformed(const uint16_t *source)
{
    uint8_t encoded[PIXELS * 3u + 16u];
    uint16_t decoded[PIXELS];
    size_t size;
    size_t n;
    uint8_t padded[PIXELS * 3u + 17u];

    size = rgb565_qoi_compress(source, PIXELS, encoded, sizeof(encoded));
    check(size != 0u, "malformed fixture compress");
    memcpy(padded, encoded, size);
    padded[size] = 0u;
    check(rgb565_qoi_decompress(padded, size + 1u, decoded, PIXELS) == PIXELS,
          "transport padding rejected");
    for (n = 0u; n < size; n++)
        check(rgb565_qoi_decompress(encoded, n, decoded, PIXELS) == 0u,
              "truncated stream accepted");

    encoded[0] = 'x';
    check(rgb565_qoi_decompress(encoded, size, decoded, PIXELS) == 0u,
          "bad magic accepted");
    encoded[0] = 'q';
    encoded[size - 1u] = 0u;
    check(rgb565_qoi_decompress(encoded, size, decoded, PIXELS) == 0u,
          "bad padding accepted");
    encoded[size - 1u] = 1u;
    encoded[8] = 0xffu;
    check(rgb565_qoi_decompress(encoded, size, decoded, PIXELS) == 0u,
          "reserved chunk accepted");
}

int main(void)
{
    uint16_t pixels[PIXELS];

    make_pixels(pixels);
    roundtrip(pixels);
    malformed(pixels);
    if (failures != 0u)
        return 1;
    puts("qoi: roundtrip and malformed-stream checks passed");
    return 0;
}
