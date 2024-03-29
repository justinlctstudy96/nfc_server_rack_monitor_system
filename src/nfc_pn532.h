#ifndef __NFC_PN532_H__
#define __NFC_PN532_H__

#define CS1 4
#define CS2 5
#define CS3 16
#define CS4 13
#define CS5 14
#define CS6 21
#define CS7 22
#define CS8 25
#define CS9 26
#define CS10 27

#define NFC_TIMEOUT 100

#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

#define TAG_NOT_PRESENT "XXXX"
#define TAG_NULL_CONTENT "NNNN"

// nfc configuration //
#define READER_COUNT 10
extern NfcAdapter nfc1;
extern NfcAdapter nfc2;
extern NfcAdapter nfc3;
extern NfcAdapter nfc4;
extern NfcAdapter nfc5;
extern NfcAdapter nfc6;
extern NfcAdapter nfc7;
extern NfcAdapter nfc8;
extern NfcAdapter nfc9;
extern NfcAdapter nfc10;
///////////////////////


extern String NFCs_uid [READER_COUNT];
extern String NFCs_content [READER_COUNT];

void nfc_begin();
void nfc_read_all();
void printNFCsContents();
void nfc_read(NfcAdapter &nfc, int reader_num);
boolean nfc_write(NfcAdapter &nfc, String content);

#endif