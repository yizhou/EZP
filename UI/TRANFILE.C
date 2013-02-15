/*-------------------------------------------------------------------
* Name: tranfile.c
* compiler :  Watcom C 10.0
* Copyright (c) 1994,1995 REDTEK BUSINESS TECHNOLOGY LTD.
-------------------------------------------------------------------*/
#include "ezpHead.h"
#include <sys/utime.h>
#include "tranfile.h"

#pragma check_stack(off)

#define PRO_VERSION     0x100

extern unsigned short PortBase,PortBase_1,PortBase_2;
extern unsigned short PortBase_3,PortBase_4,PortBase_5,PortBase_6;
extern unsigned short COMM_INT;
extern volatile word TimerTicks;

static byte *write_ptr;      /* Interrupt buffer            */
static byte *read_ptr;       /* Interrupt buffer            */
static byte *buff_limit;     /* End of interrupt buffer     */
static word old_stat;        /* Modem status for flow-contr */
static word carrier;         /* Carrier detect              */

static word user_abort = 0;
byte *int_buffer;         /* Pointer to interrupt buffer */
static SYS syst;                 /* Structure for MODEM status */

void TestAbortKey()
{
    int ch;

    if(kbhit())
    {
      if((ch=getch())==0x1b)
         user_abort=1;
      if(ch==0) getch();
    }
}

static word read_chan (word bytes, register byte *buffer)
{
    word count;                          /* Byte count                      */
    word avail;                          /* Bytes available                 */

    TimerTicks = 0;               /* Set initial timeout value       */
    count = bytes;                       /* Set byte-count                  */

    //while (count && TimerTicks<TIMOUT)   /* If byte request or no timeout   */
    while (count && TimerTicks<TIMOUT/2)   /* If byte request or no timeout   */
    {
        avail = write_ptr - read_ptr;    /* Bytes available                 */
        if (avail)                       /* If bytes available              */
        {
            if (avail > count)           /* If more bytes than we need      */
                avail = count;           /* Take only what we need          */
            memcpy (buffer,read_ptr,avail);
            count -= avail;              /* Update count                    */
            read_ptr +=avail;            /* Update read pointer             */
            buffer   +=avail;            /* Update write pointer            */
            TimerTicks = 0;              /* Set new timer value             */
        }

        _disable();                      /* Clear interrupts                */
        if (read_ptr == write_ptr)       /* If no bytes available           */
            read_ptr = write_ptr = int_buffer;
        _enable();                       /* Enable interrupts               */
        TestAbortKey();
    }
    return(bytes - count);               /* Actual characters received      */
}

static void flush()
{
    _disable();
    read_ptr = write_ptr = int_buffer;   /* Initialize the interrupt buffer */
    _enable();
}

/****************************************************************************/
/*    Write 'bytes' bytes from buffer to the UART. Don't return until done  */
/*    unless the carrier failed or the hardware broke.                      */
/****************************************************************************/
static word write_chan (word bytes, register byte *buffer)
{
    word status;

    TimerTicks = 0;
    while( bytes!=0 && TimerTicks<TIMOUT && !user_abort )
    {
        while (
          ( status = (inp (PortBase_6) & 0xb0)    /* Check CTS and DSR only */
              ) != old_stat)                  /* If not the same as before  */
        {                                     /* Flow control loop          */
            if ( (status & 0x80 )  != carrier)
            {                       /* ... if not same as before  */
                // printf("drop carrier,st=%x\n",status);
                user_abort = 0x0FFFF;     /* Set the abort flag         */
                return bytes;            /* ... and get out            */
            }
        }

        status = inp(PortBase_5);          /* Get line-status            */
        if ( (status & 0x20)!=0 )      /* If TX holding reg empty    */
        {
            outp(PortBase, *buffer++);   /* Send the byte              */
            bytes--;                          /* Bump the byte-count        */
            TimerTicks = 0;                   /* Set new timer-value        */
        }

        TestAbortKey();
    }

    return bytes;
}

/****************************************************************************/
/*                           Send ^Xes to cancel                            */
/****************************************************************************/
static void cancel()
{
    byte buffer = CAN;
    short xes = OVRHD;

    user_abort=0;                         /* Reset flag so write_chan works */
    while(xes--)
        write_chan(1,&buffer);
}

