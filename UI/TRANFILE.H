#define GET_CRC         0x00                /* Private Get CRC function     */
#define SET_CRC         0x01                /* Private Set CRC function     */

#define NORM            0x01                /* Private, show normal data    */
#define FNAM            0x02                /* Private, show filename,etc   */
#define EOF_            0x04                /* Private, show end of file    */

#define TIMOUT          (90*8)              /* Timeout (ticks) for read     */
#define TRIES           0x10                /* Number of tries to sync      */

#define EOT             0x04                /* "D" - 64                     */
#define ACK             0x06                /* "F" - 64                     */
#define NAK             0x15                /* "U" - 64                     */
#define SYN             0x16                /* "V" - 64                     */
#define CAN             0x18                /* "X" - 64                     */

#define JM_NRM          0x00                /* Normal exit                  */
#define JM_MEM          0x04                /* No memory available          */
#define JM_ABT          0x07                /* Aborted                      */
#define JM_MAX          0xFFFF              /* Maximum buffer space exceeded*/

#define byte unsigned char                  /* Make code readable           */
#define word unsigned short int             /* Make code readable           */

/****************************************************************************/
/*                     Structures and templates                             */
/****************************************************************************/
typedef struct
    {                             /* Structure for JMODEM status  */
        word  s_blk;                        /* Block being sent / received  */
        word  s_len;                        /* Length of the block          */
        unsigned long s_byt;              /* Bytes so far                 */
        word  s_cps;                        /* Speed, characters / second   */
    } SYS;

typedef struct                    /* JMODEM block header structure */
    {
        word len;                          /* Block length                  */
        byte blk_typ;                      /* Kind of block EOF, COMP, etc  */
        byte blk_num;                      /* Block number (starts at 1 )   */
        byte blk_dat;                      /* First data character          */
    } JBUF;

