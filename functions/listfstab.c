/*      Copyright (C) 2022 Vincent Stafford

        This program is free software: you can redistribute it and/or modify
        it under the terms of the GNU Affero General Public License as
        published by the Free Software Foundation, either version 3 of the
        License, or (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU Affero General Public License for more details.

        You should have received a copy of the GNU Affero General Public License
        along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "../include/listfstab.h"
#include "../include/misc.h"

const char disk_dir[19] = "/dev/disk/by-uuid/";

FILE *open_fstab() {
  FILE *fstab = fopen("/etc/fstab", "r");
  if (!fstab) {
    printf("/etc/fstab is not readable, or does not exist!\n");
    return NULL;
  }
  return fstab;
}

void close_file(FILE *fp) {
  fclose(fp);
}

void list_fstab_files() {
    FILE *fstab = open_fstab();
    if (fstab == NULL) {
        exit(EXIT_FAILURE);
    }
    char buffer[LINE_MAX];
    char uuid[LINE_MAX];
    int ent_num = 1;
    int space_pos;

    printf("Entries within /etc/fstab\n");
    while (fgets(buffer, LINE_MAX, fstab)) {
            if(buffer[0] != '#' && buffer[0] != ' ' && buffer[0] != '\0' && buffer[0] != '\n') {
                printf("%i. %s", ent_num, buffer); /*Removes Junk Lines from file*/
                ent_num+=1;
                if(buffer[0] == 'U') { //Detects if an entry uses a UUID
                    for (long unsigned int i = 0; i <= arr_len(buffer); i++) {
                        uuid[i] = buffer[i]; //Copying buffer to uuid var so we can manipulate it
                    }
                    memmove(&uuid[0], &uuid[5], LINE_MAX+1); //Detects the end of the UUID
                    for (int i = 0; i < LINE_MAX; i+=1) {
                        if (uuid[i] == ' ') {
                            space_pos = i;
                            break;
                        } else { //Triggers if there is no space after the UUID.
                            space_pos = -1;
                        }
                    }
                    if (space_pos == -1) { /*Error handling*/
                        printf("Error reading FSTAB entries.");
                        exit(EXIT_FAILURE);
                    }
                    for(int i = space_pos; i <= LINE_MAX; i++) { // Loop to set everything after entry to null
                        uuid[i] = '\0';
                    }
                    char line_buf[LINE_MAX + 19];
                    char drive_name[32];
                    strcpy(line_buf, disk_dir); //copy set drive directory to buf
                    strcat(line_buf, uuid); //Add UUID
                    // readlink(line_buf, drive_name, arr_len(drive_name)); // Return us the drive name
                    if (readlink(line_buf, drive_name, arr_len(drive_name)) == -1) {
                        exit(EXIT_FAILURE);
                    }
                    memmove(&drive_name[0], &drive_name[6], 32+1); //Cropping the ../../ off
                    printf("|--> Drive: %s\n", drive_name); //Output
                    memset(line_buf, 0, arr_len(line_buf)); //Force line_buf[] to be empty so output is correct.
                }
            }
        }
    close_file(fstab);
    return;
}

