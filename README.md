# libSceAudioIn - microphone

This will compile into a vpk to be installed on your vita.
It will run 3 recordings , each for 5 seconds , and save the RAW output ( 16 bit PCM , 16000 Hz , Mono-1 Channel )
as .txt files to ux0:data/ . It also will save each one with a wav header, so it can be played in most mediaplayers. Vitashell's player does not seem to accept .wav though .

Each of the recordings differ slightly in their parameters, but each one seems to work ok.


You can also load the Raw data in Audacity [ File -> Import.. -> Raw data ] 
Choose your .txt and give it these settings:
	Encoding : Signed 16 bit PCM
	Byte order : Little Endian
	Channels : 1 Channel (Mono)
	Start offset : 0 [bytes]
	Amount to import : 100 [%]
	Sample rate : 16000 [Hz]








