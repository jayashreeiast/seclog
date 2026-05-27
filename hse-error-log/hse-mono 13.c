#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "libhse.h"
#include "hse_interface.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#define BL_MONO_CNT_TBL_OFFSET   0x121000U   // Offset in NOR Flash
#define FLASH_DEVICE   "/dev/mtd0"      //  MTD partition
#define BL_MONOCNTTBL_PRESENCE_IDX   15U
#define BL_MONOCNTTBL_VALID_VAL      0x004D4354U   // ASCII "MCT"


static uint8_t monoCntTblBuffer[HSE_NVM_CONTAINER_CNT_TBL_SIZE];


#define COUNTER_SIZE         4u
#define ASCII_F80           0x463830u
#define VERSION_ADDR        0x6FFF00

// Alignment helper (round x up to nearest multiple of a)
#define ALIGN_UP(x, a)   (((x) + ((a) - 1)) & ~((a) - 1))
#define ENG_SW_VERSION 




/* Read monotonic counter */
uint32_t read_counter(uint32_t index){
    void *counterValue = NULL;
    hseSrvDescriptor_t srv_desc;
    uint64_t counterResult = 0;

    // Allocate DMA-capable memory
    counterValue = hse_mem_alloc(COUNTER_SIZE);
    if (!counterValue) {
        printf("Failed to allocate DMA memory for counter\n");
        return -ENOMEM;
    }

    // Zero the buffer
    memset(counterValue, 0, COUNTER_SIZE);

    // Prepare HSE service descriptor
    memset(&srv_desc, 0, sizeof(srv_desc));
    srv_desc.srvId = HSE_SRV_ID_READ_COUNTER;
    srv_desc.hseSrv.readCounterReq.counterIndex = index;
    srv_desc.hseSrv.readCounterReq.pCounterVal = hse_virt_to_dma(counterValue);

    // Send the request
    int ret = hse_srv_req_sync(HSE_CHANNEL_ANY, &srv_desc, sizeof(srv_desc));
    if (ret != 0) {
        printf("READ_COUNTER request failed: error code = %d\n", ret);
        hse_mem_free(counterValue);
        return ret;
    }

    // Copy result from DMA buffer
    memcpy(&counterResult, counterValue, COUNTER_SIZE);
    hse_mem_free(counterValue);

    return (uint32_t)counterResult;
}

/* Increment Counter */
int increment_counter(uint32_t index, uint32_t value)
{
    hseSrvDescriptor_t srv_desc;

    memset(&srv_desc, 0, sizeof(srv_desc));
    srv_desc.srvId = HSE_SRV_ID_INCREMENT_COUNTER;
    srv_desc.hseSrv.incCounterReq.counterIndex = index;
    srv_desc.hseSrv.incCounterReq.value = value;

    int ret = hse_srv_req_sync(HSE_CHANNEL_ANY, &srv_desc, sizeof(srv_desc));
    return ret;
}

/* Read version value from memory */
uint32_t read_version_increment_value() {
    FILE *fp = fopen("/dev/mmcblk0", "rb");
    if (!fp) {
        perror("Failed to open /dev/mmcblk0");
        return ASCII_F80;
    }

    if (fseek(fp, VERSION_ADDR, SEEK_SET) != 0) {
        perror("fseek failed");
        fclose(fp);
        return ASCII_F80;
    }

    uint8_t version_bytes[3];
    if (fread(version_bytes, 1, 3, fp) != 3) {
        perror("fread failed");
        fclose(fp);
        return ASCII_F80;
    }

    fclose(fp);

    uint32_t val = ((uint32_t)version_bytes[0] << 16) |
                   ((uint32_t)version_bytes[1] << 8) |
                   (uint32_t)version_bytes[2];

    return val;
}

/* Convert 3-byte ASCII uint32_t to string */
void counter_value_to_str(uint32_t value, char *out_str) {
    out_str[0] = (value >> 16) & 0xFF;
    out_str[1] = (value >> 8) & 0xFF;
    out_str[2] = value & 0xFF;
    out_str[3] = '\0';
}

//Publish Monotonic Counter Table
int publish_monotonic_counter_table(void)
{
    hseSrvDescriptor_t srv_desc;
    int ret;

    // Allocate DMA-capable buffer for the table
    void *mct_buf=NULL;
    mct_buf= hse_mem_alloc(HSE_NVM_CONTAINER_CNT_TBL_SIZE);
    if (!mct_buf) {
        printf("Failed to allocate DMA memory for monotonic counter table\n");
        return -ENOMEM;
    }

    memset(mct_buf, 0, HSE_NVM_CONTAINER_CNT_TBL_SIZE);

    // Clear descriptor
    memset(&srv_desc, 0, sizeof(srv_desc));

    // Fill descriptor
    srv_desc.srvId = HSE_SRV_ID_PUBLISH_LOAD_CNT_TBL;
    srv_desc.hseSrv.publishLoadCntTblReq.action = HSE_NVM_CNT_TBL_ACTION_PUBLISH;
    srv_desc.hseSrv.publishLoadCntTblReq.pNvmContainerCntTbl = hse_virt_to_dma(mct_buf);

    // Send request synchronously
    ret = hse_srv_req_sync(HSE_CHANNEL_ANY, &srv_desc, sizeof(srv_desc));
    if (ret != 0) {
        printf("PUBLISH_LOAD_CNT_TBL request failed: %d\n", ret);
        hse_mem_free(mct_buf);
        return ret;
    }

    printf("Monotonic Counter Table successfully published.\n");
    memcpy(monoCntTblBuffer, mct_buf, HSE_NVM_CONTAINER_CNT_TBL_SIZE);
    hse_mem_free(mct_buf);
    return 0;
}