/****************************************************************************/
/*                          Send the JMODEM block                           */
/****************************************************************************/
static word send_blk (word blk_len, register SYS *sys_ptr, register byte *buffer)
{
    byte ack_buf;                         /* Buffer for ACK/NAK             */
    word n,tries = 10;                   /* Attempts to send the block     */

    while ((tries--) && !user_abort)
    {
        write_chan(blk_len,buffer);       /* Send the JMODEM block*/

        myDelay(1);
        flush();                          /* Clear back channel noise       */
        do
        {
            TestAbortKey();
            ack_buf = (char) 0x00;        /* Clear the return buffer        */
            n=read_chan(1,&ack_buf);      /* Receive a response             */
        } while ( (ack_buf != ACK)        /* Stay in loop until we          */
               && (ack_buf != CAN)        /*  ... get something useful      */
               && (ack_buf != NAK)        /* This helps re-sync in noise    */
               && (ack_buf == (char) 0x00)
               && (!user_abort) );

        if ( ack_buf == CAN || user_abort )        /* Check for an abort    */
        {
            break;                        /* User aborted  */
        }

        if (ack_buf == ACK)               /* If good block                  */
        {
            if (tries == 9)               /* If no retries                  */
            {
                sys_ptr->s_len += BLK_SIZ;    /* Increase block-size        */
                if (sys_ptr->s_len > DAT_MAX) /* If too large               */
                    sys_ptr->s_len = DAT_MAX;
            }
            else
            {
                sys_ptr->s_len >>=1 ;
                if (sys_ptr->s_len < 0x40)
                     sys_ptr->s_len = 0x40;
            }
            return JM_NRM;                    /* Show good                 */
        }
    }

    cancel();                             /* Send cancel (^Xes)             */
    return JM_ABT;                        /* Abort local program            */
}

/****************************************************************************/
/*                        Receive the JMODEM block                          */
/****************************************************************************/
static word recv_blk (word *blk_len, register byte *buffer)
{
    register JBUF *buff;                  /* Pointer type JBUF              */
    byte nak_buf;                         /* Buffer for ACK/NAK             */
    word tries = 10;                      /* Attempts to receive the block  */
    word ret_val;                         /* Block length returned          */

    buff = (JBUF * )buffer;               /* Assign pointer type JBUF       */
    while ((tries--) && !user_abort)
    {
        ret_val = read_chan(2,buffer);    /* Receive the block size         */
        if (ret_val == 2)                 /* If we received the length      */
        {
            *blk_len = buff->len;         /* So caller knows size           */
          #if 0
            if(syst.s_blk==0)   // get filename,etc
            {
               if(*blk_len!=13+sizeof(long)+2*sizeof(time_t)+sizeof(word)+OVRHD)
                    goto lbl_nak;
                   //*blk_len=31;
            }
          #endif

            if (*blk_len > DAT_LEN)       /* If way out of line             */
                break;                    /* NAK it                         */
            ret_val = read_chan(          /* Get more data                  */
                      (*blk_len)-2 ,      /* Size to read                   */
                      &buff->blk_typ);    /* Where to put it                */
            if (ret_val == (*blk_len)-2)  /* If we got what we requested    */
            {
                return JM_NRM;
            }
        }

        if (buff->blk_typ == CAN)         /* If transmitter sent ^Xes       */
        {
            break;                        /* The other side has aborted     */
        }

        if(syst.s_blk>0)   // 0=get filename,etc
           read_chan (DAT_LEN,buffer);       /* Make sure other end stops  */
      // lbl_nak:
        nak_buf = NAK;                    /* Get a NAK                      */
        write_chan(1,&nak_buf);           /* Send to remote                 */
        flush();                          /* Flush the buffer               */
    }

    cancel();                             /* Send cancel (^Xes)             */
    return JM_ABT;                        /* Abort local program            */
}

/****************************************************************************/
/*                         Synchronize during receive                       */
/****************************************************************************/
static word rx_sync()
{
    word n,tries;                        /* Attempts to synchronize        */
    byte ack_nak;                         /* Single byte buffer for ACK/NAK */

    flush();                              /* Clear the interrupt buffer     */
    tries = TRIES;                        /* Attempts to synchronize        */
    while (!user_abort && (tries--))
    {
        ack_nak = (char) 0x00;            /* Clear the buffer               */
        n=read_chan(1,&ack_nak);          /* Receive ACK, NAK, or SYN       */

        if (ack_nak == CAN)               /* If a ^X                        */
            break;

        if ( ack_nak == ACK )             /* If a good response */
            return JM_NRM;                /* Show handshake     */
        if ( ack_nak == NAK )             /* If a good response */
        {
            ack_nak = ACK;
            write_chan(1,&ack_nak);       /* Send a ACK response */
            return JM_NRM;
        }

        TestAbortKey();
        ack_nak = NAK;
        write_chan(1,&ack_nak);          /* Keep sending NAKs    */
    }

    cancel();                             /* Send cancel (^Xes) */
    return JM_ABT;
}
/****************************************************************************/
/*                         Synchronize during transmit                      */
/****************************************************************************/
static word tx_sync()
{
    word ret_val;
    ret_val = rx_sync();
    if (!ret_val)                       /* If success                       */
    {
        flush();                        /* Flush the input buffer           */
        //myDelay(28);
    }
    return ret_val;
}

