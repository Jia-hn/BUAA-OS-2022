/*
 * operations on IDE disk.
 */

#include "fs.h"
#include "lib.h"
#include <mmu.h>

 int time_read() {
 	int tmp;
	tmp = 1;
	syscall_write_dev((int)&tmp, 0x15000000, 4);
	syscall_read_dev((int)&tmp, 0x15000010, 4);
	return tmp;
 }
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
void raid0_write(u_int secno, void *src, u_int nsecs) {
	u_int i;
	for (i = secno; i < secno + nsecs; i++) {
		if (i % 2 == 0) {
			ide_write(1, i / 2, src + (i - secno) * 0x200, 1);
		} else {
			ide_write(2, (i - 1)/2, src + (i - secno) * 0x200, 1);
		}
	}
}

void raid0_read(u_int secno, void *dst, u_int nsecs) {
	u_int i;
	for (i = secno; i < secno + nsecs; i++) {
        if (i % 2 == 0) {
            ide_read(1, i / 2, dst + (i - secno) * 0x200, 1); 
        } else {
            ide_read(2, (i - 1)/2, dst + (i - secno) * 0x200, 1); 
        }   
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
