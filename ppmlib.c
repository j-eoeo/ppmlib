/* $Id: ppmlib.c 16 2024-01-08 03:29:26Z nishi $ */
/* Simple PPM (netpbm) library. */

#include "ppmlib.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void ppmlib_parse(unsigned const char* _data, size_t size, int* width, int* height, int* type, unsigned char** buffer, void(*read_data)(int, int, int, int, int)){
	if(buffer != NULL){
		*buffer = NULL;
	}
	unsigned char* data = malloc(size);
	memcpy(data, _data, size);
	if(size > 2){
		if(data[0] == 'P' && (data[1] >= '1' && data[1] <= '6')){
			int incr = 0;
			*type = data[1] - '1' + 1;
			int i;
			int n = data[2] == '\n' ? 0 : 2;
			int cn = 0;
			int s = 2;
			int max = ((*type) - 1) % 3 == 0 ? 2 : 3;
			*width = -1;
			int max_n;
			*height = -1;
			int start_data = -1;
			int data_count = 0;
			for(i = 2; i < size; i++){
				unsigned char c = data[i];
				if(c == '#' && n == 0){
					for(; i < size && data[i] != '\n'; i++);
					n = 0;
					s = i + 1;
					cn = 0;
					continue;
				}else if((c == '\n' || c == ' ') && max > 0){
					if(c == '\n'){
						n = 0;
					}
					int j;
					bool found = false;
					for(j = s; j < s + cn; j++){
						if(data[j] >= '0' && data[j] <= '9'){
							found = true;
							break;
						}
					}
					if(found){
						char* num = malloc(cn + 1);
						num[cn] = 0;
						memcpy(num, data + s, cn);
						if(*width == -1){
							*width = atoi(num);
						}else if(*height == -1){
							*height = atoi(num);
							if(buffer != NULL){
								*buffer = malloc((*width * 3) * (*height));
							}
						}else{
							max_n = atoi(num);
						}
						max--;
						if(max == 0){
							start_data = i + 1;
							free(num);
							break;
						}
						free(num);
					}
					cn = 0;
					s = i + 1;
					for(; s < size && (data[s] == '\n' || data[s] == ' '); s++);
					continue;
				}
				cn++;
				n++;
			}
			if(start_data != -1){
				int wincr = 0;
				int eincr = 0;
				char* str_num = malloc(1);
				str_num[0] = 0;
				int str_len = 0;
				bool used = false;
				int colors[3];
				colors[0] = -1;
				colors[1] = -1;
				colors[2] = -1;
				for(i = start_data; i < size + 1 && eincr < (*width) * (*height) * ((*type == 3) || (*type == 6) ? 3 : 1); i++){
					unsigned char c = data[i];
					if(*type == 4 && i != size){
						int ch = !(data[i] & 0b10000000) ? 255 : 0;
						if(buffer != NULL){
							(*buffer)[eincr * 3 + 0] = ch;
							(*buffer)[eincr * 3 + 1] = ch;
							(*buffer)[eincr * 3 + 2] = ch;
						}
						if(read_data != NULL){
							read_data(eincr % (*width), eincr / (*width), ch, ch, ch);
						}
						eincr++;
						data[i] = data[i] << 1;
						incr++;
						wincr++;
						if(wincr == *width){
							wincr = 0;
							incr = 0;
						}else if(incr < 8){
							i--;
						}else{
							incr = 0;
						}
					}else if(*type == 1){
						if(!(c == ' ' || c == '\n')){
							char* b = malloc(2);
							b[0] = c;
							b[1] = 0;
							int ch = atoi(b);
							ch = (1 - ch) * 255;
							if(buffer != NULL){
								(*buffer)[eincr * 3 + 0] = ch;
								(*buffer)[eincr * 3 + 1] = ch;
								(*buffer)[eincr * 3 + 2] = ch;
							}
							if(read_data != NULL){
								read_data(eincr % (*width), eincr / (*width), ch, ch, ch);
							}
							eincr++;
						}
					}else if(*type == 5){
						int ch;
						if(max_n > 255){
							ch = (double)((data[i] << 8) | data[i + 1]) / max_n * 255;
							i++;
						}else{
							ch = (double)data[i] / max_n * 255;
						}
						if(buffer != NULL){
							(*buffer)[eincr * 3 + 0] = ch;
							(*buffer)[eincr * 3 + 1] = ch;
							(*buffer)[eincr * 3 + 2] = ch;
						}
						if(read_data != NULL){
							read_data(eincr % (*width), eincr / (*width), ch, ch, ch);
						}
						eincr++;
					}else if(*type == 2){
						if(c == ' ' || c == '\n'){
							if(used){
								int ch = (double)atoi(str_num) / max_n * 255;
								if(buffer != NULL){
									(*buffer)[eincr * 3 + 0] = ch;
									(*buffer)[eincr * 3 + 1] = ch;
									(*buffer)[eincr * 3 + 2] = ch;
								}
								if(read_data != NULL){
									read_data(eincr % (*width), eincr / (*width), ch, ch, ch);
								}
								eincr++;
							}
							free(str_num);
							str_num = malloc(1);
							str_num[0] = 0;
							str_len = 0;
							used = false;
						}else{
							used = true;
							char* tmp = malloc(str_len + 1);
							tmp[str_len] = c;
							tmp[str_len + 1] = 0;
							memcpy(tmp, str_num, str_len);
							str_len++;
							free(str_num);
							str_num = tmp;
						}
					}else if(*type == 6){
						int ch;
						if(max_n > 255){
							ch = (double)((data[i] << 8) | data[i + 1]) / max_n * 255;
							i++;
						}else{
							ch = (double)data[i] / max_n * 255;
						}
						if(buffer != NULL){
							(*buffer)[eincr] = ch;
						}
						if(read_data != NULL){
							if(colors[0] == -1){
								colors[0] = ch;
							}else if(colors[1] == -1){
								colors[1] = ch;
							}else if(colors[2] == -1){
								colors[2] = ch;
								read_data((eincr / 3) % (*width), (eincr / 3) / (*width), colors[0], colors[1], colors[2]);
								colors[0] = colors[1] = colors[2] = -1;
							}
						}
						eincr++;
					}else if(*type == 3){
						if(c == ' ' || c == '\n'){
							if(used){
								double ch = (double)atoi(str_num) / max_n * 255;
								if(buffer != NULL){
									(*buffer)[eincr] = ch;
								}
								if(read_data != NULL){
									if(colors[0] == -1){
										colors[0] = ch;
									}else if(colors[1] == -1){
										colors[1] = ch;
									}else if(colors[2] == -1){
										colors[2] = ch;
										read_data((eincr / 3) % (*width), (eincr / 3) / (*width), colors[0], colors[1], colors[2]);
										colors[0] = colors[1] = colors[2] = -1;
									}
								}
								eincr++;
							}
							free(str_num);
							str_num = malloc(1);
							str_num[0] = 0;
							str_len = 0;
							used = false;
						}else{
							used = true;
							char* tmp = malloc(str_len + 1);
							tmp[str_len] = c;
							tmp[str_len + 1] = 0;
							memcpy(tmp, str_num, str_len);
							str_len++;
							free(str_num);
							str_num = tmp;
						}
					}
				}
				free(str_num);
			}
		}
	}else{
		return;
	}
}
