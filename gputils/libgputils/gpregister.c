
#include "stdhdr.h"
#include "libgputils.h"

/*------------------------------------------------------------------------------------------------*/

static int
mcu_cmp(const void *P1, const void *P2) {
  const gp_register_table_t *t1 = (const gp_register_table_t *)P1;
  const gp_register_table_t *t2 = (const gp_register_table_t *)P2;

  return strcasecmp(t1->name, t2->name);
}

/*------------------------------------------------------------------------------------------------*/

static int
reg_cmp(const void *P1, const void *P2) {
  const gp_register_t **r1 = (const gp_register_t **)P1;
  const gp_register_t **r2 = (const gp_register_t **)P2;
  unsigned int a1 = (*r1)->address;
  unsigned int a2 = (*r2)->address;

  if (a1 > a2) {
    return 1;
  }
  else if (a1 < a2) {
    return -1;
  }
  else {
    return 0;
  }
}

/*------------------------------------------------------------------------------------------------*/

const gp_register_table_t *
gp_register_find_mcu(const gp_register_table_t *Table, int Table_size, const char *McuName) {
  gp_register_table_t mcu;

  if ((Table == NULL) || (McuName == NULL)) {
    return NULL;
  }

  mcu.name = McuName;
  return (const gp_register_table_t *)bsearch(&mcu, Table, Table_size, sizeof(gp_register_table_t),
                                              mcu_cmp);
}

/*------------------------------------------------------------------------------------------------*/

const gp_register_t *
gp_register_find_reg(const gp_register_table_t *Mcu, unsigned int Address) {
  gp_register_t reg;
  const gp_register_t *rptr = &reg;
  const gp_register_t **ret;

  if (Mcu == NULL) {
    return NULL;
  }

  reg.address = Address;
  ret = (const gp_register_t **)bsearch(&rptr, Mcu->registers, Mcu->register_number,
                                        sizeof(gp_register_t *), reg_cmp);
  return ((ret != NULL) ? *ret : NULL);
}

/*------------------------------------------------------------------------------------------------*/

const char *
gp_register_find_reg_name(const gp_register_table_t *Mcu, unsigned int Address) {
  const gp_register_t *ret;

  ret = gp_register_find_reg(Mcu, Address);
  return ((ret != NULL) ? (ret->name) : NULL);
}
