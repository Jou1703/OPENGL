
#include "base64.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// a larger size should be faster, but takes more runtime memory
//BUFFERSIZE = 4096
//BUFFERSIZE = 65536
#define BUFFERSIZE 16777216

///////////////////////////encode
const int CHARS_PER_LINE = 72;

void base64_init_encodestate(base64_encodestate* state_in)
{
	state_in->step = step_A;
	state_in->result = 0;
	state_in->stepcount = 0;
}

char base64_encode_value(char value_in)
{
	static const char* encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	if (value_in > 63) return '=';
	return encoding[(int)value_in];
}

int base64_encode_block(const char* plaintext_in, int length_in, char* code_out, base64_encodestate* state_in)
{
	const char* plainchar = plaintext_in;
	const char* const plaintextend = plaintext_in + length_in;
	char* codechar = code_out;
	char result;
	char fragment;
	
	result = state_in->result;
	
	switch (state_in->step)
	{
		while (1)
		{
	case step_A:
			if (plainchar == plaintextend)
			{
				state_in->result = result;
				state_in->step = step_A;
				return codechar - code_out;
			}
			fragment = *plainchar++;
			result = (fragment & 0x0fc) >> 2;
			*codechar++ = base64_encode_value(result);
			result = (fragment & 0x003) << 4;
	case step_B:
			if (plainchar == plaintextend)
			{
				state_in->result = result;
				state_in->step = step_B;
				return codechar - code_out;
			}
			fragment = *plainchar++;
			result |= (fragment & 0x0f0) >> 4;
			*codechar++ = base64_encode_value(result);
			result = (fragment & 0x00f) << 2;
	case step_C:
			if (plainchar == plaintextend)
			{
				state_in->result = result;
				state_in->step = step_C;
				return codechar - code_out;
			}
			fragment = *plainchar++;
			result |= (fragment & 0x0c0) >> 6;
			*codechar++ = base64_encode_value(result);
			result  = (fragment & 0x03f) >> 0;
			*codechar++ = base64_encode_value(result);
			
			++(state_in->stepcount);
			if (state_in->stepcount == CHARS_PER_LINE/4)
			{
				*codechar++ = '\n';
				state_in->stepcount = 0;
			}
		}
	}
	/* control should not reach here */
	return codechar - code_out;
}

int base64_encode_blockend(char* code_out, base64_encodestate* state_in)
{
	char* codechar = code_out;
	
	switch (state_in->step)
	{
	case step_B:
		*codechar++ = base64_encode_value(state_in->result);
		*codechar++ = '=';
		*codechar++ = '=';
		break;
	case step_C:
		*codechar++ = base64_encode_value(state_in->result);
		*codechar++ = '=';
		break;
	case step_A:
		break;
	}
	*codechar++ = '\n';
	
	return codechar - code_out;
}

///////////////////////////decode
int base64_decode_value(char value_in)
{
	static const char decoding[] = {62,-1,-1,-1,63,52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};
	static const char decoding_size = sizeof(decoding);
	value_in -= 43;
	if (value_in < 0 || value_in > decoding_size) return -1;
	return decoding[(int)value_in];
}

void base64_init_decodestate(base64_decodestate* state_in)
{
	state_in->step = step_a;
	state_in->plainchar = 0;
}

int base64_decode_block(const char* code_in, const int length_in, char* plaintext_out, base64_decodestate* state_in)
{
	const char* codechar = code_in;
	char* plainchar = plaintext_out;
	char fragment;
	
	*plainchar = state_in->plainchar;
	
	switch (state_in->step)
	{
		while (1)
		{
	case step_a:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_a;
					state_in->plainchar = *plainchar;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar    = (fragment & 0x03f) << 2;
	case step_b:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_b;
					state_in->plainchar = *plainchar;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar++ |= (fragment & 0x030) >> 4;
			*plainchar    = (fragment & 0x00f) << 4;
	case step_c:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_c;
					state_in->plainchar = *plainchar;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar++ |= (fragment & 0x03c) >> 2;
			*plainchar    = (fragment & 0x003) << 6;
	case step_d:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_d;
					state_in->plainchar = *plainchar;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar++   |= (fragment & 0x03f);
		}
	}
	/* control should not reach here */
	return plainchar - plaintext_out;
}

