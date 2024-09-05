#include <stdint.h>
#include <inttypes.h>

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

#define U32V(v) (v & 0xFFFFFFFF)

#define ROTL32(v, n) \
   (U32V((v) << (n)) | ((v) >> (32 - (n))))
#define ROTATELEFT(v,c) (ROTL32(v,c))
#define ROTR32(v, n) ROTL32(v, 32 - (n))
#define ROTATERIGHT(v,c) (ROTR32(v,c))

uint32_t rotL(const Rc5_64_t*c, uint32_t x,uint32_t n)
{
    uint32_t nn = n % c->bits;
    uint32_t xx = x & c->msk;
    uint32_t out;
    if(nn == 0)
    {
        return x;
    }
    uint32_t l1 = (c->bits - n);
    uint32_t l2 =  ROTATERIGHT(xx,l1) & c->msk;
    uint32_t l3 = ROTATELEFT(xx, n);
    out = l3 | l2;
    // out = (xx << n) | ((xx >>(c->bits - n)) & c->msk);
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
#include <getopt.h>
#include <stdint.h>
#include <inttypes.h>

int main(int argc, char *argv[])
{
    uint64_t output = 0;
    uint8_t seed_value = 0xFF;
    int opt;
    int mac_flag = 0;
    uint8_t mfgMac[6] = {0};
    char mac_str[13] = {0};  // 12 hex digits + null terminator

    while ((opt = getopt(argc, argv, "s:m:")) != -1) {
        switch (opt) {
            case 's':
                seed_value = (uint8_t)strtol(optarg, NULL, 16);
                break;
            case 'm':
                if (strlen(optarg) != 12) {
                    fprintf(stderr, "Invalid MAC format. Please enter 12 hexadecimal digits.\n");
                    return 1;
                }
                strncpy(mac_str, optarg, 12);
                mac_flag = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s -s <seed_in_hex> -m <input_mac_12_hex_digits>\n", argv[0]);
                return 1;
        }
    }

    if (!mac_flag) {
        fprintf(stderr, "MAC address is required. Use -m flag to specify the MAC.\n");
        return 1;
    }

    // Convert MAC string to bytes
    for (int i = 0; i < 6; i++) {
        sscanf(mac_str + (i * 2), "%2hhx", &mfgMac[i]);
    }

    // uint8_t mfgMac[6] = {0x38,0x00,0xC8,0x02,0xED,0x08};
      uint8_t beaconid[8];
    for (uint8_t i = 0; i < sizeof(mfgMac); i++) { beaconid[i] = mfgMac[i]; }
    beaconid[6] = 0x00;  // bits[55:48] Reserved
    beaconid[7] = seed_value;  // bits[63:56] GUARD ID

    // uint64_t seed = (uint64_t)((uint64_t)0xFC << 56 | 
    //                            (uint64_t)mfgMac[0] << 56 | 
    //                            (uint64_t)mfgMac[1] << 32 | 
    //                            (uint64_t)mfgMac[2] << 24 | 
    //                            (uint64_t)mfgMac[3] << 16 | 
    //                            (uint64_t)mfgMac[4] << 8  | 
    //                            (uint64_t)mfgMac[5]);

    // printf("Seed: 0x%" PRIX64 "\n", (uint8_t *)&beaconid);
    printf("BeaconID: ");
       for (uint8_t i = 0; i < 8; i++) {
           printf("%02X", beaconid[i]);
       }
    printf("\n");
    Encrypt((uint8_t *)&beaconid, (uint8_t *)&output);

    // Print the output
    printf("Output Decimal: %" PRIu64 "\n", output);
    printf("Output Hex: %" PRIX64 "\n", output);

    return 0;
}
