#include <aic_core.h>
#include <ske_secure_port.h>

/* function: disable ske secure port
 * parameters: none
 * return: none
 * caution:
 */
void ske_sec_disable_secure_port(void)
{
    u32 val, mask = ~(((u32)1)<<SKE_SECURE_PORT_OFFSET);

    val = readl(SKE_BASE + SKE_CFG);
    writel(val & mask, SKE_BASE + SKE_CFG);
}

/* function: enable ske secure port
 * parameters:
 *     sp_key_idx ----------------- input, index of secure port key, (sp_key_idx & 0x7FFF) must be in [1,SKE_MAX_KEY_IDX],
 *                                  if the MSB(sp_key_idx) is 1, that means using low 128bit of the 256bit key
 * return: none
 * caution:
 */
void ske_sec_enable_secure_port(u16 sp_key_idx)
{
    u32 val, flag = (((u32)1)<<SKE_SECURE_PORT_OFFSET);

    val = readl(SKE_BASE + SKE_CFG);
    writel(val | flag, SKE_BASE + SKE_CFG);
}

