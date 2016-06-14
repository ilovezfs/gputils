
#ifndef __GPREGISTER_H__
#define __GPREGISTER_H__

/* This file is generated automatically by the build-register-db.pl, 2016-01-17 10:12:31 UTC. */

typedef struct {
  const char   *name;
  unsigned int  address;
} gp_bit_t;

typedef struct {
  const char     *name;
  unsigned int    address;
  unsigned int    bit_number;
  const gp_bit_t *bits;
} gp_register_t;

typedef struct {
  const char           *name;
  unsigned int          register_number;
  const gp_register_t **registers;
} gp_register_table_t;

extern const gp_register_table_t gp_register_db[];
extern const int gp_register_db_size;

extern const gp_register_table_t *gp_register_find_mcu(const gp_register_table_t *Table, int Table_size, const char *McuName);

extern const gp_register_t *gp_register_find_reg(const gp_register_table_t *Mcu, unsigned int Address);
extern const char *gp_register_find_reg_name(const gp_register_table_t *Mcu, unsigned int Address);

extern const gp_bit_t *gp_register_find_bit(const gp_register_t *Reg, unsigned int Address);
extern const char *gp_register_find_bit_name(const gp_register_t *Reg, unsigned int Address);

#endif /* __GPREGISTER_H__ */
