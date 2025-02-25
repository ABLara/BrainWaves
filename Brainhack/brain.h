
#ifndef Brain_h
#define Brain_h


#define MAX_PACKET_LENGTH 32
#define EEG_POWER_BANDS 8

// TK some kind of reset timer if we don't receive a packet for >3 seconds?

#ifdef _LINUX_
                void brain_init(int fd);
#else
			#include<windows.h>
                void brain_init(HANDLE fd);
#endif
		// Run this in the main loop.
		int brain_update();

		// String with most recent error.
		char* brain_readErrors();

		// Returns comme-delimited string of all available brain data.
		// Sequence is as below.
		char* brain_readCSV();

		// Individual pieces of brain data.
		unsigned char brain_readSignalQuality();
		unsigned char brain_readAttention();
		unsigned char brain_readMeditation();
		unsigned long* brain_readPowerArray();
		unsigned long brain_readDelta();
		unsigned long brain_readTheta();
		unsigned long brain_readLowAlpha();
		unsigned long brain_readHighAlpha();
		unsigned long brain_readLowBeta();
		unsigned long brain_readHighBeta();
		unsigned long brain_readLowGamma();
		unsigned long brain_readMidGamma();

                void brain_printDebug();


#endif



