

#include <stdio.h>
#include <unistd.h>

#include "brain.h"

                #ifdef _LINUX_
                int serial_fd;
                #else
                HANDLE serial_fd;
                #endif

		unsigned char packetData[MAX_PACKET_LENGTH];
		int inPacket;
		unsigned char latestByte;
		unsigned char lastByte;
		unsigned char packetIndex;
		unsigned char packetLength;
		unsigned char checksum;
		unsigned char checksumAccumulator;
		unsigned char eegPowerLength;
		int hasPower;


		void brain_clearPacket();
		void brain_clearEegPower();
		int brain_parsePacket();
		void brain_printPacket();
		void brain_init();
		void brain_printCSV(); // maybe should be public?
		void brain_printDebug();

		// With current hardware, at most we would have...
		// 3 x 3 char unsigned chars
		// 8 x 10 char ulongs
		// 10 x 1 char commas
		// 1 x 1 char 0 (string termination)
		// -------------------------
		// 100 characters
		char csvBuffer[100];

		// Longest error is
		// 22 x 1 char unsigned chars
		// 1 x 1 char 0 (string termination)
		char latestError[23];

		unsigned char signalQuality;
		unsigned char attention;
		unsigned char meditation;

		int freshPacket;

		// Lighter to just make this public, instead of using the getter?
		unsigned long eegPower[EEG_POWER_BANDS];


