#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_wav_header(FILE* myfile, unsigned int *samp_rate, unsigned int *bits_per_samp, unsigned int *num_samp);
void read_wav_data(FILE* myfile, int *data, unsigned int samp_rate, unsigned int bits_per_samp, unsigned int num_samp);
int conv_bit_size(unsigned int in, int bps);

/*THX TO http://ubuntuforums.org/showthread.php?t=960693*/

int main(void)
{
   const char* filename = "C:\\Users\\Samuel\\Documents\\Visual Studio 2013\\Projects\\SoundTest\\SoundTest\\sine_wobble.wav";
   FILE* myfile; fopen_s(&myfile, filename, "rb");

   unsigned int samp_rate, bits_per_samp, num_samp;
   read_wav_header(myfile, &samp_rate, &bits_per_samp, &num_samp);
	printf("samp_rate=[%d] bits_per_samp=[%d] num_samp=[%d]\n",
		     samp_rate, bits_per_samp, num_samp);

	int *data = (int *) malloc(num_samp * sizeof(int));
   read_wav_data(myfile, data, samp_rate, bits_per_samp, num_samp);

   fclose(myfile);
	unsigned int i;
	for (i = 0; i < num_samp; i+=2) {
		printf("%d %d\n", data[i], data[i+1]);
	}

	return EXIT_SUCCESS;
}

void read_wav_header(FILE* myfile, unsigned int *samp_rate, unsigned int *bits_per_samp, unsigned int *num_samp)
{
        unsigned char buf[5];

        /* ChunkID (RIFF for little-endian, RIFX for big-endian) */
        fread(buf, 1, 4, myfile);
        buf[4] = '\0';
        if (strcmp((char*)buf, "RIFF")) exit(EXIT_FAILURE);

        /* ChunkSize */
        fread(buf, 1, 4, myfile);

        /* Format */
        fread(buf, 1, 4, myfile);
        buf[4] = '\0';
        if (strcmp((char*)buf, "WAVE")) exit(EXIT_FAILURE);

        /* Subchunk1ID */
        fread(buf, 1, 4, myfile);
        buf[4] = '\0';
        if (strcmp((char*)buf, "fmt ")) exit(EXIT_FAILURE);

        /* Subchunk1Size (16 for PCM) */
        fread(buf, 1, 4, myfile);
        if (buf[0] != 16 || buf[1] || buf[2] || buf[3]) exit(EXIT_FAILURE);

        /* AudioFormat (PCM = 1, other values indicate compression) */
        fread(buf, 1, 2, myfile);
        if (buf[0] != 1 || buf[1]) exit(EXIT_FAILURE);

        /* NumChannels (Mono = 1, Stereo = 2, etc) */
        fread(buf, 1, 2, myfile);
        unsigned int num_ch = buf[0] + (buf[1] << 8);
	if (num_ch != 2) exit(EXIT_FAILURE); /*WE REQUIRE STEREO!*/

        /* SampleRate (8000, 44100, etc) */
        fread(buf, 1, 4, myfile);
        *samp_rate = buf[0] + (buf[1] << 8) +
                (buf[2] << 16) + (buf[3] << 24);

        /* ByteRate (SampleRate * NumChannels * BitsPerSample / 8) */
        fread(buf, 1, 4, myfile);
        const unsigned int byte_rate = buf[0] + (buf[1] << 8) +
                (buf[2] << 16) + (buf[3] << 24);

        /* BlockAlign (NumChannels * BitsPerSample / 8) */
        fread(buf, 1, 2, myfile);
        const unsigned int block_align = buf[0] + (buf[1] << 8);

        /* BitsPerSample */
        fread(buf, 1, 2, myfile);
        *bits_per_samp = buf[0] + (buf[1] << 8);

        if (byte_rate != ((*samp_rate * num_ch * *bits_per_samp) >> 3))
		exit(EXIT_FAILURE);

        if (block_align != ((num_ch * *bits_per_samp) >> 3))
		exit(EXIT_FAILURE);

        /* Subchunk2ID */
        fread(buf, 1, 4, myfile);
        buf[4] = '\0';
        if (strcmp((char*)buf, "data")) exit(EXIT_FAILURE);

        /* Subchunk2Size (NumSamples * NumChannels * BitsPerSample / 8) */
        fread(buf, 1, 4, myfile);
        const unsigned int subchunk2_size = buf[0] + (buf[1] << 8) +
                (buf[2] << 16) + (buf[3] << 24);
        *num_samp = (subchunk2_size << 3) / (
                        num_ch * *bits_per_samp);
}


void read_wav_data(FILE* myfile, int *data, unsigned int samp_rate, unsigned int bits_per_samp, unsigned int num_samp)
{
   unsigned char buf;
	unsigned int i, j;
   for (i=0; i < num_samp; ++i) {
		unsigned int tmp = 0;
		for (j=0; j != bits_per_samp; j+=8) {
			fread(&buf, 1, 1, myfile);
			tmp += buf << j;
		}
		data[i] = conv_bit_size(tmp, bits_per_samp);
   }
}


int conv_bit_size(unsigned int in, int bps)
{
        const unsigned int max = (1 << (bps-1)) - 1;
        return in > max ? in - (max<<1) : in;
}
