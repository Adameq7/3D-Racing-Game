#include <stdio.h>
#include <stdlib.h>
#include <bass.h>

void PlayMusic(char* c)
{
    HSTREAM streamHandle;
	BASS_Init (-1, 44100, 0, 0, NULL);
    streamHandle = BASS_StreamCreateFile(FALSE, c, 0, 0, 0);
    BASS_ChannelPlay(streamHandle, FALSE);
}
