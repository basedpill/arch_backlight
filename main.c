#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdint.h>

#define BACKLIGHT_PATH "/sys/class/backlight/"


void usage(){
	printf(
		"\nUsage: Use -s and number between 1 & 100 for brightness"
		"\n"
		"- read max brightness\n"
		"- changes brightness file\n\n"
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
		printf("%s NOT FOUND!", BACKLIGHT_PATH);
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
	if(strcmp(argv[1],"-h") == 0){
		usage();
		exit ( EXIT_FAILURE );
	}

	struct dirent *entry;
	DIR *dir = opendir(BACKLIGHT_PATH);
	if (dir == NULL) {
        	printf("Error: Could not open directory %s\n", BACKLIGHT_PATH);
        	return 1;
	}
	entry = readdir(dir);
	if (entry == NULL) {
        	printf("Error: No entries in directory %s\n", BACKLIGHT_PATH);
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
    		fprintf(stderr, "Error: No valid backlight device found in %s\n", BACKLIGHT_PATH);
    		closedir(dir);
    		return 1;
	}
	char backlight_device[1024];
	char brightness_path[1024];
	snprintf(brightness_path, sizeof(brightness_path), "%s%s/brightness", BACKLIGHT_PATH, entry->d_name);
	snprintf(backlight_device, sizeof(backlight_device), "%s%s/max_brightness", BACKLIGHT_PATH, entry->d_name);
    	closedir(dir);	

	char *mb = max_brightness(backlight_device);
	if (mb) {
		printf("Max Brightness: %s", mb);
	}
	
	if (strcmp(argv[1],"-s")== 0){
		int number = atoi(argv[2]);
		printf("\nBrightness set to: %d\n", number);
                int scale = (number*(atoi(mb))/100);
		set_brightness(brightness_path,scale);
		free(mb);
		return EXIT_SUCCESS;
	}
}


