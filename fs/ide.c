/*
 * operations on IDE disk.
 */

#include "fs.h"
#include "lib.h"
#include <mmu.h>

// Overview:
// 	read data from IDE disk. First issue a read request through
// 	disk register and then copy data from disk buffer
// 	(512 bytes, a sector) to destination array.
//
// Parameters:
//	diskno: disk number.
// 	secno: start sector number.
// 	dst: destination for data read from IDE disk.
// 	nsecs: the number of sectors to read.
//
// Post-Condition:
// 	If error occurrs during the read of the IDE disk, panic.
// 
// Hint: use syscalls to access device registers and buffers
/*** exercise 5.2 ***/
int raid4_valid(u_int diskno) {
	int tmp;
	tmp = diskno;
	if (syscall_write_dev((int)&tmp, 0x13000010, 4)) {
        user_panic("error in ide_read()");  
    }
	tmp = 0;
	if (syscall_write_dev((int)&tmp, 0x13000000, 4)) {
        user_panic("error in ide_read()");
    }
	tmp = 0;
	if (syscall_write_dev((int)&tmp, 0x13000020, 4)) {
        user_panic("error in ide_read()");
    }
	if (syscall_read_dev((int)&tmp, 0x13000030, 4)) {
        user_panic("error in ide_read()");
    }
	if (tmp == 0) {
        return 0;
    } else {
		return 1;
	}
}

int raid4_write(u_int blockno, void *src) {
	int i, j, k;
	int cnt = 0;
	for (i = 0; i < 2; i++) {
		for (j = 1; j <= 4; j++) {
			if (raid4_valid(j) == 0) {
				if (i == 0) {
					cnt++;
				}
				continue;
			}
			ide_write(j, 2 * blockno + i, src + (i * 4 + j - 1) * 0x200, 1);
		}
	}
	char ans[512];
	for (i = 0; i < 2; i++) {
		for (k = 0; k < 512; k++) {
			ans[k] = 0;
		}
		for (j = 1; j <= 4; j++) {
			char *tmp = src + (i * 4 + j - 1) * 0x200;
			for (k = 0; k < 512; k++) {
				ans[k] ^= tmp[k];
			}
		}
		if (raid4_valid(j) == 0) {
			if (i == 0) {
				cnt++;
			}
			continue;
		} else {
			ide_write(5, 2 * blockno + i, (void *)ans, 1);
		}
	}
	return cnt;
}

 int raid4_read(u_int blockno, void *dst) {
 	int i, j, k;
	int cnt = 0;
	for (i = 1; i <= 5; i++) {
		if (raid4_valid(i) == 0) {
			cnt ++;
		}
	}
	char ans[512];
	char *tmp;
	if (cnt == 0) {
		for (i = 0; i < 2; i++) {
			for (j = 1; j <= 4; j++) {
				ide_read(j, 2 * blockno + i, dst + (i * 4 + j - 1) * 0x200, 1);
			}
		}
		for (i = 0; i < 2; i++) {
			ide_read(5, 2 * blockno + i, (void *)ans, 1);
            for (j = 1; j <= 4; j++) {
				tmp = dst + (i * 4 + j - 1) * 0x200;
				for (k = 0; k < 512; k++) {
					ans[k] ^= tmp[k];
				}
			}
			for (k = 0; k < 512; k++) {
				if (ans[k] != 0) {
					return -1;
				}
			}
		}
		return 0;
	} else if (cnt == 1) {
		if (raid4_valid(5) == 0) {
			for (i = 0; i < 2; i++) {
            	for (j = 1; j <= 4; j++) {
                	ide_read(j, 2 * blockno + i, dst + (i * 4 + j - 1) * 0x200, 1);
            	}
        	}
		} else {
			int id;
			for (i = 0; i < 2; i++) {
                for (j = 1; j <= 4; j++) {
					if (raid4_valid(j) == 0) {
						id = j;
						continue;
					}
                    ide_read(j, 2 * blockno + i, dst + (i * 4 + j - 1) * 0x200, 1);
                }
            }
			for (i = 0; i < 2; i++) {
				ide_read(5, 2 * blockno + i, (void *)ans, 1);
				for (j = 1; j <= 4; j++) {
					if (raid4_valid(j) == 0) {
						continue;
					}
					tmp = dst + (i * 4 + j - 1) * 0x200;
                	for (k = 0; k < 512; k++) {
                    	ans[k] ^= tmp[k];
                	}
				}
				user_bcopy(ans, dst + (i * 4 + id - 1) * 0x200, 512);
			}
		}
		return 1;
	} else {
		return cnt;
	}
 }

void
ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs)
{
	// 0x200: the size of a sector: 512 bytes.
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int offset = 0;
	int tmp;

	while (offset_begin + offset < offset_end) {
		// Your code here
		// error occurred, then panic.
		tmp = diskno;
		if (syscall_write_dev((int)&tmp, 0x13000010, 4)) {
			user_panic("error in ide_read()");	
		}
		tmp = offset_begin + offset;
		if (syscall_write_dev((int)&tmp, 0x13000000, 4)) {
			user_panic("error in ide_read()");
		}
		tmp = 0;
		if (syscall_write_dev((int)&tmp, 0x13000020, 4)) {
			user_panic("error in ide_read()");
		}
		if (syscall_read_dev((int)&tmp, 0x13000030, 4)) {
			user_panic("error in ide_read()");
		}
		if (tmp == 0) {
			user_panic("error in ide_read()");
		}
		if (syscall_read_dev((int)(dst + offset), 0x13004000, 512)) {
			user_panic("error in ide_read()");
		}
		offset += 0x200;
	}
}


// Overview:
// 	write data to IDE disk.
//
// Parameters:
//	diskno: disk number.
//	secno: start sector number.
// 	src: the source data to write into IDE disk.
//	nsecs: the number of sectors to write.
//
// Post-Condition:
//	If error occurrs during the read of the IDE disk, panic.
//
// Hint: use syscalls to access device registers and buffers
/*** exercise 5.2 ***/
void
ide_write(u_int diskno, u_int secno, void *src, u_int nsecs)
{
	// Your code here
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int offset = 0;
	int tmp;

	// DO NOT DELETE WRITEF !!!
	writef("diskno: %d\n", diskno);

	while (offset_begin + offset < offset_end) {
		if (syscall_write_dev((int)(src + offset), 0x13004000, 512)) {
			user_panic("error in ide_write()");
		}
		tmp = diskno;
		if (syscall_write_dev((int)&tmp, 0x13000010, 4)) {
			user_panic("error in ide_write()");
		}
		tmp = offset_begin + offset;
		if (syscall_write_dev((int)&tmp, 0x13000000, 4)) {
			user_panic("error in ide_write()");
		}
		tmp = 1;
		if (syscall_write_dev((int)&tmp, 0x13000020, 4)) {
			user_panic("error in ide_write()");
		}
		if (syscall_read_dev((int)&tmp, 0x13000030, 4)) {
			user_panic("error in ide_write()");
		}
		if (tmp == 0) {
			user_panic("error in ide_write()");
		}
		offset += 0x200;
	}
}
