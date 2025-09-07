// SPDX-License-Identifier: BSD-3-Clause

#define OUT
#define INOUT

#define STRINGIFY(s) #s

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t usize;

typedef float f32;
typedef double f64;

void err(char const *fmt, ...);
void errfile(char const *name, char const *data, usize datalen, usize pos, usize len, char const *fmt, ...);
void showfile(FILE *fp, char const *name, char const *data, usize datalen, usize pos, usize len);
FILE *openread(char const *file);
