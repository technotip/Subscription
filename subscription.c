#include "hookapi.h"

#define BYTES20_TO_BUF(buf_raw, i)\
{\
    unsigned char* buf = (unsigned char*)buf_raw;\
    *(uint64_t*)(buf + 0) = *(uint64_t*)(i +  0);\
    *(uint64_t*)(buf + 8) = *(uint64_t*)(i +  8);\
    *(uint64_t*)(buf + 16) = *(uint64_t*)(i + 16);\
    *(uint8_t*)(buf + 4) = *(uint8_t*)(i + 4);\
}

uint8_t txn[278] =
{
    /* size,upto */
    /* 3,  0, tt = Payment           */   0x12U, 0x00U, 0x00U,
    /* 5,  3, flags                  */   0x22U, 0x00U, 0x00U, 0x00U, 0x00U,
    /* 5,  8, sequence               */   0x24U, 0x00U, 0x00U, 0x00U, 0x00U,
    /* 6,  13, firstledgersequence   */   0x20U, 0x1AU, 0x00U, 0x00U, 0x00U, 0x00U,
    /* 6,  19, lastledgersequence    */   0x20U, 0x1BU, 0x00U, 0x00U, 0x00U, 0x00U,
    /*  49, 25  amount               */   0x61U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,                         
                                        0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,
                                        0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,
                                        0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,
                                        0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U,
                                        0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99U, 0x99,
    /* 9,   74,  fee                 */   0x68U, 0x40U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
    /* 35,  83, signingpubkey        */   0x73U, 0x21U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    /* 22,  118, account             */   0x81U, 0x14U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    /* 22,  140, destination         */   0x83U, 0x14U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    /* 116, 162  emit details        */ 
    /* 0,   278                      */ 
};

// TX BUILDER
#define FLS_OUT    (txn + 15U) 
#define LLS_OUT    (txn + 21U) 
#define FEE_OUT    (txn + 75U) 
#define AMOUNT_OUT (txn + 25U)
#define XAH_OUT    (txn + 26U)
#define HOOK_ACC   (txn + 120U)
#define DEST_ACC   (txn + 142U)
#define EMIT_OUT   (txn + 162U) 


#define SETUP_CURRENT_MONTH()\
uint8_t current_month = 0;\
{\
    int64_t s = ledger_last_time() + 946684800;\
    int64_t z = s / 86400 + 719468;\
    int64_t era = (z >= 0 ? z : z - 146096) / 146097;\
    uint64_t doe = (uint64_t)(z - era * 146097);\
    uint64_t yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;\
    int64_t y = (int64_t)(yoe) + era * 400;\
    uint64_t doy = doe - (365*yoe + yoe/4 - yoe/100);\
    uint64_t mp = (5*doy + 2)/153;\
    uint64_t d = doy - (153*mp+2)/5 + 1;\
    uint64_t m = mp + (mp < 10 ? 3 : -9);\
    y += (m <= 2);\
    current_month = m;\
}