int write_mct_to_flash(const uint8_t *buffer, size_t size)
{
    int fd = open(FLASH_DEVICE, O_RDWR);
    if (fd < 0) {
        perror("open flash");
        return -1;
    }
 
    // --- Query flash info ---
    struct mtd_info_user meminfo;
    if (ioctl(fd, MEMGETINFO, &meminfo) < 0) {
        perror("MEMGETINFO");
        close(fd);
        return -1;
    }
 
    printf("Flash erase block size = 0x%X, total flash size = 0x%X\n",
           meminfo.erasesize, meminfo.size);
 
    // --- Align erase length ---
    size_t erase_len = ALIGN_UP(size, meminfo.erasesize);
 
    // ---  Erase ---
    struct erase_info_user erase;
    erase.start  = BL_MONO_CNT_TBL_OFFSET;
    erase.length = erase_len;
 
    if (ioctl(fd, MEMERASE, &erase) < 0) {
        perror("MEMERASE");
        close(fd);
        return -1;
    }
    printf("Erased region at offset 0x%X, length 0x%zX\n",
           erase.start, erase.length);
 
    // --- Write ---
    if (lseek(fd, BL_MONO_CNT_TBL_OFFSET, SEEK_SET) < 0) {
        perror("lseek");
        close(fd);
        return -1;
    }
 
    ssize_t written = write(fd, buffer, size);
    if (written != (ssize_t)size) {
        perror("write");
        close(fd);
        return -1;
    }
 
    printf("Wrote %zd bytes of MCT data at offset 0x%X\n", written, BL_MONO_CNT_TBL_OFFSET);
 
    close(fd);
    return 0;
}



int load_monotonic_counter_table(void)

{
    int fd = -1;
    void *mct_hse_buf = NULL;   // DMA-capable buffer for HSE
    void *tmp_buf = NULL;       // normal user buffer for flash read
    ssize_t ret;
    int hse_ret = 0;

    hseSrvDescriptor_t srv_desc;
    fd = open(FLASH_DEVICE, O_RDONLY);
    if (fd < 0) {
        perror("open flash");
        return -1;
    }
    
    // allocate a plain user-space buffer to read from flash 
    tmp_buf = malloc(HSE_NVM_CONTAINER_CNT_TBL_SIZE);
    if (!tmp_buf) {
        perror("malloc tmp_buf");
        close(fd);
        return -ENOMEM;
    }
    
    memset(tmp_buf, 0, HSE_NVM_CONTAINER_CNT_TBL_SIZE);
 
    // read the blob from NOR into the plain buffer 
    if (lseek(fd, BL_MONO_CNT_TBL_OFFSET, SEEK_SET) < 0) {
        perror("lseek");
        free(tmp_buf);
        close(fd);
        return -1;
    }
    
    ret = read(fd, tmp_buf, HSE_NVM_CONTAINER_CNT_TBL_SIZE);
    if (ret < 0) {
        perror("read");
        free(tmp_buf);
        close(fd);
        return -1;
    }

    if ((size_t)ret < HSE_NVM_CONTAINER_CNT_TBL_SIZE) {
        // partial read ok to handle; zero the rest 
        memset((uint8_t*)tmp_buf + ret, 0, HSE_NVM_CONTAINER_CNT_TBL_SIZE - ret);
    }
 
    close(fd); fd = -1;
 
    // allocate DMA buffer for HSE and copy the data there 
    mct_hse_buf = hse_mem_alloc(HSE_NVM_CONTAINER_CNT_TBL_SIZE);
    if (!mct_hse_buf) {
        fprintf(stderr, "hse_mem_alloc failed\n");
        free(tmp_buf);
        return -ENOMEM;
    }
    

    // Copy normal user buffer into HSE DMA buffer 
    memcpy(mct_hse_buf, tmp_buf, HSE_NVM_CONTAINER_CNT_TBL_SIZE);
    
    // prepare HSE LOAD descriptor 
    memset(&srv_desc, 0, sizeof(srv_desc));
    srv_desc.srvId = HSE_SRV_ID_PUBLISH_LOAD_CNT_TBL;
    srv_desc.hseSrv.publishLoadCntTblReq.action = HSE_NVM_CNT_TBL_ACTION_LOAD;
    srv_desc.hseSrv.publishLoadCntTblReq.pNvmContainerCntTbl = hse_virt_to_dma(mct_hse_buf);
 
    // send to HSE 
    hse_ret = hse_srv_req_sync(HSE_CHANNEL_ANY, &srv_desc, sizeof(srv_desc));

    if (hse_ret != 0) {
        fprintf(stderr, "LOAD_CNT_TBL request failed: %d\n", hse_ret);
        hse_mem_free(mct_hse_buf);
        free(tmp_buf);
        return hse_ret;
    }
 
    printf("Monotonic Counter Table successfully loaded into HSE.\n");
    
    // done — free buffers 
    hse_mem_free(mct_hse_buf);
    free(tmp_buf);
    return 0;

}


 

