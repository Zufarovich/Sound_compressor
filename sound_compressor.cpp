#include <stdio.h>
#include <sndfile.h>
#include <cassert>
#include <fftw3.h>

#define BUFFER_LEN 512
#define MAX_CHANNELS 6 
#define ERROR_OPEN_INPUT -1
#define ERROR_OPEN_OUTPUT -2
#define EXTRA_CHANNELS -3

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

    if (SFINFO_INPUT.channels > MAX_CHANNELS)
    {
        printf("Not able to process more than %d channels\n", MAX_CHANNELS);
        return EXTRA_CHANNELS;
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

    fftw_complex *out; 
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * BUFFER_LEN/2 + 1);
    fftw_plan p = fftw_plan_dft_r2c_1d(BUFFER_LEN, data, out, FFTW_ESTIMATE);

    while ((readcount = sf_read_double (SOUND_FILE, data, BUFFER_LEN)))
    {  
        process_data (data, readcount, SFINFO_INPUT.channels) ;
        sf_write_double (OUTPUT_FILE, data, readcount) ;
        fftw_execute(p);

        for (int i = 0; i < readcount; i++)
            printf("%lf %lf\n", *out[i*2], *out[1 + i*2]);
    } 
    
    fftw_destroy_plan(p);
    fftw_free (out); 

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
    double channel_gain [MAX_CHANNELS] = { 1, 1, 1, 1, 1, 1 } ;
    int k, chan ;

    for (chan = 0 ; chan < channels ; chan ++)
        for (k = chan ; k < count ; k+= channels)
            data [k] *= channel_gain [chan] ;

    return ;
}
