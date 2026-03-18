#ifndef base64H
#define base64H

#include <math.h>
#include <stdlib.h>
//#include "bfc\platform\types.h"
#include "stdint.h"
#pragma option push -w-csu
namespace AAbase64{

static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};

void build_decoding_table() {
        if(decoding_table)return;
    decoding_table = (char *)malloc(256);
    for (int i = 0; i < 0x40; i++)
        decoding_table[encoding_table[i]] = i;
}


void base64_cleanup() {
        if(decoding_table)
            free(decoding_table);
        decoding_table=NULL;
}

class autobase64init
{
public:
        inline __fastcall autobase64init(void){build_decoding_table();};
        inline __fastcall ~autobase64init(void){base64_cleanup();};
};

char *base64_encode(const unsigned char *data,
                    size_t input_length,
                    size_t *output_length) {

    *output_length = (size_t) (4.0 * ceil((double) input_length / 3.0));

    char *encoded_data = (char *)malloc(*output_length);
    if (encoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';

    return encoded_data;
}


size_t base64_decode_length(const char *data,
                        size_t input_length) {
    size_t res;
    if (decoding_table == NULL) build_decoding_table();

    if (input_length % 4 != 0) return 0;

    res = input_length / 4 * 3;
    if (data[input_length - 1] == '=') res--;
    if (data[input_length - 2] == '=') res--;
    
    return res;
}

unsigned char *base64_decode(const char *data,
                    size_t input_length,
                    size_t *output_length) {

    if (decoding_table == NULL) build_decoding_table();

    if (input_length % 4 != 0) return NULL;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded_data = (char *)malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;) {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
                        + (sextet_b << 2 * 6)
                        + (sextet_c << 1 * 6)
                        + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }
    return decoded_data;
}

struct base64Progressive
{
        const char *data;
        char *decoded_data;
        size_t input_length;
        size_t output_length;
        int i,j;
        uint32_t sextet_a,sextet_b,sextet_c,sextet_d,triple;
};

base64Progressive *base64_decode_start(const char *data, //[in|src]
                    size_t input_length, //[in]
                    char *decoded_data, //[in|dest]
                    size_t *output_length) //[in|out]
{
        if (input_length % 4 != 0) return NULL;
        size_t length=base64_decode_length(data,input_length);
        if(*output_length<length)return NULL;
        if(!decoded_data)return NULL;
        base64Progressive *handle=(base64Progressive *)malloc(sizeof(base64Progressive));
        handle->data=data;
        handle->decoded_data=decoded_data;
        handle->input_length=input_length;
        handle->output_length=length;
        handle->i=0;
        handle->j=0;
        return handle;
}

void base64_decode_end(base64Progressive *handle)
{
        free(handle);
}

size_t base64_decode_segment(
                                base64Progressive *handle,
                                                                    size_t length)
{
    for (handle->i; handle->i < handle->input_length;) {

        handle->sextet_a = handle->data[handle->i] == '=' ? 0 & handle->i++ : decoding_table[handle->data[handle->i++]];
        handle->sextet_b = handle->data[handle->i] == '=' ? 0 & handle->i++ : decoding_table[handle->data[handle->i++]];
        handle->sextet_c = handle->data[handle->i] == '=' ? 0 & handle->i++ : decoding_table[handle->data[handle->i++]];
        handle->sextet_d = handle->data[handle->i] == '=' ? 0 & handle->i++ : decoding_table[handle->data[handle->i++]];

        handle->triple = (handle->sextet_a << 3 * 6)
                        + (handle->sextet_b << 2 * 6)
                        + (handle->sextet_c << 1 * 6)
                        + (handle->sextet_d << 0 * 6);

        if (handle->j < handle->output_length) handle->decoded_data[handle->j++] = (handle->triple >> 2 * 8) & 0xFF;
        if (handle->j < handle->output_length) handle->decoded_data[handle->j++] = (handle->triple >> 1 * 8) & 0xFF;
        if (handle->j < handle->output_length) handle->decoded_data[handle->j++] = (handle->triple >> 0 * 8) & 0xFF;
        if(handle->j>length)
        {
                break;
        }
    }
    return handle->j;
}

static AAbase64::autobase64init ab64i;
};//namespace AAbase64

#pragma option pop //-w-csu
#endif //base64H
