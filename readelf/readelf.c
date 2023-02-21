/* This is a simplefied ELF reader.
 * You can contact me if you find any bugs.
 *
 * Luming Wang<wlm199558@126.com>
 */

#include "kerelf.h"
#include <stdio.h>
#define BY2PG 0x1000
/* Overview:
 *   Check whether it is a ELF file.
 *
 * Pre-Condition:
 *   binary must longer than 4 byte.
 *
 * Post-Condition:
 *   Return 0 if `binary` isn't an elf. Otherwise
 * return 1.
 */
int is_elf_format(u_char *binary)
{
        Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;
        if (ehdr->e_ident[EI_MAG0] == ELFMAG0 &&
                ehdr->e_ident[EI_MAG1] == ELFMAG1 &&
                ehdr->e_ident[EI_MAG2] == ELFMAG2 &&
                ehdr->e_ident[EI_MAG3] == ELFMAG3) {
                return 1;
        }

        return 0;
}

/* Overview:
 *   read an elf format binary file. get ELF's information
 *
 * Pre-Condition:
 *   `binary` can't be NULL and `size` is the size of binary.
 *
 * Post-Condition:
 *   Return 0 if success. Otherwise return < 0.
 *   If success, output address of every section in ELF.
 */

/*
    Exercise 1.2. Please complete func "readelf". 
*/
int readelf(u_char *binary, int size)
{
        Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;

        int Nr;
		int Nr2;

        Elf32_Phdr *phdr = NULL;
		Elf32_Phdr *phdr2 = NULL;


        u_char *ptr_ph_table = NULL;
        Elf32_Half ph_entry_count;
        Elf32_Half ph_entry_size;


        // check whether `binary` is a ELF file.
        if (size < 4 || !is_elf_format(binary)) {
                printf("not a standard elf format\n");
                return 0;
        }

        // get section table addr, section header number and section header size.

        // for each section header, output section number and section addr. 
        // hint: section number starts at 0.

		ptr_ph_table = binary + ehdr -> e_phoff;
        ph_entry_count = ehdr -> e_phnum;
        ph_entry_size = ehdr -> e_phentsize;

        for (Nr = 0; Nr < ph_entry_count; Nr++) {
			int flag = 0;
			phdr = (Elf32_Phdr *)(ptr_ph_table + Nr * ph_entry_size);
			for(Nr2 = 0; Nr2 < ph_entry_count; Nr2++) {
				if(Nr == Nr2) continue;
				phdr2 = (Elf32_Phdr *)(ptr_ph_table + Nr2 * ph_entry_size);
				if (ROUNDDOWN(phdr -> p_vaddr,BY2PG) == ROUNDDOWN(phdr2 -> p_vaddr + phdr2 -> p_filesz,BY2PG)) {
					flag = 1;
					if(phdr2 -> p_vaddr + phdr2 -> p_filesz > phdr -> p_vaddr) {
						flag = 2;
					}
				}
				if (ROUNDDOWN(phdr -> p_vaddr + phdr -> p_filesz,BY2PG) == ROUNDDOWN(phdr2 -> p_vaddr,BY2PG)) {
                    flag = 3;
                    if(phdr -> p_vaddr + phdr -> p_filesz > phdr2 -> p_vaddr) {
                        flag = 4;
                    }
                }
			}
			if (flag == 0) {
            	printf("%d:0x%x,0x%x\n", Nr, phdr -> p_filesz, phdr -> p_memsz);	
			} else if (flag == 1) {
				printf("Overlay at page va : 0x%x\n", (unsigned int)ROUNDDOWN(phdr -> p_vaddr,BY2PG));
			} else if (flag == 2) {
				printf("Conflict at page va : 0x%x\n", (unsigned int)ROUNDDOWN(phdr -> p_vaddr,BY2PG));
			} else if (flag == 3) {
                printf("Overlay at page va : 0x%x\n", (unsigned int)ROUNDDOWN(phdr2 -> p_vaddr,BY2PG));
            } else if (flag == 4) {
                printf("Conflict at page va : 0x%x\n", (unsigned int)ROUNDDOWN(phdr2 -> p_vaddr,BY2PG));
            }
        }		

        return 0;
}

