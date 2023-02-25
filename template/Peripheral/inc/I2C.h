#ifndef USER_I2C_H_
#define USER_I2C_H_


// The Wire library is a C++ class; I've created a work-alike to my
// BitBang_I2C API which is a set of C functions to simplify I2C

void I2CInit(int iSpeed);
void I2CWrite(uint8_t u8Addr, uint8_t *pData, int iLen);
void I2CRead(uint8_t u8Addr, uint8_t *pData, int iLen);
int I2CTest(uint8_t u8Addr);


#endif /* USER_I2C_H_ */
