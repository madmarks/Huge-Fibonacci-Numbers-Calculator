#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <time.h>

#define UINT64_MEM_NUMBER_LEN (11 * 1024)

typedef struct memory_represented_hex_number {
    uint64_t * num;
    uint64_t len;
} mrhexT;

int mrhex_init(mrhexT *, const uint64_t);
int mrhex_add(mrhexT *, mrhexT *, mrhexT *);
void swap_any_data(void *, void *, const size_t);
void reverse_array(void *, const uint64_t, const size_t);
void reverse_and_flatten_array(void *, const uint64_t, const size_t, const _Bool);
void mrhex2mrdec(void *, uint64_t, void *, void *, uint64_t *);
void print_fibonacci(uint64_t, const uint8_t * const, uint64_t);

/*
 * Huge Fibonacci Numbers Calculator
 * 
 * F_0 = 0, F_1 = 1, F_2 = 1, F_3 = 2, F_4 = 3, ...
 * 
 * 0, 1, 1, 2, 3, 5, 8, 13, 21, ...
 * 
 * F_1038394 for less than 5 minutes on virtual machine. (Release mode)
 * 
 * 
 * ./fibonacci > result.txt
 */
int main() {
    
    uint64_t n = UINT64_MAX;
    
    printf("n = ");
    scanf("%llu", &n);

    time_t t = clock();

    _Bool is_little_endian = true;

//    uint64_t Fn_minus_2 = 1; // F_1 = 1
//    uint64_t Fn_minus_1 = 1; // F_2 = 1
//    uint64_t Fn = 2; // F_3 = 2
//    uint64_t n = 3;
//
//    while (Fn > Fn_minus_1) {
//        Fn_minus_2 = Fn_minus_1;
//        Fn_minus_1 = Fn;
//        Fn = Fn_minus_1 + Fn_minus_2;
//        n++;
//    }
//
//    printf("Max uint64_t Fibonacci number F_%"PRIu64" = %"PRIu64"\n",
//            n - (uint64_t) 1, Fn_minus_1);

    mrhexT * mrhex_temp;

    mrhexT * mrhex_Fn_minus_2 = (mrhexT *) malloc(sizeof (mrhexT)); // F_1 = 1
    if (NULL == mrhex_Fn_minus_2)
        return 1;

    mrhexT * mrhex_Fn_minus_1 = (mrhexT *) malloc(sizeof (mrhexT)); // F_2 = 1
    if (NULL == mrhex_Fn_minus_1)
        return 1;

    mrhexT * mrhex_Fn = (mrhexT *) malloc(sizeof (mrhexT)); // F_3 = 2 
    if (NULL == mrhex_Fn)
        return 1;

    uint64_t mrhex_n = 3;

    if (mrhex_init(mrhex_Fn_minus_2, (uint64_t) 1)) // F_1 = 1
        return 1;

    if (mrhex_init(mrhex_Fn_minus_1, (uint64_t) 1)) // F_2 = 1
        return 1;

    if (mrhex_init(mrhex_Fn, (uint64_t) 2)) // F_3 = 2
        return 1;

    for (;;) {
        mrhex_temp = mrhex_Fn_minus_2;
        mrhex_Fn_minus_2 = mrhex_Fn_minus_1;
        mrhex_Fn_minus_1 = mrhex_Fn;
        mrhex_Fn = mrhex_temp;
        mrhex_n++;

        if ((n == (mrhex_n - 1)) || mrhex_add(mrhex_Fn, mrhex_Fn_minus_1, mrhex_Fn_minus_2))
            break;
    }

    // mrhex_Fn_minus_1 is found Fibonacci number
    
    reverse_and_flatten_array(
            (void *) mrhex_Fn_minus_1->num,
            mrhex_Fn_minus_1->len,
            sizeof (uint64_t),
            is_little_endian
            );

    // hexadecimal to decimal conversion requires 1.21 times more space 
    uint64_t result_len = ((mrhex_Fn_minus_2->len * sizeof (uint64_t) * 1.21 + 8) / 8) * 8;

    free(mrhex_Fn_minus_2->num);
    void * result_ptr = calloc((size_t) result_len, sizeof (uint8_t));
    if (NULL == result_ptr) {
        printf("Cannot allocate memory!\n");
        return 1;
    }

    mrhex2mrdec(
            (void *) (mrhex_Fn_minus_1->num),
            (uint64_t) (8 * mrhex_Fn_minus_1->len),
            (void *) (mrhex_Fn->num),
            result_ptr,
            &result_len
            );
    
    // ((result_len + 1) / 2) because 2 decimal digits are stored in one byte
    uint64_t result_len_in_bytes = (uint64_t) ((result_len + 1) / 2);

    printf("Max found ");
    print_fibonacci((uint64_t) (mrhex_n - 1), (uint8_t *)result_ptr, result_len_in_bytes);
    
    free(result_ptr);
    free(mrhex_Fn->num);
    free(mrhex_Fn_minus_1->num);

    free(mrhex_Fn);
    free(mrhex_Fn_minus_1);
    free(mrhex_Fn_minus_2);

    t = clock() - t;
    printf("Execution time: %lf sec\n", (double) ((double) t / CLOCKS_PER_SEC));

    return 0;
}

int mrhex_init(mrhexT * mrhex_num, const uint64_t init_number) {

    mrhex_num->num = (uint64_t *) calloc((size_t) UINT64_MEM_NUMBER_LEN, sizeof (uint64_t));
    if (NULL == mrhex_num->num) {
        printf("Cannot allocate memory!\n");
        return 1;
    }

    mrhex_num->num[0] = init_number;
    mrhex_num->len = (uint64_t) 1;

    return 0;
}

