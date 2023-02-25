#ifndef USER_SPI_H_
#define USER_SPI_H_

// SPI1 (polling mode)
void SPI_write(uint8_t *pData, int iLen);
void SPI_begin(int iSpeed, int iMode);

#endif /* USER_SPI_H_ */