static word calc_crc(word command,                /* Set or Check CRC       */
              word length,                        /* Buffer length          */
              register byte *buffer)              /* Pointer to the buffer  */
{
    register word *wrds;                          /* Address string as word */
    word crc=0;                                   /* Start at zero          */

    if (length <3)                                /* Check forvalid string  */
        return JM_MAX;                            /* Nothing to CRC         */

    length -=2;                                   /* Don't CRC the CRC      */
    do
    {
       crc += (word) *buffer++;           /* Sum first              */
       // crc  <<= (length & 0x07);          /* Rotate max 7 bits left */
    } while (--length);

    wrds = (word *) buffer;                       /* Set up to point to CRC */
    if (command == GET_CRC)
        return (crc - *wrds);                     /* Return  0 if CRC okay  */
    //else                                          /* Else command = SET_CRC */
    *wrds = crc;                              /* Set the CRC in  string */
    return crc;                               /* Return the CRC also    */
}

void (interrupt far *old_com)();             /* Pointer to old commu intr.  */

void interrupt far com_int()
{
   *write_ptr = (byte) inp(PortBase);         /* Put byte in buffer        */
   outp(0x20,0x20);                           /* Reset hardware controller */
   if (write_ptr < buff_limit)                 /* Check buffer for overflow */
        write_ptr++;                           /* Bump pointer if room      */
}

static void open_chan ()
{
    short i;

    buff_limit = int_buffer + DAT_LEN-1; /* Set up buffer end pointer       */

    _disable();
    old_com  = _dos_getvect( COMM_INT );
    _dos_setvect( COMM_INT, com_int );
    _enable();

    outp(PortBase_1,1);

    for (i=0; i<8; i++)        /* Edge-triggering, read the ports */
        inp(PortBase + i);           /* Port to clear */
    outp(0x20,0x20);           /* Reset the hardware controller */

    myDelay(10);

    flush();                   /* Clear interrupt buffer again */

    i = inp(PortBase_6);        /* Get current modem status */
    old_stat = i & 0xb0;             /* Get current modem control */
    carrier  = i & 0x80;             /* Get any modem carrier */
}

static void close_chan ()
{
    _disable();
    _dos_setvect(COMM_INT, old_com);
    outp(PortBase_1,7);
    _enable();
}


int RecvFile(unsigned char *buffer)
{
    struct utimbuf file_time;
    time_t  actime,modtime;

    byte ch;
    byte file_name[80],*str;
    register JBUF *buff;
    word status;                        /* TX and RX status */
    word tries;
    word data_len;
    long file_len;
    int  handle=-1;                      /* For file I/O   */
    word version=0;

    buff = (JBUF *) buffer;
    //syst.s_len=13+sizeof(long)+2*sizeof(time_t)+OVRHD;
    syst.s_byt = 0;
    syst.s_blk = 0;                        /* Starting block */

    open_chan();                     /* Open com channel */
    status = rx_sync();              /* Synchronize */
    if(status)
    {
       MessageBox(GetTitleString(ERRORINFORM),
            "线路不好,接收失败",1,1);
       goto cleanup;
    }

    FaxStatus("开始接收...",RECEIVE_MODE);

    tries = 10;               /* Attempts to receive */
    while (!user_abort && !status && (tries--) )
    {
        status = recv_blk (&syst.s_len, buffer);
        if (status)           /* If bad */
            break;

        if( !calc_crc(GET_CRC,syst.s_len,buffer)
        && buff->blk_num == (byte) syst.s_blk )
        {
            tries=10;                    /* Reset count */
            syst.s_len -= OVRHD;         /* Subtract overhead */
            syst.s_blk++;                 /* Block number */

            if(syst.s_blk==1) /*- must be filename,filelen,file date -*/
            {
                if ((buff->blk_typ & FNAM)!=0)
                {
                    strcpy(file_name,&buff->blk_dat);
                    handle=open(file_name,O_RDWR|O_BINARY|O_CREAT|O_TRUNC,S_IWUSR );
                    if(handle==-1)
                    {
                       status=-80;
                    lbl_recv_err:
                       ch = CAN;
                       write_chan(1,&ch);  /*- Cancel -*/
                       // puts("can not creat file");
                       goto cleanup;
                    }

                    str=&buff->blk_dat+13;
                    file_len=*(long *)str;
                    // printf("file len=%d\n",file_len);

                    str += sizeof(long);
                    actime=*(time_t *)str;

                    str += sizeof(time_t);
                    modtime=*(time_t *)str;

                    str += sizeof(time_t);
                    version=*(word *)str;

                    if(version!=PRO_VERSION)
                    {
                        status=-100;
                        goto lbl_recv_err;
                    }

                    ch = ACK;             /* Good */
                    write_chan(1,&ch);    /* Send the ACK */
                    //syst.s_len = BLK_SIZ;    /* Set beginning block size */
                    continue;
                }
                else
                {
                    status=-101;
                    goto lbl_recv_err;
                }
            }

            ch = ACK;             /* Good */
            write_chan(1,&ch);    /* Send the ACK */
            data_len = write(handle,&buff->blk_dat,syst.s_len);
            if(!data_len)
            {
                status=-2;
                break;
            }

            syst.s_byt += data_len;   /* Total bytes */
            TellFaxProcStatus(syst.s_byt,file_len);

            if ((buff->blk_typ & EOF_)!=0)
            {
                status = JM_NRM;          /* Set status */
                // puts("receive ok.");
                goto cleanup;             /* exit routine  */
            }
        }
        else
        {
            ch = NAK;            /* Bad block */
            // printf("retry recv blk(%d) ...\n",syst.s_blk+1);
            write_chan(1, &ch);     /* Send the NAK */
        }
    }  /*-- end of while --*/

    status = JM_ABT;

  cleanup:
    if(handle>0)
    {
       close(handle);
       if(!status)
       {
          file_time.actime=actime;
          file_time.modtime=modtime;
          utime(file_name,&file_time);   // change file's date to orgin date
       }
       else
          unlink(file_name);
    }

    close_chan();                     /* Aborted  */
    return status;
}

