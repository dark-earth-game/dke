#include <idc.idc>
static main() {
  auto f;
  auto_wait();
  f = fopen("out/dkev.asm", "w");
  gen_file(OFILE_ASM, f, 0, BADADDR, GENFLG_ASMTYPE);
  fclose(f);
  f = fopen("out/dkev.lst", "w");
  gen_file(OFILE_LST, f, 0, BADADDR, 0);
  fclose(f);
  qexit(0);
}