int mrhex_add(mrhexT * mrhex_Fn, mrhexT * mrhex_Fn_minus_1, mrhexT * mrhex_Fn_minus_2) {

    uint64_t carry = (uint64_t) 0;

    mrhex_Fn->len = mrhex_Fn_minus_1->len;

    for (uint64_t i = (uint64_t) 0; i < mrhex_Fn->len; i++) {
        mrhex_Fn->num[i] =
                carry + mrhex_Fn_minus_1->num[i] + mrhex_Fn_minus_2->num[i];

        carry = (mrhex_Fn->num[i] < mrhex_Fn_minus_1->num[i]) ?
                (uint64_t) 1 : (uint64_t) 0;
    }

    if (carry == (uint64_t) 0)
        return 0;

    // carry = 1
    if (mrhex_Fn->len < (uint64_t) UINT64_MEM_NUMBER_LEN) {
        mrhex_Fn->num[mrhex_Fn->len] = carry;
        mrhex_Fn->len++;

        return 0;
    }

    return 1;
}

/*
 * Swap any data (does NOT support overlapping)
 */
void swap_any_data(void * ptr1, void * ptr2, const size_t size) {

    void * tmp = malloc(size);

    if (NULL != tmp) {
        memcpy(tmp, ptr1, size);
        memcpy(ptr1, ptr2, size);
        memcpy(ptr2, tmp, size);

        free(tmp);
    }
}

/* 
 * Reverse array represented in memory:
 *
 * a[0], a[1], a[2], ..., a[n-2], a[n-1] 
 *
 * to
 * 
 * a[n-1], a[n-2], ..., a[2], a[1], a[0]
 */
void reverse_array(
        void * arr,
        const uint64_t num_arr_elements,
        const size_t size_of_type
        ) {

    uint64_t i;

    for (i = (uint64_t) 0; i < (uint64_t) (num_arr_elements / 2); i++) {

        void * ptr1 = (void *) (arr + i * size_of_type);
        void * ptr2 = (void *) (arr + (num_arr_elements - i - 1) * size_of_type);

        swap_any_data(ptr1, ptr2, size_of_type);
    }
}

void reverse_and_flatten_array(
        void * arr,
        const uint64_t num_arr_elements,
        const size_t size_of_type,
        const _Bool is_little_endian
        ) {

    reverse_array(arr, num_arr_elements, size_of_type);

    if (is_little_endian) {

        uint8_t * first_byte;
        uint8_t * x;
        uint8_t * y;

        for (uint64_t i = (uint64_t) 0; i < num_arr_elements; i++) {

            first_byte = (uint8_t *) (arr + i * size_of_type);

            for (size_t j = 0; j < (size_t) (size_of_type / 2); j++) {

                x = (uint8_t *) (first_byte + j);
                y = (uint8_t *) (first_byte + (size_of_type - j - 1));

                // swap

                *x ^= *y;
                *y ^= *x;
                *x ^= *y;
            }
        }
    }
}

void mrhex2mrdec(
        void * mem_a,
        uint64_t arr_dividend_len,
        void * mem_b,
        void * arr_remainder,
        uint64_t * arr_remainder_len
        ) {

    uint8_t dividend;
    uint8_t divisor = 0x0A; // 10 
    uint8_t quotient;
    uint8_t remainder;

    void * arr_dividend = mem_a;
    void * arr_quotient = mem_b;

    *arr_remainder_len = (uint64_t) 0;

    uint64_t i;
    void * tmp;

    _Bool has_non_zero_quotient;

    do {
        remainder = 0;
        has_non_zero_quotient = false;

        for (i = (uint64_t) 0; i < arr_dividend_len; i++) {

            dividend = (remainder << 4) ^ (*((uint8_t *) (arr_dividend + i)) >> 4);
            quotient = (dividend / divisor) << 4;
            remainder = dividend % divisor;

            dividend = (remainder << 4) ^ (*(uint8_t *) (arr_dividend + i) & 0x0F);
            quotient ^= (dividend / divisor);
            remainder = dividend % divisor;

            if (quotient)
                has_non_zero_quotient = true;

            *(uint8_t *) (arr_quotient + i) = quotient;
        }

        tmp = arr_dividend;
        arr_dividend = arr_quotient;
        arr_quotient = tmp;

        while (0 == *(uint8_t *) (arr_dividend)) {
            arr_dividend++;
            arr_dividend_len--;
        }

        // we store 2 decimals in one byte to reduce twice space needed
        if (0 == (*arr_remainder_len % 2))
            *(uint8_t *) (arr_remainder + *arr_remainder_len / 2) = remainder;
        else
            *(uint8_t *) (arr_remainder + *arr_remainder_len / 2) ^= (remainder <<= 4);

        (*arr_remainder_len)++;

    } while (has_non_zero_quotient);
}

void print_fibonacci(uint64_t n, const uint8_t * const fib_ptr, uint64_t len)
{
    printf("Fibonacci number F_%"PRIu64" = ", n);
    
    uint64_t i = (uint64_t)(len - 1);
    
    if((*(fib_ptr + i) >> 4) != 0)
        printf("%c", (*(fib_ptr + i) >> 4) ^ 0x30);
    
    printf("%c", (*(fib_ptr + i) & 0x0F) ^ 0x30);
    
    while(i > (uint64_t)0)
    {
        i--;
        printf("%c", (*(fib_ptr + i) >> 4) ^ 0x30);
        printf("%c", (*(fib_ptr + i) & 0x0F) ^ 0x30);
    }
    
    printf("\n");
}