#include "os.h"
void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn){
    int i;
    int entrynumber;
    uint64_t *tablePointer = phys_to_virt(pt<<12);              //given page number add offset to create address
    for(i=0;i<4;i++){
        uint64_t mask=512-1;                                    //9-bits for 512 entries
        int maskoffset = ((57)-9*(i+1));                        // ignore sign-extend
        mask = mask << maskoffset;
        entrynumber=(vpn & mask)>>maskoffset;
        if(!isvalid(tablePointer[entrynumber])){                //faced invalid entry
            if(ppn ==NO_MAPPING)                                // invalid entry + needed to be destroyed = done
                return;

            tablePointer[entrynumber]=((alloc_page_frame()<<12)|1); //if was invalid and we need to create one - create, set bits 1-11 to 0 and valid bit to 1
        }
        tablePointer=phys_to_virt(tablePointer[entrynumber&((~0)<<12)]); //entry without 12 first bits
    }
    entrynumber=(512-1)<<12;
    if(ppn!=NO_MAPPING){
        tablePointer[entrynumber] = (ppn<<12) | 1;
    }
    else{
        tablePointer[entrynumber] = (tablePointer[entrynumber]>>1)<<1;//turn of last bit
    }

}


uint64_t page_table_query(uint64_t pt, uint64_t vpn){
    int i;
    int entrynumber;
    uint64_t *tablePointer = phys_to_virt(pt<<12);              //given page number add offset to create address
    uint64_t res;
    for(i=0;i<5;i++){
        uint64_t mask=512-1;                                    //9-bits for 512 entries
        int maskoffset = ((57)-9*(i+1));                        // ignore sign-extend
        mask = mask << maskoffset;
        entrynumber=(vpn & mask)>>maskoffset;
        if(!isvalid(tablePointer[entrynumber])){                //faced invalid entry
           return NO_MAPPING;
        }
        if(i<4)
            tablePointer=phys_to_virt(tablePointer[entrynumber&((~0)<<12)]); //entry without 12 first bits
        else{
            res= tablePointer[entrynumber]>>12;
        }
    }
    return res;

}
int isvalid(uint64_t entry){
    if((1&entry)>0)
        return 1;
    return 0;
}