/////////////////////////out

void encode(const char* infile, const char* outfile)
{
	FILE* in_fp = fopen(infile,"rb");
	if (in_fp == NULL)
	{
		printf("in_fp open err\n");
	}
	FILE* out_fp = fopen(outfile,"wb");
	if (out_fp == NULL)
	{
		printf("in_out open err\n");
	}
	
	base64_encodestate _state;
	base64_init_encodestate(&_state);
	const int N = BUFFERSIZE;

	char* plaintext = (char*)malloc(N);
	char* code = (char*)malloc(sizeof(char[2*N]));
	int plainlength;
	int codelength;

	do
	{
		plainlength = fread(plaintext,1,N,in_fp);
		codelength = base64_encode_block(plaintext, plainlength, code, &_state);
		fwrite(code,1,codelength,out_fp);
	}
	while (plainlength > 0);

	codelength = base64_encode_blockend(code, &_state);
	fwrite(code,1,codelength,out_fp);
	base64_init_encodestate(&_state);

	free(plaintext);
	free(code);

	fflush(out_fp);
	fclose(in_fp);
	fclose(out_fp);
}

int update(const char* infile, const char* outfile)
{
	FILE* in_fp = fopen(infile,"rb");
	if (in_fp == NULL)
	{
		printf("in_fp open err\n");
		return 0;
	}
	FILE* out_fp = fopen(outfile,"wb");
	if (out_fp == NULL)
	{
		printf("in_out open err\n");
		return 0;
	}

	base64_decodestate _state;
	base64_init_decodestate(&_state);
	const int N = BUFFERSIZE;

	int codelength;
	int plainlength;
	int i=0;
	char line[128] = {0};
	char ver[128] = {0};
	char update_date[128] = {0};
	char date_len[128] = {0};
	int position = 0;
	char* code = (char*)malloc(N);
	if (code == NULL)
		return 0;
	char* plaintext = (char*)malloc(sizeof(char[N]));
	if (plaintext == NULL)
		return 0;
	
	for(i=0; i<3; ++i)
	{
		fgets(line,128,in_fp);
		if (i==0)
		{
			if(strncmp(line,"VERSION=",strlen("VERSION="))==0)
			{
				
				memcpy(ver,line+strlen("VERSION="),sizeof(ver));
			}
			else
			{
				printf("version err\n");
				return 0;
			}
		}
		else if(i==1)
		{
			if(strncmp(line,"DATE=",strlen("DATE="))==0)
			{
				memcpy(update_date,line+strlen("DATE="),sizeof(update_date));
			}
			else
			{
				printf("update_date err\n");
				return 0;
			}
		}
		else if(i==2)
		{
			if (strncmp(line,"DATA_LEN=",strlen("DATA_LEN="))==0)
			{
				memcpy(date_len,line+strlen("DATA_LEN="),sizeof(date_len));
			}
			else
			{
				printf("data_len err\n");
				return 0;
			}
		}
	}
	
	do
	{
		codelength = fread(code,1,N,in_fp);
		position+=codelength;
		plainlength = base64_decode_block(code, codelength, plaintext, &_state); 
		fwrite(plaintext,1,plainlength,out_fp);
	}
	while (codelength > 0);

	if (position != atoi(date_len))
	{
		printf("pos = %d data_len = %d err!\n",position,atoi(date_len));
		return 0;		
	}
	base64_init_decodestate(&_state);

	free(plaintext);
	free(code);

	fflush(out_fp);
	fclose(in_fp);
	fclose(out_fp);
	return 1;
}






















