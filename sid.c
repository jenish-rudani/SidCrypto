#include <stdint.h>
#pragma clang optimize off

typedef union uint64_union_s
{
    uint64_t r64;
    uint32_t r32[2];
    uint16_t r16[4];
    uint8_t  r8[8];        
} __attribute__((packed)) uint64_union_t;


/**
 * @brief data for doing the rc5 stuff
 */
typedef struct Rc5_64_s
{
    uint16_t rounds;
    uint32_t msk;
    uint32_t bits;
    uint32_t ks[68];
} Rc5_64_t;


const Rc5_64_t Rc5_64 =
{
    .rounds = 32,
    .msk = 0xffffffff,
    .bits = 32,
    .ks = {0x711773be, 0x1dd61065, 0xd32d1156, 0x394dbb7e, 0x14175453, 0x2353b8cf, 0x7f454a57, 0xc0f08e4c,
        0x2d90811e, 0x324c303a, 0x26b41449, 0xd1d123fc, 0xf3397259, 0x6333fce5, 0x6028f05a, 0xf16e7bf6,
        0x96d79386, 0x02b57731, 0xb1fee9b3, 0x42673955, 0xd30019f7, 0x6c8929a0, 0xd6ada0ab, 0x84585b45,
        0x8b294f49, 0x305746c0, 0x7e600cab, 0x23461123, 0x6ed796fb, 0xd2dc32d2, 0x96f036a1, 0xcc953866,
        0x2695474e, 0x604de0dd, 0x28558d0e, 0x4cfad621, 0xbbe4514a, 0x727518c5, 0x72741794, 0xb2145b3f,
        0x6c87b796, 0x229573ad, 0x8820a3bc, 0xbc094e8d, 0xad1b9f3b, 0x46b8c7af, 0xd2c5d39f, 0x739cfd83,
        0xd5b14421, 0x93b2b042, 0x867982cf, 0x30c14299, 0x0ddaceb0, 0x23fe0916, 0x7e1295f6, 0x46b16d29,
        0x952bc188, 0xee8c0d51, 0x187d4236, 0xfb6452ad, 0xc1a0f7ab, 0x9d0d1efd, 0x6d45f1cf, 0xaa464e36,
        0x7b51148a, 0x0d00fec6, 0x7b29e89f, 0xe766276a}
};

uint32_t rotL(const Rc5_64_t*c, uint32_t x,uint32_t n)
{
    uint32_t nn = n % c->bits;
    uint32_t xx = x & c->msk;
    uint32_t out;
    if(nn == 0)
    {
        return x;
    }
    out = (xx << n) | ((xx >>(c->bits - n)) & c->msk);
    return out;
    
}


uint32_t rotR(const Rc5_64_t*c, uint32_t x,uint32_t n)
{
    uint32_t nn = n % c->bits;
    uint32_t xx = x & c->msk;
    uint32_t out;
    if(nn == 0)
    {
        return x;
    }
    out = (xx >> n) | ((xx << (c->bits - n)) & c->msk);
    return out;
    
}


/**
* @brief Encrypt for the Sbeacon BeaconID
* @param in unencrypted data
* @param out pointer to encrypted data (uint64)
* @return encrypted data
* uses the Rc5_64 declared above
*/

uint64_t Encrypt(uint8_t* inptr,uint8_t *outptr)
{
    int i;
    uint64_t *inp = (uint64_t *)inptr;
    uint64_union_t in;
    in.r64 = *inp;
    uint32_t a = in.r32[0];
    uint32_t b = in.r32[1];
    uint64_union_t out;
    a = (a + Rc5_64.ks[0]) & Rc5_64.msk;
    b = (b + Rc5_64.ks[1]) & Rc5_64.msk;    
	for (i = 1; i <= Rc5_64.rounds; i++) {
		a = (rotL(&Rc5_64,a^b, b) + Rc5_64.ks[2*i]) & Rc5_64.msk;
        b = (rotL(&Rc5_64,b^a, a) + Rc5_64.ks[2*i+1]) & Rc5_64.msk;
	}
    out.r32[1] = b;
    out.r32[0] = a;
    uint64_t *oout = (uint64_t *)outptr;
    *oout = out.r64;
    return out.r64;
}

uint64_t Decrypt(uint8_t* inptr,uint8_t *outptr)
{
    int i;
    uint64_t *inp = (uint64_t *)inptr;
    uint64_union_t in;
    in.r64 = *inp;
    uint64_union_t out;
	uint32_t a = in.r32[0];
    uint32_t b = in.r32[1];
    
	
	for (i = Rc5_64.rounds; i > 0; i--) {
		b = (rotR(&Rc5_64,b-Rc5_64.ks[2*i+1],a) ^ a);
        a = (rotR(&Rc5_64,a-Rc5_64.ks[2*i],b) ^ b);
	}
    b = (b - Rc5_64.ks[1]) & Rc5_64.msk;
	a = (a - Rc5_64.ks[0]) & Rc5_64.msk;
	out.r32[1] = b;
    out.r32[0] = a;
    uint64_t *oout = (uint64_t *)outptr;
    *oout = out.r64;
    return out.r64;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_in_hex>\n", argv[0]);
        return 1;
    }

    uint64_t input;
    uint64_t output;

    // Parse the input from command line argument
    if (sscanf(argv[1], "%lld", &input) != 1) {
        fprintf(stderr, "Invalid input format. Please enter a decimal number.\n");
        return 1;
    }

    Decrypt((uint8_t *)&input, (uint8_t *)&output);

    // Print the output, masking to ensure 56-bit output as per original code
    printf("%012llX\n", output & 0x0000FFFFFFFFFFFFFF);

    return 0;
}
