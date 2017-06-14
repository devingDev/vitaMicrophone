#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/io/fcntl.h> 
#include <psp2/audioin.h> 
#include <stdio.h>
#include <string>
#include <string.h>

#include "debugScreen.h"
#include "wavheader.h"


#define printf psvDebugScreenPrintf

/*

Relevant header : https://github.com/vitasdk/vita-headers/blob/master/include/psp2/audioin.h


#Porttype: 0 Parameter 0: 
#	Grain : 256  , 512
#
#Porttype: 2 Parameter 0 :
#	Grain: 256




ERROR return values are defined in the header audioin.h !

OPEN A PORT : 
	int sceAudioInOpenPort(int portType, int grain, int freq, int param);
	
	
		@Parameters
		porttype : 
			SCE_AUDIO_IN_PORT_TYPE_VOICE = 0
			SCE_AUDIO_IN_PORT_TYPE_RAW	= 0x0002
			
		frequency:
			160000
			?
			
		grain : 256 or 512 with porttype = 0 . 256 with porttype = 2

		param : 
			Probably these below, 0 works.
			SCE_AUDIO_IN_PARAM_FORMAT_S16_MONO = 0
			SCE_AUDIO_IN_GETSTATUS_MUTE		= 1		
	
		@Return
		If it could open a port it returns a positive number;
		If it fails to open a port it returns an error code ( negative value )
			example errors : SCE_AUDIO_IN_ERROR_INVALID_SIZE <=> wrong grain value
							SCE_AUDIO_IN_ERROR_INVALID_SAMPLE_FREQ <=> wrong frequency value 
												



	
	
CLOSE A PORT : 
	int sceAudioInReleasePort(int port);
	
		If it closes a port it returns 0.
		If it fails it returns an error code ( negative value ), example SCE_AUDIO_IN_ERROR_NOT_OPENED
		
		
		
READ INPUT (?):
	int sceAudioInInput(int port, void *destPtr);
		
		@Parameters
		port : an opened port number
		
		destPtr: pointer to a buffer 
	
		@Return
		If passed an open port + buffer , it returns 0.
		If passed an invalid port it will return an Error code ( negative value ).


*/

void testAudioInPort(int porttype , int grain , int frequency , int param);



int main(int argc, char *argv[]) {
	psvDebugScreenInit();
	printf("Audio Mic Sample !\r\n\r\n");
	sceKernelDelayThread(1*500000);
	
	int portType = SCE_AUDIO_IN_PORT_TYPE_VOICE;
	int grain = 256;
	int frequency = 16000;
	int param = SCE_AUDIO_IN_PARAM_FORMAT_S16_MONO;
	testAudioInPort(portType , grain , frequency , param); // test1
	
	portType = SCE_AUDIO_IN_PORT_TYPE_VOICE;
	grain = 512;
	frequency = 16000;
	param = SCE_AUDIO_IN_PARAM_FORMAT_S16_MONO;
	testAudioInPort(portType , grain , frequency , param); // test2
	
	portType = SCE_AUDIO_IN_PORT_TYPE_RAW;
	grain = 256;
	frequency = 16000;
	param = SCE_AUDIO_IN_PARAM_FORMAT_S16_MONO;
	testAudioInPort(portType , grain , frequency , param); // test3
	
	
	printf("\r\n\r\nDone !");
	sceKernelDelayThread(1*20000000); // 20s
	sceKernelExitProcess(0);
    return 0;
}

void testAudioInPort(int portType , int grain , int frequency , int param){
	
	printf("Opening audio input port...\r\nporttype : %d  grain : %d  frequency : %d  param : %d\r\n" , portType , grain , frequency , param);
	int openPort = sceAudioInOpenPort(portType, grain , frequency,  param);
	
	sceKernelDelayThread(1*100000);
	
	if(openPort < 0){
		printf("Failed to open audio input port ! Error is : %d\r\n", openPort);
	}else{
		printf("Opened port ! Port number is : %d\r\n", openPort);
		
		sceKernelDelayThread(2*1000000);
		
		printf("For loop with sceAudioInInput() !\r\n");
		unsigned int buffer_size = grain * 2 ;  // note : I think for how I handle the buffer, buffer_size has to be grain * 2. I tried grain *2 * 10 and the audio was really big, because it probably saved many ZEROs :)
		char * _micBuffer = new char[buffer_size];
		int inInput = 0;
		int loops = 0; //  loops = 1000 => If grain is 256 will yield 16 seconds.  32 seconds if grain is 512 .
							
		int recordTimeInMS = 5000 ;						// 5 seconds
		loops = static_cast<int>(recordTimeInMS /  16 * 256 / grain) ;	// formula => loops = (Miliseconds_wanted / 16 * 256 / grain)
		int samples = (recordTimeInMS / 16) * 256;												// Now even if grain is 512 it should have same duration 
		
		
		std::string fileName = "ux0:data/audioInputSampleBufferContent-porttype" + std::to_string(portType) + "-grain" + std::to_string(grain) + ".txt";
		std::string savePrint = "\r\nSaving audio input buffer content to : " + fileName + " also a .wav was created !";
		int fhl = sceIoOpen(fileName.c_str(), SCE_O_WRONLY | SCE_O_CREAT, 0777);
		int writtenBytes = 0;

		/* CREATE WAV Header */
		std::string filenameWav = fileName + ".wav";
		int fhwav = sceIoOpen(filenameWav.c_str() , SCE_O_WRONLY | SCE_O_CREAT , 0777);
		
		struct wavfile headerWav;
		strcpy(headerWav.id , "RIFF");
		headerWav.totallength = (samples * 1) + sizeof(struct wavfile) -8; // size in bytes of raw data +  sizeof(wavfile) -8;
		strcpy(headerWav.wavefmt , "WAVEfmt ");
		headerWav.format = 16;
		headerWav.pcm = 1;
		headerWav.channels = 1;
		headerWav.frequency = 16000;
		headerWav.bits_per_sample = 16;
		headerWav.bytes_per_second = headerWav.channels * headerWav.frequency * headerWav.bits_per_sample / 8;
		headerWav.bytes_by_capture = headerWav.channels * headerWav.bits_per_sample / 8;
		headerWav.bytes_in_data = samples * headerWav.channels * headerWav.bits_per_sample / 8;
		strcpy(headerWav.data , "data");

		//char headerWavBuf[sizeof(headerWav)];
		//memcpy( headerWavBuf , &headerWav , sizeof(headerWav));
		sceIoWrite(fhwav , &headerWav , sizeof(headerWav));
		
		for(int i = 0; i < loops ; i++){
			inInput = sceAudioInInput (openPort, _micBuffer);
			writtenBytes = sceIoWrite(fhl, _micBuffer, buffer_size);
			sceIoWrite(fhwav , _micBuffer , buffer_size );
                }
                sceIoClose(fhl);
		sceIoClose(fhwav);
		
		
		
		printf("Closing audio input port..\r\n");
		int releasePort = sceAudioInReleasePort ( openPort );
		
		printf("return value of sceAudioInReleasePort() : %d\r\n" , releasePort);
		sceKernelDelayThread(1*200000);
		
		
		printf(savePrint.c_str());
	
		
		
	}
	
}