#ifdef _LINUX_
void brain_init(int fd) {
#else
void brain_init(HANDLE fd) {
#endif
        serial_fd=fd;
	freshPacket = 0;
	inPacket = 0;
	packetIndex = 0;
	packetLength = 0;
	checksum = 0;
	checksumAccumulator = 0;
	eegPowerLength = 0;
	hasPower = 0;

	signalQuality = 200;
	attention = 0;
	meditation = 0;
	brain_clearEegPower();
}

int brain_update() {

#ifdef _LINUX_
	if ((read (serial_fd,&latestByte,1)) == 1) {
#else
        unsigned long nbytes;
		ReadFile(serial_fd, &latestByte, 1,&nbytes, NULL);
	if (nbytes == 1) {
#endif
                printf("byte=%03d\n",latestByte);

		// Build a packet if we know we're and not just listening for sync unsigned chars.
		if (inPacket) {

			// First unsigned char after the sync bytes is the length of the upcoming packet.
			if (packetIndex == 0) {
				packetLength = latestByte;

				// Catch error if packet is too long
				if (packetLength > MAX_PACKET_LENGTH) {
					// Packet exceeded max length
					// Send an error
					sprintf(latestError, "ERROR: Packet too long");
					inPacket = 0;
				}
			}
			else if (packetIndex <= packetLength) {
				// Run of the mill data unsigned chars.

				// Print them here

				// Store the unsigned char in an array for parsing later.
				packetData[packetIndex - 1] = latestByte;

				// Keep building the checksum.
				checksumAccumulator += latestByte;
			}
			else if (packetIndex > packetLength) {
				// We're at the end of the data payload.

				// Check the checksum.
				checksum = latestByte;
				checksumAccumulator = 255 - checksumAccumulator;

				// Do they match?
				if (checksum == checksumAccumulator) {

					// Parse the data. parsePacker() returns true if parsing succeeds.
					if (brain_parsePacket()) {
						freshPacket = 1;
					}
					else {
						// Parsing failed, send an error.
						sprintf(latestError, "ERROR: Could not parse");
						// good place to print the packet if debugging
					}
				}
				else {
					// Checksum mismatch, send an error.
					sprintf(latestError, "ERROR: Checksum");
					// good place to print the packet if debugging
				}
				// End of packet

				// Reset, prep for next packet
				inPacket = 0;
			}

			packetIndex++;
		}

		// Look for the start of the packet
		if ((latestByte == 170) && (lastByte == 170) && !inPacket) {
			// Start of packet
			inPacket = 1;
			packetIndex = 0;
			packetLength = 0; // Technically not necessarry.
			checksum = 0; // Technically not necessary.
			checksumAccumulator = 0;
			//clearPacket(); // Zeros the packet array, technically not necessarry.
			//clearEegPower(); // Zeros the EEG power. Necessary if hasPower turns false... better off on the gettter end?
		}

		// Keep track of the last unsigned char so we can find the sync byte pairs.
		lastByte = latestByte;
	}

	if(freshPacket) {
		freshPacket = 0;
		return 1;
	}
	else {
		return 0;
	}

}

void brain_clearPacket() {
   unsigned char i;
	for (i = 0; i < MAX_PACKET_LENGTH; i++) {
		packetData[i] = 0;
	}
}

void brain_clearEegPower() {
   unsigned char i;
	// Zero the power bands.
	for(i = 0; i < EEG_POWER_BANDS; i++) {
		eegPower[i] = 0;
	}
}

int brain_parsePacket() {
   unsigned char i;
   int j;
	// Loop through the packet, extracting data.
	// Based on mindset_communications_protocol.pdf from the Neurosky Mindset SDK.
	hasPower = 0;
	brain_clearEegPower();	// clear the eeg power to make sure we're honest about missing values... null would be better than 0.

	for (i = 0; i < packetLength; i++) {
		switch (packetData[i]) {
			case 2:
				signalQuality = packetData[++i];
				break;
			case 4:
				attention = packetData[++i];
				break;
			case 5:
				meditation = packetData[++i];
				break;
			case 131:
				// ASIC_EEG_POWER: eight big-endian 3-unsigned char unsigned integer values representing delta, theta, low-alpha high-alpha, low-beta, high-beta, low-gamma, and mid-gamma EEG band power values
				// The next unsigned char sets the length, usually 24 (Eight 24-bit numbers... big endian?)
				eegPowerLength = packetData[++i];

				// Extract the values. Possible memory savings here by creating three temp longs?
				for(j = 0; j < EEG_POWER_BANDS; j++) {
					//eegPower[j] = ((unsigned long)packetData[++i] << 16) | ((unsigned long)packetData[++i] << 8) | (unsigned long)packetData[++i];
					eegPower[j] = ((unsigned long)packetData[i+1] << 16) | ((unsigned long)packetData[i+2] << 8) | (unsigned long)packetData[i+3];
                                        i+=3;
				}

				hasPower = 1;
				// This seems to happen once during start-up on the force trainer. Strange. Wise to wait a couple of packets before
				// you start reading.

				break;
			default:
				return 0;
		}
	}
	return 1;
}

// DEPRECATED, sticking around for debug use
void brain_printCSV() {
    int i;
	// Print the CSV over serial
	printf("%d",signalQuality);
	printf(",");
	printf("%d",attention);
	printf(",");
	printf("%d",meditation);

	if (hasPower) {
		for(i = 0; i < EEG_POWER_BANDS; i++) {
			printf(",");
			printf("%d",(unsigned int)eegPower[i]);
		}
	}

	printf("\n");
}

char* brain_readErrors() {
	return latestError;
}

char* brain_readCSV() {
	// spit out a big string?
	// find out how big this really needs to be
	// should be popped off the stack once it goes out of scope?
	// make the character array as small as possible

	if(hasPower) {

		sprintf(csvBuffer,"%d,%d,%d,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu",
			signalQuality,
			attention,
			meditation,
			eegPower[0],
			eegPower[1],
			eegPower[2],
			eegPower[3],
			eegPower[4],
			eegPower[5],
			eegPower[6],
			eegPower[7]
		);

		return csvBuffer;
	}
	else {
		sprintf(csvBuffer,"%d,%d,%d",
			signalQuality,
			attention,
			meditation
		);

		return csvBuffer;
	}
}

// For debugging, print the entire contents of the packet data array.
void brain_printPacket() {
  unsigned char i;
	printf("[");
	for (i = 0; i < MAX_PACKET_LENGTH; i++) {
		printf("%d",packetData[i]);

			if (i < MAX_PACKET_LENGTH - 1) {
				printf(", ");
			}
	}
	printf("]\n");
}

void brain_printDebug() {
	printf("\n");
	printf("--- Start Packet ---\n");
	printf("Signal Quality: ");
	printf("%d\n",signalQuality);
	printf("Attention: ");
	printf("%d\n",attention);
	printf("Meditation: ");
	printf("%d\n",meditation);

	if (hasPower) {
		printf("\n");
		printf("EEG POWER:\n");
		printf("Delta: ");
		printf("%ld\n",eegPower[0]);
		printf("Theta: ");
		printf("%ld\n",eegPower[1]);
		printf("Low Alpha: ");
		printf("%ld\n",eegPower[2]);
		printf("High Alpha: ");
		printf("%ld\n",eegPower[3]);
		printf("Low Beta: ");
		printf("%ld\n",eegPower[4]);
		printf("High Beta: ");
		printf("%ld\n",eegPower[5]);
		printf("Low Gamma: ");
		printf("%ld\n",eegPower[6]);
		printf("Mid Gamma: ");
		printf("%ld\n",eegPower[7]);

	}

	printf("\n");
	printf("Checksum Calculated: ");
	printf("%d\n",checksumAccumulator);
	printf("Checksum Expected: ");
	printf("%d\n",checksum);

	printf("--- End Packet ---\n");
	printf("\n");
}

unsigned char brain_readSignalQuality() {
	return signalQuality;
}

unsigned char brain_readAttention() {
	return attention;
}

unsigned char brain_readMeditation() {
	return meditation;
}

unsigned long* brain_readPowerArray() {
	return eegPower;
}

unsigned long brain_readDelta() {
	return eegPower[0];
}

unsigned long brain_readTheta() {
	return eegPower[1];
}

unsigned long brain_readLowAlpha() {
	return eegPower[2];
}

unsigned long brain_readHighAlpha() {
	return eegPower[3];
}

unsigned long brain_readLowBeta() {
	return eegPower[4];
}

unsigned long brain_readHighBeta() {
	return eegPower[5];
}

unsigned long brain_readLowGamma() {
	return eegPower[6];
}

unsigned long brain_readMidGamma() {
	return eegPower[7];
}
