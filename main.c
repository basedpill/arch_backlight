#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <cstring>
#include <cstdint>

#define FILE_BRIGHTNESS "/sys/backlight/*/brightness"
#define FILE_MAX_BRIGHTNESS "/sys/class/backlight/"


void usage(char *name){
	printf(
		"Usage: hopefully gui implementation"
		"\n"
		"- read max brightness"
		"- make brightness specified out of max brightness"
		"- if max is 15 do: input/100*15"
	);
}

char* max_brightness(char *PATH){
    FILE *fp;
    char temp[1024];
    char* max_b = NULL;
	fp = fopen(PATH, "r");
	if(fp != NULL){
		if (fgets(temp, sizeof(temp), fp) != NULL){
			max_b = (char*)malloc(strlen(temp) + 1);
			if (max_b != NULL){
				strcpy(max_b, temp);
			}
		}
		fclose(fp);
	} else{
		printf("%s NOT FOUND!", FILE_MAX_BRIGHTNESS);
	}	
	return max_b;
}

int set_brightness(char *PATH, int number){
	FILE *fp;
	fp = fopen(PATH, "w");
	if (fp == NULL){
		perror("No backlight device found");
		return 1;
	}
	else{
		fprintf(fp, "%d",number);
		fclose(fp);
		return 0;		
	}
}



int main(int argc, char **argv){
	if(argc <= 1){
		usage(argv[0]);
		exit ( EXIT_FAILURE );
	}

	struct dirent *entry;
	DIR *dir = opendir(FILE_MAX_BRIGHTNESS);
	if (dir == NULL) {
        	printf("Error: Could not open directory %s\n", FILE_MAX_BRIGHTNESS);
        	return 1;
	}
	entry = readdir(dir);
	if (entry == NULL) {
        	printf("Error: No entries in directory %s\n", FILE_MAX_BRIGHTNESS);
        	closedir(dir);
        	return 1;
	}
	while(entry) {
		if(strcmp(entry->d_name,".") != 0 && strcmp(entry->d_name,"..") != 0){
			break;
		}
 		entry = readdir(dir);
	}
	if (entry == NULL) {
    		fprintf(stderr, "Error: No valid backlight device found in %s\n", FILE_MAX_BRIGHTNESS);
    		closedir(dir);
    		return 1;
	}
	char backlight_device[1024];
	char brightness_path[1024];
	snprintf(brightness_path, sizeof(brightness_path), "%s%s/brightness", FILE_MAX_BRIGHTNESS, entry->d_name);
	snprintf(backlight_device, sizeof(backlight_device), "%s%s/max_brightness", FILE_MAX_BRIGHTNESS, entry->d_name);
    	closedir(dir);	

	char *mb = max_brightness(backlight_device);
	if (mb) {
		printf("Max Brightness: %s", mb);
	}
	
	if (strcmp(argv[1],"-s")== 0){
		int number = atoi(argv[2]);
		printf("\nBrightness set to: %d\n", number);
		int fin1 = atoi(mb);
                int fin2 = number*fin1;
		int fin3 = fin2/100;
		printf("Max brightness reads: %d\n", fin1);
		set_brightness(brightness_path,fin3);
		free(mb);
		return EXIT_SUCCESS;
	}
}