/* Check presence of Monotonic Counter Table */
bool is_mct_present(void)
{
    uint32_t cnt_val = read_counter(BL_MONOCNTTBL_PRESENCE_IDX);
    // Mask lower 
    cnt_val &= 0x00FFFFFF;
    if (cnt_val == (BL_MONOCNTTBL_VALID_VAL & 0x00FFFFFF)) {
        printf("MCT is present at (counter[%u] = 0x%06X)\n",
               BL_MONOCNTTBL_PRESENCE_IDX, cnt_val);
        return true;
    }
    printf("MCT not present at (counter[%u] = 0x%06X)\n",
           BL_MONOCNTTBL_PRESENCE_IDX, cnt_val);
    return false;
}

 

int main(void)
{
    DECLARE_SET_ZERO(hseSrvDescriptor_t, srv_desc);
    uint16_t status;
    int err;
    uint32_t BL2_MonoCntIdx = 2u;
    uint32_t FIP_MonoCntIdx = 6u;
    char bl2_version_str[4];
    char fip_version_str[4];
    char inc_val_str[4];
    uint32_t val2,val6;
    uint32_t increment_val;
    
    
    err = hse_dev_open();
    if (err)
        return err;

    status = hse_check_status();
    if (!(status & HSE_STATUS_INSTALL_OK)) {
        err = ENODEV;
        goto out_dev_close;
    }

    if (status & (1U << 11)) {
        printf("Super user rights granted\n");
    } else {
        err = ENODEV;
        goto out_dev_close;
    }



  #ifdef ENG_SW_VERSION
    // Read BL2 version
   
     val2 = read_counter(BL2_MonoCntIdx);
    
    counter_value_to_str(val2, bl2_version_str);
    printf("BL2 version from Monotonic Counter[%u] Value: %s\n", BL2_MonoCntIdx, bl2_version_str);

    if (val2 == ASCII_F80) {
        // Read FIP version
         val6 = read_counter(FIP_MonoCntIdx);
        
        counter_value_to_str(val6, fip_version_str);

        if (val6 == val2) {
            printf("FIP version from Monotonic Counter[%u] Value: %s\n", FIP_MonoCntIdx, fip_version_str);
            return val6;
        }

        increment_val = read_version_increment_value();

        if ((increment_val == ASCII_F80) && (val6 == 0u)) 
        {
            
            err = increment_counter(FIP_MonoCntIdx, increment_val);
            if (!err) {
                printf("Increment function called successfully\n");
                val6 = read_counter(FIP_MonoCntIdx);
                counter_value_to_str(val6, fip_version_str); 
                
            }
          if (val6 == val2) {
               printf("FIP version from Monotonic Counter[%u] Value: %s\n", FIP_MonoCntIdx, fip_version_str);         
             return val6;
            }      
        }

        // None matched
        
        counter_value_to_str(increment_val, inc_val_str);
        printf("FIP version is not compatible with BL2, current FIP Version is %s\n", inc_val_str);
    
    } else {
        printf("BL2 is running with some other version: 0x%X\n", val2);
        return val2;
    }

  
  #else
   err = load_monotonic_counter_table();
                if (err) {
                    goto out_dev_close;
                }
				
    bool mct_exists=is_mct_present();
    if(mct_exists)
    { 			  
		 val2 = read_counter(BL2_MonoCntIdx); 
                counter_value_to_str(val2, bl2_version_str);
                printf("BL2 version from Monotonic Counter[%u] Value: %s\n", BL2_MonoCntIdx, bl2_version_str);	
		// Read FIP version
		 val6 = read_counter(FIP_MonoCntIdx);
               counter_value_to_str(val6, fip_version_str);
		if(val6 == 0U)
		{
		 increment_val = read_version_increment_value();
			err = increment_counter(FIP_MonoCntIdx, increment_val);
            if (!err) {
                printf("Increment function called successfully\n");
                val6 = read_counter(FIP_MonoCntIdx);
                counter_value_to_str(val6, fip_version_str);
				
			err = publish_monotonic_counter_table();
			if (err) {
				goto out_dev_close;
			}
			printf("published MCT\n");
	
			// Write to NOR Flash
			err = write_mct_to_flash(monoCntTblBuffer, HSE_NVM_CONTAINER_CNT_TBL_SIZE);
			if (err) {
			goto out_dev_close;
			}
			printf("MCT Publish + Flash Write complete.\n");
		      }
                 }
		else{
			printf("FIP version from Monotonic Counter[%u] Value: %s\n", FIP_MonoCntIdx, fip_version_str);  
			
		}
     
    }

    else
    {
    printf("MCT not present at index 15");
    }
  
#endif
out_dev_close:
    hse_dev_close();
    return err;
}