int SendFile(byte *file_name,unsigned char *buffer)
{
    struct stat file_time;
    byte bFirst;
    byte *str;
    register JBUF *buff;
    word status;                        /* TX and RX status */
    word data_len;
    long file_len;
    int  i,handle;                      /* For file I/O */
    word version=PRO_VERSION;

    buff = (JBUF *) buffer;

    syst.s_len = BLK_SIZ;                  /* Set beginning block size */
    syst.s_byt = 0;
    syst.s_blk = 0;                        /* Starting block */

    handle=open(file_name,O_RDONLY|O_BINARY);
    if(handle==-1)
    {
       //puts("open file error");
       status=-1;
       goto cleanup;
    }

    i=strlen(file_name)-1;
    while(i>=0)
    {
       byte ch=file_name[i];
       if(ch=='\\' || ch=='/' || ch==':')
          break;
       i--;
    }
    i++;

    // printf("file=%s\n",file_name+i);

    open_chan();                     /* Open COM port */
    status = tx_sync();              /* Synchronize */
    // puts("sync ok");
    if(status)
    {
       MessageBox(GetTitleString(ERRORINFORM),
            "线路不好,传送失败",1,1);
       goto cleanup;
    }

    FaxStatus("开始传送...",SEND_MODE);

    bFirst=1;
    while ( !user_abort && !status )
    {
        buff->blk_num = (byte)syst.s_blk++;    /* Block number */
        buff->blk_typ = NORM;             /* Assume Normal */
        if(bFirst)
        {
            bFirst=0;
            file_len=filelength(handle);
            fstat(handle,&file_time);

            str=&buff->blk_dat;
            strcpy(str,file_name+i);
            str += 13;
            *(long *)str=file_len;

            str += sizeof(long);
            *(time_t *)str=file_time.st_atime;

            str += sizeof(time_t);
            *(time_t *)str=file_time.st_mtime;

            str += sizeof(time_t);
            *(word *)str=version;

            str += sizeof(word);
            data_len=(word)( str - &buff->blk_dat);

            buff->blk_typ |= FNAM;
        }
        else
        {
            data_len = read(handle,&buff->blk_dat,syst.s_len);
            if (!data_len)                   /* Past end of file */
                break;
            if (data_len != syst.s_len)      /* Less than request  */
                buff->blk_typ |= EOF_;        /* Its end of file    */
        }

        buff->len = (data_len+OVRHD);    /* Length of block    */
        syst.s_byt += (long) data_len;   /* Running count      */
        // printf("Total send %d bytes.\n",syst.s_byt);

        calc_crc(SET_CRC, buff->len, buffer);
        status = send_blk(buff->len,&syst,buffer);
        TellFaxProcStatus(syst.s_byt,file_len);
        // printf("s_s=%x ",status);

        if ( (buff->blk_typ&EOF_)!=0 )       /* Last record        */
            break;
    } /*- end of while -*/

    if (status)
    {
        cancel();                         /* Send ^Xes          */
        // printf("send abort.\n");
    }

    close_chan();                     /* Close the port     */
    close(handle);

  cleanup:
    return status;                                   /* Normal exit         */
}

