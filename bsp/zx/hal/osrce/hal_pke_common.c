#include <stdio.h>
#include <pke_common.h>

/* function: load input operand to baseaddr
 * parameters:
 *     baseaddr ------------------- output, destination data
 *     data ----------------------- input, source data
 *     wordLen -------------------- input, word length of data
 * return: none
 * caution:
 *     1. operands are both U32 little-endian
 */
void pke_load_operand(u32 *baseaddr, u32 *data, u32 wordLen)
{
    u32 i;

    if(baseaddr != data)
    {
        for (i = 0; i < wordLen; i++)
        {
            *((volatile u32 *)(baseaddr+i)) = data[i];
        }
    }
    else
    {;}
}

/* function: get result operand from baseaddr
 * parameters:
 *     baseaddr ------------------- input, source data
 *     data ----------------------- output, destination data
 *     wordLen -------------------- input, word length of data
 * return: none
 * caution:
 *     1. operands are both U32 little-endian
 */
void pke_read_operand(u32 *baseaddr, u32 *data, u32 wordLen)
{
    u32 i;

    if(baseaddr != data)
    {
        for (i = 0; i < wordLen; i++)
        {
            data[i] = *((volatile u32 *)(baseaddr+i));
        }
    }
    else
    {;}
}

/* function: load input operand(U8 big-endian) to baseaddr
 * parameters:
 *     baseaddr ------------------- output, destination data
 *     data ----------------------- input, source data, U8 big-endian
 *     byteLen -------------------- input, byte length of data
 * return: none
 * caution:
 */
void pke_load_operand_U8(u32 *baseaddr, u8 *data, u32 byteLen)
{
    u32 t, i;

    if(baseaddr != (u32 *)data)
    {
        for(data+=(byteLen-1); byteLen>3; byteLen-=4)
        {
            t  = (u32)(*data);
            t |= ((u32)(*(data-1)))<<8;
            t |= ((u32)(*(data-2)))<<16;
            t |= ((u32)(*(data-3)))<<24;

            *((volatile u32 *)(baseaddr++)) = t;

            data -= 4;
        }

        if(byteLen)
        {
            t = 0;
            for(i=0; i < byteLen; i++)
            {
                t |= ((u32)(*data))<<(i<<3);
                data--;
            }

            *((volatile u32 *)(baseaddr)) = t;
        }
        else
        {;}
    }
    else
    {;}
}

/* function: get result operand(U8 big-endian) from baseaddr
 * parameters:
 *     baseaddr ------------------- input, source data
 *     data ----------------------- output, destination data, U8 big-endian
 *     byteLen -------------------- input, byte length of data
 * return: none
 * caution:
 */
void pke_read_operand_U8(u32 *baseaddr, u8 *data, u32 byteLen)
{
    u32 t, i;

    if(baseaddr != (u32 *)data)
    {
        for (data+=(byteLen-1); byteLen>3; byteLen-=4)
        {
            t = *((volatile u32 *)(baseaddr++));

            *data     = (t)&0xFF;
            *(data-1) = (t>>8)&0xFF;
            *(data-2) = (t>>16)&0xFF;
            *(data-3) = (t>>24)&0xFF;

            data -= 4;
        }

        if(byteLen)
        {
            t = *((volatile u32 *)(baseaddr));

            for(i=0; i < byteLen; i++)
            {
                *data = (t>>(i<<3))&0xFF;
                data--;
            }
        }
    }
    else
    {;}
}

/* function: set operand with an u32 value
 * parameters:
 *     baseaddr ------------------- output, operand
 *     wordLen -------------------- input, word length of operand
 *     b -------------------------- input, u32 value b
 * return: none
 * caution:
 *     1. wordLen can not be 0
 */
void pke_set_operand_uint32_value(u32 *baseaddr, u32 wordLen, u32 b)
{
    u32 i = wordLen;

    while(i>1)
    {
        *((volatile u32 *)(baseaddr+(--i))) = 0;  //baseaddr[--i] = 0;
    }

    *((volatile u32 *)(baseaddr)) = b;            //baseaddr[0] = b;
}

