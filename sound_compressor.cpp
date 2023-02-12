#include <stdio.h>
#include <sndfile.h>
#include <cassert>

#define BUFFER_LEN 1024
#define MAX_CHANNELS 6 
#define ERROR_OPEN_INPUT -1
#define ERROR_OPEN_OUTPUT -2

SNDFILE* SOUND_FILE;
SNDFILE* OUTPUT_FILE;
SF_INFO SFINFO_INPUT;
SF_INFO SFINFO_OUTPUT;

void process_data (double *data, int count, int channels);
void fill_info(SF_INFO* input, SF_INFO* output);

int main(){

    if (!(SOUND_FILE = sf_open("sample-15s.wav", SFM_READ, &SFINFO_INPUT)))
    {
        printf("%s\n", sf_strerror(SOUND_FILE));
        printf("Unable to open the input file\n");
        return ERROR_OPEN_INPUT;
    }

    fill_info(&SFINFO_INPUT, &SFINFO_OUTPUT);

    if (!(OUTPUT_FILE = sf_open("output.wav", SFM_WRITE, &SFINFO_OUTPUT)))
    {
        printf("%s\n", sf_strerror(OUTPUT_FILE));
        printf("Unable to open the output file\n");
        return ERROR_OPEN_OUTPUT;
    }

    int readcount = 0;
    double data [BUFFER_LEN];

    while ((readcount = sf_read_double (SOUND_FILE, data, BUFFER_LEN)))
    {  
        process_data (data, readcount, SFINFO_INPUT.channels) ;
        sf_write_double (OUTPUT_FILE, data, readcount) ;
    }
    
    sf_close(SOUND_FILE);
    sf_close(OUTPUT_FILE);

    return 0;
}

void fill_info(SF_INFO* input, SF_INFO* output)
{   
        assert(input && output);

        output->samplerate = input->samplerate;
        output->channels = input->channels;
        output->format = input->format;
}

void process_data (double *data, int count, int channels)
{    
    double channel_gain [MAX_CHANNELS] = { 0.5, 0.8, 0.1, 0.4, 0.4, 0.9 } ;
    int k, chan ;

    for (chan = 0 ; chan < channels ; chan ++)
        for (k = chan ; k < count ; k+= channels)
            data [k] *= channel_gain [chan] ;

    return ;
}