int64_t hook(uint32_t) {

    uint8_t business_acc[21] = { 0x00U };
    uint8_t business_key[1] = { 'B' };
    if (otxn_param(business_acc, 20, SBUF(business_key)) != 20)
        rollback(SBUF("Direct debit: Subscription Account Info Missing"), 1);

    BYTES20_TO_BUF(DEST_ACC, business_acc);

    uint8_t payment_ns[32] = { 0xB1U, 0xCEU, 0x0EU, 0x75U, 0xAEU, 0x18U, 0x22U, 0x3DU, 0x25U, 0x16U, 0x1EU, 0x4BU, 0x64U, 0x6FU, 0x6FU, 0xC0U, 0x3DU, 0x5CU, 0xFEU, 0x51U, 0xAEU, 0x8EU, 0x2DU, 0x68U, 0xB1U, 0xAEU, 0x87U, 0x65U, 0x65U, 0x83U,0x82U, 0xC5U }; // PAYMENT
    uint8_t receipt_ns[32] = { 0xAAU, 0x71U, 0xEAU, 0xB4U, 0xF7U, 0xDDU, 0xF7U, 0x04U, 0xAAU, 0x71U, 0x5DU, 0x97U, 0x81U, 0xF2U, 0x90U, 0x1DU, 0xB5U, 0xA4U, 0xD4U, 0x3EU, 0x6FU, 0x3FU, 0xB3U, 0x18U, 0xDFU, 0xAAU, 0x93U, 0xCEU, 0x11U, 0x94U, 0x62U, 0x49U }; // RECEIPT

    uint8_t count[1] = { 0x00U };

    uint8_t sub_no[3] = { 'N', 'U', 'M' };
    uint8_t num[1] = { 0x00U };
    int8_t isSub = otxn_param(SBUF(num), SBUF(sub_no));

    uint8_t hook_acc[20];
    otxn_field(SBUF(hook_acc), sfAccount);
    hook_account(HOOK_ACC, 20);
    if (BUFFER_EQUAL_20(HOOK_ACC, hook_acc)) {

        if(state(SBUF(count), business_acc, 32) == 1) {

            if(isSub == 1) {
                // UNSUBSCRIBE
                if(count[0] == 0x00U) {
                    state_set(0, 0, business_acc, 32);
                    business_acc[20] = num[0];
                    state_foreign_set(0, 0, business_acc, 32, SBUF(payment_ns), SBUF(hook_acc));
                    state_foreign_set(0, 0, business_acc, 32, SBUF(receipt_ns), SBUF(hook_acc));                
                } else if(count[0] == num[0]) {
                    --count[0];
                    state_set(SBUF(count), business_acc, 32);
                    business_acc[20] = num[0];
                    state_foreign_set(0, 0, business_acc, 32, SBUF(payment_ns), SBUF(hook_acc));
                    state_foreign_set(0, 0, business_acc, 32, SBUF(receipt_ns), SBUF(hook_acc));    
                } else if(count[0] > num[0]) {
                    state_foreign_set(0x00U, 1, business_acc, 32, SBUF(payment_ns), SBUF(hook_acc));
                    state_foreign_set(0x00U, 1, business_acc, 32, SBUF(receipt_ns), SBUF(hook_acc));    
                } else {
                    rollback(SBUF("Direct debit: Wrong Subscription Number Passed"), 2);    
                }
                accept(SBUF("Direct Debit: Successfully Unsubscribed."), 3);
            }

            ++count[0];  
        } 
        // SUBSCRIBE
        state_set(SBUF(count), business_acc, 32);    

        uint8_t request_buf[48]; // < xlf 8b req amount, 20b currency, 20b issuer > 
        uint8_t request_key[3] = { 'A', 'M', 'T' };

        int8_t isNative = otxn_param(SBUF(request_buf), SBUF(request_key));
        if (isNative < 8)
            rollback(SBUF("Direct debit: Wrong AMT - < xlf 8b req amount, 20b currency, 20b issuer >"), 4);

        int64_t request = *((int64_t*)request_buf);

        if (float_compare(request, 0, COMPARE_LESS | COMPARE_EQUAL) == 1)
            rollback(SBUF("Direct debit: Invalid Amount"), 5);   

        if(isNative == 8) {
            int64_t toDrops =  float_multiply(request, 6197953087261802496);
            if(float_sto(XAH_OUT, 48, 0, 0, 0, 0, toDrops, 0) < 0) {
                rollback(SBUF("Direct debit: Wrong AMT - Invalid Native Currency"), 6);
            }
        } else {
            if(float_sto(AMOUNT_OUT,  49, request_buf + 8, 20, request_buf + 28, 20, request, sfAmount) < 0) {
                rollback(SBUF("Direct debit: Wrong AMT - < xlf 8b req amount, 20b currency, 20b issuer >"), 7);
            }
        }

        business_acc[20] = count[0];
        state_foreign_set(isNative == 8 ? (XAH_OUT - 1) : AMOUNT_OUT, 49, business_acc, 32, SBUF(payment_ns), SBUF(hook_acc));
        accept(SBUF("Direct Debit: Successfully Subscribed."), 8);

    } else {
        otxn_field(SBUF(hook_acc), sfDestination);
        if(state(SBUF(count), business_acc, 32) != 1) {
            rollback(SBUF("Direct debit: Not Subscribed."), 9);
        }

        business_acc[20] = num[0];
        if(state_foreign(AMOUNT_OUT, 49, business_acc, 32, SBUF(payment_ns), SBUF(hook_acc)) < 8) {
            rollback(SBUF("Direct debit: Probably Unsubscribed."), 10);
        }

        SETUP_CURRENT_MONTH();
        uint8_t paid = 0;
        if(state_foreign(SVAR(paid), business_acc, 32, SBUF(receipt_ns), SBUF(hook_acc)) == 1) {
            if(paid == current_month) {
                rollback(SBUF("Direct debit: Paid for the month."), 11);
            }
        }
        state_foreign_set(SVAR(current_month), business_acc, 32, SBUF(receipt_ns), SBUF(hook_acc));

        etxn_reserve(1);
        uint32_t fls = (uint32_t)ledger_seq() + 1;
        *((uint32_t *)(FLS_OUT)) = FLIP_ENDIAN(fls);
        uint32_t lls = fls + 4;
        *((uint32_t *)(LLS_OUT)) = FLIP_ENDIAN(lls);
        etxn_details(EMIT_OUT, 116U);
        {
            int64_t fee = etxn_fee_base(SBUF(txn));
            uint8_t *b = FEE_OUT;
            *b++ = 0b01000000 + ((fee >> 56) & 0b00111111);
            *b++ = (fee >> 48) & 0xFFU;
            *b++ = (fee >> 40) & 0xFFU;
            *b++ = (fee >> 32) & 0xFFU;
            *b++ = (fee >> 24) & 0xFFU;
            *b++ = (fee >> 16) & 0xFFU;
            *b++ = (fee >> 8) & 0xFFU;
            *b++ = (fee >> 0) & 0xFFU;
        }
        uint8_t emithash[32]; 
        if(emit(SBUF(emithash), SBUF(txn)) != 32)
            rollback(SBUF("Direct Debit: Failed To Emit."), 12);    
   }
        
    accept(SBUF("Direct Debit: Successful."), 13);
   _g(1,1);
   return 0;    
}