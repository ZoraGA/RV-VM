#include "ZoraGA/defs/RVCSRFmt.h"

namespace ZoraGA::RVVM
{

rv_csr_addr_fmt rv_csr_addr(uint16_t add)
{
    rv_csr_addr_fmt ret = {.addr = add};
    return ret;
}

}
